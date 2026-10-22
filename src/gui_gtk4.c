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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_GUI_GTK

#అన్తర్భావ <gdk/gdkkeysyms.h>
#అన్తర్భావ <gdk/gdk.h>
#అన్తర్భావ <gtk/gtk.h>
#అన్తర్భావ "gui_gtk4_f.h"
#అన్తర్భావ "gui_gtk4_cb.h"
#అన్తర్భావ "gui_gtk4_da.h"
#నిర్వచితమైతే FEAT_TOOLBAR
# include "gui_gtk4_tb.h"
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
# include "gui_gtk4_menu.h"
#అయితే_ముగింపు

/*
 * Geometry string parser, replacing XParseGeometry to remove X11 dependency.
 * Format: [WIDTHxHEIGHT][{+-}XOFF{+-}YOFF]
 */
#నిర్వచించు No_విలువ		0x0000
#నిర్వచించు వెడల్పు_విలువ	0x0004
#నిర్వచించు ఎత్తు_విలువ	0x0008

    స్థిర పూర్ణాంకం
vim_parse_geometry(స్థిరరాశి అక్షరం *str, గుర్తులేని పూర్ణాంకం *వెడల్పు, గుర్తులేని పూర్ణాంకం *ఎత్తు)
{
    పూర్ణాంకం ముసుగు = No_విలువ;
    అక్షరం *చివర;
    దీర్ఘ val;

    ఒకవేళ (str == ఖాళీ || *str == NUL)
	తిరిగి ముసుగు;

    // Parse width
    ఒకవేళ (*str != '+' && *str != '-')
    {
	val = strtol(str, &చివర, 10);
	ఒకవేళ (చివర != str)
	{
	    *వెడల్పు = (గుర్తులేని పూర్ణాంకం)val;
	    ముసుగు |= వెడల్పు_విలువ;
	    str = చివర;
	}
    }

    // Parse 'x' or 'X' separator and height
    ఒకవేళ (*str == 'x' || *str == 'X')
    {
	str++;
	val = strtol(str, &చివర, 10);
	ఒకవేళ (చివర != str)
	{
	    *ఎత్తు = (గుర్తులేని పూర్ణాంకం)val;
	    ముసుగు |= ఎత్తు_విలువ;
	    str = చివర;
	}
    }

    తిరిగి ముసుగు;
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
స్థిర పూర్ణాంకం చివరి_shape = 0;
#అయితే_ముగింపు

#నిర్వచించు DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
స్థిర గణన {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
స్థిర పూర్ణాంకం gtk4_main_loop_level = 0;
స్థిర పూర్ణాంకం gtk4_main_loop_quit = FALSE;

#నిర్వచితమైతే USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#అయితే_ముగింపు

రకనిర్వచనం gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
స్థిర నిర్మాణం special_కీలకం
{
    guint కీలకం_sym;
    char_u code0;
    char_u code1;
}
స్థిరరాశి special_కీలకాలు[] =
{
    {GDK_కీలకం_పైకి,	'k', 'u'},
    {GDK_కీలకం_కిందికి,	'k', 'd'},
    {GDK_కీలకం_ఎడమ,	'k', 'l'},
    {GDK_కీలకం_కుడి,	'k', 'r'},
    {GDK_కీలకం_F1,	'k', '1'},
    {GDK_కీలకం_F2,	'k', '2'},
    {GDK_కీలకం_F3,	'k', '3'},
    {GDK_కీలకం_F4,	'k', '4'},
    {GDK_కీలకం_F5,	'k', '5'},
    {GDK_కీలకం_F6,	'k', '6'},
    {GDK_కీలకం_F7,	'k', '7'},
    {GDK_కీలకం_F8,	'k', '8'},
    {GDK_కీలకం_F9,	'k', '9'},
    {GDK_కీలకం_F10,	'k', ';'},
    {GDK_కీలకం_F11,	'F', '1'},
    {GDK_కీలకం_F12,	'F', '2'},
    {GDK_కీలకం_Help,	'%', '1'},
    {GDK_కీలకం_Undo,	'&', '8'},
    {GDK_కీలకం_వెనుక_Space,	'k', 'b'},
    {GDK_కీలకం_చొప్పించు,	'k', 'I'},
    {GDK_కీలకం_Delete,	'k', 'D'},
    {GDK_కీలకం_Home,	'k', 'h'},
    {GDK_కీలకం_చివర,	'@', '7'},
    {GDK_కీలకం_పూర్వ,	'k', 'P'},
    {GDK_కీలకం_తదుపరి,	'k', 'N'},
    {GDK_కీలకం_Print,	'%', '9'},
    {GDK_కీలకం_KP_ఎడమ,	'k', 'l'},
    {GDK_కీలకం_KP_కుడి,	'k', 'r'},
    {GDK_కీలకం_KP_పైకి,	'k', 'u'},
    {GDK_కీలకం_KP_కిందికి,	'k', 'd'},
    {GDK_కీలకం_KP_చొప్పించు,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_కీలకం_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_కీలకం_KP_Home,	'K', '1'},
    {GDK_కీలకం_KP_చివర,	'K', '4'},
    {GDK_కీలకం_KP_పూర్వ,	'K', '3'},
    {GDK_కీలకం_KP_తదుపరి,	'K', '5'},
    {GDK_కీలకం_KP_కూడు,	'K', '6'},
    {GDK_కీలకం_KP_వ్యవకలనం, 'K', '7'},
    {GDK_కీలకం_KP_Divide,	'K', '8'},
    {GDK_కీలకం_KP_Multiply, 'K', '9'},
    {GDK_కీలకం_KP_Enter,	'K', 'A'},
    {GDK_కీలకం_KP_Decimal,	'K', 'B'},
    {GDK_కీలకం_KP_0,	'K', 'C'},
    {GDK_కీలకం_KP_1,	'K', 'D'},
    {GDK_కీలకం_KP_2,	'K', 'E'},
    {GDK_కీలకం_KP_3,	'K', 'F'},
    {GDK_కీలకం_KP_4,	'K', 'G'},
    {GDK_కీలకం_KP_5,	'K', 'H'},
    {GDK_కీలకం_KP_6,	'K', 'I'},
    {GDK_కీలకం_KP_7,	'K', 'J'},
    {GDK_కీలకం_KP_8,	'K', 'K'},
    {GDK_కీలకం_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    స్థిర పూర్ణాంకం
keyval_గమ్యం_string(గుర్తులేని పూర్ణాంకం keyval, char_u *string)
{
    పూర్ణాంకం		len;
    guint32	uc;

    uc = gdk_keyval_గమ్యం_unicode(keyval);
    ఒకవేళ (uc != 0)
    {
	len = utf_char2bytes((పూర్ణాంకం)uc, string);
    }
    లేకపోతే
    {
	len = 1;
	ఎంపిక (keyval)
	{
	    స్థితి GDK_కీలకం_Tab: స్థితి GDK_కీలకం_KP_Tab: స్థితి GDK_కీలకం_ISO_ఎడమ_Tab:
		string[0] = TAB;
		ఆపు;
	    స్థితి GDK_కీలకం_Linefeed:
		string[0] = NL;
		ఆపు;
	    స్థితి GDK_కీలకం_Return: స్థితి GDK_కీలకం_ISO_Enter: స్థితి GDK_కీలకం_3270_Enter:
		string[0] = CAR;
		ఆపు;
	    స్థితి GDK_కీలకం_Escape:
		string[0] = ESC;
		ఆపు;
	    అప్రమేయ:
		len = 0;
		ఆపు;
	}
    }
    string[len] = NUL;
    తిరిగి len;
}

    స్థిర పూర్ణాంకం
modifiers_gdk2vim(guint state)
{
    పూర్ణాంకం modifiers = 0;

    ఒకవేళ (state & GDK_జరుపు_ముసుగు)
	modifiers |= MOD_ముసుగు_జరుపు;
    ఒకవేళ (state & GDK_CONTROL_ముసుగు)
	modifiers |= MOD_ముసుగు_CTRL;
    ఒకవేళ (state & GDK_ALT_ముసుగు)
	modifiers |= MOD_ముసుగు_ALT;
    ఒకవేళ (state & GDK_META_ముసుగు)
	modifiers |= MOD_ముసుగు_META;
    ఒకవేళ (state & GDK_SUPER_ముసుగు)
	modifiers |= MOD_ముసుగు_CMD;

    తిరిగి modifiers;
}

స్థిర GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
స్థిర gboolean కీలకం_press_event(Gtk_Event_Controller_కీలకం *controller, guint keyval, guint keycode, GdkModifierType state, gpointer దత్తాంశం);
స్థిర శూన్య కీలకం_release_event(Gtk_Event_Controller_కీలకం *controller, guint keyval, guint keycode, GdkModifierType state, gpointer దత్తాంశం);
స్థిర శూన్య button_press_event(GtkGestureClick *gesture, పూర్ణాంకం n_press, ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం);
స్థిర శూన్య button_release_event(GtkGestureClick *gesture, పూర్ణాంకం n_press, ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం);
స్థిర శూన్య motion_notify_event(GtkEventControllerMotion *controller, ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం);
స్థిర శూన్య enter_notify_event(GtkEventControllerMotion *controller, ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం);
స్థిర gboolean scroll_event(GtkEventControllerScroll *controller, ద్విగుణ dx, ద్విగుణ dy, gpointer దత్తాంశం);
స్థిర శూన్య focus_లోపల_event(GtkEventControllerFocus *controller, gpointer దత్తాంశం);
స్థిర శూన్య focus_బహిర్గతం_event(GtkEventControllerFocus *controller, gpointer దత్తాంశం);
#నిర్వచితమైతే FEAT_DND
స్థిర gboolean drop_cb(Gtk_Drop_లక్ష్యం_Async *లక్ష్యం, GdkDrop *drop, ద్విగుణ x, ద్విగుణ y, శూన్య *దత్తాంశం);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
స్థిర శూన్య tabline_enter_cb(GtkEventController *controller, ద్విగుణ x, ద్విగుణ y, శూన్య *udata);
స్థిర శూన్య on_select_tab(GtkNotebook *notebook, gpointer *పుట, gint idx, gpointer దత్తాంశం);
స్థిర శూన్య on_tab_reordered(GtkNotebook *notebook, gpointer *పుట, gint idx, gpointer దత్తాంశం);
స్థిర VimMenu *create_tabline_popup_menu(శూన్య);
స్థిర శూన్య tabline_కూడు_css(శూన్య);
స్థిర శూన్య tabline_menu_press_event(GtkGestureClick *gesture, పూర్ణాంకం n_press, ద్విగుణ x, ద్విగుణ y, GtkWidget *popover);
#అయితే_ముగింపు
స్థిర శూన్య mainwin_destroy_cb(GObject *object, gpointer దత్తాంశం);
స్థిర gboolean delete_event_cb(Gtk_కిటికీ *కిటికీ_2, gpointer దత్తాంశం);
స్థిర పూర్ణాంకం query_సూచిక_pos(పూర్ణాంకం *x, పూర్ణాంకం *y, GdkModifierType *state);
స్థిర శూన్య mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_దత్తాంశం);
స్థిర శూన్య సమితి_form_కొలత(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు);
స్థిర శూన్య drawarea_realize_cb(GtkWidget *widget, gpointer దత్తాంశం);
స్థిర శూన్య drawarea_unrealize_cb(GtkWidget *widget, gpointer దత్తాంశం);
స్థిర శూన్య clipboard_మారింది_cb(GdkClipboard *clipboard, gpointer user_దత్తాంశం);
#నిర్వచితమైతే FEAT_MENU
స్థిర శూన్య show_menubar_popover(శూన్య);
#అయితే_ముగింపు

స్థిర స్థిరరాశి అక్షరం *prgname = ఖాళీ;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    స్థిర gboolean
arg_సరిపోలిక(స్థిరరాశి అక్షరం *s, స్థిరరాశి అక్షరం *name, అక్షరం **విలువ)
{
    size_t  len = అక్షరశ్రేణి_పొడవు(name);

    ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, name, len) != 0)
	తిరిగి FALSE;
    ఒకవేళ (s[len] == '=')
	*విలువ = (అక్షరం *)s + len + 1;
    లేకపోతే ఒకవేళ (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	తిరిగి FALSE;
    తిరిగి TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    శూన్య
gui_mch_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    పూర్ణాంకం	i = 0;

    అయితే (i < *argc)
    {
	అక్షరం	*s = argv[i];
	అక్షరం	*విలువ = ఖాళీ;
	పూర్ణాంకం	has_inline_విలువ = FALSE;
	పూర్ణాంకం	n_strip;

	ఒకవేళ (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    కొనసాగు;
	}

	ఒకవేళ (strchr(s, '=') != ఖాళీ)
	    has_inline_విలువ = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	ఒకవేళ (!has_inline_విలువ)
	{
	    ఒకవేళ (i + 1 < *argc
		    && strcmp(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		విలువ = argv[i + 1];
	    లేకపోతే
		విలువ = ఖాళీ;
	}

	ఒకవేళ (arg_సరిపోలిక(s, "-fn", &విలువ) || arg_సరిపోలిక(s, "-font", &విలువ))
	    font_వాదం = విలువ;
	లేకపోతే ఒకవేళ (arg_సరిపోలిక(s, "-geom", &విలువ)
		|| arg_సరిపోలిక(s, "-geometry", &విలువ))
	{
	    ఒకవేళ (విలువ != ఖాళీ)
		gui.geom = vim_strsave((char_u *)విలువ);
	}
	లేకపోతే ఒకవేళ (arg_సరిపోలిక(s, "-bg", &విలువ)
		|| arg_సరిపోలిక(s, "-background", &విలువ))
	    background_వాదం = విలువ;
	లేకపోతే ఒకవేళ (arg_సరిపోలిక(s, "-fg", &విలువ)
		|| arg_సరిపోలిక(s, "-foreground", &విలువ))
	    foreground_వాదం = విలువ;
#నిర్వచితమైతే FEAT_NETBEANS_INTG
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_పోలిక(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_విలువ = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#అయితే_ముగింపు
	లేకపోతే ఒకవేళ (arg_సరిపోలిక(s, "--prg-name", &విలువ))
	    // GTK4 specific
	    prgname = విలువ;
	లేకపోతే
	{
	    i++;
	    కొనసాగు;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	ఒకవేళ (విలువ != ఖాళీ && !has_inline_విలువ)
	    n_strip = 2;

	ఒకవేళ (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    ఒకవేళ (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * పరిమాణం(అక్షరం *));
	    argv[*argc] = ఖాళీ;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    శూన్య
gui_mch_free_all(శూన్య)
{
}

    స్థిర guint
timeout_కూడు(పూర్ణాంకం సమయం, timeout_cb_type (*callback)(gpointer), పూర్ణాంకం *flagp)
{
    తిరిగి g_timeout_కూడు((guint)సమయం, (GSourceFunc)callback, flagp);
}

    స్థిర శూన్య
timeout_remove(guint timer)
{
    g_ఆధారం_remove(timer);
}

స్థిర long_u blink_waittime = 700;
స్థిర long_u blink_ontime = 400;
స్థిర long_u blink_offtime = 250;
స్థిర guint blink_timer = 0;

    స్థిర timeout_cb_type
blink_cb(gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_కూడు(blink_offtime, blink_cb, ఖాళీ);
    }
    లేకపోతే
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_కూడు(blink_ontime, blink_cb, ఖాళీ);
    }
    తిరిగి FALSE;
}

    పూర్ణాంకం
gui_mch_is_blinking(శూన్య)
{
    తిరిగి blink_state != BLINK_NONE;
}

    పూర్ణాంకం
gui_mch_is_blink_off(శూన్య)
{
    తిరిగి blink_state == BLINK_OFF;
}

    శూన్య
gui_mch_సమితి_blinking(దీర్ఘ waittime, దీర్ఘ on, దీర్ఘ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    ఒకవేళ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ఒకవేళ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    శూన్య
gui_mch_ప్రారంభం_blink(శూన్య)
{
    ఒకవేళ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
    {
	blink_timer = timeout_కూడు(blink_waittime, blink_cb, ఖాళీ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    పూర్ణాంకం
gui_mch_early_init_check(పూర్ణాంకం give_సందేశం వాడని)
{
    తిరిగి OK;
}

    పూర్ణాంకం
gui_mch_init_check(శూన్య)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_సమితి_prgname(prgname == ఖాళీ ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    ఒకవేళ (g_getenv("EGL_LOG_LEVEL") == ఖాళీ)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    తిరిగి OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    పూర్ణాంకం
gui_mch_init(శూన్య)
{
    // Allocate GdkRGBA color structs.
    gui.fgcolor = g_new0(GdkRGBA, 1);
    gui.bgcolor = g_new0(GdkRGBA, 1);
    gui.spcolor = g_new0(GdkRGBA, 1);

    gui.def_norm_pixel = 0x00000000;	// black
    gui.def_వెనుక_pixel = 0x00ffffff;	// white
    gui.norm_pixel = gui.def_norm_pixel;
    gui.వెనుక_pixel = gui.def_వెనుక_pixel;

    gui.scrollbar_వెడల్పు = SB_DEFAULT_వెడల్పు;
    gui.scrollbar_ఎత్తు = SB_DEFAULT_వెడల్పు;

    // Create the main window.
    gui.mainwin = gtk_కిటికీ_new();
    gtk_widget_సమితి_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.పాఠ్యం_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_సమితి_పునాది_dir(gui.పాఠ్యం_context, PANGO_DIRECTION_LTR);

    g_signal_connect(G_OBJECT(gui.mainwin), "close-request",
		     G_CALLBACK(delete_event_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), ఖాళీ);

    // A vertical box holds the menubar, toolbar and main text window.
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_సమితి_homogeneous(GTK_BOX(vbox), FALSE);
    gtk_కిటికీ_సమితి_బిడ్డ(GTK_కిటికీ(gui.mainwin), vbox);

#నిర్వచితమైతే FEAT_MENU
    gui.menubar = vim_menu_bar_new();
    gtk_widget_సమితి_name(gui.menubar, "vim-menubar");
    gtk_widget_సమితి_visible(gui.menubar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.menubar);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
    gui.toolbar = vim_toolbar_new();
    gtk_widget_సమితి_name(gui.toolbar, "vim-toolbar");
    gtk_widget_సమితి_visible(gui.toolbar, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.toolbar);
    vim_toolbar_సమితి_style(VIM_TOOLBAR(gui.toolbar), toolbar_flags, tbis_flags);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_TABLINE
    gui.tabline = gtk_notebook_new();
    gtk_notebook_సమితి_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_సమితి_show_tabs(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_notebook_సమితి_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
    gtk_widget_కూడు_css_class(gui.tabline, "vim-tabline");
    tabline_కూడు_css();
    gtk_widget_సమితి_visible(gui.tabline, FALSE);
    gtk_box_append(GTK_BOX(vbox), gui.tabline);

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ఖాళీ);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), ఖాళీ);

	// Make sure that tabline_enter_cb() is always called before
	// tabpage_enter_cb().
	gtk_event_controller_సమితి_ప్రసరణ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	gtk_widget_కూడు_controller(gui.tabline, mcontroller);
    }

    // Create right click popup menu for tabline
    {
	GtkGesture	*click;
	VimMenu		*menu;

	click = gtk_gesture_click_new();
	menu = create_tabline_popup_menu();

	gtk_widget_సమితి_మాతృ(GTK_WIDGET(menu), gui.tabline);
	g_object_సమితి_దత్తాంశం(G_OBJECT(gui.tabline), "menu", menu);

	gtk_popover_సమితి_has_arrow(GTK_POPOVER(menu), FALSE);
	gtk_popover_సమితి_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
	// Make popover start at top left corner
	gtk_widget_సమితి_halign(GTK_WIDGET(menu), GTK_ALIGN_ప్రారంభం);

	// Listen for anny mouse button
	gtk_gesture_single_సమితి_button(GTK_GESTURE_SINGLE(click), 0);

	g_signal_connect_object(click, "pressed",
		G_CALLBACK(tabline_menu_press_event),
		menu, G_CONNECT_DEFAULT);
	gtk_widget_కూడు_controller(gui.tabline, GTK_EVENT_CONTROLLER(click));
    }
#అయితే_ముగింపు

    // The form widget manages absolute positioning of scrollbars and the draw
    // area.
    gui.formwin = vim_form_new();
    gtk_widget_సమితి_name(gui.formwin, "vim-gtk-form");
    gtk_widget_సమితి_vexpand(gui.formwin, TRUE);
    gtk_widget_సమితి_hexpand(gui.formwin, TRUE);
    gtk_box_append(GTK_BOX(vbox), gui.formwin);

    // The drawing area for the editor content.
    gui.drawarea = vim_draw_area_new();
    gtk_widget_సమితి_focusable(gui.drawarea, TRUE);
    gtk_widget_సమితి_vexpand(gui.drawarea, TRUE);
    gtk_widget_సమితి_hexpand(gui.drawarea, TRUE);
    vim_form_put(VIM_FORM(gui.formwin), gui.drawarea, 0, 0);

    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ఖాళీ);

    // Set up event controllers.
    {
	GtkEventController *కీలకం_ctrl = gtk_event_controller_కీలకం_new();
	g_signal_connect(కీలకం_ctrl, "key-pressed",
			 G_CALLBACK(కీలకం_press_event), ఖాళీ);
	g_signal_connect(కీలకం_ctrl, "key-released",
			 G_CALLBACK(కీలకం_release_event), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea, కీలకం_ctrl);
#నిర్వచితమైతే FEAT_XIM
	xim_init();
#అయితే_ముగింపు
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_సమితి_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), ఖాళీ);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), ఖాళీ);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_లోపల_event), ఖాళీ);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_బహిర్గతం_event), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea, focus);
    }

#నిర్వచితమైతే FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_లక్ష్యం = gtk_drop_లక్ష్యం_async_new(ఖాళీ,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_సమితి_dnd_targets();
	g_signal_connect(gui.drop_లక్ష్యం, "drop",
			 G_CALLBACK(drop_cb), ఖాళీ);
	gtk_widget_కూడు_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_లక్ష్యం));
    }
