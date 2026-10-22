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
#جے_تعریف_شدہ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_GNOME
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
خارجي فراغ bonobo_dock_شے_سیٹ_behavior(Bonobo_Dock_شے *dock_شے, Bonobo_Dock_شے_Behavior beh);
#جے_انت

#إذا defined(FEAT_GUI_GTK)
# إذا GTK_CHECK_نسخہ(3,0,0)
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
#جے_انت

#جے_تعریف_شدہ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#جے_انت

/*
 * Easy-to-use macro for multihead support.
 */
#تعریف_کرو GET_X_ATOM(atom)	gdk_x11_atom_منزل_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
تعداد
{
    نشانہ_TYPE_NONE,
    نشانہ_UTF8_STRING,
    نشانہ_STRING,
    نشانہ_COMPOUND_متن,
    نشانہ_HTML,
    نشانہ_متن,
    نشانہ_متن_URI_LIST,
    نشانہ_متن_PLAIN,
    نشانہ_متن_PLAIN_UTF8,
    نشانہ_VIM_MT,
    نشانہ_VIM,
    نشانہ_VIMENC_MT,
    نشانہ_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_نشانہ_اندراج selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, نشانہ_VIMENC},
    {VIM_ATOM_NAME,	0, نشانہ_VIM},
    {VIMENC_MIMETYPE_NAME,  0, نشانہ_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, نشانہ_VIM_MT},
    {"text/html",	0, نشانہ_HTML},
    {"UTF8_STRING",	0, نشانہ_UTF8_STRING},
    {"COMPOUND_TEXT",	0, نشانہ_COMPOUND_متن},
    {"TEXT",		0, نشانہ_متن},
    {"STRING",		0, نشانہ_STRING},
    {"text/plain;charset=utf-8", 0, نشانہ_متن_PLAIN_UTF8},
    {"text/plain",	0, نشانہ_متن_PLAIN}
};
#تعریف_کرو N_SELECTION_TARGETS ARRAY_لمائی(selection_targets)

#جے_تعریف_شدہ FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_نشانہ_اندراج dnd_targets[] =
{
    {"text/uri-list",	0, نشانہ_متن_URI_LIST},
    {"text/html",	0, نشانہ_HTML},
    {"UTF8_STRING",	0, نشانہ_UTF8_STRING},
    {"STRING",		0, نشانہ_STRING},
    {"text/plain",	0, نشانہ_متن_PLAIN}
};
# define N_DND_TARGETS ARRAY_لمائی(dnd_targets)
#جے_انت


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#تعریف_کرو DEFAULT_FONT	"Monospace 12"

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#جے_انت

#إذا !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ساكن GdkAtom wm_protocols_atom = GDK_NONE;
ساكن GdkAtom save_yourself_atom = GDK_NONE;
#جے_انت

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
ساكن بنية special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدیں[] =
{
    {GDK_اتے,		'k', 'u'},
    {GDK_تھلے,		'k', 'd'},
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
#جے_تعریف_شدہ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#جے_انت
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_پٹھ_Space,	'k', 'b'},
    {GDK_داخل_کرو,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_پٹھ_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_سرا,		'@', '7'},
    {GDK_پہلوں,		'k', 'P'},
    {GDK_اگلا,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_بایاں,	'k', 'l'},
    {GDK_KP_دایاں,	'k', 'r'},
    {GDK_KP_اتے,		'k', 'u'},
    {GDK_KP_تھلے,	'k', 'd'},
    {GDK_KP_داخل_کرو,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_سرا,	'K', '4'},
    {GDK_KP_پہلوں,	'K', '3'},  // page up
    {GDK_KP_اگلا,	'K', '5'},  // page down

    {GDK_KP_جوڑو,	'K', '6'},
    {GDK_KP_گھٹاؤ,	'K', '7'},
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
تعريفنوع بنية
{
    ثابت حرف	    *name;
    بنانشانی صحيح    flags;
}
cmdline_option_T;

ساكن ثابت cmdline_option_T cmdline_options[] =
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
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_قدر},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
#جے_انت
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
#جے_تعریف_شدہ FEAT_GUI_GNOME
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
# إذا 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#جے_انت
    {عدم, 0}
};

ساكن صحيح    gui_argc = 0;
ساكن حرف **gui_argv = عدم;

ساكن ثابت حرف *role_دلیل = عدم;
#إذا defined(USE_GNOME_SESSION)
ساكن ثابت حرف *restart_command = عدم;
ساكن       حرف *abs_restart_command = عدم;
#جے_انت
ساكن صحيح لبھیا_iconic_arg = FALSE;

#جے_تعریف_شدہ FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ساكن صحيح using_gnome = 0;
#وإلا
# define using_gnome 0
#جے_انت

// Comment out the following line to ignore code for resize history tracking.
#تعریف_کرو TRACK_RESIZE_HISTORY
#جے_تعریف_شدہ TRACK_RESIZE_HISTORY
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
    صحيح ورتیا;		// If true, can't match for discard. Only matches once.
    صحيح چوڑائی;
    صحيح اچائی;
    صحيح from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحيح seq;		// for ch_log messages
# endif
    بنية resize_history *اگلا;
} resize_hist_T;

