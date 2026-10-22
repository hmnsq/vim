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

#अन्तर्भाव "vim.h"

#ifdef FEAT_GUI_GTK

#अन्तर्भाव <gdk/gdkkeysyms.h>
#अन्तर्भाव <gdk/gdk.h>
#अन्तर्भाव <gtk/gtk.h>
#अन्तर्भाव "gui_gtk4_f.h"
#अन्तर्भाव "gui_gtk4_cb.h"
#अन्तर्भाव "gui_gtk4_da.h"
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
#define No_मूल्य		0x0000
#define Width_मूल्य	0x0004
#define Height_मूल्य	0x0008

    स्थिर पूर्णाङ्क
vim_parse_geometry(ध्रुव वर्ण *str, अचिन्हित पूर्णाङ्क *width, अचिन्हित पूर्णाङ्क *height)
{
    पूर्णाङ्क mask = No_मूल्य;
    वर्ण *end;
    दीर्घ val;

    यदि (str == रिक्त || *str == NUL)
	प्रत्यागम mask;

    // Parse width
    यदि (*str != '+' && *str != '-')
    {
	val = म_से_दीर्घ(str, &end, 10);
	यदि (end != str)
	{
	    *width = (अचिन्हित पूर्णाङ्क)val;
	    mask |= Width_मूल्य;
	    str = end;
	}
    }

    // Parse 'x' or 'X' separator and height
    यदि (*str == 'x' || *str == 'X')
    {
	str++;
	val = म_से_दीर्घ(str, &end, 10);
	यदि (end != str)
	{
	    *height = (अचिन्हित पूर्णाङ्क)val;
	    mask |= Height_मूल्य;
	    str = end;
	}
    }

    प्रत्यागम mask;
}

#यदि defined(FEAT_MOUSESHAPE)
// Last set mouse pointer shape
स्थिर पूर्णाङ्क last_shape = 0;
#endif

#define DEFAULT_FONT	"Monospace 10"

// Cursor blinking state
स्थिर गणना {
    BLINK_NONE,
    BLINK_OFF,
    BLINK_ON
} blink_state = BLINK_NONE;

// GTK4 main loop compatibility
स्थिर पूर्णाङ्क gtk4_main_loop_level = 0;
स्थिर पूर्णाङ्क gtk4_main_loop_quit = FALSE;

#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

प्रकारनिर्देश gboolean timeout_cb_type;

/*
 * Table of special key mappings.
 */
स्थिर संरचना special_कुंजी
{
    guint कुंजी_sym;
    char_u code0;
    char_u code1;
}
ध्रुव special_कुंजियाँ[] =
{
    {GDK_कुंजी_Up,	'k', 'u'},
    {GDK_कुंजी_Down,	'k', 'd'},
    {GDK_कुंजी_बायाँ,	'k', 'l'},
    {GDK_कुंजी_दायाँ,	'k', 'r'},
    {GDK_कुंजी_F1,	'k', '1'},
    {GDK_कुंजी_F2,	'k', '2'},
    {GDK_कुंजी_F3,	'k', '3'},
    {GDK_कुंजी_F4,	'k', '4'},
    {GDK_कुंजी_F5,	'k', '5'},
    {GDK_कुंजी_F6,	'k', '6'},
    {GDK_कुंजी_F7,	'k', '7'},
    {GDK_कुंजी_F8,	'k', '8'},
    {GDK_कुंजी_F9,	'k', '9'},
    {GDK_कुंजी_F10,	'k', ';'},
    {GDK_कुंजी_F11,	'F', '1'},
    {GDK_कुंजी_F12,	'F', '2'},
    {GDK_कुंजी_Help,	'%', '1'},
    {GDK_कुंजी_Undo,	'&', '8'},
    {GDK_कुंजी_Back_Space,	'k', 'b'},
    {GDK_कुंजी_जोड़ो,	'k', 'I'},
    {GDK_कुंजी_Delete,	'k', 'D'},
    {GDK_कुंजी_Home,	'k', 'h'},
    {GDK_कुंजी_End,	'@', '7'},
    {GDK_कुंजी_Prior,	'k', 'P'},
    {GDK_कुंजी_Next,	'k', 'N'},
    {GDK_कुंजी_Print,	'%', '9'},
    {GDK_कुंजी_KP_बायाँ,	'k', 'l'},
    {GDK_कुंजी_KP_दायाँ,	'k', 'r'},
    {GDK_कुंजी_KP_Up,	'k', 'u'},
    {GDK_कुंजी_KP_Down,	'k', 'd'},
    {GDK_कुंजी_KP_जोड़ो,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_कुंजी_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_कुंजी_KP_Home,	'K', '1'},
    {GDK_कुंजी_KP_End,	'K', '4'},
    {GDK_कुंजी_KP_Prior,	'K', '3'},
    {GDK_कुंजी_KP_Next,	'K', '5'},
    {GDK_कुंजी_KP_Add,	'K', '6'},
    {GDK_कुंजी_KP_Subtract, 'K', '7'},
    {GDK_कुंजी_KP_Divide,	'K', '8'},
    {GDK_कुंजी_KP_Multiply, 'K', '9'},
    {GDK_कुंजी_KP_Enter,	'K', 'A'},
    {GDK_कुंजी_KP_Decimal,	'K', 'B'},
    {GDK_कुंजी_KP_0,	'K', 'C'},
    {GDK_कुंजी_KP_1,	'K', 'D'},
    {GDK_कुंजी_KP_2,	'K', 'E'},
    {GDK_कुंजी_KP_3,	'K', 'F'},
    {GDK_कुंजी_KP_4,	'K', 'G'},
    {GDK_कुंजी_KP_5,	'K', 'H'},
    {GDK_कुंजी_KP_6,	'K', 'I'},
    {GDK_कुंजी_KP_7,	'K', 'J'},
    {GDK_कुंजी_KP_8,	'K', 'K'},
    {GDK_कुंजी_KP_9,	'K', 'L'},
    {0, 0, 0}
};

    स्थिर पूर्णाङ्क
keyval_to_string(अचिन्हित पूर्णाङ्क keyval, char_u *string)
{
    पूर्णाङ्क		len;
    guint32	uc;

    uc = gdk_keyval_to_unicode(keyval);
    यदि (uc != 0)
    {
	len = utf_char2bytes((पूर्णाङ्क)uc, string);
    }
    अन्यथा
    {
	len = 1;
	विकल्प (keyval)
	{
	    पक्ष GDK_कुंजी_Tab: पक्ष GDK_कुंजी_KP_Tab: पक्ष GDK_कुंजी_ISO_बायाँ_Tab:
		string[0] = TAB;
		भङ्ग;
	    पक्ष GDK_कुंजी_Linefeed:
		string[0] = NL;
		भङ्ग;
	    पक्ष GDK_कुंजी_Return: पक्ष GDK_कुंजी_ISO_Enter: पक्ष GDK_कुंजी_3270_Enter:
		string[0] = CAR;
		भङ्ग;
	    पक्ष GDK_कुंजी_Escape:
		string[0] = ESC;
		भङ्ग;
	    स्वभाव:
		len = 0;
		भङ्ग;
	}
    }
    string[len] = NUL;
    प्रत्यागम len;
}

    स्थिर पूर्णाङ्क