#అయితే_ముగింపు

    gui.border_స్థానభ్రంశం = gui.border_వెడల్పు;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_సూచిక = gdk_cursor_new_from_name("none", ఖాళీ);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	ఒకవేళ (primary != ఖాళీ)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_మారింది_cb), &clip_నక్షత్రం);
	ఒకవేళ (regular != ఖాళీ)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_మారింది_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_నక్షత్రం);

    తిరిగి OK;
}

    శూన్య
gui_mch_new_colors(శూన్య)
{
    ఒకవేళ (gui.drawarea != ఖాళీ && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_వరుస_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    పూర్ణాంకం
gui_mch_open(శూన్య)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_వెడల్పు;
    guint pixel_ఎత్తు;

    ఒకవేళ (gui.geom != ఖాళీ)
    {
	పూర్ణాంకం		ముసుగు;
	గుర్తులేని పూర్ణాంకం	w, h;

	ముసుగు = vim_parse_geometry((అక్షరం *)gui.geom, &w, &h);

	ఒకవేళ (ముసుగు & వెడల్పు_విలువ)
	    Columns = w;
	ఒకవేళ (ముసుగు & ఎత్తు_విలువ)
	{
	    ఒకవేళ (p_కిటికీ > (దీర్ఘ)h - 1 || !option_was_సమితి((char_u *)"window"))
		p_కిటికీ = h - 1;
	    అడ్డువరుసలు = h;
	}
	హద్దు_screen_కొలత();

	VIM_CLEAR(gui.geom);
    }
    లేకపోతే
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	ఒకవేళ (Columns > 80)
	    Columns = 80;
	ఒకవేళ (అడ్డువరుసలు > 24)
	    అడ్డువరుసలు = 24;
    }

    pixel_వెడల్పు = (guint)(gui_get_పునాది_వెడల్పు() + Columns * gui.char_వెడల్పు);
    pixel_ఎత్తు = (guint)(gui_get_పునాది_ఎత్తు() + అడ్డువరుసలు * gui.char_ఎత్తు);

    pixel_వెడల్పు  += get_menu_tool_వెడల్పు();
    pixel_ఎత్తు += get_menu_tool_ఎత్తు();

    ఒకవేళ (foreground_వాదం != ఖాళీ)
	fg_pixel = gui_get_color((char_u *)foreground_వాదం);
    ఒకవేళ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ఒకవేళ (background_వాదం != ఖాళీ)
	bg_pixel = gui_get_color((char_u *)background_వాదం);
    ఒకవేళ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ఒకవేళ (దొరికింది_తిరగేయి_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_వెనుక_pixel = fg_pixel;
    }
    లేకపోతే
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_వెనుక_pixel = bg_pixel;
    }

    సమితి_సాధారణ_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ఖాళీ);
    // Resize is handled by GtkForm's size_allocate callback.

    సమితి_form_కొలత((పూర్ణాంకం)pixel_వెడల్పు - get_menu_tool_వెడల్పు(),
	    (పూర్ణాంకం)pixel_ఎత్తు - get_menu_tool_ఎత్తు());

    gtk_కిటికీ_present(GTK_కిటికీ(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    తిరిగి OK;
}

    శూన్య
gui_mch_exit(పూర్ణాంకం rc వాడని)
{
    ఒకవేళ (gui.mainwin != ఖాళీ)
    {
#నిర్వచితమైతే FEAT_GUI_TABLINE
	// Must unparent popover menu for tabline or we will get warning
	gtk_widget_unparent(g_object_get_దత్తాంశం(G_OBJECT(gui.tabline), "menu"));
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_BEVAL_GUI
	// Make sure to destroy popover used for balloon eval, or we will get a
	// warning from GTK that the draw area still has children left.
	gui_mch_destroy_beval_area(balloonEval);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MENU
	// Make sure to unparent any popover menus
	{
	    vimmenu_T *menu;

	    FOR_ALL_MENUS(menu)
	    {
		ఒకవేళ ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != ఖాళీ)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#అయితే_ముగింపు
	gtk_కిటికీ_destroy(GTK_కిటికీ(gui.mainwin));
    }
}

    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    తిరిగి FAIL;
}

    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x వాడని, పూర్ణాంకం y వాడని)
{
    // GTK4/Wayland: window positioning not available
}

    పూర్ణాంకం
gui_mch_maximized(శూన్య)
{
    తిరిగి gtk_కిటికీ_is_maximized(GTK_కిటికీ(gui.mainwin));
}

    శూన్య
gui_mch_unmaximize(శూన్య)
{
    ఒకవేళ (gui.mainwin != ఖాళీ)
	gtk_కిటికీ_unmaximize(GTK_కిటికీ(gui.mainwin));
}

    శూన్య
gui_mch_సమితి_fullscreen(పూర్ణాంకం flag)
{
    ఒకవేళ (gui.mainwin == ఖాళీ)
	తిరిగి;
    ఒకవేళ (flag)
	gtk_కిటికీ_fullscreen(GTK_కిటికీ(gui.mainwin));
    లేకపోతే
	gtk_కిటికీ_unfullscreen(GTK_కిటికీ(gui.mainwin));
}

    స్థిర శూన్య
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec వాడని, gpointer user_దత్తాంశం వాడని)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    ఒకవేళ (gtk_కిటికీ_is_fullscreen(GTK_కిటికీ(obj)))
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
    శూన్య
gui_mch_newfont(శూన్య)
{
    పూర్ణాంకం w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_వెడల్పు(gui.formwin);
    h = gtk_widget_get_ఎత్తు(gui.formwin);
    gui_resize_shell(w, h);
}

    శూన్య
gui_mch_settitle(char_u *title, char_u *icon వాడని)
{
    ఒకవేళ (title != ఖాళీ && gui.mainwin != ఖాళీ)
	gtk_కిటికీ_సమితి_title(GTK_కిటికీ(gui.mainwin), (స్థిరరాశి అక్షరం *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    శూన్య
gui_gtk_init_decor_ఎత్తు(శూన్య)
{
    పూర్ణాంకం h = gtk_widget_get_ఎత్తు(gui.mainwin);

    ఒకవేళ (h == 0)
	తిరిగి;

    h -= get_menu_tool_ఎత్తు();
    h -= gtk_widget_get_ఎత్తు(gui.formwin);

    gui.decor_ఎత్తు = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    స్థిర శూన్య
సమితి_form_కొలత(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    ఒకవేళ (gtk_widget_get_వెడల్పు(gui.formwin) == వెడల్పు
	    && gtk_widget_get_ఎత్తు(gui.formwin) == ఎత్తు)
	తిరిగి;

    // Remember the size the form widget is supposed to get. An allocation
    // that arrives before the compositor has answered this request still has
    // the previous size and must not be used.
    gui.pending_form_w = వెడల్పు;
    gui.pending_form_h = ఎత్తు;
    gui.pending_form_skip = 1;

    // GtkWindow default size also includes client side decorations, so must
    // include it also.  It also keeps the natural width of the toolbar from
    // deciding the width.
    gtk_కిటికీ_సమితి_default_కొలత(GTK_కిటికీ(gui.mainwin),
	    వెడల్పు + get_menu_tool_వెడల్పు(),
	    ఎత్తు + get_menu_tool_ఎత్తు() + gui.decor_ఎత్తు);

    // The window drops a request made while it is being presented, and
    // "gui.decor_height" is not known before that.  A size request on the form
    // widget is honoured then; it is dropped again once the size was given.
    gtk_widget_సమితి_కొలత_request(gui.formwin, వెడల్పు, ఎత్తు);
}

    శూన్య
gui_mch_సమితి_shellsize(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు,
	పూర్ణాంకం min_వెడల్పు వాడని, పూర్ణాంకం min_ఎత్తు వాడని,
	పూర్ణాంకం పునాది_వెడల్పు వాడని, పూర్ణాంకం పునాది_ఎత్తు వాడని,
	పూర్ణాంకం direction వాడని)
{
    సమితి_form_కొలత(వెడల్పు, ఎత్తు);

    gui_mch_update();
}

    శూన్య
gui_mch_get_screen_dimensions(పూర్ణాంకం *screen_w, పూర్ణాంకం *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    ఒకవేళ (surface != ఖాళీ)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	ఒకవేళ (monitor != ఖాళీ)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.వెడల్పు;
	    *screen_h = geom.ఎత్తు;
	    తిరిగి;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#నిర్వచితమైతే FEAT_MENU
    శూన్య
gui_mch_enable_menu(పూర్ణాంకం showit)
{
    ఒకవేళ (gui.menubar != ఖాళీ)
    {
	gtk_widget_సమితి_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	ఒకవేళ (gui.drawarea != ఖాళీ)
	    gtk_widget_వరుస_draw(gui.drawarea);
    }
}
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_TOOLBAR
    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    ఒకవేళ (gui.toolbar != ఖాళీ)
    {
	gtk_widget_సమితి_visible(gui.toolbar, showit);
	ఒకవేళ (showit)
	    vim_toolbar_సమితి_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	ఒకవేళ (gui.drawarea != ఖాళీ)
	    gtk_widget_వరుస_draw(gui.drawarea);
    }
}
#అయితే_ముగింపు

    శూన్య
gui_mch_సమితి_dark_theme(పూర్ణాంకం dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    ఒకవేళ (settings != ఖాళీ)
	g_object_సమితి(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, ఖాళీ);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    పూర్ణాంకం
gui_mch_adjust_charheight(శూన్య)
{
    PangoFontMetrics *metrics;
    పూర్ణాంకం ascent;
    పూర్ణాంకం descent;

    metrics = pango_context_get_metrics(gui.పాఠ్యం_context, gui.norm_font,
			    pango_context_get_language(gui.పాఠ్యం_context));
    ascent = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);
    pango_font_metrics_unref(metrics);

    gui.char_ఎత్తు = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ఎత్తు = MAX(gui.char_ఎత్తు, gui.char_ascent + 1);

    తిరిగి OK;
}

రకనిర్వచనం నిర్మాణం {
    PangoFontDescription    *ఫలితం;
    gboolean		    పూర్తి;
} Font_Dialog_దత్తాంశం;

    స్థిర శూన్య
font_dialog_ముగింపు_cb(GObject *ఆధారం, GAsync_ఫలితం *res, gpointer దత్తాంశం)
{
    Font_Dialog_దత్తాంశం *fdd = (Font_Dialog_దత్తాంశం *)దత్తాంశం;
    fdd->ఫలితం = gtk_font_dialog_ఎంచుకో_font_ముగింపు(
		    GTK_FONT_DIALOG(ఆధారం), res, ఖాళీ);
    fdd->పూర్తి = TRUE;
}

    స్థిర gboolean
font_filter(gpointer అంశం, gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (PANGO_IS_FONT_FAMILY(అంశం))
	తిరిగి pango_font_family_is_monospace(PANGO_FONT_FAMILY(అంశం));
    ఒకవేళ (PANGO_IS_FONT_FACE(అంశం))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(అంశం));
	ఒకవేళ (family != ఖాళీ)
	    తిరిగి pango_font_family_is_monospace(family);
    }
    తిరిగి TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = ఖాళీ;
    char_u		*fontname = ఖాళీ;
    Font_Dialog_దత్తాంశం	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_సమితి_modal(dlg, TRUE);
    gtk_font_dialog_సమితి_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, ఖాళీ, ఖాళీ)));

    ఒకవేళ (oldval != ఖాళీ && oldval[0] != NUL)
    {
	char_u *oldname;

	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ఖాళీ);
	లేకపోతే
	    oldname = oldval;

	ఒకవేళ (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    ఒకవేళ (p != ఖాళీ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ఒకవేళ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((స్థిరరాశి అక్షరం *)oldname);
	ఒకవేళ (oldname != oldval)
	    vim_free(oldname);
    }
    లేకపోతే
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.ఫలితం = ఖాళీ;
    fdd.పూర్తి = FALSE;

    gtk_font_dialog_ఎంచుకో_font(dlg, GTK_కిటికీ(gui.mainwin),
	    initial, ఖాళీ, font_dialog_ముగింపు_cb, &fdd);

    అయితే (!fdd.పూర్తి)
	g_main_context_iteration(ఖాళీ, TRUE);

    ఒకవేళ (fdd.ఫలితం != ఖాళీ)
    {
	అక్షరం *name = pango_font_description_గమ్యం_string(fdd.ఫలితం);
	ఒకవేళ (name != ఖాళీ)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ఒకవేళ (p != ఖాళీ && ఆదానం_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ఆదానం_conv, p, ఖాళీ);
		vim_free(p);
	    }
	    లేకపోతే
		fontname = p;
	}
	pango_font_description_free(fdd.ఫలితం);
    }

    ఒకవేళ (initial != ఖాళీ)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    తిరిగి fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    స్థిర శూన్య
