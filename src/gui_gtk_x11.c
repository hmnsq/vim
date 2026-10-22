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
#جيڪڏهن_وصف_ٿيل USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
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
خارجي فراغ bonobo_dock_شي_سيٽ_behavior(Bonobo_Dock_شي *dock_شي, Bonobo_Dock_شي_Behavior beh);
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI_GTK)
# إذا GTK_CHECK_نسخو(3,0,0)
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#جيڪڏهن_پورو

/*
 * Easy-to-use macro for multihead support.
 */
#وصف_ڪريو GET_X_ATOM(atom)	gdk_x11_atom_منزل_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
تعداد
{
    هدف_TYPE_NONE,
    هدف_UTF8_STRING,
    هدف_STRING,
    هدف_COMPOUND_متن,
    هدف_HTML,
    هدف_متن,
    هدف_متن_URI_LIST,
    هدف_متن_PLAIN,
    هدف_متن_PLAIN_UTF8,
    هدف_VIM_MT,
    هدف_VIM,
    هدف_VIMENC_MT,
    هدف_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_هدف_داخلا selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, هدف_VIMENC},
    {VIM_ATOM_NAME,	0, هدف_VIM},
    {VIMENC_MIMETYPE_NAME,  0, هدف_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, هدف_VIM_MT},
    {"text/html",	0, هدف_HTML},
    {"UTF8_STRING",	0, هدف_UTF8_STRING},
    {"COMPOUND_TEXT",	0, هدف_COMPOUND_متن},
    {"TEXT",		0, هدف_متن},
    {"STRING",		0, هدف_STRING},
    {"text/plain;charset=utf-8", 0, هدف_متن_PLAIN_UTF8},
    {"text/plain",	0, هدف_متن_PLAIN}
};
#وصف_ڪريو N_SELECTION_TARGETS ARRAY_ڊيگهائي(selection_targets)

#جيڪڏهن_وصف_ٿيل FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ساكن ثابت Gtk_هدف_داخلا dnd_targets[] =
{
    {"text/uri-list",	0, هدف_متن_URI_LIST},
    {"text/html",	0, هدف_HTML},
    {"UTF8_STRING",	0, هدف_UTF8_STRING},
    {"STRING",		0, هدف_STRING},
    {"text/plain",	0, هدف_متن_PLAIN}
};
# define N_DND_TARGETS ARRAY_ڊيگهائي(dnd_targets)
#جيڪڏهن_پورو


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#وصف_ڪريو DEFAULT_FONT	"Monospace 12"

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#جيڪڏهن_پورو

#إذا !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ساكن GdkAtom wm_protocols_atom = GDK_NONE;
ساكن GdkAtom save_yourself_atom = GDK_NONE;
#جيڪڏهن_پورو

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
ساكن بنية special_ڪنجي
{
    guint ڪنجي_sym;
    char_u code0;
    char_u code1;
}
ثابت special_ڪنجيون[] =
{
    {GDK_مٿي,		'k', 'u'},
    {GDK_هيٺ_ڏانهن,		'k', 'd'},
    {GDK_کاٻو,		'k', 'l'},
    {GDK_ساڄو,		'k', 'r'},
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
#جيڪڏهن_وصف_ٿيل SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#جيڪڏهن_پورو
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_پٺ_Space,	'k', 'b'},
    {GDK_وجهو,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_پٺ_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_پڇاڙي,		'@', '7'},
    {GDK_اڳواٽ,		'k', 'P'},
    {GDK_ايندڙ,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_کاٻو,	'k', 'l'},
    {GDK_KP_ساڄو,	'k', 'r'},
    {GDK_KP_مٿي,		'k', 'u'},
    {GDK_KP_هيٺ_ڏانهن,	'k', 'd'},
    {GDK_KP_وجهو,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_پڇاڙي,	'K', '4'},
    {GDK_KP_اڳواٽ,	'K', '3'},  // page up
    {GDK_KP_ايندڙ,	'K', '5'},  // page down

    {GDK_KP_جوڙيو,	'K', '6'},
    {GDK_KP_ڪٽيو,	'K', '7'},
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
#وصف_ڪريو ARG_FONT	1
#وصف_ڪريو ARG_GEOMETRY	2
#وصف_ڪريو ARG_ابتو	3
#وصف_ڪريو ARG_NOREVERSE	4
#وصف_ڪريو ARG_BACKGROUND	5
#وصف_ڪريو ARG_FOREGROUND	6
#وصف_ڪريو ARG_ICONIC	7
#وصف_ڪريو ARG_ROLE	8
#وصف_ڪريو ARG_NETBEANS	9
#وصف_ڪريو ARG_XRM		10	// ignored
#وصف_ڪريو ARG_MENUFONT	11	// ignored
#وصف_ڪريو ARG_اشاريو_نقاب	0x00ff
#وصف_ڪريو ARG_HAS_قدر	0x0100	// a value is expected after the argument
#وصف_ڪريو ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#وصف_ڪريو ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#وصف_ڪريو ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#وصف_ڪريو ARG_KEEP	0x1000	// don't remove argument from argv[]

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
    بنانشان صحيح    flags;
}
cmdline_option_T;

ساكن ثابت cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_قدر},
    {"-font",		ARG_FONT|ARG_HAS_قدر},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_قدر},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_قدر},
    {"-rv",		ARG_ابتو},
    {"-reverse",	ARG_ابتو},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_قدر},
    {"-background",	ARG_BACKGROUND|ARG_HAS_قدر},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_قدر},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_قدر},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_قدر},
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_قدر},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_قدر},	// not implemented
#جيڪڏهن_پورو
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
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
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
#جيڪڏهن_پورو
    {عدم, 0}
};

ساكن صحيح    gui_argc = 0;
ساكن حرف **gui_argv = عدم;

ساكن ثابت حرف *role_دليل = عدم;
#إذا defined(USE_GNOME_SESSION)
ساكن ثابت حرف *restart_command = عدم;
ساكن       حرف *abs_restart_command = عدم;
#جيڪڏهن_پورو
ساكن صحيح مليو_iconic_arg = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ساكن صحيح using_gnome = 0;
#وإلا
# define using_gnome 0
#جيڪڏهن_پورو

// Comment out the following line to ignore code for resize history tracking.
#وصف_ڪريو TRACK_RESIZE_HISTORY
#جيڪڏهن_وصف_ٿيل TRACK_RESIZE_HISTORY
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
    صحيح استعمال_ٿيل;		// If true, can't match for discard. Only matches once.
    صحيح ويڪر;
    صحيح اوچائي;
    صحيح from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحيح seq;		// for ch_log messages
# endif
    بنية resize_history *ايندڙ;
} resize_hist_T;

