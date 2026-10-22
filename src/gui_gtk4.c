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

#ifdef FEAT_GUI_GTK

#ಅನ್ತರ್ಭಾವ <gdk/gdkkeysyms.h>
#ಅನ್ತರ್ಭಾವ <gdk/gdk.h>
#ಅನ್ತರ್ಭಾವ <gtk/gtk.h>
#ಅನ್ತರ್ಭಾವ "gui_gtk4_f.h"
#ಅನ್ತರ್ಭಾವ "gui_gtk4_cb.h"
#ಅನ್ತರ್ಭಾವ "gui_gtk4_da.h"
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
#define No_ಮೌಲ್ಯ		0x0000
#define Width_ಮೌಲ್ಯ	0x0004
#define Height_ಮೌಲ್ಯ	0x0008

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
vim_parse_geometry(ಧ್ರುವ ವರ್ಣ *str, unsigned ಪೂರ್ಣಾಙ್ಕ *width, unsigned ಪೂರ್ಣಾಙ್ಕ *height)
{
    ಪೂರ್ಣಾಙ್ಕ mask = No_ಮೌಲ್ಯ;
    ವರ್ಣ *end;
    long val;

    ಯದಿ (str == ರಿಕ್ತ || *str == NUL)
	ಪ್ರತ್ಯಾಗಮ mask;

    // Parse width
    ಯದಿ (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	ಯದಿ (end != str)
	{
	    *width = (unsigned ಪೂರ್ಣಾಙ್ಕ)val;
	    mask |= Width_ಮೌಲ್ಯ;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    ಯದಿ (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	ಯದಿ (end != str)
	{
	    *height = (unsigned ಪೂರ್ಣಾಙ್ಕ)val;
	    mask |= Height_ಮೌಲ್ಯ;
	    str = end;
	}
    }

    ಪ್ರತ್ಯಾಗಮ mask;
}

#ಯದಿ defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ಸ್ಥಿರ ಗಣನಾ {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gtk4_main_loop_level = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ಸ್ಥಿರ ಸಂರಚನಾ special_ಕೀಲಿ
{
    guint ಕೀಲಿ_sym;
    char_u code0;
    char_u code1;
}
ಧ್ರುವ special_ಕೀಲಿಗಳು[] =
{
    {GDK_ಕೀಲಿ_Up,	'k', 'u'},
    {GDK_ಕೀಲಿ_Down,	'k', 'd'},
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
    {GDK_ಕೀಲಿ_Back_Space,	'k', 'b'},
    {GDK_ಕೀಲಿ_ಸೇರಿಸು,	'k', 'I'},
    {GDK_ಕೀಲಿ_Delete,	'k', 'D'},
    {GDK_ಕೀಲಿ_Home,	'k', 'h'},
    {GDK_ಕೀಲಿ_End,	'@', '7'},
    {GDK_ಕೀಲಿ_Prior,	'k', 'P'},
    {GDK_ಕೀಲಿ_Next,	'k', 'N'},
    {GDK_ಕೀಲಿ_Print,	'%', '9'},
    {GDK_ಕೀಲಿ_KP_ಎಡ,	'k', 'l'},
    {GDK_ಕೀಲಿ_KP_ಬಲ,	'k', 'r'},
    {GDK_ಕೀಲಿ_KP_Up,	'k', 'u'},
    {GDK_ಕೀಲಿ_KP_Down,	'k', 'd'},
    {GDK_ಕೀಲಿ_KP_ಸೇರಿಸು,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_ಕೀಲಿ_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_ಕೀಲಿ_KP_Home,	'K', '1'},
    {GDK_ಕೀಲಿ_KP_End,	'K', '4'},
    {GDK_ಕೀಲಿ_KP_Prior,	'K', '3'},
    {GDK_ಕೀಲಿ_KP_Next,	'K', '5'},
    {GDK_ಕೀಲಿ_KP_Add,	'K', '6'},
    {GDK_ಕೀಲಿ_KP_Subtract, 'K', '7'},
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

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
keyval_to_string(unsigned ಪೂರ್ಣಾಙ್ಕ keyval, char_u *string)
{
    ಪೂರ್ಣಾಙ್ಕ		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    ಯದಿ (uc != 0)
    {
	len = utf_char2bytes((ಪೂರ್ಣಾಙ್ಕ)uc, string);
    }
    ಅನ್ಯಥಾ
    {
	len = 1;
	ವಿಕಲ್ಪ (keyval)
	{
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Tab: ಪಕ್ಷ GDK_ಕೀಲಿ_KP_Tab: ಪಕ್ಷ GDK_ಕೀಲಿ_ISO_ಎಡ_Tab:
		string[0] = TAB;
		ಭಙ್ಗ;
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Linefeed:
		string[0] = NL;
		ಭಙ್ಗ;
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Return: ಪಕ್ಷ GDK_ಕೀಲಿ_ISO_Enter: ಪಕ್ಷ GDK_ಕೀಲಿ_3270_Enter:
		string[0] = CAR;
		ಭಙ್ಗ;
	    ಪಕ್ಷ GDK_ಕೀಲಿ_Escape:
		string[0] = ESC;
		ಭಙ್ಗ;
	    ಸ್ವಭಾವ:
		len = 0;
		ಭಙ್ಗ;
	}
    }
    string[len] = NUL;
    ಪ್ರತ್ಯಾಗಮ len;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
modifiers_gdk2vim(guint state)
{
    ಪೂರ್ಣಾಙ್ಕ modifiers = 0;

    ಯದಿ (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    ಯದಿ (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    ಯದಿ (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    ಯದಿ (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    ಯದಿ (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

ಸ್ಥಿರ GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ಸ್ಥಿರ gboolean ಕೀಲಿ_press_event(Gtk_Event_Controller_ಕೀಲಿ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ ಕೀಲಿ_release_event(Gtk_Event_Controller_ಕೀಲಿ *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ button_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಙ್ಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ button_release_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಙ್ಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ motion_notify_event(GtkEventControllerMotion *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ enter_notify_event(GtkEventControllerMotion *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data);
ಸ್ಥಿರ gboolean scroll_event(GtkEventControllerScroll *controller, ದ್ವಿಗುಣ dx, ದ್ವಿಗುಣ dy, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ focus_in_event(GtkEventControllerFocus *controller, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
ಸ್ಥಿರ gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, ಶೂನ್ಯ *data);
#endif
#ifdef FEAT_GUI_TABLINE
ಸ್ಥಿರ ಶೂನ್ಯ tabline_enter_cb(GtkEventController *controller, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, ಶೂನ್ಯ *udata);
ಸ್ಥಿರ ಶೂನ್ಯ on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ಸ್ಥಿರ VimMenu *create_tabline_popup_menu(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabline_add_css(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ tabline_menu_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಙ್ಕ n_press, ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, GtkWidget *popover);
#endif
ಸ್ಥಿರ ಶೂನ್ಯ mainwin_destroy_cb(GObject *object, gpointer data);
ಸ್ಥಿರ gboolean delete_event_cb(GtkWindow *window, gpointer data);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ query_pointer_pos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y, GdkModifierType *state);
ಸ್ಥಿರ ಶೂನ್ಯ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
ಸ್ಥಿರ ಶೂನ್ಯ set_form_size(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height);
ಸ್ಥಿರ ಶೂನ್ಯ drawarea_realize_cb(GtkWidget *widget, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
ಸ್ಥಿರ ಶೂನ್ಯ clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
ಸ್ಥಿರ ಶೂನ್ಯ show_menubar_popover(ಶೂನ್ಯ);
#endif

ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *prgname = ರಿಕ್ತ;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ಸ್ಥಿರ gboolean
arg_match(ಧ್ರುವ ವರ್ಣ *s, ಧ್ರುವ ವರ್ಣ *name, ವರ್ಣ **ಮೌಲ್ಯ)
{
    size_t  len = strlen(name);

    ಯದಿ (strncmp(s, name, len) != 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (s[len] == '=')
	*ಮೌಲ್ಯ = (ವರ್ಣ *)s + len + 1;
    ಅನ್ಯಥಾ ಯದಿ (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಙ್ಕ *argc, ವರ್ಣ **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    ಪೂರ್ಣಾಙ್ಕ	i = 0;

    ಯಾವತ್ (i < *argc)
    {
	ವರ್ಣ	*s = argv[i];
	ವರ್ಣ	*ಮೌಲ್ಯ = ರಿಕ್ತ;
	ಪೂರ್ಣಾಙ್ಕ	has_inline_ಮೌಲ್ಯ = FALSE;
	ಪೂರ್ಣಾಙ್ಕ	n_strip;

	ಯದಿ (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ಅನುವರ್ತ;
	}

	ಯದಿ (strchr(s, '=') != ರಿಕ್ತ)
	    has_inline_ಮೌಲ್ಯ = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ಯದಿ (!has_inline_ಮೌಲ್ಯ)
	{
	    ಯದಿ (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		ಮೌಲ್ಯ = argv[i + 1];
	    ಅನ್ಯಥಾ
		ಮೌಲ್ಯ = ರಿಕ್ತ;
	}

	ಯದಿ (arg_match(s, "-fn", &ಮೌಲ್ಯ) || arg_match(s, "-font", &ಮೌಲ್ಯ))
	    font_argument = ಮೌಲ್ಯ;
	ಅನ್ಯಥಾ ಯದಿ (arg_match(s, "-geom", &ಮೌಲ್ಯ)
		|| arg_match(s, "-geometry", &ಮೌಲ್ಯ))
	{
	    ಯದಿ (ಮೌಲ್ಯ != ರಿಕ್ತ)
		gui.geom = vim_strsave((char_u *)ಮೌಲ್ಯ);
	}
	ಅನ್ಯಥಾ ಯದಿ (arg_match(s, "-bg", &ಮೌಲ್ಯ)
		|| arg_match(s, "-background", &ಮೌಲ್ಯ))
	    background_argument = ಮೌಲ್ಯ;
	ಅನ್ಯಥಾ ಯದಿ (arg_match(s, "-fg", &ಮೌಲ್ಯ)
		|| arg_match(s, "-foreground", &ಮೌಲ್ಯ))
	    foreground_argument = ಮೌಲ್ಯ;
#ifdef FEAT_NETBEANS_INTG
	ಅನ್ಯಥಾ ಯದಿ (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_ಮೌಲ್ಯ = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	ಅನ್ಯಥಾ ಯದಿ (arg_match(s, "--prg-name", &ಮೌಲ್ಯ))
	    // GTK4 specific
	    prgname = ಮೌಲ್ಯ;
	ಅನ್ಯಥಾ
	{
	    i++;
	    ಅನುವರ್ತ;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ಯದಿ (ಮೌಲ್ಯ != ರಿಕ್ತ && !has_inline_ಮೌಲ್ಯ)
	    n_strip = 2;

	ಯದಿ (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ಯದಿ (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * ಪರಿಮಾಣ(ವರ್ಣ *));
	    argv[*argc] = ರಿಕ್ತ;
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
timeout_add(ಪೂರ್ಣಾಙ್ಕ time, timeout_cb_type (*callback)(gpointer), ಪೂರ್ಣಾಙ್ಕ *flagp)
{
    ಪ್ರತ್ಯಾಗಮ g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

ಸ್ಥಿರ long_u blink_waittime = 700;
ಸ್ಥಿರ long_u blink_ontime = 400;
ಸ್ಥಿರ long_u blink_offtime = 250;
ಸ್ಥಿರ guint blink_timer = 0;

    ಸ್ಥಿರ timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    ಯದಿ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, ರಿಕ್ತ);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಙ್ಕ may_call_gui_update_cursor)
{
    ಯದಿ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ಯದಿ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ಶೂನ್ಯ
gui_mch_start_blink(ಶೂನ್ಯ)
{
    ಯದಿ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ಯದಿ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, ರಿಕ್ತ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_early_init_check(ಪೂರ್ಣಾಙ್ಕ give_message UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == ರಿಕ್ತ ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ಯದಿ (g_getenv("EGL_LOG_LEVEL") == ರಿಕ್ತ)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init(ಶೂನ್ಯ)
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
		     G_CALLBACK(delete_event_cb), ರಿಕ್ತ);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ರಿಕ್ತ);

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
		     G_CALLBACK(on_select_tab), ರಿಕ್ತ);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ರಿಕ್ತ);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ರಿಕ್ತ);

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
		     G_CALLBACK(drawarea_realize_cb), ರಿಕ್ತ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ರಿಕ್ತ);

    // Set up event controllers.
    {
	GtkEventController *ಕೀಲಿ_ctrl = gtk_event_controller_ಕೀಲಿ_new();
	g_signal_connect(ಕೀಲಿ_ctrl, "key-pressed",
			 G_CALLBACK(ಕೀಲಿ_press_event), ರಿಕ್ತ);
	g_signal_connect(ಕೀಲಿ_ctrl, "key-released",
			 G_CALLBACK(ಕೀಲಿ_release_event), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea, ಕೀಲಿ_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ರಿಕ್ತ);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ರಿಕ್ತ);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), ರಿಕ್ತ);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(ರಿಕ್ತ,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), ರಿಕ್ತ);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", ರಿಕ್ತ);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ಯದಿ (primary != ರಿಕ್ತ)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	ಯದಿ (regular != ರಿಕ್ತ)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    ಯದಿ (gui.drawarea != ರಿಕ್ತ && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಙ್ಕ
gui_mch_open(ಶೂನ್ಯ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    ಯದಿ (gui.geom != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ		mask;
	unsigned ಪೂರ್ಣಾಙ್ಕ	w, h;

	mask = vim_parse_geometry((ವರ್ಣ *)gui.geom, &w, &h);

	ಯದಿ (mask & Width_ಮೌಲ್ಯ)
	    Columns = w;
	ಯದಿ (mask & Height_ಮೌಲ್ಯ)
	{
	    ಯದಿ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    ಅನ್ಯಥಾ
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ಯದಿ (Columns > 80)
	    Columns = 80;
	ಯದಿ (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    ಯದಿ (foreground_argument != ರಿಕ್ತ)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    ಯದಿ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ಯದಿ (background_argument != ರಿಕ್ತ)
	bg_pixel = gui_get_color((char_u *)background_argument);
    ಯದಿ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ಯದಿ (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    ಅನ್ಯಥಾ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ರಿಕ್ತ);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((ಪೂರ್ಣಾಙ್ಕ)pixel_width - get_menu_tool_width(),
	    (ಪೂರ್ಣಾಙ್ಕ)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಙ್ಕ rc UNUSED)
{
    ಯದಿ (gui.mainwin != ರಿಕ್ತ)
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
		ಯದಿ ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ರಿಕ್ತ)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
gui_mch_set_winpos(ಪೂರ್ಣಾಙ್ಕ x UNUSED, ಪೂರ್ಣಾಙ್ಕ y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_maximized(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_unmaximize(ಶೂನ್ಯ)
{
    ಯದಿ (gui.mainwin != ರಿಕ್ತ)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_set_fullscreen(ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (gui.mainwin == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    ಅನ್ಯಥಾ
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ಯದಿ (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    ಪೂರ್ಣಾಙ್ಕ w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    ಶೂನ್ಯ
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    ಯದಿ (title != ರಿಕ್ತ && gui.mainwin != ರಿಕ್ತ)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ಧ್ರುವ ವರ್ಣ *)title);
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
gui_gtk_init_decor_height(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ h = gtk_widget_get_height(gui.mainwin);

    ಯದಿ (h == 0)
	ಪ್ರತ್ಯಾಗಮ;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_form_size(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ಯದಿ (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	ಪ್ರತ್ಯಾಗಮ;

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

    ಶೂನ್ಯ
gui_mch_set_shellsize(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height,
	ಪೂರ್ಣಾಙ್ಕ min_width UNUSED, ಪೂರ್ಣಾಙ್ಕ min_height UNUSED,
	ಪೂರ್ಣಾಙ್ಕ base_width UNUSED, ಪೂರ್ಣಾಙ್ಕ base_height UNUSED,
	ಪೂರ್ಣಾಙ್ಕ direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಙ್ಕ *screen_w, ಪೂರ್ಣಾಙ್ಕ *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ಯದಿ (surface != ರಿಕ್ತ)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ಯದಿ (monitor != ರಿಕ್ತ)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    ಪ್ರತ್ಯಾಗಮ;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (gui.menubar != ರಿಕ್ತ)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (gui.toolbar != ರಿಕ್ತ)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	ಯದಿ (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    ಶೂನ್ಯ
gui_mch_set_dark_theme(ಪೂರ್ಣಾಙ್ಕ dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ಯದಿ (settings != ರಿಕ್ತ)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ರಿಕ್ತ);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    PangoFontMetrics *metrics;
    ಪೂರ್ಣಾಙ್ಕ ascent;
    ಪೂರ್ಣಾಙ್ಕ descent;

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

    ಪ್ರತ್ಯಾಗಮ OK;
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    ಸ್ಥಿರ ಶೂನ್ಯ
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, ರಿಕ್ತ);
    fdd->done = TRUE;
}

    ಸ್ಥಿರ gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    ಯದಿ (PANGO_IS_FONT_FAMILY(item))
	ಪ್ರತ್ಯಾಗಮ pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    ಯದಿ (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	ಯದಿ (family != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ pango_font_family_is_monospace(family);
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ರಿಕ್ತ;
    char_u		*fontname = ರಿಕ್ತ;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ರಿಕ್ತ, ರಿಕ್ತ)));

    ಯದಿ (oldval != ರಿಕ್ತ && oldval[0] != NUL)
    {
	char_u *oldname;

	ಯದಿ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ರಿಕ್ತ);
	ಅನ್ಯಥಾ
	    oldname = oldval;

	ಯದಿ (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ಯದಿ (p != ರಿಕ್ತ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ಯದಿ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ಧ್ರುವ ವರ್ಣ *)oldname);
	ಯದಿ (oldname != oldval)
	    vim_free(oldname);
    }
    ಅನ್ಯಥಾ
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = ರಿಕ್ತ;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, ರಿಕ್ತ, font_dialog_finish_cb, &fdd);

    ಯಾವತ್ (!fdd.done)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);

    ಯದಿ (fdd.result != ರಿಕ್ತ)
    {
	ವರ್ಣ *name = pango_font_description_to_string(fdd.result);
	ಯದಿ (name != ರಿಕ್ತ)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ಯದಿ (p != ರಿಕ್ತ && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ಒಳಹರಿವು_conv, p, ರಿಕ್ತ);
		vim_free(p);
	    }
	    ಅನ್ಯಥಾ
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    ಯದಿ (initial != ರಿಕ್ತ)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ಪ್ರತ್ಯಾಗಮ fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ascii_glyph_table_init(ಶೂನ್ಯ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    ಪೂರ್ಣಾಙ್ಕ		    i;

    ಯದಿ (gui.ascii_glyphs != ರಿಕ್ತ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ಯದಿ (gui.ascii_font != ರಿಕ್ತ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ರಿಕ್ತ;
    gui.ascii_font   = ರಿಕ್ತ;

    ಪ್ರತಿ (i = 0; i < 128; ++i)
    {
	ಯದಿ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ಅನ್ಯಥಾ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ಧ್ರುವ ವರ್ಣ *)ascii_chars,
			      0, ಪರಿಮಾಣ(ascii_chars), attr_list, ರಿಕ್ತ);

    ಯದಿ (item_list != ರಿಕ್ತ && item_list->next == ರಿಕ್ತ)
    {
	PangoItem   *item;
	ಪೂರ್ಣಾಙ್ಕ	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ಧ್ರುವ ವರ್ಣ *)ascii_chars, ಪರಿಮಾಣ(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	ಯದಿ (gui.ascii_glyphs->ಅಂಕೆ_glyphs == (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(ascii_chars))
	{
	    ಪ್ರತಿ (i = 0; i < gui.ascii_glyphs->ಅಂಕೆ_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ರಿಕ್ತ;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ರಿಕ್ತ;
	}
    }

    g_list_foreach(item_list, (GFunc)(ಶೂನ್ಯ *)&pango_item_free, ರಿಕ್ತ);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
get_styled_font_variants(ಶೂನ್ಯ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    ಯದಿ (plain_font == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    ಯದಿ (bold_font != ರಿಕ್ತ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_init_font(char_u *font_name, ಪೂರ್ಣಾಙ್ಕ fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ಪೂರ್ಣಾಙ್ಕ			    width;

    ಯದಿ (font_name == ರಿಕ್ತ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ಯದಿ (font_desc == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, ರಿಕ್ತ);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ಯದಿ (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	ಪೂರ್ಣಾಙ್ಕ im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	ಪೂರ್ಣಾಙ್ಕ im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    GuiFont
gui_mch_get_font(char_u *name, ಪೂರ್ಣಾಙ್ಕ report_error)
{
    PangoFontDescription *font;

    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    font = pango_font_description_from_string((ಧ್ರುವ ವರ್ಣ *)name);
    ಯದಿ (font == ರಿಕ್ತ)
    {
	ಯದಿ (report_error)
	    semsg(_(e_unknown_font_str), name);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    // Ensure a size is set
    ಯದಿ (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ಪ್ರತ್ಯಾಗಮ font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    ಯದಿ (font != NOFONT)
    {
	ವರ್ಣ *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ಪ್ರತ್ಯಾಗಮ ret;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಯದಿ (font != NOFONT)
	pango_font_description_free(font);
}

    ಶೂನ್ಯ
gui_mch_expand_font(
    optexpand_T	*args,
    ಶೂನ್ಯ	*param,
    ಪೂರ್ಣಾಙ್ಕ		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			n_families = 0;
    ಪೂರ್ಣಾಙ್ಕ			wide = *(ಪೂರ್ಣಾಙ್ಕ *)param;

    ಯದಿ (args->oe_include_orig_val && *args->oe_opt_ಮೌಲ್ಯ == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ಯದಿ (add_match((char_u *)DEFAULT_FONT) != OK)
	    ಪ್ರತ್ಯಾಗಮ;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_families; i++)
    {
	ಯದಿ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ಅನುವರ್ತ;

	ಧ್ರುವ ವರ್ಣ *fam_name = pango_font_family_get_name(font_families[i]);
	ಯದಿ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ಒಳಹರಿವು_conv,
		    (char_u *)fam_name, ರಿಕ್ತ);
	    ಯದಿ (buf != ರಿಕ್ತ)
	    {
		ಯದಿ (add_match(buf) != OK)
		{
		    vim_free(buf);
		    ಭಙ್ಗ;
		}
		vim_free(buf);
	    }
	    ಅನ್ಯಥಾ
		ಭಙ್ಗ;
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (add_match((char_u *)fam_name) != OK)
		ಭಙ್ಗ;
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
    ಯದಿ (!gui.in_use)
	ಪ್ರತ್ಯಾಗಮ INVALCOLOR;

    ಯದಿ (name != ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ gui_get_color_cmn(name);

    ಪ್ರತ್ಯಾಗಮ INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ g, ಪೂರ್ಣಾಙ್ಕ b)
{
    ಪ್ರತ್ಯಾಗಮ gui_get_rgb_color_cmn(r, g, b);
}

    ಸ್ಥಿರ GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ಪ್ರತ್ಯಾಗಮ rgba;
}

    ಶೂನ್ಯ
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    ಶೂನ್ಯ
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    ಶೂನ್ಯ
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ಪ್ರತ್ಯಾಗಮ pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    ಶೂನ್ಯ
gui_gtk4_update_size(ಶೂನ್ಯ)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.ಅಂಕೆ_rows, gui.ಅಂಕೆ_cols);
}

#ifdef FEAT_NETBEANS_INTG
    ಶೂನ್ಯ
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	ಪೂರ್ಣಾಙ್ಕ		row,
	ಪೂರ್ಣಾಙ್ಕ		col,
	ಪೂರ್ಣಾಙ್ಕ		width,
	ಪೂರ್ಣಾಙ್ಕ		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    ಶೂನ್ಯ
gui_mch_clear_block(ಪೂರ್ಣಾಙ್ಕ row1, ಪೂರ್ಣಾಙ್ಕ col1, ಪೂರ್ಣಾಙ್ಕ row2, ಪೂರ್ಣಾಙ್ಕ col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	gtk_widget_queue_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	gtk_widget_queue_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + ಅಂಕೆ_ಸಾಲುಗಳು, gui.scroll_region_bot,
	    gui.scroll_region_ಎಡ, gui.scroll_region_ಬಲ);
    gui_clear_block(
	    gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1, gui.scroll_region_ಎಡ,
	    gui.scroll_region_bot, gui.scroll_region_ಬಲ);

    gtk_widget_queue_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + ಅಂಕೆ_ಸಾಲುಗಳು, row, gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು,
	    gui.scroll_region_ಎಡ, gui.scroll_region_ಬಲ);
    gui_clear_block(
	    row, gui.scroll_region_ಎಡ,
	    row + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    ಶೂನ್ಯ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಙ್ಕ msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (ಪೂರ್ಣಾಙ್ಕ)Rows - 1, (ಪೂರ್ಣಾಙ್ಕ)Columns - 1);
}

    ಶೂನ್ಯ
gui_mch_invert_rectangle(ಪೂರ್ಣಾಙ್ಕ r, ಪೂರ್ಣಾಙ್ಕ c, ಪೂರ್ಣಾಙ್ಕ nr, ಪೂರ್ಣಾಙ್ಕ nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ಸ್ಥಿರ gboolean
ಕೀಲಿ_press_event(Gtk_Event_Controller_ಕೀಲಿ *controller UNUSED,
	guint ಕೀಲಿ_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    ಪೂರ್ಣಾಙ್ಕ		len;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		modifiers;
    ಪೂರ್ಣಾಙ್ಕ		ಕೀಲಿ;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ಯದಿ (xic != ರಿಕ್ತ)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ಯದಿ (event != ರಿಕ್ತ && gtk_im_context_filter_keypress(xic, event))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
#endif

#ifdef FEAT_MENU
    ಯದಿ (ಕೀಲಿ_sym == GDK_ಕೀಲಿ_F10 && gui.menubar != ರಿಕ್ತ)
    {
	ಸ್ಥಿರ char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ಯದಿ (check_map(k10, State, FALSE, TRUE, FALSE, ರಿಕ್ತ, ರಿಕ್ತ) == ರಿಕ್ತ)
	{
	    show_menubar_popover();
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
    }
#endif

    len = keyval_to_string(ಕೀಲಿ_sym, string2);

    ಯದಿ (len > 1 && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	len = convert_ಒಳಹರಿವು(string2, len, ಪರಿಮಾಣ(string2));

    s = string2;
    d = string;
    ಪ್ರತಿ (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ಯದಿ (d[-1] == CSI && d + 2 < string + ಪರಿಮಾಣ(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (ಪೂರ್ಣಾಙ್ಕ)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ಯದಿ (ಕೀಲಿ_sym == GDK_ಕೀಲಿ_ISO_ಎಡ_Tab)
    {
	ಕೀಲಿ_sym = GDK_ಕೀಲಿ_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ಯದಿ ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif

    // Check for special keys
    ಯದಿ (len == 0 || len == 1)
    {
	ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    ಯದಿ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ಕೀಲಿಗಳು[i].code0;
		string[2] = special_ಕೀಲಿಗಳು[i].code1;
		len = -3;
		ಭಙ್ಗ;
	    }
	}
    }

    ಯದಿ (len == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    ಯದಿ (len == -3)
	ಕೀಲಿ = TO_SPECIAL(string[1], string[2]);
    ಅನ್ಯಥಾ
    {
	string[len] = NUL;
	ಕೀಲಿ = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
    ಯದಿ (ಕೀಲಿ == CSI)
	ಕೀಲಿ = K_CSI;
    ಯದಿ (IS_SPECIAL(ಕೀಲಿ))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ಕೀಲಿ);
	string[2] = K_THIRD(ಕೀಲಿ);
	len = 3;
    }
    ಅನ್ಯಥಾ
    {
	ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);
	modifiers = may_remove_shift_modifier(modifiers, ಕೀಲಿ);
	len = mb_char2bytes(ಕೀಲಿ, string);
    }

    ಯದಿ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_ಒಳಹರಿವು_buf(string2, 3);
    }

    {
	ಪೂರ್ಣಾಙ್ಕ int_ch = check_for_interrupt(ಕೀಲಿ, modifiers);
	ಯದಿ (int_ch != NUL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_ಒಳಹರಿವು_buf(string, len);

    ಯದಿ (p_mh)
	gui_mch_mousehide(TRUE);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೀಲಿ_release_event(Gtk_Event_Controller_ಕೀಲಿ *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mouse_timed_out = TRUE;
ಸ್ಥಿರ guint mouse_click_timer = 0;

    ಸ್ಥಿರ timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(ಪೂರ್ಣಾಙ್ಕ *)data = TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
modifiers_gdk2mouse(guint state)
{
    ಪೂರ್ಣಾಙ್ಕ modifiers = 0;

    ಯದಿ (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    ಯದಿ (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    ಯದಿ (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    ಪ್ರತ್ಯಾಗಮ modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mouse_pressed_button = -1;

ಸ್ಥಿರ guint motion_repeat_timer = 0;
ಸ್ಥಿರ gboolean motion_repeat_offset = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mouse_press_x = -1, mouse_press_y = -1;

    ಸ್ಥಿರ ಶೂನ್ಯ
button_press_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಙ್ಕ n_press UNUSED,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ button;
    ಪೂರ್ಣಾಙ್ಕ repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ಯದಿ (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    ವಿಕಲ್ಪ (btn)
    {
	ಪಕ್ಷ 1: button = MOUSE_ಎಡ; ಭಙ್ಗ;
	ಪಕ್ಷ 2: button = MOUSE_ಮಧ್ಯ; ಭಙ್ಗ;
	ಪಕ್ಷ 3: button = MOUSE_ಬಲ; ಭಙ್ಗ;
	ಪಕ್ಷ 8: button = MOUSE_X1; ಭಙ್ಗ;
	ಪಕ್ಷ 9: button = MOUSE_X2; ಭಙ್ಗ;
	ಸ್ವಭಾವ: ಪ್ರತ್ಯಾಗಮ;
    }

    mouse_press_x = (ಪೂರ್ಣಾಙ್ಕ)x;
    mouse_press_y = (ಪೂರ್ಣಾಙ್ಕ)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (ಪೂರ್ಣಾಙ್ಕ)x, (ಪೂರ್ಣಾಙ್ಕ)y, repeated_click, vim_modifiers);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
button_release_event(GtkGestureClick *gesture, ಪೂರ್ಣಾಙ್ಕ n_press UNUSED,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ಯದಿ (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (ಪೂರ್ಣಾಙ್ಕ)x, (ಪೂರ್ಣಾಙ್ಕ)y, FALSE, vim_modifiers);
}

ಸ್ಥಿರ ದ್ವಿಗುಣ prev_mouse_x = -1.0;
ಸ್ಥಿರ ದ್ವಿಗುಣ prev_mouse_y = -1.0;

ಸ್ಥಿರ GdkModifierType cur_state = 0;

    ಸ್ಥಿರ timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ x, y;

    ಯದಿ (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ಪ್ರತ್ಯಾಗಮ G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ಯದಿ (vim_used_in_ಒಳಹರಿವು_buf() > 10)
	ಪ್ರತ್ಯಾಗಮ G_SOURCE_CONTINUE;

    x = (ಪೂರ್ಣಾಙ್ಕ)prev_mouse_x;
    y = (ಪೂರ್ಣಾಙ್ಕ)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ಯದಿ (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ಪ್ರತ್ಯಾಗಮ G_SOURCE_CONTINUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ಯದಿ (mouse_pressed_button >= 0 && event != ರಿಕ್ತ
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	ಯದಿ (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ಯದಿ (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	ಪೂರ್ಣಾಙ್ಕ		w, h;

	ಯದಿ (event != ರಿಕ್ತ)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (ಪೂರ್ಣಾಙ್ಕ)x, (ಪೂರ್ಣಾಙ್ಕ)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	ಯದಿ (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ಯದಿ (x < 0 || y < 0 || x >= w || y >= h)
	{
	    ಪೂರ್ಣಾಙ್ಕ	dx, dy;
	    ಪೂರ್ಣಾಙ್ಕ offshoot;
	    ಪೂರ್ಣಾಙ್ಕ	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ಯದಿ (offshoot > 127)
		delay = 5;
	    ಅನ್ಯಥಾ
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, ರಿಕ್ತ);
	}
	ಅನ್ಯಥಾ
	    motion_repeat_timer = 0;
    }
    ಅನ್ಯಥಾ
	gui_mouse_moved((ಪೂರ್ಣಾಙ್ಕ)x, (ಪೂರ್ಣಾಙ್ಕ)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ಯದಿ (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
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
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	ದ್ವಿಗುಣ x, ದ್ವಿಗುಣ y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ಸ್ಥಿರ gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	ದ್ವಿಗುಣ dx UNUSED, ದ್ವಿಗುಣ dy, gpointer data UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ button;
    int_u vim_modifiers;
    ಪೂರ್ಣಾಙ್ಕ x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ಯದಿ (event == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    state = gdk_event_get_modifier_state(event);

    ಯದಿ (dy < 0)
	button = MOUSE_4;	// scroll up
    ಅನ್ಯಥಾ ಯದಿ (dy > 0)
	button = MOUSE_5;	// scroll down
    ಅನ್ಯಥಾ ಯದಿ (dx < 0)
	button = MOUSE_7;	// scroll left
    ಅನ್ಯಥಾ ಯದಿ (dx > 0)
	button = MOUSE_6;	// scroll right
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ಯದಿ (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ಯದಿ (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ಯದಿ (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ಯದಿ (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ಯದಿ defined(FEAT_IMAGE)
    ಸ್ಥಿರ ಶೂನ್ಯ
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	ಶೂನ್ಯ		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ಯದಿ defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), ರಿಕ್ತ);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

ಪ್ರಕಾರನಿರ್ದೇಶ ಗಣನಾ
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    ವರ್ಣ	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ReadData	rd;
    GdkDrop	*drop;
    ಪೂರ್ಣಾಙ್ಕ		x;
    ಪೂರ್ಣಾಙ್ಕ		y;
} DropReadData;
#endif

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ReadData	rd;
    Clipboard_T *cbd;
    ವರ್ಣ	*mime_type;
} ClipReadData;

    ಸ್ಥಿರ ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ಯದಿ (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ಪ್ರತ್ಯಾಗಮ rd;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    ಯದಿ (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    ಅನ್ಯಥಾ ಯದಿ (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    ಯದಿ (rd->arr != ರಿಕ್ತ)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
ಸ್ಥಿರ ಶೂನ್ಯ drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
read_data_ಒಳಹರಿವು_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_ಒಳಹರಿವು_stream_read_finish(stream, result, ರಿಕ್ತ);

    ಯದಿ (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	ಯದಿ (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    ಅನ್ಯಥಾ ಯದಿ (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ಯದಿ (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    ಪೂರ್ಣಾಙ್ಕ		    motion_type = MAUTO;
	    char_u	    *tofree = ರಿಕ್ತ;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ರಿಕ್ತ;

	    ಯದಿ (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	ಅನ್ಯಥಾ ಯದಿ (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ಧ್ರುವ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ರಿಕ್ತ;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    ಅನ್ಯಥಾ
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ಧ್ರುವ uint8_t *)rd->buf, r);
	g_ಒಳಹರಿವು_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_ಮೇಲಿನ,
		rd->cancel, (GAsyncReadyCallback)read_data_ಒಳಹರಿವು_cb, rd);
	ಪ್ರತ್ಯಾಗಮ;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    ಶೂನ್ಯ
gui_gtk_set_dnd_targets(ಶೂನ್ಯ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    ಯದಿ (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
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

    ಯದಿ (text == ರಿಕ್ತ || *text == NUL || !query_pointer_pos(ರಿಕ್ತ, ರಿಕ್ತ, &state))
	ಪ್ರತ್ಯಾಗಮ;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    ಯದಿ (dropkey[2] != 0)
	add_to_ಒಳಹರಿವು_buf(dropkey, (ಪೂರ್ಣಾಙ್ಕ)ಪರಿಮಾಣ(dropkey));
    ಅನ್ಯಥಾ
	add_to_ಒಳಹರಿವು_buf(dropkey + 3, (ಪೂರ್ಣಾಙ್ಕ)(ಪರಿಮಾಣ(dropkey) - 3));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_read_ಮೌಲ್ಯ_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ಧ್ರುವ GValue    *ಮೌಲ್ಯ = gdk_drop_read_ಮೌಲ್ಯ_finish(drop, result, ರಿಕ್ತ);
    gboolean	    success = FALSE;

    ಯದಿ (ಮೌಲ್ಯ == ರಿಕ್ತ)
	ಗಚ್ಛ exit;

    ಯದಿ (G_ಮೌಲ್ಯ_HOLDS(ಮೌಲ್ಯ, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_ಮೌಲ್ಯ_get_boxed(ಮೌಲ್ಯ);
	ಪೂರ್ಣಾಙ್ಕ	nfiles = g_slist_length(files);
	char_u	**fnames;
	ಪೂರ್ಣಾಙ್ಕ	i;

	ಯದಿ (nfiles <= 0)
	    ಗಚ್ಛ exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ಯದಿ (fnames == ರಿಕ್ತ)
	    ಗಚ್ಛ exit;

	i = 0;
	ಪ್ರತಿ (GSList *l = files; l != ರಿಕ್ತ; l = l->next)
	{
	    GFile *file = l->data;
	    ವರ್ಣ *path = g_file_get_path(file);
	    ಯದಿ (path != ರಿಕ್ತ)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	ಯದಿ (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ಅನ್ಯಥಾ
	    vim_free(fnames);
	success = TRUE;
    }
    ಅನ್ಯಥಾ ಯದಿ (G_ಮೌಲ್ಯ_HOLDS(ಮೌಲ್ಯ, G_TYPE_STRING))
    {
	ಧ್ರುವ ವರ್ಣ *text = g_ಮೌಲ್ಯ_get_string(ಮೌಲ್ಯ);

	ಯದಿ (text == ರಿಕ್ತ || *text == NUL)
	    ಗಚ್ಛ exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ಧ್ರುವ ವರ್ಣ	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, ರಿಕ್ತ);
    ಯದಿ (in_stream == ರಿಕ್ತ || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	ಪ್ರತ್ಯಾಗಮ;
    }

    ಪ್ರತಿಜ್ಞಾ(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_ಒಳಹರಿವು_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ಮೇಲಿನ,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_ಒಳಹರಿವು_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ಸ್ಥಿರ GCancellable *dnd_cancel = ರಿಕ್ತ;
ಸ್ಥಿರ guint dnd_timeout_id = 0;

    ಸ್ಥಿರ gboolean
drop_timeout_cb(ಶೂನ್ಯ *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ಪ್ರತ್ಯಾಗಮ G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ಸ್ಥಿರ gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	ದ್ವಿಗುಣ		    x,
	ದ್ವಿಗುಣ		    y,
	ಶೂನ್ಯ		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = ರಿಕ್ತ;

    ಯದಿ (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, ಪರಿಮಾಣ(*drd));
	gdk_drop_read_ಮೌಲ್ಯ_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ಮೌಲ್ಯ_cb, drd);
    }
    ಅನ್ಯಥಾ ಯದಿ (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *m[] = {"text/html", ರಿಕ್ತ};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, ಪರಿಮಾಣ(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ಅನ್ಯಥಾ ಯದಿ (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, ಪರಿಮಾಣ(*drd));
	gdk_drop_read_ಮೌಲ್ಯ_async(drop, G_TYPE_STRING, G_PRIORITY_ಮೇಲಿನ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ಮೌಲ್ಯ_cb, drd);
    }

    ಯದಿ (dnd_cancel != ರಿಕ್ತ)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ಯದಿ (drd != ರಿಕ್ತ)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (ಪೂರ್ಣಾಙ್ಕ)x;
	drd->y = (ಪೂರ್ಣಾಙ್ಕ)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, ರಿಕ್ತ);

	ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = ರಿಕ್ತ;
    gui.drawarea = ರಿಕ್ತ;
    ಯದಿ (!exiting)
	gui_shell_closed();
}

    ಸ್ಥಿರ gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ಸ್ಥಿರ timeout_cb_type
ಒಳಹರಿವು_timer_cb(gpointer data)
{
    ಪೂರ್ಣಾಙ್ಕ *timed_out = (ಪೂರ್ಣಾಙ್ಕ *)data;

    *timed_out = TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;	// don't call me again
}

    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ cnt = 0;

    ಯಾವತ್ (g_main_context_pending(ರಿಕ್ತ) && !vim_is_ಒಳಹರಿವು_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    ಸ್ಥಿರ timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ಪ್ರತ್ಯಾಗಮ TRUE; // Keep repeating
}
#endif

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_wait_for_chars(long wtime)
{
    ಪೂರ್ಣಾಙ್ಕ		focus;
    guint	timer;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	timed_out;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    ಯದಿ (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   ಒಳಹರಿವು_timer_cb, &timed_out);
    ಅನ್ಯಥಾ
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ಯದಿ (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, ರಿಕ್ತ);
#endif

    focus = gui.in_focus;

    ಕುರು
    {
	// Stop or start blinking when focus changes
	ಯದಿ (gui.in_focus != focus)
	{
	    ಯದಿ (gui.in_focus)
		gui_mch_start_blink();
	    ಅನ್ಯಥಾ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಯದಿ (did_add_timer)
	    ಗಚ್ಛ theend;
# endif
#endif

	ಯದಿ (gui.mainwin == ರಿಕ್ತ)
	    ಗಚ್ಛ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ಯದಿ (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    ಗಚ್ಛ theend;
	}

	ಯದಿ (!ಒಳಹರಿವು_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ರಿಕ್ತ, TRUE);
	    --gtk4_main_loop_level;
	}

	ಯದಿ (ಒಳಹರಿವು_available())
	{
	    retval = OK;
	    ಗಚ್ಛ theend;
	}
    } ಯಾವತ್ (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    ಯದಿ (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    ಯದಿ (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	gtk_widget_queue_draw(gui.drawarea);
    ಯದಿ (gui.mainwin != ರಿಕ್ತ && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    GdkDisplay *display;

    ಯದಿ (gui.mainwin != ರಿಕ್ತ && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ಯದಿ (display != ರಿಕ್ತ)
	    gdk_display_beep(display);
    }
}

    ಶೂನ್ಯ *
gui_mch_get_display(ಶೂನ್ಯ)
{
    ಯದಿ (gui.mainwin != ರಿಕ್ತ && gtk_widget_get_display(gui.mainwin))
	ಪ್ರತ್ಯಾಗಮ gtk_widget_get_display(gui.mainwin);
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    ಶೂನ್ಯ
gui_mch_set_foreground(ಶೂನ್ಯ)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
query_pointer_pos(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    ದ್ವಿಗುಣ	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    ಯದಿ (gui.drawarea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ಯದಿ (native == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    surface = gtk_native_get_surface(native);
    ಯದಿ (surface == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ಯದಿ (display == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    seat = gdk_display_get_default_seat(display);
    ಯದಿ (seat == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    pointer = gdk_seat_get_pointer(seat);
    ಯದಿ (pointer == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (x != ರಿಕ್ತ && y != ರಿಕ್ತ)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (ಭಿನ್ನ)(sx - nx);
	src.y = (ಭಿನ್ನ)(sy - ny);
	ಯದಿ (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	*x = (ಪೂರ್ಣಾಙ್ಕ)dst.x;
	*y = (ಪೂರ್ಣಾಙ್ಕ)dst.y;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಙ್ಕ *x, ಪೂರ್ಣಾಙ್ಕ *y)
{
    ಯದಿ (!query_pointer_pos(x, y, ರಿಕ್ತ))
    {
	*x = 0;
	*y = 0;
    }
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಙ್ಕ x UNUSED, ಪೂರ್ಣಾಙ್ಕ y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಙ್ಕ hide)
{
    ಯದಿ (gui.pointer_hidden == hide)
	ಪ್ರತ್ಯಾಗಮ;

    gui.pointer_hidden = hide;
    ಯದಿ (gui.blank_pointer != ರಿಕ್ತ)
    {
	ಯದಿ (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	ಅನ್ಯಥಾ
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#ಅನ್ಯಥಾ
	    gtk_widget_set_cursor(gui.drawarea, ರಿಕ್ತ);
#endif
    }
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಙ್ಕ i;

    ಪ್ರತಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	ಯದಿ (name[0] == special_ಕೀಲಿಗಳು[i].code0
		&& name[1] == special_ಕೀಲಿಗಳು[i].code1)
	    ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
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
gui_gtk4_update_scrollbar_size(ಶೂನ್ಯ)
{
    win_T	*wp;
    ಪೂರ್ಣಾಙ್ಕ		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ಎಡ].id;

	ಯದಿ (sbar == ರಿಕ್ತ || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ಎಡ]
		    && wp->w_scrollbars[SBAR_ಬಲ].id != ರಿಕ್ತ))
	    sbar = wp->w_scrollbars[SBAR_ಬಲ].id;

	ಯದಿ (sbar != ರಿಕ್ತ && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    ಪೂರ್ಣಾಙ್ಕ		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    ಯದಿ (sw > 0)
	    {
		w = sw;
		ಭಙ್ಗ;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ಯದಿ (sbar != ರಿಕ್ತ && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	ಪೂರ್ಣಾಙ್ಕ		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	ಯದಿ (sh > 0)
	    h = sh;
    }

    ಯದಿ (w != -1)
	gui.scrollbar_width = w;
    ಯದಿ (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ flag)
{
    ಯದಿ (sb->id != ರಿಕ್ತ)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	ಯದಿ (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#ಯದಿ defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ grey)
{
    ಯದಿ (menu->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ hidden)
{
    ಯದಿ (menu->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
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
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಙ್ಕ showit)
{
    ಯದಿ (gui.tabline == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ gui.tabline != ರಿಕ್ತ && gtk_widget_get_visible(gui.tabline);
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ tabpage_hover = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	ದ್ವಿಗುಣ		    x UNUSED,
	ದ್ವಿಗುಣ		    y UNUSED,
	ಶೂನ್ಯ		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	ದ್ವಿಗುಣ		    x UNUSED,
	ದ್ವಿಗುಣ		    y UNUSED,
	ಶೂನ್ಯ		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_add_css(ಶೂನ್ಯ)
{
    GtkCssProvider  *provider;
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *css =
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
# ಯದಿ GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# ಅನ್ಯಥಾ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    ಪೂರ್ಣಾಙ್ಕ idx = gtk_notebook_page_ಅಂಕೆ(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    ಯದಿ (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    ಪೂರ್ಣಾಙ್ಕ		nr = 0;
    ಪೂರ್ಣಾಙ್ಕ		tab_ಅಂಕೆ;
    ಪೂರ್ಣಾಙ್ಕ		curtabidx = 0;
    char_u	*labeltext;

    ಯದಿ (gui.tabline == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ignore_tabline_evt = TRUE;

    ಪ್ರತಿ (tp = first_tabpage; tp != ರಿಕ್ತ; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ಯದಿ (tp == curtab)
	    curtabidx = nr;

	tab_ಅಂಕೆ = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	ಯದಿ (page == ರಿಕ್ತ)
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

	    gtk_notebook_ಸೇರಿಸು_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, ರಿಕ್ತ);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_ಅಂಕೆ));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, ರಿಕ್ತ);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	ಯದಿ (label != ರಿಕ್ತ && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ಧ್ರುವ ವರ್ಣ *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ಧ್ರುವ gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    ಯಾವತ್ (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != ರಿಕ್ತ)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ಯದಿ (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    ಅನ್ಯಥಾ
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ಯದಿ (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    ಶೂನ್ಯ
gui_mch_set_curtab(ಪೂರ್ಣಾಙ್ಕ nr)
{
    ಯದಿ (gui.tabline != ರಿಕ್ತ)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ಯದಿ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ಯದಿ (ignore_tabline_evt)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ಅನ್ಯಥಾ
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, ಶೂನ್ಯ *udata)
{
    ಯದಿ (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
add_tabline_menu_item(VimMenu *menu, ಧ್ರುವ ವರ್ಣ *name, ಪೂರ್ಣಾಙ್ಕ resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_ಸೇರಿಸು_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    ಸ್ಥಿರ VimMenu *
create_tabline_popup_menu(ಶೂನ್ಯ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ಪ್ರತ್ಯಾಗಮ menu;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	ಪೂರ್ಣಾಙ್ಕ		n_press UNUSED,
	ದ್ವಿಗುಣ		x,
	ದ್ವಿಗುಣ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ಯದಿ (but == GDK_BUTTON_SECONDARY)
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
    ಅನ್ಯಥಾ ಯದಿ (but == GDK_BUTTON_PRIMARY)
    {
	ಯದಿ (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ಅನ್ಯಥಾ ಯದಿ (but == GDK_BUTTON_ಮಧ್ಯ)
    {
	ಯದಿ (tabpage_hover != 0)
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

#ಯದಿ defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, ಪೂರ್ಣಾಙ್ಕ typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ಯದಿ (sign == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    ಶೂನ್ಯ *
gui_mch_register_sign(char_u *signfile)
{
    ಯದಿ (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = ರಿಕ್ತ;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ಧ್ರುವ ವರ್ಣ *)signfile,
							    &error);
	ಯದಿ (sign != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    ಯದಿ (sign != ರಿಕ್ತ)
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
    ಪೂರ್ಣಾಙ್ಕ
gui_gtk_draw_string(ಪೂರ್ಣಾಙ್ಕ row, ಪೂರ್ಣಾಙ್ಕ col, char_u *s, ಪೂರ್ಣಾಙ್ಕ len, ಪೂರ್ಣಾಙ್ಕ flags)
{
    ಪೂರ್ಣಾಙ್ಕ cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	gtk_widget_queue_draw(gui.drawarea);

    ಪ್ರತ್ಯಾಗಮ cells;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#ಯದಿ defined(FEAT_MENU)
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
translate_mnemonic_tag(char_u *name, ಪೂರ್ಣಾಙ್ಕ use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ಪೂರ್ಣಾಙ್ಕ	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಪ್ರತಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ಯದಿ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ಯದಿ (buf != ರಿಕ್ತ)
    {
	pdest = buf;
	ಪ್ರತಿ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ಯದಿ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ಪ್ರತ್ಯಾಗಮ buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ಶೂನ್ಯ
gui_gtk_set_mnemonics(ಪೂರ್ಣಾಙ್ಕ enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ಯದಿ (menu->id == ರಿಕ್ತ)
	    ಅನುವರ್ತ;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ಯದಿ (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ಧ್ರುವ ವರ್ಣ *)name);
	vim_free(name);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
popupmenu_closed_cb(GtkWidget *popover, ಶೂನ್ಯ *udata UNUSED)
{
    gtk_widget_unparent(popover);
    ಯದಿ (gui.drawarea != ರಿಕ್ತ)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_popup_at(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ಯದಿ (menu == ರಿಕ್ತ || menu->submenu_id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

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
	    G_CALLBACK(popupmenu_closed_cb), ರಿಕ್ತ);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    ಶೂನ್ಯ
gui_make_popup(char_u *path_name, ಪೂರ್ಣಾಙ್ಕ mouse_pos)
{
    vimmenu_T	*menu;
    ಪೂರ್ಣಾಙ್ಕ		x, y;

    menu = gui_find_menu(path_name);
    ಯದಿ (menu == ರಿಕ್ತ || menu->submenu_id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ಅನ್ಯಥಾ
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    ಪೂರ್ಣಾಙ್ಕ
get_menu_tool_width(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಪೂರ್ಣಾಙ್ಕ
get_menu_tool_height(ಶೂನ್ಯ)
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

    ಪೂರ್ಣಾಙ್ಕ height = 0;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	ಪೂರ್ಣಾಙ್ಕ		h;

	ಯದಿ (widgets[i] == ರಿಕ್ತ || !gtk_widget_get_visible(widgets[i]))
	    ಅನುವರ್ತ;

	h = gtk_widget_get_height(widgets[i]);

	ಯದಿ (h == 0)
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
	ಅನ್ಯಥಾ
	    height += h;
    }
    ಪ್ರತ್ಯಾಗಮ height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    ಶೂನ್ಯ
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ಯದಿ (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    ಅನ್ಯಥಾ
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ಸ್ಥಿರ GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ಯದಿ (gui.mainwin == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    display = gtk_widget_get_display(gui.mainwin);
    ಯದಿ (display == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

    ಯದಿ (cbd == &clip_plus)
    {
	ಯದಿ (provider != ರಿಕ್ತ)
	    *provider = gui.regular_provider;
	ಪ್ರತ್ಯಾಗಮ gdk_display_get_clipboard(display);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (provider != ರಿಕ್ತ)
	    *provider = gui.primary_provider;
	ಪ್ರತ್ಯಾಗಮ gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ಧ್ರುವ ವರ್ಣ	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, ರಿಕ್ತ);
    ಯದಿ (in_stream == ರಿಕ್ತ)
    {
	read_data_free(&crd->rd);
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_ಒಳಹರಿವು_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ಮೇಲಿನ,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_ಒಳಹರಿವು_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ರಿಕ್ತ
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ರಿಕ್ತ);
    ಯದಿ (clipboard == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, ಪರಿಮಾಣ(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_ಮೇಲಿನ, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(ರಿಕ್ತ);
	ಯಾವತ್ (!g_cancellable_is_cancelled(cancel)
		&& time(ರಿಕ್ತ) < start + 3)
	    g_main_context_iteration(ರಿಕ್ತ, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    ಶೂನ್ಯ
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    ಯದಿ (in_clipboard_set)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (gdk_clipboard_is_local(clipboard))
	ಪ್ರತ್ಯಾಗಮ;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    ಪೂರ್ಣಾಙ್ಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    ಪೂರ್ಣಾಙ್ಕ			ret;

    ಯದಿ (cb == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    ಪ್ರತ್ಯಾಗಮ ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ರಿಕ್ತ);
    ಯದಿ (clipboard == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ಯದಿ (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, ರಿಕ್ತ);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ gtk4_main_loop_level;
}

    ಶೂನ್ಯ
gtk_main_quit(ಶೂನ್ಯ)
{
    gtk4_main_loop_quit = TRUE;
}

#ಯದಿ defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *mshape_css_names[] =
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
mch_set_mouse_shape(ಪೂರ್ಣಾಙ್ಕ shape)
{
    GdkCursor	*c;
    ಧ್ರುವ ವರ್ಣ	*css_name = "default";

    ಯದಿ (gui.drawarea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    ಅನ್ಯಥಾ
    {
	ಯದಿ (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ಅನ್ಯಥಾ ಯದಿ (shape < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	ಅನ್ಯಥಾ
	    ಪ್ರತ್ಯಾಗಮ;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ರಿಕ್ತ);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ಯದಿ (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_text_area_w = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ last_text_area_h = 0;

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
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ toolbar_icon_names[] =
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
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    ಸ್ಥಿರ GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ರಿಕ್ತ;

    // Try specified icon file first
    ಯದಿ (menu->iconfile != ರಿಕ್ತ)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ಯದಿ (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ಧ್ರುವ ವರ್ಣ *)buf, ರಿಕ್ತ);

	    ಯದಿ (texture != ರಿಕ್ತ)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ಯದಿ (image == ರಿಕ್ತ)
    {
	ಧ್ರುವ ವರ್ಣ *icon_name = "image-missing";
	ಪೂರ್ಣಾಙ್ಕ n = (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(toolbar_icon_names);

	ಯದಿ (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ಪ್ರತ್ಯಾಗಮ image;
}
# endif

    ಸ್ಥಿರ ಶೂನ್ಯ
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    ಯದಿ (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    ಅನ್ಯಥಾ ಯದಿ (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ಸ್ಥಿರ gboolean did_ಸಂದೇಶ = FALSE;

	ಯದಿ (State & MODE_CMDLINE)
	    ಪ್ರತ್ಯಾಗಮ;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	ಯದಿ (tooltip != ರಿಕ್ತ && utf_valid_string(tooltip, ರಿಕ್ತ))
	{
	    ಸಂದೇಶ((ವರ್ಣ *)tooltip);
	    did_ಸಂದೇಶ = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	ಅನ್ಯಥಾ ಯದಿ (did_ಸಂದೇಶ)
	{
	    ಸಂದೇಶ("");
	    did_ಸಂದೇಶ = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    ಶೂನ್ಯ
gui_mch_add_menu(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    ಯದಿ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	ಪ್ರತ್ಯಾಗಮ;
    }

    parent = menu->parent;

    ಯದಿ ((parent != ರಿಕ್ತ && parent->submenu_id == ರಿಕ್ತ)
	    || !menu_is_menubar(menu->name))
	ಪ್ರತ್ಯಾಗಮ;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != ರಿಕ್ತ || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    ಯದಿ (parent != ರಿಕ್ತ)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ಧ್ರುವ ವರ್ಣ *)text, ರಿಕ್ತ, ರಿಕ್ತ));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ಸೇರಿಸು_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    ಅನ್ಯಥಾ
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ಧ್ರುವ ವರ್ಣ *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ಸೇರಿಸು_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    ಶೂನ್ಯ
gui_mch_add_menu_item(vimmenu_T *menu, ಪೂರ್ಣಾಙ್ಕ idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    ಯದಿ (parent != ರಿಕ್ತ && menu_is_toolbar(parent->name))
    {
	ಯದಿ (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ಸೇರಿಸು_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ಅನ್ಯಥಾ
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    ಯದಿ (tooltip != ರಿಕ್ತ && !utf_valid_string(tooltip, ರಿಕ್ತ))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ಸೇರಿಸು_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ಧ್ರುವ ವರ್ಣ *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ಧ್ರುವ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
# endif

    // Menu items (non-toolbar)
    ಯದಿ (parent == ರಿಕ್ತ || parent->submenu_id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ಸೇರಿಸು_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    ಅನ್ಯಥಾ
    {
	char_u	    *text;
	char_u	    *accel_text = ರಿಕ್ತ;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	ಯದಿ (menu->actext != ರಿಕ್ತ && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ಧ್ರುವ ವರ್ಣ *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	ಯದಿ (accel_text != ರಿಕ್ತ)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ಧ್ರುವ ವರ್ಣ *)accel_text);

	vim_menu_ಸೇರಿಸು_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    ಶೂನ್ಯ
gui_mch_toggle_tearoffs(ಪೂರ್ಣಾಙ್ಕ enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    ಶೂನ್ಯ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ಯದಿ (menu->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    ಯದಿ (tooltip != ರಿಕ್ತ && utf_valid_string(tooltip, ರಿಕ್ತ))
	gtk_widget_set_tooltip_text(menu->id, (ಧ್ರುವ ವರ್ಣ *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    ಶೂನ್ಯ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ಯದಿ (menu->parent != ರಿಕ್ತ && menu_is_toolbar(menu->parent->name))
    {
	ಯದಿ (menu->id != ರಿಕ್ತ)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ರಿಕ್ತ;
	}
	ಪ್ರತ್ಯಾಗಮ;
    }

    // For popup menus, unparent the menu as well
    ಯದಿ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ಯದಿ (menu->submenu_id != ರಿಕ್ತ)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ಅನ್ಯಥಾ ಯದಿ (menu->parent == ರಿಕ್ತ && menu->id != ರಿಕ್ತ)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ಅನ್ಯಥಾ ಯದಿ (menu->id != ರಿಕ್ತ)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    ಶೂನ್ಯ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ಪೂರ್ಣಾಙ್ಕ x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
show_menubar_popover(ಶೂನ್ಯ)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ಯದಿ (gui.menubar == ರಿಕ್ತ || gui.drawarea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ರಿಕ್ತ);
	ಪ್ರತ್ಯಾಗಮ;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ರಿಕ್ತ);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    ಯದಿ (sb->id == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ಪ್ರತ್ಯಾಗಮ;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_ಮೌಲ್ಯ(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    ಶೂನ್ಯ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    ಯದಿ (sb->id != ರಿಕ್ತ)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_xpadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ formwin_w = gtk_widget_get_width(gui.formwin);
    ಪೂರ್ಣಾಙ್ಕ sbar_w = 0;
    ಪೂರ್ಣಾಙ್ಕ xpad;

    ಯದಿ (gui.which_scrollbars[SBAR_ಎಡ])
	sbar_w += gui.scrollbar_width;
    ಯದಿ (gui.which_scrollbars[SBAR_ಬಲ])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    ಪ್ರತ್ಯಾಗಮ (xpad < 0) ? 0 : xpad;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_get_scrollbar_ypadding(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ formwin_h = gtk_widget_get_height(gui.formwin);
    ಪೂರ್ಣಾಙ್ಕ ypad;

    ypad = formwin_h - last_text_area_h;
    ಯದಿ (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    ಪ್ರತ್ಯಾಗಮ (ypad < 0) ? 0 : ypad;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
adjustment_ಮೌಲ್ಯ_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long ಮೌಲ್ಯ = (long)gtk_adjustment_get_ಮೌಲ್ಯ(adj);

    ಯದಿ (sb != ರಿಕ್ತ)
	gui_drag_scrollbar(sb, ಮೌಲ್ಯ, FALSE);
}

    ಶೂನ್ಯ
gui_mch_create_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಙ್ಕ orient)
{
    GtkAdjustment *adj;
    ಯದಿ (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ರಿಕ್ತ);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_ಮೌಲ್ಯ_changed), ರಿಕ್ತ);
}

    ಶೂನ್ಯ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ಯದಿ (sb->id != ರಿಕ್ತ)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ರಿಕ್ತ;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    ಶೂನ್ಯ
gui_mch_set_text_area_pos(ಪೂರ್ಣಾಙ್ಕ x, ಪೂರ್ಣಾಙ್ಕ y, ಪೂರ್ಣಾಙ್ಕ w, ಪೂರ್ಣಾಙ್ಕ h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ಯದಿ (w <= 0 || h <= 0)
	ಪ್ರತ್ಯಾಗಮ;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    ಶೂನ್ಯ
gui_gtk4_calculate_bleed(ಪೂರ್ಣಾಙ್ಕ width, ಪೂರ್ಣಾಙ್ಕ height)
{
    ಪೂರ್ಣಾಙ್ಕ old_ಬಲ = gui.bleed_ಬಲ;
    ಪೂರ್ಣಾಙ್ಕ old_bot = gui.bleed_bot;

    gui.bleed_ಬಲ = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    ಯದಿ (gui.which_scrollbars[SBAR_ಎಡ])
	gui.bleed_ಬಲ -= gui.scrollbar_width;
    ಯದಿ (gui.which_scrollbars[SBAR_ಬಲ])
	gui.bleed_ಬಲ -= gui.scrollbar_width;
    ಯದಿ (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    ಯದಿ (gui.bleed_ಬಲ < 0)
	gui.bleed_ಬಲ = 0;
    ಯದಿ (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ಯದಿ (old_ಬಲ != gui.bleed_ಬಲ || old_bot != gui.bleed_bot)
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
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, ರಿಕ್ತ);
    fdd->done = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, ರಿಕ್ತ);
    fdd->done = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, ರಿಕ್ತ);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(ಪೂರ್ಣಾಙ್ಕ saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = ರಿಕ್ತ;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    ಯದಿ (initdir == ರಿಕ್ತ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ಅನ್ಯಥಾ ಯದಿ (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ಯದಿ (title != ರಿಕ್ತ)
	gtk_file_dialog_set_title(dlg, (ಧ್ರುವ ವರ್ಣ *)title);

    {
	GFile *dir = g_file_new_for_path((ಧ್ರುವ ವರ್ಣ *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ಯದಿ (filter != ರಿಕ್ತ && *filter != NUL)
    {
	ಪೂರ್ಣಾಙ್ಕ		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ಯದಿ (patt != ರಿಕ್ತ)
	{
	    ಯಾವತ್ (p != ರಿಕ್ತ && *p != NUL)
	    {
		ಯದಿ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ಯದಿ (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    ಅನ್ಯಥಾ
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			ಯದಿ (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ಯದಿ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ಅನ್ಯಥಾ
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

    ಯದಿ (saving && dflt != ರಿಕ್ತ && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ಧ್ರುವ ವರ್ಣ *)dflt);

    fdd.result = ರಿಕ್ತ;
    fdd.done = FALSE;

    ಯದಿ (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), ರಿಕ್ತ,
		file_dialog_save_cb, &fdd);
    ಅನ್ಯಥಾ
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), ರಿಕ್ತ,
		file_dialog_open_cb, &fdd);

    ಯಾವತ್ (!fdd.done)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);

    ಯದಿ (fdd.result != ರಿಕ್ತ)
    {
	ವರ್ಣ *path = g_file_get_path(fdd.result);
	ಯದಿ (path != ರಿಕ್ತ)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ಪ್ರತ್ಯಾಗಮ result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = ರಿಕ್ತ;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ಯದಿ (title != ರಿಕ್ತ)
	gtk_file_dialog_set_title(dlg, (ಧ್ರುವ ವರ್ಣ *)title);

    ಯದಿ (initdir != ರಿಕ್ತ && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ಧ್ರುವ ವರ್ಣ *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = ರಿಕ್ತ;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), ರಿಕ್ತ,
	    file_dialog_folder_cb, &fdd);

    ಯಾವತ್ (!fdd.done)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);

    ಯದಿ (fdd.result != ರಿಕ್ತ)
    {
	ವರ್ಣ *path = g_file_get_path(fdd.result);
	ಯದಿ (path != ರಿಕ್ತ)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ಪ್ರತ್ಯಾಗಮ result;
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
    ಸ್ಥಿರ ವರ್ಣ **
split_button_string(char_u *button_string, ಪೂರ್ಣಾಙ್ಕ *n_buttons, ವರ್ಣ **tofree)
{
    ವರ್ಣ	    **array;
    char_u	    *p;
    unsigned ಪೂರ್ಣಾಙ್ಕ    ಎಣಿಕೆ = 1;

    button_string = (char_u *)g_strdup((ಧ್ರುವ ವರ್ಣ *)button_string);

    ಪ್ರತಿ (p = button_string; *p != NUL; ++p)
	ಯದಿ (*p == DLG_BUTTON_SEP)
	    ++ಎಣಿಕೆ;

    array = g_malloc_n(ಎಣಿಕೆ, ಪರಿಮಾಣ(ವರ್ಣ *));
    ಎಣಿಕೆ = 0;

    ಯದಿ (array != ರಿಕ್ತ)
    {
	array[ಎಣಿಕೆ++] = (ವರ್ಣ *)button_string;
	ಪ್ರತಿ (p = button_string; *p != NUL; )
	{
	    ಯದಿ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ಎಣಿಕೆ++] = (ವರ್ಣ *)p;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ಅನ್ಯಥಾ
		MB_PTR_ADV(p);
	}
    }

    *tofree = (ವರ್ಣ *)button_string;
    *n_buttons = ಎಣಿಕೆ;
    ಪ್ರತ್ಯಾಗಮ array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ *
dialog_type_to_icon(ಪೂರ್ಣಾಙ್ಕ type)
{
    ವಿಕಲ್ಪ (type)
    {
	ಪಕ್ಷ VIM_ERROR:
	    ಪ್ರತ್ಯಾಗಮ "dialog-error-symbolic";
	ಪಕ್ಷ VIM_WARNING:
	    ಪ್ರತ್ಯಾಗಮ "dialog-warning-symbolic";
	ಪಕ್ಷ VIM_INFO:
	    ಪ್ರತ್ಯಾಗಮ "dialog-information-symbolic";
	ಪಕ್ಷ VIM_QUESTION:
	    ಪ್ರತ್ಯಾಗಮ "dialog-question-symbolic";
	ಸ್ವಭಾವ:
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

// Data associated with each button in the dialog
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    ಪೂರ್ಣಾಙ್ಕ		but_idx;
    ಪೂರ್ಣಾಙ್ಕ		*response;
    gboolean	*done;
} DialogButtonState;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    ಸ್ಥಿರ gboolean
dialog_ಕೀಲಿ_pressed_cb(
	Gtk_Event_Controller_ಕೀಲಿ	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ಯದಿ (keyval == GDK_ಕೀಲಿ_Escape)
    {
	*dstate->done = TRUE;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	ಪ್ರತ್ಯಾಗಮ gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
gui_mch_dialog(
	ಪೂರ್ಣಾಙ್ಕ	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ಪೂರ್ಣಾಙ್ಕ	def_but,
	char_u	*textfield,
	ಪೂರ್ಣಾಙ್ಕ	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ಧ್ರುವ ವರ್ಣ		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = ರಿಕ್ತ;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *ಕೀಲಿ_controller;
    DialogButtonState	*but_states = ರಿಕ್ತ;
    ವರ್ಣ		*tofree = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    ಯದಿ (utf8_title != ರಿಕ್ತ)
	gtk_window_set_title(win, (ಧ್ರುವ ವರ್ಣ *)utf8_title);
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
    gtk_widget_set_margin_ತುದಿ(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    ಯದಿ (icon_name != ರಿಕ್ತ)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ಧ್ರುವ ವರ್ಣ *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    ಕೀಲಿ_controller = gtk_event_controller_ಕೀಲಿ_new();
    g_signal_connect(ಕೀಲಿ_controller, "key-pressed",
	    G_CALLBACK(dialog_ಕೀಲಿ_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), ಕೀಲಿ_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    ಯದಿ (textfield != ರಿಕ್ತ)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	ಯದಿ (utf8_text != ರಿಕ್ತ)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ಧ್ರುವ ವರ್ಣ *)utf8_text);
	ಅನ್ಯಥಾ
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    ಅನ್ಯಥಾ
    {
	GListModel  *controllers;
	ಪೂರ್ಣಾಙ್ಕ	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_ವಸ್ತುಗಳು(controllers);
	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    ಯದಿ (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ಯದಿ (buttons != ರಿಕ್ತ)
    {
	GtkWidget   *but_box;
	ವರ್ಣ	    **buttons_arr; // Note that array is allocated, not strings
	ಪೂರ್ಣಾಙ್ಕ	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ಯದಿ (vim_strchr(p_go, GO_VERTICAL) != ರಿಕ್ತ)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ಅನ್ಯಥಾ
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, ಪರಿಮಾಣ(DialogButtonState));

	ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    ಯದಿ (but_label == ರಿಕ್ತ)
		ಅನುವರ್ತ;

	    but = gtk_button_new_with_mnemonic((ವರ್ಣ *)but_label);
	    ಯದಿ (i == def_but - 1)
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

    ಯಾವತ್ (!done && !win_closed)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);

    ಯದಿ (done)
    {
	ಯದಿ (textfield != ರಿಕ್ತ)
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
    ಪ್ರತ್ಯಾಗಮ response == -1 ? 0 : response + 1;
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

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
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

ಸ್ಥಿರ SharedFindReplace find_widgets = {0};
ಸ್ಥಿರ SharedFindReplace repl_widgets = {0};

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    ಪೂರ್ಣಾಙ್ಕ			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    ಯದಿ (flags == FRD_FINDNEXT)
    {
	repl_text = ರಿಕ್ತ;
	sfr = &find_widgets;
    }
    ಅನ್ಯಥಾ
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    ಯದಿ (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    ಯದಿ (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = ರಿಕ್ತ;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ಧ್ರುವ gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    ಯದಿ (!entry_text)
	ಪ್ರತ್ಯಾಗಮ; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    ಯದಿ (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    ಯದಿ (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    ಸ್ಥಿರ gboolean
find_ಕೀಲಿ_pressed_cb(
	Gtk_Event_Controller_ಕೀಲಿ	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ಯದಿ (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // the Escape key synthesizes a cancellation action
    ಯದಿ (keyval == GDK_ಕೀಲಿ_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
entry_activate_cb(GtkWidget *widget UNUSED, ಶೂನ್ಯ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ಸ್ಥಿರ ಶೂನ್ಯ
find_replace_dialog_create(char_u *arg, ಪೂರ್ಣಾಙ್ಕ do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    ಪೂರ್ಣಾಙ್ಕ			wword = FALSE;
    ಪೂರ್ಣಾಙ್ಕ			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*ಕೀಲಿ_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    ಯದಿ (entry_text != ರಿಕ್ತ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ರಿಕ್ತ);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ಯದಿ (frdp->dialog != ರಿಕ್ತ)
    {
	ಯದಿ (entry_text != ರಿಕ್ತ)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (ವರ್ಣ *)entry_text);
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
	ಯದಿ (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ಪ್ರತ್ಯಾಗಮ;
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
    gtk_widget_set_margin_ತುದಿ(vertbox, 12);
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
    sensitive = (entry_text != ರಿಕ್ತ && entry_text[0] != NUL);
    ಯದಿ (entry_text != ರಿಕ್ತ)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (ವರ್ಣ *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ಯದಿ (do_replace)
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
    ಅನ್ಯಥಾ
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

    ಕೀಲಿ_controller = gtk_event_controller_ಕೀಲಿ_new();
    g_signal_connect(ಕೀಲಿ_controller, "key-pressed",
	    G_CALLBACK(find_ಕೀಲಿ_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), ಕೀಲಿ_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ಯದಿ (do_replace)
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
    ಯದಿ (do_replace && entry_text != ರಿಕ್ತ && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    ಶೂನ್ಯ
gui_mch_find_dialog(exarg_T *eap)
{
    ಯದಿ (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    ಶೂನ್ಯ
gui_mch_replace_dialog(exarg_T *eap)
{
    ಯದಿ (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    ಶೂನ್ಯ
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
ಸ್ಥಿರ GtkPrintDialog *print_dialog = ರಿಕ್ತ;
ಸ್ಥಿರ GtkPrintSettings *print_settings = ರಿಕ್ತ;
ಸ್ಥಿರ GtkPageSetup *page_setup = ರಿಕ್ತ;
ಸ್ಥಿರ GtkPrintSetup *print_setup = ರಿಕ್ತ;
ಸ್ಥಿರ GFile *print_file;

    ಸ್ಥಿರ ಶೂನ್ಯ
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = ರಿಕ್ತ;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    ಯದಿ (print_setup == ರಿಕ್ತ)
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
	ದ್ವಿಗುಣ		*page_width,
	ದ್ವಿಗುಣ		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ಧ್ರುವ ವರ್ಣ *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = ರಿಕ್ತ;
    print_setup = ರಿಕ್ತ;

    ಯದಿ (print_settings == ರಿಕ್ತ)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ಯದಿ (opt->present)
	{
	    ಯದಿ (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ಅನ್ಯಥಾ ಯದಿ (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ಅನ್ಯಥಾ
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ಯದಿ (opt->present)
	{
	    ಯದಿ (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    ಅನ್ಯಥಾ
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ಯದಿ (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ಧ್ರುವ ವರ್ಣ *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ಯದಿ (opt->present)
	{
	    ಯದಿ (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    ಅನ್ಯಥಾ
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    ಯದಿ (page_setup == ರಿಕ್ತ)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), ರಿಕ್ತ,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    ಯಾವತ್ (!done)
	g_main_context_iteration(ರಿಕ್ತ, TRUE);

    ಯದಿ (print_setup == ರಿಕ್ತ)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_ಸಂದೇಶ = FALSE;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
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
    ಯದಿ (temp == ರಿಕ್ತ)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_ಸಂದೇಶ = FALSE;
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    print_file = g_file_new_for_path((ವರ್ಣ *)temp);

    print_dialog = dialog;

    ಪ್ರತ್ಯಾಗಮ temp;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    ವರ್ಣ *path = g_file_get_path(file);

    (ಶೂನ್ಯ)gtk_print_dialog_print_file_finish(dialog, result, ರಿಕ್ತ);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    ಶೂನ್ಯ
gui_gtk4_print_finish(ಶೂನ್ಯ)
{
    ಯದಿ (print_dialog != ರಿಕ್ತ)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, ರಿಕ್ತ,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ರಿಕ್ತ;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    GdkTexture *texture;
} image_gdk_T;

ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    Gsk_Render_ಗಂಟು   **nodes; // Note that elements may be NULL
    ಪೂರ್ಣಾಙ್ಕ		    n_nodes;
} image_placement_gdk_T;

    ಪೂರ್ಣಾಙ್ಕ
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    ಪೂರ್ಣಾಙ್ಕ		    w, h;

    ಯದಿ (ctx == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    ಅನ್ಯಥಾ
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
clear_nodes(image_placement_gdk_T *ctx)
{
    ಯದಿ (ctx->nodes == ರಿಕ್ತ || gui.drawarea == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < ctx->n_nodes; i++)
    {
	ಯದಿ (ctx->nodes[i] == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_ಗಂಟು_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    ಪೂರ್ಣಾಙ್ಕ
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ಯದಿ (ctx == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    place->backend_data = ctx;
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಶೂನ್ಯ
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    ಶೂನ್ಯ
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    ಪೂರ್ಣಾಙ್ಕ			    n_rects;
    ಪೂರ್ಣಾಙ್ಕ			    iw, ih;
    Gsk_Render_ಗಂಟು	    *texture_ಗಂಟು;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ಯದಿ (rects == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    clear_nodes(ctx);
    ಯದಿ (n_rects == 0)
	ಪ್ರತ್ಯಾಗಮ;

    image_get_dimensions(img, &iw, &ih);
    texture_ಗಂಟು = gsk_texture_ಗಂಟು_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ಯದಿ (texture_ಗಂಟು == ರಿಕ್ತ)
	// Not sure if this can happen...
	ಪ್ರತ್ಯಾಗಮ;

    ctx->nodes = g_malloc_n(n_rects, ಪರಿಮಾಣ(Gsk_Render_ಗಂಟು *));
    ctx->n_nodes = n_rects;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಙ್ಕ		row, col;
	ಪೂರ್ಣಾಙ್ಕ		posx, posy;
	ಪೂರ್ಣಾಙ್ಕ		x, y, w, h;
	Gsk_Render_ಗಂಟು	*ಗಂಟು;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	ಗಂಟು = gsk_clip_ಗಂಟು_new(texture_ಗಂಟು,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ಯದಿ (ಗಂಟು != ರಿಕ್ತ)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), ಗಂಟು);

	ctx->nodes[i] = ಗಂಟು;
    }
    gsk_render_ಗಂಟು_unref(texture_ಗಂಟು);
}

    ಶೂನ್ಯ
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
