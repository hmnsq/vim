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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_GUI_GTK

#ଅନ୍ତର୍ଭାଵ <gdk/gdkkeysyms.h>
#ଅନ୍ତର୍ଭାଵ <gdk/gdk.h>
#ଅନ୍ତର୍ଭାଵ <gtk/gtk.h>
#ଅନ୍ତର୍ଭାଵ "gui_gtk4_f.h"
#ଅନ୍ତର୍ଭାଵ "gui_gtk4_cb.h"
#ଅନ୍ତର୍ଭାଵ "gui_gtk4_da.h"
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
#define No_ମୂଲ୍ୟ		0x0000
#define Width_ମୂଲ୍ୟ	0x0004
#define Height_ମୂଲ୍ୟ	0x0008

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_parse_geometry(ଅଚଳ ଅକ୍ଷର *str, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *width, unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା *height)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା mask = No_ମୂଲ୍ୟ;
    ଅକ୍ଷର *end;
    long val;

    ଯଦି (str == ଖାଲି || *str == NUL)
	ଫେରନ୍ତୁ mask;

    // Parse width
    ଯଦି (*str != '+' && *str != '-')
    {
	val = strtol(str, &end, 10);
	ଯଦି (end != str)
	{
	    *width = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val;
	    mask |= Width_ମୂଲ୍ୟ;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    ଯଦି (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &end, 10);
	ଯଦି (end != str)
	{
	    *height = (unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)val;
	    mask |= Height_ମୂଲ୍ୟ;
	    str = end;
	}
    }

    ଫେରନ୍ତୁ mask;
}

#ଯଦି defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
ସ୍ଥିର ଗଣନା {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gtk4_main_loop_level = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
ସ୍ଥିର ସଂରଚନା special_ଚାବି
{
    guint ଚାବି_sym;
    char_u code0;
    char_u code1;
}
ଅଚଳ special_ଚାବିଗୁଡ଼ିକ[] =
{
    {GDK_ଚାବି_Up,	'k', 'u'},
    {GDK_ଚାବି_Down,	'k', 'd'},
    {GDK_ଚାବି_ବାମ,	'k', 'l'},
    {GDK_ଚାବି_ଡାହାଣ,	'k', 'r'},
    {GDK_ଚାବି_F1,	'k', '1'},
    {GDK_ଚାବି_F2,	'k', '2'},
    {GDK_ଚାବି_F3,	'k', '3'},
    {GDK_ଚାବି_F4,	'k', '4'},
    {GDK_ଚାବି_F5,	'k', '5'},
    {GDK_ଚାବି_F6,	'k', '6'},
    {GDK_ଚାବି_F7,	'k', '7'},
    {GDK_ଚାବି_F8,	'k', '8'},
    {GDK_ଚାବି_F9,	'k', '9'},
    {GDK_ଚାବି_F10,	'k', ';'},
    {GDK_ଚାବି_F11,	'F', '1'},
    {GDK_ଚାବି_F12,	'F', '2'},
    {GDK_ଚାବି_Help,	'%', '1'},
    {GDK_ଚାବି_Undo,	'&', '8'},
    {GDK_ଚାବି_Back_Space,	'k', 'b'},
    {GDK_ଚାବି_ଭର୍ତ୍ତି,	'k', 'I'},
    {GDK_ଚାବି_Delete,	'k', 'D'},
    {GDK_ଚାବି_Home,	'k', 'h'},
    {GDK_ଚାବି_End,	'@', '7'},
    {GDK_ଚାବି_Prior,	'k', 'P'},
    {GDK_ଚାବି_Next,	'k', 'N'},
    {GDK_ଚାବି_Print,	'%', '9'},
    {GDK_ଚାବି_KP_ବାମ,	'k', 'l'},
    {GDK_ଚାବି_KP_ଡାହାଣ,	'k', 'r'},
    {GDK_ଚାବି_KP_Up,	'k', 'u'},
    {GDK_ଚାବି_KP_Down,	'k', 'd'},
    {GDK_ଚାବି_KP_ଭର୍ତ୍ତି,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_ଚାବି_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_ଚାବି_KP_Home,	'K', '1'},
    {GDK_ଚାବି_KP_End,	'K', '4'},
    {GDK_ଚାବି_KP_Prior,	'K', '3'},
    {GDK_ଚାବି_KP_Next,	'K', '5'},
    {GDK_ଚାବି_KP_Add,	'K', '6'},
    {GDK_ଚାବି_KP_Subtract, 'K', '7'},
    {GDK_ଚାବି_KP_Divide,	'K', '8'},
    {GDK_ଚାବି_KP_Multiply, 'K', '9'},
    {GDK_ଚାବି_KP_Enter,	'K', 'A'},
    {GDK_ଚାବି_KP_Decimal,	'K', 'B'},
    {GDK_ଚାବି_KP_0,	'K', 'C'},
    {GDK_ଚାବି_KP_1,	'K', 'D'},
    {GDK_ଚାବି_KP_2,	'K', 'E'},
    {GDK_ଚାବି_KP_3,	'K', 'F'},
    {GDK_ଚାବି_KP_4,	'K', 'G'},
    {GDK_ଚାବି_KP_5,	'K', 'H'},
    {GDK_ଚାବି_KP_6,	'K', 'I'},
    {GDK_ଚାବି_KP_7,	'K', 'J'},
    {GDK_ଚାବି_KP_8,	'K', 'K'},
    {GDK_ଚାବି_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
keyval_to_string(unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା keyval, char_u *string)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    ଯଦି (uc != 0)
    {
	len = utf_char2bytes((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)uc, string);
    }
    ଅନ୍ୟଥା
    {
	len = 1;
	ବାଛ (keyval)
	{
	    ସ୍ଥିତି GDK_ଚାବି_Tab: ସ୍ଥିତି GDK_ଚାବି_KP_Tab: ସ୍ଥିତି GDK_ଚାବି_ISO_ବାମ_Tab:
		string[0] = TAB;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_ଚାବି_Linefeed:
		string[0] = NL;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_ଚାବି_Return: ସ୍ଥିତି GDK_ଚାବି_ISO_Enter: ସ୍ଥିତି GDK_ଚାବି_3270_Enter:
		string[0] = CAR;
		ଭାଙ୍ଗ;
	    ସ୍ଥିତି GDK_ଚାବି_Escape:
		string[0] = ESC;
		ଭାଙ୍ଗ;
	    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		len = 0;
		ଭାଙ୍ଗ;
	}
    }
    string[len] = NUL;
    ଫେରନ୍ତୁ len;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
modifiers_gdk2vim(guint state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers = 0;

    ଯଦି (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    ଯଦି (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    ଯଦି (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    ଯଦି (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    ଯଦି (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    ଫେରନ୍ତୁ modifiers;
}

ସ୍ଥିର GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
ସ୍ଥିର gboolean ଚାବି_press_event(Gtk_Event_Controller_ଚାବି *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ ଚାବି_release_event(Gtk_Event_Controller_ଚାବି *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ button_press_event(GtkGestureClick *gesture, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_press, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ button_release_event(GtkGestureClick *gesture, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_press, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ motion_notify_event(GtkEventControllerMotion *controller, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ enter_notify_event(GtkEventControllerMotion *controller, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data);
ସ୍ଥିର gboolean scroll_event(GtkEventControllerScroll *controller, ଦ୍ୱିଗୁଣ dx, ଦ୍ୱିଗୁଣ dy, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ focus_in_event(GtkEventControllerFocus *controller, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
ସ୍ଥିର gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, ଶୂନ୍ୟ *data);
#endif
#ifdef FEAT_GUI_TABLINE
ସ୍ଥିର ଶୂନ୍ୟ tabline_enter_cb(GtkEventController *controller, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, ଶୂନ୍ୟ *udata);
ସ୍ଥିର ଶୂନ୍ୟ on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
ସ୍ଥିର VimMenu *create_tabline_popup_menu(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ tabline_add_css(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ tabline_menu_press_event(GtkGestureClick *gesture, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_press, ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, GtkWidget *popover);
#endif
ସ୍ଥିର ଶୂନ୍ୟ mainwin_destroy_cb(GObject *object, gpointer data);
ସ୍ଥିର gboolean delete_event_cb(GtkWindow *window, gpointer data);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା query_pointer_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y, GdkModifierType *state);
ସ୍ଥିର ଶୂନ୍ୟ mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
ସ୍ଥିର ଶୂନ୍ୟ set_form_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height);
ସ୍ଥିର ଶୂନ୍ୟ drawarea_realize_cb(GtkWidget *widget, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
ସ୍ଥିର ଶୂନ୍ୟ clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
ସ୍ଥିର ଶୂନ୍ୟ show_menubar_popover(ଶୂନ୍ୟ);
#endif

ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *prgname = ଖାଲି;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    ସ୍ଥିର gboolean
arg_match(ଅଚଳ ଅକ୍ଷର *s, ଅଚଳ ଅକ୍ଷର *name, ଅକ୍ଷର **ମୂଲ୍ୟ)
{
    size_t  len = strlen(name);

    ଯଦି (strncmp(s, name, len) != 0)
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (s[len] == '=')
	*ମୂଲ୍ୟ = (ଅକ୍ଷର *)s + len + 1;
    ଅନ୍ୟଥା ଯଦି (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ଶୂନ୍ୟ
gui_mch_prepare(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *argc, ଅକ୍ଷର **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i = 0;

    ଯେତେବେଳେ (i < *argc)
    {
	ଅକ୍ଷର	*s = argv[i];
	ଅକ୍ଷର	*ମୂଲ୍ୟ = ଖାଲି;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	has_inline_ମୂଲ୍ୟ = FALSE;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n_strip;

	ଯଦି (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    ଜାରି;
	}

	ଯଦି (strchr(s, '=') != ଖାଲି)
	    has_inline_ମୂଲ୍ୟ = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ଯଦି (!has_inline_ମୂଲ୍ୟ)
	{
	    ଯଦି (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		ମୂଲ୍ୟ = argv[i + 1];
	    ଅନ୍ୟଥା
		ମୂଲ୍ୟ = ଖାଲି;
	}

	ଯଦି (arg_match(s, "-fn", &ମୂଲ୍ୟ) || arg_match(s, "-font", &ମୂଲ୍ୟ))
	    font_argument = ମୂଲ୍ୟ;
	ଅନ୍ୟଥା ଯଦି (arg_match(s, "-geom", &ମୂଲ୍ୟ)
		|| arg_match(s, "-geometry", &ମୂଲ୍ୟ))
	{
	    ଯଦି (ମୂଲ୍ୟ != ଖାଲି)
		gui.geom = vim_strsave((char_u *)ମୂଲ୍ୟ);
	}
	ଅନ୍ୟଥା ଯଦି (arg_match(s, "-bg", &ମୂଲ୍ୟ)
		|| arg_match(s, "-background", &ମୂଲ୍ୟ))
	    background_argument = ମୂଲ୍ୟ;
	ଅନ୍ୟଥା ଯଦି (arg_match(s, "-fg", &ମୂଲ୍ୟ)
		|| arg_match(s, "-foreground", &ମୂଲ୍ୟ))
	    foreground_argument = ମୂଲ୍ୟ;
#ifdef FEAT_NETBEANS_INTG
	ଅନ୍ୟଥା ଯଦି (strncmp(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_ମୂଲ୍ୟ = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	ଅନ୍ୟଥା ଯଦି (arg_match(s, "--prg-name", &ମୂଲ୍ୟ))
	    // GTK4 specific
	    prgname = ମୂଲ୍ୟ;
	ଅନ୍ୟଥା
	{
	    i++;
	    ଜାରି;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ଯଦି (ମୂଲ୍ୟ != ଖାଲି && !has_inline_ମୂଲ୍ୟ)
	    n_strip = 2;

	ଯଦି (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ଯଦି (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * ଆକାର(ଅକ୍ଷର *));
	    argv[*argc] = ଖାଲି;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    ଶୂନ୍ୟ
gui_mch_free_all(ଶୂନ୍ୟ)
{
}

    ସ୍ଥିର guint
timeout_add(ପୂର୍ଣ୍ଣସଂଖ୍ୟା time, timeout_cb_type (*callback)(gpointer), ପୂର୍ଣ୍ଣସଂଖ୍ୟା *flagp)
{
    ଫେରନ୍ତୁ g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    ସ୍ଥିର ଶୂନ୍ୟ
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

ସ୍ଥିର long_u blink_waittime = 700;
ସ୍ଥିର long_u blink_ontime = 400;
ସ୍ଥିର long_u blink_offtime = 250;
ସ୍ଥିର guint blink_timer = 0;

    ସ୍ଥିର timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    ଯଦି (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, ଖାଲି);
    }
    ଅନ୍ୟଥା
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, ଖାଲି);
    }
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blinking(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state != BLINK_NONE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_is_blink_off(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ blink_state == BLINK_OFF;
}

    ଶୂନ୍ୟ
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    ଶୂନ୍ୟ
gui_mch_stop_blink(ପୂର୍ଣ୍ଣସଂଖ୍ୟା may_call_gui_update_cursor)
{
    ଯଦି (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ଯଦି (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    ଶୂନ୍ୟ
gui_mch_start_blink(ଶୂନ୍ୟ)
{
    ଯଦି (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ଯଦି (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, ଖାଲି);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_early_init_check(ପୂର୍ଣ୍ଣସଂଖ୍ୟା give_message UNUSED)
{
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_check(ଶୂନ୍ୟ)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == ଖାଲି ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ଯଦି (g_getenv("EGL_LOG_LEVEL") == ଖାଲି)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    ଫେରନ୍ତୁ OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init(ଶୂନ୍ୟ)
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
		     G_CALLBACK(delete_event_cb), ଖାଲି);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ଖାଲି);

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
		     G_CALLBACK(on_select_tab), ଖାଲି);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ଖାଲି);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ଖାଲି);

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
		     G_CALLBACK(drawarea_realize_cb), ଖାଲି);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ଖାଲି);

    // Set up event controllers.
    {
	GtkEventController *ଚାବି_ctrl = gtk_event_controller_ଚାବି_new();
	g_signal_connect(ଚାବି_ctrl, "key-pressed",
			 G_CALLBACK(ଚାବି_press_event), ଖାଲି);
	g_signal_connect(ଚାବି_ctrl, "key-released",
			 G_CALLBACK(ଚାବି_release_event), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea, ଚାବି_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ଖାଲି);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ଖାଲି);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), ଖାଲି);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(ଖାଲି,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), ଖାଲି);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", ଖାଲି);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ଯଦି (primary != ଖାଲି)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	ଯଦି (regular != ଖାଲି)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
gui_mch_new_colors(ଶୂନ୍ୟ)
{
    ଯଦି (gui.drawarea != ଖାଲି && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_open(ଶୂନ୍ୟ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    ଯଦି (gui.geom != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mask;
	unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା	w, h;

	mask = vim_parse_geometry((ଅକ୍ଷର *)gui.geom, &w, &h);

	ଯଦି (mask & Width_ମୂଲ୍ୟ)
	    Columns = w;
	ଯଦି (mask & Height_ମୂଲ୍ୟ)
	{
	    ଯଦି (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    ଅନ୍ୟଥା
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ଯଦି (Columns > 80)
	    Columns = 80;
	ଯଦି (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    ଯଦି (foreground_argument != ଖାଲି)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    ଯଦି (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ଯଦି (background_argument != ଖାଲି)
	bg_pixel = gui_get_color((char_u *)background_argument);
    ଯଦି (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ଯଦି (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    ଅନ୍ୟଥା
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ଖାଲି);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pixel_width - get_menu_tool_width(),
	    (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
gui_mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା rc UNUSED)
{
    ଯଦି (gui.mainwin != ଖାଲି)
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
		ଯଦି ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ଖାଲି)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
gui_mch_set_winpos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_maximized(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    ଶୂନ୍ୟ
gui_mch_unmaximize(ଶୂନ୍ୟ)
{
    ଯଦି (gui.mainwin != ଖାଲି)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    ଶୂନ୍ୟ
gui_mch_set_fullscreen(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ଯଦି (gui.mainwin == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    ଅନ୍ୟଥା
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    ସ୍ଥିର ଶୂନ୍ୟ
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ଯଦି (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    ଶୂନ୍ୟ
gui_mch_newfont(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    ଶୂନ୍ୟ
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    ଯଦି (title != ଖାଲି && gui.mainwin != ଖାଲି)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ଅଚଳ ଅକ୍ଷର *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    ଶୂନ୍ୟ
gui_gtk_init_decor_height(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା h = gtk_widget_get_height(gui.mainwin);

    ଯଦି (h == 0)
	ଫେରନ୍ତୁ;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_form_size(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ଯଦି (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	ଫେରନ୍ତୁ;

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

    ଶୂନ୍ୟ
gui_mch_set_shellsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_width UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା min_height UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_width UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା base_height UNUSED,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    ଶୂନ୍ୟ
gui_mch_get_screen_dimensions(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ଯଦି (surface != ଖାଲି)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ଯଦି (monitor != ଖାଲି)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    ଫେରନ୍ତୁ;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    ଶୂନ୍ୟ
gui_mch_enable_menu(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (gui.menubar != ଖାଲି)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ଯଦି (gui.drawarea != ଖାଲି)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    ଶୂନ୍ୟ
gui_mch_show_toolbar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (gui.toolbar != ଖାଲି)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	ଯଦି (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ଯଦି (gui.drawarea != ଖାଲି)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    ଶୂନ୍ୟ
gui_mch_set_dark_theme(ପୂର୍ଣ୍ଣସଂଖ୍ୟା dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ଯଦି (settings != ଖାଲି)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ଖାଲି);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_adjust_charheight(ଶୂନ୍ୟ)
{
    PangoFontMetrics *metrics;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ascent;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା descent;

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

    ଫେରନ୍ତୁ OK;
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    ସ୍ଥିର ଶୂନ୍ୟ
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, ଖାଲି);
    fdd->done = TRUE;
}

    ସ୍ଥିର gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    ଯଦି (PANGO_IS_FONT_FAMILY(item))
	ଫେରନ୍ତୁ pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    ଯଦି (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	ଯଦି (family != ଖାଲି)
	    ଫେରନ୍ତୁ pango_font_family_is_monospace(family);
    }
    ଫେରନ୍ତୁ TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ଖାଲି;
    char_u		*fontname = ଖାଲି;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ଖାଲି, ଖାଲି)));

    ଯଦି (oldval != ଖାଲି && oldval[0] != NUL)
    {
	char_u *oldname;

	ଯଦି (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ଖାଲି);
	ଅନ୍ୟଥା
	    oldname = oldval;

	ଯଦି (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ଯଦି (p != ଖାଲି)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ଯଦି (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ଅଚଳ ଅକ୍ଷର *)oldname);
	ଯଦି (oldname != oldval)
	    vim_free(oldname);
    }
    ଅନ୍ୟଥା
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = ଖାଲି;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, ଖାଲି, font_dialog_finish_cb, &fdd);

    ଯେତେବେଳେ (!fdd.done)
	g_main_context_iteration(ଖାଲି, TRUE);

    ଯଦି (fdd.result != ଖାଲି)
    {
	ଅକ୍ଷର *name = pango_font_description_to_string(fdd.result);
	ଯଦି (name != ଖାଲି)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ଯଦି (p != ଖାଲି && ନିବେଶ_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ନିବେଶ_conv, p, ଖାଲି);
		vim_free(p);
	    }
	    ଅନ୍ୟଥା
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    ଯଦି (initial != ଖାଲି)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    ଫେରନ୍ତୁ fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
ascii_glyph_table_init(ଶୂନ୍ୟ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;

    ଯଦି (gui.ascii_glyphs != ଖାଲି)
	pango_glyph_string_free(gui.ascii_glyphs);
    ଯଦି (gui.ascii_font != ଖାଲି)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ଖାଲି;
    gui.ascii_font   = ଖାଲି;

    ପାଇଁ (i = 0; i < 128; ++i)
    {
	ଯଦି (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ଅନ୍ୟଥା
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ଅଚଳ ଅକ୍ଷର *)ascii_chars,
			      0, ଆକାର(ascii_chars), attr_list, ଖାଲି);

    ଯଦି (item_list != ଖାଲି && item_list->next == ଖାଲି)
    {
	PangoItem   *item;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ଅଚଳ ଅକ୍ଷର *)ascii_chars, ଆକାର(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	ଯଦି (gui.ascii_glyphs->ଅଙ୍କ_glyphs == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(ascii_chars))
	{
	    ପାଇଁ (i = 0; i < gui.ascii_glyphs->ଅଙ୍କ_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	ଅନ୍ୟଥା
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ଖାଲି;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ଖାଲି;
	}
    }

    g_list_foreach(item_list, (GFunc)(ଶୂନ୍ୟ *)&pango_item_free, ଖାଲି);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    ସ୍ଥିର ଶୂନ୍ୟ
get_styled_font_variants(ଶୂନ୍ୟ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    ଯଦି (plain_font == ଖାଲି)
	ଫେରନ୍ତୁ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    ଯଦି (bold_font != ଖାଲି)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_init_font(char_u *font_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    width;

    ଯଦି (font_name == ଖାଲି)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ଯଦି (font_desc == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, ଖାଲି);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ଯଦି (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    ଫେରନ୍ତୁ OK;
}

    GuiFont
gui_mch_get_font(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା report_error)
{
    PangoFontDescription *font;

    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    font = pango_font_description_from_string((ଅଚଳ ଅକ୍ଷର *)name);
    ଯଦି (font == ଖାଲି)
    {
	ଯଦି (report_error)
	    semsg(_(e_unknown_font_str), name);
	ଫେରନ୍ତୁ ଖାଲି;
    }

    // Ensure a size is set
    ଯଦି (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    ଫେରନ୍ତୁ font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    ଯଦି (font != NOFONT)
    {
	ଅକ୍ଷର *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	ଫେରନ୍ତୁ ret;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ଶୂନ୍ୟ
gui_mch_free_font(GuiFont font)
{
    ଯଦି (font != NOFONT)
	pango_font_description_free(font);
}

    ଶୂନ୍ୟ
gui_mch_expand_font(
    optexpand_T	*args,
    ଶୂନ୍ୟ	*param,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			n_families = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			wide = *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)param;

    ଯଦି (args->oe_include_orig_val && *args->oe_opt_ମୂଲ୍ୟ == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ଯଦି (add_match((char_u *)DEFAULT_FONT) != OK)
	    ଫେରନ୍ତୁ;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_families; i++)
    {
	ଯଦି (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ଜାରି;

	ଅଚଳ ଅକ୍ଷର *fam_name = pango_font_family_get_name(font_families[i]);
	ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ନିବେଶ_conv,
		    (char_u *)fam_name, ଖାଲି);
	    ଯଦି (buf != ଖାଲି)
	    {
		ଯଦି (add_match(buf) != OK)
		{
		    vim_free(buf);
		    ଭାଙ୍ଗ;
		}
		vim_free(buf);
	    }
	    ଅନ୍ୟଥା
		ଭାଙ୍ଗ;
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (add_match((char_u *)fam_name) != OK)
		ଭାଙ୍ଗ;
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
    ଯଦି (!gui.in_use)
	ଫେରନ୍ତୁ INVALCOLOR;

    ଯଦି (name != ଖାଲି)
	ଫେରନ୍ତୁ gui_get_color_cmn(name);

    ଫେରନ୍ତୁ INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା g, ପୂର୍ଣ୍ଣସଂଖ୍ୟା b)
{
    ଫେରନ୍ତୁ gui_get_rgb_color_cmn(r, g, b);
}

    ସ୍ଥିର GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    ଫେରନ୍ତୁ rgba;
}

    ଶୂନ୍ୟ
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    ଶୂନ୍ୟ
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    ଶୂନ୍ୟ
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    ଫେରନ୍ତୁ pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    ଶୂନ୍ୟ
gui_gtk4_update_size(ଶୂନ୍ୟ)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.ଅଙ୍କ_rows, gui.ଅଙ୍କ_cols);
}

#ifdef FEAT_NETBEANS_INTG
    ଶୂନ୍ୟ
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		col,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		width,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    ଶୂନ୍ୟ
gui_mch_clear_block(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col1, ପୂର୍ଣ୍ଣସଂଖ୍ୟା row2, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ଯଦି (gui.drawarea != ଖାଲି)
	gtk_widget_queue_draw(gui.drawarea);
}

    ଶୂନ୍ୟ
gui_mch_clear_all(ଶୂନ୍ୟ)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ଯଦି (gui.drawarea != ଖାଲି)
	gtk_widget_queue_draw(gui.drawarea);
}

    ଶୂନ୍ୟ
gui_mch_delete_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, gui.scroll_region_bot,
	    gui.scroll_region_ବାମ, gui.scroll_region_ଡାହାଣ);
    gui_clear_block(
	    gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ + 1, gui.scroll_region_ବାମ,
	    gui.scroll_region_bot, gui.scroll_region_ଡାହାଣ);

    gtk_widget_queue_draw(gui.drawarea);
}

    ଶୂନ୍ୟ
gui_mch_ଭର୍ତ୍ତି_ଧାଡ଼ିଗୁଡ଼ିକ(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ, row, gui.scroll_region_bot - ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ,
	    gui.scroll_region_ବାମ, gui.scroll_region_ଡାହାଣ);
    gui_clear_block(
	    row, gui.scroll_region_ବାମ,
	    row + ଅଙ୍କ_ଧାଡ଼ିଗୁଡ଼ିକ - 1, gui.scroll_region_ଡାହାଣ);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    ଶୂନ୍ୟ
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    ଶୂନ୍ୟ
gui_mch_flash(ପୂର୍ଣ୍ଣସଂଖ୍ୟା msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 1);
    gui_mch_flush();
    ui_delay((long)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns - 1);
}

    ଶୂନ୍ୟ
gui_mch_invert_rectangle(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r, ପୂର୍ଣ୍ଣସଂଖ୍ୟା c, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr, ପୂର୍ଣ୍ଣସଂଖ୍ୟା nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    ସ୍ଥିର gboolean
ଚାବି_press_event(Gtk_Event_Controller_ଚାବି *controller UNUSED,
	guint ଚାବି_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଚାବି;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ଯଦି (xic != ଖାଲି)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ଯଦି (event != ଖାଲି && gtk_im_context_filter_keypress(xic, event))
	    ଫେରନ୍ତୁ TRUE;
    }
#endif

#ifdef FEAT_MENU
    ଯଦି (ଚାବି_sym == GDK_ଚାବି_F10 && gui.menubar != ଖାଲି)
    {
	ସ୍ଥିର char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ଯଦି (check_map(k10, State, FALSE, TRUE, FALSE, ଖାଲି, ଖାଲି) == ଖାଲି)
	{
	    show_menubar_popover();
	    ଫେରନ୍ତୁ TRUE;
	}
    }
#endif

    len = keyval_to_string(ଚାବି_sym, string2);

    ଯଦି (len > 1 && ନିବେଶ_conv.vc_type != CONV_NONE)
	len = convert_ନିବେଶ(string2, len, ଆକାର(string2));

    s = string2;
    d = string;
    ପାଇଁ (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ଯଦି (d[-1] == CSI && d + 2 < string + ଆକାର(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ଯଦି (ଚାବି_sym == GDK_ଚାବି_ISO_ବାମ_Tab)
    {
	ଚାବି_sym = GDK_ଚାବି_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ଯଦି ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	ଫେରନ୍ତୁ FALSE;
#endif

    // Check for special keys
    ଯଦି (len == 0 || len == 1)
    {
	ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != 0; i++)
	{
	    ଯଦି (special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym == ଚାବି_sym)
	    {
		string[0] = CSI;
		string[1] = special_ଚାବିଗୁଡ଼ିକ[i].code0;
		string[2] = special_ଚାବିଗୁଡ଼ିକ[i].code1;
		len = -3;
		ଭାଙ୍ଗ;
	    }
	}
    }

    ଯଦି (len == 0)
	ଫେରନ୍ତୁ TRUE;

    ଯଦି (len == -3)
	ଚାବି = TO_SPECIAL(string[1], string[2]);
    ଅନ୍ୟଥା
    {
	string[len] = NUL;
	ଚାବି = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    ଚାବି = simplify_ଚାବି(ଚାବି, &modifiers);
    ଯଦି (ଚାବି == CSI)
	ଚାବି = K_CSI;
    ଯଦି (IS_SPECIAL(ଚାବି))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ଚାବି);
	string[2] = K_THIRD(ଚାବି);
	len = 3;
    }
    ଅନ୍ୟଥା
    {
	ଚାବି = may_adjust_ଚାବି_for_ctrl(modifiers, ଚାବି);
	modifiers = may_remove_shift_modifier(modifiers, ଚାବି);
	len = mb_char2bytes(ଚାବି, string);
    }

    ଯଦି (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_ନିବେଶ_buf(string2, 3);
    }

    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା int_ch = check_for_interrupt(ଚାବି, modifiers);
	ଯଦି (int_ch != NUL)
	{
	    trash_ନିବେଶ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_ନିବେଶ_buf(string, len);

    ଯଦି (p_mh)
	gui_mch_mousehide(TRUE);

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
ଚାବି_release_event(Gtk_Event_Controller_ଚାବି *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_timed_out = TRUE;
ସ୍ଥିର guint mouse_click_timer = 0;

    ସ୍ଥିର timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)data = TRUE;
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
modifiers_gdk2mouse(guint state)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା modifiers = 0;

    ଯଦି (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    ଯଦି (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    ଯଦି (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    ଫେରନ୍ତୁ modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_pressed_button = -1;

ସ୍ଥିର guint motion_repeat_timer = 0;
ସ୍ଥିର gboolean motion_repeat_offset = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_press_x = -1, mouse_press_y = -1;

    ସ୍ଥିର ଶୂନ୍ୟ
button_press_event(GtkGestureClick *gesture, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_press UNUSED,
	ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ଯଦି (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    ବାଛ (btn)
    {
	ସ୍ଥିତି 1: button = MOUSE_ବାମ; ଭାଙ୍ଗ;
	ସ୍ଥିତି 2: button = MOUSE_ମଧ୍ୟ; ଭାଙ୍ଗ;
	ସ୍ଥିତି 3: button = MOUSE_ଡାହାଣ; ଭାଙ୍ଗ;
	ସ୍ଥିତି 8: button = MOUSE_X1; ଭାଙ୍ଗ;
	ସ୍ଥିତି 9: button = MOUSE_X2; ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ: ଫେରନ୍ତୁ;
    }

    mouse_press_x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x;
    mouse_press_y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y, repeated_click, vim_modifiers);
}

    ସ୍ଥିର ଶୂନ୍ୟ
button_release_event(GtkGestureClick *gesture, ପୂର୍ଣ୍ଣସଂଖ୍ୟା n_press UNUSED,
	ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ଯଦି (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y, FALSE, vim_modifiers);
}

ସ୍ଥିର ଦ୍ୱିଗୁଣ prev_mouse_x = -1.0;
ସ୍ଥିର ଦ୍ୱିଗୁଣ prev_mouse_y = -1.0;

ସ୍ଥିର GdkModifierType cur_state = 0;

    ସ୍ଥିର timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y;

    ଯଦି (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	ଫେରନ୍ତୁ G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ଯଦି (vim_used_in_ନିବେଶ_buf() > 10)
	ଫେରନ୍ତୁ G_SOURCE_CONTINUE;

    x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prev_mouse_x;
    y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ଯଦି (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    ଫେରନ୍ତୁ G_SOURCE_CONTINUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ଯଦି (mouse_pressed_button >= 0 && event != ଖାଲି
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	ଯଦି (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ଯଦି (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w, h;

	ଯଦି (event != ଖାଲି)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	ଯଦି (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ଯଦି (x < 0 || y < 0 || x >= w || y >= h)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dx, dy;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା offshoot;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ଯଦି (offshoot > 127)
		delay = 5;
	    ଅନ୍ୟଥା
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, ଖାଲି);
	}
	ଅନ୍ୟଥା
	    motion_repeat_timer = 0;
    }
    ଅନ୍ୟଥା
	gui_mouse_moved((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ଯଦି (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    ସ୍ଥିର ଶୂନ୍ୟ
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	ଦ୍ୱିଗୁଣ x, ଦ୍ୱିଗୁଣ y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    ସ୍ଥିର gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	ଦ୍ୱିଗୁଣ dx UNUSED, ଦ୍ୱିଗୁଣ dy, gpointer data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା button;
    int_u vim_modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ଯଦି (event == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    state = gdk_event_get_modifier_state(event);

    ଯଦି (dy < 0)
	button = MOUSE_4;	// scroll up
    ଅନ୍ୟଥା ଯଦି (dy > 0)
	button = MOUSE_5;	// scroll down
    ଅନ୍ୟଥା ଯଦି (dx < 0)
	button = MOUSE_7;	// scroll left
    ଅନ୍ୟଥା ଯଦି (dx > 0)
	button = MOUSE_6;	// scroll right
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    ଫେରନ୍ତୁ TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ଯଦି (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ଯଦି (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    ଯଦି (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ଯଦି (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ଯଦି defined(FEAT_IMAGE)
    ସ୍ଥିର ଶୂନ୍ୟ
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	ଶୂନ୍ୟ		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ଯଦି defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), ଖାଲି);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    ସ୍ଥିର ଶୂନ୍ୟ
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

ପ୍ରକାରବ୍ୟାଖ୍ୟା ଗଣନା
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    ଅକ୍ଷର	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ReadData	rd;
    GdkDrop	*drop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		y;
} DropReadData;
#endif

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ReadData	rd;
    Clipboard_T *cbd;
    ଅକ୍ଷର	*mime_type;
} ClipReadData;

    ସ୍ଥିର ReadData *
read_data_new(ReadDataType type, size_t sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ଯଦି (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    ଫେରନ୍ତୁ rd;
}

    ସ୍ଥିର ଶୂନ୍ୟ
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    ଯଦି (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    ଅନ୍ୟଥା ଯଦି (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    ଯଦି (rd->arr != ଖାଲି)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
ସ୍ଥିର ଶୂନ୍ୟ drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
read_data_ନିବେଶ_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_ନିବେଶ_stream_read_finish(stream, result, ଖାଲି);

    ଯଦି (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	ଯଦି (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    ଅନ୍ୟଥା ଯଦି (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ଯଦି (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    long	    len;
	    char_u	    *actual, *final;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    motion_type = MAUTO;
	    char_u	    *tofree = ଖାଲି;

	    len = (long)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ଖାଲି;

	    ଯଦି (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	ଅନ୍ୟଥା ଯଦି (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ଅଚଳ uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ଖାଲି;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    ଅନ୍ୟଥା
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ଅଚଳ uint8_t *)rd->buf, r);
	g_ନିବେଶ_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_ଉପର,
		rd->cancel, (GAsyncReadyCallback)read_data_ନିବେଶ_cb, rd);
	ଫେରନ୍ତୁ;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    ଶୂନ୍ୟ
gui_gtk_set_dnd_targets(ଶୂନ୍ୟ)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    ଯଦି (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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

    ଯଦି (text == ଖାଲି || *text == NUL || !query_pointer_pos(ଖାଲି, ଖାଲି, &state))
	ଫେରନ୍ତୁ;

    dnd_yank_drag_data((char_u *)text, (long)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    ଯଦି (dropkey[2] != 0)
	add_to_ନିବେଶ_buf(dropkey, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଆକାର(dropkey));
    ଅନ୍ୟଥା
	add_to_ନିବେଶ_buf(dropkey + 3, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(ଆକାର(dropkey) - 3));
}

    ସ୍ଥିର ଶୂନ୍ୟ
drop_read_ମୂଲ୍ୟ_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ଅଚଳ GValue    *ମୂଲ୍ୟ = gdk_drop_read_ମୂଲ୍ୟ_finish(drop, result, ଖାଲି);
    gboolean	    success = FALSE;

    ଯଦି (ମୂଲ୍ୟ == ଖାଲି)
	ଯାଅ exit;

    ଯଦି (G_ମୂଲ୍ୟ_HOLDS(ମୂଲ୍ୟ, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_ମୂଲ୍ୟ_get_boxed(ମୂଲ୍ୟ);
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	nfiles = g_slist_length(files);
	char_u	**fnames;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

	ଯଦି (nfiles <= 0)
	    ଯାଅ exit;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ଯଦି (fnames == ଖାଲି)
	    ଯାଅ exit;

	i = 0;
	ପାଇଁ (GSList *l = files; l != ଖାଲି; l = l->next)
	{
	    GFile *file = l->data;
	    ଅକ୍ଷର *path = g_file_get_path(file);
	    ଯଦି (path != ଖାଲି)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	ଯଦି (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	ଅନ୍ୟଥା
	    vim_free(fnames);
	success = TRUE;
    }
    ଅନ୍ୟଥା ଯଦି (G_ମୂଲ୍ୟ_HOLDS(ମୂଲ୍ୟ, G_TYPE_STRING))
    {
	ଅଚଳ ଅକ୍ଷର *text = g_ମୂଲ୍ୟ_get_string(ମୂଲ୍ୟ);

	ଯଦି (text == ଖାଲି || *text == NUL)
	    ଯାଅ exit;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

exit:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    ସ୍ଥିର ଶୂନ୍ୟ
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ଅଚଳ ଅକ୍ଷର	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, ଖାଲି);
    ଯଦି (in_stream == ଖାଲି || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	ଫେରନ୍ତୁ;
    }

    ପ୍ରତିଜ୍ଞା(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_ନିବେଶ_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ଉପର,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_ନିବେଶ_cb, drd);
}

// GCancellable of current DND operation, else NULL.
ସ୍ଥିର GCancellable *dnd_cancel = ଖାଲି;
ସ୍ଥିର guint dnd_timeout_id = 0;

    ସ୍ଥିର gboolean
drop_timeout_cb(ଶୂନ୍ୟ *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    ଫେରନ୍ତୁ G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    ସ୍ଥିର gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	ଦ୍ୱିଗୁଣ		    x,
	ଦ୍ୱିଗୁଣ		    y,
	ଶୂନ୍ୟ		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = ଖାଲି;

    ଯଦି (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, ଆକାର(*drd));
	gdk_drop_read_ମୂଲ୍ୟ_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_ଉପର,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ମୂଲ୍ୟ_cb, drd);
    }
    ଅନ୍ୟଥା ଯଦି (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *m[] = {"text/html", ଖାଲି};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, ଆକାର(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_ଉପର,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    ଅନ୍ୟଥା ଯଦି (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, ଆକାର(*drd));
	gdk_drop_read_ମୂଲ୍ୟ_async(drop, G_TYPE_STRING, G_PRIORITY_ଉପର,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_ମୂଲ୍ୟ_cb, drd);
    }

    ଯଦି (dnd_cancel != ଖାଲି)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ଯଦି (drd != ଖାଲି)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)x;
	drd->y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, ଖାଲି);

	ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = ଖାଲି;
    gui.drawarea = ଖାଲି;
    ଯଦି (!exiting)
	gui_shell_closed();
}

    ସ୍ଥିର gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    ଫେରନ୍ତୁ TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    ସ୍ଥିର timeout_cb_type
ନିବେଶ_timer_cb(gpointer data)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା *timed_out = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା *)data;

    *timed_out = TRUE;
    ଫେରନ୍ତୁ FALSE;	// don't call me again
}

    ଶୂନ୍ୟ
gui_mch_update(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cnt = 0;

    ଯେତେବେଳେ (g_main_context_pending(ଖାଲି) && !vim_is_ନିବେଶ_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(ଖାଲି, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    ସ୍ଥିର timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    ଫେରନ୍ତୁ TRUE; // Keep repeating
}
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_wait_for_chars(long wtime)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		focus;
    guint	timer;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	timed_out;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    ଯଦି (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   ନିବେଶ_timer_cb, &timed_out);
    ଅନ୍ୟଥା
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ଯଦି (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, ଖାଲି);
#endif

    focus = gui.in_focus;

    କର
    {
	// Stop or start blinking when focus changes
	ଯଦି (gui.in_focus != focus)
	{
	    ଯଦି (gui.in_focus)
		gui_mch_start_blink();
	    ଅନ୍ୟଥା
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ଯଦି (did_add_timer)
	    ଯାଅ theend;
# endif
#endif

	ଯଦି (gui.mainwin == ଖାଲି)
	    ଯାଅ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ଯଦି (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    ଯାଅ theend;
	}

	ଯଦି (!ନିବେଶ_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ଖାଲି, TRUE);
	    --gtk4_main_loop_level;
	}

	ଯଦି (ନିବେଶ_available())
	{
	    retval = OK;
	    ଯାଅ theend;
	}
    } ଯେତେବେଳେ (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    ଯଦି (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    ଯଦି (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    ଫେରନ୍ତୁ retval;
}

    ଶୂନ୍ୟ
gui_mch_flush(ଶୂନ୍ୟ)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ଯଦି (gui.drawarea != ଖାଲି)
	gtk_widget_queue_draw(gui.drawarea);
    ଯଦି (gui.mainwin != ଖାଲି && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    ଶୂନ୍ୟ
gui_mch_beep(ଶୂନ୍ୟ)
{
    GdkDisplay *display;

    ଯଦି (gui.mainwin != ଖାଲି && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ଯଦି (display != ଖାଲି)
	    gdk_display_beep(display);
    }
}

    ଶୂନ୍ୟ *
gui_mch_get_display(ଶୂନ୍ୟ)
{
    ଯଦି (gui.mainwin != ଖାଲି && gtk_widget_get_display(gui.mainwin))
	ଫେରନ୍ତୁ gtk_widget_get_display(gui.mainwin);
    ଫେରନ୍ତୁ ଖାଲି;
}

    ଶୂନ୍ୟ
gui_mch_iconify(ଶୂନ୍ୟ)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    ଶୂନ୍ୟ
gui_mch_set_foreground(ଶୂନ୍ୟ)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
query_pointer_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    ଦ୍ୱିଗୁଣ	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    ଯଦି (gui.drawarea == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ଯଦି (native == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    surface = gtk_native_get_surface(native);
    ଯଦି (surface == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ଯଦି (display == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    seat = gdk_display_get_default_seat(display);
    ଯଦି (seat == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    pointer = gdk_seat_get_pointer(seat);
    ଯଦି (pointer == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (x != ଖାଲି && y != ଖାଲି)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (ଦଶମିକ)(sx - nx);
	src.y = (ଦଶମିକ)(sy - ny);
	ଯଦି (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    ଫେରନ୍ତୁ FALSE;

	*x = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dst.x;
	*y = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dst.y;
    }
    ଫେରନ୍ତୁ TRUE;
}

    ଶୂନ୍ୟ
gui_mch_getmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *y)
{
    ଯଦି (!query_pointer_pos(x, y, ଖାଲି))
    {
	*x = 0;
	*y = 0;
    }
}

    ଶୂନ୍ୟ
gui_mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    ଶୂନ୍ୟ
gui_mch_mousehide(ପୂର୍ଣ୍ଣସଂଖ୍ୟା hide)
{
    ଯଦି (gui.pointer_hidden == hide)
	ଫେରନ୍ତୁ;

    gui.pointer_hidden = hide;
    ଯଦି (gui.blank_pointer != ଖାଲି)
    {
	ଯଦି (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	ଅନ୍ୟଥା
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#ଅନ୍ୟଥା
	    gtk_widget_set_cursor(gui.drawarea, ଖାଲି);
#endif
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_haskey(char_u *name)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

    ପାଇଁ (i = 0; special_ଚାବିଗୁଡ଼ିକ[i].ଚାବି_sym != 0; i++)
	ଯଦି (name[0] == special_ଚାବିଗୁଡ଼ିକ[i].code0
		&& name[1] == special_ଚାବିଗୁଡ଼ିକ[i].code1)
	    ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
gui_mch_forked(ଶୂନ୍ୟ)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_gtk4_update_scrollbar_size(ଶୂନ୍ୟ)
{
    win_T	*wp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ବାମ].id;

	ଯଦି (sbar == ଖାଲି || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ବାମ]
		    && wp->w_scrollbars[SBAR_ଡାହାଣ].id != ଖାଲି))
	    sbar = wp->w_scrollbars[SBAR_ଡାହାଣ].id;

	ଯଦି (sbar != ଖାଲି && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    ଯଦି (sw > 0)
	    {
		w = sw;
		ଭାଙ୍ଗ;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ଯଦି (sbar != ଖାଲି && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	ଯଦି (sh > 0)
	    h = sh;
    }

    ଯଦି (w != -1)
	gui.scrollbar_width = w;
    ଯଦି (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    ଶୂନ୍ୟ
gui_mch_enable_scrollbar(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flag)
{
    ଯଦି (sb->id != ଖାଲି)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	ଯଦି (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#ଯଦି defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    ଶୂନ୍ୟ
gui_mch_menu_grey(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା grey)
{
    ଯଦି (menu->id == ଖାଲି)
	ଫେରନ୍ତୁ;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    ଶୂନ୍ୟ
gui_mch_menu_hidden(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା hidden)
{
    ଯଦି (menu->id == ଖାଲି)
	ଫେରନ୍ତୁ;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    ଶୂନ୍ୟ
gui_mch_draw_menubar(ଶୂନ୍ୟ)
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
    ଶୂନ୍ୟ
gui_mch_show_tabline(ପୂର୍ଣ୍ଣସଂଖ୍ୟା showit)
{
    ଯଦି (gui.tabline == ଖାଲି)
	ଫେରନ୍ତୁ;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_showing_tabline(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ gui.tabline != ଖାଲି && gtk_widget_get_visible(gui.tabline);
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା tabpage_hover = 0;

    ସ୍ଥିର ଶୂନ୍ୟ
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	ଦ୍ୱିଗୁଣ		    x UNUSED,
	ଦ୍ୱିଗୁଣ		    y UNUSED,
	ଶୂନ୍ୟ		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    ସ୍ଥିର ଶୂନ୍ୟ
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	ଦ୍ୱିଗୁଣ		    x UNUSED,
	ଦ୍ୱିଗୁଣ		    y UNUSED,
	ଶୂନ୍ୟ		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tabline_add_css(ଶୂନ୍ୟ)
{
    GtkCssProvider  *provider;
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *css =
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
# ଯଦି GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx = gtk_notebook_page_ଅଙ୍କ(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    ଯଦି (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    ଶୂନ୍ୟ
gui_mch_update_tabline(ଶୂନ୍ୟ)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nr = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tab_ଅଙ୍କ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		curtabidx = 0;
    char_u	*labeltext;

    ଯଦି (gui.tabline == ଖାଲି)
	ଫେରନ୍ତୁ;

    ignore_tabline_evt = TRUE;

    ପାଇଁ (tp = first_tabpage; tp != ଖାଲି; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ଯଦି (tp == curtab)
	    curtabidx = nr;

	tab_ଅଙ୍କ = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	ଯଦି (page == ଖାଲି)
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

	    gtk_notebook_ଭର୍ତ୍ତି_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, ଖାଲି);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_ଅଙ୍କ));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, ଖାଲି);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	ଯଦି (label != ଖାଲି && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ଅଚଳ ଅକ୍ଷର *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ଅଚଳ gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    ଯେତେବେଳେ (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != ଖାଲି)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ଯଦି (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    ଅନ୍ୟଥା
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ଯଦି (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    ଶୂନ୍ୟ
gui_mch_set_curtab(ପୂର୍ଣ୍ଣସଂଖ୍ୟା nr)
{
    ଯଦି (gui.tabline != ଖାଲି)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ଯଦି (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    ଯଦି (ignore_tabline_evt)
	ଫେରନ୍ତୁ;

    ଯଦି ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ଅନ୍ୟଥା
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, ଶୂନ୍ୟ *udata)
{
    ଯଦି (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    ସ୍ଥିର ଶୂନ୍ୟ
add_tabline_menu_item(VimMenu *menu, ଅଚଳ ଅକ୍ଷର *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_ଭର୍ତ୍ତି_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    ସ୍ଥିର VimMenu *
create_tabline_popup_menu(ଶୂନ୍ୟ)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    ଫେରନ୍ତୁ menu;
}

    ସ୍ଥିର ଶୂନ୍ୟ
tabline_menu_press_event(
	GtkGestureClick *gesture,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		n_press UNUSED,
	ଦ୍ୱିଗୁଣ		x,
	ଦ୍ୱିଗୁଣ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ଯଦି (but == GDK_BUTTON_SECONDARY)
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
    ଅନ୍ୟଥା ଯଦି (but == GDK_BUTTON_PRIMARY)
    {
	ଯଦି (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    ଅନ୍ୟଥା ଯଦି (but == GDK_BUTTON_ମଧ୍ୟ)
    {
	ଯଦି (tabpage_hover != 0)
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

#ଯଦି defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    ଶୂନ୍ୟ
gui_mch_drawsign(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, ପୂର୍ଣ୍ଣସଂଖ୍ୟା typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ଯଦି (sign == ଖାଲି)
	ଫେରନ୍ତୁ;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    ଶୂନ୍ୟ *
gui_mch_register_sign(char_u *signfile)
{
    ଯଦି (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = ଖାଲି;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ଅଚଳ ଅକ୍ଷର *)signfile,
							    &error);
	ଯଦି (sign != ଖାଲି)
	    ଫେରନ୍ତୁ sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

    ଶୂନ୍ୟ
gui_mch_destroy_sign(ଶୂନ୍ୟ *sign)
{
    ଯଦି (sign != ଖାଲି)
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_gtk_draw_string(ପୂର୍ଣ୍ଣସଂଖ୍ୟା row, ପୂର୍ଣ୍ଣସଂଖ୍ୟା col, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    ଯଦି (gui.drawarea != ଖାଲି)
	gtk_widget_queue_draw(gui.drawarea);

    ଫେରନ୍ତୁ cells;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    ଫେରନ୍ତୁ FAIL;
}

#ଯଦି defined(FEAT_MENU)
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
    ସ୍ଥିର char_u *
translate_mnemonic_tag(char_u *name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ପାଇଁ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ଯଦି (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ଯଦି (buf != ଖାଲି)
    {
	pdest = buf;
	ପାଇଁ (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ଯଦି (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    ଅନ୍ୟଥା ଯଦି (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    ଅନ୍ୟଥା ଯଦି (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    ଅନ୍ୟଥା ଯଦି (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    ଫେରନ୍ତୁ buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    ଶୂନ୍ୟ
gui_gtk_set_mnemonics(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ଯଦି (menu->id == ଖାଲି)
	    ଜାରି;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ଯଦି (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ଅଚଳ ଅକ୍ଷର *)name);
	vim_free(name);
    }
}

    ସ୍ଥିର ଶୂନ୍ୟ
popupmenu_closed_cb(GtkWidget *popover, ଶୂନ୍ୟ *udata UNUSED)
{
    gtk_widget_unparent(popover);
    ଯଦି (gui.drawarea != ଖାଲି)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gui_gtk_popup_at(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ଯଦି (menu == ଖାଲି || menu->submenu_id == ଖାଲି)
	ଫେରନ୍ତୁ;

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
	    G_CALLBACK(popupmenu_closed_cb), ଖାଲି);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    ଶୂନ୍ୟ
gui_make_popup(char_u *path_name, ପୂର୍ଣ୍ଣସଂଖ୍ୟା mouse_pos)
{
    vimmenu_T	*menu;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y;

    menu = gui_find_menu(path_name);
    ଯଦି (menu == ଖାଲି || menu->submenu_id == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (mouse_pos)
	gui_mch_getmouse(&x, &y);
    ଅନ୍ୟଥା
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_tool_width(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_menu_tool_height(ଶୂନ୍ୟ)
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା height = 0;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		h;

	ଯଦି (widgets[i] == ଖାଲି || !gtk_widget_get_visible(widgets[i]))
	    ଜାରି;

	h = gtk_widget_get_height(widgets[i]);

	ଯଦି (h == 0)
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
	ଅନ୍ୟଥା
	    height += h;
    }
    ଫେରନ୍ତୁ height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    ଶୂନ୍ୟ
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ଯଦି (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    ଅନ୍ୟଥା
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    ସ୍ଥିର GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ଯଦି (gui.mainwin == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    display = gtk_widget_get_display(gui.mainwin);
    ଯଦି (display == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;

    ଯଦି (cbd == &clip_plus)
    {
	ଯଦି (provider != ଖାଲି)
	    *provider = gui.regular_provider;
	ଫେରନ୍ତୁ gdk_display_get_clipboard(display);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (provider != ଖାଲି)
	    *provider = gui.primary_provider;
	ଫେରନ୍ତୁ gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ଅଚଳ ଅକ୍ଷର	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, ଖାଲି);
    ଯଦି (in_stream == ଖାଲି)
    {
	read_data_free(&crd->rd);
	ଫେରନ୍ତୁ;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_ନିବେଶ_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ଉପର,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_ନିବେଶ_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    ଶୂନ୍ୟ
clip_mch_request_selection(Clipboard_T *cbd)
{
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ଖାଲି
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ଖାଲି);
    ଯଦି (clipboard == ଖାଲି)
	ଫେରନ୍ତୁ;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, ଆକାର(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_ଉପର, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = time(ଖାଲି);
	ଯେତେବେଳେ (!g_cancellable_is_cancelled(cancel)
		&& time(ଖାଲି) < start + 3)
	    g_main_context_iteration(ଖାଲି, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    ଶୂନ୍ୟ
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    ସ୍ଥିର ଶୂନ୍ୟ
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    ଯଦି (in_clipboard_set)
	ଫେରନ୍ତୁ;
    ଯଦି (gdk_clipboard_is_local(clipboard))
	ଫେରନ୍ତୁ;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret;

    ଯଦି (cb == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    ଫେରନ୍ତୁ ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    ଶୂନ୍ୟ
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ଖାଲି);
    ଯଦି (clipboard == ଖାଲି)
	ଫେରନ୍ତୁ;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ଯଦି (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, ଖାଲି);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ gtk4_main_loop_level;
}

    ଶୂନ୍ୟ
gtk_main_quit(ଶୂନ୍ୟ)
{
    gtk4_main_loop_quit = TRUE;
}

#ଯଦି defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *mshape_css_names[] =
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

    ଶୂନ୍ୟ
mch_set_mouse_shape(ପୂର୍ଣ୍ଣସଂଖ୍ୟା shape)
{
    GdkCursor	*c;
    ଅଚଳ ଅକ୍ଷର	*css_name = "default";

    ଯଦି (gui.drawarea == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    ଅନ୍ୟଥା
    {
	ଯଦି (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	ଅନ୍ୟଥା ଯଦି (shape < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	ଅନ୍ୟଥା
	    ଫେରନ୍ତୁ;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ଖାଲି);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ଯଦି (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_text_area_w = 0;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା last_text_area_h = 0;

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
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ toolbar_icon_names[] =
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

    ସ୍ଥିର ଶୂନ୍ୟ
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    ସ୍ଥିର GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ଖାଲି;

    // Try specified icon file first
    ଯଦି (menu->iconfile != ଖାଲି)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ଯଦି (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ଅଚଳ ଅକ୍ଷର *)buf, ଖାଲି);

	    ଯଦି (texture != ଖାଲି)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ଯଦି (image == ଖାଲି)
    {
	ଅଚଳ ଅକ୍ଷର *icon_name = "image-missing";
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(toolbar_icon_names);

	ଯଦି (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    ଫେରନ୍ତୁ image;
}
# endif

    ସ୍ଥିର ଶୂନ୍ୟ
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    ଯଦି (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    ଅନ୍ୟଥା ଯଦି (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	ସ୍ଥିର gboolean did_ସନ୍ଦେଶ = FALSE;

	ଯଦି (State & MODE_CMDLINE)
	    ଫେରନ୍ତୁ;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	ଯଦି (tooltip != ଖାଲି && utf_valid_string(tooltip, ଖାଲି))
	{
	    ସନ୍ଦେଶ((ଅକ୍ଷର *)tooltip);
	    did_ସନ୍ଦେଶ = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	ଅନ୍ୟଥା ଯଦି (did_ସନ୍ଦେଶ)
	{
	    ସନ୍ଦେଶ("");
	    did_ସନ୍ଦେଶ = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    ଶୂନ୍ୟ
gui_mch_add_menu(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    ଯଦି (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	ଫେରନ୍ତୁ;
    }

    parent = menu->parent;

    ଯଦି ((parent != ଖାଲି && parent->submenu_id == ଖାଲି)
	    || !menu_is_menubar(menu->name))
	ଫେରନ୍ତୁ;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != ଖାଲି || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    ଯଦି (parent != ଖାଲି)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ଅଚଳ ଅକ୍ଷର *)text, ଖାଲି, ଖାଲି));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_ଭର୍ତ୍ତି_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    ଅନ୍ୟଥା
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ଅଚଳ ଅକ୍ଷର *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_ଭର୍ତ୍ତି_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    ଶୂନ୍ୟ
gui_mch_add_menu_item(vimmenu_T *menu, ପୂର୍ଣ୍ଣସଂଖ୍ୟା idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    ଯଦି (parent != ଖାଲି && menu_is_toolbar(parent->name))
    {
	ଯଦି (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_ଭର୍ତ୍ତି_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	ଅନ୍ୟଥା
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    ଯଦି (tooltip != ଖାଲି && !utf_valid_string(tooltip, ଖାଲି))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_ଭର୍ତ୍ତି_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ଅଚଳ ଅକ୍ଷର *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ଅଚଳ gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	ଫେରନ୍ତୁ;
    }
# endif

    // Menu items (non-toolbar)
    ଯଦି (parent == ଖାଲି || parent->submenu_id == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_ଭର୍ତ୍ତି_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    ଅନ୍ୟଥା
    {
	char_u	    *text;
	char_u	    *accel_text = ଖାଲି;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	ଯଦି (menu->actext != ଖାଲି && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ଅଚଳ ଅକ୍ଷର *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	ଯଦି (accel_text != ଖାଲି)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ଅଚଳ ଅକ୍ଷର *)accel_text);

	vim_menu_ଭର୍ତ୍ତି_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    ଶୂନ୍ୟ
gui_mch_toggle_tearoffs(ପୂର୍ଣ୍ଣସଂଖ୍ୟା enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    ଶୂନ୍ୟ
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ଯଦି (menu->id == ଖାଲି)
	ଫେରନ୍ତୁ;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    ଯଦି (tooltip != ଖାଲି && utf_valid_string(tooltip, ଖାଲି))
	gtk_widget_set_tooltip_text(menu->id, (ଅଚଳ ଅକ୍ଷର *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    ଶୂନ୍ୟ
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ଯଦି (menu->parent != ଖାଲି && menu_is_toolbar(menu->parent->name))
    {
	ଯଦି (menu->id != ଖାଲି)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ଖାଲି;
	}
	ଫେରନ୍ତୁ;
    }

    // For popup menus, unparent the menu as well
    ଯଦି (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ଯଦି (menu->submenu_id != ଖାଲି)
	    gtk_widget_unparent(menu->submenu_id);
    }
    ଅନ୍ୟଥା ଯଦି (menu->parent == ଖାଲି && menu->id != ଖାଲି)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    ଅନ୍ୟଥା ଯଦି (menu->id != ଖାଲି)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    ଶୂନ୍ୟ
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    ସ୍ଥିର ଶୂନ୍ୟ
show_menubar_popover(ଶୂନ୍ୟ)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ଯଦି (gui.menubar == ଖାଲି || gui.drawarea == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ଖାଲି);
	ଫେରନ୍ତୁ;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ଖାଲି);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, long val, long size, long max)
{
    GtkAdjustment *adj;

    ଯଦି (sb->id == ଖାଲି)
	ଫେରନ୍ତୁ;
    ଯଦି (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	ଫେରନ୍ତୁ;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_ମୂଲ୍ୟ(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    ଶୂନ୍ୟ
gui_mch_set_scrollbar_pos(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    ଯଦି (sb->id != ଖାଲି)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_xpadding(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା formwin_w = gtk_widget_get_width(gui.formwin);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା sbar_w = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା xpad;

    ଯଦି (gui.which_scrollbars[SBAR_ବାମ])
	sbar_w += gui.scrollbar_width;
    ଯଦି (gui.which_scrollbars[SBAR_ଡାହାଣ])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    ଫେରନ୍ତୁ (xpad < 0) ? 0 : xpad;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_get_scrollbar_ypadding(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା formwin_h = gtk_widget_get_height(gui.formwin);
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ypad;

    ypad = formwin_h - last_text_area_h;
    ଯଦି (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    ଫେରନ୍ତୁ (ypad < 0) ? 0 : ypad;
}

    ସ୍ଥିର ଶୂନ୍ୟ
adjustment_ମୂଲ୍ୟ_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    long ମୂଲ୍ୟ = (long)gtk_adjustment_get_ମୂଲ୍ୟ(adj);

    ଯଦି (sb != ଖାଲି)
	gui_drag_scrollbar(sb, ମୂଲ୍ୟ, FALSE);
}

    ଶୂନ୍ୟ
gui_mch_create_scrollbar(scrollbar_T *sb, ପୂର୍ଣ୍ଣସଂଖ୍ୟା orient)
{
    GtkAdjustment *adj;
    ଯଦି (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ଖାଲି);
    ଅନ୍ୟଥା
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ଖାଲି);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_ମୂଲ୍ୟ_changed), ଖାଲି);
}

    ଶୂନ୍ୟ
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ଯଦି (sb->id != ଖାଲି)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ଖାଲି;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    ଶୂନ୍ୟ
gui_mch_set_text_area_pos(ପୂର୍ଣ୍ଣସଂଖ୍ୟା x, ପୂର୍ଣ୍ଣସଂଖ୍ୟା y, ପୂର୍ଣ୍ଣସଂଖ୍ୟା w, ପୂର୍ଣ୍ଣସଂଖ୍ୟା h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ଯଦି (w <= 0 || h <= 0)
	ଫେରନ୍ତୁ;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    ଶୂନ୍ୟ
gui_gtk4_calculate_bleed(ପୂର୍ଣ୍ଣସଂଖ୍ୟା width, ପୂର୍ଣ୍ଣସଂଖ୍ୟା height)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_ଡାହାଣ = gui.bleed_ଡାହାଣ;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_bot = gui.bleed_bot;

    gui.bleed_ଡାହାଣ = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    ଯଦି (gui.which_scrollbars[SBAR_ବାମ])
	gui.bleed_ଡାହାଣ -= gui.scrollbar_width;
    ଯଦି (gui.which_scrollbars[SBAR_ଡାହାଣ])
	gui.bleed_ଡାହାଣ -= gui.scrollbar_width;
    ଯଦି (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    ଯଦି (gui.bleed_ଡାହାଣ < 0)
	gui.bleed_ଡାହାଣ = 0;
    ଯଦି (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ଯଦି (old_ଡାହାଣ != gui.bleed_ଡାହାଣ || old_bot != gui.bleed_bot)
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
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    ସ୍ଥିର ଶୂନ୍ୟ
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, ଖାଲି);
    fdd->done = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, ଖାଲି);
    fdd->done = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, ଖାଲି);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = ଖାଲି;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    ଯଦି (initdir == ଖାଲି || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    ଅନ୍ୟଥା ଯଦି (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ଯଦି (title != ଖାଲି)
	gtk_file_dialog_set_title(dlg, (ଅଚଳ ଅକ୍ଷର *)title);

    {
	GFile *dir = g_file_new_for_path((ଅଚଳ ଅକ୍ଷର *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ଯଦି (filter != ଖାଲି && *filter != NUL)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ଯଦି (patt != ଖାଲି)
	{
	    ଯେତେବେଳେ (p != ଖାଲି && *p != NUL)
	    {
		ଯଦି (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ଯଦି (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    ଅନ୍ୟଥା
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			ଯଦି (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ଯଦି (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		ଅନ୍ୟଥା
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

    ଯଦି (saving && dflt != ଖାଲି && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ଅଚଳ ଅକ୍ଷର *)dflt);

    fdd.result = ଖାଲି;
    fdd.done = FALSE;

    ଯଦି (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), ଖାଲି,
		file_dialog_save_cb, &fdd);
    ଅନ୍ୟଥା
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), ଖାଲି,
		file_dialog_open_cb, &fdd);

    ଯେତେବେଳେ (!fdd.done)
	g_main_context_iteration(ଖାଲି, TRUE);

    ଯଦି (fdd.result != ଖାଲି)
    {
	ଅକ୍ଷର *path = g_file_get_path(fdd.result);
	ଯଦି (path != ଖାଲି)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ଫେରନ୍ତୁ result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = ଖାଲି;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    ଯଦି (title != ଖାଲି)
	gtk_file_dialog_set_title(dlg, (ଅଚଳ ଅକ୍ଷର *)title);

    ଯଦି (initdir != ଖାଲି && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ଅଚଳ ଅକ୍ଷର *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = ଖାଲି;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), ଖାଲି,
	    file_dialog_folder_cb, &fdd);

    ଯେତେବେଳେ (!fdd.done)
	g_main_context_iteration(ଖାଲି, TRUE);

    ଯଦି (fdd.result != ଖାଲି)
    {
	ଅକ୍ଷର *path = g_file_get_path(fdd.result);
	ଯଦି (path != ଖାଲି)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    ଫେରନ୍ତୁ result;
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
    ସ୍ଥିର ଅକ୍ଷର **
split_button_string(char_u *button_string, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *n_buttons, ଅକ୍ଷର **tofree)
{
    ଅକ୍ଷର	    **array;
    char_u	    *p;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    ଗଣନା_2 = 1;

    button_string = (char_u *)g_strdup((ଅଚଳ ଅକ୍ଷର *)button_string);

    ପାଇଁ (p = button_string; *p != NUL; ++p)
	ଯଦି (*p == DLG_BUTTON_SEP)
	    ++ଗଣନା_2;

    array = g_malloc_n(ଗଣନା_2, ଆକାର(ଅକ୍ଷର *));
    ଗଣନା_2 = 0;

    ଯଦି (array != ଖାଲି)
    {
	array[ଗଣନା_2++] = (ଅକ୍ଷର *)button_string;
	ପାଇଁ (p = button_string; *p != NUL; )
	{
	    ଯଦି (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[ଗଣନା_2++] = (ଅକ୍ଷର *)p;
	    }
	    ଅନ୍ୟଥା ଯଦି (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    ଅନ୍ୟଥା
		MB_PTR_ADV(p);
	}
    }

    *tofree = (ଅକ୍ଷର *)button_string;
    *n_buttons = ଗଣନା_2;
    ଫେରନ୍ତୁ array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର *
dialog_type_to_icon(ପୂର୍ଣ୍ଣସଂଖ୍ୟା type)
{
    ବାଛ (type)
    {
	ସ୍ଥିତି VIM_ERROR:
	    ଫେରନ୍ତୁ "dialog-error-symbolic";
	ସ୍ଥିତି VIM_WARNING:
	    ଫେରନ୍ତୁ "dialog-warning-symbolic";
	ସ୍ଥିତି VIM_INFO:
	    ଫେରନ୍ତୁ "dialog-information-symbolic";
	ସ୍ଥିତି VIM_QUESTION:
	    ଫେରନ୍ତୁ "dialog-question-symbolic";
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

// Data associated with each button in the dialog
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		but_idx;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*response;
    gboolean	*done;
} DialogButtonState;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    ସ୍ଥିର ଶୂନ୍ୟ
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    ସ୍ଥିର gboolean
dialog_ଚାବି_pressed_cb(
	Gtk_Event_Controller_ଚାବି	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ଯଦି (keyval == GDK_ଚାବି_Escape)
    {
	*dstate->done = TRUE;
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	ଫେରନ୍ତୁ gtk_widget_mnemonic_activate(dstate->win, FALSE);
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gui_mch_dialog(
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	def_but,
	char_u	*textfield,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ଅଚଳ ଅକ୍ଷର		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = ଖାଲି;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *ଚାବି_controller;
    DialogButtonState	*but_states = ଖାଲି;
    ଅକ୍ଷର		*tofree = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    ଯଦି (utf8_title != ଖାଲି)
	gtk_window_set_title(win, (ଅଚଳ ଅକ୍ଷର *)utf8_title);
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
    gtk_widget_set_margin_ଶୀର୍ଷ(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    ଯଦି (icon_name != ଖାଲି)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ଅଚଳ ଅକ୍ଷର *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    ଚାବି_controller = gtk_event_controller_ଚାବି_new();
    g_signal_connect(ଚାବି_controller, "key-pressed",
	    G_CALLBACK(dialog_ଚାବି_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), ଚାବି_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    ଯଦି (textfield != ଖାଲି)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	ଯଦି (utf8_text != ଖାଲି)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ଅଚଳ ଅକ୍ଷର *)utf8_text);
	ଅନ୍ୟଥା
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    ଅନ୍ୟଥା
    {
	GListModel  *controllers;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_ବସ୍ତୁଗୁଡ଼ିକ(controllers);
	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    ଯଦି (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ଯଦି (buttons != ଖାଲି)
    {
	GtkWidget   *but_box;
	ଅକ୍ଷର	    **buttons_arr; // Note that array is allocated, not strings
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ଯଦି (vim_strchr(p_go, GO_VERTICAL) != ଖାଲି)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	ଅନ୍ୟଥା
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, ଆକାର(DialogButtonState));

	ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    ଯଦି (but_label == ଖାଲି)
		ଜାରି;

	    but = gtk_button_new_with_mnemonic((ଅକ୍ଷର *)but_label);
	    ଯଦି (i == def_but - 1)
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

    ଯେତେବେଳେ (!done && !win_closed)
	g_main_context_iteration(ଖାଲି, TRUE);

    ଯଦି (done)
    {
	ଯଦି (textfield != ଖାଲି)
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
    ଫେରନ୍ତୁ response == -1 ? 0 : response + 1;
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

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
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

ସ୍ଥିର SharedFindReplace find_widgets = {0};
ସ୍ଥିର SharedFindReplace repl_widgets = {0};

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    ଯଦି (flags == FRD_FINDNEXT)
    {
	repl_text = ଖାଲି;
	sfr = &find_widgets;
    }
    ଅନ୍ୟଥା
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    ଯଦି (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    ଯଦି (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    ସ୍ଥିର ଶୂନ୍ୟ
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = ଖାଲି;
}

    ସ୍ଥିର ଶୂନ୍ୟ
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ଅଚଳ gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    ଯଦି (!entry_text)
	ଫେରନ୍ତୁ; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    ଯଦି (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    ଯଦି (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    ସ୍ଥିର gboolean
find_ଚାବି_pressed_cb(
	Gtk_Event_Controller_ଚାବି	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ଯଦି (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	ଫେରନ୍ତୁ FALSE;

    // the Escape key synthesizes a cancellation action
    ଯଦି (keyval == GDK_ଚାବି_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	ଫେରନ୍ତୁ TRUE;
    }

    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
entry_activate_cb(GtkWidget *widget UNUSED, ଶୂନ୍ୟ *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    ସ୍ଥିର ଶୂନ୍ୟ
find_replace_dialog_create(char_u *arg, ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			wword = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*ଚାବି_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    ଯଦି (entry_text != ଖାଲି && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, ଖାଲି);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ଯଦି (frdp->dialog != ଖାଲି)
    {
	ଯଦି (entry_text != ଖାଲି)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (ଅକ୍ଷର *)entry_text);
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
	ଯଦି (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	ଫେରନ୍ତୁ;
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
    gtk_widget_set_margin_ଶୀର୍ଷ(vertbox, 12);
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
    sensitive = (entry_text != ଖାଲି && entry_text[0] != NUL);
    ଯଦି (entry_text != ଖାଲି)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (ଅକ୍ଷର *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ଯଦି (do_replace)
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
    ଅନ୍ୟଥା
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

    ଚାବି_controller = gtk_event_controller_ଚାବି_new();
    g_signal_connect(ଚାବି_controller, "key-pressed",
	    G_CALLBACK(find_ଚାବି_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), ଚାବି_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ଯଦି (do_replace)
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
    ଯଦି (do_replace && entry_text != ଖାଲି && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    ଶୂନ୍ୟ
gui_mch_find_dialog(exarg_T *eap)
{
    ଯଦି (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    ଶୂନ୍ୟ
gui_mch_replace_dialog(exarg_T *eap)
{
    ଯଦି (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    ଶୂନ୍ୟ
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
ସ୍ଥିର GtkPrintDialog *print_dialog = ଖାଲି;
ସ୍ଥିର GtkPrintSettings *print_settings = ଖାଲି;
ସ୍ଥିର GtkPageSetup *page_setup = ଖାଲି;
ସ୍ଥିର GtkPrintSetup *print_setup = ଖାଲି;
ସ୍ଥିର GFile *print_file;

    ସ୍ଥିର ଶୂନ୍ୟ
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = ଖାଲି;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    ଯଦି (print_setup == ଖାଲି)
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
	ଦ୍ୱିଗୁଣ		*page_width,
	ଦ୍ୱିଗୁଣ		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ଅଚଳ ଅକ୍ଷର *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = ଖାଲି;
    print_setup = ଖାଲି;

    ଯଦି (print_settings == ଖାଲି)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ଯଦି (opt->present)
	{
	    ଯଦି (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    ଅନ୍ୟଥା ଯଦି (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    ଅନ୍ୟଥା
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ଯଦି (opt->present)
	{
	    ଯଦି (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    ଅନ୍ୟଥା
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ଯଦି (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ଅଚଳ ଅକ୍ଷର *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ଯଦି (opt->present)
	{
	    ଯଦି (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    ଅନ୍ୟଥା
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    ଯଦି (page_setup == ଖାଲି)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), ଖାଲି,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    ଯେତେବେଳେ (!done)
	g_main_context_iteration(ଖାଲି, TRUE);

    ଯଦି (print_setup == ଖାଲି)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_ସନ୍ଦେଶ = FALSE;
	ଫେରନ୍ତୁ ଖାଲି;
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
    ଯଦି (temp == ଖାଲି)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_ସନ୍ଦେଶ = FALSE;
	ଫେରନ୍ତୁ ଖାଲି;
    }
    print_file = g_file_new_for_path((ଅକ୍ଷର *)temp);

    print_dialog = dialog;

    ଫେରନ୍ତୁ temp;
}

    ସ୍ଥିର ଶୂନ୍ୟ
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    ଅକ୍ଷର *path = g_file_get_path(file);

    (ଶୂନ୍ୟ)gtk_print_dialog_print_file_finish(dialog, result, ଖାଲି);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    ଶୂନ୍ୟ
gui_gtk4_print_finish(ଶୂନ୍ୟ)
{
    ଯଦି (print_dialog != ଖାଲି)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, ଖାଲି,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ଖାଲି;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    GdkTexture *texture;
} image_gdk_T;

ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    Gsk_Render_ଗଣ୍ଠି   **nodes; // Note that elements may be NULL
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    n_nodes;
} image_placement_gdk_T;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    size_t	    stride;
    GBytes	    *bytes;
    size_t	    size;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    w, h;

    ଯଦି (ctx == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    ଅନ୍ୟଥା
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    ସ୍ଥିର ଶୂନ୍ୟ
clear_nodes(image_placement_gdk_T *ctx)
{
    ଯଦି (ctx->nodes == ଖାଲି || gui.drawarea == ଖାଲି)
	ଫେରନ୍ତୁ;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < ctx->n_nodes; i++)
    {
	ଯଦି (ctx->nodes[i] == ଖାଲି)
	    ଜାରି;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_ଗଣ୍ଠି_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ଯଦି (ctx == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;

    place->backend_data = ctx;
    ଫେରନ୍ତୁ OK;
}

    ଶୂନ୍ୟ
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    ଶୂନ୍ୟ
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    n_rects;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    iw, ih;
    Gsk_Render_ଗଣ୍ଠି	    *texture_ଗଣ୍ଠି;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    ଯଦି (rects == ଖାଲି)
	ଫେରନ୍ତୁ;

    clear_nodes(ctx);
    ଯଦି (n_rects == 0)
	ଫେରନ୍ତୁ;

    image_get_dimensions(img, &iw, &ih);
    texture_ଗଣ୍ଠି = gsk_texture_ଗଣ୍ଠି_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ଯଦି (texture_ଗଣ୍ଠି == ଖାଲି)
	// Not sure if this can happen...
	ଫେରନ୍ତୁ;

    ctx->nodes = g_malloc_n(n_rects, ଆକାର(Gsk_Render_ଗଣ୍ଠି *));
    ctx->n_nodes = n_rects;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		row, col;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		posx, posy;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x, y, w, h;
	Gsk_Render_ଗଣ୍ଠି	*ଗଣ୍ଠି;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	ଗଣ୍ଠି = gsk_clip_ଗଣ୍ଠି_new(texture_ଗଣ୍ଠି,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ଯଦି (ଗଣ୍ଠି != ଖାଲି)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), ଗଣ୍ଠି);

	ctx->nodes[i] = ଗଣ୍ଠି;
    }
    gsk_render_ଗଣ୍ଠି_unref(texture_ଗଣ୍ଠି);
}

    ଶୂନ୍ୟ
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
