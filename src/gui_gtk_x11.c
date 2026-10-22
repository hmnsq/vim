/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * Porting to GTK+ was done by:
 *
 * (C) 1998,1999,2000 by Marcin Dalecki <martin@dalecki.de>
 *
 * With GREAT support and continuous encouragements by Andy Kahn and of
 * course Bram Moolenaar!
 *
 * Support for GTK+ 2 was added by:
 *
 * (C) 2002,2003  Jason Hildebrand  <jason@peaceworks.ca>
 *		  Daniel Elstner  <daniel.elstner@gmx.net>
 *
 * Support for GTK+ 3 was added by:
 *
 * 2016  Kazunobu Kuriyama  <kazunobu.kuriyama@gmail.com>
 */

#شامل "vim.h"
#اگر_تعریف_شدہ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GNOME
// Gnome redefines _() and N_().  Grrr...
# ifdef _
#  undef _
# endif
# ifdef N_
#  undef N_
# endif
# ifdef textdomain
#  undef textdomain
# endif
# ifdef bindtextdomain
#  undef bindtextdomain
# endif
# ifdef bind_textdomain_codeset
#  undef bind_textdomain_codeset
# endif
# اگر defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
خارجی خالی bonobo_dock_شے_سیٹ_behavior(Bonobo_Dock_شے *dock_شے, Bonobo_Dock_شے_Behavior beh);
#اگر_ختم

#اگر defined(FEAT_GUI_GTK)
# اگر GTK_CHECK_نسخہ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# ورنہ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ورنہ
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#اگر_ختم

#اگر_تعریف_شدہ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#اگر_ختم

/*
 * Easy-to-use macro for multihead support.
 */
#تعریف_کرو GET_X_ATOM(atom)	gdk_x11_atom_مقصد_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
شمار
{
    ہدف_TYPE_NONE,
    ہدف_UTF8_STRING,
    ہدف_STRING,
    ہدف_COMPOUND_متن,
    ہدف_HTML,
    ہدف_متن,
    ہدف_متن_URI_LIST,
    ہدف_متن_PLAIN,
    ہدف_متن_PLAIN_UTF8,
    ہدف_VIM_MT,
    ہدف_VIM,
    ہدف_VIMENC_MT,
    ہدف_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساکن ثابت Gtk_ہدف_اندراج selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, ہدف_VIMENC},
    {VIM_ATOM_NAME,	0, ہدف_VIM},
    {VIMENC_MIMETYPE_NAME,  0, ہدف_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, ہدف_VIM_MT},
    {"text/html",	0, ہدف_HTML},
    {"UTF8_STRING",	0, ہدف_UTF8_STRING},
    {"COMPOUND_TEXT",	0, ہدف_COMPOUND_متن},
    {"TEXT",		0, ہدف_متن},
    {"STRING",		0, ہدف_STRING},
    {"text/plain;charset=utf-8", 0, ہدف_متن_PLAIN_UTF8},
    {"text/plain",	0, ہدف_متن_PLAIN}
};
#تعریف_کرو N_SELECTION_TARGETS ARRAY_لمبائی(selection_targets)

#اگر_تعریف_شدہ FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساکن ثابت Gtk_ہدف_اندراج dnd_targets[] =
{
    {"text/uri-list",	0, ہدف_متن_URI_LIST},
    {"text/html",	0, ہدف_HTML},
    {"UTF8_STRING",	0, ہدف_UTF8_STRING},
    {"STRING",		0, ہدف_STRING},
    {"text/plain",	0, ہدف_متن_PLAIN}
};
# define N_DND_TARGETS ARRAY_لمبائی(dnd_targets)
#اگر_ختم


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#تعریف_کرو DEFAULT_FONT	"Monospace 12"

#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#اگر_ختم

#اگر !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ساکن GdkAtom wm_protocols_atom = GDK_NONE;
ساکن GdkAtom save_yourself_atom = GDK_NONE;
#اگر_ختم

/*
 * Atoms used to control/reference X11 selections.
 */
ساکن GdkAtom html_atom = GDK_NONE;
ساکن GdkAtom utf8_string_atom = GDK_NONE;
ساکن GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
ساکن GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
ساکن GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
ساکن GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
ساکن ساخت special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدیں[] =
{
    {GDK_اوپر,		'k', 'u'},
    {GDK_نیچے_کو,		'k', 'd'},
    {GDK_بایاں,		'k', 'l'},
    {GDK_دایاں,		'k', 'r'},
    {GDK_F1,		'k', '1'},
    {GDK_F2,		'k', '2'},
    {GDK_F3,		'k', '3'},
    {GDK_F4,		'k', '4'},
    {GDK_F5,		'k', '5'},
    {GDK_F6,		'k', '6'},
    {GDK_F7,		'k', '7'},
    {GDK_F8,		'k', '8'},
    {GDK_F9,		'k', '9'},
    {GDK_F10,		'k', ';'},
    {GDK_F11,		'F', '1'},
    {GDK_F12,		'F', '2'},
    {GDK_F13,		'F', '3'},
    {GDK_F14,		'F', '4'},
    {GDK_F15,		'F', '5'},
    {GDK_F16,		'F', '6'},
    {GDK_F17,		'F', '7'},
    {GDK_F18,		'F', '8'},
    {GDK_F19,		'F', '9'},
    {GDK_F20,		'F', 'A'},
    {GDK_F21,		'F', 'B'},
    {GDK_Pause,		'F', 'B'}, // Pause == F21 according to netbeans.txt
    {GDK_F22,		'F', 'C'},
    {GDK_F23,		'F', 'D'},
    {GDK_F24,		'F', 'E'},
    {GDK_F25,		'F', 'F'},
    {GDK_F26,		'F', 'G'},
    {GDK_F27,		'F', 'H'},
    {GDK_F28,		'F', 'I'},
    {GDK_F29,		'F', 'J'},
    {GDK_F30,		'F', 'K'},
    {GDK_F31,		'F', 'L'},
    {GDK_F32,		'F', 'M'},
    {GDK_F33,		'F', 'N'},
    {GDK_F34,		'F', 'O'},
    {GDK_F35,		'F', 'P'},
#اگر_تعریف_شدہ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#اگر_ختم
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_عقب_Space,	'k', 'b'},
    {GDK_داخل_کرو,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_عقب_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_انتہا,		'@', '7'},
    {GDK_پیشگی,		'k', 'P'},
    {GDK_اگلا,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_بایاں,	'k', 'l'},
    {GDK_KP_دایاں,	'k', 'r'},
    {GDK_KP_اوپر,		'k', 'u'},
    {GDK_KP_نیچے_کو,	'k', 'd'},
    {GDK_KP_داخل_کرو,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_انتہا,	'K', '4'},
    {GDK_KP_پیشگی,	'K', '3'},  // page up
    {GDK_KP_اگلا,	'K', '5'},  // page down

    {GDK_KP_جوڑو,	'K', '6'},
    {GDK_KP_تفریق,	'K', '7'},
    {GDK_KP_Divide,	'K', '8'},
    {GDK_KP_Multiply,	'K', '9'},
    {GDK_KP_Enter,	'K', 'A'},
    {GDK_KP_Decimal,	'K', 'B'},

    {GDK_KP_0,		'K', 'C'},
    {GDK_KP_1,		'K', 'D'},
    {GDK_KP_2,		'K', 'E'},
    {GDK_KP_3,		'K', 'F'},
    {GDK_KP_4,		'K', 'G'},
    {GDK_KP_5,		'K', 'H'},
    {GDK_KP_6,		'K', 'I'},
    {GDK_KP_7,		'K', 'J'},
    {GDK_KP_8,		'K', 'K'},
    {GDK_KP_9,		'K', 'L'},

    // End of list marker:
    {0, 0, 0}
};

/*
 * Flags for command line options table below.
 */
#تعریف_کرو ARG_FONT	1
#تعریف_کرو ARG_GEOMETRY	2
#تعریف_کرو ARG_الٹ	3
#تعریف_کرو ARG_NOREVERSE	4
#تعریف_کرو ARG_BACKGROUND	5
#تعریف_کرو ARG_FOREGROUND	6
#تعریف_کرو ARG_ICONIC	7
#تعریف_کرو ARG_ROLE	8
#تعریف_کرو ARG_NETBEANS	9
#تعریف_کرو ARG_XRM		10	// ignored
#تعریف_کرو ARG_MENUFONT	11	// ignored
#تعریف_کرو ARG_اشاریہ_نقاب	0x00ff
#تعریف_کرو ARG_HAS_قدر	0x0100	// a value is expected after the argument
#تعریف_کرو ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#تعریف_کرو ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#تعریف_کرو ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#تعریف_کرو ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
قسمتعریف ساخت
{
    ثابت حرف	    *name;
    غیرعلامتی صحیح    flags;
}
cmdline_option_T;

ساکن ثابت cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_قدر},
    {"-font",		ARG_FONT|ARG_HAS_قدر},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_قدر},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_قدر},
    {"-rv",		ARG_الٹ},
    {"-reverse",	ARG_الٹ},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_قدر},
    {"-background",	ARG_BACKGROUND|ARG_HAS_قدر},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_قدر},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_قدر},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_قدر},
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_قدر},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
#اگر_ختم
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_قدر|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_قدر|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_قدر|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_قدر},
#اگر_تعریف_شدہ FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_قدر},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_قدر},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_قدر},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# اگر 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#اگر_ختم
    {تہی, 0}
};

ساکن صحیح    gui_argc = 0;
ساکن حرف **gui_argv = تہی;

ساکن ثابت حرف *role_دلیل = تہی;
#اگر defined(USE_GNOME_SESSION)
ساکن ثابت حرف *restart_command = تہی;
ساکن       حرف *abs_restart_command = تہی;
#اگر_ختم
ساکن صحیح ملا_iconic_arg = FALSE;

#اگر_تعریف_شدہ FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ساکن صحیح using_gnome = 0;
#ورنہ
# define using_gnome 0
#اگر_ختم

// Comment out the following line to ignore code for resize history tracking.
#تعریف_کرو TRACK_RESIZE_HISTORY
#اگر_تعریف_شدہ TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# اگر 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
قسمتعریف ساخت resize_history {
    صحیح مستعمل;		// If true, can't match for discard. Only matches once.
    صحیح چوڑائی;
    صحیح اونچائی;
    صحیح from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحیح seq;		// for ch_log messages
# endif
    ساخت resize_history *اگلا;
} resize_hist_T;

// never NULL during execution
ساکن resize_hist_T *latest_resize_hist;
// list of stale resize requests
ساکن resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ساکن صحیح mch_csd_چوڑائی = 0;
ساکن صحیح mch_csd_اونچائی = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
ساکن صحیح mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    ساکن خالی
alloc_resize_hist(صحیح چوڑائی, صحیح اونچائی, صحیح from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *پچھلا_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->چوڑائی = چوڑائی;
    new_hist->اونچائی = اونچائی;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    پچھلا_hist->اگلا = old_resize_hists;
    old_resize_hists = پچھلا_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = پچھلا_hist->seq + 1;
    ch_log(تہی, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, چوڑائی, اونچائی, (صحیح)Columns, (صحیح)صفیں);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    ساکن خالی
clear_resize_hists(خالی)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحیح		    i = 0;
# endif

    اگر (latest_resize_hist)
	latest_resize_hist->مستعمل = TRUE;
    جب (old_resize_hists != تہی)
    {
	resize_hist_T *اگلا_hist = old_resize_hists->اگلا;

	vim_free(old_resize_hists);
	old_resize_hists = اگلا_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(تہی, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define مطابقت_چوڑائی_اونچائی(hist, w, h) \
	  (!hist->مستعمل && hist->چوڑائی == w && hist->اونچائی == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ساکن صحیح
مطابقت_stale_چوڑائی_اونچائی(صحیح چوڑائی, صحیح اونچائی)
{
    resize_hist_T *hist = old_resize_hists;

    برائے (hist = old_resize_hists; hist != تہی; hist = hist->اگلا)
	اگر (مطابقت_چوڑائی_اونچائی(hist, چوڑائی, اونچائی))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(تہی, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->مستعمل = TRUE;
	    واپس TRUE;
	}
    واپس FALSE;
}

# اگر defined(EXITFREE)
    ساکن خالی
free_all_resize_hist(خالی)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#اگر_ختم

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
ساکن guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    خالی
gui_mch_prepare(صحیح *argc, حرف **argv)
{
    ثابت cmdline_option_T  *option;
    صحیح			    i	= 0;
    صحیح			    len = 0;

#اگر defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    اگر (strchr(argv[0], G_DIR_SEPARATOR) != تہی)
    {
	char_u buf[MAXPATHL];

	اگر (mch_بھرا_Name((char_u *)argv[0], buf, (صحیح)حجم(buf), TRUE) == OK)
	{
	    abs_restart_command = (حرف *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#اگر_ختم

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(حرف *, *argc + 1);

    g_return_if_fail(gui_argv != تہی);

    gui_argv[gui_argc++] = argv[i++];

    جب (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	اگر (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    جاری;
	}

	// Look for argv[i] in cmdline_options[] table.
	برائے (option = &cmdline_options[0]; option->name != تہی; ++option)
	{
	    len = لڑی_لمبائی(option->name);

	    اگر (لڑی_موازنہ(argv[i], option->name, len) == 0)
	    {
		اگر (argv[i][len] == '\0')
		    توڑو;
		// allow --foo=bar style
		اگر (argv[i][len] == '=' && (option->flags & ARG_HAS_قدر))
		    توڑو;
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		اگر (vim_strchr((char_u *)":=", argv[i][len]) != تہی
			&& (option->flags & ARG_اشاریہ_نقاب) == ARG_NETBEANS)
		    توڑو;
#اگر_ختم
	    }
	    ورنہ اگر ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (حرف *)option->name;
		توڑو;
	    }
	}
	اگر (option->name == تہی) // no match
	{
	    ++i;
	    جاری;
	}

	اگر (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	ورنہ
	{
	    حرف *قدر = تہی;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    اگر (option->flags & ARG_HAS_قدر)
	    {
		اگر (argv[i][len] == '=')
		    قدر = &argv[i][len + 1];
		ورنہ اگر (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    قدر = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    انتخاب (option->flags & ARG_اشاریہ_نقاب)
	    {
		حالت ARG_الٹ:
		    ملا_الٹ_arg = TRUE;
		    توڑو;
		حالت ARG_NOREVERSE:
		    ملا_الٹ_arg = FALSE;
		    توڑو;
		حالت ARG_FONT:
		    font_دلیل = قدر;
		    توڑو;
		حالت ARG_GEOMETRY:
		    اگر (قدر != تہی)
			gui.geom = vim_strsave((char_u *)قدر);
		    توڑو;
		حالت ARG_BACKGROUND:
		    background_دلیل = قدر;
		    توڑو;
		حالت ARG_FOREGROUND:
		    foreground_دلیل = قدر;
		    توڑو;
		حالت ARG_ICONIC:
		    ملا_iconic_arg = TRUE;
		    توڑو;
		حالت ARG_ROLE:
		    role_دلیل = قدر; // used later in gui_mch_open()
		    توڑو;
#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
		حالت ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    توڑو;
#اگر_ختم
		طےشدہ:
		    توڑو;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	اگر ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	اگر (option->flags & ARG_KEEP)
	    ++i;
	ورنہ
	{
	    // Remove the flag from the argument vector.
	    اگر (--*argc > i)
	    {
		صحیح n_strip = 1;

		// Move the argument's value as well, if there is one.
		اگر ((option->flags & ARG_HAS_قدر)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    اگر (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		اگر (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * حجم(حرف *));
	    }
	    argv[*argc] = تہی;
	}
    }

    gui_argv[gui_argc] = تہی;
}

#اگر defined(EXITFREE)
    خالی
gui_mch_free_all(خالی)
{
    vim_free(gui_argv);
# اگر defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#اگر_ختم

#اگر !GTK_CHECK_نسخہ(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ساکن gint
visibility_event(GtkWidget *widget غیر_مستعمل,
		 GdkEventVisibility *event,
		 gpointer ڈیٹا غیر_مستعمل)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    اگر (gui.متن_gc != تہی)
	gdk_gc_سیٹ_exposures(gui.متن_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    واپس FALSE;
}
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن gboolean
draw_event(GtkWidget *widget غیر_مستعمل,
	   cairo_t   *cr,
	   gpointer   user_ڈیٹا غیر_مستعمل)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    اگر (gui.starting)
	واپس FALSE;

    برآمد_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_سیٹ_ماخذ_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    واپس FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# ورنہ
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# اگر GTK_CHECK_نسخہ(3,10,0)
    ساکن gboolean
scale_ضربی_عدد_event(GtkWidget *widget,
		   GParamSpec* pspec غیر_مستعمل,
		   gpointer   user_ڈیٹا غیر_مستعمل)
{
    اگر (gui.surface != تہی)
	cairo_surface_destroy(gui.surface);

    صحیح	    w, h;
    gtk_کھڑکی_get_سائز(GTK_کھڑکی(gui.mainwin), &w, &h);
    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    صحیح	    usable_اونچائی = h;
    اگر (gtk_socket_id != 0)
	usable_اونچائی -= (gui.char_اونچائی - (gui.char_اونچائی/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_اونچائی);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  اگر defined(FEAT_IMAGE) && GTK_CHECK_نسخہ(3,10,0)
    gui.scale = gtk_widget_get_scale_ضربی_عدد(widget);
#  endif

    واپس TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#ورنہ // !GTK_CHECK_VERSION(3,0,0)
    ساکن gint
expose_event(GtkWidget *widget غیر_مستعمل,
	     GdkEventExpose *event,
	     gpointer ڈیٹا غیر_مستعمل)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    اگر (gui.starting)
	واپس FALSE;

    برآمد_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.چوڑائی, event->area.اونچائی);

    // Clear the border areas if needed
    اگر (event->area.x < FILL_X(0))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, 0, FILL_X(0), 0);
    اگر (event->area.y < FILL_Y(0))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, 0, 0, FILL_Y(0));
    اگر (event->area.x > FILL_X(Columns))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2,
			      FILL_X((صحیح)Columns), 0, 0, 0);
    اگر (event->area.y > FILL_Y(صفیں))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, FILL_Y((صحیح)صفیں), 0, 0);

    واپس FALSE;
}
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ساکن gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer ڈیٹا غیر_مستعمل)
{
    اگر (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (صحیح)GDK_PROPERTY_NEW_قدر
	    && GDK_کھڑکی_XID(event->کھڑکی_2) == comm_کھڑکی
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.کھڑکی_2 = comm_کھڑکی;
	xev.xproperty.state = Property_New_قدر;
	serverEventProc(GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(widget)),
								      &xev, 0);
    }
    واپس FALSE;
}
#اگر_ختم // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ساکن خالی
gtk_settings_xft_dpi_بدلا_cb(GtkSettings *gtk_settings غیر_مستعمل,
				    GParamSpec *pspec غیر_مستعمل,
				    gpointer ڈیٹا غیر_مستعمل)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    اگر (gui.متن_context != تہی)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_بنیاد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    اگر (gui.norm_font != تہی)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? تہی : p_guifont, FALSE);
	gui_سیٹ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

قسمتعریف gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
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


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کرو BLINK_NONE  0
#تعریف_کرو BLINK_OFF   1
#تعریف_کرو BLINK_ON    2

ساکن صحیح blink_state = BLINK_NONE;
ساکن long_u blink_waittime = 700;
ساکن long_u blink_ontime = 400;
ساکن long_u blink_offtime = 250;
ساکن guint blink_timer = 0;

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

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    خالی
gui_mch_stop_blink(صحیح may_call_gui_update_cursor)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    اگر (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

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
    gui_may_flush();

    واپس FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    خالی
gui_mch_آغاز_blink(خالی)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.اندر_focus)
    {
	blink_timer = timeout_جوڑو(blink_waittime, blink_cb, تہی);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ساکن gint
enter_notify_event(GtkWidget *widget غیر_مستعمل,
		   Gdk_Event_پار *event غیر_مستعمل,
		   gpointer ڈیٹا غیر_مستعمل)
{
    اگر (blink_state == BLINK_NONE)
	gui_mch_آغاز_blink();

    // make sure keyboard input goes there
    اگر (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    واپس FALSE;
}

    ساکن gint
leave_notify_event(GtkWidget *widget غیر_مستعمل,
		   Gdk_Event_پار *event غیر_مستعمل,
		   gpointer ڈیٹا غیر_مستعمل)
{
    اگر (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    واپس FALSE;
}

    ساکن gint
focus_اندر_event(GtkWidget *widget,
	       GdkEventFocus *event غیر_مستعمل,
	       gpointer ڈیٹا غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI_DIALOG
    اگر (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    ورنہ
#اگر_ختم
	gui_focus_change(TRUE);

    اگر (blink_state == BLINK_NONE)
	gui_mch_آغاز_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    اگر (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    واپس TRUE;
}

    ساکن gint
focus_برآمد_event(GtkWidget *widget غیر_مستعمل,
		GdkEventFocus *event غیر_مستعمل,
		gpointer ڈیٹا غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_GUI_DIALOG
    اگر (gui.dialogs_فعال > 0)
	++gui.dialog_focus_pending;
#اگر_ختم

    gui_focus_change(FALSE);

    اگر (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    واپس TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    ساکن صحیح
keyval_مقصد_string(غیرعلامتی صحیح keyval, char_u *string)
{
    صحیح	    len;
    guint32 uc;

    uc = gdk_keyval_مقصد_unicode(keyval);
    اگر (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((صحیح)uc, string);
    }
    ورنہ
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	انتخاب (keyval)
	{
	    حالت GDK_Tab: حالت GDK_KP_Tab: حالت GDK_ISO_بایاں_Tab:
		string[0] = TAB;
		توڑو;
	    حالت GDK_Linefeed:
		string[0] = NL;
		توڑو;
	    حالت GDK_Return: حالت GDK_ISO_Enter: حالت GDK_3270_Enter:
		string[0] = CAR;
		توڑو;
	    حالت GDK_Escape:
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
    اگر (state & GDK_MOD1_نقاب)
	modifiers |= MOD_نقاب_ALT;
#اگر GTK_CHECK_نسخہ(2,10,0)
    اگر (state & GDK_META_نقاب)
	modifiers |= MOD_نقاب_META;
    اگر (state & GDK_SUPER_نقاب)
	modifiers |= MOD_نقاب_CMD;
#ورنہ
    اگر (state & GDK_MOD4_نقاب)
	modifiers |= MOD_نقاب_CMD;
#اگر_ختم

    واپس modifiers;
}

    ساکن صحیح
modifiers_gdk2mouse(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_سرکاؤ_نقاب)
	modifiers |= MOUSE_سرکاؤ;
    اگر (state & GDK_CONTROL_نقاب)
	modifiers |= MOUSE_CTRL;
    اگر (state & GDK_MOD1_نقاب)
	modifiers |= MOUSE_ALT;

    واپس modifiers;
}

/*
 * Main keyboard handler:
 */
    ساکن gint
کلید_press_event(GtkWidget *widget غیر_مستعمل,
		Gdk_Event_کلید *event,
		gpointer ڈیٹا غیر_مستعمل)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	کلید_sym;
    صحیح		len;
    صحیح		i;
    صحیح		modifiers;
    صحیح		کلید;
    guint	state;
    char_u	*s, *d;
    صحیح		ctrl_سابقہ_added = 0;

    gui.event_وقت = event->وقت;
    کلید_sym = event->keyval;
    state = event->state;

#اگر_تعریف_شدہ FEAT_XIM
    اگر (xim_قطار_کلید_press_event(event, TRUE))
	واپس TRUE;
#اگر_ختم

#اگر_تعریف_شدہ SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    اگر (کلید_sym == SunXK_F36 || کلید_sym == SunXK_F37)
	len = 0;
    ورنہ
#اگر_ختم
    {
	len = keyval_مقصد_string(کلید_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	اگر (len > 1 && داخلہ_conv.vc_type != CONV_NONE)
	    len = convert_داخلہ(string2, len, حجم(string2));

	s = string2;
	d = string;
	برائے (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    اگر (d[-1] == CSI && d + 2 < string + حجم(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (صحیح)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    اگر (کلید_sym == GDK_ISO_بایاں_Tab)
    {
	کلید_sym = GDK_Tab;
	state |= GDK_سرکاؤ_نقاب;
    }

#اگر_تعریف_شدہ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    اگر ((state & GDK_MOD1_نقاب)
	    && gui.menu_is_فعال
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	واپس FALSE;
#اگر_ختم

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
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

#اگر_تعریف_شدہ GDK_کلید_dead_circumflex
    // Belgian Ctrl+[ workaround
    اگر (len == 0 && کلید_sym == GDK_کلید_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_نقاب_CTRL;
	string[3] = '[';
	len = 4;
	جوڑو_مقصد_داخلہ_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	واپس TRUE;
    }
#اگر_ختم

    اگر (len == 0)   // Unrecognized key
	واپس TRUE;

    // For some keys a shift modifier is translated into another key code.
    اگر (len == -3)
	کلید = مقصد_SPECIAL(string[1], string[2]);
    ورنہ
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
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
	// Some keys need adjustment when the Ctrl modifier is used.
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_سرکاؤ_modifier(modifiers, کلید);

	len = mb_char2bytes(کلید, string);
    }

    اگر (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڑو_مقصد_داخلہ_buf(string2, 3);
	اگر (modifiers == 0x4)
	    ctrl_سابقہ_added = 1;
    }

    // Check if the key interrupts.
    {
	صحیح int_ch = check_for_مداخلت(کلید, modifiers);

	اگر (int_ch != NUL)
	{
	    trash_داخلہ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    اگر (ctrl_سابقہ_added && len == 2
		    && ((صحیح)string[0]) == 195
		    && ((صحیح)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    جوڑو_مقصد_داخلہ_buf(string, len);

    // blank out the pointer if necessary
    اگر (p_mh)
	gui_mch_mousehide(TRUE);

    واپس TRUE;
}

#اگر defined(FEAT_XIM) || GTK_CHECK_نسخہ(3,0,0)
    ساکن gboolean
کلید_release_event(GtkWidget *widget غیر_مستعمل,
		  Gdk_Event_کلید *event غیر_مستعمل,
		  gpointer ڈیٹا غیر_مستعمل)
{
# اگر defined(FEAT_XIM)
    gui.event_وقت = event->وقت;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    واپس xim_قطار_کلید_press_event(event, FALSE);
# ورنہ
    واپس TRUE;
# endif
}
#اگر_ختم


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ساکن صحیح اندر_selection_clear_event = FALSE;

    ساکن gint
selection_clear_event(GtkWidget		*widget غیر_مستعمل,
		      GdkEventSelection	*event,
		      gpointer		user_ڈیٹا غیر_مستعمل)
{
    اندر_selection_clear_event = TRUE;
    اگر (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    ورنہ
	clip_lose_selection(&clip_ستارہ);
    اندر_selection_clear_event = FALSE;

    واپس TRUE;
}

#تعریف_کرو RS_NONE	0	// selection_received_cb() not called yet
#تعریف_کرو RS_OK	1	// selection_received_cb() called and OK
#تعریف_کرو RS_FAIL	2	// selection_received_cb() called and failed
ساکن صحیح received_selection = RS_NONE;

    ساکن خالی
selection_received_cb(GtkWidget		*widget غیر_مستعمل,
		      Gtk_Selection_ڈیٹا	*ڈیٹا,
		      guint		وقت_2 غیر_مستعمل,
		      gpointer		user_ڈیٹا غیر_مستعمل)
{
    Clipboard_T	    *cbd;
    char_u	    *متن;
    char_u	    *tmpbuf = تہی;
    guchar	    *tmpbuf_utf8 = تہی;
    صحیح		    len;
    صحیح		    motion_type = MAUTO;

    اگر (gtk_selection_ڈیٹا_get_selection(ڈیٹا) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ورنہ
	cbd = &clip_ستارہ;

    متن = (char_u *)gtk_selection_ڈیٹا_get_ڈیٹا(ڈیٹا);
    len = gtk_selection_ڈیٹا_get_لمبائی(ڈیٹا);

    اگر (متن == تہی || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	واپس;
    }

    اگر (gtk_selection_ڈیٹا_get_ڈیٹا_type(ڈیٹا) == vim_atom
	    || gtk_selection_ڈیٹا_get_ڈیٹا_type(ڈیٹا) == vim_mt_atom)
    {
	motion_type = *متن++;
	--len;
    }
    ورنہ اگر (gtk_selection_ڈیٹا_get_ڈیٹا_type(ڈیٹا) == vimenc_atom
	    || gtk_selection_ڈیٹا_get_ڈیٹا_type(ڈیٹا) == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;

	motion_type = *متن++;
	--len;

	enc = متن;
	متن += STRLEN(متن) + 1;
	len -= متن - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_تیاری(&conv, enc, p_enc);
	اگر (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, متن, &len);
	    اگر (tmpbuf != تہی)
		متن = tmpbuf;
	    convert_تیاری(&conv, تہی, تہی);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    ورنہ
    {
	tmpbuf_utf8 = gtk_selection_ڈیٹا_get_متن(ڈیٹا);
	اگر (tmpbuf_utf8 != تہی)
	{
	    len = STRLEN(tmpbuf_utf8);
	    اگر (داخلہ_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&داخلہ_conv, tmpbuf_utf8, &len);
		اگر (tmpbuf != تہی)
		    متن = tmpbuf;
	    }
	    ورنہ
		متن = tmpbuf_utf8;
	}
	ورنہ اگر (len >= 2 && متن[0] == 0xff && متن[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_تیاری_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    اگر (conv.vc_type != CONV_NONE)
	    {
		متن += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, متن, &len);
		convert_تیاری(&conv, تہی, تہی);
	    }
	    اگر (tmpbuf != تہی)
		متن = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    جب (len > 0 && متن[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, متن, (طویل)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ساکن خالی
selection_get_cb(GtkWidget	    *widget غیر_مستعمل,
		 Gtk_Selection_ڈیٹا   *selection_ڈیٹا,
		 guint		    info,
		 guint		    وقت_2 غیر_مستعمل,
		 gpointer	    user_ڈیٹا غیر_مستعمل)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    صحیح		    لمبائی;
    صحیح		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    اگر (gtk_selection_ڈیٹا_get_selection(selection_ڈیٹا)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ورنہ
	cbd = &clip_ستارہ;

    اگر (!cbd->owned)
	واپس;			// Shouldn't ever happen

    اگر (info != (guint)ہدف_STRING
	    && (!clip_html || info != (guint)ہدف_HTML)
	    && info != (guint)ہدف_UTF8_STRING
	    && info != (guint)ہدف_VIMENC
	    && info != (guint)ہدف_VIM
	    && info != (guint)ہدف_VIMENC_MT
	    && info != (guint)ہدف_VIM_MT
	    && info != (guint)ہدف_COMPOUND_متن
	    && info != (guint)ہدف_متن_PLAIN
	    && info != (guint)ہدف_متن_PLAIN_UTF8
	    && info != (guint)ہدف_متن)
	واپس;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    اگر (motion_type < 0 || string == تہی)
	واپس;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    لمبائی = MIN(tmplen, (long_u)(G_MAXINT - 1));

    اگر (info == (guint)ہدف_VIM || info == (guint)ہدف_VIM_MT)
    {
	tmpbuf = alloc(لمبائی + 1);
	اگر (tmpbuf != تہی)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)لمبائی);
	}
	// For our own format, the first byte contains the motion type
	++لمبائی;
	vim_free(string);
	string = tmpbuf;
	اگر (info == (guint)ہدف_VIM)
	    type = vim_atom;
	ورنہ
	    type = vim_mt_atom;
    }

    ورنہ اگر (info == (guint)ہدف_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_تیاری_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	اگر (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &لمبائی);
	    convert_تیاری(&conv, تہی, تہی);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	اگر (string != تہی)
	{
	    tmpbuf = alloc(لمبائی + 2);
	    اگر (tmpbuf != تہی)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)لمبائی);
		vim_free(string);
		string = tmpbuf;
		لمبائی += 2;
	    }

#اگر !GTK_CHECK_نسخہ(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_ڈیٹا->type = selection_ڈیٹا->ہدف;
	    selection_ڈیٹا->format = 16;	// 16 bits per char
#اگر_ختم
	    gtk_selection_ڈیٹا_سیٹ(selection_ڈیٹا, html_atom, 16,
							      string, لمبائی);
	    vim_free(string);
	}
	واپس;
    }
    ورنہ اگر (info == (guint)ہدف_VIMENC || info == (guint)ہدف_VIMENC_MT)
    {
	صحیح l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(لمبائی + l + 2);
	اگر (tmpbuf != تہی)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)لمبائی);
	    لمبائی += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	اگر (info == (guint)ہدف_VIMENC)
	    type = vimenc_atom;
	ورنہ
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    ورنہ
    {
	اگر (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &لمبائی);
	    vim_free(string);
	    اگر (tmpbuf == تہی)
		واپس;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	اگر (g_utf8_validate((ثابت حرف *)string, (gssize)لمبائی, تہی))
	{
	    gtk_selection_ڈیٹا_سیٹ_متن(selection_ڈیٹا,
					(ثابت حرف *)string, لمبائی);
	}
	vim_free(string);
	واپس;
    }

    اگر (string != تہی)
    {
#اگر !GTK_CHECK_نسخہ(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_ڈیٹا->type = selection_ڈیٹا->ہدف;
	selection_ڈیٹا->format = 8;	// 8 bits per char
#اگر_ختم
	gtk_selection_ڈیٹا_سیٹ(selection_ڈیٹا, type, 8, string, لمبائی);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    صحیح
gui_mch_early_init_check(صحیح give_پیغام)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    اگر ((p == تہی || *p == NUL) && (q == تہی || *q == NUL))
    {
	gui.dying = true;
	اگر (give_پیغام)
	    emsg(_((حرف *)e_cannot_open_display));
	واپس FAIL;
    }
    واپس OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    صحیح
gui_mch_init_check(خالی)
{
#اگر_تعریف_شدہ USE_GRESOURCE
    ساکن صحیح res_registered = FALSE;

    اگر (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_رجسٹر_resource();
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GNOME
    اگر (gtk_socket_id == 0)
	using_gnome = 1;
#اگر_ختم

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_سیٹ_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    اگر (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((حرف *)e_cannot_open_display));
	واپس FAIL;
    }

    واپس OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


ساکن guint mouse_click_timer = 0;
ساکن صحیح mouse_timed_برآمد = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ساکن timeout_cb_type
mouse_click_timer_cb(gpointer ڈیٹا)
{
    // we don't use this information currently
    صحیح *timed_برآمد = (صحیح *) ڈیٹا;

    *timed_برآمد = TRUE;
    واپس FALSE;		// don't happen again
}

ساکن guint		motion_repeat_timer  = 0;
ساکن صحیح		motion_repeat_انحراف = FALSE;
ساکن timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ساکن خالی
process_motion_notify(صحیح x, صحیح y, GdkModifierType state)
{
    صحیح	    button;
    int_u   vim_modifiers;
    Gtk_تخصیص تخصیص;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب |
		       GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب |
		       GDK_BUTTON5_نقاب))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    اگر (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	واپس;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_تخصیص(gui.drawarea, &تخصیص);

    اگر (x < 0 || y < 0
	    || x >= تخصیص.چوڑائی
	    || y >= تخصیص.اونچائی)
    {

	صحیح dx;
	صحیح dy;
	صحیح offshoot;
	صحیح delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - تخصیص.چوڑائی;
	dy = y < 0 ? -y : y - تخصیص.اونچائی;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	اگر (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	ورنہ
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	اگر (!motion_repeat_timer)
	    motion_repeat_timer = timeout_جوڑو(delay, motion_repeat_timer_cb,
									 تہی);
    }
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن GdkDevice *
gui_gtk_get_اشارہ_گر_device(GtkWidget *widget)
{
    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(widget);
    GdkDisplay * ثابت dpy = gdk_کھڑکی_get_display(win);
# اگر GTK_CHECK_نسخہ(3,20,0)
    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
    واپس gdk_seat_get_اشارہ_گر(seat);
# ورنہ
    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
    واپس gdk_device_manager_get_client_اشارہ_گر(mngr);
# endif
}

    ساکن Gdk_کھڑکی *
gui_gtk_get_اشارہ_گر(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(widget);
    GdkDevice * ثابت dev = gui_gtk_get_اشارہ_گر_device(widget);
    واپس gdk_کھڑکی_get_device_position(win, dev , x, y, state);
}

# اگر defined(FEAT_GUI_TABLINE)
    ساکن Gdk_کھڑکی *
gui_gtk_کھڑکی_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ثابت dev = gui_gtk_get_اشارہ_گر_device(widget);
    واپس gdk_device_get_کھڑکی_at_position(dev, x, y);
}
# endif
#ورنہ // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_اشارہ_گر(wid, x, y, s) \
    gdk_کھڑکی_get_اشارہ_گر((wid)->کھڑکی_2, x, y, s)
# define gui_gtk_کھڑکی_at_position(wid, x, y)	gdk_کھڑکی_at_اشارہ_گر(x, y)
#اگر_ختم

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ساکن timeout_cb_type
motion_repeat_timer_cb(gpointer ڈیٹا غیر_مستعمل)
{
    صحیح		    x;
    صحیح		    y;
    GdkModifierType state;

    gui_gtk_get_اشارہ_گر(gui.drawarea, &x, &y, &state);

    اگر (!(state & (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب |
		   GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب |
		   GDK_BUTTON5_نقاب)))
    {
	motion_repeat_timer = 0;
	واپس FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    اگر (vim_مستعمل_اندر_داخلہ_buf() > 10)
	واپس TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    اگر (motion_repeat_انحراف)
	x += gui.char_چوڑائی;

    motion_repeat_انحراف = !motion_repeat_انحراف;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    واپس FALSE;
}

    ساکن gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer ڈیٹا غیر_مستعمل)
{
    اگر (event->is_hint)
    {
	صحیح		x;
	صحیح		y;
	GdkModifierType	state;

	gui_gtk_get_اشارہ_گر(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    ورنہ
    {
	process_motion_notify((صحیح)event->x, (صحیح)event->y,
			      (GdkModifierType)event->state);
    }

    واپس TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    ساکن gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer ڈیٹا غیر_مستعمل)
{
    صحیح button;
    صحیح repeated_click = FALSE;
    صحیح x, y;
    int_u vim_modifiers;

    gui.event_وقت = event->وقت;

    // Make sure we have focus now we've been selected
    اگر (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    اگر (event->type != GDK_BUTTON_PRESS)
	واپس FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    اگر (!mouse_timed_برآمد && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_برآمد = FALSE;
    mouse_click_timer = timeout_جوڑو(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_برآمد);

    انتخاب (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	حالت 1:
		button = MOUSE_بایاں;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_نقاب;
		توڑو;
	حالت 2: button = MOUSE_درمیان; توڑو;
	حالت 3: button = MOUSE_دایاں; توڑو;
	حالت 8: button = MOUSE_X1; توڑو;
	حالت 9: button = MOUSE_X2; توڑو;
	طےشدہ:
	    واپس FALSE;		// Unknown button
    }

#اگر_تعریف_شدہ FEAT_XIM
    // cancel any preediting
    اگر (im_is_preediting())
	xim_reset();
#اگر_ختم

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    واپس TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    ساکن gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer ڈیٹا غیر_مستعمل)
{
    صحیح	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#اگر GTK_CHECK_نسخہ(3,4,0)
    ساکن دوہرا  acc_x, acc_y;
# اگر !GTK_CHECK_نسخہ(3,22,0)
    ساکن guint32 آخری_smooth_event_وقت;
# endif
#اگر_ختم

    اگر (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    انتخاب (event->direction)
    {
	حالت GDK_SCROLL_اوپر:
	    button = MOUSE_4;
	    توڑو;
	حالت GDK_SCROLL_نیچے_کو:
	    button = MOUSE_5;
	    توڑو;
	حالت GDK_SCROLL_بایاں:
	    button = MOUSE_7;
	    توڑو;
	حالت GDK_SCROLL_دایاں:
	    button = MOUSE_6;
	    توڑو;
#اگر GTK_CHECK_نسخہ(3,4,0)
	حالت GDK_SCROLL_SMOOTH:
	    اگر (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		واپس FALSE;
	    }
# اگر GTK_CHECK_نسخہ(3,22,0)
	    اگر (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# ورنہ
	    اگر (event->وقت - آخری_smooth_event_وقت > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# اگر !GTK_CHECK_نسخہ(3,22,0)
	    آخری_smooth_event_وقت = event->وقت;
# endif
	    توڑو;
#اگر_ختم
	طےشدہ: // This shouldn't happen
	    واپس FALSE;
    }

#اگر_تعریف_شدہ FEAT_XIM
    // cancel any preediting
    اگر (im_is_preediting())
	xim_reset();
#اگر_ختم

    vim_modifiers = modifiers_gdk2mouse(event->state);

#اگر GTK_CHECK_نسخہ(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    اگر (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	جب (acc_x >= 1.0)
	{ //دایاں
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	جب (acc_x <= -1.0)
	{ //بایاں
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	جب (acc_y >= 1.0)
	{ //نیچے_کو
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	جب (acc_y <= -1.0)
	{ //اوپر
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
    }
    ورنہ
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    اگر (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    ورنہ
#اگر_ختم
	gui_send_mouse_event(button, (صحیح)event->x, (صحیح)event->y,
		FALSE, vim_modifiers);

    واپس TRUE;
}


    ساکن gint
button_release_event(GtkWidget *widget غیر_مستعمل,
		     GdkEventButton *event,
		     gpointer ڈیٹا غیر_مستعمل)
{
    صحیح x, y;
    int_u vim_modifiers;

    gui.event_وقت = event->وقت;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    اگر (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    انتخاب (event->button)
    {
	حالت 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    توڑو;
    }

    واپس TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    ساکن خالی
grab_notify_event(GtkWidget *widget غیر_مستعمل,
		  gboolean  was_grabbed,
		  gpointer  ڈیٹا غیر_مستعمل)
{
    اگر (!was_grabbed)
	dragging_button_state = 0;
}


#اگر_تعریف_شدہ FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ساکن صحیح
گنتی_and_decode_uri_list(char_u *برآمد, char_u *raw, صحیح len)
{
    صحیح		i;
    char_u	*p = برآمد;
    صحیح		گنتی = 0;

    برائے (i = 0; i < len; ++i)
    {
	اگر (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    اگر (p > برآمد && p[-1] != NUL)
	    {
		++گنتی;
		*p++ = NUL;
	    }
	}
	ورنہ اگر (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	ورنہ
	    *p++ = raw[i];
    }
    اگر (p > برآمد && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++گنتی;
    }
    واپس گنتی;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    ساکن صحیح
filter_uri_list(char_u **outlist, صحیح max, char_u *src)
{
    صحیح	i, j;

    برائے (i = j = 0; i < max; ++i)
    {
	outlist[i] = تہی;
	اگر (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    اگر (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    جب (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    واپس j;
}

    ساکن char_u **
parse_uri_list(صحیح *گنتی, char_u *ڈیٹا, صحیح len)
{
    صحیح	    n	    = 0;
    char_u  *tmp    = تہی;
    char_u  **array = تہی;

    اگر (ڈیٹا != تہی && len > 0 && (tmp = alloc(len + 1)) != تہی)
    {
	n = گنتی_and_decode_uri_list(tmp, ڈیٹا, len);
	اگر (n > 0 && (array = ALLOC_MULT(char_u *, n)) != تہی)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *گنتی = n;
    واپس array;
}

    ساکن خالی
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_ڈیٹا	*ڈیٹا,
		     guint		وقت_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    صحیح	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_ڈیٹا_get_ڈیٹا(ڈیٹا),
	    gtk_selection_ڈیٹا_get_لمبائی(ڈیٹا));

    اگر (fnames != تہی && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_اختتام(context, TRUE, FALSE, وقت_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    ورنہ
	vim_free(fnames);
}

    ساکن خالی
drag_handle_متن(GdkDragContext	    *context,
		 Gtk_Selection_ڈیٹا   *ڈیٹا,
		 guint		    وقت_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *متن;
    صحیح	    len;
    char_u  *tmpbuf = تہی;

    متن = (char_u *)gtk_selection_ڈیٹا_get_ڈیٹا(ڈیٹا);
    len = gtk_selection_ڈیٹا_get_لمبائی(ڈیٹا);

    اگر (gtk_selection_ڈیٹا_get_ڈیٹا_type(ڈیٹا) == utf8_string_atom)
    {
	اگر (داخلہ_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&داخلہ_conv, متن, &len);
	اگر (tmpbuf != تہی)
	    متن = tmpbuf;
    }

    dnd_yank_drag_ڈیٹا(متن, (طویل)len);
    gtk_drag_اختتام(context, TRUE, FALSE, وقت_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    اگر (dropkey[2] != 0)
	جوڑو_مقصد_داخلہ_buf(dropkey, (صحیح)حجم(dropkey));
    ورنہ
	جوڑو_مقصد_داخلہ_buf(dropkey + 3, (صحیح)(حجم(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ساکن خالی
drag_ڈیٹا_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_ڈیٹا	*ڈیٹا,
		      guint		info,
		      guint		وقت_2,
		      gpointer		user_ڈیٹا غیر_مستعمل)
{
    GdkModifierType state;

    // Guard against trash
    ثابت guchar * ثابت ڈیٹا_ڈیٹا = gtk_selection_ڈیٹا_get_ڈیٹا(ڈیٹا);
    ثابت gint ڈیٹا_لمبائی = gtk_selection_ڈیٹا_get_لمبائی(ڈیٹا);
    ثابت gint ڈیٹا_format = gtk_selection_ڈیٹا_get_format(ڈیٹا);

    اگر (ڈیٹا_ڈیٹا == تہی
	    || ڈیٹا_لمبائی <= 0
	    || ڈیٹا_format != 8
	    || ڈیٹا_ڈیٹا[ڈیٹا_لمبائی] != '\0')
    {
	gtk_drag_اختتام(context, FALSE, FALSE, وقت_2);
	واپس;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_اشارہ_گر(widget, تہی, تہی, &state);

    // Not sure about the role of "text/plain" here...
    اگر (info == (guint)ہدف_متن_URI_LIST)
	drag_handle_uri_list(context, ڈیٹا, وقت_2, state, x, y);
    ورنہ
	drag_handle_متن(context, ڈیٹا, وقت_2, state);

}
#اگر_ختم // FEAT_DND


#اگر defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ساکن خالی
sm_client_check_بدلا_any(GnomeClient	    *client غیر_مستعمل,
			    gint	    کلید,
			    GnomeDialogType type غیر_مستعمل,
			    gpointer	    ڈیٹا غیر_مستعمل)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# اگر defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_بدلا_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    برآمد_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_کلید_return(کلید, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    ساکن gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase غیر_مستعمل,
			GnomeSaveStyle	    save_style غیر_مستعمل,
			gboolean	    shutdown غیر_مستعمل,
			GnomeInteractStyle  interact_style,
			gboolean	    fast غیر_مستعمل,
			gpointer	    ڈیٹا غیر_مستعمل)
{
    ساکن ثابت حرف	suffix[] = "-session.vim";
    حرف		*session_file;
    غیرعلامتی صحیح	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    اگر (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_معمول,
					 &sm_client_check_بدلا_any,
					 تہی);
    برآمد_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_راستہ(
			gnome_client_get_config_سابقہ(client));
    len = لڑی_لمبائی(session_file);

    اگر (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(حرف, session_file, len + حجم(suffix));
    حافظہ_نقل(session_file + len, suffix, حجم(suffix));

    success = write_session_file((char_u *)session_file);

    اگر (success)
    {
	ثابت حرف  *argv[8];
	صحیح	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = تہی;

	gnome_client_سیٹ_discard_command(client, i, (حرف **)argv);

	// Tell the session manager how to restore the just saved session.
	// This is easily done thanks to Vim's -S option.  Pass the -f flag
	// since there's no need to fork -- it might even cause confusion.
	// Also pass the window role to give the WM something to match on.
	// The role is set in gui_mch_open(), thus should _never_ be NULL.
	i = 0;
	argv[i++] = restart_command;
	argv[i++] = "-f";
	argv[i++] = "-g";
	argv[i++] = "--role";
	argv[i++] = gtk_کھڑکی_get_role(GTK_کھڑکی(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = تہی;

	gnome_client_سیٹ_restart_command(client, i, (حرف **)argv);
	gnome_client_سیٹ_clone_command(client, 0, تہی);
    }

    g_free(session_file);

    واپس success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    ساکن خالی
sm_client_die(GnomeClient *client غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
    // Don't write messages to the GUI anymore
    بھرا_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    ساکن خالی
تیاری_save_yourself(خالی)
{
    GnomeClient *client;

    client = gnome_master_client();
    اگر (client == تہی)
	واپس;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), تہی);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), تہی);
}

#ورنہ // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    ساکن gboolean
local_xsmp_handle_درخواستیں(
    GIOChannel		*ماخذ غیر_مستعمل,
    GIOCondition	condition,
    gpointer		ڈیٹا)
{
    اگر (condition == G_IO_اندر)
    {
	// Do stuff; maybe close connection
	اگر (xsmp_handle_درخواستیں() == FAIL)
	    g_io_channel_unref((GIOChannel *)ڈیٹا);
	واپس TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)ڈیٹا);
    xsmp_close();
    واپس TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    ساکن خالی
تیاری_save_yourself(خالی)
{
    Atom    *existing_atoms = تہی;
    صحیح	    گنتی = 0;

# ifdef USE_XSMP
    اگر (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_جوڑو_watch(g_io, G_IO_اندر | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_درخواستیں, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    ورنہ
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	اگر (!dpy)
	    واپس;

	Gdk_کھڑکی * ثابت mainwin_win = gtk_widget_get_کھڑکی(gui.mainwin);
	اگر (XGetWMProtocols(dpy, GDK_کھڑکی_XID(mainwin_win),
		    &existing_atoms, &گنتی))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    صحیح	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    برائے (i = 0; i < گنتی; ++i)
		اگر (existing_atoms[i] == save_yourself_xatom)
		    توڑو;

	    اگر (i == گنتی)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, گنتی + 1);
		اگر (new_atoms != تہی)
		{
		    حافظہ_نقل(new_atoms, existing_atoms, گنتی * حجم(Atom));
		    new_atoms[گنتی] = save_yourself_xatom;
		    XSetWMProtocols(GDK_کھڑکی_XDISPLAY(mainwin_win),
			    GDK_کھڑکی_XID(mainwin_win),
			    new_atoms, گنتی + 1);
		    vim_free(new_atoms);
		}
	    }
	    XFree(existing_atoms);
	}
    }
}

/*
 * Installing a global event filter seems to be the only way to catch
 * client messages of type WM_PROTOCOLS without overriding GDK's own
 * client message event filter.  Well, that's still better than trying
 * to guess what the GDK filter had done if it had been invoked instead
 *
 * GTK2_FIXME:	This doesn't seem to work.  For some reason we never
 * receive WM_SAVE_YOURSELF even though everything is set up correctly.
 * I have the nasty feeling modern session managers just don't send this
 * deprecated message anymore.	Addition: confirmed by several people.
 *
 * The GNOME session support is much cooler anyway.  Unlike this ugly
 * WM_SAVE_YOURSELF hack it actually stores the session...  And yes,
 * it should work with KDE as well.
 */
    ساکن GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event غیر_مستعمل,
		    gpointer ڈیٹا غیر_مستعمل)
{
    XEvent *xevent = (XEvent *)xev;

    اگر (xevent != تہی
	    && xevent->type == Client_پیغام
	    && xevent->xclient.پیغام_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.ڈیٹا.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	برآمد_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin)),
		    GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.mainwin)),
		    تہی, 0);
	واپس GDK_FILTER_REMOVE;
    }

    واپس GDK_FILTER_CONTINUE;
}
#اگر_ختم // !USE_GNOME_SESSION

    ساکن GdkPixbuf *
pixbuf_new_from_png_ڈیٹا(ثابت غیرعلامتی حرف *ڈیٹا, غیرعلامتی صحیح len)
{
    GInput_دھارا *دھارا;
    GdkPixbuf    *pixbuf;

    دھارا = g_حافظہ_داخلہ_دھارا_new_from_ڈیٹا(ڈیٹا, len, تہی);
    pixbuf = gdk_pixbuf_new_from_دھارا(دھارا, تہی, تہی);
    g_object_unref(دھارا);
    واپس pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ساکن خالی
mainwin_realize(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
#شامل "../runtime/vim16x16_png.h"
#شامل "../runtime/vim32x32_png.h"
#شامل "../runtime/vim48x48_png.h"

    Gdk_کھڑکی * ثابت mainwin_win = gtk_widget_get_کھڑکی(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    اگر (echo_wid_arg)
    {
	اگر (gui_mch_get_display())
	    لکھو("WID: %ld\n", (طویل)GDK_کھڑکی_XID(mainwin_win));
	ورنہ
	    لکھو("WID: 0\n");
	fflush(stdout);
    }

    اگر (vim_strchr(p_go, GO_ICON) != تہی)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	اگر (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_کھڑکی_سیٹ_icon_name(GTK_کھڑکی(gui.mainwin), "gvim");
	ورنہ
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = تہی;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_ڈیٹا(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ڈیٹا(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ڈیٹا(vim48x48_png, vim48x48_png_len));

	    gtk_کھڑکی_سیٹ_icon_list(GTK_کھڑکی(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(خالی *)&g_object_unref, تہی);
	    g_list_free(icons);
	}
    }

#اگر !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_کھڑکی_جوڑو_filter(تہی, &global_event_filter, تہی);
#اگر_ختم
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#اگر defined(USE_GNOME_SESSION)
    اگر (using_gnome)
#اگر_ختم
	تیاری_save_yourself();

#اگر_تعریف_شدہ FEAT_CLIENTSERVER
    اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 && gui_mch_get_display())
    {
	اگر (serverName == تہی && server_Delayed_آغاز_Name != تہی)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_کھڑکی = GDK_کھڑکی_XID(mainwin_win);

	    (خالی)server_رجسٹر_Name(GDK_کھڑکی_XDISPLAY(mainwin_win),
				    server_Delayed_آغاز_Name);
	}
	ورنہ
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_کھڑکی(GDK_کھڑکی_XDISPLAY(mainwin_win),
					GDK_کھڑکی_XID(mainwin_win));
	}
	gtk_widget_جوڑو_events(gui.mainwin, GDK_PROPERTY_CHANGE_نقاب);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), تہی);
    }
#اگر_ختم
}

    ساکن GdkCursor *
create_blank_اشارہ_گر(خالی)
{
    GdkCursor	*cursor;

#اگر GTK_CHECK_نسخہ(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#ورنہ
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_کھڑکی	*جڑ_کھڑکی = تہی;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_نقاب;
    حرف	blank_ڈیٹا[] = { 0x0 };
    جڑ_کھڑکی = gtk_widget_get_جڑ_کھڑکی(gui.mainwin);
    blank_نقاب = gdk_bitmap_create_from_ڈیٹا(جڑ_کھڑکی, blank_ڈیٹا, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_نقاب, blank_نقاب,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_نقاب);
#اگر_ختم

    واپس cursor;
}

    ساکن خالی
mainwin_screen_بدلا_cb(GtkWidget  *widget,
			  GdkScreen  *پچھلا_screen غیر_مستعمل,
			  gpointer   ڈیٹا غیر_مستعمل)
{
    اگر (!gtk_widget_has_screen(widget))
	واپس;

    /*
     * Recreate the invisible mouse cursor.
     */
    اگر (gui.blank_اشارہ_گر != تہی)
#اگر GTK_CHECK_نسخہ(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_اشارہ_گر));
#ورنہ
	gdk_cursor_unref(gui.blank_اشارہ_گر);
#اگر_ختم

    gui.blank_اشارہ_گر = create_blank_اشارہ_گر();

    اگر (gui.اشارہ_گر_hidden && gtk_widget_get_کھڑکی(gui.drawarea) != تہی)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		gui.blank_اشارہ_گر);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    اگر (gui.متن_context != تہی)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(widget);
    pango_context_سیٹ_بنیاد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    اگر (gui.norm_font != تہی)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_سیٹ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ساکن gboolean
mainwin_state_event_cb(GtkWidget *widget غیر_مستعمل,
		       Gdk_Event_کھڑکی_State *event,
		       gpointer user_ڈیٹا غیر_مستعمل)
{
    اگر (event->new_کھڑکی_state & GDK_کھڑکی_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    واپس FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    ساکن خالی
drawarea_realize_cb(GtkWidget *widget, gpointer ڈیٹا غیر_مستعمل)
{
    GtkWidget *sbar;
    Gtk_تخصیص تخصیص;

#اگر_تعریف_شدہ FEAT_XIM
    xim_init();
#اگر_ختم
    gui_mch_new_colors();
#اگر GTK_CHECK_نسخہ(3,0,0)
    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_چوڑائی(widget),
	    gtk_widget_get_allocated_اونچائی(widget));
#ورنہ
    gui.متن_gc = gdk_gc_new(gui.drawarea->کھڑکی_2);
#اگر_ختم

    gui.blank_اشارہ_گر = create_blank_اشارہ_گر();
    اگر (gui.اشارہ_گر_hidden)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(widget), gui.blank_اشارہ_گر);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_بایاں].id;
    اگر (!sbar || (!gui.which_scrollbars[SBAR_بایاں]
				    && firstwin->w_scrollbars[SBAR_دایاں].id))
	sbar = firstwin->w_scrollbars[SBAR_دایاں].id;
    gtk_widget_get_تخصیص(sbar, &تخصیص);
    اگر (sbar && gtk_widget_get_realized(sbar) && تخصیص.چوڑائی)
	gui.scrollbar_چوڑائی = تخصیص.چوڑائی;

    sbar = gui.bottom_sbar.id;
    اگر (sbar && gtk_widget_get_realized(sbar) && تخصیص.اونچائی)
	gui.scrollbar_اونچائی = تخصیص.اونچائی;
}

/*
 * Properly clean up on shutdown.
 */
    ساکن خالی
drawarea_unrealize_cb(GtkWidget *widget غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
    // Don't write messages to the GUI anymore
    بھرا_screen = FALSE;

#اگر_تعریف_شدہ FEAT_XIM
    im_shutdown();
#اگر_ختم
    اگر (gui.ascii_glyphs != تہی)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = تہی;
    }
    اگر (gui.ascii_font != تہی)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = تہی;
    }
    g_object_unref(gui.متن_context);
    gui.متن_context = تہی;

#اگر GTK_CHECK_نسخہ(3,0,0)
    اگر (gui.surface != تہی)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = تہی;
    }
#ورنہ
    g_object_unref(gui.متن_gc);
    gui.متن_gc = تہی;
#اگر_ختم

#اگر GTK_CHECK_نسخہ(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_اشارہ_گر));
#ورنہ
    gdk_cursor_unref(gui.blank_اشارہ_گر);
#اگر_ختم
    gui.blank_اشارہ_گر = تہی;
}

#اگر GTK_CHECK_نسخہ(3,22,2)
    ساکن خالی
drawarea_style_updated_cb(GtkWidget *widget غیر_مستعمل,
			 gpointer ڈیٹا غیر_مستعمل)
#ورنہ
    ساکن خالی
drawarea_style_سیٹ_cb(GtkWidget	*widget غیر_مستعمل,
		      GtkStyle	*پچھلا_style غیر_مستعمل,
		      gpointer	ڈیٹا غیر_مستعمل)
#اگر_ختم
{
    gui_mch_new_colors();
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       ڈیٹا غیر_مستعمل)
{
    ساکن صحیح cur_چوڑائی = 0;
    ساکن صحیح cur_اونچائی = 0;

    g_return_val_if_fail(event
	    && event->چوڑائی >= 1 && event->اونچائی >= 1, TRUE);

# اگر GTK_CHECK_نسخہ(3,22,2) && !GTK_CHECK_نسخہ(3,22,4)
    // As of 3.22.2, GdkWindows have started distributing configure events to
    // their "native" children (https://git.gnome.org/browse/gtk+/commit/?h=gtk-3-22&id=12579fe71b3b8f79eb9c1b80e429443bcc437dd0).
    //
    // As can be seen from the implementation of move_native_children() and
    // configure_native_child() in gdkwindow.c, those functions actually
    // propagate configure events to every child, failing to distinguish
    // "native" one from non-native one.
    //
    // Naturally, configure events propagated to here like that are fallacious
    // and, as a matter of fact, they trigger a geometric collapse of
    // gui.drawarea in fullscreen and maximized modes.
    //
    // To filter out such nuisance events, we are making use of the fact that
    // the field send_event of such GdkEventConfigures is set to FALSE in
    // configure_native_child().
    //
    // Obviously, this is a terrible hack making GVim depend on GTK's
    // implementation details.  Therefore, watch out any relevant internal
    // changes happening in GTK in the feature (sigh).
    //
    // Follow-up
    // After a few weeks later, the GdkWindow change mentioned above was
    // reverted (https://git.gnome.org/browse/gtk+/commit/?h=gtk-3-22&id=f70039cb9603a02d2369fec4038abf40a1711155).
    // The corresponding official release is 3.22.4.
    اگر (event->send_event == FALSE)
	واپس TRUE;
# endif

    اگر (event->چوڑائی == cur_چوڑائی && event->اونچائی == cur_اونچائی)
	واپس TRUE;

    cur_چوڑائی = event->چوڑائی;
    cur_اونچائی = event->اونچائی;

    اگر (gui.surface != تہی)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->چوڑائی, event->اونچائی);

    gtk_widget_قطار_draw(widget);

    واپس TRUE;
}
#اگر_ختم

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ساکن gint
delete_event_cb(GtkWidget *widget غیر_مستعمل,
		GdkEventAny *event غیر_مستعمل,
		gpointer ڈیٹا غیر_مستعمل)
{
    gui_shell_closed();
    واپس TRUE;
}

#اگر defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساکن صحیح
get_شے_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation شے_orientation = GTK_ORIENTATION_HORIZONTAL;

    اگر (using_gnome && widget != تہی)
    {
	GtkWidget *والد;
	Bonobo_Dock_شے *dockitem;

	والد	 = gtk_widget_get_والد(widget);
	اگر (G_TYPE_FROM_INSTANCE(والد) == BONOBO_TYPE_DOCK_شے)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = والد;
	    dockitem = BONOBO_DOCK_شے(widget);

	    اگر (dockitem == تہی || dockitem->is_floating)
		واپس 0;
	    شے_orientation = bonobo_dock_شے_get_orientation(dockitem);
	}
    }
# ورنہ
#  define شے_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    اگر (widget != تہی
	    && شے_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# اگر GTK_CHECK_نسخہ(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_تخصیص تخصیص;

	gtk_widget_get_تخصیص(widget, &تخصیص);
	اگر (تخصیص.اونچائی > 1)
	    واپس تخصیص.اونچائی;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  اگر GTK_CHECK_نسخہ(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_اونچائی(widget, &min_h, &natural_h);
	    واپس MAX(min_h, natural_h);
	}
#  ورنہ
	واپس تخصیص.اونچائی;
#  endif
# ورنہ
	اگر (orientation == GTK_ORIENTATION_HORIZONTAL)
	    واپس widget->تخصیص.اونچائی;
	ورنہ
	    واپس widget->تخصیص.چوڑائی;
# endif
    }
    واپس 0;
}
#اگر_ختم

    ساکن صحیح
get_menu_tool_چوڑائی(خالی)
{
    صحیح چوڑائی = 0;

#اگر_تعریف_شدہ FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    چوڑائی += get_شے_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    چوڑائی += get_شے_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    اگر (gui.tabline != تہی)
	چوڑائی += get_شے_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#اگر_ختم

    واپس چوڑائی;
}

    ساکن صحیح
get_menu_tool_اونچائی(خالی)
{
    صحیح اونچائی = 0;

#اگر_تعریف_شدہ FEAT_MENU
    اونچائی += get_شے_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_TOOLBAR
    اونچائی += get_شے_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    اگر (gui.tabline != تہی
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	اونچائی += get_شے_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#اگر_ختم

    واپس اونچائی;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ساکن صحیح init_کھڑکی_hints_state = 0;

    ساکن خالی
update_کھڑکی_manager_hints(صحیح force_چوڑائی, صحیح force_اونچائی)
{
    ساکن صحیح old_چوڑائی  = 0;
    ساکن صحیح old_اونچائی = 0;
    ساکن صحیح old_min_چوڑائی  = 0;
    ساکن صحیح old_min_اونچائی = 0;
    ساکن صحیح old_char_چوڑائی  = 0;
    ساکن صحیح old_char_اونچائی = 0;

    صحیح چوڑائی;
    صحیح اونچائی;
    صحیح min_چوڑائی;
    صحیح min_اونچائی;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    اگر (!(force_چوڑائی && force_اونچائی)  &&  init_کھڑکی_hints_state > 0)
    {
	// Don't do it!
	init_کھڑکی_hints_state = 2;
	واپس;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    چوڑائی  = gui_get_بنیاد_چوڑائی();
    اونچائی = gui_get_بنیاد_اونچائی();
#اگر_تعریف_شدہ FEAT_MENU
    اونچائی += tabline_اونچائی() * gui.char_اونچائی;
#اگر_ختم
    چوڑائی  += get_menu_tool_چوڑائی();
    اونچائی += get_menu_tool_اونچائی();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    اگر (force_چوڑائی && force_اونچائی)
    {
	min_چوڑائی  = force_چوڑائی;
	min_اونچائی = force_اونچائی;
    }
    ورنہ
    {
	min_چوڑائی  = چوڑائی  + MIN_COLUMNS * gui.char_چوڑائی;
	min_اونچائی = اونچائی + MIN_سطریں   * gui.char_اونچائی;
    }

    // Avoid an expose event when the size didn't change.
    اگر (چوڑائی != old_چوڑائی
	    || اونچائی != old_اونچائی
	    || min_چوڑائی != old_min_چوڑائی
	    || min_اونچائی != old_min_اونچائی
	    || gui.char_چوڑائی != old_char_چوڑائی
	    || gui.char_اونچائی != old_char_اونچائی)
    {
	GdkGeometry	geometry;
	Gdk_کھڑکی_Hints	geometry_نقاب;

	geometry.چوڑائی_inc   = gui.char_چوڑائی;
	geometry.اونچائی_inc  = gui.char_اونچائی;
	geometry.بنیاد_چوڑائی  = چوڑائی;
	geometry.بنیاد_اونچائی = اونچائی;
	geometry.min_چوڑائی   = min_چوڑائی;
	geometry.min_اونچائی  = min_اونچائی;
	geometry_نقاب	     = GDK_HINT_بنیاد_سائز|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_سائز;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_کھڑکی_سیٹ_geometry_hints(GTK_کھڑکی(gui.mainwin), تہی,
				      &geometry, geometry_نقاب);
	old_چوڑائی       = چوڑائی;
	old_اونچائی      = اونچائی;
	old_min_چوڑائی   = min_چوڑائی;
	old_min_اونچائی  = min_اونچائی;
	old_char_چوڑائی  = gui.char_چوڑائی;
	old_char_اونچائی = gui.char_اونچائی;
    }
}

#اگر defined(FEAT_GUI_DARKTHEME)
    خالی
gui_mch_سیٹ_dark_theme(صحیح dark)
{
# اگر GTK_CHECK_نسخہ(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_سیٹ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, تہی);
# endif
}
#اگر_ختم // FEAT_GUI_DARKTHEME

#اگر_تعریف_شدہ FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ساکن خالی
icon_سائز_بدلا_foreach(GtkWidget *widget, gpointer user_ڈیٹا)
{
    اگر (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# اگر GTK_CHECK_نسخہ(3,10,0)
	اگر (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ثابت Gtk_Icon_سائز icon_سائز = GPOINTER_مقصد_INT(user_ڈیٹا);
	    ثابت gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, تہی);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_سائز);
	}
# ورنہ
	// User-defined icons are stored in a GtkIconSet
	اگر (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_سیٹ)
	{
	    Gtk_Icon_سیٹ	*icon_سیٹ;
	    Gtk_Icon_سائز	icon_سائز;

	    gtk_image_get_icon_سیٹ(image, &icon_سیٹ, &icon_سائز);
	    icon_سائز = (Gtk_Icon_سائز)(طویل)user_ڈیٹا;

	    gtk_icon_سیٹ_ref(icon_سیٹ);
	    gtk_image_سیٹ_from_icon_سیٹ(image, icon_سیٹ, icon_سائز);
	    gtk_icon_سیٹ_unref(icon_سیٹ);
	}
# endif
    }
    ورنہ اگر (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_سائز_بدلا_foreach,
			      user_ڈیٹا);
    }
}

    ساکن خالی
سیٹ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_سائز	    سائز;
    Gtk_Icon_سائز	    oldsize;

    اگر ((toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_متن | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    ورنہ اگر ((toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS))
		      == (TOOLBAR_متن | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    ورنہ اگر (toolbar_flags & TOOLBAR_متن)
	style = GTK_TOOLBAR_متن;
    ورنہ
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_سیٹ_style(toolbar, style);
# اگر !GTK_CHECK_نسخہ(3,0,0)
    gtk_toolbar_سیٹ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    انتخاب (tbis_flags)
    {
	حالت TBIS_TINY:	    سائز = GTK_ICON_سائز_MENU;		توڑو;
	حالت TBIS_SMALL:    سائز = GTK_ICON_سائز_SMALL_TOOLBAR;	توڑو;
	حالت TBIS_MEDIUM:   سائز = GTK_ICON_سائز_BUTTON;	توڑو;
	حالت TBIS_LARGE:    سائز = GTK_ICON_سائز_LARGE_TOOLBAR;	توڑو;
	حالت TBIS_HUGE:     سائز = GTK_ICON_سائز_DND;		توڑو;
	حالت TBIS_GIANT:    سائز = GTK_ICON_سائز_DIALOG;	توڑو;
	طےشدہ:	    سائز = GTK_ICON_سائز_INVALID;	توڑو;
    }
    oldsize = gtk_toolbar_get_icon_سائز(toolbar);

    اگر (سائز == GTK_ICON_سائز_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_سائز(toolbar);
	سائز = gtk_toolbar_get_icon_سائز(toolbar);
    }
    اگر (سائز != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_سائز_بدلا_foreach,
			      GINT_مقصد_اشارہ_گر((صحیح)سائز));
    }
    gtk_toolbar_سیٹ_icon_سائز(toolbar, سائز);
}

#اگر_ختم // FEAT_TOOLBAR

#اگر defined(FEAT_GUI_TABLINE)
ساکن صحیح ignore_tabline_evt = FALSE;
ساکن GtkWidget *tabline_menu;
# اگر !GTK_CHECK_نسخہ(3,0,0)
ساکن GtkTooltips *tabline_tooltip;
# endif
ساکن صحیح clicked_صفحہ;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساکن خالی
tabline_menu_handler(Gtk_Menu_شے *شے غیر_مستعمل, gpointer user_ڈیٹا)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_صفحہ, (صحیح)(طویل)user_ڈیٹا);
}

    ساکن خالی
جوڑو_tabline_menu_شے(GtkWidget *menu, char_u *متن, صحیح resp)
{
    GtkWidget	*شے;
    char_u	*utf_متن;

    utf_متن = CONVERT_مقصد_UTF8(متن);
    شے = gtk_menu_شے_new_with_label((ثابت حرف *)utf_متن);
    gtk_widget_show(شے);
    CONVERT_مقصد_UTF8_FREE(utf_متن);

    gtk_container_جوڑو(GTK_CONTAINER(menu), شے);
    g_signal_connect(G_OBJECT(شے), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_مقصد_اشارہ_گر(resp));
}

/*
 * Create a menu for the tab line.
 */
    ساکن GtkWidget *
create_tabline_menu(خالی)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    جوڑو_tabline_menu_شے(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    جوڑو_tabline_menu_شے(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    جوڑو_tabline_menu_شے(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    واپس menu;
}

    ساکن gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    اگر (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	صحیح		x = bevent->x;
	صحیح		y = bevent->y;
	GtkWidget	*tabwidget;
	Gdk_کھڑکی	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	اگر (hold_gui_events || cmdwin_type != 0)
	    واپس TRUE;

	tabwin = gui_gtk_کھڑکی_at_position(gui.mainwin, &x, &y);

	gdk_کھڑکی_get_user_ڈیٹا(tabwin, (gpointer)&tabwidget);
	clicked_صفحہ = GPOINTER_مقصد_INT(g_object_get_ڈیٹا(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	اگر (bevent->button == 3)
	{
# اگر GTK_CHECK_نسخہ(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    اگر (gui.is_wayland)
	    {
		صحیح x2, y2;
		gui_gtk_get_اشارہ_گر(gui.mainwin, &x2, &y2, تہی);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_کھڑکی(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, تہی);
	    }
	    ورنہ
#  endif
	    gtk_menu_popup_at_اشارہ_گر(GTK_MENU(widget), event);
# ورنہ
	    gtk_menu_popup(GTK_MENU(widget), تہی, تہی, تہی, تہی,
						bevent->button, bevent->وقت);
# endif
	    // We handled the event.
	    واپس TRUE;
	}
	ورنہ اگر (bevent->button == 1)
	{
	    اگر (clicked_صفحہ == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	ورنہ اگر (bevent->button == 2)
	{
	    اگر (clicked_صفحہ != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_صفحہ, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    واپس FALSE;
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

# اگر GTK_CHECK_نسخہ(2,10,0)
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
# endif

/*
 * Show or hide the tabline.
 */
    خالی
gui_mch_show_tabline(صحیح showit)
{
    اگر (gui.tabline == تہی)
	واپس;

    اگر (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    صحیح w, h;

	    gtk_کھڑکی_get_سائز(GTK_کھڑکی(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_کھڑکی_manager_hints(0, 0);
	اگر (showit)
	    gtk_widget_سیٹ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(خالی)
{
    واپس gui.tabline != تہی
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    خالی
gui_mch_update_tabline(خالی)
{
    GtkWidget	    *صفحہ;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    صحیح		    nr = 0;
    صحیح		    tab_عدد;
    صحیح		    curtabidx = 0;
    char_u	    *labeltext;

    اگر (gui.tabline == تہی)
	واپس;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    برائے (tp = پہلا_tabpage; tp != تہی; tp = tp->tp_اگلا, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	tab_عدد = nr + 1;

	صفحہ = gtk_notebook_get_nth_صفحہ(GTK_NOTEBOOK(gui.tabline), nr);
	اگر (صفحہ == تہی)
	{
	    // Add notebook page
# اگر GTK_CHECK_نسخہ(3,2,0)
	    صفحہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_سیٹ_homogeneous(GTK_BOX(صفحہ), FALSE);
# ورنہ
	    صفحہ = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(صفحہ);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# اگر !GTK_CHECK_نسخہ(3,14,0)
	    gtk_misc_سیٹ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_جوڑو(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_داخل_کرو_صفحہ(GTK_NOTEBOOK(gui.tabline),
		    صفحہ,
		    event_box,
		    nr);
# اگر GTK_CHECK_نسخہ(2,10,0)
	    gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    صفحہ,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), صفحہ);
	g_object_سیٹ_ڈیٹا(G_OBJECT(event_box), "tab_num",
						     GINT_مقصد_اشارہ_گر(tab_عدد));
	label = gtk_bin_get_بچہ(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
	gtk_label_سیٹ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_مقصد_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_مقصد_UTF8(NameBuff);
# اگر GTK_CHECK_نسخہ(3,0,0)
	gtk_widget_سیٹ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
# ورنہ
	gtk_tooltips_سیٹ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ثابت حرف *)labeltext, تہی);
# endif
	CONVERT_مقصد_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    جب (gtk_notebook_get_nth_صفحہ(GTK_NOTEBOOK(gui.tabline), nr) != تہی)
	gtk_notebook_remove_صفحہ(GTK_NOTEBOOK(gui.tabline), nr);

    اگر (gtk_notebook_get_current_صفحہ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سیٹ_current_صفحہ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    خالی
gui_mch_سیٹ_curtab(صحیح nr)
{
    اگر (gui.tabline == تہی)
	واپس;

    ignore_tabline_evt = TRUE;
    اگر (gtk_notebook_get_current_صفحہ(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_سیٹ_current_صفحہ(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#اگر_ختم // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    خالی
gui_gtk_سیٹ_selection_targets(GdkAtom selection)
{
    صحیح		    i, j = 0;
    ساکن صحیح	    n_targets = N_SELECTION_TARGETS;
    ساکن Gtk_ہدف_اندراج  targets[N_SELECTION_TARGETS];

    اگر (targets[0].ہدف == تہی)
    {
	برائے (i = 0; i < (صحیح)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    اگر (!clip_html && selection_targets[i].info == ہدف_HTML)
		n_targets--;
	    ورنہ
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_جوڑو_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    خالی
gui_gtk_سیٹ_dnd_targets(خالی)
{
    صحیح		    i, j = 0;
    صحیح		    n_targets = N_DND_TARGETS;
    Gtk_ہدف_اندراج  targets[N_DND_TARGETS];

    برائے (i = 0; i < (صحیح)N_DND_TARGETS; ++i)
    {
	اگر (!clip_html && dnd_targets[i].info == ہدف_HTML)
	    n_targets--;
	ورنہ
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_سیٹ(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحیح
gui_mch_init(خالی)
{
    GtkWidget *vbox;

#اگر_تعریف_شدہ FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    اگر (using_gnome)
    {
	gnome_پروگرام_init(VIMPACKAGE, VIM_نسخہ_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, تہی);
# اگر defined(LC_NUMERIC)
	{
	    حرف *p = setlocale(LC_NUMERIC, تہی);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    اگر (p == تہی || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#اگر_ختم
    VIM_CLEAR(gui_argv);

#اگر GLIB_CHECK_نسخہ(2,1,3)
    // Set the human-readable application name
    g_سیٹ_application_name("Vim");
#اگر_ختم
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    سیٹ_option_قدر_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#اگر_تعریف_شدہ FEAT_TOOLBAR
    gui_gtk_رجسٹر_stock_icons();
#اگر_ختم
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#اگر !GTK_CHECK_نسخہ(3,0,0)
# اگر 0
    gtk_rc_parse("gtkrc");
# endif
#اگر_ختم

    // Initialize values
    gui.border_چوڑائی = 2;
    gui.scrollbar_چوڑائی = SB_DEFAULT_چوڑائی;
    gui.scrollbar_اونچائی = SB_DEFAULT_چوڑائی;
#اگر GTK_CHECK_نسخہ(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#ورنہ
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#اگر_ختم

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.عقب_pixel = gui.def_عقب_pixel;

    اگر (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	اگر (plug != تہی && gtk_plug_get_socket_کھڑکی(GTK_PLUG(plug)) != تہی)
	{
	    gui.mainwin = plug;
	}
	ورنہ
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (غیرعلامتی صحیح)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    اگر (gtk_socket_id == 0)
    {
#اگر_تعریف_شدہ FEAT_GUI_GNOME
	اگر (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", تہی);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	ورنہ
#اگر_ختم
	    gui.mainwin = gtk_کھڑکی_new(GTK_کھڑکی_TOPLEVEL);
    }

    gtk_widget_سیٹ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_بنیاد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_جوڑو_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_نقاب);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), تہی);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), تہی);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_بدلا_cb), تہی);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), تہی);

    gui.accel_گروہ = gtk_accel_گروہ_new();
    gtk_کھڑکی_جوڑو_accel_گروہ(GTK_کھڑکی(gui.mainwin), gui.accel_گروہ);

    // A vertical box holds the menubar, toolbar and main text window.
#اگر GTK_CHECK_نسخہ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(vbox), FALSE);
#ورنہ
    vbox = gtk_vbox_new(FALSE, 0);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI_GNOME
    اگر (using_gnome)
    {
# اگر defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_سیٹ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    ورنہ
#اگر_ختم
    {
	gtk_container_جوڑو(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#اگر_تعریف_شدہ FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_سیٹ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_سیٹ(gtk_settings, "gtk-menu-bar-accel", تہی, تہی);
    }


# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
    {
	Bonobo_Dock_شے *dockitem;

	gnome_app_سیٹ_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_شے_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_شے_سیٹ_behavior(dockitem,
		bonobo_dock_شے_get_behavior(dockitem)
				       | BONOBO_DOCK_شے_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    ورنہ
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_آغاز(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#اگر_ختم	// FEAT_MENU

#اگر_تعریف_شدہ FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# ورنہ
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_سیٹ_name(gui.toolbar, "vim-toolbar");
    سیٹ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
    {
	Bonobo_Dock_شے *dockitem;

	gnome_app_سیٹ_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_شے_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_شے_سیٹ_behavior(dockitem,
		bonobo_dock_شے_get_behavior(dockitem)
				       | BONOBO_DOCK_شے_BEH_NEVER_FLOATING);
	gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(gui.toolbar), 0);
    }
    ورنہ
# endif	// FEAT_GUI_GNOME
    {
	اگر (vim_strchr(p_go, GO_TOOLBAR) != تہی
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_آغاز(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#اگر_ختم // FEAT_TOOLBAR

#اگر_تعریف_شدہ FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_آغاز(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_سیٹ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# اگر !GTK_CHECK_نسخہ(3,0,0)
    gtk_notebook_سیٹ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# اگر !GTK_CHECK_نسخہ(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *صفحہ, *label, *event_box;

	// Add the first tab.
# اگر GTK_CHECK_نسخہ(3,2,0)
	صفحہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_سیٹ_homogeneous(GTK_BOX(صفحہ), FALSE);
# ورنہ
	صفحہ = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(صفحہ);
	gtk_container_جوڑو(GTK_CONTAINER(gui.tabline), صفحہ);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_سیٹ_ڈیٹا(G_OBJECT(event_box), "tab_num", GINT_مقصد_اشارہ_گر(1L));
# اگر !GTK_CHECK_نسخہ(3,14,0)
	gtk_misc_سیٹ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_جوڑو(GTK_CONTAINER(event_box), label);
	gtk_notebook_سیٹ_tab_label(GTK_NOTEBOOK(gui.tabline), صفحہ, event_box);
# اگر GTK_CHECK_نسخہ(2,10,0)
	gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), صفحہ, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), تہی);
# اگر GTK_CHECK_نسخہ(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), تہی);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#اگر_ختم // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(gui.formwin), 0);
#اگر !GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_events(gui.formwin, GDK_EXPOSURE_نقاب);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,22,2)
    gtk_widget_سیٹ_name(gui.formwin, "vim-gtk-form");
#اگر_ختم

    gui.drawarea = gtk_drawing_area_new();
#اگر GTK_CHECK_نسخہ(3,0,0)
    gui.surface = تہی;
#اگر_ختم

#اگر_تعریف_شدہ GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    اگر (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#اگر_ختم

    // Determine which events we will filter.
    gint event_نقاب =
	GDK_EXPOSURE_نقاب |
	GDK_ENTER_NOTIFY_نقاب |
	GDK_LEAVE_NOTIFY_نقاب |
	GDK_BUTTON_PRESS_نقاب |
	GDK_BUTTON_RELEASE_نقاب |
	GDK_کلید_PRESS_نقاب |
	GDK_کلید_RELEASE_نقاب |
	GDK_اشارہ_گر_MOTION_نقاب |
	GDK_اشارہ_گر_MOTION_HINT_نقاب;
#اگر GTK_CHECK_نسخہ(3,4,0)
    اگر (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_نقاب |= GDK_SCROLL_نقاب;
    }
    ورنہ
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_نقاب |= GDK_SMOOTH_SCROLL_نقاب;
    }
#ورنہ
    event_نقاب |= GDK_SCROLL_نقاب;
#اگر_ختم
    gtk_widget_سیٹ_events(gui.drawarea, event_نقاب);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_آغاز(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(کلید_press_event), تہی);
#اگر defined(FEAT_XIM) || GTK_CHECK_نسخہ(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&کلید_release_event), تہی);
#اگر_ختم
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), تہی);
#اگر GTK_CHECK_نسخہ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), تہی);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), تہی);
#ورنہ
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_سیٹ_cb), تہی);
#اگر_ختم

#اگر !GTK_CHECK_نسخہ(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#اگر_ختم

#اگر !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#اگر_ختم

    اگر (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_سیٹ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_ستارہ.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_انحراف = gui.border_چوڑائی;

#اگر GTK_CHECK_نسخہ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), تہی);
#ورنہ
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), تہی);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), تہی);
#اگر_ختم

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    اگر (vim_strchr(p_go, GO_اشارہ_گر) != تہی)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), تہی);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), تہی);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    اگر (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_برآمد_event), تہی);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_اندر_event), تہی);
    }
    ورنہ
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_برآمد_event), تہی);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_اندر_event), تہی);
#اگر_تعریف_شدہ FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_برآمد_event), تہی);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_اندر_event), تہی);
#اگر_ختم // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), تہی);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.اندر_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_بدلا_cb), تہی);
    }

#اگر defined(FEAT_IMAGE) && GTK_CHECK_نسخہ(3,10,0)
    gui.scale = gtk_widget_get_scale_ضربی_عدد(gui.formwin);
#اگر_ختم

    واپس OK;
}

#اگر defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    خالی
gui_mch_forked(خالی)
{
    اگر (!using_gnome)
	واپس;

    GnomeClient *client;

    client = gnome_master_client();

    اگر (client != تہی)
	gnome_client_سیٹ_process_id(client, getpid());
}
#اگر_ختم // USE_GNOME_SESSION

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن GdkRGBA
color_مقصد_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    واپس rgba;
}

    ساکن خالی
سیٹ_cairo_ماخذ_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ثابت GdkRGBA rgba = color_مقصد_rgba(color);
    cairo_سیٹ_ماخذ_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#اگر_ختم // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    خالی
gui_mch_new_colors(خالی)
{
    اگر (gui.drawarea != تہی
#اگر GTK_CHECK_نسخہ(3,22,2)
	    && gui.formwin != تہی
#اگر_ختم
	    && gtk_widget_get_کھڑکی(gui.drawarea) != تہی)
    {
#اگر !GTK_CHECK_نسخہ(3,22,2)
	Gdk_کھڑکی * ثابت da_win = gtk_widget_get_کھڑکی(gui.drawarea);
#اگر_ختم
#اگر GTK_CHECK_نسخہ(3,22,2)
	GtkStyleContext * ثابت context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar * ثابت css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.عقب_pixel >> 16) & 0xff,
		 (gui.عقب_pixel >> 8) & 0xff,
		 gui.عقب_pixel & 0xff);

	gtk_css_provider_بھرو_from_ڈیٹا(provider, css, -1, تہی);
	gtk_style_context_جوڑو_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#وگرنہ_اگر GTK_CHECK_نسخہ(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_مقصد_rgba(gui.عقب_pixel);
	{
	    cairo_نمونہ_t * ثابت pat = cairo_نمونہ_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    اگر (pat != تہی)
	    {
		gdk_کھڑکی_سیٹ_background_نمونہ(da_win, pat);
		cairo_نمونہ_destroy(pat);
	    }
	    ورنہ
		gdk_کھڑکی_سیٹ_background_rgba(da_win, &rgba);
	}
#ورنہ // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.عقب_pixel;
	gdk_کھڑکی_سیٹ_background(da_win, &color);
#اگر_ختم // !GTK_CHECK_VERSION(3,22,2)
    }
}

/*
 * One-shot idle callback to issue a corrective gtk_window_resize() after the
 * startup configure event has been processed.  At startup
 * gui_mch_set_shellsize() runs before the CSD offsets are known (they are
 * measured from the first configure response), so the window ends up
 * mch_csd_height pixels too short and mch_csd_width pixels too narrow.  This
 * callback re-issues the shellsize with the now-known offsets so the physical
 * window matches Vim's model.
 */
    ساکن gboolean
startup_resize_correction_cb(gpointer ڈیٹا غیر_مستعمل)
{
    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    واپس FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ساکن gint
form_configure_event(GtkWidget *widget غیر_مستعمل,
		     GdkEventConfigure *event,
		     gpointer ڈیٹا غیر_مستعمل)
{
    صحیح	    usable_اونچائی = event->اونچائی;
#اگر_تعریف_شدہ TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    صحیح	    w, h;
    gtk_کھڑکی_get_سائز(GTK_کھڑکی(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(تہی, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (صحیح)Columns, (صحیح)صفیں);
# endif

    // Look through history of recent vim resize requests.
    // If this event matches:
    //	    - "latest resize hist" We're caught up;
    //		clear the history and process this event.
    //		If history is, old to new, 100, 99, 100, 99. If this event is
    //		99 for the stale, it is matched against the current. History
    //		is cleared, we may bounce, but no worse than before.
    //	    - "older/stale hist" If match an unused event in history,
    //		then discard this event, and mark the matching event as used.
    //	    - "no match" Figure it's a user resize event, clear history.
    // NOTE: clear history is default, then all incoming events are processed

    اگر (!مطابقت_چوڑائی_اونچائی(latest_resize_hist, w, h)
					     && مطابقت_stale_چوڑائی_اونچائی(w, h))
	// discard stale event
	واپس TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    اگر ((mch_csd_اونچائی == 0 || mch_csd_چوڑائی == 0)
	    && latest_resize_hist != تہی
	    && !latest_resize_hist->مستعمل
	    && latest_resize_hist->from_shellsize
	    && gui.char_اونچائی > 0)
    {
	صحیح pot_csd_w = latest_resize_hist->چوڑائی - w;
	صحیح pot_csd_h = latest_resize_hist->اونچائی - h;

	اگر (pot_csd_w > 0 && pot_csd_w < gui.char_چوڑائی)
	{
	    mch_csd_چوڑائی = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (صحیح)Columns * gui.char_چوڑائی
						    + gui_get_بنیاد_چوڑائی();
	}
	اگر (pot_csd_h > 0 && pot_csd_h < gui.char_اونچائی)
	{
	    mch_csd_اونچائی = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_اونچائی += mch_csd_اونچائی;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	اگر ((mch_csd_اونچائی > 0 || mch_csd_چوڑائی > 0) && gtk_socket_id == 0)
	    g_idle_جوڑو_بھرا(GTK_ترجیح_RESIZE, startup_resize_correction_cb, تہی, تہی);
    }
    clear_resize_hists();
#اگر_ختم

#اگر GTK_CHECK_نسخہ(3,22,2) && !GTK_CHECK_نسخہ(3,22,4)
    // As of 3.22.2, GdkWindows have started distributing configure events to
    // their "native" children (https://git.gnome.org/browse/gtk+/commit/?h=gtk-3-22&id=12579fe71b3b8f79eb9c1b80e429443bcc437dd0).
    //
    // As can be seen from the implementation of move_native_children() and
    // configure_native_child() in gdkwindow.c, those functions actually
    // propagate configure events to every child, failing to distinguish
    // "native" one from non-native one.
    //
    // Naturally, configure events propagated to here like that are fallacious
    // and, as a matter of fact, they trigger a geometric collapse of
    // gui.formwin.
    //
    // To filter out such fallacious events, check if the given event is the
    // one that was sent out to the right place. Ignore it if not.
    //
    // Follow-up
    // After a few weeks later, the GdkWindow change mentioned above was
    // reverted (https://git.gnome.org/browse/gtk+/commit/?h=gtk-3-22&id=f70039cb9603a02d2369fec4038abf40a1711155).
    // The corresponding official release is 3.22.4.
    اگر (event->کھڑکی_2 != gtk_widget_get_کھڑکی(gui.formwin))
	واپس TRUE;
#اگر_ختم

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    اگر (gtk_socket_id != 0)
	usable_اونچائی -= (gui.char_اونچائی - (gui.char_اونچائی/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	صحیح use_چوڑائی = event->چوڑائی;

#اگر_تعریف_شدہ TRACK_RESIZE_HISTORY
	اگر (mch_pending_form_w > event->چوڑائی
		&& mch_pending_form_w - event->چوڑائی < gui.char_چوڑائی)
	    use_چوڑائی = mch_pending_form_w;
#اگر_ختم
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_چوڑائی, usable_اونچائی);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    واپس TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    ساکن خالی
mainwin_destroy_cb(GObject *object غیر_مستعمل, gpointer ڈیٹا غیر_مستعمل)
{
    // Don't write messages to the GUI anymore
    بھرا_screen = FALSE;

    gui.mainwin  = تہی;
    gui.drawarea = تہی;

    اگر (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#اگر_تعریف_شدہ USE_GRESOURCE
    gui_gtk_unregister_resource();
#اگر_ختم
}

    خالی
gui_gtk_get_screen_geom_کا_win(
	GtkWidget *wid,
	صحیح نقطہ_x,	    // x position of window if not initialized
	صحیح نقطہ_y,	    // y position of window if not initialized
	صحیح *screen_x,
	صحیح *screen_y,
	صحیح *چوڑائی,
	صحیح *اونچائی)
{
    GdkRectangle geometry;
    Gdk_کھڑکی *win = gtk_widget_get_کھڑکی(wid);
#اگر GTK_CHECK_نسخہ(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    اگر (wid != تہی && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    ورنہ
	dpy = gdk_display_get_default();
    اگر (win != تہی)
	monitor = gdk_display_get_monitor_at_کھڑکی(dpy, win);
    ورنہ
	monitor = gdk_display_get_monitor_at_نقطہ(dpy, نقطہ_x, نقطہ_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#ورنہ
    GdkScreen* screen;
    صحیح monitor;

    اگر (wid != تہی && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    ورنہ
	screen = gdk_screen_get_default();
    اگر (win != تہی)
	monitor = gdk_screen_get_monitor_at_کھڑکی(screen, win);
    ورنہ
	monitor = gdk_screen_get_monitor_at_نقطہ(screen, نقطہ_x, نقطہ_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#اگر_ختم
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *چوڑائی = geometry.چوڑائی;
    *اونچائی = geometry.اونچائی;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ساکن خالی
gui_gtk_get_screen_dimensions(
	صحیح نقطہ_x,
	صحیح نقطہ_y,
	صحیح *screen_w,
	صحیح *screen_h)
{
    صحیح	    x, y;

    gui_gtk_get_screen_geom_کا_win(gui.mainwin, نقطہ_x, نقطہ_y,
						   &x, &y, screen_w, screen_h);

    // Subtract 'guiheadroom' from the height to allow some room for the
    // window manager (task list and window title bar).
    *screen_h -= p_ghr;

    /*
     * FIXME: dirty trick: Because the gui_get_base_height() doesn't include
     * the toolbar and menubar for GTK, we subtract them from the screen
     * height, so that the window size can be made to fit on the screen.
     * This should be completely changed later.
     */
    *screen_w -= get_menu_tool_چوڑائی();
    *screen_h -= get_menu_tool_اونچائی();
}

    خالی
gui_mch_get_screen_dimensions(صحیح *screen_w, صحیح *screen_h)
{
    gui_gtk_get_screen_dimensions(0, 0, screen_w, screen_h);
}


/*
 * Bit of a hack to ensure we start GtkPlug windows with the correct window
 * hints (and thus the required size from -geom), but that after that we
 * put the hints back to normal (the actual minimum size) so we may
 * subsequently be resized smaller.  GtkSocket (the parent end) uses the
 * plug's window 'min hints to set *its* minimum size, but that's also the
 * only way we have of making ourselves bigger (by set lines/columns).
 * Thus set hints at start-up to ensure correct init. size, then a
 * second after the final attempt to reset the real minimum hints (done by
 * scrollbar init.), actually do the standard hints and stop the timer.
 * We'll not let the default hints be set while this timer's active.
 */
    ساکن timeout_cb_type
check_startup_plug_hints(gpointer ڈیٹا غیر_مستعمل)
{
    اگر (init_کھڑکی_hints_state == 1)
    {
	// Safe to use normal hints now
	init_کھڑکی_hints_state = 0;
	update_کھڑکی_manager_hints(0, 0);
	واپس FALSE;   // stop timer
    }

    // Keep on trying
    init_کھڑکی_hints_state = 1;
    واپس TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(خالی)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_چوڑائی;
    guint		pixel_اونچائی;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    اگر (role_دلیل != تہی)
    {
	gtk_کھڑکی_سیٹ_role(GTK_کھڑکی(gui.mainwin), role_دلیل);
    }
    ورنہ
    {
	حرف *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (غیرعلامتی)mch_get_pid(),
			       (غیرعلامتی)g_random_int(),
			       (غیرعلامتی)وقت(تہی));

	gtk_کھڑکی_سیٹ_role(GTK_کھڑکی(gui.mainwin), role);
	g_free(role);
    }

    اگر (gui_win_x != -1 && gui_win_y != -1)
	gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    اگر (gui.geom != تہی)
    {
	صحیح		نقاب;
	غیرعلامتی صحیح	w, h;
	صحیح		x = 0;
	صحیح		y = 0;

	نقاب = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);

	اگر (نقاب & چوڑائی_قدر)
	    Columns = w;
	اگر (نقاب & اونچائی_قدر)
	{
	    اگر (p_کھڑکی > (طویل)h - 1 || !option_was_سیٹ((char_u *)"window"))
		p_کھڑکی = h - 1;
	    صفیں = h;
	}
	حد_بندی_screen_سائز();

	pixel_چوڑائی = (guint)(gui_get_بنیاد_چوڑائی() + Columns * gui.char_چوڑائی);
	pixel_اونچائی = (guint)(gui_get_بنیاد_اونچائی() + صفیں * gui.char_اونچائی);

	pixel_چوڑائی  += get_menu_tool_چوڑائی();
	pixel_اونچائی += get_menu_tool_اونچائی();

	اگر (نقاب & (XValue | YValue))
	{
	    صحیح ww, hh;

#اگر_تعریف_شدہ FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#ورنہ
	    gui_mch_get_screen_dimensions(&ww, &hh);
#اگر_ختم
	    hh += p_ghr + get_menu_tool_اونچائی();
	    ww += get_menu_tool_چوڑائی();
	    اگر (نقاب & XNegative)
		x += ww - pixel_چوڑائی;
	    اگر (نقاب & YNegative)
		y += hh - pixel_اونچائی;
	    gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	اگر (gtk_socket_id != 0  &&  (نقاب & چوڑائی_قدر || نقاب & اونچائی_قدر))
	{
	    update_کھڑکی_manager_hints(pixel_چوڑائی, pixel_اونچائی);
	    init_کھڑکی_hints_state = 1;
	    timeout_جوڑو(1000, check_startup_plug_hints, تہی);
	}
    }

    pixel_چوڑائی = (guint)(gui_get_بنیاد_چوڑائی() + Columns * gui.char_چوڑائی);
    pixel_اونچائی = (guint)(gui_get_بنیاد_اونچائی() + صفیں * gui.char_اونچائی);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    اگر (gtk_socket_id == 0)
	gtk_کھڑکی_resize(GTK_کھڑکی(gui.mainwin), pixel_چوڑائی, pixel_اونچائی);
    update_کھڑکی_manager_hints(0, 0);

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

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    سیٹ_معمول_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), تہی);

    /*
     * Notify the fixed area about the need to resize the contents of the
     * gui.formwin, which we use for random positioning of the included
     * components.
     *
     * We connect this signal deferred finally after anything is in place,
     * since this is intended to handle resizements coming from the window
     * manager upon us and should not interfere with what VIM is requesting
     * upon startup.
     */
#اگر_تعریف_شدہ TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#اگر_ختم
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), تہی);
#اگر GTK_CHECK_نسخہ(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ضربی_عدد_event), تہی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_سیٹ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_ڈیٹا_received_cb), تہی);
#اگر_ختم

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	اگر (ملا_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	غیرعلامتی طویل menu_handler = 0;
# ifdef FEAT_TOOLBAR
	غیرعلامتی طویل tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	اگر (using_gnome && vim_strchr(p_go, GO_MENUS) == تہی)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  تہی);
# ifdef FEAT_TOOLBAR
	اگر (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == تہی
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  تہی);
# endif
#اگر_ختم
	gtk_widget_show(gui.mainwin);

#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	اگر (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	اگر (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#اگر_ختم
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), تہی);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), تہی);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), تہی);

    واپس OK;
}

/*
 * Clean up for when exiting Vim.
 */
    خالی
gui_mch_exit(صحیح rc غیر_مستعمل)
{
    // Clean up, unless we don't want to invoke free().
    اگر (gui.mainwin != تہی && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    صحیح
gui_mch_get_winpos(صحیح *x, صحیح *y)
{
    اگر (gui_mch_get_display())
    {
	gtk_کھڑکی_get_position(GTK_کھڑکی(gui.mainwin), x, y);
	واپس OK;
    }
    واپس FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    خالی
gui_mch_سیٹ_winpos(صحیح x, صحیح y)
{
    gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), x, y);
}

#اگر !GTK_CHECK_نسخہ(3,0,0)
# اگر 0
ساکن صحیح resize_idle_installed = FALSE;
/*
 * Idle handler to force resize.  Used by gui_mch_set_shellsize() to ensure
 * the shell size doesn't exceed the window size, i.e. if the window manager
 * ignored our size request.  Usually this happens if the window is maximized.
 *
 * FIXME: It'd be nice if we could find a little more orthodox solution.
 * See also the remark below in gui_mch_set_shellsize().
 *
 * DISABLED: When doing ":set lines+=1" this function would first invoke
 * gui_resize_shell() with the old size, then the normal callback would
 * report the new size through form_configure_event().  That caused the window
 * layout to be messed up.
 */
    ساکن gboolean
force_shell_resize_idle(gpointer ڈیٹا)
{
    اگر (gui.mainwin != تہی
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	صحیح چوڑائی;
	صحیح اونچائی;

	gtk_کھڑکی_get_سائز(GTK_کھڑکی(gui.mainwin), &چوڑائی, &اونچائی);

	چوڑائی  -= get_menu_tool_چوڑائی();
	اونچائی -= get_menu_tool_اونچائی();

	gui_resize_shell(چوڑائی, اونچائی);
    }

    resize_idle_installed = FALSE;
    واپس FALSE; // don't call me again
}
# endif
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    صحیح
gui_mch_maximized(خالی)
{
    واپس (gui.mainwin != تہی && gtk_widget_get_کھڑکی(gui.mainwin) != تہی
	    && (gdk_کھڑکی_get_state(gtk_widget_get_کھڑکی(gui.mainwin))
					       & GDK_کھڑکی_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    خالی
gui_mch_unmaximize(خالی)
{
    اگر (gui.mainwin != تہی)
	gtk_کھڑکی_unmaximize(GTK_کھڑکی(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    خالی
gui_mch_newfont(خالی)
{
    صحیح w, h;

    gtk_کھڑکی_get_سائز(GTK_کھڑکی(gui.mainwin), &w, &h);
    w -= get_menu_tool_چوڑائی();
    h -= get_menu_tool_اونچائی();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    خالی
gui_mch_سیٹ_shellsize(صحیح چوڑائی, صحیح اونچائی,
		      صحیح min_چوڑائی غیر_مستعمل,  صحیح min_اونچائی غیر_مستعمل,
		      صحیح بنیاد_چوڑائی غیر_مستعمل, صحیح بنیاد_اونچائی غیر_مستعمل,
		      صحیح direction غیر_مستعمل)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    اگر (gtk_socket_id == 0)
	update_کھڑکی_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    چوڑائی  += get_menu_tool_چوڑائی();
    اونچائی += get_menu_tool_اونچائی();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    چوڑائی  += mch_csd_چوڑائی;
    اونچائی += mch_csd_اونچائی;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_چوڑائی > 0)
			    ? چوڑائی - get_menu_tool_چوڑائی() - mch_csd_چوڑائی : 0;

#اگر_تعریف_شدہ TRACK_RESIZE_HISTORY
    alloc_resize_hist(چوڑائی, اونچائی, TRUE);
#اگر_ختم
    اگر (gtk_socket_id == 0)
	gtk_کھڑکی_resize(GTK_کھڑکی(gui.mainwin), چوڑائی, اونچائی);
    ورنہ
	update_کھڑکی_manager_hints(چوڑائی, اونچائی);

#اگر !GTK_CHECK_نسخہ(3,0,0)
# اگر 0
    اگر (!resize_idle_installed)
    {
	g_idle_جوڑو_بھرا(GDK_ترجیح_EVENTS + 10,
			&force_shell_resize_idle, تہی, تہی);
	resize_idle_installed = TRUE;
    }
# endif
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    خالی
gui_mch_settitle(char_u *title, char_u *icon غیر_مستعمل)
{
    اگر (title != تہی && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, تہی);

    gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(gui.mainwin), (ثابت حرف *)title);

    اگر (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#اگر defined(FEAT_MENU)
    خالی
gui_mch_enable_menu(صحیح showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
	widget = gui.menubar_h;
    ورنہ
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    اگر (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	اگر (showit)
	    gtk_widget_show(widget);
	ورنہ
	    gtk_widget_hide(widget);

	update_کھڑکی_manager_hints(0, 0);
    }
}
#اگر_ختم // FEAT_MENU

#اگر defined(FEAT_TOOLBAR)
    خالی
gui_mch_show_toolbar(صحیح showit)
{
    GtkWidget *widget;

    اگر (gui.toolbar == تہی)
	واپس;

# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
	widget = gui.toolbar_h;
    ورنہ
# endif
	widget = gui.toolbar;

    اگر (showit)
	سیٹ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    اگر (!showit != !gtk_widget_get_visible(widget))
    {
	اگر (showit)
	    gtk_widget_show(widget);
	ورنہ
	    gtk_widget_hide(widget);

	update_کھڑکی_manager_hints(0, 0);
    }
}
#اگر_ختم // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    ساکن صحیح
is_cjk_font(PangoFontDescription *font_desc)
{
    ساکن ثابت حرف * ثابت cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    غیرعلامتی	i;
    صحیح		is_cjk = FALSE;

    font = pango_context_بھرو_font(gui.متن_context, font_desc);

    اگر (font == تہی)
	واپس FALSE;

    برائے (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	اگر (coverage != تہی)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#اگر PANGO_نسخہ_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#ورنہ
	    pango_coverage_unref(coverage);
#اگر_ختم
	}
    }

    g_object_unref(font);

    واپس is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(خالی)
{
    PangoFontMetrics	*metrics;
    صحیح			ascent;
    صحیح			descent;

    metrics = pango_context_get_metrics(gui.متن_context, gui.norm_font,
				pango_context_get_language(gui.متن_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_اونچائی = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_اونچائی = MAX(gui.char_اونچائی, gui.char_ascent + 1);

    واپس OK;
}

#اگر GTK_CHECK_نسخہ(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ساکن gboolean
font_filter(ثابت PangoFontFamily *family,
	    ثابت PangoFontFace   *face غیر_مستعمل,
	    gpointer		   ڈیٹا غیر_مستعمل)
{
    واپس pango_font_family_is_monospace((PangoFontFamily *)family);
}
#اگر_ختم

/*
 * Put up a font dialog and return the selected font name in allocated memory.
 * "oldval" is the previous value.  Return NULL when cancelled.
 * This should probably go into gui_gtk.c.  Hmm.
 * FIXME:
 * The GTK2 font selection dialog has no filtering API.  So we could either
 * a) implement our own (possibly copying the code from somewhere else) or
 * b) just live with it.
 */
    char_u *
gui_mch_font_dialog(char_u *oldval)
{
    GtkWidget	*dialog;
    صحیح		response;
    char_u	*fontname = تہی;
    char_u	*oldname;

#اگر GTK_CHECK_نسخہ(3,2,0)
    dialog = gtk_font_chooser_dialog_new(تہی, تہی);
    gtk_font_chooser_سیٹ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    تہی, تہی);
#ورنہ
    dialog = gtk_font_selection_dialog_new(تہی);
#اگر_ختم

    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(dialog), GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_destroy_with_والد(GTK_کھڑکی(dialog), TRUE);

    اگر (oldval != تہی && oldval[0] != NUL)
    {
	اگر (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, تہی);
	ورنہ
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	اگر (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    اگر (p != تہی)
	    {
		STRCPY(p + STRLEN(p), " 10");
		اگر (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#اگر GTK_CHECK_نسخہ(3,2,0)
	gtk_font_chooser_سیٹ_font(
		GTK_FONT_CHOOSER(dialog), (ثابت gchar *)oldname);
#ورنہ
	gtk_font_selection_dialog_سیٹ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ثابت حرف *)oldname);
#اگر_ختم

	اگر (oldname != oldval)
	    vim_free(oldname);
    }
    ورنہ
#اگر GTK_CHECK_نسخہ(3,2,0)
	gtk_font_chooser_سیٹ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#ورنہ
	gtk_font_selection_dialog_سیٹ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#اگر_ختم

    response = gtk_dialog_تسلسل(GTK_DIALOG(dialog));

    اگر (response == GTK_RESPONSE_OK)
    {
	حرف *name;

#اگر GTK_CHECK_نسخہ(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#ورنہ
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#اگر_ختم
	اگر (name != تہی)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
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
    }

    اگر (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    واپس fontname;
}

/*
 * Some monospace fonts don't support a bold weight, and fall back
 * silently to the regular weight.  But this is no good since our text
 * drawing function can emulate bold by overstriking.  So let's try
 * to detect whether bold weight is actually available and emulate it
 * otherwise.
 *
 * Note that we don't need to check for italic style since Xft can
 * emulate italic on its own, provided you have a proper fontconfig
 * setup.  We wouldn't be able to emulate it in Vim anyway.
 */
    ساکن خالی
get_styled_font_variants(خالی)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_بھرو_font(gui.متن_context, gui.norm_font);

    اگر (plain_font == تہی)
	واپس;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سیٹ_وزن(bold_font_desc, PANGO_وزن_BOLD);

    bold_font = pango_context_بھرو_font(gui.متن_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    اگر (bold_font != تہی)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    اگر (bold_font != تہی && gui.font_can_bold)
	g_object_unref(plain_font);
}

ساکن PangoEngineShape *default_shape_engine = تہی;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
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

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
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

    اگر (شے_list != تہی && شے_list->اگلا == تہی) // play safe
    {
	Pango_شے   *شے;
	صحیح	    چوڑائی;

	شے  = (Pango_شے *)شے_list->ڈیٹا;
	چوڑائی = gui.char_چوڑائی * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = شے->analysis.shape_engine;

	gui.ascii_font = شے->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &شے->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->عدد_glyphs == حجم(ascii_chars));

	برائے (i = 0; i < gui.ascii_glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_انحراف += MAX(0, چوڑائی - geom->چوڑائی) / 2;
	    geom->چوڑائی = چوڑائی;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(شے_list, (GFunc)(خالی *)&pango_شے_free, تہی);
    g_list_free(شے_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحیح
gui_mch_init_font(char_u *font_name, صحیح fontset غیر_مستعمل)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحیح			    چوڑائی;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
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
    /*
     * Set char_width to half the width obtained from pango_layout_get_size()
     * for CJK fixed_width/bi-width fonts.  An unpatched version of Xft leads
     * Pango to use the same width for both non-CJK characters (e.g. Latin
     * letters and numbers) and CJK characters.  This results in 's p a c e d
     * o u t' rendering when a CJK 'fixed width' font is used. To work around
     * that, divide the width returned by Pango by 2 if cjk_width is equal to
     * width for CJK fonts.
     *
     * For related bugs, see:
     * http://bugzilla.gnome.org/show_bug.cgi?id=106618
     * http://bugzilla.gnome.org/show_bug.cgi?id=106624
     *
     * With this, for all four of the following cases, Vim works fine:
     *	   guifont=CJK_fixed_width_font
     *	   guifont=Non_CJK_fixed_font
     *	   guifont=Non_CJK_fixed_font,CJK_Fixed_font
     *	   guifont=Non_CJK_fixed_font guifontwide=CJK_fixed_font
     */
    اگر (is_cjk_font(gui.norm_font))
    {
	صحیح cjk_چوڑائی;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_سیٹ_متن(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_سائز(layout, &cjk_چوڑائی, تہی);

	اگر (چوڑائی == cjk_چوڑائی)  // Xft not patched
	    چوڑائی /= 2;
    }
    g_object_unref(layout);

    gui.char_چوڑائی = (چوڑائی / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    اگر (gui.char_چوڑائی <= 0)
	gui.char_چوڑائی = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_سیٹ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_جدول_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    اگر (gui.wide_font != تہی
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = تہی;
    }

    اگر (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    ورنہ
    {
	// Preserve the logical dimensions of the screen.
	update_کھڑکی_manager_hints(0, 0);
    }

    واپس OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, صحیح report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    اگر (!gui.اندر_use || name == تہی)
	واپس تہی;

    اگر (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, تہی);
	اگر (buf != تہی)
	{
	    font = pango_font_description_from_string((ثابت حرف *)buf);
	    vim_free(buf);
	}
	ورنہ
	    font = تہی;
    }
    ورنہ
	font = pango_font_description_from_string((ثابت حرف *)name);

    اگر (font != تہی)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	اگر (pango_font_description_get_سائز(font) <= 0)
	    pango_font_description_سیٹ_سائز(font, 10 * PANGO_SCALE);

	real_font = pango_context_بھرو_font(gui.متن_context, font);

	اگر (real_font == تہی)
	{
	    pango_font_description_free(font);
	    font = تہی;
	}
	ورنہ
	    g_object_unref(real_font);
    }

    اگر (font == تہی)
    {
	اگر (report_error)
	    semsg(_((حرف *)e_unknown_font_str), name);
	واپس تہی;
    }

    واپس font;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name غیر_مستعمل)
{
    اگر (font != NOFONT)
    {
	حرف	*pangoname = pango_font_description_مقصد_string(font);

	اگر (pangoname != تہی)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    واپس s;
	}
    }
    واپس تہی;
}
#اگر_ختم

/*
 * If a font is not going to be used, free its structure.
 */
    خالی
gui_mch_free_font(GuiFont font)
{
    اگر (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
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
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
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

	ثابت حرف* fam_name = pango_font_family_get_name(font_families[i]);
	اگر (داخلہ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&داخلہ_conv, (char_u*)fam_name, تہی);
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
 * Return the Pixel value (color) for the given color name.
 *
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T color = INVALCOLOR;

    اگر (!gui.اندر_use)		// can't do this when GUI not running
	واپس color;

    اگر (name != تہی)
	color = gui_get_color_cmn(name);

#اگر GTK_CHECK_نسخہ(3,0,0)
    واپس color;
#ورنہ
    اگر (color == INVALCOLOR)
	واپس INVALCOLOR;

    واپس gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#اگر_ختم
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    واپس gui_get_rgb_color_cmn(r, g, b);
#ورنہ
    GdkColor gcolor;
    صحیح ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    واپس ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#اگر_ختم
}

/*
 * Set the current text foreground color.
 */
    خالی
gui_mch_سیٹ_fg_color(guicolor_T color)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    *gui.fgcolor = color_مقصد_rgba(color);
#ورنہ
    gui.fgcolor->pixel = (غیرعلامتی طویل)color;
#اگر_ختم
}

/*
 * Set the current text background color.
 */
    خالی
gui_mch_سیٹ_bg_color(guicolor_T color)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    *gui.bgcolor = color_مقصد_rgba(color);
#ورنہ
    gui.bgcolor->pixel = (غیرعلامتی طویل)color;
#اگر_ختم
}

/*
 * Set the current text special color.
 */
    خالی
gui_mch_سیٹ_sp_color(guicolor_T color)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    *gui.spcolor = color_مقصد_rgba(color);
#ورنہ
    gui.spcolor->pixel = (غیرعلامتی طویل)color;
#اگر_ختم
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#تعریف_کرو داخل_کرو_PANGO_ATTR(Attribute, AttrList, آغاز_2, انتہا_2)  \
    G_STMT_آغاز{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->آغاز_اشاریہ = (آغاز_2);		    \
	tmp_attr_->انتہا_اشاریہ = (انتہا_2);			    \
	pango_attr_list_داخل_کرو((AttrList), tmp_attr_);	    \
    }G_STMT_انتہا

    ساکن خالی
apply_wide_font_attr(char_u *s, صحیح len, PangoAttrList *attr_list)
{
    char_u  *آغاز = تہی;
    char_u  *p;
    صحیح	    uc;

    برائے (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	اگر (آغاز == تہی)
	{
	    اگر (uc >= 0x80 && utf_char2cells(uc) == 2)
		آغاز = p;
	}
	ورنہ اگر (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, آغاز - s, p - s);
	    آغاز = تہی;
	}
    }

    اگر (آغاز != تہی)
	داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, آغاز - s, len);
}

    ساکن صحیح
گنتی_cluster_cells(char_u *s, Pango_شے *شے,
		    PangoGlyphString* glyphs, صحیح i,
		    صحیح *cluster_چوڑائی,
		    صحیح *آخری_glyph_rbearing)
{
    char_u  *p;
    صحیح	    اگلا;	// glyph start index of next cluster
    صحیح	    آغاز, انتہا; // string segment of current cluster
    صحیح	    چوڑائی;	// real cluster width in Pango units
    صحیح	    uc;
    صحیح	    cellcount = 0;

    چوڑائی = glyphs->glyphs[i].geometry.چوڑائی;

    برائے (اگلا = i + 1; اگلا < glyphs->عدد_glyphs; ++اگلا)
    {
	اگر (glyphs->glyphs[اگلا].attr.is_cluster_آغاز)
	    توڑو;
	ورنہ اگر (glyphs->glyphs[اگلا].geometry.چوڑائی > چوڑائی)
	    چوڑائی = glyphs->glyphs[اگلا].geometry.چوڑائی;
    }

    آغاز = شے->انحراف + glyphs->log_clusters[i];
    انتہا   = شے->انحراف + ((اگلا < glyphs->عدد_glyphs) ?
			    glyphs->log_clusters[اگلا] : شے->لمبائی);

    برائے (p = s + آغاز; p < s + انتہا; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	اگر (uc < 0x80)
	    ++cellcount;
	ورنہ اگر (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    اگر (آخری_glyph_rbearing != تہی
	    && cellcount > 0 && اگلا == glyphs->عدد_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(شے->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, تہی);

	اگر (PANGO_RBEARING(ink_rect) > 0)
	    *آخری_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    اگر (cellcount > 0)
	*cluster_چوڑائی = چوڑائی;

    واپس cellcount;
}

/*
 * If there are only combining characters in the cluster, we cannot just
 * change the width of the previous glyph since there is none.	Therefore
 * some guesswork is needed.
 *
 * If ink_rect.x is negative Pango apparently has taken care of the composing
 * by itself.  Actually setting x_offset = 0 should be sufficient then, but due
 * to problems with composing from different fonts we still need to fine-tune
 * x_offset to avoid ugliness.
 *
 * If ink_rect.x is not negative, force overstriking by pointing x_offset to
 * the position of the previous glyph.	Apparently this happens only with old
 * X fonts which don't provide the special combining information needed by
 * Pango.
 */
    ساکن خالی
تیاری_zero_چوڑائی_cluster(Pango_شے *شے, PangoGlyphInfo *glyph,
			 صحیح آخری_cellcount, صحیح آخری_cluster_چوڑائی,
			 صحیح آخری_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحیح		    چوڑائی;

    چوڑائی = آخری_cellcount * gui.char_چوڑائی * PANGO_SCALE;
    glyph->geometry.x_انحراف = -چوڑائی + MAX(0, چوڑائی - آخری_cluster_چوڑائی) / 2;
    glyph->geometry.چوڑائی = 0;

    pango_font_get_glyph_extents(شے->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    اگر (ink_rect.x < 0)
    {
	glyph->geometry.x_انحراف += آخری_glyph_rbearing;
	glyph->geometry.y_انحراف  = logical_rect.اونچائی
		- (gui.char_اونچائی - p_linespace) * PANGO_SCALE;
    }
    ورنہ
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_انحراف = -چوڑائی + MAX(0, چوڑائی - ink_rect.چوڑائی) / 2;
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
draw_glyph_string(صحیح صف, صحیح ستون, صحیح عدد_cells, صحیح flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#ورنہ
    ساکن خالی
draw_glyph_string(صحیح صف, صحیح ستون, صحیح عدد_cells, صحیح flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#اگر_ختم
{
    اگر (!(flags & DRAW_TRANSP))
    {
#اگر GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(ستون), FILL_Y(صف),
			عدد_cells * gui.char_چوڑائی, gui.char_اونچائی);
	cairo_fill(cr);
#ورنہ
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->کھڑکی_2,
			   gui.متن_gc,
			   TRUE,
			   FILL_X(ستون),
			   FILL_Y(صف),
			   عدد_cells * gui.char_چوڑائی,
			   gui.char_اونچائی);
#اگر_ختم
    }

#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_ماخذ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_مقصد(cr, متن_X(ستون), متن_Y(صف));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#ورنہ
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->کھڑکی_2,
		    gui.متن_gc,
		    font,
		    متن_X(ستون),
		    متن_Y(صف),
		    glyphs);
#اگر_ختم

    // redraw the contents with an offset of 1 to emulate bold
    اگر ((flags & DRAW_BOLD) && !gui.font_can_bold)
#اگر GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_مقصد(cr, متن_X(ستون) + 1, متن_Y(صف));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#ورنہ
	gdk_draw_glyphs(gui.drawarea->کھڑکی_2,
			gui.متن_gc,
			font,
			متن_X(ستون) + 1,
			متن_Y(صف),
			glyphs);
#اگر_ختم
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
draw_under(صحیح flags, صحیح صف, صحیح ستون, صحیح cells, cairo_t *cr)
#ورنہ
    ساکن خالی
draw_under(صحیح flags, صحیح صف, صحیح ستون, صحیح cells)
#اگر_ختم
{
    صحیح			i;
    صحیح			انحراف;
    ساکن ثابت صحیح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    صحیح			y = FILL_Y(صف + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    اگر (flags & DRAW_UNDERC)
    {
#اگر GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_مقصد(cr, FILL_X(ستون) + 1, y - 2 + 0.5);
	برائے (i = FILL_X(ستون) + 1; i < FILL_X(ستون + cells); ++i)
	{
	    انحراف = val[i % 8];
	    cairo_سطر_مقصد(cr, i, y - انحراف + 0.5);
	}
	cairo_stroke(cr);
#ورنہ
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.spcolor);
	برائے (i = FILL_X(ستون); i < FILL_X(ستون + cells); ++i)
	{
	    انحراف = val[i % 8];
	    gdk_draw_نقطہ(gui.drawarea->کھڑکی_2, gui.متن_gc, i, y - انحراف);
	}
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#اگر_ختم
    }

    // Draw a strikethrough line
    اگر (flags & DRAW_STRIKE)
    {
#اگر GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_مقصد(cr, FILL_X(ستون), y + 1 - gui.char_اونچائی/2 + 0.5);
	cairo_سطر_مقصد(cr, FILL_X(ستون + cells), y + 1 - gui.char_اونچائی/2 + 0.5);
	cairo_stroke(cr);
#ورنہ
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.spcolor);
	gdk_draw_سطر(gui.drawarea->کھڑکی_2, gui.متن_gc,
		      FILL_X(ستون), y + 1 - gui.char_اونچائی/2,
		      FILL_X(ستون + cells), y + 1 - gui.char_اونچائی/2);
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#اگر_ختم
    }

    // Underline: draw a line at the bottom of the character cell.
    اگر (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	اگر (p_linespace > 1)
	    y -= p_linespace - 1;
#اگر GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_مقصد(cr, FILL_X(ستون), y + 0.5);
	cairo_سطر_مقصد(cr, FILL_X(ستون + cells), y + 0.5);
	cairo_stroke(cr);
#ورنہ
	gdk_draw_سطر(gui.drawarea->کھڑکی_2, gui.متن_gc,
		      FILL_X(ستون), y,
		      FILL_X(ستون + cells) - 1, y);
#اگر_ختم
    }
}

    صحیح
gui_gtk_draw_string(صحیح صف, صحیح ستون, char_u *s, صحیح len, صحیح flags)
{
    char_u	*conv_buf = تہی;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    صحیح		convlen;
    char_u	*sp, *bp;
    صحیح		plen;
    صحیح		len_مجموع;	// return value needs to add up since we are
				// printing substrings
    صحیح		byte_مجموع;	// byte position in string
    char_u	*cs;		// current *s pointer
    صحیح		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    صحیح		should_need_pango = FALSE;
    صحیح		slen;
    صحیح		is_ligature;
    صحیح		is_utf8;
    char_u	backup_ch;

    اگر (gui.متن_context == تہی || gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس len;

    اگر (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != تہی, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	برائے (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    اگر ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		اگر (new_conv_buf == تہی)
		    واپس len;
		plen += bp - conv_buf;
		mch_memmove(new_conv_buf, conv_buf, plen);
		new_conv_buf[plen] = ' ';
		mch_memmove(new_conv_buf + plen + 1, conv_buf + plen,
							  convlen - plen + 1);
		vim_free(conv_buf);
		conv_buf = new_conv_buf;
		++convlen;
		bp = conv_buf + plen;
		plen = 1;
	    }
	    sp += (*mb_ptr2len)(sp);
	    bp += plen;
	}
	s = conv_buf;
	len = convlen;
    }

    /*
     * Ligature support and complex utf-8 char optimization:
     * String received to output to screen can print using pre-cached glyphs
     * (fast) or Pango (slow). Ligatures and multibype utf-8 must use Pango.
     * Since we receive mixed content string, split it into logical segments
     * that are guaranteed to go through glyphs as much as possible. Since
     * single ligature char prints as ascii, print it that way.
     */
    len_مجموع = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_مجموع = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    اگر (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    اگر (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    جب (cs < s + len)
    {
	slen = 0;
	جب (slen < (len - byte_مجموع))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    اگر (is_ligature && !needs_pango)
	    {
		اگر ((slen + 1) < (len - byte_مجموع))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		ورنہ
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    اگر ((!is_utf8) && ((slen + 1) < (len - byte_مجموع)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    اگر (needs_pango != should_need_pango) // mode switch
		توڑو;
	    اگر (needs_pango)
	    {
		اگر (is_ligature)
		{
		    slen++; // ligature char by char
		}
		ورنہ // is_utf8
		{
		    اگر ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			جب ((slen < (len - byte_مجموع))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    ورنہ اگر ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    ورنہ اگر ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    ورنہ اگر ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    ورنہ
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    ورنہ
	    {
		slen++; // ascii
	    }
	}

	اگر (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_مجموع += gui_gtk_draw_string_ext(صف, ستون + len_مجموع, cs, slen, flags,
					    needs_pango);
	اگر (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_مجموع += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    واپس len_مجموع;
}

#اگر GTK_CHECK_نسخہ(3,0,0)
ساکن cairo_region_t *dirty_region = تہی;

    ساکن خالی
قطار_draw_area(صحیح x, صحیح y, صحیح چوڑائی, صحیح اونچائی)
{
    cairo_rectangle_int_t rect;

    اگر (چوڑائی <= 0 || اونچائی <= 0 || gui.drawarea == تہی)
	واپس;

    rect.x = x;
    rect.y = y;
    rect.چوڑائی = چوڑائی;
    rect.اونچائی = اونچائی;

    اگر (dirty_region == تہی)
	dirty_region = cairo_region_create_rectangle(&rect);
    ورنہ
	cairo_region_union_rectangle(dirty_region, &rect);
}
#اگر_ختم

    صحیح
gui_gtk_draw_string_ext(
	صحیح	صف,
	صحیح	ستون,
	char_u	*s,
	صحیح	len,
	صحیح	flags,
	صحیح	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    صحیح			عمود_انحراف = 0;  // column offset in cells
    صحیح			i;
#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_t		*cr;
#اگر_ختم

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_انحراف;
    area.y = FILL_Y(صف);
    area.چوڑائی	= gui.عدد_ستون_تعداد * gui.char_چوڑائی;
    area.اونچائی = gui.char_اونچائی;

#اگر GTK_CHECK_نسخہ(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.چوڑائی, area.اونچائی);
    cairo_clip(cr);
#ورنہ
    gdk_gc_سیٹ_clip_origin(gui.متن_gc, 0, 0);
    gdk_gc_سیٹ_clip_rectangle(gui.متن_gc, &area);
#اگر_ختم

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    اگر (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != تہی
	    && !force_pango)
    {
	char_u *p;

	برائے (p = s; p < s + len; ++p)
	    اگر (*p & 0x80)
		جاؤ not_ascii;

	pango_glyph_string_سیٹ_سائز(glyphs, len);

	برائے (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#اگر GTK_CHECK_نسخہ(3,0,0)
	draw_glyph_string(صف, ستون, len, flags, gui.ascii_font, glyphs, cr);
#ورنہ
	draw_glyph_string(صف, ستون, len, flags, gui.ascii_font, glyphs);
#اگر_ختم

	عمود_انحراف = len;
    }
    ورنہ
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*شے_list;
	صحیح		cluster_چوڑائی;
	صحیح		آخری_glyph_rbearing;
	صحیح		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	اگر (!utf_valid_string(s, s + len))
	{
	    عمود_انحراف = len;
	    جاؤ skipitall;
	}

	// original width of the current cluster
	cluster_چوڑائی = PANGO_SCALE * gui.char_چوڑائی;

	// right bearing of the last non-composing glyph
	آخری_glyph_rbearing = PANGO_SCALE * gui.char_چوڑائی;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	اگر (gui.wide_font != تہی)
	    apply_wide_font_attr(s, len, attr_list);

	اگر ((flags & DRAW_BOLD) && gui.font_can_bold)
	    داخل_کرو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
			      attr_list, 0, len);
	اگر (flags & DRAW_ITALIC)
	    داخل_کرو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	شے_list = pango_itemize(gui.متن_context,
				  (ثابت حرف *)s, 0, len, attr_list, تہی);

	جب (شے_list != تہی)
	{
	    Pango_شے	*شے;
	    صحیح		شے_cells = 0; // item length in cells

	    شے = (Pango_شے *)شے_list->ڈیٹا;
	    شے_list = g_list_delete_link(شے_list, شے_list);
	    /*
	     * Increment the bidirectional embedding level by 1 if it is not
	     * even.  An odd number means the output will be RTL, but we don't
	     * want that since Vim handles right-to-left text on its own.  It
	     * would probably be sufficient to just set level = 0, but you can
	     * never know :)
	     *
	     * Unfortunately we can't take advantage of Pango's ability to
	     * render both LTR and RTL at the same time.  In order to support
	     * that, Vim's main screen engine would have to make use of Pango
	     * functionality.
	     */
	    شے->analysis.level = (شے->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    شے->analysis.shape_engine = default_shape_engine;

#اگر_تعریف_شدہ HAVE_PANGO_SHAPE_بھرا
	    pango_shape_بھرا((ثابت حرف *)s + شے->انحراف, شے->لمبائی,
		    (ثابت حرف *)s, len, &شے->analysis, glyphs);
#ورنہ
	    pango_shape((ثابت حرف *)s + شے->انحراف, شے->لمبائی,
			&شے->analysis, glyphs);
#اگر_ختم
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    برائے (i = 0; i < glyphs->عدد_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		اگر (glyph->attr.is_cluster_آغاز)
		{
		    صحیح cellcount;

		    cellcount = گنتی_cluster_cells(
			s, شے, glyphs, i, &cluster_چوڑائی,
			(شے_list != تہی) ? &آخری_glyph_rbearing : تہی);

		    اگر (cellcount > 0)
		    {
			صحیح چوڑائی;

			چوڑائی = cellcount * gui.char_چوڑائی * PANGO_SCALE;
			glyph->geometry.x_انحراف +=
					    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
			glyph->geometry.چوڑائی = چوڑائی;
		    }
		    ورنہ
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			تیاری_zero_چوڑائی_cluster(شے, glyph, cells,
						 cluster_چوڑائی,
						 آخری_glyph_rbearing);
		    }

		    شے_cells += cellcount;
		    cells = cellcount;
		}
		ورنہ اگر (i > 0)
		{
		    صحیح چوڑائی;

		    // There is a previous glyph, so we deal with combining
		    // characters the canonical way.
		    // In some circumstances Pango uses a positive x_offset,
		    // then use the width of the previous glyph for this one
		    // and set the previous width to zero.
		    // Otherwise we get a negative x_offset, Pango has already
		    // positioned the combining char, keep the widths as they
		    // are.
		    // For both adjust the x_offset to position the glyph in
		    // the middle.
		    اگر (glyph->geometry.x_انحراف >= 0)
		    {
			glyphs->glyphs[i].geometry.چوڑائی =
					 glyphs->glyphs[i - 1].geometry.چوڑائی;
			glyphs->glyphs[i - 1].geometry.چوڑائی = 0;
		    }
		    چوڑائی = cells * gui.char_چوڑائی * PANGO_SCALE;
		    glyph->geometry.x_انحراف +=
					    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
		}
		ورنہ // i == 0 "cannot happen"
		{
		    glyph->geometry.چوڑائی = 0;
		}
	    }

	    //// Aaaaand action! **
#اگر GTK_CHECK_نسخہ(3,0,0)
	    draw_glyph_string(صف, ستون + عمود_انحراف, شے_cells,
			      flags, شے->analysis.font, glyphs,
			      cr);
#ورنہ
	    draw_glyph_string(صف, ستون + عمود_انحراف, شے_cells,
			      flags, شے->analysis.font, glyphs);
#اگر_ختم

	    pango_شے_free(شے);

	    عمود_انحراف += شے_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#اگر GTK_CHECK_نسخہ(3,0,0)
    draw_under(flags, صف, ستون, عمود_انحراف, cr);
#ورنہ
    draw_under(flags, صف, ستون, عمود_انحراف);
#اگر_ختم

    pango_glyph_string_free(glyphs);

#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_destroy(cr);
    قطار_draw_area(FILL_X(ستون), FILL_Y(صف),
	    عمود_انحراف * gui.char_چوڑائی + 1, gui.char_اونچائی);
#ورنہ
    gdk_gc_سیٹ_clip_rectangle(gui.متن_gc, تہی);
#اگر_ختم

    واپس عمود_انحراف;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
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

#اگر defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحیح
gui_get_x11_windis(کھڑکی *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    اگر (dpy)
    {
	*dis = dpy;
	*win = GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.mainwin));
	واپس OK;
    }

    *dis = تہی;
    *win = 0;
    واپس FAIL;
}
#اگر_ختم

    Display *
gui_mch_get_display(خالی)
{
    اگر (gui.mainwin != تہی && gtk_widget_get_کھڑکی(gui.mainwin) != تہی
#اگر GTK_CHECK_نسخہ(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#اگر_ختم
	)
	واپس GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin));
    ورنہ
	واپس تہی;
}

    خالی
gui_mch_beep(خالی)
{
    GdkDisplay *display;

    اگر (gui.mainwin != تہی && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    ورنہ
	display = gdk_display_get_default();

    اگر (display != تہی)
	gdk_display_beep(display);
}

    خالی
gui_mch_flash(صحیح msec)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    // TODO Replace GdkGC with Cairo
    (خالی)msec;
#ورنہ
    GdkGCValues	اقدار;
    GdkGC	*invert_gc;

    اگر (gui.drawarea->کھڑکی_2 == تہی)
	واپس;

    اقدار.foreground.pixel = gui.norm_pixel ^ gui.عقب_pixel;
    اقدار.background.pixel = gui.norm_pixel ^ gui.عقب_pixel;
    اقدار.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_اقدار(gui.drawarea->کھڑکی_2,
				       &اقدار,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_سیٹ_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحیح)Columns) + gui.border_انحراف,
		       FILL_Y((صحیح)صفیں) + gui.border_انحراف);

    gui_mch_flush();
    ui_delay((طویل)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحیح)Columns) + gui.border_انحراف,
		       FILL_Y((صحیح)صفیں) + gui.border_انحراف);

    gdk_gc_destroy(invert_gc);
#اگر_ختم
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    خالی
gui_mch_invert_rectangle(صحیح r, صحیح c, صحیح nr, صحیح nc)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    ثابت GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_چوڑائی, nr * gui.char_اونچائی
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);

    cairo_سیٹ_ماخذ_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# اگر CAIRO_نسخہ >= CAIRO_نسخہ_ENCODE(1,9,2)
    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# ورنہ
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
    cairo_fill(cr);

    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
#ورنہ
    GdkGCValues اقدار;
    GdkGC *invert_gc;

    اگر (gui.drawarea->کھڑکی_2 == تہی)
	واپس;

    اقدار.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_اقدار(gui.drawarea->کھڑکی_2,
				       &اقدار,
				       GDK_GC_FUNCTION);
    gdk_gc_سیٹ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_چوڑائی, (nr) * gui.char_اونچائی);
    gdk_gc_destroy(invert_gc);
#اگر_ختم
}

/*
 * Iconify the GUI window.
 */
    خالی
gui_mch_iconify(خالی)
{
    gtk_کھڑکی_iconify(GTK_کھڑکی(gui.mainwin));
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    خالی
gui_mch_سیٹ_foreground(خالی)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_کھڑکی_سیٹ_keep_above(GTK_کھڑکی(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_کھڑکی_present(GTK_کھڑکی(gui.mainwin));
    gui_may_flush();
    gtk_کھڑکی_سیٹ_keep_above(GTK_کھڑکی(gui.mainwin), FALSE);
    gui_may_flush();
}
#اگر_ختم

/*
 * Draw a cursor without focus.
 */
    خالی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحیح		i = 1;
#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_t    *cr;
#اگر_ختم

    اگر (gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس;

#اگر GTK_CHECK_نسخہ(3,0,0)
    cr = cairo_create(gui.surface);
#اگر_ختم

    gui_mch_سیٹ_fg_color(color);

#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_ماخذ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#ورنہ
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#اگر_ختم
    اگر (mb_lefthalve(gui.صف, gui.ستون))
	i = 2;
#اگر GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
    cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.ستون) + 0.5, FILL_Y(gui.صف) + 0.5,
	    i * gui.char_چوڑائی - 1, gui.char_اونچائی - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#ورنہ
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    FALSE,
	    FILL_X(gui.ستون), FILL_Y(gui.صف),
	    i * gui.char_چوڑائی - 1, gui.char_اونچائی - 1);
#اگر_ختم
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    خالی
gui_mch_draw_part_cursor(صحیح w, صحیح h, guicolor_T color)
{
    اگر (gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس;

    gui_mch_سیٹ_fg_color(color);

#اگر GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_سیٹ_ماخذ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_دایاں ? FILL_X(gui.ستون + 1) - w :
# endif
	    FILL_X(gui.ستون), FILL_Y(gui.صف) + gui.char_اونچائی - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#ورنہ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_دایاں ? FILL_X(gui.ستون + 1) - w :
# endif
	    FILL_X(gui.ستون),
	    FILL_Y(gui.صف) + gui.char_اونچائی - h,
	    w, h);
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    خالی
gui_mch_update(خالی)
{
    صحیح cnt = 0;	// prevent endless loop
    جب (g_main_context_pending(تہی) && !vim_is_داخلہ_buf_بھرا()
	    && ++cnt < 100)
    {
#اگر GTK_CHECK_نسخہ(3,0,0)
	صحیح prio = 0;
	g_main_context_prepare(تہی, &prio);
	// peek internal scheduling of redraw
	اگر (prio == GDK_ترجیح_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#اگر_ختم
	g_main_context_iteration(تہی, TRUE);
    }
}

    ساکن timeout_cb_type
داخلہ_timer_cb(gpointer ڈیٹا)
{
    صحیح *timed_برآمد = (صحیح *) ڈیٹا;

    // Just inform the caller about the occurrence of it
    *timed_برآمد = TRUE;

    واپس FALSE;		// don't happen again
}

#اگر_تعریف_شدہ FEAT_کام_CHANNEL
    ساکن timeout_cb_type
channel_poll_cb(gpointer ڈیٹا غیر_مستعمل)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    واپس TRUE;		// repeat
}
#اگر_ختم

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
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

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
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
	    // Need to recompute the waiting time.
	    جاؤ theend;
# endif
#اگر_ختم

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	اگر (!داخلہ_دستیاب())
	{
	    g_main_context_iteration(تہی, TRUE);
#اگر_تعریف_شدہ GDK_WINDOWING_WAYLAND
    اگر (gui.is_wayland && clip_ستارہ.state == SELECT_اندر_پیش_رفت)
	gui_may_flush();
#اگر_ختم
	}

	// Got char, return immediately
	اگر (داخلہ_دستیاب())
	{
	    retval = OK;
	    جاؤ theend;
	}
    } جب (wtime < 0 || !timed_برآمد);

    /*
     * Flush all eventually pending (drawing) events.
     */
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


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    خالی
gui_mch_flush(خالی)
{
    اگر (gui.mainwin == تہی || !gtk_widget_get_realized(gui.mainwin))
	واپس;
#اگر GTK_CHECK_نسخہ(3,0,0)
    اگر (dirty_region != تہی && gui.drawarea != تہی)
    {
	gtk_widget_قطار_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = تہی;
    }
#ورنہ
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       واپس;
#اگر_ختم
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    خالی
gui_mch_clear_بلاک(صحیح row1arg, صحیح col1arg, صحیح row2arg, صحیح col2arg)
{

    صحیح col1 = check_ستون(col1arg);
    صحیح col2 = check_ستون(col2arg);
    صحیح row1 = check_صف(row1arg);
    صحیح row2 = check_صف(row2arg);

#اگر GTK_CHECK_نسخہ(3,0,0)
    اگر (gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس;
#ورنہ
    GdkColor color;

    اگر (gui.drawarea->کھڑکی_2 == تہی)
	واپس;

    color.pixel = gui.عقب_pixel;
#اگر_ختم

#اگر GTK_CHECK_نسخہ(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ثابت GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_چوڑائی + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_اونچائی
	};
	cairo_t * ثابت cr = cairo_create(gui.surface);
# اگر GTK_CHECK_نسخہ(3,22,2)
	سیٹ_cairo_ماخذ_rgba_from_color(cr, gui.عقب_pixel);
# ورنہ
	Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(gui.drawarea);
	cairo_نمونہ_t * ثابت pat = gdk_کھڑکی_get_background_نمونہ(win);
	اگر (pat != تہی)
	    cairo_سیٹ_ماخذ(cr, pat);
	ورنہ
	    سیٹ_cairo_ماخذ_rgba_from_color(cr, gui.عقب_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
	cairo_fill(cr);
	cairo_destroy(cr);

	قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
    }
#ورنہ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_چوڑائی
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_اونچائی);
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
}

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
gui_gtk_کھڑکی_clear(Gdk_کھڑکی *win)
{
    ثابت GdkRectangle rect = {
	0, 0, gdk_کھڑکی_get_چوڑائی(win), gdk_کھڑکی_get_اونچائی(win)
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);
# اگر GTK_CHECK_نسخہ(3,22,2)
    سیٹ_cairo_ماخذ_rgba_from_color(cr, gui.عقب_pixel);
# ورنہ
    cairo_نمونہ_t * ثابت pat = gdk_کھڑکی_get_background_نمونہ(win);
    اگر (pat != تہی)
	cairo_سیٹ_ماخذ(cr, pat);
    ورنہ
	سیٹ_cairo_ماخذ_rgba_from_color(cr, gui.عقب_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
    cairo_fill(cr);
    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اونچائی);
}
#ورنہ
# define gui_gtk_کھڑکی_clear(win)  gdk_کھڑکی_clear(win)
#اگر_ختم

    خالی
gui_mch_clear_all(خالی)
{
    اگر (gtk_widget_get_کھڑکی(gui.drawarea) != تہی)
	gui_gtk_کھڑکی_clear(gtk_widget_get_کھڑکی(gui.drawarea));
}

#اگر !GTK_CHECK_نسخہ(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ساکن خالی
check_copy_area(خالی)
{
    GdkEvent        *event;
    صحیح                expose_گنتی;
    اگر (gui.visibility != GDK_VISIBILITY_PARTIAL)
	واپس;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    کرو
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->کھڑکی_2);
	اگر (event == تہی)
	    توڑو; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.چوڑائی, event->expose.area.اونچائی);
	expose_گنتی = event->expose.گنتی;
	gdk_event_free(event);
    }
    جب (expose_گنتی > 0); // more events follow
    gui_can_update_cursor();
}
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)

#اگر GTK_CHECK_نسخہ(3,0,0)
    ساکن خالی
gui_gtk_surface_copy_rect(صحیح dest_x, صحیح dest_y,
			  صحیح src_x,  صحیح src_y,
			  صحیح چوڑائی,  صحیح اونچائی)
{
    cairo_t * ثابت cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ساکن cairo_surface_t *scroll_scratch = تہی;
    ساکن صحیح scratch_w = 0, scratch_h = 0;
    صحیح آخری_صف = صفیں - 1, آخری_صف_y = آخری_صف * gui.char_اونچائی;
    bool آخری_صف_overlap = (dest_y + اونچائی) > آخری_صف_y;
    اگر (gui.is_wayland && ( !(State & منوال_CMDLINE) || !آخری_صف_overlap) )
    {
	اگر (dest_y < src_y) // scroll up
	{
	    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_ماخذ);
	    cairo_سیٹ_ماخذ_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اونچائی);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	ورنہ
	{   //  reuse surface when scrolling, only realloc if larger
	    اگر (scroll_scratch == تہی || چوڑائی > scratch_w || اونچائی > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), چوڑائی, اونچائی);
		scratch_w = چوڑائی, scratch_h = اونچائی;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_سیٹ_ماخذ_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_ماخذ);
	    cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اونچائی);
	    cairo_clip(cr);
	    cairo_سیٹ_ماخذ_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    ورنہ
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اونچائی);
	cairo_clip(cr);
	cairo_ڈالو_گروہ(cr);
	cairo_سیٹ_ماخذ_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_نکالو_گروہ_مقصد_ماخذ(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#اگر_ختم

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    خالی
gui_mch_delete_سطریں(صحیح صف, صحیح عدد_سطریں)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    ثابت صحیح ncols = gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1;
    ثابت صحیح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحیح src_nrows = nrows - عدد_سطریں;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    gui.char_چوڑائی * ncols + 1,     gui.char_اونچائی * src_nrows);
    gui_clear_بلاک(
	    gui.scroll_region_bot - عدد_سطریں + 1, gui.scroll_region_بایاں,
	    gui.scroll_region_bot,		   gui.scroll_region_دایاں);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,	gui.char_اونچائی * nrows);
#ورنہ
    اگر (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	واپس;			// Can't see the window

    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سیٹ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_کھڑکی_copy_area(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.drawarea->کھڑکی_2,
	    FILL_X(gui.scroll_region_بایاں),
	    FILL_Y(صف + عدد_سطریں),
	    gui.char_چوڑائی * (gui.scroll_region_دایاں
					    - gui.scroll_region_بایاں + 1) + 1,
	    gui.char_اونچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1));

    gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
						       gui.scroll_region_بایاں,
		    gui.scroll_region_bot, gui.scroll_region_دایاں);
    check_copy_area();
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    خالی
gui_mch_داخل_کرو_سطریں(صحیح صف, صحیح عدد_سطریں)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    ثابت صحیح ncols = gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1;
    ثابت صحیح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحیح src_nrows = nrows - عدد_سطریں;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,     gui.char_اونچائی * src_nrows);
    gui_clear_بلاک(
	    صف,		 gui.scroll_region_بایاں,
	    صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,	gui.char_اونچائی * nrows);
#ورنہ
    اگر (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	واپس;			// Can't see the window

    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سیٹ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_کھڑکی_copy_area(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    gui.drawarea->کھڑکی_2,
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * (gui.scroll_region_دایاں
					    - gui.scroll_region_بایاں + 1) + 1,
	    gui.char_اونچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1));

    gui_clear_بلاک(صف, gui.scroll_region_بایاں,
				صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
    check_copy_area();
#اگر_ختم // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    خالی
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	ہدف;
    غیرعلامتی	i;
    time_t	آغاز;

    برائے (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	اگر (!clip_html && selection_targets[i].info == ہدف_HTML)
	    جاری;
	received_selection = RS_NONE;
	ہدف = gdk_atom_intern(selection_targets[i].ہدف, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, ہدف,
			      (guint32)GDK_CURRENT_وقت);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	آغاز = وقت(تہی);
	جب (received_selection == RS_NONE && وقت(تہی) < آغاز + 3)
	    g_main_context_iteration(تہی, TRUE);	// wait for selection_received_cb

	اگر (received_selection != RS_FAIL)
	    واپس;
    }

    اگر (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    خالی
clip_mch_lose_selection(Clipboard_T *cbd غیر_مستعمل)
{
    اگر (اندر_selection_clear_event)
	واپس;

    gtk_selection_owner_سیٹ(تہی, cbd->gtk_sel_atom, gui.event_وقت);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    صحیح
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    اگر (is_autocmd_blocked())
	واپس OK;

    صحیح success;

    success = gtk_selection_owner_سیٹ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_وقت);
    // don't update on every visual selection change
    اگر (!(cbd->owned && VIsual_فعال))
	gui_gtk_سیٹ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    واپس (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    خالی
clip_mch_سیٹ_selection(Clipboard_T *cbd غیر_مستعمل)
{
}

#اگر defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    صحیح
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    واپس gdk_selection_owner_get(cbd->gtk_sel_atom) != تہی;
}
#اگر_ختم


#اگر defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    خالی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->id == تہی)
	واپس;

    اگر (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    اگر (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_سیٹ_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    خالی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    اگر (menu->id == 0)
	واپس;

    اگر (hidden)
    {
	اگر (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    ورنہ
    {
	اگر (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    خالی
gui_mch_draw_menubar(خالی)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#اگر_ختم // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    خالی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (sb->id == تہی)
	واپس;

    gtk_widget_سیٹ_visible(sb->id, flag);
    update_کھڑکی_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#اگر GTK_CHECK_نسخہ(3,0,0)
    واپس (long_u)pixel;
#ورنہ
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (غیرعلامتی طویل)pixel, &color);

    واپس (guicolor_T)(
	    (((غیرعلامتی)color.red   & 0xff00) << 8)
	 |  ((غیرعلامتی)color.green & 0xff00)
	 | (((غیرعلامتی)color.blue  & 0xff00) >> 8));
#اگر_ختم
}

/*
 * Get current mouse coordinates in text window.
 */
    خالی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    gui_gtk_get_اشارہ_گر(gui.drawarea, x, y, تہی);
}

    خالی
gui_mch_setmouse(صحیح x, صحیح y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    اگر (dpy)
	XWarp_اشارہ_گر(dpy, (کھڑکی)0,
		GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#اگر_تعریف_شدہ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساکن صحیح آخری_shape = 0;
#اگر_ختم

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    خالی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.اشارہ_گر_hidden == hide)
	واپس;

    gui.اشارہ_گر_hidden = hide;
    اگر (gtk_widget_get_کھڑکی(gui.drawarea) && gui.blank_اشارہ_گر != تہی)
    {
	اگر (hide)
	    gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		    gui.blank_اشارہ_گر);
	ورنہ
#اگر_تعریف_شدہ FEAT_MOUSESHAPE
	    mch_سیٹ_mouse_shape(آخری_shape);
#ورنہ
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea), تہی);
#اگر_ختم
    }
}

#اگر defined(FEAT_MOUSESHAPE)

# اگر GTK_CHECK_نسخہ(3,0,0)
ساکن ثابت حرف * mshape_css_نام[] =
{
    "default",                  // arrow aka GDK_LEFT_PTR
    "blank",                    // blank aka GDK_CURSOR_IS_PIXMAP
    "text",                     // beam aka GDK_XTERM
    "ns-resize",                // updown aka GDK_SB_V_DOUBLE_ARROW
    "nwse-resize",              // udsizing aka GDK_SIZING
    "ew-resize",                // leftright aka GDK_SB_H_DOUBLE_ARROW
    "ew-resize",                // lrsizing aka GDK_SIZING
    "progress",                 // busy aka GDK_WATCH
    "not-allowed",              // no aka GDK_X_CURSOR
    "crosshair",                // crosshair aka GDK_CROSSHAIR
    "pointer",                  // hand1 aka GDK_HAND1
    "pointer",                  // hand2 aka GDK_HAND2
    "default",                  // pencil aka GDK_PENCIL (no css analogue)
    "help",                     // question aka GDK_QUESTION_ARROW
    "default",                  // right-arrow aka GDK_RIGHT_PTR (no css analogue)
    "default",                  // up-arrow aka GDK_CENTER_PTR (no css analogue)
    "default"                   // GDK_LEFT_PTR (no css analogue)
};
# ورنہ
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساکن ثابت صحیح mshape_ids[] =
{
    GDK_بایاں_PTR,		// arrow
    GDK_CURSOR_IS_PIXMAP,	// blank
    GDK_XTERM,			// beam
    GDK_SB_V_DOUBLE_ARROW,	// updown
    GDK_SIZING,			// udsizing
    GDK_SB_H_DOUBLE_ARROW,	// leftright
    GDK_SIZING,			// lrsizing
    GDK_WATCH,			// busy
    GDK_X_CURSOR,		// no
    GDK_CROSSHAIR,		// crosshair
    GDK_HAND1,			// hand1
    GDK_HAND2,			// hand2
    GDK_PENCIL,			// pencil
    GDK_QUESTION_ARROW,		// question
    GDK_دایاں_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_بایاں_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    خالی
mch_سیٹ_mouse_shape(صحیح shape)
{
    GdkCursor	   *c;
    صحیح		   id;                    // Only id or css_name is used.
# اگر GTK_CHECK_نسخہ(3,0,0)
    ثابت حرف     *css_name = "default";
    GdkDisplay     *disp;
# endif

    اگر (gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس;

    اگر (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		gui.blank_اشارہ_گر);
    ورنہ
    {
	اگر (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    اگر (id >= GDK_آخری_CURSOR)
		id = GDK_بایاں_PTR;
	    ورنہ
		id &= ~1;	// they are always even (why?)
	}
# اگر GTK_CHECK_نسخہ(3,0,0)
	ورنہ اگر (shape < (صحیح)ARRAY_لمبائی(mshape_css_نام))
	    css_name = mshape_css_نام[shape];
# ورنہ
	ورنہ اگر (shape < (صحیح)ARRAY_لمبائی(mshape_ids))
	    id = mshape_ids[shape];
# endif
	ورنہ
	    واپس;
# اگر GTK_CHECK_نسخہ(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# ورنہ
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea), c);
# اگر GTK_CHECK_نسخہ(3,0,0)
	g_object_unref(G_OBJECT(c));
# ورنہ
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    اگر (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}
#اگر_ختم // FEAT_MOUSESHAPE


#اگر defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_چوڑائی  (2 * gui.char_چوڑائی)
# define SIGN_اونچائی (gui.char_اونچائی)
# define SIGN_ASPECT ((دوہرا)SIGN_اونچائی / (دوہرا)SIGN_چوڑائی)

    خالی
gui_mch_drawsign(صحیح صف, صحیح ستون, صحیح typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    اگر (sign == تہی || gui.drawarea == تہی
	    || gtk_widget_get_کھڑکی(gui.drawarea) == تہی)
	واپس;

    صحیح چوڑائی;
    صحیح اونچائی;
    صحیح xoffset;
    صحیح yoffset;
    صحیح need_scale;

    چوڑائی  = gdk_pixbuf_get_چوڑائی(sign);
    اونچائی = gdk_pixbuf_get_اونچائی(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (چوڑائی > SIGN_چوڑائی + 2
	    || اونچائی != SIGN_اونچائی
	    || (چوڑائی < 3 * SIGN_چوڑائی / 4
		&& اونچائی < 3 * SIGN_اونچائی / 4));
    اگر (need_scale)
    {
	دوہرا  aspect;
	صحیح	    w = چوڑائی;
	صحیح	    h = اونچائی;

	// Keep the original aspect ratio
	aspect = (دوہرا)اونچائی / (دوہرا)چوڑائی;
	چوڑائی  = (دوہرا)SIGN_چوڑائی * SIGN_ASPECT / aspect;
	چوڑائی  = MIN(چوڑائی, SIGN_چوڑائی);
	اگر (((دوہرا)(MAX(اونچائی, SIGN_اونچائی)) /
		    (دوہرا)(MIN(اونچائی, SIGN_اونچائی))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    اونچائی = (دوہرا)SIGN_اونچائی * SIGN_ASPECT / aspect;
	    اونچائی = MIN(اونچائی, SIGN_اونچائی);
	}
	ورنہ
	    اونچائی = (دوہرا)چوڑائی * aspect;

	اگر (w == چوڑائی && h == اونچائی)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	ورنہ
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, چوڑائی, اونچائی,
		    GDK_INTERP_BILINEAR);
	    اگر (sign == تہی)
		واپس; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (چوڑائی  - SIGN_چوڑائی)  / 2;
    yoffset = (اونچائی - SIGN_اونچائی) / 2;

# اگر GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_چوڑائی, SIGN_اونچائی);
	bg_cr = cairo_create(bg_surf);
	cairo_سیٹ_ماخذ_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_چوڑائی, SIGN_اونچائی);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_سیٹ_ماخذ_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_سیٹ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_سیٹ_ماخذ_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_سیٹ_ماخذ_surface(cr, sign_surf, FILL_X(ستون), FILL_Y(صف));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	قطار_draw_area(
		FILL_X(ستون), FILL_Y(ستون), چوڑائی, اونچائی);

    }
# ورنہ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->کھڑکی_2,
	    gui.متن_gc,
	    TRUE,
	    FILL_X(ستون),
	    FILL_Y(صف),
	    SIGN_چوڑائی,
	    SIGN_اونچائی);

    gdk_pixbuf_render_مقصد_drawable_alpha(sign,
	    gui.drawarea->کھڑکی_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(ستون) - MIN(0, xoffset),
	    FILL_Y(صف) - MIN(0, yoffset),
	    MIN(چوڑائی,	SIGN_چوڑائی),
	    MIN(اونچائی, SIGN_اونچائی),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_معمول,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    اگر (need_scale)
	g_object_unref(sign);
}

    خالی *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    اگر (signfile[0] != NUL && signfile[0] != '-' && gui.اندر_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = تہی;
	char_u	    *پیغام_2;

	sign = gdk_pixbuf_new_from_file((ثابت حرف *)signfile, &error);

	اگر (error == تہی)
	    واپس sign;

	پیغام_2 = (char_u *)error->پیغام_2;

	اگر (پیغام_2 != تہی && داخلہ_conv.vc_type != CONV_NONE)
	    پیغام_2 = string_convert(&داخلہ_conv, پیغام_2, تہی);

	اگر (پیغام_2 != تہی)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", پیغام_2);

	    اگر (داخلہ_conv.vc_type != CONV_NONE)
		vim_free(پیغام_2);
	}
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

#اگر_ختم // FEAT_SIGN_ICONS

#اگر defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

قسمتعریف ساخت
{
    cairo_surface_t *surf;
} image_cairo_T;

/*
 * Convert the popup's RGB / RGBA pixel buffer into the cached cairo surface's
 * pixel layout.  Cairo's CAIRO_FORMAT_ARGB32 / RGB24 wants native-endian uint32
 * quartets shaped as 0xAARRGGBB, with ARGB32 pixels premultiplied; we build the
 * integer per pixel so the resulting bytes come out correct on either
 * endianness.
 */
    ساکن خالی
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->ڈیٹا;
    uint8_t *dst;
    صحیح	    w = img->چوڑائی;
    صحیح	    h = img->اونچائی;
    صحیح	    stride;
    صحیح	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_ڈیٹا(surf);
    stride = cairo_image_surface_get_stride(surf);

    اگر (img->fmt == IMAGE_FORMAT_RGBA)
    {
	برائے (y = 0; y < h; ++y)
	{
	    غیرعلامتی صحیح *p = (غیرعلامتی صحیح *)(dst + y * stride);

	    برائے (x = 0; x < w; ++x)
	    {
		صحیح	idx = (y * w + x) * 4;
		uint8_t r = src[idx + 0];
		uint8_t g = src[idx + 1];
		uint8_t b = src[idx + 2];
		uint8_t a = src[idx + 3];

		// Premultiply alpha for ARGB32.
		r = (r * a + 127) / 255;
		g = (g * a + 127) / 255;
		b = (b * a + 127) / 255;
		p[x] = ((int_u)a << 24) | ((int_u)r << 16) | ((int_u)g << 8) | b;
	    }
	}
    }
    ورنہ
    {
	برائے (y = 0; y < h; ++y)
	{
	    غیرعلامتی صحیح *p = (غیرعلامتی صحیح *)(dst + y * stride);

	    برائے (x = 0; x < w; ++x)
	    {
		صحیح	idx = (y * w + x) * 3;
		uint8_t r = src[idx + 0];
		uint8_t g = src[idx + 1];
		uint8_t b = src[idx + 2];

		// RGB24's high byte is unused but conventionally 0xff.
		p[x] = (0xffu << 24) |
		    ((int_u)r << 16) | ((int_u)g << 8) | (int_u)b;
	    }
	}
    }
    cairo_surface_mark_dirty(surf);
}

    صحیح
image_gui_init(image_T *img)
{
    // We only support GTK3
# اگر GTK_CHECK_نسخہ(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    اگر (ctx == تہی)
	واپس FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->چوڑائی, img->اونچائی);

    اگر (cairo_surface_حالت(ctx->surf) != CAIRO_حالت_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	جاؤ fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_ڈیٹا = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    واپس OK;
fail:
    vim_free(ctx);
    واپس FAIL;
# ورنہ
    واپس FAIL;
# endif
}

    خالی
image_gui_uninit(image_T *img)
{
# اگر GTK_CHECK_نسخہ(3,0,0)
    image_cairo_T *ctx = img->backend_ڈیٹا;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    صحیح
image_placement_gui_init(image_placement_T *رکھو غیر_مستعمل)
{
# اگر GTK_CHECK_نسخہ(3,0,0)
    واپس OK;
# ورنہ
    واپس FAIL;
# endif
}

    خالی
image_placement_gui_uninit(image_placement_T *رکھو غیر_مستعمل)
{
}

    خالی
image_placement_gui_draw(image_placement_T *رکھو)
{
# اگر GTK_CHECK_نسخہ(3,0,0)
    image_T		    *img = رکھو->img;
    image_cairo_T	    *ictx = img->backend_ڈیٹا;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    اگر (rects == تہی || n_rects == 0)
	واپس;

    cairo_surface_سیٹ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    برائے (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		صف, ستون;
	صحیح		posx, posy;
	صحیح		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(رکھو, rect, &صف, &ستون, &x, &y, &w, &h, false);
	posx = FILL_X(ستون);
	posy = FILL_Y(صف);

	cairo_save(cr);
	cairo_سیٹ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_سیٹ_ماخذ_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	قطار_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    خالی
image_placement_gui_clear(image_placement_T *رکھو غیر_مستعمل)
{
}

#اگر_ختم // FEAT_IMAGE