modifiers_gdk2vim(guint state)
{
    पूर्णाङ्क modifiers = 0;

    यदि (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    यदि (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    यदि (state & GDK_ALT_MASK)
	modifiers |= MOD_MASK_ALT;
    यदि (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    यदि (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;

    प्रत्यागम modifiers;
}

स्थिर GtkWidget *vbox;		// the main vertical box

// Forward declarations for event callbacks
स्थिर gboolean कुंजी_press_event(Gtk_Event_Controller_कुंजी *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
स्थिर शून्य कुंजी_release_event(Gtk_Event_Controller_कुंजी *controller, guint keyval, guint keycode, GdkModifierType state, gpointer data);
स्थिर शून्य button_press_event(GtkGestureClick *gesture, पूर्णाङ्क n_press, द्विगुण x, द्विगुण y, gpointer data);
स्थिर शून्य button_release_event(GtkGestureClick *gesture, पूर्णाङ्क n_press, द्विगुण x, द्विगुण y, gpointer data);
स्थिर शून्य motion_notify_event(GtkEventControllerMotion *controller, द्विगुण x, द्विगुण y, gpointer data);
स्थिर शून्य enter_notify_event(GtkEventControllerMotion *controller, द्विगुण x, द्विगुण y, gpointer data);
स्थिर gboolean scroll_event(GtkEventControllerScroll *controller, द्विगुण dx, द्विगुण dy, gpointer data);
स्थिर शून्य focus_in_event(GtkEventControllerFocus *controller, gpointer data);
स्थिर शून्य focus_out_event(GtkEventControllerFocus *controller, gpointer data);
#ifdef FEAT_DND
स्थिर gboolean drop_cb(GtkDropTargetAsync *target, GdkDrop *drop, द्विगुण x, द्विगुण y, शून्य *data);
#endif
#ifdef FEAT_GUI_TABLINE
स्थिर शून्य tabline_enter_cb(GtkEventController *controller, द्विगुण x, द्विगुण y, शून्य *udata);
स्थिर शून्य on_select_tab(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
स्थिर शून्य on_tab_reordered(GtkNotebook *notebook, gpointer *page, gint idx, gpointer data);
स्थिर VimMenu *create_tabline_popup_menu(शून्य);
स्थिर शून्य tabline_add_css(शून्य);
स्थिर शून्य tabline_menu_press_event(GtkGestureClick *gesture, पूर्णाङ्क n_press, द्विगुण x, द्विगुण y, GtkWidget *popover);
#endif
स्थिर शून्य mainwin_destroy_cb(GObject *object, gpointer data);
स्थिर gboolean delete_event_cb(GtkWindow *window, gpointer data);
स्थिर पूर्णाङ्क query_pointer_pos(पूर्णाङ्क *x, पूर्णाङ्क *y, GdkModifierType *state);
स्थिर शून्य mainwin_fullscreened_cb(GObject *obj, GParamSpec *pspec, gpointer user_data);
स्थिर शून्य set_form_size(पूर्णाङ्क width, पूर्णाङ्क height);
स्थिर शून्य drawarea_realize_cb(GtkWidget *widget, gpointer data);
स्थिर शून्य drawarea_unrealize_cb(GtkWidget *widget, gpointer data);
स्थिर शून्य clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data);
#ifdef FEAT_MENU
स्थिर शून्य show_menubar_popover(शून्य);
#endif

स्थिर ध्रुव वर्ण *prgname = रिक्त;

/*
 * Check if "s" is the option "name" (which includes the leading dash(es)).
 * "value" is set to the value if the option uses 'opt=val' format.
 */
    स्थिर gboolean
arg_match(ध्रुव वर्ण *s, ध्रुव वर्ण *name, वर्ण **मूल्य)
{
    माप_प्रकार  len = म_माप(name);

    यदि (म_भेदन(s, name, len) != 0)
	प्रत्यागम FALSE;
    यदि (s[len] == '=')
	*मूल्य = (वर्ण *)s + len + 1;
    अन्यथा यदि (s[len] != NUL)
	// Something follows the option name that isn't "=": this is a
	// different, longer option (e.g. "-fnord" while matching "-fn").
	प्रत्यागम FALSE;
    प्रत्यागम TRUE;
}

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    शून्य
gui_mch_prepare(पूर्णाङ्क *argc, वर्ण **argv)
{
    // Don't call gtk_init() here.  It will be called in
    // gui_mch_init_check() after the fork.  Calling it before fork
    // breaks the display connection in the child process, causing gvim
    // to fail to start without --nofork.

    पूर्णाङ्क	i = 0;

    यावत् (i < *argc)
    {
	वर्ण	*s = argv[i];
	वर्ण	*मूल्य = रिक्त;
	पूर्णाङ्क	has_inline_मूल्य = FALSE;
	पूर्णाङ्क	n_strip;

	यदि (s[0] != '-' && s[0] != '+')
	{
	    ++i;
	    अनुवर्त;
	}

	यदि (म_अक्षर(s, '=') != रिक्त)
	    has_inline_मूल्य = TRUE;

	// If the value was not given inline (no "="), it would come from the
	// next argv element.  Do not treat that next element as this option's
	// value if it is "--" (end-of-options marker) or is another option.
	यदि (!has_inline_मूल्य)
	{
	    यदि (i + 1 < *argc
		    && म_भेद(argv[i + 1], "--") != 0
		    && !((argv[i + 1][0] == '-' || argv[i + 1][0] == '+')
			&& !vim_isdigit(argv[i + 1][1])))
		मूल्य = argv[i + 1];
	    अन्यथा
		मूल्य = रिक्त;
	}

	यदि (arg_match(s, "-fn", &मूल्य) || arg_match(s, "-font", &मूल्य))
	    font_argument = मूल्य;
	अन्यथा यदि (arg_match(s, "-geom", &मूल्य)
		|| arg_match(s, "-geometry", &मूल्य))
	{
	    यदि (मूल्य != रिक्त)
		gui.geom = vim_strsave((char_u *)मूल्य);
	}
	अन्यथा यदि (arg_match(s, "-bg", &मूल्य)
		|| arg_match(s, "-background", &मूल्य))
	    background_argument = मूल्य;
	अन्यथा यदि (arg_match(s, "-fg", &मूल्य)
		|| arg_match(s, "-foreground", &मूल्य))
	    foreground_argument = मूल्य;
#ifdef FEAT_NETBEANS_INTG
	अन्यथा यदि (म_भेदन(s, "-nb", 3) == 0)
	{
	    gui.dofork = false; // don't fork() when starting GUI
	    netbeansArg = argv[i];
	    has_inline_मूल्य = TRUE; // -nb uses non standard syntax, just
				     // remove the flag.
	}
#endif
	अन्यथा यदि (arg_match(s, "--prg-name", &मूल्य))
	    // GTK4 specific
	    prgname = मूल्य;
	अन्यथा
	{
	    i++;
	    अनुवर्त;
	}

	// Remove the flag from the argument vector.
	n_strip = 1;
	// Move the argument's value as well, but only if it was consumed
	// from a separate argv element (the "-opt=value" form lives inside
	// the flag's own element and is stripped along with it already).
	यदि (मूल्य != रिक्त && !has_inline_मूल्य)
	    n_strip = 2;

	यदि (*argc - n_strip >= i)
	{
	    *argc -= n_strip;
	    यदि (*argc > i)
		mch_memmove(&argv[i], &argv[i + n_strip],
			(*argc - i) * परिमाण(वर्ण *));
	    argv[*argc] = रिक्त;
	}
    }
}

/*
 * Free all GUI related resources.
 */
    शून्य
gui_mch_free_all(शून्य)
{
}

    स्थिर guint
timeout_add(पूर्णाङ्क समय, timeout_cb_type (*callback)(gpointer), पूर्णाङ्क *flagp)
{
    प्रत्यागम g_timeout_add((guint)समय, (GSourceFunc)callback, flagp);
}

    स्थिर शून्य
timeout_remove(guint timer)
{
    g_source_remove(timer);
}

स्थिर long_u blink_waittime = 700;
स्थिर long_u blink_ontime = 400;
स्थिर long_u blink_offtime = 250;
स्थिर guint blink_timer = 0;

    स्थिर timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    यदि (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, रिक्त);
    }
    अन्यथा
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, रिक्त);
    }
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
gui_mch_is_blinking(शून्य)
{
    प्रत्यागम blink_state != BLINK_NONE;
}

    पूर्णाङ्क
gui_mch_is_blink_off(शून्य)
{
    प्रत्यागम blink_state == BLINK_OFF;
}

    शून्य
gui_mch_set_blinking(दीर्घ waittime, दीर्घ on, दीर्घ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

    शून्य
gui_mch_stop_blink(पूर्णाङ्क may_call_gui_update_cursor)
{
    यदि (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    यदि (blink_state == BLINK_OFF && may_call_gui_update_cursor)
	gui_update_cursor(TRUE, FALSE);
    blink_state = BLINK_NONE;
}

    शून्य
gui_mch_start_blink(शून्य)
{
    यदि (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    यदि (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, रिक्त);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
    }
}

    पूर्णाङ्क
gui_mch_early_init_check(पूर्णाङ्क give_message UNUSED)
{
    प्रत्यागम OK;
}

    पूर्णाङ्क
gui_mch_init_check(शून्य)
{
    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.  Also sets WM_CLASS on X11. If "--prg-name" is specified, then use
    // that.
    g_set_prgname(prgname == रिक्त ? "gvim" : prgname);

    // Suppress noisy EGL warnings when GL is not available.  Only set
    // this when actually starting the GUI, so non-GUI invocations are
    // not affected.
    यदि (g_getenv("EGL_LOG_LEVEL") == रिक्त)
	setenv("EGL_LOG_LEVEL", "fatal", 0);

    // Call gtk_init() here after fork().  Calling it before fork() breaks
    // the display connection in the child process.
    gtk_init();
    प्रत्यागम OK;
}

/*
 * Initialise the GUI.  Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    पूर्णाङ्क
gui_mch_init(शून्य)
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
		     G_CALLBACK(delete_event_cb), रिक्त);
    g_signal_connect(G_OBJECT(gui.mainwin), "notify::fullscreened",
		     G_CALLBACK(mainwin_fullscreened_cb), रिक्त);

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
		     G_CALLBACK(on_select_tab), रिक्त);
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), रिक्त);

    {
	GtkEventController *mcontroller = gtk_event_controller_motion_new();

	g_signal_connect(mcontroller, "enter",
		G_CALLBACK(tabline_enter_cb), रिक्त);

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
		     G_CALLBACK(drawarea_realize_cb), रिक्त);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), रिक्त);

    // Set up event controllers.
    {
	GtkEventController *कुंजी_ctrl = gtk_event_controller_कुंजी_new();
	g_signal_connect(कुंजी_ctrl, "key-pressed",
			 G_CALLBACK(कुंजी_press_event), रिक्त);
	g_signal_connect(कुंजी_ctrl, "key-released",
			 G_CALLBACK(कुंजी_release_event), रिक्त);
	gtk_widget_add_controller(gui.drawarea, कुंजी_ctrl);
#ifdef FEAT_XIM
	xim_init();
#endif
    }

    {
	GtkGesture *click = gtk_gesture_click_new();
	gtk_gesture_single_set_button(GTK_GESTURE_SINGLE(click), 0);
	g_signal_connect(click, "pressed",
			 G_CALLBACK(button_press_event), रिक्त);
	g_signal_connect(click, "released",
			 G_CALLBACK(button_release_event), रिक्त);
	gtk_widget_add_controller(gui.drawarea, GTK_EVENT_CONTROLLER(click));
    }

    {
	GtkEventController *motion = gtk_event_controller_motion_new();
	g_signal_connect(motion, "motion",
			 G_CALLBACK(motion_notify_event), रिक्त);
	g_signal_connect(motion, "enter",
			 G_CALLBACK(enter_notify_event), रिक्त);
	gtk_widget_add_controller(gui.drawarea, motion);
    }

    {
	GtkEventController *scroll = gtk_event_controller_scroll_new(
		GTK_EVENT_CONTROLLER_SCROLL_BOTH_AXES
		| GTK_EVENT_CONTROLLER_SCROLL_DISCRETE);
	g_signal_connect(scroll, "scroll",
			 G_CALLBACK(scroll_event), रिक्त);
	gtk_widget_add_controller(gui.drawarea, scroll);
    }

    {
	GtkEventController *focus = gtk_event_controller_focus_new();
	g_signal_connect(focus, "enter",
			 G_CALLBACK(focus_in_event), रिक्त);
	g_signal_connect(focus, "leave",
			 G_CALLBACK(focus_out_event), रिक्त);
	gtk_widget_add_controller(gui.drawarea, focus);
    }

#ifdef FEAT_DND
    // Set up drag-and-drop target for files and text. We use async variant so
    // we can handle html format.
    {
	gui.drop_target = gtk_drop_target_async_new(रिक्त,
		GDK_ACTION_COPY | GDK_ACTION_MOVE);

	gui_gtk_set_dnd_targets();
	g_signal_connect(gui.drop_target, "drop",
			 G_CALLBACK(drop_cb), रिक्त);
	gtk_widget_add_controller(gui.drawarea,
		GTK_EVENT_CONTROLLER(gui.drop_target));
    }
#endif

    gui.border_offset = gui.border_width;

    // Create a blank (invisible) cursor for hiding the mouse pointer.
    gui.blank_pointer = gdk_cursor_new_from_name("none", रिक्त);

    {
	GdkDisplay   *display = gtk_widget_get_display(gui.mainwin);
	GdkClipboard *primary = gdk_display_get_primary_clipboard(display);
	GdkClipboard *regular = gdk_display_get_clipboard(display);

	यदि (primary != रिक्त)
	    g_signal_connect(primary, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_star);
	यदि (regular != रिक्त)
	    g_signal_connect(regular, "changed",
		    G_CALLBACK(clipboard_changed_cb), &clip_plus);
    }

    gui.regular_provider = vim_content_provider_new(&clip_plus);
    gui.primary_provider = vim_content_provider_new(&clip_star);

    प्रत्यागम OK;
}

    शून्य
gui_mch_new_colors(शून्य)
{
    यदि (gui.drawarea != रिक्त && gtk_widget_get_realized(gui.drawarea))
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    पूर्णाङ्क
gui_mch_open(शून्य)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint pixel_width;
    guint pixel_height;

    यदि (gui.geom != रिक्त)
    {
	पूर्णाङ्क		mask;
	अचिन्हित पूर्णाङ्क	w, h;

	mask = vim_parse_geometry((वर्ण *)gui.geom, &w, &h);

	यदि (mask & Width_मूल्य)
	    Columns = w;
	यदि (mask & Height_मूल्य)
	{
	    यदि (p_window > (दीर्घ)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	VIM_CLEAR(gui.geom);
    }
    अन्यथा
    {
	// Use 80x24 as the default GUI size, unless geometry was specified.
	यदि (Columns > 80)
	    Columns = 80;
	यदि (Rows > 24)
	    Rows = 24;
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    pixel_width  += get_menu_tool_width();
    pixel_height += get_menu_tool_height();

    यदि (foreground_argument != रिक्त)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    यदि (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    यदि (background_argument != रिक्त)
	bg_pixel = gui_get_color((char_u *)background_argument);
    यदि (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    यदि (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    अन्यथा
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    set_normal_colors();
    gui_check_colors();
    highlight_gui_started();

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), रिक्त);
    // Resize is handled by GtkForm's size_allocate callback.

    set_form_size((पूर्णाङ्क)pixel_width - get_menu_tool_width(),
	    (पूर्णाङ्क)pixel_height - get_menu_tool_height());

    gtk_window_present(GTK_WINDOW(gui.mainwin));

    // Undo the 80x24 clamp above, gui_init() asks for this size next.  Drain
    // the pending allocation before that, or gui_resize_shell() overwrites it.
    gui_mch_update();

    // Make sure the drawing area gets keyboard focus.
    gtk_widget_grab_focus(gui.drawarea);
    gui_focus_change(TRUE);

    प्रत्यागम OK;
}

    शून्य
gui_mch_exit(पूर्णाङ्क rc UNUSED)
{
    यदि (gui.mainwin != रिक्त)
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
		यदि ((menu->name[0] == ']' || menu_is_popup(menu->name))
			&& menu->submenu_id != रिक्त)
		    gtk_widget_unparent(menu->submenu_id);
	    }
	}
#endif
	gtk_window_destroy(GTK_WINDOW(gui.mainwin));
    }
}

    पूर्णाङ्क
gui_mch_get_winpos(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    // GTK4 does not provide a window position API.
    *x = 0;
    *y = 0;
    प्रत्यागम FAIL;
}

    शून्य
gui_mch_set_winpos(पूर्णाङ्क x UNUSED, पूर्णाङ्क y UNUSED)
{
    // GTK4/Wayland: window positioning not available
}

    पूर्णाङ्क
gui_mch_maximized(शून्य)
{
    प्रत्यागम gtk_window_is_maximized(GTK_WINDOW(gui.mainwin));
}

    शून्य
gui_mch_unmaximize(शून्य)
{
    यदि (gui.mainwin != रिक्त)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

    शून्य
gui_mch_set_fullscreen(पूर्णाङ्क flag)
{
    यदि (gui.mainwin == रिक्त)
	प्रत्यागम;
    यदि (flag)
	gtk_window_fullscreen(GTK_WINDOW(gui.mainwin));
    अन्यथा
	gtk_window_unfullscreen(GTK_WINDOW(gui.mainwin));
}

    स्थिर शून्य
mainwin_fullscreened_cb(GObject *obj,
	GParamSpec *pspec UNUSED, gpointer user_data UNUSED)
{
    // Force a redraw of the drawing area when entering fullscreen mode.
    यदि (gtk_window_is_fullscreen(GTK_WINDOW(obj)))
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
    शून्य
gui_mch_newfont(शून्य)
{
    पूर्णाङ्क w, h;

    // Do not subtract width and height with menubar, toolbar, etc, because
    // those are not part of the shell.
    w = gtk_widget_get_width(gui.formwin);
    h = gtk_widget_get_height(gui.formwin);
    gui_resize_shell(w, h);
}

    शून्य
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    यदि (title != रिक्त && gui.mainwin != रिक्त)
	gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ध्रुव वर्ण *)title);
}

/*
 * Get height of window decorations, that we cannot determine directly. For
 * example, the GtkHeaderBar widget. This is called in gui_resize_shell(), we
 * cannot call it in gui_set_shellsize(), because that may be called before the
 * drawarea/formwin is resized, which may cause the drawarea to be bigger than
 * it actually is (while the window size is up to date), causing a negative
 * "decor_height".
 */
    शून्य
gui_gtk_init_decor_height(शून्य)
{
    पूर्णाङ्क h = gtk_widget_get_height(gui.mainwin);

    यदि (h == 0)
	प्रत्यागम;

    h -= get_menu_tool_height();
    h -= gtk_widget_get_height(gui.formwin);

    gui.decor_height = h;
}

/*
 * Ask for the form widget, and thus the shell, to become "width" by "height"
 * pixels.
 */
    स्थिर शून्य
set_form_size(पूर्णाङ्क width, पूर्णाङ्क height)
{
    // Nothing to do when the form widget already has this size: no allocation
    // would follow and the size request below would never be dropped.
    यदि (gtk_widget_get_width(gui.formwin) == width
	    && gtk_widget_get_height(gui.formwin) == height)
	प्रत्यागम;

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

    शून्य
gui_mch_set_shellsize(पूर्णाङ्क width, पूर्णाङ्क height,
	पूर्णाङ्क min_width UNUSED, पूर्णाङ्क min_height UNUSED,
	पूर्णाङ्क base_width UNUSED, पूर्णाङ्क base_height UNUSED,
	पूर्णाङ्क direction UNUSED)
{
    set_form_size(width, height);

    gui_mch_update();
}

    शून्य
gui_mch_get_screen_dimensions(पूर्णाङ्क *screen_w, पूर्णाङ्क *screen_h)
{
    GdkDisplay *display = gtk_widget_get_display(gui.mainwin);
    GdkSurface *surface = gtk_native_get_surface(GTK_NATIVE(gui.mainwin));

    यदि (surface != रिक्त)
    {
	GdkMonitor *monitor = gdk_display_get_monitor_at_surface(display,
		surface);
	यदि (monitor != रिक्त)
	{
	    GdkRectangle geom;
	    gdk_monitor_get_geometry(monitor, &geom);
	    *screen_w = geom.width;
	    *screen_h = geom.height;
	    प्रत्यागम;
	}
    }

    *screen_w = 800;
    *screen_h = 600;
}

#ifdef FEAT_MENU
    शून्य
gui_mch_enable_menu(पूर्णाङ्क showit)
{
    यदि (gui.menubar != रिक्त)
    {
	gtk_widget_set_visible(gui.menubar, showit);
	// Draw area might become blank after this for some reason, queue a
	// redraw, same for toolbar as well.
	यदि (gui.drawarea != रिक्त)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

#ifdef FEAT_TOOLBAR
    शून्य
gui_mch_show_toolbar(पूर्णाङ्क showit)
{
    यदि (gui.toolbar != रिक्त)
    {
	gtk_widget_set_visible(gui.toolbar, showit);
	यदि (showit)
	    vim_toolbar_set_style(VIM_TOOLBAR(gui.toolbar),
		    toolbar_flags, tbis_flags);
	यदि (gui.drawarea != रिक्त)
	    gtk_widget_queue_draw(gui.drawarea);
    }
}
#endif

    शून्य
gui_mch_set_dark_theme(पूर्णाङ्क dark)
{
    // GTK4: use GtkSettings
    GtkSettings *settings = gtk_settings_get_default();
    यदि (settings != रिक्त)
	g_object_set(settings, "gtk-application-prefer-dark-theme",
		(gboolean)dark, रिक्त);
}

/*
 * ============================================================
 * Font handling
 * ============================================================
 */

    पूर्णाङ्क
gui_mch_adjust_charheight(शून्य)
{
    PangoFontMetrics *metrics;
    पूर्णाङ्क ascent;
    पूर्णाङ्क descent;

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

    प्रत्यागम OK;
}

प्रकारनिर्देश संरचना {
    PangoFontDescription    *result;
    gboolean		    done;
} FontDialogData;

    स्थिर शून्य
font_dialog_finish_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FontDialogData *fdd = (FontDialogData *)data;
    fdd->result = gtk_font_dialog_choose_font_finish(
		    GTK_FONT_DIALOG(source), res, रिक्त);
    fdd->done = TRUE;
}

    स्थिर gboolean
font_filter(gpointer item, gpointer data UNUSED)
{
    यदि (PANGO_IS_FONT_FAMILY(item))
	प्रत्यागम pango_font_family_is_monospace(PANGO_FONT_FAMILY(item));
    यदि (PANGO_IS_FONT_FACE(item))
    {
	PangoFontFamily *family = pango_font_face_get_family(
		PANGO_FONT_FACE(item));
	यदि (family != रिक्त)
	    प्रत्यागम pango_font_family_is_monospace(family);
    }
    प्रत्यागम TRUE;
}

    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkFontDialog	*dlg;
    PangoFontDescription *initial = रिक्त;
    char_u		*fontname = रिक्त;
    FontDialogData	fdd;

    dlg = gtk_font_dialog_new();
    gtk_font_dialog_set_modal(dlg, TRUE);
    gtk_font_dialog_set_filter(dlg,
	    GTK_FILTER(gtk_custom_filter_new(
		    (GtkCustomFilterFunc)font_filter, रिक्त, रिक्त)));

    यदि (oldval != रिक्त && oldval[0] != NUL)
    {
	char_u *oldname;

	यदि (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, रिक्त);
	अन्यथा
	    oldname = oldval;

	यदि (STRLEN(oldname) > 0 && !vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u *p = vim_strnsave(oldname, STRLEN(oldname) + 3);
	    यदि (p != रिक्त)
	    {
		STRCPY(p + STRLEN(p), " 10");
		यदि (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

	initial = pango_font_description_from_string((ध्रुव वर्ण *)oldname);
	यदि (oldname != oldval)
	    vim_free(oldname);
    }
    अन्यथा
	initial = pango_font_description_from_string(DEFAULT_FONT);

    fdd.result = रिक्त;
    fdd.done = FALSE;

    gtk_font_dialog_choose_font(dlg, GTK_WINDOW(gui.mainwin),
	    initial, रिक्त, font_dialog_finish_cb, &fdd);

    यावत् (!fdd.done)
	g_main_context_iteration(रिक्त, TRUE);

    यदि (fdd.result != रिक्त)
    {
	वर्ण *name = pango_font_description_to_string(fdd.result);
	यदि (name != रिक्त)
	{
	    char_u *p;

	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    यदि (p != रिक्त && निवेश_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&निवेश_conv, p, रिक्त);
		vim_free(p);
	    }
	    अन्यथा
		fontname = p;
	}
	pango_font_description_free(fdd.result);
    }

    यदि (initial != रिक्त)
	pango_font_description_free(initial);
    g_object_unref(dlg);

    प्रत्यागम fontname;
}

/*
 * Build a table of glyphs for ASCII characters 32..126.
 * This avoids the overhead of itemize+shape for the common case.
 */
    स्थिर शून्य
ascii_glyph_table_init(शून्य)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    पूर्णाङ्क		    i;

    यदि (gui.ascii_glyphs != रिक्त)
	pango_glyph_string_free(gui.ascii_glyphs);
    यदि (gui.ascii_font != रिक्त)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = रिक्त;
    gui.ascii_font   = रिक्त;

    प्रति (i = 0; i < 128; ++i)
    {
	यदि (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	अन्यथा
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ध्रुव वर्ण *)ascii_chars,
			      0, परिमाण(ascii_chars), attr_list, रिक्त);

    यदि (item_list != रिक्त && item_list->next == रिक्त)
    {
	PangoItem   *item;
	पूर्णाङ्क	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ध्रुव वर्ण *)ascii_chars, परिमाण(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	यदि (gui.ascii_glyphs->अंक_glyphs == (पूर्णाङ्क)परिमाण(ascii_chars))
	{
	    प्रति (i = 0; i < gui.ascii_glyphs->अंक_glyphs; ++i)
	    {
		PangoGlyphGeometry *geom;

		geom = &gui.ascii_glyphs->glyphs[i].geometry;
		geom->x_offset += MAX(0, width - geom->width) / 2;
		geom->width = width;
	    }
	}
	अन्यथा
	{
	    pango_glyph_string_free(gui.ascii_glyphs);
	    gui.ascii_glyphs = रिक्त;
	    g_object_unref(gui.ascii_font);
	    gui.ascii_font = रिक्त;
	}
    }

    g_list_foreach(item_list, (GFunc)(शून्य *)&pango_item_free, रिक्त);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

    स्थिर शून्य
get_styled_font_variants(शून्य)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = FALSE;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);
    यदि (plain_font == रिक्त)
	प्रत्यागम;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    यदि (bold_font != रिक्त)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    g_object_unref(plain_font);
}

    पूर्णाङ्क
gui_mch_init_font(char_u *font_name, पूर्णाङ्क fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    पूर्णाङ्क			    width;

    यदि (font_name == रिक्त)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);
    यदि (font_desc == रिक्त)
	प्रत्यागम FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, रिक्त);
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE / 2) / PANGO_SCALE;
    यदि (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // im window position depends on cursor size which depends on font metrics
    // update the position after we've initialized font. Make sure to not go out
    // of bounds of the screen.
    {
	पूर्णाङ्क im_row = gui.row < screen_Rows ? gui.row : screen_Rows - 1;
	पूर्णाङ्क im_col = gui.col < screen_Columns ? gui.col : screen_Columns - 1;
	im_set_position(im_row, im_col);
    }
    प्रत्यागम OK;
}

    GuiFont
gui_mch_get_font(char_u *name, पूर्णाङ्क report_error)
{
    PangoFontDescription *font;

    यदि (name == रिक्त)
	प्रत्यागम रिक्त;

    font = pango_font_description_from_string((ध्रुव वर्ण *)name);
    यदि (font == रिक्त)
    {
	यदि (report_error)
	    semsg(_(e_unknown_font_str), name);
	प्रत्यागम रिक्त;
    }

    // Ensure a size is set
    यदि (pango_font_description_get_size(font) <= 0)
	pango_font_description_set_size(font, 10 * PANGO_SCALE);

    प्रत्यागम font;
}

    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    यदि (font != NOFONT)
    {
	वर्ण *desc = pango_font_description_to_string(font);
	char_u *ret = vim_strsave((char_u *)desc);
	g_free(desc);
	प्रत्यागम ret;
    }
    प्रत्यागम रिक्त;
}

    शून्य
gui_mch_free_font(GuiFont font)
{
    यदि (font != NOFONT)
	pango_font_description_free(font);
}

    शून्य
gui_mch_expand_font(
    optexpand_T	*args,
    शून्य	*param,
    पूर्णाङ्क		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = रिक्त;
    पूर्णाङ्क			n_families = 0;
    पूर्णाङ्क			wide = *(पूर्णाङ्क *)param;

    यदि (args->oe_include_orig_val && *args->oe_opt_मूल्य == NUL && !wide)
    {
	// If guifont is empty, suggest the default so the user can modify it.
	यदि (add_match((char_u *)DEFAULT_FONT) != OK)
	    प्रत्यागम;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    प्रति (पूर्णाङ्क i = 0; i < n_families; i++)
    {
	यदि (!wide && !pango_font_family_is_monospace(font_families[i]))
	    अनुवर्त;

	ध्रुव वर्ण *fam_name = pango_font_family_get_name(font_families[i]);
	यदि (निवेश_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&निवेश_conv,
		    (char_u *)fam_name, रिक्त);
	    यदि (buf != रिक्त)
	    {
		यदि (add_match(buf) != OK)
		{
		    vim_free(buf);
		    भङ्ग;
		}
		vim_free(buf);
	    }
	    अन्यथा
		भङ्ग;
	}
	अन्यथा
	{
	    यदि (add_match((char_u *)fam_name) != OK)
		भङ्ग;
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
    यदि (!gui.in_use)
	प्रत्यागम INVALCOLOR;

    यदि (name != रिक्त)
	प्रत्यागम gui_get_color_cmn(name);

    प्रत्यागम INVALCOLOR;
}

    guicolor_T
gui_mch_get_rgb_color(पूर्णाङ्क r, पूर्णाङ्क g, पूर्णाङ्क b)
{
    प्रत्यागम gui_get_rgb_color_cmn(r, g, b);
}

    स्थिर GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = (color & 0xff) / 255.0;
    rgba.alpha = 1.0;
    प्रत्यागम rgba;
}

    शून्य
gui_mch_set_fg_color(guicolor_T color)
{
    *gui.fgcolor = color_to_rgba(color);
}

    शून्य
gui_mch_set_bg_color(guicolor_T color)
{
    *gui.bgcolor = color_to_rgba(color);
}

    शून्य
gui_mch_set_sp_color(guicolor_T color)
{
    *gui.spcolor = color_to_rgba(color);
}

    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
    प्रत्यागम pixel;
}

/*
 * ============================================================
 * Drawing
 * ============================================================
 */

    शून्य
gui_gtk4_update_size(शून्य)
{
    vim_draw_area_set_size(VIM_DRAW_AREA(gui.drawarea),
	    gui.अंक_rows, gui.अंक_cols);
}

#ifdef FEAT_NETBEANS_INTG
    शून्य
gui_gtk4_add_multisign(
	cairo_surface_t *surf,
	पूर्णाङ्क		row,
	पूर्णाङ्क		col,
	पूर्णाङ्क		width,
	पूर्णाङ्क		height)
{
    vim_draw_area_add_multisign(VIM_DRAW_AREA(gui.drawarea), surf, row, col,
	    width, height);
}
#endif

    शून्य
gui_mch_clear_block(पूर्णाङ्क row1, पूर्णाङ्क col1, पूर्णाङ्क row2, पूर्णाङ्क col2)
{
    vim_draw_area_clear_block(VIM_DRAW_AREA(gui.drawarea), row1,
	    col1, row2, col2);
    यदि (gui.drawarea != रिक्त)
	gtk_widget_queue_draw(gui.drawarea);
}

    शून्य
gui_mch_clear_all(शून्य)
{
    vim_draw_area_clear(VIM_DRAW_AREA(gui.drawarea));
    यदि (gui.drawarea != रिक्त)
	gtk_widget_queue_draw(gui.drawarea);
}

    शून्य
gui_mch_delete_पंक्तियाँ(पूर्णाङ्क row, पूर्णाङ्क अंक_पंक्तियाँ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row, row + अंक_पंक्तियाँ, gui.scroll_region_bot,
	    gui.scroll_region_बायाँ, gui.scroll_region_दायाँ);
    gui_clear_block(
	    gui.scroll_region_bot - अंक_पंक्तियाँ + 1, gui.scroll_region_बायाँ,
	    gui.scroll_region_bot, gui.scroll_region_दायाँ);

    gtk_widget_queue_draw(gui.drawarea);
}

    शून्य
gui_mch_जोड़ो_पंक्तियाँ(पूर्णाङ्क row, पूर्णाङ्क अंक_पंक्तियाँ)
{
    vim_draw_area_move_block(VIM_DRAW_AREA(gui.drawarea),
	    row + अंक_पंक्तियाँ, row, gui.scroll_region_bot - अंक_पंक्तियाँ,
	    gui.scroll_region_बायाँ, gui.scroll_region_दायाँ);
    gui_clear_block(
	    row, gui.scroll_region_बायाँ,
	    row + अंक_पंक्तियाँ - 1, gui.scroll_region_दायाँ);

    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * See vim_draw_area_set_cursor() for more information
 */
    शून्य
gui_gtk4_draw_cursor(guicolor_T bg, guicolor_T fg, पूर्णाङ्क w, पूर्णाङ्क h)
{
    gui_mch_set_bg_color(bg);
    gui_mch_set_fg_color(fg);
    vim_draw_area_set_cursor(VIM_DRAW_AREA(gui.drawarea), w, h);
    gtk_widget_queue_draw(gui.drawarea);
}

    शून्य
gui_mch_flash(पूर्णाङ्क msec)
{
    // Invert the screen, wait, then invert back
    gui_mch_invert_rectangle(0, 0, (पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Columns - 1);
    gui_mch_flush();
    ui_delay((दीर्घ)msec, TRUE);
    gui_mch_invert_rectangle(0, 0, (पूर्णाङ्क)Rows - 1, (पूर्णाङ्क)Columns - 1);
}

    शून्य
gui_mch_invert_rectangle(पूर्णाङ्क r, पूर्णाङ्क c, पूर्णाङ्क nr, पूर्णाङ्क nc)
{
    vim_draw_area_invert_block(VIM_DRAW_AREA(gui.drawarea), r, c, nr, nc);
    gtk_widget_queue_draw(gui.drawarea);
}

/*
 * ============================================================
 * Event handling
 * ============================================================
 */

    स्थिर gboolean
कुंजी_press_event(Gtk_Event_Controller_कुंजी *controller UNUSED,
	guint कुंजी_sym, guint keycode UNUSED,
	GdkModifierType state, gpointer data UNUSED)
{
    char_u	string[32], string2[32];
    पूर्णाङ्क		len;
    पूर्णाङ्क		i;
    पूर्णाङ्क		modifiers;
    पूर्णाङ्क		कुंजी;
    char_u	*s, *d;

#ifdef FEAT_XIM
    // Let the input method have a go at the key event.
    // If it consumed the event, we're done.
    यदि (xic != रिक्त)
    {
	GdkEvent *event = gtk_event_controller_get_current_event(
		GTK_EVENT_CONTROLLER(controller));
	यदि (event != रिक्त && gtk_im_context_filter_keypress(xic, event))
	    प्रत्यागम TRUE;
    }
#endif

#ifdef FEAT_MENU
    यदि (कुंजी_sym == GDK_कुंजी_F10 && gui.menubar != रिक्त)
    {
	स्थिर char_u k10[] = {K_SPECIAL, 'k', ';', 0};

	यदि (check_map(k10, State, FALSE, TRUE, FALSE, रिक्त, रिक्त) == रिक्त)
	{
	    show_menubar_popover();
	    प्रत्यागम TRUE;
	}
    }
#endif

    len = keyval_to_string(कुंजी_sym, string2);

    यदि (len > 1 && निवेश_conv.vc_type != CONV_NONE)
	len = convert_निवेश(string2, len, परिमाण(string2));

    s = string2;
    d = string;
    प्रति (i = 0; i < len; ++i)
    {
	*d++ = s[i];
	यदि (d[-1] == CSI && d + 2 < string + परिमाण(string))
	{
	    *d++ = KS_EXTRA;
	    *d++ = (पूर्णाङ्क)KE_CSI;
	}
    }
    len = d - string;

    // Shift-Tab results in Left_Tab
    यदि (कुंजी_sym == GDK_कुंजी_ISO_बायाँ_Tab)
    {
	कुंजी_sym = GDK_कुंजी_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    यदि ((state & GDK_ALT_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// Tell GTK we have not handled the key (so it can handle it).
	प्रत्यागम FALSE;
#endif

    // Check for special keys
    यदि (len == 0 || len == 1)
    {
	प्रति (i = 0; special_कुंजियाँ[i].कुंजी_sym != 0; i++)
	{
	    यदि (special_कुंजियाँ[i].कुंजी_sym == कुंजी_sym)
	    {
		string[0] = CSI;
		string[1] = special_कुंजियाँ[i].code0;
		string[2] = special_कुंजियाँ[i].code1;
		len = -3;
		भङ्ग;
	    }
	}
    }

    यदि (len == 0)
	प्रत्यागम TRUE;

    यदि (len == -3)
	कुंजी = TO_SPECIAL(string[1], string[2]);
    अन्यथा
    {
	string[len] = NUL;
	कुंजी = mb_ptr2char(string);
    }

    modifiers = modifiers_gdk2vim(state);

    कुंजी = simplify_कुंजी(कुंजी, &modifiers);
    यदि (कुंजी == CSI)
	कुंजी = K_CSI;
    यदि (IS_SPECIAL(कुंजी))
    {
	string[0] = CSI;
	string[1] = K_SECOND(कुंजी);
	string[2] = K_THIRD(कुंजी);
	len = 3;
    }
    अन्यथा
    {
	कुंजी = may_adjust_कुंजी_for_ctrl(modifiers, कुंजी);
	modifiers = may_remove_shift_modifier(modifiers, कुंजी);
	len = mb_char2bytes(कुंजी, string);
    }

    यदि (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_निवेश_buf(string2, 3);
    }

    {
	पूर्णाङ्क int_ch = check_for_interrupt(कुंजी, modifiers);
	यदि (int_ch != NUL)
	{
	    trash_निवेश_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    add_to_निवेश_buf(string, len);

    यदि (p_mh)
	gui_mch_mousehide(TRUE);

    प्रत्यागम TRUE;
}

    स्थिर शून्य
कुंजी_release_event(Gtk_Event_Controller_कुंजी *controller UNUSED,
	guint keyval UNUSED, guint keycode UNUSED,
	GdkModifierType state UNUSED, gpointer data UNUSED)
{
}

स्थिर पूर्णाङ्क mouse_timed_out = TRUE;
स्थिर guint mouse_click_timer = 0;

    स्थिर timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    *(पूर्णाङ्क *)data = TRUE;
    प्रत्यागम FALSE;
}

    स्थिर पूर्णाङ्क
modifiers_gdk2mouse(guint state)
{
    पूर्णाङ्क modifiers = 0;

    यदि (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    यदि (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    यदि (state & GDK_ALT_MASK)
	modifiers |= MOUSE_ALT;

    प्रत्यागम modifiers;
}

// GdkModifierType has no mask covering the mouse buttons only.
#define ANY_BUTTON_MASK (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK \
	| GDK_BUTTON3_MASK | GDK_BUTTON4_MASK | GDK_BUTTON5_MASK)

// Track which mouse button is currently pressed for drag detection.
// -1 means no button is pressed (MOUSE_LEFT is 0x00, so can't use 0).
स्थिर पूर्णाङ्क mouse_pressed_button = -1;

स्थिर guint motion_repeat_timer = 0;
स्थिर gboolean motion_repeat_offset = FALSE;
स्थिर पूर्णाङ्क mouse_press_x = -1, mouse_press_y = -1;

    स्थिर शून्य
button_press_event(GtkGestureClick *gesture, पूर्णाङ्क n_press UNUSED,
	द्विगुण x, द्विगुण y, gpointer data UNUSED)
{
    पूर्णाङ्क button;
    पूर्णाङ्क repeated_click = FALSE;
    int_u vim_modifiers;
    guint btn;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    btn = gdk_button_event_get_button(event);

    यदि (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
	    &mouse_timed_out);

    विकल्प (btn)
    {
	पक्ष 1: button = MOUSE_बायाँ; भङ्ग;
	पक्ष 2: button = MOUSE_मध्य; भङ्ग;
	पक्ष 3: button = MOUSE_दायाँ; भङ्ग;
	पक्ष 8: button = MOUSE_X1; भङ्ग;
	पक्ष 9: button = MOUSE_X2; भङ्ग;
	स्वभाव: प्रत्यागम;
    }

    mouse_press_x = (पूर्णाङ्क)x;
    mouse_press_y = (पूर्णाङ्क)y;

    mouse_pressed_button = button;
    vim_modifiers = modifiers_gdk2mouse(state);
    gui_send_mouse_event(button, (पूर्णाङ्क)x, (पूर्णाङ्क)y, repeated_click, vim_modifiers);
}

    स्थिर शून्य
button_release_event(GtkGestureClick *gesture, पूर्णाङ्क n_press UNUSED,
	द्विगुण x, द्विगुण y, gpointer data UNUSED)
{
    पूर्णाङ्क vim_modifiers;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(gesture));
    state = gdk_event_get_modifier_state(event);
    vim_modifiers = modifiers_gdk2mouse(state);

    // If we are repeating motion events, then stop
    यदि (motion_repeat_timer != 0)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    mouse_pressed_button = -1;
    gui_send_mouse_event(MOUSE_RELEASE, (पूर्णाङ्क)x, (पूर्णाङ्क)y, FALSE, vim_modifiers);
}

स्थिर द्विगुण prev_mouse_x = -1.0;
स्थिर द्विगुण prev_mouse_y = -1.0;

स्थिर GdkModifierType cur_state = 0;

    स्थिर timeout_cb_type
mouse_repeat_timer_cb(gpointer data UNUSED)
{
    पूर्णाङ्क x, y;

    यदि (mouse_pressed_button == -1)
    {
	motion_repeat_timer = 0;
	प्रत्यागम G_SOURCE_REMOVE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    यदि (vim_used_in_निवेश_buf() > 10)
	प्रत्यागम G_SOURCE_CONTINUE;

    x = (पूर्णाङ्क)prev_mouse_x;
    y = (पूर्णाङ्क)prev_mouse_y;

    // Fake a motion event.
    //
    // Trick: Pretend the mouse moved to the next character on every other
    // event, otherwise drag events will be discarded, because they are still in
    // the same character.
    यदि (motion_repeat_offset)
	x += gui.char_width;
    motion_repeat_offset = !motion_repeat_offset;

    gui_send_mouse_event(MOUSE_DRAG, x, y, FALSE,
	    modifiers_gdk2mouse(cur_state));

    // Always continue, when button is released, then this timer is removed.
    प्रत्यागम G_SOURCE_CONTINUE;
}

    स्थिर शून्य
motion_notify_event(GtkEventControllerMotion *controller UNUSED,
	द्विगुण x, द्विगुण y, gpointer data UNUSED)
{
    GdkEvent *event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));

    // The button release event may have gone to another widget, e.g. a modal
    // dialog. Forget about the pressed button when no button is down anymore,
    // otherwise moving the mouse would be taken for a drag.
    यदि (mouse_pressed_button >= 0 && event != रिक्त
	    && !(gdk_event_get_modifier_state(event) & ANY_BUTTON_MASK))
    {
	यदि (motion_repeat_timer != 0)
	{
	    timeout_remove(motion_repeat_timer);
	    motion_repeat_timer = 0;
	}
	mouse_pressed_button = -1;
    }

    यदि (mouse_pressed_button >= 0)
    {
	GdkModifierType state;
	पूर्णाङ्क		w, h;

	यदि (event != रिक्त)
	{
	    cur_state = state = gdk_event_get_modifier_state(event);
	    gui_send_mouse_event(MOUSE_DRAG, (पूर्णाङ्क)x, (पूर्णाङ्क)y,
		    FALSE, modifiers_gdk2mouse(state));
	}

	// Only start repeating motion if pointer is outside of draw area (and
	// mouse button is being pressed down). Make frequency of motion
	// repeats depend on how far away the pointer is from the start of the
	// drag.
	//
	w = gtk_widget_get_width(gui.drawarea);
	h = gtk_widget_get_height(gui.drawarea);

	यदि (motion_repeat_timer > 0)
	    timeout_remove(motion_repeat_timer);

	यदि (x < 0 || y < 0 || x >= w || y >= h)
	{
	    पूर्णाङ्क	dx, dy;
	    पूर्णाङ्क offshoot;
	    पूर्णाङ्क	delay;

	    dx = x < 0 ? -x + mouse_press_x : x - mouse_press_x;
	    dy = y < 0 ? -y + mouse_press_y : y - mouse_press_y;

	    offshoot = dx > dy ? dx : dy;

	    यदि (offshoot > 127)
		delay = 5;
	    अन्यथा
		delay = (130 * (127 - offshoot)) / 127 + 5;

	    motion_repeat_timer = timeout_add(delay,
		    mouse_repeat_timer_cb, रिक्त);
	}
	अन्यथा
	    motion_repeat_timer = 0;
    }
    अन्यथा
	gui_mouse_moved((पूर्णाङ्क)x, (पूर्णाङ्क)y);

    // Only unhide if mouse actually moved. GTK seems to send a motion event
    // when switching tabs, causing the cursor to unhide.
    यदि (p_mh && ((prev_mouse_x == -1 || prev_mouse_y == -1)
		|| (भ_असल(prev_mouse_x - x) > 0.05
		    && भ_असल(prev_mouse_y - y) > 0.05)))
	gui_mch_mousehide(FALSE);

    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area. Fixes issues with menu
    // still being focused.
    gtk_widget_grab_focus(gui.drawarea);
}

    स्थिर शून्य
enter_notify_event(GtkEventControllerMotion *controller UNUSED,
	द्विगुण x, द्विगुण y, gpointer data UNUSED)
{
    prev_mouse_x = x;
    prev_mouse_y = y;

    // Make sure keyboard input goes to the drawing area.
    gtk_widget_grab_focus(gui.drawarea);
}

    स्थिर gboolean
scroll_event(GtkEventControllerScroll *controller UNUSED,
	द्विगुण dx UNUSED, द्विगुण dy, gpointer data UNUSED)
{
    पूर्णाङ्क button;
    int_u vim_modifiers;
    पूर्णाङ्क x, y;
    GdkModifierType state;
    GdkEvent *event;

    event = gtk_event_controller_get_current_event(
	    GTK_EVENT_CONTROLLER(controller));
    यदि (event == रिक्त)
	प्रत्यागम FALSE;
    state = gdk_event_get_modifier_state(event);

    यदि (dy < 0)
	button = MOUSE_4;	// scroll up
    अन्यथा यदि (dy > 0)
	button = MOUSE_5;	// scroll down
    अन्यथा यदि (dx < 0)
	button = MOUSE_7;	// scroll left
    अन्यथा यदि (dx > 0)
	button = MOUSE_6;	// scroll right
    अन्यथा
	प्रत्यागम FALSE;

    vim_modifiers = modifiers_gdk2mouse(state);

    gui_mch_getmouse(&x, &y);
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    प्रत्यागम TRUE;
}

    स्थिर शून्य
focus_in_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    यदि (gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(TRUE);
	यदि (blink_state == BLINK_NONE)
	    gui_mch_start_blink();
    }
}

    स्थिर शून्य
focus_out_event(GtkEventControllerFocus *controller UNUSED,
	gpointer data UNUSED)
{
    यदि (!gtk_window_is_active(GTK_WINDOW(gui.mainwin)))
    {
	gui_focus_change(FALSE);
	यदि (blink_state != BLINK_NONE)
	    gui_mch_stop_blink(TRUE);
    }
}

#यदि defined(FEAT_IMAGE)
    स्थिर शून्य
scale_factor_cb(GdkSurface  *surface,
	GParamSpec	    *pspec UNUSED,
	शून्य		    *udata UNUSED)
{
    gui.scale = gdk_surface_get_scale(surface);
    redraw_all_later(UPD_VALID);
}
#endif

    स्थिर शून्य
drawarea_realize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#यदि defined(FEAT_IMAGE)
    // Use GdkSurface, as that handles fractional scale values.
    GdkSurface *surface = gtk_native_get_surface(
	    gtk_widget_get_native(gui.drawarea));

    gui.scale = gdk_surface_get_scale(surface);
    g_signal_connect(G_OBJECT(surface), "notify::scale",
	    G_CALLBACK(scale_factor_cb), रिक्त);
    redraw_all_later(UPD_VALID);
#endif
    gui_mch_new_colors();
}

    स्थिर शून्य
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#ifdef FEAT_XIM
    im_shutdown();
#endif
}

प्रकारनिर्देश गणना
{
    READ_DATA_CLIP,	    // Clipboard data
    READ_DATA_DROP,	    // URI or text DND data
    READ_DATA_DROP_DATA	    // HTML DND data
} ReadDataType;

#define READDATA_BUFSIZE 4096

// Used for reading clipboard and DND data.
प्रकारनिर्देश संरचना
{
    ReadDataType    type;

    GCancellable    *cancel;
    GByteArray	    *arr; // NULL when not needed
    वर्ण	    *buf; // NULL when not needed
} ReadData;

#ifdef FEAT_DND
प्रकारनिर्देश संरचना
{
    ReadData	rd;
    GdkDrop	*drop;
    पूर्णाङ्क		x;
    पूर्णाङ्क		y;
} DropReadData;
#endif

प्रकारनिर्देश संरचना
{
    ReadData	rd;
    Clipboard_T *cbd;
    वर्ण	*mime_type;
} ClipReadData;

    स्थिर ReadData *
read_data_new(ReadDataType type, माप_प्रकार sz)
{
    ReadData *rd = g_malloc0(sz);

    rd->type = type;
    rd->cancel = g_cancellable_new();

    यदि (type == READ_DATA_CLIP || type == READ_DATA_DROP_DATA)
    {
	rd->arr = g_byte_array_new();
	rd->buf = g_malloc(READDATA_BUFSIZE);
    }

    प्रत्यागम rd;
}

    स्थिर शून्य
read_data_free(ReadData *rd)
{
    g_cancellable_cancel(rd->cancel);

    यदि (rd->type == READ_DATA_CLIP)
	g_free(((ClipReadData *)rd)->mime_type);
    अन्यथा यदि (rd->type == READ_DATA_DROP || rd->type == READ_DATA_DROP_DATA)
	g_object_unref(((DropReadData *)rd)->drop);
    g_object_unref(rd->cancel);
    यदि (rd->arr != रिक्त)
	g_byte_array_free(rd->arr, TRUE);
    g_free(rd->buf);
    g_free(rd);
}

#ifdef FEAT_DND
स्थिर शून्य drop_read_text(GdkDrop *drop, char_u *text);
#endif

/*
 * General purpose callback for reading data from an input stream, either from
 * clipboard or DND.
 */
    स्थिर शून्य
read_data_निवेश_cb(
	GInputStream	*stream,
	GAsyncResult	*result,
	ReadData	*rd)
{
    ssize_t r = g_निवेश_stream_read_finish(stream, result, रिक्त);

    यदि (r == -1)
    {
	// Error occurred
	DropReadData *drd = (DropReadData *)rd;

	यदि (rd->type == READ_DATA_DROP_DATA)
	    gdk_drop_finish(drd->drop, 0);
    }
    अन्यथा यदि (r == 0)
    {
	// EOF, don't need to check for READ_DATA_DROP because that uses GType.
	यदि (rd->type == READ_DATA_CLIP)
	{
	    ClipReadData    *crd = (ClipReadData *)rd;
	    दीर्घ	    len;
	    char_u	    *actual, *final;
	    पूर्णाङ्क		    motion_type = MAUTO;
	    char_u	    *tofree = रिक्त;

	    len = (दीर्घ)rd->arr->len;
	    actual = final = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = रिक्त;

	    यदि (clip_convert_data(&final, &len, &motion_type,
			STRCMP(crd->mime_type, VIM_MIMETYPE_NAME) == 0,
			STRCMP(crd->mime_type, VIMENC_MIMETYPE_NAME) == 0,
			&tofree) == OK)
		clip_yank_selection(motion_type, final, len, crd->cbd);
	    g_free(actual);
	    vim_free(tofree);
	}
#ifdef FEAT_DND
	अन्यथा यदि (rd->type == READ_DATA_DROP_DATA)
	{
	    DropReadData    *drd = (DropReadData *)rd;
	    char_u	    *str;

	    // Append NUL
	    g_byte_array_append(rd->arr, (ध्रुव uint8_t *)"", 1);
	    str = g_byte_array_free(rd->arr, FALSE);
	    rd->arr = रिक्त;
	    drop_read_text(drd->drop, str);
	    g_free(str);
	    gdk_drop_finish(drd->drop, gdk_drop_get_actions(drd->drop));
	}
#endif
    }
    अन्यथा
    {
	// Continue reading
	g_byte_array_append(rd->arr, (ध्रुव uint8_t *)rd->buf, r);
	g_निवेश_stream_read_async(stream, rd->buf, READDATA_BUFSIZE,
		G_PRIORITY_ऊपरी,
		rd->cancel, (GAsyncReadyCallback)read_data_निवेश_cb, rd);
	प्रत्यागम;
    }

    read_data_free(rd);
    g_object_unref(stream);
}

#ifdef FEAT_DND
/*
 * Set up for receiving DND items.
 */
    शून्य
gui_gtk_set_dnd_targets(शून्य)
{
    GdkContentFormatsBuilder	*builder = gdk_content_formats_builder_new();
    GdkContentFormats		*formats;

    gdk_content_formats_builder_add_gtype(builder, GDK_TYPE_FILE_LIST);
    gdk_content_formats_builder_add_gtype(builder, G_TYPE_STRING);
    यदि (clip_html)
	gdk_content_formats_builder_add_mime_type(builder, "text/html");

    formats = gdk_content_formats_builder_free_to_formats(builder);
    gtk_drop_target_async_set_formats(gui.drop_target, formats);
    gdk_content_formats_unref(formats);
}

/*
 * Handle textual DND data. Note that this does not finish the drop.
 */
    स्थिर शून्य
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

    यदि (text == रिक्त || *text == NUL || !query_pointer_pos(रिक्त, रिक्त, &state))
	प्रत्यागम;

    dnd_yank_drag_data((char_u *)text, (दीर्घ)STRLEN(text));

    dropkey[2] = modifiers_gdk2vim(state);
    यदि (dropkey[2] != 0)
	add_to_निवेश_buf(dropkey, (पूर्णाङ्क)परिमाण(dropkey));
    अन्यथा
	add_to_निवेश_buf(dropkey + 3, (पूर्णाङ्क)(परिमाण(dropkey) - 3));
}

    स्थिर शून्य
drop_read_मूल्य_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    ध्रुव GValue    *मूल्य = gdk_drop_read_मूल्य_finish(drop, result, रिक्त);
    gboolean	    success = FALSE;

    यदि (मूल्य == रिक्त)
	गच्छ निकास;

    यदि (G_मूल्य_HOLDS(मूल्य, GDK_TYPE_FILE_LIST))
    {
	GSList	*files = g_मूल्य_get_boxed(मूल्य);
	पूर्णाङ्क	nfiles = g_slist_length(files);
	char_u	**fnames;
	पूर्णाङ्क	i;

	यदि (nfiles <= 0)
	    गच्छ निकास;

	fnames = ALLOC_MULT(char_u *, nfiles);
	यदि (fnames == रिक्त)
	    गच्छ निकास;

	i = 0;
	प्रति (GSList *l = files; l != रिक्त; l = l->next)
	{
	    GFile *file = l->data;
	    वर्ण *path = g_file_get_path(file);
	    यदि (path != रिक्त)
		fnames[i++] = vim_strsave((char_u *)path);
	    g_free(path);
	}
	nfiles = i;

	यदि (nfiles > 0)
	    gui_handle_drop(drd->x, drd->y, 0, fnames, nfiles);
	अन्यथा
	    vim_free(fnames);
	success = TRUE;
    }
    अन्यथा यदि (G_मूल्य_HOLDS(मूल्य, G_TYPE_STRING))
    {
	ध्रुव वर्ण *text = g_मूल्य_get_string(मूल्य);

	यदि (text == रिक्त || *text == NUL)
	    गच्छ निकास;

	drop_read_text(drop, (char_u *)text);
	success = TRUE;
    }

निकास:
    gdk_drop_finish(drop, success ? gdk_drop_get_actions(drop) : 0);
    read_data_free(&drd->rd);
}

    स्थिर शून्य
drop_read_cb(GdkDrop *drop, GAsyncResult *result, DropReadData *drd)
{
    GInputStream    *in_stream;
    ध्रुव वर्ण	    *m;

    in_stream = gdk_drop_read_finish(drop, result, &m, रिक्त);
    यदि (in_stream == रिक्त || STRCMP(m, "text/html") != 0)
    {
	read_data_free(&drd->rd);
	gdk_drop_finish(drop, 0);
	प्रत्यागम;
    }

    प्रतिज्ञा(STRCMP(m, "text/html") == 0);

    // GTK docs says not to use blocking read calls, so do it async.
    g_निवेश_stream_read_async(in_stream, drd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ऊपरी,
	    drd->rd.cancel, (GAsyncReadyCallback)read_data_निवेश_cb, drd);
}

// GCancellable of current DND operation, else NULL.
स्थिर GCancellable *dnd_cancel = रिक्त;
स्थिर guint dnd_timeout_id = 0;

    स्थिर gboolean
drop_timeout_cb(शून्य *udata UNUSED)
{
    g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    dnd_timeout_id = 0;

    प्रत्यागम G_SOURCE_REMOVE;
}

/*
 * Drag-and-drop handler for files and text.
 */
    स्थिर gboolean
drop_cb(
	GtkDropTargetAsync  *target UNUSED,
	GdkDrop		    *drop,
	द्विगुण		    x,
	द्विगुण		    y,
	शून्य		    *udata UNUSED)
{
    GdkContentFormats	*formats = gdk_drop_get_formats(drop);
    DropReadData	*drd = रिक्त;

    यदि (gdk_content_formats_contain_gtype(formats, GDK_TYPE_FILE_LIST))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, परिमाण(*drd));
	gdk_drop_read_मूल्य_async(drop, GDK_TYPE_FILE_LIST, G_PRIORITY_ऊपरी,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_मूल्य_cb, drd);
    }
    अन्यथा यदि (clip_html
	    && gdk_content_formats_contain_mime_type(formats, "text/html"))
    {
	स्थिर ध्रुव वर्ण *m[] = {"text/html", रिक्त};

	drd = (DropReadData *)read_data_new(READ_DATA_DROP_DATA, परिमाण(*drd));
	gdk_drop_read_async(drop, m, G_PRIORITY_ऊपरी,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_cb, drd);
    }
    अन्यथा यदि (gdk_content_formats_contain_gtype(formats, G_TYPE_STRING))
    {
	drd = (DropReadData *)read_data_new(READ_DATA_DROP, परिमाण(*drd));
	gdk_drop_read_मूल्य_async(drop, G_TYPE_STRING, G_PRIORITY_ऊपरी,
		drd->rd.cancel, (GAsyncReadyCallback)drop_read_मूल्य_cb, drd);
    }

    यदि (dnd_cancel != रिक्त)
	g_cancellable_cancel(dnd_cancel);
    g_clear_object(&dnd_cancel);
    g_clear_handle_id(&dnd_timeout_id, timeout_remove);

    यदि (drd != रिक्त)
    {
	drd->drop = g_object_ref(drop);
	drd->x = (पूर्णाङ्क)x;
	drd->y = (पूर्णाङ्क)y;
	dnd_cancel = g_object_ref(drd->rd.cancel);

	// Trying to spin the event loop here causes this GTK assertion error:
	// Gtk:ERROR:../gtk/gtk/gtkdrop.c:70:gtk_drop_begin_event: assertion failed: (self->active == FALSE)
	//
	// This shouldn't be an issue, because Vim DND occurs asynchronously in
	// the first place anyways.

	// Add a 3 second timeout
	dnd_timeout_id = timeout_add(3000, drop_timeout_cb, रिक्त);

	प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}
#endif

    स्थिर शून्य
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    gui.mainwin = रिक्त;
    gui.drawarea = रिक्त;
    यदि (!exiting)
	gui_shell_closed();
}

    स्थिर gboolean
delete_event_cb(GtkWindow *window UNUSED, gpointer data UNUSED)
{
    gui_shell_closed();
    प्रत्यागम TRUE;
}

/*
 * ============================================================
 * Misc functions
 * ============================================================
 */

    स्थिर timeout_cb_type
निवेश_timer_cb(gpointer data)
{
    पूर्णाङ्क *timed_out = (पूर्णाङ्क *)data;

    *timed_out = TRUE;
    प्रत्यागम FALSE;	// don't call me again
}

    शून्य
gui_mch_update(शून्य)
{
    पूर्णाङ्क cnt = 0;

    यावत् (g_main_context_pending(रिक्त) && !vim_is_निवेश_buf_full()
								&& ++cnt < 100)
	g_main_context_iteration(रिक्त, TRUE);
}

#ifdef FEAT_JOB_CHANNEL
    स्थिर timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();
    प्रत्यागम TRUE; // Keep repeating
}
#endif

    पूर्णाङ्क
gui_mch_wait_for_chars(दीर्घ wtime)
{
    पूर्णाङ्क		focus;
    guint	timer;
    स्थिर पूर्णाङ्क	timed_out;
    पूर्णाङ्क		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    यदि (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   निवेश_timer_cb, &timed_out);
    अन्यथा
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    यदि (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, रिक्त);
#endif

    focus = gui.in_focus;

    कुरु
    {
	// Stop or start blinking when focus changes
	यदि (gui.in_focus != focus)
	{
	    यदि (gui.in_focus)
		gui_mch_start_blink();
	    अन्यथा
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	यदि (did_add_timer)
	    गच्छ theend;
# endif
#endif

	यदि (gui.mainwin == रिक्त)
	    गच्छ theend;

	// gtk_main_quit() is a wake-up request; consume it so later
	// waits resume.
	यदि (gtk4_main_loop_quit)
	{
	    gtk4_main_loop_quit = FALSE;
	    गच्छ theend;
	}

	यदि (!निवेश_available())
	{
	    ++gtk4_main_loop_level;
	    g_main_context_iteration(रिक्त, TRUE);
	    --gtk4_main_loop_level;
	}

	यदि (निवेश_available())
	{
	    retval = OK;
	    गच्छ theend;
	}
    } यावत् (wtime < 0 || !timed_out);

    gui_mch_update();

theend:
    यदि (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    यदि (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    प्रत्यागम retval;
}

    शून्य
gui_mch_flush(शून्य)
{
    // Ensure the offscreen surface content gets painted to the widget.
    यदि (gui.drawarea != रिक्त)
	gtk_widget_queue_draw(gui.drawarea);
    यदि (gui.mainwin != रिक्त && gtk_widget_get_realized(gui.mainwin))
	gdk_display_flush(gtk_widget_get_display(gui.mainwin));
}

    शून्य
gui_mch_beep(शून्य)
{
    GdkDisplay *display;

    यदि (gui.mainwin != रिक्त && gtk_widget_get_realized(gui.mainwin))
    {
	display = gtk_widget_get_display(gui.mainwin);
	यदि (display != रिक्त)
	    gdk_display_beep(display);
    }
}

    शून्य *
gui_mch_get_display(शून्य)
{
    यदि (gui.mainwin != रिक्त && gtk_widget_get_display(gui.mainwin))
	प्रत्यागम gtk_widget_get_display(gui.mainwin);
    प्रत्यागम रिक्त;
}

    शून्य
gui_mch_iconify(शून्य)
{
    gtk_window_minimize(GTK_WINDOW(gui.mainwin));
}

    शून्य
gui_mch_set_foreground(शून्य)
{
    gtk_window_present(GTK_WINDOW(gui.mainwin));
}

    स्थिर पूर्णाङ्क
query_pointer_pos(पूर्णाङ्क *x, पूर्णाङ्क *y, GdkModifierType *state)
{
    GtkNative	    *native;
    GdkSurface	    *surface;
    GdkDisplay	    *display;
    GdkSeat	    *seat;
    GdkDevice	    *pointer;
    द्विगुण	     sx, sy, nx, ny;
    graphene_point_t src, dst;

    यदि (gui.drawarea == रिक्त)
	प्रत्यागम FALSE;
    native = gtk_widget_get_native(gui.drawarea);
    यदि (native == रिक्त)
	प्रत्यागम FALSE;
    surface = gtk_native_get_surface(native);
    यदि (surface == रिक्त)
	प्रत्यागम FALSE;
    display = gtk_widget_get_display(gui.drawarea);
    यदि (display == रिक्त)
	प्रत्यागम FALSE;
    seat = gdk_display_get_default_seat(display);
    यदि (seat == रिक्त)
	प्रत्यागम FALSE;
    pointer = gdk_seat_get_pointer(seat);
    यदि (pointer == रिक्त)
	प्रत्यागम FALSE;

    यदि (!gdk_surface_get_device_position(surface, pointer, &sx, &sy, state))
	प्रत्यागम FALSE;

    यदि (x != रिक्त && y != रिक्त)
    {
	gtk_native_get_surface_transform(native, &nx, &ny);
	src.x = (भिन्न)(sx - nx);
	src.y = (भिन्न)(sy - ny);
	यदि (!gtk_widget_compute_point(GTK_WIDGET(native), gui.drawarea,
		    &src, &dst))
	    प्रत्यागम FALSE;

	*x = (पूर्णाङ्क)dst.x;
	*y = (पूर्णाङ्क)dst.y;
    }
    प्रत्यागम TRUE;
}

    शून्य
gui_mch_getmouse(पूर्णाङ्क *x, पूर्णाङ्क *y)
{
    यदि (!query_pointer_pos(x, y, रिक्त))
    {
	*x = 0;
	*y = 0;
    }
}

    शून्य
gui_mch_setmouse(पूर्णाङ्क x UNUSED, पूर्णाङ्क y UNUSED)
{
    // GTK4/Wayland: cannot warp pointer
}

    शून्य
gui_mch_mousehide(पूर्णाङ्क hide)
{
    यदि (gui.pointer_hidden == hide)
	प्रत्यागम;

    gui.pointer_hidden = hide;
    यदि (gui.blank_pointer != रिक्त)
    {
	यदि (hide)
	    gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
	अन्यथा
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#अन्यथा
	    gtk_widget_set_cursor(gui.drawarea, रिक्त);
#endif
    }
}

    पूर्णाङ्क
gui_mch_haskey(char_u *name)
{
    पूर्णाङ्क i;

    प्रति (i = 0; special_कुंजियाँ[i].कुंजी_sym != 0; i++)
	यदि (name[0] == special_कुंजियाँ[i].code0
		&& name[1] == special_कुंजियाँ[i].code1)
	    प्रत्यागम OK;
    प्रत्यागम FAIL;
}

    शून्य
gui_mch_forked(शून्य)
{
}

/*
 * Try getting the actual size of the scrollbar, and update gui.scrollbar_width
 * and gui.scrollbar_height.
 */
    स्थिर शून्य
gui_gtk4_update_scrollbar_size(शून्य)
{
    win_T	*wp;
    पूर्णाङ्क		w = -1, h = -1;
    GtkWidget	*sbar;

    FOR_ALL_WINDOWS(wp)
    {
	sbar = wp->w_scrollbars[SBAR_बायाँ].id;

	यदि (sbar == रिक्त || !gtk_widget_get_visible(sbar)
		|| (!gui.which_scrollbars[SBAR_बायाँ]
		    && wp->w_scrollbars[SBAR_दायाँ].id != रिक्त))
	    sbar = wp->w_scrollbars[SBAR_दायाँ].id;

	यदि (sbar != रिक्त && gtk_widget_get_visible(sbar))
	{
	    GtkRequisition  min, nat;
	    पूर्णाङ्क		    sw;

	    // Use preferred size, since widget may not have its size allocated
	    // yet.
	    gtk_widget_get_preferred_size(sbar, &min, &nat);
	    sw = MAX(min.width, nat.width);
	    यदि (sw > 0)
	    {
		w = sw;
		भङ्ग;
	    }
	}

    }

    sbar = gui.bottom_sbar.id;
    यदि (sbar != रिक्त && gtk_widget_get_visible(sbar))
    {
	GtkRequisition min, nat;
	पूर्णाङ्क		    sh;

	gtk_widget_get_preferred_size(sbar, &min, &nat);
	sh = MAX(min.height, nat.height);

	यदि (sh > 0)
	    h = sh;
    }

    यदि (w != -1)
	gui.scrollbar_width = w;
    यदि (h != -1)
	gui.scrollbar_height = h;
}


/*
 * ============================================================
 * Scrollbar
 * ============================================================
 */

    शून्य
gui_mch_enable_scrollbar(scrollbar_T *sb, पूर्णाङ्क flag)
{
    यदि (sb->id != रिक्त)
    {
	gboolean was_visible = gtk_widget_get_visible(sb->id);

	gtk_widget_set_visible(sb->id, flag);

	यदि (!was_visible && flag)
	    gui_gtk4_update_scrollbar_size();
    }
}

#यदि defined(FEAT_MENU)
/*
 * ============================================================
 * Menu stubs
 * ============================================================
 */

    शून्य
gui_mch_menu_grey(vimmenu_T *menu, पूर्णाङ्क grey)
{
    यदि (menu->id == रिक्त)
	प्रत्यागम;
    gtk_widget_set_sensitive(menu->id, !grey);
    gui_mch_update();
}

/*
 * Make menu item hidden or not hidden.
 */
    शून्य
gui_mch_menu_hidden(vimmenu_T *menu, पूर्णाङ्क hidden)
{
    यदि (menu->id == रिक्त)
	प्रत्यागम;
    gtk_widget_set_visible(menu->id, !hidden);
    gui_mch_update();
}

    शून्य
gui_mch_draw_menubar(शून्य)
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
    शून्य
gui_mch_show_tabline(पूर्णाङ्क showit)
{
    यदि (gui.tabline == रिक्त)
	प्रत्यागम;
    gtk_widget_set_visible(gui.tabline, showit);
    gui_mch_update();
}

    पूर्णाङ्क
gui_mch_showing_tabline(शून्य)
{
    प्रत्यागम gui.tabline != रिक्त && gtk_widget_get_visible(gui.tabline);
}

स्थिर पूर्णाङ्क ignore_tabline_evt = FALSE;
// Page hovered over in tab line, zero indicates none
स्थिर पूर्णाङ्क tabpage_hover = 0;

    स्थिर शून्य
tabpage_enter_cb(
	GtkEventController  *controller UNUSED,
	द्विगुण		    x UNUSED,
	द्विगुण		    y UNUSED,
	शून्य		    *udata)
{
    tabpage_hover = GPOINTER_TO_INT(udata);
}

    स्थिर शून्य
tabline_enter_cb(
	GtkEventController  *controller UNUSED,
	द्विगुण		    x UNUSED,
	द्विगुण		    y UNUSED,
	शून्य		    *udata UNUSED)
{
    tabpage_hover = 0;
}

/*
 * CSS for the tab pages line: flat rounded tabs, the current tab with a light
 * background.  Only applies to the notebook with the "vim-tabline" CSS class.
 */
    स्थिर शून्य
tabline_add_css(शून्य)
{
    GtkCssProvider  *provider;
    स्थिर ध्रुव वर्ण *css =
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
# यदि GTK_CHECK_VERSION(4, 12, 0)
    gtk_css_provider_load_from_string(provider, css);
# अन्यथा
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
    स्थिर शून्य
tab_close_clicked_cb(GtkButton *button UNUSED, gpointer data)
{
    पूर्णाङ्क idx = gtk_notebook_page_अंक(GTK_NOTEBOOK(gui.tabline),
							   GTK_WIDGET(data));

    यदि (idx >= 0)
	send_tabline_menu_event(idx + 1, TABLINE_MENU_CLOSE);
}

    शून्य
gui_mch_update_tabline(शून्य)
{
    GtkWidget	*page;
    GtkWidget	*event_box;
    GtkWidget	*label;
    tabpage_T	*tp;
    पूर्णाङ्क		nr = 0;
    पूर्णाङ्क		tab_अंक;
    पूर्णाङ्क		curtabidx = 0;
    char_u	*labeltext;

    यदि (gui.tabline == रिक्त)
	प्रत्यागम;

    ignore_tabline_evt = TRUE;

    प्रति (tp = first_tabpage; tp != रिक्त; tp = tp->tp_next, ++nr)
    {
	GtkNotebookPage	    *page_widget;
	GtkWidget	    *tab_widget;
	GtkEventController  *mcontroller;

	यदि (tp == curtab)
	    curtabidx = nr;

	tab_अंक = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	यदि (page == रिक्त)
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

	    gtk_notebook_जोड़ो_page(GTK_NOTEBOOK(gui.tabline),
		    page, event_box, nr);
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page, TRUE);
	    g_object_set(gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page),
		    "tab-expand", TRUE, रिक्त);
	}

	// Attach motion event controller to detect which tab page is currently
	// hovered over.
	page_widget = gtk_notebook_get_page(GTK_NOTEBOOK(gui.tabline), page);
	mcontroller = gtk_event_controller_motion_new();
	g_signal_connect(mcontroller, "enter", G_CALLBACK(tabpage_enter_cb),
		GINT_TO_POINTER(tab_अंक));
	gtk_event_controller_set_propagation_phase(
		mcontroller, GTK_PHASE_CAPTURE);
	g_object_get(page_widget, "tab", &tab_widget, रिक्त);
	gtk_widget_add_controller(tab_widget, mcontroller);

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	label = g_object_get_data(G_OBJECT(event_box), "label");
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	यदि (label != रिक्त && GTK_IS_LABEL(label))
	    gtk_label_set_text(GTK_LABEL(label), (ध्रुव वर्ण *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_widget_set_tooltip_text(event_box, (ध्रुव gchar *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    यावत् (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != रिक्त)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    // With a single tab the tab is not highlighted and has no close button.
    यदि (nr == 1)
	gtk_widget_add_css_class(gui.tabline, "single-tab");
    अन्यथा
	gtk_widget_remove_css_class(gui.tabline, "single-tab");

    यदि (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    ignore_tabline_evt = FALSE;
}

    शून्य
gui_mch_set_curtab(पूर्णाङ्क nr)
{
    यदि (gui.tabline != रिक्त)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
}

/*
 * Handle selecting one of the tabs.
 */
    स्थिर शून्य
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    यदि (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

/*
 * Handle reordering the tabs (using D&D).
 */
    स्थिर शून्य
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    यदि (ignore_tabline_evt)
	प्रत्यागम;

    यदि ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    अन्यथा
	tabpage_move(idx);
}

/*
 * Handle selecting an item in the tab line popup menu.
 */
    स्थिर शून्य
tabline_menu_event_cb(VimMenuItem *item UNUSED, VimMenuItemEvent event, शून्य *udata)
{
    यदि (event == VIM_MENU_ITEM_CLICKED)
	send_tabline_menu_event(tabpage_hover, GPOINTER_TO_INT(udata));
}

    स्थिर शून्य
add_tabline_menu_item(VimMenu *menu, ध्रुव वर्ण *name, पूर्णाङ्क resp)
{
    VimMenuItem *item = VIM_MENU_ITEM(vim_menu_item_new(name,
		tabline_menu_event_cb, GINT_TO_POINTER(resp)));

    vim_menu_जोड़ो_item(menu, item, -1);
}

/*
 * Create a menu for the tab line.
 */
    स्थिर VimMenu *
create_tabline_popup_menu(शून्य)
{
    VimMenu *menu = VIM_MENU(vim_menu_new());

    add_tabline_menu_item(menu, _("Close Tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, _("New Tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, _("Open Tab..."), TABLINE_MENU_OPEN);

    प्रत्यागम menu;
}

    स्थिर शून्य
tabline_menu_press_event(
	GtkGestureClick *gesture,
	पूर्णाङ्क		n_press UNUSED,
	द्विगुण		x,
	द्विगुण		y,
	GtkWidget	*popover)
{
    guint but;

    but = gtk_gesture_single_get_current_button(GTK_GESTURE_SINGLE(gesture));
    यदि (but == GDK_BUTTON_SECONDARY)
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
    अन्यथा यदि (but == GDK_BUTTON_PRIMARY)
    {
	यदि (tabpage_hover == 0)
	    // Click after all tabs moves to next tab page.  When "x" is
	    // small guess it's the left button.
	    send_tabline_event(x < 50 ? -1 : 0);
    }
    अन्यथा यदि (but == GDK_BUTTON_मध्य)
    {
	यदि (tabpage_hover != 0)
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

#यदि defined(FEAT_SIGN_ICONS)
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)

    शून्य
gui_mch_drawsign(पूर्णाङ्क row, पूर्णाङ्क col, पूर्णाङ्क typenr)
{
    GdkTexture *sign;

    sign = (GdkTexture *)sign_get_image(typenr);
    यदि (sign == रिक्त)
	प्रत्यागम;

    vim_draw_area_add_sign(VIM_DRAW_AREA(gui.drawarea), sign,
	    row, col, SIGN_WIDTH, SIGN_HEIGHT);

    gtk_widget_queue_draw(gui.drawarea);
}

    शून्य *
gui_mch_register_sign(char_u *signfile)
{
    यदि (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GError	    *error = रिक्त;
	GdkTexture  *sign;

	sign = gdk_texture_new_from_filename((ध्रुव वर्ण *)signfile,
							    &error);
	यदि (sign != रिक्त)
	    प्रत्यागम sign;

	semsg("E255: %s", error->message);
	g_error_free(error);
    }
    प्रत्यागम रिक्त;
}

    शून्य
gui_mch_destroy_sign(शून्य *sign)
{
    यदि (sign != रिक्त)
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
    पूर्णाङ्क
gui_gtk_draw_string(पूर्णाङ्क row, पूर्णाङ्क col, char_u *s, पूर्णाङ्क len, पूर्णाङ्क flags)
{
    पूर्णाङ्क cells;

    cells = vim_draw_area_draw_string(VIM_DRAW_AREA(gui.drawarea),
	    row, col, s, len, flags);

    यदि (gui.drawarea != रिक्त)
	gtk_widget_queue_draw(gui.drawarea);

    प्रत्यागम cells;
}

    पूर्णाङ्क
gui_get_x11_windis(Window *win UNUSED, Display **dis UNUSED)
{
    // GTK4: not applicable
    प्रत्यागम FAIL;
}

#यदि defined(FEAT_MENU)
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
    स्थिर char_u *
translate_mnemonic_tag(char_u *name, पूर्णाङ्क use_mnemonic)
{
    char_u  *buf;
    char_u  *psrc;
    char_u  *pdest;
    पूर्णाङ्क	    n_underscores = 0;

    name = CONVERT_TO_UTF8(name);
    यदि (name == रिक्त)
	प्रत्यागम रिक्त;

    प्रति (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	यदि (*psrc == '_')
	    ++n_underscores;

    buf = alloc(psrc - name + n_underscores + 1);
    यदि (buf != रिक्त)
    {
	pdest = buf;
	प्रति (psrc = name; *psrc != NUL && *psrc != TAB; ++psrc)
	{
	    यदि (*psrc == '_')
	    {
		*pdest++ = '_';
		*pdest++ = '_';
	    }
	    अन्यथा यदि (*psrc != '&')
	    {
		*pdest++ = *psrc;
	    }
	    अन्यथा यदि (*(psrc + 1) == '&')
	    {
		*pdest++ = *psrc++;
	    }
	    अन्यथा यदि (use_mnemonic)
	    {
		*pdest++ = '_';
	    }
	}
	*pdest = NUL;
    }

    CONVERT_TO_UTF8_FREE(name);
    प्रत्यागम buf;
}

/*
 * Enable or disable accelerators for the toplevel menus.
 */
    शून्य
gui_gtk_set_mnemonics(पूर्णाङ्क enable)
{
    vimmenu_T	*menu;
    char_u	*name;

    FOR_ALL_MENUS(menu)
    {
	यदि (menu->id == रिक्त)
	    अनुवर्त;

	name = translate_mnemonic_tag(menu->name, enable);
	// Don't think the check if necessary but still do it anyways
	यदि (VIM_IS_MENU_BAR_ITEM(menu->id))
	    vim_menu_bar_item_set_text(VIM_MENU_BAR_ITEM(menu->id),
		    (ध्रुव वर्ण *)name);
	vim_free(name);
    }
}

    स्थिर शून्य
popupmenu_closed_cb(GtkWidget *popover, शून्य *udata UNUSED)
{
    gtk_widget_unparent(popover);
    यदि (gui.drawarea != रिक्त)
	gtk_widget_queue_draw(gui.drawarea);
}

/*
 * Open a popup for the given menu at point (x, y).
 */
    स्थिर शून्य
gui_gtk_popup_at(vimmenu_T *menu, पूर्णाङ्क x, पूर्णाङ्क y)
{
    GtkWidget	    *popover;
    GdkRectangle    rect;

    यदि (menu == रिक्त || menu->submenu_id == रिक्त)
	प्रत्यागम;

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
	    G_CALLBACK(popupmenu_closed_cb), रिक्त);
    gtk_popover_popup(GTK_POPOVER(popover));
}

    शून्य
gui_make_popup(char_u *path_name, पूर्णाङ्क mouse_pos)
{
    vimmenu_T	*menu;
    पूर्णाङ्क		x, y;

    menu = gui_find_menu(path_name);
    यदि (menu == रिक्त || menu->submenu_id == रिक्त)
	प्रत्यागम;

    यदि (mouse_pos)
	gui_mch_getmouse(&x, &y);
    अन्यथा
    {
	x = FILL_X(curwin->w_wincol + curwin->w_wcol + 1) + 1;
	y = FILL_Y(W_WINROW(curwin) + curwin->w_wrow + 1) + 1;
    }

    gui_gtk_popup_at(menu, x, y);

}
#endif // FEAT_MENU

    पूर्णाङ्क
get_menu_tool_width(शून्य)
{
    प्रत्यागम 0;
}

    पूर्णाङ्क
get_menu_tool_height(शून्य)
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

    पूर्णाङ्क height = 0;

    प्रति (पूर्णाङ्क i = 0; i < (पूर्णाङ्क)ARRAY_LENGTH(widgets); i++)
    {
	GtkRequisition	min;
	GtkRequisition	nat;
	पूर्णाङ्क		h;

	यदि (widgets[i] == रिक्त || !gtk_widget_get_visible(widgets[i]))
	    अनुवर्त;

	h = gtk_widget_get_height(widgets[i]);

	यदि (h == 0)
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
	अन्यथा
	    height += h;
    }
    प्रत्यागम height;
}

/*
 * Update selection targets for regular and primary selections.
 */
    शून्य
gui_gtk_update_selection_formats(Clipboard_T *cbd)
{
    // This will call the ref_formats() vfunc of the content provider, see
    // gui_gtk4_cb.c
    यदि (cbd == &clip_plus)
	gdk_content_provider_content_changed(gui.regular_provider);
    अन्यथा
	gdk_content_provider_content_changed(gui.primary_provider);
}

/*
 * Get the GdkClipboard and GdkContentProvider for the given Clipboard_T.
 * clip_star (*) uses PRIMARY, clip_plus (+) uses CLIPBOARD.
 */
    स्थिर GdkClipboard *
gtk4_get_clipboard(Clipboard_T *cbd, GdkContentProvider **provider)
{
    GdkDisplay *display;

    यदि (gui.mainwin == रिक्त)
	प्रत्यागम रिक्त;

    display = gtk_widget_get_display(gui.mainwin);
    यदि (display == रिक्त)
	प्रत्यागम रिक्त;

    यदि (cbd == &clip_plus)
    {
	यदि (provider != रिक्त)
	    *provider = gui.regular_provider;
	प्रत्यागम gdk_display_get_clipboard(display);
    }
    अन्यथा
    {
	यदि (provider != रिक्त)
	    *provider = gui.primary_provider;
	प्रत्यागम gdk_display_get_primary_clipboard(display);
    }
}

/*
 * Callback for gdk_clipboard_read_async().
 */
    स्थिर शून्य
clip_read_cb(GdkClipboard *cb, GAsyncResult *result, ClipReadData *crd)
{
    GInputStream    *in_stream;
    ध्रुव वर्ण	    *mime_type;

    in_stream = gdk_clipboard_read_finish(cb, result, &mime_type, रिक्त);
    यदि (in_stream == रिक्त)
    {
	read_data_free(&crd->rd);
	प्रत्यागम;
    }

    // Not sure if we need to copy the string, but do it anyways.
    crd->mime_type = g_strdup(mime_type);
    g_निवेश_stream_read_async(in_stream, crd->rd.buf, READDATA_BUFSIZE,
	    G_PRIORITY_ऊपरी,
	    crd->rd.cancel, (GAsyncReadyCallback)read_data_निवेश_cb, crd);
}

/*
 * Request the selection from the clipboard.
 */
    शून्य
clip_mch_request_selection(Clipboard_T *cbd)
{
    स्थिर ध्रुव वर्ण *mimes_no_html[] = {
	VIMENC_MIMETYPE_NAME,
	VIM_MIMETYPE_NAME,
	"text/plain;charset=utf-8",
	"text/plain",
	रिक्त
    };

    GdkClipboard *clipboard;
    ClipReadData *crd;
    GCancellable *cancel;

    clipboard = gtk4_get_clipboard(cbd, रिक्त);
    यदि (clipboard == रिक्त)
	प्रत्यागम;

    crd = (ClipReadData *)read_data_new(READ_DATA_CLIP, परिमाण(*crd));
    crd->cbd = cbd;
    cancel = g_object_ref(crd->rd.cancel);

    gdk_clipboard_read_async(
	    clipboard, clip_html ? supported_mimes : mimes_no_html,
	    G_PRIORITY_ऊपरी, crd->rd.cancel,
	    (GAsyncReadyCallback)clip_read_cb, crd);

    {
	समय_प्रकार start;

	// Spin until the done receiving data, with a 3-second wall-clock
	// timeout as a safety net. Use the GCancellable as a way to signal if
	// done (and also use it to cancel if timed out as well).
	start = समय(रिक्त);
	यावत् (!g_cancellable_is_cancelled(cancel)
		&& समय(रिक्त) < start + 3)
	    g_main_context_iteration(रिक्त, TRUE);

	g_cancellable_cancel(cancel);
	g_object_unref(cancel);
    }
}

स्थिर पूर्णाङ्क in_clipboard_set = FALSE;

/*
 * Send the current selection to the clipboard. Do nothing for because we
 * subclass GdkContentProvider which will provide the data only when needed.
 */
    शून्य
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

    स्थिर शून्य
clipboard_changed_cb(GdkClipboard *clipboard, gpointer user_data)
{
    Clipboard_T *cbd = (Clipboard_T *)user_data;

    यदि (in_clipboard_set)
	प्रत्यागम;
    यदि (gdk_clipboard_is_local(clipboard))
	प्रत्यागम;
    clip_lose_selection(cbd);
}

/*
 * Own the selection.
 */
    पूर्णाङ्क
clip_mch_own_selection(Clipboard_T *cbd)
{
    GdkContentProvider	*cp;
    GdkClipboard	*cb = gtk4_get_clipboard(cbd, &cp);
    पूर्णाङ्क			ret;

    यदि (cb == रिक्त)
	प्रत्यागम FAIL;

    in_clipboard_set = TRUE;
    ret = gdk_clipboard_set_content(cb, cp);
    in_clipboard_set = FALSE;

    प्रत्यागम ret ? OK : FAIL;
}

/*
 * Disown the selection.  In GTK4, we clear the clipboard content to
 * release ownership.
 */
    शून्य
clip_mch_lose_selection(Clipboard_T *cbd)
{
    GdkClipboard *clipboard;

    clipboard = gtk4_get_clipboard(cbd, रिक्त);
    यदि (clipboard == रिक्त)
	प्रत्यागम;

    // Only release ownership if we still own it. We don't want to clear the
    // current selection when we aren't actually the source.
    यदि (gdk_clipboard_is_local(clipboard))
	gdk_clipboard_set_content(clipboard, रिक्त);
}

// GTK4 does not have gtk_main_level/gtk_main_quit.
// Provide compatibility stubs using a simple flag.
    guint
gtk_main_level(शून्य)
{
    प्रत्यागम gtk4_main_loop_level;
}

    शून्य
gtk_main_quit(शून्य)
{
    gtk4_main_loop_quit = TRUE;
}

#यदि defined(FEAT_MOUSESHAPE)

// Table of CSS cursor names corresponding to Vim's mouse shape IDs.
// Keep in sync with the mshape_names[] table in misc2.c.
स्थिर ध्रुव वर्ण *mshape_css_names[] =
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

    शून्य
mch_set_mouse_shape(पूर्णाङ्क shape)
{
    GdkCursor	*c;
    ध्रुव वर्ण	*css_name = "default";

    यदि (gui.drawarea == रिक्त)
	प्रत्यागम;

    यदि (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gtk_widget_set_cursor(gui.drawarea, gui.blank_pointer);
    अन्यथा
    {
	यदि (shape >= MSHAPE_NUMBERED)
	    css_name = "default";
	अन्यथा यदि (shape < (पूर्णाङ्क)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
	अन्यथा
	    प्रत्यागम;

	// GTK4: gdk_cursor_new_from_name(name, fallback)
	c = gdk_cursor_new_from_name(css_name, रिक्त);
	gtk_widget_set_cursor(gui.drawarea, c);
	g_object_unref(G_OBJECT(c));
    }
    यदि (shape != MSHAPE_HIDE)
	last_shape = shape;
}

#endif // FEAT_MOUSESHAPE


/*
 * Menus, scrollbars, dialogs, toolbar.
 * (merged from gui_gtk4.c)
 */



स्थिर पूर्णाङ्क last_text_area_w = 0;
स्थिर पूर्णाङ्क last_text_area_h = 0;

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
स्थिर ध्रुव वर्ण * ध्रुव toolbar_icon_names[] =
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

    स्थिर शून्य
toolbar_button_clicked_cb(GtkWidget *widget UNUSED, gpointer data)
{
    gui_menu_cb((vimmenu_T *)data);
}

    स्थिर GtkWidget *
create_toolbar_icon(vimmenu_T *menu)
{
    char_u	buf[MAXPATHL];
    GtkWidget	*image = रिक्त;

    // Try specified icon file first
    यदि (menu->iconfile != रिक्त)
    {
	expand_env(menu->iconfile, buf, MAXPATHL);
	यदि (vim_fexists(buf))
	{
	    GdkTexture *texture = gdk_texture_new_from_filename(
		    (ध्रुव वर्ण *)buf, रिक्त);

	    यदि (texture != रिक्त)
	    {
		image = gtk_image_new_from_paintable(
			GDK_PAINTABLE(texture));
		gtk_widget_set_size_request(image, 24, 24);
		g_object_unref(texture);
	    }
	}
    }

    // Use themed icon
    यदि (image == रिक्त)
    {
	ध्रुव वर्ण *icon_name = "image-missing";
	पूर्णाङ्क n = (पूर्णाङ्क)ARRAY_LENGTH(toolbar_icon_names);

	यदि (menu->iconidx >= 0 && menu->iconidx < n)
	    icon_name = toolbar_icon_names[menu->iconidx];

	image = gtk_image_new_from_icon_name(icon_name);
    }

    प्रत्यागम image;
}
# endif

    स्थिर शून्य
menu_button_clicked_cb(
	VimMenuItem	    *item UNUSED,
	VimMenuItemEvent    event,
	vimmenu_T	    *menu)
{
    यदि (event == VIM_MENU_ITEM_CLICKED)
	gui_menu_cb(menu);
    अन्यथा यदि (event == VIM_MENU_ITEM_SELECTED)
    {
	// Show tooltip instantly in cmdline message.
	char_u		*tooltip;
	स्थिर gboolean did_संदेश = FALSE;

	यदि (State & MODE_CMDLINE)
	    प्रत्यागम;

	tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	यदि (tooltip != रिक्त && utf_valid_string(tooltip, रिक्त))
	{
	    संदेश((वर्ण *)tooltip);
	    did_संदेश = TRUE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	अन्यथा यदि (did_संदेश)
	{
	    संदेश("");
	    did_संदेश = FALSE;
	    setcursor();
	    out_flush_cursor(TRUE, FALSE);
	}
	CONVERT_TO_UTF8_FREE(tooltip);
    }
}

    शून्य
gui_mch_add_menu(vimmenu_T *menu, पूर्णाङ्क idx)
{
    vimmenu_T	*parent;
    GtkWidget	*parent_widget;
    gboolean	use_mnemonic;
    char_u	*text;

    यदि (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	// Attach the popover to drawarea's parent rather than to drawarea
	// itself. GtkDrawingArea is a leaf widget whose snapshot does not
	// iterate children, and parenting a popover to it has been observed to
	// leave the drawing area blank while the popover is open.
	menu->submenu_id = g_object_ref_sink(vim_menu_new());
	gtk_widget_set_parent(menu->submenu_id, gui.drawarea);
	प्रत्यागम;
    }

    parent = menu->parent;

    यदि ((parent != रिक्त && parent->submenu_id == रिक्त)
	    || !menu_is_menubar(menu->name))
	प्रत्यागम;

    menu->submenu_id = g_object_ref_sink(vim_menu_new());

    use_mnemonic = parent != रिक्त || p_wak[0] != 'n';
    text = translate_mnemonic_tag(menu->name, use_mnemonic);

    यदि (parent != रिक्त)
    {
	parent_widget = parent->submenu_id;
	menu->id = g_object_ref_sink(vim_menu_item_new(
		    (ध्रुव वर्ण *)text, रिक्त, रिक्त));

	vim_menu_item_set_submenu(VIM_MENU_ITEM(menu->id),
		VIM_MENU(menu->submenu_id));
	vim_menu_जोड़ो_item(VIM_MENU(parent_widget),
		VIM_MENU_ITEM(menu->id), idx);
    }
    अन्यथा
    {
	parent_widget = gui.menubar;
	menu->id = g_object_ref_sink(vim_menu_bar_item_new(
		    (ध्रुव वर्ण *)text, VIM_MENU(menu->submenu_id)));

	vim_menu_bar_जोड़ो_item(VIM_MENU_BAR(parent_widget),
		VIM_MENU_BAR_ITEM(menu->id), idx);
    }

    vim_free(text);
}

    शून्य
gui_mch_add_menu_item(vimmenu_T *menu, पूर्णाङ्क idx)
{
    vimmenu_T	*parent = menu->parent;

# ifdef FEAT_TOOLBAR
    यदि (parent != रिक्त && menu_is_toolbar(parent->name))
    {
	यदि (menu_is_separator(menu->name))
	{
	    menu->id =
		vim_toolbar_जोड़ो_separator(VIM_TOOLBAR(gui.toolbar), idx);
	}
	अन्यथा
	{
	    GtkWidget	*btn;
	    GtkWidget	*icon;
	    char_u	*text;
	    char_u	*tooltip;

	    text    = CONVERT_TO_UTF8(menu->dname);
	    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
	    यदि (tooltip != रिक्त && !utf_valid_string(tooltip, रिक्त))
		CONVERT_TO_UTF8_FREE(tooltip);

	    icon = create_toolbar_icon(menu);
	    btn = vim_toolbar_जोड़ो_button(VIM_TOOLBAR(gui.toolbar),
		    icon, (ध्रुव वर्ण *)text, idx);
	    gtk_widget_set_tooltip_text(btn, (ध्रुव gchar *)tooltip);

	    g_signal_connect(btn, "clicked",
		    G_CALLBACK(toolbar_button_clicked_cb), menu);

	    menu->id = btn;
	    CONVERT_TO_UTF8_FREE(text);
	    CONVERT_TO_UTF8_FREE(tooltip);
	}
	प्रत्यागम;
    }
# endif

    // Menu items (non-toolbar)
    यदि (parent == रिक्त || parent->submenu_id == रिक्त)
	प्रत्यागम;

    यदि (menu_is_separator(menu->name))
    {
	menu->id = g_object_ref_sink(vim_menu_जोड़ो_separator(
		VIM_MENU(parent->submenu_id), idx));
    }
    अन्यथा
    {
	char_u	    *text;
	char_u	    *accel_text = रिक्त;
	gboolean    use_mnemonic;

	use_mnemonic = p_wak[0] != 'n';
	text = translate_mnemonic_tag(menu->name, use_mnemonic);

	यदि (menu->actext != रिक्त && menu->actext[0] != NUL)
	    accel_text = CONVERT_TO_UTF8(menu->actext);

	// Add our own reference to the widget
	menu->id = g_object_ref_sink(vim_menu_item_new((ध्रुव वर्ण *)text,
		    (VimMenuItemFunc)menu_button_clicked_cb, menu));

	यदि (accel_text != रिक्त)
	    vim_menu_item_set_accel(VIM_MENU_ITEM(menu->id),
		    (ध्रुव वर्ण *)accel_text);

	vim_menu_जोड़ो_item(VIM_MENU(parent->submenu_id),
		VIM_MENU_ITEM(menu->id), idx);

	vim_free(text);
	CONVERT_TO_UTF8_FREE(accel_text);
    }
}

    शून्य
gui_mch_toggle_tearoffs(पूर्णाङ्क enable UNUSED)
{
    // GTK4: tearoff menus don't exist.
}

    शून्य
gui_mch_menu_set_tip(vimmenu_T *menu)
{
    char_u *tooltip;

    यदि (menu->id == रिक्त)
	प्रत्यागम;

    tooltip = CONVERT_TO_UTF8(menu->strings[MENU_INDEX_TIP]);
    यदि (tooltip != रिक्त && utf_valid_string(tooltip, रिक्त))
	gtk_widget_set_tooltip_text(menu->id, (ध्रुव वर्ण *)tooltip);
    CONVERT_TO_UTF8_FREE(tooltip);
}

    शून्य
gui_mch_destroy_menu(vimmenu_T *menu)
{
    // For toolbar buttons and separators, remove from the toolbar box.
    यदि (menu->parent != रिक्त && menu_is_toolbar(menu->parent->name))
    {
	यदि (menu->id != रिक्त)
	{
	    vim_toolbar_remove(VIM_TOOLBAR(gui.toolbar), menu->id);
	    menu->id = रिक्त;
	}
	प्रत्यागम;
    }

    // For popup menus, unparent the menu as well
    यदि (menu->name[0] == ']' || menu_is_popup(menu->name))
    {
	यदि (menu->submenu_id != रिक्त)
	    gtk_widget_unparent(menu->submenu_id);
    }
    अन्यथा यदि (menu->parent == रिक्त && menu->id != रिक्त)
	// Remove from menubar, if not NULL.
	vim_menu_bar_remove(VIM_MENU_BAR(gui.menubar), menu->id);
    // "menu->id" is NULL for window toolbar
    अन्यथा यदि (menu->id != रिक्त)
	// Remove from parent menu
	vim_menu_remove(VIM_MENU(menu->parent->submenu_id), menu->id);

    g_clear_object(&menu->submenu_id);
    g_clear_object(&menu->id);
}

    शून्य
gui_mch_show_popupmenu(vimmenu_T *menu)
{
    पूर्णाङ्क x, y;

    gui_mch_getmouse(&x, &y);
    gui_gtk_popup_at(menu, x, y);
}

    स्थिर शून्य
show_menubar_popover(शून्य)
{
    GtkWidget	    *menu;
    GdkRectangle    rect;

    यदि (gui.menubar == रिक्त || gui.drawarea == रिक्त)
	प्रत्यागम;

    यदि (gtk_widget_is_visible(gui.menubar))
    {
	// If menubar is visible, then just show first menu in menubar, like how
	// GTK traditionally seems to do it?
	vim_menu_bar_show(VIM_MENU_BAR(gui.menubar), रिक्त);
	प्रत्यागम;
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
    g_signal_connect(menu, "closed", G_CALLBACK(popupmenu_closed_cb), रिक्त);
    gtk_popover_popup(GTK_POPOVER(menu));
}

#endif // FEAT_MENU

/*
 * ============================================================
 * Scrollbar functions
 * ============================================================
 */

    शून्य
gui_mch_set_scrollbar_thumb(scrollbar_T *sb, दीर्घ val, दीर्घ size, दीर्घ max)
{
    GtkAdjustment *adj;

    यदि (sb->id == रिक्त)
	प्रत्यागम;
    यदि (!GTK_IS_WIDGET(sb->id) || !GTK_IS_SCROLLBAR(sb->id))
	प्रत्यागम;

    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));
    gtk_adjustment_set_lower(adj, 0.0);
    gtk_adjustment_set_upper(adj, (gdouble)max + 1);
    gtk_adjustment_set_मूल्य(adj, (gdouble)val);
    gtk_adjustment_set_step_increment(adj, 1.0);
    gtk_adjustment_set_page_increment(adj, (gdouble)(size > 2 ? size - 2 : 1));
    gtk_adjustment_set_page_size(adj, (gdouble)size);
}

    शून्य
gui_mch_set_scrollbar_pos(scrollbar_T *sb, पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    यदि (sb->id != रिक्त)
	vim_form_move_resize(VIM_FORM(gui.formwin), sb->id, x, y, w, h);
}

    पूर्णाङ्क
gui_mch_get_scrollbar_xpadding(शून्य)
{
    पूर्णाङ्क formwin_w = gtk_widget_get_width(gui.formwin);
    पूर्णाङ्क sbar_w = 0;
    पूर्णाङ्क xpad;

    यदि (gui.which_scrollbars[SBAR_बायाँ])
	sbar_w += gui.scrollbar_width;
    यदि (gui.which_scrollbars[SBAR_दायाँ])
	sbar_w += gui.scrollbar_width;

    xpad = formwin_w - last_text_area_w - sbar_w;
    प्रत्यागम (xpad < 0) ? 0 : xpad;
}

    पूर्णाङ्क
gui_mch_get_scrollbar_ypadding(शून्य)
{
    पूर्णाङ्क formwin_h = gtk_widget_get_height(gui.formwin);
    पूर्णाङ्क ypad;

    ypad = formwin_h - last_text_area_h;
    यदि (gui.which_scrollbars[SBAR_BOTTOM])
	ypad -= gui.scrollbar_height;

    प्रत्यागम (ypad < 0) ? 0 : ypad;
}

    स्थिर शून्य
adjustment_मूल्य_changed(GtkAdjustment *adj, gpointer data UNUSED)
{
    scrollbar_T *sb = (scrollbar_T *)g_object_get_data(G_OBJECT(adj), "vim-sb");
    दीर्घ मूल्य = (दीर्घ)gtk_adjustment_get_मूल्य(adj);

    यदि (sb != रिक्त)
	gui_drag_scrollbar(sb, मूल्य, FALSE);
}

    शून्य
gui_mch_create_scrollbar(scrollbar_T *sb, पूर्णाङ्क orient)
{
    GtkAdjustment *adj;
    यदि (orient == SBAR_HORIZ)
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_HORIZONTAL, रिक्त);
    अन्यथा
	sb->id = gtk_scrollbar_new(GTK_ORIENTATION_VERTICAL, रिक्त);

    gtk_widget_add_css_class(sb->id, "vim-scrollbar");
    adj = gtk_scrollbar_get_adjustment(GTK_SCROLLBAR(sb->id));

    gtk_widget_set_visible(sb->id, FALSE);
    vim_form_put(VIM_FORM(gui.formwin), sb->id, 0, 0);

    g_object_set_data(G_OBJECT(adj), "vim-sb", (gpointer)sb);
    g_signal_connect(G_OBJECT(adj), "value-changed",
	    G_CALLBACK(adjustment_मूल्य_changed), रिक्त);
}

    शून्य
gui_mch_destroy_scrollbar(scrollbar_T *sb)
{
    यदि (sb->id != रिक्त)
    {
	vim_form_remove(VIM_FORM(gui.formwin), sb->id);
	sb->id = रिक्त;
    }
}

/*
 * ============================================================
 * Text area position
 * ============================================================
 */

    शून्य
gui_mch_set_text_area_pos(पूर्णाङ्क x, पूर्णाङ्क y, पूर्णाङ्क w, पूर्णाङ्क h)
{
    // "h" may be negative especially when draw area size is smaller than
    // "gui.char_height".
    यदि (w <= 0 || h <= 0)
	प्रत्यागम;
    last_text_area_w = w;
    last_text_area_h = h;

    vim_form_move_resize(VIM_FORM(gui.formwin), gui.drawarea, x, y, w, h);
}

/*
 * Calculate the number of pixels to bleed background color to. Should be called
 * after all UI elements are positioned and resized.
 */
    शून्य
gui_gtk4_calculate_bleed(पूर्णाङ्क width, पूर्णाङ्क height)
{
    पूर्णाङ्क old_दायाँ = gui.bleed_दायाँ;
    पूर्णाङ्क old_bot = gui.bleed_bot;

    gui.bleed_दायाँ = width - last_text_area_w;
    gui.bleed_bot = height - last_text_area_h;

    यदि (gui.which_scrollbars[SBAR_बायाँ])
	gui.bleed_दायाँ -= gui.scrollbar_width;
    यदि (gui.which_scrollbars[SBAR_दायाँ])
	gui.bleed_दायाँ -= gui.scrollbar_width;
    यदि (gui.which_scrollbars[SBAR_BOTTOM])
	gui.bleed_bot -= gui.scrollbar_height;

    // Not sure if this can happen, but be safe...
    यदि (gui.bleed_दायाँ < 0)
	gui.bleed_दायाँ = 0;
    यदि (gui.bleed_bot < 0)
	gui.bleed_bot = 0;

    // Make sure to update draw area if changed
    यदि (old_दायाँ != gui.bleed_दायाँ || old_bot != gui.bleed_bot)
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
प्रकारनिर्देश संरचना {
    GFile	*result;
    gboolean	done;
} FileDialogData;

    स्थिर शून्य
file_dialog_open_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_open_finish(
		    GTK_FILE_DIALOG(source), res, रिक्त);
    fdd->done = TRUE;
}

    स्थिर शून्य
file_dialog_save_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_save_finish(
		    GTK_FILE_DIALOG(source), res, रिक्त);
    fdd->done = TRUE;
}

    स्थिर शून्य
file_dialog_folder_cb(GObject *source, GAsyncResult *res, gpointer data)
{
    FileDialogData *fdd = (FileDialogData *)data;
    fdd->result = gtk_file_dialog_select_folder_finish(
		    GTK_FILE_DIALOG(source), res, रिक्त);
    fdd->done = TRUE;
}

    char_u *
gui_mch_browse(पूर्णाङ्क saving,
	char_u *title,
	char_u *dflt,
	char_u *ext UNUSED,
	char_u *initdir,
	char_u *filter)
{
    GtkFileDialog   *dlg;
    FileDialogData  fdd;
    char_u	    dirbuf[MAXPATHL];
    char_u	    *result = रिक्त;
    GListStore	    *filters;

    title = CONVERT_TO_UTF8(title);

    यदि (initdir == रिक्त || *initdir == NUL)
	mch_dirname(dirbuf, MAXPATHL);
    अन्यथा यदि (vim_FullName(initdir, dirbuf, MAXPATHL - 2, FALSE) == FAIL)
	dirbuf[0] = NUL;
    add_pathsep(dirbuf);

    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    यदि (title != रिक्त)
	gtk_file_dialog_set_title(dlg, (ध्रुव वर्ण *)title);

    {
	GFile *dir = g_file_new_for_path((ध्रुव वर्ण *)dirbuf);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    // Add file filters
    filters = g_list_store_new(GTK_TYPE_FILE_FILTER);
    यदि (filter != रिक्त && *filter != NUL)
    {
	पूर्णाङ्क		i = 0;
	char_u		*patt;
	char_u		*p = filter;
	GtkFileFilter	*gfilter;

	gfilter = gtk_file_filter_new();
	patt = alloc(STRLEN(filter));
	यदि (patt != रिक्त)
	{
	    यावत् (p != रिक्त && *p != NUL)
	    {
		यदि (*p == '\n' || *p == ';' || *p == '\t')
		{
		    STRNCPY(patt, filter, i);
		    patt[i] = '\0';
		    यदि (*p == '\t')
			gtk_file_filter_set_name(gfilter, (gchar *)patt);
		    अन्यथा
		    {
			gtk_file_filter_add_pattern(gfilter, (gchar *)patt);
			यदि (*p == '\n')
			{
			    g_list_store_append(filters, gfilter);
			    g_object_unref(gfilter);
			    यदि (*(p + 1) != NUL)
				gfilter = gtk_file_filter_new();
			}
		    }
		    filter = ++p;
		    i = 0;
		}
		अन्यथा
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

    यदि (saving && dflt != रिक्त && *dflt != NUL)
	gtk_file_dialog_set_initial_name(dlg, (ध्रुव वर्ण *)dflt);

    fdd.result = रिक्त;
    fdd.done = FALSE;

    यदि (saving)
	gtk_file_dialog_save(dlg, GTK_WINDOW(gui.mainwin), रिक्त,
		file_dialog_save_cb, &fdd);
    अन्यथा
	gtk_file_dialog_open(dlg, GTK_WINDOW(gui.mainwin), रिक्त,
		file_dialog_open_cb, &fdd);

    यावत् (!fdd.done)
	g_main_context_iteration(रिक्त, TRUE);

    यदि (fdd.result != रिक्त)
    {
	वर्ण *path = g_file_get_path(fdd.result);
	यदि (path != रिक्त)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    प्रत्यागम result;
}

    char_u *
gui_mch_browsedir(char_u *title, char_u *initdir)
{
    GtkFileDialog	*dlg;
    FileDialogData	fdd;
    char_u		*result = रिक्त;

    title = CONVERT_TO_UTF8(title);
    gui_mch_mousehide(FALSE);

    dlg = gtk_file_dialog_new();
    gtk_file_dialog_set_modal(dlg, TRUE);
    यदि (title != रिक्त)
	gtk_file_dialog_set_title(dlg, (ध्रुव वर्ण *)title);

    यदि (initdir != रिक्त && *initdir != NUL)
    {
	GFile *dir = g_file_new_for_path((ध्रुव वर्ण *)initdir);
	gtk_file_dialog_set_initial_folder(dlg, dir);
	g_object_unref(dir);
    }

    fdd.result = रिक्त;
    fdd.done = FALSE;

    gtk_file_dialog_select_folder(dlg, GTK_WINDOW(gui.mainwin), रिक्त,
	    file_dialog_folder_cb, &fdd);

    यावत् (!fdd.done)
	g_main_context_iteration(रिक्त, TRUE);

    यदि (fdd.result != रिक्त)
    {
	वर्ण *path = g_file_get_path(fdd.result);
	यदि (path != रिक्त)
	{
	    result = vim_strsave((char_u *)path);
	    g_free(path);
	}
	g_object_unref(fdd.result);
    }

    g_object_unref(dlg);
    CONVERT_TO_UTF8_FREE(title);

    प्रत्यागम result;
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
    स्थिर वर्ण **
split_button_string(char_u *button_string, पूर्णाङ्क *n_buttons, वर्ण **tofree)
{
    वर्ण	    **array;
    char_u	    *p;
    अचिन्हित पूर्णाङ्क    गिनती = 1;

    button_string = (char_u *)g_strdup((ध्रुव वर्ण *)button_string);

    प्रति (p = button_string; *p != NUL; ++p)
	यदि (*p == DLG_BUTTON_SEP)
	    ++गिनती;

    array = g_malloc_n(गिनती, परिमाण(वर्ण *));
    गिनती = 0;

    यदि (array != रिक्त)
    {
	array[गिनती++] = (वर्ण *)button_string;
	प्रति (p = button_string; *p != NUL; )
	{
	    यदि (*p == DLG_BUTTON_SEP)
	    {
		*p++ = NUL;
		array[गिनती++] = (वर्ण *)p;
	    }
	    अन्यथा यदि (*p == DLG_HOTKEY_CHAR)
		*p++ = '_';
	    अन्यथा
		MB_PTR_ADV(p);
	}
    }

    *tofree = (वर्ण *)button_string;
    *n_buttons = गिनती;
    प्रत्यागम array;
}

/*
 * Convert VIM_GENERIC, VIM_ERROR, etc into an icon name. Returns NULL for
 * VIM_GENERIC.
 */
    स्थिर ध्रुव वर्ण *
dialog_type_to_icon(पूर्णाङ्क type)
{
    विकल्प (type)
    {
	पक्ष VIM_ERROR:
	    प्रत्यागम "dialog-error-symbolic";
	पक्ष VIM_WARNING:
	    प्रत्यागम "dialog-warning-symbolic";
	पक्ष VIM_INFO:
	    प्रत्यागम "dialog-information-symbolic";
	पक्ष VIM_QUESTION:
	    प्रत्यागम "dialog-question-symbolic";
	स्वभाव:
	    भङ्ग;
    }
    प्रत्यागम रिक्त;
}

// Data associated with each button in the dialog
प्रकारनिर्देश संरचना
{
    पूर्णाङ्क		but_idx;
    पूर्णाङ्क		*response;
    gboolean	*done;
} DialogButtonState;

प्रकारनिर्देश संरचना
{
    GtkWidget	*win;
    gboolean	*done;
    gboolean	no_alt;
} DialogState;

    स्थिर शून्य
dialog_button_clicked_cb(GtkButton *button UNUSED, DialogButtonState *state)
{
    *state->response = state->but_idx;
    *state->done = TRUE;
}

    स्थिर gboolean
dialog_कुंजी_pressed_cb(
	Gtk_Event_Controller_कुंजी	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	DialogState		*dstate)
{
    यदि (keyval == GDK_कुंजी_Escape)
    {
	*dstate->done = TRUE;
	प्रत्यागम TRUE;
    }

    यदि (dstate->no_alt && !(state & gtk_accelerator_get_default_mod_mask()))
	प्रत्यागम gtk_widget_mnemonic_activate(dstate->win, FALSE);
    प्रत्यागम FALSE;
}

    स्थिर gboolean
dialog_close_request_cb(GtkWindow *win UNUSED, gboolean *win_closed)
{
    *win_closed = TRUE;
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
gui_mch_dialog(
	पूर्णाङ्क	type,
	char_u	*title,
	char_u	*message,
	char_u	*buttons,
	पूर्णाङ्क	def_but,
	char_u	*textfield,
	पूर्णाङ्क	ex_cmd UNUSED)
{
    GtkWindow		*win = GTK_WINDOW(gtk_window_new());
    GtkWidget		*vertbox;
    GtkWidget		*message_box;
    ध्रुव वर्ण		*icon_name;
    GtkWidget		*icon;
    GtkWidget		*label;
    GtkWidget		*entry = रिक्त;
    char_u		*utf8_title;
    char_u		*utf8_message;
    GtkEventController *कुंजी_controller;
    DialogButtonState	*but_states = रिक्त;
    वर्ण		*tofree = रिक्त;
    पूर्णाङ्क			response = -1;
    gboolean		done = FALSE;
    gboolean		win_closed = FALSE;
    DialogState		state;

    utf8_title = CONVERT_TO_UTF8(title);
    यदि (utf8_title != रिक्त)
	gtk_window_set_title(win, (ध्रुव वर्ण *)utf8_title);
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
    gtk_widget_set_margin_शीर्ष(vertbox, 24);
    gtk_widget_set_margin_bottom(vertbox, 24);
    gtk_widget_set_margin_start(vertbox, 12);
    gtk_widget_set_margin_end(vertbox, 12);

    // Add the message label
    message_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_widget_set_halign(message_box, GTK_ALIGN_CENTER);
    gtk_box_append(GTK_BOX(vertbox), message_box);

    // If type is not VIM_GENERIC, add an icon to make the dialog look nicer :)
    icon_name = dialog_type_to_icon(type);
    यदि (icon_name != रिक्त)
    {
	icon = gtk_image_new_from_icon_name(icon_name);
	gtk_image_set_icon_size(GTK_IMAGE(icon), GTK_ICON_SIZE_LARGE);
	gtk_box_append(GTK_BOX(message_box), icon);
    }

    utf8_message = CONVERT_TO_UTF8(message);
    label = gtk_label_new((ध्रुव वर्ण *)utf8_message);
    CONVERT_TO_UTF8_FREE(utf8_message);
    gtk_label_set_wrap(GTK_LABEL(label), TRUE);
    gtk_label_set_max_width_chars(GTK_LABEL(label), 40);
    gtk_box_append(GTK_BOX(message_box), label);

    // Close the dialog when the <Esc> key is pressed. Also allow using
    // mnemonics without <Alt> key (if there is no text field).
    कुंजी_controller = gtk_event_controller_कुंजी_new();
    g_signal_connect(कुंजी_controller, "key-pressed",
	    G_CALLBACK(dialog_कुंजी_pressed_cb), &state);
    gtk_widget_add_controller(GTK_WIDGET(win), कुंजी_controller);
    state.done = &done;
    state.win = GTK_WIDGET(win);

    यदि (textfield != रिक्त)
    {
	// Add text entry so user can enter text
	char_u *utf8_text = CONVERT_TO_UTF8(textfield);

	entry = gtk_entry_new();

	यदि (utf8_text != रिक्त)
	    gtk_editable_set_text(GTK_EDITABLE(entry), (ध्रुव वर्ण *)utf8_text);
	अन्यथा
	    gtk_editable_set_text(GTK_EDITABLE(entry), "");
	CONVERT_TO_UTF8_FREE(utf8_text);

	// Make it so that pressing enter key will activate "def_but" button
	// (which is set as the default widget).
	gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
	gtk_box_append(GTK_BOX(vertbox), entry);
	state.no_alt = FALSE;
    }
    अन्यथा
    {
	GListModel  *controllers;
	पूर्णाङ्क	    len;

	// Set all shortcut controllers in the window to not require a modifier
	// for mnemonics.
	controllers = gtk_widget_observe_controllers(GTK_WIDGET(win));
	len  = g_list_model_get_n_वस्तुएँ(controllers);
	प्रति (पूर्णाङ्क i = 0; i < len; i++)
	{
	    GtkEventController *controller;

	    controller = g_list_model_get_item(controllers, i);
	    यदि (GTK_IS_SHORTCUT_CONTROLLER(controller))
		gtk_shortcut_controller_set_mnemonics_modifiers(
			GTK_SHORTCUT_CONTROLLER(controller), 0);
	}
	g_object_unref(controllers);

	gtk_window_set_mnemonics_visible(win, TRUE);
	state.no_alt = TRUE;
    }

    यदि (buttons != रिक्त)
    {
	GtkWidget   *but_box;
	वर्ण	    **buttons_arr; // Note that array is allocated, not strings
	पूर्णाङ्क	    n_buttons;

	// Check 'v' flag in 'guioptions': vertical button placement.
	यदि (vim_strchr(p_go, GO_VERTICAL) != रिक्त)
	    but_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
	अन्यथा
	    but_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 6);

	gtk_widget_set_halign(but_box, GTK_ALIGN_CENTER);
	gtk_box_set_homogeneous(GTK_BOX(but_box), TRUE);
	gtk_box_append(GTK_BOX(vertbox), but_box);

	buttons_arr = split_button_string(buttons, &n_buttons, &tofree);

	but_states = g_malloc_n(n_buttons, परिमाण(DialogButtonState));

	प्रति (पूर्णाङ्क i = 0; i < n_buttons; i++)
	{
	    char_u		*but_label;
	    GtkWidget		*but;
	    DialogButtonState	*but_state = but_states + i;

	    but_label = CONVERT_TO_UTF8((char_u *)buttons_arr[i]);
	    यदि (but_label == रिक्त)
		अनुवर्त;

	    but = gtk_button_new_with_mnemonic((वर्ण *)but_label);
	    यदि (i == def_but - 1)
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

    यावत् (!done && !win_closed)
	g_main_context_iteration(रिक्त, TRUE);

    यदि (done)
    {
	यदि (textfield != रिक्त)
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
    प्रत्यागम response == -1 ? 0 : response + 1;
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

प्रकारनिर्देश संरचना
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

स्थिर SharedFindReplace find_widgets = {0};
स्थिर SharedFindReplace repl_widgets = {0};

    स्थिर शून्य
find_replace_cb(GtkWidget *widget UNUSED, gpointer data)
{
    पूर्णाङ्क			flags;
    char_u		*find_text;
    char_u		*repl_text;
    gboolean		direction_down;
    SharedFindReplace	*sfr;

    flags = GPOINTER_TO_INT(data);

    यदि (flags == FRD_FINDNEXT)
    {
	repl_text = रिक्त;
	sfr = &find_widgets;
    }
    अन्यथा
    {
	repl_text = (char_u *)gtk_editable_get_text(
		GTK_EDITABLE(repl_widgets.with));
	sfr = &repl_widgets;
    }

    find_text = (char_u *)gtk_editable_get_text(GTK_EDITABLE(sfr->what));
    direction_down = gtk_check_button_get_active(
	    GTK_CHECK_BUTTON(sfr->down));

    यदि (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->wword)))
	flags |= FRD_WHOLE_WORD;
    यदि (gtk_check_button_get_active(GTK_CHECK_BUTTON(sfr->mcase)))
	flags |= FRD_MATCH_CASE;

    repl_text = CONVERT_FROM_UTF8(repl_text);
    find_text = CONVERT_FROM_UTF8(find_text);
    gui_do_findrepl(flags, find_text, repl_text, direction_down);
    CONVERT_FROM_UTF8_FREE(repl_text);
    CONVERT_FROM_UTF8_FREE(find_text);
}

    स्थिर शून्य
dialog_destroyed_cb(GtkWidget *widget UNUSED, gpointer data)
{
    *(GtkWidget **)data = रिक्त;
}

    स्थिर शून्य
entry_changed_cb(GtkWidget *entry, GtkWidget *dialog)
{
    ध्रुव gchar	*entry_text;
    gboolean	nonempty;

    entry_text = gtk_editable_get_text(GTK_EDITABLE(entry));

    यदि (!entry_text)
	प्रत्यागम; // Shouldn't happen

    nonempty = (entry_text[0] != '\0');

    यदि (dialog == find_widgets.dialog)
	gtk_widget_set_sensitive(find_widgets.find, nonempty);

    यदि (dialog == repl_widgets.dialog)
    {
	gtk_widget_set_sensitive(repl_widgets.find, nonempty);
	gtk_widget_set_sensitive(repl_widgets.replace, nonempty);
	gtk_widget_set_sensitive(repl_widgets.all, nonempty);
    }
}

    स्थिर gboolean
find_कुंजी_pressed_cb(
	Gtk_Event_Controller_कुंजी	*controller UNUSED,
	guint			keyval,
	guint			keycode UNUSED,
	GdkModifierType		state,
	SharedFindReplace	*frdp)
{
    // If the user is holding one of the key modifiers we will just bail out,
    // thus preserving the possibility of normal focus traversal.
    यदि (state & (GDK_CONTROL_MASK | GDK_SHIFT_MASK))
	प्रत्यागम FALSE;

    // the Escape key synthesizes a cancellation action
    यदि (keyval == GDK_कुंजी_Escape)
    {
	// Destroy rather than hide the dialog: reusing a hidden toplevel loses
	// window decorations on Wayland
	gtk_window_destroy(GTK_WINDOW(frdp->dialog));
	प्रत्यागम TRUE;
    }

    प्रत्यागम FALSE;
}

    स्थिर शून्य
entry_activate_cb(GtkWidget *widget UNUSED, शून्य *udata)
{
    gtk_widget_grab_focus(GTK_WIDGET(udata));
}

    स्थिर शून्य
find_replace_dialog_create(char_u *arg, पूर्णाङ्क do_replace)
{
    SharedFindReplace	*frdp;
    char_u		*entry_text;
    पूर्णाङ्क			wword = FALSE;
    पूर्णाङ्क			mcase = !p_ic;
    GtkWidget		*vertbox, *grid, *hbox, *tmp, *btn;
    gboolean		sensitive;
    GtkEventController	*कुंजी_controller;

    frdp = do_replace ? &repl_widgets : &find_widgets;
    entry_text = get_find_dialog_text(arg, &wword, &mcase);

    यदि (entry_text != रिक्त && output_conv.vc_type != CONV_NONE)
    {
	char_u *old = entry_text;
	entry_text = string_convert(&output_conv, entry_text, रिक्त);
	vim_free(old);
    }

    // If the dialog already exists, just raise it.
    यदि (frdp->dialog != रिक्त)
    {
	यदि (entry_text != रिक्त)
	{
	    gtk_editable_set_text(GTK_EDITABLE(frdp->what),
		    (वर्ण *)entry_text);
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
	यदि (do_replace && g_utf8_strlen(
		    gtk_editable_get_text(GTK_EDITABLE(frdp->what)), -1) > 0)
	    gtk_widget_grab_focus(frdp->with);

	vim_free(entry_text);
	प्रत्यागम;
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
    gtk_widget_set_margin_शीर्ष(vertbox, 12);
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
    sensitive = (entry_text != रिक्त && entry_text[0] != NUL);
    यदि (entry_text != रिक्त)
	gtk_editable_set_text(GTK_EDITABLE(frdp->what), (वर्ण *)entry_text);
    gtk_grid_attach(GTK_GRID(grid), frdp->what, 1, 0, 1, 1);

    यदि (do_replace)
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
    अन्यथा
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

    कुंजी_controller = gtk_event_controller_कुंजी_new();
    g_signal_connect(कुंजी_controller, "key-pressed",
	    G_CALLBACK(find_कुंजी_pressed_cb), frdp);
    gtk_widget_add_controller(GTK_WIDGET(frdp->dialog), कुंजी_controller);

    g_signal_connect(G_OBJECT(frdp->what), "changed",
	    G_CALLBACK(entry_changed_cb), frdp->dialog);

    // Make it so that when entry is activated, this button will be activated.
    gtk_window_set_default_widget(GTK_WINDOW(frdp->dialog), btn);
    g_signal_connect(btn, "clicked", G_CALLBACK(find_replace_cb),
	    GINT_TO_POINTER(do_replace ? FRD_R_FINDNEXT : FRD_FINDNEXT));
    gtk_box_append(GTK_BOX(hbox), btn);

    यदि (do_replace)
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
    यदि (do_replace && entry_text != रिक्त && entry_text[0] != NUL)
	gtk_widget_grab_focus(frdp->with);

    vim_free(entry_text);
}

    शून्य
gui_mch_find_dialog(exarg_T *eap)
{
    यदि (gui.in_use)
	find_replace_dialog_create(eap->arg, FALSE);
}

    शून्य
gui_mch_replace_dialog(exarg_T *eap)
{
    यदि (gui.in_use)
	find_replace_dialog_create(eap->arg, TRUE);
}

/*
 * ============================================================
 * Help find (for :helpfind command)
 * ============================================================
 */

    शून्य
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
स्थिर GtkPrintDialog *print_dialog = रिक्त;
स्थिर GtkPrintSettings *print_settings = रिक्त;
स्थिर GtkPageSetup *page_setup = रिक्त;
स्थिर GtkPrintSetup *print_setup = रिक्त;
स्थिर GFile *print_file;

    स्थिर शून्य
print_dialog_cb(GtkPrintDialog *dialog, GAsyncResult *result, gboolean *done)
{
    GError *error = रिक्त;
    print_setup = gtk_print_dialog_setup_finish(dialog, result, &error);

    यदि (print_setup == रिक्त)
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
	द्विगुण		*page_width,
	द्विगुण		*page_height)
{
    GtkPrintDialog  *dialog = gtk_print_dialog_new();
    gboolean	    done = FALSE;
    char_u	    *temp;

    gtk_print_dialog_set_title(dialog, (ध्रुव वर्ण *)jobname);
    gtk_print_dialog_set_modal(dialog, TRUE);

    print_dialog = रिक्त;
    print_setup = रिक्त;

    यदि (print_settings == रिक्त)
    {
	option_table_T *opt;

	print_settings = gtk_print_settings_new();

	opt = &printer_opts[OPT_PRINT_DUPLEX];
	यदि (opt->present)
	{
	    यदि (STRNICMP(opt->string, "off", 3) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_SIMPLEX);
	    अन्यथा यदि (STRNICMP(opt->string, "short", 5) == 0)
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_VERTICAL);
	    अन्यथा
		gtk_print_settings_set_duplex(print_settings,
			GTK_PRINT_DUPLEX_HORIZONTAL);
	}

	opt = &printer_opts[OPT_PRINT_PORTRAIT];
	यदि (opt->present)
	{
	    यदि (*opt->string == 'y')
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_PORTRAIT);
	    अन्यथा
		gtk_print_settings_set_orientation(print_settings,
			GTK_PAGE_ORIENTATION_LANDSCAPE);
	}

	opt = &printer_opts[OPT_PRINT_PAPER];
	यदि (opt->present)
	{
	    gtk_print_settings_set_paper_size(print_settings,
		    gtk_paper_size_new((ध्रुव वर्ण *)opt->string));
	}

	opt = &printer_opts[OPT_PRINT_COLLATE];
	यदि (opt->present)
	{
	    यदि (*opt->string == 'y')
		gtk_print_settings_set_collate(print_settings, TRUE);
	    अन्यथा
		gtk_print_settings_set_collate(print_settings, FALSE);
	}
    }

    यदि (page_setup == रिक्त)
	page_setup = gtk_page_setup_new();

    gtk_print_dialog_set_print_settings(dialog, print_settings);
    gtk_print_dialog_set_page_setup(dialog, page_setup);

    gtk_print_dialog_setup(dialog, GTK_WINDOW(gui.mainwin), रिक्त,
	    (GAsyncReadyCallback)print_dialog_cb, &done);

    यावत् (!done)
	g_main_context_iteration(रिक्त, TRUE);

    यदि (print_setup == रिक्त)
    {
	g_object_unref(dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_संदेश = FALSE;
	प्रत्यागम रिक्त;
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
    यदि (temp == रिक्त)
    {
	emsg(_(e_cant_find_temp_file_for_writing));
	g_clear_object(&dialog);
	g_clear_pointer(&print_setup, gtk_print_setup_unref);

	psettings->user_abort_संदेश = FALSE;
	प्रत्यागम रिक्त;
    }
    print_file = g_file_new_for_path((वर्ण *)temp);

    print_dialog = dialog;

    प्रत्यागम temp;
}

    स्थिर शून्य
print_file_cb(
	GtkPrintDialog	*dialog,
	GAsyncResult	*result,
	GFile		*file)
{
    वर्ण *path = g_file_get_path(file);

    (शून्य)gtk_print_dialog_print_file_finish(dialog, result, रिक्त);
    g_object_unref(dialog);

    // Don't use g_file_delete() to be consistent
    mch_remove(path);
    g_free(path);
}


    शून्य
gui_gtk4_print_finish(शून्य)
{
    यदि (print_dialog != रिक्त)
    {
	gtk_print_dialog_print_file(
		print_dialog, GTK_WINDOW(gui.mainwin),
		print_setup, print_file, रिक्त,
		(GAsyncReadyCallback)print_file_cb, print_file);
	print_dialog = रिक्त;
	g_clear_pointer(&print_setup, gtk_print_setup_unref);
    }
}
#endif // USE_GTK4_PRINT_DIALOG

#ifdef FEAT_IMAGE_GUI

प्रकारनिर्देश संरचना
{
    GdkTexture *texture;
} image_gdk_T;

प्रकारनिर्देश संरचना
{
    Gsk_Render_गाँठ   **nodes; // Note that elements may be NULL
    पूर्णाङ्क		    n_nodes;
} image_placement_gdk_T;

    पूर्णाङ्क
image_gui_init(image_T *img)
{
    image_gdk_T	    *ctx = ALLOC_CLEAR_ONE(image_gdk_T);
    GdkMemoryFormat fmt;
    माप_प्रकार	    stride;
    GBytes	    *bytes;
    माप_प्रकार	    size;
    पूर्णाङ्क		    w, h;

    यदि (ctx == रिक्त)
	प्रत्यागम FAIL;

    यदि (img->fmt == IMAGE_FORMAT_RGBA)
	fmt = GDK_MEMORY_R8G8B8A8;
    अन्यथा
	fmt = GDK_MEMORY_R8G8B8;

    image_get_dimensions(img, &w, &h);

    size = (माप_प्रकार)w * h * img->fmt;
    stride = w * img->fmt;

    bytes = g_bytes_new(img->data, size);
    ctx->texture = gdk_memory_texture_new(w, h, fmt, bytes, stride);
    g_bytes_unref(bytes);

    img->backend_data = ctx;
    प्रत्यागम OK;
}

    शून्य
image_gui_uninit(image_T *img)
{
    image_gdk_T *ctx = img->backend_data;

    g_object_unref(ctx->texture);
    vim_free(ctx);
}

    स्थिर शून्य
clear_nodes(image_placement_gdk_T *ctx)
{
    यदि (ctx->nodes == रिक्त || gui.drawarea == रिक्त)
	प्रत्यागम;

    प्रति (पूर्णाङ्क i = 0; i < ctx->n_nodes; i++)
    {
	यदि (ctx->nodes[i] == रिक्त)
	    अनुवर्त;
	vim_draw_area_remove_external(
		VIM_DRAW_AREA(gui.drawarea), ctx->nodes[i]);
	gsk_render_गाँठ_unref(ctx->nodes[i]);
    }
    g_clear_pointer(&ctx->nodes, g_free);
    ctx->n_nodes = 0;
}

    पूर्णाङ्क
image_placement_gui_init(image_placement_T *place)
{
    image_placement_gdk_T *ctx = ALLOC_CLEAR_ONE(image_placement_gdk_T);

    यदि (ctx == रिक्त)
	प्रत्यागम FAIL;

    place->backend_data = ctx;
    प्रत्यागम OK;
}

    शून्य
image_placement_gui_uninit(image_placement_T *place)
{
    image_placement_gdk_T *ctx = place->backend_data;

    clear_nodes(ctx);
    vim_free(ctx);
}

    शून्य
image_placement_gui_draw(image_placement_T *place)
{
    image_T		    *img = place->img;
    image_placement_gdk_T   *ctx = place->backend_data;
    image_gdk_T		    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    पूर्णाङ्क			    n_rects;
    पूर्णाङ्क			    iw, ih;
    Gsk_Render_गाँठ	    *texture_गाँठ;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    यदि (rects == रिक्त)
	प्रत्यागम;

    clear_nodes(ctx);
    यदि (n_rects == 0)
	प्रत्यागम;

    image_get_dimensions(img, &iw, &ih);
    texture_गाँठ = gsk_texture_गाँठ_new(ictx->texture,
	    &GRAPHENE_RECT_INIT(
		FILL_X(place->col) - FILL_X(place->crop_box.x1),
		FILL_Y(place->row) - FILL_Y(place->crop_box.y1)
		+ FILL_Y(place->row_off),
		PHY2LOG(iw), PHY2LOG(ih)));

    यदि (texture_गाँठ == रिक्त)
	// Not sure if this can happen...
	प्रत्यागम;

    ctx->nodes = g_malloc_n(n_rects, परिमाण(Gsk_Render_गाँठ *));
    ctx->n_nodes = n_rects;

    प्रति (पूर्णाङ्क i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	पूर्णाङ्क		row, col;
	पूर्णाङ्क		posx, posy;
	पूर्णाङ्क		x, y, w, h;
	Gsk_Render_गाँठ	*गाँठ;

	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	गाँठ = gsk_clip_गाँठ_new(texture_गाँठ,
		&GRAPHENE_RECT_INIT(posx, posy, w, h));

	यदि (गाँठ != रिक्त)
	    vim_draw_area_add_external(VIM_DRAW_AREA(gui.drawarea), गाँठ);

	ctx->nodes[i] = गाँठ;
    }
    gsk_render_गाँठ_unref(texture_गाँठ);
}

    शून्य
image_placement_gui_clear(image_placement_T *place)
{
    image_placement_gdk_T   *ctx = place->backend_data;

    clear_nodes(ctx);
}

#endif

#endif // FEAT_GUI_GTK
