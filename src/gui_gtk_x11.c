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

#ضمن "vim.h"
#که_تعریف_شوی USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GNOME
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
# إذا defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
خارجي فراغ bonobo_dock_توکی_سېټ_behavior(Bonobo_Dock_توکی *dock_توکی, Bonobo_Dock_توکی_Behavior beh);
#د_که_پای

#إذا defined(FEAT_GUI_GTK)
# إذا GTK_CHECK_نسخه(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# وإلا
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# وإلا
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#د_که_پای

#که_تعریف_شوی HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#د_که_پای

/*
 * Easy-to-use macro for multihead support.
 */
#تعریف_کړه GET_X_ATOM(atom)	gdk_x11_atom_منزل_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
تعداد
{
    موخه_TYPE_NONE,
    موخه_UTF8_STRING,
    موخه_STRING,
    موخه_COMPOUND_متن,
    موخه_HTML,
    موخه_متن,
    موخه_متن_URI_LIST,
    موخه_متن_PLAIN,
    موخه_متن_PLAIN_UTF8,
    موخه_VIM_MT,
    موخه_VIM,
    موخه_VIMENC_MT,
    موخه_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_موخه_ثبت selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, موخه_VIMENC},
    {VIM_ATOM_NAME,	0, موخه_VIM},
    {VIMENC_MIMETYPE_NAME,  0, موخه_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, موخه_VIM_MT},
    {"text/html",	0, موخه_HTML},
    {"UTF8_STRING",	0, موخه_UTF8_STRING},
    {"COMPOUND_TEXT",	0, موخه_COMPOUND_متن},
    {"TEXT",		0, موخه_متن},
    {"STRING",		0, موخه_STRING},
    {"text/plain;charset=utf-8", 0, موخه_متن_PLAIN_UTF8},
    {"text/plain",	0, موخه_متن_PLAIN}
};
#تعریف_کړه N_SELECTION_TARGETS ARRAY_اوږدتیا(selection_targets)

#که_تعریف_شوی FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_موخه_ثبت dnd_targets[] =
{
    {"text/uri-list",	0, موخه_متن_URI_LIST},
    {"text/html",	0, موخه_HTML},
    {"UTF8_STRING",	0, موخه_UTF8_STRING},
    {"STRING",		0, موخه_STRING},
    {"text/plain",	0, موخه_متن_PLAIN}
};
# define N_DND_TARGETS ARRAY_اوږدتیا(dnd_targets)
#د_که_پای


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#تعریف_کړه DEFAULT_FONT	"Monospace 12"

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#د_که_پای

#إذا !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ساكن GdkAtom wm_protocols_atom = GDK_NONE;
ساكن GdkAtom save_yourself_atom = GDK_NONE;
#د_که_پای

/*
 * Atoms used to control/reference X11 selections.
 */
ساكن GdkAtom html_atom = GDK_NONE;
ساكن GdkAtom utf8_string_atom = GDK_NONE;
ساكن GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
ساكن GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
ساكن GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
ساكن GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
ساكن بنية special_کلی
{
    guint کلی_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیګانې[] =
{
    {GDK_پورته,		'k', 'u'},
    {GDK_ښکته,		'k', 'd'},
    {GDK_کیڼ,		'k', 'l'},
    {GDK_ښی,		'k', 'r'},
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
#که_تعریف_شوی SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#د_که_پای
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_شا_Space,	'k', 'b'},
    {GDK_ورننباسه,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_شا_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_پای_ټکی,		'@', '7'},
    {GDK_مخکینی,		'k', 'P'},
    {GDK_بل,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_کیڼ,	'k', 'l'},
    {GDK_KP_ښی,	'k', 'r'},
    {GDK_KP_پورته,		'k', 'u'},
    {GDK_KP_ښکته,	'k', 'd'},
    {GDK_KP_ورننباسه,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_پای_ټکی,	'K', '4'},
    {GDK_KP_مخکینی,	'K', '3'},  // page up
    {GDK_KP_بل,	'K', '5'},  // page down

    {GDK_KP_جمع_کړه,	'K', '6'},
    {GDK_KP_کمول,	'K', '7'},
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
#تعریف_کړه ARG_FONT	1
#تعریف_کړه ARG_GEOMETRY	2
#تعریف_کړه ARG_سرچپه	3
#تعریف_کړه ARG_NOREVERSE	4
#تعریف_کړه ARG_BACKGROUND	5
#تعریف_کړه ARG_FOREGROUND	6
#تعریف_کړه ARG_ICONIC	7
#تعریف_کړه ARG_ROLE	8
#تعریف_کړه ARG_NETBEANS	9
#تعریف_کړه ARG_XRM		10	// ignored
#تعریف_کړه ARG_MENUFONT	11	// ignored
#تعریف_کړه ARG_شاخص_ماسک	0x00ff
#تعریف_کړه ARG_HAS_ارزښت	0x0100	// a value is expected after the argument
#تعریف_کړه ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#تعریف_کړه ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#تعریف_کړه ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#تعریف_کړه ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
تعريفنوع بنية
{
    ثابت حرف	    *name;
    بینښه صحيح    flags;
}
cmdline_option_T;

ساكن ثابت cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_ارزښت},
    {"-font",		ARG_FONT|ARG_HAS_ارزښت},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_ارزښت},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_ارزښت},
    {"-rv",		ARG_سرچپه},
    {"-reverse",	ARG_سرچپه},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_ارزښت},
    {"-background",	ARG_BACKGROUND|ARG_HAS_ارزښت},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_ارزښت},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_ارزښت},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_ارزښت},
#که_تعریف_شوی FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_ارزښت},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_ارزښت},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_ارزښت},	// not implemented
#د_که_پای
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_ارزښت|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_ارزښت|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_ارزښت|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_ارزښت},
#که_تعریف_شوی FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_ارزښت},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# إذا 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#د_که_پای
    {عدم, 0}
};

ساكن صحيح    gui_argc = 0;
ساكن حرف **gui_argv = عدم;

ساكن ثابت حرف *role_دلیل = عدم;
#إذا defined(USE_GNOME_SESSION)
ساكن ثابت حرف *restart_command = عدم;
ساكن       حرف *abs_restart_command = عدم;
#د_که_پای
ساكن صحيح وموندل_iconic_arg = FALSE;

#که_تعریف_شوی FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ساكن صحيح using_gnome = 0;
#وإلا
# define using_gnome 0
#د_که_پای

// Comment out the following line to ignore code for resize history tracking.
#تعریف_کړه TRACK_RESIZE_HISTORY
#که_تعریف_شوی TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# إذا 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
تعريفنوع بنية resize_history {
    صحيح کارول_شوی;		// If true, can't match for discard. Only matches once.
    صحيح پلنوالی;
    صحيح لوړوالی;
    صحيح from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحيح seq;		// for ch_log messages
# endif
    بنية resize_history *بل;
} resize_hist_T;

// never NULL during execution
ساكن resize_hist_T *latest_resize_hist;
// list of stale resize requests
ساكن resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ساكن صحيح mch_csd_پلنوالی = 0;
ساكن صحيح mch_csd_لوړوالی = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
ساكن صحيح mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    ساكن فراغ
alloc_resize_hist(صحيح پلنوالی, صحيح لوړوالی, صحيح from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->پلنوالی = پلنوالی;
    new_hist->لوړوالی = لوړوالی;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->بل = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(عدم, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, پلنوالی, لوړوالی, (صحيح)Columns, (صحيح)لیکې);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    ساكن فراغ
clear_resize_hists(فراغ)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحيح		    i = 0;
# endif

    إذا (latest_resize_hist)
	latest_resize_hist->کارول_شوی = TRUE;
    طالما (old_resize_hists != عدم)
    {
	resize_hist_T *بل_hist = old_resize_hists->بل;

	vim_free(old_resize_hists);
	old_resize_hists = بل_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define سمون_پلنوالی_لوړوالی(hist, w, h) \
	  (!hist->کارول_شوی && hist->پلنوالی == w && hist->لوړوالی == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ساكن صحيح
سمون_stale_پلنوالی_لوړوالی(صحيح پلنوالی, صحيح لوړوالی)
{
    resize_hist_T *hist = old_resize_hists;

    لكل (hist = old_resize_hists; hist != عدم; hist = hist->بل)
	إذا (سمون_پلنوالی_لوړوالی(hist, پلنوالی, لوړوالی))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(عدم, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->کارول_شوی = TRUE;
	    أرجع TRUE;
	}
    أرجع FALSE;
}

# إذا defined(EXITFREE)
    ساكن فراغ
free_all_resize_hist(فراغ)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#د_که_پای

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
ساكن guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    فراغ
gui_mch_prepare(صحيح *argc, حرف **argv)
{
    ثابت cmdline_option_T  *option;
    صحيح			    i	= 0;
    صحيح			    len = 0;

#إذا defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    إذا (strchr(argv[0], G_DIR_SEPARATOR) != عدم)
    {
	char_u buf[MAXPATHL];

	إذا (mch_ډک_Name((char_u *)argv[0], buf, (صحيح)حجم(buf), TRUE) == OK)
	{
	    abs_restart_command = (حرف *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#د_که_پای

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(حرف *, *argc + 1);

    g_return_if_fail(gui_argv != عدم);

    gui_argv[gui_argc++] = argv[i++];

    طالما (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	إذا (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    استمر;
	}

	// Look for argv[i] in cmdline_options[] table.
	لكل (option = &cmdline_options[0]; option->name != عدم; ++option)
	{
	    len = مزي_اوږدوالی(option->name);

	    إذا (مزي_پرتله(argv[i], option->name, len) == 0)
	    {
		إذا (argv[i][len] == '\0')
		    اكسر;
		// allow --foo=bar style
		إذا (argv[i][len] == '=' && (option->flags & ARG_HAS_ارزښت))
		    اكسر;
#که_تعریف_شوی FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		إذا (vim_strchr((char_u *)":=", argv[i][len]) != عدم
			&& (option->flags & ARG_شاخص_ماسک) == ARG_NETBEANS)
		    اكسر;
#د_که_پای
	    }
	    وإلا إذا ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (حرف *)option->name;
		اكسر;
	    }
	}
	إذا (option->name == عدم) // no match
	{
	    ++i;
	    استمر;
	}

	إذا (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	وإلا
	{
	    حرف *ارزښت = عدم;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    إذا (option->flags & ARG_HAS_ارزښت)
	    {
		إذا (argv[i][len] == '=')
		    ارزښت = &argv[i][len + 1];
		وإلا إذا (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    ارزښت = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    بدل (option->flags & ARG_شاخص_ماسک)
	    {
		حالة ARG_سرچپه:
		    وموندل_سرچپه_arg = TRUE;
		    اكسر;
		حالة ARG_NOREVERSE:
		    وموندل_سرچپه_arg = FALSE;
		    اكسر;
		حالة ARG_FONT:
		    font_دلیل = ارزښت;
		    اكسر;
		حالة ARG_GEOMETRY:
		    إذا (ارزښت != عدم)
			gui.geom = vim_strsave((char_u *)ارزښت);
		    اكسر;
		حالة ARG_BACKGROUND:
		    background_دلیل = ارزښت;
		    اكسر;
		حالة ARG_FOREGROUND:
		    foreground_دلیل = ارزښت;
		    اكسر;
		حالة ARG_ICONIC:
		    وموندل_iconic_arg = TRUE;
		    اكسر;
		حالة ARG_ROLE:
		    role_دلیل = ارزښت; // used later in gui_mch_open()
		    اكسر;
#که_تعریف_شوی FEAT_NETBEANS_INTG
		حالة ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    اكسر;
#د_که_پای
		افتراضي:
		    اكسر;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	إذا ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	إذا (option->flags & ARG_KEEP)
	    ++i;
	وإلا
	{
	    // Remove the flag from the argument vector.
	    إذا (--*argc > i)
	    {
		صحيح n_strip = 1;

		// Move the argument's value as well, if there is one.
		إذا ((option->flags & ARG_HAS_ارزښت)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    إذا (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		إذا (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * حجم(حرف *));
	    }
	    argv[*argc] = عدم;
	}
    }

    gui_argv[gui_argc] = عدم;
}

#إذا defined(EXITFREE)
    فراغ
gui_mch_free_all(فراغ)
{
    vim_free(gui_argv);
# إذا defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#د_که_پای

#إذا !GTK_CHECK_نسخه(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ساكن gint
visibility_event(GtkWidget *widget ناکارول_شوی,
		 GdkEventVisibility *event,
		 gpointer معلومات ناکارول_شوی)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    إذا (gui.متن_gc != عدم)
	gdk_gc_سېټ_exposures(gui.متن_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    أرجع FALSE;
}
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن gboolean
draw_event(GtkWidget *widget ناکارول_شوی,
	   cairo_t   *cr,
	   gpointer   user_معلومات ناکارول_شوی)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    بهر_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_سېټ_سرچینه_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    أرجع FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# وإلا
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# إذا GTK_CHECK_نسخه(3,10,0)
    ساكن gboolean
scale_ضریب_event(GtkWidget *widget,
		   GParamSpec* pspec ناکارول_شوی,
		   gpointer   user_معلومات ناکارول_شوی)
{
    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    صحيح	    w, h;
    gtk_کړکۍ_get_اندازه(GTK_کړکۍ(gui.mainwin), &w, &h);
    gui.surface = gdk_کړکۍ_create_similar_surface(
	    gtk_widget_get_کړکۍ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    صحيح	    usable_لوړوالی = h;
    إذا (gtk_socket_id != 0)
	usable_لوړوالی -= (gui.char_لوړوالی - (gui.char_لوړوالی/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_لوړوالی);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخه(3,10,0)
    gui.scale = gtk_widget_get_scale_ضریب(widget);
#  endif

    أرجع TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
expose_event(GtkWidget *widget ناکارول_شوی,
	     GdkEventExpose *event,
	     gpointer معلومات ناکارول_شوی)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    بهر_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.پلنوالی, event->area.لوړوالی);

    // Clear the border areas if needed
    إذا (event->area.x < FILL_X(0))
	gdk_کړکۍ_clear_area(gui.drawarea->کړکۍ_2, 0, 0, FILL_X(0), 0);
    إذا (event->area.y < FILL_Y(0))
	gdk_کړکۍ_clear_area(gui.drawarea->کړکۍ_2, 0, 0, 0, FILL_Y(0));
    إذا (event->area.x > FILL_X(Columns))
	gdk_کړکۍ_clear_area(gui.drawarea->کړکۍ_2,
			      FILL_X((صحيح)Columns), 0, 0, 0);
    إذا (event->area.y > FILL_Y(لیکې))
	gdk_کړکۍ_clear_area(gui.drawarea->کړکۍ_2, 0, FILL_Y((صحيح)لیکې), 0, 0);

    أرجع FALSE;
}
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

#که_تعریف_شوی FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ساكن gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer معلومات ناکارول_شوی)
{
    إذا (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (صحيح)GDK_PROPERTY_NEW_ارزښت
	    && GDK_کړکۍ_XID(event->کړکۍ_2) == comm_کړکۍ
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.کړکۍ_2 = comm_کړکۍ;
	xev.xproperty.state = Property_New_ارزښت;
	serverEventProc(GDK_کړکۍ_XDISPLAY(gtk_widget_get_کړکۍ(widget)),
								      &xev, 0);
    }
    أرجع FALSE;
}
#د_که_پای // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ساكن فراغ
gtk_settings_xft_dpi_بدل_شوی_cb(GtkSettings *gtk_settings ناکارول_شوی,
				    GParamSpec *pspec ناکارول_شوی,
				    gpointer معلومات ناکارول_شوی)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سېټ_بنسټ_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? عدم : p_guifont, FALSE);
	gui_سېټ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

تعريفنوع gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ساكن guint
timeout_جمع_کړه(صحيح وخت, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_جمع_کړه((guint)وخت, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_سرچینه_remove(timer);
}


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#تعریف_کړه BLINK_NONE  0
#تعریف_کړه BLINK_OFF   1
#تعریف_کړه BLINK_ON    2

ساكن صحيح blink_state = BLINK_NONE;
ساكن long_u blink_waittime = 700;
ساكن long_u blink_ontime = 400;
ساكن long_u blink_offtime = 250;
ساكن guint blink_timer = 0;

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
gui_mch_سېټ_blinking(اوږد waittime, اوږد on, اوږد off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    فراغ
gui_mch_stop_blink(صحيح may_call_gui_update_cursor)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    إذا (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

    ساكن timeout_cb_type
blink_cb(gpointer معلومات ناکارول_شوی)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جمع_کړه(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جمع_کړه(blink_ontime, blink_cb, عدم);
    }
    gui_may_flush();

    أرجع FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_پیل_blink(فراغ)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.دننه_focus)
    {
	blink_timer = timeout_جمع_کړه(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ساكن gint
enter_notify_event(GtkWidget *widget ناکارول_شوی,
		   Gdk_Event_تېرېدنه *event ناکارول_شوی,
		   gpointer معلومات ناکارول_شوی)
{
    إذا (blink_state == BLINK_NONE)
	gui_mch_پیل_blink();

    // make sure keyboard input goes there
    إذا (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    أرجع FALSE;
}

    ساكن gint
leave_notify_event(GtkWidget *widget ناکارول_شوی,
		   Gdk_Event_تېرېدنه *event ناکارول_شوی,
		   gpointer معلومات ناکارول_شوی)
{
    إذا (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    أرجع FALSE;
}

    ساكن gint
focus_دننه_event(GtkWidget *widget,
	       GdkEventFocus *event ناکارول_شوی,
	       gpointer معلومات ناکارول_شوی)
{
#که_تعریف_شوی FEAT_GUI_DIALOG
    إذا (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    وإلا
#د_که_پای
	gui_focus_change(TRUE);

    إذا (blink_state == BLINK_NONE)
	gui_mch_پیل_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    إذا (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    أرجع TRUE;
}

    ساكن gint
focus_بهر_event(GtkWidget *widget ناکارول_شوی,
		GdkEventFocus *event ناکارول_شوی,
		gpointer معلومات ناکارول_شوی)
{
#که_تعریف_شوی FEAT_GUI_DIALOG
    إذا (gui.dialogs_فعال > 0)
	++gui.dialog_focus_pending;
#د_که_پای

    gui_focus_change(FALSE);

    إذا (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    أرجع TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    ساكن صحيح
keyval_منزل_string(بینښه صحيح keyval, char_u *string)
{
    صحيح	    len;
    guint32 uc;

    uc = gdk_keyval_منزل_unicode(keyval);
    إذا (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((صحيح)uc, string);
    }
    وإلا
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	بدل (keyval)
	{
	    حالة GDK_Tab: حالة GDK_KP_Tab: حالة GDK_ISO_کیڼ_Tab:
		string[0] = TAB;
		اكسر;
	    حالة GDK_Linefeed:
		string[0] = NL;
		اكسر;
	    حالة GDK_Return: حالة GDK_ISO_Enter: حالة GDK_3270_Enter:
		string[0] = CAR;
		اكسر;
	    حالة GDK_Escape:
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

    إذا (state & GDK_ښويدنه_ماسک)
	modifiers |= MOD_ماسک_ښويدنه;
    إذا (state & GDK_CONTROL_ماسک)
	modifiers |= MOD_ماسک_CTRL;
    إذا (state & GDK_MOD1_ماسک)
	modifiers |= MOD_ماسک_ALT;
#إذا GTK_CHECK_نسخه(2,10,0)
    إذا (state & GDK_META_ماسک)
	modifiers |= MOD_ماسک_META;
    إذا (state & GDK_SUPER_ماسک)
	modifiers |= MOD_ماسک_CMD;
#وإلا
    إذا (state & GDK_MOD4_ماسک)
	modifiers |= MOD_ماسک_CMD;
#د_که_پای

    أرجع modifiers;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_ښويدنه_ماسک)
	modifiers |= MOUSE_ښويدنه;
    إذا (state & GDK_CONTROL_ماسک)
	modifiers |= MOUSE_CTRL;
    إذا (state & GDK_MOD1_ماسک)
	modifiers |= MOUSE_ALT;

    أرجع modifiers;
}

/*
 * Main keyboard handler:
 */
    ساكن gint
کلی_press_event(GtkWidget *widget ناکارول_شوی,
		Gdk_Event_کلی *event,
		gpointer معلومات ناکارول_شوی)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	کلی_sym;
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		کلی;
    guint	state;
    char_u	*s, *d;
    صحيح		ctrl_مختاړی_added = 0;

    gui.event_وخت = event->وخت;
    کلی_sym = event->keyval;
    state = event->state;

#که_تعریف_شوی FEAT_XIM
    إذا (xim_کتار_کلی_press_event(event, TRUE))
	أرجع TRUE;
#د_که_پای

#که_تعریف_شوی SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    إذا (کلی_sym == SunXK_F36 || کلی_sym == SunXK_F37)
	len = 0;
    وإلا
#د_که_پای
    {
	len = keyval_منزل_string(کلی_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	إذا (len > 1 && ننوت_conv.vc_type != CONV_NONE)
	    len = convert_ننوت(string2, len, حجم(string2));

	s = string2;
	d = string;
	لكل (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    إذا (d[-1] == CSI && d + 2 < string + حجم(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (صحيح)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    إذا (کلی_sym == GDK_ISO_کیڼ_Tab)
    {
	کلی_sym = GDK_Tab;
	state |= GDK_ښويدنه_ماسک;
    }

#که_تعریف_شوی FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    إذا ((state & GDK_MOD1_ماسک)
	    && gui.menu_is_فعال
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	أرجع FALSE;
#د_که_پای

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیګانې[i].کلی_sym != 0; i++)
	{
	    إذا (special_کلیګانې[i].کلی_sym == کلی_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیګانې[i].code0;
		string[2] = special_کلیګانې[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

#که_تعریف_شوی GDK_کلی_dead_circumflex
    // Belgian Ctrl+[ workaround
    إذا (len == 0 && کلی_sym == GDK_کلی_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_ماسک_CTRL;
	string[3] = '[';
	len = 4;
	جمع_کړه_منزل_ننوت_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	أرجع TRUE;
    }
#د_که_پای

    إذا (len == 0)   // Unrecognized key
	أرجع TRUE;

    // For some keys a shift modifier is translated into another key code.
    إذا (len == -3)
	کلی = منزل_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	کلی = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    کلی = simplify_کلی(کلی, &modifiers);
    إذا (کلی == CSI)
	کلی = K_CSI;
    إذا (IS_SPECIAL(کلی))
    {
	string[0] = CSI;
	string[1] = K_دوهم(کلی);
	string[2] = K_دریم(کلی);
	len = 3;
    }
    وإلا
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	کلی = may_adjust_کلی_for_ctrl(modifiers, کلی);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_ښويدنه_modifier(modifiers, کلی);

	len = mb_char2bytes(کلی, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جمع_کړه_منزل_ننوت_buf(string2, 3);
	إذا (modifiers == 0x4)
	    ctrl_مختاړی_added = 1;
    }

    // Check if the key interrupts.
    {
	صحيح int_ch = check_for_مداخله(کلی, modifiers);

	إذا (int_ch != NUL)
	{
	    trash_ننوت_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    إذا (ctrl_مختاړی_added && len == 2
		    && ((صحيح)string[0]) == 195
		    && ((صحيح)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    جمع_کړه_منزل_ننوت_buf(string, len);

    // blank out the pointer if necessary
    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

#إذا defined(FEAT_XIM) || GTK_CHECK_نسخه(3,0,0)
    ساكن gboolean
کلی_release_event(GtkWidget *widget ناکارول_شوی,
		  Gdk_Event_کلی *event ناکارول_شوی,
		  gpointer معلومات ناکارول_شوی)
{
# إذا defined(FEAT_XIM)
    gui.event_وخت = event->وخت;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    أرجع xim_کتار_کلی_press_event(event, FALSE);
# وإلا
    أرجع TRUE;
# endif
}
#د_که_پای


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ساكن صحيح دننه_selection_clear_event = FALSE;

    ساكن gint
selection_clear_event(GtkWidget		*widget ناکارول_شوی,
		      GdkEventSelection	*event,
		      gpointer		user_معلومات ناکارول_شوی)
{
    دننه_selection_clear_event = TRUE;
    إذا (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    وإلا
	clip_lose_selection(&clip_ستوری);
    دننه_selection_clear_event = FALSE;

    أرجع TRUE;
}

#تعریف_کړه RS_NONE	0	// selection_received_cb() not called yet
#تعریف_کړه RS_OK	1	// selection_received_cb() called and OK
#تعریف_کړه RS_FAIL	2	// selection_received_cb() called and failed
ساكن صحيح received_selection = RS_NONE;

    ساكن فراغ
selection_received_cb(GtkWidget		*widget ناکارول_شوی,
		      Gtk_Selection_معلومات	*معلومات,
		      guint		وخت_2 ناکارول_شوی,
		      gpointer		user_معلومات ناکارول_شوی)
{
    Clipboard_T	    *cbd;
    char_u	    *متن;
    char_u	    *tmpbuf = عدم;
    guchar	    *tmpbuf_utf8 = عدم;
    صحيح		    len;
    صحيح		    motion_type = MAUTO;

    إذا (gtk_selection_معلومات_get_selection(معلومات) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_ستوری;

    متن = (char_u *)gtk_selection_معلومات_get_معلومات(معلومات);
    len = gtk_selection_معلومات_get_اوږدتیا(معلومات);

    إذا (متن == عدم || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	أرجع;
    }

    إذا (gtk_selection_معلومات_get_معلومات_type(معلومات) == vim_atom
	    || gtk_selection_معلومات_get_معلومات_type(معلومات) == vim_mt_atom)
    {
	motion_type = *متن++;
	--len;
    }
    وإلا إذا (gtk_selection_معلومات_get_معلومات_type(معلومات) == vimenc_atom
	    || gtk_selection_معلومات_get_معلومات_type(معلومات) == vimenc_mt_atom)
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
	إذا (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, متن, &len);
	    إذا (tmpbuf != عدم)
		متن = tmpbuf;
	    convert_تیاری(&conv, عدم, عدم);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    وإلا
    {
	tmpbuf_utf8 = gtk_selection_معلومات_get_متن(معلومات);
	إذا (tmpbuf_utf8 != عدم)
	{
	    len = STRLEN(tmpbuf_utf8);
	    إذا (ننوت_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&ننوت_conv, tmpbuf_utf8, &len);
		إذا (tmpbuf != عدم)
		    متن = tmpbuf;
	    }
	    وإلا
		متن = tmpbuf_utf8;
	}
	وإلا إذا (len >= 2 && متن[0] == 0xff && متن[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_تیاری_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    إذا (conv.vc_type != CONV_NONE)
	    {
		متن += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, متن, &len);
		convert_تیاری(&conv, عدم, عدم);
	    }
	    إذا (tmpbuf != عدم)
		متن = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    طالما (len > 0 && متن[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, متن, (اوږد)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ساكن فراغ
selection_get_cb(GtkWidget	    *widget ناکارول_شوی,
		 Gtk_Selection_معلومات   *selection_معلومات,
		 guint		    info,
		 guint		    وخت_2 ناکارول_شوی,
		 gpointer	    user_معلومات ناکارول_شوی)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    صحيح		    اوږدتیا;
    صحيح		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    إذا (gtk_selection_معلومات_get_selection(selection_معلومات)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_ستوری;

    إذا (!cbd->owned)
	أرجع;			// Shouldn't ever happen

    إذا (info != (guint)موخه_STRING
	    && (!clip_html || info != (guint)موخه_HTML)
	    && info != (guint)موخه_UTF8_STRING
	    && info != (guint)موخه_VIMENC
	    && info != (guint)موخه_VIM
	    && info != (guint)موخه_VIMENC_MT
	    && info != (guint)موخه_VIM_MT
	    && info != (guint)موخه_COMPOUND_متن
	    && info != (guint)موخه_متن_PLAIN
	    && info != (guint)موخه_متن_PLAIN_UTF8
	    && info != (guint)موخه_متن)
	أرجع;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    إذا (motion_type < 0 || string == عدم)
	أرجع;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    اوږدتیا = MIN(tmplen, (long_u)(G_MAXINT - 1));

    إذا (info == (guint)موخه_VIM || info == (guint)موخه_VIM_MT)
    {
	tmpbuf = alloc(اوږدتیا + 1);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)اوږدتیا);
	}
	// For our own format, the first byte contains the motion type
	++اوږدتیا;
	vim_free(string);
	string = tmpbuf;
	إذا (info == (guint)موخه_VIM)
	    type = vim_atom;
	وإلا
	    type = vim_mt_atom;
    }

    وإلا إذا (info == (guint)موخه_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_تیاری_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	إذا (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &اوږدتیا);
	    convert_تیاری(&conv, عدم, عدم);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	إذا (string != عدم)
	{
	    tmpbuf = alloc(اوږدتیا + 2);
	    إذا (tmpbuf != عدم)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)اوږدتیا);
		vim_free(string);
		string = tmpbuf;
		اوږدتیا += 2;
	    }

#إذا !GTK_CHECK_نسخه(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_معلومات->type = selection_معلومات->موخه;
	    selection_معلومات->format = 16;	// 16 bits per char
#د_که_پای
	    gtk_selection_معلومات_سېټ(selection_معلومات, html_atom, 16,
							      string, اوږدتیا);
	    vim_free(string);
	}
	أرجع;
    }
    وإلا إذا (info == (guint)موخه_VIMENC || info == (guint)موخه_VIMENC_MT)
    {
	صحيح l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(اوږدتیا + l + 2);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)اوږدتیا);
	    اوږدتیا += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	إذا (info == (guint)موخه_VIMENC)
	    type = vimenc_atom;
	وإلا
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    وإلا
    {
	إذا (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &اوږدتیا);
	    vim_free(string);
	    إذا (tmpbuf == عدم)
		أرجع;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	إذا (g_utf8_validate((ثابت حرف *)string, (gssize)اوږدتیا, عدم))
	{
	    gtk_selection_معلومات_سېټ_متن(selection_معلومات,
					(ثابت حرف *)string, اوږدتیا);
	}
	vim_free(string);
	أرجع;
    }

    إذا (string != عدم)
    {
#إذا !GTK_CHECK_نسخه(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_معلومات->type = selection_معلومات->موخه;
	selection_معلومات->format = 8;	// 8 bits per char
#د_که_پای
	gtk_selection_معلومات_سېټ(selection_معلومات, type, 8, string, اوږدتیا);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    صحيح
gui_mch_early_init_check(صحيح give_پیغام)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    إذا ((p == عدم || *p == NUL) && (q == عدم || *q == NUL))
    {
	gui.dying = true;
	إذا (give_پیغام)
	    emsg(_((حرف *)e_cannot_open_display));
	أرجع FAIL;
    }
    أرجع OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    صحيح
gui_mch_init_check(فراغ)
{
#که_تعریف_شوی USE_GRESOURCE
    ساكن صحيح res_registered = FALSE;

    إذا (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_ثبتوونکی_resource();
    }
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GNOME
    إذا (gtk_socket_id == 0)
	using_gnome = 1;
#د_که_پای

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_سېټ_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    إذا (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((حرف *)e_cannot_open_display));
	أرجع FAIL;
    }

    أرجع OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


ساكن guint mouse_click_timer = 0;
ساكن صحيح mouse_timed_بهر = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer معلومات)
{
    // we don't use this information currently
    صحيح *timed_بهر = (صحيح *) معلومات;

    *timed_بهر = TRUE;
    أرجع FALSE;		// don't happen again
}

ساكن guint		motion_repeat_timer  = 0;
ساكن صحيح		motion_repeat_دننه_واټن = FALSE;
ساكن timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ساكن فراغ
process_motion_notify(صحيح x, صحيح y, GdkModifierType state)
{
    صحيح	    button;
    int_u   vim_modifiers;
    Gtk_ویش ویش;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_ماسک | GDK_BUTTON2_ماسک |
		       GDK_BUTTON3_ماسک | GDK_BUTTON4_ماسک |
		       GDK_BUTTON5_ماسک))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    إذا (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	أرجع;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_ویش(gui.drawarea, &ویش);

    إذا (x < 0 || y < 0
	    || x >= ویش.پلنوالی
	    || y >= ویش.لوړوالی)
    {

	صحيح dx;
	صحيح dy;
	صحيح offshoot;
	صحيح delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - ویش.پلنوالی;
	dy = y < 0 ? -y : y - ویش.لوړوالی;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	إذا (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	وإلا
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	إذا (!motion_repeat_timer)
	    motion_repeat_timer = timeout_جمع_کړه(delay, motion_repeat_timer_cb,
									 عدم);
    }
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن GdkDevice *
gui_gtk_get_اشاره_ګر_device(GtkWidget *widget)
{
    Gdk_کړکۍ * ثابت win = gtk_widget_get_کړکۍ(widget);
    GdkDisplay * ثابت dpy = gdk_کړکۍ_get_display(win);
# إذا GTK_CHECK_نسخه(3,20,0)
    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
    أرجع gdk_seat_get_اشاره_ګر(seat);
# وإلا
    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
    أرجع gdk_device_manager_get_client_اشاره_ګر(mngr);
# endif
}

    ساكن Gdk_کړکۍ *
gui_gtk_get_اشاره_ګر(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_کړکۍ * ثابت win = gtk_widget_get_کړکۍ(widget);
    GdkDevice * ثابت dev = gui_gtk_get_اشاره_ګر_device(widget);
    أرجع gdk_کړکۍ_get_device_position(win, dev , x, y, state);
}

# إذا defined(FEAT_GUI_TABLINE)
    ساكن Gdk_کړکۍ *
gui_gtk_کړکۍ_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ثابت dev = gui_gtk_get_اشاره_ګر_device(widget);
    أرجع gdk_device_get_کړکۍ_at_position(dev, x, y);
}
# endif
#وإلا // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_اشاره_ګر(wid, x, y, s) \
    gdk_کړکۍ_get_اشاره_ګر((wid)->کړکۍ_2, x, y, s)
# define gui_gtk_کړکۍ_at_position(wid, x, y)	gdk_کړکۍ_at_اشاره_ګر(x, y)
#د_که_پای

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ساكن timeout_cb_type
motion_repeat_timer_cb(gpointer معلومات ناکارول_شوی)
{
    صحيح		    x;
    صحيح		    y;
    GdkModifierType state;

    gui_gtk_get_اشاره_ګر(gui.drawarea, &x, &y, &state);

    إذا (!(state & (GDK_BUTTON1_ماسک | GDK_BUTTON2_ماسک |
		   GDK_BUTTON3_ماسک | GDK_BUTTON4_ماسک |
		   GDK_BUTTON5_ماسک)))
    {
	motion_repeat_timer = 0;
	أرجع FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_کارول_شوی_دننه_ننوت_buf() > 10)
	أرجع TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    إذا (motion_repeat_دننه_واټن)
	x += gui.char_پلنوالی;

    motion_repeat_دننه_واټن = !motion_repeat_دننه_واټن;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    أرجع FALSE;
}

    ساكن gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer معلومات ناکارول_شوی)
{
    إذا (event->is_hint)
    {
	صحيح		x;
	صحيح		y;
	GdkModifierType	state;

	gui_gtk_get_اشاره_ګر(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    وإلا
    {
	process_motion_notify((صحيح)event->x, (صحيح)event->y,
			      (GdkModifierType)event->state);
    }

    أرجع TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    ساكن gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer معلومات ناکارول_شوی)
{
    صحيح button;
    صحيح repeated_click = FALSE;
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_وخت = event->وخت;

    // Make sure we have focus now we've been selected
    إذا (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    إذا (event->type != GDK_BUTTON_PRESS)
	أرجع FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    إذا (!mouse_timed_بهر && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_بهر = FALSE;
    mouse_click_timer = timeout_جمع_کړه(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_بهر);

    بدل (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	حالة 1:
		button = MOUSE_کیڼ;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_ماسک;
		اكسر;
	حالة 2: button = MOUSE_منځ; اكسر;
	حالة 3: button = MOUSE_ښی; اكسر;
	حالة 8: button = MOUSE_X1; اكسر;
	حالة 9: button = MOUSE_X2; اكسر;
	افتراضي:
	    أرجع FALSE;		// Unknown button
    }

#که_تعریف_شوی FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#د_که_پای

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    أرجع TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    ساكن gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer معلومات ناکارول_شوی)
{
    صحيح	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#إذا GTK_CHECK_نسخه(3,4,0)
    ساكن مزدوج  acc_x, acc_y;
# إذا !GTK_CHECK_نسخه(3,22,0)
    ساكن guint32 وروستی_smooth_event_وخت;
# endif
#د_که_پای

    إذا (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    بدل (event->direction)
    {
	حالة GDK_SCROLL_پورته:
	    button = MOUSE_4;
	    اكسر;
	حالة GDK_SCROLL_ښکته:
	    button = MOUSE_5;
	    اكسر;
	حالة GDK_SCROLL_کیڼ:
	    button = MOUSE_7;
	    اكسر;
	حالة GDK_SCROLL_ښی:
	    button = MOUSE_6;
	    اكسر;
#إذا GTK_CHECK_نسخه(3,4,0)
	حالة GDK_SCROLL_SMOOTH:
	    إذا (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		أرجع FALSE;
	    }
# إذا GTK_CHECK_نسخه(3,22,0)
	    إذا (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# وإلا
	    إذا (event->وخت - وروستی_smooth_event_وخت > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# إذا !GTK_CHECK_نسخه(3,22,0)
	    وروستی_smooth_event_وخت = event->وخت;
# endif
	    اكسر;
#د_که_پای
	افتراضي: // This shouldn't happen
	    أرجع FALSE;
    }

#که_تعریف_شوی FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#د_که_پای

    vim_modifiers = modifiers_gdk2mouse(event->state);

#إذا GTK_CHECK_نسخه(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    إذا (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	طالما (acc_x >= 1.0)
	{ //ښی
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_x <= -1.0)
	{ //کیڼ
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y >= 1.0)
	{ //ښکته
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y <= -1.0)
	{ //پورته
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
    }
    وإلا
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    إذا (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    وإلا
#د_که_پای
	gui_send_mouse_event(button, (صحيح)event->x, (صحيح)event->y,
		FALSE, vim_modifiers);

    أرجع TRUE;
}


    ساكن gint
button_release_event(GtkWidget *widget ناکارول_شوی,
		     GdkEventButton *event,
		     gpointer معلومات ناکارول_شوی)
{
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_وخت = event->وخت;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    إذا (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    بدل (event->button)
    {
	حالة 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    اكسر;
    }

    أرجع TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    ساكن فراغ
grab_notify_event(GtkWidget *widget ناکارول_شوی,
		  gboolean  was_grabbed,
		  gpointer  معلومات ناکارول_شوی)
{
    إذا (!was_grabbed)
	dragging_button_state = 0;
}


#که_تعریف_شوی FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ساكن صحيح
شمېر_and_decode_uri_list(char_u *بهر, char_u *raw, صحيح len)
{
    صحيح		i;
    char_u	*p = بهر;
    صحيح		شمېر = 0;

    لكل (i = 0; i < len; ++i)
    {
	إذا (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    إذا (p > بهر && p[-1] != NUL)
	    {
		++شمېر;
		*p++ = NUL;
	    }
	}
	وإلا إذا (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	وإلا
	    *p++ = raw[i];
    }
    إذا (p > بهر && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++شمېر;
    }
    أرجع شمېر;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    ساكن صحيح
filter_uri_list(char_u **outlist, صحيح max, char_u *src)
{
    صحيح	i, j;

    لكل (i = j = 0; i < max; ++i)
    {
	outlist[i] = عدم;
	إذا (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    إذا (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    طالما (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    أرجع j;
}

    ساكن char_u **
parse_uri_list(صحيح *شمېر, char_u *معلومات, صحيح len)
{
    صحيح	    n	    = 0;
    char_u  *tmp    = عدم;
    char_u  **array = عدم;

    إذا (معلومات != عدم && len > 0 && (tmp = alloc(len + 1)) != عدم)
    {
	n = شمېر_and_decode_uri_list(tmp, معلومات, len);
	إذا (n > 0 && (array = ALLOC_MULT(char_u *, n)) != عدم)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *شمېر = n;
    أرجع array;
}

    ساكن فراغ
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_معلومات	*معلومات,
		     guint		وخت_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    صحيح	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_معلومات_get_معلومات(معلومات),
	    gtk_selection_معلومات_get_اوږدتیا(معلومات));

    إذا (fnames != عدم && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_پای(context, TRUE, FALSE, وخت_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    وإلا
	vim_free(fnames);
}

    ساكن فراغ
drag_handle_متن(GdkDragContext	    *context,
		 Gtk_Selection_معلومات   *معلومات,
		 guint		    وخت_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *متن;
    صحيح	    len;
    char_u  *tmpbuf = عدم;

    متن = (char_u *)gtk_selection_معلومات_get_معلومات(معلومات);
    len = gtk_selection_معلومات_get_اوږدتیا(معلومات);

    إذا (gtk_selection_معلومات_get_معلومات_type(معلومات) == utf8_string_atom)
    {
	إذا (ننوت_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&ننوت_conv, متن, &len);
	إذا (tmpbuf != عدم)
	    متن = tmpbuf;
    }

    dnd_yank_drag_معلومات(متن, (اوږد)len);
    gtk_drag_پای(context, TRUE, FALSE, وخت_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    إذا (dropkey[2] != 0)
	جمع_کړه_منزل_ننوت_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	جمع_کړه_منزل_ننوت_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ساكن فراغ
drag_معلومات_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_معلومات	*معلومات,
		      guint		info,
		      guint		وخت_2,
		      gpointer		user_معلومات ناکارول_شوی)
{
    GdkModifierType state;

    // Guard against trash
    ثابت guchar * ثابت معلومات_معلومات = gtk_selection_معلومات_get_معلومات(معلومات);
    ثابت gint معلومات_اوږدتیا = gtk_selection_معلومات_get_اوږدتیا(معلومات);
    ثابت gint معلومات_format = gtk_selection_معلومات_get_format(معلومات);

    إذا (معلومات_معلومات == عدم
	    || معلومات_اوږدتیا <= 0
	    || معلومات_format != 8
	    || معلومات_معلومات[معلومات_اوږدتیا] != '\0')
    {
	gtk_drag_پای(context, FALSE, FALSE, وخت_2);
	أرجع;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_اشاره_ګر(widget, عدم, عدم, &state);

    // Not sure about the role of "text/plain" here...
    إذا (info == (guint)موخه_متن_URI_LIST)
	drag_handle_uri_list(context, معلومات, وخت_2, state, x, y);
    وإلا
	drag_handle_متن(context, معلومات, وخت_2, state);

}
#د_که_پای // FEAT_DND


#إذا defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ساكن فراغ
sm_client_check_بدل_شوی_any(GnomeClient	    *client ناکارول_شوی,
			    gint	    کلی,
			    GnomeDialogType type ناکارول_شوی,
			    gpointer	    معلومات ناکارول_شوی)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# إذا defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_بدل_شوی_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    بهر_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_کلی_return(کلی, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    ساكن gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase ناکارول_شوی,
			GnomeSaveStyle	    save_style ناکارول_شوی,
			gboolean	    shutdown ناکارول_شوی,
			GnomeInteractStyle  interact_style,
			gboolean	    fast ناکارول_شوی,
			gpointer	    معلومات ناکارول_شوی)
{
    ساكن ثابت حرف	suffix[] = "-session.vim";
    حرف		*session_file;
    بینښه صحيح	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    إذا (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_نورمال,
					 &sm_client_check_بدل_شوی_any,
					 عدم);
    بهر_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_لار(
			gnome_client_get_config_مختاړی(client));
    len = مزي_اوږدوالی(session_file);

    إذا (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(حرف, session_file, len + حجم(suffix));
    حافظه_نقل(session_file + len, suffix, حجم(suffix));

    success = write_session_file((char_u *)session_file);

    إذا (success)
    {
	ثابت حرف  *argv[8];
	صحيح	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = عدم;

	gnome_client_سېټ_discard_command(client, i, (حرف **)argv);

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
	argv[i++] = gtk_کړکۍ_get_role(GTK_کړکۍ(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = عدم;

	gnome_client_سېټ_restart_command(client, i, (حرف **)argv);
	gnome_client_سېټ_clone_command(client, 0, عدم);
    }

    g_free(session_file);

    أرجع success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    ساكن فراغ
sm_client_die(GnomeClient *client ناکارول_شوی, gpointer معلومات ناکارول_شوی)
{
    // Don't write messages to the GUI anymore
    ډک_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    ساكن فراغ
تیاری_save_yourself(فراغ)
{
    GnomeClient *client;

    client = gnome_master_client();
    إذا (client == عدم)
	أرجع;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), عدم);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), عدم);
}

#وإلا // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    ساكن gboolean
local_xsmp_handle_غوښتنې(
    GIOChannel		*سرچینه ناکارول_شوی,
    GIOCondition	condition,
    gpointer		معلومات)
{
    إذا (condition == G_IO_دننه)
    {
	// Do stuff; maybe close connection
	إذا (xsmp_handle_غوښتنې() == FAIL)
	    g_io_channel_unref((GIOChannel *)معلومات);
	أرجع TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)معلومات);
    xsmp_close();
    أرجع TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    ساكن فراغ
تیاری_save_yourself(فراغ)
{
    Atom    *existing_atoms = عدم;
    صحيح	    شمېر = 0;

# ifdef USE_XSMP
    إذا (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_جمع_کړه_watch(g_io, G_IO_دننه | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_غوښتنې, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    وإلا
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	إذا (!dpy)
	    أرجع;

	Gdk_کړکۍ * ثابت mainwin_win = gtk_widget_get_کړکۍ(gui.mainwin);
	إذا (XGetWMProtocols(dpy, GDK_کړکۍ_XID(mainwin_win),
		    &existing_atoms, &شمېر))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    صحيح	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    لكل (i = 0; i < شمېر; ++i)
		إذا (existing_atoms[i] == save_yourself_xatom)
		    اكسر;

	    إذا (i == شمېر)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, شمېر + 1);
		إذا (new_atoms != عدم)
		{
		    حافظه_نقل(new_atoms, existing_atoms, شمېر * حجم(Atom));
		    new_atoms[شمېر] = save_yourself_xatom;
		    XSetWMProtocols(GDK_کړکۍ_XDISPLAY(mainwin_win),
			    GDK_کړکۍ_XID(mainwin_win),
			    new_atoms, شمېر + 1);
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
    ساكن GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event ناکارول_شوی,
		    gpointer معلومات ناکارول_شوی)
{
    XEvent *xevent = (XEvent *)xev;

    إذا (xevent != عدم
	    && xevent->type == Client_پیغام
	    && xevent->xclient.پیغام_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.معلومات.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	بهر_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_کړکۍ_XDISPLAY(gtk_widget_get_کړکۍ(gui.mainwin)),
		    GDK_کړکۍ_XID(gtk_widget_get_کړکۍ(gui.mainwin)),
		    عدم, 0);
	أرجع GDK_FILTER_REMOVE;
    }

    أرجع GDK_FILTER_CONTINUE;
}
#د_که_پای // !USE_GNOME_SESSION

    ساكن GdkPixbuf *
pixbuf_new_from_png_معلومات(ثابت بینښه حرف *معلومات, بینښه صحيح len)
{
    GInput_بهیر *بهیر;
    GdkPixbuf    *pixbuf;

    بهیر = g_حافظه_ننوت_بهیر_new_from_معلومات(معلومات, len, عدم);
    pixbuf = gdk_pixbuf_new_from_بهیر(بهیر, عدم, عدم);
    g_object_unref(بهیر);
    أرجع pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ساكن فراغ
mainwin_realize(GtkWidget *widget ناکارول_شوی, gpointer معلومات ناکارول_شوی)
{
#ضمن "../runtime/vim16x16_png.h"
#ضمن "../runtime/vim32x32_png.h"
#ضمن "../runtime/vim48x48_png.h"

    Gdk_کړکۍ * ثابت mainwin_win = gtk_widget_get_کړکۍ(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    إذا (echo_wid_arg)
    {
	إذا (gui_mch_get_display())
	    چاپ_کړه("WID: %ld\n", (اوږد)GDK_کړکۍ_XID(mainwin_win));
	وإلا
	    چاپ_کړه("WID: 0\n");
	fflush(stdout);
    }

    إذا (vim_strchr(p_go, GO_ICON) != عدم)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	إذا (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_کړکۍ_سېټ_icon_name(GTK_کړکۍ(gui.mainwin), "gvim");
	وإلا
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = عدم;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim48x48_png, vim48x48_png_len));

	    gtk_کړکۍ_سېټ_icon_list(GTK_کړکۍ(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(فراغ *)&g_object_unref, عدم);
	    g_list_free(icons);
	}
    }

#إذا !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_کړکۍ_جمع_کړه_filter(عدم, &global_event_filter, عدم);
#د_که_پای
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#إذا defined(USE_GNOME_SESSION)
    إذا (using_gnome)
#د_که_پای
	تیاری_save_yourself();

#که_تعریف_شوی FEAT_CLIENTSERVER
    إذا (clientserver_لاره == CLIENTSERVER_لاره_X11 && gui_mch_get_display())
    {
	إذا (serverName == عدم && server_Delayed_پیل_Name != عدم)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_کړکۍ = GDK_کړکۍ_XID(mainwin_win);

	    (فراغ)server_ثبتوونکی_Name(GDK_کړکۍ_XDISPLAY(mainwin_win),
				    server_Delayed_پیل_Name);
	}
	وإلا
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_کړکۍ(GDK_کړکۍ_XDISPLAY(mainwin_win),
					GDK_کړکۍ_XID(mainwin_win));
	}
	gtk_widget_جمع_کړه_events(gui.mainwin, GDK_PROPERTY_CHANGE_ماسک);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), عدم);
    }
#د_که_پای
}

    ساكن GdkCursor *
create_blank_اشاره_ګر(فراغ)
{
    GdkCursor	*cursor;

#إذا GTK_CHECK_نسخه(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#وإلا
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_کړکۍ	*ریښه_کړکۍ = عدم;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_ماسک;
    حرف	blank_معلومات[] = { 0x0 };
    ریښه_کړکۍ = gtk_widget_get_ریښه_کړکۍ(gui.mainwin);
    blank_ماسک = gdk_bitmap_create_from_معلومات(ریښه_کړکۍ, blank_معلومات, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_ماسک, blank_ماسک,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_ماسک);
#د_که_پای

    أرجع cursor;
}

    ساكن فراغ
mainwin_screen_بدل_شوی_cb(GtkWidget  *widget,
			  GdkScreen  *پخوانی_screen ناکارول_شوی,
			  gpointer   معلومات ناکارول_شوی)
{
    إذا (!gtk_widget_has_screen(widget))
	أرجع;

    /*
     * Recreate the invisible mouse cursor.
     */
    إذا (gui.blank_اشاره_ګر != عدم)
#إذا GTK_CHECK_نسخه(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_اشاره_ګر));
#وإلا
	gdk_cursor_unref(gui.blank_اشاره_ګر);
#د_که_پای

    gui.blank_اشاره_ګر = create_blank_اشاره_ګر();

    إذا (gui.اشاره_ګر_hidden && gtk_widget_get_کړکۍ(gui.drawarea) != عدم)
	gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(gui.drawarea),
		gui.blank_اشاره_ګر);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(widget);
    pango_context_سېټ_بنسټ_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_سېټ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ساكن gboolean
mainwin_state_event_cb(GtkWidget *widget ناکارول_شوی,
		       Gdk_Event_کړکۍ_State *event,
		       gpointer user_معلومات ناکارول_شوی)
{
    إذا (event->new_کړکۍ_state & GDK_کړکۍ_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    أرجع FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    ساكن فراغ
drawarea_realize_cb(GtkWidget *widget, gpointer معلومات ناکارول_شوی)
{
    GtkWidget *sbar;
    Gtk_ویش ویش;

#که_تعریف_شوی FEAT_XIM
    xim_init();
#د_که_پای
    gui_mch_new_colors();
#إذا GTK_CHECK_نسخه(3,0,0)
    gui.surface = gdk_کړکۍ_create_similar_surface(
	    gtk_widget_get_کړکۍ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_پلنوالی(widget),
	    gtk_widget_get_allocated_لوړوالی(widget));
#وإلا
    gui.متن_gc = gdk_gc_new(gui.drawarea->کړکۍ_2);
#د_که_پای

    gui.blank_اشاره_ګر = create_blank_اشاره_ګر();
    إذا (gui.اشاره_ګر_hidden)
	gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(widget), gui.blank_اشاره_ګر);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_کیڼ].id;
    إذا (!sbar || (!gui.which_scrollbars[SBAR_کیڼ]
				    && firstwin->w_scrollbars[SBAR_ښی].id))
	sbar = firstwin->w_scrollbars[SBAR_ښی].id;
    gtk_widget_get_ویش(sbar, &ویش);
    إذا (sbar && gtk_widget_get_realized(sbar) && ویش.پلنوالی)
	gui.scrollbar_پلنوالی = ویش.پلنوالی;

    sbar = gui.bottom_sbar.id;
    إذا (sbar && gtk_widget_get_realized(sbar) && ویش.لوړوالی)
	gui.scrollbar_لوړوالی = ویش.لوړوالی;
}

/*
 * Properly clean up on shutdown.
 */
    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget ناکارول_شوی, gpointer معلومات ناکارول_شوی)
{
    // Don't write messages to the GUI anymore
    ډک_screen = FALSE;

#که_تعریف_شوی FEAT_XIM
    im_shutdown();
#د_که_پای
    إذا (gui.ascii_glyphs != عدم)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = عدم;
    }
    إذا (gui.ascii_font != عدم)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = عدم;
    }
    g_object_unref(gui.متن_context);
    gui.متن_context = عدم;

#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (gui.surface != عدم)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = عدم;
    }
#وإلا
    g_object_unref(gui.متن_gc);
    gui.متن_gc = عدم;
#د_که_پای

#إذا GTK_CHECK_نسخه(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_اشاره_ګر));
#وإلا
    gdk_cursor_unref(gui.blank_اشاره_ګر);
#د_که_پای
    gui.blank_اشاره_ګر = عدم;
}

#إذا GTK_CHECK_نسخه(3,22,2)
    ساكن فراغ
drawarea_style_updated_cb(GtkWidget *widget ناکارول_شوی,
			 gpointer معلومات ناکارول_شوی)
#وإلا
    ساكن فراغ
drawarea_style_سېټ_cb(GtkWidget	*widget ناکارول_شوی,
		      GtkStyle	*پخوانی_style ناکارول_شوی,
		      gpointer	معلومات ناکارول_شوی)
#د_که_پای
{
    gui_mch_new_colors();
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       معلومات ناکارول_شوی)
{
    ساكن صحيح cur_پلنوالی = 0;
    ساكن صحيح cur_لوړوالی = 0;

    g_return_val_if_fail(event
	    && event->پلنوالی >= 1 && event->لوړوالی >= 1, TRUE);

# إذا GTK_CHECK_نسخه(3,22,2) && !GTK_CHECK_نسخه(3,22,4)
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
    إذا (event->send_event == FALSE)
	أرجع TRUE;
# endif

    إذا (event->پلنوالی == cur_پلنوالی && event->لوړوالی == cur_لوړوالی)
	أرجع TRUE;

    cur_پلنوالی = event->پلنوالی;
    cur_لوړوالی = event->لوړوالی;

    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_کړکۍ_create_similar_surface(
	    gtk_widget_get_کړکۍ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->پلنوالی, event->لوړوالی);

    gtk_widget_کتار_draw(widget);

    أرجع TRUE;
}
#د_که_پای

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ساكن gint
delete_event_cb(GtkWidget *widget ناکارول_شوی,
		GdkEventAny *event ناکارول_شوی,
		gpointer معلومات ناکارول_شوی)
{
    gui_shell_closed();
    أرجع TRUE;
}

#إذا defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساكن صحيح
get_توکی_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation توکی_orientation = GTK_ORIENTATION_HORIZONTAL;

    إذا (using_gnome && widget != عدم)
    {
	GtkWidget *پلار;
	Bonobo_Dock_توکی *dockitem;

	پلار	 = gtk_widget_get_پلار(widget);
	إذا (G_TYPE_FROM_INSTANCE(پلار) == BONOBO_TYPE_DOCK_توکی)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = پلار;
	    dockitem = BONOBO_DOCK_توکی(widget);

	    إذا (dockitem == عدم || dockitem->is_floating)
		أرجع 0;
	    توکی_orientation = bonobo_dock_توکی_get_orientation(dockitem);
	}
    }
# وإلا
#  define توکی_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    إذا (widget != عدم
	    && توکی_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# إذا GTK_CHECK_نسخه(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_ویش ویش;

	gtk_widget_get_ویش(widget, &ویش);
	إذا (ویش.لوړوالی > 1)
	    أرجع ویش.لوړوالی;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  إذا GTK_CHECK_نسخه(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_لوړوالی(widget, &min_h, &natural_h);
	    أرجع MAX(min_h, natural_h);
	}
#  وإلا
	أرجع ویش.لوړوالی;
#  endif
# وإلا
	إذا (orientation == GTK_ORIENTATION_HORIZONTAL)
	    أرجع widget->ویش.لوړوالی;
	وإلا
	    أرجع widget->ویش.پلنوالی;
# endif
    }
    أرجع 0;
}
#د_که_پای

    ساكن صحيح
get_menu_tool_پلنوالی(فراغ)
{
    صحيح پلنوالی = 0;

#که_تعریف_شوی FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    پلنوالی += get_توکی_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    پلنوالی += get_توکی_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    إذا (gui.tabline != عدم)
	پلنوالی += get_توکی_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#د_که_پای

    أرجع پلنوالی;
}

    ساكن صحيح
get_menu_tool_لوړوالی(فراغ)
{
    صحيح لوړوالی = 0;

#که_تعریف_شوی FEAT_MENU
    لوړوالی += get_توکی_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#د_که_پای
#که_تعریف_شوی FEAT_TOOLBAR
    لوړوالی += get_توکی_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#د_که_پای
#که_تعریف_شوی FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    إذا (gui.tabline != عدم
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	لوړوالی += get_توکی_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#د_که_پای

    أرجع لوړوالی;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ساكن صحيح init_کړکۍ_hints_state = 0;

    ساكن فراغ
update_کړکۍ_manager_hints(صحيح force_پلنوالی, صحيح force_لوړوالی)
{
    ساكن صحيح old_پلنوالی  = 0;
    ساكن صحيح old_لوړوالی = 0;
    ساكن صحيح old_min_پلنوالی  = 0;
    ساكن صحيح old_min_لوړوالی = 0;
    ساكن صحيح old_char_پلنوالی  = 0;
    ساكن صحيح old_char_لوړوالی = 0;

    صحيح پلنوالی;
    صحيح لوړوالی;
    صحيح min_پلنوالی;
    صحيح min_لوړوالی;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    إذا (!(force_پلنوالی && force_لوړوالی)  &&  init_کړکۍ_hints_state > 0)
    {
	// Don't do it!
	init_کړکۍ_hints_state = 2;
	أرجع;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    پلنوالی  = gui_get_بنسټ_پلنوالی();
    لوړوالی = gui_get_بنسټ_لوړوالی();
#که_تعریف_شوی FEAT_MENU
    لوړوالی += tabline_لوړوالی() * gui.char_لوړوالی;
#د_که_پای
    پلنوالی  += get_menu_tool_پلنوالی();
    لوړوالی += get_menu_tool_لوړوالی();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    إذا (force_پلنوالی && force_لوړوالی)
    {
	min_پلنوالی  = force_پلنوالی;
	min_لوړوالی = force_لوړوالی;
    }
    وإلا
    {
	min_پلنوالی  = پلنوالی  + MIN_COLUMNS * gui.char_پلنوالی;
	min_لوړوالی = لوړوالی + MIN_کرښې   * gui.char_لوړوالی;
    }

    // Avoid an expose event when the size didn't change.
    إذا (پلنوالی != old_پلنوالی
	    || لوړوالی != old_لوړوالی
	    || min_پلنوالی != old_min_پلنوالی
	    || min_لوړوالی != old_min_لوړوالی
	    || gui.char_پلنوالی != old_char_پلنوالی
	    || gui.char_لوړوالی != old_char_لوړوالی)
    {
	GdkGeometry	geometry;
	Gdk_کړکۍ_Hints	geometry_ماسک;

	geometry.پلنوالی_inc   = gui.char_پلنوالی;
	geometry.لوړوالی_inc  = gui.char_لوړوالی;
	geometry.بنسټ_پلنوالی  = پلنوالی;
	geometry.بنسټ_لوړوالی = لوړوالی;
	geometry.min_پلنوالی   = min_پلنوالی;
	geometry.min_لوړوالی  = min_لوړوالی;
	geometry_ماسک	     = GDK_HINT_بنسټ_اندازه|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_اندازه;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_کړکۍ_سېټ_geometry_hints(GTK_کړکۍ(gui.mainwin), عدم,
				      &geometry, geometry_ماسک);
	old_پلنوالی       = پلنوالی;
	old_لوړوالی      = لوړوالی;
	old_min_پلنوالی   = min_پلنوالی;
	old_min_لوړوالی  = min_لوړوالی;
	old_char_پلنوالی  = gui.char_پلنوالی;
	old_char_لوړوالی = gui.char_لوړوالی;
    }
}

#إذا defined(FEAT_GUI_DARKTHEME)
    فراغ
gui_mch_سېټ_dark_theme(صحيح dark)
{
# إذا GTK_CHECK_نسخه(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_سېټ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, عدم);
# endif
}
#د_که_پای // FEAT_GUI_DARKTHEME

#که_تعریف_شوی FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ساكن فراغ
icon_اندازه_بدل_شوی_foreach(GtkWidget *widget, gpointer user_معلومات)
{
    إذا (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# إذا GTK_CHECK_نسخه(3,10,0)
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ثابت Gtk_Icon_اندازه icon_اندازه = GPOINTER_منزل_INT(user_معلومات);
	    ثابت gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, عدم);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_اندازه);
	}
# وإلا
	// User-defined icons are stored in a GtkIconSet
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_سېټ)
	{
	    Gtk_Icon_سېټ	*icon_سېټ;
	    Gtk_Icon_اندازه	icon_اندازه;

	    gtk_image_get_icon_سېټ(image, &icon_سېټ, &icon_اندازه);
	    icon_اندازه = (Gtk_Icon_اندازه)(اوږد)user_معلومات;

	    gtk_icon_سېټ_ref(icon_سېټ);
	    gtk_image_سېټ_from_icon_سېټ(image, icon_سېټ, icon_اندازه);
	    gtk_icon_سېټ_unref(icon_سېټ);
	}
# endif
    }
    وإلا إذا (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_اندازه_بدل_شوی_foreach,
			      user_معلومات);
    }
}

    ساكن فراغ
سېټ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_اندازه	    اندازه;
    Gtk_Icon_اندازه	    oldsize;

    إذا ((toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_متن | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    وإلا إذا ((toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS))
		      == (TOOLBAR_متن | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    وإلا إذا (toolbar_flags & TOOLBAR_متن)
	style = GTK_TOOLBAR_متن;
    وإلا
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_سېټ_style(toolbar, style);
# إذا !GTK_CHECK_نسخه(3,0,0)
    gtk_toolbar_سېټ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    بدل (tbis_flags)
    {
	حالة TBIS_TINY:	    اندازه = GTK_ICON_اندازه_MENU;		اكسر;
	حالة TBIS_SMALL:    اندازه = GTK_ICON_اندازه_SMALL_TOOLBAR;	اكسر;
	حالة TBIS_MEDIUM:   اندازه = GTK_ICON_اندازه_BUTTON;	اكسر;
	حالة TBIS_LARGE:    اندازه = GTK_ICON_اندازه_LARGE_TOOLBAR;	اكسر;
	حالة TBIS_HUGE:     اندازه = GTK_ICON_اندازه_DND;		اكسر;
	حالة TBIS_GIANT:    اندازه = GTK_ICON_اندازه_DIALOG;	اكسر;
	افتراضي:	    اندازه = GTK_ICON_اندازه_INVALID;	اكسر;
    }
    oldsize = gtk_toolbar_get_icon_اندازه(toolbar);

    إذا (اندازه == GTK_ICON_اندازه_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_اندازه(toolbar);
	اندازه = gtk_toolbar_get_icon_اندازه(toolbar);
    }
    إذا (اندازه != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_اندازه_بدل_شوی_foreach,
			      GINT_منزل_اشاره_ګر((صحيح)اندازه));
    }
    gtk_toolbar_سېټ_icon_اندازه(toolbar, اندازه);
}

#د_که_پای // FEAT_TOOLBAR

#إذا defined(FEAT_GUI_TABLINE)
ساكن صحيح ignore_tabline_evt = FALSE;
ساكن GtkWidget *tabline_menu;
# إذا !GTK_CHECK_نسخه(3,0,0)
ساكن GtkTooltips *tabline_tooltip;
# endif
ساكن صحيح clicked_پاڼه;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_handler(Gtk_Menu_توکی *توکی ناکارول_شوی, gpointer user_معلومات)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_پاڼه, (صحيح)(اوږد)user_معلومات);
}

    ساكن فراغ
جمع_کړه_tabline_menu_توکی(GtkWidget *menu, char_u *متن, صحيح resp)
{
    GtkWidget	*توکی;
    char_u	*utf_متن;

    utf_متن = CONVERT_منزل_UTF8(متن);
    توکی = gtk_menu_توکی_new_with_label((ثابت حرف *)utf_متن);
    gtk_widget_show(توکی);
    CONVERT_منزل_UTF8_FREE(utf_متن);

    gtk_container_جمع_کړه(GTK_CONTAINER(menu), توکی);
    g_signal_connect(G_OBJECT(توکی), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_منزل_اشاره_ګر(resp));
}

/*
 * Create a menu for the tab line.
 */
    ساكن GtkWidget *
create_tabline_menu(فراغ)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    جمع_کړه_tabline_menu_توکی(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    جمع_کړه_tabline_menu_توکی(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    جمع_کړه_tabline_menu_توکی(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    أرجع menu;
}

    ساكن gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    إذا (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	صحيح		x = bevent->x;
	صحيح		y = bevent->y;
	GtkWidget	*tabwidget;
	Gdk_کړکۍ	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	إذا (hold_gui_events || cmdwin_type != 0)
	    أرجع TRUE;

	tabwin = gui_gtk_کړکۍ_at_position(gui.mainwin, &x, &y);

	gdk_کړکۍ_get_user_معلومات(tabwin, (gpointer)&tabwidget);
	clicked_پاڼه = GPOINTER_منزل_INT(g_object_get_معلومات(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	إذا (bevent->button == 3)
	{
# إذا GTK_CHECK_نسخه(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    إذا (gui.is_wayland)
	    {
		صحيح x2, y2;
		gui_gtk_get_اشاره_ګر(gui.mainwin, &x2, &y2, عدم);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_کړکۍ(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, عدم);
	    }
	    وإلا
#  endif
	    gtk_menu_popup_at_اشاره_ګر(GTK_MENU(widget), event);
# وإلا
	    gtk_menu_popup(GTK_MENU(widget), عدم, عدم, عدم, عدم,
						bevent->button, bevent->وخت);
# endif
	    // We handled the event.
	    أرجع TRUE;
	}
	وإلا إذا (bevent->button == 1)
	{
	    إذا (clicked_پاڼه == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	وإلا إذا (bevent->button == 2)
	{
	    إذا (clicked_پاڼه != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_پاڼه, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    أرجع FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    ساكن فراغ
on_select_tab(
	GtkNotebook	*notebook ناکارول_شوی,
	gpointer	*پاڼه ناکارول_شوی,
	gint		idx,
	gpointer	معلومات ناکارول_شوی)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# إذا GTK_CHECK_نسخه(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook ناکارول_شوی,
	gpointer	*پاڼه ناکارول_شوی,
	gint		idx,
	gpointer	معلومات ناکارول_شوی)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_شاخص(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وإلا
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    فراغ
gui_mch_show_tabline(صحيح showit)
{
    إذا (gui.tabline == عدم)
	أرجع;

    إذا (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    صحيح w, h;

	    gtk_کړکۍ_get_اندازه(GTK_کړکۍ(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_سېټ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_کړکۍ_manager_hints(0, 0);
	إذا (showit)
	    gtk_widget_سېټ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحيح
gui_mch_showing_tabline(فراغ)
{
    أرجع gui.tabline != عدم
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    فراغ
gui_mch_update_tabline(فراغ)
{
    GtkWidget	    *پاڼه;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    صحيح		    nr = 0;
    صحيح		    tab_عدد;
    صحيح		    curtabidx = 0;
    char_u	    *labeltext;

    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = لومړی_tabpage; tp != عدم; tp = tp->tp_بل, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	tab_عدد = nr + 1;

	پاڼه = gtk_notebook_get_nth_پاڼه(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (پاڼه == عدم)
	{
	    // Add notebook page
# إذا GTK_CHECK_نسخه(3,2,0)
	    پاڼه = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_سېټ_homogeneous(GTK_BOX(پاڼه), FALSE);
# وإلا
	    پاڼه = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(پاڼه);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# إذا !GTK_CHECK_نسخه(3,14,0)
	    gtk_misc_سېټ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_جمع_کړه(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_ورننباسه_پاڼه(GTK_NOTEBOOK(gui.tabline),
		    پاڼه,
		    event_box,
		    nr);
# إذا GTK_CHECK_نسخه(2,10,0)
	    gtk_notebook_سېټ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    پاڼه,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), پاڼه);
	g_object_سېټ_معلومات(G_OBJECT(event_box), "tab_num",
						     GINT_منزل_اشاره_ګر(tab_عدد));
	label = gtk_bin_get_ماشوم(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
	gtk_label_سېټ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_منزل_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
# إذا GTK_CHECK_نسخه(3,0,0)
	gtk_widget_سېټ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
# وإلا
	gtk_tooltips_سېټ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ثابت حرف *)labeltext, عدم);
# endif
	CONVERT_منزل_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    طالما (gtk_notebook_get_nth_پاڼه(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_پاڼه(GTK_NOTEBOOK(gui.tabline), nr);

    إذا (gtk_notebook_get_current_پاڼه(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سېټ_current_پاڼه(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سېټ_curtab(صحيح nr)
{
    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;
    إذا (gtk_notebook_get_current_پاڼه(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_سېټ_current_پاڼه(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#د_که_پای // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    فراغ
gui_gtk_سېټ_selection_targets(GdkAtom selection)
{
    صحيح		    i, j = 0;
    ساكن صحيح	    n_targets = N_SELECTION_TARGETS;
    ساكن Gtk_موخه_ثبت  targets[N_SELECTION_TARGETS];

    إذا (targets[0].موخه == عدم)
    {
	لكل (i = 0; i < (صحيح)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    إذا (!clip_html && selection_targets[i].info == موخه_HTML)
		n_targets--;
	    وإلا
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_جمع_کړه_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    فراغ
gui_gtk_سېټ_dnd_targets(فراغ)
{
    صحيح		    i, j = 0;
    صحيح		    n_targets = N_DND_TARGETS;
    Gtk_موخه_ثبت  targets[N_DND_TARGETS];

    لكل (i = 0; i < (صحيح)N_DND_TARGETS; ++i)
    {
	إذا (!clip_html && dnd_targets[i].info == موخه_HTML)
	    n_targets--;
	وإلا
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_سېټ(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    صحيح
gui_mch_init(فراغ)
{
    GtkWidget *vbox;

#که_تعریف_شوی FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    إذا (using_gnome)
    {
	gnome_پروګرام_init(VIMPACKAGE, VIM_نسخه_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, عدم);
# إذا defined(LC_NUMERIC)
	{
	    حرف *p = setlocale(LC_NUMERIC, عدم);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    إذا (p == عدم || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#د_که_پای
    VIM_CLEAR(gui_argv);

#إذا GLIB_CHECK_نسخه(2,1,3)
    // Set the human-readable application name
    g_سېټ_application_name("Vim");
#د_که_پای
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    سېټ_option_ارزښت_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#که_تعریف_شوی FEAT_TOOLBAR
    gui_gtk_ثبتوونکی_stock_icons();
#د_که_پای
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#إذا !GTK_CHECK_نسخه(3,0,0)
# إذا 0
    gtk_rc_parse("gtkrc");
# endif
#د_که_پای

    // Initialize values
    gui.border_پلنوالی = 2;
    gui.scrollbar_پلنوالی = SB_DEFAULT_پلنوالی;
    gui.scrollbar_لوړوالی = SB_DEFAULT_پلنوالی;
#إذا GTK_CHECK_نسخه(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#وإلا
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#د_که_پای

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.شا_pixel = gui.def_شا_pixel;

    إذا (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	إذا (plug != عدم && gtk_plug_get_socket_کړکۍ(GTK_PLUG(plug)) != عدم)
	{
	    gui.mainwin = plug;
	}
	وإلا
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (بینښه صحيح)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    إذا (gtk_socket_id == 0)
    {
#که_تعریف_شوی FEAT_GUI_GNOME
	إذا (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", عدم);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	وإلا
#د_که_پای
	    gui.mainwin = gtk_کړکۍ_new(GTK_کړکۍ_TOPLEVEL);
    }

    gtk_widget_سېټ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سېټ_بنسټ_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_جمع_کړه_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_ماسک);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_بدل_شوی_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), عدم);

    gui.accel_ډله = gtk_accel_ډله_new();
    gtk_کړکۍ_جمع_کړه_accel_ډله(GTK_کړکۍ(gui.mainwin), gui.accel_ډله);

    // A vertical box holds the menubar, toolbar and main text window.
#إذا GTK_CHECK_نسخه(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سېټ_homogeneous(GTK_BOX(vbox), FALSE);
#وإلا
    vbox = gtk_vbox_new(FALSE, 0);
#د_که_پای

#که_تعریف_شوی FEAT_GUI_GNOME
    إذا (using_gnome)
    {
# إذا defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_سېټ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    وإلا
#د_که_پای
    {
	gtk_container_جمع_کړه(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#که_تعریف_شوی FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_سېټ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_سېټ(gtk_settings, "gtk-menu-bar-accel", عدم, عدم);
    }


# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
    {
	Bonobo_Dock_توکی *dockitem;

	gnome_app_سېټ_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_توکی_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_توکی_سېټ_behavior(dockitem,
		bonobo_dock_توکی_get_behavior(dockitem)
				       | BONOBO_DOCK_توکی_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_پیل(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#د_که_پای	// FEAT_MENU

#که_تعریف_شوی FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# وإلا
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_سېټ_name(gui.toolbar, "vim-toolbar");
    سېټ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
    {
	Bonobo_Dock_توکی *dockitem;

	gnome_app_سېټ_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_توکی_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_توکی_سېټ_behavior(dockitem,
		bonobo_dock_توکی_get_behavior(dockitem)
				       | BONOBO_DOCK_توکی_BEH_NEVER_FLOATING);
	gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(gui.toolbar), 0);
    }
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_پیل(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#د_که_پای // FEAT_TOOLBAR

#که_تعریف_شوی FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_پیل(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_سېټ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سېټ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سېټ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# إذا !GTK_CHECK_نسخه(3,0,0)
    gtk_notebook_سېټ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# إذا !GTK_CHECK_نسخه(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *پاڼه, *label, *event_box;

	// Add the first tab.
# إذا GTK_CHECK_نسخه(3,2,0)
	پاڼه = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_سېټ_homogeneous(GTK_BOX(پاڼه), FALSE);
# وإلا
	پاڼه = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(پاڼه);
	gtk_container_جمع_کړه(GTK_CONTAINER(gui.tabline), پاڼه);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_سېټ_معلومات(G_OBJECT(event_box), "tab_num", GINT_منزل_اشاره_ګر(1L));
# إذا !GTK_CHECK_نسخه(3,14,0)
	gtk_misc_سېټ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_جمع_کړه(GTK_CONTAINER(event_box), label);
	gtk_notebook_سېټ_tab_label(GTK_NOTEBOOK(gui.tabline), پاڼه, event_box);
# إذا GTK_CHECK_نسخه(2,10,0)
	gtk_notebook_سېټ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), پاڼه, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), عدم);
# إذا GTK_CHECK_نسخه(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), عدم);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#د_که_پای // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_سېټ_border_پلنوالی(GTK_CONTAINER(gui.formwin), 0);
#إذا !GTK_CHECK_نسخه(3,0,0)
    gtk_widget_سېټ_events(gui.formwin, GDK_EXPOSURE_ماسک);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,22,2)
    gtk_widget_سېټ_name(gui.formwin, "vim-gtk-form");
#د_که_پای

    gui.drawarea = gtk_drawing_area_new();
#إذا GTK_CHECK_نسخه(3,0,0)
    gui.surface = عدم;
#د_که_پای

#که_تعریف_شوی GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    إذا (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#د_که_پای

    // Determine which events we will filter.
    gint event_ماسک =
	GDK_EXPOSURE_ماسک |
	GDK_ENTER_NOTIFY_ماسک |
	GDK_LEAVE_NOTIFY_ماسک |
	GDK_BUTTON_PRESS_ماسک |
	GDK_BUTTON_RELEASE_ماسک |
	GDK_کلی_PRESS_ماسک |
	GDK_کلی_RELEASE_ماسک |
	GDK_اشاره_ګر_MOTION_ماسک |
	GDK_اشاره_ګر_MOTION_HINT_ماسک;
#إذا GTK_CHECK_نسخه(3,4,0)
    إذا (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_ماسک |= GDK_SCROLL_ماسک;
    }
    وإلا
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_ماسک |= GDK_SMOOTH_SCROLL_ماسک;
    }
#وإلا
    event_ماسک |= GDK_SCROLL_ماسک;
#د_که_پای
    gtk_widget_سېټ_events(gui.drawarea, event_ماسک);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_پیل(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(کلی_press_event), عدم);
#إذا defined(FEAT_XIM) || GTK_CHECK_نسخه(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&کلی_release_event), عدم);
#د_که_پای
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), عدم);
#إذا GTK_CHECK_نسخه(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), عدم);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), عدم);
#وإلا
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_سېټ_cb), عدم);
#د_که_پای

#إذا !GTK_CHECK_نسخه(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#د_که_پای

#إذا !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#د_که_پای

    إذا (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_سېټ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_ستوری.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_دننه_واټن = gui.border_پلنوالی;

#إذا GTK_CHECK_نسخه(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), عدم);
#وإلا
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), عدم);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), عدم);
#د_که_پای

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    إذا (vim_strchr(p_go, GO_اشاره_ګر) != عدم)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), عدم);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), عدم);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    إذا (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_بهر_event), عدم);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_دننه_event), عدم);
    }
    وإلا
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_بهر_event), عدم);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_دننه_event), عدم);
#که_تعریف_شوی FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_بهر_event), عدم);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_دننه_event), عدم);
#د_که_پای // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), عدم);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.دننه_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_بدل_شوی_cb), عدم);
    }

#إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخه(3,10,0)
    gui.scale = gtk_widget_get_scale_ضریب(gui.formwin);
#د_که_پای

    أرجع OK;
}

#إذا defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    فراغ
gui_mch_forked(فراغ)
{
    إذا (!using_gnome)
	أرجع;

    GnomeClient *client;

    client = gnome_master_client();

    إذا (client != عدم)
	gnome_client_سېټ_process_id(client, getpid());
}
#د_که_پای // USE_GNOME_SESSION

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن GdkRGBA
color_منزل_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    أرجع rgba;
}

    ساكن فراغ
سېټ_cairo_سرچینه_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ثابت GdkRGBA rgba = color_منزل_rgba(color);
    cairo_سېټ_سرچینه_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#د_که_پای // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم
#إذا GTK_CHECK_نسخه(3,22,2)
	    && gui.formwin != عدم
#د_که_پای
	    && gtk_widget_get_کړکۍ(gui.drawarea) != عدم)
    {
#إذا !GTK_CHECK_نسخه(3,22,2)
	Gdk_کړکۍ * ثابت da_win = gtk_widget_get_کړکۍ(gui.drawarea);
#د_که_پای
#إذا GTK_CHECK_نسخه(3,22,2)
	GtkStyleContext * ثابت context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar * ثابت css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.شا_pixel >> 16) & 0xff,
		 (gui.شا_pixel >> 8) & 0xff,
		 gui.شا_pixel & 0xff);

	gtk_css_provider_بارول_from_معلومات(provider, css, -1, عدم);
	gtk_style_context_جمع_کړه_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#وإلاإذا GTK_CHECK_نسخه(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_منزل_rgba(gui.شا_pixel);
	{
	    cairo_نمونه_t * ثابت pat = cairo_نمونه_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    إذا (pat != عدم)
	    {
		gdk_کړکۍ_سېټ_background_نمونه(da_win, pat);
		cairo_نمونه_destroy(pat);
	    }
	    وإلا
		gdk_کړکۍ_سېټ_background_rgba(da_win, &rgba);
	}
#وإلا // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.شا_pixel;
	gdk_کړکۍ_سېټ_background(da_win, &color);
#د_که_پای // !GTK_CHECK_VERSION(3,22,2)
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
    ساكن gboolean
startup_resize_correction_cb(gpointer معلومات ناکارول_شوی)
{
    gui_سېټ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    أرجع FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ساكن gint
form_configure_event(GtkWidget *widget ناکارول_شوی,
		     GdkEventConfigure *event,
		     gpointer معلومات ناکارول_شوی)
{
    صحيح	    usable_لوړوالی = event->لوړوالی;
#که_تعریف_شوی TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    صحيح	    w, h;
    gtk_کړکۍ_get_اندازه(GTK_کړکۍ(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (صحيح)Columns, (صحيح)لیکې);
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

    إذا (!سمون_پلنوالی_لوړوالی(latest_resize_hist, w, h)
					     && سمون_stale_پلنوالی_لوړوالی(w, h))
	// discard stale event
	أرجع TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    إذا ((mch_csd_لوړوالی == 0 || mch_csd_پلنوالی == 0)
	    && latest_resize_hist != عدم
	    && !latest_resize_hist->کارول_شوی
	    && latest_resize_hist->from_shellsize
	    && gui.char_لوړوالی > 0)
    {
	صحيح pot_csd_w = latest_resize_hist->پلنوالی - w;
	صحيح pot_csd_h = latest_resize_hist->لوړوالی - h;

	إذا (pot_csd_w > 0 && pot_csd_w < gui.char_پلنوالی)
	{
	    mch_csd_پلنوالی = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (صحيح)Columns * gui.char_پلنوالی
						    + gui_get_بنسټ_پلنوالی();
	}
	إذا (pot_csd_h > 0 && pot_csd_h < gui.char_لوړوالی)
	{
	    mch_csd_لوړوالی = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_لوړوالی += mch_csd_لوړوالی;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	إذا ((mch_csd_لوړوالی > 0 || mch_csd_پلنوالی > 0) && gtk_socket_id == 0)
	    g_idle_جمع_کړه_ډک(GTK_لومړیتوب_RESIZE, startup_resize_correction_cb, عدم, عدم);
    }
    clear_resize_hists();
#د_که_پای

#إذا GTK_CHECK_نسخه(3,22,2) && !GTK_CHECK_نسخه(3,22,4)
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
    إذا (event->کړکۍ_2 != gtk_widget_get_کړکۍ(gui.formwin))
	أرجع TRUE;
#د_که_پای

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    إذا (gtk_socket_id != 0)
	usable_لوړوالی -= (gui.char_لوړوالی - (gui.char_لوړوالی/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	صحيح use_پلنوالی = event->پلنوالی;

#که_تعریف_شوی TRACK_RESIZE_HISTORY
	إذا (mch_pending_form_w > event->پلنوالی
		&& mch_pending_form_w - event->پلنوالی < gui.char_پلنوالی)
	    use_پلنوالی = mch_pending_form_w;
#د_که_پای
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_پلنوالی, usable_لوړوالی);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    أرجع TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    ساكن فراغ
mainwin_destroy_cb(GObject *object ناکارول_شوی, gpointer معلومات ناکارول_شوی)
{
    // Don't write messages to the GUI anymore
    ډک_screen = FALSE;

    gui.mainwin  = عدم;
    gui.drawarea = عدم;

    إذا (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#که_تعریف_شوی USE_GRESOURCE
    gui_gtk_unregister_resource();
#د_که_پای
}

    فراغ
gui_gtk_get_screen_geom_د_win(
	GtkWidget *wid,
	صحيح ټکی_x,	    // x position of window if not initialized
	صحيح ټکی_y,	    // y position of window if not initialized
	صحيح *screen_x,
	صحيح *screen_y,
	صحيح *پلنوالی,
	صحيح *لوړوالی)
{
    GdkRectangle geometry;
    Gdk_کړکۍ *win = gtk_widget_get_کړکۍ(wid);
#إذا GTK_CHECK_نسخه(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    إذا (wid != عدم && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    وإلا
	dpy = gdk_display_get_default();
    إذا (win != عدم)
	monitor = gdk_display_get_monitor_at_کړکۍ(dpy, win);
    وإلا
	monitor = gdk_display_get_monitor_at_ټکی(dpy, ټکی_x, ټکی_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#وإلا
    GdkScreen* screen;
    صحيح monitor;

    إذا (wid != عدم && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    وإلا
	screen = gdk_screen_get_default();
    إذا (win != عدم)
	monitor = gdk_screen_get_monitor_at_کړکۍ(screen, win);
    وإلا
	monitor = gdk_screen_get_monitor_at_ټکی(screen, ټکی_x, ټکی_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#د_که_پای
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *پلنوالی = geometry.پلنوالی;
    *لوړوالی = geometry.لوړوالی;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ساكن فراغ
gui_gtk_get_screen_dimensions(
	صحيح ټکی_x,
	صحيح ټکی_y,
	صحيح *screen_w,
	صحيح *screen_h)
{
    صحيح	    x, y;

    gui_gtk_get_screen_geom_د_win(gui.mainwin, ټکی_x, ټکی_y,
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
    *screen_w -= get_menu_tool_پلنوالی();
    *screen_h -= get_menu_tool_لوړوالی();
}

    فراغ
gui_mch_get_screen_dimensions(صحيح *screen_w, صحيح *screen_h)
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
    ساكن timeout_cb_type
check_startup_plug_hints(gpointer معلومات ناکارول_شوی)
{
    إذا (init_کړکۍ_hints_state == 1)
    {
	// Safe to use normal hints now
	init_کړکۍ_hints_state = 0;
	update_کړکۍ_manager_hints(0, 0);
	أرجع FALSE;   // stop timer
    }

    // Keep on trying
    init_کړکۍ_hints_state = 1;
    أرجع TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحيح
gui_mch_open(فراغ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_پلنوالی;
    guint		pixel_لوړوالی;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    إذا (role_دلیل != عدم)
    {
	gtk_کړکۍ_سېټ_role(GTK_کړکۍ(gui.mainwin), role_دلیل);
    }
    وإلا
    {
	حرف *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (بینښه)mch_get_pid(),
			       (بینښه)g_random_int(),
			       (بینښه)وخت(عدم));

	gtk_کړکۍ_سېټ_role(GTK_کړکۍ(gui.mainwin), role);
	g_free(role);
    }

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gtk_کړکۍ_move(GTK_کړکۍ(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    إذا (gui.geom != عدم)
    {
	صحيح		ماسک;
	بینښه صحيح	w, h;
	صحيح		x = 0;
	صحيح		y = 0;

	ماسک = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);

	إذا (ماسک & پلنوالی_ارزښت)
	    Columns = w;
	إذا (ماسک & لوړوالی_ارزښت)
	{
	    إذا (p_کړکۍ > (اوږد)h - 1 || !option_was_سېټ((char_u *)"window"))
		p_کړکۍ = h - 1;
	    لیکې = h;
	}
	حد_screen_اندازه();

	pixel_پلنوالی = (guint)(gui_get_بنسټ_پلنوالی() + Columns * gui.char_پلنوالی);
	pixel_لوړوالی = (guint)(gui_get_بنسټ_لوړوالی() + لیکې * gui.char_لوړوالی);

	pixel_پلنوالی  += get_menu_tool_پلنوالی();
	pixel_لوړوالی += get_menu_tool_لوړوالی();

	إذا (ماسک & (XValue | YValue))
	{
	    صحيح ww, hh;

#که_تعریف_شوی FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#وإلا
	    gui_mch_get_screen_dimensions(&ww, &hh);
#د_که_پای
	    hh += p_ghr + get_menu_tool_لوړوالی();
	    ww += get_menu_tool_پلنوالی();
	    إذا (ماسک & XNegative)
		x += ww - pixel_پلنوالی;
	    إذا (ماسک & YNegative)
		y += hh - pixel_لوړوالی;
	    gtk_کړکۍ_move(GTK_کړکۍ(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	إذا (gtk_socket_id != 0  &&  (ماسک & پلنوالی_ارزښت || ماسک & لوړوالی_ارزښت))
	{
	    update_کړکۍ_manager_hints(pixel_پلنوالی, pixel_لوړوالی);
	    init_کړکۍ_hints_state = 1;
	    timeout_جمع_کړه(1000, check_startup_plug_hints, عدم);
	}
    }

    pixel_پلنوالی = (guint)(gui_get_بنسټ_پلنوالی() + Columns * gui.char_پلنوالی);
    pixel_لوړوالی = (guint)(gui_get_بنسټ_لوړوالی() + لیکې * gui.char_لوړوالی);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    إذا (gtk_socket_id == 0)
	gtk_کړکۍ_resize(GTK_کړکۍ(gui.mainwin), pixel_پلنوالی, pixel_لوړوالی);
    update_کړکۍ_manager_hints(0, 0);

    إذا (foreground_دلیل != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_دلیل);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_دلیل != عدم)
	bg_pixel = gui_get_color((char_u *)background_دلیل);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (وموندل_سرچپه_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_شا_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_شا_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    سېټ_نورمال_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), عدم);

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
#که_تعریف_شوی TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#د_که_پای
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), عدم);
#إذا GTK_CHECK_نسخه(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ضریب_event), عدم);
#د_که_پای

#که_تعریف_شوی FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_سېټ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_معلومات_received_cb), عدم);
#د_که_پای

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	إذا (وموندل_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	بینښه اوږد menu_handler = 0;
# ifdef FEAT_TOOLBAR
	بینښه اوږد tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	إذا (using_gnome && vim_strchr(p_go, GO_MENUS) == عدم)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  عدم);
# ifdef FEAT_TOOLBAR
	إذا (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == عدم
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  عدم);
# endif
#د_که_پای
	gtk_widget_show(gui.mainwin);

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	إذا (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	إذا (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#د_که_پای
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), عدم);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), عدم);

    أرجع OK;
}

/*
 * Clean up for when exiting Vim.
 */
    فراغ
gui_mch_exit(صحيح rc ناکارول_شوی)
{
    // Clean up, unless we don't want to invoke free().
    إذا (gui.mainwin != عدم && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    صحيح
gui_mch_get_winpos(صحيح *x, صحيح *y)
{
    إذا (gui_mch_get_display())
    {
	gtk_کړکۍ_get_position(GTK_کړکۍ(gui.mainwin), x, y);
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_سېټ_winpos(صحيح x, صحيح y)
{
    gtk_کړکۍ_move(GTK_کړکۍ(gui.mainwin), x, y);
}

#إذا !GTK_CHECK_نسخه(3,0,0)
# إذا 0
ساكن صحيح resize_idle_installed = FALSE;
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
    ساكن gboolean
force_shell_resize_idle(gpointer معلومات)
{
    إذا (gui.mainwin != عدم
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	صحيح پلنوالی;
	صحيح لوړوالی;

	gtk_کړکۍ_get_اندازه(GTK_کړکۍ(gui.mainwin), &پلنوالی, &لوړوالی);

	پلنوالی  -= get_menu_tool_پلنوالی();
	لوړوالی -= get_menu_tool_لوړوالی();

	gui_resize_shell(پلنوالی, لوړوالی);
    }

    resize_idle_installed = FALSE;
    أرجع FALSE; // don't call me again
}
# endif
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    صحيح
gui_mch_maximized(فراغ)
{
    أرجع (gui.mainwin != عدم && gtk_widget_get_کړکۍ(gui.mainwin) != عدم
	    && (gdk_کړکۍ_get_state(gtk_widget_get_کړکۍ(gui.mainwin))
					       & GDK_کړکۍ_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_کړکۍ_unmaximize(GTK_کړکۍ(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    فراغ
gui_mch_newfont(فراغ)
{
    صحيح w, h;

    gtk_کړکۍ_get_اندازه(GTK_کړکۍ(gui.mainwin), &w, &h);
    w -= get_menu_tool_پلنوالی();
    h -= get_menu_tool_لوړوالی();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    فراغ
gui_mch_سېټ_shellsize(صحيح پلنوالی, صحيح لوړوالی,
		      صحيح min_پلنوالی ناکارول_شوی,  صحيح min_لوړوالی ناکارول_شوی,
		      صحيح بنسټ_پلنوالی ناکارول_شوی, صحيح بنسټ_لوړوالی ناکارول_شوی,
		      صحيح direction ناکارول_شوی)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    إذا (gtk_socket_id == 0)
	update_کړکۍ_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    پلنوالی  += get_menu_tool_پلنوالی();
    لوړوالی += get_menu_tool_لوړوالی();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    پلنوالی  += mch_csd_پلنوالی;
    لوړوالی += mch_csd_لوړوالی;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_پلنوالی > 0)
			    ? پلنوالی - get_menu_tool_پلنوالی() - mch_csd_پلنوالی : 0;

#که_تعریف_شوی TRACK_RESIZE_HISTORY
    alloc_resize_hist(پلنوالی, لوړوالی, TRUE);
#د_که_پای
    إذا (gtk_socket_id == 0)
	gtk_کړکۍ_resize(GTK_کړکۍ(gui.mainwin), پلنوالی, لوړوالی);
    وإلا
	update_کړکۍ_manager_hints(پلنوالی, لوړوالی);

#إذا !GTK_CHECK_نسخه(3,0,0)
# إذا 0
    إذا (!resize_idle_installed)
    {
	g_idle_جمع_کړه_ډک(GDK_لومړیتوب_EVENTS + 10,
			&force_shell_resize_idle, عدم, عدم);
	resize_idle_installed = TRUE;
    }
# endif
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    فراغ
gui_mch_settitle(char_u *title, char_u *icon ناکارول_شوی)
{
    إذا (title != عدم && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, عدم);

    gtk_کړکۍ_سېټ_title(GTK_کړکۍ(gui.mainwin), (ثابت حرف *)title);

    إذا (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#إذا defined(FEAT_MENU)
    فراغ
gui_mch_enable_menu(صحيح showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
	widget = gui.menubar_h;
    وإلا
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    إذا (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	إذا (showit)
	    gtk_widget_show(widget);
	وإلا
	    gtk_widget_hide(widget);

	update_کړکۍ_manager_hints(0, 0);
    }
}
#د_که_پای // FEAT_MENU

#إذا defined(FEAT_TOOLBAR)
    فراغ
gui_mch_show_toolbar(صحيح showit)
{
    GtkWidget *widget;

    إذا (gui.toolbar == عدم)
	أرجع;

# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
	widget = gui.toolbar_h;
    وإلا
# endif
	widget = gui.toolbar;

    إذا (showit)
	سېټ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    إذا (!showit != !gtk_widget_get_visible(widget))
    {
	إذا (showit)
	    gtk_widget_show(widget);
	وإلا
	    gtk_widget_hide(widget);

	update_کړکۍ_manager_hints(0, 0);
    }
}
#د_که_پای // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    ساكن صحيح
is_cjk_font(PangoFontDescription *font_desc)
{
    ساكن ثابت حرف * ثابت cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    بینښه	i;
    صحيح		is_cjk = FALSE;

    font = pango_context_بارول_font(gui.متن_context, font_desc);

    إذا (font == عدم)
	أرجع FALSE;

    لكل (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	إذا (coverage != عدم)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#إذا PANGO_نسخه_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#وإلا
	    pango_coverage_unref(coverage);
#د_که_پای
	}
    }

    g_object_unref(font);

    أرجع is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحيح
gui_mch_adjust_charheight(فراغ)
{
    PangoFontMetrics	*metrics;
    صحيح			ascent;
    صحيح			descent;

    metrics = pango_context_get_metrics(gui.متن_context, gui.norm_font,
				pango_context_get_language(gui.متن_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_لوړوالی = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_لوړوالی = MAX(gui.char_لوړوالی, gui.char_ascent + 1);

    أرجع OK;
}

#إذا GTK_CHECK_نسخه(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ساكن gboolean
font_filter(ثابت PangoFontFamily *family,
	    ثابت PangoFontFace   *face ناکارول_شوی,
	    gpointer		   معلومات ناکارول_شوی)
{
    أرجع pango_font_family_is_monospace((PangoFontFamily *)family);
}
#د_که_پای

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
    صحيح		response;
    char_u	*fontname = عدم;
    char_u	*oldname;

#إذا GTK_CHECK_نسخه(3,2,0)
    dialog = gtk_font_chooser_dialog_new(عدم, عدم);
    gtk_font_chooser_سېټ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    عدم, عدم);
#وإلا
    dialog = gtk_font_selection_dialog_new(عدم);
#د_که_پای

    gtk_کړکۍ_سېټ_transient_for(GTK_کړکۍ(dialog), GTK_کړکۍ(gui.mainwin));
    gtk_کړکۍ_سېټ_destroy_with_پلار(GTK_کړکۍ(dialog), TRUE);

    إذا (oldval != عدم && oldval[0] != NUL)
    {
	إذا (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, عدم);
	وإلا
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	إذا (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    إذا (p != عدم)
	    {
		STRCPY(p + STRLEN(p), " 10");
		إذا (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#إذا GTK_CHECK_نسخه(3,2,0)
	gtk_font_chooser_سېټ_font(
		GTK_FONT_CHOOSER(dialog), (ثابت gchar *)oldname);
#وإلا
	gtk_font_selection_dialog_سېټ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ثابت حرف *)oldname);
#د_که_پای

	إذا (oldname != oldval)
	    vim_free(oldname);
    }
    وإلا
#إذا GTK_CHECK_نسخه(3,2,0)
	gtk_font_chooser_سېټ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#وإلا
	gtk_font_selection_dialog_سېټ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#د_که_پای

    response = gtk_dialog_پرلپسې(GTK_DIALOG(dialog));

    إذا (response == GTK_RESPONSE_OK)
    {
	حرف *name;

#إذا GTK_CHECK_نسخه(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#وإلا
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#د_که_پای
	إذا (name != عدم)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && ننوت_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ننوت_conv, p, عدم);
		vim_free(p);
	    }
	    وإلا
		fontname = p;
	}
    }

    إذا (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    أرجع fontname;
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
    ساكن فراغ
get_styled_font_variants(فراغ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_بارول_font(gui.متن_context, gui.norm_font);

    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سېټ_وزن(bold_font_desc, PANGO_وزن_BOLD);

    bold_font = pango_context_بارول_font(gui.متن_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    إذا (bold_font != عدم)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    إذا (bold_font != عدم && gui.font_can_bold)
	g_object_unref(plain_font);
}

ساكن PangoEngineShape *default_shape_engine = عدم;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    ساكن فراغ
ascii_glyph_جدول_init(فراغ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *توکی_list;
    صحيح		    i;

    إذا (gui.ascii_glyphs != عدم)
	pango_glyph_string_free(gui.ascii_glyphs);
    إذا (gui.ascii_font != عدم)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = عدم;
    gui.ascii_font   = عدم;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    لكل (i = 0; i < 128; ++i)
    {
	إذا (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	وإلا
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    توکی_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (توکی_list != عدم && توکی_list->بل == عدم) // play safe
    {
	Pango_توکی   *توکی;
	صحيح	    پلنوالی;

	توکی  = (Pango_توکی *)توکی_list->معلومات;
	پلنوالی = gui.char_پلنوالی * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = توکی->analysis.shape_engine;

	gui.ascii_font = توکی->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &توکی->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->عدد_glyphs == حجم(ascii_chars));

	لكل (i = 0; i < gui.ascii_glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_دننه_واټن += MAX(0, پلنوالی - geom->پلنوالی) / 2;
	    geom->پلنوالی = پلنوالی;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(توکی_list, (GFunc)(فراغ *)&pango_توکی_free, عدم);
    g_list_free(توکی_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset ناکارول_شوی)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    پلنوالی;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_سېټ_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_سېټ_متن(layout, "MW", 2);
    pango_layout_get_اندازه(layout, &پلنوالی, عدم);
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
    إذا (is_cjk_font(gui.norm_font))
    {
	صحيح cjk_پلنوالی;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_سېټ_متن(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_اندازه(layout, &cjk_پلنوالی, عدم);

	إذا (پلنوالی == cjk_پلنوالی)  // Xft not patched
	    پلنوالی /= 2;
    }
    g_object_unref(layout);

    gui.char_پلنوالی = (پلنوالی / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    إذا (gui.char_پلنوالی <= 0)
	gui.char_پلنوالی = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_سېټ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_جدول_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    إذا (gui.wide_font != عدم
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = عدم;
    }

    إذا (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    وإلا
    {
	// Preserve the logical dimensions of the screen.
	update_کړکۍ_manager_hints(0, 0);
    }

    أرجع OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, صحيح report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    إذا (!gui.دننه_use || name == عدم)
	أرجع عدم;

    إذا (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, عدم);
	إذا (buf != عدم)
	{
	    font = pango_font_description_from_string((ثابت حرف *)buf);
	    vim_free(buf);
	}
	وإلا
	    font = عدم;
    }
    وإلا
	font = pango_font_description_from_string((ثابت حرف *)name);

    إذا (font != عدم)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	إذا (pango_font_description_get_اندازه(font) <= 0)
	    pango_font_description_سېټ_اندازه(font, 10 * PANGO_SCALE);

	real_font = pango_context_بارول_font(gui.متن_context, font);

	إذا (real_font == عدم)
	{
	    pango_font_description_free(font);
	    font = عدم;
	}
	وإلا
	    g_object_unref(real_font);
    }

    إذا (font == عدم)
    {
	إذا (report_error)
	    semsg(_((حرف *)e_unknown_font_str), name);
	أرجع عدم;
    }

    أرجع font;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name ناکارول_شوی)
{
    إذا (font != NOFONT)
    {
	حرف	*pangoname = pango_font_description_منزل_string(font);

	إذا (pangoname != عدم)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    أرجع s;
	}
    }
    أرجع عدم;
}
#د_که_پای

/*
 * If a font is not going to be used, free its structure.
 */
    فراغ
gui_mch_free_font(GuiFont font)
{
    إذا (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
    فراغ
gui_mch_expand_font(
    optexpand_T	*دلیلونه,
    فراغ	*param,
    صحيح		(*جمع_کړه_سمون)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (دلیلونه->oe_include_orig_val && *دلیلونه->oe_opt_ارزښت == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	إذا (جمع_کړه_سمون((char_u *)DEFAULT_FONT) != OK)
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

	ثابت حرف* fam_name = pango_font_family_get_name(font_families[i]);
	إذا (ننوت_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ننوت_conv, (char_u*)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (جمع_کړه_سمون(buf) != OK)
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
	    إذا (جمع_کړه_سمون((char_u *)fam_name) != OK)
		اكسر;
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

    إذا (!gui.دننه_use)		// can't do this when GUI not running
	أرجع color;

    إذا (name != عدم)
	color = gui_get_color_cmn(name);

#إذا GTK_CHECK_نسخه(3,0,0)
    أرجع color;
#وإلا
    إذا (color == INVALCOLOR)
	أرجع INVALCOLOR;

    أرجع gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#د_که_پای
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    أرجع gui_get_rgb_color_cmn(r, g, b);
#وإلا
    GdkColor gcolor;
    صحيح ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    أرجع ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#د_که_پای
}

/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_سېټ_fg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    *gui.fgcolor = color_منزل_rgba(color);
#وإلا
    gui.fgcolor->pixel = (بینښه اوږد)color;
#د_که_پای
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_سېټ_bg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    *gui.bgcolor = color_منزل_rgba(color);
#وإلا
    gui.bgcolor->pixel = (بینښه اوږد)color;
#د_که_پای
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سېټ_sp_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    *gui.spcolor = color_منزل_rgba(color);
#وإلا
    gui.spcolor->pixel = (بینښه اوږد)color;
#د_که_پای
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#تعریف_کړه ورننباسه_PANGO_ATTR(Attribute, AttrList, پیل_2, پای_ټکی_2)  \
    G_STMT_پیل{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->پیل_شاخص = (پیل_2);		    \
	tmp_attr_->پای_ټکی_شاخص = (پای_ټکی_2);			    \
	pango_attr_list_ورننباسه((AttrList), tmp_attr_);	    \
    }G_STMT_پای_ټکی

    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *پیل = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (پیل == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		پیل = p;
	}
	وإلا إذا (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ورننباسه_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, پیل - s, p - s);
	    پیل = عدم;
	}
    }

    إذا (پیل != عدم)
	ورننباسه_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, پیل - s, len);
}

    ساكن صحيح
شمېر_cluster_cells(char_u *s, Pango_توکی *توکی,
		    PangoGlyphString* glyphs, صحيح i,
		    صحيح *cluster_پلنوالی,
		    صحيح *وروستی_glyph_rbearing)
{
    char_u  *p;
    صحيح	    بل;	// glyph start index of next cluster
    صحيح	    پیل, پای_ټکی; // string segment of current cluster
    صحيح	    پلنوالی;	// real cluster width in Pango units
    صحيح	    uc;
    صحيح	    cellcount = 0;

    پلنوالی = glyphs->glyphs[i].geometry.پلنوالی;

    لكل (بل = i + 1; بل < glyphs->عدد_glyphs; ++بل)
    {
	إذا (glyphs->glyphs[بل].attr.is_cluster_پیل)
	    اكسر;
	وإلا إذا (glyphs->glyphs[بل].geometry.پلنوالی > پلنوالی)
	    پلنوالی = glyphs->glyphs[بل].geometry.پلنوالی;
    }

    پیل = توکی->دننه_واټن + glyphs->log_clusters[i];
    پای_ټکی   = توکی->دننه_واټن + ((بل < glyphs->عدد_glyphs) ?
			    glyphs->log_clusters[بل] : توکی->اوږدتیا);

    لكل (p = s + پیل; p < s + پای_ټکی; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (وروستی_glyph_rbearing != عدم
	    && cellcount > 0 && بل == glyphs->عدد_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(توکی->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *وروستی_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_پلنوالی = پلنوالی;

    أرجع cellcount;
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
    ساكن فراغ
تیاری_zero_پلنوالی_cluster(Pango_توکی *توکی, PangoGlyphInfo *glyph,
			 صحيح وروستی_cellcount, صحيح وروستی_cluster_پلنوالی,
			 صحيح وروستی_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    پلنوالی;

    پلنوالی = وروستی_cellcount * gui.char_پلنوالی * PANGO_SCALE;
    glyph->geometry.x_دننه_واټن = -پلنوالی + MAX(0, پلنوالی - وروستی_cluster_پلنوالی) / 2;
    glyph->geometry.پلنوالی = 0;

    pango_font_get_glyph_extents(توکی->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_دننه_واټن += وروستی_glyph_rbearing;
	glyph->geometry.y_دننه_واټن  = logical_rect.لوړوالی
		- (gui.char_لوړوالی - p_linespace) * PANGO_SCALE;
    }
    وإلا
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_دننه_واټن = -پلنوالی + MAX(0, پلنوالی - ink_rect.پلنوالی) / 2;
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
draw_glyph_string(صحيح لیکه, صحيح ستون, صحيح عدد_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#وإلا
    ساكن فراغ
draw_glyph_string(صحيح لیکه, صحيح ستون, صحيح عدد_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#د_که_پای
{
    إذا (!(flags & DRAW_TRANSP))
    {
#إذا GTK_CHECK_نسخه(3,0,0)
	cairo_سېټ_سرچینه_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(ستون), FILL_Y(لیکه),
			عدد_cells * gui.char_پلنوالی, gui.char_لوړوالی);
	cairo_fill(cr);
#وإلا
	gdk_gc_سېټ_foreground(gui.متن_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->کړکۍ_2,
			   gui.متن_gc,
			   TRUE,
			   FILL_X(ستون),
			   FILL_Y(لیکه),
			   عدد_cells * gui.char_پلنوالی,
			   gui.char_لوړوالی);
#د_که_پای
    }

#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_سېټ_سرچینه_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_منزل(cr, متن_X(ستون), متن_Y(لیکه));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#وإلا
    gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->کړکۍ_2,
		    gui.متن_gc,
		    font,
		    متن_X(ستون),
		    متن_Y(لیکه),
		    glyphs);
#د_که_پای

    // redraw the contents with an offset of 1 to emulate bold
    إذا ((flags & DRAW_BOLD) && !gui.font_can_bold)
#إذا GTK_CHECK_نسخه(3,0,0)
    {
	cairo_سېټ_سرچینه_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, متن_X(ستون) + 1, متن_Y(لیکه));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#وإلا
	gdk_draw_glyphs(gui.drawarea->کړکۍ_2,
			gui.متن_gc,
			font,
			متن_X(ستون) + 1,
			متن_Y(لیکه),
			glyphs);
#د_که_پای
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
draw_under(صحيح flags, صحيح لیکه, صحيح ستون, صحيح cells, cairo_t *cr)
#وإلا
    ساكن فراغ
draw_under(صحيح flags, صحيح لیکه, صحيح ستون, صحيح cells)
#د_که_پای
{
    صحيح			i;
    صحيح			دننه_واټن;
    ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    صحيح			y = FILL_Y(لیکه + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    إذا (flags & DRAW_UNDERC)
    {
#إذا GTK_CHECK_نسخه(3,0,0)
	cairo_سېټ_کرښه_پلنوالی(cr, 1.0);
	cairo_سېټ_کرښه_cap(cr, CAIRO_کرښه_CAP_BUTT);
	cairo_سېټ_سرچینه_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(ستون) + 1, y - 2 + 0.5);
	لكل (i = FILL_X(ستون) + 1; i < FILL_X(ستون + cells); ++i)
	{
	    دننه_واټن = val[i % 8];
	    cairo_کرښه_منزل(cr, i, y - دننه_واټن + 0.5);
	}
	cairo_stroke(cr);
#وإلا
	gdk_gc_سېټ_foreground(gui.متن_gc, gui.spcolor);
	لكل (i = FILL_X(ستون); i < FILL_X(ستون + cells); ++i)
	{
	    دننه_واټن = val[i % 8];
	    gdk_draw_ټکی(gui.drawarea->کړکۍ_2, gui.متن_gc, i, y - دننه_واټن);
	}
	gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
#د_که_پای
    }

    // Draw a strikethrough line
    إذا (flags & DRAW_STRIKE)
    {
#إذا GTK_CHECK_نسخه(3,0,0)
	cairo_سېټ_کرښه_پلنوالی(cr, 1.0);
	cairo_سېټ_کرښه_cap(cr, CAIRO_کرښه_CAP_BUTT);
	cairo_سېټ_سرچینه_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(ستون), y + 1 - gui.char_لوړوالی/2 + 0.5);
	cairo_کرښه_منزل(cr, FILL_X(ستون + cells), y + 1 - gui.char_لوړوالی/2 + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_gc_سېټ_foreground(gui.متن_gc, gui.spcolor);
	gdk_draw_کرښه(gui.drawarea->کړکۍ_2, gui.متن_gc,
		      FILL_X(ستون), y + 1 - gui.char_لوړوالی/2,
		      FILL_X(ستون + cells), y + 1 - gui.char_لوړوالی/2);
	gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
#د_که_پای
    }

    // Underline: draw a line at the bottom of the character cell.
    إذا (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
#إذا GTK_CHECK_نسخه(3,0,0)
	cairo_سېټ_کرښه_پلنوالی(cr, 1.0);
	cairo_سېټ_کرښه_cap(cr, CAIRO_کرښه_CAP_BUTT);
	cairo_سېټ_سرچینه_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, FILL_X(ستون), y + 0.5);
	cairo_کرښه_منزل(cr, FILL_X(ستون + cells), y + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_draw_کرښه(gui.drawarea->کړکۍ_2, gui.متن_gc,
		      FILL_X(ستون), y,
		      FILL_X(ستون + cells) - 1, y);
#د_که_پای
    }
}

    صحيح
gui_gtk_draw_string(صحيح لیکه, صحيح ستون, char_u *s, صحيح len, صحيح flags)
{
    char_u	*conv_buf = عدم;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    صحيح		convlen;
    char_u	*sp, *bp;
    صحيح		plen;
    صحيح		len_مجموعه;	// return value needs to add up since we are
				// printing substrings
    صحيح		byte_مجموعه;	// byte position in string
    char_u	*cs;		// current *s pointer
    صحيح		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    صحيح		should_need_pango = FALSE;
    صحيح		slen;
    صحيح		is_ligature;
    صحيح		is_utf8;
    char_u	backup_ch;

    إذا (gui.متن_context == عدم || gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع len;

    إذا (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != عدم, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	لكل (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    إذا ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		إذا (new_conv_buf == عدم)
		    أرجع len;
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
    len_مجموعه = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_مجموعه = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    إذا (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    إذا (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    طالما (cs < s + len)
    {
	slen = 0;
	طالما (slen < (len - byte_مجموعه))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    إذا (is_ligature && !needs_pango)
	    {
		إذا ((slen + 1) < (len - byte_مجموعه))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		وإلا
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    إذا ((!is_utf8) && ((slen + 1) < (len - byte_مجموعه)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    إذا (needs_pango != should_need_pango) // mode switch
		اكسر;
	    إذا (needs_pango)
	    {
		إذا (is_ligature)
		{
		    slen++; // ligature char by char
		}
		وإلا // is_utf8
		{
		    إذا ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			طالما ((slen < (len - byte_مجموعه))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    وإلا إذا ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    وإلا إذا ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    وإلا إذا ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    وإلا
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    وإلا
	    {
		slen++; // ascii
	    }
	}

	إذا (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_مجموعه += gui_gtk_draw_string_ext(لیکه, ستون + len_مجموعه, cs, slen, flags,
					    needs_pango);
	إذا (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_مجموعه += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    أرجع len_مجموعه;
}

#إذا GTK_CHECK_نسخه(3,0,0)
ساكن cairo_region_t *dirty_region = عدم;

    ساكن فراغ
کتار_draw_area(صحيح x, صحيح y, صحيح پلنوالی, صحيح لوړوالی)
{
    cairo_rectangle_int_t rect;

    إذا (پلنوالی <= 0 || لوړوالی <= 0 || gui.drawarea == عدم)
	أرجع;

    rect.x = x;
    rect.y = y;
    rect.پلنوالی = پلنوالی;
    rect.لوړوالی = لوړوالی;

    إذا (dirty_region == عدم)
	dirty_region = cairo_region_create_rectangle(&rect);
    وإلا
	cairo_region_union_rectangle(dirty_region, &rect);
}
#د_که_پای

    صحيح
gui_gtk_draw_string_ext(
	صحيح	لیکه,
	صحيح	ستون,
	char_u	*s,
	صحيح	len,
	صحيح	flags,
	صحيح	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    صحيح			ستنه_دننه_واټن = 0;  // column offset in cells
    صحيح			i;
#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_t		*cr;
#د_که_پای

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_دننه_واټن;
    area.y = FILL_Y(لیکه);
    area.پلنوالی	= gui.عدد_ستون_شمېر * gui.char_پلنوالی;
    area.لوړوالی = gui.char_لوړوالی;

#إذا GTK_CHECK_نسخه(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.پلنوالی, area.لوړوالی);
    cairo_clip(cr);
#وإلا
    gdk_gc_سېټ_clip_origin(gui.متن_gc, 0, 0);
    gdk_gc_سېټ_clip_rectangle(gui.متن_gc, &area);
#د_که_پای

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    إذا (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != عدم
	    && !force_pango)
    {
	char_u *p;

	لكل (p = s; p < s + len; ++p)
	    إذا (*p & 0x80)
		اذهب not_ascii;

	pango_glyph_string_سېټ_اندازه(glyphs, len);

	لكل (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#إذا GTK_CHECK_نسخه(3,0,0)
	draw_glyph_string(لیکه, ستون, len, flags, gui.ascii_font, glyphs, cr);
#وإلا
	draw_glyph_string(لیکه, ستون, len, flags, gui.ascii_font, glyphs);
#د_که_پای

	ستنه_دننه_واټن = len;
    }
    وإلا
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*توکی_list;
	صحيح		cluster_پلنوالی;
	صحيح		وروستی_glyph_rbearing;
	صحيح		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	إذا (!utf_valid_string(s, s + len))
	{
	    ستنه_دننه_واټن = len;
	    اذهب skipitall;
	}

	// original width of the current cluster
	cluster_پلنوالی = PANGO_SCALE * gui.char_پلنوالی;

	// right bearing of the last non-composing glyph
	وروستی_glyph_rbearing = PANGO_SCALE * gui.char_پلنوالی;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	إذا (gui.wide_font != عدم)
	    apply_wide_font_attr(s, len, attr_list);

	إذا ((flags & DRAW_BOLD) && gui.font_can_bold)
	    ورننباسه_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
			      attr_list, 0, len);
	إذا (flags & DRAW_ITALIC)
	    ورننباسه_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	توکی_list = pango_itemize(gui.متن_context,
				  (ثابت حرف *)s, 0, len, attr_list, عدم);

	طالما (توکی_list != عدم)
	{
	    Pango_توکی	*توکی;
	    صحيح		توکی_cells = 0; // item length in cells

	    توکی = (Pango_توکی *)توکی_list->معلومات;
	    توکی_list = g_list_delete_link(توکی_list, توکی_list);
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
	    توکی->analysis.level = (توکی->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    توکی->analysis.shape_engine = default_shape_engine;

#که_تعریف_شوی HAVE_PANGO_SHAPE_ډک
	    pango_shape_ډک((ثابت حرف *)s + توکی->دننه_واټن, توکی->اوږدتیا,
		    (ثابت حرف *)s, len, &توکی->analysis, glyphs);
#وإلا
	    pango_shape((ثابت حرف *)s + توکی->دننه_واټن, توکی->اوږدتیا,
			&توکی->analysis, glyphs);
#د_که_پای
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    لكل (i = 0; i < glyphs->عدد_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		إذا (glyph->attr.is_cluster_پیل)
		{
		    صحيح cellcount;

		    cellcount = شمېر_cluster_cells(
			s, توکی, glyphs, i, &cluster_پلنوالی,
			(توکی_list != عدم) ? &وروستی_glyph_rbearing : عدم);

		    إذا (cellcount > 0)
		    {
			صحيح پلنوالی;

			پلنوالی = cellcount * gui.char_پلنوالی * PANGO_SCALE;
			glyph->geometry.x_دننه_واټن +=
					    MAX(0, پلنوالی - cluster_پلنوالی) / 2;
			glyph->geometry.پلنوالی = پلنوالی;
		    }
		    وإلا
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			تیاری_zero_پلنوالی_cluster(توکی, glyph, cells,
						 cluster_پلنوالی,
						 وروستی_glyph_rbearing);
		    }

		    توکی_cells += cellcount;
		    cells = cellcount;
		}
		وإلا إذا (i > 0)
		{
		    صحيح پلنوالی;

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
		    إذا (glyph->geometry.x_دننه_واټن >= 0)
		    {
			glyphs->glyphs[i].geometry.پلنوالی =
					 glyphs->glyphs[i - 1].geometry.پلنوالی;
			glyphs->glyphs[i - 1].geometry.پلنوالی = 0;
		    }
		    پلنوالی = cells * gui.char_پلنوالی * PANGO_SCALE;
		    glyph->geometry.x_دننه_واټن +=
					    MAX(0, پلنوالی - cluster_پلنوالی) / 2;
		}
		وإلا // i == 0 "cannot happen"
		{
		    glyph->geometry.پلنوالی = 0;
		}
	    }

	    //// Aaaaand action! **
#إذا GTK_CHECK_نسخه(3,0,0)
	    draw_glyph_string(لیکه, ستون + ستنه_دننه_واټن, توکی_cells,
			      flags, توکی->analysis.font, glyphs,
			      cr);
#وإلا
	    draw_glyph_string(لیکه, ستون + ستنه_دننه_واټن, توکی_cells,
			      flags, توکی->analysis.font, glyphs);
#د_که_پای

	    pango_توکی_free(توکی);

	    ستنه_دننه_واټن += توکی_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#إذا GTK_CHECK_نسخه(3,0,0)
    draw_under(flags, لیکه, ستون, ستنه_دننه_واټن, cr);
#وإلا
    draw_under(flags, لیکه, ستون, ستنه_دننه_واټن);
#د_که_پای

    pango_glyph_string_free(glyphs);

#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_destroy(cr);
    کتار_draw_area(FILL_X(ستون), FILL_Y(لیکه),
	    ستنه_دننه_واټن * gui.char_پلنوالی + 1, gui.char_لوړوالی);
#وإلا
    gdk_gc_سېټ_clip_rectangle(gui.متن_gc, عدم);
#د_که_پای

    أرجع ستنه_دننه_واټن;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیګانې[i].کلی_sym != 0; i++)
	إذا (name[0] == special_کلیګانې[i].code0
		&& name[1] == special_کلیګانې[i].code1)
	    أرجع OK;
    أرجع FAIL;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحيح
gui_get_x11_windis(کړکۍ *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
    {
	*dis = dpy;
	*win = GDK_کړکۍ_XID(gtk_widget_get_کړکۍ(gui.mainwin));
	أرجع OK;
    }

    *dis = عدم;
    *win = 0;
    أرجع FAIL;
}
#د_که_پای

    Display *
gui_mch_get_display(فراغ)
{
    إذا (gui.mainwin != عدم && gtk_widget_get_کړکۍ(gui.mainwin) != عدم
#إذا GTK_CHECK_نسخه(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#د_که_پای
	)
	أرجع GDK_کړکۍ_XDISPLAY(gtk_widget_get_کړکۍ(gui.mainwin));
    وإلا
	أرجع عدم;
}

    فراغ
gui_mch_beep(فراغ)
{
    GdkDisplay *display;

    إذا (gui.mainwin != عدم && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    وإلا
	display = gdk_display_get_default();

    إذا (display != عدم)
	gdk_display_beep(display);
}

    فراغ
gui_mch_flash(صحيح msec)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    // TODO Replace GdkGC with Cairo
    (فراغ)msec;
#وإلا
    GdkGCValues	ارزښتونه;
    GdkGC	*invert_gc;

    إذا (gui.drawarea->کړکۍ_2 == عدم)
	أرجع;

    ارزښتونه.foreground.pixel = gui.norm_pixel ^ gui.شا_pixel;
    ارزښتونه.background.pixel = gui.norm_pixel ^ gui.شا_pixel;
    ارزښتونه.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_ارزښتونه(gui.drawarea->کړکۍ_2,
				       &ارزښتونه,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_سېټ_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحيح)Columns) + gui.border_دننه_واټن,
		       FILL_Y((صحيح)لیکې) + gui.border_دننه_واټن);

    gui_mch_flush();
    ui_delay((اوږد)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحيح)Columns) + gui.border_دننه_واټن,
		       FILL_Y((صحيح)لیکې) + gui.border_دننه_واټن);

    gdk_gc_destroy(invert_gc);
#د_که_پای
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    ثابت GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_پلنوالی, nr * gui.char_لوړوالی
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);

    cairo_سېټ_سرچینه_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# إذا CAIRO_نسخه >= CAIRO_نسخه_ENCODE(1,9,2)
    cairo_سېټ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# وإلا
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
    cairo_fill(cr);

    cairo_destroy(cr);

    کتار_draw_area(rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
#وإلا
    GdkGCValues ارزښتونه;
    GdkGC *invert_gc;

    إذا (gui.drawarea->کړکۍ_2 == عدم)
	أرجع;

    ارزښتونه.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_ارزښتونه(gui.drawarea->کړکۍ_2,
				       &ارزښتونه,
				       GDK_GC_FUNCTION);
    gdk_gc_سېټ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_پلنوالی, (nr) * gui.char_لوړوالی);
    gdk_gc_destroy(invert_gc);
#د_که_پای
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    gtk_کړکۍ_iconify(GTK_کړکۍ(gui.mainwin));
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سېټ_foreground(فراغ)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_کړکۍ_سېټ_keep_above(GTK_کړکۍ(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_کړکۍ_present(GTK_کړکۍ(gui.mainwin));
    gui_may_flush();
    gtk_کړکۍ_سېټ_keep_above(GTK_کړکۍ(gui.mainwin), FALSE);
    gui_may_flush();
}
#د_که_پای

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحيح		i = 1;
#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_t    *cr;
#د_که_پای

    إذا (gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع;

#إذا GTK_CHECK_نسخه(3,0,0)
    cr = cairo_create(gui.surface);
#د_که_پای

    gui_mch_سېټ_fg_color(color);

#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_سېټ_سرچینه_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#وإلا
    gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
#د_که_پای
    إذا (mb_lefthalve(gui.لیکه, gui.ستون))
	i = 2;
#إذا GTK_CHECK_نسخه(3,0,0)
    cairo_سېټ_کرښه_پلنوالی(cr, 1.0);
    cairo_سېټ_کرښه_cap(cr, CAIRO_کرښه_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.ستون) + 0.5, FILL_Y(gui.لیکه) + 0.5,
	    i * gui.char_پلنوالی - 1, gui.char_لوړوالی - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#وإلا
    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, gui.متن_gc,
	    FALSE,
	    FILL_X(gui.ستون), FILL_Y(gui.لیکه),
	    i * gui.char_پلنوالی - 1, gui.char_لوړوالی - 1);
#د_که_پای
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
{
    إذا (gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع;

    gui_mch_سېټ_fg_color(color);

#إذا GTK_CHECK_نسخه(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_سېټ_سرچینه_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ښی ? FILL_X(gui.ستون + 1) - w :
# endif
	    FILL_X(gui.ستون), FILL_Y(gui.لیکه) + gui.char_لوړوالی - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, gui.متن_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ښی ? FILL_X(gui.ستون + 1) - w :
# endif
	    FILL_X(gui.ستون),
	    FILL_Y(gui.لیکه) + gui.char_لوړوالی - h,
	    w, h);
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    فراغ
gui_mch_update(فراغ)
{
    صحيح cnt = 0;	// prevent endless loop
    طالما (g_main_context_pending(عدم) && !vim_is_ننوت_buf_ډک()
	    && ++cnt < 100)
    {
#إذا GTK_CHECK_نسخه(3,0,0)
	صحيح prio = 0;
	g_main_context_prepare(عدم, &prio);
	// peek internal scheduling of redraw
	إذا (prio == GDK_لومړیتوب_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#د_که_پای
	g_main_context_iteration(عدم, TRUE);
    }
}

    ساكن timeout_cb_type
ننوت_timer_cb(gpointer معلومات)
{
    صحيح *timed_بهر = (صحيح *) معلومات;

    // Just inform the caller about the occurrence of it
    *timed_بهر = TRUE;

    أرجع FALSE;		// don't happen again
}

#که_تعریف_شوی FEAT_دنده_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer معلومات ناکارول_شوی)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    أرجع TRUE;		// repeat
}
#د_که_پای

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    صحيح
gui_mch_wait_for_chars(اوږد wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_بهر;
    صحيح		retval = FAIL;
#که_تعریف_شوی FEAT_دنده_CHANNEL
    guint	channel_timer = 0;
#د_که_پای

    timed_بهر = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    إذا (wtime >= 0)
	timer = timeout_جمع_کړه(wtime == 0 ? 1L : wtime,
						   ننوت_timer_cb, &timed_بهر);
    وإلا
	timer = 0;

#که_تعریف_شوی FEAT_دنده_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_جمع_کړه(20, channel_poll_cb, عدم);
#د_که_پای

    focus = gui.دننه_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.دننه_focus != focus)
	{
	    إذا (gui.دننه_focus)
		gui_mch_پیل_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.دننه_focus;
	}

#که_تعریف_شوی پیغام_کتار
# ifdef FEAT_TIMERS
	did_جمع_کړه_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جمع_کړه_timer)
	    // Need to recompute the waiting time.
	    اذهب theend;
# endif
#د_که_پای

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	إذا (!ننوت_شته())
	{
	    g_main_context_iteration(عدم, TRUE);
#که_تعریف_شوی GDK_WINDOWING_WAYLAND
    إذا (gui.is_wayland && clip_ستوری.state == SELECT_دننه_پرمختګ)
	gui_may_flush();
#د_که_پای
	}

	// Got char, return immediately
	إذا (ننوت_شته())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_بهر);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_بهر)
	timeout_remove(timer);
#که_تعریف_شوی FEAT_دنده_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#د_که_پای

    أرجع retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    فراغ
gui_mch_flush(فراغ)
{
    إذا (gui.mainwin == عدم || !gtk_widget_get_realized(gui.mainwin))
	أرجع;
#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (dirty_region != عدم && gui.drawarea != عدم)
    {
	gtk_widget_کتار_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = عدم;
    }
#وإلا
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       أرجع;
#د_که_پای
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_بلاک(صحيح row1arg, صحيح col1arg, صحيح row2arg, صحيح col2arg)
{

    صحيح col1 = check_ستون(col1arg);
    صحيح col2 = check_ستون(col2arg);
    صحيح row1 = check_لیکه(row1arg);
    صحيح row2 = check_لیکه(row2arg);

#إذا GTK_CHECK_نسخه(3,0,0)
    إذا (gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع;
#وإلا
    GdkColor color;

    إذا (gui.drawarea->کړکۍ_2 == عدم)
	أرجع;

    color.pixel = gui.شا_pixel;
#د_که_پای

#إذا GTK_CHECK_نسخه(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ثابت GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_پلنوالی + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_لوړوالی
	};
	cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخه(3,22,2)
	سېټ_cairo_سرچینه_rgba_from_color(cr, gui.شا_pixel);
# وإلا
	Gdk_کړکۍ * ثابت win = gtk_widget_get_کړکۍ(gui.drawarea);
	cairo_نمونه_t * ثابت pat = gdk_کړکۍ_get_background_نمونه(win);
	إذا (pat != عدم)
	    cairo_سېټ_سرچینه(cr, pat);
	وإلا
	    سېټ_cairo_سرچینه_rgba_from_color(cr, gui.شا_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
	cairo_fill(cr);
	cairo_destroy(cr);

	کتار_draw_area(rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سېټ_foreground(gui.متن_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->کړکۍ_2, gui.متن_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_پلنوالی
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_لوړوالی);
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
}

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
gui_gtk_کړکۍ_clear(Gdk_کړکۍ *win)
{
    ثابت GdkRectangle rect = {
	0, 0, gdk_کړکۍ_get_پلنوالی(win), gdk_کړکۍ_get_لوړوالی(win)
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخه(3,22,2)
    سېټ_cairo_سرچینه_rgba_from_color(cr, gui.شا_pixel);
# وإلا
    cairo_نمونه_t * ثابت pat = gdk_کړکۍ_get_background_نمونه(win);
    إذا (pat != عدم)
	cairo_سېټ_سرچینه(cr, pat);
    وإلا
	سېټ_cairo_سرچینه_rgba_from_color(cr, gui.شا_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
    cairo_fill(cr);
    cairo_destroy(cr);

    کتار_draw_area(rect.x, rect.y, rect.پلنوالی, rect.لوړوالی);
}
#وإلا
# define gui_gtk_کړکۍ_clear(win)  gdk_کړکۍ_clear(win)
#د_که_پای

    فراغ
gui_mch_clear_all(فراغ)
{
    إذا (gtk_widget_get_کړکۍ(gui.drawarea) != عدم)
	gui_gtk_کړکۍ_clear(gtk_widget_get_کړکۍ(gui.drawarea));
}

#إذا !GTK_CHECK_نسخه(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ساكن فراغ
check_copy_area(فراغ)
{
    GdkEvent        *event;
    صحيح                expose_شمېر;
    إذا (gui.visibility != GDK_VISIBILITY_PARTIAL)
	أرجع;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    افعل
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->کړکۍ_2);
	إذا (event == عدم)
	    اكسر; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.پلنوالی, event->expose.area.لوړوالی);
	expose_شمېر = event->expose.شمېر;
	gdk_event_free(event);
    }
    طالما (expose_شمېر > 0); // more events follow
    gui_can_update_cursor();
}
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)

#إذا GTK_CHECK_نسخه(3,0,0)
    ساكن فراغ
gui_gtk_surface_copy_rect(صحيح dest_x, صحيح dest_y,
			  صحيح src_x,  صحيح src_y,
			  صحيح پلنوالی,  صحيح لوړوالی)
{
    cairo_t * ثابت cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ساكن cairo_surface_t *scroll_scratch = عدم;
    ساكن صحيح scratch_w = 0, scratch_h = 0;
    صحيح وروستی_لیکه = لیکې - 1, وروستی_لیکه_y = وروستی_لیکه * gui.char_لوړوالی;
    bool وروستی_لیکه_overlap = (dest_y + لوړوالی) > وروستی_لیکه_y;
    إذا (gui.is_wayland && ( !(State & ډېرپېښ_CMDLINE) || !وروستی_لیکه_overlap) )
    {
	إذا (dest_y < src_y) // scroll up
	{
	    cairo_سېټ_operator(cr, CAIRO_OPERATOR_سرچینه);
	    cairo_سېټ_سرچینه_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, پلنوالی, لوړوالی);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	وإلا
	{   //  reuse surface when scrolling, only realloc if larger
	    إذا (scroll_scratch == عدم || پلنوالی > scratch_w || لوړوالی > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), پلنوالی, لوړوالی);
		scratch_w = پلنوالی, scratch_h = لوړوالی;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_سېټ_سرچینه_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_سېټ_operator(cr, CAIRO_OPERATOR_سرچینه);
	    cairo_rectangle(cr, dest_x, dest_y, پلنوالی, لوړوالی);
	    cairo_clip(cr);
	    cairo_سېټ_سرچینه_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    وإلا
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, پلنوالی, لوړوالی);
	cairo_clip(cr);
	cairo_ټېل_ډله(cr);
	cairo_سېټ_سرچینه_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_راوباسه_ډله_منزل_سرچینه(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#د_که_پای

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_کرښې(صحيح لیکه, صحيح عدد_کرښې)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_ښی - gui.scroll_region_کیڼ + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - لیکه + 1;
    ثابت صحيح src_nrows = nrows - عدد_کرښې;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه + عدد_کرښې),
	    gui.char_پلنوالی * ncols + 1,     gui.char_لوړوالی * src_nrows);
    gui_clear_بلاک(
	    gui.scroll_region_bot - عدد_کرښې + 1, gui.scroll_region_کیڼ,
	    gui.scroll_region_bot,		   gui.scroll_region_ښی);
    کتار_draw_area(
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    gui.char_پلنوالی * ncols + 1,	gui.char_لوړوالی * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

    gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سېټ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_کړکۍ_copy_area(gui.drawarea->کړکۍ_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    gui.drawarea->کړکۍ_2,
	    FILL_X(gui.scroll_region_کیڼ),
	    FILL_Y(لیکه + عدد_کرښې),
	    gui.char_پلنوالی * (gui.scroll_region_ښی
					    - gui.scroll_region_کیڼ + 1) + 1,
	    gui.char_لوړوالی * (gui.scroll_region_bot - لیکه - عدد_کرښې + 1));

    gui_clear_بلاک(gui.scroll_region_bot - عدد_کرښې + 1,
						       gui.scroll_region_کیڼ,
		    gui.scroll_region_bot, gui.scroll_region_ښی);
    check_copy_area();
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_ورننباسه_کرښې(صحيح لیکه, صحيح عدد_کرښې)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_ښی - gui.scroll_region_کیڼ + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - لیکه + 1;
    ثابت صحيح src_nrows = nrows - عدد_کرښې;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه + عدد_کرښې),
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    gui.char_پلنوالی * ncols + 1,     gui.char_لوړوالی * src_nrows);
    gui_clear_بلاک(
	    لیکه,		 gui.scroll_region_کیڼ,
	    لیکه + عدد_کرښې - 1, gui.scroll_region_ښی);
    کتار_draw_area(
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    gui.char_پلنوالی * ncols + 1,	gui.char_لوړوالی * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

    gdk_gc_سېټ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سېټ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_کړکۍ_copy_area(gui.drawarea->کړکۍ_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه + عدد_کرښې),
	    gui.drawarea->کړکۍ_2,
	    FILL_X(gui.scroll_region_کیڼ), FILL_Y(لیکه),
	    gui.char_پلنوالی * (gui.scroll_region_ښی
					    - gui.scroll_region_کیڼ + 1) + 1,
	    gui.char_لوړوالی * (gui.scroll_region_bot - لیکه - عدد_کرښې + 1));

    gui_clear_بلاک(لیکه, gui.scroll_region_کیڼ,
				لیکه + عدد_کرښې - 1, gui.scroll_region_ښی);
    check_copy_area();
#د_که_پای // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	موخه;
    بینښه	i;
    time_t	پیل;

    لكل (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	إذا (!clip_html && selection_targets[i].info == موخه_HTML)
	    استمر;
	received_selection = RS_NONE;
	موخه = gdk_atom_intern(selection_targets[i].موخه, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, موخه,
			      (guint32)GDK_CURRENT_وخت);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	پیل = وخت(عدم);
	طالما (received_selection == RS_NONE && وخت(عدم) < پیل + 3)
	    g_main_context_iteration(عدم, TRUE);	// wait for selection_received_cb

	إذا (received_selection != RS_FAIL)
	    أرجع;
    }

    إذا (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_کړکۍ_XDISPLAY(gtk_widget_get_کړکۍ(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd ناکارول_شوی)
{
    إذا (دننه_selection_clear_event)
	أرجع;

    gtk_selection_owner_سېټ(عدم, cbd->gtk_sel_atom, gui.event_وخت);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    صحيح
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    إذا (is_autocmd_blocked())
	أرجع OK;

    صحيح success;

    success = gtk_selection_owner_سېټ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_وخت);
    // don't update on every visual selection change
    إذا (!(cbd->owned && VIsual_فعال))
	gui_gtk_سېټ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    أرجع (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    فراغ
clip_mch_سېټ_selection(Clipboard_T *cbd ناکارول_شوی)
{
}

#إذا defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    صحيح
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    أرجع gdk_selection_owner_get(cbd->gtk_sel_atom) != عدم;
}
#د_که_پای


#إذا defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    فراغ
gui_mch_menu_grey(vimmenu_T *menu, صحيح grey)
{
    إذا (menu->id == عدم)
	أرجع;

    إذا (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    إذا (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_سېټ_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    فراغ
gui_mch_menu_hidden(vimmenu_T *menu, صحيح hidden)
{
    إذا (menu->id == 0)
	أرجع;

    إذا (hidden)
    {
	إذا (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    وإلا
    {
	إذا (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    فراغ
gui_mch_draw_menubar(فراغ)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#د_که_پای // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    إذا (sb->id == عدم)
	أرجع;

    gtk_widget_سېټ_visible(sb->id, flag);
    update_کړکۍ_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#إذا GTK_CHECK_نسخه(3,0,0)
    أرجع (long_u)pixel;
#وإلا
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (بینښه اوږد)pixel, &color);

    أرجع (guicolor_T)(
	    (((بینښه)color.red   & 0xff00) << 8)
	 |  ((بینښه)color.green & 0xff00)
	 | (((بینښه)color.blue  & 0xff00) >> 8));
#د_که_پای
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    gui_gtk_get_اشاره_ګر(gui.drawarea, x, y, عدم);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
	XWarp_اشاره_ګر(dpy, (کړکۍ)0,
		GDK_کړکۍ_XID(gtk_widget_get_کړکۍ(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#که_تعریف_شوی FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساكن صحيح وروستی_shape = 0;
#د_که_پای

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشاره_ګر_hidden == hide)
	أرجع;

    gui.اشاره_ګر_hidden = hide;
    إذا (gtk_widget_get_کړکۍ(gui.drawarea) && gui.blank_اشاره_ګر != عدم)
    {
	إذا (hide)
	    gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(gui.drawarea),
		    gui.blank_اشاره_ګر);
	وإلا
#که_تعریف_شوی FEAT_MOUSESHAPE
	    mch_سېټ_mouse_shape(وروستی_shape);
#وإلا
	gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(gui.drawarea), عدم);
#د_که_پای
    }
}

#إذا defined(FEAT_MOUSESHAPE)

# إذا GTK_CHECK_نسخه(3,0,0)
ساكن ثابت حرف * mshape_css_نومونه[] =
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
# وإلا
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ساكن ثابت صحيح mshape_ids[] =
{
    GDK_کیڼ_PTR,		// arrow
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
    GDK_ښی_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_کیڼ_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    فراغ
mch_سېټ_mouse_shape(صحيح shape)
{
    GdkCursor	   *c;
    صحيح		   id;                    // Only id or css_name is used.
# إذا GTK_CHECK_نسخه(3,0,0)
    ثابت حرف     *css_name = "default";
    GdkDisplay     *disp;
# endif

    إذا (gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشاره_ګر_hidden)
	gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(gui.drawarea),
		gui.blank_اشاره_ګر);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    إذا (id >= GDK_وروستی_CURSOR)
		id = GDK_کیڼ_PTR;
	    وإلا
		id &= ~1;	// they are always even (why?)
	}
# إذا GTK_CHECK_نسخه(3,0,0)
	وإلا إذا (shape < (صحيح)ARRAY_اوږدتیا(mshape_css_نومونه))
	    css_name = mshape_css_نومونه[shape];
# وإلا
	وإلا إذا (shape < (صحيح)ARRAY_اوږدتیا(mshape_ids))
	    id = mshape_ids[shape];
# endif
	وإلا
	    أرجع;
# إذا GTK_CHECK_نسخه(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# وإلا
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_کړکۍ_سېټ_cursor(gtk_widget_get_کړکۍ(gui.drawarea), c);
# إذا GTK_CHECK_نسخه(3,0,0)
	g_object_unref(G_OBJECT(c));
# وإلا
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    إذا (shape != MSHAPE_HIDE)
	وروستی_shape = shape;
}
#د_که_پای // FEAT_MOUSESHAPE


#إذا defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_پلنوالی  (2 * gui.char_پلنوالی)
# define SIGN_لوړوالی (gui.char_لوړوالی)
# define SIGN_ASPECT ((مزدوج)SIGN_لوړوالی / (مزدوج)SIGN_پلنوالی)

    فراغ
gui_mch_drawsign(صحيح لیکه, صحيح ستون, صحيح typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    إذا (sign == عدم || gui.drawarea == عدم
	    || gtk_widget_get_کړکۍ(gui.drawarea) == عدم)
	أرجع;

    صحيح پلنوالی;
    صحيح لوړوالی;
    صحيح xoffset;
    صحيح yoffset;
    صحيح need_scale;

    پلنوالی  = gdk_pixbuf_get_پلنوالی(sign);
    لوړوالی = gdk_pixbuf_get_لوړوالی(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (پلنوالی > SIGN_پلنوالی + 2
	    || لوړوالی != SIGN_لوړوالی
	    || (پلنوالی < 3 * SIGN_پلنوالی / 4
		&& لوړوالی < 3 * SIGN_لوړوالی / 4));
    إذا (need_scale)
    {
	مزدوج  aspect;
	صحيح	    w = پلنوالی;
	صحيح	    h = لوړوالی;

	// Keep the original aspect ratio
	aspect = (مزدوج)لوړوالی / (مزدوج)پلنوالی;
	پلنوالی  = (مزدوج)SIGN_پلنوالی * SIGN_ASPECT / aspect;
	پلنوالی  = MIN(پلنوالی, SIGN_پلنوالی);
	إذا (((مزدوج)(MAX(لوړوالی, SIGN_لوړوالی)) /
		    (مزدوج)(MIN(لوړوالی, SIGN_لوړوالی))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    لوړوالی = (مزدوج)SIGN_لوړوالی * SIGN_ASPECT / aspect;
	    لوړوالی = MIN(لوړوالی, SIGN_لوړوالی);
	}
	وإلا
	    لوړوالی = (مزدوج)پلنوالی * aspect;

	إذا (w == پلنوالی && h == لوړوالی)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	وإلا
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, پلنوالی, لوړوالی,
		    GDK_INTERP_BILINEAR);
	    إذا (sign == عدم)
		أرجع; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (پلنوالی  - SIGN_پلنوالی)  / 2;
    yoffset = (لوړوالی - SIGN_لوړوالی) / 2;

# إذا GTK_CHECK_نسخه(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_پلنوالی, SIGN_لوړوالی);
	bg_cr = cairo_create(bg_surf);
	cairo_سېټ_سرچینه_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_پلنوالی, SIGN_لوړوالی);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_سېټ_سرچینه_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_سېټ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_سېټ_سرچینه_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_سېټ_سرچینه_surface(cr, sign_surf, FILL_X(ستون), FILL_Y(لیکه));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	کتار_draw_area(
		FILL_X(ستون), FILL_Y(ستون), پلنوالی, لوړوالی);

    }
# وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سېټ_foreground(gui.متن_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->کړکۍ_2,
	    gui.متن_gc,
	    TRUE,
	    FILL_X(ستون),
	    FILL_Y(لیکه),
	    SIGN_پلنوالی,
	    SIGN_لوړوالی);

    gdk_pixbuf_render_منزل_drawable_alpha(sign,
	    gui.drawarea->کړکۍ_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(ستون) - MIN(0, xoffset),
	    FILL_Y(لیکه) - MIN(0, yoffset),
	    MIN(پلنوالی,	SIGN_پلنوالی),
	    MIN(لوړوالی, SIGN_لوړوالی),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_نورمال,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    إذا (need_scale)
	g_object_unref(sign);
}

    فراغ *
gui_mch_ثبتوونکی_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.دننه_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = عدم;
	char_u	    *پیغام_2;

	sign = gdk_pixbuf_new_from_file((ثابت حرف *)signfile, &error);

	إذا (error == عدم)
	    أرجع sign;

	پیغام_2 = (char_u *)error->پیغام_2;

	إذا (پیغام_2 != عدم && ننوت_conv.vc_type != CONV_NONE)
	    پیغام_2 = string_convert(&ننوت_conv, پیغام_2, عدم);

	إذا (پیغام_2 != عدم)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", پیغام_2);

	    إذا (ننوت_conv.vc_type != CONV_NONE)
		vim_free(پیغام_2);
	}
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

#د_که_پای // FEAT_SIGN_ICONS

#إذا defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

تعريفنوع بنية
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
    ساكن فراغ
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->معلومات;
    uint8_t *dst;
    صحيح	    w = img->پلنوالی;
    صحيح	    h = img->لوړوالی;
    صحيح	    stride;
    صحيح	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_معلومات(surf);
    stride = cairo_image_surface_get_stride(surf);

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
    {
	لكل (y = 0; y < h; ++y)
	{
	    بینښه صحيح *p = (بینښه صحيح *)(dst + y * stride);

	    لكل (x = 0; x < w; ++x)
	    {
		صحيح	idx = (y * w + x) * 4;
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
    وإلا
    {
	لكل (y = 0; y < h; ++y)
	{
	    بینښه صحيح *p = (بینښه صحيح *)(dst + y * stride);

	    لكل (x = 0; x < w; ++x)
	    {
		صحيح	idx = (y * w + x) * 3;
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

    صحيح
image_gui_init(image_T *img)
{
    // We only support GTK3
# إذا GTK_CHECK_نسخه(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->پلنوالی, img->لوړوالی);

    إذا (cairo_surface_حالت(ctx->surf) != CAIRO_حالت_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	اذهب fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_معلومات = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    أرجع OK;
fail:
    vim_free(ctx);
    أرجع FAIL;
# وإلا
    أرجع FAIL;
# endif
}

    فراغ
image_gui_uninit(image_T *img)
{
# إذا GTK_CHECK_نسخه(3,0,0)
    image_cairo_T *ctx = img->backend_معلومات;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    صحيح
image_placement_gui_init(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
# إذا GTK_CHECK_نسخه(3,0,0)
    أرجع OK;
# وإلا
    أرجع FAIL;
# endif
}

    فراغ
image_placement_gui_uninit(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
}

    فراغ
image_placement_gui_draw(image_placement_T *ځای_پرځای_کړه)
{
# إذا GTK_CHECK_نسخه(3,0,0)
    image_T		    *img = ځای_پرځای_کړه->img;
    image_cairo_T	    *ictx = img->backend_معلومات;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&ځای_پرځای_کړه->visible, &n_rects);
    إذا (rects == عدم || n_rects == 0)
	أرجع;

    cairo_surface_سېټ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		لیکه, ستون;
	صحيح		posx, posy;
	صحيح		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(ځای_پرځای_کړه, rect, &لیکه, &ستون, &x, &y, &w, &h, false);
	posx = FILL_X(ستون);
	posy = FILL_Y(لیکه);

	cairo_save(cr);
	cairo_سېټ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_سېټ_سرچینه_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	کتار_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    فراغ
image_placement_gui_clear(image_placement_T *ځای_پرځای_کړه ناکارول_شوی)
{
}

#د_که_پای // FEAT_IMAGE