ascii_glyph_పట్టిక_init(శూన్య)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *అంశం_list;
    పూర్ణాంకం		    i;

    ఒకవేళ (gui.ascii_glyphs != ఖాళీ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ఒకవేళ (gui.ascii_font != ఖాళీ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ఖాళీ;
    gui.ascii_font   = ఖాళీ;

    కొరకు (i = 0; i < 128; ++i)
    {
	ఒకవేళ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	లేకపోతే
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    అంశం_list = pango_itemize(gui.పాఠ్యం_context, (స్థిరరాశి అక్షరం *)ascii_chars,
			      0, పరిమాణం(ascii_chars), attr_list, ఖాళీ);

    ఒకవేళ (అంశం_list != ఖాళీ && అంశం_list->తదుపరి == ఖాళీ)
    {
	Pango_అంశం   *అంశం;
	పూర్ణాంకం	    వెడల్పు;

	అంశం  = (Pango_అంశం *)అంశం_list->దత్తాంశం;
	వెడల్పు = gui.char_వెడల్పు * PANGO_SCALE;

	gui.ascii_font = అంశం->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((స్థిరరాశి అక్షరం *)ascii_chars, పరిమాణం(ascii_chars),
		    &అంశం->analysis, gui.ascii_glyphs);

	ఒకవేళ (gui.ascii_glyphs->అంకె_glyphs == (పూర్ణాంకం)పరిమాణం(ascii_chars))
	{
	    కొరకు (i = 0; i < gui.ascii_glyphs->అంకె_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_స్థానభ్రంశం += MAX(0, వెడల్పు - geom->వెడల్పు) / 2;
		geom->వెడల్పు = వెడల్పు;
	    }
	}
	లేకపోతే
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = ఖాళీ;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = ఖాళీ;
	}
    }

    g_list_foreach(అంశం_list, (GFunc)(శూన్య *)&pango_అంశం_free, ఖాళీ);
    g_list_free(అంశం_list);
    pango_attr_list_unref(attr_list);
}

    స్థిర శూన్య
get_styled_font_variants(శూన్య)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, gui.norm_font);
    ఒకవేళ (plain_font == ఖాళీ)
	తిరిగి;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_సమితి_బరువు(bold_font_desc, PANGO_బరువు_BOLD);

    bold_font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, bold_font_desc);
    ఒకవేళ (bold_font != ఖాళీ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    పూర్ణాంకం
gui_mch_init_font(char_u *font_name, పూర్ణాంకం fontset వాడని)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    పూర్ణాంకం			    వెడల్పు;

    ఒకవేళ (font_name == ఖాళీ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    ఒకవేళ (font_desc == ఖాళీ)
	తిరిగి FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_సమితి_font_description(gui.పాఠ్యం_context, font_desc);

    layout = pango_layout_new(gui.పాఠ్యం_context);
    pango_layout_సమితి_పాఠ్యం(layout, "MW", 2);
    pango_layout_get_కొలత(layout, &వెడల్పు, ఖాళీ);
    g_object_unref(layout);

    gui.char_వెడల్పు = (వెడల్పు / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    ఒకవేళ (gui.char_వెడల్పు <= 0)
	gui.char_వెడల్పు = 8;

    gui_mch_adjust_charheight();

    hl_సమితి_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_పట్టిక_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	పూర్ణాంకం im_అడ్డువరుస = gui.అడ్డువరుస < screen_అడ్డువరుసలు ? gui.అడ్డువరుస : screen_అడ్డువరుసలు - 1;
	పూర్ణాంకం im_నిలువు = gui.నిలువు < screen_Columns ? gui.నిలువు : screen_Columns - 1;
	im_సమితి_position(im_అడ్డువరుస, im_నిలువు);
    }
    తిరిగి OK;
}

    GuiFont
gui_mch_get_font(char_u *name, పూర్ణాంకం report_error)
{
    PangoFontDescription *font;

    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    font = pango_font_description_from_string((స్థిరరాశి అక్షరం *)name);
    ఒకవేళ (font == ఖాళీ)
    {
	ఒకవేళ (report_error)
	    semsg(_(e_unknown_font_str), name);
	తిరిగి ఖాళీ;
    }

    // Ensure a size is set
    ఒకవేళ (pango_font_description_get_కొలత(font) <= 0)
	pango_font_description_సమితి_కొలత(font, 10 * PANGO_SCALE);

    తిరిగి font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name వాడని)
{
    ఒకవేళ (font != NOFONT)
    {
	అక్షరం *desc = pango_font_description_గమ్యం_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	తిరిగి ret;
    }
    తిరిగి ఖాళీ;
}

    శూన్య
gui_mch_free_font(GuiFont font)
{
    ఒకవేళ (font != NOFONT)
	pango_font_description_free(font);
}

    శూన్య
gui_mch_expand_font(
    optexpand_T	*వాదనలు,
    శూన్య	*param,
    పూర్ణాంకం		(*కూడు_సరిపోలిక)(char_u *val))
{
    PangoFontFamily	**font_families = ఖాళీ;
    పూర్ణాంకం			n_families = 0;
    పూర్ణాంకం			wide = *(పూర్ణాంకం *)param;

    ఒకవేళ (వాదనలు->oe_include_orig_val && *వాదనలు->oe_opt_విలువ == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	ఒకవేళ (కూడు_సరిపోలిక((char_u *)DEFAULT_FONT) != OK)
	    తిరిగి;
    }

    pango_context_list_families(
	    gui.పాఠ్యం_context,
	    &font_families,
	    &n_families);

    కొరకు (పూర్ణాంకం i = 0; i < n_families; i++)
    {
	ఒకవేళ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    కొనసాగు;

	స్థిరరాశి అక్షరం *fam_name = pango_font_family_get_name(font_families[i]);
	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ఆదానం_conv,
		    (char_u *)fam_name, ఖాళీ);
	    ఒకవేళ (buf != ఖాళీ)
	    {
		ఒకవేళ (కూడు_సరిపోలిక(buf) != OK)
		{
		    vim_free(buf);
		    ఆపు;
		}
		vim_free(buf);
	    }
	    లేకపోతే
		ఆపు;
	}
	లేకపోతే
	{
	    ఒకవేళ (కూడు_సరిపోలిక((char_u *)fam_name) != OK)
		ఆపు;
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
    ఒకవేళ (!gui.లోపల_use)
	తిరిగి INVALCOLOR;

    ఒకవేళ (name != ఖాళీ)
	తిరిగి gui_get_color_cmn(name);

    తిరిగి INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
    తిరిగి gui_get_rgb_color_cmn(r, g, b);
}

    స్థిర GdkRGBA
color_గమ్యం_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    తిరిగి rgba;
}

    శూన్య
gui_mch_సమితి_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_గమ్యం_rgba(color);
}

    శూన్య
gui_mch_సమితి_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_గమ్యం_rgba(color);
}

    శూన్య
gui_mch_సమితి_sp_color(guicolor_T color)
{
    *gui.spcolor = color_గమ్యం_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    తిరిగి pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    శూన్య
gui_gtk4_update_కొలత(శూన్య)
{
    vim_draw_area_సమితి_కొలత(VIM_DRAW_AREA(gui.drawarea),
	    gui.అంకె_అడ్డువరుసలు, gui.అంకె_నిలువుల_సంఖ్య);
}

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    శూన్య
gui_gtk4_కూడు_multisign(
	cairo_surface_t *surf,
	పూర్ణాంకం		అడ్డువరుస,
	పూర్ణాంకం		నిలువు,
	పూర్ణాంకం		వెడల్పు,
	పూర్ణాంకం		ఎత్తు)
{
    vim_draw_area_కూడు_multisign(VIM_DRAW_AREA(gui.drawarea), surf, అడ్డువరుస, నిలువు,
	    వెడల్పు, ఎత్తు);
}
#అయితే_ముగింపు

    శూన్య
gui_mch_clear_ఖండం(పూర్ణాంకం row1, పూర్ణాంకం col1, పూర్ణాంకం row2, పూర్ణాంకం col2)
{
    vim_draw_area_clear_ఖండం(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    ఒకవేళ (gui.drawarea != ఖాళీ)
	gtk_widget_వరుస_draw(gui.drawarea);
}

    శూన్య
gui_mch_clear_all(శూన్య)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    ఒకవేళ (gui.drawarea != ఖాళీ)
	gtk_widget_వరుస_draw(gui.drawarea);
}

    శూన్య
gui_mch_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    vim_draw_area_move_ఖండం(VIM_DRAW_AREA(gui.drawarea),
	    అడ్డువరుస, అడ్డువరుస + అంకె_పంక్తులు, gui.scroll_region_bot,
	    gui.scroll_region_ఎడమ, gui.scroll_region_కుడి);
    gui_clear_ఖండం(
	    gui.scroll_region_bot - అంకె_పంక్తులు + 1, gui.scroll_region_ఎడమ,
	    gui.scroll_region_bot, gui.scroll_region_కుడి);

    gtk_widget_వరుస_draw(gui.drawarea);
}

    శూన్య
gui_mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
    vim_draw_area_move_ఖండం(VIM_DRAW_AREA(gui.drawarea),
	    అడ్డువరుస + అంకె_పంక్తులు, అడ్డువరుస, gui.scroll_region_bot - అంకె_పంక్తులు,
	    gui.scroll_region_ఎడమ, gui.scroll_region_కుడి);
    gui_clear_ఖండం(
	    అడ్డువరుస, gui.scroll_region_ఎడమ,
	    అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);

    gtk_widget_వరుస_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    శూన్య
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, పూర్ణాంకం w, పూర్ణాంకం h)
{
    gui_mch_సమితి_bg_color(bg);
    gui_mch_సమితి_fg_color(fg);
    vim_draw_area_సమితి_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_వరుస_draw(gui.drawarea);
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)Columns - 1);
    gui_mch_flush();
    ui_delay((దీర్ఘ)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (పూర్ణాంకం)అడ్డువరుసలు - 1, (పూర్ణాంకం)Columns - 1);
}

    శూన్య
gui_mch_invert_rectangle(పూర్ణాంకం r, పూర్ణాంకం c, పూర్ణాంకం nr, పూర్ణాంకం nc)
{
    vim_draw_area_invert_ఖండం(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_వరుస_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    స్థిర gboolean
కీలకం_press_event(Gtk_Event_Controller_కీలకం *controller వాడని,
	guint కీలకం_sym, guint keycode వాడని,
	GdkModifierType state, gpointer దత్తాంశం వాడని)
{
    char_u	string[32], string2[32];
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		కీలకం;
    char_u	*s, *d;

#నిర్వచితమైతే FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    ఒకవేళ (xic != ఖాళీ)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	ఒకవేళ (event != ఖాళీ && gtk_im_context_filter_keypress(xic, event))
	    తిరిగి TRUE;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MENU
    ఒకవేళ (కీలకం_sym == GDK_కీలకం_F10 && gui.menubar != ఖాళీ)
    {
	స్థిర char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	ఒకవేళ (check_map(k10, State, FALSE, TRUE, FALSE, ఖాళీ, ఖాళీ) == ఖాళీ)
	{
	    show_menubar_popover();
	    తిరిగి TRUE;
	}
    }
#అయితే_ముగింపు

    len = keyval_గమ్యం_string(కీలకం_sym, string2);

    ఒకవేళ (len > 1 && ఆదానం_conv.vc_type != CONV_NONE)
	len = convert_ఆదానం(string2, len, పరిమాణం(string2));

    s = string2;
    d = string;
    కొరకు (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	ఒకవేళ (d[-1] == CSI && d + 2 < string + పరిమాణం(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (పూర్ణాంకం)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    ఒకవేళ (కీలకం_sym == GDK_కీలకం_ISO_ఎడమ_Tab)
    {
	కీలకం_sym = GDK_కీలకం_Tab;
	state |= GDK_జరుపు_ముసుగు;
    }

#నిర్వచితమైతే FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ఒకవేళ ((state & GDK_ALT_ముసుగు)
	    && gui.menu_is_చురుకైన
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	తిరిగి FALSE;
#అయితే_ముగింపు

    // Check for special keys
    ఒకవేళ (len == 0 || len == 1)
    {
	కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != 0; i++)
	{
	    ఒకవేళ (special_కీలకాలు[i].కీలకం_sym == కీలకం_sym)
	    {
		string[0] = CSI;
		string[1] = special_కీలకాలు[i].code0;
		string[2] = special_కీలకాలు[i].code1;
		len = -3;
		ఆపు;
	    }
	}
    }

    ఒకవేళ (len == 0)
	తిరిగి TRUE;

    ఒకవేళ (len == -3)
	కీలకం = గమ్యం_SPECIAL(string[1], string[2]);
    లేకపోతే
    {
	string[len] = NUL;
	కీలకం = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    కీలకం = simplify_కీలకం(కీలకం, &modifiers);
    ఒకవేళ (కీలకం == CSI)
	కీలకం = K_CSI;
    ఒకవేళ (IS_SPECIAL(కీలకం))
    {
	string[0] = CSI;
	string[1] = K_రెండవ(కీలకం);
	string[2] = K_మూడవ(కీలకం);
	len = 3;
    }
    లేకపోతే
    {
	కీలకం = may_adjust_కీలకం_for_ctrl(modifiers, కీలకం);
	modifiers = may_remove_జరుపు_modifier(modifiers, కీలకం);
	len = mb_char2bytes(కీలకం, string);
    }

    ఒకవేళ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	కూడు_గమ్యం_ఆదానం_buf(string2, 3);
    }

    {
	పూర్ణాంకం int_ch = check_for_అంతరాయం(కీలకం, modifiers);
	ఒకవేళ (int_ch != NUL)
	{
	    trash_ఆదానం_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    కూడు_గమ్యం_ఆదానం_buf(string, len);

    ఒకవేళ (p_mh)
	gui_mch_mousehide(TRUE);

    తిరిగి TRUE;
}

    స్థిర శూన్య
కీలకం_release_event(Gtk_Event_Controller_కీలకం *controller వాడని,
	guint keyval వాడని, guint keycode వాడని,
	GdkModifierType state వాడని, gpointer దత్తాంశం వాడని)
{
}

స్థిర పూర్ణాంకం mouse_timed_బహిర్గతం = TRUE;
స్థిర guint mouse_click_timer = 0;

    స్థిర timeout_cb_type
mouse_click_timer_cb(gpointer దత్తాంశం)
{
    *(పూర్ణాంకం *)దత్తాంశం = TRUE;
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
modifiers_gdk2mouse(guint state)
{
    పూర్ణాంకం modifiers = 0;

    ఒకవేళ (state & GDK_జరుపు_ముసుగు)
	modifiers |= MOUSE_జరుపు;
    ఒకవేళ (state & GDK_CONTROL_ముసుగు)
	modifiers |= MOUSE_CTRL;
    ఒకవేళ (state & GDK_ALT_ముసుగు)
	modifiers |= MOUSE_ALT;

    తిరిగి modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#నిర్వచించు ANY_BUTTON_ముసుగు (GDK_BUTTON1_ముసుగు | GDK_BUTTON2_ముసుగు \
	| GDK_BUTTON3_ముసుగు | GDK_BUTTON4_ముసుగు | GDK_BUTTON5_ముసుగు)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
స్థిర పూర్ణాంకం mouse_pressed_button = -1;

స్థిర guint motion_repeat_timer = 0;
స్థిర gboolean motion_repeat_స్థానభ్రంశం = FALSE;
స్థిర పూర్ణాంకం mouse_press_x = -1, mouse_press_y = -1;

    స్థిర శూన్య
button_press_event(GtkGestureClick *gesture, పూర్ణాంకం n_press వాడని,
	ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం button;
    పూర్ణాంకం repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    ఒకవేళ (!mouse_timed_బహిర్గతం && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_బహిర్గతం = FALSE;
    mouse_click_timer = timeout_కూడు(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_బహిర్గతం);

    ఎంపిక (btn)
    {
	స్థితి 1: button = MOUSE_ఎడమ; ఆపు;
	స్థితి 2: button = MOUSE_మధ్య; ఆపు;
	స్థితి 3: button = MOUSE_కుడి; ఆపు;
	స్థితి 8: button = MOUSE_X1; ఆపు;
	స్థితి 9: button = MOUSE_X2; ఆపు;
	అప్రమేయ: తిరిగి;
    }

    mouse_press_x = (పూర్ణాంకం)x;
    mouse_press_y = (పూర్ణాంకం)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (పూర్ణాంకం)x, (పూర్ణాంకం)y, repeated_click, vim_modifiers);
}

    స్థిర శూన్య
button_release_event(GtkGestureClick *gesture, పూర్ణాంకం n_press వాడని,
	ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    ఒకవేళ (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (పూర్ణాంకం)x, (పూర్ణాంకం)y, FALSE, vim_modifiers);
}

స్థిర ద్విగుణ prev_mouse_x = -1.0;
స్థిర ద్విగుణ prev_mouse_y = -1.0;

స్థిర GdkModifierType cur_state = 0;

    స్థిర timeout_cb_type
mouse_repeat_timer_cb(gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం x, y;

    ఒకవేళ (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	తిరిగి G_ఆధారం_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ఒకవేళ (vim_వాడిన_లోపల_ఆదానం_buf() > 10)
	తిరిగి G_ఆధారం_CONTINUE;

    x = (పూర్ణాంకం)prev_mouse_x;
    y = (పూర్ణాంకం)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    ఒకవేళ (motion_repeat_స్థానభ్రంశం)
	x += gui.char_వెడల్పు;
    motion_repeat_స్థానభ్రంశం = !motion_repeat_స్థానభ్రంశం;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    తిరిగి G_ఆధారం_CONTINUE;
}

    స్థిర శూన్య
motion_notify_event(GtkEventControllerMotion *controller వాడని,
	ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం వాడని)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    ఒకవేళ (mouse_pressed_button >= 0 && event != ఖాళీ
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_ముసుగు))
    {
	ఒకవేళ (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    ఒకవేళ (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	పూర్ణాంకం		w, h;

	ఒకవేళ (event != ఖాళీ)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (పూర్ణాంకం)x, (పూర్ణాంకం)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_వెడల్పు(gui.drawarea);
	h = gtk_widget_get_ఎత్తు(gui.drawarea);

	ఒకవేళ (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	ఒకవేళ (x < 0 || y < 0 || x >= w || y >= h)
	{
	    పూర్ణాంకం	dx, dy;
	    పూర్ణాంకం offshoot;
	    పూర్ణాంకం	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    ఒకవేళ (offshoot > 127)
		delay = 5;
	    లేకపోతే
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_కూడు(delay,
		    mouse_repeat_timer_cb, ఖాళీ);
	}
	లేకపోతే
	    motion_repeat_timer = 0;
    }
    లేకపోతే
	gui_mouse_moved((పూర్ణాంకం)x, (పూర్ణాంకం)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    ఒకవేళ (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (fabs(prev_mouse_x - x) > 0.05
		    && fabs(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    స్థిర శూన్య
enter_notify_event(GtkEventControllerMotion *controller వాడని,
	ద్విగుణ x, ద్విగుణ y, gpointer దత్తాంశం వాడని)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    స్థిర gboolean
scroll_event(GtkEventControllerScroll *controller వాడని,
	ద్విగుణ dx వాడని, ద్విగుణ dy, gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం button;
    int_u vim_modifiers;
    పూర్ణాంకం x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    ఒకవేళ (event == ఖాళీ)
	తిరిగి FALSE;
    state = gdk_event_get_modifier_state(event);

    ఒకవేళ (dy < 0)
	button = MOUSE_4;	// scroll up
    లేకపోతే ఒకవేళ (dy > 0)
	button = MOUSE_5;	// scroll down
    లేకపోతే ఒకవేళ (dx < 0)
	button = MOUSE_7;	// scroll left
    లేకపోతే ఒకవేళ (dx > 0)
	button = MOUSE_6;	// scroll right
    లేకపోతే
	తిరిగి FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    తిరిగి TRUE;
}

    స్థిర శూన్య
focus_లోపల_event(GtkEventControllerFocus *controller వాడని,
	gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (gtk_కిటికీ_is_చురుకైన(GTK_కిటికీ(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	ఒకవేళ (blink_state == BLINK_NONE)
	    gui_mch_ప్రారంభం_blink();
    }
}

    స్థిర శూన్య
focus_బహిర్గతం_event(GtkEventControllerFocus *controller వాడని,
	gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (!gtk_కిటికీ_is_చురుకైన(GTK_కిటికీ(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	ఒకవేళ (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#ఒకవేళ defined(FEAT_IMAGE)
    స్థిర శూన్య
scale_గుణకం_cb(GdkSurface  *surface,
	GParamSpec	    *pspec వాడని,
	శూన్య		    *udata వాడని)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#అయితే_ముగింపు

    స్థిర శూన్య
drawarea_realize_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం వాడని)
{
#ఒకవేళ defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_గుణకం_cb), ఖాళీ);
    redraw_all_later(UPD_VALID);
#అయితే_ముగింపు
    gui_mch_new_colors();
}

    స్థిర శూన్య
drawarea_unrealize_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం వాడని)
{
#నిర్వచితమైతే FEAT_XIM
    im_shutdown();
#అయితే_ముగింపు
}

రకనిర్వచనం గణన
{
    READ_దత్తాంశం_CLIP,	    // Clipboard data
    READ_దత్తాంశం_DROP,	    // URI or text DND data
    READ_దత్తాంశం_DROP_దత్తాంశం	    // HTML DND data
} Read_దత్తాంశం_Type;

#నిర్వచించు READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
రకనిర్వచనం నిర్మాణం
{
    Read_దత్తాంశం_Type    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    అక్షరం	    *buf; // NULL when not needed
} Read_దత్తాంశం;

#నిర్వచితమైతే FEAT_DND
రకనిర్వచనం నిర్మాణం
{
    Read_దత్తాంశం	rd;
    GdkDrop	*drop;
    పూర్ణాంకం		x;
    పూర్ణాంకం		y;
} Drop_Read_దత్తాంశం;
#అయితే_ముగింపు

రకనిర్వచనం నిర్మాణం
{
    Read_దత్తాంశం	rd;
    Clipboard_T *cbd;
    అక్షరం	*mime_type;
} Clip_Read_దత్తాంశం;

    స్థిర Read_దత్తాంశం *
read_దత్తాంశం_new(Read_దత్తాంశం_Type type, size_t sz)
{
    Read_దత్తాంశం *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    ఒకవేళ (type == READ_దత్తాంశం_CLIP || type == READ_దత్తాంశం_DROP_దత్తాంశం)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    తిరిగి rd;
}

    స్థిర శూన్య
read_దత్తాంశం_free(Read_దత్తాంశం *rd)
{
    g_cancellable_cancel(rd->cancel);

    ఒకవేళ (rd->type == READ_దత్తాంశం_CLIP)
	g_free(((Clip_Read_దత్తాంశం *)rd)->mime_type);
    లేకపోతే ఒకవేళ (rd->type == READ_దత్తాంశం_DROP || rd->type == READ_దత్తాంశం_DROP_దత్తాంశం)
	g_object_unref(((Drop_Read_దత్తాంశం *)rd)->drop);
    g_object_unref(rd->cancel);
    ఒకవేళ (rd->arr != ఖాళీ)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#నిర్వచితమైతే FEAT_DND
స్థిర శూన్య drop_read_పాఠ్యం(GdkDrop *drop, char_u *పాఠ్యం);
#అయితే_ముగింపు

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    స్థిర శూన్య
read_దత్తాంశం_ఆదానం_cb(
	GInput_ప్రవాహం	*ప్రవాహం,
	GAsync_ఫలితం	*ఫలితం,
	Read_దత్తాంశం	*rd)
{
    ssize_t r = g_ఆదానం_ప్రవాహం_read_ముగింపు(ప్రవాహం, ఫలితం, ఖాళీ);

    ఒకవేళ (r == -1)
    {
	// Error occurred
	Drop_Read_దత్తాంశం *drd = (Drop_Read_దత్తాంశం *)rd;

	ఒకవేళ (rd->type == READ_దత్తాంశం_DROP_దత్తాంశం)
	    gdk_drop_ముగింపు(drd->drop, 0);
    }
    లేకపోతే ఒకవేళ (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	ఒకవేళ (rd->type == READ_దత్తాంశం_CLIP)
	{
	    Clip_Read_దత్తాంశం    *crd = (Clip_Read_దత్తాంశం *)rd;
	    దీర్ఘ	    len;
	    char_u	    *actual, *final;
	    పూర్ణాంకం		    motion_type = MAUTO;
	    char_u	    *tofree = ఖాళీ;

	    len = (దీర్ఘ)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ఖాళీ;

	    ఒకవేళ (clip_convert_దత్తాంశం(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#నిర్వచితమైతే FEAT_DND
	లేకపోతే ఒకవేళ (rd->type == READ_దత్తాంశం_DROP_దత్తాంశం)
	{
	    Drop_Read_దత్తాంశం    *drd = (Drop_Read_దత్తాంశం *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (స్థిరరాశి uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = ఖాళీ;
	    drop_read_పాఠ్యం(drd->drop, str);
	    g_free(str);
	    gdk_drop_ముగింపు(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#అయితే_ముగింపు
    }
    లేకపోతే
    {
	// Continue reading
	g_byte_array_append(rd->arr, (స్థిరరాశి uint8_t *)rd->buf, r);
	g_ఆదానం_ప్రవాహం_read_async(ప్రవాహం, rd->buf, READDATA_BUFSIZE,
		G_ప్రాధాన్యత_ఎగువ,
		rd->cancel, (GAsyncReadyCallback)read_దత్తాంశం_ఆదానం_cb, rd);
	తిరిగి;
    }

    read_దత్తాంశం_free(rd);
    g_object_unref(ప్రవాహం);
}

#నిర్వచితమైతే FEAT_DND
/*
 * Set up for receiving DND items.
 */
    శూన్య
gui_gtk_సమితి_dnd_targets(శూన్య)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_కూడు_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_కూడు_gtype(builder, G_TYPE_STRING);
    ఒకవేళ (clip_html)
	gdk_content_formats_builder_కూడు_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_గమ్యం_formats(builder);
    gtk_drop_లక్ష్యం_async_సమితి_formats(gui.drop_లక్ష్యం, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    స్థిర శూన్య
drop_read_పాఠ్యం(GdkDrop *drop వాడని, char_u *పాఠ్యం)
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

    ఒకవేళ (పాఠ్యం == ఖాళీ || *పాఠ్యం == NUL || !query_సూచిక_pos(ఖాళీ, ఖాళీ, &state))
	తిరిగి;

    dnd_yank_drag_దత్తాంశం((char_u *)పాఠ్యం, (దీర్ఘ)STRLEN(పాఠ్యం));

    dropkey[2] = modifiers_gdk2vim(state);
    ఒకవేళ (dropkey[2] != 0)
	కూడు_గమ్యం_ఆదానం_buf(dropkey, (పూర్ణాంకం)పరిమాణం(dropkey));
    లేకపోతే
	కూడు_గమ్యం_ఆదానం_buf(dropkey + 3, (పూర్ణాంకం)(పరిమాణం(dropkey) - 3));
}

    స్థిర శూన్య
drop_read_విలువ_cb(GdkDrop *drop, GAsync_ఫలితం *ఫలితం, Drop_Read_దత్తాంశం *drd)
{
    స్థిరరాశి GValue    *విలువ = gdk_drop_read_విలువ_ముగింపు(drop, ఫలితం, ఖాళీ);
    gboolean	    success = FALSE;

    ఒకవేళ (విలువ == ఖాళీ)
	వెళ్ళు నిష్క్రమించు;

    ఒకవేళ (G_విలువ_HOLDS(విలువ, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_విలువ_get_boxed(విలువ);
	పూర్ణాంకం	nfiles = g_slist_నిడివి(files);
	char_u	**fnames;
	పూర్ణాంకం	i;

	ఒకవేళ (nfiles <= 0)
	    వెళ్ళు నిష్క్రమించు;

	fnames = ALLOC_MULT(char_u *, nfiles);
	ఒకవేళ (fnames == ఖాళీ)
	    వెళ్ళు నిష్క్రమించు;

	i = 0;
	కొరకు (GSList *l = files; l != ఖాళీ; l = l->తదుపరి)
	{
	    GFile *file = l->దత్తాంశం;
	    అక్షరం *మార్గం = g_file_get_మార్గం(file);
	    ఒకవేళ (మార్గం != ఖాళీ)
		fnames[i++] = vim_strsave((char_u *)మార్గం);
	    g_free(మార్గం);
	}
	nfiles = i;

	ఒకవేళ (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	లేకపోతే
	    vim_free(fnames);
	success = TRUE;
    }
    లేకపోతే ఒకవేళ (G_విలువ_HOLDS(విలువ, G_TYPE_STRING))
    {
	స్థిరరాశి అక్షరం *పాఠ్యం = g_విలువ_get_string(విలువ);

	ఒకవేళ (పాఠ్యం == ఖాళీ || *పాఠ్యం == NUL)
	    వెళ్ళు నిష్క్రమించు;

	drop_read_పాఠ్యం(drop, (char_u *)పాఠ్యం);
	success = TRUE;
    }

నిష్క్రమించు:
    gdk_drop_ముగింపు(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_దత్తాంశం_free(&drd->rd);
}

    స్థిర శూన్య
drop_read_cb(GdkDrop *drop, GAsync_ఫలితం *ఫలితం, Drop_Read_దత్తాంశం *drd)
{
    GInput_ప్రవాహం    *లోపల_ప్రవాహం;
    స్థిరరాశి అక్షరం	    *m;

    లోపల_ప్రవాహం = gdk_drop_read_ముగింపు(drop, ఫలితం, &m, ఖాళీ);
    ఒకవేళ (లోపల_ప్రవాహం == ఖాళీ || STRCMP(m, "text/html") != 0)
    {
	read_దత్తాంశం_free(&drd->rd);
	gdk_drop_ముగింపు(drop, 0);
	తిరిగి;
    }

    ప్రతిజ్ఞా(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_ఆదానం_ప్రవాహం_read_async(లోపల_ప్రవాహం, drd->rd.buf, READDATA_BUFSIZE,
	    G_ప్రాధాన్యత_ఎగువ,
	    drd->rd.cancel, (GAsyncReadyCallback)read_దత్తాంశం_ఆదానం_cb, drd);
}

// GCancellable of current DND operation, else NULL.
స్థిర GCancellable *dnd_cancel = ఖాళీ;
స్థిర guint dnd_timeout_id = 0;

    స్థిర gboolean
drop_timeout_cb(శూన్య *udata వాడని)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    తిరిగి G_ఆధారం_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    స్థిర gboolean
drop_cb(
	Gtk_Drop_లక్ష్యం_Async  *లక్ష్యం వాడని,
	GdkDrop		    *drop,
	ద్విగుణ		    x,
	ద్విగుణ		    y,
	శూన్య		    *udata వాడని)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    Drop_Read_దత్తాంశం	*drd = ఖాళీ;

    ఒకవేళ (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (Drop_Read_దత్తాంశం *)read_దత్తాంశం_new(READ_దత్తాంశం_DROP, పరిమాణం(*drd));
	gdk_drop_read_విలువ_async(drop, GDK_TYPE_FILE_LIST, G_ప్రాధాన్యత_ఎగువ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_విలువ_cb, drd);
    }
    లేకపోతే ఒకవేళ (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	స్థిర స్థిరరాశి అక్షరం *m[] = {"text/html", ఖాళీ};

	drd = (Drop_Read_దత్తాంశం *)read_దత్తాంశం_new(READ_దత్తాంశం_DROP_దత్తాంశం, పరిమాణం(*drd));
	gdk_drop_read_async(drop, m, G_ప్రాధాన్యత_ఎగువ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    లేకపోతే ఒకవేళ (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (Drop_Read_దత్తాంశం *)read_దత్తాంశం_new(READ_దత్తాంశం_DROP, పరిమాణం(*drd));
	gdk_drop_read_విలువ_async(drop, G_TYPE_STRING, G_ప్రాధాన్యత_ఎగువ,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_విలువ_cb, drd);
    }

    ఒకవేళ (dnd_cancel != ఖాళీ)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    ఒకవేళ (drd != ఖాళీ)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (పూర్ణాంకం)x;
	drd->y = (పూర్ణాంకం)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_కూడు(3000, drop_timeout_cb, ఖాళీ);

	తిరిగి TRUE;
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు

    స్థిర శూన్య
mainwin_destroy_cb(GObject *object వాడని, gpointer దత్తాంశం వాడని)
{
    gui.mainwin = ఖాళీ;
    gui.drawarea = ఖాళీ;
    ఒకవేళ (!exiting)
	gui_shell_closed();
}

    స్థిర gboolean
delete_event_cb(Gtk_కిటికీ *కిటికీ_2 వాడని, gpointer దత్తాంశం వాడని)
{
    gui_shell_closed();
    తిరిగి TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    స్థిర timeout_cb_type
ఆదానం_timer_cb(gpointer దత్తాంశం)
{
    పూర్ణాంకం *timed_బహిర్గతం = (పూర్ణాంకం *)దత్తాంశం;

    *timed_బహిర్గతం = TRUE;
    తిరిగి FALSE;	// don't call me again
}

    శూన్య
gui_mch_update(శూన్య)
{
    పూర్ణాంకం cnt = 0;

    అయితే (g_main_context_pending(ఖాళీ) && !vim_is_ఆదానం_buf_నిండిన()
								&& ++cnt < 100)
	g_main_context_iteration(ఖాళీ, TRUE);
}

#నిర్వచితమైతే FEAT_పని_CHANNEL
    స్థిర timeout_cb_type
channel_poll_cb(gpointer దత్తాంశం వాడని)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    తిరిగి TRUE; // Keep repeating
}
#అయితే_ముగింపు

    పూర్ణాంకం
gui_mch_wait_for_chars(దీర్ఘ wtime)
{
    పూర్ణాంకం		focus;
    guint	timer;
    స్థిర పూర్ణాంకం	timed_బహిర్గతం;
    పూర్ణాంకం		retval = FAIL;
#నిర్వచితమైతే FEAT_పని_CHANNEL
    guint	channel_timer = 0;
#అయితే_ముగింపు

    timed_బహిర్గతం = FALSE;

    ఒకవేళ (wtime >= 0)
	timer = timeout_కూడు(wtime == 0 ? 1L : wtime,
						   ఆదానం_timer_cb, &timed_బహిర్గతం);
    లేకపోతే
	timer = 0;

#నిర్వచితమైతే FEAT_పని_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ఒకవేళ (channel_any_keep_open())
	channel_timer = timeout_కూడు(20, channel_poll_cb, ఖాళీ);
#అయితే_ముగింపు

    focus = gui.లోపల_focus;

    చేయి
    {
	// Stop or start blinking when focus changes
	ఒకవేళ (gui.లోపల_focus != focus)
	{
	    ఒకవేళ (gui.లోపల_focus)
		gui_mch_ప్రారంభం_blink();
	    లేకపోతే
		gui_mch_stop_blink(TRUE);
	    focus = gui.లోపల_focus;
	}

#నిర్వచితమైతే సందేశం_వరుస
# ifdef FEAT_TIMERS
	did_కూడు_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ఒకవేళ (did_కూడు_timer)
	    వెళ్ళు theend;
# endif
#అయితే_ముగింపు

	ఒకవేళ (gui.mainwin == ఖాళీ)
	    వెళ్ళు theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	ఒకవేళ (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    వెళ్ళు theend;
	}

	ఒకవేళ (!ఆదానం_అందుబాటు())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(ఖాళీ, TRUE);
	    --gtk4_main_loop_level;
	}

	ఒకవేళ (ఆదానం_అందుబాటు())
	{
	    retval = OK;
	    వెళ్ళు theend;
	}
    } అయితే (wtime < 0 || !timed_బహిర్గతం);

    gui_mch_update();

theend:
    ఒకవేళ (timer != 0 && !timed_బహిర్గతం)
	timeout_remove(timer);
#నిర్వచితమైతే FEAT_పని_CHANNEL
    ఒకవేళ (channel_timer != 0)
	timeout_remove(channel_timer);
#అయితే_ముగింపు

    తిరిగి retval;
}

    శూన్య
gui_mch_flush(శూన్య)
{
    // Ensure the offscreen surface content gets painted to the widget.
    ఒకవేళ (gui.drawarea != ఖాళీ)
	gtk_widget_వరుస_draw(gui.drawarea);
    ఒకవేళ (gui.mainwin != ఖాళీ && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    శూన్య
gui_mch_beep(శూన్య)
{
    GdkDisplay *display;

    ఒకవేళ (gui.mainwin != ఖాళీ && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	ఒకవేళ (display != ఖాళీ)
	    gdk_display_beep(display);
    }
}

    శూన్య *
gui_mch_get_display(శూన్య)
{
    ఒకవేళ (gui.mainwin != ఖాళీ && gtk_widget_get_display(gui.mainwin))
	తిరిగి gtk_widget_get_display(gui.mainwin);
    తిరిగి ఖాళీ;
}

    శూన్య
gui_mch_iconify(శూన్య)
{
    gtk_కిటికీ_minimize(GTK_కిటికీ(gui.mainwin));
}

    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    gtk_కిటికీ_present(GTK_కిటికీ(gui.mainwin));
}

    స్థిర పూర్ణాంకం
query_సూచిక_pos(పూర్ణాంకం *x, పూర్ణాంకం *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *సూచిక;
    ద్విగుణ	     sx, sy, nx, ny;
    graphene_బిందువు_t src, dst;

    ఒకవేళ (gui.drawarea == ఖాళీ)
	తిరిగి FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    ఒకవేళ (native == ఖాళీ)
	తిరిగి FALSE;
    surface = gtk_native_get_surface(native);
    ఒకవేళ (surface == ఖాళీ)
	తిరిగి FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    ఒకవేళ (display == ఖాళీ)
	తిరిగి FALSE;
    seat = gdk_display_get_default_seat(display);
    ఒకవేళ (seat == ఖాళీ)
	తిరిగి FALSE;
    సూచిక = gdk_seat_get_సూచిక(seat);
    ఒకవేళ (సూచిక == ఖాళీ)
	తిరిగి FALSE;

    ఒకవేళ (!gdk_surface_get_device_position(surface, సూచిక, &sx, &sy, state))
	తిరిగి FALSE;

    ఒకవేళ (x != ఖాళీ && y != ఖాళీ)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (దశాంశం)(sx - nx);
	src.y = (దశాంశం)(sy - ny);
	ఒకవేళ (!gtk_widget_compute_బిందువు(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    తిరిగి FALSE;

	*x = (పూర్ణాంకం)dst.x;
	*y = (పూర్ణాంకం)dst.y;
    }
    తిరిగి TRUE;
}

    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    ఒకవేళ (!query_సూచిక_pos(x, y, ఖాళీ))
    {
	*x = 0;
	*y = 0;
    }
}

    శూన్య
gui_mch_setmouse(పూర్ణాంకం x వాడని, పూర్ణాంకం y వాడని)
{
    // GTK4/Wayland: cannot warp pointer
}

    శూన్య
gui_mch_mousehide(పూర్ణాంకం hide)
{
    ఒకవేళ (gui.సూచిక_hidden == hide)
	తిరిగి;

    gui.సూచిక_hidden = hide;
    ఒకవేళ (gui.blank_సూచిక != ఖాళీ)
    {
	ఒకవేళ (hide)
	    gtk_widget_సమితి_cursor(gui.drawarea, gui.blank_సూచిక);
	లేకపోతే
#నిర్వచితమైతే FEAT_MOUSESHAPE
	    mch_సమితి_mouse_shape(చివరి_shape);
#లేకపోతే
	    gtk_widget_సమితి_cursor(gui.drawarea, ఖాళీ);
#అయితే_ముగింపు
    }
}

    పూర్ణాంకం
gui_mch_haskey(char_u *name)
{
    పూర్ణాంకం i;

    కొరకు (i = 0; special_కీలకాలు[i].కీలకం_sym != 0; i++)
	ఒకవేళ (name[0] == special_కీలకాలు[i].code0
		&& name[1] == special_కీలకాలు[i].code1)
	    తిరిగి OK;
    తిరిగి FAIL;
}

    శూన్య
gui_mch_forked(శూన్య)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    స్థిర శూన్య
gui_gtk4_update_scrollbar_కొలత(శూన్య)
{
    win_T	*wp;
    పూర్ణాంకం		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_ఎడమ].id;

	ఒకవేళ (sbar == ఖాళీ || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_ఎడమ]
		    && wp->w_scrollbars[SBAR_కుడి].id != ఖాళీ))
	    sbar = wp->w_scrollbars[SBAR_కుడి].id;

	ఒకవేళ (sbar != ఖాళీ && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    పూర్ణాంకం		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_కొలత(sbar, &min, &nat);
	    sw = MAX(min.వెడల్పు, nat.వెడల్పు);
	    ఒకవేళ (sw > 0)
	    {
		w = sw;
		ఆపు;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    ఒకవేళ (sbar != ఖాళీ && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	పూర్ణాంకం		    sh;

	gtk_widget_get_preferred_కొలత(sbar, &min, &nat);
	sh = MAX(min.ఎత్తు, nat.ఎత్తు);

	ఒకవేళ (sh > 0)
	    h = sh;
    }

    ఒకవేళ (w != -1)
	gui.scrollbar_వెడల్పు = w;
    ఒకవేళ (h != -1)
	gui.scrollbar_ఎత్తు = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    శూన్య
gui_mch_enable_scrollbar(scrollbar_T *sb, పూర్ణాంకం flag)
{
    ఒకవేళ (sb->id != ఖాళీ)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_సమితి_visible(sb->id, flag);

	ఒకవేళ (!was_visible && flag)
	    gui_gtk4_update_scrollbar_కొలత();
    }
}

#ఒకవేళ defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    శూన్య
gui_mch_menu_grey(vimmenu_T *menu, పూర్ణాంకం grey)
{
    ఒకవేళ (menu->id == ఖాళీ)
	తిరిగి;
    gtk_widget_సమితి_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    శూన్య
gui_mch_menu_hidden(vimmenu_T *menu, పూర్ణాంకం hidden)
{
    ఒకవేళ (menu->id == ఖాళీ)
	తిరిగి;
    gtk_widget_సమితి_visible(menu->id, !hidden);
    gui_mch_update();
}

    శూన్య
gui_mch_draw_menubar(శూన్య)
{
    // Just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#అయితే_ముగింపు

/*
 * ============================================================
 * Tabline
 * ============================================================
 */

#నిర్వచితమైతే FEAT_GUI_TABLINE
    శూన్య
gui_mch_show_tabline(పూర్ణాంకం showit)
{
    ఒకవేళ (gui.tabline == ఖాళీ)
	తిరిగి;
    gtk_widget_సమితి_visible(gui.tabline, showit);
    gui_mch_update();
}

    పూర్ణాంకం
gui_mch_showing_tabline(శూన్య)
{
    తిరిగి gui.tabline != ఖాళీ && gtk_widget_get_visible(gui.tabline);
}

స్థిర పూర్ణాంకం ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
స్థిర పూర్ణాంకం tabpage_hover = 0;

    స్థిర శూన్య
tabpage_enter_cb(
	GtkEventController  *controller వాడని,
	ద్విగుణ		    x వాడని,
	ద్విగుణ		    y వాడని,
	శూన్య		    *udata)
{
    tabpage_hover = GPOINTER_గమ్యం_INT(udata);
}

    స్థిర శూన్య
tabline_enter_cb(
	GtkEventController  *controller వాడని,
	ద్విగుణ		    x వాడని,
	ద్విగుణ		    y వాడని,
	శూన్య		    *udata వాడని)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    స్థిర శూన్య
tabline_కూడు_css(శూన్య)
{
    GtkCssProvider  *provider;
    స్థిర స్థిరరాశి అక్షరం *css =
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
# ఒకవేళ GTK_CHECK_సంస్కరణ(4, 12, 0)
    gtk_css_provider_ఎక్కించు_from_string(provider, css);
# లేకపోతే
    gtk_css_provider_ఎక్కించు_from_దత్తాంశం(provider, css, -1);
# endif
    gtk_style_context_కూడు_provider_for_display(
	    gtk_widget_get_display(gui.tabline), GTK_STYLE_PROVIDER(provider),
	    GTK_STYLE_PROVIDER_ప్రాధాన్యత_APPLICATION);
    g_object_unref(provider);
}

/*
 * Handle clicking the close button of a tab.
 */
    స్థిర శూన్య
tab_close_clicked_cb(GtkButton *button వాడని, gpointer దత్తాంశం)
{
    పూర్ణాంకం idx = gtk_notebook_పుట_అంకె(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(దత్తాంశం));

    ఒకవేళ (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    శూన్య
gui_mch_update_tabline(శూన్య)
{
    GtkWidget	*పుట;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    పూర్ణాంకం		nr = 0;
    పూర్ణాంకం		tab_అంకె;
    పూర్ణాంకం		curtabidx = 0;
    char_u	*labeltext;

    ఒకవేళ (gui.tabline == ఖాళీ)
	తిరిగి;

    ignore_tabline_evt = TRUE;

    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, ++nr)
    {
	Gtk_Notebook_పుట	    *పుట_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	ఒకవేళ (tp == curtab)
	    curtabidx = nr;

	tab_అంకె = nr + 1;

	పుట = gtk_notebook_get_nth_పుట(GTK_NOTEBOOK(gui.tabline), nr);
	ఒకవేళ (పుట == ఖాళీ)
	{
	    GtkWidget	*spacer;
	    GtkWidget	*close_button;

	    పుట = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_widget_సమితి_visible(పుట, TRUE);
	    event_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_సమితి_visible(event_box, TRUE);

	    spacer = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
	    gtk_widget_కూడు_css_class(spacer, "vim-tab-spacer");
	    gtk_box_append(GTK_BOX(event_box), spacer);

	    label = gtk_label_new("-Empty-");
	    gtk_widget_సమితి_hexpand(label, TRUE);
	    gtk_label_సమితి_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_చివర);
	    gtk_box_append(GTK_BOX(event_box), label);
	    gtk_widget_సమితి_visible(label, TRUE);
	    g_object_సమితి_దత్తాంశం(G_OBJECT(event_box), "label", label);

	    close_button = gtk_button_new_from_icon_name("window-close-symbolic");
	    gtk_widget_కూడు_css_class(close_button, "flat");
	    gtk_widget_కూడు_css_class(close_button, "circular");
	    gtk_widget_సమితి_valign(close_button, GTK_ALIGN_CENTER);
	    gtk_widget_సమితి_focus_on_click(close_button, FALSE);
	    gtk_widget_సమితి_tooltip_పాఠ్యం(close_button, _("Close Tab"));
	    g_signal_connect(close_button, "clicked",
		    G_CALLBACK(tab_close_clicked_cb), పుట);
	    gtk_box_append(GTK_BOX(event_box), close_button);

	    gtk_notebook_చొప్పించు_పుట(GTK_NOTEBOOK(gui.tabline),
		    పుట, event_box, nr);
	    gtk_notebook_సమితి_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    పుట, TRUE);
	    g_object_సమితి(gtk_notebook_get_పుట(GTK_NOTEBOOK(gui.tabline), పుట),
		    "tab-expand", TRUE, ఖాళీ);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	పుట_widget = gtk_notebook_get_పుట(GTK_NOTEBOOK(gui.tabline), పుట);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_గమ్యం_సూచిక(tab_అంకె));
	gtk_event_controller_సమితి_ప్రసరణ_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(పుట_widget, "tab", &tab_widget, ఖాళీ);
	gtk_widget_కూడు_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), పుట);
	label = g_object_get_దత్తాంశం(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_గమ్యం_UTF8(NameBuff);
	ఒకవేళ (label != ఖాళీ && GTK_IS_LABEL(label))
	    gtk_label_సమితి_పాఠ్యం(GTK_LABEL(label), (స్థిరరాశి అక్షరం *)labeltext);
	CONVERT_గమ్యం_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_గమ్యం_UTF8(NameBuff);
	gtk_widget_సమితి_tooltip_పాఠ్యం(event_box, (స్థిరరాశి gchar *)labeltext);
	CONVERT_గమ్యం_UTF8_FREE(labeltext);
    }

    అయితే (gtk_notebook_get_nth_పుట(GTK_NOTEBOOK(gui.tabline), nr) != ఖాళీ)
	gtk_notebook_remove_పుట(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    ఒకవేళ (nr == 1)
	gtk_widget_కూడు_css_class(gui.tabline, "single-tab");
    లేకపోతే
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    ఒకవేళ (gtk_notebook_get_current_పుట(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_సమితి_current_పుట(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    శూన్య
gui_mch_సమితి_curtab(పూర్ణాంకం nr)
{
    ఒకవేళ (gui.tabline != ఖాళీ)
	gtk_notebook_సమితి_current_పుట(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    స్థిర శూన్య
on_select_tab(
	GtkNotebook	*notebook వాడని,
	gpointer	*పుట వాడని,
	gint		idx,
	gpointer	దత్తాంశం వాడని)
{
    ఒకవేళ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    స్థిర శూన్య
on_tab_reordered(
	GtkNotebook	*notebook వాడని,
	gpointer	*పుట వాడని,
	gint		idx,
	gpointer	దత్తాంశం వాడని)
{
    ఒకవేళ (ignore_tabline_evt)
	తిరిగి;

    ఒకవేళ ((tabpage_సూచీ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    లేకపోతే
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    స్థిర శూన్య
tabline_menu_event_cb(Vim_Menu_అంశం *అంశం వాడని, Vim_Menu_అంశం_Event event, శూన్య *udata)
{
    ఒకవేళ (event == VIM_MENU_అంశం_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_గమ్యం_INT(udata));
}

    స్థిర శూన్య
కూడు_tabline_menu_అంశం(VimMenu *menu, స్థిరరాశి అక్షరం *name, పూర్ణాంకం resp)
{
    Vim_Menu_అంశం *అంశం = VIM_MENU_అంశం(vim_menu_అంశం_new(name,
		tabline_menu_event_cb, GINT_గమ్యం_సూచిక(resp)));

    vim_menu_చొప్పించు_అంశం(menu, అంశం, -1);
}

/*
 * Create a menu for the tab line.
 */
    స్థిర VimMenu *
create_tabline_popup_menu(శూన్య)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    కూడు_tabline_menu_అంశం(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    కూడు_tabline_menu_అంశం(menu, _("New Tab"), TABLINE_MENU_NEW);
    కూడు_tabline_menu_అంశం(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    తిరిగి menu;
}

    స్థిర శూన్య
tabline_menu_press_event(
	GtkGestureClick *gesture,
	పూర్ణాంకం		n_press వాడని,
	ద్విగుణ		x,
	ద్విగుణ		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    ఒకవేళ (but == GDK_BUTTON_SECONDARY)
    {
	// Right mouse button pressed, popup menu
	GdkRectangle rect;
	rect.x = x;
	rect.y = y;
	rect.వెడల్పు = 1;
	rect.ఎత్తు = 1;

	gtk_popover_సమితి_pointing_గమ్యం(GTK_POPOVER(popover), &rect);
	gtk_popover_popup(GTK_POPOVER(popover));
    }
    లేకపోతే ఒకవేళ (but == GDK_BUTTON_PRIMARY)
    {
	ఒకవేళ (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    లేకపోతే ఒకవేళ (but == GDK_BUTTON_మధ్య)
    {
	ఒకవేళ (tabpage_hover != 0)
	    // Middle mouse click on tabpage label closes that tab.
	    send_tabline_menu_event(tabpage_hover, TABLINE_MENU_CLOSE);
    }
}
#అయితే_ముగింపు

/*
 * ============================================================
 * Sign support
 * ============================================================
 */

#ఒకవేళ defined(FEAT_SIGN_ICONS)
# define SIGN_వెడల్పు  (2 * gui.char_వెడల్పు)
# define SIGN_ఎత్తు (gui.char_ఎత్తు)

    శూన్య
gui_mch_drawsign(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    ఒకవేళ (sign == ఖాళీ)
	తిరిగి;

    vim_draw_area_కూడు_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    అడ్డువరుస, నిలువు, SIGN_వెడల్పు, SIGN_ఎత్తు);

    gtk_widget_వరుస_draw(gui.drawarea);
}

    శూన్య *
gui_mch_రిజిస్టరు_sign(char_u *signfile)
{
    ఒకవేళ (signfile[0] != NUL && signfile[0] != '-' && gui.లోపల_use)
    {
	GError	    *error = ఖాళీ;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((స్థిరరాశి అక్షరం *)signfile,
							    &error);
	ఒకవేళ (sign != ఖాళీ)
	    తిరిగి sign;

	semsg("E255: %s", error->సందేశం_2);
	g_error_free(error);
    }
    తిరిగి ఖాళీ;
}

    శూన్య
gui_mch_destroy_sign(శూన్య *sign)
{
    ఒకవేళ (sign != ఖాళీ)
	g_object_unref(sign);
}
#అయితే_ముగింపు

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
    పూర్ణాంకం
gui_gtk_draw_string(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags)
{
    పూర్ణాంకం cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    అడ్డువరుస, నిలువు, s, len, flags);

    ఒకవేళ (gui.drawarea != ఖాళీ)
	gtk_widget_వరుస_draw(gui.drawarea);

    తిరిగి cells;
}

    పూర్ణాంకం
gui_get_x11_windis(కిటికీ *win వాడని, Display **dis వాడని)
{
    // GTK4: not applicable
    తిరిగి FAIL;
}

#ఒకవేళ defined(FEAT_MENU)
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
    స్థిర char_u *
translate_mnemonic_ట్యాగ్(char_u *name, పూర్ణాంకం use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    పూర్ణాంకం	    n_underscores = 0;

    name = CONVERT_గమ్యం_UTF8(name);
    ఒకవేళ (name == ఖాళీ)
	తిరిగి ఖాళీ;

    కొరకు (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	ఒకవేళ (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    ఒకవేళ (buf != ఖాళీ)
    {
	pdest = buf;
	కొరకు (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    ఒకవేళ (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    లేకపోతే ఒకవేళ (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    లేకపోతే ఒకవేళ (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    లేకపోతే ఒకవేళ (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_గమ్యం_UTF8_FREE(name);
    తిరిగి buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    శూన్య
gui_gtk_సమితి_mnemonics(పూర్ణాంకం enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	ఒకవేళ (menu->id == ఖాళీ)
	    కొనసాగు;

	name = translate_mnemonic_ట్యాగ్(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	ఒకవేళ (VIM_IS_MENU_BAR_అంశం(menu->id))
	    vim_menu_bar_అంశం_సమితి_పాఠ్యం(VIM_MENU_BAR_అంశం(menu->id),
		    (స్థిరరాశి అక్షరం *)name);
	vim_free(name);
    }
}

    స్థిర శూన్య
popupmenu_closed_cb(GtkWidget *popover, శూన్య *udata వాడని)
{
    gtk_widget_unparent(popover);
    ఒకవేళ (gui.drawarea != ఖాళీ)
	gtk_widget_వరుస_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    స్థిర శూన్య
gui_gtk_popup_at(vimmenu_T *menu, పూర్ణాంకం x, పూర్ణాంకం y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    ఒకవేళ (menu == ఖాళీ || menu->submenu_id == ఖాళీ)
	తిరిగి;

    popover = vim_menu_copy(VIM_MENU(menu->submenu_id));
    gtk_widget_సమితి_మాతృ(popover, gui.drawarea);

    rect.x = x;
    rect.y = y;
    rect.వెడల్పు = rect.ఎత్తు = 1;

    // Make sure popover aligns down-and-to-the-right of the pointer.
    gtk_popover_సమితి_position(GTK_POPOVER(popover), GTK_POS_BOTTOM);
    gtk_widget_సమితి_halign(popover, GTK_ALIGN_ప్రారంభం);
    gtk_popover_సమితి_pointing_గమ్యం(GTK_POPOVER(popover), &rect);

    g_signal_connect(GTK_POPOVER(popover), "closed",
	    G_CALLBACK(popupmenu_closed_cb), ఖాళీ);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    శూన్య
gui_make_popup(char_u *మార్గం_name, పూర్ణాంకం mouse_pos)
{
    vimmenu_T	*menu;
    పూర్ణాంకం		x, y;

    menu = gui_కనుగొను_menu(మార్గం_name);
    ఒకవేళ (menu == ఖాళీ || menu->submenu_id == ఖాళీ)
	తిరిగి;

    ఒకవేళ (mouse_pos)
	gui_mch_getmouse(&x, &y);
    లేకపోతే
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#అయితే_ముగింపు // FEAT_MENU

    పూర్ణాంకం
get_menu_tool_వెడల్పు(శూన్య)
{
    తిరిగి 0;
}

    పూర్ణాంకం
get_menu_tool_ఎత్తు(శూన్య)
{
    GtkWidget *widgets[] = {
#నిర్వచితమైతే FEAT_MENU
	gui.menubar,
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TOOLBAR
	gui.toolbar,
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
	gui.tabline
#అయితే_ముగింపు
    };

    పూర్ణాంకం ఎత్తు = 0;

    కొరకు (పూర్ణాంకం i = 0; i < (పూర్ణాంకం)ARRAY_నిడివి(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	పూర్ణాంకం		h;

	ఒకవేళ (widgets[i] == ఖాళీ || !gtk_widget_get_visible(widgets[i]))
	    కొనసాగు;

	h = gtk_widget_get_ఎత్తు(widgets[i]);

	ఒకవేళ (h == 0)
	{
	    // Allocation hasn't been updated yet (widget just became visible).
	    // Query the preferred height so the caller gets a valid value
	    // before the layout pass runs.  Use the maximum of minimum and
	    // natural height: GTK may allocate min_h even when natural_h is
	    // smaller (e.g. GtkNotebook tab bar has min_h > natural_h due to
	    // CSS).
	    gtk_widget_get_preferred_కొలత(widgets[i], &min, &nat);
	    ఎత్తు += MAX(min.ఎత్తు, nat.ఎత్తు);
	}
	లేకపోతే
	    ఎత్తు += h;
    }
    తిరిగి ఎత్తు;
}

/*
 * Update selection targets for regular and primary selections.
 */
    శూన్య
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    ఒకవేళ (cbd == &clip_plus)
	gdk_content_provider_content_మారింది(gui.regular_provider);
    లేకపోతే
	gdk_content_provider_content_మారింది(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    స్థిర GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    ఒకవేళ (gui.mainwin == ఖాళీ)
	తిరిగి ఖాళీ;

    display = gtk_widget_get_display(gui.mainwin);
    ఒకవేళ (display == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (cbd == &clip_plus)
    {
	ఒకవేళ (provider != ఖాళీ)
	    *provider = gui.regular_provider;
	తిరిగి gdk_display_get_clipboard(display);
    }
    లేకపోతే
    {
	ఒకవేళ (provider != ఖాళీ)
	    *provider = gui.primary_provider;
	తిరిగి gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    స్థిర శూన్య
clip_read_cb(GdkClipboard *cb, GAsync_ఫలితం *ఫలితం, Clip_Read_దత్తాంశం *crd)
{
    GInput_ప్రవాహం    *లోపల_ప్రవాహం;
    స్థిరరాశి అక్షరం	    *mime_type;

    లోపల_ప్రవాహం = gdk_clipboard_read_ముగింపు(cb, ఫలితం, &mime_type, ఖాళీ);
    ఒకవేళ (లోపల_ప్రవాహం == ఖాళీ)
    {
	read_దత్తాంశం_free(&crd->rd);
	తిరిగి;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_ఆదానం_ప్రవాహం_read_async(లోపల_ప్రవాహం, crd->rd.buf, READDATA_BUFSIZE,
	    G_ప్రాధాన్యత_ఎగువ,
	    crd->rd.cancel, (GAsyncReadyCallback)read_దత్తాంశం_ఆదానం_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
    స్థిర స్థిరరాశి అక్షరం *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	ఖాళీ
    };

    GdkClipboard *clipboard;
    Clip_Read_దత్తాంశం *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, ఖాళీ);
    ఒకవేళ (clipboard == ఖాళీ)
	తిరిగి;

    crd = (Clip_Read_దత్తాంశం *)read_దత్తాంశం_new(READ_దత్తాంశం_CLIP, పరిమాణం(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_ప్రాధాన్యత_ఎగువ, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	time_t ప్రారంభం;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	ప్రారంభం = సమయం(ఖాళీ);
	అయితే (!g_cancellable_is_cancelled(cancel)
		&& సమయం(ఖాళీ) < ప్రారంభం + 3)
	    g_main_context_iteration(ఖాళీ, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

స్థిర పూర్ణాంకం లోపల_clipboard_సమితి = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    శూన్య
clip_mch_సమితి_selection(Clipboard_T *cbd వాడని)
{
}

    స్థిర శూన్య
clipboard_మారింది_cb(GdkClipboard *clipboard, gpointer user_దత్తాంశం)
{
    Clipboard_T *cbd = (Clipboard_T *)user_దత్తాంశం;

    ఒకవేళ (లోపల_clipboard_సమితి)
	తిరిగి;
    ఒకవేళ (gdk_clipboard_is_local(clipboard))
	తిరిగి;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    పూర్ణాంకం			ret;

    ఒకవేళ (cb == ఖాళీ)
	తిరిగి FAIL;

    లోపల_clipboard_సమితి = TRUE;
    ret = gdk_clipboard_సమితి_content(cb, cp);
    లోపల_clipboard_సమితి = FALSE;

    తిరిగి ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    శూన్య
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, ఖాళీ);
    ఒకవేళ (clipboard == ఖాళీ)
	తిరిగి;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    ఒకవేళ (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_సమితి_content(clipboard, ఖాళీ);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(శూన్య)
{
    తిరిగి gtk4_main_loop_level;
}

    శూన్య
gtk_main_quit(శూన్య)
{
    gtk4_main_loop_quit = TRUE;
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
స్థిర స్థిరరాశి అక్షరం *mshape_css_పేర్లు[] =
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

    శూన్య
mch_సమితి_mouse_shape(పూర్ణాంకం shape)
{
    GdkCursor	*c;
    స్థిరరాశి అక్షరం	*css_name = "default";

    ఒకవేళ (gui.drawarea == ఖాళీ)
	తిరిగి;

    ఒకవేళ (shape == MSHAPE_HIDE || gui.సూచిక_hidden)
	gtk_widget_సమితి_cursor(gui.drawarea, gui.blank_సూచిక);
    లేకపోతే
    {
	ఒకవేళ (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	లేకపోతే ఒకవేళ (shape < (పూర్ణాంకం)ARRAY_నిడివి(mshape_css_పేర్లు))
	    css_name = mshape_css_పేర్లు[shape];
	లేకపోతే
	    తిరిగి;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, ఖాళీ);
	gtk_widget_సమితి_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    ఒకవేళ (shape != MSHAPE_HIDE)
	చివరి_shape = shape;
}

#అయితే_ముగింపు // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



స్థిర పూర్ణాంకం చివరి_పాఠ్యం_area_w = 0;
స్థిర పూర్ణాంకం చివరి_పాఠ్యం_area_h = 0;

/*
 * ============================================================
 * Menu functions
 * ============================================================
 */

#నిర్వచితమైతే FEAT_MENU

# ifdef FEAT_TOOLBAR
/*
 * Icon name table for toolbar buttons.
 * Must match toolbar_names[] in menu.c.
 */
స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి toolbar_icon_పేర్లు[] =
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

    స్థిర శూన్య
toolbar_button_clicked_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    gui_menu_cb((vimmenu_T *)దత్తాంశం);
}

    స్థిర GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = ఖాళీ;

    // Try specified icon file first
    ఒకవేళ (menu->iconfile != ఖాళీ)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	ఒకవేళ (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (స్థిరరాశి అక్షరం *)buf, ఖాళీ);

	    ఒకవేళ (texture != ఖాళీ)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_సమితి_కొలత_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    ఒకవేళ (image == ఖాళీ)
    {
	స్థిరరాశి అక్షరం *icon_name = "image-missing";
	పూర్ణాంకం n = (పూర్ణాంకం)ARRAY_నిడివి(toolbar_icon_పేర్లు);

	ఒకవేళ (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_పేర్లు[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    తిరిగి image;
}
# endif

    స్థిర శూన్య
menu_button_clicked_cb(
	Vim_Menu_అంశం	    *అంశం వాడని,
	Vim_Menu_అంశం_Event    event,
	vimmenu_T	    *menu)
{
    ఒకవేళ (event == VIM_MENU_అంశం_CLICKED)
	gui_menu_cb(menu);
    లేకపోతే ఒకవేళ (event == VIM_MENU_అంశం_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	స్థిర gboolean did_సందేశం = FALSE;

	ఒకవేళ (State & బహుళకం_CMDLINE)
	    తిరిగి;

	tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
	ఒకవేళ (tooltip != ఖాళీ && utf_valid_string(tooltip, ఖాళీ))
	{
	    సందేశం((అక్షరం *)tooltip);
	    did_సందేశం = TRUE;
	    setcursor();
	    బహిర్గతం_flush_cursor(TRUE, FALSE);
	}
	లేకపోతే ఒకవేళ (did_సందేశం)
	{
	    సందేశం("");
	    did_సందేశం = FALSE;
	    setcursor();
	    బహిర్గతం_flush_cursor(TRUE, FALSE);
	}
	CONVERT_గమ్యం_UTF8_FREE(tooltip);
    }
}

    శూన్య
gui_mch_కూడు_menu(vimmenu_T *menu, పూర్ణాంకం idx)
{
    vimmenu_T	*మాతృ;
    GtkWidget	*మాతృ_widget;
    gboolean	use_mnemonic;
    char_u	*పాఠ్యం;

    ఒకవేళ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_సమితి_మాతృ(menu->submenu_id, gui.drawarea);
	తిరిగి;
    }

    మాతృ = menu->మాతృ;

    ఒకవేళ ((మాతృ != ఖాళీ && మాతృ->submenu_id == ఖాళీ)
	    || !menu_is_menubar(menu->name))
	తిరిగి;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = మాతృ != ఖాళీ || p_wak[0] != 'n';
    పాఠ్యం = translate_mnemonic_ట్యాగ్(menu->name, use_mnemonic);

    ఒకవేళ (మాతృ != ఖాళీ)
    {
	మాతృ_widget = మాతృ->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_అంశం_new(
		    (స్థిరరాశి అక్షరం *)పాఠ్యం, ఖాళీ, ఖాళీ));

	vim_menu_అంశం_సమితి_submenu(VIM_MENU_అంశం(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_చొప్పించు_అంశం(VIM_MENU(మాతృ_widget),
		VIM_MENU_అంశం(menu->id), idx);
    }
    లేకపోతే
    {
	మాతృ_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_అంశం_new(
		    (స్థిరరాశి అక్షరం *)పాఠ్యం, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_చొప్పించు_అంశం(VIM_MENU_BAR(మాతృ_widget),
		VIM_MENU_BAR_అంశం(menu->id), idx);
    }

    vim_free(పాఠ్యం);
}

    శూన్య
gui_mch_కూడు_menu_అంశం(vimmenu_T *menu, పూర్ణాంకం idx)
{
    vimmenu_T	*మాతృ = menu->మాతృ;

# ifdef FEAT_TOOLBAR
    ఒకవేళ (మాతృ != ఖాళీ && menu_is_toolbar(మాతృ->name))
    {
	ఒకవేళ (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_చొప్పించు_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	లేకపోతే
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*పాఠ్యం;
	    char_u	*tooltip;

	    పాఠ్యం    = CONVERT_గమ్యం_UTF8(menu->dname);
	    tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
	    ఒకవేళ (tooltip != ఖాళీ && !utf_valid_string(tooltip, ఖాళీ))
		CONVERT_గమ్యం_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_చొప్పించు_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (స్థిరరాశి అక్షరం *)పాఠ్యం, idx);
	    gtk_widget_సమితి_tooltip_పాఠ్యం(btn, (స్థిరరాశి gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_గమ్యం_UTF8_FREE(పాఠ్యం);
	    CONVERT_గమ్యం_UTF8_FREE(tooltip);
	}
	తిరిగి;
    }
# endif

    // Menu items (non-toolbar)
    ఒకవేళ (మాతృ == ఖాళీ || మాతృ->submenu_id == ఖాళీ)
	తిరిగి;

    ఒకవేళ (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_చొప్పించు_separator(
		VIM_MENU(మాతృ->submenu_id), idx));
    }
    లేకపోతే
    {
	char_u	    *పాఠ్యం;
	char_u	    *accel_పాఠ్యం = ఖాళీ;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	పాఠ్యం = translate_mnemonic_ట్యాగ్(menu->name, use_mnemonic);

	ఒకవేళ (menu->actext != ఖాళీ && menu->actext[0] != NUL)
	    accel_పాఠ్యం = CONVERT_గమ్యం_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_అంశం_new((స్థిరరాశి అక్షరం *)పాఠ్యం,
		    (Vim_Menu_అంశం_Func)menu_button_clicked_cb, menu));

	ఒకవేళ (accel_పాఠ్యం != ఖాళీ)
	    vim_menu_అంశం_సమితి_accel(VIM_MENU_అంశం(menu->id),
		    (స్థిరరాశి అక్షరం *)accel_పాఠ్యం);

	vim_menu_చొప్పించు_అంశం(VIM_MENU(మాతృ->submenu_id),
		VIM_MENU_అంశం(menu->id), idx);

	vim_free(పాఠ్యం);
	CONVERT_గమ్యం_UTF8_FREE(accel_పాఠ్యం);
    }
}

    శూన్య
gui_mch_toggle_tearoffs(పూర్ణాంకం enable వాడని)
{
    // GTK4: tearoff menus don't exist.
}

    శూన్య
gui_mch_menu_సమితి_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    ఒకవేళ (menu->id == ఖాళీ)
	తిరిగి;

    tooltip = CONVERT_గమ్యం_UTF8(menu->అక్షరశ్రేణులు[MENU_సూచీ_TIP]);
    ఒకవేళ (tooltip != ఖాళీ && utf_valid_string(tooltip, ఖాళీ))
	gtk_widget_సమితి_tooltip_పాఠ్యం(menu->id, (స్థిరరాశి అక్షరం *)tooltip);
    CONVERT_గమ్యం_UTF8_FREE(tooltip);
}

    శూన్య
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    ఒకవేళ (menu->మాతృ != ఖాళీ && menu_is_toolbar(menu->మాతృ->name))
    {
	ఒకవేళ (menu->id != ఖాళీ)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = ఖాళీ;
	}
	తిరిగి;
    }

    // For popup menus, unparent the menu as well
    ఒకవేళ (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	ఒకవేళ (menu->submenu_id != ఖాళీ)
	    gtk_widget_unparent(menu->submenu_id);
    }
    లేకపోతే ఒకవేళ (menu->మాతృ == ఖాళీ && menu->id != ఖాళీ)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    లేకపోతే ఒకవేళ (menu->id != ఖాళీ)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->మాతృ->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    శూన్య
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    పూర్ణాంకం x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    స్థిర శూన్య
show_menubar_popover(శూన్య)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    ఒకవేళ (gui.menubar == ఖాళీ || gui.drawarea == ఖాళీ)
	తిరిగి;

    ఒకవేళ (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), ఖాళీ);
	తిరిగి;
    }

    // Copy and convert the menubar into a menu popover
    menu = vim_menu_bar_గమ్యం_menu(VIM_MENU_BAR(gui.menubar));

    gtk_widget_సమితి_మాతృ(menu, gui.drawarea);
    gtk_popover_సమితి_position(GTK_POPOVER(menu), GTK_POS_BOTTOM);
    rect.x = 0;
    rect.y = 0;
    rect.వెడల్పు = 1;
    rect.ఎత్తు = 1;
    gtk_popover_సమితి_pointing_గమ్యం(GTK_POPOVER(menu), &rect);
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), ఖాళీ);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#అయితే_ముగింపు // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    శూన్య
gui_mch_సమితి_scrollbar_thumb(scrollbar_T *sb, దీర్ఘ val, దీర్ఘ కొలత, దీర్ఘ max)
{
    GtkAdjustment *adj;

    ఒకవేళ (sb->id == ఖాళీ)
	తిరిగి;
    ఒకవేళ (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	తిరిగి;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_సమితి_క్రింది(adj, 0.0);
    gtk_adjustment_సమితి_పై(adj, (gdouble)max + 1);
    gtk_adjustment_సమితి_విలువ(adj, (gdouble)val);
    gtk_adjustment_సమితి_అడుగు_increment(adj, 1.0);
    gtk_adjustment_సమితి_పుట_increment(adj, (gdouble)(కొలత > 2 ? కొలత - 2 : 1));
    gtk_adjustment_సమితి_పుట_కొలత(adj, (gdouble)కొలత);
}

    శూన్య
gui_mch_సమితి_scrollbar_pos(scrollbar_T *sb, పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    ఒకవేళ (sb->id != ఖాళీ)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    పూర్ణాంకం
gui_mch_get_scrollbar_xpadding(శూన్య)
{
    పూర్ణాంకం formwin_w = gtk_widget_get_వెడల్పు(gui.formwin);
    పూర్ణాంకం sbar_w = 0;
    పూర్ణాంకం xpad;

    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
	sbar_w += gui.scrollbar_వెడల్పు;
    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి])
	sbar_w += gui.scrollbar_వెడల్పు;

    xpad = formwin_w - చివరి_పాఠ్యం_area_w - sbar_w;
    తిరిగి (xpad < 0) ? 0 : xpad;
}

    పూర్ణాంకం
gui_mch_get_scrollbar_ypadding(శూన్య)
{
    పూర్ణాంకం formwin_h = gtk_widget_get_ఎత్తు(gui.formwin);
    పూర్ణాంకం ypad;

    ypad = formwin_h - చివరి_పాఠ్యం_area_h;
    ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_ఎత్తు;

    తిరిగి (ypad < 0) ? 0 : ypad;
}

    స్థిర శూన్య
adjustment_విలువ_మారింది(GtkAdjustment *adj, gpointer దత్తాంశం వాడని)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_దత్తాంశం(G_OBJECT(adj), "vim-sb");
    దీర్ఘ విలువ = (దీర్ఘ)gtk_adjustment_get_విలువ(adj);

    ఒకవేళ (sb != ఖాళీ)
	gui_drag_scrollbar(sb, విలువ, FALSE);
}

    శూన్య
gui_mch_create_scrollbar(scrollbar_T *sb, పూర్ణాంకం orient)
{
    GtkAdjustment *adj;
    ఒకవేళ (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, ఖాళీ);
    లేకపోతే
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, ఖాళీ);

    gtk_widget_కూడు_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_సమితి_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_సమితి_దత్తాంశం(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_విలువ_మారింది), ఖాళీ);
}

    శూన్య
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    ఒకవేళ (sb->id != ఖాళీ)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = ఖాళీ;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    శూన్య
gui_mch_సమితి_పాఠ్యం_area_pos(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం w, పూర్ణాంకం h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    ఒకవేళ (w <= 0 || h <= 0)
	తిరిగి;
    చివరి_పాఠ్యం_area_w = w;
    చివరి_పాఠ్యం_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    శూన్య
gui_gtk4_calculate_bleed(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    పూర్ణాంకం old_కుడి = gui.bleed_కుడి;
    పూర్ణాంకం old_bot = gui.bleed_bot;

    gui.bleed_కుడి = వెడల్పు - చివరి_పాఠ్యం_area_w;
    gui.bleed_bot = ఎత్తు - చివరి_పాఠ్యం_area_h;

    ఒకవేళ (gui.which_scrollbars[SBAR_ఎడమ])
	gui.bleed_కుడి -= gui.scrollbar_వెడల్పు;
    ఒకవేళ (gui.which_scrollbars[SBAR_కుడి])
	gui.bleed_కుడి -= gui.scrollbar_వెడల్పు;
    ఒకవేళ (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_ఎత్తు;

    // Not sure if this can happen, but be safe...
    ఒకవేళ (gui.bleed_కుడి < 0)
	gui.bleed_కుడి = 0;
    ఒకవేళ (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    ఒకవేళ (old_కుడి != gui.bleed_కుడి || old_bot != gui.bleed_bot)
	gtk_widget_వరుస_draw(GTK_WIDGET(gui.drawarea));
}

/*
 * ============================================================
 * Browse dialogs
 * ============================================================
 */

/*
 * Blocking helper: run a GtkFileDialog and wait for result.
 */
రకనిర్వచనం నిర్మాణం {
    GFile	*ఫలితం;
    gboolean	పూర్తి;
} File_Dialog_దత్తాంశం;

    స్థిర శూన్య
file_dialog_open_cb(GObject *ఆధారం, GAsync_ఫలితం *res, gpointer దత్తాంశం)
{
    File_Dialog_దత్తాంశం *fdd = (File_Dialog_దత్తాంశం *)దత్తాంశం;
    fdd->ఫలితం = gtk_file_dialog_open_ముగింపు(
		    GTK_FILE_DIALOG(ఆధారం), res, ఖాళీ);
    fdd->పూర్తి = TRUE;
}

    స్థిర శూన్య
file_dialog_save_cb(GObject *ఆధారం, GAsync_ఫలితం *res, gpointer దత్తాంశం)
{
    File_Dialog_దత్తాంశం *fdd = (File_Dialog_దత్తాంశం *)దత్తాంశం;
    fdd->ఫలితం = gtk_file_dialog_save_ముగింపు(
		    GTK_FILE_DIALOG(ఆధారం), res, ఖాళీ);
    fdd->పూర్తి = TRUE;
}

    స్థిర శూన్య
file_dialog_folder_cb(GObject *ఆధారం, GAsync_ఫలితం *res, gpointer దత్తాంశం)
{
    File_Dialog_దత్తాంశం *fdd = (File_Dialog_దత్తాంశం *)దత్తాంశం;
    fdd->ఫలితం = gtk_file_dialog_select_folder_ముగింపు(
		    GTK_FILE_DIALOG(ఆధారం), res, ఖాళీ);
    fdd->పూర్తి = TRUE;
}

    char_u *
gui_mch_browse(పూర్ణాంకం saving,
	char_u *title,
	char_u *dflt,
	char_u *ext వాడని,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    File_Dialog_దత్తాంశం  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *ఫలితం = ఖాళీ;
    GListStore	    *filters;

    title = CONVERT_గమ్యం_UTF8(title);

    ఒకవేళ (initdir == ఖాళీ || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    లేకపోతే ఒకవేళ (vim_నిండిన_Name(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    కూడు_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_సమితి_modal(dlg, TRUE);
    ఒకవేళ (title != ఖాళీ)
	gtk_file_dialog_సమితి_title(dlg, (స్థిరరాశి అక్షరం *)title);

    {
	GFile *dir = g_file_new_for_మార్గం((స్థిరరాశి అక్షరం *)dirbuf);
	gtk_file_dialog_సమితి_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    ఒకవేళ (filter != ఖాళీ && *filter != NUL)
    {
	పూర్ణాంకం		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	ఒకవేళ (patt != ఖాళీ)
	{
	    అయితే (p != ఖాళీ && *p != NUL)
	    {
		ఒకవేళ (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    ఒకవేళ (*p == '\t')
			gtk_file_filter_సమితి_name(gfilter, (gchar *)patt);
		    లేకపోతే
		    {
			gtk_file_filter_కూడు_నమూనా(gfilter, (gchar *)patt);
			ఒకవేళ (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    ఒకవేళ (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		లేకపోతే
		{
		    p++;
		    i++;
		}
	    }
	}
	vim_free(patt);
    }
    gtk_file_dialog_సమితి_filters(dlg, G_LIST_MODEL(filters));
    g_object_unref(filters);

    ఒకవేళ (saving && dflt != ఖాళీ && *dflt != NUL)
	gtk_file_dialog_సమితి_initial_name(dlg, (స్థిరరాశి అక్షరం *)dflt);

    fdd.ఫలితం = ఖాళీ;
    fdd.పూర్తి = FALSE;

    ఒకవేళ (saving)
	gtk_file_dialog_save(dlg, GTK_కిటికీ(gui.mainwin), ఖాళీ,
		file_dialog_save_cb, &fdd);
    లేకపోతే
	gtk_file_dialog_open(dlg, GTK_కిటికీ(gui.mainwin), ఖాళీ,
		file_dialog_open_cb, &fdd);

    అయితే (!fdd.పూర్తి)
	g_main_context_iteration(ఖాళీ, TRUE);

    ఒకవేళ (fdd.ఫలితం != ఖాళీ)
    {
	అక్షరం *మార్గం = g_file_get_మార్గం(fdd.ఫలితం);
	ఒకవేళ (మార్గం != ఖాళీ)
	{
	    ఫలితం = vim_strsave((char_u *)మార్గం);
	    g_free(మార్గం);
	}
	g_object_unref(fdd.ఫలితం);
    }

    g_object_unref(dlg);
    CONVERT_గమ్యం_UTF8_FREE(title);

    తిరిగి ఫలితం;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    File_Dialog_దత్తాంశం	fdd;
    char_u		*ఫలితం = ఖాళీ;

    title = CONVERT_గమ్యం_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_సమితి_modal(dlg, TRUE);
    ఒకవేళ (title != ఖాళీ)
	gtk_file_dialog_సమితి_title(dlg, (స్థిరరాశి అక్షరం *)title);

    ఒకవేళ (initdir != ఖాళీ && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_మార్గం((స్థిరరాశి అక్షరం *)initdir);
	gtk_file_dialog_సమితి_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.ఫలితం = ఖాళీ;
    fdd.పూర్తి = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_కిటికీ(gui.mainwin), ఖాళీ,
	    file_dialog_folder_cb, &fdd);

    అయితే (!fdd.పూర్తి)
	g_main_context_iteration(ఖాళీ, TRUE);

    ఒకవేళ (fdd.ఫలితం != ఖాళీ)
    {
	అక్షరం *మార్గం = g_file_get_మార్గం(fdd.ఫలితం);
	ఒకవేళ (మార్గం != ఖాళీ)
	{
	    ఫలితం = vim_strsave((char_u *)మార్గం);
	    g_free(మార్గం);
	}
	g_object_unref(fdd.ఫలితం);
    }

    g_object_unref(dlg);
    CONVERT_గమ్యం_UTF8_FREE(title);

    తిరిగి ఫలితం;
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
    స్థిర అక్షరం **
విభజించు_button_string(char_u *button_string, పూర్ణాంకం *n_buttons, అక్షరం **tofree)
{
    అక్షరం	    **array;
    char_u	    *p;
    గుర్తులేని పూర్ణాంకం    లెక్క = 1;

    button_string = (char_u *)g_strdup((స్థిరరాశి అక్షరం *)button_string);

    కొరకు (p = button_string; *p != NUL; ++p)
	ఒకవేళ (*p == DLG_BUTTON_SEP)
	    ++లెక్క;

    array = g_malloc_n(లెక్క, పరిమాణం(అక్షరం *));
    లెక్క = 0;

    ఒకవేళ (array != ఖాళీ)
    {
	array[లెక్క++] = (అక్షరం *)button_string;
	కొరకు (p = button_string; *p != NUL; )
	{
	    ఒకవేళ (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[లెక్క++] = (అక్షరం *)p;
	    }
	    లేకపోతే ఒకవేళ (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    లేకపోతే
		MB_PTR_ADV(p);
	}
    }

    *tofree = (అక్షరం *)button_string;
    *n_buttons = లెక్క;
    తిరిగి array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    స్థిర స్థిరరాశి అక్షరం *
dialog_type_గమ్యం_icon(పూర్ణాంకం type)
{
    ఎంపిక (type)
    {
	స్థితి VIM_ERROR:
	    తిరిగి "dialog-error-symbolic";
	స్థితి VIM_WARNING:
	    తిరిగి "dialog-warning-symbolic";
	స్థితి VIM_INFO:
	    తిరిగి "dialog-information-symbolic";
	స్థితి VIM_QUESTION:
	    తిరిగి "dialog-question-symbolic";
	అప్రమేయ:
	    ఆపు;
    }
    తిరిగి ఖాళీ;
}

// Data associated with each button in the dialog
రకనిర్వచనం నిర్మాణం
{
    పూర్ణాంకం		but_idx;
    పూర్ణాంకం		*response;
    gboolean	*పూర్తి;
} DialogButtonState;

రకనిర్వచనం నిర్మాణం
{
    GtkWidget	*win;
    gboolean	*పూర్తి;
    gboolean	no_alt;
} DialogState;

    స్థిర శూన్య
dialog_button_clicked_cb(GtkButton *button వాడని, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->పూర్తి = TRUE;
}

    స్థిర gboolean
dialog_కీలకం_pressed_cb(
	Gtk_Event_Controller_కీలకం	*controller వాడని,
	guint			keyval,
	guint			keycode వాడని,
	GdkModifierType		state,
	DialogState		*dstate)
{
    ఒకవేళ (keyval == GDK_కీలకం_Escape)
    {
	*dstate->పూర్తి = TRUE;
	తిరిగి TRUE;
    }

    ఒకవేళ (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_ముసుగు()))
	తిరిగి gtk_widget_mnemonic_activate(dstate->win, FALSE);
    తిరిగి FALSE;
}

    స్థిర gboolean
dialog_close_request_cb(Gtk_కిటికీ *win వాడని, gboolean *win_closed)
{
    *win_closed = TRUE;
    తిరిగి FALSE;
}

    పూర్ణాంకం
gui_mch_dialog(
	పూర్ణాంకం	type,
	char_u	*title,
	char_u	*సందేశం_2,
	char_u	*buttons,
	పూర్ణాంకం	def_but,
	char_u	*textfield,
	పూర్ణాంకం	ex_cmd వాడని)
{
    Gtk_కిటికీ		*win = GTK_కిటికీ(gtk_కిటికీ_new());
    GtkWidget		*vertbox;
    GtkWidget		*సందేశం_box;
    స్థిరరాశి అక్షరం		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*నమోదు_2 = ఖాళీ;
    char_u		*utf8_title;
    char_u		*utf8_సందేశం;
    GtkEventController *కీలకం_controller;
    DialogButtonState	*but_స్థితులు = ఖాళీ;
    అక్షరం		*tofree = ఖాళీ;
    పూర్ణాంకం			response = -1;
    gboolean		పూర్తి = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_గమ్యం_UTF8(title);
    ఒకవేళ (utf8_title != ఖాళీ)
	gtk_కిటికీ_సమితి_title(win, (స్థిరరాశి అక్షరం *)utf8_title);
    CONVERT_గమ్యం_UTF8_FREE(utf8_title);

    gtk_కిటికీ_సమితి_transient_for(win, GTK_కిటికీ(gui.mainwin));
    gtk_కిటికీ_సమితి_modal(win, TRUE);
    gtk_కిటికీ_సమితి_default_కొలత(win, 300, -1);
    gtk_కిటికీ_సమితి_destroy_with_మాతృ(win, TRUE);
    g_signal_connect(win, "close-request",
	    G_CALLBACK(dialog_close_request_cb), &win_closed);

    // Create main vertical layout container
    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 16);
    gtk_కిటికీ_సమితి_బిడ్డ(win, vertbox);
    gtk_widget_సమితి_margin_శిఖరం(vertbox, 24);
    gtk_widget_సమితి_margin_bottom(vertbox, 24);
    gtk_widget_సమితి_margin_ప్రారంభం(vertbox, 12);
    gtk_widget_సమితి_margin_చివర(vertbox, 12);

    // Add the message label
    సందేశం_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_సమితి_halign(సందేశం_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), సందేశం_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_గమ్యం_icon(type);
    ఒకవేళ (icon_name != ఖాళీ)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_సమితి_icon_కొలత(GTK_IMAGE(icon), GTK_ICON_కొలత_LARGE);
	gtk_box_append(GTK_BOX(సందేశం_box), icon);
    }

    utf8_సందేశం = CONVERT_గమ్యం_UTF8(సందేశం_2);
    label = gtk_label_new((స్థిరరాశి అక్షరం *)utf8_సందేశం);
    CONVERT_గమ్యం_UTF8_FREE(utf8_సందేశం);
    gtk_label_సమితి_wrap(GTK_LABEL(label), TRUE);
    gtk_label_సమితి_max_వెడల్పు_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(సందేశం_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    కీలకం_controller = gtk_event_controller_కీలకం_new();
    g_signal_connect(కీలకం_controller, "key-pressed",
	    G_CALLBACK(dialog_కీలకం_pressed_cb), &state);
    gtk_widget_కూడు_controller(GTK_WIDGET(win), కీలకం_controller);
    state.పూర్తి = &పూర్తి;
    state.win = GTK_WIDGET(win);

    ఒకవేళ (textfield != ఖాళీ)
    {
	// Add text entry so user can enter text
	char_u *utf8_పాఠ్యం = CONVERT_గమ్యం_UTF8(textfield);

	నమోదు_2 = gtk_నమోదు_new();

	ఒకవేళ (utf8_పాఠ్యం != ఖాళీ)
	    gtk_editable_సమితి_పాఠ్యం(GTK_EDITABLE(నమోదు_2), (స్థిరరాశి అక్షరం *)utf8_పాఠ్యం);
	లేకపోతే
	    gtk_editable_సమితి_పాఠ్యం(GTK_EDITABLE(నమోదు_2), "");
	CONVERT_గమ్యం_UTF8_FREE(utf8_పాఠ్యం);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_నమోదు_సమితి_activates_default(GTK_నమోదు(నమోదు_2), TRUE);
	gtk_box_append(GTK_BOX(vertbox), నమోదు_2);
	state.no_alt = FALSE;
    }
    లేకపోతే
    {
	GListModel  *controllers;
	పూర్ణాంకం	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_వస్తువులు(controllers);
	కొరకు (పూర్ణాంకం i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_అంశం(controllers, i);
	    ఒకవేళ (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_సమితి_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_కిటికీ_సమితి_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    ఒకవేళ (buttons != ఖాళీ)
    {
	GtkWidget   *but_box;
	అక్షరం	    **buttons_arr; // Note that array is allocated, not strings
	పూర్ణాంకం	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	ఒకవేళ (vim_strchr(p_go, GO_VERTICAL) != ఖాళీ)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	లేకపోతే
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_సమితి_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_సమితి_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = విభజించు_button_string(buttons, &n_buttons, &tofree);

	but_స్థితులు = g_malloc_n(n_buttons, పరిమాణం(DialogButtonState));

	కొరకు (పూర్ణాంకం i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_స్థితులు + i;

	    but_label = CONVERT_గమ్యం_UTF8((char_u *)buttons_arr[i]);
	    ఒకవేళ (but_label == ఖాళీ)
		కొనసాగు;

	    but = gtk_button_new_with_mnemonic((అక్షరం *)but_label);
	    ఒకవేళ (i == def_but - 1)
		gtk_కిటికీ_సమితి_default_widget(win, but);
	    gtk_box_append(GTK_BOX(but_box), but);
	    CONVERT_గమ్యం_UTF8_FREE(but_label);

	    but_state->but_idx = i;
	    but_state->response = &response;
	    but_state->పూర్తి = &పూర్తి;

	    g_signal_connect(but, "clicked",
		    G_CALLBACK(dialog_button_clicked_cb), but_state);
	}
	g_free(buttons_arr);
    }

    gtk_కిటికీ_present(win);

    అయితే (!పూర్తి && !win_closed)
	g_main_context_iteration(ఖాళీ, TRUE);

    ఒకవేళ (పూర్తి)
    {
	ఒకవేళ (textfield != ఖాళీ)
	{
	    // Get the text the user entered
	    char_u *పాఠ్యం;

	    పాఠ్యం = (char_u *)gtk_editable_get_పాఠ్యం(GTK_EDITABLE(నమోదు_2));
	    పాఠ్యం = CONVERT_FROM_UTF8(పాఠ్యం);
	    vim_strncpy(textfield, పాఠ్యం, IOSIZE - 1);
	    CONVERT_FROM_UTF8_FREE(పాఠ్యం);
	}

	gtk_కిటికీ_destroy(win);
    }
    g_free(but_స్థితులు);
    g_free(tofree);

    // Vim buttons are indexed starting from one.
    తిరిగి response == -1 ? 0 : response + 1;
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

రకనిర్వచనం నిర్మాణం
{
    GtkWidget *dialog;
    GtkWidget *what;	    // Find what entry
    GtkWidget *with;	    // Replace with entry
    GtkWidget *wword;	    // Whole word check
    GtkWidget *mcase;	    // Match case check
    GtkWidget *పైకి;	    // Direction up radio
    GtkWidget *కిందికి;	    // Direction down radio
    GtkWidget *కనుగొను;	    // 'Find Next' action button
    GtkWidget *replace;	    // 'Replace With' action button
    GtkWidget *all;	    // 'Replace All' action button
} Shared_కనుగొను_Replace_2;

స్థిర Shared_కనుగొను_Replace_2 కనుగొను_widgets = {0};
స్థిర Shared_కనుగొను_Replace_2 repl_widgets = {0};

    స్థిర శూన్య
కనుగొను_replace_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    పూర్ణాంకం			flags;
    char_u		*కనుగొను_పాఠ్యం;
    char_u		*repl_పాఠ్యం;
    gboolean		direction_కిందికి;
    Shared_కనుగొను_Replace_2	*sfr;

    flags = GPOINTER_గమ్యం_INT(దత్తాంశం);

    ఒకవేళ (flags == FRD_FINDNEXT)
    {
	repl_పాఠ్యం = ఖాళీ;
	sfr = &కనుగొను_widgets;
    }
    లేకపోతే
    {
	repl_పాఠ్యం = (char_u *)gtk_editable_get_పాఠ్యం(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    కనుగొను_పాఠ్యం = (char_u *)gtk_editable_get_పాఠ్యం(GTK_EDITABLE(sfr->what));
    direction_కిందికి = gtk_check_button_get_చురుకైన(
	    GTK_CHECK_BUTTON(sfr->కిందికి));

    ఒకవేళ (gtk_check_button_get_చురుకైన(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_సంపూర్ణ_మాట;
    ఒకవేళ (gtk_check_button_get_చురుకైన(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_సరిపోలిక_CASE;

    repl_పాఠ్యం = CONVERT_FROM_UTF8(repl_పాఠ్యం);
    కనుగొను_పాఠ్యం = CONVERT_FROM_UTF8(కనుగొను_పాఠ్యం);
    gui_do_findrepl(flags, కనుగొను_పాఠ్యం, repl_పాఠ్యం, direction_కిందికి);
    CONVERT_FROM_UTF8_FREE(repl_పాఠ్యం);
    CONVERT_FROM_UTF8_FREE(కనుగొను_పాఠ్యం);
}

    స్థిర శూన్య
dialog_destroyed_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం)
{
    *(GtkWidget **)దత్తాంశం = ఖాళీ;
}

    స్థిర శూన్య
నమోదు_మారింది_cb(GtkWidget *నమోదు_2, GtkWidget *dialog)
{
    స్థిరరాశి gchar	*నమోదు_పాఠ్యం;
    gboolean	nonempty;

    నమోదు_పాఠ్యం = gtk_editable_get_పాఠ్యం(GTK_EDITABLE(నమోదు_2));

    ఒకవేళ (!నమోదు_పాఠ్యం)
	తిరిగి; // Shouldn't happen

    nonempty = (నమోదు_పాఠ్యం[0] != '\0');

    ఒకవేళ (dialog == కనుగొను_widgets.dialog)
	gtk_widget_సమితి_sensitive(కనుగొను_widgets.కనుగొను, nonempty);

    ఒకవేళ (dialog == repl_widgets.dialog)
    {
	gtk_widget_సమితి_sensitive(repl_widgets.కనుగొను, nonempty);
	gtk_widget_సమితి_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_సమితి_sensitive(repl_widgets.all, nonempty);
    }
}

    స్థిర gboolean
కనుగొను_కీలకం_pressed_cb(
	Gtk_Event_Controller_కీలకం	*controller వాడని,
	guint			keyval,
	guint			keycode వాడని,
	GdkModifierType		state,
	Shared_కనుగొను_Replace_2	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    ఒకవేళ (state & (GDK_CONTROL_ముసుగు | GDK_జరుపు_ముసుగు))
	తిరిగి FALSE;

    // the Escape key synthesizes a cancellation action
    ఒకవేళ (keyval == GDK_కీలకం_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_కిటికీ_destroy(GTK_కిటికీ(frdp->dialog));
	తిరిగి TRUE;
    }

    తిరిగి FALSE;
}

    స్థిర శూన్య
నమోదు_activate_cb(GtkWidget *widget వాడని, శూన్య *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    స్థిర శూన్య
కనుగొను_replace_dialog_create(char_u *arg, పూర్ణాంకం do_replace)
{
    Shared_కనుగొను_Replace_2	*frdp;
    char_u		*నమోదు_పాఠ్యం;
    పూర్ణాంకం			wword = FALSE;
    పూర్ణాంకం			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*కీలకం_controller;

    frdp = do_replace ? &repl_widgets : &కనుగొను_widgets;
    నమోదు_పాఠ్యం = get_కనుగొను_dialog_పాఠ్యం(arg, &wword, &mcase);

    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = నమోదు_పాఠ్యం;
	నమోదు_పాఠ్యం = string_convert(&output_conv, నమోదు_పాఠ్యం, ఖాళీ);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    ఒకవేళ (frdp->dialog != ఖాళీ)
    {
	ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	{
	    gtk_editable_సమితి_పాఠ్యం(GTK_EDITABLE(frdp->what),
		    (అక్షరం *)నమోదు_పాఠ్యం);
	    gtk_check_button_సమితి_చురుకైన(GTK_CHECK_BUTTON(frdp->wword),
		    (gboolean)wword);
	    gtk_check_button_సమితి_చురుకైన(GTK_CHECK_BUTTON(frdp->mcase),
		    (gboolean)mcase);
	}
	gtk_కిటికీ_present(GTK_కిటికీ(frdp->dialog));

	// For :promptfind dialog, always give keyboard focus to 'what' entry.
	// For :promptrepl dialog, give it to 'with' entry if 'what' has a
	// non-empty entry; otherwise, to 'what' entry.
	gtk_widget_grab_focus(frdp->what);
	ఒకవేళ (do_replace && g_utf8_strlen(
		    gtk_editable_get_పాఠ్యం(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(నమోదు_పాఠ్యం);
	తిరిగి;
    }

    // Create a new dialog window.
    frdp->dialog = gtk_కిటికీ_new();
    gtk_కిటికీ_సమితి_transient_for(GTK_కిటికీ(frdp->dialog),
	    GTK_కిటికీ(gui.mainwin));
    gtk_కిటికీ_సమితి_destroy_with_మాతృ(GTK_కిటికీ(frdp->dialog), TRUE);
    gtk_కిటికీ_సమితి_title(GTK_కిటికీ(frdp->dialog),
	    do_replace ? _("VIM - Search and Replace...")
	    : _("VIM - Search..."));
    gtk_కిటికీ_సమితి_resizable(GTK_కిటికీ(frdp->dialog), FALSE);

    g_signal_connect(frdp->dialog, "destroy",
	    G_CALLBACK(dialog_destroyed_cb), &frdp->dialog);

    vertbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
    gtk_widget_సమితి_margin_ప్రారంభం(vertbox, 12);
    gtk_widget_సమితి_margin_చివర(vertbox, 12);
    gtk_widget_సమితి_margin_శిఖరం(vertbox, 12);
    gtk_widget_సమితి_margin_bottom(vertbox, 12);
    gtk_కిటికీ_సమితి_బిడ్డ(GTK_కిటికీ(frdp->dialog), vertbox);

    // Grid for labels + entries
    grid = gtk_grid_new();
    gtk_grid_సమితి_అడ్డువరుస_spacing(GTK_GRID(grid), 6);
    gtk_grid_సమితి_స్తంభం_spacing(GTK_GRID(grid), 6);
    gtk_box_append(GTK_BOX(vertbox), grid);

    // "Find what:" label + entry
    tmp = gtk_label_new(_("Find what:"));
    gtk_label_సమితి_xalign(GTK_LABEL(tmp), 0.0);
    gtk_grid_attach(GTK_GRID(grid), tmp, 0, 0, 1, 1);

    frdp->what = gtk_నమోదు_new();
    gtk_widget_సమితి_hexpand(frdp->what, TRUE);
    sensitive = (నమోదు_పాఠ్యం != ఖాళీ && నమోదు_పాఠ్యం[0] != NUL);
    ఒకవేళ (నమోదు_పాఠ్యం != ఖాళీ)
	gtk_editable_సమితి_పాఠ్యం(GTK_EDITABLE(frdp->what), (అక్షరం *)నమోదు_పాఠ్యం);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    ఒకవేళ (do_replace)
    {
	// "Replace with:" label + entry
	tmp = gtk_label_new(_("Replace with:"));
	gtk_label_సమితి_xalign(GTK_LABEL(tmp), 0.0);
	gtk_grid_attach(GTK_GRID(grid), tmp, 0, 1, 1, 1);

	frdp->with = gtk_నమోదు_new();
	gtk_widget_సమితి_hexpand(frdp->with, TRUE);
	gtk_grid_attach(GTK_GRID(grid), frdp->with, 1, 1, 1, 1);
	gtk_నమోదు_సమితి_activates_default(GTK_నమోదు(frdp->with), TRUE);

	// Make the entry activation only change the input focus onto the
	// with item.
	gtk_నమోదు_సమితి_activates_default(GTK_నమోదు(frdp->what), FALSE);
	g_signal_connect(G_OBJECT(frdp->what), "activate",
		G_CALLBACK(నమోదు_activate_cb), frdp->with);
    }
    లేకపోతే
	// Make the entry activation do the search.
	gtk_నమోదు_సమితి_activates_default(GTK_నమోదు(frdp->what), TRUE);

    // Checkboxes
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    frdp->wword = gtk_check_button_new_with_label(_("Match whole word only"));
    gtk_check_button_సమితి_చురుకైన(GTK_CHECK_BUTTON(frdp->wword),
	    (gboolean)wword);
    gtk_box_append(GTK_BOX(hbox), frdp->wword);

    frdp->mcase = gtk_check_button_new_with_label(_("Match case"));
    gtk_check_button_సమితి_చురుకైన(GTK_CHECK_BUTTON(frdp->mcase),
	    (gboolean)mcase);
    gtk_box_append(GTK_BOX(hbox), frdp->mcase);

    // Direction radio buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    tmp = gtk_label_new(_("Direction:"));
    gtk_box_append(GTK_BOX(hbox), tmp);

    frdp->పైకి = gtk_check_button_new_with_label(_("Up"));
    gtk_box_append(GTK_BOX(hbox), frdp->పైకి);

    frdp->కిందికి = gtk_check_button_new_with_label(_("Down"));
    gtk_check_button_సమితి_సమూహం(GTK_CHECK_BUTTON(frdp->కిందికి),
	    GTK_CHECK_BUTTON(frdp->పైకి));
    gtk_check_button_సమితి_చురుకైన(GTK_CHECK_BUTTON(frdp->కిందికి), TRUE);
    gtk_box_append(GTK_BOX(hbox), frdp->కిందికి);

    // Action buttons
    hbox = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);
    gtk_widget_సమితి_halign(hbox, GTK_ALIGN_చివర);
    gtk_box_append(GTK_BOX(vertbox), hbox);

    btn = gtk_button_new_with_label(_("Find Next"));
    gtk_widget_సమితి_sensitive(btn, sensitive);
    frdp->కనుగొను = btn;

    కీలకం_controller = gtk_event_controller_కీలకం_new();
    g_signal_connect(కీలకం_controller, "key-pressed",
	    G_CALLBACK(కనుగొను_కీలకం_pressed_cb), frdp);
    gtk_widget_కూడు_controller(GTK_WIDGET(frdp->dialog), కీలకం_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(నమోదు_మారింది_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_కిటికీ_సమితి_default_widget(GTK_కిటికీ(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(కనుగొను_replace_cb),
	    GINT_గమ్యం_సూచిక(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    ఒకవేళ (do_replace)
    {
	btn = gtk_button_new_with_label(_("Replace"));
	gtk_widget_సమితి_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(కనుగొను_replace_cb),
		GINT_గమ్యం_సూచిక(FRD_REPLACE));
	frdp->replace = btn;
	gtk_box_append(GTK_BOX(hbox), btn);

	btn = gtk_button_new_with_label(_("Replace All"));
	gtk_widget_సమితి_sensitive(btn, sensitive);
	g_signal_connect(btn, "clicked", G_CALLBACK(కనుగొను_replace_cb),
		GINT_గమ్యం_సూచిక(FRD_REPLACEALL));
	frdp->all = btn;
	gtk_box_append(GTK_BOX(hbox), btn);
    }

    btn = gtk_button_new_with_label(_("Close"));
    g_signal_connect_swapped(btn, "clicked",
	    G_CALLBACK(gtk_కిటికీ_destroy), frdp->dialog);
    gtk_box_append(GTK_BOX(hbox), btn);

    gtk_కిటికీ_present(GTK_కిటికీ(frdp->dialog));
    gtk_widget_grab_focus(frdp->what);
    ఒకవేళ (do_replace && నమోదు_పాఠ్యం != ఖాళీ && నమోదు_పాఠ్యం[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(నమోదు_పాఠ్యం);
}

    శూన్య
gui_mch_కనుగొను_dialog(exarg_T *eap)
{
    ఒకవేళ (gui.లోపల_use)
	కనుగొను_replace_dialog_create(eap->arg, FALSE);
}

    శూన్య
gui_mch_replace_dialog(exarg_T *eap)
{
    ఒకవేళ (gui.లోపల_use)
	కనుగొను_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    శూన్య
ex_helpfind(exarg_T *eap వాడని)
{
    do_cmdline_cmd((char_u *)"emenu ToolBar.FindHelp");
}

/*
 * ============================================================
 * Native print dialog
 * ============================================================
 */

#నిర్వచితమైతే USE_GTK4_PRINT_DIALOG
స్థిర GtkPrintDialog *print_dialog = ఖాళీ;
స్థిర GtkPrintSettings *print_settings = ఖాళీ;
స్థిర Gtk_పుట_అమరిక *పుట_అమరిక = ఖాళీ;
స్థిర Gtk_Print_అమరిక *print_అమరిక = ఖాళీ;
స్థిర GFile *print_file;

    స్థిర శూన్య
print_dialog_cb(GtkPrintDialog *dialog, GAsync_ఫలితం *ఫలితం, gboolean *పూర్తి)
{
    GError *error = ఖాళీ;
    print_అమరిక = gtk_print_dialog_అమరిక_ముగింపు(dialog, ఫలితం, &error);

    ఒకవేళ (print_అమరిక == ఖాళీ)
    {
	semsg(_(e_print_error_str), error->సందేశం_2);
	g_error_free(error);
    }
    *పూర్తి = TRUE;
}

/*
 * Return file to write to on success (allocated string), or NULL on failure
 * (e.g. user cancelled)
 */
    char_u *
gui_gtk4_print_dialog(
	prt_settings_T	*psettings,
	char_u		*jobname,
	ద్విగుణ		*పుట_వెడల్పు,
	ద్విగుణ		*పుట_ఎత్తు)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    పూర్తి = FALSE;
    char_u	    *తాత్కాలిక;

    gtk_print_dialog_సమితి_title(dialog, (స్థిరరాశి అక్షరం *)jobname);
    gtk_print_dialog_సమితి_modal(dialog, TRUE);

    print_dialog = ఖాళీ;
    print_అమరిక = ఖాళీ;

    ఒకవేళ (print_settings == ఖాళీ)
    {
	option_పట్టిక_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	ఒకవేళ (opt->present)
	{
	    ఒకవేళ (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_సమితి_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    లేకపోతే ఒకవేళ (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_సమితి_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    లేకపోతే
		gtk_print_settings_సమితి_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	ఒకవేళ (opt->present)
	{
	    ఒకవేళ (*opt->string == 'y')
		gtk_print_settings_సమితి_orientation(print_settings,
			GTK_పుట_ORIENTATION_PORTRAIT);
	    లేకపోతే
		gtk_print_settings_సమితి_orientation(print_settings,
			GTK_పుట_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	ఒకవేళ (opt->present)
	{
	    gtk_print_settings_సమితి_paper_కొలత(print_settings,
		    gtk_paper_కొలత_new((స్థిరరాశి అక్షరం *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	ఒకవేళ (opt->present)
	{
	    ఒకవేళ (*opt->string == 'y')
		gtk_print_settings_సమితి_collate(print_settings, TRUE);
	    లేకపోతే
		gtk_print_settings_సమితి_collate(print_settings, FALSE);
	}
    }

    ఒకవేళ (పుట_అమరిక == ఖాళీ)
	పుట_అమరిక = gtk_పుట_అమరిక_new();

    gtk_print_dialog_సమితి_print_settings(dialog, print_settings);
    gtk_print_dialog_సమితి_పుట_అమరిక(dialog, పుట_అమరిక);

    gtk_print_dialog_అమరిక(dialog, GTK_కిటికీ(gui.mainwin), ఖాళీ,
	    (GAsyncReadyCallback)print_dialog_cb, &పూర్తి);

    అయితే (!పూర్తి)
	g_main_context_iteration(ఖాళీ, TRUE);

    ఒకవేళ (print_అమరిక == ఖాళీ)
    {
	g_object_unref(dialog);
	g_clear_సూచిక(&print_అమరిక, gtk_print_అమరిక_unref);

	psettings->user_abort_సందేశం = FALSE;
	తిరిగి ఖాళీ;
    }

    g_clear_object(&print_settings);
    g_clear_object(&పుట_అమరిక);
    print_settings = g_object_ref(
	    gtk_print_అమరిక_get_print_settings(print_అమరిక));
    పుట_అమరిక = g_object_ref(gtk_print_అమరిక_get_పుట_అమరిక(print_అమరిక));

    {
	GtkPrintDuplex duplex = gtk_print_settings_get_duplex(print_settings);
	psettings->duplex = duplex != GTK_PRINT_DUPLEX_SIMPLEX;
    }

    psettings->has_color = gtk_print_settings_get_use_color(print_settings);

    *పుట_వెడల్పు = gtk_పుట_అమరిక_get_paper_వెడల్పు(పుట_అమరిక,
	    GTK_UNIT_POINTS);
    *పుట_ఎత్తు = gtk_పుట_అమరిక_get_paper_ఎత్తు(పుట_అమరిక,
	    GTK_UNIT_POINTS);

    తాత్కాలిక  = vim_tempname('p', FALSE);
    ఒకవేళ (తాత్కాలిక == ఖాళీ)
    {
	emsg(_(e_cant_కనుగొను_తాత్కాలిక_file_for_writing));
	g_clear_object(&dialog);
	g_clear_సూచిక(&print_అమరిక, gtk_print_అమరిక_unref);

	psettings->user_abort_సందేశం = FALSE;
	తిరిగి ఖాళీ;
    }
    print_file = g_file_new_for_మార్గం((అక్షరం *)తాత్కాలిక);

    print_dialog = dialog;

    తిరిగి తాత్కాలిక;
}

    స్థిర శూన్య
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsync_ఫలితం	*ఫలితం,
	GFile		*file)
{
    అక్షరం *మార్గం = g_file_get_మార్గం(file);

    (శూన్య)gtk_print_dialog_print_file_ముగింపు(dialog, ఫలితం, ఖాళీ);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(మార్గం);
    g_free(మార్గం);
}


    శూన్య
gui_gtk4_print_ముగింపు(శూన్య)
{
    ఒకవేళ (print_dialog != ఖాళీ)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_కిటికీ(gui.mainwin),
		print_అమరిక, print_file, ఖాళీ,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = ఖాళీ;
	g_clear_సూచిక(&print_అమరిక, gtk_print_అమరిక_unref);
    }
}
#అయితే_ముగింపు // USE_GTK4_PRINT_DIALOG

#నిర్వచితమైతే FEAT_IMAGE_GUI

రకనిర్వచనం నిర్మాణం
{
    GdkTexture *texture;
} image_gdk_T;

రకనిర్వచనం నిర్మాణం
{
    Gsk_Render_కణుపు   **nodes; // Note that elements may be NULL
    పూర్ణాంకం		    n_nodes;
} image_placement_gdk_T;

    పూర్ణాంకం
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    Gdk_జ్ఞాపకం_Format fmt;
    size_t	    stride;
    GBytes	    *బైట్లు;
    size_t	    కొలత;
    పూర్ణాంకం		    w, h;

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    ఒకవేళ (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_జ్ఞాపకం_R8_G8_B8_A8;
    లేకపోతే
	fmt = GDK_జ్ఞాపకం_R8_G8_B8;

    image_get_dimensions(img, &w, &h);

    కొలత = (size_t)w * h * img->fmt;
    stride = w * img->fmt;

    బైట్లు = g_బైట్లు_new(img->దత్తాంశం, కొలత);
    ctx->texture = gdk_జ్ఞాపకం_texture_new(w, h, fmt, బైట్లు, stride);
    g_బైట్లు_unref(బైట్లు);

    img->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_దత్తాంశం;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    స్థిర శూన్య
clear_nodes(image_placement_gdk_T *ctx)
{
    ఒకవేళ (ctx->nodes == ఖాళీ || gui.drawarea == ఖాళీ)
	తిరిగి;

    కొరకు (పూర్ణాంకం i = 0; i < ctx->n_nodes; i++)
    {
	ఒకవేళ (ctx->nodes[i] == ఖాళీ)
	    కొనసాగు;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_కణుపు_unref(ctx->nodes[i]);
    }
    g_clear_సూచిక(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    పూర్ణాంకం
image_placement_gui_init(image_placement_T *ఉంచు)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    ఉంచు->backend_దత్తాంశం = ctx;
    తిరిగి OK;
}

    శూన్య
image_placement_gui_uninit(image_placement_T *ఉంచు)
{
    image_placement_gdk_T *ctx = ఉంచు->backend_దత్తాంశం;

    clear_nodes(ctx);
    vim_free(ctx);
}

    శూన్య
image_placement_gui_draw(image_placement_T *ఉంచు)
{
    image_T		    *img = ఉంచు->img;
    image_placement_gdk_T   *ctx = ఉంచు->backend_దత్తాంశం;
    image_gdk_T		    *ictx = img->backend_దత్తాంశం;
    pixman_box32_t	    *rects;
    పూర్ణాంకం			    n_rects;
    పూర్ణాంకం			    iw, ih;
    Gsk_Render_కణుపు	    *texture_కణుపు;

    rects = pixman_region32_rectangles(&ఉంచు->visible, &n_rects);
    ఒకవేళ (rects == ఖాళీ)
	తిరిగి;

    clear_nodes(ctx);
    ఒకవేళ (n_rects == 0)
	తిరిగి;

    image_get_dimensions(img, &iw, &ih);
    texture_కణుపు = gsk_texture_కణుపు_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(ఉంచు->నిలువు) - FILL_X(ఉంచు->crop_box.x1),
		FILL_Y(ఉంచు->అడ్డువరుస) - FILL_Y(ఉంచు->crop_box.y1)
		+ FILL_Y(ఉంచు->అడ్డువరుస_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    ఒకవేళ (texture_కణుపు == ఖాళీ)
	// Not sure if this can happen...
	తిరిగి;

    ctx->nodes = g_malloc_n(n_rects, పరిమాణం(Gsk_Render_కణుపు *));
    ctx->n_nodes = n_rects;

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	పూర్ణాంకం		అడ్డువరుస, నిలువు;
	పూర్ణాంకం		posx, posy;
	పూర్ణాంకం		x, y, w, h;
	Gsk_Render_కణుపు	*కణుపు;

	image_placement_subrect(ఉంచు, rect, &అడ్డువరుస, &నిలువు, &x, &y, &w, &h, false);
	posx = FILL_X(నిలువు);
	posy = FILL_Y(అడ్డువరుస);

	కణుపు = gsk_clip_కణుపు_new(texture_కణుపు,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	ఒకవేళ (కణుపు != ఖాళీ)
	    vim_draw_area_కూడు_external(VIM_DRAW_AREA(gui.drawarea), కణుపు);

	ctx->nodes[i] = కణుపు;
    }
    gsk_render_కణుపు_unref(texture_కణుపు);
}

    శూన్య
image_placement_gui_clear(image_placement_T *ఉంచు)
{
    image_placement_gdk_T   *ctx = ఉంచు->backend_దత్తాంశం;

    clear_nodes(ctx);
}

#అయితే_ముగింపు

#అయితే_ముగింపు // FEAT_GUI_GTK
