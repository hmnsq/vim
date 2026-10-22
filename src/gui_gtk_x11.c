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

#അന്തര്ഭാവ "vim.h"
#ifdef USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#endif

#ifdef FEAT_GUI_GNOME
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
# എങ്കിൽ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
ബാഹ്യ ശൂന്യം bonobo_dock_item_set_behavior(BonoboDockItem *dock_item, BonoboDockItemBehavior beh);
#endif

#എങ്കിൽ defined(FEAT_GUI_GTK)
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# അല്ലെങ്കിൽ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# അല്ലെങ്കിൽ
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#endif

#ifdef HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#endif

/*
 * Easy-to-use macro for multihead support.
 */
#define GET_X_ATOM(atom)	gdk_x11_atom_to_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
എണ്ണൽ
{
    TARGET_TYPE_NONE,
    TARGET_UTF8_STRING,
    TARGET_STRING,
    TARGET_COMPOUND_TEXT,
    TARGET_HTML,
    TARGET_TEXT,
    TARGET_TEXT_URI_LIST,
    TARGET_TEXT_PLAIN,
    TARGET_TEXT_PLAIN_UTF8,
    TARGET_VIM_MT,
    TARGET_VIM,
    TARGET_VIMENC_MT,
    TARGET_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
സ്ഥിതം സ്ഥിരം GtkTargetEntry selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, TARGET_VIMENC},
    {VIM_ATOM_NAME,	0, TARGET_VIM},
    {VIMENC_MIMETYPE_NAME,  0, TARGET_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, TARGET_VIM_MT},
    {"text/html",	0, TARGET_HTML},
    {"UTF8_STRING",	0, TARGET_UTF8_STRING},
    {"COMPOUND_TEXT",	0, TARGET_COMPOUND_TEXT},
    {"TEXT",		0, TARGET_TEXT},
    {"STRING",		0, TARGET_STRING},
    {"text/plain;charset=utf-8", 0, TARGET_TEXT_PLAIN_UTF8},
    {"text/plain",	0, TARGET_TEXT_PLAIN}
};
#define N_SELECTION_TARGETS ARRAY_LENGTH(selection_targets)

#ifdef FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
സ്ഥിതം സ്ഥിരം GtkTargetEntry dnd_targets[] =
{
    {"text/uri-list",	0, TARGET_TEXT_URI_LIST},
    {"text/html",	0, TARGET_HTML},
    {"UTF8_STRING",	0, TARGET_UTF8_STRING},
    {"STRING",		0, TARGET_STRING},
    {"text/plain",	0, TARGET_TEXT_PLAIN}
};
# define N_DND_TARGETS ARRAY_LENGTH(dnd_targets)
#endif


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#define DEFAULT_FONT	"Monospace 12"

#എങ്കിൽ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#endif

#എങ്കിൽ !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
സ്ഥിതം GdkAtom wm_protocols_atom = GDK_NONE;
സ്ഥിതം GdkAtom save_yourself_atom = GDK_NONE;
#endif

/*
 * Atoms used to control/reference X11 selections.
 */
സ്ഥിതം GdkAtom html_atom = GDK_NONE;
സ്ഥിതം GdkAtom utf8_string_atom = GDK_NONE;
സ്ഥിതം GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
സ്ഥിതം GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
സ്ഥിതം GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
സ്ഥിതം GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
സ്ഥിതം ഘടന special_താക്കോൽ
{
    guint താക്കോൽ_sym;
    char_u code0;
    char_u code1;
}
സ്ഥിരം special_താക്കോലുകൾ[] =
{
    {GDK_Up,		'k', 'u'},
    {GDK_Down,		'k', 'd'},
    {GDK_ഇടത്,		'k', 'l'},
    {GDK_വലത്,		'k', 'r'},
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
#ifdef SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#endif
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_BackSpace,	'k', 'b'},
    {GDK_തിരുകുക,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_BackTab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_End,		'@', '7'},
    {GDK_Prior,		'k', 'P'},
    {GDK_Next,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_ഇടത്,	'k', 'l'},
    {GDK_KP_വലത്,	'k', 'r'},
    {GDK_KP_Up,		'k', 'u'},
    {GDK_KP_Down,	'k', 'd'},
    {GDK_KP_തിരുകുക,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_End,	'K', '4'},
    {GDK_KP_Prior,	'K', '3'},  // page up
    {GDK_KP_Next,	'K', '5'},  // page down

    {GDK_KP_Add,	'K', '6'},
    {GDK_KP_Subtract,	'K', '7'},
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
#define ARG_FONT	1
#define ARG_GEOMETRY	2
#define ARG_REVERSE	3
#define ARG_NOREVERSE	4
#define ARG_BACKGROUND	5
#define ARG_FOREGROUND	6
#define ARG_ICONIC	7
#define ARG_ROLE	8
#define ARG_NETBEANS	9
#define ARG_XRM		10	// ignored
#define ARG_MENUFONT	11	// ignored
#define ARG_INDEX_MASK	0x00ff
#define ARG_HAS_മൂല്യം	0x0100	// a value is expected after the argument
#define ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#define ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#define ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#define ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
തരനിർവചനം ഘടന
{
    സ്ഥിരം അക്ഷരം	    *name;
    unsigned പൂർണ്ണസംഖ്യ    flags;
}
cmdline_option_T;

സ്ഥിതം സ്ഥിരം cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_മൂല്യം},
    {"-font",		ARG_FONT|ARG_HAS_മൂല്യം},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_മൂല്യം},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_മൂല്യം},
    {"-rv",		ARG_REVERSE},
    {"-reverse",	ARG_REVERSE},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_മൂല്യം},
    {"-background",	ARG_BACKGROUND|ARG_HAS_മൂല്യം},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_മൂല്യം},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_മൂല്യം},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_മൂല്യം},
#ifdef FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_മൂല്യം},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_മൂല്യം},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_മൂല്യം},	// not implemented
#endif
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_മൂല്യം|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_മൂല്യം|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_മൂല്യം|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
#ifdef FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_മൂല്യം},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# എങ്കിൽ 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#endif
    {ശൂന്യ, 0}
};

സ്ഥിതം പൂർണ്ണസംഖ്യ    gui_argc = 0;
സ്ഥിതം അക്ഷരം **gui_argv = ശൂന്യ;

സ്ഥിതം സ്ഥിരം അക്ഷരം *role_argument = ശൂന്യ;
#എങ്കിൽ defined(USE_GNOME_SESSION)
സ്ഥിതം സ്ഥിരം അക്ഷരം *restart_command = ശൂന്യ;
സ്ഥിതം       അക്ഷരം *abs_restart_command = ശൂന്യ;
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ found_iconic_arg = FALSE;

#ifdef FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ using_gnome = 0;
#അല്ലെങ്കിൽ
# define using_gnome 0
#endif

// Comment out the following line to ignore code for resize history tracking.
#define TRACK_RESIZE_HISTORY
#ifdef TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# എങ്കിൽ 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
തരനിർവചനം ഘടന resize_history {
    പൂർണ്ണസംഖ്യ used;		// If true, can't match for discard. Only matches once.
    പൂർണ്ണസംഖ്യ width;
    പൂർണ്ണസംഖ്യ height;
    പൂർണ്ണസംഖ്യ from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    പൂർണ്ണസംഖ്യ seq;		// for ch_log messages
# endif
    ഘടന resize_history *next;
} resize_hist_T;