// never NULL during execution
ساكن resize_hist_T *latest_resize_hist;
// list of stale resize requests
ساكن resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ساكن صحيح mch_csd_ويڪر = 0;
ساكن صحيح mch_csd_اوچائي = 0;

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
alloc_resize_hist(صحيح ويڪر, صحيح اوچائي, صحيح from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->ويڪر = ويڪر;
    new_hist->اوچائي = اوچائي;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->ايندڙ = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(عدم, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, ويڪر, اوچائي, (صحيح)Columns, (صحيح)صفون);
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
	latest_resize_hist->استعمال_ٿيل = TRUE;
    طالما (old_resize_hists != عدم)
    {
	resize_hist_T *ايندڙ_hist = old_resize_hists->ايندڙ;

	vim_free(old_resize_hists);
	old_resize_hists = ايندڙ_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define ميلاپ_ويڪر_اوچائي(hist, w, h) \
	  (!hist->استعمال_ٿيل && hist->ويڪر == w && hist->اوچائي == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ساكن صحيح
ميلاپ_stale_ويڪر_اوچائي(صحيح ويڪر, صحيح اوچائي)
{
    resize_hist_T *hist = old_resize_hists;

    لكل (hist = old_resize_hists; hist != عدم; hist = hist->ايندڙ)
	إذا (ميلاپ_ويڪر_اوچائي(hist, ويڪر, اوچائي))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(عدم, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->استعمال_ٿيل = TRUE;
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
#جيڪڏهن_پورو

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

	إذا (mch_ڀريل_Name((char_u *)argv[0], buf, (صحيح)حجم(buf), TRUE) == OK)
	{
	    abs_restart_command = (حرف *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#جيڪڏهن_پورو

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
	    len = لڙي_ڊيگهه(option->name);

	    إذا (لڙي_ڀيٽ(argv[i], option->name, len) == 0)
	    {
		إذا (argv[i][len] == '\0')
		    اكسر;
		// allow --foo=bar style
		إذا (argv[i][len] == '=' && (option->flags & ARG_HAS_قدر))
		    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		إذا (vim_strchr((char_u *)":=", argv[i][len]) != عدم
			&& (option->flags & ARG_اشاريو_نقاب) == ARG_NETBEANS)
		    اكسر;
#جيڪڏهن_پورو
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
	    بدل (option->flags & ARG_اشاريو_نقاب)
	    {
		حالة ARG_ابتو:
		    مليو_ابتو_arg = TRUE;
		    اكسر;
		حالة ARG_NOREVERSE:
		    مليو_ابتو_arg = FALSE;
		    اكسر;
		حالة ARG_FONT:
		    font_دليل = قدر;
		    اكسر;
		حالة ARG_GEOMETRY:
		    إذا (قدر != عدم)
			gui.geom = vim_strsave((char_u *)قدر);
		    اكسر;
		حالة ARG_BACKGROUND:
		    background_دليل = قدر;
		    اكسر;
		حالة ARG_FOREGROUND:
		    foreground_دليل = قدر;
		    اكسر;
		حالة ARG_ICONIC:
		    مليو_iconic_arg = TRUE;
		    اكسر;
		حالة ARG_ROLE:
		    role_دليل = قدر; // used later in gui_mch_open()
		    اكسر;
#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
		حالة ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    اكسر;
#جيڪڏهن_پورو
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
#جيڪڏهن_پورو

#إذا !GTK_CHECK_نسخو(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ساكن gint
visibility_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		 GdkEventVisibility *event,
		 gpointer معلومات اڻ_استعمال_ٿيل)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    إذا (gui.متن_gc != عدم)
	gdk_gc_سيٽ_exposures(gui.متن_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    أرجع FALSE;
}
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن gboolean
draw_event(GtkWidget *widget اڻ_استعمال_ٿيل,
	   cairo_t   *cr,
	   gpointer   user_معلومات اڻ_استعمال_ٿيل)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    ٻاهريون_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_سيٽ_ذريعو_surface(cr, gui.surface, 0, 0);
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

# إذا GTK_CHECK_نسخو(3,10,0)
    ساكن gboolean
scale_ضرب_ڪندڙ_event(GtkWidget *widget,
		   GParamSpec* pspec اڻ_استعمال_ٿيل,
		   gpointer   user_معلومات اڻ_استعمال_ٿيل)
{
    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    صحيح	    w, h;
    gtk_دري_get_سائيز(GTK_دري(gui.mainwin), &w, &h);
    gui.surface = gdk_دري_create_similar_surface(
	    gtk_widget_get_دري(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    صحيح	    usable_اوچائي = h;
    إذا (gtk_socket_id != 0)
	usable_اوچائي -= (gui.char_اوچائي - (gui.char_اوچائي/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_اوچائي);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخو(3,10,0)
    gui.scale = gtk_widget_get_scale_ضرب_ڪندڙ(widget);
#  endif

    أرجع TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#وإلا // !GTK_CHECK_VERSION(3,0,0)
    ساكن gint
expose_event(GtkWidget *widget اڻ_استعمال_ٿيل,
	     GdkEventExpose *event,
	     gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    إذا (gui.starting)
	أرجع FALSE;

    ٻاهريون_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.ويڪر, event->area.اوچائي);

    // Clear the border areas if needed
    إذا (event->area.x < FILL_X(0))
	gdk_دري_clear_area(gui.drawarea->دري_2, 0, 0, FILL_X(0), 0);
    إذا (event->area.y < FILL_Y(0))
	gdk_دري_clear_area(gui.drawarea->دري_2, 0, 0, 0, FILL_Y(0));
    إذا (event->area.x > FILL_X(Columns))
	gdk_دري_clear_area(gui.drawarea->دري_2,
			      FILL_X((صحيح)Columns), 0, 0, 0);
    إذا (event->area.y > FILL_Y(صفون))
	gdk_دري_clear_area(gui.drawarea->دري_2, 0, FILL_Y((صحيح)صفون), 0, 0);

    أرجع FALSE;
}
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

#جيڪڏهن_وصف_ٿيل FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ساكن gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (صحيح)GDK_PROPERTY_NEW_قدر
	    && GDK_دري_XID(event->دري_2) == comm_دري
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.دري_2 = comm_دري;
	xev.xproperty.state = Property_New_قدر;
	serverEventProc(GDK_دري_XDISPLAY(gtk_widget_get_دري(widget)),
								      &xev, 0);
    }
    أرجع FALSE;
}
#جيڪڏهن_پورو // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ساكن فراغ
gtk_settings_xft_dpi_بدليو_cb(GtkSettings *gtk_settings اڻ_استعمال_ٿيل,
				    GParamSpec *pspec اڻ_استعمال_ٿيل,
				    gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سيٽ_بنياد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? عدم : p_guifont, FALSE);
	gui_سيٽ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

تعريفنوع gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ساكن guint
timeout_جوڙيو(صحيح وقت, timeout_cb_type (*callback)(gpointer), صحيح *flagp)
{
    أرجع g_timeout_جوڙيو((guint)وقت, (GSourceFunc)callback, flagp);
}

    ساكن فراغ
timeout_remove(guint timer)
{
    g_ذريعو_remove(timer);
}


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#وصف_ڪريو BLINK_NONE  0
#وصف_ڪريو BLINK_OFF   1
#وصف_ڪريو BLINK_ON    2

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
gui_mch_سيٽ_blinking(ڊگهو waittime, ڊگهو on, ڊگهو off)
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
blink_cb(gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_جوڙيو(blink_offtime, blink_cb, عدم);
    }
    وإلا
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_جوڙيو(blink_ontime, blink_cb, عدم);
    }
    gui_may_flush();

    أرجع FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    فراغ
gui_mch_شروعات_blink(فراغ)
{
    إذا (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    إذا (blink_waittime && blink_ontime && blink_offtime && gui.اندريون_focus)
    {
	blink_timer = timeout_جوڙيو(blink_waittime, blink_cb, عدم);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ساكن gint
enter_notify_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		   Gdk_Event_پار *event اڻ_استعمال_ٿيل,
		   gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (blink_state == BLINK_NONE)
	gui_mch_شروعات_blink();

    // make sure keyboard input goes there
    إذا (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    أرجع FALSE;
}

    ساكن gint
leave_notify_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		   Gdk_Event_پار *event اڻ_استعمال_ٿيل,
		   gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    أرجع FALSE;
}

    ساكن gint
focus_اندريون_event(GtkWidget *widget,
	       GdkEventFocus *event اڻ_استعمال_ٿيل,
	       gpointer معلومات اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI_DIALOG
    إذا (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    وإلا
#جيڪڏهن_پورو
	gui_focus_change(TRUE);

    إذا (blink_state == BLINK_NONE)
	gui_mch_شروعات_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    إذا (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    أرجع TRUE;
}

    ساكن gint
focus_ٻاهريون_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		GdkEventFocus *event اڻ_استعمال_ٿيل,
		gpointer معلومات اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل FEAT_GUI_DIALOG
    إذا (gui.dialogs_سرگرم > 0)
	++gui.dialog_focus_pending;
#جيڪڏهن_پورو

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
keyval_منزل_string(بنانشان صحيح keyval, char_u *string)
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
	    حالة GDK_Tab: حالة GDK_KP_Tab: حالة GDK_ISO_کاٻو_Tab:
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

    إذا (state & GDK_سرڪايو_نقاب)
	modifiers |= MOD_نقاب_سرڪايو;
    إذا (state & GDK_CONTROL_نقاب)
	modifiers |= MOD_نقاب_CTRL;
    إذا (state & GDK_MOD1_نقاب)
	modifiers |= MOD_نقاب_ALT;
#إذا GTK_CHECK_نسخو(2,10,0)
    إذا (state & GDK_META_نقاب)
	modifiers |= MOD_نقاب_META;
    إذا (state & GDK_SUPER_نقاب)
	modifiers |= MOD_نقاب_CMD;
#وإلا
    إذا (state & GDK_MOD4_نقاب)
	modifiers |= MOD_نقاب_CMD;
#جيڪڏهن_پورو

    أرجع modifiers;
}

    ساكن صحيح
modifiers_gdk2mouse(guint state)
{
    صحيح modifiers = 0;

    إذا (state & GDK_سرڪايو_نقاب)
	modifiers |= MOUSE_سرڪايو;
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
ڪنجي_press_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		Gdk_Event_ڪنجي *event,
		gpointer معلومات اڻ_استعمال_ٿيل)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	ڪنجي_sym;
    صحيح		len;
    صحيح		i;
    صحيح		modifiers;
    صحيح		ڪنجي;
    guint	state;
    char_u	*s, *d;
    صحيح		ctrl_اڳياڙي_added = 0;

    gui.event_وقت = event->وقت;
    ڪنجي_sym = event->keyval;
    state = event->state;

#جيڪڏهن_وصف_ٿيل FEAT_XIM
    إذا (xim_قطار_ڪنجي_press_event(event, TRUE))
	أرجع TRUE;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    إذا (ڪنجي_sym == SunXK_F36 || ڪنجي_sym == SunXK_F37)
	len = 0;
    وإلا
#جيڪڏهن_پورو
    {
	len = keyval_منزل_string(ڪنجي_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	إذا (len > 1 && داخلا_conv.vc_type != CONV_NONE)
	    len = convert_داخلا(string2, len, حجم(string2));

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
    إذا (ڪنجي_sym == GDK_ISO_کاٻو_Tab)
    {
	ڪنجي_sym = GDK_Tab;
	state |= GDK_سرڪايو_نقاب;
    }

#جيڪڏهن_وصف_ٿيل FEAT_MENU
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
#جيڪڏهن_پورو

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    إذا (len == 0 || len == 1)
    {
	لكل (i = 0; special_ڪنجيون[i].ڪنجي_sym != 0; i++)
	{
	    إذا (special_ڪنجيون[i].ڪنجي_sym == ڪنجي_sym)
	    {
		string[0] = CSI;
		string[1] = special_ڪنجيون[i].code0;
		string[2] = special_ڪنجيون[i].code1;
		len = -3;
		اكسر;
	    }
	}
    }

#جيڪڏهن_وصف_ٿيل GDK_ڪنجي_dead_circumflex
    // Belgian Ctrl+[ workaround
    إذا (len == 0 && ڪنجي_sym == GDK_ڪنجي_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_نقاب_CTRL;
	string[3] = '[';
	len = 4;
	جوڙيو_منزل_داخلا_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	أرجع TRUE;
    }
#جيڪڏهن_پورو

    إذا (len == 0)   // Unrecognized key
	أرجع TRUE;

    // For some keys a shift modifier is translated into another key code.
    إذا (len == -3)
	ڪنجي = منزل_SPECIAL(string[1], string[2]);
    وإلا
    {
	string[len] = NUL;
	ڪنجي = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    ڪنجي = simplify_ڪنجي(ڪنجي, &modifiers);
    إذا (ڪنجي == CSI)
	ڪنجي = K_CSI;
    إذا (IS_SPECIAL(ڪنجي))
    {
	string[0] = CSI;
	string[1] = K_ٻيو(ڪنجي);
	string[2] = K_ٽيون(ڪنجي);
	len = 3;
    }
    وإلا
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ڪنجي = may_adjust_ڪنجي_for_ctrl(modifiers, ڪنجي);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_سرڪايو_modifier(modifiers, ڪنجي);

	len = mb_char2bytes(ڪنجي, string);
    }

    إذا (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	جوڙيو_منزل_داخلا_buf(string2, 3);
	إذا (modifiers == 0x4)
	    ctrl_اڳياڙي_added = 1;
    }

    // Check if the key interrupts.
    {
	صحيح int_ch = check_for_مداخلت(ڪنجي, modifiers);

	إذا (int_ch != NUL)
	{
	    trash_داخلا_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    إذا (ctrl_اڳياڙي_added && len == 2
		    && ((صحيح)string[0]) == 195
		    && ((صحيح)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    جوڙيو_منزل_داخلا_buf(string, len);

    // blank out the pointer if necessary
    إذا (p_mh)
	gui_mch_mousehide(TRUE);

    أرجع TRUE;
}

#إذا defined(FEAT_XIM) || GTK_CHECK_نسخو(3,0,0)
    ساكن gboolean
ڪنجي_release_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		  Gdk_Event_ڪنجي *event اڻ_استعمال_ٿيل,
		  gpointer معلومات اڻ_استعمال_ٿيل)
{
# إذا defined(FEAT_XIM)
    gui.event_وقت = event->وقت;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    أرجع xim_قطار_ڪنجي_press_event(event, FALSE);
# وإلا
    أرجع TRUE;
# endif
}
#جيڪڏهن_پورو


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ساكن صحيح اندريون_selection_clear_event = FALSE;

    ساكن gint
selection_clear_event(GtkWidget		*widget اڻ_استعمال_ٿيل,
		      GdkEventSelection	*event,
		      gpointer		user_معلومات اڻ_استعمال_ٿيل)
{
    اندريون_selection_clear_event = TRUE;
    إذا (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    وإلا
	clip_lose_selection(&clip_تارو);
    اندريون_selection_clear_event = FALSE;

    أرجع TRUE;
}

#وصف_ڪريو RS_NONE	0	// selection_received_cb() not called yet
#وصف_ڪريو RS_OK	1	// selection_received_cb() called and OK
#وصف_ڪريو RS_FAIL	2	// selection_received_cb() called and failed
ساكن صحيح received_selection = RS_NONE;

    ساكن فراغ
selection_received_cb(GtkWidget		*widget اڻ_استعمال_ٿيل,
		      Gtk_Selection_معلومات	*معلومات,
		      guint		وقت_2 اڻ_استعمال_ٿيل,
		      gpointer		user_معلومات اڻ_استعمال_ٿيل)
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
	cbd = &clip_تارو;

    متن = (char_u *)gtk_selection_معلومات_get_معلومات(معلومات);
    len = gtk_selection_معلومات_get_ڊيگهائي(معلومات);

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
	convert_تياري(&conv, enc, p_enc);
	إذا (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, متن, &len);
	    إذا (tmpbuf != عدم)
		متن = tmpbuf;
	    convert_تياري(&conv, عدم, عدم);
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
	    إذا (داخلا_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&داخلا_conv, tmpbuf_utf8, &len);
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
	    convert_تياري_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    إذا (conv.vc_type != CONV_NONE)
	    {
		متن += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, متن, &len);
		convert_تياري(&conv, عدم, عدم);
	    }
	    إذا (tmpbuf != عدم)
		متن = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    طالما (len > 0 && متن[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, متن, (ڊگهو)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ساكن فراغ
selection_get_cb(GtkWidget	    *widget اڻ_استعمال_ٿيل,
		 Gtk_Selection_معلومات   *selection_معلومات,
		 guint		    info,
		 guint		    وقت_2 اڻ_استعمال_ٿيل,
		 gpointer	    user_معلومات اڻ_استعمال_ٿيل)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    صحيح		    ڊيگهائي;
    صحيح		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    إذا (gtk_selection_معلومات_get_selection(selection_معلومات)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وإلا
	cbd = &clip_تارو;

    إذا (!cbd->owned)
	أرجع;			// Shouldn't ever happen

    إذا (info != (guint)هدف_STRING
	    && (!clip_html || info != (guint)هدف_HTML)
	    && info != (guint)هدف_UTF8_STRING
	    && info != (guint)هدف_VIMENC
	    && info != (guint)هدف_VIM
	    && info != (guint)هدف_VIMENC_MT
	    && info != (guint)هدف_VIM_MT
	    && info != (guint)هدف_COMPOUND_متن
	    && info != (guint)هدف_متن_PLAIN
	    && info != (guint)هدف_متن_PLAIN_UTF8
	    && info != (guint)هدف_متن)
	أرجع;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    إذا (motion_type < 0 || string == عدم)
	أرجع;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    ڊيگهائي = MIN(tmplen, (long_u)(G_MAXINT - 1));

    إذا (info == (guint)هدف_VIM || info == (guint)هدف_VIM_MT)
    {
	tmpbuf = alloc(ڊيگهائي + 1);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)ڊيگهائي);
	}
	// For our own format, the first byte contains the motion type
	++ڊيگهائي;
	vim_free(string);
	string = tmpbuf;
	إذا (info == (guint)هدف_VIM)
	    type = vim_atom;
	وإلا
	    type = vim_mt_atom;
    }

    وإلا إذا (info == (guint)هدف_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_تياري_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	إذا (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &ڊيگهائي);
	    convert_تياري(&conv, عدم, عدم);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	إذا (string != عدم)
	{
	    tmpbuf = alloc(ڊيگهائي + 2);
	    إذا (tmpbuf != عدم)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)ڊيگهائي);
		vim_free(string);
		string = tmpbuf;
		ڊيگهائي += 2;
	    }

#إذا !GTK_CHECK_نسخو(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_معلومات->type = selection_معلومات->هدف;
	    selection_معلومات->format = 16;	// 16 bits per char
#جيڪڏهن_پورو
	    gtk_selection_معلومات_سيٽ(selection_معلومات, html_atom, 16,
							      string, ڊيگهائي);
	    vim_free(string);
	}
	أرجع;
    }
    وإلا إذا (info == (guint)هدف_VIMENC || info == (guint)هدف_VIMENC_MT)
    {
	صحيح l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(ڊيگهائي + l + 2);
	إذا (tmpbuf != عدم)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)ڊيگهائي);
	    ڊيگهائي += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	إذا (info == (guint)هدف_VIMENC)
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
	    tmpbuf = string_convert(&output_conv, string, &ڊيگهائي);
	    vim_free(string);
	    إذا (tmpbuf == عدم)
		أرجع;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	إذا (g_utf8_validate((ثابت حرف *)string, (gssize)ڊيگهائي, عدم))
	{
	    gtk_selection_معلومات_سيٽ_متن(selection_معلومات,
					(ثابت حرف *)string, ڊيگهائي);
	}
	vim_free(string);
	أرجع;
    }

    إذا (string != عدم)
    {
#إذا !GTK_CHECK_نسخو(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_معلومات->type = selection_معلومات->هدف;
	selection_معلومات->format = 8;	// 8 bits per char
#جيڪڏهن_پورو
	gtk_selection_معلومات_سيٽ(selection_معلومات, type, 8, string, ڊيگهائي);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    صحيح
gui_mch_early_init_check(صحيح give_پيغام)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    إذا ((p == عدم || *p == NUL) && (q == عدم || *q == NUL))
    {
	gui.dying = true;
	إذا (give_پيغام)
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
#جيڪڏهن_وصف_ٿيل USE_GRESOURCE
    ساكن صحيح res_registered = FALSE;

    إذا (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_رجسٽر_resource();
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
    إذا (gtk_socket_id == 0)
	using_gnome = 1;
#جيڪڏهن_پورو

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_سيٽ_prgname("gvim");

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
ساكن صحيح mouse_timed_ٻاهريون = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ساكن timeout_cb_type
mouse_click_timer_cb(gpointer معلومات)
{
    // we don't use this information currently
    صحيح *timed_ٻاهريون = (صحيح *) معلومات;

    *timed_ٻاهريون = TRUE;
    أرجع FALSE;		// don't happen again
}

ساكن guint		motion_repeat_timer  = 0;
ساكن صحيح		motion_repeat_وٿي = FALSE;
ساكن timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ساكن فراغ
process_motion_notify(صحيح x, صحيح y, GdkModifierType state)
{
    صحيح	    button;
    int_u   vim_modifiers;
    Gtk_ورهاست ورهاست;

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
    gtk_widget_get_ورهاست(gui.drawarea, &ورهاست);

    إذا (x < 0 || y < 0
	    || x >= ورهاست.ويڪر
	    || y >= ورهاست.اوچائي)
    {

	صحيح dx;
	صحيح dy;
	صحيح offshoot;
	صحيح delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - ورهاست.ويڪر;
	dy = y < 0 ? -y : y - ورهاست.اوچائي;

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
	    motion_repeat_timer = timeout_جوڙيو(delay, motion_repeat_timer_cb,
									 عدم);
    }
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن GdkDevice *
gui_gtk_get_اشارو_device(GtkWidget *widget)
{
    Gdk_دري * ثابت win = gtk_widget_get_دري(widget);
    GdkDisplay * ثابت dpy = gdk_دري_get_display(win);
# إذا GTK_CHECK_نسخو(3,20,0)
    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
    أرجع gdk_seat_get_اشارو(seat);
# وإلا
    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
    أرجع gdk_device_manager_get_client_اشارو(mngr);
# endif
}

    ساكن Gdk_دري *
gui_gtk_get_اشارو(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_دري * ثابت win = gtk_widget_get_دري(widget);
    GdkDevice * ثابت dev = gui_gtk_get_اشارو_device(widget);
    أرجع gdk_دري_get_device_position(win, dev , x, y, state);
}

# إذا defined(FEAT_GUI_TABLINE)
    ساكن Gdk_دري *
gui_gtk_دري_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ثابت dev = gui_gtk_get_اشارو_device(widget);
    أرجع gdk_device_get_دري_at_position(dev, x, y);
}
# endif
#وإلا // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_اشارو(wid, x, y, s) \
    gdk_دري_get_اشارو((wid)->دري_2, x, y, s)
# define gui_gtk_دري_at_position(wid, x, y)	gdk_دري_at_اشارو(x, y)
#جيڪڏهن_پورو

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ساكن timeout_cb_type
motion_repeat_timer_cb(gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح		    x;
    صحيح		    y;
    GdkModifierType state;

    gui_gtk_get_اشارو(gui.drawarea, &x, &y, &state);

    إذا (!(state & (GDK_BUTTON1_نقاب | GDK_BUTTON2_نقاب |
		   GDK_BUTTON3_نقاب | GDK_BUTTON4_نقاب |
		   GDK_BUTTON5_نقاب)))
    {
	motion_repeat_timer = 0;
	أرجع FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    إذا (vim_استعمال_ٿيل_اندريون_داخلا_buf() > 10)
	أرجع TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    إذا (motion_repeat_وٿي)
	x += gui.char_ويڪر;

    motion_repeat_وٿي = !motion_repeat_وٿي;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    أرجع FALSE;
}

    ساكن gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (event->is_hint)
    {
	صحيح		x;
	صحيح		y;
	GdkModifierType	state;

	gui_gtk_get_اشارو(widget, &x, &y, &state);
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
		   gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح button;
    صحيح repeated_click = FALSE;
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_وقت = event->وقت;

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
    إذا (!mouse_timed_ٻاهريون && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_ٻاهريون = FALSE;
    mouse_click_timer = timeout_جوڙيو(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_ٻاهريون);

    بدل (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	حالة 1:
		button = MOUSE_کاٻو;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_نقاب;
		اكسر;
	حالة 2: button = MOUSE_وچ; اكسر;
	حالة 3: button = MOUSE_ساڄو; اكسر;
	حالة 8: button = MOUSE_X1; اكسر;
	حالة 9: button = MOUSE_X2; اكسر;
	افتراضي:
	    أرجع FALSE;		// Unknown button
    }

#جيڪڏهن_وصف_ٿيل FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#جيڪڏهن_پورو

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
	     gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#إذا GTK_CHECK_نسخو(3,4,0)
    ساكن مزدوج  acc_x, acc_y;
# إذا !GTK_CHECK_نسخو(3,22,0)
    ساكن guint32 آخري_smooth_event_وقت;
# endif
#جيڪڏهن_پورو

    إذا (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    بدل (event->direction)
    {
	حالة GDK_SCROLL_مٿي:
	    button = MOUSE_4;
	    اكسر;
	حالة GDK_SCROLL_هيٺ_ڏانهن:
	    button = MOUSE_5;
	    اكسر;
	حالة GDK_SCROLL_کاٻو:
	    button = MOUSE_7;
	    اكسر;
	حالة GDK_SCROLL_ساڄو:
	    button = MOUSE_6;
	    اكسر;
#إذا GTK_CHECK_نسخو(3,4,0)
	حالة GDK_SCROLL_SMOOTH:
	    إذا (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		أرجع FALSE;
	    }
# إذا GTK_CHECK_نسخو(3,22,0)
	    إذا (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# وإلا
	    إذا (event->وقت - آخري_smooth_event_وقت > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# إذا !GTK_CHECK_نسخو(3,22,0)
	    آخري_smooth_event_وقت = event->وقت;
# endif
	    اكسر;
#جيڪڏهن_پورو
	افتراضي: // This shouldn't happen
	    أرجع FALSE;
    }

#جيڪڏهن_وصف_ٿيل FEAT_XIM
    // cancel any preediting
    إذا (im_is_preediting())
	xim_reset();
#جيڪڏهن_پورو

    vim_modifiers = modifiers_gdk2mouse(event->state);

#إذا GTK_CHECK_نسخو(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    إذا (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	طالما (acc_x >= 1.0)
	{ //ساڄو
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_x <= -1.0)
	{ //کاٻو
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y >= 1.0)
	{ //هيٺ_ڏانهن
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (صحيح)event->x, (صحيح)event->y,
		    FALSE, vim_modifiers);
	}
	طالما (acc_y <= -1.0)
	{ //مٿي
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
#جيڪڏهن_پورو
	gui_send_mouse_event(button, (صحيح)event->x, (صحيح)event->y,
		FALSE, vim_modifiers);

    أرجع TRUE;
}


    ساكن gint
button_release_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		     GdkEventButton *event,
		     gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح x, y;
    int_u vim_modifiers;

    gui.event_وقت = event->وقت;

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
grab_notify_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		  gboolean  was_grabbed,
		  gpointer  معلومات اڻ_استعمال_ٿيل)
{
    إذا (!was_grabbed)
	dragging_button_state = 0;
}


#جيڪڏهن_وصف_ٿيل FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ساكن صحيح
ڳڻپ_and_decode_uri_list(char_u *ٻاهريون, char_u *raw, صحيح len)
{
    صحيح		i;
    char_u	*p = ٻاهريون;
    صحيح		ڳڻپ = 0;

    لكل (i = 0; i < len; ++i)
    {
	إذا (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    إذا (p > ٻاهريون && p[-1] != NUL)
	    {
		++ڳڻپ;
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
    إذا (p > ٻاهريون && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++ڳڻپ;
    }
    أرجع ڳڻپ;
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
parse_uri_list(صحيح *ڳڻپ, char_u *معلومات, صحيح len)
{
    صحيح	    n	    = 0;
    char_u  *tmp    = عدم;
    char_u  **array = عدم;

    إذا (معلومات != عدم && len > 0 && (tmp = alloc(len + 1)) != عدم)
    {
	n = ڳڻپ_and_decode_uri_list(tmp, معلومات, len);
	إذا (n > 0 && (array = ALLOC_MULT(char_u *, n)) != عدم)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *ڳڻپ = n;
    أرجع array;
}

    ساكن فراغ
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_معلومات	*معلومات,
		     guint		وقت_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    صحيح	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_معلومات_get_معلومات(معلومات),
	    gtk_selection_معلومات_get_ڊيگهائي(معلومات));

    إذا (fnames != عدم && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_پڄاڻي(context, TRUE, FALSE, وقت_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    وإلا
	vim_free(fnames);
}

    ساكن فراغ
drag_handle_متن(GdkDragContext	    *context,
		 Gtk_Selection_معلومات   *معلومات,
		 guint		    وقت_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *متن;
    صحيح	    len;
    char_u  *tmpbuf = عدم;

    متن = (char_u *)gtk_selection_معلومات_get_معلومات(معلومات);
    len = gtk_selection_معلومات_get_ڊيگهائي(معلومات);

    إذا (gtk_selection_معلومات_get_معلومات_type(معلومات) == utf8_string_atom)
    {
	إذا (داخلا_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&داخلا_conv, متن, &len);
	إذا (tmpbuf != عدم)
	    متن = tmpbuf;
    }

    dnd_yank_drag_معلومات(متن, (ڊگهو)len);
    gtk_drag_پڄاڻي(context, TRUE, FALSE, وقت_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    إذا (dropkey[2] != 0)
	جوڙيو_منزل_داخلا_buf(dropkey, (صحيح)حجم(dropkey));
    وإلا
	جوڙيو_منزل_داخلا_buf(dropkey + 3, (صحيح)(حجم(dropkey) - 3));
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
		      guint		وقت_2,
		      gpointer		user_معلومات اڻ_استعمال_ٿيل)
{
    GdkModifierType state;

    // Guard against trash
    ثابت guchar * ثابت معلومات_معلومات = gtk_selection_معلومات_get_معلومات(معلومات);
    ثابت gint معلومات_ڊيگهائي = gtk_selection_معلومات_get_ڊيگهائي(معلومات);
    ثابت gint معلومات_format = gtk_selection_معلومات_get_format(معلومات);

    إذا (معلومات_معلومات == عدم
	    || معلومات_ڊيگهائي <= 0
	    || معلومات_format != 8
	    || معلومات_معلومات[معلومات_ڊيگهائي] != '\0')
    {
	gtk_drag_پڄاڻي(context, FALSE, FALSE, وقت_2);
	أرجع;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_اشارو(widget, عدم, عدم, &state);

    // Not sure about the role of "text/plain" here...
    إذا (info == (guint)هدف_متن_URI_LIST)
	drag_handle_uri_list(context, معلومات, وقت_2, state, x, y);
    وإلا
	drag_handle_متن(context, معلومات, وقت_2, state);

}
#جيڪڏهن_پورو // FEAT_DND


#إذا defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ساكن فراغ
sm_client_check_بدليو_any(GnomeClient	    *client اڻ_استعمال_ٿيل,
			    gint	    ڪنجي,
			    GnomeDialogType type اڻ_استعمال_ٿيل,
			    gpointer	    معلومات اڻ_استعمال_ٿيل)
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
    shutdown_cancelled = check_بدليو_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    ٻاهريون_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_ڪنجي_return(ڪنجي, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    ساكن gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase اڻ_استعمال_ٿيل,
			GnomeSaveStyle	    save_style اڻ_استعمال_ٿيل,
			gboolean	    shutdown اڻ_استعمال_ٿيل,
			GnomeInteractStyle  interact_style,
			gboolean	    fast اڻ_استعمال_ٿيل,
			gpointer	    معلومات اڻ_استعمال_ٿيل)
{
    ساكن ثابت حرف	suffix[] = "-session.vim";
    حرف		*session_file;
    بنانشان صحيح	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    إذا (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_عام,
					 &sm_client_check_بدليو_any,
					 عدم);
    ٻاهريون_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_رستو(
			gnome_client_get_config_اڳياڙي(client));
    len = لڙي_ڊيگهه(session_file);

    إذا (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(حرف, session_file, len + حجم(suffix));
    يادگيري_نقل(session_file + len, suffix, حجم(suffix));

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

	gnome_client_سيٽ_discard_command(client, i, (حرف **)argv);

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
	argv[i++] = gtk_دري_get_role(GTK_دري(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = عدم;

	gnome_client_سيٽ_restart_command(client, i, (حرف **)argv);
	gnome_client_سيٽ_clone_command(client, 0, عدم);
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
sm_client_die(GnomeClient *client اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Don't write messages to the GUI anymore
    ڀريل_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    ساكن فراغ
تياري_save_yourself(فراغ)
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
local_xsmp_handle_درخواستون(
    GIOChannel		*ذريعو اڻ_استعمال_ٿيل,
    GIOCondition	condition,
    gpointer		معلومات)
{
    إذا (condition == G_IO_اندريون)
    {
	// Do stuff; maybe close connection
	إذا (xsmp_handle_درخواستون() == FAIL)
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
تياري_save_yourself(فراغ)
{
    Atom    *existing_atoms = عدم;
    صحيح	    ڳڻپ = 0;

# ifdef USE_XSMP
    إذا (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_جوڙيو_watch(g_io, G_IO_اندريون | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_درخواستون, (gpointer)g_io);
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

	Gdk_دري * ثابت mainwin_win = gtk_widget_get_دري(gui.mainwin);
	إذا (XGetWMProtocols(dpy, GDK_دري_XID(mainwin_win),
		    &existing_atoms, &ڳڻپ))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    صحيح	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    لكل (i = 0; i < ڳڻپ; ++i)
		إذا (existing_atoms[i] == save_yourself_xatom)
		    اكسر;

	    إذا (i == ڳڻپ)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, ڳڻپ + 1);
		إذا (new_atoms != عدم)
		{
		    يادگيري_نقل(new_atoms, existing_atoms, ڳڻپ * حجم(Atom));
		    new_atoms[ڳڻپ] = save_yourself_xatom;
		    XSetWMProtocols(GDK_دري_XDISPLAY(mainwin_win),
			    GDK_دري_XID(mainwin_win),
			    new_atoms, ڳڻپ + 1);
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
		    GdkEvent *event اڻ_استعمال_ٿيل,
		    gpointer معلومات اڻ_استعمال_ٿيل)
{
    XEvent *xevent = (XEvent *)xev;

    إذا (xevent != عدم
	    && xevent->type == Client_پيغام
	    && xevent->xclient.پيغام_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.معلومات.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	ٻاهريون_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_دري_XDISPLAY(gtk_widget_get_دري(gui.mainwin)),
		    GDK_دري_XID(gtk_widget_get_دري(gui.mainwin)),
		    عدم, 0);
	أرجع GDK_FILTER_REMOVE;
    }

    أرجع GDK_FILTER_CONTINUE;
}
#جيڪڏهن_پورو // !USE_GNOME_SESSION

    ساكن GdkPixbuf *
pixbuf_new_from_png_معلومات(ثابت بنانشان حرف *معلومات, بنانشان صحيح len)
{
    GInput_وهڪرو *وهڪرو;
    GdkPixbuf    *pixbuf;

    وهڪرو = g_يادگيري_داخلا_وهڪرو_new_from_معلومات(معلومات, len, عدم);
    pixbuf = gdk_pixbuf_new_from_وهڪرو(وهڪرو, عدم, عدم);
    g_object_unref(وهڪرو);
    أرجع pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ساكن فراغ
mainwin_realize(GtkWidget *widget اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
#ضمن "../runtime/vim16x16_png.h"
#ضمن "../runtime/vim32x32_png.h"
#ضمن "../runtime/vim48x48_png.h"

    Gdk_دري * ثابت mainwin_win = gtk_widget_get_دري(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    إذا (echo_wid_arg)
    {
	إذا (gui_mch_get_display())
	    لکو("WID: %ld\n", (ڊگهو)GDK_دري_XID(mainwin_win));
	وإلا
	    لکو("WID: 0\n");
	fflush(stdout);
    }

    إذا (vim_strchr(p_go, GO_ICON) != عدم)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	إذا (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_دري_سيٽ_icon_name(GTK_دري(gui.mainwin), "gvim");
	وإلا
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = عدم;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_معلومات(vim48x48_png, vim48x48_png_len));

	    gtk_دري_سيٽ_icon_list(GTK_دري(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(فراغ *)&g_object_unref, عدم);
	    g_list_free(icons);
	}
    }

#إذا !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_دري_جوڙيو_filter(عدم, &global_event_filter, عدم);
#جيڪڏهن_پورو
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#إذا defined(USE_GNOME_SESSION)
    إذا (using_gnome)
#جيڪڏهن_پورو
	تياري_save_yourself();

#جيڪڏهن_وصف_ٿيل FEAT_CLIENTSERVER
    إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 && gui_mch_get_display())
    {
	إذا (serverName == عدم && server_Delayed_شروعات_Name != عدم)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_دري = GDK_دري_XID(mainwin_win);

	    (فراغ)server_رجسٽر_Name(GDK_دري_XDISPLAY(mainwin_win),
				    server_Delayed_شروعات_Name);
	}
	وإلا
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_دري(GDK_دري_XDISPLAY(mainwin_win),
					GDK_دري_XID(mainwin_win));
	}
	gtk_widget_جوڙيو_events(gui.mainwin, GDK_PROPERTY_CHANGE_نقاب);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), عدم);
    }
#جيڪڏهن_پورو
}

    ساكن GdkCursor *
create_blank_اشارو(فراغ)
{
    GdkCursor	*cursor;

#إذا GTK_CHECK_نسخو(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#وإلا
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_دري	*پاڙ_دري = عدم;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_نقاب;
    حرف	blank_معلومات[] = { 0x0 };
    پاڙ_دري = gtk_widget_get_پاڙ_دري(gui.mainwin);
    blank_نقاب = gdk_bitmap_create_from_معلومات(پاڙ_دري, blank_معلومات, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_نقاب, blank_نقاب,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_نقاب);
#جيڪڏهن_پورو

    أرجع cursor;
}

    ساكن فراغ
mainwin_screen_بدليو_cb(GtkWidget  *widget,
			  GdkScreen  *اڳيون_screen اڻ_استعمال_ٿيل,
			  gpointer   معلومات اڻ_استعمال_ٿيل)
{
    إذا (!gtk_widget_has_screen(widget))
	أرجع;

    /*
     * Recreate the invisible mouse cursor.
     */
    إذا (gui.blank_اشارو != عدم)
#إذا GTK_CHECK_نسخو(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_اشارو));
#وإلا
	gdk_cursor_unref(gui.blank_اشارو);
#جيڪڏهن_پورو

    gui.blank_اشارو = create_blank_اشارو();

    إذا (gui.اشارو_hidden && gtk_widget_get_دري(gui.drawarea) != عدم)
	gdk_دري_سيٽ_cursor(gtk_widget_get_دري(gui.drawarea),
		gui.blank_اشارو);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    إذا (gui.متن_context != عدم)
	g_object_unref(gui.متن_context);

    gui.متن_context = gtk_widget_create_pango_context(widget);
    pango_context_سيٽ_بنياد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    إذا (gui.norm_font != عدم)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_سيٽ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ساكن gboolean
mainwin_state_event_cb(GtkWidget *widget اڻ_استعمال_ٿيل,
		       Gdk_Event_دري_State *event,
		       gpointer user_معلومات اڻ_استعمال_ٿيل)
{
    إذا (event->new_دري_state & GDK_دري_STATE_FULLSCREEN)
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
drawarea_realize_cb(GtkWidget *widget, gpointer معلومات اڻ_استعمال_ٿيل)
{
    GtkWidget *sbar;
    Gtk_ورهاست ورهاست;

#جيڪڏهن_وصف_ٿيل FEAT_XIM
    xim_init();
#جيڪڏهن_پورو
    gui_mch_new_colors();
#إذا GTK_CHECK_نسخو(3,0,0)
    gui.surface = gdk_دري_create_similar_surface(
	    gtk_widget_get_دري(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_ويڪر(widget),
	    gtk_widget_get_allocated_اوچائي(widget));
#وإلا
    gui.متن_gc = gdk_gc_new(gui.drawarea->دري_2);
#جيڪڏهن_پورو

    gui.blank_اشارو = create_blank_اشارو();
    إذا (gui.اشارو_hidden)
	gdk_دري_سيٽ_cursor(gtk_widget_get_دري(widget), gui.blank_اشارو);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_کاٻو].id;
    إذا (!sbar || (!gui.which_scrollbars[SBAR_کاٻو]
				    && firstwin->w_scrollbars[SBAR_ساڄو].id))
	sbar = firstwin->w_scrollbars[SBAR_ساڄو].id;
    gtk_widget_get_ورهاست(sbar, &ورهاست);
    إذا (sbar && gtk_widget_get_realized(sbar) && ورهاست.ويڪر)
	gui.scrollbar_ويڪر = ورهاست.ويڪر;

    sbar = gui.bottom_sbar.id;
    إذا (sbar && gtk_widget_get_realized(sbar) && ورهاست.اوچائي)
	gui.scrollbar_اوچائي = ورهاست.اوچائي;
}

/*
 * Properly clean up on shutdown.
 */
    ساكن فراغ
drawarea_unrealize_cb(GtkWidget *widget اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Don't write messages to the GUI anymore
    ڀريل_screen = FALSE;

#جيڪڏهن_وصف_ٿيل FEAT_XIM
    im_shutdown();
#جيڪڏهن_پورو
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

#إذا GTK_CHECK_نسخو(3,0,0)
    إذا (gui.surface != عدم)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = عدم;
    }
#وإلا
    g_object_unref(gui.متن_gc);
    gui.متن_gc = عدم;
#جيڪڏهن_پورو

#إذا GTK_CHECK_نسخو(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_اشارو));
#وإلا
    gdk_cursor_unref(gui.blank_اشارو);
#جيڪڏهن_پورو
    gui.blank_اشارو = عدم;
}

#إذا GTK_CHECK_نسخو(3,22,2)
    ساكن فراغ
drawarea_style_updated_cb(GtkWidget *widget اڻ_استعمال_ٿيل,
			 gpointer معلومات اڻ_استعمال_ٿيل)
#وإلا
    ساكن فراغ
drawarea_style_سيٽ_cb(GtkWidget	*widget اڻ_استعمال_ٿيل,
		      GtkStyle	*اڳيون_style اڻ_استعمال_ٿيل,
		      gpointer	معلومات اڻ_استعمال_ٿيل)
#جيڪڏهن_پورو
{
    gui_mch_new_colors();
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       معلومات اڻ_استعمال_ٿيل)
{
    ساكن صحيح cur_ويڪر = 0;
    ساكن صحيح cur_اوچائي = 0;

    g_return_val_if_fail(event
	    && event->ويڪر >= 1 && event->اوچائي >= 1, TRUE);

# إذا GTK_CHECK_نسخو(3,22,2) && !GTK_CHECK_نسخو(3,22,4)
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

    إذا (event->ويڪر == cur_ويڪر && event->اوچائي == cur_اوچائي)
	أرجع TRUE;

    cur_ويڪر = event->ويڪر;
    cur_اوچائي = event->اوچائي;

    إذا (gui.surface != عدم)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_دري_create_similar_surface(
	    gtk_widget_get_دري(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->ويڪر, event->اوچائي);

    gtk_widget_قطار_draw(widget);

    أرجع TRUE;
}
#جيڪڏهن_پورو

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ساكن gint
delete_event_cb(GtkWidget *widget اڻ_استعمال_ٿيل,
		GdkEventAny *event اڻ_استعمال_ٿيل,
		gpointer معلومات اڻ_استعمال_ٿيل)
{
    gui_shell_closed();
    أرجع TRUE;
}

#إذا defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ساكن صحيح
get_شي_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation شي_orientation = GTK_ORIENTATION_HORIZONTAL;

    إذا (using_gnome && widget != عدم)
    {
	GtkWidget *والد;
	Bonobo_Dock_شي *dockitem;

	والد	 = gtk_widget_get_والد(widget);
	إذا (G_TYPE_FROM_INSTANCE(والد) == BONOBO_TYPE_DOCK_شي)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = والد;
	    dockitem = BONOBO_DOCK_شي(widget);

	    إذا (dockitem == عدم || dockitem->is_floating)
		أرجع 0;
	    شي_orientation = bonobo_dock_شي_get_orientation(dockitem);
	}
    }
# وإلا
#  define شي_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    إذا (widget != عدم
	    && شي_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# إذا GTK_CHECK_نسخو(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_ورهاست ورهاست;

	gtk_widget_get_ورهاست(widget, &ورهاست);
	إذا (ورهاست.اوچائي > 1)
	    أرجع ورهاست.اوچائي;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  إذا GTK_CHECK_نسخو(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_اوچائي(widget, &min_h, &natural_h);
	    أرجع MAX(min_h, natural_h);
	}
#  وإلا
	أرجع ورهاست.اوچائي;
#  endif
# وإلا
	إذا (orientation == GTK_ORIENTATION_HORIZONTAL)
	    أرجع widget->ورهاست.اوچائي;
	وإلا
	    أرجع widget->ورهاست.ويڪر;
# endif
    }
    أرجع 0;
}
#جيڪڏهن_پورو

    ساكن صحيح
get_menu_tool_ويڪر(فراغ)
{
    صحيح ويڪر = 0;

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    ويڪر += get_شي_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    ويڪر += get_شي_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    إذا (gui.tabline != عدم)
	ويڪر += get_شي_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#جيڪڏهن_پورو

    أرجع ويڪر;
}

    ساكن صحيح
get_menu_tool_اوچائي(فراغ)
{
    صحيح اوچائي = 0;

#جيڪڏهن_وصف_ٿيل FEAT_MENU
    اوچائي += get_شي_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    اوچائي += get_شي_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    إذا (gui.tabline != عدم
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	اوچائي += get_شي_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#جيڪڏهن_پورو

    أرجع اوچائي;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ساكن صحيح init_دري_hints_state = 0;

    ساكن فراغ
update_دري_manager_hints(صحيح force_ويڪر, صحيح force_اوچائي)
{
    ساكن صحيح old_ويڪر  = 0;
    ساكن صحيح old_اوچائي = 0;
    ساكن صحيح old_min_ويڪر  = 0;
    ساكن صحيح old_min_اوچائي = 0;
    ساكن صحيح old_char_ويڪر  = 0;
    ساكن صحيح old_char_اوچائي = 0;

    صحيح ويڪر;
    صحيح اوچائي;
    صحيح min_ويڪر;
    صحيح min_اوچائي;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    إذا (!(force_ويڪر && force_اوچائي)  &&  init_دري_hints_state > 0)
    {
	// Don't do it!
	init_دري_hints_state = 2;
	أرجع;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    ويڪر  = gui_get_بنياد_ويڪر();
    اوچائي = gui_get_بنياد_اوچائي();
#جيڪڏهن_وصف_ٿيل FEAT_MENU
    اوچائي += tabline_اوچائي() * gui.char_اوچائي;
#جيڪڏهن_پورو
    ويڪر  += get_menu_tool_ويڪر();
    اوچائي += get_menu_tool_اوچائي();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    إذا (force_ويڪر && force_اوچائي)
    {
	min_ويڪر  = force_ويڪر;
	min_اوچائي = force_اوچائي;
    }
    وإلا
    {
	min_ويڪر  = ويڪر  + MIN_COLUMNS * gui.char_ويڪر;
	min_اوچائي = اوچائي + MIN_سٽون   * gui.char_اوچائي;
    }

    // Avoid an expose event when the size didn't change.
    إذا (ويڪر != old_ويڪر
	    || اوچائي != old_اوچائي
	    || min_ويڪر != old_min_ويڪر
	    || min_اوچائي != old_min_اوچائي
	    || gui.char_ويڪر != old_char_ويڪر
	    || gui.char_اوچائي != old_char_اوچائي)
    {
	GdkGeometry	geometry;
	Gdk_دري_Hints	geometry_نقاب;

	geometry.ويڪر_inc   = gui.char_ويڪر;
	geometry.اوچائي_inc  = gui.char_اوچائي;
	geometry.بنياد_ويڪر  = ويڪر;
	geometry.بنياد_اوچائي = اوچائي;
	geometry.min_ويڪر   = min_ويڪر;
	geometry.min_اوچائي  = min_اوچائي;
	geometry_نقاب	     = GDK_HINT_بنياد_سائيز|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_سائيز;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_دري_سيٽ_geometry_hints(GTK_دري(gui.mainwin), عدم,
				      &geometry, geometry_نقاب);
	old_ويڪر       = ويڪر;
	old_اوچائي      = اوچائي;
	old_min_ويڪر   = min_ويڪر;
	old_min_اوچائي  = min_اوچائي;
	old_char_ويڪر  = gui.char_ويڪر;
	old_char_اوچائي = gui.char_اوچائي;
    }
}

#إذا defined(FEAT_GUI_DARKTHEME)
    فراغ
gui_mch_سيٽ_dark_theme(صحيح dark)
{
# إذا GTK_CHECK_نسخو(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_سيٽ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, عدم);
# endif
}
#جيڪڏهن_پورو // FEAT_GUI_DARKTHEME

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ساكن فراغ
icon_سائيز_بدليو_foreach(GtkWidget *widget, gpointer user_معلومات)
{
    إذا (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# إذا GTK_CHECK_نسخو(3,10,0)
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ثابت Gtk_Icon_سائيز icon_سائيز = GPOINTER_منزل_INT(user_معلومات);
	    ثابت gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, عدم);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_سائيز);
	}
# وإلا
	// User-defined icons are stored in a GtkIconSet
	إذا (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_سيٽ)
	{
	    Gtk_Icon_سيٽ	*icon_سيٽ;
	    Gtk_Icon_سائيز	icon_سائيز;

	    gtk_image_get_icon_سيٽ(image, &icon_سيٽ, &icon_سائيز);
	    icon_سائيز = (Gtk_Icon_سائيز)(ڊگهو)user_معلومات;

	    gtk_icon_سيٽ_ref(icon_سيٽ);
	    gtk_image_سيٽ_from_icon_سيٽ(image, icon_سيٽ, icon_سائيز);
	    gtk_icon_سيٽ_unref(icon_سيٽ);
	}
# endif
    }
    وإلا إذا (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_سائيز_بدليو_foreach,
			      user_معلومات);
    }
}

    ساكن فراغ
سيٽ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_سائيز	    سائيز;
    Gtk_Icon_سائيز	    oldsize;

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

    gtk_toolbar_سيٽ_style(toolbar, style);
# إذا !GTK_CHECK_نسخو(3,0,0)
    gtk_toolbar_سيٽ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    بدل (tbis_flags)
    {
	حالة TBIS_TINY:	    سائيز = GTK_ICON_سائيز_MENU;		اكسر;
	حالة TBIS_SMALL:    سائيز = GTK_ICON_سائيز_SMALL_TOOLBAR;	اكسر;
	حالة TBIS_MEDIUM:   سائيز = GTK_ICON_سائيز_BUTTON;	اكسر;
	حالة TBIS_LARGE:    سائيز = GTK_ICON_سائيز_LARGE_TOOLBAR;	اكسر;
	حالة TBIS_HUGE:     سائيز = GTK_ICON_سائيز_DND;		اكسر;
	حالة TBIS_GIANT:    سائيز = GTK_ICON_سائيز_DIALOG;	اكسر;
	افتراضي:	    سائيز = GTK_ICON_سائيز_INVALID;	اكسر;
    }
    oldsize = gtk_toolbar_get_icon_سائيز(toolbar);

    إذا (سائيز == GTK_ICON_سائيز_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_سائيز(toolbar);
	سائيز = gtk_toolbar_get_icon_سائيز(toolbar);
    }
    إذا (سائيز != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_سائيز_بدليو_foreach,
			      GINT_منزل_اشارو((صحيح)سائيز));
    }
    gtk_toolbar_سيٽ_icon_سائيز(toolbar, سائيز);
}

#جيڪڏهن_پورو // FEAT_TOOLBAR

#إذا defined(FEAT_GUI_TABLINE)
ساكن صحيح ignore_tabline_evt = FALSE;
ساكن GtkWidget *tabline_menu;
# إذا !GTK_CHECK_نسخو(3,0,0)
ساكن GtkTooltips *tabline_tooltip;
# endif
ساكن صحيح clicked_صفحو;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ساكن فراغ
tabline_menu_handler(Gtk_Menu_شي *شي اڻ_استعمال_ٿيل, gpointer user_معلومات)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_صفحو, (صحيح)(ڊگهو)user_معلومات);
}

    ساكن فراغ
جوڙيو_tabline_menu_شي(GtkWidget *menu, char_u *متن, صحيح resp)
{
    GtkWidget	*شي;
    char_u	*utf_متن;

    utf_متن = CONVERT_منزل_UTF8(متن);
    شي = gtk_menu_شي_new_with_label((ثابت حرف *)utf_متن);
    gtk_widget_show(شي);
    CONVERT_منزل_UTF8_FREE(utf_متن);

    gtk_container_جوڙيو(GTK_CONTAINER(menu), شي);
    g_signal_connect(G_OBJECT(شي), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_منزل_اشارو(resp));
}

/*
 * Create a menu for the tab line.
 */
    ساكن GtkWidget *
create_tabline_menu(فراغ)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    جوڙيو_tabline_menu_شي(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    جوڙيو_tabline_menu_شي(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    جوڙيو_tabline_menu_شي(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

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
	Gdk_دري	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	إذا (hold_gui_events || cmdwin_type != 0)
	    أرجع TRUE;

	tabwin = gui_gtk_دري_at_position(gui.mainwin, &x, &y);

	gdk_دري_get_user_معلومات(tabwin, (gpointer)&tabwidget);
	clicked_صفحو = GPOINTER_منزل_INT(g_object_get_معلومات(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	إذا (bevent->button == 3)
	{
# إذا GTK_CHECK_نسخو(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    إذا (gui.is_wayland)
	    {
		صحيح x2, y2;
		gui_gtk_get_اشارو(gui.mainwin, &x2, &y2, عدم);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_دري(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, عدم);
	    }
	    وإلا
#  endif
	    gtk_menu_popup_at_اشارو(GTK_MENU(widget), event);
# وإلا
	    gtk_menu_popup(GTK_MENU(widget), عدم, عدم, عدم, عدم,
						bevent->button, bevent->وقت);
# endif
	    // We handled the event.
	    أرجع TRUE;
	}
	وإلا إذا (bevent->button == 1)
	{
	    إذا (clicked_صفحو == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	وإلا إذا (bevent->button == 2)
	{
	    إذا (clicked_صفحو != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_صفحو, TABLINE_MENU_CLOSE);
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
	GtkNotebook	*notebook اڻ_استعمال_ٿيل,
	gpointer	*صفحو اڻ_استعمال_ٿيل,
	gint		idx,
	gpointer	معلومات اڻ_استعمال_ٿيل)
{
    إذا (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# إذا GTK_CHECK_نسخو(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ساكن فراغ
on_tab_reordered(
	GtkNotebook	*notebook اڻ_استعمال_ٿيل,
	gpointer	*صفحو اڻ_استعمال_ٿيل,
	gint		idx,
	gpointer	معلومات اڻ_استعمال_ٿيل)
{
    إذا (ignore_tabline_evt)
	أرجع;

    إذا ((tabpage_اشاريو(curtab) - 1) < idx)
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

	    gtk_دري_get_سائيز(GTK_دري(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_سيٽ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_دري_manager_hints(0, 0);
	إذا (showit)
	    gtk_widget_سيٽ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
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
    GtkWidget	    *صفحو;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    صحيح		    nr = 0;
    صحيح		    tab_انگ;
    صحيح		    curtabidx = 0;
    char_u	    *labeltext;

    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    لكل (tp = پهريون_tabpage; tp != عدم; tp = tp->tp_ايندڙ, ++nr)
    {
	إذا (tp == curtab)
	    curtabidx = nr;

	tab_انگ = nr + 1;

	صفحو = gtk_notebook_get_nth_صفحو(GTK_NOTEBOOK(gui.tabline), nr);
	إذا (صفحو == عدم)
	{
	    // Add notebook page
# إذا GTK_CHECK_نسخو(3,2,0)
	    صفحو = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_سيٽ_homogeneous(GTK_BOX(صفحو), FALSE);
# وإلا
	    صفحو = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(صفحو);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# إذا !GTK_CHECK_نسخو(3,14,0)
	    gtk_misc_سيٽ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_جوڙيو(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_وجهو_صفحو(GTK_NOTEBOOK(gui.tabline),
		    صفحو,
		    event_box,
		    nr);
# إذا GTK_CHECK_نسخو(2,10,0)
	    gtk_notebook_سيٽ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    صفحو,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), صفحو);
	g_object_سيٽ_معلومات(G_OBJECT(event_box), "tab_num",
						     GINT_منزل_اشارو(tab_انگ));
	label = gtk_bin_get_ٻار(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
	gtk_label_سيٽ_متن(GTK_LABEL(label), (ثابت حرف *)labeltext);
	CONVERT_منزل_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_منزل_UTF8(NameBuff);
# إذا GTK_CHECK_نسخو(3,0,0)
	gtk_widget_سيٽ_tooltip_متن(event_box, (ثابت gchar *)labeltext);
# وإلا
	gtk_tooltips_سيٽ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ثابت حرف *)labeltext, عدم);
# endif
	CONVERT_منزل_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    طالما (gtk_notebook_get_nth_صفحو(GTK_NOTEBOOK(gui.tabline), nr) != عدم)
	gtk_notebook_remove_صفحو(GTK_NOTEBOOK(gui.tabline), nr);

    إذا (gtk_notebook_get_current_صفحو(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_سيٽ_current_صفحو(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    فراغ
gui_mch_سيٽ_curtab(صحيح nr)
{
    إذا (gui.tabline == عدم)
	أرجع;

    ignore_tabline_evt = TRUE;
    إذا (gtk_notebook_get_current_صفحو(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_سيٽ_current_صفحو(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#جيڪڏهن_پورو // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    فراغ
gui_gtk_سيٽ_selection_targets(GdkAtom selection)
{
    صحيح		    i, j = 0;
    ساكن صحيح	    n_targets = N_SELECTION_TARGETS;
    ساكن Gtk_هدف_داخلا  targets[N_SELECTION_TARGETS];

    إذا (targets[0].هدف == عدم)
    {
	لكل (i = 0; i < (صحيح)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    إذا (!clip_html && selection_targets[i].info == هدف_HTML)
		n_targets--;
	    وإلا
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_جوڙيو_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    فراغ
gui_gtk_سيٽ_dnd_targets(فراغ)
{
    صحيح		    i, j = 0;
    صحيح		    n_targets = N_DND_TARGETS;
    Gtk_هدف_داخلا  targets[N_DND_TARGETS];

    لكل (i = 0; i < (صحيح)N_DND_TARGETS; ++i)
    {
	إذا (!clip_html && dnd_targets[i].info == هدف_HTML)
	    n_targets--;
	وإلا
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_سيٽ(gui.drawarea,
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

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    إذا (using_gnome)
    {
	gnome_پروگرام_init(VIMPACKAGE, VIM_نسخو_SHORT,
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
#جيڪڏهن_پورو
    VIM_CLEAR(gui_argv);

#إذا GLIB_CHECK_نسخو(2,1,3)
    // Set the human-readable application name
    g_سيٽ_application_name("Vim");
#جيڪڏهن_پورو
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    سيٽ_option_قدر_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
    gui_gtk_رجسٽر_stock_icons();
#جيڪڏهن_پورو
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#إذا !GTK_CHECK_نسخو(3,0,0)
# إذا 0
    gtk_rc_parse("gtkrc");
# endif
#جيڪڏهن_پورو

    // Initialize values
    gui.border_ويڪر = 2;
    gui.scrollbar_ويڪر = SB_DEFAULT_ويڪر;
    gui.scrollbar_اوچائي = SB_DEFAULT_ويڪر;
#إذا GTK_CHECK_نسخو(3,0,0)
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
#جيڪڏهن_پورو

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.پٺ_pixel = gui.def_پٺ_pixel;

    إذا (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	إذا (plug != عدم && gtk_plug_get_socket_دري(GTK_PLUG(plug)) != عدم)
	{
	    gui.mainwin = plug;
	}
	وإلا
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (بنانشان صحيح)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    إذا (gtk_socket_id == 0)
    {
#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
	إذا (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", عدم);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	وإلا
#جيڪڏهن_پورو
	    gui.mainwin = gtk_دري_new(GTK_دري_TOPLEVEL);
    }

    gtk_widget_سيٽ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.متن_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_سيٽ_بنياد_dir(gui.متن_context, PANGO_DIRECTION_LTR);

    gtk_container_سيٽ_border_ويڪر(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_جوڙيو_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_نقاب);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_بدليو_cb), عدم);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), عدم);

    gui.accel_گروهه = gtk_accel_گروهه_new();
    gtk_دري_جوڙيو_accel_گروهه(GTK_دري(gui.mainwin), gui.accel_گروهه);

    // A vertical box holds the menubar, toolbar and main text window.
#إذا GTK_CHECK_نسخو(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_سيٽ_homogeneous(GTK_BOX(vbox), FALSE);
#وإلا
    vbox = gtk_vbox_new(FALSE, 0);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GNOME
    إذا (using_gnome)
    {
# إذا defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_سيٽ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    وإلا
#جيڪڏهن_پورو
    {
	gtk_container_جوڙيو(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#جيڪڏهن_وصف_ٿيل FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_سيٽ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_سيٽ(gtk_settings, "gtk-menu-bar-accel", عدم, عدم);
    }


# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
    {
	Bonobo_Dock_شي *dockitem;

	gnome_app_سيٽ_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_شي_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_شي_سيٽ_behavior(dockitem,
		bonobo_dock_شي_get_behavior(dockitem)
				       | BONOBO_DOCK_شي_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_شروعات(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#جيڪڏهن_پورو	// FEAT_MENU

#جيڪڏهن_وصف_ٿيل FEAT_TOOLBAR
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
    gtk_widget_سيٽ_name(gui.toolbar, "vim-toolbar");
    سيٽ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    إذا (using_gnome)
    {
	Bonobo_Dock_شي *dockitem;

	gnome_app_سيٽ_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_شي_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_شي_سيٽ_behavior(dockitem,
		bonobo_dock_شي_get_behavior(dockitem)
				       | BONOBO_DOCK_شي_BEH_NEVER_FLOATING);
	gtk_container_سيٽ_border_ويڪر(GTK_CONTAINER(gui.toolbar), 0);
    }
    وإلا
# endif	// FEAT_GUI_GNOME
    {
	إذا (vim_strchr(p_go, GO_TOOLBAR) != عدم
		&& (toolbar_flags & (TOOLBAR_متن | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_شروعات(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#جيڪڏهن_پورو // FEAT_TOOLBAR

#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_شروعات(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_سيٽ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سيٽ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_سيٽ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# إذا !GTK_CHECK_نسخو(3,0,0)
    gtk_notebook_سيٽ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# إذا !GTK_CHECK_نسخو(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *صفحو, *label, *event_box;

	// Add the first tab.
# إذا GTK_CHECK_نسخو(3,2,0)
	صفحو = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_سيٽ_homogeneous(GTK_BOX(صفحو), FALSE);
# وإلا
	صفحو = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(صفحو);
	gtk_container_جوڙيو(GTK_CONTAINER(gui.tabline), صفحو);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_سيٽ_معلومات(G_OBJECT(event_box), "tab_num", GINT_منزل_اشارو(1L));
# إذا !GTK_CHECK_نسخو(3,14,0)
	gtk_misc_سيٽ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_جوڙيو(GTK_CONTAINER(event_box), label);
	gtk_notebook_سيٽ_tab_label(GTK_NOTEBOOK(gui.tabline), صفحو, event_box);
# إذا GTK_CHECK_نسخو(2,10,0)
	gtk_notebook_سيٽ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), صفحو, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), عدم);
# إذا GTK_CHECK_نسخو(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), عدم);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#جيڪڏهن_پورو // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_سيٽ_border_ويڪر(GTK_CONTAINER(gui.formwin), 0);
#إذا !GTK_CHECK_نسخو(3,0,0)
    gtk_widget_سيٽ_events(gui.formwin, GDK_EXPOSURE_نقاب);
#جيڪڏهن_پورو
#إذا GTK_CHECK_نسخو(3,22,2)
    gtk_widget_سيٽ_name(gui.formwin, "vim-gtk-form");
#جيڪڏهن_پورو

    gui.drawarea = gtk_drawing_area_new();
#إذا GTK_CHECK_نسخو(3,0,0)
    gui.surface = عدم;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    إذا (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#جيڪڏهن_پورو

    // Determine which events we will filter.
    gint event_نقاب =
	GDK_EXPOSURE_نقاب |
	GDK_ENTER_NOTIFY_نقاب |
	GDK_LEAVE_NOTIFY_نقاب |
	GDK_BUTTON_PRESS_نقاب |
	GDK_BUTTON_RELEASE_نقاب |
	GDK_ڪنجي_PRESS_نقاب |
	GDK_ڪنجي_RELEASE_نقاب |
	GDK_اشارو_MOTION_نقاب |
	GDK_اشارو_MOTION_HINT_نقاب;
#إذا GTK_CHECK_نسخو(3,4,0)
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
#جيڪڏهن_پورو
    gtk_widget_سيٽ_events(gui.drawarea, event_نقاب);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_شروعات(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(ڪنجي_press_event), عدم);
#إذا defined(FEAT_XIM) || GTK_CHECK_نسخو(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&ڪنجي_release_event), عدم);
#جيڪڏهن_پورو
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), عدم);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), عدم);
#إذا GTK_CHECK_نسخو(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), عدم);
#جيڪڏهن_پورو
#إذا GTK_CHECK_نسخو(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), عدم);
#وإلا
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_سيٽ_cb), عدم);
#جيڪڏهن_پورو

#إذا !GTK_CHECK_نسخو(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#جيڪڏهن_پورو

#إذا !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#جيڪڏهن_پورو

    إذا (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_سيٽ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_تارو.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_وٿي = gui.border_ويڪر;

#إذا GTK_CHECK_نسخو(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), عدم);
#وإلا
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), عدم);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), عدم);
#جيڪڏهن_پورو

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    إذا (vim_strchr(p_go, GO_اشارو) != عدم)
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
			 G_CALLBACK(focus_ٻاهريون_event), عدم);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_اندريون_event), عدم);
    }
    وإلا
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_ٻاهريون_event), عدم);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_اندريون_event), عدم);
#جيڪڏهن_وصف_ٿيل FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_ٻاهريون_event), عدم);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_اندريون_event), عدم);
#جيڪڏهن_پورو // FEAT_GUI_TABLINE
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
    gui.اندريون_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_بدليو_cb), عدم);
    }

#إذا defined(FEAT_IMAGE) && GTK_CHECK_نسخو(3,10,0)
    gui.scale = gtk_widget_get_scale_ضرب_ڪندڙ(gui.formwin);
#جيڪڏهن_پورو

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
	gnome_client_سيٽ_process_id(client, getpid());
}
#جيڪڏهن_پورو // USE_GNOME_SESSION

#إذا GTK_CHECK_نسخو(3,0,0)
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
سيٽ_cairo_ذريعو_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ثابت GdkRGBA rgba = color_منزل_rgba(color);
    cairo_سيٽ_ذريعو_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#جيڪڏهن_پورو // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    فراغ
gui_mch_new_colors(فراغ)
{
    إذا (gui.drawarea != عدم
#إذا GTK_CHECK_نسخو(3,22,2)
	    && gui.formwin != عدم
#جيڪڏهن_پورو
	    && gtk_widget_get_دري(gui.drawarea) != عدم)
    {
#إذا !GTK_CHECK_نسخو(3,22,2)
	Gdk_دري * ثابت da_win = gtk_widget_get_دري(gui.drawarea);
#جيڪڏهن_پورو
#إذا GTK_CHECK_نسخو(3,22,2)
	GtkStyleContext * ثابت context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar * ثابت css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.پٺ_pixel >> 16) & 0xff,
		 (gui.پٺ_pixel >> 8) & 0xff,
		 gui.پٺ_pixel & 0xff);

	gtk_css_provider_ڀريو_from_معلومات(provider, css, -1, عدم);
	gtk_style_context_جوڙيو_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#وإلاإذا GTK_CHECK_نسخو(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_منزل_rgba(gui.پٺ_pixel);
	{
	    cairo_نمونو_t * ثابت pat = cairo_نمونو_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    إذا (pat != عدم)
	    {
		gdk_دري_سيٽ_background_نمونو(da_win, pat);
		cairo_نمونو_destroy(pat);
	    }
	    وإلا
		gdk_دري_سيٽ_background_rgba(da_win, &rgba);
	}
#وإلا // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.پٺ_pixel;
	gdk_دري_سيٽ_background(da_win, &color);
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,22,2)
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
startup_resize_correction_cb(gpointer معلومات اڻ_استعمال_ٿيل)
{
    gui_سيٽ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    أرجع FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ساكن gint
form_configure_event(GtkWidget *widget اڻ_استعمال_ٿيل,
		     GdkEventConfigure *event,
		     gpointer معلومات اڻ_استعمال_ٿيل)
{
    صحيح	    usable_اوچائي = event->اوچائي;
#جيڪڏهن_وصف_ٿيل TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    صحيح	    w, h;
    gtk_دري_get_سائيز(GTK_دري(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(عدم, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (صحيح)Columns, (صحيح)صفون);
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

    إذا (!ميلاپ_ويڪر_اوچائي(latest_resize_hist, w, h)
					     && ميلاپ_stale_ويڪر_اوچائي(w, h))
	// discard stale event
	أرجع TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    إذا ((mch_csd_اوچائي == 0 || mch_csd_ويڪر == 0)
	    && latest_resize_hist != عدم
	    && !latest_resize_hist->استعمال_ٿيل
	    && latest_resize_hist->from_shellsize
	    && gui.char_اوچائي > 0)
    {
	صحيح pot_csd_w = latest_resize_hist->ويڪر - w;
	صحيح pot_csd_h = latest_resize_hist->اوچائي - h;

	إذا (pot_csd_w > 0 && pot_csd_w < gui.char_ويڪر)
	{
	    mch_csd_ويڪر = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (صحيح)Columns * gui.char_ويڪر
						    + gui_get_بنياد_ويڪر();
	}
	إذا (pot_csd_h > 0 && pot_csd_h < gui.char_اوچائي)
	{
	    mch_csd_اوچائي = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_اوچائي += mch_csd_اوچائي;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	إذا ((mch_csd_اوچائي > 0 || mch_csd_ويڪر > 0) && gtk_socket_id == 0)
	    g_idle_جوڙيو_ڀريل(GTK_ترجيح_RESIZE, startup_resize_correction_cb, عدم, عدم);
    }
    clear_resize_hists();
#جيڪڏهن_پورو

#إذا GTK_CHECK_نسخو(3,22,2) && !GTK_CHECK_نسخو(3,22,4)
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
    إذا (event->دري_2 != gtk_widget_get_دري(gui.formwin))
	أرجع TRUE;
#جيڪڏهن_پورو

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    إذا (gtk_socket_id != 0)
	usable_اوچائي -= (gui.char_اوچائي - (gui.char_اوچائي/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	صحيح use_ويڪر = event->ويڪر;

#جيڪڏهن_وصف_ٿيل TRACK_RESIZE_HISTORY
	إذا (mch_pending_form_w > event->ويڪر
		&& mch_pending_form_w - event->ويڪر < gui.char_ويڪر)
	    use_ويڪر = mch_pending_form_w;
#جيڪڏهن_پورو
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_ويڪر, usable_اوچائي);
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
mainwin_destroy_cb(GObject *object اڻ_استعمال_ٿيل, gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Don't write messages to the GUI anymore
    ڀريل_screen = FALSE;

    gui.mainwin  = عدم;
    gui.drawarea = عدم;

    إذا (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#جيڪڏهن_وصف_ٿيل USE_GRESOURCE
    gui_gtk_unregister_resource();
#جيڪڏهن_پورو
}

    فراغ
gui_gtk_get_screen_geom_جو_win(
	GtkWidget *wid,
	صحيح نقطو_x,	    // x position of window if not initialized
	صحيح نقطو_y,	    // y position of window if not initialized
	صحيح *screen_x,
	صحيح *screen_y,
	صحيح *ويڪر,
	صحيح *اوچائي)
{
    GdkRectangle geometry;
    Gdk_دري *win = gtk_widget_get_دري(wid);
#إذا GTK_CHECK_نسخو(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    إذا (wid != عدم && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    وإلا
	dpy = gdk_display_get_default();
    إذا (win != عدم)
	monitor = gdk_display_get_monitor_at_دري(dpy, win);
    وإلا
	monitor = gdk_display_get_monitor_at_نقطو(dpy, نقطو_x, نقطو_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#وإلا
    GdkScreen* screen;
    صحيح monitor;

    إذا (wid != عدم && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    وإلا
	screen = gdk_screen_get_default();
    إذا (win != عدم)
	monitor = gdk_screen_get_monitor_at_دري(screen, win);
    وإلا
	monitor = gdk_screen_get_monitor_at_نقطو(screen, نقطو_x, نقطو_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#جيڪڏهن_پورو
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *ويڪر = geometry.ويڪر;
    *اوچائي = geometry.اوچائي;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ساكن فراغ
gui_gtk_get_screen_dimensions(
	صحيح نقطو_x,
	صحيح نقطو_y,
	صحيح *screen_w,
	صحيح *screen_h)
{
    صحيح	    x, y;

    gui_gtk_get_screen_geom_جو_win(gui.mainwin, نقطو_x, نقطو_y,
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
    *screen_w -= get_menu_tool_ويڪر();
    *screen_h -= get_menu_tool_اوچائي();
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
check_startup_plug_hints(gpointer معلومات اڻ_استعمال_ٿيل)
{
    إذا (init_دري_hints_state == 1)
    {
	// Safe to use normal hints now
	init_دري_hints_state = 0;
	update_دري_manager_hints(0, 0);
	أرجع FALSE;   // stop timer
    }

    // Keep on trying
    init_دري_hints_state = 1;
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
    guint		pixel_ويڪر;
    guint		pixel_اوچائي;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    إذا (role_دليل != عدم)
    {
	gtk_دري_سيٽ_role(GTK_دري(gui.mainwin), role_دليل);
    }
    وإلا
    {
	حرف *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (بنانشان)mch_get_pid(),
			       (بنانشان)g_random_int(),
			       (بنانشان)وقت(عدم));

	gtk_دري_سيٽ_role(GTK_دري(gui.mainwin), role);
	g_free(role);
    }

    إذا (gui_win_x != -1 && gui_win_y != -1)
	gtk_دري_move(GTK_دري(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    إذا (gui.geom != عدم)
    {
	صحيح		نقاب;
	بنانشان صحيح	w, h;
	صحيح		x = 0;
	صحيح		y = 0;

	نقاب = XParseGeometry((حرف *)gui.geom, &x, &y, &w, &h);

	إذا (نقاب & ويڪر_قدر)
	    Columns = w;
	إذا (نقاب & اوچائي_قدر)
	{
	    إذا (p_دري > (ڊگهو)h - 1 || !option_was_سيٽ((char_u *)"window"))
		p_دري = h - 1;
	    صفون = h;
	}
	حد_بندي_screen_سائيز();

	pixel_ويڪر = (guint)(gui_get_بنياد_ويڪر() + Columns * gui.char_ويڪر);
	pixel_اوچائي = (guint)(gui_get_بنياد_اوچائي() + صفون * gui.char_اوچائي);

	pixel_ويڪر  += get_menu_tool_ويڪر();
	pixel_اوچائي += get_menu_tool_اوچائي();

	إذا (نقاب & (XValue | YValue))
	{
	    صحيح ww, hh;

#جيڪڏهن_وصف_ٿيل FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#وإلا
	    gui_mch_get_screen_dimensions(&ww, &hh);
#جيڪڏهن_پورو
	    hh += p_ghr + get_menu_tool_اوچائي();
	    ww += get_menu_tool_ويڪر();
	    إذا (نقاب & XNegative)
		x += ww - pixel_ويڪر;
	    إذا (نقاب & YNegative)
		y += hh - pixel_اوچائي;
	    gtk_دري_move(GTK_دري(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	إذا (gtk_socket_id != 0  &&  (نقاب & ويڪر_قدر || نقاب & اوچائي_قدر))
	{
	    update_دري_manager_hints(pixel_ويڪر, pixel_اوچائي);
	    init_دري_hints_state = 1;
	    timeout_جوڙيو(1000, check_startup_plug_hints, عدم);
	}
    }

    pixel_ويڪر = (guint)(gui_get_بنياد_ويڪر() + Columns * gui.char_ويڪر);
    pixel_اوچائي = (guint)(gui_get_بنياد_اوچائي() + صفون * gui.char_اوچائي);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    إذا (gtk_socket_id == 0)
	gtk_دري_resize(GTK_دري(gui.mainwin), pixel_ويڪر, pixel_اوچائي);
    update_دري_manager_hints(0, 0);

    إذا (foreground_دليل != عدم)
	fg_pixel = gui_get_color((char_u *)foreground_دليل);
    إذا (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    إذا (background_دليل != عدم)
	bg_pixel = gui_get_color((char_u *)background_دليل);
    إذا (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    إذا (مليو_ابتو_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_پٺ_pixel = fg_pixel;
    }
    وإلا
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_پٺ_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    سيٽ_عام_colors();

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
#جيڪڏهن_وصف_ٿيل TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#جيڪڏهن_پورو
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), عدم);
#إذا GTK_CHECK_نسخو(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ضرب_ڪندڙ_event), عدم);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_سيٽ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_معلومات_received_cb), عدم);
#جيڪڏهن_پورو

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	إذا (مليو_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	بنانشان ڊگهو menu_handler = 0;
# ifdef FEAT_TOOLBAR
	بنانشان ڊگهو tool_handler = 0;
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
#جيڪڏهن_پورو
	gtk_widget_show(gui.mainwin);

#إذا defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	إذا (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	إذا (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#جيڪڏهن_پورو
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
gui_mch_exit(صحيح rc اڻ_استعمال_ٿيل)
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
	gtk_دري_get_position(GTK_دري(gui.mainwin), x, y);
	أرجع OK;
    }
    أرجع FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    فراغ
gui_mch_سيٽ_winpos(صحيح x, صحيح y)
{
    gtk_دري_move(GTK_دري(gui.mainwin), x, y);
}

#إذا !GTK_CHECK_نسخو(3,0,0)
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
	صحيح ويڪر;
	صحيح اوچائي;

	gtk_دري_get_سائيز(GTK_دري(gui.mainwin), &ويڪر, &اوچائي);

	ويڪر  -= get_menu_tool_ويڪر();
	اوچائي -= get_menu_tool_اوچائي();

	gui_resize_shell(ويڪر, اوچائي);
    }

    resize_idle_installed = FALSE;
    أرجع FALSE; // don't call me again
}
# endif
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    صحيح
gui_mch_maximized(فراغ)
{
    أرجع (gui.mainwin != عدم && gtk_widget_get_دري(gui.mainwin) != عدم
	    && (gdk_دري_get_state(gtk_widget_get_دري(gui.mainwin))
					       & GDK_دري_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    فراغ
gui_mch_unmaximize(فراغ)
{
    إذا (gui.mainwin != عدم)
	gtk_دري_unmaximize(GTK_دري(gui.mainwin));
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

    gtk_دري_get_سائيز(GTK_دري(gui.mainwin), &w, &h);
    w -= get_menu_tool_ويڪر();
    h -= get_menu_tool_اوچائي();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    فراغ
gui_mch_سيٽ_shellsize(صحيح ويڪر, صحيح اوچائي,
		      صحيح min_ويڪر اڻ_استعمال_ٿيل,  صحيح min_اوچائي اڻ_استعمال_ٿيل,
		      صحيح بنياد_ويڪر اڻ_استعمال_ٿيل, صحيح بنياد_اوچائي اڻ_استعمال_ٿيل,
		      صحيح direction اڻ_استعمال_ٿيل)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    إذا (gtk_socket_id == 0)
	update_دري_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    ويڪر  += get_menu_tool_ويڪر();
    اوچائي += get_menu_tool_اوچائي();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    ويڪر  += mch_csd_ويڪر;
    اوچائي += mch_csd_اوچائي;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_ويڪر > 0)
			    ? ويڪر - get_menu_tool_ويڪر() - mch_csd_ويڪر : 0;

#جيڪڏهن_وصف_ٿيل TRACK_RESIZE_HISTORY
    alloc_resize_hist(ويڪر, اوچائي, TRUE);
#جيڪڏهن_پورو
    إذا (gtk_socket_id == 0)
	gtk_دري_resize(GTK_دري(gui.mainwin), ويڪر, اوچائي);
    وإلا
	update_دري_manager_hints(ويڪر, اوچائي);

#إذا !GTK_CHECK_نسخو(3,0,0)
# إذا 0
    إذا (!resize_idle_installed)
    {
	g_idle_جوڙيو_ڀريل(GDK_ترجيح_EVENTS + 10,
			&force_shell_resize_idle, عدم, عدم);
	resize_idle_installed = TRUE;
    }
# endif
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)
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
gui_mch_settitle(char_u *title, char_u *icon اڻ_استعمال_ٿيل)
{
    إذا (title != عدم && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, عدم);

    gtk_دري_سيٽ_title(GTK_دري(gui.mainwin), (ثابت حرف *)title);

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

	update_دري_manager_hints(0, 0);
    }
}
#جيڪڏهن_پورو // FEAT_MENU

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
	سيٽ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    إذا (!showit != !gtk_widget_get_visible(widget))
    {
	إذا (showit)
	    gtk_widget_show(widget);
	وإلا
	    gtk_widget_hide(widget);

	update_دري_manager_hints(0, 0);
    }
}
#جيڪڏهن_پورو // FEAT_TOOLBAR

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
    بنانشان	i;
    صحيح		is_cjk = FALSE;

    font = pango_context_ڀريو_font(gui.متن_context, font_desc);

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
#إذا PANGO_نسخو_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#وإلا
	    pango_coverage_unref(coverage);
#جيڪڏهن_پورو
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
    gui.char_اوچائي = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_اوچائي = MAX(gui.char_اوچائي, gui.char_ascent + 1);

    أرجع OK;
}

#إذا GTK_CHECK_نسخو(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ساكن gboolean
font_filter(ثابت PangoFontFamily *family,
	    ثابت PangoFontFace   *face اڻ_استعمال_ٿيل,
	    gpointer		   معلومات اڻ_استعمال_ٿيل)
{
    أرجع pango_font_family_is_monospace((PangoFontFamily *)family);
}
#جيڪڏهن_پورو

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

#إذا GTK_CHECK_نسخو(3,2,0)
    dialog = gtk_font_chooser_dialog_new(عدم, عدم);
    gtk_font_chooser_سيٽ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    عدم, عدم);
#وإلا
    dialog = gtk_font_selection_dialog_new(عدم);
#جيڪڏهن_پورو

    gtk_دري_سيٽ_transient_for(GTK_دري(dialog), GTK_دري(gui.mainwin));
    gtk_دري_سيٽ_destroy_with_والد(GTK_دري(dialog), TRUE);

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

#إذا GTK_CHECK_نسخو(3,2,0)
	gtk_font_chooser_سيٽ_font(
		GTK_FONT_CHOOSER(dialog), (ثابت gchar *)oldname);
#وإلا
	gtk_font_selection_dialog_سيٽ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ثابت حرف *)oldname);
#جيڪڏهن_پورو

	إذا (oldname != oldval)
	    vim_free(oldname);
    }
    وإلا
#إذا GTK_CHECK_نسخو(3,2,0)
	gtk_font_chooser_سيٽ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#وإلا
	gtk_font_selection_dialog_سيٽ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#جيڪڏهن_پورو

    response = gtk_dialog_لاڳيتو(GTK_DIALOG(dialog));

    إذا (response == GTK_RESPONSE_OK)
    {
	حرف *name;

#إذا GTK_CHECK_نسخو(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#وإلا
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#جيڪڏهن_پورو
	إذا (name != عدم)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    إذا (p != عدم && داخلا_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&داخلا_conv, p, عدم);
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

    plain_font = pango_context_ڀريو_font(gui.متن_context, gui.norm_font);

    إذا (plain_font == عدم)
	أرجع;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_سيٽ_وزن(bold_font_desc, PANGO_وزن_BOLD);

    bold_font = pango_context_ڀريو_font(gui.متن_context, bold_font_desc);
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
    GList	    *شي_list;
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
    شي_list = pango_itemize(gui.متن_context, (ثابت حرف *)ascii_chars,
			      0, حجم(ascii_chars), attr_list, عدم);

    إذا (شي_list != عدم && شي_list->ايندڙ == عدم) // play safe
    {
	Pango_شي   *شي;
	صحيح	    ويڪر;

	شي  = (Pango_شي *)شي_list->معلومات;
	ويڪر = gui.char_ويڪر * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = شي->analysis.shape_engine;

	gui.ascii_font = شي->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت حرف *)ascii_chars, حجم(ascii_chars),
		    &شي->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->انگ_glyphs == حجم(ascii_chars));

	لكل (i = 0; i < gui.ascii_glyphs->انگ_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_وٿي += MAX(0, ويڪر - geom->ويڪر) / 2;
	    geom->ويڪر = ويڪر;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(شي_list, (GFunc)(فراغ *)&pango_شي_free, عدم);
    g_list_free(شي_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحيح
gui_mch_init_font(char_u *font_name, صحيح fontset اڻ_استعمال_ٿيل)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحيح			    ويڪر;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    إذا (font_name == عدم)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    إذا (font_desc == عدم)
	أرجع FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_سيٽ_font_description(gui.متن_context, font_desc);

    layout = pango_layout_new(gui.متن_context);
    pango_layout_سيٽ_متن(layout, "MW", 2);
    pango_layout_get_سائيز(layout, &ويڪر, عدم);
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
	صحيح cjk_ويڪر;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_سيٽ_متن(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_سائيز(layout, &cjk_ويڪر, عدم);

	إذا (ويڪر == cjk_ويڪر)  // Xft not patched
	    ويڪر /= 2;
    }
    g_object_unref(layout);

    gui.char_ويڪر = (ويڪر / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    إذا (gui.char_ويڪر <= 0)
	gui.char_ويڪر = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_سيٽ_font_name(font_name);

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
	update_دري_manager_hints(0, 0);
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
    إذا (!gui.اندريون_use || name == عدم)
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
	إذا (pango_font_description_get_سائيز(font) <= 0)
	    pango_font_description_سيٽ_سائيز(font, 10 * PANGO_SCALE);

	real_font = pango_context_ڀريو_font(gui.متن_context, font);

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
gui_mch_get_fontname(GuiFont font, char_u *name اڻ_استعمال_ٿيل)
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
#جيڪڏهن_پورو

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
    optexpand_T	*دليلون,
    فراغ	*param,
    صحيح		(*جوڙيو_ميلاپ)(char_u *val))
{
    PangoFontFamily	**font_families = عدم;
    صحيح			n_families = 0;
    صحيح			wide = *(صحيح *)param;

    إذا (دليلون->oe_include_orig_val && *دليلون->oe_opt_قدر == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	إذا (جوڙيو_ميلاپ((char_u *)DEFAULT_FONT) != OK)
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
	إذا (داخلا_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&داخلا_conv, (char_u*)fam_name, عدم);
	    إذا (buf != عدم)
	    {
		إذا (جوڙيو_ميلاپ(buf) != OK)
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
	    إذا (جوڙيو_ميلاپ((char_u *)fam_name) != OK)
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

    إذا (!gui.اندريون_use)		// can't do this when GUI not running
	أرجع color;

    إذا (name != عدم)
	color = gui_get_color_cmn(name);

#إذا GTK_CHECK_نسخو(3,0,0)
    أرجع color;
#وإلا
    إذا (color == INVALCOLOR)
	أرجع INVALCOLOR;

    أرجع gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#جيڪڏهن_پورو
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(صحيح r, صحيح g, صحيح b)
{
#إذا GTK_CHECK_نسخو(3,0,0)
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
#جيڪڏهن_پورو
}

/*
 * Set the current text foreground color.
 */
    فراغ
gui_mch_سيٽ_fg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    *gui.fgcolor = color_منزل_rgba(color);
#وإلا
    gui.fgcolor->pixel = (بنانشان ڊگهو)color;
#جيڪڏهن_پورو
}

/*
 * Set the current text background color.
 */
    فراغ
gui_mch_سيٽ_bg_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    *gui.bgcolor = color_منزل_rgba(color);
#وإلا
    gui.bgcolor->pixel = (بنانشان ڊگهو)color;
#جيڪڏهن_پورو
}

/*
 * Set the current text special color.
 */
    فراغ
gui_mch_سيٽ_sp_color(guicolor_T color)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    *gui.spcolor = color_منزل_rgba(color);
#وإلا
    gui.spcolor->pixel = (بنانشان ڊگهو)color;
#جيڪڏهن_پورو
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#وصف_ڪريو وجهو_PANGO_ATTR(Attribute, AttrList, شروعات_2, پڇاڙي_2)  \
    G_STMT_شروعات{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->شروعات_اشاريو = (شروعات_2);		    \
	tmp_attr_->پڇاڙي_اشاريو = (پڇاڙي_2);			    \
	pango_attr_list_وجهو((AttrList), tmp_attr_);	    \
    }G_STMT_پڇاڙي

    ساكن فراغ
apply_wide_font_attr(char_u *s, صحيح len, PangoAttrList *attr_list)
{
    char_u  *شروعات = عدم;
    char_u  *p;
    صحيح	    uc;

    لكل (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	إذا (شروعات == عدم)
	{
	    إذا (uc >= 0x80 && utf_char2cells(uc) == 2)
		شروعات = p;
	}
	وإلا إذا (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    وجهو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, شروعات - s, p - s);
	    شروعات = عدم;
	}
    }

    إذا (شروعات != عدم)
	وجهو_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, شروعات - s, len);
}

    ساكن صحيح
ڳڻپ_cluster_cells(char_u *s, Pango_شي *شي,
		    PangoGlyphString* glyphs, صحيح i,
		    صحيح *cluster_ويڪر,
		    صحيح *آخري_glyph_rbearing)
{
    char_u  *p;
    صحيح	    ايندڙ;	// glyph start index of next cluster
    صحيح	    شروعات, پڇاڙي; // string segment of current cluster
    صحيح	    ويڪر;	// real cluster width in Pango units
    صحيح	    uc;
    صحيح	    cellcount = 0;

    ويڪر = glyphs->glyphs[i].geometry.ويڪر;

    لكل (ايندڙ = i + 1; ايندڙ < glyphs->انگ_glyphs; ++ايندڙ)
    {
	إذا (glyphs->glyphs[ايندڙ].attr.is_cluster_شروعات)
	    اكسر;
	وإلا إذا (glyphs->glyphs[ايندڙ].geometry.ويڪر > ويڪر)
	    ويڪر = glyphs->glyphs[ايندڙ].geometry.ويڪر;
    }

    شروعات = شي->وٿي + glyphs->log_clusters[i];
    پڇاڙي   = شي->وٿي + ((ايندڙ < glyphs->انگ_glyphs) ?
			    glyphs->log_clusters[ايندڙ] : شي->ڊيگهائي);

    لكل (p = s + شروعات; p < s + پڇاڙي; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	إذا (uc < 0x80)
	    ++cellcount;
	وإلا إذا (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    إذا (آخري_glyph_rbearing != عدم
	    && cellcount > 0 && ايندڙ == glyphs->انگ_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(شي->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, عدم);

	إذا (PANGO_RBEARING(ink_rect) > 0)
	    *آخري_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    إذا (cellcount > 0)
	*cluster_ويڪر = ويڪر;

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
تياري_zero_ويڪر_cluster(Pango_شي *شي, PangoGlyphInfo *glyph,
			 صحيح آخري_cellcount, صحيح آخري_cluster_ويڪر,
			 صحيح آخري_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحيح		    ويڪر;

    ويڪر = آخري_cellcount * gui.char_ويڪر * PANGO_SCALE;
    glyph->geometry.x_وٿي = -ويڪر + MAX(0, ويڪر - آخري_cluster_ويڪر) / 2;
    glyph->geometry.ويڪر = 0;

    pango_font_get_glyph_extents(شي->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    إذا (ink_rect.x < 0)
    {
	glyph->geometry.x_وٿي += آخري_glyph_rbearing;
	glyph->geometry.y_وٿي  = logical_rect.اوچائي
		- (gui.char_اوچائي - p_linespace) * PANGO_SCALE;
    }
    وإلا
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_وٿي = -ويڪر + MAX(0, ويڪر - ink_rect.ويڪر) / 2;
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
draw_glyph_string(صحيح صف, صحيح کانو, صحيح انگ_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#وإلا
    ساكن فراغ
draw_glyph_string(صحيح صف, صحيح کانو, صحيح انگ_cells, صحيح flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#جيڪڏهن_پورو
{
    إذا (!(flags & DRAW_TRANSP))
    {
#إذا GTK_CHECK_نسخو(3,0,0)
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(کانو), FILL_Y(صف),
			انگ_cells * gui.char_ويڪر, gui.char_اوچائي);
	cairo_fill(cr);
#وإلا
	gdk_gc_سيٽ_foreground(gui.متن_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->دري_2,
			   gui.متن_gc,
			   TRUE,
			   FILL_X(کانو),
			   FILL_Y(صف),
			   انگ_cells * gui.char_ويڪر,
			   gui.char_اوچائي);
#جيڪڏهن_پورو
    }

#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_سيٽ_ذريعو_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_منزل(cr, متن_X(کانو), متن_Y(صف));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#وإلا
    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->دري_2,
		    gui.متن_gc,
		    font,
		    متن_X(کانو),
		    متن_Y(صف),
		    glyphs);
#جيڪڏهن_پورو

    // redraw the contents with an offset of 1 to emulate bold
    إذا ((flags & DRAW_BOLD) && !gui.font_can_bold)
#إذا GTK_CHECK_نسخو(3,0,0)
    {
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, متن_X(کانو) + 1, متن_Y(صف));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#وإلا
	gdk_draw_glyphs(gui.drawarea->دري_2,
			gui.متن_gc,
			font,
			متن_X(کانو) + 1,
			متن_Y(صف),
			glyphs);
#جيڪڏهن_پورو
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
draw_under(صحيح flags, صحيح صف, صحيح کانو, صحيح cells, cairo_t *cr)
#وإلا
    ساكن فراغ
draw_under(صحيح flags, صحيح صف, صحيح کانو, صحيح cells)
#جيڪڏهن_پورو
{
    صحيح			i;
    صحيح			وٿي;
    ساكن ثابت صحيح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    صحيح			y = FILL_Y(صف + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    إذا (flags & DRAW_UNDERC)
    {
#إذا GTK_CHECK_نسخو(3,0,0)
	cairo_سيٽ_سٽ_ويڪر(cr, 1.0);
	cairo_سيٽ_سٽ_cap(cr, CAIRO_سٽ_CAP_BUTT);
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(کانو) + 1, y - 2 + 0.5);
	لكل (i = FILL_X(کانو) + 1; i < FILL_X(کانو + cells); ++i)
	{
	    وٿي = val[i % 8];
	    cairo_سٽ_منزل(cr, i, y - وٿي + 0.5);
	}
	cairo_stroke(cr);
#وإلا
	gdk_gc_سيٽ_foreground(gui.متن_gc, gui.spcolor);
	لكل (i = FILL_X(کانو); i < FILL_X(کانو + cells); ++i)
	{
	    وٿي = val[i % 8];
	    gdk_draw_نقطو(gui.drawarea->دري_2, gui.متن_gc, i, y - وٿي);
	}
	gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
#جيڪڏهن_پورو
    }

    // Draw a strikethrough line
    إذا (flags & DRAW_STRIKE)
    {
#إذا GTK_CHECK_نسخو(3,0,0)
	cairo_سيٽ_سٽ_ويڪر(cr, 1.0);
	cairo_سيٽ_سٽ_cap(cr, CAIRO_سٽ_CAP_BUTT);
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_منزل(cr, FILL_X(کانو), y + 1 - gui.char_اوچائي/2 + 0.5);
	cairo_سٽ_منزل(cr, FILL_X(کانو + cells), y + 1 - gui.char_اوچائي/2 + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_gc_سيٽ_foreground(gui.متن_gc, gui.spcolor);
	gdk_draw_سٽ(gui.drawarea->دري_2, gui.متن_gc,
		      FILL_X(کانو), y + 1 - gui.char_اوچائي/2,
		      FILL_X(کانو + cells), y + 1 - gui.char_اوچائي/2);
	gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
#جيڪڏهن_پورو
    }

    // Underline: draw a line at the bottom of the character cell.
    إذا (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	إذا (p_linespace > 1)
	    y -= p_linespace - 1;
#إذا GTK_CHECK_نسخو(3,0,0)
	cairo_سيٽ_سٽ_ويڪر(cr, 1.0);
	cairo_سيٽ_سٽ_cap(cr, CAIRO_سٽ_CAP_BUTT);
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_منزل(cr, FILL_X(کانو), y + 0.5);
	cairo_سٽ_منزل(cr, FILL_X(کانو + cells), y + 0.5);
	cairo_stroke(cr);
#وإلا
	gdk_draw_سٽ(gui.drawarea->دري_2, gui.متن_gc,
		      FILL_X(کانو), y,
		      FILL_X(کانو + cells) - 1, y);
#جيڪڏهن_پورو
    }
}

    صحيح
gui_gtk_draw_string(صحيح صف, صحيح کانو, char_u *s, صحيح len, صحيح flags)
{
    char_u	*conv_buf = عدم;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    صحيح		convlen;
    char_u	*sp, *bp;
    صحيح		plen;
    صحيح		len_جوڙ;	// return value needs to add up since we are
				// printing substrings
    صحيح		byte_جوڙ;	// byte position in string
    char_u	*cs;		// current *s pointer
    صحيح		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    صحيح		should_need_pango = FALSE;
    صحيح		slen;
    صحيح		is_ligature;
    صحيح		is_utf8;
    char_u	backup_ch;

    إذا (gui.متن_context == عدم || gtk_widget_get_دري(gui.drawarea) == عدم)
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
    len_جوڙ = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_جوڙ = 0;
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
	طالما (slen < (len - byte_جوڙ))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    إذا (is_ligature && !needs_pango)
	    {
		إذا ((slen + 1) < (len - byte_جوڙ))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		وإلا
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    إذا ((!is_utf8) && ((slen + 1) < (len - byte_جوڙ)))
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
			طالما ((slen < (len - byte_جوڙ))
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
	len_جوڙ += gui_gtk_draw_string_ext(صف, کانو + len_جوڙ, cs, slen, flags,
					    needs_pango);
	إذا (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_جوڙ += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    أرجع len_جوڙ;
}

#إذا GTK_CHECK_نسخو(3,0,0)
ساكن cairo_region_t *dirty_region = عدم;

    ساكن فراغ
قطار_draw_area(صحيح x, صحيح y, صحيح ويڪر, صحيح اوچائي)
{
    cairo_rectangle_int_t rect;

    إذا (ويڪر <= 0 || اوچائي <= 0 || gui.drawarea == عدم)
	أرجع;

    rect.x = x;
    rect.y = y;
    rect.ويڪر = ويڪر;
    rect.اوچائي = اوچائي;

    إذا (dirty_region == عدم)
	dirty_region = cairo_region_create_rectangle(&rect);
    وإلا
	cairo_region_union_rectangle(dirty_region, &rect);
}
#جيڪڏهن_پورو

    صحيح
gui_gtk_draw_string_ext(
	صحيح	صف,
	صحيح	کانو,
	char_u	*s,
	صحيح	len,
	صحيح	flags,
	صحيح	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    صحيح			ڪالم_وٿي = 0;  // column offset in cells
    صحيح			i;
#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_t		*cr;
#جيڪڏهن_پورو

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_وٿي;
    area.y = FILL_Y(صف);
    area.ويڪر	= gui.انگ_کانن_ڳڻپ * gui.char_ويڪر;
    area.اوچائي = gui.char_اوچائي;

#إذا GTK_CHECK_نسخو(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.ويڪر, area.اوچائي);
    cairo_clip(cr);
#وإلا
    gdk_gc_سيٽ_clip_origin(gui.متن_gc, 0, 0);
    gdk_gc_سيٽ_clip_rectangle(gui.متن_gc, &area);
#جيڪڏهن_پورو

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

	pango_glyph_string_سيٽ_سائيز(glyphs, len);

	لكل (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#إذا GTK_CHECK_نسخو(3,0,0)
	draw_glyph_string(صف, کانو, len, flags, gui.ascii_font, glyphs, cr);
#وإلا
	draw_glyph_string(صف, کانو, len, flags, gui.ascii_font, glyphs);
#جيڪڏهن_پورو

	ڪالم_وٿي = len;
    }
    وإلا
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*شي_list;
	صحيح		cluster_ويڪر;
	صحيح		آخري_glyph_rbearing;
	صحيح		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	إذا (!utf_valid_string(s, s + len))
	{
	    ڪالم_وٿي = len;
	    اذهب skipitall;
	}

	// original width of the current cluster
	cluster_ويڪر = PANGO_SCALE * gui.char_ويڪر;

	// right bearing of the last non-composing glyph
	آخري_glyph_rbearing = PANGO_SCALE * gui.char_ويڪر;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	إذا (gui.wide_font != عدم)
	    apply_wide_font_attr(s, len, attr_list);

	إذا ((flags & DRAW_BOLD) && gui.font_can_bold)
	    وجهو_PANGO_ATTR(pango_attr_وزن_new(PANGO_وزن_BOLD),
			      attr_list, 0, len);
	إذا (flags & DRAW_ITALIC)
	    وجهو_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	شي_list = pango_itemize(gui.متن_context,
				  (ثابت حرف *)s, 0, len, attr_list, عدم);

	طالما (شي_list != عدم)
	{
	    Pango_شي	*شي;
	    صحيح		شي_cells = 0; // item length in cells

	    شي = (Pango_شي *)شي_list->معلومات;
	    شي_list = g_list_delete_link(شي_list, شي_list);
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
	    شي->analysis.level = (شي->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    شي->analysis.shape_engine = default_shape_engine;

#جيڪڏهن_وصف_ٿيل HAVE_PANGO_SHAPE_ڀريل
	    pango_shape_ڀريل((ثابت حرف *)s + شي->وٿي, شي->ڊيگهائي,
		    (ثابت حرف *)s, len, &شي->analysis, glyphs);
#وإلا
	    pango_shape((ثابت حرف *)s + شي->وٿي, شي->ڊيگهائي,
			&شي->analysis, glyphs);
#جيڪڏهن_پورو
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    لكل (i = 0; i < glyphs->انگ_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		إذا (glyph->attr.is_cluster_شروعات)
		{
		    صحيح cellcount;

		    cellcount = ڳڻپ_cluster_cells(
			s, شي, glyphs, i, &cluster_ويڪر,
			(شي_list != عدم) ? &آخري_glyph_rbearing : عدم);

		    إذا (cellcount > 0)
		    {
			صحيح ويڪر;

			ويڪر = cellcount * gui.char_ويڪر * PANGO_SCALE;
			glyph->geometry.x_وٿي +=
					    MAX(0, ويڪر - cluster_ويڪر) / 2;
			glyph->geometry.ويڪر = ويڪر;
		    }
		    وإلا
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			تياري_zero_ويڪر_cluster(شي, glyph, cells,
						 cluster_ويڪر,
						 آخري_glyph_rbearing);
		    }

		    شي_cells += cellcount;
		    cells = cellcount;
		}
		وإلا إذا (i > 0)
		{
		    صحيح ويڪر;

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
		    إذا (glyph->geometry.x_وٿي >= 0)
		    {
			glyphs->glyphs[i].geometry.ويڪر =
					 glyphs->glyphs[i - 1].geometry.ويڪر;
			glyphs->glyphs[i - 1].geometry.ويڪر = 0;
		    }
		    ويڪر = cells * gui.char_ويڪر * PANGO_SCALE;
		    glyph->geometry.x_وٿي +=
					    MAX(0, ويڪر - cluster_ويڪر) / 2;
		}
		وإلا // i == 0 "cannot happen"
		{
		    glyph->geometry.ويڪر = 0;
		}
	    }

	    //// Aaaaand action! **
#إذا GTK_CHECK_نسخو(3,0,0)
	    draw_glyph_string(صف, کانو + ڪالم_وٿي, شي_cells,
			      flags, شي->analysis.font, glyphs,
			      cr);
#وإلا
	    draw_glyph_string(صف, کانو + ڪالم_وٿي, شي_cells,
			      flags, شي->analysis.font, glyphs);
#جيڪڏهن_پورو

	    pango_شي_free(شي);

	    ڪالم_وٿي += شي_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#إذا GTK_CHECK_نسخو(3,0,0)
    draw_under(flags, صف, کانو, ڪالم_وٿي, cr);
#وإلا
    draw_under(flags, صف, کانو, ڪالم_وٿي);
#جيڪڏهن_پورو

    pango_glyph_string_free(glyphs);

#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_destroy(cr);
    قطار_draw_area(FILL_X(کانو), FILL_Y(صف),
	    ڪالم_وٿي * gui.char_ويڪر + 1, gui.char_اوچائي);
#وإلا
    gdk_gc_سيٽ_clip_rectangle(gui.متن_gc, عدم);
#جيڪڏهن_پورو

    أرجع ڪالم_وٿي;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحيح
gui_mch_haskey(char_u *name)
{
    صحيح i;

    لكل (i = 0; special_ڪنجيون[i].ڪنجي_sym != 0; i++)
	إذا (name[0] == special_ڪنجيون[i].code0
		&& name[1] == special_ڪنجيون[i].code1)
	    أرجع OK;
    أرجع FAIL;
}

#إذا defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحيح
gui_get_x11_windis(دري *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
    {
	*dis = dpy;
	*win = GDK_دري_XID(gtk_widget_get_دري(gui.mainwin));
	أرجع OK;
    }

    *dis = عدم;
    *win = 0;
    أرجع FAIL;
}
#جيڪڏهن_پورو

    Display *
gui_mch_get_display(فراغ)
{
    إذا (gui.mainwin != عدم && gtk_widget_get_دري(gui.mainwin) != عدم
#إذا GTK_CHECK_نسخو(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#جيڪڏهن_پورو
	)
	أرجع GDK_دري_XDISPLAY(gtk_widget_get_دري(gui.mainwin));
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
#إذا GTK_CHECK_نسخو(3,0,0)
    // TODO Replace GdkGC with Cairo
    (فراغ)msec;
#وإلا
    GdkGCValues	قدرون;
    GdkGC	*invert_gc;

    إذا (gui.drawarea->دري_2 == عدم)
	أرجع;

    قدرون.foreground.pixel = gui.norm_pixel ^ gui.پٺ_pixel;
    قدرون.background.pixel = gui.norm_pixel ^ gui.پٺ_pixel;
    قدرون.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_قدرون(gui.drawarea->دري_2,
				       &قدرون,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_سيٽ_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->دري_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحيح)Columns) + gui.border_وٿي,
		       FILL_Y((صحيح)صفون) + gui.border_وٿي);

    gui_mch_flush();
    ui_delay((ڊگهو)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->دري_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحيح)Columns) + gui.border_وٿي,
		       FILL_Y((صحيح)صفون) + gui.border_وٿي);

    gdk_gc_destroy(invert_gc);
#جيڪڏهن_پورو
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    فراغ
gui_mch_invert_rectangle(صحيح r, صحيح c, صحيح nr, صحيح nc)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    ثابت GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_ويڪر, nr * gui.char_اوچائي
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);

    cairo_سيٽ_ذريعو_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# إذا CAIRO_نسخو >= CAIRO_نسخو_ENCODE(1,9,2)
    cairo_سيٽ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# وإلا
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ويڪر, rect.اوچائي);
    cairo_fill(cr);

    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.ويڪر, rect.اوچائي);
#وإلا
    GdkGCValues قدرون;
    GdkGC *invert_gc;

    إذا (gui.drawarea->دري_2 == عدم)
	أرجع;

    قدرون.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_قدرون(gui.drawarea->دري_2,
				       &قدرون,
				       GDK_GC_FUNCTION);
    gdk_gc_سيٽ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->دري_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_ويڪر, (nr) * gui.char_اوچائي);
    gdk_gc_destroy(invert_gc);
#جيڪڏهن_پورو
}

/*
 * Iconify the GUI window.
 */
    فراغ
gui_mch_iconify(فراغ)
{
    gtk_دري_iconify(GTK_دري(gui.mainwin));
}

#إذا defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    فراغ
gui_mch_سيٽ_foreground(فراغ)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_دري_سيٽ_keep_above(GTK_دري(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_دري_present(GTK_دري(gui.mainwin));
    gui_may_flush();
    gtk_دري_سيٽ_keep_above(GTK_دري(gui.mainwin), FALSE);
    gui_may_flush();
}
#جيڪڏهن_پورو

/*
 * Draw a cursor without focus.
 */
    فراغ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحيح		i = 1;
#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_t    *cr;
#جيڪڏهن_پورو

    إذا (gtk_widget_get_دري(gui.drawarea) == عدم)
	أرجع;

#إذا GTK_CHECK_نسخو(3,0,0)
    cr = cairo_create(gui.surface);
#جيڪڏهن_پورو

    gui_mch_سيٽ_fg_color(color);

#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_سيٽ_ذريعو_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#وإلا
    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
#جيڪڏهن_پورو
    إذا (mb_lefthalve(gui.صف, gui.کانو))
	i = 2;
#إذا GTK_CHECK_نسخو(3,0,0)
    cairo_سيٽ_سٽ_ويڪر(cr, 1.0);
    cairo_سيٽ_سٽ_cap(cr, CAIRO_سٽ_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.کانو) + 0.5, FILL_Y(gui.صف) + 0.5,
	    i * gui.char_ويڪر - 1, gui.char_اوچائي - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#وإلا
    gdk_draw_rectangle(gui.drawarea->دري_2, gui.متن_gc,
	    FALSE,
	    FILL_X(gui.کانو), FILL_Y(gui.صف),
	    i * gui.char_ويڪر - 1, gui.char_اوچائي - 1);
#جيڪڏهن_پورو
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    فراغ
gui_mch_draw_part_cursor(صحيح w, صحيح h, guicolor_T color)
{
    إذا (gtk_widget_get_دري(gui.drawarea) == عدم)
	أرجع;

    gui_mch_سيٽ_fg_color(color);

#إذا GTK_CHECK_نسخو(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_سيٽ_ذريعو_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ساڄو ? FILL_X(gui.کانو + 1) - w :
# endif
	    FILL_X(gui.کانو), FILL_Y(gui.صف) + gui.char_اوچائي - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->دري_2, gui.متن_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ساڄو ? FILL_X(gui.کانو + 1) - w :
# endif
	    FILL_X(gui.کانو),
	    FILL_Y(gui.صف) + gui.char_اوچائي - h,
	    w, h);
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)
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
    طالما (g_main_context_pending(عدم) && !vim_is_داخلا_buf_ڀريل()
	    && ++cnt < 100)
    {
#إذا GTK_CHECK_نسخو(3,0,0)
	صحيح prio = 0;
	g_main_context_prepare(عدم, &prio);
	// peek internal scheduling of redraw
	إذا (prio == GDK_ترجيح_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#جيڪڏهن_پورو
	g_main_context_iteration(عدم, TRUE);
    }
}

    ساكن timeout_cb_type
داخلا_timer_cb(gpointer معلومات)
{
    صحيح *timed_ٻاهريون = (صحيح *) معلومات;

    // Just inform the caller about the occurrence of it
    *timed_ٻاهريون = TRUE;

    أرجع FALSE;		// don't happen again
}

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    ساكن timeout_cb_type
channel_poll_cb(gpointer معلومات اڻ_استعمال_ٿيل)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    أرجع TRUE;		// repeat
}
#جيڪڏهن_پورو

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
gui_mch_wait_for_chars(ڊگهو wtime)
{
    صحيح		focus;
    guint	timer;
    ساكن صحيح	timed_ٻاهريون;
    صحيح		retval = FAIL;
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    guint	channel_timer = 0;
#جيڪڏهن_پورو

    timed_ٻاهريون = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    إذا (wtime >= 0)
	timer = timeout_جوڙيو(wtime == 0 ? 1L : wtime,
						   داخلا_timer_cb, &timed_ٻاهريون);
    وإلا
	timer = 0;

#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    إذا (channel_any_keep_open())
	channel_timer = timeout_جوڙيو(20, channel_poll_cb, عدم);
#جيڪڏهن_پورو

    focus = gui.اندريون_focus;

    افعل
    {
	// Stop or start blinking when focus changes
	إذا (gui.اندريون_focus != focus)
	{
	    إذا (gui.اندريون_focus)
		gui_mch_شروعات_blink();
	    وإلا
		gui_mch_stop_blink(TRUE);
	    focus = gui.اندريون_focus;
	}

#جيڪڏهن_وصف_ٿيل پيغام_قطار
# ifdef FEAT_TIMERS
	did_جوڙيو_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	إذا (did_جوڙيو_timer)
	    // Need to recompute the waiting time.
	    اذهب theend;
# endif
#جيڪڏهن_پورو

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	إذا (!داخلا_موجود())
	{
	    g_main_context_iteration(عدم, TRUE);
#جيڪڏهن_وصف_ٿيل GDK_WINDOWING_WAYLAND
    إذا (gui.is_wayland && clip_تارو.state == SELECT_اندريون_ترقي)
	gui_may_flush();
#جيڪڏهن_پورو
	}

	// Got char, return immediately
	إذا (داخلا_موجود())
	{
	    retval = OK;
	    اذهب theend;
	}
    } طالما (wtime < 0 || !timed_ٻاهريون);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    إذا (timer != 0 && !timed_ٻاهريون)
	timeout_remove(timer);
#جيڪڏهن_وصف_ٿيل FEAT_ڪم_CHANNEL
    إذا (channel_timer != 0)
	timeout_remove(channel_timer);
#جيڪڏهن_پورو

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
#إذا GTK_CHECK_نسخو(3,0,0)
    إذا (dirty_region != عدم && gui.drawarea != عدم)
    {
	gtk_widget_قطار_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = عدم;
    }
#وإلا
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       أرجع;
#جيڪڏهن_پورو
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    فراغ
gui_mch_clear_بلاڪ(صحيح row1arg, صحيح col1arg, صحيح row2arg, صحيح col2arg)
{

    صحيح col1 = check_کانو(col1arg);
    صحيح col2 = check_کانو(col2arg);
    صحيح row1 = check_صف(row1arg);
    صحيح row2 = check_صف(row2arg);

#إذا GTK_CHECK_نسخو(3,0,0)
    إذا (gtk_widget_get_دري(gui.drawarea) == عدم)
	أرجع;
#وإلا
    GdkColor color;

    إذا (gui.drawarea->دري_2 == عدم)
	أرجع;

    color.pixel = gui.پٺ_pixel;
#جيڪڏهن_پورو

#إذا GTK_CHECK_نسخو(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ثابت GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_ويڪر + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_اوچائي
	};
	cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخو(3,22,2)
	سيٽ_cairo_ذريعو_rgba_from_color(cr, gui.پٺ_pixel);
# وإلا
	Gdk_دري * ثابت win = gtk_widget_get_دري(gui.drawarea);
	cairo_نمونو_t * ثابت pat = gdk_دري_get_background_نمونو(win);
	إذا (pat != عدم)
	    cairo_سيٽ_ذريعو(cr, pat);
	وإلا
	    سيٽ_cairo_ذريعو_rgba_from_color(cr, gui.پٺ_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.ويڪر, rect.اوچائي);
	cairo_fill(cr);
	cairo_destroy(cr);

	قطار_draw_area(rect.x, rect.y, rect.ويڪر, rect.اوچائي);
    }
#وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سيٽ_foreground(gui.متن_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->دري_2, gui.متن_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_ويڪر
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_اوچائي);
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)
}

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
gui_gtk_دري_clear(Gdk_دري *win)
{
    ثابت GdkRectangle rect = {
	0, 0, gdk_دري_get_ويڪر(win), gdk_دري_get_اوچائي(win)
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);
# إذا GTK_CHECK_نسخو(3,22,2)
    سيٽ_cairo_ذريعو_rgba_from_color(cr, gui.پٺ_pixel);
# وإلا
    cairo_نمونو_t * ثابت pat = gdk_دري_get_background_نمونو(win);
    إذا (pat != عدم)
	cairo_سيٽ_ذريعو(cr, pat);
    وإلا
	سيٽ_cairo_ذريعو_rgba_from_color(cr, gui.پٺ_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ويڪر, rect.اوچائي);
    cairo_fill(cr);
    cairo_destroy(cr);

    قطار_draw_area(rect.x, rect.y, rect.ويڪر, rect.اوچائي);
}
#وإلا
# define gui_gtk_دري_clear(win)  gdk_دري_clear(win)
#جيڪڏهن_پورو

    فراغ
gui_mch_clear_all(فراغ)
{
    إذا (gtk_widget_get_دري(gui.drawarea) != عدم)
	gui_gtk_دري_clear(gtk_widget_get_دري(gui.drawarea));
}

#إذا !GTK_CHECK_نسخو(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ساكن فراغ
check_copy_area(فراغ)
{
    GdkEvent        *event;
    صحيح                expose_ڳڻپ;
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
	event = gdk_event_get_graphics_expose(gui.drawarea->دري_2);
	إذا (event == عدم)
	    اكسر; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.ويڪر, event->expose.area.اوچائي);
	expose_ڳڻپ = event->expose.ڳڻپ;
	gdk_event_free(event);
    }
    طالما (expose_ڳڻپ > 0); // more events follow
    gui_can_update_cursor();
}
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)

#إذا GTK_CHECK_نسخو(3,0,0)
    ساكن فراغ
gui_gtk_surface_copy_rect(صحيح dest_x, صحيح dest_y,
			  صحيح src_x,  صحيح src_y,
			  صحيح ويڪر,  صحيح اوچائي)
{
    cairo_t * ثابت cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ساكن cairo_surface_t *scroll_scratch = عدم;
    ساكن صحيح scratch_w = 0, scratch_h = 0;
    صحيح آخري_صف = صفون - 1, آخري_صف_y = آخري_صف * gui.char_اوچائي;
    bool آخري_صف_overlap = (dest_y + اوچائي) > آخري_صف_y;
    إذا (gui.is_wayland && ( !(State & گهڻائي_CMDLINE) || !آخري_صف_overlap) )
    {
	إذا (dest_y < src_y) // scroll up
	{
	    cairo_سيٽ_operator(cr, CAIRO_OPERATOR_ذريعو);
	    cairo_سيٽ_ذريعو_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, ويڪر, اوچائي);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	وإلا
	{   //  reuse surface when scrolling, only realloc if larger
	    إذا (scroll_scratch == عدم || ويڪر > scratch_w || اوچائي > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), ويڪر, اوچائي);
		scratch_w = ويڪر, scratch_h = اوچائي;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_سيٽ_ذريعو_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_سيٽ_operator(cr, CAIRO_OPERATOR_ذريعو);
	    cairo_rectangle(cr, dest_x, dest_y, ويڪر, اوچائي);
	    cairo_clip(cr);
	    cairo_سيٽ_ذريعو_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    وإلا
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, ويڪر, اوچائي);
	cairo_clip(cr);
	cairo_ڌڪو_گروهه(cr);
	cairo_سيٽ_ذريعو_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_ڪڍو_گروهه_منزل_ذريعو(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#جيڪڏهن_پورو

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    فراغ
gui_mch_delete_سٽون(صحيح صف, صحيح انگ_سٽون)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_ساڄو - gui.scroll_region_کاٻو + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحيح src_nrows = nrows - انگ_سٽون;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف + انگ_سٽون),
	    gui.char_ويڪر * ncols + 1,     gui.char_اوچائي * src_nrows);
    gui_clear_بلاڪ(
	    gui.scroll_region_bot - انگ_سٽون + 1, gui.scroll_region_کاٻو,
	    gui.scroll_region_bot,		   gui.scroll_region_ساڄو);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    gui.char_ويڪر * ncols + 1,	gui.char_اوچائي * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سيٽ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_دري_copy_area(gui.drawarea->دري_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    gui.drawarea->دري_2,
	    FILL_X(gui.scroll_region_کاٻو),
	    FILL_Y(صف + انگ_سٽون),
	    gui.char_ويڪر * (gui.scroll_region_ساڄو
					    - gui.scroll_region_کاٻو + 1) + 1,
	    gui.char_اوچائي * (gui.scroll_region_bot - صف - انگ_سٽون + 1));

    gui_clear_بلاڪ(gui.scroll_region_bot - انگ_سٽون + 1,
						       gui.scroll_region_کاٻو,
		    gui.scroll_region_bot, gui.scroll_region_ساڄو);
    check_copy_area();
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    فراغ
gui_mch_وجهو_سٽون(صحيح صف, صحيح انگ_سٽون)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    ثابت صحيح ncols = gui.scroll_region_ساڄو - gui.scroll_region_کاٻو + 1;
    ثابت صحيح nrows = gui.scroll_region_bot - صف + 1;
    ثابت صحيح src_nrows = nrows - انگ_سٽون;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف + انگ_سٽون),
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    gui.char_ويڪر * ncols + 1,     gui.char_اوچائي * src_nrows);
    gui_clear_بلاڪ(
	    صف,		 gui.scroll_region_کاٻو,
	    صف + انگ_سٽون - 1, gui.scroll_region_ساڄو);
    قطار_draw_area(
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    gui.char_ويڪر * ncols + 1,	gui.char_اوچائي * nrows);
#وإلا
    إذا (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	أرجع;			// Can't see the window

    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.fgcolor);
    gdk_gc_سيٽ_background(gui.متن_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_دري_copy_area(gui.drawarea->دري_2, gui.متن_gc,
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف + انگ_سٽون),
	    gui.drawarea->دري_2,
	    FILL_X(gui.scroll_region_کاٻو), FILL_Y(صف),
	    gui.char_ويڪر * (gui.scroll_region_ساڄو
					    - gui.scroll_region_کاٻو + 1) + 1,
	    gui.char_اوچائي * (gui.scroll_region_bot - صف - انگ_سٽون + 1));

    gui_clear_بلاڪ(صف, gui.scroll_region_کاٻو,
				صف + انگ_سٽون - 1, gui.scroll_region_ساڄو);
    check_copy_area();
#جيڪڏهن_پورو // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    فراغ
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	هدف;
    بنانشان	i;
    time_t	شروعات;

    لكل (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	إذا (!clip_html && selection_targets[i].info == هدف_HTML)
	    استمر;
	received_selection = RS_NONE;
	هدف = gdk_atom_intern(selection_targets[i].هدف, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, هدف,
			      (guint32)GDK_CURRENT_وقت);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	شروعات = وقت(عدم);
	طالما (received_selection == RS_NONE && وقت(عدم) < شروعات + 3)
	    g_main_context_iteration(عدم, TRUE);	// wait for selection_received_cb

	إذا (received_selection != RS_FAIL)
	    أرجع;
    }

    إذا (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_دري_XDISPLAY(gtk_widget_get_دري(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    فراغ
clip_mch_lose_selection(Clipboard_T *cbd اڻ_استعمال_ٿيل)
{
    إذا (اندريون_selection_clear_event)
	أرجع;

    gtk_selection_owner_سيٽ(عدم, cbd->gtk_sel_atom, gui.event_وقت);
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

    success = gtk_selection_owner_سيٽ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_وقت);
    // don't update on every visual selection change
    إذا (!(cbd->owned && VIsual_سرگرم))
	gui_gtk_سيٽ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    أرجع (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    فراغ
clip_mch_سيٽ_selection(Clipboard_T *cbd اڻ_استعمال_ٿيل)
{
}

#إذا defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    صحيح
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    أرجع gdk_selection_owner_get(cbd->gtk_sel_atom) != عدم;
}
#جيڪڏهن_پورو


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
	gtk_widget_سيٽ_sensitive(menu->id, !grey);
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
#جيڪڏهن_پورو // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    فراغ
gui_mch_enable_scrollbar(scrollbar_T *sb, صحيح flag)
{
    إذا (sb->id == عدم)
	أرجع;

    gtk_widget_سيٽ_visible(sb->id, flag);
    update_دري_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#إذا GTK_CHECK_نسخو(3,0,0)
    أرجع (long_u)pixel;
#وإلا
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (بنانشان ڊگهو)pixel, &color);

    أرجع (guicolor_T)(
	    (((بنانشان)color.red   & 0xff00) << 8)
	 |  ((بنانشان)color.green & 0xff00)
	 | (((بنانشان)color.blue  & 0xff00) >> 8));
#جيڪڏهن_پورو
}

/*
 * Get current mouse coordinates in text window.
 */
    فراغ
gui_mch_getmouse(صحيح *x, صحيح *y)
{
    gui_gtk_get_اشارو(gui.drawarea, x, y, عدم);
}

    فراغ
gui_mch_setmouse(صحيح x, صحيح y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    إذا (dpy)
	XWarp_اشارو(dpy, (دري)0,
		GDK_دري_XID(gtk_widget_get_دري(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#جيڪڏهن_وصف_ٿيل FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ساكن صحيح آخري_shape = 0;
#جيڪڏهن_پورو

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    فراغ
gui_mch_mousehide(صحيح hide)
{
    إذا (gui.اشارو_hidden == hide)
	أرجع;

    gui.اشارو_hidden = hide;
    إذا (gtk_widget_get_دري(gui.drawarea) && gui.blank_اشارو != عدم)
    {
	إذا (hide)
	    gdk_دري_سيٽ_cursor(gtk_widget_get_دري(gui.drawarea),
		    gui.blank_اشارو);
	وإلا
#جيڪڏهن_وصف_ٿيل FEAT_MOUSESHAPE
	    mch_سيٽ_mouse_shape(آخري_shape);
#وإلا
	gdk_دري_سيٽ_cursor(gtk_widget_get_دري(gui.drawarea), عدم);
#جيڪڏهن_پورو
    }
}

#إذا defined(FEAT_MOUSESHAPE)

# إذا GTK_CHECK_نسخو(3,0,0)
ساكن ثابت حرف * mshape_css_نالا[] =
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
    GDK_کاٻو_PTR,		// arrow
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
    GDK_ساڄو_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_کاٻو_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    فراغ
mch_سيٽ_mouse_shape(صحيح shape)
{
    GdkCursor	   *c;
    صحيح		   id;                    // Only id or css_name is used.
# إذا GTK_CHECK_نسخو(3,0,0)
    ثابت حرف     *css_name = "default";
    GdkDisplay     *disp;
# endif

    إذا (gtk_widget_get_دري(gui.drawarea) == عدم)
	أرجع;

    إذا (shape == MSHAPE_HIDE || gui.اشارو_hidden)
	gdk_دري_سيٽ_cursor(gtk_widget_get_دري(gui.drawarea),
		gui.blank_اشارو);
    وإلا
    {
	إذا (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    إذا (id >= GDK_آخري_CURSOR)
		id = GDK_کاٻو_PTR;
	    وإلا
		id &= ~1;	// they are always even (why?)
	}
# إذا GTK_CHECK_نسخو(3,0,0)
	وإلا إذا (shape < (صحيح)ARRAY_ڊيگهائي(mshape_css_نالا))
	    css_name = mshape_css_نالا[shape];
# وإلا
	وإلا إذا (shape < (صحيح)ARRAY_ڊيگهائي(mshape_ids))
	    id = mshape_ids[shape];
# endif
	وإلا
	    أرجع;
# إذا GTK_CHECK_نسخو(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# وإلا
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_دري_سيٽ_cursor(gtk_widget_get_دري(gui.drawarea), c);
# إذا GTK_CHECK_نسخو(3,0,0)
	g_object_unref(G_OBJECT(c));
# وإلا
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    إذا (shape != MSHAPE_HIDE)
	آخري_shape = shape;
}
#جيڪڏهن_پورو // FEAT_MOUSESHAPE


#إذا defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_ويڪر  (2 * gui.char_ويڪر)
# define SIGN_اوچائي (gui.char_اوچائي)
# define SIGN_ASPECT ((مزدوج)SIGN_اوچائي / (مزدوج)SIGN_ويڪر)

    فراغ
gui_mch_drawsign(صحيح صف, صحيح کانو, صحيح typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    إذا (sign == عدم || gui.drawarea == عدم
	    || gtk_widget_get_دري(gui.drawarea) == عدم)
	أرجع;

    صحيح ويڪر;
    صحيح اوچائي;
    صحيح xoffset;
    صحيح yoffset;
    صحيح need_scale;

    ويڪر  = gdk_pixbuf_get_ويڪر(sign);
    اوچائي = gdk_pixbuf_get_اوچائي(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (ويڪر > SIGN_ويڪر + 2
	    || اوچائي != SIGN_اوچائي
	    || (ويڪر < 3 * SIGN_ويڪر / 4
		&& اوچائي < 3 * SIGN_اوچائي / 4));
    إذا (need_scale)
    {
	مزدوج  aspect;
	صحيح	    w = ويڪر;
	صحيح	    h = اوچائي;

	// Keep the original aspect ratio
	aspect = (مزدوج)اوچائي / (مزدوج)ويڪر;
	ويڪر  = (مزدوج)SIGN_ويڪر * SIGN_ASPECT / aspect;
	ويڪر  = MIN(ويڪر, SIGN_ويڪر);
	إذا (((مزدوج)(MAX(اوچائي, SIGN_اوچائي)) /
		    (مزدوج)(MIN(اوچائي, SIGN_اوچائي))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    اوچائي = (مزدوج)SIGN_اوچائي * SIGN_ASPECT / aspect;
	    اوچائي = MIN(اوچائي, SIGN_اوچائي);
	}
	وإلا
	    اوچائي = (مزدوج)ويڪر * aspect;

	إذا (w == ويڪر && h == اوچائي)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	وإلا
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, ويڪر, اوچائي,
		    GDK_INTERP_BILINEAR);
	    إذا (sign == عدم)
		أرجع; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (ويڪر  - SIGN_ويڪر)  / 2;
    yoffset = (اوچائي - SIGN_اوچائي) / 2;

# إذا GTK_CHECK_نسخو(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ويڪر, SIGN_اوچائي);
	bg_cr = cairo_create(bg_surf);
	cairo_سيٽ_ذريعو_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ويڪر, SIGN_اوچائي);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_سيٽ_ذريعو_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_سيٽ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_سيٽ_ذريعو_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_سيٽ_ذريعو_surface(cr, sign_surf, FILL_X(کانو), FILL_Y(صف));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	قطار_draw_area(
		FILL_X(کانو), FILL_Y(کانو), ويڪر, اوچائي);

    }
# وإلا // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_سيٽ_foreground(gui.متن_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->دري_2,
	    gui.متن_gc,
	    TRUE,
	    FILL_X(کانو),
	    FILL_Y(صف),
	    SIGN_ويڪر,
	    SIGN_اوچائي);

    gdk_pixbuf_render_منزل_drawable_alpha(sign,
	    gui.drawarea->دري_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(کانو) - MIN(0, xoffset),
	    FILL_Y(صف) - MIN(0, yoffset),
	    MIN(ويڪر,	SIGN_ويڪر),
	    MIN(اوچائي, SIGN_اوچائي),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_عام,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    إذا (need_scale)
	g_object_unref(sign);
}

    فراغ *
gui_mch_رجسٽر_sign(char_u *signfile)
{
    إذا (signfile[0] != NUL && signfile[0] != '-' && gui.اندريون_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = عدم;
	char_u	    *پيغام;

	sign = gdk_pixbuf_new_from_file((ثابت حرف *)signfile, &error);

	إذا (error == عدم)
	    أرجع sign;

	پيغام = (char_u *)error->پيغام;

	إذا (پيغام != عدم && داخلا_conv.vc_type != CONV_NONE)
	    پيغام = string_convert(&داخلا_conv, پيغام, عدم);

	إذا (پيغام != عدم)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", پيغام);

	    إذا (داخلا_conv.vc_type != CONV_NONE)
		vim_free(پيغام);
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

#جيڪڏهن_پورو // FEAT_SIGN_ICONS

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
    صحيح	    w = img->ويڪر;
    صحيح	    h = img->اوچائي;
    صحيح	    stride;
    صحيح	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_معلومات(surf);
    stride = cairo_image_surface_get_stride(surf);

    إذا (img->fmt == IMAGE_FORMAT_RGBA)
    {
	لكل (y = 0; y < h; ++y)
	{
	    بنانشان صحيح *p = (بنانشان صحيح *)(dst + y * stride);

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
	    بنانشان صحيح *p = (بنانشان صحيح *)(dst + y * stride);

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
# إذا GTK_CHECK_نسخو(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    إذا (ctx == عدم)
	أرجع FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->ويڪر, img->اوچائي);

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
# إذا GTK_CHECK_نسخو(3,0,0)
    image_cairo_T *ctx = img->backend_معلومات;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    صحيح
image_placement_gui_init(image_placement_T *رکو اڻ_استعمال_ٿيل)
{
# إذا GTK_CHECK_نسخو(3,0,0)
    أرجع OK;
# وإلا
    أرجع FAIL;
# endif
}

    فراغ
image_placement_gui_uninit(image_placement_T *رکو اڻ_استعمال_ٿيل)
{
}

    فراغ
image_placement_gui_draw(image_placement_T *رکو)
{
# إذا GTK_CHECK_نسخو(3,0,0)
    image_T		    *img = رکو->img;
    image_cairo_T	    *ictx = img->backend_معلومات;
    pixman_box32_t	    *rects;
    صحيح			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&رکو->visible, &n_rects);
    إذا (rects == عدم || n_rects == 0)
	أرجع;

    cairo_surface_سيٽ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    لكل (صحيح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحيح		صف, کانو;
	صحيح		posx, posy;
	صحيح		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(رکو, rect, &صف, &کانو, &x, &y, &w, &h, false);
	posx = FILL_X(کانو);
	posy = FILL_Y(صف);

	cairo_save(cr);
	cairo_سيٽ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_سيٽ_ذريعو_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	قطار_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    فراغ
image_placement_gui_clear(image_placement_T *رکو اڻ_استعمال_ٿيل)
{
}

#جيڪڏهن_پورو // FEAT_IMAGE