// never NULL during execution
ساكن resize_hist_T *latest_resize_hist;
// list of stale resize requests
ساكن resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ساكن صحيح mch_csd_چوڑائی = 0;
ساكن صحيح mch_csd_اچائی = 0;

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
alloc_resize_hist(صحيح چوڑائی, صحيح اچائی, صحيح from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->چوڑائی = چوڑائی;
    new_hist->اچائی = اچائی;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->اگلا = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(عدم, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, چوڑائی, اچائی, (صحيح)Columns, (صحيح)صفاں);
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
	latest_resize_hist->ورتیا = TRUE;
    طالما (old_resize_hists != عدم)
    {
	resize_hist_T *اگلا_hist = old_resize_hists->اگلا;

	vim_free(old_resize_hists);
	old_resize_hists = اگلا_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define میل_چوڑائی_اچائی(hist, w, h) \
	  (!hist->ورتیا && hist->چوڑائی == w && hist->اچائی == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ساكن صحيح
میل_stale_چوڑائی_اچائی(صحيح چوڑائی, صحيح اچائی)
{
    resize_hist_T *hist = old_resize_hists;

    لكل (hist = old_resize_hists; hist != عدم; hist = hist->اگلا)
	إذا (میل_چوڑائی_اچائی(hist, چوڑائی, اچائی))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(عدم, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->ورتیا = TRUE;
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
#جے_انت

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

	إذا (mch_بھریا_Name((char_u *)argv[0], buf, (صحيح)حجم(buf), TRUE) == OK)
	{
	    abs_restart_command = (حرف *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#جے_انت

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
	    len = لڑی_لمبائی(option->name);

	    إذا (لڑی_مقابلہ(argv[i], option->name, len) == 0)
	    {
		إذا (argv[i][len] == '\0')
		    اكسر;
		// allow --foo=bar style
		إذا (argv[i][len] == '=' && (option->flags & ARG_HAS_قدر))
		    اكسر;
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		إذا (vim_strchr((char_u *)":=", argv[i][len]) != عدم
			&& (option->flags & ARG_اشاریہ_نقاب) == ARG_NETBEANS)
		    اكسر;
#جے_انت
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
	    حرف *قدر = عدم;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    إذا (option->flags & ARG_HAS_قدر)
	    {
		إذا (argv[i][len] == '=')
		    قدر = &argv[i][len + 1];
		وإلا إذا (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    قدر = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    بدل (option->flags & ARG_اشاریہ_نقاب)
	    {
		حالة ARG_الٹ:
		    لبھیا_الٹ_arg = TRUE;
		    اكسر;
		حالة ARG_NOREVERSE:
		    لبھیا_الٹ_arg = FALSE;
		    اكسر;
		حالة ARG_FONT:
		    font_دلیل = قدر;
		    اكسر;
		حالة ARG_GEOMETRY:
		    إذا (قدر != عدم)
			gui.geom = vim_strsave((char_u *)قدر);
		    اكسر;
		حالة ARG_BACKGROUND:
		    background_دلیل = قدر;
		    اكسر;
		حالة ARG_FOREGROUND:
		    foreground_دلیل = قدر;
		    اكسر;
		حالة ARG_ICONIC:
		    لبھیا_iconic_arg = TRUE;
		    اكسر;
		حالة ARG_ROLE:
		    role_دلیل = قدر; // used later in gui_mch_open()
		    اكسر;
#جے_تعریف_شدہ FEAT_NETBEANS_INTG
		حالة ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    اكسر;
#جے_انت
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
		إذا ((option->flags & ARG_HAS_قدر)
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
#جے_انت

#إذا !GTK_CHECK_نسخہ(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ساكن gint
visibility_event(GtkWidget *widget بن_ورتیا,
		 GdkEventVisibility *event,
		 gpointer انکڑے بن_ورتیا)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    إذا (gui.متن_gc != عدم)
	gdk_gc_سیٹ_exposures(gui.متن_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    أرجع FALSE;
}
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن gboolean
draw_event(GtkWidget *widget بن_ورتیا,
	   cairo_t   *cr,
	   gpointer   user_انکڑے بن_ورتیا)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    باہرلا_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_سیٹ_سروت_surface(cr, gui.surface, 0, 0);
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

# إذا GTK_CHECK_نسخہ(3,10,0)
    ساكن gboolean
scale_ضارب_event(GtkWidget *widget,
		   GParamSpec* pspec بن_ورتیا,
		   gpointer   user_انکڑے بن_ورتیا)
{
    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    صحيح	    w, h;
    gtk_کھڑکی_get_ناپ(GTK_کھڑکی(gui.mainwin), &w, &h);
    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    صحيح	    usable_اچائی = h;
    إذا (gtk_socket_id != 0)
	usable_اچائی -= (gui.char_اچائی - (gui.char_اچائی/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_اچائی);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخہ(3,10,0)
    gui.scale = gtk_widget_get_scale_ضارب(widget);
#  endif

    أرجع TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
expose_event(GtkWidget *widget بن_ورتیا,
	     GdkEventExpose *event,
	     gpointer انکڑے بن_ورتیا)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    باہرلا_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.چوڑائی, event->area.اچائی);

    // Clear the border areas if needed
    إذا (event->area.x < FILL_X(0))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, 0, FILL_X(0), 0);
    إذا (event->area.y < FILL_Y(0))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, 0, 0, FILL_Y(0));
    إذا (event->area.x > FILL_X(Columns))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2,
			      FILL_X((صحيح)Columns), 0, 0, 0);
    إذا (event->area.y > FILL_Y(صفاں))
	gdk_کھڑکی_clear_area(gui.drawarea->کھڑکی_2, 0, FILL_Y((صحيح)صفاں), 0, 0);

    أرجع FALSE;
}
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

#جے_تعریف_شدہ FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ساكن gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer انکڑے بن_ورتیا)
{
    إذا (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (صحيح)GDK_PROPERTY_NEW_قدر
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
    أرجع FALSE;
}
#جے_انت // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ساكن فراغ
gtk_settings_xft_dpi_بدلیا_cb(GtkSettings *gtk_settings بن_ورتیا,
				    GParamSpec *pspec بن_ورتیا,
				    gpointer انکڑے بن_ورتیا)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_ادھار_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? عدم : p_guifont, FALSE);
	gui_سیٹ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

تعريفنوع gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ساكن guint
timeout_جوڑو(صحيح ویلا, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_جوڑو((guint)ویلا, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_سروت_remove(timer);
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
gui_mch_سیٹ_blinking(لما waittime, لما on, لما off)
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
blink_cb(gpointer انکڑے بن_ورتیا)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جوڑو(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جوڑو(blink_ontime, blink_cb, عدم);
    }
    gui_may_flush();

    أرجع FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_شروع_blink(فراغ)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.وچ_focus)
    {
	blink_timer = timeout_جوڑو(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ساكن gint
enter_notify_event(GtkWidget *widget بن_ورتیا,
		   Gdk_Event_پار *event بن_ورتیا,
		   gpointer انکڑے بن_ورتیا)
{
    إذا (blink_state == BLINK_NONE)
	gui_mch_شروع_blink();

    // make sure keyboard input goes there
    إذا (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    أرجع FALSE;
}

    ساكن gint
leave_notify_event(GtkWidget *widget بن_ورتیا,
		   Gdk_Event_پار *event بن_ورتیا,
		   gpointer انکڑے بن_ورتیا)
{
    إذا (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    أرجع FALSE;
}

    ساكن gint
focus_وچ_event(GtkWidget *widget,
	       GdkEventFocus *event بن_ورتیا,
	       gpointer انکڑے بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_GUI_DIALOG
    إذا (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    وإلا
#جے_انت
	gui_focus_change(TRUE);

    إذا (blink_state == BLINK_NONE)
	gui_mch_شروع_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    إذا (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    أرجع TRUE;
}

    ساكن gint
focus_باہرلا_event(GtkWidget *widget بن_ورتیا,
		GdkEventFocus *event بن_ورتیا,
		gpointer انکڑے بن_ورتیا)
{
#جے_تعریف_شدہ FEAT_GUI_DIALOG
    إذا (gui.dialogs_سرگرم > 0)
	++gui.dialog_focus_pending;
#جے_انت

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
keyval_منزل_string(بنانشانی صحيح keyval, char_u *string)
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
	    حالة GDK_Tab: حالة GDK_KP_Tab: حالة GDK_ISO_بایاں_Tab:
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

    إذا (state & GDK_کھسکاؤ_نقاب)
	modifiers |= MOD_نقاب_کھسکاؤ;
    إذا (state & GDK_CONTROL_نقاب)
	modifiers |= MOD_نقاب_CTRL;
    إذا (state & GDK_MOD1_نقاب)
	modifiers |= MOD_نقاب_ALT;
#إذا GTK_CHECK_نسخہ(2,10,0)
    إذا (state & GDK_META_نقاب)
	modifiers |= MOD_نقاب_META;
    إذا (state & GDK_SUPER_نقاب)
	modifiers |= MOD_نقاب_CMD;
#وإلا
    إذا (state & GDK_MOD4_نقاب)
	modifiers |= MOD_نقاب_CMD;
#جے_انت

    أرجع modifiers;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_کھسکاؤ_نقاب)
	modifiers |= MOUSE_کھسکاؤ;
    إذا (state & GDK_CONTROL_نقاب)
	modifiers |= MOUSE_CTRL;
    إذا (state & GDK_MOD1_نقاب)
	modifiers |= MOUSE_ALT;

    أرجع modifiers;
}

/*
 * Main keyboard handler:
 */
    ساكن gint
کلید_press_event(GtkWidget *widget بن_ورتیا,
		Gdk_Event_کلید *event,
		gpointer انکڑے بن_ورتیا)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	کلید_sym;
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		کلید;
    guint	state;
    char_u	*s, *d;
    صحيح		ctrl_اگیتر_added = 0;

    gui.event_ویلا = event->ویلا;
    کلید_sym = event->keyval;
    state = event->state;

#جے_تعریف_شدہ FEAT_XIM
    إذا (xim_قطار_کلید_press_event(event, TRUE))
	أرجع TRUE;
#جے_انت

#جے_تعریف_شدہ SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    إذا (کلید_sym == SunXK_F36 || کلید_sym == SunXK_F37)
	len = 0;
    وإلا
#جے_انت
    {
	len = keyval_منزل_string(کلید_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	إذا (len > 1 && داخلہ_conv.vc_type != CONV_NONE)
	    len = convert_داخلہ(string2, len, حجم(string2));

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
    إذا (کلید_sym == GDK_ISO_بایاں_Tab)
    {
	کلید_sym = GDK_Tab;
	state |= GDK_کھسکاؤ_نقاب;
    }

#جے_تعریف_شدہ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    إذا ((state & GDK_MOD1_نقاب)
	    && gui.menu_is_سرگرم
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	أرجع FALSE;
#جے_انت

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	{
	    إذا (special_کلیدیں[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدیں[i].code0;
		string[2] = special_کلیدیں[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

#جے_تعریف_شدہ GDK_کلید_dead_circumflex
    // Belgian Ctrl+[ workaround
    إذا (len == 0 && کلید_sym == GDK_کلید_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_نقاب_CTRL;
	string[3] = '[';
	len = 4;
	جوڑو_منزل_داخلہ_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	أرجع TRUE;
    }
#جے_انت

    إذا (len == 0)   // Unrecognized key
	أرجع TRUE;

    // For some keys a shift modifier is translated into another key code.
    إذا (len == -3)
	کلید = منزل_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	کلید = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    کلید = simplify_کلید(کلید, &modifiers);
    إذا (کلید == CSI)
	کلید = K_CSI;
    إذا (IS_SPECIAL(کلید))
    {
	string[0] = CSI;
	string[1] = K_دوجا(کلید);
	string[2] = K_تیجا(کلید);
	len = 3;
    }
    وإلا
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_کھسکاؤ_modifier(modifiers, کلید);

	len = mb_char2bytes(کلید, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڑو_منزل_داخلہ_buf(string2, 3);
	إذا (modifiers == 0x4)
	    ctrl_اگیتر_added = 1;
    }

    // Check if the key interrupts.
    {
	صحيح int_ch = check_for_رکاوٹ(کلید, modifiers);

	إذا (int_ch != NUL)
	{
	    trash_داخلہ_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    إذا (ctrl_اگیتر_added && len == 2
		    && ((صحيح)string[0]) == 195
		    && ((صحيح)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    جوڑو_منزل_داخلہ_buf(string, len);

    // blank out the pointer if necessary
    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

#إذا defined(FEAT_XIM) || GTK_CHECK_نسخہ(3,0,0)
    ساكن gboolean
کلید_release_event(GtkWidget *widget بن_ورتیا,
		  Gdk_Event_کلید *event بن_ورتیا,
		  gpointer انکڑے بن_ورتیا)
{
# إذا defined(FEAT_XIM)
    gui.event_ویلا = event->ویلا;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    أرجع xim_قطار_کلید_press_event(event, FALSE);
# وإلا
    أرجع TRUE;
# endif
}
#جے_انت


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ساكن صحيح وچ_selection_clear_event = FALSE;

    ساكن gint
selection_clear_event(GtkWidget		*widget بن_ورتیا,
		      GdkEventSelection	*event,
		      gpointer		user_انکڑے بن_ورتیا)
{
    وچ_selection_clear_event = TRUE;
    إذا (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    وإلا
	clip_lose_selection(&clip_تارا);
    وچ_selection_clear_event = FALSE;

    أرجع TRUE;
}

#تعریف_کرو RS_NONE	0	// selection_received_cb() not called yet
#تعریف_کرو RS_OK	1	// selection_received_cb() called and OK
#تعریف_کرو RS_FAIL	2	// selection_received_cb() called and failed
ساكن صحيح received_selection = RS_NONE;

    ساكن فراغ
selection_received_cb(GtkWidget		*widget بن_ورتیا,
		      Gtk_Selection_انکڑے	*انکڑے,
		      guint		ویلا_2 بن_ورتیا,
		      gpointer		user_انکڑے بن_ورتیا)
{
    Clipboard_T	    *cbd;
    char_u	    *متن;
    char_u	    *tmpbuf = عدم;
    guchar	    *tmpbuf_utf8 = عدم;
    صحيح		    len;
    صحيح		    motion_type = MAUTO;

    إذا (gtk_selection_انکڑے_get_selection(انکڑے) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_تارا;

    متن = (char_u *)gtk_selection_انکڑے_get_انکڑے(انکڑے);
    len = gtk_selection_انکڑے_get_لمائی(انکڑے);

    إذا (متن == عدم || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	أرجع;
    }

    إذا (gtk_selection_انکڑے_get_انکڑے_type(انکڑے) == vim_atom
	    || gtk_selection_انکڑے_get_انکڑے_type(انکڑے) == vim_mt_atom)
    {
	motion_type = *متن++;
	--len;
    }
    وإلا إذا (gtk_selection_انکڑے_get_انکڑے_type(انکڑے) == vimenc_atom
	    || gtk_selection_انکڑے_get_انکڑے_type(انکڑے) == vimenc_mt_atom)
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
	tmpbuf_utf8 = gtk_selection_انکڑے_get_متن(انکڑے);
	إذا (tmpbuf_utf8 != عدم)
	{
	    len = STRLEN(tmpbuf_utf8);
	    إذا (داخلہ_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&داخلہ_conv, tmpbuf_utf8, &len);
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

    clip_yank_selection(motion_type, متن, (لما)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ساكن فراغ
selection_get_cb(GtkWidget	    *widget بن_ورتیا,
		 Gtk_Selection_انکڑے   *selection_انکڑے,
		 guint		    info,
		 guint		    ویلا_2 بن_ورتیا,
		 gpointer	    user_انکڑے بن_ورتیا)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    صحيح		    لمائی;
    صحيح		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    إذا (gtk_selection_انکڑے_get_selection(selection_انکڑے)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_تارا;

    إذا (!cbd->owned)
	أرجع;			// Shouldn't ever happen

    إذا (info != (guint)نشانہ_STRING
	    && (!clip_html || info != (guint)نشانہ_HTML)
	    && info != (guint)نشانہ_UTF8_STRING
	    && info != (guint)نشانہ_VIMENC
	    && info != (guint)نشانہ_VIM
	    && info != (guint)نشانہ_VIMENC_MT
	    && info != (guint)نشانہ_VIM_MT
	    && info != (guint)نشانہ_COMPOUND_متن
	    && info != (guint)نشانہ_متن_PLAIN
	    && info != (guint)نشانہ_متن_PLAIN_UTF8
	    && info != (guint)نشانہ_متن)
	أرجع;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    إذا (motion_type < 0 || string == عدم)
	أرجع;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    لمائی = MIN(tmplen, (long_u)(G_MAXINT - 1));

    إذا (info == (guint)نشانہ_VIM || info == (guint)نشانہ_VIM_MT)
    {
	tmpbuf = alloc(لمائی + 1);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)لمائی);
	}
	// For our own format, the first byte contains the motion type
	++لمائی;
	vim_free(string);
	string = tmpbuf;
	إذا (info == (guint)نشانہ_VIM)
	    type = vim_atom;
	وإلا
	    type = vim_mt_atom;
    }

    وإلا إذا (info == (guint)نشانہ_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_تیاری_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	إذا (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &لمائی);
	    convert_تیاری(&conv, عدم, عدم);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	إذا (string != عدم)
	{
	    tmpbuf = alloc(لمائی + 2);
	    إذا (tmpbuf != عدم)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)لمائی);
		vim_free(string);
		string = tmpbuf;
		لمائی += 2;
	    }

#إذا !GTK_CHECK_نسخہ(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_انکڑے->type = selection_انکڑے->نشانہ;
	    selection_انکڑے->format = 16;	// 16 bits per char
#جے_انت
	    gtk_selection_انکڑے_سیٹ(selection_انکڑے, html_atom, 16,
							      string, لمائی);
	    vim_free(string);
	}
	أرجع;
    }
    وإلا إذا (info == (guint)نشانہ_VIMENC || info == (guint)نشانہ_VIMENC_MT)
    {
	صحيح l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(لمائی + l + 2);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)لمائی);
	    لمائی += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	إذا (info == (guint)نشانہ_VIMENC)
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
	    tmpbuf = string_convert(&output_conv, string, &لمائی);
	    vim_free(string);
	    إذا (tmpbuf == عدم)
		أرجع;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	إذا (g_utf8_validate((ثابت حرف *)string, (gssize)لمائی, عدم))
	{
	    gtk_selection_انکڑے_سیٹ_متن(selection_انکڑے,
					(ثابت حرف *)string, لمائی);
	}
	vim_free(string);
	أرجع;
    }

    إذا (string != عدم)
    {
#إذا !GTK_CHECK_نسخہ(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_انکڑے->type = selection_انکڑے->نشانہ;
	selection_انکڑے->format = 8;	// 8 bits per char
#جے_انت
	gtk_selection_انکڑے_سیٹ(selection_انکڑے, type, 8, string, لمائی);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    صحيح
gui_mch_early_init_check(صحيح give_سنیہا)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    إذا ((p == عدم || *p == NUL) && (q == عدم || *q == NUL))
    {
	gui.dying = true;
	إذا (give_سنیہا)
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
#جے_تعریف_شدہ USE_GRESOURCE
    ساكن صحيح res_registered = FALSE;

    إذا (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_رجسٹر_resource();
    }
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_GNOME
    إذا (gtk_socket_id == 0)
	using_gnome = 1;
#جے_انت

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_سیٹ_prgname("gvim");

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
ساكن صحيح mouse_timed_باہرلا = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer انکڑے)
{
    // we don't use this information currently
    صحيح *timed_باہرلا = (صحيح *) انکڑے;

    *timed_باہرلا = TRUE;
    أرجع FALSE;		// don't happen again
}

ساكن guint		motion_repeat_timer  = 0;
ساكن صحيح		motion_repeat_وتھ = FALSE;
ساكن timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ساكن فراغ
process_motion_notify(صحيح x, صحيح y, GdkModifierType state)
{
    صحيح	    button;
    int_u   vim_modifiers;
    Gtk_ونڈ ونڈ;

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
    gtk_widget_get_ونڈ(gui.drawarea, &ونڈ);

    إذا (x < 0 || y < 0
	    || x >= ونڈ.چوڑائی
	    || y >= ونڈ.اچائی)
    {

	صحيح dx;
	صحيح dy;
	صحيح offshoot;
	صحيح delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - ونڈ.چوڑائی;
	dy = y < 0 ? -y : y - ونڈ.اچائی;

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
	    motion_repeat_timer = timeout_جوڑو(delay, motion_repeat_timer_cb,
									 عدم);
    }
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن GdkDevice *
gui_gtk_get_اشارہ_گر_device(GtkWidget *widget)
{
    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(widget);
    GdkDisplay * ثابت dpy = gdk_کھڑکی_get_display(win);
# إذا GTK_CHECK_نسخہ(3,20,0)
    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
    أرجع gdk_seat_get_اشارہ_گر(seat);
# وإلا
    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
    أرجع gdk_device_manager_get_client_اشارہ_گر(mngr);
# endif
}

    ساكن Gdk_کھڑکی *
gui_gtk_get_اشارہ_گر(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(widget);
    GdkDevice * ثابت dev = gui_gtk_get_اشارہ_گر_device(widget);
    أرجع gdk_کھڑکی_get_device_position(win, dev , x, y, state);
}

# إذا defined(FEAT_GUI_TABLINE)
    ساكن Gdk_کھڑکی *
gui_gtk_کھڑکی_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ثابت dev = gui_gtk_get_اشارہ_گر_device(widget);
    أرجع gdk_device_get_کھڑکی_at_position(dev, x, y);
}
# endif
#وإلا // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_اشارہ_گر(wid, x, y, s) \
    gdk_کھڑکی_get_اشارہ_گر((wid)->کھڑکی_2, x, y, s)
# define gui_gtk_کھڑکی_at_position(wid, x, y)	gdk_کھڑکی_at_اشارہ_گر(x, y)
#جے_انت

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ساكن timeout_cb_type
motion_repeat_timer_cb(gpointer انکڑے بن_ورتیا)
{
    صحيح		    x;
    صحيح		    y;
    GdkModifierType state;

    gui_gtk_get_اشارہ_گر(gui.drawarea, &x, &y, &state);

    إذا (!(state & (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب |
		   GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب |
		   GDK_BUTTON5_نقاب)))
    {
	motion_repeat_timer = 0;
	أرجع FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_ورتیا_وچ_داخلہ_buf() > 10)
	أرجع TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    إذا (motion_repeat_وتھ)
	x += gui.char_چوڑائی;

    motion_repeat_وتھ = !motion_repeat_وتھ;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    أرجع FALSE;
}

    ساكن gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer انکڑے بن_ورتیا)
{
    إذا (event->is_hint)
    {
	صحيح		x;
	صحيح		y;
	GdkModifierType	state;

	gui_gtk_get_اشارہ_گر(widget, &x, &y, &state);
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
		   gpointer انکڑے بن_ورتیا)
{
    صحيح button;
    صحيح repeated_click = FALSE;
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_ویلا = event->ویلا;

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
    إذا (!mouse_timed_باہرلا && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_باہرلا = FALSE;
    mouse_click_timer = timeout_جوڑو(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_باہرلا);

    بدل (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	حالة 1:
		button = MOUSE_بایاں;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_نقاب;
		اكسر;
	حالة 2: button = MOUSE_درمیان; اكسر;
	حالة 3: button = MOUSE_دایاں; اكسر;
	حالة 8: button = MOUSE_X1; اكسر;
	حالة 9: button = MOUSE_X2; اكسر;
	افتراضي:
	    أرجع FALSE;		// Unknown button
    }

#جے_تعریف_شدہ FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#جے_انت

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
	     gpointer انکڑے بن_ورتیا)
{
    صحيح	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#إذا GTK_CHECK_نسخہ(3,4,0)
    ساكن مزدوج  acc_x, acc_y;
# إذا !GTK_CHECK_نسخہ(3,22,0)
    ساكن guint32 آخری_smooth_event_ویلا;
# endif
#جے_انت

    إذا (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    بدل (event->direction)
    {
	حالة GDK_SCROLL_اتے:
	    button = MOUSE_4;
	    اكسر;
	حالة GDK_SCROLL_تھلے:
	    button = MOUSE_5;
	    اكسر;
	حالة GDK_SCROLL_بایاں:
	    button = MOUSE_7;
	    اكسر;
	حالة GDK_SCROLL_دایاں:
	    button = MOUSE_6;
	    اكسر;
#إذا GTK_CHECK_نسخہ(3,4,0)
	حالة GDK_SCROLL_SMOOTH:
	    إذا (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		أرجع FALSE;
	    }
# إذا GTK_CHECK_نسخہ(3,22,0)
	    إذا (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# وإلا
	    إذا (event->ویلا - آخری_smooth_event_ویلا > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# إذا !GTK_CHECK_نسخہ(3,22,0)
	    آخری_smooth_event_ویلا = event->ویلا;
# endif
	    اكسر;
#جے_انت
	افتراضي: // This shouldn't happen
	    أرجع FALSE;
    }

#جے_تعریف_شدہ FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#جے_انت

    vim_modifiers = modifiers_gdk2mouse(event->state);

#إذا GTK_CHECK_نسخہ(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    إذا (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	طالما (acc_x >= 1.0)
	{ //دایاں
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_x <= -1.0)
	{ //بایاں
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y >= 1.0)
	{ //تھلے
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y <= -1.0)
	{ //اتے
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
#جے_انت
	gui_send_mouse_event(button, (صحيح)event->x, (صحيح)event->y,
		FALSE, vim_modifiers);

    أرجع TRUE;
}


    ساكن gint
button_release_event(GtkWidget *widget بن_ورتیا,
		     GdkEventButton *event,
		     gpointer انکڑے بن_ورتیا)
{
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_ویلا = event->ویلا;

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
grab_notify_event(GtkWidget *widget بن_ورتیا,
		  gboolean  was_grabbed,
		  gpointer  انکڑے بن_ورتیا)
{
    إذا (!was_grabbed)
	dragging_button_state = 0;
}


#جے_تعریف_شدہ FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ساكن صحيح
گنتی_and_decode_uri_list(char_u *باہرلا, char_u *raw, صحيح len)
{
    صحيح		i;
    char_u	*p = باہرلا;
    صحيح		گنتی = 0;

    لكل (i = 0; i < len; ++i)
    {
	إذا (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    إذا (p > باہرلا && p[-1] != NUL)
	    {
		++گنتی;
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
    إذا (p > باہرلا && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++گنتی;
    }
    أرجع گنتی;
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
parse_uri_list(صحيح *گنتی, char_u *انکڑے, صحيح len)
{
    صحيح	    n	    = 0;
    char_u  *tmp    = عدم;
    char_u  **array = عدم;

    إذا (انکڑے != عدم && len > 0 && (tmp = alloc(len + 1)) != عدم)
    {
	n = گنتی_and_decode_uri_list(tmp, انکڑے, len);
	إذا (n > 0 && (array = ALLOC_MULT(char_u *, n)) != عدم)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *گنتی = n;
    أرجع array;
}

    ساكن فراغ
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_انکڑے	*انکڑے,
		     guint		ویلا_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    صحيح	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_انکڑے_get_انکڑے(انکڑے),
	    gtk_selection_انکڑے_get_لمائی(انکڑے));

    إذا (fnames != عدم && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_انت(context, TRUE, FALSE, ویلا_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    وإلا
	vim_free(fnames);
}

    ساكن فراغ
drag_handle_متن(GdkDragContext	    *context,
		 Gtk_Selection_انکڑے   *انکڑے,
		 guint		    ویلا_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *متن;
    صحيح	    len;
    char_u  *tmpbuf = عدم;

    متن = (char_u *)gtk_selection_انکڑے_get_انکڑے(انکڑے);
    len = gtk_selection_انکڑے_get_لمائی(انکڑے);

    إذا (gtk_selection_انکڑے_get_انکڑے_type(انکڑے) == utf8_string_atom)
    {
	إذا (داخلہ_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&داخلہ_conv, متن, &len);
	إذا (tmpbuf != عدم)
	    متن = tmpbuf;
    }

    dnd_yank_drag_انکڑے(متن, (لما)len);
    gtk_drag_انت(context, TRUE, FALSE, ویلا_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    إذا (dropkey[2] != 0)
	جوڑو_منزل_داخلہ_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	جوڑو_منزل_داخلہ_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ساكن فراغ
drag_انکڑے_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_انکڑے	*انکڑے,
		      guint		info,
		      guint		ویلا_2,
		      gpointer		user_انکڑے بن_ورتیا)
{
    GdkModifierType state;

    // Guard against trash
    ثابت guchar * ثابت انکڑے_انکڑے = gtk_selection_انکڑے_get_انکڑے(انکڑے);
    ثابت gint انکڑے_لمائی = gtk_selection_انکڑے_get_لمائی(انکڑے);
    ثابت gint انکڑے_format = gtk_selection_انکڑے_get_format(انکڑے);

    إذا (انکڑے_انکڑے == عدم
	    || انکڑے_لمائی <= 0
	    || انکڑے_format != 8
	    || انکڑے_انکڑے[انکڑے_لمائی] != '\0')
    {
	gtk_drag_انت(context, FALSE, FALSE, ویلا_2);
	أرجع;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_اشارہ_گر(widget, عدم, عدم, &state);

    // Not sure about the role of "text/plain" here...
    إذا (info == (guint)نشانہ_متن_URI_LIST)
	drag_handle_uri_list(context, انکڑے, ویلا_2, state, x, y);
    وإلا
	drag_handle_متن(context, انکڑے, ویلا_2, state);

}
#جے_انت // FEAT_DND


#إذا defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ساكن فراغ
sm_client_check_بدلیا_any(GnomeClient	    *client بن_ورتیا,
			    gint	    کلید,
			    GnomeDialogType type بن_ورتیا,
			    gpointer	    انکڑے بن_ورتیا)
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
    shutdown_cancelled = check_بدلیا_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    باہرلا_flush();
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
    ساكن gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase بن_ورتیا,
			GnomeSaveStyle	    save_style بن_ورتیا,
			gboolean	    shutdown بن_ورتیا,
			GnomeInteractStyle  interact_style,
			gboolean	    fast بن_ورتیا,
			gpointer	    انکڑے بن_ورتیا)
{
    ساكن ثابت حرف	suffix[] = "-session.vim";
    حرف		*session_file;
    بنانشانی صحيح	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    إذا (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_عام,
					 &sm_client_check_بدلیا_any,
					 عدم);
    باہرلا_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_رستہ(
			gnome_client_get_config_اگیتر(client));
    len = لڑی_لمبائی(session_file);

    إذا (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(حرف, session_file, len + حجم(suffix));
    حافظہ_نقل(session_file + len, suffix, حجم(suffix));

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
	argv[i] = عدم;

	gnome_client_سیٹ_restart_command(client, i, (حرف **)argv);
	gnome_client_سیٹ_clone_command(client, 0, عدم);
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
sm_client_die(GnomeClient *client بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
    // Don't write messages to the GUI anymore
    بھریا_screen = FALSE;

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
local_xsmp_handle_بینتیاں(
    GIOChannel		*سروت بن_ورتیا,
    GIOCondition	condition,
    gpointer		انکڑے)
{
    إذا (condition == G_IO_وچ)
    {
	// Do stuff; maybe close connection
	إذا (xsmp_handle_بینتیاں() == FAIL)
	    g_io_channel_unref((GIOChannel *)انکڑے);
	أرجع TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)انکڑے);
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
    صحيح	    گنتی = 0;

# ifdef USE_XSMP
    إذا (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_جوڑو_watch(g_io, G_IO_وچ | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_بینتیاں, (gpointer)g_io);
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

	Gdk_کھڑکی * ثابت mainwin_win = gtk_widget_get_کھڑکی(gui.mainwin);
	إذا (XGetWMProtocols(dpy, GDK_کھڑکی_XID(mainwin_win),
		    &existing_atoms, &گنتی))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    صحيح	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    لكل (i = 0; i < گنتی; ++i)
		إذا (existing_atoms[i] == save_yourself_xatom)
		    اكسر;

	    إذا (i == گنتی)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, گنتی + 1);
		إذا (new_atoms != عدم)
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
    ساكن GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event بن_ورتیا,
		    gpointer انکڑے بن_ورتیا)
{
    XEvent *xevent = (XEvent *)xev;

    إذا (xevent != عدم
	    && xevent->type == Client_سنیہا
	    && xevent->xclient.سنیہا_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.انکڑے.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	باہرلا_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin)),
		    GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.mainwin)),
		    عدم, 0);
	أرجع GDK_FILTER_REMOVE;
    }

    أرجع GDK_FILTER_CONTINUE;
}
#جے_انت // !USE_GNOME_SESSION

    ساكن GdkPixbuf *
pixbuf_new_from_png_انکڑے(ثابت بنانشانی حرف *انکڑے, بنانشانی صحيح len)
{
    GInput_دھارا *دھارا;
    GdkPixbuf    *pixbuf;

    دھارا = g_حافظہ_داخلہ_دھارا_new_from_انکڑے(انکڑے, len, عدم);
    pixbuf = gdk_pixbuf_new_from_دھارا(دھارا, عدم, عدم);
    g_object_unref(دھارا);
    أرجع pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ساكن فراغ
mainwin_realize(GtkWidget *widget بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
#ضمن "../runtime/vim16x16_png.h"
#ضمن "../runtime/vim32x32_png.h"
#ضمن "../runtime/vim48x48_png.h"

    Gdk_کھڑکی * ثابت mainwin_win = gtk_widget_get_کھڑکی(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    إذا (echo_wid_arg)
    {
	إذا (gui_mch_get_display())
	    لکھو("WID: %ld\n", (لما)GDK_کھڑکی_XID(mainwin_win));
	وإلا
	    لکھو("WID: 0\n");
	fflush(stdout);
    }

    إذا (vim_strchr(p_go, GO_ICON) != عدم)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	إذا (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_کھڑکی_سیٹ_icon_name(GTK_کھڑکی(gui.mainwin), "gvim");
	وإلا
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = عدم;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_انکڑے(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_انکڑے(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_انکڑے(vim48x48_png, vim48x48_png_len));

	    gtk_کھڑکی_سیٹ_icon_list(GTK_کھڑکی(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(فراغ *)&g_object_unref, عدم);
	    g_list_free(icons);
	}
    }

#إذا !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_کھڑکی_جوڑو_filter(عدم, &global_event_filter, عدم);
#جے_انت
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#إذا defined(USE_GNOME_SESSION)
    إذا (using_gnome)
#جے_انت
	تیاری_save_yourself();

#جے_تعریف_شدہ FEAT_CLIENTSERVER
    إذا (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 && gui_mch_get_display())
    {
	إذا (serverName == عدم && server_Delayed_شروع_Name != عدم)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_کھڑکی = GDK_کھڑکی_XID(mainwin_win);

	    (فراغ)server_رجسٹر_Name(GDK_کھڑکی_XDISPLAY(mainwin_win),
				    server_Delayed_شروع_Name);
	}
	وإلا
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
			G_CALLBACK(property_event), عدم);
    }
#جے_انت
}

    ساكن GdkCursor *
create_blank_اشارہ_گر(فراغ)
{
    GdkCursor	*cursor;

#إذا GTK_CHECK_نسخہ(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#وإلا
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_کھڑکی	*جڑ_کھڑکی = عدم;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_نقاب;
    حرف	blank_انکڑے[] = { 0x0 };
    جڑ_کھڑکی = gtk_widget_get_جڑ_کھڑکی(gui.mainwin);
    blank_نقاب = gdk_bitmap_create_from_انکڑے(جڑ_کھڑکی, blank_انکڑے, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_نقاب, blank_نقاب,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_نقاب);
#جے_انت

    أرجع cursor;
}

    ساكن فراغ
mainwin_screen_بدلیا_cb(GtkWidget  *widget,
			  GdkScreen  *پچھلا_screen بن_ورتیا,
			  gpointer   انکڑے بن_ورتیا)
{
    إذا (!gtk_widget_has_screen(widget))
	أرجع;

    /*
     * Recreate the invisible mouse cursor.
     */
    إذا (gui.blank_اشارہ_گر != عدم)
#إذا GTK_CHECK_نسخہ(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_اشارہ_گر));
#وإلا
	gdk_cursor_unref(gui.blank_اشارہ_گر);
#جے_انت

    gui.blank_اشارہ_گر = create_blank_اشارہ_گر();

    إذا (gui.اشارہ_گر_hidden && gtk_widget_get_کھڑکی(gui.drawarea) != عدم)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		gui.blank_اشارہ_گر);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(widget);
    pango_context_سیٹ_ادھار_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_سیٹ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ساكن gboolean
mainwin_state_event_cb(GtkWidget *widget بن_ورتیا,
		       Gdk_Event_کھڑکی_State *event,
		       gpointer user_انکڑے بن_ورتیا)
{
    إذا (event->new_کھڑکی_state & GDK_کھڑکی_STATE_FULLSCREEN)
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
drawarea_realize_cb(GtkWidget *widget, gpointer انکڑے بن_ورتیا)
{
    GtkWidget *sbar;
    Gtk_ونڈ ونڈ;

#جے_تعریف_شدہ FEAT_XIM
    xim_init();
#جے_انت
    gui_mch_new_colors();
#إذا GTK_CHECK_نسخہ(3,0,0)
    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_چوڑائی(widget),
	    gtk_widget_get_allocated_اچائی(widget));
#وإلا
    gui.متن_gc = gdk_gc_new(gui.drawarea->کھڑکی_2);
#جے_انت

    gui.blank_اشارہ_گر = create_blank_اشارہ_گر();
    إذا (gui.اشارہ_گر_hidden)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(widget), gui.blank_اشارہ_گر);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_بایاں].id;
    إذا (!sbar || (!gui.which_scrollbars[SBAR_بایاں]
				    && firstwin->w_scrollbars[SBAR_دایاں].id))
	sbar = firstwin->w_scrollbars[SBAR_دایاں].id;
    gtk_widget_get_ونڈ(sbar, &ونڈ);
    إذا (sbar && gtk_widget_get_realized(sbar) && ونڈ.چوڑائی)
	gui.scrollbar_چوڑائی = ونڈ.چوڑائی;

    sbar = gui.bottom_sbar.id;
    إذا (sbar && gtk_widget_get_realized(sbar) && ونڈ.اچائی)
	gui.scrollbar_اچائی = ونڈ.اچائی;
}

/*
 * Properly clean up on shutdown.
 */
    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
    // Don't write messages to the GUI anymore
    بھریا_screen = FALSE;

#جے_تعریف_شدہ FEAT_XIM
    im_shutdown();
#جے_انت
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

#إذا GTK_CHECK_نسخہ(3,0,0)
    إذا (gui.surface != عدم)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = عدم;
    }
#وإلا
    g_object_unref(gui.متن_gc);
    gui.متن_gc = عدم;
#جے_انت

#إذا GTK_CHECK_نسخہ(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_اشارہ_گر));
#وإلا
    gdk_cursor_unref(gui.blank_اشارہ_گر);
#جے_انت
    gui.blank_اشارہ_گر = عدم;
}

#إذا GTK_CHECK_نسخہ(3,22,2)
    ساكن فراغ
drawarea_style_updated_cb(GtkWidget *widget بن_ورتیا,
			 gpointer انکڑے بن_ورتیا)
#وإلا
    ساكن فراغ
drawarea_style_سیٹ_cb(GtkWidget	*widget بن_ورتیا,
		      GtkStyle	*پچھلا_style بن_ورتیا,
		      gpointer	انکڑے بن_ورتیا)
#جے_انت
{
    gui_mch_new_colors();
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       انکڑے بن_ورتیا)
{
    ساكن صحيح cur_چوڑائی = 0;
    ساكن صحيح cur_اچائی = 0;

    g_return_val_if_fail(event
	    && event->چوڑائی >= 1 && event->اچائی >= 1, TRUE);

# إذا GTK_CHECK_نسخہ(3,22,2) && !GTK_CHECK_نسخہ(3,22,4)
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

    إذا (event->چوڑائی == cur_چوڑائی && event->اچائی == cur_اچائی)
	أرجع TRUE;

    cur_چوڑائی = event->چوڑائی;
    cur_اچائی = event->اچائی;

    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_کھڑکی_create_similar_surface(
	    gtk_widget_get_کھڑکی(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->چوڑائی, event->اچائی);

    gtk_widget_قطار_draw(widget);

    أرجع TRUE;
}
#جے_انت

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ساكن gint
delete_event_cb(GtkWidget *widget بن_ورتیا,
		GdkEventAny *event بن_ورتیا,
		gpointer انکڑے بن_ورتیا)
{
    gui_shell_closed();
    أرجع TRUE;
}

#إذا defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساكن صحيح
get_شے_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation شے_orientation = GTK_ORIENTATION_HORIZONTAL;

    إذا (using_gnome && widget != عدم)
    {
	GtkWidget *ماپا;
	Bonobo_Dock_شے *dockitem;

	ماپا	 = gtk_widget_get_ماپا(widget);
	إذا (G_TYPE_FROM_INSTANCE(ماپا) == BONOBO_TYPE_DOCK_شے)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = ماپا;
	    dockitem = BONOBO_DOCK_شے(widget);

	    إذا (dockitem == عدم || dockitem->is_floating)
		أرجع 0;
	    شے_orientation = bonobo_dock_شے_get_orientation(dockitem);
	}
    }
# وإلا
#  define شے_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    إذا (widget != عدم
	    && شے_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# إذا GTK_CHECK_نسخہ(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_ونڈ ونڈ;

	gtk_widget_get_ونڈ(widget, &ونڈ);
	إذا (ونڈ.اچائی > 1)
	    أرجع ونڈ.اچائی;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  إذا GTK_CHECK_نسخہ(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_اچائی(widget, &min_h, &natural_h);
	    أرجع MAX(min_h, natural_h);
	}
#  وإلا
	أرجع ونڈ.اچائی;
#  endif
# وإلا
	إذا (orientation == GTK_ORIENTATION_HORIZONTAL)
	    أرجع widget->ونڈ.اچائی;
	وإلا
	    أرجع widget->ونڈ.چوڑائی;
# endif
    }
    أرجع 0;
}
#جے_انت

    ساكن صحيح
get_menu_tool_چوڑائی(فراغ)
{
    صحيح چوڑائی = 0;

#جے_تعریف_شدہ FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    چوڑائی += get_شے_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    چوڑائی += get_شے_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    إذا (gui.tabline != عدم)
	چوڑائی += get_شے_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#جے_انت

    أرجع چوڑائی;
}

    ساكن صحيح
get_menu_tool_اچائی(فراغ)
{
    صحيح اچائی = 0;

#جے_تعریف_شدہ FEAT_MENU
    اچائی += get_شے_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#جے_انت
#جے_تعریف_شدہ FEAT_TOOLBAR
    اچائی += get_شے_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#جے_انت
#جے_تعریف_شدہ FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    إذا (gui.tabline != عدم
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	اچائی += get_شے_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#جے_انت

    أرجع اچائی;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ساكن صحيح init_کھڑکی_hints_state = 0;

    ساكن فراغ
update_کھڑکی_manager_hints(صحيح force_چوڑائی, صحيح force_اچائی)
{
    ساكن صحيح old_چوڑائی  = 0;
    ساكن صحيح old_اچائی = 0;
    ساكن صحيح old_min_چوڑائی  = 0;
    ساكن صحيح old_min_اچائی = 0;
    ساكن صحيح old_char_چوڑائی  = 0;
    ساكن صحيح old_char_اچائی = 0;

    صحيح چوڑائی;
    صحيح اچائی;
    صحيح min_چوڑائی;
    صحيح min_اچائی;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    إذا (!(force_چوڑائی && force_اچائی)  &&  init_کھڑکی_hints_state > 0)
    {
	// Don't do it!
	init_کھڑکی_hints_state = 2;
	أرجع;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    چوڑائی  = gui_get_ادھار_چوڑائی();
    اچائی = gui_get_ادھار_اچائی();
#جے_تعریف_شدہ FEAT_MENU
    اچائی += tabline_اچائی() * gui.char_اچائی;
#جے_انت
    چوڑائی  += get_menu_tool_چوڑائی();
    اچائی += get_menu_tool_اچائی();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    إذا (force_چوڑائی && force_اچائی)
    {
	min_چوڑائی  = force_چوڑائی;
	min_اچائی = force_اچائی;
    }
    وإلا
    {
	min_چوڑائی  = چوڑائی  + MIN_COLUMNS * gui.char_چوڑائی;
	min_اچائی = اچائی + MIN_سطریں   * gui.char_اچائی;
    }

    // Avoid an expose event when the size didn't change.
    إذا (چوڑائی != old_چوڑائی
	    || اچائی != old_اچائی
	    || min_چوڑائی != old_min_چوڑائی
	    || min_اچائی != old_min_اچائی
	    || gui.char_چوڑائی != old_char_چوڑائی
	    || gui.char_اچائی != old_char_اچائی)
    {
	GdkGeometry	geometry;
	Gdk_کھڑکی_Hints	geometry_نقاب;

	geometry.چوڑائی_inc   = gui.char_چوڑائی;
	geometry.اچائی_inc  = gui.char_اچائی;
	geometry.ادھار_چوڑائی  = چوڑائی;
	geometry.ادھار_اچائی = اچائی;
	geometry.min_چوڑائی   = min_چوڑائی;
	geometry.min_اچائی  = min_اچائی;
	geometry_نقاب	     = GDK_HINT_ادھار_ناپ|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_ناپ;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_کھڑکی_سیٹ_geometry_hints(GTK_کھڑکی(gui.mainwin), عدم,
				      &geometry, geometry_نقاب);
	old_چوڑائی       = چوڑائی;
	old_اچائی      = اچائی;
	old_min_چوڑائی   = min_چوڑائی;
	old_min_اچائی  = min_اچائی;
	old_char_چوڑائی  = gui.char_چوڑائی;
	old_char_اچائی = gui.char_اچائی;
    }
}

#إذا defined(FEAT_GUI_DARKTHEME)
    فراغ
gui_mch_سیٹ_dark_theme(صحيح dark)
{
# إذا GTK_CHECK_نسخہ(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_سیٹ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, عدم);
# endif
}
#جے_انت // FEAT_GUI_DARKTHEME

#جے_تعریف_شدہ FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ساكن فراغ
icon_ناپ_بدلیا_foreach(GtkWidget *widget, gpointer user_انکڑے)
{
    إذا (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# إذا GTK_CHECK_نسخہ(3,10,0)
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ثابت Gtk_Icon_ناپ icon_ناپ = GPOINTER_منزل_INT(user_انکڑے);
	    ثابت gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, عدم);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_ناپ);
	}
# وإلا
	// User-defined icons are stored in a GtkIconSet
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_سیٹ)
	{
	    Gtk_Icon_سیٹ	*icon_سیٹ;
	    Gtk_Icon_ناپ	icon_ناپ;

	    gtk_image_get_icon_سیٹ(image, &icon_سیٹ, &icon_ناپ);
	    icon_ناپ = (Gtk_Icon_ناپ)(لما)user_انکڑے;

	    gtk_icon_سیٹ_ref(icon_سیٹ);
	    gtk_image_سیٹ_from_icon_سیٹ(image, icon_سیٹ, icon_ناپ);
	    gtk_icon_سیٹ_unref(icon_سیٹ);
	}
# endif
    }
    وإلا إذا (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_ناپ_بدلیا_foreach,
			      user_انکڑے);
    }
}

    ساكن فراغ
سیٹ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_ناپ	    ناپ;
    Gtk_Icon_ناپ	    oldsize;

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

    gtk_toolbar_سیٹ_style(toolbar, style);
# إذا !GTK_CHECK_نسخہ(3,0,0)
    gtk_toolbar_سیٹ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    بدل (tbis_flags)
    {
	حالة TBIS_TINY:	    ناپ = GTK_ICON_ناپ_MENU;		اكسر;
	حالة TBIS_SMALL:    ناپ = GTK_ICON_ناپ_SMALL_TOOLBAR;	اكسر;
	حالة TBIS_MEDIUM:   ناپ = GTK_ICON_ناپ_BUTTON;	اكسر;
	حالة TBIS_LARGE:    ناپ = GTK_ICON_ناپ_LARGE_TOOLBAR;	اكسر;
	حالة TBIS_HUGE:     ناپ = GTK_ICON_ناپ_DND;		اكسر;
	حالة TBIS_GIANT:    ناپ = GTK_ICON_ناپ_DIALOG;	اكسر;
	افتراضي:	    ناپ = GTK_ICON_ناپ_INVALID;	اكسر;
    }
    oldsize = gtk_toolbar_get_icon_ناپ(toolbar);

    إذا (ناپ == GTK_ICON_ناپ_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_ناپ(toolbar);
	ناپ = gtk_toolbar_get_icon_ناپ(toolbar);
    }
    إذا (ناپ != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_ناپ_بدلیا_foreach,
			      GINT_منزل_اشارہ_گر((صحيح)ناپ));
    }
    gtk_toolbar_سیٹ_icon_ناپ(toolbar, ناپ);
}

#جے_انت // FEAT_TOOLBAR

#إذا defined(FEAT_GUI_TABLINE)
ساكن صحيح ignore_tabline_evt = FALSE;
ساكن GtkWidget *tabline_menu;
# إذا !GTK_CHECK_نسخہ(3,0,0)
ساكن GtkTooltips *tabline_tooltip;
# endif
ساكن صحيح clicked_ورقہ;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_handler(Gtk_Menu_شے *شے بن_ورتیا, gpointer user_انکڑے)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_ورقہ, (صحيح)(لما)user_انکڑے);
}

    ساكن فراغ
جوڑو_tabline_menu_شے(GtkWidget *menu, char_u *متن, صحيح resp)
{
    GtkWidget	*شے;
    char_u	*utf_متن;

    utf_متن = CONVERT_منزل_UTF8(متن);
    شے = gtk_menu_شے_new_with_label((ثابت حرف *)utf_متن);
    gtk_widget_show(شے);
    CONVERT_منزل_UTF8_FREE(utf_متن);

    gtk_container_جوڑو(GTK_CONTAINER(menu), شے);
    g_signal_connect(G_OBJECT(شے), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_منزل_اشارہ_گر(resp));
}

/*
 * Create a menu for the tab line.
 */
    ساكن GtkWidget *
create_tabline_menu(فراغ)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    جوڑو_tabline_menu_شے(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    جوڑو_tabline_menu_شے(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    جوڑو_tabline_menu_شے(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

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
	Gdk_کھڑکی	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	إذا (hold_gui_events || cmdwin_type != 0)
	    أرجع TRUE;

	tabwin = gui_gtk_کھڑکی_at_position(gui.mainwin, &x, &y);

	gdk_کھڑکی_get_user_انکڑے(tabwin, (gpointer)&tabwidget);
	clicked_ورقہ = GPOINTER_منزل_INT(g_object_get_انکڑے(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	إذا (bevent->button == 3)
	{
# إذا GTK_CHECK_نسخہ(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    إذا (gui.is_wayland)
	    {
		صحيح x2, y2;
		gui_gtk_get_اشارہ_گر(gui.mainwin, &x2, &y2, عدم);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_کھڑکی(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, عدم);
	    }
	    وإلا
#  endif
	    gtk_menu_popup_at_اشارہ_گر(GTK_MENU(widget), event);
# وإلا
	    gtk_menu_popup(GTK_MENU(widget), عدم, عدم, عدم, عدم,
						bevent->button, bevent->ویلا);
# endif
	    // We handled the event.
	    أرجع TRUE;
	}
	وإلا إذا (bevent->button == 1)
	{
	    إذا (clicked_ورقہ == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	وإلا إذا (bevent->button == 2)
	{
	    إذا (clicked_ورقہ != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_ورقہ, TABLINE_MENU_CLOSE);
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
	GtkNotebook	*notebook بن_ورتیا,
	gpointer	*ورقہ بن_ورتیا,
	gint		idx,
	gpointer	انکڑے بن_ورتیا)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# إذا GTK_CHECK_نسخہ(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook بن_ورتیا,
	gpointer	*ورقہ بن_ورتیا,
	gint		idx,
	gpointer	انکڑے بن_ورتیا)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_اشاریہ(curtab) - 1) < idx)
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

	    gtk_کھڑکی_get_ناپ(GTK_کھڑکی(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_کھڑکی_manager_hints(0, 0);
	إذا (showit)
	    gtk_widget_سیٹ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
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
    GtkWidget	    *ورقہ;
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
    لكل (tp = پہلا_tabpage; tp != عدم; tp = tp->tp_اگلا, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	tab_عدد = nr + 1;

	ورقہ = gtk_notebook_get_nth_ورقہ(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (ورقہ == عدم)
	{
	    // Add notebook page
# إذا GTK_CHECK_نسخہ(3,2,0)
	    ورقہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_سیٹ_homogeneous(GTK_BOX(ورقہ), FALSE);
# وإلا
	    ورقہ = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(ورقہ);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# إذا !GTK_CHECK_نسخہ(3,14,0)
	    gtk_misc_سیٹ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_جوڑو(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_داخل_کرو_ورقہ(GTK_NOTEBOOK(gui.tabline),
		    ورقہ,
		    event_box,
		    nr);
# إذا GTK_CHECK_نسخہ(2,10,0)
	    gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ورقہ,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ورقہ);
	g_object_سیٹ_انکڑے(G_OBJECT(event_box), "tab_num",
						     GINT_منزل_اشارہ_گر(tab_عدد));
	label = gtk_bin_get_بچہ(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
	gtk_label_سیٹ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_منزل_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
# إذا GTK_CHECK_نسخہ(3,0,0)
	gtk_widget_سیٹ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
# وإلا
	gtk_tooltips_سیٹ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ثابت حرف *)labeltext, عدم);
# endif
	CONVERT_منزل_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    طالما (gtk_notebook_get_nth_ورقہ(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_ورقہ(GTK_NOTEBOOK(gui.tabline), nr);

    إذا (gtk_notebook_get_current_ورقہ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سیٹ_current_ورقہ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سیٹ_curtab(صحيح nr)
{
    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;
    إذا (gtk_notebook_get_current_ورقہ(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_سیٹ_current_ورقہ(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#جے_انت // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    فراغ
gui_gtk_سیٹ_selection_targets(GdkAtom selection)
{
    صحيح		    i, j = 0;
    ساكن صحيح	    n_targets = N_SELECTION_TARGETS;
    ساكن Gtk_نشانہ_اندراج  targets[N_SELECTION_TARGETS];

    إذا (targets[0].نشانہ == عدم)
    {
	لكل (i = 0; i < (صحيح)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    إذا (!clip_html && selection_targets[i].info == نشانہ_HTML)
		n_targets--;
	    وإلا
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
    فراغ
gui_gtk_سیٹ_dnd_targets(فراغ)
{
    صحيح		    i, j = 0;
    صحيح		    n_targets = N_DND_TARGETS;
    Gtk_نشانہ_اندراج  targets[N_DND_TARGETS];

    لكل (i = 0; i < (صحيح)N_DND_TARGETS; ++i)
    {
	إذا (!clip_html && dnd_targets[i].info == نشانہ_HTML)
	    n_targets--;
	وإلا
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
    صحيح
gui_mch_init(فراغ)
{
    GtkWidget *vbox;

#جے_تعریف_شدہ FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    إذا (using_gnome)
    {
	gnome_پروگرام_init(VIMPACKAGE, VIM_نسخہ_SHORT,
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
#جے_انت
    VIM_CLEAR(gui_argv);

#إذا GLIB_CHECK_نسخہ(2,1,3)
    // Set the human-readable application name
    g_سیٹ_application_name("Vim");
#جے_انت
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    سیٹ_option_قدر_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#جے_تعریف_شدہ FEAT_TOOLBAR
    gui_gtk_رجسٹر_stock_icons();
#جے_انت
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#إذا !GTK_CHECK_نسخہ(3,0,0)
# إذا 0
    gtk_rc_parse("gtkrc");
# endif
#جے_انت

    // Initialize values
    gui.border_چوڑائی = 2;
    gui.scrollbar_چوڑائی = SB_DEFAULT_چوڑائی;
    gui.scrollbar_اچائی = SB_DEFAULT_چوڑائی;
#إذا GTK_CHECK_نسخہ(3,0,0)
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
#جے_انت

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پٹھ_pixel = gui.def_پٹھ_pixel;

    إذا (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	إذا (plug != عدم && gtk_plug_get_socket_کھڑکی(GTK_PLUG(plug)) != عدم)
	{
	    gui.mainwin = plug;
	}
	وإلا
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (بنانشانی صحيح)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    إذا (gtk_socket_id == 0)
    {
#جے_تعریف_شدہ FEAT_GUI_GNOME
	إذا (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", عدم);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	وإلا
#جے_انت
	    gui.mainwin = gtk_کھڑکی_new(GTK_کھڑکی_TOPLEVEL);
    }

    gtk_widget_سیٹ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سیٹ_ادھار_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_جوڑو_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_نقاب);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_بدلیا_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), عدم);

    gui.accel_گروہ = gtk_accel_گروہ_new();
    gtk_کھڑکی_جوڑو_accel_گروہ(GTK_کھڑکی(gui.mainwin), gui.accel_گروہ);

    // A vertical box holds the menubar, toolbar and main text window.
#إذا GTK_CHECK_نسخہ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سیٹ_homogeneous(GTK_BOX(vbox), FALSE);
#وإلا
    vbox = gtk_vbox_new(FALSE, 0);
#جے_انت

#جے_تعریف_شدہ FEAT_GUI_GNOME
    إذا (using_gnome)
    {
# إذا defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_سیٹ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    وإلا
#جے_انت
    {
	gtk_container_جوڑو(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#جے_تعریف_شدہ FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_سیٹ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_سیٹ(gtk_settings, "gtk-menu-bar-accel", عدم, عدم);
    }


# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
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
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_شروع(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#جے_انت	// FEAT_MENU

#جے_تعریف_شدہ FEAT_TOOLBAR
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
    gtk_widget_سیٹ_name(gui.toolbar, "vim-toolbar");
    سیٹ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
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
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_شروع(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#جے_انت // FEAT_TOOLBAR

#جے_تعریف_شدہ FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_شروع(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_سیٹ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سیٹ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# إذا !GTK_CHECK_نسخہ(3,0,0)
    gtk_notebook_سیٹ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# إذا !GTK_CHECK_نسخہ(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *ورقہ, *label, *event_box;

	// Add the first tab.
# إذا GTK_CHECK_نسخہ(3,2,0)
	ورقہ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_سیٹ_homogeneous(GTK_BOX(ورقہ), FALSE);
# وإلا
	ورقہ = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(ورقہ);
	gtk_container_جوڑو(GTK_CONTAINER(gui.tabline), ورقہ);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_سیٹ_انکڑے(G_OBJECT(event_box), "tab_num", GINT_منزل_اشارہ_گر(1L));
# إذا !GTK_CHECK_نسخہ(3,14,0)
	gtk_misc_سیٹ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_جوڑو(GTK_CONTAINER(event_box), label);
	gtk_notebook_سیٹ_tab_label(GTK_NOTEBOOK(gui.tabline), ورقہ, event_box);
# إذا GTK_CHECK_نسخہ(2,10,0)
	gtk_notebook_سیٹ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), ورقہ, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), عدم);
# إذا GTK_CHECK_نسخہ(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), عدم);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#جے_انت // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_سیٹ_border_چوڑائی(GTK_CONTAINER(gui.formwin), 0);
#إذا !GTK_CHECK_نسخہ(3,0,0)
    gtk_widget_سیٹ_events(gui.formwin, GDK_EXPOSURE_نقاب);
#جے_انت
#إذا GTK_CHECK_نسخہ(3,22,2)
    gtk_widget_سیٹ_name(gui.formwin, "vim-gtk-form");
#جے_انت

    gui.drawarea = gtk_drawing_area_new();
#إذا GTK_CHECK_نسخہ(3,0,0)
    gui.surface = عدم;
#جے_انت

#جے_تعریف_شدہ GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    إذا (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#جے_انت

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
#إذا GTK_CHECK_نسخہ(3,4,0)
    إذا (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
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
    وإلا
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_نقاب |= GDK_SMOOTH_SCROLL_نقاب;
    }
#وإلا
    event_نقاب |= GDK_SCROLL_نقاب;
#جے_انت
    gtk_widget_سیٹ_events(gui.drawarea, event_نقاب);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_شروع(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(کلید_press_event), عدم);
#إذا defined(FEAT_XIM) || GTK_CHECK_نسخہ(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&کلید_release_event), عدم);
#جے_انت
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), عدم);
#إذا GTK_CHECK_نسخہ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), عدم);
#جے_انت
#إذا GTK_CHECK_نسخہ(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), عدم);
#وإلا
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_سیٹ_cb), عدم);
#جے_انت

#إذا !GTK_CHECK_نسخہ(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#جے_انت

#إذا !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#جے_انت

    إذا (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_سیٹ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_تارا.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_وتھ = gui.border_چوڑائی;

#إذا GTK_CHECK_نسخہ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), عدم);
#وإلا
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), عدم);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), عدم);
#جے_انت

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    إذا (vim_strchr(p_go, GO_اشارہ_گر) != عدم)
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
			 G_CALLBACK(focus_باہرلا_event), عدم);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_وچ_event), عدم);
    }
    وإلا
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_باہرلا_event), عدم);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_وچ_event), عدم);
#جے_تعریف_شدہ FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_باہرلا_event), عدم);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_وچ_event), عدم);
#جے_انت // FEAT_GUI_TABLINE
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
    gui.وچ_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_بدلیا_cb), عدم);
    }

#إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخہ(3,10,0)
    gui.scale = gtk_widget_get_scale_ضارب(gui.formwin);
#جے_انت

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
	gnome_client_سیٹ_process_id(client, getpid());
}
#جے_انت // USE_GNOME_SESSION

#إذا GTK_CHECK_نسخہ(3,0,0)
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
سیٹ_cairo_سروت_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ثابت GdkRGBA rgba = color_منزل_rgba(color);
    cairo_سیٹ_سروت_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#جے_انت // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم
#إذا GTK_CHECK_نسخہ(3,22,2)
	    && gui.formwin != عدم
#جے_انت
	    && gtk_widget_get_کھڑکی(gui.drawarea) != عدم)
    {
#إذا !GTK_CHECK_نسخہ(3,22,2)
	Gdk_کھڑکی * ثابت da_win = gtk_widget_get_کھڑکی(gui.drawarea);
#جے_انت
#إذا GTK_CHECK_نسخہ(3,22,2)
	GtkStyleContext * ثابت context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar * ثابت css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.پٹھ_pixel >> 16) & 0xff,
		 (gui.پٹھ_pixel >> 8) & 0xff,
		 gui.پٹھ_pixel & 0xff);

	gtk_css_provider_لدو_from_انکڑے(provider, css, -1, عدم);
	gtk_style_context_جوڑو_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#وإلاإذا GTK_CHECK_نسخہ(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_منزل_rgba(gui.پٹھ_pixel);
	{
	    cairo_نمونہ_t * ثابت pat = cairo_نمونہ_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    إذا (pat != عدم)
	    {
		gdk_کھڑکی_سیٹ_background_نمونہ(da_win, pat);
		cairo_نمونہ_destroy(pat);
	    }
	    وإلا
		gdk_کھڑکی_سیٹ_background_rgba(da_win, &rgba);
	}
#وإلا // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.پٹھ_pixel;
	gdk_کھڑکی_سیٹ_background(da_win, &color);
#جے_انت // !GTK_CHECK_VERSION(3,22,2)
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
startup_resize_correction_cb(gpointer انکڑے بن_ورتیا)
{
    gui_سیٹ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    أرجع FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ساكن gint
form_configure_event(GtkWidget *widget بن_ورتیا,
		     GdkEventConfigure *event,
		     gpointer انکڑے بن_ورتیا)
{
    صحيح	    usable_اچائی = event->اچائی;
#جے_تعریف_شدہ TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    صحيح	    w, h;
    gtk_کھڑکی_get_ناپ(GTK_کھڑکی(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (صحيح)Columns, (صحيح)صفاں);
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

    إذا (!میل_چوڑائی_اچائی(latest_resize_hist, w, h)
					     && میل_stale_چوڑائی_اچائی(w, h))
	// discard stale event
	أرجع TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    إذا ((mch_csd_اچائی == 0 || mch_csd_چوڑائی == 0)
	    && latest_resize_hist != عدم
	    && !latest_resize_hist->ورتیا
	    && latest_resize_hist->from_shellsize
	    && gui.char_اچائی > 0)
    {
	صحيح pot_csd_w = latest_resize_hist->چوڑائی - w;
	صحيح pot_csd_h = latest_resize_hist->اچائی - h;

	إذا (pot_csd_w > 0 && pot_csd_w < gui.char_چوڑائی)
	{
	    mch_csd_چوڑائی = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (صحيح)Columns * gui.char_چوڑائی
						    + gui_get_ادھار_چوڑائی();
	}
	إذا (pot_csd_h > 0 && pot_csd_h < gui.char_اچائی)
	{
	    mch_csd_اچائی = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_اچائی += mch_csd_اچائی;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	إذا ((mch_csd_اچائی > 0 || mch_csd_چوڑائی > 0) && gtk_socket_id == 0)
	    g_idle_جوڑو_بھریا(GTK_ترجیح_RESIZE, startup_resize_correction_cb, عدم, عدم);
    }
    clear_resize_hists();
#جے_انت

#إذا GTK_CHECK_نسخہ(3,22,2) && !GTK_CHECK_نسخہ(3,22,4)
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
    إذا (event->کھڑکی_2 != gtk_widget_get_کھڑکی(gui.formwin))
	أرجع TRUE;
#جے_انت

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    إذا (gtk_socket_id != 0)
	usable_اچائی -= (gui.char_اچائی - (gui.char_اچائی/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	صحيح use_چوڑائی = event->چوڑائی;

#جے_تعریف_شدہ TRACK_RESIZE_HISTORY
	إذا (mch_pending_form_w > event->چوڑائی
		&& mch_pending_form_w - event->چوڑائی < gui.char_چوڑائی)
	    use_چوڑائی = mch_pending_form_w;
#جے_انت
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_چوڑائی, usable_اچائی);
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
mainwin_destroy_cb(GObject *object بن_ورتیا, gpointer انکڑے بن_ورتیا)
{
    // Don't write messages to the GUI anymore
    بھریا_screen = FALSE;

    gui.mainwin  = عدم;
    gui.drawarea = عدم;

    إذا (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#جے_تعریف_شدہ USE_GRESOURCE
    gui_gtk_unregister_resource();
#جے_انت
}

    فراغ
gui_gtk_get_screen_geom_دا_win(
	GtkWidget *wid,
	صحيح نقطہ_x,	    // x position of window if not initialized
	صحيح نقطہ_y,	    // y position of window if not initialized
	صحيح *screen_x,
	صحيح *screen_y,
	صحيح *چوڑائی,
	صحيح *اچائی)
{
    GdkRectangle geometry;
    Gdk_کھڑکی *win = gtk_widget_get_کھڑکی(wid);
#إذا GTK_CHECK_نسخہ(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    إذا (wid != عدم && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    وإلا
	dpy = gdk_display_get_default();
    إذا (win != عدم)
	monitor = gdk_display_get_monitor_at_کھڑکی(dpy, win);
    وإلا
	monitor = gdk_display_get_monitor_at_نقطہ(dpy, نقطہ_x, نقطہ_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#وإلا
    GdkScreen* screen;
    صحيح monitor;

    إذا (wid != عدم && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    وإلا
	screen = gdk_screen_get_default();
    إذا (win != عدم)
	monitor = gdk_screen_get_monitor_at_کھڑکی(screen, win);
    وإلا
	monitor = gdk_screen_get_monitor_at_نقطہ(screen, نقطہ_x, نقطہ_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#جے_انت
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *چوڑائی = geometry.چوڑائی;
    *اچائی = geometry.اچائی;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ساكن فراغ
gui_gtk_get_screen_dimensions(
	صحيح نقطہ_x,
	صحيح نقطہ_y,
	صحيح *screen_w,
	صحيح *screen_h)
{
    صحيح	    x, y;

    gui_gtk_get_screen_geom_دا_win(gui.mainwin, نقطہ_x, نقطہ_y,
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
    *screen_h -= get_menu_tool_اچائی();
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
check_startup_plug_hints(gpointer انکڑے بن_ورتیا)
{
    إذا (init_کھڑکی_hints_state == 1)
    {
	// Safe to use normal hints now
	init_کھڑکی_hints_state = 0;
	update_کھڑکی_manager_hints(0, 0);
	أرجع FALSE;   // stop timer
    }

    // Keep on trying
    init_کھڑکی_hints_state = 1;
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
    guint		pixel_چوڑائی;
    guint		pixel_اچائی;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    إذا (role_دلیل != عدم)
    {
	gtk_کھڑکی_سیٹ_role(GTK_کھڑکی(gui.mainwin), role_دلیل);
    }
    وإلا
    {
	حرف *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (بنانشانی)mch_get_pid(),
			       (بنانشانی)g_random_int(),
			       (بنانشانی)ویلا(عدم));

	gtk_کھڑکی_سیٹ_role(GTK_کھڑکی(gui.mainwin), role);
	g_free(role);
    }

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    إذا (gui.geom != عدم)
    {
	صحيح		نقاب;
	بنانشانی صحيح	w, h;
	صحيح		x = 0;
	صحيح		y = 0;

	نقاب = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);

	إذا (نقاب & چوڑائی_قدر)
	    Columns = w;
	إذا (نقاب & اچائی_قدر)
	{
	    إذا (p_کھڑکی > (لما)h - 1 || !option_was_سیٹ((char_u *)"window"))
		p_کھڑکی = h - 1;
	    صفاں = h;
	}
	حد_بندی_screen_ناپ();

	pixel_چوڑائی = (guint)(gui_get_ادھار_چوڑائی() + Columns * gui.char_چوڑائی);
	pixel_اچائی = (guint)(gui_get_ادھار_اچائی() + صفاں * gui.char_اچائی);

	pixel_چوڑائی  += get_menu_tool_چوڑائی();
	pixel_اچائی += get_menu_tool_اچائی();

	إذا (نقاب & (XValue | YValue))
	{
	    صحيح ww, hh;

#جے_تعریف_شدہ FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#وإلا
	    gui_mch_get_screen_dimensions(&ww, &hh);
#جے_انت
	    hh += p_ghr + get_menu_tool_اچائی();
	    ww += get_menu_tool_چوڑائی();
	    إذا (نقاب & XNegative)
		x += ww - pixel_چوڑائی;
	    إذا (نقاب & YNegative)
		y += hh - pixel_اچائی;
	    gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	إذا (gtk_socket_id != 0  &&  (نقاب & چوڑائی_قدر || نقاب & اچائی_قدر))
	{
	    update_کھڑکی_manager_hints(pixel_چوڑائی, pixel_اچائی);
	    init_کھڑکی_hints_state = 1;
	    timeout_جوڑو(1000, check_startup_plug_hints, عدم);
	}
    }

    pixel_چوڑائی = (guint)(gui_get_ادھار_چوڑائی() + Columns * gui.char_چوڑائی);
    pixel_اچائی = (guint)(gui_get_ادھار_اچائی() + صفاں * gui.char_اچائی);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    إذا (gtk_socket_id == 0)
	gtk_کھڑکی_resize(GTK_کھڑکی(gui.mainwin), pixel_چوڑائی, pixel_اچائی);
    update_کھڑکی_manager_hints(0, 0);

    إذا (foreground_دلیل != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_دلیل);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_دلیل != عدم)
	bg_pixel = gui_get_color((char_u *)background_دلیل);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (لبھیا_الٹ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_پٹھ_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_پٹھ_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    سیٹ_عام_colors();

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
#جے_تعریف_شدہ TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#جے_انت
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), عدم);
#إذا GTK_CHECK_نسخہ(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ضارب_event), عدم);
#جے_انت

#جے_تعریف_شدہ FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_سیٹ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_انکڑے_received_cb), عدم);
#جے_انت

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	إذا (لبھیا_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	بنانشانی لما menu_handler = 0;
# ifdef FEAT_TOOLBAR
	بنانشانی لما tool_handler = 0;
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
#جے_انت
	gtk_widget_show(gui.mainwin);

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	إذا (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	إذا (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#جے_انت
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
gui_mch_exit(صحيح rc بن_ورتیا)
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
	gtk_کھڑکی_get_position(GTK_کھڑکی(gui.mainwin), x, y);
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_سیٹ_winpos(صحيح x, صحيح y)
{
    gtk_کھڑکی_move(GTK_کھڑکی(gui.mainwin), x, y);
}

#إذا !GTK_CHECK_نسخہ(3,0,0)
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
force_shell_resize_idle(gpointer انکڑے)
{
    إذا (gui.mainwin != عدم
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	صحيح چوڑائی;
	صحيح اچائی;

	gtk_کھڑکی_get_ناپ(GTK_کھڑکی(gui.mainwin), &چوڑائی, &اچائی);

	چوڑائی  -= get_menu_tool_چوڑائی();
	اچائی -= get_menu_tool_اچائی();

	gui_resize_shell(چوڑائی, اچائی);
    }

    resize_idle_installed = FALSE;
    أرجع FALSE; // don't call me again
}
# endif
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    صحيح
gui_mch_maximized(فراغ)
{
    أرجع (gui.mainwin != عدم && gtk_widget_get_کھڑکی(gui.mainwin) != عدم
	    && (gdk_کھڑکی_get_state(gtk_widget_get_کھڑکی(gui.mainwin))
					       & GDK_کھڑکی_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_کھڑکی_unmaximize(GTK_کھڑکی(gui.mainwin));
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

    gtk_کھڑکی_get_ناپ(GTK_کھڑکی(gui.mainwin), &w, &h);
    w -= get_menu_tool_چوڑائی();
    h -= get_menu_tool_اچائی();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    فراغ
gui_mch_سیٹ_shellsize(صحيح چوڑائی, صحيح اچائی,
		      صحيح min_چوڑائی بن_ورتیا,  صحيح min_اچائی بن_ورتیا,
		      صحيح ادھار_چوڑائی بن_ورتیا, صحيح ادھار_اچائی بن_ورتیا,
		      صحيح direction بن_ورتیا)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    إذا (gtk_socket_id == 0)
	update_کھڑکی_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    چوڑائی  += get_menu_tool_چوڑائی();
    اچائی += get_menu_tool_اچائی();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    چوڑائی  += mch_csd_چوڑائی;
    اچائی += mch_csd_اچائی;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_چوڑائی > 0)
			    ? چوڑائی - get_menu_tool_چوڑائی() - mch_csd_چوڑائی : 0;

#جے_تعریف_شدہ TRACK_RESIZE_HISTORY
    alloc_resize_hist(چوڑائی, اچائی, TRUE);
#جے_انت
    إذا (gtk_socket_id == 0)
	gtk_کھڑکی_resize(GTK_کھڑکی(gui.mainwin), چوڑائی, اچائی);
    وإلا
	update_کھڑکی_manager_hints(چوڑائی, اچائی);

#إذا !GTK_CHECK_نسخہ(3,0,0)
# إذا 0
    إذا (!resize_idle_installed)
    {
	g_idle_جوڑو_بھریا(GDK_ترجیح_EVENTS + 10,
			&force_shell_resize_idle, عدم, عدم);
	resize_idle_installed = TRUE;
    }
# endif
#جے_انت // !GTK_CHECK_VERSION(3,0,0)
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
gui_mch_settitle(char_u *title, char_u *icon بن_ورتیا)
{
    إذا (title != عدم && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, عدم);

    gtk_کھڑکی_سیٹ_title(GTK_کھڑکی(gui.mainwin), (ثابت حرف *)title);

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

	update_کھڑکی_manager_hints(0, 0);
    }
}
#جے_انت // FEAT_MENU

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
	سیٹ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    إذا (!showit != !gtk_widget_get_visible(widget))
    {
	إذا (showit)
	    gtk_widget_show(widget);
	وإلا
	    gtk_widget_hide(widget);

	update_کھڑکی_manager_hints(0, 0);
    }
}
#جے_انت // FEAT_TOOLBAR

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
    بنانشانی	i;
    صحيح		is_cjk = FALSE;

    font = pango_context_لدو_font(gui.متن_context, font_desc);

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
#إذا PANGO_نسخہ_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#وإلا
	    pango_coverage_unref(coverage);
#جے_انت
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
    gui.char_اچائی = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_اچائی = MAX(gui.char_اچائی, gui.char_ascent + 1);

    أرجع OK;
}

#إذا GTK_CHECK_نسخہ(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ساكن gboolean
font_filter(ثابت PangoFontFamily *family,
	    ثابت PangoFontFace   *face بن_ورتیا,
	    gpointer		   انکڑے بن_ورتیا)
{
    أرجع pango_font_family_is_monospace((PangoFontFamily *)family);
}
#جے_انت

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

#إذا GTK_CHECK_نسخہ(3,2,0)
    dialog = gtk_font_chooser_dialog_new(عدم, عدم);
    gtk_font_chooser_سیٹ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    عدم, عدم);
#وإلا
    dialog = gtk_font_selection_dialog_new(عدم);
#جے_انت

    gtk_کھڑکی_سیٹ_transient_for(GTK_کھڑکی(dialog), GTK_کھڑکی(gui.mainwin));
    gtk_کھڑکی_سیٹ_destroy_with_ماپا(GTK_کھڑکی(dialog), TRUE);

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

#إذا GTK_CHECK_نسخہ(3,2,0)
	gtk_font_chooser_سیٹ_font(
		GTK_FONT_CHOOSER(dialog), (ثابت gchar *)oldname);
#وإلا
	gtk_font_selection_dialog_سیٹ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ثابت حرف *)oldname);
#جے_انت

	إذا (oldname != oldval)
	    vim_free(oldname);
    }
    وإلا
#إذا GTK_CHECK_نسخہ(3,2,0)
	gtk_font_chooser_سیٹ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#وإلا
	gtk_font_selection_dialog_سیٹ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#جے_انت

    response = gtk_dialog_لگاتار(GTK_DIALOG(dialog));

    إذا (response == GTK_RESPONSE_OK)
    {
	حرف *name;

#إذا GTK_CHECK_نسخہ(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#وإلا
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#جے_انت
	إذا (name != عدم)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && داخلہ_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&داخلہ_conv, p, عدم);
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

    plain_font = pango_context_لدو_font(gui.متن_context, gui.norm_font);

    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سیٹ_بھار(bold_font_desc, PANGO_بھار_BOLD);

    bold_font = pango_context_لدو_font(gui.متن_context, bold_font_desc);
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
    GList	    *شے_list;
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
    شے_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (شے_list != عدم && شے_list->اگلا == عدم) // play safe
    {
	Pango_شے   *شے;
	صحيح	    چوڑائی;

	شے  = (Pango_شے *)شے_list->انکڑے;
	چوڑائی = gui.char_چوڑائی * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = شے->analysis.shape_engine;

	gui.ascii_font = شے->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &شے->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->عدد_glyphs == حجم(ascii_chars));

	لكل (i = 0; i < gui.ascii_glyphs->عدد_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_وتھ += MAX(0, چوڑائی - geom->چوڑائی) / 2;
	    geom->چوڑائی = چوڑائی;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(شے_list, (GFunc)(فراغ *)&pango_شے_free, عدم);
    g_list_free(شے_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset بن_ورتیا)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    چوڑائی;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_سیٹ_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_سیٹ_متن(layout, "MW", 2);
    pango_layout_get_ناپ(layout, &چوڑائی, عدم);
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
	صحيح cjk_چوڑائی;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_سیٹ_متن(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_ناپ(layout, &cjk_چوڑائی, عدم);

	إذا (چوڑائی == cjk_چوڑائی)  // Xft not patched
	    چوڑائی /= 2;
    }
    g_object_unref(layout);

    gui.char_چوڑائی = (چوڑائی / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    إذا (gui.char_چوڑائی <= 0)
	gui.char_چوڑائی = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_سیٹ_font_name(font_name);

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
	update_کھڑکی_manager_hints(0, 0);
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
    إذا (!gui.وچ_use || name == عدم)
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
	إذا (pango_font_description_get_ناپ(font) <= 0)
	    pango_font_description_سیٹ_ناپ(font, 10 * PANGO_SCALE);

	real_font = pango_context_لدو_font(gui.متن_context, font);

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
gui_mch_get_fontname(GuiFont font, char_u *name بن_ورتیا)
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
#جے_انت

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
    optexpand_T	*دلیلاں,
    فراغ	*param,
    صحيح		(*جوڑو_میل)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (دلیلاں->oe_include_orig_val && *دلیلاں->oe_opt_قدر == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	إذا (جوڑو_میل((char_u *)DEFAULT_FONT) != OK)
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
	إذا (داخلہ_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&داخلہ_conv, (char_u*)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (جوڑو_میل(buf) != OK)
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
	    إذا (جوڑو_میل((char_u *)fam_name) != OK)
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

    إذا (!gui.وچ_use)		// can't do this when GUI not running
	أرجع color;

    إذا (name != عدم)
	color = gui_get_color_cmn(name);

#إذا GTK_CHECK_نسخہ(3,0,0)
    أرجع color;
#وإلا
    إذا (color == INVALCOLOR)
	أرجع INVALCOLOR;

    أرجع gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#جے_انت
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
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
#جے_انت
}

/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_سیٹ_fg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    *gui.fgcolor = color_منزل_rgba(color);
#وإلا
    gui.fgcolor->pixel = (بنانشانی لما)color;
#جے_انت
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_سیٹ_bg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    *gui.bgcolor = color_منزل_rgba(color);
#وإلا
    gui.bgcolor->pixel = (بنانشانی لما)color;
#جے_انت
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سیٹ_sp_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    *gui.spcolor = color_منزل_rgba(color);
#وإلا
    gui.spcolor->pixel = (بنانشانی لما)color;
#جے_انت
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#تعریف_کرو داخل_کرو_PANGO_ATTR(Attribute, AttrList, شروع_2, سرا_2)  \
    G_STMT_شروع{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->شروع_اشاریہ = (شروع_2);		    \
	tmp_attr_->سرا_اشاریہ = (سرا_2);			    \
	pango_attr_list_داخل_کرو((AttrList), tmp_attr_);	    \
    }G_STMT_سرا

    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *شروع = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (شروع == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		شروع = p;
	}
	وإلا إذا (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, شروع - s, p - s);
	    شروع = عدم;
	}
    }

    إذا (شروع != عدم)
	داخل_کرو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, شروع - s, len);
}

    ساكن صحيح
گنتی_cluster_cells(char_u *s, Pango_شے *شے,
		    PangoGlyphString* glyphs, صحيح i,
		    صحيح *cluster_چوڑائی,
		    صحيح *آخری_glyph_rbearing)
{
    char_u  *p;
    صحيح	    اگلا;	// glyph start index of next cluster
    صحيح	    شروع, سرا; // string segment of current cluster
    صحيح	    چوڑائی;	// real cluster width in Pango units
    صحيح	    uc;
    صحيح	    cellcount = 0;

    چوڑائی = glyphs->glyphs[i].geometry.چوڑائی;

    لكل (اگلا = i + 1; اگلا < glyphs->عدد_glyphs; ++اگلا)
    {
	إذا (glyphs->glyphs[اگلا].attr.is_cluster_شروع)
	    اكسر;
	وإلا إذا (glyphs->glyphs[اگلا].geometry.چوڑائی > چوڑائی)
	    چوڑائی = glyphs->glyphs[اگلا].geometry.چوڑائی;
    }

    شروع = شے->وتھ + glyphs->log_clusters[i];
    سرا   = شے->وتھ + ((اگلا < glyphs->عدد_glyphs) ?
			    glyphs->log_clusters[اگلا] : شے->لمائی);

    لكل (p = s + شروع; p < s + سرا; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (آخری_glyph_rbearing != عدم
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
				     &ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *آخری_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_چوڑائی = چوڑائی;

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
تیاری_zero_چوڑائی_cluster(Pango_شے *شے, PangoGlyphInfo *glyph,
			 صحيح آخری_cellcount, صحيح آخری_cluster_چوڑائی,
			 صحيح آخری_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    چوڑائی;

    چوڑائی = آخری_cellcount * gui.char_چوڑائی * PANGO_SCALE;
    glyph->geometry.x_وتھ = -چوڑائی + MAX(0, چوڑائی - آخری_cluster_چوڑائی) / 2;
    glyph->geometry.چوڑائی = 0;

    pango_font_get_glyph_extents(شے->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_وتھ += آخری_glyph_rbearing;
	glyph->geometry.y_وتھ  = logical_rect.اچائی
		- (gui.char_اچائی - p_linespace) * PANGO_SCALE;
    }
    وإلا
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_وتھ = -چوڑائی + MAX(0, چوڑائی - ink_rect.چوڑائی) / 2;
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
draw_glyph_string(صحيح صف, صحيح خانہ, صحيح عدد_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#وإلا
    ساكن فراغ
draw_glyph_string(صحيح صف, صحيح خانہ, صحيح عدد_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#جے_انت
{
    إذا (!(flags & DRAW_TRANSP))
    {
#إذا GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سروت_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(خانہ), FILL_Y(صف),
			عدد_cells * gui.char_چوڑائی, gui.char_اچائی);
	cairo_fill(cr);
#وإلا
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->کھڑکی_2,
			   gui.متن_gc,
			   TRUE,
			   FILL_X(خانہ),
			   FILL_Y(صف),
			   عدد_cells * gui.char_چوڑائی,
			   gui.char_اچائی);
#جے_انت
    }

#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_سروت_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_منزل(cr, متن_X(خانہ), متن_Y(صف));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#وإلا
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->کھڑکی_2,
		    gui.متن_gc,
		    font,
		    متن_X(خانہ),
		    متن_Y(صف),
		    glyphs);
#جے_انت

    // redraw the contents with an offset of 1 to emulate bold
    إذا ((flags & DRAW_BOLD) && !gui.font_can_bold)
#إذا GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_سیٹ_سروت_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, متن_X(خانہ) + 1, متن_Y(صف));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#وإلا
	gdk_draw_glyphs(gui.drawarea->کھڑکی_2,
			gui.متن_gc,
			font,
			متن_X(خانہ) + 1,
			متن_Y(صف),
			glyphs);
#جے_انت
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
draw_under(صحيح flags, صحيح صف, صحيح خانہ, صحيح cells, cairo_t *cr)
#وإلا
    ساكن فراغ
draw_under(صحيح flags, صحيح صف, صحيح خانہ, صحيح cells)
#جے_انت
{
    صحيح			i;
    صحيح			وتھ;
    ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    صحيح			y = FILL_Y(صف + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    إذا (flags & DRAW_UNDERC)
    {
#إذا GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_سروت_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(خانہ) + 1, y - 2 + 0.5);
	لكل (i = FILL_X(خانہ) + 1; i < FILL_X(خانہ + cells); ++i)
	{
	    وتھ = val[i % 8];
	    cairo_سطر_منزل(cr, i, y - وتھ + 0.5);
	}
	cairo_stroke(cr);
#وإلا
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.spcolor);
	لكل (i = FILL_X(خانہ); i < FILL_X(خانہ + cells); ++i)
	{
	    وتھ = val[i % 8];
	    gdk_draw_نقطہ(gui.drawarea->کھڑکی_2, gui.متن_gc, i, y - وتھ);
	}
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#جے_انت
    }

    // Draw a strikethrough line
    إذا (flags & DRAW_STRIKE)
    {
#إذا GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_سروت_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(خانہ), y + 1 - gui.char_اچائی/2 + 0.5);
	cairo_سطر_منزل(cr, FILL_X(خانہ + cells), y + 1 - gui.char_اچائی/2 + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.spcolor);
	gdk_draw_سطر(gui.drawarea->کھڑکی_2, gui.متن_gc,
		      FILL_X(خانہ), y + 1 - gui.char_اچائی/2,
		      FILL_X(خانہ + cells), y + 1 - gui.char_اچائی/2);
	gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#جے_انت
    }

    // Underline: draw a line at the bottom of the character cell.
    إذا (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
#إذا GTK_CHECK_نسخہ(3,0,0)
	cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
	cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_سیٹ_سروت_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, FILL_X(خانہ), y + 0.5);
	cairo_سطر_منزل(cr, FILL_X(خانہ + cells), y + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_draw_سطر(gui.drawarea->کھڑکی_2, gui.متن_gc,
		      FILL_X(خانہ), y,
		      FILL_X(خانہ + cells) - 1, y);
#جے_انت
    }
}

    صحيح
gui_gtk_draw_string(صحيح صف, صحيح خانہ, char_u *s, صحيح len, صحيح flags)
{
    char_u	*conv_buf = عدم;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    صحيح		convlen;
    char_u	*sp, *bp;
    صحيح		plen;
    صحيح		len_جوڑ;	// return value needs to add up since we are
				// printing substrings
    صحيح		byte_جوڑ;	// byte position in string
    char_u	*cs;		// current *s pointer
    صحيح		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    صحيح		should_need_pango = FALSE;
    صحيح		slen;
    صحيح		is_ligature;
    صحيح		is_utf8;
    char_u	backup_ch;

    إذا (gui.متن_context == عدم || gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
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
    len_جوڑ = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_جوڑ = 0;
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
	طالما (slen < (len - byte_جوڑ))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    إذا (is_ligature && !needs_pango)
	    {
		إذا ((slen + 1) < (len - byte_جوڑ))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		وإلا
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    إذا ((!is_utf8) && ((slen + 1) < (len - byte_جوڑ)))
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
			طالما ((slen < (len - byte_جوڑ))
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
	len_جوڑ += gui_gtk_draw_string_ext(صف, خانہ + len_جوڑ, cs, slen, flags,
					    needs_pango);
	إذا (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_جوڑ += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    أرجع len_جوڑ;
}

#إذا GTK_CHECK_نسخہ(3,0,0)
ساكن cairo_region_t *dirty_region = عدم;

    ساكن فراغ
قطار_draw_area(صحيح x, صحيح y, صحيح چوڑائی, صحيح اچائی)
{
    cairo_rectangle_int_t rect;

    إذا (چوڑائی <= 0 || اچائی <= 0 || gui.drawarea == عدم)
	أرجع;

    rect.x = x;
    rect.y = y;
    rect.چوڑائی = چوڑائی;
    rect.اچائی = اچائی;

    إذا (dirty_region == عدم)
	dirty_region = cairo_region_create_rectangle(&rect);
    وإلا
	cairo_region_union_rectangle(dirty_region, &rect);
}
#جے_انت

    صحيح
gui_gtk_draw_string_ext(
	صحيح	صف,
	صحيح	خانہ,
	char_u	*s,
	صحيح	len,
	صحيح	flags,
	صحيح	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    صحيح			کالم_وتھ = 0;  // column offset in cells
    صحيح			i;
#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_t		*cr;
#جے_انت

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_وتھ;
    area.y = FILL_Y(صف);
    area.چوڑائی	= gui.عدد_خانے_گنتی * gui.char_چوڑائی;
    area.اچائی = gui.char_اچائی;

#إذا GTK_CHECK_نسخہ(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.چوڑائی, area.اچائی);
    cairo_clip(cr);
#وإلا
    gdk_gc_سیٹ_clip_origin(gui.متن_gc, 0, 0);
    gdk_gc_سیٹ_clip_rectangle(gui.متن_gc, &area);
#جے_انت

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

	pango_glyph_string_سیٹ_ناپ(glyphs, len);

	لكل (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#إذا GTK_CHECK_نسخہ(3,0,0)
	draw_glyph_string(صف, خانہ, len, flags, gui.ascii_font, glyphs, cr);
#وإلا
	draw_glyph_string(صف, خانہ, len, flags, gui.ascii_font, glyphs);
#جے_انت

	کالم_وتھ = len;
    }
    وإلا
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*شے_list;
	صحيح		cluster_چوڑائی;
	صحيح		آخری_glyph_rbearing;
	صحيح		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	إذا (!utf_valid_string(s, s + len))
	{
	    کالم_وتھ = len;
	    اذهب skipitall;
	}

	// original width of the current cluster
	cluster_چوڑائی = PANGO_SCALE * gui.char_چوڑائی;

	// right bearing of the last non-composing glyph
	آخری_glyph_rbearing = PANGO_SCALE * gui.char_چوڑائی;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	إذا (gui.wide_font != عدم)
	    apply_wide_font_attr(s, len, attr_list);

	إذا ((flags & DRAW_BOLD) && gui.font_can_bold)
	    داخل_کرو_PANGO_ATTR(pango_attr_بھار_new(PANGO_بھار_BOLD),
			      attr_list, 0, len);
	إذا (flags & DRAW_ITALIC)
	    داخل_کرو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	شے_list = pango_itemize(gui.متن_context,
				  (ثابت حرف *)s, 0, len, attr_list, عدم);

	طالما (شے_list != عدم)
	{
	    Pango_شے	*شے;
	    صحيح		شے_cells = 0; // item length in cells

	    شے = (Pango_شے *)شے_list->انکڑے;
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

#جے_تعریف_شدہ HAVE_PANGO_SHAPE_بھریا
	    pango_shape_بھریا((ثابت حرف *)s + شے->وتھ, شے->لمائی,
		    (ثابت حرف *)s, len, &شے->analysis, glyphs);
#وإلا
	    pango_shape((ثابت حرف *)s + شے->وتھ, شے->لمائی,
			&شے->analysis, glyphs);
#جے_انت
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

		إذا (glyph->attr.is_cluster_شروع)
		{
		    صحيح cellcount;

		    cellcount = گنتی_cluster_cells(
			s, شے, glyphs, i, &cluster_چوڑائی,
			(شے_list != عدم) ? &آخری_glyph_rbearing : عدم);

		    إذا (cellcount > 0)
		    {
			صحيح چوڑائی;

			چوڑائی = cellcount * gui.char_چوڑائی * PANGO_SCALE;
			glyph->geometry.x_وتھ +=
					    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
			glyph->geometry.چوڑائی = چوڑائی;
		    }
		    وإلا
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
		وإلا إذا (i > 0)
		{
		    صحيح چوڑائی;

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
		    إذا (glyph->geometry.x_وتھ >= 0)
		    {
			glyphs->glyphs[i].geometry.چوڑائی =
					 glyphs->glyphs[i - 1].geometry.چوڑائی;
			glyphs->glyphs[i - 1].geometry.چوڑائی = 0;
		    }
		    چوڑائی = cells * gui.char_چوڑائی * PANGO_SCALE;
		    glyph->geometry.x_وتھ +=
					    MAX(0, چوڑائی - cluster_چوڑائی) / 2;
		}
		وإلا // i == 0 "cannot happen"
		{
		    glyph->geometry.چوڑائی = 0;
		}
	    }

	    //// Aaaaand action! **
#إذا GTK_CHECK_نسخہ(3,0,0)
	    draw_glyph_string(صف, خانہ + کالم_وتھ, شے_cells,
			      flags, شے->analysis.font, glyphs,
			      cr);
#وإلا
	    draw_glyph_string(صف, خانہ + کالم_وتھ, شے_cells,
			      flags, شے->analysis.font, glyphs);
#جے_انت

	    pango_شے_free(شے);

	    کالم_وتھ += شے_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#إذا GTK_CHECK_نسخہ(3,0,0)
    draw_under(flags, صف, خانہ, کالم_وتھ, cr);
#وإلا
    draw_under(flags, صف, خانہ, کالم_وتھ);
#جے_انت

    pango_glyph_string_free(glyphs);

#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_destroy(cr);
    قطار_draw_area(FILL_X(خانہ), FILL_Y(صف),
	    کالم_وتھ * gui.char_چوڑائی + 1, gui.char_اچائی);
#وإلا
    gdk_gc_سیٹ_clip_rectangle(gui.متن_gc, عدم);
#جے_انت

    أرجع کالم_وتھ;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_کلیدیں[i].کلید_sym != 0; i++)
	إذا (name[0] == special_کلیدیں[i].code0
		&& name[1] == special_کلیدیں[i].code1)
	    أرجع OK;
    أرجع FAIL;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحيح
gui_get_x11_windis(کھڑکی *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
    {
	*dis = dpy;
	*win = GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.mainwin));
	أرجع OK;
    }

    *dis = عدم;
    *win = 0;
    أرجع FAIL;
}
#جے_انت

    Display *
gui_mch_get_display(فراغ)
{
    إذا (gui.mainwin != عدم && gtk_widget_get_کھڑکی(gui.mainwin) != عدم
#إذا GTK_CHECK_نسخہ(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#جے_انت
	)
	أرجع GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin));
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
#إذا GTK_CHECK_نسخہ(3,0,0)
    // TODO Replace GdkGC with Cairo
    (فراغ)msec;
#وإلا
    GdkGCValues	اقدار;
    GdkGC	*invert_gc;

    إذا (gui.drawarea->کھڑکی_2 == عدم)
	أرجع;

    اقدار.foreground.pixel = gui.norm_pixel ^ gui.پٹھ_pixel;
    اقدار.background.pixel = gui.norm_pixel ^ gui.پٹھ_pixel;
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
		       FILL_X((صحيح)Columns) + gui.border_وتھ,
		       FILL_Y((صحيح)صفاں) + gui.border_وتھ);

    gui_mch_flush();
    ui_delay((لما)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحيح)Columns) + gui.border_وتھ,
		       FILL_Y((صحيح)صفاں) + gui.border_وتھ);

    gdk_gc_destroy(invert_gc);
#جے_انت
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    ثابت GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_چوڑائی, nr * gui.char_اچائی
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);

    cairo_سیٹ_سروت_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# إذا CAIRO_نسخہ >= CAIRO_نسخہ_ENCODE(1,9,2)
    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# وإلا
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اچائی);
    cairo_fill(cr);

    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اچائی);
#وإلا
    GdkGCValues اقدار;
    GdkGC *invert_gc;

    إذا (gui.drawarea->کھڑکی_2 == عدم)
	أرجع;

    اقدار.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_اقدار(gui.drawarea->کھڑکی_2,
				       &اقدار,
				       GDK_GC_FUNCTION);
    gdk_gc_سیٹ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_چوڑائی, (nr) * gui.char_اچائی);
    gdk_gc_destroy(invert_gc);
#جے_انت
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    gtk_کھڑکی_iconify(GTK_کھڑکی(gui.mainwin));
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سیٹ_foreground(فراغ)
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
#جے_انت

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحيح		i = 1;
#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_t    *cr;
#جے_انت

    إذا (gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
	أرجع;

#إذا GTK_CHECK_نسخہ(3,0,0)
    cr = cairo_create(gui.surface);
#جے_انت

    gui_mch_سیٹ_fg_color(color);

#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_سروت_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#وإلا
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
#جے_انت
    إذا (mb_lefthalve(gui.صف, gui.خانہ))
	i = 2;
#إذا GTK_CHECK_نسخہ(3,0,0)
    cairo_سیٹ_سطر_چوڑائی(cr, 1.0);
    cairo_سیٹ_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.خانہ) + 0.5, FILL_Y(gui.صف) + 0.5,
	    i * gui.char_چوڑائی - 1, gui.char_اچائی - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#وإلا
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    FALSE,
	    FILL_X(gui.خانہ), FILL_Y(gui.صف),
	    i * gui.char_چوڑائی - 1, gui.char_اچائی - 1);
#جے_انت
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
{
    إذا (gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
	أرجع;

    gui_mch_سیٹ_fg_color(color);

#إذا GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_سیٹ_سروت_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_دایاں ? FILL_X(gui.خانہ + 1) - w :
# endif
	    FILL_X(gui.خانہ), FILL_Y(gui.صف) + gui.char_اچائی - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_دایاں ? FILL_X(gui.خانہ + 1) - w :
# endif
	    FILL_X(gui.خانہ),
	    FILL_Y(gui.صف) + gui.char_اچائی - h,
	    w, h);
#جے_انت // !GTK_CHECK_VERSION(3,0,0)
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
    طالما (g_main_context_pending(عدم) && !vim_is_داخلہ_buf_بھریا()
	    && ++cnt < 100)
    {
#إذا GTK_CHECK_نسخہ(3,0,0)
	صحيح prio = 0;
	g_main_context_prepare(عدم, &prio);
	// peek internal scheduling of redraw
	إذا (prio == GDK_ترجیح_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#جے_انت
	g_main_context_iteration(عدم, TRUE);
    }
}

    ساكن timeout_cb_type
داخلہ_timer_cb(gpointer انکڑے)
{
    صحيح *timed_باہرلا = (صحيح *) انکڑے;

    // Just inform the caller about the occurrence of it
    *timed_باہرلا = TRUE;

    أرجع FALSE;		// don't happen again
}

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer انکڑے بن_ورتیا)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    أرجع TRUE;		// repeat
}
#جے_انت

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
gui_mch_wait_for_chars(لما wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_باہرلا;
    صحيح		retval = FAIL;
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    guint	channel_timer = 0;
#جے_انت

    timed_باہرلا = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    إذا (wtime >= 0)
	timer = timeout_جوڑو(wtime == 0 ? 1L : wtime,
						   داخلہ_timer_cb, &timed_باہرلا);
    وإلا
	timer = 0;

#جے_تعریف_شدہ FEAT_کم_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_جوڑو(20, channel_poll_cb, عدم);
#جے_انت

    focus = gui.وچ_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.وچ_focus != focus)
	{
	    إذا (gui.وچ_focus)
		gui_mch_شروع_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.وچ_focus;
	}

#جے_تعریف_شدہ سنیہا_قطار
# ifdef FEAT_TIMERS
	did_جوڑو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جوڑو_timer)
	    // Need to recompute the waiting time.
	    اذهب theend;
# endif
#جے_انت

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	إذا (!داخلہ_دستیاب())
	{
	    g_main_context_iteration(عدم, TRUE);
#جے_تعریف_شدہ GDK_WINDOWING_WAYLAND
    إذا (gui.is_wayland && clip_تارا.state == SELECT_وچ_ترقی)
	gui_may_flush();
#جے_انت
	}

	// Got char, return immediately
	إذا (داخلہ_دستیاب())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_باہرلا);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_باہرلا)
	timeout_remove(timer);
#جے_تعریف_شدہ FEAT_کم_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#جے_انت

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
#إذا GTK_CHECK_نسخہ(3,0,0)
    إذا (dirty_region != عدم && gui.drawarea != عدم)
    {
	gtk_widget_قطار_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = عدم;
    }
#وإلا
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       أرجع;
#جے_انت
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_بلاک(صحيح row1arg, صحيح col1arg, صحيح row2arg, صحيح col2arg)
{

    صحيح col1 = check_خانہ(col1arg);
    صحيح col2 = check_خانہ(col2arg);
    صحيح row1 = check_صف(row1arg);
    صحيح row2 = check_صف(row2arg);

#إذا GTK_CHECK_نسخہ(3,0,0)
    إذا (gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
	أرجع;
#وإلا
    GdkColor color;

    إذا (gui.drawarea->کھڑکی_2 == عدم)
	أرجع;

    color.pixel = gui.پٹھ_pixel;
#جے_انت

#إذا GTK_CHECK_نسخہ(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ثابت GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_چوڑائی + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_اچائی
	};
	cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخہ(3,22,2)
	سیٹ_cairo_سروت_rgba_from_color(cr, gui.پٹھ_pixel);
# وإلا
	Gdk_کھڑکی * ثابت win = gtk_widget_get_کھڑکی(gui.drawarea);
	cairo_نمونہ_t * ثابت pat = gdk_کھڑکی_get_background_نمونہ(win);
	إذا (pat != عدم)
	    cairo_سیٹ_سروت(cr, pat);
	وإلا
	    سیٹ_cairo_سروت_rgba_from_color(cr, gui.پٹھ_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اچائی);
	cairo_fill(cr);
	cairo_destroy(cr);

	قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اچائی);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->کھڑکی_2, gui.متن_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_چوڑائی
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_اچائی);
#جے_انت // !GTK_CHECK_VERSION(3,0,0)
}

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
gui_gtk_کھڑکی_clear(Gdk_کھڑکی *win)
{
    ثابت GdkRectangle rect = {
	0, 0, gdk_کھڑکی_get_چوڑائی(win), gdk_کھڑکی_get_اچائی(win)
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخہ(3,22,2)
    سیٹ_cairo_سروت_rgba_from_color(cr, gui.پٹھ_pixel);
# وإلا
    cairo_نمونہ_t * ثابت pat = gdk_کھڑکی_get_background_نمونہ(win);
    إذا (pat != عدم)
	cairo_سیٹ_سروت(cr, pat);
    وإلا
	سیٹ_cairo_سروت_rgba_from_color(cr, gui.پٹھ_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.چوڑائی, rect.اچائی);
    cairo_fill(cr);
    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.چوڑائی, rect.اچائی);
}
#وإلا
# define gui_gtk_کھڑکی_clear(win)  gdk_کھڑکی_clear(win)
#جے_انت

    فراغ
gui_mch_clear_all(فراغ)
{
    إذا (gtk_widget_get_کھڑکی(gui.drawarea) != عدم)
	gui_gtk_کھڑکی_clear(gtk_widget_get_کھڑکی(gui.drawarea));
}

#إذا !GTK_CHECK_نسخہ(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ساكن فراغ
check_copy_area(فراغ)
{
    GdkEvent        *event;
    صحيح                expose_گنتی;
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
	event = gdk_event_get_graphics_expose(gui.drawarea->کھڑکی_2);
	إذا (event == عدم)
	    اكسر; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.چوڑائی, event->expose.area.اچائی);
	expose_گنتی = event->expose.گنتی;
	gdk_event_free(event);
    }
    طالما (expose_گنتی > 0); // more events follow
    gui_can_update_cursor();
}
#جے_انت // !GTK_CHECK_VERSION(3,0,0)

#إذا GTK_CHECK_نسخہ(3,0,0)
    ساكن فراغ
gui_gtk_surface_copy_rect(صحيح dest_x, صحيح dest_y,
			  صحيح src_x,  صحيح src_y,
			  صحيح چوڑائی,  صحيح اچائی)
{
    cairo_t * ثابت cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ساكن cairo_surface_t *scroll_scratch = عدم;
    ساكن صحيح scratch_w = 0, scratch_h = 0;
    صحيح آخری_صف = صفاں - 1, آخری_صف_y = آخری_صف * gui.char_اچائی;
    bool آخری_صف_overlap = (dest_y + اچائی) > آخری_صف_y;
    إذا (gui.is_wayland && ( !(State & منوال_CMDLINE) || !آخری_صف_overlap) )
    {
	إذا (dest_y < src_y) // scroll up
	{
	    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_سروت);
	    cairo_سیٹ_سروت_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اچائی);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	وإلا
	{   //  reuse surface when scrolling, only realloc if larger
	    إذا (scroll_scratch == عدم || چوڑائی > scratch_w || اچائی > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), چوڑائی, اچائی);
		scratch_w = چوڑائی, scratch_h = اچائی;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_سیٹ_سروت_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_سیٹ_operator(cr, CAIRO_OPERATOR_سروت);
	    cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اچائی);
	    cairo_clip(cr);
	    cairo_سیٹ_سروت_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    وإلا
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, چوڑائی, اچائی);
	cairo_clip(cr);
	cairo_ڈالو_گروہ(cr);
	cairo_سیٹ_سروت_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_نکالو_گروہ_منزل_سروت(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#جے_انت

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_سطریں(صحيح صف, صحيح عدد_سطریں)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحيح src_nrows = nrows - عدد_سطریں;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    gui.char_چوڑائی * ncols + 1,     gui.char_اچائی * src_nrows);
    gui_clear_بلاک(
	    gui.scroll_region_bot - عدد_سطریں + 1, gui.scroll_region_بایاں,
	    gui.scroll_region_bot,		   gui.scroll_region_دایاں);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,	gui.char_اچائی * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

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
	    gui.char_اچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1));

    gui_clear_بلاک(gui.scroll_region_bot - عدد_سطریں + 1,
						       gui.scroll_region_بایاں,
		    gui.scroll_region_bot, gui.scroll_region_دایاں);
    check_copy_area();
#جے_انت // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_داخل_کرو_سطریں(صحيح صف, صحيح عدد_سطریں)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_دایاں - gui.scroll_region_بایاں + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحيح src_nrows = nrows - عدد_سطریں;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,     gui.char_اچائی * src_nrows);
    gui_clear_بلاک(
	    صف,		 gui.scroll_region_بایاں,
	    صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * ncols + 1,	gui.char_اچائی * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سیٹ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_کھڑکی_copy_area(gui.drawarea->کھڑکی_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف + عدد_سطریں),
	    gui.drawarea->کھڑکی_2,
	    FILL_X(gui.scroll_region_بایاں), FILL_Y(صف),
	    gui.char_چوڑائی * (gui.scroll_region_دایاں
					    - gui.scroll_region_بایاں + 1) + 1,
	    gui.char_اچائی * (gui.scroll_region_bot - صف - عدد_سطریں + 1));

    gui_clear_بلاک(صف, gui.scroll_region_بایاں,
				صف + عدد_سطریں - 1, gui.scroll_region_دایاں);
    check_copy_area();
#جے_انت // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	نشانہ;
    بنانشانی	i;
    time_t	شروع;

    لكل (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	إذا (!clip_html && selection_targets[i].info == نشانہ_HTML)
	    استمر;
	received_selection = RS_NONE;
	نشانہ = gdk_atom_intern(selection_targets[i].نشانہ, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, نشانہ,
			      (guint32)GDK_CURRENT_ویلا);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	شروع = ویلا(عدم);
	طالما (received_selection == RS_NONE && ویلا(عدم) < شروع + 3)
	    g_main_context_iteration(عدم, TRUE);	// wait for selection_received_cb

	إذا (received_selection != RS_FAIL)
	    أرجع;
    }

    إذا (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_کھڑکی_XDISPLAY(gtk_widget_get_کھڑکی(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd بن_ورتیا)
{
    إذا (وچ_selection_clear_event)
	أرجع;

    gtk_selection_owner_سیٹ(عدم, cbd->gtk_sel_atom, gui.event_ویلا);
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

    success = gtk_selection_owner_سیٹ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_ویلا);
    // don't update on every visual selection change
    إذا (!(cbd->owned && VIsual_سرگرم))
	gui_gtk_سیٹ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    أرجع (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    فراغ
clip_mch_سیٹ_selection(Clipboard_T *cbd بن_ورتیا)
{
}

#إذا defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    صحيح
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    أرجع gdk_selection_owner_get(cbd->gtk_sel_atom) != عدم;
}
#جے_انت


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
	gtk_widget_سیٹ_sensitive(menu->id, !grey);
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
#جے_انت // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    إذا (sb->id == عدم)
	أرجع;

    gtk_widget_سیٹ_visible(sb->id, flag);
    update_کھڑکی_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#إذا GTK_CHECK_نسخہ(3,0,0)
    أرجع (long_u)pixel;
#وإلا
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (بنانشانی لما)pixel, &color);

    أرجع (guicolor_T)(
	    (((بنانشانی)color.red   & 0xff00) << 8)
	 |  ((بنانشانی)color.green & 0xff00)
	 | (((بنانشانی)color.blue  & 0xff00) >> 8));
#جے_انت
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    gui_gtk_get_اشارہ_گر(gui.drawarea, x, y, عدم);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
	XWarp_اشارہ_گر(dpy, (کھڑکی)0,
		GDK_کھڑکی_XID(gtk_widget_get_کھڑکی(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#جے_تعریف_شدہ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساكن صحيح آخری_shape = 0;
#جے_انت

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشارہ_گر_hidden == hide)
	أرجع;

    gui.اشارہ_گر_hidden = hide;
    إذا (gtk_widget_get_کھڑکی(gui.drawarea) && gui.blank_اشارہ_گر != عدم)
    {
	إذا (hide)
	    gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		    gui.blank_اشارہ_گر);
	وإلا
#جے_تعریف_شدہ FEAT_MOUSESHAPE
	    mch_سیٹ_mouse_shape(آخری_shape);
#وإلا
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea), عدم);
#جے_انت
    }
}

#إذا defined(FEAT_MOUSESHAPE)

# إذا GTK_CHECK_نسخہ(3,0,0)
ساكن ثابت حرف * mshape_css_ناں[] =
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

    فراغ
mch_سیٹ_mouse_shape(صحيح shape)
{
    GdkCursor	   *c;
    صحيح		   id;                    // Only id or css_name is used.
# إذا GTK_CHECK_نسخہ(3,0,0)
    ثابت حرف     *css_name = "default";
    GdkDisplay     *disp;
# endif

    إذا (gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشارہ_گر_hidden)
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea),
		gui.blank_اشارہ_گر);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    إذا (id >= GDK_آخری_CURSOR)
		id = GDK_بایاں_PTR;
	    وإلا
		id &= ~1;	// they are always even (why?)
	}
# إذا GTK_CHECK_نسخہ(3,0,0)
	وإلا إذا (shape < (صحيح)ARRAY_لمائی(mshape_css_ناں))
	    css_name = mshape_css_ناں[shape];
# وإلا
	وإلا إذا (shape < (صحيح)ARRAY_لمائی(mshape_ids))
	    id = mshape_ids[shape];
# endif
	وإلا
	    أرجع;
# إذا GTK_CHECK_نسخہ(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# وإلا
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_کھڑکی_سیٹ_cursor(gtk_widget_get_کھڑکی(gui.drawarea), c);
# إذا GTK_CHECK_نسخہ(3,0,0)
	g_object_unref(G_OBJECT(c));
# وإلا
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    إذا (shape != MSHAPE_HIDE)
	آخری_shape = shape;
}
#جے_انت // FEAT_MOUSESHAPE


#إذا defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_چوڑائی  (2 * gui.char_چوڑائی)
# define SIGN_اچائی (gui.char_اچائی)
# define SIGN_ASPECT ((مزدوج)SIGN_اچائی / (مزدوج)SIGN_چوڑائی)

    فراغ
gui_mch_drawsign(صحيح صف, صحيح خانہ, صحيح typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    إذا (sign == عدم || gui.drawarea == عدم
	    || gtk_widget_get_کھڑکی(gui.drawarea) == عدم)
	أرجع;

    صحيح چوڑائی;
    صحيح اچائی;
    صحيح xoffset;
    صحيح yoffset;
    صحيح need_scale;

    چوڑائی  = gdk_pixbuf_get_چوڑائی(sign);
    اچائی = gdk_pixbuf_get_اچائی(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (چوڑائی > SIGN_چوڑائی + 2
	    || اچائی != SIGN_اچائی
	    || (چوڑائی < 3 * SIGN_چوڑائی / 4
		&& اچائی < 3 * SIGN_اچائی / 4));
    إذا (need_scale)
    {
	مزدوج  aspect;
	صحيح	    w = چوڑائی;
	صحيح	    h = اچائی;

	// Keep the original aspect ratio
	aspect = (مزدوج)اچائی / (مزدوج)چوڑائی;
	چوڑائی  = (مزدوج)SIGN_چوڑائی * SIGN_ASPECT / aspect;
	چوڑائی  = MIN(چوڑائی, SIGN_چوڑائی);
	إذا (((مزدوج)(MAX(اچائی, SIGN_اچائی)) /
		    (مزدوج)(MIN(اچائی, SIGN_اچائی))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    اچائی = (مزدوج)SIGN_اچائی * SIGN_ASPECT / aspect;
	    اچائی = MIN(اچائی, SIGN_اچائی);
	}
	وإلا
	    اچائی = (مزدوج)چوڑائی * aspect;

	إذا (w == چوڑائی && h == اچائی)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	وإلا
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, چوڑائی, اچائی,
		    GDK_INTERP_BILINEAR);
	    إذا (sign == عدم)
		أرجع; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (چوڑائی  - SIGN_چوڑائی)  / 2;
    yoffset = (اچائی - SIGN_اچائی) / 2;

# إذا GTK_CHECK_نسخہ(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_چوڑائی, SIGN_اچائی);
	bg_cr = cairo_create(bg_surf);
	cairo_سیٹ_سروت_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_چوڑائی, SIGN_اچائی);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_سیٹ_سروت_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_سیٹ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_سیٹ_سروت_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_سیٹ_سروت_surface(cr, sign_surf, FILL_X(خانہ), FILL_Y(صف));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	قطار_draw_area(
		FILL_X(خانہ), FILL_Y(خانہ), چوڑائی, اچائی);

    }
# وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سیٹ_foreground(gui.متن_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->کھڑکی_2,
	    gui.متن_gc,
	    TRUE,
	    FILL_X(خانہ),
	    FILL_Y(صف),
	    SIGN_چوڑائی,
	    SIGN_اچائی);

    gdk_pixbuf_render_منزل_drawable_alpha(sign,
	    gui.drawarea->کھڑکی_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(خانہ) - MIN(0, xoffset),
	    FILL_Y(صف) - MIN(0, yoffset),
	    MIN(چوڑائی,	SIGN_چوڑائی),
	    MIN(اچائی, SIGN_اچائی),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_عام,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    إذا (need_scale)
	g_object_unref(sign);
}

    فراغ *
gui_mch_رجسٹر_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.وچ_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = عدم;
	char_u	    *سنیہا;

	sign = gdk_pixbuf_new_from_file((ثابت حرف *)signfile, &error);

	إذا (error == عدم)
	    أرجع sign;

	سنیہا = (char_u *)error->سنیہا;

	إذا (سنیہا != عدم && داخلہ_conv.vc_type != CONV_NONE)
	    سنیہا = string_convert(&داخلہ_conv, سنیہا, عدم);

	إذا (سنیہا != عدم)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", سنیہا);

	    إذا (داخلہ_conv.vc_type != CONV_NONE)
		vim_free(سنیہا);
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

#جے_انت // FEAT_SIGN_ICONS

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
    uint8_t *src = img->انکڑے;
    uint8_t *dst;
    صحيح	    w = img->چوڑائی;
    صحيح	    h = img->اچائی;
    صحيح	    stride;
    صحيح	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_انکڑے(surf);
    stride = cairo_image_surface_get_stride(surf);

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
    {
	لكل (y = 0; y < h; ++y)
	{
	    بنانشانی صحيح *p = (بنانشانی صحيح *)(dst + y * stride);

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
	    بنانشانی صحيح *p = (بنانشانی صحيح *)(dst + y * stride);

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
# إذا GTK_CHECK_نسخہ(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->چوڑائی, img->اچائی);

    إذا (cairo_surface_حالت(ctx->surf) != CAIRO_حالت_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	اذهب fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_انکڑے = ctx;

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
# إذا GTK_CHECK_نسخہ(3,0,0)
    image_cairo_T *ctx = img->backend_انکڑے;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    صحيح
image_placement_gui_init(image_placement_T *رکھو بن_ورتیا)
{
# إذا GTK_CHECK_نسخہ(3,0,0)
    أرجع OK;
# وإلا
    أرجع FAIL;
# endif
}

    فراغ
image_placement_gui_uninit(image_placement_T *رکھو بن_ورتیا)
{
}

    فراغ
image_placement_gui_draw(image_placement_T *رکھو)
{
# إذا GTK_CHECK_نسخہ(3,0,0)
    image_T		    *img = رکھو->img;
    image_cairo_T	    *ictx = img->backend_انکڑے;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&رکھو->visible, &n_rects);
    إذا (rects == عدم || n_rects == 0)
	أرجع;

    cairo_surface_سیٹ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		صف, خانہ;
	صحيح		posx, posy;
	صحيح		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(رکھو, rect, &صف, &خانہ, &x, &y, &w, &h, false);
	posx = FILL_X(خانہ);
	posy = FILL_Y(صف);

	cairo_save(cr);
	cairo_سیٹ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_سیٹ_سروت_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	قطار_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    فراغ
image_placement_gui_clear(image_placement_T *رکھو بن_ورتیا)
{
}

#جے_انت // FEAT_IMAGE