// never NULL during execution
സ്ഥിതം resize_hist_T *latest_resize_hist;
// list of stale resize requests
സ്ഥിതം resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
സ്ഥിതം പൂർണ്ണസംഖ്യ mch_csd_width = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ mch_csd_height = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
സ്ഥിതം പൂർണ്ണസംഖ്യ mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    സ്ഥിതം ശൂന്യം
alloc_resize_hist(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height, പൂർണ്ണസംഖ്യ from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->width = width;
    new_hist->height = height;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->next = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(ശൂന്യ, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, width, height, (പൂർണ്ണസംഖ്യ)Columns, (പൂർണ്ണസംഖ്യ)Rows);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    സ്ഥിതം ശൂന്യം
clear_resize_hists(ശൂന്യം)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    പൂർണ്ണസംഖ്യ		    i = 0;
# endif

    എങ്കിൽ (latest_resize_hist)
	latest_resize_hist->used = TRUE;
    ആകുമ്പോൾ (old_resize_hists != ശൂന്യ)
    {
	resize_hist_T *next_hist = old_resize_hists->next;

	vim_free(old_resize_hists);
	old_resize_hists = next_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ശൂന്യ, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define MATCH_WIDTH_HEIGHT(hist, w, h) \
	  (!hist->used && hist->width == w && hist->height == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
match_stale_width_height(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height)
{
    resize_hist_T *hist = old_resize_hists;

    വേണ്ടി (hist = old_resize_hists; hist != ശൂന്യ; hist = hist->next)
	എങ്കിൽ (MATCH_WIDTH_HEIGHT(hist, width, height))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(ശൂന്യ, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->used = TRUE;
	    തിരികെ TRUE;
	}
    തിരികെ FALSE;
}

# എങ്കിൽ defined(EXITFREE)
    സ്ഥിതം ശൂന്യം
free_all_resize_hist(ശൂന്യം)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#endif

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
സ്ഥിതം guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ശൂന്യം
gui_mch_prepare(പൂർണ്ണസംഖ്യ *argc, അക്ഷരം **argv)
{
    സ്ഥിരം cmdline_option_T  *option;
    പൂർണ്ണസംഖ്യ			    i	= 0;
    പൂർണ്ണസംഖ്യ			    len = 0;

#എങ്കിൽ defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    എങ്കിൽ (strchr(argv[0], G_DIR_SEPARATOR) != ശൂന്യ)
    {
	char_u buf[MAXPATHL];

	എങ്കിൽ (mch_FullName((char_u *)argv[0], buf, (പൂർണ്ണസംഖ്യ)വലുപ്പം(buf), TRUE) == OK)
	{
	    abs_restart_command = (അക്ഷരം *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#endif

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(അക്ഷരം *, *argc + 1);

    g_return_if_fail(gui_argv != ശൂന്യ);

    gui_argv[gui_argc++] = argv[i++];

    ആകുമ്പോൾ (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	എങ്കിൽ (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    തുടരുക;
	}

	// Look for argv[i] in cmdline_options[] table.
	വേണ്ടി (option = &cmdline_options[0]; option->name != ശൂന്യ; ++option)
	{
	    len = strlen(option->name);

	    എങ്കിൽ (strncmp(argv[i], option->name, len) == 0)
	    {
		എങ്കിൽ (argv[i][len] == '\0')
		    നിർത്തുക;
		// allow --foo=bar style
		എങ്കിൽ (argv[i][len] == '=' && (option->flags & ARG_HAS_മൂല്യം))
		    നിർത്തുക;
#ifdef FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		എങ്കിൽ (vim_strchr((char_u *)":=", argv[i][len]) != ശൂന്യ
			&& (option->flags & ARG_INDEX_MASK) == ARG_NETBEANS)
		    നിർത്തുക;
#endif
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (അക്ഷരം *)option->name;
		നിർത്തുക;
	    }
	}
	എങ്കിൽ (option->name == ശൂന്യ) // no match
	{
	    ++i;
	    തുടരുക;
	}

	എങ്കിൽ (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	അല്ലെങ്കിൽ
	{
	    അക്ഷരം *മൂല്യം = ശൂന്യ;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    എങ്കിൽ (option->flags & ARG_HAS_മൂല്യം)
	    {
		എങ്കിൽ (argv[i][len] == '=')
		    മൂല്യം = &argv[i][len + 1];
		അല്ലെങ്കിൽ എങ്കിൽ (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    മൂല്യം = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    തിരഞ്ഞെടുപ്പ് (option->flags & ARG_INDEX_MASK)
	    {
		അവസ്ഥ ARG_REVERSE:
		    found_reverse_arg = TRUE;
		    നിർത്തുക;
		അവസ്ഥ ARG_NOREVERSE:
		    found_reverse_arg = FALSE;
		    നിർത്തുക;
		അവസ്ഥ ARG_FONT:
		    font_argument = മൂല്യം;
		    നിർത്തുക;
		അവസ്ഥ ARG_GEOMETRY:
		    എങ്കിൽ (മൂല്യം != ശൂന്യ)
			gui.geom = vim_strsave((char_u *)മൂല്യം);
		    നിർത്തുക;
		അവസ്ഥ ARG_BACKGROUND:
		    background_argument = മൂല്യം;
		    നിർത്തുക;
		അവസ്ഥ ARG_FOREGROUND:
		    foreground_argument = മൂല്യം;
		    നിർത്തുക;
		അവസ്ഥ ARG_ICONIC:
		    found_iconic_arg = TRUE;
		    നിർത്തുക;
		അവസ്ഥ ARG_ROLE:
		    role_argument = മൂല്യം; // used later in gui_mch_open()
		    നിർത്തുക;
#ifdef FEAT_NETBEANS_INTG
		അവസ്ഥ ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    നിർത്തുക;
#endif
		സ്ഥിരസ്ഥിതി:
		    നിർത്തുക;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	എങ്കിൽ ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	എങ്കിൽ (option->flags & ARG_KEEP)
	    ++i;
	അല്ലെങ്കിൽ
	{
	    // Remove the flag from the argument vector.
	    എങ്കിൽ (--*argc > i)
	    {
		പൂർണ്ണസംഖ്യ n_strip = 1;

		// Move the argument's value as well, if there is one.
		എങ്കിൽ ((option->flags & ARG_HAS_മൂല്യം)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    എങ്കിൽ (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		എങ്കിൽ (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * വലുപ്പം(അക്ഷരം *));
	    }
	    argv[*argc] = ശൂന്യ;
	}
    }

    gui_argv[gui_argc] = ശൂന്യ;
}

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
gui_mch_free_all(ശൂന്യം)
{
    vim_free(gui_argv);
# എങ്കിൽ defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#endif

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    സ്ഥിതം gint
visibility_event(GtkWidget *widget UNUSED,
		 GdkEventVisibility *event,
		 gpointer data UNUSED)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    എങ്കിൽ (gui.text_gc != ശൂന്യ)
	gdk_gc_set_exposures(gui.text_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    തിരികെ FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gboolean
draw_event(GtkWidget *widget UNUSED,
	   cairo_t   *cr,
	   gpointer   user_data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    എങ്കിൽ (gui.starting)
	തിരികെ FALSE;

    out_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_set_source_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    തിരികെ FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# അല്ലെങ്കിൽ
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
    സ്ഥിതം gboolean
scale_factor_event(GtkWidget *widget,
		   GParamSpec* pspec UNUSED,
		   gpointer   user_data UNUSED)
{
    എങ്കിൽ (gui.surface != ശൂന്യ)
	cairo_surface_destroy(gui.surface);

    പൂർണ്ണസംഖ്യ	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    പൂർണ്ണസംഖ്യ	    usable_height = h;
    എങ്കിൽ (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_height);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  എങ്കിൽ defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(widget);
#  endif

    തിരികെ TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gint
expose_event(GtkWidget *widget UNUSED,
	     GdkEventExpose *event,
	     gpointer data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    എങ്കിൽ (gui.starting)
	തിരികെ FALSE;

    out_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.width, event->area.height);

    // Clear the border areas if needed
    എങ്കിൽ (event->area.x < FILL_X(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, FILL_X(0), 0);
    എങ്കിൽ (event->area.y < FILL_Y(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, 0, FILL_Y(0));
    എങ്കിൽ (event->area.x > FILL_X(Columns))
	gdk_window_clear_area(gui.drawarea->window,
			      FILL_X((പൂർണ്ണസംഖ്യ)Columns), 0, 0, 0);
    എങ്കിൽ (event->area.y > FILL_Y(Rows))
	gdk_window_clear_area(gui.drawarea->window, 0, FILL_Y((പൂർണ്ണസംഖ്യ)Rows), 0, 0);

    തിരികെ FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#ifdef FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    സ്ഥിതം gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer data UNUSED)
{
    എങ്കിൽ (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (പൂർണ്ണസംഖ്യ)GDK_PROPERTY_NEW_മൂല്യം
	    && GDK_WINDOW_XID(event->window) == commWindow
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.window = commWindow;
	xev.xproperty.state = Property_New_മൂല്യം;
	serverEventProc(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(widget)),
								      &xev, 0);
    }
    തിരികെ FALSE;
}
#endif // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    സ്ഥിതം ശൂന്യം
gtk_settings_xft_dpi_changed_cb(GtkSettings *gtk_settings UNUSED,
				    GParamSpec *pspec UNUSED,
				    gpointer data UNUSED)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    എങ്കിൽ (gui.text_context != ശൂന്യ)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    എങ്കിൽ (gui.norm_font != ശൂന്യ)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? ശൂന്യ : p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

തരനിർവചനം gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    സ്ഥിതം guint
timeout_add(പൂർണ്ണസംഖ്യ time, timeout_cb_type (*callback)(gpointer), പൂർണ്ണസംഖ്യ *flagp)
{
    തിരികെ g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    സ്ഥിതം ശൂന്യം
timeout_remove(guint timer)
{
    g_source_remove(timer);
}


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#define BLINK_NONE  0
#define BLINK_OFF   1
#define BLINK_ON    2

സ്ഥിതം പൂർണ്ണസംഖ്യ blink_state = BLINK_NONE;
സ്ഥിതം long_u blink_waittime = 700;
സ്ഥിതം long_u blink_ontime = 400;
സ്ഥിതം long_u blink_offtime = 250;
സ്ഥിതം guint blink_timer = 0;

    പൂർണ്ണസംഖ്യ
gui_mch_is_blinking(ശൂന്യം)
{
    തിരികെ blink_state != BLINK_NONE;
}

    പൂർണ്ണസംഖ്യ
gui_mch_is_blink_off(ശൂന്യം)
{
    തിരികെ blink_state == BLINK_OFF;
}

    ശൂന്യം
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ശൂന്യം
gui_mch_stop_blink(പൂർണ്ണസംഖ്യ may_call_gui_update_cursor)
{
    എങ്കിൽ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    എങ്കിൽ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

    സ്ഥിതം timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    എങ്കിൽ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, ശൂന്യ);
    }
    gui_may_flush();

    തിരികെ FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ശൂന്യം
gui_mch_start_blink(ശൂന്യം)
{
    എങ്കിൽ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    എങ്കിൽ (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, ശൂന്യ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    സ്ഥിതം gint
enter_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    എങ്കിൽ (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes there
    എങ്കിൽ (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    തിരികെ FALSE;
}

    സ്ഥിതം gint
leave_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    എങ്കിൽ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    തിരികെ FALSE;
}

    സ്ഥിതം gint
focus_in_event(GtkWidget *widget,
	       GdkEventFocus *event UNUSED,
	       gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    എങ്കിൽ (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    അല്ലെങ്കിൽ
#endif
	gui_focus_change(TRUE);

    എങ്കിൽ (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    എങ്കിൽ (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    തിരികെ TRUE;
}

    സ്ഥിതം gint
focus_out_event(GtkWidget *widget UNUSED,
		GdkEventFocus *event UNUSED,
		gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    എങ്കിൽ (gui.dialogs_active > 0)
	++gui.dialog_focus_pending;
#endif

    gui_focus_change(FALSE);

    എങ്കിൽ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    തിരികെ TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
keyval_to_string(unsigned പൂർണ്ണസംഖ്യ keyval, char_u *string)
{
    പൂർണ്ണസംഖ്യ	    len;
    guint32 uc;

    uc = gdk_keyval_to_unicode(keyval);
    എങ്കിൽ (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((പൂർണ്ണസംഖ്യ)uc, string);
    }
    അല്ലെങ്കിൽ
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	തിരഞ്ഞെടുപ്പ് (keyval)
	{
	    അവസ്ഥ GDK_Tab: അവസ്ഥ GDK_KP_Tab: അവസ്ഥ GDK_ISO_ഇടത്_Tab:
		string[0] = TAB;
		നിർത്തുക;
	    അവസ്ഥ GDK_Linefeed:
		string[0] = NL;
		നിർത്തുക;
	    അവസ്ഥ GDK_Return: അവസ്ഥ GDK_ISO_Enter: അവസ്ഥ GDK_3270_Enter:
		string[0] = CAR;
		നിർത്തുക;
	    അവസ്ഥ GDK_Escape:
		string[0] = ESC;
		നിർത്തുക;
	    സ്ഥിരസ്ഥിതി:
		len = 0;
		നിർത്തുക;
	}
    }
    string[len] = NUL;

    തിരികെ len;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
modifiers_gdk2vim(guint state)
{
    പൂർണ്ണസംഖ്യ modifiers = 0;

    എങ്കിൽ (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    എങ്കിൽ (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    എങ്കിൽ (state & GDK_MOD1_MASK)
	modifiers |= MOD_MASK_ALT;
#എങ്കിൽ GTK_CHECK_VERSION(2,10,0)
    എങ്കിൽ (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    എങ്കിൽ (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;
#അല്ലെങ്കിൽ
    എങ്കിൽ (state & GDK_MOD4_MASK)
	modifiers |= MOD_MASK_CMD;
#endif

    തിരികെ modifiers;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
modifiers_gdk2mouse(guint state)
{
    പൂർണ്ണസംഖ്യ modifiers = 0;

    എങ്കിൽ (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    എങ്കിൽ (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    എങ്കിൽ (state & GDK_MOD1_MASK)
	modifiers |= MOUSE_ALT;

    തിരികെ modifiers;
}

/*
 * Main keyboard handler:
 */
    സ്ഥിതം gint
താക്കോൽ_press_event(GtkWidget *widget UNUSED,
		Gdk_Event_താക്കോൽ *event,
		gpointer data UNUSED)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	താക്കോൽ_sym;
    പൂർണ്ണസംഖ്യ		len;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		modifiers;
    പൂർണ്ണസംഖ്യ		താക്കോൽ;
    guint	state;
    char_u	*s, *d;
    പൂർണ്ണസംഖ്യ		ctrl_prefix_added = 0;

    gui.event_time = event->time;
    താക്കോൽ_sym = event->keyval;
    state = event->state;

#ifdef FEAT_XIM
    എങ്കിൽ (xim_queue_താക്കോൽ_press_event(event, TRUE))
	തിരികെ TRUE;
#endif

#ifdef SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    എങ്കിൽ (താക്കോൽ_sym == SunXK_F36 || താക്കോൽ_sym == SunXK_F37)
	len = 0;
    അല്ലെങ്കിൽ
#endif
    {
	len = keyval_to_string(താക്കോൽ_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	എങ്കിൽ (len > 1 && നിവേശം_conv.vc_type != CONV_NONE)
	    len = convert_നിവേശം(string2, len, വലുപ്പം(string2));

	s = string2;
	d = string;
	വേണ്ടി (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    എങ്കിൽ (d[-1] == CSI && d + 2 < string + വലുപ്പം(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (പൂർണ്ണസംഖ്യ)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    എങ്കിൽ (താക്കോൽ_sym == GDK_ISO_ഇടത്_Tab)
    {
	താക്കോൽ_sym = GDK_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    എങ്കിൽ ((state & GDK_MOD1_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	തിരികെ FALSE;
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    എങ്കിൽ (len == 0 || len == 1)
    {
	വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != 0; i++)
	{
	    എങ്കിൽ (special_താക്കോലുകൾ[i].താക്കോൽ_sym == താക്കോൽ_sym)
	    {
		string[0] = CSI;
		string[1] = special_താക്കോലുകൾ[i].code0;
		string[2] = special_താക്കോലുകൾ[i].code1;
		len = -3;
		നിർത്തുക;
	    }
	}
    }

#ifdef GDK_താക്കോൽ_dead_circumflex
    // Belgian Ctrl+[ workaround
    എങ്കിൽ (len == 0 && താക്കോൽ_sym == GDK_താക്കോൽ_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_MASK_CTRL;
	string[3] = '[';
	len = 4;
	add_to_നിവേശം_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	തിരികെ TRUE;
    }
#endif

    എങ്കിൽ (len == 0)   // Unrecognized key
	തിരികെ TRUE;

    // For some keys a shift modifier is translated into another key code.
    എങ്കിൽ (len == -3)
	താക്കോൽ = TO_SPECIAL(string[1], string[2]);
    അല്ലെങ്കിൽ
    {
	string[len] = NUL;
	താക്കോൽ = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    താക്കോൽ = simplify_താക്കോൽ(താക്കോൽ, &modifiers);
    എങ്കിൽ (താക്കോൽ == CSI)
	താക്കോൽ = K_CSI;
    എങ്കിൽ (IS_SPECIAL(താക്കോൽ))
    {
	string[0] = CSI;
	string[1] = K_SECOND(താക്കോൽ);
	string[2] = K_THIRD(താക്കോൽ);
	len = 3;
    }
    അല്ലെങ്കിൽ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	താക്കോൽ = may_adjust_താക്കോൽ_for_ctrl(modifiers, താക്കോൽ);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_shift_modifier(modifiers, താക്കോൽ);

	len = mb_char2bytes(താക്കോൽ, string);
    }

    എങ്കിൽ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_നിവേശം_buf(string2, 3);
	എങ്കിൽ (modifiers == 0x4)
	    ctrl_prefix_added = 1;
    }

    // Check if the key interrupts.
    {
	പൂർണ്ണസംഖ്യ int_ch = check_for_interrupt(താക്കോൽ, modifiers);

	എങ്കിൽ (int_ch != NUL)
	{
	    trash_നിവേശം_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    എങ്കിൽ (ctrl_prefix_added && len == 2
		    && ((പൂർണ്ണസംഖ്യ)string[0]) == 195
		    && ((പൂർണ്ണസംഖ്യ)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    add_to_നിവേശം_buf(string, len);

    // blank out the pointer if necessary
    എങ്കിൽ (p_mh)
	gui_mch_mousehide(TRUE);

    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gboolean
താക്കോൽ_release_event(GtkWidget *widget UNUSED,
		  Gdk_Event_താക്കോൽ *event UNUSED,
		  gpointer data UNUSED)
{
# എങ്കിൽ defined(FEAT_XIM)
    gui.event_time = event->time;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    തിരികെ xim_queue_താക്കോൽ_press_event(event, FALSE);
# അല്ലെങ്കിൽ
    തിരികെ TRUE;
# endif
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
സ്ഥിതം പൂർണ്ണസംഖ്യ in_selection_clear_event = FALSE;

    സ്ഥിതം gint
selection_clear_event(GtkWidget		*widget UNUSED,
		      GdkEventSelection	*event,
		      gpointer		user_data UNUSED)
{
    in_selection_clear_event = TRUE;
    എങ്കിൽ (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    അല്ലെങ്കിൽ
	clip_lose_selection(&clip_star);
    in_selection_clear_event = FALSE;

    തിരികെ TRUE;
}

#define RS_NONE	0	// selection_received_cb() not called yet
#define RS_OK	1	// selection_received_cb() called and OK
#define RS_FAIL	2	// selection_received_cb() called and failed
സ്ഥിതം പൂർണ്ണസംഖ്യ received_selection = RS_NONE;

    സ്ഥിതം ശൂന്യം
selection_received_cb(GtkWidget		*widget UNUSED,
		      GtkSelectionData	*data,
		      guint		time_ UNUSED,
		      gpointer		user_data UNUSED)
{
    Clipboard_T	    *cbd;
    char_u	    *text;
    char_u	    *tmpbuf = ശൂന്യ;
    guchar	    *tmpbuf_utf8 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    len;
    പൂർണ്ണസംഖ്യ		    motion_type = MAUTO;

    എങ്കിൽ (gtk_selection_data_get_selection(data) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    അല്ലെങ്കിൽ
	cbd = &clip_star;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    എങ്കിൽ (text == ശൂന്യ || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	തിരികെ;
    }

    എങ്കിൽ (gtk_selection_data_get_data_type(data) == vim_atom
	    || gtk_selection_data_get_data_type(data) == vim_mt_atom)
    {
	motion_type = *text++;
	--len;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (gtk_selection_data_get_data_type(data) == vimenc_atom
	    || gtk_selection_data_get_data_type(data) == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;

	motion_type = *text++;
	--len;

	enc = text;
	text += STRLEN(text) + 1;
	len -= text - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_setup(&conv, enc, p_enc);
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, text, &len);
	    എങ്കിൽ (tmpbuf != ശൂന്യ)
		text = tmpbuf;
	    convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    അല്ലെങ്കിൽ
    {
	tmpbuf_utf8 = gtk_selection_data_get_text(data);
	എങ്കിൽ (tmpbuf_utf8 != ശൂന്യ)
	{
	    len = STRLEN(tmpbuf_utf8);
	    എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&നിവേശം_conv, tmpbuf_utf8, &len);
		എങ്കിൽ (tmpbuf != ശൂന്യ)
		    text = tmpbuf;
	    }
	    അല്ലെങ്കിൽ
		text = tmpbuf_utf8;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (len >= 2 && text[0] == 0xff && text[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_setup_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    എങ്കിൽ (conv.vc_type != CONV_NONE)
	    {
		text += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, text, &len);
		convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	    }
	    എങ്കിൽ (tmpbuf != ശൂന്യ)
		text = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    ആകുമ്പോൾ (len > 0 && text[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, text, (long)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    സ്ഥിതം ശൂന്യം
selection_get_cb(GtkWidget	    *widget UNUSED,
		 GtkSelectionData   *selection_data,
		 guint		    info,
		 guint		    time_ UNUSED,
		 gpointer	    user_data UNUSED)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    പൂർണ്ണസംഖ്യ		    length;
    പൂർണ്ണസംഖ്യ		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    എങ്കിൽ (gtk_selection_data_get_selection(selection_data)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    അല്ലെങ്കിൽ
	cbd = &clip_star;

    എങ്കിൽ (!cbd->owned)
	തിരികെ;			// Shouldn't ever happen

    എങ്കിൽ (info != (guint)TARGET_STRING
	    && (!clip_html || info != (guint)TARGET_HTML)
	    && info != (guint)TARGET_UTF8_STRING
	    && info != (guint)TARGET_VIMENC
	    && info != (guint)TARGET_VIM
	    && info != (guint)TARGET_VIMENC_MT
	    && info != (guint)TARGET_VIM_MT
	    && info != (guint)TARGET_COMPOUND_TEXT
	    && info != (guint)TARGET_TEXT_PLAIN
	    && info != (guint)TARGET_TEXT_PLAIN_UTF8
	    && info != (guint)TARGET_TEXT)
	തിരികെ;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    എങ്കിൽ (motion_type < 0 || string == ശൂന്യ)
	തിരികെ;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    length = MIN(tmplen, (long_u)(G_MAXINT - 1));

    എങ്കിൽ (info == (guint)TARGET_VIM || info == (guint)TARGET_VIM_MT)
    {
	tmpbuf = alloc(length + 1);
	എങ്കിൽ (tmpbuf != ശൂന്യ)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)length);
	}
	// For our own format, the first byte contains the motion type
	++length;
	vim_free(string);
	string = tmpbuf;
	എങ്കിൽ (info == (guint)TARGET_VIM)
	    type = vim_atom;
	അല്ലെങ്കിൽ
	    type = vim_mt_atom;
    }

    അല്ലെങ്കിൽ എങ്കിൽ (info == (guint)TARGET_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_setup_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	എങ്കിൽ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &length);
	    convert_setup(&conv, ശൂന്യ, ശൂന്യ);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	എങ്കിൽ (string != ശൂന്യ)
	{
	    tmpbuf = alloc(length + 2);
	    എങ്കിൽ (tmpbuf != ശൂന്യ)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)length);
		vim_free(string);
		string = tmpbuf;
		length += 2;
	    }

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_data->type = selection_data->target;
	    selection_data->format = 16;	// 16 bits per char
#endif
	    gtk_selection_data_set(selection_data, html_atom, 16,
							      string, length);
	    vim_free(string);
	}
	തിരികെ;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (info == (guint)TARGET_VIMENC || info == (guint)TARGET_VIMENC_MT)
    {
	പൂർണ്ണസംഖ്യ l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(length + l + 2);
	എങ്കിൽ (tmpbuf != ശൂന്യ)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)length);
	    length += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	എങ്കിൽ (info == (guint)TARGET_VIMENC)
	    type = vimenc_atom;
	അല്ലെങ്കിൽ
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &length);
	    vim_free(string);
	    എങ്കിൽ (tmpbuf == ശൂന്യ)
		തിരികെ;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	എങ്കിൽ (g_utf8_validate((സ്ഥിരം അക്ഷരം *)string, (gssize)length, ശൂന്യ))
	{
	    gtk_selection_data_set_text(selection_data,
					(സ്ഥിരം അക്ഷരം *)string, length);
	}
	vim_free(string);
	തിരികെ;
    }

    എങ്കിൽ (string != ശൂന്യ)
    {
#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_data->type = selection_data->target;
	selection_data->format = 8;	// 8 bits per char
#endif
	gtk_selection_data_set(selection_data, type, 8, string, length);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_early_init_check(പൂർണ്ണസംഖ്യ give_message)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    എങ്കിൽ ((p == ശൂന്യ || *p == NUL) && (q == ശൂന്യ || *q == NUL))
    {
	gui.dying = true;
	എങ്കിൽ (give_message)
	    emsg(_((അക്ഷരം *)e_cannot_open_display));
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init_check(ശൂന്യം)
{
#ifdef USE_GRESOURCE
    സ്ഥിതം പൂർണ്ണസംഖ്യ res_registered = FALSE;

    എങ്കിൽ (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_register_resource();
    }
#endif

#ifdef FEAT_GUI_GNOME
    എങ്കിൽ (gtk_socket_id == 0)
	using_gnome = 1;
#endif

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_set_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    എങ്കിൽ (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((അക്ഷരം *)e_cannot_open_display));
	തിരികെ FAIL;
    }

    തിരികെ OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


സ്ഥിതം guint mouse_click_timer = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ mouse_timed_out = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    സ്ഥിതം timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    // we don't use this information currently
    പൂർണ്ണസംഖ്യ *timed_out = (പൂർണ്ണസംഖ്യ *) data;

    *timed_out = TRUE;
    തിരികെ FALSE;		// don't happen again
}

സ്ഥിതം guint		motion_repeat_timer  = 0;
സ്ഥിതം പൂർണ്ണസംഖ്യ		motion_repeat_offset = FALSE;
സ്ഥിതം timeout_cb_type	motion_repeat_timer_cb(gpointer);

    സ്ഥിതം ശൂന്യം
process_motion_notify(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, GdkModifierType state)
{
    പൂർണ്ണസംഖ്യ	    button;
    int_u   vim_modifiers;
    GtkAllocation allocation;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
		       GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
		       GDK_BUTTON5_MASK))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    എങ്കിൽ (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	തിരികെ;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_allocation(gui.drawarea, &allocation);

    എങ്കിൽ (x < 0 || y < 0
	    || x >= allocation.width
	    || y >= allocation.height)
    {

	പൂർണ്ണസംഖ്യ dx;
	പൂർണ്ണസംഖ്യ dy;
	പൂർണ്ണസംഖ്യ offshoot;
	പൂർണ്ണസംഖ്യ delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - allocation.width;
	dy = y < 0 ? -y : y - allocation.height;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	എങ്കിൽ (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	അല്ലെങ്കിൽ
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	എങ്കിൽ (!motion_repeat_timer)
	    motion_repeat_timer = timeout_add(delay, motion_repeat_timer_cb,
									 ശൂന്യ);
    }
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം GdkDevice *
gui_gtk_get_pointer_device(GtkWidget *widget)
{
    GdkWindow * സ്ഥിരം win = gtk_widget_get_window(widget);
    GdkDisplay * സ്ഥിരം dpy = gdk_window_get_display(win);
# എങ്കിൽ GTK_CHECK_VERSION(3,20,0)
    GdkSeat * സ്ഥിരം seat = gdk_display_get_default_seat(dpy);
    തിരികെ gdk_seat_get_pointer(seat);
# അല്ലെങ്കിൽ
    GdkDeviceManager * സ്ഥിരം mngr = gdk_display_get_device_manager(dpy);
    തിരികെ gdk_device_manager_get_client_pointer(mngr);
# endif
}

    സ്ഥിതം GdkWindow *
gui_gtk_get_pointer(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    GdkWindow * സ്ഥിരം win = gtk_widget_get_window(widget);
    GdkDevice * സ്ഥിരം dev = gui_gtk_get_pointer_device(widget);
    തിരികെ gdk_window_get_device_position(win, dev , x, y, state);
}

# എങ്കിൽ defined(FEAT_GUI_TABLINE)
    സ്ഥിതം GdkWindow *
gui_gtk_window_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * സ്ഥിരം dev = gui_gtk_get_pointer_device(widget);
    തിരികെ gdk_device_get_window_at_position(dev, x, y);
}
# endif
#അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_pointer(wid, x, y, s) \
    gdk_window_get_pointer((wid)->window, x, y, s)
# define gui_gtk_window_at_position(wid, x, y)	gdk_window_at_pointer(x, y)
#endif

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    സ്ഥിതം timeout_cb_type
motion_repeat_timer_cb(gpointer data UNUSED)
{
    പൂർണ്ണസംഖ്യ		    x;
    പൂർണ്ണസംഖ്യ		    y;
    GdkModifierType state;

    gui_gtk_get_pointer(gui.drawarea, &x, &y, &state);

    എങ്കിൽ (!(state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
		   GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
		   GDK_BUTTON5_MASK)))
    {
	motion_repeat_timer = 0;
	തിരികെ FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    എങ്കിൽ (vim_used_in_നിവേശം_buf() > 10)
	തിരികെ TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    എങ്കിൽ (motion_repeat_offset)
	x += gui.char_width;

    motion_repeat_offset = !motion_repeat_offset;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    തിരികെ FALSE;
}

    സ്ഥിതം gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer data UNUSED)
{
    എങ്കിൽ (event->is_hint)
    {
	പൂർണ്ണസംഖ്യ		x;
	പൂർണ്ണസംഖ്യ		y;
	GdkModifierType	state;

	gui_gtk_get_pointer(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    അല്ലെങ്കിൽ
    {
	process_motion_notify((പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
			      (GdkModifierType)event->state);
    }

    തിരികെ TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    സ്ഥിതം gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer data UNUSED)
{
    പൂർണ്ണസംഖ്യ button;
    പൂർണ്ണസംഖ്യ repeated_click = FALSE;
    പൂർണ്ണസംഖ്യ x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

    // Make sure we have focus now we've been selected
    എങ്കിൽ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    എങ്കിൽ (event->type != GDK_BUTTON_PRESS)
	തിരികെ FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    എങ്കിൽ (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_out);

    തിരഞ്ഞെടുപ്പ് (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	അവസ്ഥ 1:
		button = MOUSE_ഇടത്;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_MASK;
		നിർത്തുക;
	അവസ്ഥ 2: button = MOUSE_നടുവ്; നിർത്തുക;
	അവസ്ഥ 3: button = MOUSE_വലത്; നിർത്തുക;
	അവസ്ഥ 8: button = MOUSE_X1; നിർത്തുക;
	അവസ്ഥ 9: button = MOUSE_X2; നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ FALSE;		// Unknown button
    }

#ifdef FEAT_XIM
    // cancel any preediting
    എങ്കിൽ (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    തിരികെ TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    സ്ഥിതം gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer data UNUSED)
{
    പൂർണ്ണസംഖ്യ	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
    സ്ഥിതം ഇരട്ട  acc_x, acc_y;
# എങ്കിൽ !GTK_CHECK_VERSION(3,22,0)
    സ്ഥിതം guint32 last_smooth_event_time;
# endif
#endif

    എങ്കിൽ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    തിരഞ്ഞെടുപ്പ് (event->direction)
    {
	അവസ്ഥ GDK_SCROLL_UP:
	    button = MOUSE_4;
	    നിർത്തുക;
	അവസ്ഥ GDK_SCROLL_DOWN:
	    button = MOUSE_5;
	    നിർത്തുക;
	അവസ്ഥ GDK_SCROLL_ഇടത്:
	    button = MOUSE_7;
	    നിർത്തുക;
	അവസ്ഥ GDK_SCROLL_വലത്:
	    button = MOUSE_6;
	    നിർത്തുക;
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
	അവസ്ഥ GDK_SCROLL_SMOOTH:
	    എങ്കിൽ (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		തിരികെ FALSE;
	    }
# എങ്കിൽ GTK_CHECK_VERSION(3,22,0)
	    എങ്കിൽ (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# അല്ലെങ്കിൽ
	    എങ്കിൽ (event->time - last_smooth_event_time > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# എങ്കിൽ !GTK_CHECK_VERSION(3,22,0)
	    last_smooth_event_time = event->time;
# endif
	    നിർത്തുക;
#endif
	സ്ഥിരസ്ഥിതി: // This shouldn't happen
	    തിരികെ FALSE;
    }

#ifdef FEAT_XIM
    // cancel any preediting
    എങ്കിൽ (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    എങ്കിൽ (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	ആകുമ്പോൾ (acc_x >= 1.0)
	{ //വലത്
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
		    FALSE, vim_modifiers);
	}
	ആകുമ്പോൾ (acc_x <= -1.0)
	{ //ഇടത്
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
		    FALSE, vim_modifiers);
	}
	ആകുമ്പോൾ (acc_y >= 1.0)
	{ // down
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
		    FALSE, vim_modifiers);
	}
	ആകുമ്പോൾ (acc_y <= -1.0)
	{ // up
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
		    FALSE, vim_modifiers);
	}
    }
    അല്ലെങ്കിൽ
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    എങ്കിൽ (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    അല്ലെങ്കിൽ
#endif
	gui_send_mouse_event(button, (പൂർണ്ണസംഖ്യ)event->x, (പൂർണ്ണസംഖ്യ)event->y,
		FALSE, vim_modifiers);

    തിരികെ TRUE;
}


    സ്ഥിതം gint
button_release_event(GtkWidget *widget UNUSED,
		     GdkEventButton *event,
		     gpointer data UNUSED)
{
    പൂർണ്ണസംഖ്യ x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    എങ്കിൽ (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    തിരഞ്ഞെടുപ്പ് (event->button)
    {
	അവസ്ഥ 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    നിർത്തുക;
    }

    തിരികെ TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    സ്ഥിതം ശൂന്യം
grab_notify_event(GtkWidget *widget UNUSED,
		  gboolean  was_grabbed,
		  gpointer  data UNUSED)
{
    എങ്കിൽ (!was_grabbed)
	dragging_button_state = 0;
}


#ifdef FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_and_decode_uri_list(char_u *out, char_u *raw, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*p = out;
    പൂർണ്ണസംഖ്യ		എണ്ണം = 0;

    വേണ്ടി (i = 0; i < len; ++i)
    {
	എങ്കിൽ (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    എങ്കിൽ (p > out && p[-1] != NUL)
	    {
		++എണ്ണം;
		*p++ = NUL;
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	അല്ലെങ്കിൽ
	    *p++ = raw[i];
    }
    എങ്കിൽ (p > out && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++എണ്ണം;
    }
    തിരികെ എണ്ണം;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
filter_uri_list(char_u **outlist, പൂർണ്ണസംഖ്യ max, char_u *src)
{
    പൂർണ്ണസംഖ്യ	i, j;

    വേണ്ടി (i = j = 0; i < max; ++i)
    {
	outlist[i] = ശൂന്യ;
	എങ്കിൽ (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    എങ്കിൽ (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    ആകുമ്പോൾ (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    തിരികെ j;
}

    സ്ഥിതം char_u **
parse_uri_list(പൂർണ്ണസംഖ്യ *എണ്ണം, char_u *data, പൂർണ്ണസംഖ്യ len)
{
    പൂർണ്ണസംഖ്യ	    n	    = 0;
    char_u  *tmp    = ശൂന്യ;
    char_u  **array = ശൂന്യ;

    എങ്കിൽ (data != ശൂന്യ && len > 0 && (tmp = alloc(len + 1)) != ശൂന്യ)
    {
	n = എണ്ണം_and_decode_uri_list(tmp, data, len);
	എങ്കിൽ (n > 0 && (array = ALLOC_MULT(char_u *, n)) != ശൂന്യ)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *എണ്ണം = n;
    തിരികെ array;
}

    സ്ഥിതം ശൂന്യം
drag_handle_uri_list(GdkDragContext	*context,
		     GtkSelectionData	*data,
		     guint		time_,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    പൂർണ്ണസംഖ്യ	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_data_get_data(data),
	    gtk_selection_data_get_length(data));

    എങ്കിൽ (fnames != ശൂന്യ && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_finish(context, TRUE, FALSE, time_); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    അല്ലെങ്കിൽ
	vim_free(fnames);
}

    സ്ഥിതം ശൂന്യം
drag_handle_text(GdkDragContext	    *context,
		 GtkSelectionData   *data,
		 guint		    time_,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *text;
    പൂർണ്ണസംഖ്യ	    len;
    char_u  *tmpbuf = ശൂന്യ;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    എങ്കിൽ (gtk_selection_data_get_data_type(data) == utf8_string_atom)
    {
	എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&നിവേശം_conv, text, &len);
	എങ്കിൽ (tmpbuf != ശൂന്യ)
	    text = tmpbuf;
    }

    dnd_yank_drag_data(text, (long)len);
    gtk_drag_finish(context, TRUE, FALSE, time_); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    എങ്കിൽ (dropkey[2] != 0)
	add_to_നിവേശം_buf(dropkey, (പൂർണ്ണസംഖ്യ)വലുപ്പം(dropkey));
    അല്ലെങ്കിൽ
	add_to_നിവേശം_buf(dropkey + 3, (പൂർണ്ണസംഖ്യ)(വലുപ്പം(dropkey) - 3));
}

/*
 * DND receiver.
 */
    സ്ഥിതം ശൂന്യം
drag_data_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      GtkSelectionData	*data,
		      guint		info,
		      guint		time_,
		      gpointer		user_data UNUSED)
{
    GdkModifierType state;

    // Guard against trash
    സ്ഥിരം guchar * സ്ഥിരം data_data = gtk_selection_data_get_data(data);
    സ്ഥിരം gint data_length = gtk_selection_data_get_length(data);
    സ്ഥിരം gint data_format = gtk_selection_data_get_format(data);

    എങ്കിൽ (data_data == ശൂന്യ
	    || data_length <= 0
	    || data_format != 8
	    || data_data[data_length] != '\0')
    {
	gtk_drag_finish(context, FALSE, FALSE, time_);
	തിരികെ;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_pointer(widget, ശൂന്യ, ശൂന്യ, &state);

    // Not sure about the role of "text/plain" here...
    എങ്കിൽ (info == (guint)TARGET_TEXT_URI_LIST)
	drag_handle_uri_list(context, data, time_, state, x, y);
    അല്ലെങ്കിൽ
	drag_handle_text(context, data, time_, state);

}
#endif // FEAT_DND


#എങ്കിൽ defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    സ്ഥിതം ശൂന്യം
sm_client_check_changed_any(GnomeClient	    *client UNUSED,
			    gint	    താക്കോൽ,
			    GnomeDialogType type UNUSED,
			    gpointer	    data UNUSED)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# എങ്കിൽ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_changed_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    out_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_താക്കോൽ_return(താക്കോൽ, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    സ്ഥിതം gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase UNUSED,
			GnomeSaveStyle	    save_style UNUSED,
			gboolean	    shutdown UNUSED,
			GnomeInteractStyle  interact_style,
			gboolean	    fast UNUSED,
			gpointer	    data UNUSED)
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം	suffix[] = "-session.vim";
    അക്ഷരം		*session_file;
    unsigned പൂർണ്ണസംഖ്യ	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    എങ്കിൽ (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_NORMAL,
					 &sm_client_check_changed_any,
					 ശൂന്യ);
    out_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_path(
			gnome_client_get_config_prefix(client));
    len = strlen(session_file);

    എങ്കിൽ (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(അക്ഷരം, session_file, len + വലുപ്പം(suffix));
    memcpy(session_file + len, suffix, വലുപ്പം(suffix));

    success = write_session_file((char_u *)session_file);

    എങ്കിൽ (success)
    {
	സ്ഥിരം അക്ഷരം  *argv[8];
	പൂർണ്ണസംഖ്യ	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = ശൂന്യ;

	gnome_client_set_discard_command(client, i, (അക്ഷരം **)argv);

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
	argv[i++] = gtk_window_get_role(GTK_WINDOW(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = ശൂന്യ;

	gnome_client_set_restart_command(client, i, (അക്ഷരം **)argv);
	gnome_client_set_clone_command(client, 0, ശൂന്യ);
    }

    g_free(session_file);

    തിരികെ success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    സ്ഥിതം ശൂന്യം
sm_client_die(GnomeClient *client UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    സ്ഥിതം ശൂന്യം
setup_save_yourself(ശൂന്യം)
{
    GnomeClient *client;

    client = gnome_master_client();
    എങ്കിൽ (client == ശൂന്യ)
	തിരികെ;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), ശൂന്യ);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), ശൂന്യ);
}

#അല്ലെങ്കിൽ // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    സ്ഥിതം gboolean
local_xsmp_handle_requests(
    GIOChannel		*source UNUSED,
    GIOCondition	condition,
    gpointer		data)
{
    എങ്കിൽ (condition == G_IO_IN)
    {
	// Do stuff; maybe close connection
	എങ്കിൽ (xsmp_handle_requests() == FAIL)
	    g_io_channel_unref((GIOChannel *)data);
	തിരികെ TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)data);
    xsmp_close();
    തിരികെ TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    സ്ഥിതം ശൂന്യം
setup_save_yourself(ശൂന്യം)
{
    Atom    *existing_atoms = ശൂന്യ;
    പൂർണ്ണസംഖ്യ	    എണ്ണം = 0;

# ifdef USE_XSMP
    എങ്കിൽ (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_add_watch(g_io, G_IO_IN | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_requests, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    അല്ലെങ്കിൽ
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	എങ്കിൽ (!dpy)
	    തിരികെ;

	GdkWindow * സ്ഥിരം mainwin_win = gtk_widget_get_window(gui.mainwin);
	എങ്കിൽ (XGetWMProtocols(dpy, GDK_WINDOW_XID(mainwin_win),
		    &existing_atoms, &എണ്ണം))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    പൂർണ്ണസംഖ്യ	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    വേണ്ടി (i = 0; i < എണ്ണം; ++i)
		എങ്കിൽ (existing_atoms[i] == save_yourself_xatom)
		    നിർത്തുക;

	    എങ്കിൽ (i == എണ്ണം)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, എണ്ണം + 1);
		എങ്കിൽ (new_atoms != ശൂന്യ)
		{
		    memcpy(new_atoms, existing_atoms, എണ്ണം * വലുപ്പം(Atom));
		    new_atoms[എണ്ണം] = save_yourself_xatom;
		    XSetWMProtocols(GDK_WINDOW_XDISPLAY(mainwin_win),
			    GDK_WINDOW_XID(mainwin_win),
			    new_atoms, എണ്ണം + 1);
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
    സ്ഥിതം GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event UNUSED,
		    gpointer data UNUSED)
{
    XEvent *xevent = (XEvent *)xev;

    എങ്കിൽ (xevent != ശൂന്യ
	    && xevent->type == ClientMessage
	    && xevent->xclient.message_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.data.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	out_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin)),
		    GDK_WINDOW_XID(gtk_widget_get_window(gui.mainwin)),
		    ശൂന്യ, 0);
	തിരികെ GDK_FILTER_REMOVE;
    }

    തിരികെ GDK_FILTER_CONTINUE;
}
#endif // !USE_GNOME_SESSION

    സ്ഥിതം GdkPixbuf *
pixbuf_new_from_png_data(സ്ഥിരം unsigned അക്ഷരം *data, unsigned പൂർണ്ണസംഖ്യ len)
{
    GInputStream *stream;
    GdkPixbuf    *pixbuf;

    stream = g_memory_നിവേശം_stream_new_from_data(data, len, ശൂന്യ);
    pixbuf = gdk_pixbuf_new_from_stream(stream, ശൂന്യ, ശൂന്യ);
    g_object_unref(stream);
    തിരികെ pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    സ്ഥിതം ശൂന്യം
mainwin_realize(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#അന്തര്ഭാവ "../runtime/vim16x16_png.h"
#അന്തര്ഭാവ "../runtime/vim32x32_png.h"
#അന്തര്ഭാവ "../runtime/vim48x48_png.h"

    GdkWindow * സ്ഥിരം mainwin_win = gtk_widget_get_window(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    എങ്കിൽ (echo_wid_arg)
    {
	എങ്കിൽ (gui_mch_get_display())
	    printf("WID: %ld\n", (long)GDK_WINDOW_XID(mainwin_win));
	അല്ലെങ്കിൽ
	    printf("WID: 0\n");
	fflush(stdout);
    }

    എങ്കിൽ (vim_strchr(p_go, GO_ICON) != ശൂന്യ)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	എങ്കിൽ (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_window_set_icon_name(GTK_WINDOW(gui.mainwin), "gvim");
	അല്ലെങ്കിൽ
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = ശൂന്യ;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim48x48_png, vim48x48_png_len));

	    gtk_window_set_icon_list(GTK_WINDOW(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(ശൂന്യം *)&g_object_unref, ശൂന്യ);
	    g_list_free(icons);
	}
    }

#എങ്കിൽ !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_window_add_filter(ശൂന്യ, &global_event_filter, ശൂന്യ);
#endif
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#എങ്കിൽ defined(USE_GNOME_SESSION)
    എങ്കിൽ (using_gnome)
#endif
	setup_save_yourself();

#ifdef FEAT_CLIENTSERVER
    എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 && gui_mch_get_display())
    {
	എങ്കിൽ (serverName == ശൂന്യ && serverDelayedStartName != ശൂന്യ)
	{
	    // This is a :gui command in a plain vim with no previous server
	    commWindow = GDK_WINDOW_XID(mainwin_win);

	    (ശൂന്യം)serverRegisterName(GDK_WINDOW_XDISPLAY(mainwin_win),
				    serverDelayedStartName);
	}
	അല്ലെങ്കിൽ
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    serverChangeRegisteredWindow(GDK_WINDOW_XDISPLAY(mainwin_win),
					GDK_WINDOW_XID(mainwin_win));
	}
	gtk_widget_add_events(gui.mainwin, GDK_PROPERTY_CHANGE_MASK);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), ശൂന്യ);
    }
#endif
}

    സ്ഥിതം GdkCursor *
create_blank_pointer(ശൂന്യം)
{
    GdkCursor	*cursor;

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#അല്ലെങ്കിൽ
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    GdkWindow	*വേര്_window = ശൂന്യ;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_mask;
    അക്ഷരം	blank_data[] = { 0x0 };
    വേര്_window = gtk_widget_get_വേര്_window(gui.mainwin);
    blank_mask = gdk_bitmap_create_from_data(വേര്_window, blank_data, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_mask, blank_mask,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_mask);
#endif

    തിരികെ cursor;
}

    സ്ഥിതം ശൂന്യം
mainwin_screen_changed_cb(GtkWidget  *widget,
			  GdkScreen  *previous_screen UNUSED,
			  gpointer   data UNUSED)
{
    എങ്കിൽ (!gtk_widget_has_screen(widget))
	തിരികെ;

    /*
     * Recreate the invisible mouse cursor.
     */
    എങ്കിൽ (gui.blank_pointer != ശൂന്യ)
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_pointer));
#അല്ലെങ്കിൽ
	gdk_cursor_unref(gui.blank_pointer);
#endif

    gui.blank_pointer = create_blank_pointer();

    എങ്കിൽ (gui.pointer_hidden && gtk_widget_get_window(gui.drawarea) != ശൂന്യ)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    എങ്കിൽ (gui.text_context != ശൂന്യ)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(widget);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    എങ്കിൽ (gui.norm_font != ശൂന്യ)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    സ്ഥിതം gboolean
mainwin_state_event_cb(GtkWidget *widget UNUSED,
		       GdkEventWindowState *event,
		       gpointer user_data UNUSED)
{
    എങ്കിൽ (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    തിരികെ FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    സ്ഥിതം ശൂന്യം
drawarea_realize_cb(GtkWidget *widget, gpointer data UNUSED)
{
    GtkWidget *sbar;
    GtkAllocation allocation;

#ifdef FEAT_XIM
    xim_init();
#endif
    gui_mch_new_colors();
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_width(widget),
	    gtk_widget_get_allocated_height(widget));
#അല്ലെങ്കിൽ
    gui.text_gc = gdk_gc_new(gui.drawarea->window);
#endif

    gui.blank_pointer = create_blank_pointer();
    എങ്കിൽ (gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(widget), gui.blank_pointer);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_ഇടത്].id;
    എങ്കിൽ (!sbar || (!gui.which_scrollbars[SBAR_ഇടത്]
				    && firstwin->w_scrollbars[SBAR_വലത്].id))
	sbar = firstwin->w_scrollbars[SBAR_വലത്].id;
    gtk_widget_get_allocation(sbar, &allocation);
    എങ്കിൽ (sbar && gtk_widget_get_realized(sbar) && allocation.width)
	gui.scrollbar_width = allocation.width;

    sbar = gui.bottom_sbar.id;
    എങ്കിൽ (sbar && gtk_widget_get_realized(sbar) && allocation.height)
	gui.scrollbar_height = allocation.height;
}

/*
 * Properly clean up on shutdown.
 */
    സ്ഥിതം ശൂന്യം
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

#ifdef FEAT_XIM
    im_shutdown();
#endif
    എങ്കിൽ (gui.ascii_glyphs != ശൂന്യ)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = ശൂന്യ;
    }
    എങ്കിൽ (gui.ascii_font != ശൂന്യ)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = ശൂന്യ;
    }
    g_object_unref(gui.text_context);
    gui.text_context = ശൂന്യ;

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    എങ്കിൽ (gui.surface != ശൂന്യ)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = ശൂന്യ;
    }
#അല്ലെങ്കിൽ
    g_object_unref(gui.text_gc);
    gui.text_gc = ശൂന്യ;
#endif

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_pointer));
#അല്ലെങ്കിൽ
    gdk_cursor_unref(gui.blank_pointer);
#endif
    gui.blank_pointer = ശൂന്യ;
}

#എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
    സ്ഥിതം ശൂന്യം
drawarea_style_updated_cb(GtkWidget *widget UNUSED,
			 gpointer data UNUSED)
#അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
drawarea_style_set_cb(GtkWidget	*widget UNUSED,
		      GtkStyle	*previous_style UNUSED,
		      gpointer	data UNUSED)
#endif
{
    gui_mch_new_colors();
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       data UNUSED)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ cur_width = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ cur_height = 0;

    g_return_val_if_fail(event
	    && event->width >= 1 && event->height >= 1, TRUE);

# എങ്കിൽ GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
    എങ്കിൽ (event->send_event == FALSE)
	തിരികെ TRUE;
# endif

    എങ്കിൽ (event->width == cur_width && event->height == cur_height)
	തിരികെ TRUE;

    cur_width = event->width;
    cur_height = event->height;

    എങ്കിൽ (gui.surface != ശൂന്യ)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->width, event->height);

    gtk_widget_queue_draw(widget);

    തിരികെ TRUE;
}
#endif

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    സ്ഥിതം gint
delete_event_cb(GtkWidget *widget UNUSED,
		GdkEventAny *event UNUSED,
		gpointer data UNUSED)
{
    gui_shell_closed();
    തിരികെ TRUE;
}

#എങ്കിൽ defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_item_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation item_orientation = GTK_ORIENTATION_HORIZONTAL;

    എങ്കിൽ (using_gnome && widget != ശൂന്യ)
    {
	GtkWidget *parent;
	BonoboDockItem *dockitem;

	parent	 = gtk_widget_get_parent(widget);
	എങ്കിൽ (G_TYPE_FROM_INSTANCE(parent) == BONOBO_TYPE_DOCK_ITEM)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = parent;
	    dockitem = BONOBO_DOCK_ITEM(widget);

	    എങ്കിൽ (dockitem == ശൂന്യ || dockitem->is_floating)
		തിരികെ 0;
	    item_orientation = bonobo_dock_item_get_orientation(dockitem);
	}
    }
# അല്ലെങ്കിൽ
#  define item_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    എങ്കിൽ (widget != ശൂന്യ
	    && item_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0) || !defined(FEAT_GUI_GNOME)
	GtkAllocation allocation;

	gtk_widget_get_allocation(widget, &allocation);
	എങ്കിൽ (allocation.height > 1)
	    തിരികെ allocation.height;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_height(widget, &min_h, &natural_h);
	    തിരികെ MAX(min_h, natural_h);
	}
#  അല്ലെങ്കിൽ
	തിരികെ allocation.height;
#  endif
# അല്ലെങ്കിൽ
	എങ്കിൽ (orientation == GTK_ORIENTATION_HORIZONTAL)
	    തിരികെ widget->allocation.height;
	അല്ലെങ്കിൽ
	    തിരികെ widget->allocation.width;
# endif
    }
    തിരികെ 0;
}
#endif

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_menu_tool_width(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ width = 0;

#ifdef FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    width += get_item_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    width += get_item_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    എങ്കിൽ (gui.tabline != ശൂന്യ)
	width += get_item_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#endif

    തിരികെ width;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_menu_tool_height(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ height = 0;

#ifdef FEAT_MENU
    height += get_item_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#endif
#ifdef FEAT_TOOLBAR
    height += get_item_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#endif
#ifdef FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    എങ്കിൽ (gui.tabline != ശൂന്യ
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	height += get_item_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#endif

    തിരികെ height;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
സ്ഥിതം പൂർണ്ണസംഖ്യ init_window_hints_state = 0;

    സ്ഥിതം ശൂന്യം
update_window_manager_hints(പൂർണ്ണസംഖ്യ force_width, പൂർണ്ണസംഖ്യ force_height)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_width  = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_height = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_min_width  = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_min_height = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_char_width  = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ old_char_height = 0;

    പൂർണ്ണസംഖ്യ width;
    പൂർണ്ണസംഖ്യ height;
    പൂർണ്ണസംഖ്യ min_width;
    പൂർണ്ണസംഖ്യ min_height;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    എങ്കിൽ (!(force_width && force_height)  &&  init_window_hints_state > 0)
    {
	// Don't do it!
	init_window_hints_state = 2;
	തിരികെ;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    width  = gui_get_base_width();
    height = gui_get_base_height();
#ifdef FEAT_MENU
    height += tabline_height() * gui.char_height;
#endif
    width  += get_menu_tool_width();
    height += get_menu_tool_height();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    എങ്കിൽ (force_width && force_height)
    {
	min_width  = force_width;
	min_height = force_height;
    }
    അല്ലെങ്കിൽ
    {
	min_width  = width  + MIN_COLUMNS * gui.char_width;
	min_height = height + MIN_വരികൾ   * gui.char_height;
    }

    // Avoid an expose event when the size didn't change.
    എങ്കിൽ (width != old_width
	    || height != old_height
	    || min_width != old_min_width
	    || min_height != old_min_height
	    || gui.char_width != old_char_width
	    || gui.char_height != old_char_height)
    {
	GdkGeometry	geometry;
	GdkWindowHints	geometry_mask;

	geometry.width_inc   = gui.char_width;
	geometry.height_inc  = gui.char_height;
	geometry.base_width  = width;
	geometry.base_height = height;
	geometry.min_width   = min_width;
	geometry.min_height  = min_height;
	geometry_mask	     = GDK_HINT_BASE_SIZE|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_SIZE;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_window_set_geometry_hints(GTK_WINDOW(gui.mainwin), ശൂന്യ,
				      &geometry, geometry_mask);
	old_width       = width;
	old_height      = height;
	old_min_width   = min_width;
	old_min_height  = min_height;
	old_char_width  = gui.char_width;
	old_char_height = gui.char_height;
    }
}

#എങ്കിൽ defined(FEAT_GUI_DARKTHEME)
    ശൂന്യം
gui_mch_set_dark_theme(പൂർണ്ണസംഖ്യ dark)
{
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_set(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, ശൂന്യ);
# endif
}
#endif // FEAT_GUI_DARKTHEME

#ifdef FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    സ്ഥിതം ശൂന്യം
icon_size_changed_foreach(GtkWidget *widget, gpointer user_data)
{
    എങ്കിൽ (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
	എങ്കിൽ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    സ്ഥിരം GtkIconSize icon_size = GPOINTER_TO_INT(user_data);
	    സ്ഥിരം gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, ശൂന്യ);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_size);
	}
# അല്ലെങ്കിൽ
	// User-defined icons are stored in a GtkIconSet
	എങ്കിൽ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_SET)
	{
	    GtkIconSet	*icon_set;
	    GtkIconSize	icon_size;

	    gtk_image_get_icon_set(image, &icon_set, &icon_size);
	    icon_size = (GtkIconSize)(long)user_data;

	    gtk_icon_set_ref(icon_set);
	    gtk_image_set_from_icon_set(image, icon_set, icon_size);
	    gtk_icon_set_unref(icon_set);
	}
# endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_size_changed_foreach,
			      user_data);
    }
}

    സ്ഥിതം ശൂന്യം
set_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    GtkIconSize	    size;
    GtkIconSize	    oldsize;

    എങ്കിൽ ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    അല്ലെങ്കിൽ എങ്കിൽ ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    അല്ലെങ്കിൽ എങ്കിൽ (toolbar_flags & TOOLBAR_TEXT)
	style = GTK_TOOLBAR_TEXT;
    അല്ലെങ്കിൽ
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_set_style(toolbar, style);
# എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    gtk_toolbar_set_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    തിരഞ്ഞെടുപ്പ് (tbis_flags)
    {
	അവസ്ഥ TBIS_TINY:	    size = GTK_ICON_SIZE_MENU;		നിർത്തുക;
	അവസ്ഥ TBIS_SMALL:    size = GTK_ICON_SIZE_SMALL_TOOLBAR;	നിർത്തുക;
	അവസ്ഥ TBIS_MEDIUM:   size = GTK_ICON_SIZE_BUTTON;	നിർത്തുക;
	അവസ്ഥ TBIS_LARGE:    size = GTK_ICON_SIZE_LARGE_TOOLBAR;	നിർത്തുക;
	അവസ്ഥ TBIS_HUGE:     size = GTK_ICON_SIZE_DND;		നിർത്തുക;
	അവസ്ഥ TBIS_GIANT:    size = GTK_ICON_SIZE_DIALOG;	നിർത്തുക;
	സ്ഥിരസ്ഥിതി:	    size = GTK_ICON_SIZE_INVALID;	നിർത്തുക;
    }
    oldsize = gtk_toolbar_get_icon_size(toolbar);

    എങ്കിൽ (size == GTK_ICON_SIZE_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_size(toolbar);
	size = gtk_toolbar_get_icon_size(toolbar);
    }
    എങ്കിൽ (size != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_size_changed_foreach,
			      GINT_TO_POINTER((പൂർണ്ണസംഖ്യ)size));
    }
    gtk_toolbar_set_icon_size(toolbar, size);
}

#endif // FEAT_TOOLBAR

#എങ്കിൽ defined(FEAT_GUI_TABLINE)
സ്ഥിതം പൂർണ്ണസംഖ്യ ignore_tabline_evt = FALSE;
സ്ഥിതം GtkWidget *tabline_menu;
# എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
സ്ഥിതം GtkTooltips *tabline_tooltip;
# endif
സ്ഥിതം പൂർണ്ണസംഖ്യ clicked_page;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    സ്ഥിതം ശൂന്യം
tabline_menu_handler(GtkMenuItem *item UNUSED, gpointer user_data)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_page, (പൂർണ്ണസംഖ്യ)(long)user_data);
}

    സ്ഥിതം ശൂന്യം
add_tabline_menu_item(GtkWidget *menu, char_u *text, പൂർണ്ണസംഖ്യ resp)
{
    GtkWidget	*item;
    char_u	*utf_text;

    utf_text = CONVERT_TO_UTF8(text);
    item = gtk_menu_item_new_with_label((സ്ഥിരം അക്ഷരം *)utf_text);
    gtk_widget_show(item);
    CONVERT_TO_UTF8_FREE(utf_text);

    gtk_container_add(GTK_CONTAINER(menu), item);
    g_signal_connect(G_OBJECT(item), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_TO_POINTER(resp));
}

/*
 * Create a menu for the tab line.
 */
    സ്ഥിതം GtkWidget *
create_tabline_menu(ശൂന്യം)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    add_tabline_menu_item(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    തിരികെ menu;
}

    സ്ഥിതം gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    എങ്കിൽ (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	പൂർണ്ണസംഖ്യ		x = bevent->x;
	പൂർണ്ണസംഖ്യ		y = bevent->y;
	GtkWidget	*tabwidget;
	GdkWindow	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	എങ്കിൽ (hold_gui_events || cmdwin_type != 0)
	    തിരികെ TRUE;

	tabwin = gui_gtk_window_at_position(gui.mainwin, &x, &y);

	gdk_window_get_user_data(tabwin, (gpointer)&tabwidget);
	clicked_page = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	എങ്കിൽ (bevent->button == 3)
	{
# എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    എങ്കിൽ (gui.is_wayland)
	    {
		പൂർണ്ണസംഖ്യ x2, y2;
		gui_gtk_get_pointer(gui.mainwin, &x2, &y2, ശൂന്യ);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_window(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, ശൂന്യ);
	    }
	    അല്ലെങ്കിൽ
#  endif
	    gtk_menu_popup_at_pointer(GTK_MENU(widget), event);
# അല്ലെങ്കിൽ
	    gtk_menu_popup(GTK_MENU(widget), ശൂന്യ, ശൂന്യ, ശൂന്യ, ശൂന്യ,
						bevent->button, bevent->time);
# endif
	    // We handled the event.
	    തിരികെ TRUE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (bevent->button == 1)
	{
	    എങ്കിൽ (clicked_page == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (bevent->button == 2)
	{
	    എങ്കിൽ (clicked_page != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_page, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    തിരികെ FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    സ്ഥിതം ശൂന്യം
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    എങ്കിൽ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# എങ്കിൽ GTK_CHECK_VERSION(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    സ്ഥിതം ശൂന്യം
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    എങ്കിൽ (ignore_tabline_evt)
	തിരികെ;

    എങ്കിൽ ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    അല്ലെങ്കിൽ
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    ശൂന്യം
gui_mch_show_tabline(പൂർണ്ണസംഖ്യ showit)
{
    എങ്കിൽ (gui.tabline == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    പൂർണ്ണസംഖ്യ w, h;

	    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_window_manager_hints(0, 0);
	എങ്കിൽ (showit)
	    gtk_widget_set_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_showing_tabline(ശൂന്യം)
{
    തിരികെ gui.tabline != ശൂന്യ
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    ശൂന്യം
gui_mch_update_tabline(ശൂന്യം)
{
    GtkWidget	    *page;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    പൂർണ്ണസംഖ്യ		    nr = 0;
    പൂർണ്ണസംഖ്യ		    tab_അക്കം;
    പൂർണ്ണസംഖ്യ		    curtabidx = 0;
    char_u	    *labeltext;

    എങ്കിൽ (gui.tabline == ശൂന്യ)
	തിരികെ;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    വേണ്ടി (tp = first_tabpage; tp != ശൂന്യ; tp = tp->tp_next, ++nr)
    {
	എങ്കിൽ (tp == curtab)
	    curtabidx = nr;

	tab_അക്കം = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	എങ്കിൽ (page == ശൂന്യ)
	{
	    // Add notebook page
# എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# അല്ലെങ്കിൽ
	    page = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(page);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# എങ്കിൽ !GTK_CHECK_VERSION(3,14,0)
	    gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_add(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_തിരുകുക_page(GTK_NOTEBOOK(gui.tabline),
		    page,
		    event_box,
		    nr);
# എങ്കിൽ GTK_CHECK_VERSION(2,10,0)
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	g_object_set_data(G_OBJECT(event_box), "tab_num",
						     GINT_TO_POINTER(tab_അക്കം));
	label = gtk_bin_get_child(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_label_set_text(GTK_LABEL(label), (സ്ഥിരം അക്ഷരം *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	gtk_widget_set_tooltip_text(event_box, (സ്ഥിരം gchar *)labeltext);
# അല്ലെങ്കിൽ
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (സ്ഥിരം അക്ഷരം *)labeltext, ശൂന്യ);
# endif
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    ആകുമ്പോൾ (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != ശൂന്യ)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    എങ്കിൽ (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ശൂന്യം
gui_mch_set_curtab(പൂർണ്ണസംഖ്യ nr)
{
    എങ്കിൽ (gui.tabline == ശൂന്യ)
	തിരികെ;

    ignore_tabline_evt = TRUE;
    എങ്കിൽ (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#endif // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    ശൂന്യം
gui_gtk_set_selection_targets(GdkAtom selection)
{
    പൂർണ്ണസംഖ്യ		    i, j = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    n_targets = N_SELECTION_TARGETS;
    സ്ഥിതം GtkTargetEntry  targets[N_SELECTION_TARGETS];

    എങ്കിൽ (targets[0].target == ശൂന്യ)
    {
	വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    എങ്കിൽ (!clip_html && selection_targets[i].info == TARGET_HTML)
		n_targets--;
	    അല്ലെങ്കിൽ
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_add_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    ശൂന്യം
gui_gtk_set_dnd_targets(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		    i, j = 0;
    പൂർണ്ണസംഖ്യ		    n_targets = N_DND_TARGETS;
    GtkTargetEntry  targets[N_DND_TARGETS];

    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)N_DND_TARGETS; ++i)
    {
	എങ്കിൽ (!clip_html && dnd_targets[i].info == TARGET_HTML)
	    n_targets--;
	അല്ലെങ്കിൽ
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_set(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init(ശൂന്യം)
{
    GtkWidget *vbox;

#ifdef FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    എങ്കിൽ (using_gnome)
    {
	gnome_program_init(VIMPACKAGE, VIM_VERSION_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, ശൂന്യ);
# എങ്കിൽ defined(LC_NUMERIC)
	{
	    അക്ഷരം *p = setlocale(LC_NUMERIC, ശൂന്യ);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    എങ്കിൽ (p == ശൂന്യ || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#endif
    VIM_CLEAR(gui_argv);

#എങ്കിൽ GLIB_CHECK_VERSION(2,1,3)
    // Set the human-readable application name
    g_set_application_name("Vim");
#endif
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    set_option_മൂല്യം_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#ifdef FEAT_TOOLBAR
    gui_gtk_register_stock_icons();
#endif
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
# എങ്കിൽ 0
    gtk_rc_parse("gtkrc");
# endif
#endif

    // Initialize values
    gui.border_width = 2;
    gui.scrollbar_width = SB_DEFAULT_WIDTH;
    gui.scrollbar_height = SB_DEFAULT_WIDTH;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#അല്ലെങ്കിൽ
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#endif

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.back_pixel = gui.def_back_pixel;

    എങ്കിൽ (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	എങ്കിൽ (plug != ശൂന്യ && gtk_plug_get_socket_window(GTK_PLUG(plug)) != ശൂന്യ)
	{
	    gui.mainwin = plug;
	}
	അല്ലെങ്കിൽ
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (unsigned പൂർണ്ണസംഖ്യ)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    എങ്കിൽ (gtk_socket_id == 0)
    {
#ifdef FEAT_GUI_GNOME
	എങ്കിൽ (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", ശൂന്യ);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	അല്ലെങ്കിൽ
#endif
	    gui.mainwin = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    }

    gtk_widget_set_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.text_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    gtk_container_set_border_width(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_add_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_MASK);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), ശൂന്യ);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), ശൂന്യ);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_changed_cb), ശൂന്യ);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), ശൂന്യ);

    gui.accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(gui.mainwin), gui.accel_group);

    // A vertical box holds the menubar, toolbar and main text window.
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#അല്ലെങ്കിൽ
    vbox = gtk_vbox_new(FALSE, 0);
#endif

#ifdef FEAT_GUI_GNOME
    എങ്കിൽ (using_gnome)
    {
# എങ്കിൽ defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_set_contents(GNOME_APP(gui.mainwin), vbox);
    }
    അല്ലെങ്കിൽ
#endif
    {
	gtk_container_add(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#ifdef FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_set_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_set(gtk_settings, "gtk-menu-bar-accel", ശൂന്യ, ശൂന്യ);
    }


# ifdef FEAT_GUI_GNOME
    എങ്കിൽ (using_gnome)
    {
	BonoboDockItem *dockitem;

	gnome_app_set_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_item_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_item_set_behavior(dockitem,
		bonobo_dock_item_get_behavior(dockitem)
				       | BONOBO_DOCK_ITEM_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    അല്ലെങ്കിൽ
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_start(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#endif	// FEAT_MENU

#ifdef FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# അല്ലെങ്കിൽ
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_set_name(gui.toolbar, "vim-toolbar");
    set_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    എങ്കിൽ (using_gnome)
    {
	BonoboDockItem *dockitem;

	gnome_app_set_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_item_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_item_set_behavior(dockitem,
		bonobo_dock_item_get_behavior(dockitem)
				       | BONOBO_DOCK_ITEM_BEH_NEVER_FLOATING);
	gtk_container_set_border_width(GTK_CONTAINER(gui.toolbar), 0);
    }
    അല്ലെങ്കിൽ
# endif	// FEAT_GUI_GNOME
    {
	എങ്കിൽ (vim_strchr(p_go, GO_TOOLBAR) != ശൂന്യ
		&& (toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_start(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#endif // FEAT_TOOLBAR

#ifdef FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_start(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_set_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_set_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    gtk_notebook_set_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *page, *label, *event_box;

	// Add the first tab.
# എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# അല്ലെങ്കിൽ
	page = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(page);
	gtk_container_add(GTK_CONTAINER(gui.tabline), page);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_set_data(G_OBJECT(event_box), "tab_num", GINT_TO_POINTER(1L));
# എങ്കിൽ !GTK_CHECK_VERSION(3,14,0)
	gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(gui.tabline), page, event_box);
# എങ്കിൽ GTK_CHECK_VERSION(2,10,0)
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline), page, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ശൂന്യ);
# എങ്കിൽ GTK_CHECK_VERSION(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ശൂന്യ);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#endif // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_set_border_width(GTK_CONTAINER(gui.formwin), 0);
#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_events(gui.formwin, GDK_EXPOSURE_MASK);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
    gtk_widget_set_name(gui.formwin, "vim-gtk-form");
#endif

    gui.drawarea = gtk_drawing_area_new();
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    gui.surface = ശൂന്യ;
#endif

#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    എങ്കിൽ (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#endif

    // Determine which events we will filter.
    gint event_mask =
	GDK_EXPOSURE_MASK |
	GDK_ENTER_NOTIFY_MASK |
	GDK_LEAVE_NOTIFY_MASK |
	GDK_BUTTON_PRESS_MASK |
	GDK_BUTTON_RELEASE_MASK |
	GDK_താക്കോൽ_PRESS_MASK |
	GDK_താക്കോൽ_RELEASE_MASK |
	GDK_POINTER_MOTION_MASK |
	GDK_POINTER_MOTION_HINT_MASK;
#എങ്കിൽ GTK_CHECK_VERSION(3,4,0)
    എങ്കിൽ (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_mask |= GDK_SCROLL_MASK;
    }
    അല്ലെങ്കിൽ
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_mask |= GDK_SMOOTH_SCROLL_MASK;
    }
#അല്ലെങ്കിൽ
    event_mask |= GDK_SCROLL_MASK;
#endif
    gtk_widget_set_events(gui.drawarea, event_mask);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_start(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(താക്കോൽ_press_event), ശൂന്യ);
#എങ്കിൽ defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&താക്കോൽ_release_event), ശൂന്യ);
#endif
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ശൂന്യ);
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), ശൂന്യ);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), ശൂന്യ);
#അല്ലെങ്കിൽ
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_set_cb), ശൂന്യ);
#endif

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#endif

#എങ്കിൽ !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#endif

    എങ്കിൽ (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_set_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_star.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_offset = gui.border_width;

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), ശൂന്യ);
#അല്ലെങ്കിൽ
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), ശൂന്യ);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), ശൂന്യ);
#endif

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    എങ്കിൽ (vim_strchr(p_go, GO_POINTER) != ശൂന്യ)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), ശൂന്യ);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), ശൂന്യ);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    എങ്കിൽ (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_out_event), ശൂന്യ);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_in_event), ശൂന്യ);
    }
    അല്ലെങ്കിൽ
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_out_event), ശൂന്യ);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_in_event), ശൂന്യ);
#ifdef FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_out_event), ശൂന്യ);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_in_event), ശൂന്യ);
#endif // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), ശൂന്യ);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.in_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_changed_cb), ശൂന്യ);
    }

#എങ്കിൽ defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(gui.formwin);
#endif

    തിരികെ OK;
}

#എങ്കിൽ defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    ശൂന്യം
gui_mch_forked(ശൂന്യം)
{
    എങ്കിൽ (!using_gnome)
	തിരികെ;

    GnomeClient *client;

    client = gnome_master_client();

    എങ്കിൽ (client != ശൂന്യ)
	gnome_client_set_process_id(client, getpid());
}
#endif // USE_GNOME_SESSION

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    തിരികെ rgba;
}

    സ്ഥിതം ശൂന്യം
set_cairo_source_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    സ്ഥിരം GdkRGBA rgba = color_to_rgba(color);
    cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#endif // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    ശൂന്യം
gui_mch_new_colors(ശൂന്യം)
{
    എങ്കിൽ (gui.drawarea != ശൂന്യ
#എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
	    && gui.formwin != ശൂന്യ
#endif
	    && gtk_widget_get_window(gui.drawarea) != ശൂന്യ)
    {
#എങ്കിൽ !GTK_CHECK_VERSION(3,22,2)
	GdkWindow * സ്ഥിരം da_win = gtk_widget_get_window(gui.drawarea);
#endif
#എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
	GtkStyleContext * സ്ഥിരം context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * സ്ഥിരം provider = gtk_css_provider_new();
	gchar * സ്ഥിരം css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.back_pixel >> 16) & 0xff,
		 (gui.back_pixel >> 8) & 0xff,
		 gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, ശൂന്യ);
	gtk_style_context_add_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#അല്ലെങ്കിലെങ്കിൽ GTK_CHECK_VERSION(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_to_rgba(gui.back_pixel);
	{
	    cairo_pattern_t * സ്ഥിരം pat = cairo_pattern_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    എങ്കിൽ (pat != ശൂന്യ)
	    {
		gdk_window_set_background_pattern(da_win, pat);
		cairo_pattern_destroy(pat);
	    }
	    അല്ലെങ്കിൽ
		gdk_window_set_background_rgba(da_win, &rgba);
	}
#അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.back_pixel;
	gdk_window_set_background(da_win, &color);
#endif // !GTK_CHECK_VERSION(3,22,2)
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
    സ്ഥിതം gboolean
startup_resize_correction_cb(gpointer data UNUSED)
{
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    തിരികെ FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    സ്ഥിതം gint
form_configure_event(GtkWidget *widget UNUSED,
		     GdkEventConfigure *event,
		     gpointer data UNUSED)
{
    പൂർണ്ണസംഖ്യ	    usable_height = event->height;
#ifdef TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    പൂർണ്ണസംഖ്യ	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ശൂന്യ, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (പൂർണ്ണസംഖ്യ)Columns, (പൂർണ്ണസംഖ്യ)Rows);
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

    എങ്കിൽ (!MATCH_WIDTH_HEIGHT(latest_resize_hist, w, h)
					     && match_stale_width_height(w, h))
	// discard stale event
	തിരികെ TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    എങ്കിൽ ((mch_csd_height == 0 || mch_csd_width == 0)
	    && latest_resize_hist != ശൂന്യ
	    && !latest_resize_hist->used
	    && latest_resize_hist->from_shellsize
	    && gui.char_height > 0)
    {
	പൂർണ്ണസംഖ്യ pot_csd_w = latest_resize_hist->width - w;
	പൂർണ്ണസംഖ്യ pot_csd_h = latest_resize_hist->height - h;

	എങ്കിൽ (pot_csd_w > 0 && pot_csd_w < gui.char_width)
	{
	    mch_csd_width = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (പൂർണ്ണസംഖ്യ)Columns * gui.char_width
						    + gui_get_base_width();
	}
	എങ്കിൽ (pot_csd_h > 0 && pot_csd_h < gui.char_height)
	{
	    mch_csd_height = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_height += mch_csd_height;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	എങ്കിൽ ((mch_csd_height > 0 || mch_csd_width > 0) && gtk_socket_id == 0)
	    g_idle_add_full(GTK_PRIORITY_RESIZE, startup_resize_correction_cb, ശൂന്യ, ശൂന്യ);
    }
    clear_resize_hists();
#endif

#എങ്കിൽ GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
    എങ്കിൽ (event->window != gtk_widget_get_window(gui.formwin))
	തിരികെ TRUE;
#endif

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    എങ്കിൽ (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	പൂർണ്ണസംഖ്യ use_width = event->width;

#ifdef TRACK_RESIZE_HISTORY
	എങ്കിൽ (mch_pending_form_w > event->width
		&& mch_pending_form_w - event->width < gui.char_width)
	    use_width = mch_pending_form_w;
#endif
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_width, usable_height);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    തിരികെ TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    സ്ഥിതം ശൂന്യം
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

    gui.mainwin  = ശൂന്യ;
    gui.drawarea = ശൂന്യ;

    എങ്കിൽ (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#ifdef USE_GRESOURCE
    gui_gtk_unregister_resource();
#endif
}

    ശൂന്യം
gui_gtk_get_screen_geom_of_win(
	GtkWidget *wid,
	പൂർണ്ണസംഖ്യ point_x,	    // x position of window if not initialized
	പൂർണ്ണസംഖ്യ point_y,	    // y position of window if not initialized
	പൂർണ്ണസംഖ്യ *screen_x,
	പൂർണ്ണസംഖ്യ *screen_y,
	പൂർണ്ണസംഖ്യ *width,
	പൂർണ്ണസംഖ്യ *height)
{
    GdkRectangle geometry;
    GdkWindow *win = gtk_widget_get_window(wid);
#എങ്കിൽ GTK_CHECK_VERSION(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    എങ്കിൽ (wid != ശൂന്യ && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    അല്ലെങ്കിൽ
	dpy = gdk_display_get_default();
    എങ്കിൽ (win != ശൂന്യ)
	monitor = gdk_display_get_monitor_at_window(dpy, win);
    അല്ലെങ്കിൽ
	monitor = gdk_display_get_monitor_at_point(dpy, point_x, point_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#അല്ലെങ്കിൽ
    GdkScreen* screen;
    പൂർണ്ണസംഖ്യ monitor;

    എങ്കിൽ (wid != ശൂന്യ && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    അല്ലെങ്കിൽ
	screen = gdk_screen_get_default();
    എങ്കിൽ (win != ശൂന്യ)
	monitor = gdk_screen_get_monitor_at_window(screen, win);
    അല്ലെങ്കിൽ
	monitor = gdk_screen_get_monitor_at_point(screen, point_x, point_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#endif
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *width = geometry.width;
    *height = geometry.height;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    സ്ഥിതം ശൂന്യം
gui_gtk_get_screen_dimensions(
	പൂർണ്ണസംഖ്യ point_x,
	പൂർണ്ണസംഖ്യ point_y,
	പൂർണ്ണസംഖ്യ *screen_w,
	പൂർണ്ണസംഖ്യ *screen_h)
{
    പൂർണ്ണസംഖ്യ	    x, y;

    gui_gtk_get_screen_geom_of_win(gui.mainwin, point_x, point_y,
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
    *screen_w -= get_menu_tool_width();
    *screen_h -= get_menu_tool_height();
}

    ശൂന്യം
gui_mch_get_screen_dimensions(പൂർണ്ണസംഖ്യ *screen_w, പൂർണ്ണസംഖ്യ *screen_h)
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
    സ്ഥിതം timeout_cb_type
check_startup_plug_hints(gpointer data UNUSED)
{
    എങ്കിൽ (init_window_hints_state == 1)
    {
	// Safe to use normal hints now
	init_window_hints_state = 0;
	update_window_manager_hints(0, 0);
	തിരികെ FALSE;   // stop timer
    }

    // Keep on trying
    init_window_hints_state = 1;
    തിരികെ TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    പൂർണ്ണസംഖ്യ
gui_mch_open(ശൂന്യം)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_width;
    guint		pixel_height;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    എങ്കിൽ (role_argument != ശൂന്യ)
    {
	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role_argument);
    }
    അല്ലെങ്കിൽ
    {
	അക്ഷരം *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (unsigned)mch_get_pid(),
			       (unsigned)g_random_int(),
			       (unsigned)time(ശൂന്യ));

	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role);
	g_free(role);
    }

    എങ്കിൽ (gui_win_x != -1 && gui_win_y != -1)
	gtk_window_move(GTK_WINDOW(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    എങ്കിൽ (gui.geom != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ		mask;
	unsigned പൂർണ്ണസംഖ്യ	w, h;
	പൂർണ്ണസംഖ്യ		x = 0;
	പൂർണ്ണസംഖ്യ		y = 0;

	mask = XParseGeometry((അക്ഷരം *)gui.geom, &x, &y, &w, &h);

	എങ്കിൽ (mask & Width_മൂല്യം)
	    Columns = w;
	എങ്കിൽ (mask & Height_മൂല്യം)
	{
	    എങ്കിൽ (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
	pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

	pixel_width  += get_menu_tool_width();
	pixel_height += get_menu_tool_height();

	എങ്കിൽ (mask & (XValue | YValue))
	{
	    പൂർണ്ണസംഖ്യ ww, hh;

#ifdef FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#അല്ലെങ്കിൽ
	    gui_mch_get_screen_dimensions(&ww, &hh);
#endif
	    hh += p_ghr + get_menu_tool_height();
	    ww += get_menu_tool_width();
	    എങ്കിൽ (mask & XNegative)
		x += ww - pixel_width;
	    എങ്കിൽ (mask & YNegative)
		y += hh - pixel_height;
	    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	എങ്കിൽ (gtk_socket_id != 0  &&  (mask & Width_മൂല്യം || mask & Height_മൂല്യം))
	{
	    update_window_manager_hints(pixel_width, pixel_height);
	    init_window_hints_state = 1;
	    timeout_add(1000, check_startup_plug_hints, ശൂന്യ);
	}
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    എങ്കിൽ (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), pixel_width, pixel_height);
    update_window_manager_hints(0, 0);

    എങ്കിൽ (foreground_argument != ശൂന്യ)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    എങ്കിൽ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    എങ്കിൽ (background_argument != ശൂന്യ)
	bg_pixel = gui_get_color((char_u *)background_argument);
    എങ്കിൽ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    എങ്കിൽ (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    അല്ലെങ്കിൽ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_back_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    set_normal_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ശൂന്യ);

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
#ifdef TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#endif
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), ശൂന്യ);
#എങ്കിൽ GTK_CHECK_VERSION(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_factor_event), ശൂന്യ);
#endif

#ifdef FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_set_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_data_received_cb), ശൂന്യ);
#endif

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	എങ്കിൽ (found_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#എങ്കിൽ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	unsigned long menu_handler = 0;
# ifdef FEAT_TOOLBAR
	unsigned long tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	എങ്കിൽ (using_gnome && vim_strchr(p_go, GO_MENUS) == ശൂന്യ)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ശൂന്യ);
# ifdef FEAT_TOOLBAR
	എങ്കിൽ (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == ശൂന്യ
		&& (toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ശൂന്യ);
# endif
#endif
	gtk_widget_show(gui.mainwin);

#എങ്കിൽ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	എങ്കിൽ (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	എങ്കിൽ (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#endif
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), ശൂന്യ);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), ശൂന്യ);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), ശൂന്യ);

    തിരികെ OK;
}

/*
 * Clean up for when exiting Vim.
 */
    ശൂന്യം
gui_mch_exit(പൂർണ്ണസംഖ്യ rc UNUSED)
{
    // Clean up, unless we don't want to invoke free().
    എങ്കിൽ (gui.mainwin != ശൂന്യ && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_get_winpos(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    എങ്കിൽ (gui_mch_get_display())
    {
	gtk_window_get_position(GTK_WINDOW(gui.mainwin), x, y);
	തിരികെ OK;
    }
    തിരികെ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ശൂന്യം
gui_mch_set_winpos(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
}

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
# എങ്കിൽ 0
സ്ഥിതം പൂർണ്ണസംഖ്യ resize_idle_installed = FALSE;
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
    സ്ഥിതം gboolean
force_shell_resize_idle(gpointer data)
{
    എങ്കിൽ (gui.mainwin != ശൂന്യ
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	പൂർണ്ണസംഖ്യ width;
	പൂർണ്ണസംഖ്യ height;

	gtk_window_get_size(GTK_WINDOW(gui.mainwin), &width, &height);

	width  -= get_menu_tool_width();
	height -= get_menu_tool_height();

	gui_resize_shell(width, height);
    }

    resize_idle_installed = FALSE;
    തിരികെ FALSE; // don't call me again
}
# endif
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_maximized(ശൂന്യം)
{
    തിരികെ (gui.mainwin != ശൂന്യ && gtk_widget_get_window(gui.mainwin) != ശൂന്യ
	    && (gdk_window_get_state(gtk_widget_get_window(gui.mainwin))
					       & GDK_WINDOW_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    ശൂന്യം
gui_mch_unmaximize(ശൂന്യം)
{
    എങ്കിൽ (gui.mainwin != ശൂന്യ)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ശൂന്യം
gui_mch_newfont(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ w, h;

    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    w -= get_menu_tool_width();
    h -= get_menu_tool_height();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    ശൂന്യം
gui_mch_set_shellsize(പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height,
		      പൂർണ്ണസംഖ്യ min_width UNUSED,  പൂർണ്ണസംഖ്യ min_height UNUSED,
		      പൂർണ്ണസംഖ്യ base_width UNUSED, പൂർണ്ണസംഖ്യ base_height UNUSED,
		      പൂർണ്ണസംഖ്യ direction UNUSED)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    എങ്കിൽ (gtk_socket_id == 0)
	update_window_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    width  += get_menu_tool_width();
    height += get_menu_tool_height();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    width  += mch_csd_width;
    height += mch_csd_height;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_width > 0)
			    ? width - get_menu_tool_width() - mch_csd_width : 0;

#ifdef TRACK_RESIZE_HISTORY
    alloc_resize_hist(width, height, TRUE);
#endif
    എങ്കിൽ (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), width, height);
    അല്ലെങ്കിൽ
	update_window_manager_hints(width, height);

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
# എങ്കിൽ 0
    എങ്കിൽ (!resize_idle_installed)
    {
	g_idle_add_full(GDK_PRIORITY_EVENTS + 10,
			&force_shell_resize_idle, ശൂന്യ, ശൂന്യ);
	resize_idle_installed = TRUE;
    }
# endif
#endif // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    ശൂന്യം
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    എങ്കിൽ (title != ശൂന്യ && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, ശൂന്യ);

    gtk_window_set_title(GTK_WINDOW(gui.mainwin), (സ്ഥിരം അക്ഷരം *)title);

    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#എങ്കിൽ defined(FEAT_MENU)
    ശൂന്യം
gui_mch_enable_menu(പൂർണ്ണസംഖ്യ showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    എങ്കിൽ (using_gnome)
	widget = gui.menubar_h;
    അല്ലെങ്കിൽ
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    എങ്കിൽ (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	എങ്കിൽ (showit)
	    gtk_widget_show(widget);
	അല്ലെങ്കിൽ
	    gtk_widget_hide(widget);

	update_window_manager_hints(0, 0);
    }
}
#endif // FEAT_MENU

#എങ്കിൽ defined(FEAT_TOOLBAR)
    ശൂന്യം
gui_mch_show_toolbar(പൂർണ്ണസംഖ്യ showit)
{
    GtkWidget *widget;

    എങ്കിൽ (gui.toolbar == ശൂന്യ)
	തിരികെ;

# ifdef FEAT_GUI_GNOME
    എങ്കിൽ (using_gnome)
	widget = gui.toolbar_h;
    അല്ലെങ്കിൽ
# endif
	widget = gui.toolbar;

    എങ്കിൽ (showit)
	set_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    എങ്കിൽ (!showit != !gtk_widget_get_visible(widget))
    {
	എങ്കിൽ (showit)
	    gtk_widget_show(widget);
	അല്ലെങ്കിൽ
	    gtk_widget_hide(widget);

	update_window_manager_hints(0, 0);
    }
}
#endif // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
is_cjk_font(PangoFontDescription *font_desc)
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    unsigned	i;
    പൂർണ്ണസംഖ്യ		is_cjk = FALSE;

    font = pango_context_load_font(gui.text_context, font_desc);

    എങ്കിൽ (font == ശൂന്യ)
	തിരികെ FALSE;

    വേണ്ടി (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	എങ്കിൽ (coverage != ശൂന്യ)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#എങ്കിൽ PANGO_VERSION_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#അല്ലെങ്കിൽ
	    pango_coverage_unref(coverage);
#endif
	}
    }

    g_object_unref(font);

    തിരികെ is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    പൂർണ്ണസംഖ്യ
gui_mch_adjust_charheight(ശൂന്യം)
{
    PangoFontMetrics	*metrics;
    പൂർണ്ണസംഖ്യ			ascent;
    പൂർണ്ണസംഖ്യ			descent;

    metrics = pango_context_get_metrics(gui.text_context, gui.norm_font,
				pango_context_get_language(gui.text_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_height = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_height = MAX(gui.char_height, gui.char_ascent + 1);

    തിരികെ OK;
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
// Callback function used in gui_mch_font_dialog()
    സ്ഥിതം gboolean
font_filter(സ്ഥിരം PangoFontFamily *family,
	    സ്ഥിരം PangoFontFace   *face UNUSED,
	    gpointer		   data UNUSED)
{
    തിരികെ pango_font_family_is_monospace((PangoFontFamily *)family);
}
#endif

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
    പൂർണ്ണസംഖ്യ		response;
    char_u	*fontname = ശൂന്യ;
    char_u	*oldname;

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
    dialog = gtk_font_chooser_dialog_new(ശൂന്യ, ശൂന്യ);
    gtk_font_chooser_set_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    ശൂന്യ, ശൂന്യ);
#അല്ലെങ്കിൽ
    dialog = gtk_font_selection_dialog_new(ശൂന്യ);
#endif

    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    എങ്കിൽ (oldval != ശൂന്യ && oldval[0] != NUL)
    {
	എങ്കിൽ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ശൂന്യ);
	അല്ലെങ്കിൽ
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	എങ്കിൽ (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    എങ്കിൽ (p != ശൂന്യ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		എങ്കിൽ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), (സ്ഥിരം gchar *)oldname);
#അല്ലെങ്കിൽ
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (സ്ഥിരം അക്ഷരം *)oldname);
#endif

	എങ്കിൽ (oldname != oldval)
	    vim_free(oldname);
    }
    അല്ലെങ്കിൽ
#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#അല്ലെങ്കിൽ
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#endif

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    എങ്കിൽ (response == GTK_RESPONSE_OK)
    {
	അക്ഷരം *name;

#എങ്കിൽ GTK_CHECK_VERSION(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#അല്ലെങ്കിൽ
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#endif
	എങ്കിൽ (name != ശൂന്യ)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    എങ്കിൽ (p != ശൂന്യ && നിവേശം_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&നിവേശം_conv, p, ശൂന്യ);
		vim_free(p);
	    }
	    അല്ലെങ്കിൽ
		fontname = p;
	}
    }

    എങ്കിൽ (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    തിരികെ fontname;
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
    സ്ഥിതം ശൂന്യം
get_styled_font_variants(ശൂന്യം)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);

    എങ്കിൽ (plain_font == ശൂന്യ)
	തിരികെ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    എങ്കിൽ (bold_font != ശൂന്യ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    എങ്കിൽ (bold_font != ശൂന്യ && gui.font_can_bold)
	g_object_unref(plain_font);
}

സ്ഥിതം PangoEngineShape *default_shape_engine = ശൂന്യ;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    സ്ഥിതം ശൂന്യം
ascii_glyph_table_init(ശൂന്യം)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    പൂർണ്ണസംഖ്യ		    i;

    എങ്കിൽ (gui.ascii_glyphs != ശൂന്യ)
	pango_glyph_string_free(gui.ascii_glyphs);
    എങ്കിൽ (gui.ascii_font != ശൂന്യ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ശൂന്യ;
    gui.ascii_font   = ശൂന്യ;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    വേണ്ടി (i = 0; i < 128; ++i)
    {
	എങ്കിൽ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	അല്ലെങ്കിൽ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (സ്ഥിരം അക്ഷരം *)ascii_chars,
			      0, വലുപ്പം(ascii_chars), attr_list, ശൂന്യ);

    എങ്കിൽ (item_list != ശൂന്യ && item_list->next == ശൂന്യ) // play safe
    {
	PangoItem   *item;
	പൂർണ്ണസംഖ്യ	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = item->analysis.shape_engine;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((സ്ഥിരം അക്ഷരം *)ascii_chars, വലുപ്പം(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->അക്കം_glyphs == വലുപ്പം(ascii_chars));

	വേണ്ടി (i = 0; i < gui.ascii_glyphs->അക്കം_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_offset += MAX(0, width - geom->width) / 2;
	    geom->width = width;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(item_list, (GFunc)(ശൂന്യം *)&pango_item_free, ശൂന്യ);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_init_font(char_u *font_name, പൂർണ്ണസംഖ്യ fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    പൂർണ്ണസംഖ്യ			    width;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    എങ്കിൽ (font_name == ശൂന്യ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    എങ്കിൽ (font_desc == ശൂന്യ)
	തിരികെ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, ശൂന്യ);
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
    എങ്കിൽ (is_cjk_font(gui.norm_font))
    {
	പൂർണ്ണസംഖ്യ cjk_width;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_set_text(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_size(layout, &cjk_width, ശൂന്യ);

	എങ്കിൽ (width == cjk_width)  // Xft not patched
	    width /= 2;
    }
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    എങ്കിൽ (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    എങ്കിൽ (gui.wide_font != ശൂന്യ
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = ശൂന്യ;
    }

    എങ്കിൽ (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    അല്ലെങ്കിൽ
    {
	// Preserve the logical dimensions of the screen.
	update_window_manager_hints(0, 0);
    }

    തിരികെ OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, പൂർണ്ണസംഖ്യ report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    എങ്കിൽ (!gui.in_use || name == ശൂന്യ)
	തിരികെ ശൂന്യ;

    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, ശൂന്യ);
	എങ്കിൽ (buf != ശൂന്യ)
	{
	    font = pango_font_description_from_string((സ്ഥിരം അക്ഷരം *)buf);
	    vim_free(buf);
	}
	അല്ലെങ്കിൽ
	    font = ശൂന്യ;
    }
    അല്ലെങ്കിൽ
	font = pango_font_description_from_string((സ്ഥിരം അക്ഷരം *)name);

    എങ്കിൽ (font != ശൂന്യ)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	എങ്കിൽ (pango_font_description_get_size(font) <= 0)
	    pango_font_description_set_size(font, 10 * PANGO_SCALE);

	real_font = pango_context_load_font(gui.text_context, font);

	എങ്കിൽ (real_font == ശൂന്യ)
	{
	    pango_font_description_free(font);
	    font = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
	    g_object_unref(real_font);
    }

    എങ്കിൽ (font == ശൂന്യ)
    {
	എങ്കിൽ (report_error)
	    semsg(_((അക്ഷരം *)e_unknown_font_str), name);
	തിരികെ ശൂന്യ;
    }

    തിരികെ font;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    എങ്കിൽ (font != NOFONT)
    {
	അക്ഷരം	*pangoname = pango_font_description_to_string(font);

	എങ്കിൽ (pangoname != ശൂന്യ)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    തിരികെ s;
	}
    }
    തിരികെ ശൂന്യ;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    ശൂന്യം
gui_mch_free_font(GuiFont font)
{
    എങ്കിൽ (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
    ശൂന്യം
gui_mch_expand_font(
    optexpand_T	*args,
    ശൂന്യം	*param,
    പൂർണ്ണസംഖ്യ		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = ശൂന്യ;
    പൂർണ്ണസംഖ്യ			n_families = 0;
    പൂർണ്ണസംഖ്യ			wide = *(പൂർണ്ണസംഖ്യ *)param;

    എങ്കിൽ (args->oe_include_orig_val && *args->oe_opt_മൂല്യം == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	എങ്കിൽ (add_match((char_u *)DEFAULT_FONT) != OK)
	    തിരികെ;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_families; i++)
    {
	എങ്കിൽ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    തുടരുക;

	സ്ഥിരം അക്ഷരം* fam_name = pango_font_family_get_name(font_families[i]);
	എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&നിവേശം_conv, (char_u*)fam_name, ശൂന്യ);
	    എങ്കിൽ (buf != ശൂന്യ)
	    {
		എങ്കിൽ (add_match(buf) != OK)
		{
		    vim_free(buf);
		    നിർത്തുക;
		}
		vim_free(buf);
	    }
	    അല്ലെങ്കിൽ
		നിർത്തുക;
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (add_match((char_u *)fam_name) != OK)
		നിർത്തുക;
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

    എങ്കിൽ (!gui.in_use)		// can't do this when GUI not running
	തിരികെ color;

    എങ്കിൽ (name != ശൂന്യ)
	color = gui_get_color_cmn(name);

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    തിരികെ color;
#അല്ലെങ്കിൽ
    എങ്കിൽ (color == INVALCOLOR)
	തിരികെ INVALCOLOR;

    തിരികെ gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#endif
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ g, പൂർണ്ണസംഖ്യ b)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    തിരികെ gui_get_rgb_color_cmn(r, g, b);
#അല്ലെങ്കിൽ
    GdkColor gcolor;
    പൂർണ്ണസംഖ്യ ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    തിരികെ ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#endif
}

/*
 * Set the current text foreground color.
 */
    ശൂന്യം
gui_mch_set_fg_color(guicolor_T color)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    *gui.fgcolor = color_to_rgba(color);
#അല്ലെങ്കിൽ
    gui.fgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text background color.
 */
    ശൂന്യം
gui_mch_set_bg_color(guicolor_T color)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    *gui.bgcolor = color_to_rgba(color);
#അല്ലെങ്കിൽ
    gui.bgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text special color.
 */
    ശൂന്യം
gui_mch_set_sp_color(guicolor_T color)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    *gui.spcolor = color_to_rgba(color);
#അല്ലെങ്കിൽ
    gui.spcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#define തിരുകുക_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_തിരുകുക((AttrList), tmp_attr_);	    \
    }G_STMT_END

    സ്ഥിതം ശൂന്യം
apply_wide_font_attr(char_u *s, പൂർണ്ണസംഖ്യ len, PangoAttrList *attr_list)
{
    char_u  *start = ശൂന്യ;
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    uc;

    വേണ്ടി (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	എങ്കിൽ (start == ശൂന്യ)
	{
	    എങ്കിൽ (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    തിരുകുക_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, start - s, p - s);
	    start = ശൂന്യ;
	}
    }

    എങ്കിൽ (start != ശൂന്യ)
	തിരുകുക_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, start - s, len);
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
എണ്ണം_cluster_cells(char_u *s, PangoItem *item,
		    PangoGlyphString* glyphs, പൂർണ്ണസംഖ്യ i,
		    പൂർണ്ണസംഖ്യ *cluster_width,
		    പൂർണ്ണസംഖ്യ *last_glyph_rbearing)
{
    char_u  *p;
    പൂർണ്ണസംഖ്യ	    next;	// glyph start index of next cluster
    പൂർണ്ണസംഖ്യ	    start, end; // string segment of current cluster
    പൂർണ്ണസംഖ്യ	    width;	// real cluster width in Pango units
    പൂർണ്ണസംഖ്യ	    uc;
    പൂർണ്ണസംഖ്യ	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    വേണ്ടി (next = i + 1; next < glyphs->അക്കം_glyphs; ++next)
    {
	എങ്കിൽ (glyphs->glyphs[next].attr.is_cluster_start)
	    നിർത്തുക;
	അല്ലെങ്കിൽ എങ്കിൽ (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->അക്കം_glyphs) ?
			    glyphs->log_clusters[next] : item->length);

    വേണ്ടി (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	എങ്കിൽ (uc < 0x80)
	    ++cellcount;
	അല്ലെങ്കിൽ എങ്കിൽ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    എങ്കിൽ (last_glyph_rbearing != ശൂന്യ
	    && cellcount > 0 && next == glyphs->അക്കം_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(item->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, ശൂന്യ);

	എങ്കിൽ (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    എങ്കിൽ (cellcount > 0)
	*cluster_width = width;

    തിരികെ cellcount;
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
    സ്ഥിതം ശൂന്യം
setup_zero_width_cluster(PangoItem *item, PangoGlyphInfo *glyph,
			 പൂർണ്ണസംഖ്യ last_cellcount, പൂർണ്ണസംഖ്യ last_cluster_width,
			 പൂർണ്ണസംഖ്യ last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    പൂർണ്ണസംഖ്യ		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    എങ്കിൽ (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
		- (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    അല്ലെങ്കിൽ
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം ശൂന്യം
draw_glyph_string(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ അക്കം_cells, പൂർണ്ണസംഖ്യ flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
draw_glyph_string(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ അക്കം_cells, പൂർണ്ണസംഖ്യ flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#endif
{
    എങ്കിൽ (!(flags & DRAW_TRANSP))
    {
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	cairo_set_source_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(col), FILL_Y(row),
			അക്കം_cells * gui.char_width, gui.char_height);
	cairo_fill(cr);
#അല്ലെങ്കിൽ
	gdk_gc_set_foreground(gui.text_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->window,
			   gui.text_gc,
			   TRUE,
			   FILL_X(col),
			   FILL_Y(row),
			   അക്കം_cells * gui.char_width,
			   gui.char_height);
#endif
    }

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_to(cr, TEXT_X(col), TEXT_Y(row));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#അല്ലെങ്കിൽ
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->window,
		    gui.text_gc,
		    font,
		    TEXT_X(col),
		    TEXT_Y(row),
		    glyphs);
#endif

    // redraw the contents with an offset of 1 to emulate bold
    എങ്കിൽ ((flags & DRAW_BOLD) && !gui.font_can_bold)
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    {
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, TEXT_X(col) + 1, TEXT_Y(row));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#അല്ലെങ്കിൽ
	gdk_draw_glyphs(gui.drawarea->window,
			gui.text_gc,
			font,
			TEXT_X(col) + 1,
			TEXT_Y(row),
			glyphs);
#endif
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം ശൂന്യം
draw_under(പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ cells, cairo_t *cr)
#അല്ലെങ്കിൽ
    സ്ഥിതം ശൂന്യം
draw_under(പൂർണ്ണസംഖ്യ flags, പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ cells)
#endif
{
    പൂർണ്ണസംഖ്യ			i;
    പൂർണ്ണസംഖ്യ			offset;
    സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    പൂർണ്ണസംഖ്യ			y = FILL_Y(row + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    എങ്കിൽ (flags & DRAW_UNDERC)
    {
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	cairo_set_വരി_width(cr, 1.0);
	cairo_set_വരി_cap(cr, CAIRO_വരി_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col) + 1, y - 2 + 0.5);
	വേണ്ടി (i = FILL_X(col) + 1; i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    cairo_വരി_to(cr, i, y - offset + 0.5);
	}
	cairo_stroke(cr);
#അല്ലെങ്കിൽ
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	വേണ്ടി (i = FILL_X(col); i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    gdk_draw_point(gui.drawarea->window, gui.text_gc, i, y - offset);
	}
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Draw a strikethrough line
    എങ്കിൽ (flags & DRAW_STRIKE)
    {
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	cairo_set_വരി_width(cr, 1.0);
	cairo_set_വരി_cap(cr, CAIRO_വരി_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 1 - gui.char_height/2 + 0.5);
	cairo_വരി_to(cr, FILL_X(col + cells), y + 1 - gui.char_height/2 + 0.5);
	cairo_stroke(cr);
#അല്ലെങ്കിൽ
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	gdk_draw_വരി(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y + 1 - gui.char_height/2,
		      FILL_X(col + cells), y + 1 - gui.char_height/2);
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Underline: draw a line at the bottom of the character cell.
    എങ്കിൽ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	എങ്കിൽ (p_linespace > 1)
	    y -= p_linespace - 1;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	cairo_set_വരി_width(cr, 1.0);
	cairo_set_വരി_cap(cr, CAIRO_വരി_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 0.5);
	cairo_വരി_to(cr, FILL_X(col + cells), y + 0.5);
	cairo_stroke(cr);
#അല്ലെങ്കിൽ
	gdk_draw_വരി(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y,
		      FILL_X(col + cells) - 1, y);
#endif
    }
}

    പൂർണ്ണസംഖ്യ
gui_gtk_draw_string(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, char_u *s, പൂർണ്ണസംഖ്യ len, പൂർണ്ണസംഖ്യ flags)
{
    char_u	*conv_buf = ശൂന്യ;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    പൂർണ്ണസംഖ്യ		convlen;
    char_u	*sp, *bp;
    പൂർണ്ണസംഖ്യ		plen;
    പൂർണ്ണസംഖ്യ		len_sum;	// return value needs to add up since we are
				// printing substrings
    പൂർണ്ണസംഖ്യ		byte_sum;	// byte position in string
    char_u	*cs;		// current *s pointer
    പൂർണ്ണസംഖ്യ		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    പൂർണ്ണസംഖ്യ		should_need_pango = FALSE;
    പൂർണ്ണസംഖ്യ		slen;
    പൂർണ്ണസംഖ്യ		is_ligature;
    പൂർണ്ണസംഖ്യ		is_utf8;
    char_u	backup_ch;

    എങ്കിൽ (gui.text_context == ശൂന്യ || gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ len;

    എങ്കിൽ (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != ശൂന്യ, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	വേണ്ടി (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    എങ്കിൽ ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		എങ്കിൽ (new_conv_buf == ശൂന്യ)
		    തിരികെ len;
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
    len_sum = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_sum = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    എങ്കിൽ (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    എങ്കിൽ (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    ആകുമ്പോൾ (cs < s + len)
    {
	slen = 0;
	ആകുമ്പോൾ (slen < (len - byte_sum))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    എങ്കിൽ (is_ligature && !needs_pango)
	    {
		എങ്കിൽ ((slen + 1) < (len - byte_sum))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		അല്ലെങ്കിൽ
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    എങ്കിൽ ((!is_utf8) && ((slen + 1) < (len - byte_sum)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    എങ്കിൽ (needs_pango != should_need_pango) // mode switch
		നിർത്തുക;
	    എങ്കിൽ (needs_pango)
	    {
		എങ്കിൽ (is_ligature)
		{
		    slen++; // ligature char by char
		}
		അല്ലെങ്കിൽ // is_utf8
		{
		    എങ്കിൽ ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			ആകുമ്പോൾ ((slen < (len - byte_sum))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    അല്ലെങ്കിൽ
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    അല്ലെങ്കിൽ
	    {
		slen++; // ascii
	    }
	}

	എങ്കിൽ (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_sum += gui_gtk_draw_string_ext(row, col + len_sum, cs, slen, flags,
					    needs_pango);
	എങ്കിൽ (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_sum += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    തിരികെ len_sum;
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
സ്ഥിതം cairo_region_t *dirty_region = ശൂന്യ;

    സ്ഥിതം ശൂന്യം
queue_draw_area(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y, പൂർണ്ണസംഖ്യ width, പൂർണ്ണസംഖ്യ height)
{
    cairo_rectangle_int_t rect;

    എങ്കിൽ (width <= 0 || height <= 0 || gui.drawarea == ശൂന്യ)
	തിരികെ;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    എങ്കിൽ (dirty_region == ശൂന്യ)
	dirty_region = cairo_region_create_rectangle(&rect);
    അല്ലെങ്കിൽ
	cairo_region_union_rectangle(dirty_region, &rect);
}
#endif

    പൂർണ്ണസംഖ്യ
gui_gtk_draw_string_ext(
	പൂർണ്ണസംഖ്യ	row,
	പൂർണ്ണസംഖ്യ	col,
	char_u	*s,
	പൂർണ്ണസംഖ്യ	len,
	പൂർണ്ണസംഖ്യ	flags,
	പൂർണ്ണസംഖ്യ	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    പൂർണ്ണസംഖ്യ			column_offset = 0;  // column offset in cells
    പൂർണ്ണസംഖ്യ			i;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_t		*cr;
#endif

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_offset;
    area.y = FILL_Y(row);
    area.width	= gui.അക്കം_cols * gui.char_width;
    area.height = gui.char_height;

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.width, area.height);
    cairo_clip(cr);
#അല്ലെങ്കിൽ
    gdk_gc_set_clip_origin(gui.text_gc, 0, 0);
    gdk_gc_set_clip_rectangle(gui.text_gc, &area);
#endif

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    എങ്കിൽ (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ശൂന്യ
	    && !force_pango)
    {
	char_u *p;

	വേണ്ടി (p = s; p < s + len; ++p)
	    എങ്കിൽ (*p & 0x80)
		പോകുക not_ascii;

	pango_glyph_string_set_size(glyphs, len);

	വേണ്ടി (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs, cr);
#അല്ലെങ്കിൽ
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs);
#endif

	column_offset = len;
    }
    അല്ലെങ്കിൽ
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*item_list;
	പൂർണ്ണസംഖ്യ		cluster_width;
	പൂർണ്ണസംഖ്യ		last_glyph_rbearing;
	പൂർണ്ണസംഖ്യ		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	എങ്കിൽ (!utf_valid_string(s, s + len))
	{
	    column_offset = len;
	    പോകുക skipitall;
	}

	// original width of the current cluster
	cluster_width = PANGO_SCALE * gui.char_width;

	// right bearing of the last non-composing glyph
	last_glyph_rbearing = PANGO_SCALE * gui.char_width;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	എങ്കിൽ (gui.wide_font != ശൂന്യ)
	    apply_wide_font_attr(s, len, attr_list);

	എങ്കിൽ ((flags & DRAW_BOLD) && gui.font_can_bold)
	    തിരുകുക_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
			      attr_list, 0, len);
	എങ്കിൽ (flags & DRAW_ITALIC)
	    തിരുകുക_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	item_list = pango_itemize(gui.text_context,
				  (സ്ഥിരം അക്ഷരം *)s, 0, len, attr_list, ശൂന്യ);

	ആകുമ്പോൾ (item_list != ശൂന്യ)
	{
	    PangoItem	*item;
	    പൂർണ്ണസംഖ്യ		item_cells = 0; // item length in cells

	    item = (PangoItem *)item_list->data;
	    item_list = g_list_delete_link(item_list, item_list);
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
	    item->analysis.level = (item->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    item->analysis.shape_engine = default_shape_engine;

#ifdef HAVE_PANGO_SHAPE_FULL
	    pango_shape_full((സ്ഥിരം അക്ഷരം *)s + item->offset, item->length,
		    (സ്ഥിരം അക്ഷരം *)s, len, &item->analysis, glyphs);
#അല്ലെങ്കിൽ
	    pango_shape((സ്ഥിരം അക്ഷരം *)s + item->offset, item->length,
			&item->analysis, glyphs);
#endif
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    വേണ്ടി (i = 0; i < glyphs->അക്കം_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		എങ്കിൽ (glyph->attr.is_cluster_start)
		{
		    പൂർണ്ണസംഖ്യ cellcount;

		    cellcount = എണ്ണം_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != ശൂന്യ) ? &last_glyph_rbearing : ശൂന്യ);

		    എങ്കിൽ (cellcount > 0)
		    {
			പൂർണ്ണസംഖ്യ width;

			width = cellcount * gui.char_width * PANGO_SCALE;
			glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
			glyph->geometry.width = width;
		    }
		    അല്ലെങ്കിൽ
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			setup_zero_width_cluster(item, glyph, cells,
						 cluster_width,
						 last_glyph_rbearing);
		    }

		    item_cells += cellcount;
		    cells = cellcount;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (i > 0)
		{
		    പൂർണ്ണസംഖ്യ width;

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
		    എങ്കിൽ (glyph->geometry.x_offset >= 0)
		    {
			glyphs->glyphs[i].geometry.width =
					 glyphs->glyphs[i - 1].geometry.width;
			glyphs->glyphs[i - 1].geometry.width = 0;
		    }
		    width = cells * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
		}
		അല്ലെങ്കിൽ // i == 0 "cannot happen"
		{
		    glyph->geometry.width = 0;
		}
	    }

	    //// Aaaaand action! **
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	    draw_glyph_string(row, col + column_offset, item_cells,
			      flags, item->analysis.font, glyphs,
			      cr);
#അല്ലെങ്കിൽ
	    draw_glyph_string(row, col + column_offset, item_cells,
			      flags, item->analysis.font, glyphs);
#endif

	    pango_item_free(item);

	    column_offset += item_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    draw_under(flags, row, col, column_offset, cr);
#അല്ലെങ്കിൽ
    draw_under(flags, row, col, column_offset);
#endif

    pango_glyph_string_free(glyphs);

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_destroy(cr);
    queue_draw_area(FILL_X(col), FILL_Y(row),
	    column_offset * gui.char_width + 1, gui.char_height);
#അല്ലെങ്കിൽ
    gdk_gc_set_clip_rectangle(gui.text_gc, ശൂന്യ);
#endif

    തിരികെ column_offset;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_haskey(char_u *name)
{
    പൂർണ്ണസംഖ്യ i;

    വേണ്ടി (i = 0; special_താക്കോലുകൾ[i].താക്കോൽ_sym != 0; i++)
	എങ്കിൽ (name[0] == special_താക്കോലുകൾ[i].code0
		&& name[1] == special_താക്കോലുകൾ[i].code1)
	    തിരികെ OK;
    തിരികെ FAIL;
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    പൂർണ്ണസംഖ്യ
gui_get_x11_windis(Window *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    എങ്കിൽ (dpy)
    {
	*dis = dpy;
	*win = GDK_WINDOW_XID(gtk_widget_get_window(gui.mainwin));
	തിരികെ OK;
    }

    *dis = ശൂന്യ;
    *win = 0;
    തിരികെ FAIL;
}
#endif

    Display *
gui_mch_get_display(ശൂന്യം)
{
    എങ്കിൽ (gui.mainwin != ശൂന്യ && gtk_widget_get_window(gui.mainwin) != ശൂന്യ
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#endif
	)
	തിരികെ GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin));
    അല്ലെങ്കിൽ
	തിരികെ ശൂന്യ;
}

    ശൂന്യം
gui_mch_beep(ശൂന്യം)
{
    GdkDisplay *display;

    എങ്കിൽ (gui.mainwin != ശൂന്യ && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    അല്ലെങ്കിൽ
	display = gdk_display_get_default();

    എങ്കിൽ (display != ശൂന്യ)
	gdk_display_beep(display);
}

    ശൂന്യം
gui_mch_flash(പൂർണ്ണസംഖ്യ msec)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    // TODO Replace GdkGC with Cairo
    (ശൂന്യം)msec;
#അല്ലെങ്കിൽ
    GdkGCValues	മൂല്യങ്ങൾ;
    GdkGC	*invert_gc;

    എങ്കിൽ (gui.drawarea->window == ശൂന്യ)
	തിരികെ;

    മൂല്യങ്ങൾ.foreground.pixel = gui.norm_pixel ^ gui.back_pixel;
    മൂല്യങ്ങൾ.background.pixel = gui.norm_pixel ^ gui.back_pixel;
    മൂല്യങ്ങൾ.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_മൂല്യങ്ങൾ(gui.drawarea->window,
				       &മൂല്യങ്ങൾ,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_set_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((പൂർണ്ണസംഖ്യ)Columns) + gui.border_offset,
		       FILL_Y((പൂർണ്ണസംഖ്യ)Rows) + gui.border_offset);

    gui_mch_flush();
    ui_delay((long)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((പൂർണ്ണസംഖ്യ)Columns) + gui.border_offset,
		       FILL_Y((പൂർണ്ണസംഖ്യ)Rows) + gui.border_offset);

    gdk_gc_destroy(invert_gc);
#endif
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ശൂന്യം
gui_mch_invert_rectangle(പൂർണ്ണസംഖ്യ r, പൂർണ്ണസംഖ്യ c, പൂർണ്ണസംഖ്യ nr, പൂർണ്ണസംഖ്യ nc)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിരം GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_width, nr * gui.char_height
    };
    cairo_t * സ്ഥിരം cr = cairo_create(gui.surface);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# എങ്കിൽ CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1,9,2)
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# അല്ലെങ്കിൽ
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);

    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
#അല്ലെങ്കിൽ
    GdkGCValues മൂല്യങ്ങൾ;
    GdkGC *invert_gc;

    എങ്കിൽ (gui.drawarea->window == ശൂന്യ)
	തിരികെ;

    മൂല്യങ്ങൾ.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_മൂല്യങ്ങൾ(gui.drawarea->window,
				       &മൂല്യങ്ങൾ,
				       GDK_GC_FUNCTION);
    gdk_gc_set_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_width, (nr) * gui.char_height);
    gdk_gc_destroy(invert_gc);
#endif
}

/*
 * Iconify the GUI window.
 */
    ശൂന്യം
gui_mch_iconify(ശൂന്യം)
{
    gtk_window_iconify(GTK_WINDOW(gui.mainwin));
}

#എങ്കിൽ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ശൂന്യം
gui_mch_set_foreground(ശൂന്യം)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_window_set_keep_above(GTK_WINDOW(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_window_present(GTK_WINDOW(gui.mainwin));
    gui_may_flush();
    gtk_window_set_keep_above(GTK_WINDOW(gui.mainwin), FALSE);
    gui_may_flush();
}
#endif

/*
 * Draw a cursor without focus.
 */
    ശൂന്യം
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    പൂർണ്ണസംഖ്യ		i = 1;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_t    *cr;
#endif

    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ;

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
#endif

    gui_mch_set_fg_color(color);

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#അല്ലെങ്കിൽ
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    എങ്കിൽ (mb_lefthalve(gui.row, gui.col))
	i = 2;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    cairo_set_വരി_width(cr, 1.0);
    cairo_set_വരി_cap(cr, CAIRO_വരി_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.col) + 0.5, FILL_Y(gui.row) + 0.5,
	    i * gui.char_width - 1, gui.char_height - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#അല്ലെങ്കിൽ
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    FALSE,
	    FILL_X(gui.col), FILL_Y(gui.row),
	    i * gui.char_width - 1, gui.char_height - 1);
#endif
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ശൂന്യം
gui_mch_draw_part_cursor(പൂർണ്ണസംഖ്യ w, പൂർണ്ണസംഖ്യ h, guicolor_T color)
{
    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ;

    gui_mch_set_fg_color(color);

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_വലത് ? FILL_X(gui.col + 1) - w :
# endif
	    FILL_X(gui.col), FILL_Y(gui.row) + gui.char_height - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_വലത് ? FILL_X(gui.col + 1) - w :
# endif
	    FILL_X(gui.col),
	    FILL_Y(gui.row) + gui.char_height - h,
	    w, h);
#endif // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    ശൂന്യം
gui_mch_update(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ cnt = 0;	// prevent endless loop
    ആകുമ്പോൾ (g_main_context_pending(ശൂന്യ) && !vim_is_നിവേശം_buf_full()
	    && ++cnt < 100)
    {
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	പൂർണ്ണസംഖ്യ prio = 0;
	g_main_context_prepare(ശൂന്യ, &prio);
	// peek internal scheduling of redraw
	എങ്കിൽ (prio == GDK_PRIORITY_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#endif
	g_main_context_iteration(ശൂന്യ, TRUE);
    }
}

    സ്ഥിതം timeout_cb_type
നിവേശം_timer_cb(gpointer data)
{
    പൂർണ്ണസംഖ്യ *timed_out = (പൂർണ്ണസംഖ്യ *) data;

    // Just inform the caller about the occurrence of it
    *timed_out = TRUE;

    തിരികെ FALSE;		// don't happen again
}

#ifdef FEAT_JOB_CHANNEL
    സ്ഥിതം timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    തിരികെ TRUE;		// repeat
}
#endif

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    പൂർണ്ണസംഖ്യ
gui_mch_wait_for_chars(long wtime)
{
    പൂർണ്ണസംഖ്യ		focus;
    guint	timer;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	timed_out;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    എങ്കിൽ (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   നിവേശം_timer_cb, &timed_out);
    അല്ലെങ്കിൽ
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    എങ്കിൽ (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, ശൂന്യ);
#endif

    focus = gui.in_focus;

    ചെയ്യുക
    {
	// Stop or start blinking when focus changes
	എങ്കിൽ (gui.in_focus != focus)
	{
	    എങ്കിൽ (gui.in_focus)
		gui_mch_start_blink();
	    അല്ലെങ്കിൽ
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	എങ്കിൽ (did_add_timer)
	    // Need to recompute the waiting time.
	    പോകുക theend;
# endif
#endif

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	എങ്കിൽ (!നിവേശം_available())
	{
	    g_main_context_iteration(ശൂന്യ, TRUE);
#ifdef GDK_WINDOWING_WAYLAND
    എങ്കിൽ (gui.is_wayland && clip_star.state == SELECT_IN_PROGRESS)
	gui_may_flush();
#endif
	}

	// Got char, return immediately
	എങ്കിൽ (നിവേശം_available())
	{
	    retval = OK;
	    പോകുക theend;
	}
    } ആകുമ്പോൾ (wtime < 0 || !timed_out);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    എങ്കിൽ (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    എങ്കിൽ (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    തിരികെ retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    ശൂന്യം
gui_mch_flush(ശൂന്യം)
{
    എങ്കിൽ (gui.mainwin == ശൂന്യ || !gtk_widget_get_realized(gui.mainwin))
	തിരികെ;
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    എങ്കിൽ (dirty_region != ശൂന്യ && gui.drawarea != ശൂന്യ)
    {
	gtk_widget_queue_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = ശൂന്യ;
    }
#അല്ലെങ്കിൽ
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       തിരികെ;
#endif
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ശൂന്യം
gui_mch_clear_block(പൂർണ്ണസംഖ്യ row1arg, പൂർണ്ണസംഖ്യ col1arg, പൂർണ്ണസംഖ്യ row2arg, പൂർണ്ണസംഖ്യ col2arg)
{

    പൂർണ്ണസംഖ്യ col1 = check_col(col1arg);
    പൂർണ്ണസംഖ്യ col2 = check_col(col2arg);
    പൂർണ്ണസംഖ്യ row1 = check_row(row1arg);
    പൂർണ്ണസംഖ്യ row2 = check_row(row2arg);

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ;
#അല്ലെങ്കിൽ
    GdkColor color;

    എങ്കിൽ (gui.drawarea->window == ശൂന്യ)
	തിരികെ;

    color.pixel = gui.back_pixel;
#endif

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	സ്ഥിരം GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height
	};
	cairo_t * സ്ഥിരം cr = cairo_create(gui.surface);
# എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# അല്ലെങ്കിൽ
	GdkWindow * സ്ഥിരം win = gtk_widget_get_window(gui.drawarea);
	cairo_pattern_t * സ്ഥിരം pat = gdk_window_get_background_pattern(win);
	എങ്കിൽ (pat != ശൂന്യ)
	    cairo_set_source(cr, pat);
	അല്ലെങ്കിൽ
	    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_fill(cr);
	cairo_destroy(cr);

	queue_draw_area(rect.x, rect.y, rect.width, rect.height);
    }
#അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_set_foreground(gui.text_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_width
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_height);
#endif // !GTK_CHECK_VERSION(3,0,0)
}

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം ശൂന്യം
gui_gtk_window_clear(GdkWindow *win)
{
    സ്ഥിരം GdkRectangle rect = {
	0, 0, gdk_window_get_width(win), gdk_window_get_height(win)
    };
    cairo_t * സ്ഥിരം cr = cairo_create(gui.surface);
# എങ്കിൽ GTK_CHECK_VERSION(3,22,2)
    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# അല്ലെങ്കിൽ
    cairo_pattern_t * സ്ഥിരം pat = gdk_window_get_background_pattern(win);
    എങ്കിൽ (pat != ശൂന്യ)
	cairo_set_source(cr, pat);
    അല്ലെങ്കിൽ
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);
    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
}
#അല്ലെങ്കിൽ
# define gui_gtk_window_clear(win)  gdk_window_clear(win)
#endif

    ശൂന്യം
gui_mch_clear_all(ശൂന്യം)
{
    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) != ശൂന്യ)
	gui_gtk_window_clear(gtk_widget_get_window(gui.drawarea));
}

#എങ്കിൽ !GTK_CHECK_VERSION(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    സ്ഥിതം ശൂന്യം
check_copy_area(ശൂന്യം)
{
    GdkEvent        *event;
    പൂർണ്ണസംഖ്യ                expose_എണ്ണം;
    എങ്കിൽ (gui.visibility != GDK_VISIBILITY_PARTIAL)
	തിരികെ;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    ചെയ്യുക
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->window);
	എങ്കിൽ (event == ശൂന്യ)
	    നിർത്തുക; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.width, event->expose.area.height);
	expose_എണ്ണം = event->expose.എണ്ണം;
	gdk_event_free(event);
    }
    ആകുമ്പോൾ (expose_എണ്ണം > 0); // more events follow
    gui_can_update_cursor();
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിതം ശൂന്യം
gui_gtk_surface_copy_rect(പൂർണ്ണസംഖ്യ dest_x, പൂർണ്ണസംഖ്യ dest_y,
			  പൂർണ്ണസംഖ്യ src_x,  പൂർണ്ണസംഖ്യ src_y,
			  പൂർണ്ണസംഖ്യ width,  പൂർണ്ണസംഖ്യ height)
{
    cairo_t * സ്ഥിരം cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    സ്ഥിതം cairo_surface_t *scroll_scratch = ശൂന്യ;
    സ്ഥിതം പൂർണ്ണസംഖ്യ scratch_w = 0, scratch_h = 0;
    പൂർണ്ണസംഖ്യ last_row = Rows - 1, last_row_y = last_row * gui.char_height;
    bool last_row_overlap = (dest_y + height) > last_row_y;
    എങ്കിൽ (gui.is_wayland && ( !(State & MODE_CMDLINE) || !last_row_overlap) )
    {
	എങ്കിൽ (dest_y < src_y) // scroll up
	{
	    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	    cairo_set_source_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, width, height);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	അല്ലെങ്കിൽ
	{   //  reuse surface when scrolling, only realloc if larger
	    എങ്കിൽ (scroll_scratch == ശൂന്യ || width > scratch_w || height > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), width, height);
		scratch_w = width, scratch_h = height;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_set_source_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	    cairo_rectangle(cr, dest_x, dest_y, width, height);
	    cairo_clip(cr);
	    cairo_set_source_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    അല്ലെങ്കിൽ
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, width, height);
	cairo_clip(cr);
	cairo_തള്ളുക_group(cr);
	cairo_set_source_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_എടുക്കുക_group_to_source(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#endif

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ശൂന്യം
gui_mch_delete_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിരം പൂർണ്ണസംഖ്യ ncols = gui.scroll_region_വലത് - gui.scroll_region_ഇടത് + 1;
    സ്ഥിരം പൂർണ്ണസംഖ്യ nrows = gui.scroll_region_bot - row + 1;
    സ്ഥിരം പൂർണ്ണസംഖ്യ src_nrows = nrows - അക്കം_വരികൾ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row + അക്കം_വരികൾ),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    gui.scroll_region_bot - അക്കം_വരികൾ + 1, gui.scroll_region_ഇടത്,
	    gui.scroll_region_bot,		   gui.scroll_region_വലത്);
    queue_draw_area(
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#അല്ലെങ്കിൽ
    എങ്കിൽ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	തിരികെ;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_ഇടത്),
	    FILL_Y(row + അക്കം_വരികൾ),
	    gui.char_width * (gui.scroll_region_വലത്
					    - gui.scroll_region_ഇടത് + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - അക്കം_വരികൾ + 1));

    gui_clear_block(gui.scroll_region_bot - അക്കം_വരികൾ + 1,
						       gui.scroll_region_ഇടത്,
		    gui.scroll_region_bot, gui.scroll_region_വലത്);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ശൂന്യം
gui_mch_തിരുകുക_വരികൾ(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ അക്കം_വരികൾ)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിരം പൂർണ്ണസംഖ്യ ncols = gui.scroll_region_വലത് - gui.scroll_region_ഇടത് + 1;
    സ്ഥിരം പൂർണ്ണസംഖ്യ nrows = gui.scroll_region_bot - row + 1;
    സ്ഥിരം പൂർണ്ണസംഖ്യ src_nrows = nrows - അക്കം_വരികൾ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row + അക്കം_വരികൾ),
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    row,		 gui.scroll_region_ഇടത്,
	    row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);
    queue_draw_area(
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#അല്ലെങ്കിൽ
    എങ്കിൽ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	തിരികെ;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row + അക്കം_വരികൾ),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_ഇടത്), FILL_Y(row),
	    gui.char_width * (gui.scroll_region_വലത്
					    - gui.scroll_region_ഇടത് + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - അക്കം_വരികൾ + 1));

    gui_clear_block(row, gui.scroll_region_ഇടത്,
				row + അക്കം_വരികൾ - 1, gui.scroll_region_വലത്);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    ശൂന്യം
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	target;
    unsigned	i;
    time_t	start;

    വേണ്ടി (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	എങ്കിൽ (!clip_html && selection_targets[i].info == TARGET_HTML)
	    തുടരുക;
	received_selection = RS_NONE;
	target = gdk_atom_intern(selection_targets[i].target, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, target,
			      (guint32)GDK_CURRENT_TIME);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	start = time(ശൂന്യ);
	ആകുമ്പോൾ (received_selection == RS_NONE && time(ശൂന്യ) < start + 3)
	    g_main_context_iteration(ശൂന്യ, TRUE);	// wait for selection_received_cb

	എങ്കിൽ (received_selection != RS_FAIL)
	    തിരികെ;
    }

    എങ്കിൽ (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    ശൂന്യം
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    എങ്കിൽ (in_selection_clear_event)
	തിരികെ;

    gtk_selection_owner_set(ശൂന്യ, cbd->gtk_sel_atom, gui.event_time);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    പൂർണ്ണസംഖ്യ
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    എങ്കിൽ (is_autocmd_blocked())
	തിരികെ OK;

    പൂർണ്ണസംഖ്യ success;

    success = gtk_selection_owner_set(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_time);
    // don't update on every visual selection change
    എങ്കിൽ (!(cbd->owned && VIsual_active))
	gui_gtk_set_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    തിരികെ (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ശൂന്യം
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

#എങ്കിൽ defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    പൂർണ്ണസംഖ്യ
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    തിരികെ gdk_selection_owner_get(cbd->gtk_sel_atom) != ശൂന്യ;
}
#endif


#എങ്കിൽ defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    ശൂന്യം
gui_mch_menu_grey(vimmenu_T *menu, പൂർണ്ണസംഖ്യ grey)
{
    എങ്കിൽ (menu->id == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    എങ്കിൽ (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_set_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    ശൂന്യം
gui_mch_menu_hidden(vimmenu_T *menu, പൂർണ്ണസംഖ്യ hidden)
{
    എങ്കിൽ (menu->id == 0)
	തിരികെ;

    എങ്കിൽ (hidden)
    {
	എങ്കിൽ (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ശൂന്യം
gui_mch_draw_menubar(ശൂന്യം)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#endif // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    ശൂന്യം
gui_mch_enable_scrollbar(scrollbar_T *sb, പൂർണ്ണസംഖ്യ flag)
{
    എങ്കിൽ (sb->id == ശൂന്യ)
	തിരികെ;

    gtk_widget_set_visible(sb->id, flag);
    update_window_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    തിരികെ (long_u)pixel;
#അല്ലെങ്കിൽ
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (unsigned long)pixel, &color);

    തിരികെ (guicolor_T)(
	    (((unsigned)color.red   & 0xff00) << 8)
	 |  ((unsigned)color.green & 0xff00)
	 | (((unsigned)color.blue  & 0xff00) >> 8));
#endif
}

/*
 * Get current mouse coordinates in text window.
 */
    ശൂന്യം
gui_mch_getmouse(പൂർണ്ണസംഖ്യ *x, പൂർണ്ണസംഖ്യ *y)
{
    gui_gtk_get_pointer(gui.drawarea, x, y, ശൂന്യ);
}

    ശൂന്യം
gui_mch_setmouse(പൂർണ്ണസംഖ്യ x, പൂർണ്ണസംഖ്യ y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    എങ്കിൽ (dpy)
	XWarpPointer(dpy, (Window)0,
		GDK_WINDOW_XID(gtk_widget_get_window(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
സ്ഥിതം പൂർണ്ണസംഖ്യ last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    ശൂന്യം
gui_mch_mousehide(പൂർണ്ണസംഖ്യ hide)
{
    എങ്കിൽ (gui.pointer_hidden == hide)
	തിരികെ;

    gui.pointer_hidden = hide;
    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) && gui.blank_pointer != ശൂന്യ)
    {
	എങ്കിൽ (hide)
	    gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		    gui.blank_pointer);
	അല്ലെങ്കിൽ
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#അല്ലെങ്കിൽ
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), ശൂന്യ);
#endif
    }
}

#എങ്കിൽ defined(FEAT_MOUSESHAPE)

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
സ്ഥിതം സ്ഥിരം അക്ഷരം * mshape_css_names[] =
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
# അല്ലെങ്കിൽ
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
സ്ഥിതം സ്ഥിരം പൂർണ്ണസംഖ്യ mshape_ids[] =
{
    GDK_ഇടത്_PTR,		// arrow
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
    GDK_വലത്_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_ഇടത്_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    ശൂന്യം
mch_set_mouse_shape(പൂർണ്ണസംഖ്യ shape)
{
    GdkCursor	   *c;
    പൂർണ്ണസംഖ്യ		   id;                    // Only id or css_name is used.
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    സ്ഥിരം അക്ഷരം     *css_name = "default";
    GdkDisplay     *disp;
# endif

    എങ്കിൽ (gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    എങ്കിൽ (id >= GDK_LAST_CURSOR)
		id = GDK_ഇടത്_PTR;
	    അല്ലെങ്കിൽ
		id &= ~1;	// they are always even (why?)
	}
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	അല്ലെങ്കിൽ എങ്കിൽ (shape < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
# അല്ലെങ്കിൽ
	അല്ലെങ്കിൽ എങ്കിൽ (shape < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(mshape_ids))
	    id = mshape_ids[shape];
# endif
	അല്ലെങ്കിൽ
	    തിരികെ;
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# അല്ലെങ്കിൽ
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), c);
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(c));
# അല്ലെങ്കിൽ
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    എങ്കിൽ (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif // FEAT_MOUSESHAPE


#എങ്കിൽ defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)
# define SIGN_ASPECT ((ഇരട്ട)SIGN_HEIGHT / (ഇരട്ട)SIGN_WIDTH)

    ശൂന്യം
gui_mch_drawsign(പൂർണ്ണസംഖ്യ row, പൂർണ്ണസംഖ്യ col, പൂർണ്ണസംഖ്യ typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    എങ്കിൽ (sign == ശൂന്യ || gui.drawarea == ശൂന്യ
	    || gtk_widget_get_window(gui.drawarea) == ശൂന്യ)
	തിരികെ;

    പൂർണ്ണസംഖ്യ width;
    പൂർണ്ണസംഖ്യ height;
    പൂർണ്ണസംഖ്യ xoffset;
    പൂർണ്ണസംഖ്യ yoffset;
    പൂർണ്ണസംഖ്യ need_scale;

    width  = gdk_pixbuf_get_width(sign);
    height = gdk_pixbuf_get_height(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (width > SIGN_WIDTH + 2
	    || height != SIGN_HEIGHT
	    || (width < 3 * SIGN_WIDTH / 4
		&& height < 3 * SIGN_HEIGHT / 4));
    എങ്കിൽ (need_scale)
    {
	ഇരട്ട  aspect;
	പൂർണ്ണസംഖ്യ	    w = width;
	പൂർണ്ണസംഖ്യ	    h = height;

	// Keep the original aspect ratio
	aspect = (ഇരട്ട)height / (ഇരട്ട)width;
	width  = (ഇരട്ട)SIGN_WIDTH * SIGN_ASPECT / aspect;
	width  = MIN(width, SIGN_WIDTH);
	എങ്കിൽ (((ഇരട്ട)(MAX(height, SIGN_HEIGHT)) /
		    (ഇരട്ട)(MIN(height, SIGN_HEIGHT))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    height = (ഇരട്ട)SIGN_HEIGHT * SIGN_ASPECT / aspect;
	    height = MIN(height, SIGN_HEIGHT);
	}
	അല്ലെങ്കിൽ
	    height = (ഇരട്ട)width * aspect;

	എങ്കിൽ (w == width && h == height)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	അല്ലെങ്കിൽ
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, width, height,
		    GDK_INTERP_BILINEAR);
	    എങ്കിൽ (sign == ശൂന്യ)
		തിരികെ; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (width  - SIGN_WIDTH)  / 2;
    yoffset = (height - SIGN_HEIGHT) / 2;

# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_WIDTH, SIGN_HEIGHT);
	bg_cr = cairo_create(bg_surf);
	cairo_set_source_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_WIDTH, SIGN_HEIGHT);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_set_source_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_set_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_set_source_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_set_source_surface(cr, sign_surf, FILL_X(col), FILL_Y(row));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	queue_draw_area(
		FILL_X(col), FILL_Y(col), width, height);

    }
# അല്ലെങ്കിൽ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_set_foreground(gui.text_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->window,
	    gui.text_gc,
	    TRUE,
	    FILL_X(col),
	    FILL_Y(row),
	    SIGN_WIDTH,
	    SIGN_HEIGHT);

    gdk_pixbuf_render_to_drawable_alpha(sign,
	    gui.drawarea->window,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(col) - MIN(0, xoffset),
	    FILL_Y(row) - MIN(0, yoffset),
	    MIN(width,	SIGN_WIDTH),
	    MIN(height, SIGN_HEIGHT),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_NORMAL,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    എങ്കിൽ (need_scale)
	g_object_unref(sign);
}

    ശൂന്യം *
gui_mch_register_sign(char_u *signfile)
{
    എങ്കിൽ (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = ശൂന്യ;
	char_u	    *message;

	sign = gdk_pixbuf_new_from_file((സ്ഥിരം അക്ഷരം *)signfile, &error);

	എങ്കിൽ (error == ശൂന്യ)
	    തിരികെ sign;

	message = (char_u *)error->message;

	എങ്കിൽ (message != ശൂന്യ && നിവേശം_conv.vc_type != CONV_NONE)
	    message = string_convert(&നിവേശം_conv, message, ശൂന്യ);

	എങ്കിൽ (message != ശൂന്യ)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", message);

	    എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
		vim_free(message);
	}
	g_error_free(error);
    }

    തിരികെ ശൂന്യ;
}

    ശൂന്യം
gui_mch_destroy_sign(ശൂന്യം *sign)
{
    എങ്കിൽ (sign != ശൂന്യ)
	g_object_unref(sign);
}

#endif // FEAT_SIGN_ICONS

#എങ്കിൽ defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

തരനിർവചനം ഘടന
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
    സ്ഥിതം ശൂന്യം
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->data;
    uint8_t *dst;
    പൂർണ്ണസംഖ്യ	    w = img->width;
    പൂർണ്ണസംഖ്യ	    h = img->height;
    പൂർണ്ണസംഖ്യ	    stride;
    പൂർണ്ണസംഖ്യ	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_data(surf);
    stride = cairo_image_surface_get_stride(surf);

    എങ്കിൽ (img->fmt == IMAGE_FORMAT_RGBA)
    {
	വേണ്ടി (y = 0; y < h; ++y)
	{
	    unsigned പൂർണ്ണസംഖ്യ *p = (unsigned പൂർണ്ണസംഖ്യ *)(dst + y * stride);

	    വേണ്ടി (x = 0; x < w; ++x)
	    {
		പൂർണ്ണസംഖ്യ	idx = (y * w + x) * 4;
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
    അല്ലെങ്കിൽ
    {
	വേണ്ടി (y = 0; y < h; ++y)
	{
	    unsigned പൂർണ്ണസംഖ്യ *p = (unsigned പൂർണ്ണസംഖ്യ *)(dst + y * stride);

	    വേണ്ടി (x = 0; x < w; ++x)
	    {
		പൂർണ്ണസംഖ്യ	idx = (y * w + x) * 3;
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

    പൂർണ്ണസംഖ്യ
image_gui_init(image_T *img)
{
    // We only support GTK3
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    എങ്കിൽ (ctx == ശൂന്യ)
	തിരികെ FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->width, img->height);

    എങ്കിൽ (cairo_surface_status(ctx->surf) != CAIRO_STATUS_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	പോകുക fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_data = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    തിരികെ OK;
fail:
    vim_free(ctx);
    തിരികെ FAIL;
# അല്ലെങ്കിൽ
    തിരികെ FAIL;
# endif
}

    ശൂന്യം
image_gui_uninit(image_T *img)
{
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = img->backend_data;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    പൂർണ്ണസംഖ്യ
image_placement_gui_init(image_placement_T *place UNUSED)
{
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    തിരികെ OK;
# അല്ലെങ്കിൽ
    തിരികെ FAIL;
# endif
}

    ശൂന്യം
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    ശൂന്യം
image_placement_gui_draw(image_placement_T *place)
{
# എങ്കിൽ GTK_CHECK_VERSION(3,0,0)
    image_T		    *img = place->img;
    image_cairo_T	    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    പൂർണ്ണസംഖ്യ			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    എങ്കിൽ (rects == ശൂന്യ || n_rects == 0)
	തിരികെ;

    cairo_surface_set_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    വേണ്ടി (പൂർണ്ണസംഖ്യ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	പൂർണ്ണസംഖ്യ		row, col;
	പൂർണ്ണസംഖ്യ		posx, posy;
	പൂർണ്ണസംഖ്യ		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(place, rect, &row, &col, &x, &y, &w, &h, false);
	posx = FILL_X(col);
	posy = FILL_Y(row);

	cairo_save(cr);
	cairo_set_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_set_source_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	queue_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    ശൂന്യം
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE
