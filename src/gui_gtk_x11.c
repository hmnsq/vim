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

#અન્તર્ભાવ "vim.h"
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
# જો defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
બાહ્ય શૂન્ય bonobo_dock_item_set_behavior(BonoboDockItem *dock_item, BonoboDockItemBehavior beh);
#endif

#જો defined(FEAT_GUI_GTK)
# જો GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# નહીંતર
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# નહીંતર
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
ગણના
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
સ્થિર અચલ GtkTargetEntry selection_targets[] =
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
સ્થિર અચલ GtkTargetEntry dnd_targets[] =
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

#જો defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#endif

#જો !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
સ્થિર GdkAtom wm_protocols_atom = GDK_NONE;
સ્થિર GdkAtom save_yourself_atom = GDK_NONE;
#endif

/*
 * Atoms used to control/reference X11 selections.
 */
સ્થિર GdkAtom html_atom = GDK_NONE;
સ્થિર GdkAtom utf8_string_atom = GDK_NONE;
સ્થિર GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
સ્થિર GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
સ્થિર GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
સ્થિર GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
સ્થિર સંરચના special_ચાવી
{
    guint ચાવી_sym;
    char_u code0;
    char_u code1;
}
અચલ special_ચાવીઓ[] =
{
    {GDK_Up,		'k', 'u'},
    {GDK_Down,		'k', 'd'},
    {GDK_ડાબું,		'k', 'l'},
    {GDK_જમણું,		'k', 'r'},
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
    {GDK_ઉમેરો,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_BackTab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_End,		'@', '7'},
    {GDK_Prior,		'k', 'P'},
    {GDK_Next,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_ડાબું,	'k', 'l'},
    {GDK_KP_જમણું,	'k', 'r'},
    {GDK_KP_Up,		'k', 'u'},
    {GDK_KP_Down,	'k', 'd'},
    {GDK_KP_ઉમેરો,	KS_EXTRA, (char_u)KE_KINS},
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
#define ARG_HAS_કિંમત	0x0100	// a value is expected after the argument
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
પ્રકારવ્યાખ્યા સંરચના
{
    અચલ અક્ષર	    *name;
    unsigned પૂર્ણાંક    flags;
}
cmdline_option_T;

સ્થિર અચલ cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_કિંમત},
    {"-font",		ARG_FONT|ARG_HAS_કિંમત},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_કિંમત},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_કિંમત},
    {"-rv",		ARG_REVERSE},
    {"-reverse",	ARG_REVERSE},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_કિંમત},
    {"-background",	ARG_BACKGROUND|ARG_HAS_કિંમત},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_કિંમત},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_કિંમત},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_કિંમત},
#ifdef FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_કિંમત},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_કિંમત},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_કિંમત},	// not implemented
#endif
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_કિંમત|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_કિંમત|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_કિંમત|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_કિંમત},
#ifdef FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_કિંમત},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# જો 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#endif
    {ખાલી, 0}
};

સ્થિર પૂર્ણાંક    gui_argc = 0;
સ્થિર અક્ષર **gui_argv = ખાલી;

સ્થિર અચલ અક્ષર *role_argument = ખાલી;
#જો defined(USE_GNOME_SESSION)
સ્થિર અચલ અક્ષર *restart_command = ખાલી;
સ્થિર       અક્ષર *abs_restart_command = ખાલી;
#endif
સ્થિર પૂર્ણાંક found_iconic_arg = FALSE;

#ifdef FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
સ્થિર પૂર્ણાંક using_gnome = 0;
#નહીંતર
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
# જો 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
પ્રકારવ્યાખ્યા સંરચના resize_history {
    પૂર્ણાંક used;		// If true, can't match for discard. Only matches once.
    પૂર્ણાંક width;
    પૂર્ણાંક height;
    પૂર્ણાંક from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    પૂર્ણાંક seq;		// for ch_log messages
# endif
    સંરચના resize_history *next;
} resize_hist_T;

// never NULL during execution
સ્થિર resize_hist_T *latest_resize_hist;
// list of stale resize requests
સ્થિર resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
સ્થિર પૂર્ણાંક mch_csd_width = 0;
સ્થિર પૂર્ણાંક mch_csd_height = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
સ્થિર પૂર્ણાંક mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    સ્થિર શૂન્ય
alloc_resize_hist(પૂર્ણાંક width, પૂર્ણાંક height, પૂર્ણાંક from_shellsize)
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
    ch_log(ખાલી, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, width, height, (પૂર્ણાંક)Columns, (પૂર્ણાંક)Rows);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    સ્થિર શૂન્ય
clear_resize_hists(શૂન્ય)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    પૂર્ણાંક		    i = 0;
# endif

    જો (latest_resize_hist)
	latest_resize_hist->used = TRUE;
    જ્યારે (old_resize_hists != ખાલી)
    {
	resize_hist_T *next_hist = old_resize_hists->next;

	vim_free(old_resize_hists);
	old_resize_hists = next_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ખાલી, "gui_gtk: free %d hists", i);
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
    સ્થિર પૂર્ણાંક
match_stale_width_height(પૂર્ણાંક width, પૂર્ણાંક height)
{
    resize_hist_T *hist = old_resize_hists;

    માટે (hist = old_resize_hists; hist != ખાલી; hist = hist->next)
	જો (MATCH_WIDTH_HEIGHT(hist, width, height))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(ખાલી, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->used = TRUE;
	    પાછા TRUE;
	}
    પાછા FALSE;
}

# જો defined(EXITFREE)
    સ્થિર શૂન્ય
free_all_resize_hist(શૂન્ય)
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
સ્થિર guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    શૂન્ય
gui_mch_prepare(પૂર્ણાંક *argc, અક્ષર **argv)
{
    અચલ cmdline_option_T  *option;
    પૂર્ણાંક			    i	= 0;
    પૂર્ણાંક			    len = 0;

#જો defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    જો (strchr(argv[0], G_DIR_SEPARATOR) != ખાલી)
    {
	char_u buf[MAXPATHL];

	જો (mch_FullName((char_u *)argv[0], buf, (પૂર્ણાંક)કદ(buf), TRUE) == OK)
	{
	    abs_restart_command = (અક્ષર *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#endif

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(અક્ષર *, *argc + 1);

    g_return_if_fail(gui_argv != ખાલી);

    gui_argv[gui_argc++] = argv[i++];

    જ્યારે (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	જો (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    ચાલુ;
	}

	// Look for argv[i] in cmdline_options[] table.
	માટે (option = &cmdline_options[0]; option->name != ખાલી; ++option)
	{
	    len = strlen(option->name);

	    જો (strncmp(argv[i], option->name, len) == 0)
	    {
		જો (argv[i][len] == '\0')
		    તોડો;
		// allow --foo=bar style
		જો (argv[i][len] == '=' && (option->flags & ARG_HAS_કિંમત))
		    તોડો;
#ifdef FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		જો (vim_strchr((char_u *)":=", argv[i][len]) != ખાલી
			&& (option->flags & ARG_INDEX_MASK) == ARG_NETBEANS)
		    તોડો;
#endif
	    }
	    નહીંતર જો ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (અક્ષર *)option->name;
		તોડો;
	    }
	}
	જો (option->name == ખાલી) // no match
	{
	    ++i;
	    ચાલુ;
	}

	જો (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	નહીંતર
	{
	    અક્ષર *કિંમત = ખાલી;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    જો (option->flags & ARG_HAS_કિંમત)
	    {
		જો (argv[i][len] == '=')
		    કિંમત = &argv[i][len + 1];
		નહીંતર જો (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    કિંમત = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    પસંદગી (option->flags & ARG_INDEX_MASK)
	    {
		સ્થિતિ ARG_REVERSE:
		    found_reverse_arg = TRUE;
		    તોડો;
		સ્થિતિ ARG_NOREVERSE:
		    found_reverse_arg = FALSE;
		    તોડો;
		સ્થિતિ ARG_FONT:
		    font_argument = કિંમત;
		    તોડો;
		સ્થિતિ ARG_GEOMETRY:
		    જો (કિંમત != ખાલી)
			gui.geom = vim_strsave((char_u *)કિંમત);
		    તોડો;
		સ્થિતિ ARG_BACKGROUND:
		    background_argument = કિંમત;
		    તોડો;
		સ્થિતિ ARG_FOREGROUND:
		    foreground_argument = કિંમત;
		    તોડો;
		સ્થિતિ ARG_ICONIC:
		    found_iconic_arg = TRUE;
		    તોડો;
		સ્થિતિ ARG_ROLE:
		    role_argument = કિંમત; // used later in gui_mch_open()
		    તોડો;
#ifdef FEAT_NETBEANS_INTG
		સ્થિતિ ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    તોડો;
#endif
		મૂળભૂત:
		    તોડો;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	જો ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	જો (option->flags & ARG_KEEP)
	    ++i;
	નહીંતર
	{
	    // Remove the flag from the argument vector.
	    જો (--*argc > i)
	    {
		પૂર્ણાંક n_strip = 1;

		// Move the argument's value as well, if there is one.
		જો ((option->flags & ARG_HAS_કિંમત)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    જો (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		જો (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * કદ(અક્ષર *));
	    }
	    argv[*argc] = ખાલી;
	}
    }

    gui_argv[gui_argc] = ખાલી;
}

#જો defined(EXITFREE)
    શૂન્ય
gui_mch_free_all(શૂન્ય)
{
    vim_free(gui_argv);
# જો defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#endif

#જો !GTK_CHECK_VERSION(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    સ્થિર gint
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
    જો (gui.text_gc != ખાલી)
	gdk_gc_set_exposures(gui.text_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    પાછા FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર gboolean
draw_event(GtkWidget *widget UNUSED,
	   cairo_t   *cr,
	   gpointer   user_data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    જો (gui.starting)
	પાછા FALSE;

    out_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_set_source_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    પાછા FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# નહીંતર
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# જો GTK_CHECK_VERSION(3,10,0)
    સ્થિર gboolean
scale_factor_event(GtkWidget *widget,
		   GParamSpec* pspec UNUSED,
		   gpointer   user_data UNUSED)
{
    જો (gui.surface != ખાલી)
	cairo_surface_destroy(gui.surface);

    પૂર્ણાંક	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    પૂર્ણાંક	    usable_height = h;
    જો (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_height);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  જો defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(widget);
#  endif

    પાછા TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#નહીંતર // !GTK_CHECK_VERSION(3,0,0)
    સ્થિર gint
expose_event(GtkWidget *widget UNUSED,
	     GdkEventExpose *event,
	     gpointer data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    જો (gui.starting)
	પાછા FALSE;

    out_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.width, event->area.height);

    // Clear the border areas if needed
    જો (event->area.x < FILL_X(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, FILL_X(0), 0);
    જો (event->area.y < FILL_Y(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, 0, FILL_Y(0));
    જો (event->area.x > FILL_X(Columns))
	gdk_window_clear_area(gui.drawarea->window,
			      FILL_X((પૂર્ણાંક)Columns), 0, 0, 0);
    જો (event->area.y > FILL_Y(Rows))
	gdk_window_clear_area(gui.drawarea->window, 0, FILL_Y((પૂર્ણાંક)Rows), 0, 0);

    પાછા FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#ifdef FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    સ્થિર gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer data UNUSED)
{
    જો (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (પૂર્ણાંક)GDK_PROPERTY_NEW_કિંમત
	    && GDK_WINDOW_XID(event->window) == commWindow
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.window = commWindow;
	xev.xproperty.state = Property_New_કિંમત;
	serverEventProc(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(widget)),
								      &xev, 0);
    }
    પાછા FALSE;
}
#endif // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    સ્થિર શૂન્ય
gtk_settings_xft_dpi_changed_cb(GtkSettings *gtk_settings UNUSED,
				    GParamSpec *pspec UNUSED,
				    gpointer data UNUSED)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    જો (gui.text_context != ખાલી)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    જો (gui.norm_font != ખાલી)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? ખાલી : p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

પ્રકારવ્યાખ્યા gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    સ્થિર guint
timeout_add(પૂર્ણાંક time, timeout_cb_type (*callback)(gpointer), પૂર્ણાંક *flagp)
{
    પાછા g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    સ્થિર શૂન્ય
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

સ્થિર પૂર્ણાંક blink_state = BLINK_NONE;
સ્થિર long_u blink_waittime = 700;
સ્થિર long_u blink_ontime = 400;
સ્થિર long_u blink_offtime = 250;
સ્થિર guint blink_timer = 0;

    પૂર્ણાંક
gui_mch_is_blinking(શૂન્ય)
{
    પાછા blink_state != BLINK_NONE;
}

    પૂર્ણાંક
gui_mch_is_blink_off(શૂન્ય)
{
    પાછા blink_state == BLINK_OFF;
}

    શૂન્ય
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    શૂન્ય
gui_mch_stop_blink(પૂર્ણાંક may_call_gui_update_cursor)
{
    જો (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    જો (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

    સ્થિર timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    જો (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, ખાલી);
    }
    નહીંતર
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, ખાલી);
    }
    gui_may_flush();

    પાછા FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    શૂન્ય
gui_mch_start_blink(શૂન્ય)
{
    જો (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    જો (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, ખાલી);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    સ્થિર gint
enter_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    જો (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes there
    જો (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    પાછા FALSE;
}

    સ્થિર gint
leave_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    જો (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    પાછા FALSE;
}

    સ્થિર gint
focus_in_event(GtkWidget *widget,
	       GdkEventFocus *event UNUSED,
	       gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    જો (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    નહીંતર
#endif
	gui_focus_change(TRUE);

    જો (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    જો (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    પાછા TRUE;
}

    સ્થિર gint
focus_out_event(GtkWidget *widget UNUSED,
		GdkEventFocus *event UNUSED,
		gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    જો (gui.dialogs_active > 0)
	++gui.dialog_focus_pending;
#endif

    gui_focus_change(FALSE);

    જો (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    પાછા TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    સ્થિર પૂર્ણાંક
keyval_to_string(unsigned પૂર્ણાંક keyval, char_u *string)
{
    પૂર્ણાંક	    len;
    guint32 uc;

    uc = gdk_keyval_to_unicode(keyval);
    જો (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((પૂર્ણાંક)uc, string);
    }
    નહીંતર
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	પસંદગી (keyval)
	{
	    સ્થિતિ GDK_Tab: સ્થિતિ GDK_KP_Tab: સ્થિતિ GDK_ISO_ડાબું_Tab:
		string[0] = TAB;
		તોડો;
	    સ્થિતિ GDK_Linefeed:
		string[0] = NL;
		તોડો;
	    સ્થિતિ GDK_Return: સ્થિતિ GDK_ISO_Enter: સ્થિતિ GDK_3270_Enter:
		string[0] = CAR;
		તોડો;
	    સ્થિતિ GDK_Escape:
		string[0] = ESC;
		તોડો;
	    મૂળભૂત:
		len = 0;
		તોડો;
	}
    }
    string[len] = NUL;

    પાછા len;
}

    સ્થિર પૂર્ણાંક
modifiers_gdk2vim(guint state)
{
    પૂર્ણાંક modifiers = 0;

    જો (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    જો (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    જો (state & GDK_MOD1_MASK)
	modifiers |= MOD_MASK_ALT;
#જો GTK_CHECK_VERSION(2,10,0)
    જો (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    જો (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;
#નહીંતર
    જો (state & GDK_MOD4_MASK)
	modifiers |= MOD_MASK_CMD;
#endif

    પાછા modifiers;
}

    સ્થિર પૂર્ણાંક
modifiers_gdk2mouse(guint state)
{
    પૂર્ણાંક modifiers = 0;

    જો (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    જો (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    જો (state & GDK_MOD1_MASK)
	modifiers |= MOUSE_ALT;

    પાછા modifiers;
}

/*
 * Main keyboard handler:
 */
    સ્થિર gint
ચાવી_press_event(GtkWidget *widget UNUSED,
		Gdk_Event_ચાવી *event,
		gpointer data UNUSED)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	ચાવી_sym;
    પૂર્ણાંક		len;
    પૂર્ણાંક		i;
    પૂર્ણાંક		modifiers;
    પૂર્ણાંક		ચાવી;
    guint	state;
    char_u	*s, *d;
    પૂર્ણાંક		ctrl_prefix_added = 0;

    gui.event_time = event->time;
    ચાવી_sym = event->keyval;
    state = event->state;

#ifdef FEAT_XIM
    જો (xim_queue_ચાવી_press_event(event, TRUE))
	પાછા TRUE;
#endif

#ifdef SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    જો (ચાવી_sym == SunXK_F36 || ચાવી_sym == SunXK_F37)
	len = 0;
    નહીંતર
#endif
    {
	len = keyval_to_string(ચાવી_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	જો (len > 1 && આગત_conv.vc_type != CONV_NONE)
	    len = convert_આગત(string2, len, કદ(string2));

	s = string2;
	d = string;
	માટે (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    જો (d[-1] == CSI && d + 2 < string + કદ(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (પૂર્ણાંક)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    જો (ચાવી_sym == GDK_ISO_ડાબું_Tab)
    {
	ચાવી_sym = GDK_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    જો ((state & GDK_MOD1_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	પાછા FALSE;
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    જો (len == 0 || len == 1)
    {
	માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	{
	    જો (special_ચાવીઓ[i].ચાવી_sym == ચાવી_sym)
	    {
		string[0] = CSI;
		string[1] = special_ચાવીઓ[i].code0;
		string[2] = special_ચાવીઓ[i].code1;
		len = -3;
		તોડો;
	    }
	}
    }

#ifdef GDK_ચાવી_dead_circumflex
    // Belgian Ctrl+[ workaround
    જો (len == 0 && ચાવી_sym == GDK_ચાવી_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_MASK_CTRL;
	string[3] = '[';
	len = 4;
	add_to_આગત_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	પાછા TRUE;
    }
#endif

    જો (len == 0)   // Unrecognized key
	પાછા TRUE;

    // For some keys a shift modifier is translated into another key code.
    જો (len == -3)
	ચાવી = TO_SPECIAL(string[1], string[2]);
    નહીંતર
    {
	string[len] = NUL;
	ચાવી = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    ચાવી = simplify_ચાવી(ચાવી, &modifiers);
    જો (ચાવી == CSI)
	ચાવી = K_CSI;
    જો (IS_SPECIAL(ચાવી))
    {
	string[0] = CSI;
	string[1] = K_SECOND(ચાવી);
	string[2] = K_THIRD(ચાવી);
	len = 3;
    }
    નહીંતર
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ચાવી = may_adjust_ચાવી_for_ctrl(modifiers, ચાવી);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_shift_modifier(modifiers, ચાવી);

	len = mb_char2bytes(ચાવી, string);
    }

    જો (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_આગત_buf(string2, 3);
	જો (modifiers == 0x4)
	    ctrl_prefix_added = 1;
    }

    // Check if the key interrupts.
    {
	પૂર્ણાંક int_ch = check_for_interrupt(ચાવી, modifiers);

	જો (int_ch != NUL)
	{
	    trash_આગત_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    જો (ctrl_prefix_added && len == 2
		    && ((પૂર્ણાંક)string[0]) == 195
		    && ((પૂર્ણાંક)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    add_to_આગત_buf(string, len);

    // blank out the pointer if necessary
    જો (p_mh)
	gui_mch_mousehide(TRUE);

    પાછા TRUE;
}

#જો defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    સ્થિર gboolean
ચાવી_release_event(GtkWidget *widget UNUSED,
		  Gdk_Event_ચાવી *event UNUSED,
		  gpointer data UNUSED)
{
# જો defined(FEAT_XIM)
    gui.event_time = event->time;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    પાછા xim_queue_ચાવી_press_event(event, FALSE);
# નહીંતર
    પાછા TRUE;
# endif
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
સ્થિર પૂર્ણાંક in_selection_clear_event = FALSE;

    સ્થિર gint
selection_clear_event(GtkWidget		*widget UNUSED,
		      GdkEventSelection	*event,
		      gpointer		user_data UNUSED)
{
    in_selection_clear_event = TRUE;
    જો (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    નહીંતર
	clip_lose_selection(&clip_star);
    in_selection_clear_event = FALSE;

    પાછા TRUE;
}

#define RS_NONE	0	// selection_received_cb() not called yet
#define RS_OK	1	// selection_received_cb() called and OK
#define RS_FAIL	2	// selection_received_cb() called and failed
સ્થિર પૂર્ણાંક received_selection = RS_NONE;

    સ્થિર શૂન્ય
selection_received_cb(GtkWidget		*widget UNUSED,
		      GtkSelectionData	*data,
		      guint		time_ UNUSED,
		      gpointer		user_data UNUSED)
{
    Clipboard_T	    *cbd;
    char_u	    *text;
    char_u	    *tmpbuf = ખાલી;
    guchar	    *tmpbuf_utf8 = ખાલી;
    પૂર્ણાંક		    len;
    પૂર્ણાંક		    motion_type = MAUTO;

    જો (gtk_selection_data_get_selection(data) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    નહીંતર
	cbd = &clip_star;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    જો (text == ખાલી || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	પાછા;
    }

    જો (gtk_selection_data_get_data_type(data) == vim_atom
	    || gtk_selection_data_get_data_type(data) == vim_mt_atom)
    {
	motion_type = *text++;
	--len;
    }
    નહીંતર જો (gtk_selection_data_get_data_type(data) == vimenc_atom
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
	જો (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, text, &len);
	    જો (tmpbuf != ખાલી)
		text = tmpbuf;
	    convert_setup(&conv, ખાલી, ખાલી);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    નહીંતર
    {
	tmpbuf_utf8 = gtk_selection_data_get_text(data);
	જો (tmpbuf_utf8 != ખાલી)
	{
	    len = STRLEN(tmpbuf_utf8);
	    જો (આગત_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&આગત_conv, tmpbuf_utf8, &len);
		જો (tmpbuf != ખાલી)
		    text = tmpbuf;
	    }
	    નહીંતર
		text = tmpbuf_utf8;
	}
	નહીંતર જો (len >= 2 && text[0] == 0xff && text[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_setup_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    જો (conv.vc_type != CONV_NONE)
	    {
		text += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, text, &len);
		convert_setup(&conv, ખાલી, ખાલી);
	    }
	    જો (tmpbuf != ખાલી)
		text = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    જ્યારે (len > 0 && text[len - 1] == NUL)
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
    સ્થિર શૂન્ય
selection_get_cb(GtkWidget	    *widget UNUSED,
		 GtkSelectionData   *selection_data,
		 guint		    info,
		 guint		    time_ UNUSED,
		 gpointer	    user_data UNUSED)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    પૂર્ણાંક		    length;
    પૂર્ણાંક		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    જો (gtk_selection_data_get_selection(selection_data)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    નહીંતર
	cbd = &clip_star;

    જો (!cbd->owned)
	પાછા;			// Shouldn't ever happen

    જો (info != (guint)TARGET_STRING
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
	પાછા;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    જો (motion_type < 0 || string == ખાલી)
	પાછા;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    length = MIN(tmplen, (long_u)(G_MAXINT - 1));

    જો (info == (guint)TARGET_VIM || info == (guint)TARGET_VIM_MT)
    {
	tmpbuf = alloc(length + 1);
	જો (tmpbuf != ખાલી)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)length);
	}
	// For our own format, the first byte contains the motion type
	++length;
	vim_free(string);
	string = tmpbuf;
	જો (info == (guint)TARGET_VIM)
	    type = vim_atom;
	નહીંતર
	    type = vim_mt_atom;
    }

    નહીંતર જો (info == (guint)TARGET_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_setup_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	જો (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &length);
	    convert_setup(&conv, ખાલી, ખાલી);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	જો (string != ખાલી)
	{
	    tmpbuf = alloc(length + 2);
	    જો (tmpbuf != ખાલી)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)length);
		vim_free(string);
		string = tmpbuf;
		length += 2;
	    }

#જો !GTK_CHECK_VERSION(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_data->type = selection_data->target;
	    selection_data->format = 16;	// 16 bits per char
#endif
	    gtk_selection_data_set(selection_data, html_atom, 16,
							      string, length);
	    vim_free(string);
	}
	પાછા;
    }
    નહીંતર જો (info == (guint)TARGET_VIMENC || info == (guint)TARGET_VIMENC_MT)
    {
	પૂર્ણાંક l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(length + l + 2);
	જો (tmpbuf != ખાલી)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)length);
	    length += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	જો (info == (guint)TARGET_VIMENC)
	    type = vimenc_atom;
	નહીંતર
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    નહીંતર
    {
	જો (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &length);
	    vim_free(string);
	    જો (tmpbuf == ખાલી)
		પાછા;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	જો (g_utf8_validate((અચલ અક્ષર *)string, (gssize)length, ખાલી))
	{
	    gtk_selection_data_set_text(selection_data,
					(અચલ અક્ષર *)string, length);
	}
	vim_free(string);
	પાછા;
    }

    જો (string != ખાલી)
    {
#જો !GTK_CHECK_VERSION(3,0,0)
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
    પૂર્ણાંક
gui_mch_early_init_check(પૂર્ણાંક give_message)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    જો ((p == ખાલી || *p == NUL) && (q == ખાલી || *q == NUL))
    {
	gui.dying = true;
	જો (give_message)
	    emsg(_((અક્ષર *)e_cannot_open_display));
	પાછા FAIL;
    }
    પાછા OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    પૂર્ણાંક
gui_mch_init_check(શૂન્ય)
{
#ifdef USE_GRESOURCE
    સ્થિર પૂર્ણાંક res_registered = FALSE;

    જો (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_register_resource();
    }
#endif

#ifdef FEAT_GUI_GNOME
    જો (gtk_socket_id == 0)
	using_gnome = 1;
#endif

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_set_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    જો (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((અક્ષર *)e_cannot_open_display));
	પાછા FAIL;
    }

    પાછા OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


સ્થિર guint mouse_click_timer = 0;
સ્થિર પૂર્ણાંક mouse_timed_out = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    સ્થિર timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    // we don't use this information currently
    પૂર્ણાંક *timed_out = (પૂર્ણાંક *) data;

    *timed_out = TRUE;
    પાછા FALSE;		// don't happen again
}

સ્થિર guint		motion_repeat_timer  = 0;
સ્થિર પૂર્ણાંક		motion_repeat_offset = FALSE;
સ્થિર timeout_cb_type	motion_repeat_timer_cb(gpointer);

    સ્થિર શૂન્ય
process_motion_notify(પૂર્ણાંક x, પૂર્ણાંક y, GdkModifierType state)
{
    પૂર્ણાંક	    button;
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
    જો (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	પાછા;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_allocation(gui.drawarea, &allocation);

    જો (x < 0 || y < 0
	    || x >= allocation.width
	    || y >= allocation.height)
    {

	પૂર્ણાંક dx;
	પૂર્ણાંક dy;
	પૂર્ણાંક offshoot;
	પૂર્ણાંક delay = 10;

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
	જો (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	નહીંતર
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	જો (!motion_repeat_timer)
	    motion_repeat_timer = timeout_add(delay, motion_repeat_timer_cb,
									 ખાલી);
    }
}

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર GdkDevice *
gui_gtk_get_pointer_device(GtkWidget *widget)
{
    GdkWindow * અચલ win = gtk_widget_get_window(widget);
    GdkDisplay * અચલ dpy = gdk_window_get_display(win);
# જો GTK_CHECK_VERSION(3,20,0)
    GdkSeat * અચલ seat = gdk_display_get_default_seat(dpy);
    પાછા gdk_seat_get_pointer(seat);
# નહીંતર
    GdkDeviceManager * અચલ mngr = gdk_display_get_device_manager(dpy);
    પાછા gdk_device_manager_get_client_pointer(mngr);
# endif
}

    સ્થિર GdkWindow *
gui_gtk_get_pointer(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    GdkWindow * અચલ win = gtk_widget_get_window(widget);
    GdkDevice * અચલ dev = gui_gtk_get_pointer_device(widget);
    પાછા gdk_window_get_device_position(win, dev , x, y, state);
}

# જો defined(FEAT_GUI_TABLINE)
    સ્થિર GdkWindow *
gui_gtk_window_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * અચલ dev = gui_gtk_get_pointer_device(widget);
    પાછા gdk_device_get_window_at_position(dev, x, y);
}
# endif
#નહીંતર // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_pointer(wid, x, y, s) \
    gdk_window_get_pointer((wid)->window, x, y, s)
# define gui_gtk_window_at_position(wid, x, y)	gdk_window_at_pointer(x, y)
#endif

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    સ્થિર timeout_cb_type
motion_repeat_timer_cb(gpointer data UNUSED)
{
    પૂર્ણાંક		    x;
    પૂર્ણાંક		    y;
    GdkModifierType state;

    gui_gtk_get_pointer(gui.drawarea, &x, &y, &state);

    જો (!(state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
		   GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
		   GDK_BUTTON5_MASK)))
    {
	motion_repeat_timer = 0;
	પાછા FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    જો (vim_used_in_આગત_buf() > 10)
	પાછા TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    જો (motion_repeat_offset)
	x += gui.char_width;

    motion_repeat_offset = !motion_repeat_offset;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    પાછા FALSE;
}

    સ્થિર gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer data UNUSED)
{
    જો (event->is_hint)
    {
	પૂર્ણાંક		x;
	પૂર્ણાંક		y;
	GdkModifierType	state;

	gui_gtk_get_pointer(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    નહીંતર
    {
	process_motion_notify((પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
			      (GdkModifierType)event->state);
    }

    પાછા TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    સ્થિર gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer data UNUSED)
{
    પૂર્ણાંક button;
    પૂર્ણાંક repeated_click = FALSE;
    પૂર્ણાંક x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

    // Make sure we have focus now we've been selected
    જો (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    જો (event->type != GDK_BUTTON_PRESS)
	પાછા FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    જો (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_out);

    પસંદગી (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	સ્થિતિ 1:
		button = MOUSE_ડાબું;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_MASK;
		તોડો;
	સ્થિતિ 2: button = MOUSE_મધ્ય; તોડો;
	સ્થિતિ 3: button = MOUSE_જમણું; તોડો;
	સ્થિતિ 8: button = MOUSE_X1; તોડો;
	સ્થિતિ 9: button = MOUSE_X2; તોડો;
	મૂળભૂત:
	    પાછા FALSE;		// Unknown button
    }

#ifdef FEAT_XIM
    // cancel any preediting
    જો (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    પાછા TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    સ્થિર gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer data UNUSED)
{
    પૂર્ણાંક	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#જો GTK_CHECK_VERSION(3,4,0)
    સ્થિર દ્વિગુણ  acc_x, acc_y;
# જો !GTK_CHECK_VERSION(3,22,0)
    સ્થિર guint32 last_smooth_event_time;
# endif
#endif

    જો (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    પસંદગી (event->direction)
    {
	સ્થિતિ GDK_SCROLL_UP:
	    button = MOUSE_4;
	    તોડો;
	સ્થિતિ GDK_SCROLL_DOWN:
	    button = MOUSE_5;
	    તોડો;
	સ્થિતિ GDK_SCROLL_ડાબું:
	    button = MOUSE_7;
	    તોડો;
	સ્થિતિ GDK_SCROLL_જમણું:
	    button = MOUSE_6;
	    તોડો;
#જો GTK_CHECK_VERSION(3,4,0)
	સ્થિતિ GDK_SCROLL_SMOOTH:
	    જો (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		પાછા FALSE;
	    }
# જો GTK_CHECK_VERSION(3,22,0)
	    જો (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# નહીંતર
	    જો (event->time - last_smooth_event_time > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# જો !GTK_CHECK_VERSION(3,22,0)
	    last_smooth_event_time = event->time;
# endif
	    તોડો;
#endif
	મૂળભૂત: // This shouldn't happen
	    પાછા FALSE;
    }

#ifdef FEAT_XIM
    // cancel any preediting
    જો (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

#જો GTK_CHECK_VERSION(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    જો (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	જ્યારે (acc_x >= 1.0)
	{ //જમણું
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
		    FALSE, vim_modifiers);
	}
	જ્યારે (acc_x <= -1.0)
	{ //ડાબું
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
		    FALSE, vim_modifiers);
	}
	જ્યારે (acc_y >= 1.0)
	{ // down
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
		    FALSE, vim_modifiers);
	}
	જ્યારે (acc_y <= -1.0)
	{ // up
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
		    FALSE, vim_modifiers);
	}
    }
    નહીંતર
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    જો (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    નહીંતર
#endif
	gui_send_mouse_event(button, (પૂર્ણાંક)event->x, (પૂર્ણાંક)event->y,
		FALSE, vim_modifiers);

    પાછા TRUE;
}


    સ્થિર gint
button_release_event(GtkWidget *widget UNUSED,
		     GdkEventButton *event,
		     gpointer data UNUSED)
{
    પૂર્ણાંક x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    જો (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    પસંદગી (event->button)
    {
	સ્થિતિ 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    તોડો;
    }

    પાછા TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    સ્થિર શૂન્ય
grab_notify_event(GtkWidget *widget UNUSED,
		  gboolean  was_grabbed,
		  gpointer  data UNUSED)
{
    જો (!was_grabbed)
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
    સ્થિર પૂર્ણાંક
ગણતરી_and_decode_uri_list(char_u *out, char_u *raw, પૂર્ણાંક len)
{
    પૂર્ણાંક		i;
    char_u	*p = out;
    પૂર્ણાંક		ગણતરી = 0;

    માટે (i = 0; i < len; ++i)
    {
	જો (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    જો (p > out && p[-1] != NUL)
	    {
		++ગણતરી;
		*p++ = NUL;
	    }
	}
	નહીંતર જો (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	નહીંતર
	    *p++ = raw[i];
    }
    જો (p > out && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++ગણતરી;
    }
    પાછા ગણતરી;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    સ્થિર પૂર્ણાંક
filter_uri_list(char_u **outlist, પૂર્ણાંક max, char_u *src)
{
    પૂર્ણાંક	i, j;

    માટે (i = j = 0; i < max; ++i)
    {
	outlist[i] = ખાલી;
	જો (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    જો (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    જ્યારે (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    પાછા j;
}

    સ્થિર char_u **
parse_uri_list(પૂર્ણાંક *ગણતરી, char_u *data, પૂર્ણાંક len)
{
    પૂર્ણાંક	    n	    = 0;
    char_u  *tmp    = ખાલી;
    char_u  **array = ખાલી;

    જો (data != ખાલી && len > 0 && (tmp = alloc(len + 1)) != ખાલી)
    {
	n = ગણતરી_and_decode_uri_list(tmp, data, len);
	જો (n > 0 && (array = ALLOC_MULT(char_u *, n)) != ખાલી)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *ગણતરી = n;
    પાછા array;
}

    સ્થિર શૂન્ય
drag_handle_uri_list(GdkDragContext	*context,
		     GtkSelectionData	*data,
		     guint		time_,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    પૂર્ણાંક	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_data_get_data(data),
	    gtk_selection_data_get_length(data));

    જો (fnames != ખાલી && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_finish(context, TRUE, FALSE, time_); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    નહીંતર
	vim_free(fnames);
}

    સ્થિર શૂન્ય
drag_handle_text(GdkDragContext	    *context,
		 GtkSelectionData   *data,
		 guint		    time_,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *text;
    પૂર્ણાંક	    len;
    char_u  *tmpbuf = ખાલી;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    જો (gtk_selection_data_get_data_type(data) == utf8_string_atom)
    {
	જો (આગત_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&આગત_conv, text, &len);
	જો (tmpbuf != ખાલી)
	    text = tmpbuf;
    }

    dnd_yank_drag_data(text, (long)len);
    gtk_drag_finish(context, TRUE, FALSE, time_); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    જો (dropkey[2] != 0)
	add_to_આગત_buf(dropkey, (પૂર્ણાંક)કદ(dropkey));
    નહીંતર
	add_to_આગત_buf(dropkey + 3, (પૂર્ણાંક)(કદ(dropkey) - 3));
}

/*
 * DND receiver.
 */
    સ્થિર શૂન્ય
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
    અચલ guchar * અચલ data_data = gtk_selection_data_get_data(data);
    અચલ gint data_length = gtk_selection_data_get_length(data);
    અચલ gint data_format = gtk_selection_data_get_format(data);

    જો (data_data == ખાલી
	    || data_length <= 0
	    || data_format != 8
	    || data_data[data_length] != '\0')
    {
	gtk_drag_finish(context, FALSE, FALSE, time_);
	પાછા;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_pointer(widget, ખાલી, ખાલી, &state);

    // Not sure about the role of "text/plain" here...
    જો (info == (guint)TARGET_TEXT_URI_LIST)
	drag_handle_uri_list(context, data, time_, state, x, y);
    નહીંતર
	drag_handle_text(context, data, time_, state);

}
#endif // FEAT_DND


#જો defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    સ્થિર શૂન્ય
sm_client_check_changed_any(GnomeClient	    *client UNUSED,
			    gint	    ચાવી,
			    GnomeDialogType type UNUSED,
			    gpointer	    data UNUSED)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# જો defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
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
    gnome_interaction_ચાવી_return(ચાવી, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    સ્થિર gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase UNUSED,
			GnomeSaveStyle	    save_style UNUSED,
			gboolean	    shutdown UNUSED,
			GnomeInteractStyle  interact_style,
			gboolean	    fast UNUSED,
			gpointer	    data UNUSED)
{
    સ્થિર અચલ અક્ષર	suffix[] = "-session.vim";
    અક્ષર		*session_file;
    unsigned પૂર્ણાંક	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    જો (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_NORMAL,
					 &sm_client_check_changed_any,
					 ખાલી);
    out_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_path(
			gnome_client_get_config_prefix(client));
    len = strlen(session_file);

    જો (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(અક્ષર, session_file, len + કદ(suffix));
    memcpy(session_file + len, suffix, કદ(suffix));

    success = write_session_file((char_u *)session_file);

    જો (success)
    {
	અચલ અક્ષર  *argv[8];
	પૂર્ણાંક	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = ખાલી;

	gnome_client_set_discard_command(client, i, (અક્ષર **)argv);

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
	argv[i] = ખાલી;

	gnome_client_set_restart_command(client, i, (અક્ષર **)argv);
	gnome_client_set_clone_command(client, 0, ખાલી);
    }

    g_free(session_file);

    પાછા success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
setup_save_yourself(શૂન્ય)
{
    GnomeClient *client;

    client = gnome_master_client();
    જો (client == ખાલી)
	પાછા;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), ખાલી);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), ખાલી);
}

#નહીંતર // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    સ્થિર gboolean
local_xsmp_handle_requests(
    GIOChannel		*source UNUSED,
    GIOCondition	condition,
    gpointer		data)
{
    જો (condition == G_IO_IN)
    {
	// Do stuff; maybe close connection
	જો (xsmp_handle_requests() == FAIL)
	    g_io_channel_unref((GIOChannel *)data);
	પાછા TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)data);
    xsmp_close();
    પાછા TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    સ્થિર શૂન્ય
setup_save_yourself(શૂન્ય)
{
    Atom    *existing_atoms = ખાલી;
    પૂર્ણાંક	    ગણતરી = 0;

# ifdef USE_XSMP
    જો (xsmp_icefd != -1)
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
    નહીંતર
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	જો (!dpy)
	    પાછા;

	GdkWindow * અચલ mainwin_win = gtk_widget_get_window(gui.mainwin);
	જો (XGetWMProtocols(dpy, GDK_WINDOW_XID(mainwin_win),
		    &existing_atoms, &ગણતરી))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    પૂર્ણાંક	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    માટે (i = 0; i < ગણતરી; ++i)
		જો (existing_atoms[i] == save_yourself_xatom)
		    તોડો;

	    જો (i == ગણતરી)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, ગણતરી + 1);
		જો (new_atoms != ખાલી)
		{
		    memcpy(new_atoms, existing_atoms, ગણતરી * કદ(Atom));
		    new_atoms[ગણતરી] = save_yourself_xatom;
		    XSetWMProtocols(GDK_WINDOW_XDISPLAY(mainwin_win),
			    GDK_WINDOW_XID(mainwin_win),
			    new_atoms, ગણતરી + 1);
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
    સ્થિર GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event UNUSED,
		    gpointer data UNUSED)
{
    XEvent *xevent = (XEvent *)xev;

    જો (xevent != ખાલી
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
		    ખાલી, 0);
	પાછા GDK_FILTER_REMOVE;
    }

    પાછા GDK_FILTER_CONTINUE;
}
#endif // !USE_GNOME_SESSION

    સ્થિર GdkPixbuf *
pixbuf_new_from_png_data(અચલ unsigned અક્ષર *data, unsigned પૂર્ણાંક len)
{
    GInputStream *stream;
    GdkPixbuf    *pixbuf;

    stream = g_memory_આગત_stream_new_from_data(data, len, ખાલી);
    pixbuf = gdk_pixbuf_new_from_stream(stream, ખાલી, ખાલી);
    g_object_unref(stream);
    પાછા pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    સ્થિર શૂન્ય
mainwin_realize(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#અન્તર્ભાવ "../runtime/vim16x16_png.h"
#અન્તર્ભાવ "../runtime/vim32x32_png.h"
#અન્તર્ભાવ "../runtime/vim48x48_png.h"

    GdkWindow * અચલ mainwin_win = gtk_widget_get_window(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    જો (echo_wid_arg)
    {
	જો (gui_mch_get_display())
	    printf("WID: %ld\n", (long)GDK_WINDOW_XID(mainwin_win));
	નહીંતર
	    printf("WID: 0\n");
	fflush(stdout);
    }

    જો (vim_strchr(p_go, GO_ICON) != ખાલી)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	જો (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_window_set_icon_name(GTK_WINDOW(gui.mainwin), "gvim");
	નહીંતર
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = ખાલી;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim48x48_png, vim48x48_png_len));

	    gtk_window_set_icon_list(GTK_WINDOW(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(શૂન્ય *)&g_object_unref, ખાલી);
	    g_list_free(icons);
	}
    }

#જો !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_window_add_filter(ખાલી, &global_event_filter, ખાલી);
#endif
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#જો defined(USE_GNOME_SESSION)
    જો (using_gnome)
#endif
	setup_save_yourself();

#ifdef FEAT_CLIENTSERVER
    જો (clientserver_method == CLIENTSERVER_METHOD_X11 && gui_mch_get_display())
    {
	જો (serverName == ખાલી && serverDelayedStartName != ખાલી)
	{
	    // This is a :gui command in a plain vim with no previous server
	    commWindow = GDK_WINDOW_XID(mainwin_win);

	    (શૂન્ય)serverRegisterName(GDK_WINDOW_XDISPLAY(mainwin_win),
				    serverDelayedStartName);
	}
	નહીંતર
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
			G_CALLBACK(property_event), ખાલી);
    }
#endif
}

    સ્થિર GdkCursor *
create_blank_pointer(શૂન્ય)
{
    GdkCursor	*cursor;

#જો GTK_CHECK_VERSION(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#નહીંતર
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    GdkWindow	*મૂળ_window = ખાલી;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_mask;
    અક્ષર	blank_data[] = { 0x0 };
    મૂળ_window = gtk_widget_get_મૂળ_window(gui.mainwin);
    blank_mask = gdk_bitmap_create_from_data(મૂળ_window, blank_data, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_mask, blank_mask,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_mask);
#endif

    પાછા cursor;
}

    સ્થિર શૂન્ય
mainwin_screen_changed_cb(GtkWidget  *widget,
			  GdkScreen  *previous_screen UNUSED,
			  gpointer   data UNUSED)
{
    જો (!gtk_widget_has_screen(widget))
	પાછા;

    /*
     * Recreate the invisible mouse cursor.
     */
    જો (gui.blank_pointer != ખાલી)
#જો GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_pointer));
#નહીંતર
	gdk_cursor_unref(gui.blank_pointer);
#endif

    gui.blank_pointer = create_blank_pointer();

    જો (gui.pointer_hidden && gtk_widget_get_window(gui.drawarea) != ખાલી)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    જો (gui.text_context != ખાલી)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(widget);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    જો (gui.norm_font != ખાલી)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    સ્થિર gboolean
mainwin_state_event_cb(GtkWidget *widget UNUSED,
		       GdkEventWindowState *event,
		       gpointer user_data UNUSED)
{
    જો (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    પાછા FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    સ્થિર શૂન્ય
drawarea_realize_cb(GtkWidget *widget, gpointer data UNUSED)
{
    GtkWidget *sbar;
    GtkAllocation allocation;

#ifdef FEAT_XIM
    xim_init();
#endif
    gui_mch_new_colors();
#જો GTK_CHECK_VERSION(3,0,0)
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_width(widget),
	    gtk_widget_get_allocated_height(widget));
#નહીંતર
    gui.text_gc = gdk_gc_new(gui.drawarea->window);
#endif

    gui.blank_pointer = create_blank_pointer();
    જો (gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(widget), gui.blank_pointer);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_ડાબું].id;
    જો (!sbar || (!gui.which_scrollbars[SBAR_ડાબું]
				    && firstwin->w_scrollbars[SBAR_જમણું].id))
	sbar = firstwin->w_scrollbars[SBAR_જમણું].id;
    gtk_widget_get_allocation(sbar, &allocation);
    જો (sbar && gtk_widget_get_realized(sbar) && allocation.width)
	gui.scrollbar_width = allocation.width;

    sbar = gui.bottom_sbar.id;
    જો (sbar && gtk_widget_get_realized(sbar) && allocation.height)
	gui.scrollbar_height = allocation.height;
}

/*
 * Properly clean up on shutdown.
 */
    સ્થિર શૂન્ય
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

#ifdef FEAT_XIM
    im_shutdown();
#endif
    જો (gui.ascii_glyphs != ખાલી)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = ખાલી;
    }
    જો (gui.ascii_font != ખાલી)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = ખાલી;
    }
    g_object_unref(gui.text_context);
    gui.text_context = ખાલી;

#જો GTK_CHECK_VERSION(3,0,0)
    જો (gui.surface != ખાલી)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = ખાલી;
    }
#નહીંતર
    g_object_unref(gui.text_gc);
    gui.text_gc = ખાલી;
#endif

#જો GTK_CHECK_VERSION(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_pointer));
#નહીંતર
    gdk_cursor_unref(gui.blank_pointer);
#endif
    gui.blank_pointer = ખાલી;
}

#જો GTK_CHECK_VERSION(3,22,2)
    સ્થિર શૂન્ય
drawarea_style_updated_cb(GtkWidget *widget UNUSED,
			 gpointer data UNUSED)
#નહીંતર
    સ્થિર શૂન્ય
drawarea_style_set_cb(GtkWidget	*widget UNUSED,
		      GtkStyle	*previous_style UNUSED,
		      gpointer	data UNUSED)
#endif
{
    gui_mch_new_colors();
}

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       data UNUSED)
{
    સ્થિર પૂર્ણાંક cur_width = 0;
    સ્થિર પૂર્ણાંક cur_height = 0;

    g_return_val_if_fail(event
	    && event->width >= 1 && event->height >= 1, TRUE);

# જો GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
    જો (event->send_event == FALSE)
	પાછા TRUE;
# endif

    જો (event->width == cur_width && event->height == cur_height)
	પાછા TRUE;

    cur_width = event->width;
    cur_height = event->height;

    જો (gui.surface != ખાલી)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->width, event->height);

    gtk_widget_queue_draw(widget);

    પાછા TRUE;
}
#endif

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    સ્થિર gint
delete_event_cb(GtkWidget *widget UNUSED,
		GdkEventAny *event UNUSED,
		gpointer data UNUSED)
{
    gui_shell_closed();
    પાછા TRUE;
}

#જો defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    સ્થિર પૂર્ણાંક
get_item_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation item_orientation = GTK_ORIENTATION_HORIZONTAL;

    જો (using_gnome && widget != ખાલી)
    {
	GtkWidget *parent;
	BonoboDockItem *dockitem;

	parent	 = gtk_widget_get_parent(widget);
	જો (G_TYPE_FROM_INSTANCE(parent) == BONOBO_TYPE_DOCK_ITEM)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = parent;
	    dockitem = BONOBO_DOCK_ITEM(widget);

	    જો (dockitem == ખાલી || dockitem->is_floating)
		પાછા 0;
	    item_orientation = bonobo_dock_item_get_orientation(dockitem);
	}
    }
# નહીંતર
#  define item_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    જો (widget != ખાલી
	    && item_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# જો GTK_CHECK_VERSION(3,0,0) || !defined(FEAT_GUI_GNOME)
	GtkAllocation allocation;

	gtk_widget_get_allocation(widget, &allocation);
	જો (allocation.height > 1)
	    પાછા allocation.height;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  જો GTK_CHECK_VERSION(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_height(widget, &min_h, &natural_h);
	    પાછા MAX(min_h, natural_h);
	}
#  નહીંતર
	પાછા allocation.height;
#  endif
# નહીંતર
	જો (orientation == GTK_ORIENTATION_HORIZONTAL)
	    પાછા widget->allocation.height;
	નહીંતર
	    પાછા widget->allocation.width;
# endif
    }
    પાછા 0;
}
#endif

    સ્થિર પૂર્ણાંક
get_menu_tool_width(શૂન્ય)
{
    પૂર્ણાંક width = 0;

#ifdef FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    width += get_item_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    width += get_item_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    જો (gui.tabline != ખાલી)
	width += get_item_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#endif

    પાછા width;
}

    સ્થિર પૂર્ણાંક
get_menu_tool_height(શૂન્ય)
{
    પૂર્ણાંક height = 0;

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
    જો (gui.tabline != ખાલી
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	height += get_item_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#endif

    પાછા height;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
સ્થિર પૂર્ણાંક init_window_hints_state = 0;

    સ્થિર શૂન્ય
update_window_manager_hints(પૂર્ણાંક force_width, પૂર્ણાંક force_height)
{
    સ્થિર પૂર્ણાંક old_width  = 0;
    સ્થિર પૂર્ણાંક old_height = 0;
    સ્થિર પૂર્ણાંક old_min_width  = 0;
    સ્થિર પૂર્ણાંક old_min_height = 0;
    સ્થિર પૂર્ણાંક old_char_width  = 0;
    સ્થિર પૂર્ણાંક old_char_height = 0;

    પૂર્ણાંક width;
    પૂર્ણાંક height;
    પૂર્ણાંક min_width;
    પૂર્ણાંક min_height;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    જો (!(force_width && force_height)  &&  init_window_hints_state > 0)
    {
	// Don't do it!
	init_window_hints_state = 2;
	પાછા;
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
    જો (force_width && force_height)
    {
	min_width  = force_width;
	min_height = force_height;
    }
    નહીંતર
    {
	min_width  = width  + MIN_COLUMNS * gui.char_width;
	min_height = height + MIN_લીટીઓ   * gui.char_height;
    }

    // Avoid an expose event when the size didn't change.
    જો (width != old_width
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
	gtk_window_set_geometry_hints(GTK_WINDOW(gui.mainwin), ખાલી,
				      &geometry, geometry_mask);
	old_width       = width;
	old_height      = height;
	old_min_width   = min_width;
	old_min_height  = min_height;
	old_char_width  = gui.char_width;
	old_char_height = gui.char_height;
    }
}

#જો defined(FEAT_GUI_DARKTHEME)
    શૂન્ય
gui_mch_set_dark_theme(પૂર્ણાંક dark)
{
# જો GTK_CHECK_VERSION(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_set(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, ખાલી);
# endif
}
#endif // FEAT_GUI_DARKTHEME

#ifdef FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    સ્થિર શૂન્ય
icon_size_changed_foreach(GtkWidget *widget, gpointer user_data)
{
    જો (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# જો GTK_CHECK_VERSION(3,10,0)
	જો (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    અચલ GtkIconSize icon_size = GPOINTER_TO_INT(user_data);
	    અચલ gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, ખાલી);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_size);
	}
# નહીંતર
	// User-defined icons are stored in a GtkIconSet
	જો (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_SET)
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
    નહીંતર જો (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_size_changed_foreach,
			      user_data);
    }
}

    સ્થિર શૂન્ય
set_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    GtkIconSize	    size;
    GtkIconSize	    oldsize;

    જો ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    નહીંતર જો ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    નહીંતર જો (toolbar_flags & TOOLBAR_TEXT)
	style = GTK_TOOLBAR_TEXT;
    નહીંતર
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_set_style(toolbar, style);
# જો !GTK_CHECK_VERSION(3,0,0)
    gtk_toolbar_set_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    પસંદગી (tbis_flags)
    {
	સ્થિતિ TBIS_TINY:	    size = GTK_ICON_SIZE_MENU;		તોડો;
	સ્થિતિ TBIS_SMALL:    size = GTK_ICON_SIZE_SMALL_TOOLBAR;	તોડો;
	સ્થિતિ TBIS_MEDIUM:   size = GTK_ICON_SIZE_BUTTON;	તોડો;
	સ્થિતિ TBIS_LARGE:    size = GTK_ICON_SIZE_LARGE_TOOLBAR;	તોડો;
	સ્થિતિ TBIS_HUGE:     size = GTK_ICON_SIZE_DND;		તોડો;
	સ્થિતિ TBIS_GIANT:    size = GTK_ICON_SIZE_DIALOG;	તોડો;
	મૂળભૂત:	    size = GTK_ICON_SIZE_INVALID;	તોડો;
    }
    oldsize = gtk_toolbar_get_icon_size(toolbar);

    જો (size == GTK_ICON_SIZE_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_size(toolbar);
	size = gtk_toolbar_get_icon_size(toolbar);
    }
    જો (size != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_size_changed_foreach,
			      GINT_TO_POINTER((પૂર્ણાંક)size));
    }
    gtk_toolbar_set_icon_size(toolbar, size);
}

#endif // FEAT_TOOLBAR

#જો defined(FEAT_GUI_TABLINE)
સ્થિર પૂર્ણાંક ignore_tabline_evt = FALSE;
સ્થિર GtkWidget *tabline_menu;
# જો !GTK_CHECK_VERSION(3,0,0)
સ્થિર GtkTooltips *tabline_tooltip;
# endif
સ્થિર પૂર્ણાંક clicked_page;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    સ્થિર શૂન્ય
tabline_menu_handler(GtkMenuItem *item UNUSED, gpointer user_data)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_page, (પૂર્ણાંક)(long)user_data);
}

    સ્થિર શૂન્ય
add_tabline_menu_item(GtkWidget *menu, char_u *text, પૂર્ણાંક resp)
{
    GtkWidget	*item;
    char_u	*utf_text;

    utf_text = CONVERT_TO_UTF8(text);
    item = gtk_menu_item_new_with_label((અચલ અક્ષર *)utf_text);
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
    સ્થિર GtkWidget *
create_tabline_menu(શૂન્ય)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    add_tabline_menu_item(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    પાછા menu;
}

    સ્થિર gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    જો (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	પૂર્ણાંક		x = bevent->x;
	પૂર્ણાંક		y = bevent->y;
	GtkWidget	*tabwidget;
	GdkWindow	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	જો (hold_gui_events || cmdwin_type != 0)
	    પાછા TRUE;

	tabwin = gui_gtk_window_at_position(gui.mainwin, &x, &y);

	gdk_window_get_user_data(tabwin, (gpointer)&tabwidget);
	clicked_page = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	જો (bevent->button == 3)
	{
# જો GTK_CHECK_VERSION(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    જો (gui.is_wayland)
	    {
		પૂર્ણાંક x2, y2;
		gui_gtk_get_pointer(gui.mainwin, &x2, &y2, ખાલી);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_window(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, ખાલી);
	    }
	    નહીંતર
#  endif
	    gtk_menu_popup_at_pointer(GTK_MENU(widget), event);
# નહીંતર
	    gtk_menu_popup(GTK_MENU(widget), ખાલી, ખાલી, ખાલી, ખાલી,
						bevent->button, bevent->time);
# endif
	    // We handled the event.
	    પાછા TRUE;
	}
	નહીંતર જો (bevent->button == 1)
	{
	    જો (clicked_page == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	નહીંતર જો (bevent->button == 2)
	{
	    જો (clicked_page != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_page, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    પાછા FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    સ્થિર શૂન્ય
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    જો (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# જો GTK_CHECK_VERSION(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    સ્થિર શૂન્ય
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    જો (ignore_tabline_evt)
	પાછા;

    જો ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    નહીંતર
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    શૂન્ય
gui_mch_show_tabline(પૂર્ણાંક showit)
{
    જો (gui.tabline == ખાલી)
	પાછા;

    જો (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    પૂર્ણાંક w, h;

	    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_window_manager_hints(0, 0);
	જો (showit)
	    gtk_widget_set_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    પૂર્ણાંક
gui_mch_showing_tabline(શૂન્ય)
{
    પાછા gui.tabline != ખાલી
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    શૂન્ય
gui_mch_update_tabline(શૂન્ય)
{
    GtkWidget	    *page;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    પૂર્ણાંક		    nr = 0;
    પૂર્ણાંક		    tab_અંક;
    પૂર્ણાંક		    curtabidx = 0;
    char_u	    *labeltext;

    જો (gui.tabline == ખાલી)
	પાછા;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    માટે (tp = first_tabpage; tp != ખાલી; tp = tp->tp_next, ++nr)
    {
	જો (tp == curtab)
	    curtabidx = nr;

	tab_અંક = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	જો (page == ખાલી)
	{
	    // Add notebook page
# જો GTK_CHECK_VERSION(3,2,0)
	    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# નહીંતર
	    page = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(page);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# જો !GTK_CHECK_VERSION(3,14,0)
	    gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_add(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_ઉમેરો_page(GTK_NOTEBOOK(gui.tabline),
		    page,
		    event_box,
		    nr);
# જો GTK_CHECK_VERSION(2,10,0)
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	g_object_set_data(G_OBJECT(event_box), "tab_num",
						     GINT_TO_POINTER(tab_અંક));
	label = gtk_bin_get_child(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_label_set_text(GTK_LABEL(label), (અચલ અક્ષર *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
# જો GTK_CHECK_VERSION(3,0,0)
	gtk_widget_set_tooltip_text(event_box, (અચલ gchar *)labeltext);
# નહીંતર
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (અચલ અક્ષર *)labeltext, ખાલી);
# endif
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    જ્યારે (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != ખાલી)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    જો (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    શૂન્ય
gui_mch_set_curtab(પૂર્ણાંક nr)
{
    જો (gui.tabline == ખાલી)
	પાછા;

    ignore_tabline_evt = TRUE;
    જો (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#endif // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    શૂન્ય
gui_gtk_set_selection_targets(GdkAtom selection)
{
    પૂર્ણાંક		    i, j = 0;
    સ્થિર પૂર્ણાંક	    n_targets = N_SELECTION_TARGETS;
    સ્થિર GtkTargetEntry  targets[N_SELECTION_TARGETS];

    જો (targets[0].target == ખાલી)
    {
	માટે (i = 0; i < (પૂર્ણાંક)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    જો (!clip_html && selection_targets[i].info == TARGET_HTML)
		n_targets--;
	    નહીંતર
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
    શૂન્ય
gui_gtk_set_dnd_targets(શૂન્ય)
{
    પૂર્ણાંક		    i, j = 0;
    પૂર્ણાંક		    n_targets = N_DND_TARGETS;
    GtkTargetEntry  targets[N_DND_TARGETS];

    માટે (i = 0; i < (પૂર્ણાંક)N_DND_TARGETS; ++i)
    {
	જો (!clip_html && dnd_targets[i].info == TARGET_HTML)
	    n_targets--;
	નહીંતર
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
    પૂર્ણાંક
gui_mch_init(શૂન્ય)
{
    GtkWidget *vbox;

#ifdef FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    જો (using_gnome)
    {
	gnome_program_init(VIMPACKAGE, VIM_VERSION_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, ખાલી);
# જો defined(LC_NUMERIC)
	{
	    અક્ષર *p = setlocale(LC_NUMERIC, ખાલી);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    જો (p == ખાલી || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#endif
    VIM_CLEAR(gui_argv);

#જો GLIB_CHECK_VERSION(2,1,3)
    // Set the human-readable application name
    g_set_application_name("Vim");
#endif
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    set_option_કિંમત_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#ifdef FEAT_TOOLBAR
    gui_gtk_register_stock_icons();
#endif
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#જો !GTK_CHECK_VERSION(3,0,0)
# જો 0
    gtk_rc_parse("gtkrc");
# endif
#endif

    // Initialize values
    gui.border_width = 2;
    gui.scrollbar_width = SB_DEFAULT_WIDTH;
    gui.scrollbar_height = SB_DEFAULT_WIDTH;
#જો GTK_CHECK_VERSION(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#નહીંતર
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

    જો (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	જો (plug != ખાલી && gtk_plug_get_socket_window(GTK_PLUG(plug)) != ખાલી)
	{
	    gui.mainwin = plug;
	}
	નહીંતર
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (unsigned પૂર્ણાંક)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    જો (gtk_socket_id == 0)
    {
#ifdef FEAT_GUI_GNOME
	જો (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", ખાલી);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	નહીંતર
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
		     G_CALLBACK(&delete_event_cb), ખાલી);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), ખાલી);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_changed_cb), ખાલી);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), ખાલી);

    gui.accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(gui.mainwin), gui.accel_group);

    // A vertical box holds the menubar, toolbar and main text window.
#જો GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#નહીંતર
    vbox = gtk_vbox_new(FALSE, 0);
#endif

#ifdef FEAT_GUI_GNOME
    જો (using_gnome)
    {
# જો defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_set_contents(GNOME_APP(gui.mainwin), vbox);
    }
    નહીંતર
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
	g_object_set(gtk_settings, "gtk-menu-bar-accel", ખાલી, ખાલી);
    }


# ifdef FEAT_GUI_GNOME
    જો (using_gnome)
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
    નહીંતર
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
# નહીંતર
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
    જો (using_gnome)
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
    નહીંતર
# endif	// FEAT_GUI_GNOME
    {
	જો (vim_strchr(p_go, GO_TOOLBAR) != ખાલી
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
# જો !GTK_CHECK_VERSION(3,0,0)
    gtk_notebook_set_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# જો !GTK_CHECK_VERSION(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *page, *label, *event_box;

	// Add the first tab.
# જો GTK_CHECK_VERSION(3,2,0)
	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# નહીંતર
	page = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(page);
	gtk_container_add(GTK_CONTAINER(gui.tabline), page);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_set_data(G_OBJECT(event_box), "tab_num", GINT_TO_POINTER(1L));
# જો !GTK_CHECK_VERSION(3,14,0)
	gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(gui.tabline), page, event_box);
# જો GTK_CHECK_VERSION(2,10,0)
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline), page, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ખાલી);
# જો GTK_CHECK_VERSION(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ખાલી);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#endif // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_set_border_width(GTK_CONTAINER(gui.formwin), 0);
#જો !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_events(gui.formwin, GDK_EXPOSURE_MASK);
#endif
#જો GTK_CHECK_VERSION(3,22,2)
    gtk_widget_set_name(gui.formwin, "vim-gtk-form");
#endif

    gui.drawarea = gtk_drawing_area_new();
#જો GTK_CHECK_VERSION(3,0,0)
    gui.surface = ખાલી;
#endif

#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    જો (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#endif

    // Determine which events we will filter.
    gint event_mask =
	GDK_EXPOSURE_MASK |
	GDK_ENTER_NOTIFY_MASK |
	GDK_LEAVE_NOTIFY_MASK |
	GDK_BUTTON_PRESS_MASK |
	GDK_BUTTON_RELEASE_MASK |
	GDK_ચાવી_PRESS_MASK |
	GDK_ચાવી_RELEASE_MASK |
	GDK_POINTER_MOTION_MASK |
	GDK_POINTER_MOTION_HINT_MASK;
#જો GTK_CHECK_VERSION(3,4,0)
    જો (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
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
    નહીંતર
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_mask |= GDK_SMOOTH_SCROLL_MASK;
    }
#નહીંતર
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
		       G_CALLBACK(ચાવી_press_event), ખાલી);
#જો defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&ચાવી_release_event), ખાલી);
#endif
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ખાલી);
#જો GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), ખાલી);
#endif
#જો GTK_CHECK_VERSION(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), ખાલી);
#નહીંતર
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_set_cb), ખાલી);
#endif

#જો !GTK_CHECK_VERSION(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#endif

#જો !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#endif

    જો (gtk_socket_id != 0)
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

#જો GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), ખાલી);
#નહીંતર
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), ખાલી);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), ખાલી);
#endif

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    જો (vim_strchr(p_go, GO_POINTER) != ખાલી)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), ખાલી);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), ખાલી);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    જો (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_out_event), ખાલી);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_in_event), ખાલી);
    }
    નહીંતર
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_out_event), ખાલી);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_in_event), ખાલી);
#ifdef FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_out_event), ખાલી);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_in_event), ખાલી);
#endif // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), ખાલી);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.in_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_changed_cb), ખાલી);
    }

#જો defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(gui.formwin);
#endif

    પાછા OK;
}

#જો defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    શૂન્ય
gui_mch_forked(શૂન્ય)
{
    જો (!using_gnome)
	પાછા;

    GnomeClient *client;

    client = gnome_master_client();

    જો (client != ખાલી)
	gnome_client_set_process_id(client, getpid());
}
#endif // USE_GNOME_SESSION

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    પાછા rgba;
}

    સ્થિર શૂન્ય
set_cairo_source_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    અચલ GdkRGBA rgba = color_to_rgba(color);
    cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#endif // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    શૂન્ય
gui_mch_new_colors(શૂન્ય)
{
    જો (gui.drawarea != ખાલી
#જો GTK_CHECK_VERSION(3,22,2)
	    && gui.formwin != ખાલી
#endif
	    && gtk_widget_get_window(gui.drawarea) != ખાલી)
    {
#જો !GTK_CHECK_VERSION(3,22,2)
	GdkWindow * અચલ da_win = gtk_widget_get_window(gui.drawarea);
#endif
#જો GTK_CHECK_VERSION(3,22,2)
	GtkStyleContext * અચલ context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * અચલ provider = gtk_css_provider_new();
	gchar * અચલ css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.back_pixel >> 16) & 0xff,
		 (gui.back_pixel >> 8) & 0xff,
		 gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, ખાલી);
	gtk_style_context_add_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#અથવાજો GTK_CHECK_VERSION(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_to_rgba(gui.back_pixel);
	{
	    cairo_pattern_t * અચલ pat = cairo_pattern_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    જો (pat != ખાલી)
	    {
		gdk_window_set_background_pattern(da_win, pat);
		cairo_pattern_destroy(pat);
	    }
	    નહીંતર
		gdk_window_set_background_rgba(da_win, &rgba);
	}
#નહીંતર // !GTK_CHECK_VERSION(3,4,0)
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
    સ્થિર gboolean
startup_resize_correction_cb(gpointer data UNUSED)
{
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    પાછા FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    સ્થિર gint
form_configure_event(GtkWidget *widget UNUSED,
		     GdkEventConfigure *event,
		     gpointer data UNUSED)
{
    પૂર્ણાંક	    usable_height = event->height;
#ifdef TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    પૂર્ણાંક	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ખાલી, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (પૂર્ણાંક)Columns, (પૂર્ણાંક)Rows);
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

    જો (!MATCH_WIDTH_HEIGHT(latest_resize_hist, w, h)
					     && match_stale_width_height(w, h))
	// discard stale event
	પાછા TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    જો ((mch_csd_height == 0 || mch_csd_width == 0)
	    && latest_resize_hist != ખાલી
	    && !latest_resize_hist->used
	    && latest_resize_hist->from_shellsize
	    && gui.char_height > 0)
    {
	પૂર્ણાંક pot_csd_w = latest_resize_hist->width - w;
	પૂર્ણાંક pot_csd_h = latest_resize_hist->height - h;

	જો (pot_csd_w > 0 && pot_csd_w < gui.char_width)
	{
	    mch_csd_width = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (પૂર્ણાંક)Columns * gui.char_width
						    + gui_get_base_width();
	}
	જો (pot_csd_h > 0 && pot_csd_h < gui.char_height)
	{
	    mch_csd_height = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_height += mch_csd_height;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	જો ((mch_csd_height > 0 || mch_csd_width > 0) && gtk_socket_id == 0)
	    g_idle_add_full(GTK_PRIORITY_RESIZE, startup_resize_correction_cb, ખાલી, ખાલી);
    }
    clear_resize_hists();
#endif

#જો GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
    જો (event->window != gtk_widget_get_window(gui.formwin))
	પાછા TRUE;
#endif

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    જો (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	પૂર્ણાંક use_width = event->width;

#ifdef TRACK_RESIZE_HISTORY
	જો (mch_pending_form_w > event->width
		&& mch_pending_form_w - event->width < gui.char_width)
	    use_width = mch_pending_form_w;
#endif
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_width, usable_height);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    પાછા TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    સ્થિર શૂન્ય
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

    gui.mainwin  = ખાલી;
    gui.drawarea = ખાલી;

    જો (!exiting) // only do anything if the destroy was unexpected
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

    શૂન્ય
gui_gtk_get_screen_geom_of_win(
	GtkWidget *wid,
	પૂર્ણાંક point_x,	    // x position of window if not initialized
	પૂર્ણાંક point_y,	    // y position of window if not initialized
	પૂર્ણાંક *screen_x,
	પૂર્ણાંક *screen_y,
	પૂર્ણાંક *width,
	પૂર્ણાંક *height)
{
    GdkRectangle geometry;
    GdkWindow *win = gtk_widget_get_window(wid);
#જો GTK_CHECK_VERSION(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    જો (wid != ખાલી && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    નહીંતર
	dpy = gdk_display_get_default();
    જો (win != ખાલી)
	monitor = gdk_display_get_monitor_at_window(dpy, win);
    નહીંતર
	monitor = gdk_display_get_monitor_at_point(dpy, point_x, point_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#નહીંતર
    GdkScreen* screen;
    પૂર્ણાંક monitor;

    જો (wid != ખાલી && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    નહીંતર
	screen = gdk_screen_get_default();
    જો (win != ખાલી)
	monitor = gdk_screen_get_monitor_at_window(screen, win);
    નહીંતર
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
    સ્થિર શૂન્ય
gui_gtk_get_screen_dimensions(
	પૂર્ણાંક point_x,
	પૂર્ણાંક point_y,
	પૂર્ણાંક *screen_w,
	પૂર્ણાંક *screen_h)
{
    પૂર્ણાંક	    x, y;

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

    શૂન્ય
gui_mch_get_screen_dimensions(પૂર્ણાંક *screen_w, પૂર્ણાંક *screen_h)
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
    સ્થિર timeout_cb_type
check_startup_plug_hints(gpointer data UNUSED)
{
    જો (init_window_hints_state == 1)
    {
	// Safe to use normal hints now
	init_window_hints_state = 0;
	update_window_manager_hints(0, 0);
	પાછા FALSE;   // stop timer
    }

    // Keep on trying
    init_window_hints_state = 1;
    પાછા TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    પૂર્ણાંક
gui_mch_open(શૂન્ય)
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
    જો (role_argument != ખાલી)
    {
	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role_argument);
    }
    નહીંતર
    {
	અક્ષર *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (unsigned)mch_get_pid(),
			       (unsigned)g_random_int(),
			       (unsigned)time(ખાલી));

	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role);
	g_free(role);
    }

    જો (gui_win_x != -1 && gui_win_y != -1)
	gtk_window_move(GTK_WINDOW(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    જો (gui.geom != ખાલી)
    {
	પૂર્ણાંક		mask;
	unsigned પૂર્ણાંક	w, h;
	પૂર્ણાંક		x = 0;
	પૂર્ણાંક		y = 0;

	mask = XParseGeometry((અક્ષર *)gui.geom, &x, &y, &w, &h);

	જો (mask & Width_કિંમત)
	    Columns = w;
	જો (mask & Height_કિંમત)
	{
	    જો (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
	pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

	pixel_width  += get_menu_tool_width();
	pixel_height += get_menu_tool_height();

	જો (mask & (XValue | YValue))
	{
	    પૂર્ણાંક ww, hh;

#ifdef FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#નહીંતર
	    gui_mch_get_screen_dimensions(&ww, &hh);
#endif
	    hh += p_ghr + get_menu_tool_height();
	    ww += get_menu_tool_width();
	    જો (mask & XNegative)
		x += ww - pixel_width;
	    જો (mask & YNegative)
		y += hh - pixel_height;
	    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	જો (gtk_socket_id != 0  &&  (mask & Width_કિંમત || mask & Height_કિંમત))
	{
	    update_window_manager_hints(pixel_width, pixel_height);
	    init_window_hints_state = 1;
	    timeout_add(1000, check_startup_plug_hints, ખાલી);
	}
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    જો (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), pixel_width, pixel_height);
    update_window_manager_hints(0, 0);

    જો (foreground_argument != ખાલી)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    જો (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    જો (background_argument != ખાલી)
	bg_pixel = gui_get_color((char_u *)background_argument);
    જો (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    જો (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    નહીંતર
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
		     G_CALLBACK(mainwin_destroy_cb), ખાલી);

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
				       G_CALLBACK(form_configure_event), ખાલી);
#જો GTK_CHECK_VERSION(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_factor_event), ખાલી);
#endif

#ifdef FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_set_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_data_received_cb), ખાલી);
#endif

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	જો (found_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#જો defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
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
	જો (using_gnome && vim_strchr(p_go, GO_MENUS) == ખાલી)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ખાલી);
# ifdef FEAT_TOOLBAR
	જો (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == ખાલી
		&& (toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ખાલી);
# endif
#endif
	gtk_widget_show(gui.mainwin);

#જો defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	જો (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	જો (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#endif
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), ખાલી);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), ખાલી);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), ખાલી);

    પાછા OK;
}

/*
 * Clean up for when exiting Vim.
 */
    શૂન્ય
gui_mch_exit(પૂર્ણાંક rc UNUSED)
{
    // Clean up, unless we don't want to invoke free().
    જો (gui.mainwin != ખાલી && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    પૂર્ણાંક
gui_mch_get_winpos(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    જો (gui_mch_get_display())
    {
	gtk_window_get_position(GTK_WINDOW(gui.mainwin), x, y);
	પાછા OK;
    }
    પાછા FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    શૂન્ય
gui_mch_set_winpos(પૂર્ણાંક x, પૂર્ણાંક y)
{
    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
}

#જો !GTK_CHECK_VERSION(3,0,0)
# જો 0
સ્થિર પૂર્ણાંક resize_idle_installed = FALSE;
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
    સ્થિર gboolean
force_shell_resize_idle(gpointer data)
{
    જો (gui.mainwin != ખાલી
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	પૂર્ણાંક width;
	પૂર્ણાંક height;

	gtk_window_get_size(GTK_WINDOW(gui.mainwin), &width, &height);

	width  -= get_menu_tool_width();
	height -= get_menu_tool_height();

	gui_resize_shell(width, height);
    }

    resize_idle_installed = FALSE;
    પાછા FALSE; // don't call me again
}
# endif
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    પૂર્ણાંક
gui_mch_maximized(શૂન્ય)
{
    પાછા (gui.mainwin != ખાલી && gtk_widget_get_window(gui.mainwin) != ખાલી
	    && (gdk_window_get_state(gtk_widget_get_window(gui.mainwin))
					       & GDK_WINDOW_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    શૂન્ય
gui_mch_unmaximize(શૂન્ય)
{
    જો (gui.mainwin != ખાલી)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    શૂન્ય
gui_mch_newfont(શૂન્ય)
{
    પૂર્ણાંક w, h;

    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    w -= get_menu_tool_width();
    h -= get_menu_tool_height();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    શૂન્ય
gui_mch_set_shellsize(પૂર્ણાંક width, પૂર્ણાંક height,
		      પૂર્ણાંક min_width UNUSED,  પૂર્ણાંક min_height UNUSED,
		      પૂર્ણાંક base_width UNUSED, પૂર્ણાંક base_height UNUSED,
		      પૂર્ણાંક direction UNUSED)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    જો (gtk_socket_id == 0)
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
    જો (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), width, height);
    નહીંતર
	update_window_manager_hints(width, height);

#જો !GTK_CHECK_VERSION(3,0,0)
# જો 0
    જો (!resize_idle_installed)
    {
	g_idle_add_full(GDK_PRIORITY_EVENTS + 10,
			&force_shell_resize_idle, ખાલી, ખાલી);
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

    શૂન્ય
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    જો (title != ખાલી && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, ખાલી);

    gtk_window_set_title(GTK_WINDOW(gui.mainwin), (અચલ અક્ષર *)title);

    જો (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#જો defined(FEAT_MENU)
    શૂન્ય
gui_mch_enable_menu(પૂર્ણાંક showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    જો (using_gnome)
	widget = gui.menubar_h;
    નહીંતર
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    જો (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	જો (showit)
	    gtk_widget_show(widget);
	નહીંતર
	    gtk_widget_hide(widget);

	update_window_manager_hints(0, 0);
    }
}
#endif // FEAT_MENU

#જો defined(FEAT_TOOLBAR)
    શૂન્ય
gui_mch_show_toolbar(પૂર્ણાંક showit)
{
    GtkWidget *widget;

    જો (gui.toolbar == ખાલી)
	પાછા;

# ifdef FEAT_GUI_GNOME
    જો (using_gnome)
	widget = gui.toolbar_h;
    નહીંતર
# endif
	widget = gui.toolbar;

    જો (showit)
	set_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    જો (!showit != !gtk_widget_get_visible(widget))
    {
	જો (showit)
	    gtk_widget_show(widget);
	નહીંતર
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
    સ્થિર પૂર્ણાંક
is_cjk_font(PangoFontDescription *font_desc)
{
    સ્થિર અચલ અક્ષર * અચલ cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    unsigned	i;
    પૂર્ણાંક		is_cjk = FALSE;

    font = pango_context_load_font(gui.text_context, font_desc);

    જો (font == ખાલી)
	પાછા FALSE;

    માટે (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	જો (coverage != ખાલી)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#જો PANGO_VERSION_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#નહીંતર
	    pango_coverage_unref(coverage);
#endif
	}
    }

    g_object_unref(font);

    પાછા is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    પૂર્ણાંક
gui_mch_adjust_charheight(શૂન્ય)
{
    PangoFontMetrics	*metrics;
    પૂર્ણાંક			ascent;
    પૂર્ણાંક			descent;

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

    પાછા OK;
}

#જો GTK_CHECK_VERSION(3,0,0)
// Callback function used in gui_mch_font_dialog()
    સ્થિર gboolean
font_filter(અચલ PangoFontFamily *family,
	    અચલ PangoFontFace   *face UNUSED,
	    gpointer		   data UNUSED)
{
    પાછા pango_font_family_is_monospace((PangoFontFamily *)family);
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
    પૂર્ણાંક		response;
    char_u	*fontname = ખાલી;
    char_u	*oldname;

#જો GTK_CHECK_VERSION(3,2,0)
    dialog = gtk_font_chooser_dialog_new(ખાલી, ખાલી);
    gtk_font_chooser_set_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    ખાલી, ખાલી);
#નહીંતર
    dialog = gtk_font_selection_dialog_new(ખાલી);
#endif

    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    જો (oldval != ખાલી && oldval[0] != NUL)
    {
	જો (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ખાલી);
	નહીંતર
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	જો (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    જો (p != ખાલી)
	    {
		STRCPY(p + STRLEN(p), " 10");
		જો (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#જો GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), (અચલ gchar *)oldname);
#નહીંતર
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (અચલ અક્ષર *)oldname);
#endif

	જો (oldname != oldval)
	    vim_free(oldname);
    }
    નહીંતર
#જો GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#નહીંતર
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#endif

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    જો (response == GTK_RESPONSE_OK)
    {
	અક્ષર *name;

#જો GTK_CHECK_VERSION(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#નહીંતર
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#endif
	જો (name != ખાલી)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    જો (p != ખાલી && આગત_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&આગત_conv, p, ખાલી);
		vim_free(p);
	    }
	    નહીંતર
		fontname = p;
	}
    }

    જો (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    પાછા fontname;
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
    સ્થિર શૂન્ય
get_styled_font_variants(શૂન્ય)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);

    જો (plain_font == ખાલી)
	પાછા;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    જો (bold_font != ખાલી)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    જો (bold_font != ખાલી && gui.font_can_bold)
	g_object_unref(plain_font);
}

સ્થિર PangoEngineShape *default_shape_engine = ખાલી;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    સ્થિર શૂન્ય
ascii_glyph_table_init(શૂન્ય)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    પૂર્ણાંક		    i;

    જો (gui.ascii_glyphs != ખાલી)
	pango_glyph_string_free(gui.ascii_glyphs);
    જો (gui.ascii_font != ખાલી)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ખાલી;
    gui.ascii_font   = ખાલી;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    માટે (i = 0; i < 128; ++i)
    {
	જો (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	નહીંતર
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (અચલ અક્ષર *)ascii_chars,
			      0, કદ(ascii_chars), attr_list, ખાલી);

    જો (item_list != ખાલી && item_list->next == ખાલી) // play safe
    {
	PangoItem   *item;
	પૂર્ણાંક	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = item->analysis.shape_engine;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((અચલ અક્ષર *)ascii_chars, કદ(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->અંક_glyphs == કદ(ascii_chars));

	માટે (i = 0; i < gui.ascii_glyphs->અંક_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_offset += MAX(0, width - geom->width) / 2;
	    geom->width = width;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(item_list, (GFunc)(શૂન્ય *)&pango_item_free, ખાલી);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    પૂર્ણાંક
gui_mch_init_font(char_u *font_name, પૂર્ણાંક fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    પૂર્ણાંક			    width;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    જો (font_name == ખાલી)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    જો (font_desc == ખાલી)
	પાછા FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, ખાલી);
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
    જો (is_cjk_font(gui.norm_font))
    {
	પૂર્ણાંક cjk_width;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_set_text(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_size(layout, &cjk_width, ખાલી);

	જો (width == cjk_width)  // Xft not patched
	    width /= 2;
    }
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    જો (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    જો (gui.wide_font != ખાલી
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = ખાલી;
    }

    જો (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    નહીંતર
    {
	// Preserve the logical dimensions of the screen.
	update_window_manager_hints(0, 0);
    }

    પાછા OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, પૂર્ણાંક report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    જો (!gui.in_use || name == ખાલી)
	પાછા ખાલી;

    જો (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, ખાલી);
	જો (buf != ખાલી)
	{
	    font = pango_font_description_from_string((અચલ અક્ષર *)buf);
	    vim_free(buf);
	}
	નહીંતર
	    font = ખાલી;
    }
    નહીંતર
	font = pango_font_description_from_string((અચલ અક્ષર *)name);

    જો (font != ખાલી)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	જો (pango_font_description_get_size(font) <= 0)
	    pango_font_description_set_size(font, 10 * PANGO_SCALE);

	real_font = pango_context_load_font(gui.text_context, font);

	જો (real_font == ખાલી)
	{
	    pango_font_description_free(font);
	    font = ખાલી;
	}
	નહીંતર
	    g_object_unref(real_font);
    }

    જો (font == ખાલી)
    {
	જો (report_error)
	    semsg(_((અક્ષર *)e_unknown_font_str), name);
	પાછા ખાલી;
    }

    પાછા font;
}

#જો defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    જો (font != NOFONT)
    {
	અક્ષર	*pangoname = pango_font_description_to_string(font);

	જો (pangoname != ખાલી)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    પાછા s;
	}
    }
    પાછા ખાલી;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    શૂન્ય
gui_mch_free_font(GuiFont font)
{
    જો (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
    શૂન્ય
gui_mch_expand_font(
    optexpand_T	*args,
    શૂન્ય	*param,
    પૂર્ણાંક		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = ખાલી;
    પૂર્ણાંક			n_families = 0;
    પૂર્ણાંક			wide = *(પૂર્ણાંક *)param;

    જો (args->oe_include_orig_val && *args->oe_opt_કિંમત == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	જો (add_match((char_u *)DEFAULT_FONT) != OK)
	    પાછા;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    માટે (પૂર્ણાંક i = 0; i < n_families; i++)
    {
	જો (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ચાલુ;

	અચલ અક્ષર* fam_name = pango_font_family_get_name(font_families[i]);
	જો (આગત_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&આગત_conv, (char_u*)fam_name, ખાલી);
	    જો (buf != ખાલી)
	    {
		જો (add_match(buf) != OK)
		{
		    vim_free(buf);
		    તોડો;
		}
		vim_free(buf);
	    }
	    નહીંતર
		તોડો;
	}
	નહીંતર
	{
	    જો (add_match((char_u *)fam_name) != OK)
		તોડો;
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

    જો (!gui.in_use)		// can't do this when GUI not running
	પાછા color;

    જો (name != ખાલી)
	color = gui_get_color_cmn(name);

#જો GTK_CHECK_VERSION(3,0,0)
    પાછા color;
#નહીંતર
    જો (color == INVALCOLOR)
	પાછા INVALCOLOR;

    પાછા gui_mch_get_rgb_color(
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
gui_mch_get_rgb_color(પૂર્ણાંક r, પૂર્ણાંક g, પૂર્ણાંક b)
{
#જો GTK_CHECK_VERSION(3,0,0)
    પાછા gui_get_rgb_color_cmn(r, g, b);
#નહીંતર
    GdkColor gcolor;
    પૂર્ણાંક ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    પાછા ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#endif
}

/*
 * Set the current text foreground color.
 */
    શૂન્ય
gui_mch_set_fg_color(guicolor_T color)
{
#જો GTK_CHECK_VERSION(3,0,0)
    *gui.fgcolor = color_to_rgba(color);
#નહીંતર
    gui.fgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text background color.
 */
    શૂન્ય
gui_mch_set_bg_color(guicolor_T color)
{
#જો GTK_CHECK_VERSION(3,0,0)
    *gui.bgcolor = color_to_rgba(color);
#નહીંતર
    gui.bgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text special color.
 */
    શૂન્ય
gui_mch_set_sp_color(guicolor_T color)
{
#જો GTK_CHECK_VERSION(3,0,0)
    *gui.spcolor = color_to_rgba(color);
#નહીંતર
    gui.spcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#define ઉમેરો_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_ઉમેરો((AttrList), tmp_attr_);	    \
    }G_STMT_END

    સ્થિર શૂન્ય
apply_wide_font_attr(char_u *s, પૂર્ણાંક len, PangoAttrList *attr_list)
{
    char_u  *start = ખાલી;
    char_u  *p;
    પૂર્ણાંક	    uc;

    માટે (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	જો (start == ખાલી)
	{
	    જો (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	નહીંતર જો (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ઉમેરો_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, start - s, p - s);
	    start = ખાલી;
	}
    }

    જો (start != ખાલી)
	ઉમેરો_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, start - s, len);
}

    સ્થિર પૂર્ણાંક
ગણતરી_cluster_cells(char_u *s, PangoItem *item,
		    PangoGlyphString* glyphs, પૂર્ણાંક i,
		    પૂર્ણાંક *cluster_width,
		    પૂર્ણાંક *last_glyph_rbearing)
{
    char_u  *p;
    પૂર્ણાંક	    next;	// glyph start index of next cluster
    પૂર્ણાંક	    start, end; // string segment of current cluster
    પૂર્ણાંક	    width;	// real cluster width in Pango units
    પૂર્ણાંક	    uc;
    પૂર્ણાંક	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    માટે (next = i + 1; next < glyphs->અંક_glyphs; ++next)
    {
	જો (glyphs->glyphs[next].attr.is_cluster_start)
	    તોડો;
	નહીંતર જો (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->અંક_glyphs) ?
			    glyphs->log_clusters[next] : item->length);

    માટે (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	જો (uc < 0x80)
	    ++cellcount;
	નહીંતર જો (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    જો (last_glyph_rbearing != ખાલી
	    && cellcount > 0 && next == glyphs->અંક_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(item->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, ખાલી);

	જો (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    જો (cellcount > 0)
	*cluster_width = width;

    પાછા cellcount;
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
    સ્થિર શૂન્ય
setup_zero_width_cluster(PangoItem *item, PangoGlyphInfo *glyph,
			 પૂર્ણાંક last_cellcount, પૂર્ણાંક last_cluster_width,
			 પૂર્ણાંક last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    પૂર્ણાંક		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    જો (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
		- (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    નહીંતર
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર શૂન્ય
draw_glyph_string(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક અંક_cells, પૂર્ણાંક flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#નહીંતર
    સ્થિર શૂન્ય
draw_glyph_string(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક અંક_cells, પૂર્ણાંક flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#endif
{
    જો (!(flags & DRAW_TRANSP))
    {
#જો GTK_CHECK_VERSION(3,0,0)
	cairo_set_source_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(col), FILL_Y(row),
			અંક_cells * gui.char_width, gui.char_height);
	cairo_fill(cr);
#નહીંતર
	gdk_gc_set_foreground(gui.text_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->window,
			   gui.text_gc,
			   TRUE,
			   FILL_X(col),
			   FILL_Y(row),
			   અંક_cells * gui.char_width,
			   gui.char_height);
#endif
    }

#જો GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_to(cr, TEXT_X(col), TEXT_Y(row));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#નહીંતર
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->window,
		    gui.text_gc,
		    font,
		    TEXT_X(col),
		    TEXT_Y(row),
		    glyphs);
#endif

    // redraw the contents with an offset of 1 to emulate bold
    જો ((flags & DRAW_BOLD) && !gui.font_can_bold)
#જો GTK_CHECK_VERSION(3,0,0)
    {
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, TEXT_X(col) + 1, TEXT_Y(row));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#નહીંતર
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
#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર શૂન્ય
draw_under(પૂર્ણાંક flags, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક cells, cairo_t *cr)
#નહીંતર
    સ્થિર શૂન્ય
draw_under(પૂર્ણાંક flags, પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક cells)
#endif
{
    પૂર્ણાંક			i;
    પૂર્ણાંક			offset;
    સ્થિર અચલ પૂર્ણાંક	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    પૂર્ણાંક			y = FILL_Y(row + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    જો (flags & DRAW_UNDERC)
    {
#જો GTK_CHECK_VERSION(3,0,0)
	cairo_set_લીટી_width(cr, 1.0);
	cairo_set_લીટી_cap(cr, CAIRO_લીટી_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col) + 1, y - 2 + 0.5);
	માટે (i = FILL_X(col) + 1; i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    cairo_લીટી_to(cr, i, y - offset + 0.5);
	}
	cairo_stroke(cr);
#નહીંતર
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	માટે (i = FILL_X(col); i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    gdk_draw_point(gui.drawarea->window, gui.text_gc, i, y - offset);
	}
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Draw a strikethrough line
    જો (flags & DRAW_STRIKE)
    {
#જો GTK_CHECK_VERSION(3,0,0)
	cairo_set_લીટી_width(cr, 1.0);
	cairo_set_લીટી_cap(cr, CAIRO_લીટી_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 1 - gui.char_height/2 + 0.5);
	cairo_લીટી_to(cr, FILL_X(col + cells), y + 1 - gui.char_height/2 + 0.5);
	cairo_stroke(cr);
#નહીંતર
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	gdk_draw_લીટી(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y + 1 - gui.char_height/2,
		      FILL_X(col + cells), y + 1 - gui.char_height/2);
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Underline: draw a line at the bottom of the character cell.
    જો (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	જો (p_linespace > 1)
	    y -= p_linespace - 1;
#જો GTK_CHECK_VERSION(3,0,0)
	cairo_set_લીટી_width(cr, 1.0);
	cairo_set_લીટી_cap(cr, CAIRO_લીટી_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 0.5);
	cairo_લીટી_to(cr, FILL_X(col + cells), y + 0.5);
	cairo_stroke(cr);
#નહીંતર
	gdk_draw_લીટી(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y,
		      FILL_X(col + cells) - 1, y);
#endif
    }
}

    પૂર્ણાંક
gui_gtk_draw_string(પૂર્ણાંક row, પૂર્ણાંક col, char_u *s, પૂર્ણાંક len, પૂર્ણાંક flags)
{
    char_u	*conv_buf = ખાલી;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    પૂર્ણાંક		convlen;
    char_u	*sp, *bp;
    પૂર્ણાંક		plen;
    પૂર્ણાંક		len_sum;	// return value needs to add up since we are
				// printing substrings
    પૂર્ણાંક		byte_sum;	// byte position in string
    char_u	*cs;		// current *s pointer
    પૂર્ણાંક		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    પૂર્ણાંક		should_need_pango = FALSE;
    પૂર્ણાંક		slen;
    પૂર્ણાંક		is_ligature;
    પૂર્ણાંક		is_utf8;
    char_u	backup_ch;

    જો (gui.text_context == ખાલી || gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા len;

    જો (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != ખાલી, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	માટે (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    જો ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		જો (new_conv_buf == ખાલી)
		    પાછા len;
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
    જો (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    જો (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    જ્યારે (cs < s + len)
    {
	slen = 0;
	જ્યારે (slen < (len - byte_sum))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    જો (is_ligature && !needs_pango)
	    {
		જો ((slen + 1) < (len - byte_sum))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		નહીંતર
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    જો ((!is_utf8) && ((slen + 1) < (len - byte_sum)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    જો (needs_pango != should_need_pango) // mode switch
		તોડો;
	    જો (needs_pango)
	    {
		જો (is_ligature)
		{
		    slen++; // ligature char by char
		}
		નહીંતર // is_utf8
		{
		    જો ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			જ્યારે ((slen < (len - byte_sum))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    નહીંતર જો ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    નહીંતર જો ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    નહીંતર જો ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    નહીંતર
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    નહીંતર
	    {
		slen++; // ascii
	    }
	}

	જો (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_sum += gui_gtk_draw_string_ext(row, col + len_sum, cs, slen, flags,
					    needs_pango);
	જો (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_sum += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    પાછા len_sum;
}

#જો GTK_CHECK_VERSION(3,0,0)
સ્થિર cairo_region_t *dirty_region = ખાલી;

    સ્થિર શૂન્ય
queue_draw_area(પૂર્ણાંક x, પૂર્ણાંક y, પૂર્ણાંક width, પૂર્ણાંક height)
{
    cairo_rectangle_int_t rect;

    જો (width <= 0 || height <= 0 || gui.drawarea == ખાલી)
	પાછા;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    જો (dirty_region == ખાલી)
	dirty_region = cairo_region_create_rectangle(&rect);
    નહીંતર
	cairo_region_union_rectangle(dirty_region, &rect);
}
#endif

    પૂર્ણાંક
gui_gtk_draw_string_ext(
	પૂર્ણાંક	row,
	પૂર્ણાંક	col,
	char_u	*s,
	પૂર્ણાંક	len,
	પૂર્ણાંક	flags,
	પૂર્ણાંક	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    પૂર્ણાંક			column_offset = 0;  // column offset in cells
    પૂર્ણાંક			i;
#જો GTK_CHECK_VERSION(3,0,0)
    cairo_t		*cr;
#endif

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_offset;
    area.y = FILL_Y(row);
    area.width	= gui.અંક_cols * gui.char_width;
    area.height = gui.char_height;

#જો GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.width, area.height);
    cairo_clip(cr);
#નહીંતર
    gdk_gc_set_clip_origin(gui.text_gc, 0, 0);
    gdk_gc_set_clip_rectangle(gui.text_gc, &area);
#endif

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    જો (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ખાલી
	    && !force_pango)
    {
	char_u *p;

	માટે (p = s; p < s + len; ++p)
	    જો (*p & 0x80)
		જાઓ not_ascii;

	pango_glyph_string_set_size(glyphs, len);

	માટે (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#જો GTK_CHECK_VERSION(3,0,0)
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs, cr);
#નહીંતર
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs);
#endif

	column_offset = len;
    }
    નહીંતર
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*item_list;
	પૂર્ણાંક		cluster_width;
	પૂર્ણાંક		last_glyph_rbearing;
	પૂર્ણાંક		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	જો (!utf_valid_string(s, s + len))
	{
	    column_offset = len;
	    જાઓ skipitall;
	}

	// original width of the current cluster
	cluster_width = PANGO_SCALE * gui.char_width;

	// right bearing of the last non-composing glyph
	last_glyph_rbearing = PANGO_SCALE * gui.char_width;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	જો (gui.wide_font != ખાલી)
	    apply_wide_font_attr(s, len, attr_list);

	જો ((flags & DRAW_BOLD) && gui.font_can_bold)
	    ઉમેરો_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
			      attr_list, 0, len);
	જો (flags & DRAW_ITALIC)
	    ઉમેરો_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	item_list = pango_itemize(gui.text_context,
				  (અચલ અક્ષર *)s, 0, len, attr_list, ખાલી);

	જ્યારે (item_list != ખાલી)
	{
	    PangoItem	*item;
	    પૂર્ણાંક		item_cells = 0; // item length in cells

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
	    pango_shape_full((અચલ અક્ષર *)s + item->offset, item->length,
		    (અચલ અક્ષર *)s, len, &item->analysis, glyphs);
#નહીંતર
	    pango_shape((અચલ અક્ષર *)s + item->offset, item->length,
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
	    માટે (i = 0; i < glyphs->અંક_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		જો (glyph->attr.is_cluster_start)
		{
		    પૂર્ણાંક cellcount;

		    cellcount = ગણતરી_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != ખાલી) ? &last_glyph_rbearing : ખાલી);

		    જો (cellcount > 0)
		    {
			પૂર્ણાંક width;

			width = cellcount * gui.char_width * PANGO_SCALE;
			glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
			glyph->geometry.width = width;
		    }
		    નહીંતર
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
		નહીંતર જો (i > 0)
		{
		    પૂર્ણાંક width;

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
		    જો (glyph->geometry.x_offset >= 0)
		    {
			glyphs->glyphs[i].geometry.width =
					 glyphs->glyphs[i - 1].geometry.width;
			glyphs->glyphs[i - 1].geometry.width = 0;
		    }
		    width = cells * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
		}
		નહીંતર // i == 0 "cannot happen"
		{
		    glyph->geometry.width = 0;
		}
	    }

	    //// Aaaaand action! **
#જો GTK_CHECK_VERSION(3,0,0)
	    draw_glyph_string(row, col + column_offset, item_cells,
			      flags, item->analysis.font, glyphs,
			      cr);
#નહીંતર
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
#જો GTK_CHECK_VERSION(3,0,0)
    draw_under(flags, row, col, column_offset, cr);
#નહીંતર
    draw_under(flags, row, col, column_offset);
#endif

    pango_glyph_string_free(glyphs);

#જો GTK_CHECK_VERSION(3,0,0)
    cairo_destroy(cr);
    queue_draw_area(FILL_X(col), FILL_Y(row),
	    column_offset * gui.char_width + 1, gui.char_height);
#નહીંતર
    gdk_gc_set_clip_rectangle(gui.text_gc, ખાલી);
#endif

    પાછા column_offset;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    પૂર્ણાંક
gui_mch_haskey(char_u *name)
{
    પૂર્ણાંક i;

    માટે (i = 0; special_ચાવીઓ[i].ચાવી_sym != 0; i++)
	જો (name[0] == special_ચાવીઓ[i].code0
		&& name[1] == special_ચાવીઓ[i].code1)
	    પાછા OK;
    પાછા FAIL;
}

#જો defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    પૂર્ણાંક
gui_get_x11_windis(Window *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    જો (dpy)
    {
	*dis = dpy;
	*win = GDK_WINDOW_XID(gtk_widget_get_window(gui.mainwin));
	પાછા OK;
    }

    *dis = ખાલી;
    *win = 0;
    પાછા FAIL;
}
#endif

    Display *
gui_mch_get_display(શૂન્ય)
{
    જો (gui.mainwin != ખાલી && gtk_widget_get_window(gui.mainwin) != ખાલી
#જો GTK_CHECK_VERSION(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#endif
	)
	પાછા GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin));
    નહીંતર
	પાછા ખાલી;
}

    શૂન્ય
gui_mch_beep(શૂન્ય)
{
    GdkDisplay *display;

    જો (gui.mainwin != ખાલી && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    નહીંતર
	display = gdk_display_get_default();

    જો (display != ખાલી)
	gdk_display_beep(display);
}

    શૂન્ય
gui_mch_flash(પૂર્ણાંક msec)
{
#જો GTK_CHECK_VERSION(3,0,0)
    // TODO Replace GdkGC with Cairo
    (શૂન્ય)msec;
#નહીંતર
    GdkGCValues	કિંમતો;
    GdkGC	*invert_gc;

    જો (gui.drawarea->window == ખાલી)
	પાછા;

    કિંમતો.foreground.pixel = gui.norm_pixel ^ gui.back_pixel;
    કિંમતો.background.pixel = gui.norm_pixel ^ gui.back_pixel;
    કિંમતો.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_કિંમતો(gui.drawarea->window,
				       &કિંમતો,
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
		       FILL_X((પૂર્ણાંક)Columns) + gui.border_offset,
		       FILL_Y((પૂર્ણાંક)Rows) + gui.border_offset);

    gui_mch_flush();
    ui_delay((long)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((પૂર્ણાંક)Columns) + gui.border_offset,
		       FILL_Y((પૂર્ણાંક)Rows) + gui.border_offset);

    gdk_gc_destroy(invert_gc);
#endif
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    શૂન્ય
gui_mch_invert_rectangle(પૂર્ણાંક r, પૂર્ણાંક c, પૂર્ણાંક nr, પૂર્ણાંક nc)
{
#જો GTK_CHECK_VERSION(3,0,0)
    અચલ GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_width, nr * gui.char_height
    };
    cairo_t * અચલ cr = cairo_create(gui.surface);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# જો CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1,9,2)
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# નહીંતર
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);

    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
#નહીંતર
    GdkGCValues કિંમતો;
    GdkGC *invert_gc;

    જો (gui.drawarea->window == ખાલી)
	પાછા;

    કિંમતો.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_કિંમતો(gui.drawarea->window,
				       &કિંમતો,
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
    શૂન્ય
gui_mch_iconify(શૂન્ય)
{
    gtk_window_iconify(GTK_WINDOW(gui.mainwin));
}

#જો defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    શૂન્ય
gui_mch_set_foreground(શૂન્ય)
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
    શૂન્ય
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    પૂર્ણાંક		i = 1;
#જો GTK_CHECK_VERSION(3,0,0)
    cairo_t    *cr;
#endif

    જો (gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા;

#જો GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
#endif

    gui_mch_set_fg_color(color);

#જો GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#નહીંતર
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    જો (mb_lefthalve(gui.row, gui.col))
	i = 2;
#જો GTK_CHECK_VERSION(3,0,0)
    cairo_set_લીટી_width(cr, 1.0);
    cairo_set_લીટી_cap(cr, CAIRO_લીટી_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.col) + 0.5, FILL_Y(gui.row) + 0.5,
	    i * gui.char_width - 1, gui.char_height - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#નહીંતર
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
    શૂન્ય
gui_mch_draw_part_cursor(પૂર્ણાંક w, પૂર્ણાંક h, guicolor_T color)
{
    જો (gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા;

    gui_mch_set_fg_color(color);

#જો GTK_CHECK_VERSION(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_જમણું ? FILL_X(gui.col + 1) - w :
# endif
	    FILL_X(gui.col), FILL_Y(gui.row) + gui.char_height - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#નહીંતર // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_જમણું ? FILL_X(gui.col + 1) - w :
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
    શૂન્ય
gui_mch_update(શૂન્ય)
{
    પૂર્ણાંક cnt = 0;	// prevent endless loop
    જ્યારે (g_main_context_pending(ખાલી) && !vim_is_આગત_buf_full()
	    && ++cnt < 100)
    {
#જો GTK_CHECK_VERSION(3,0,0)
	પૂર્ણાંક prio = 0;
	g_main_context_prepare(ખાલી, &prio);
	// peek internal scheduling of redraw
	જો (prio == GDK_PRIORITY_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#endif
	g_main_context_iteration(ખાલી, TRUE);
    }
}

    સ્થિર timeout_cb_type
આગત_timer_cb(gpointer data)
{
    પૂર્ણાંક *timed_out = (પૂર્ણાંક *) data;

    // Just inform the caller about the occurrence of it
    *timed_out = TRUE;

    પાછા FALSE;		// don't happen again
}

#ifdef FEAT_JOB_CHANNEL
    સ્થિર timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    પાછા TRUE;		// repeat
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
    પૂર્ણાંક
gui_mch_wait_for_chars(long wtime)
{
    પૂર્ણાંક		focus;
    guint	timer;
    સ્થિર પૂર્ણાંક	timed_out;
    પૂર્ણાંક		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    જો (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   આગત_timer_cb, &timed_out);
    નહીંતર
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    જો (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, ખાલી);
#endif

    focus = gui.in_focus;

    કરો
    {
	// Stop or start blinking when focus changes
	જો (gui.in_focus != focus)
	{
	    જો (gui.in_focus)
		gui_mch_start_blink();
	    નહીંતર
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	જો (did_add_timer)
	    // Need to recompute the waiting time.
	    જાઓ theend;
# endif
#endif

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	જો (!આગત_available())
	{
	    g_main_context_iteration(ખાલી, TRUE);
#ifdef GDK_WINDOWING_WAYLAND
    જો (gui.is_wayland && clip_star.state == SELECT_IN_PROGRESS)
	gui_may_flush();
#endif
	}

	// Got char, return immediately
	જો (આગત_available())
	{
	    retval = OK;
	    જાઓ theend;
	}
    } જ્યારે (wtime < 0 || !timed_out);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    જો (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    જો (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    પાછા retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    શૂન્ય
gui_mch_flush(શૂન્ય)
{
    જો (gui.mainwin == ખાલી || !gtk_widget_get_realized(gui.mainwin))
	પાછા;
#જો GTK_CHECK_VERSION(3,0,0)
    જો (dirty_region != ખાલી && gui.drawarea != ખાલી)
    {
	gtk_widget_queue_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = ખાલી;
    }
#નહીંતર
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       પાછા;
#endif
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    શૂન્ય
gui_mch_clear_block(પૂર્ણાંક row1arg, પૂર્ણાંક col1arg, પૂર્ણાંક row2arg, પૂર્ણાંક col2arg)
{

    પૂર્ણાંક col1 = check_col(col1arg);
    પૂર્ણાંક col2 = check_col(col2arg);
    પૂર્ણાંક row1 = check_row(row1arg);
    પૂર્ણાંક row2 = check_row(row2arg);

#જો GTK_CHECK_VERSION(3,0,0)
    જો (gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા;
#નહીંતર
    GdkColor color;

    જો (gui.drawarea->window == ખાલી)
	પાછા;

    color.pixel = gui.back_pixel;
#endif

#જો GTK_CHECK_VERSION(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	અચલ GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height
	};
	cairo_t * અચલ cr = cairo_create(gui.surface);
# જો GTK_CHECK_VERSION(3,22,2)
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# નહીંતર
	GdkWindow * અચલ win = gtk_widget_get_window(gui.drawarea);
	cairo_pattern_t * અચલ pat = gdk_window_get_background_pattern(win);
	જો (pat != ખાલી)
	    cairo_set_source(cr, pat);
	નહીંતર
	    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_fill(cr);
	cairo_destroy(cr);

	queue_draw_area(rect.x, rect.y, rect.width, rect.height);
    }
#નહીંતર // !GTK_CHECK_VERSION(3,0,0)
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

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર શૂન્ય
gui_gtk_window_clear(GdkWindow *win)
{
    અચલ GdkRectangle rect = {
	0, 0, gdk_window_get_width(win), gdk_window_get_height(win)
    };
    cairo_t * અચલ cr = cairo_create(gui.surface);
# જો GTK_CHECK_VERSION(3,22,2)
    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# નહીંતર
    cairo_pattern_t * અચલ pat = gdk_window_get_background_pattern(win);
    જો (pat != ખાલી)
	cairo_set_source(cr, pat);
    નહીંતર
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);
    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
}
#નહીંતર
# define gui_gtk_window_clear(win)  gdk_window_clear(win)
#endif

    શૂન્ય
gui_mch_clear_all(શૂન્ય)
{
    જો (gtk_widget_get_window(gui.drawarea) != ખાલી)
	gui_gtk_window_clear(gtk_widget_get_window(gui.drawarea));
}

#જો !GTK_CHECK_VERSION(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    સ્થિર શૂન્ય
check_copy_area(શૂન્ય)
{
    GdkEvent        *event;
    પૂર્ણાંક                expose_ગણતરી;
    જો (gui.visibility != GDK_VISIBILITY_PARTIAL)
	પાછા;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    કરો
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->window);
	જો (event == ખાલી)
	    તોડો; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.width, event->expose.area.height);
	expose_ગણતરી = event->expose.ગણતરી;
	gdk_event_free(event);
    }
    જ્યારે (expose_ગણતરી > 0); // more events follow
    gui_can_update_cursor();
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#જો GTK_CHECK_VERSION(3,0,0)
    સ્થિર શૂન્ય
gui_gtk_surface_copy_rect(પૂર્ણાંક dest_x, પૂર્ણાંક dest_y,
			  પૂર્ણાંક src_x,  પૂર્ણાંક src_y,
			  પૂર્ણાંક width,  પૂર્ણાંક height)
{
    cairo_t * અચલ cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    સ્થિર cairo_surface_t *scroll_scratch = ખાલી;
    સ્થિર પૂર્ણાંક scratch_w = 0, scratch_h = 0;
    પૂર્ણાંક last_row = Rows - 1, last_row_y = last_row * gui.char_height;
    bool last_row_overlap = (dest_y + height) > last_row_y;
    જો (gui.is_wayland && ( !(State & MODE_CMDLINE) || !last_row_overlap) )
    {
	જો (dest_y < src_y) // scroll up
	{
	    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	    cairo_set_source_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, width, height);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	નહીંતર
	{   //  reuse surface when scrolling, only realloc if larger
	    જો (scroll_scratch == ખાલી || width > scratch_w || height > scratch_h)
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
    નહીંતર
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, width, height);
	cairo_clip(cr);
	cairo_ધકેલો_group(cr);
	cairo_set_source_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_કાઢો_group_to_source(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#endif

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    શૂન્ય
gui_mch_delete_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
#જો GTK_CHECK_VERSION(3,0,0)
    અચલ પૂર્ણાંક ncols = gui.scroll_region_જમણું - gui.scroll_region_ડાબું + 1;
    અચલ પૂર્ણાંક nrows = gui.scroll_region_bot - row + 1;
    અચલ પૂર્ણાંક src_nrows = nrows - અંક_લીટીઓ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row + અંક_લીટીઓ),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    gui.scroll_region_bot - અંક_લીટીઓ + 1, gui.scroll_region_ડાબું,
	    gui.scroll_region_bot,		   gui.scroll_region_જમણું);
    queue_draw_area(
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#નહીંતર
    જો (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	પાછા;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_ડાબું),
	    FILL_Y(row + અંક_લીટીઓ),
	    gui.char_width * (gui.scroll_region_જમણું
					    - gui.scroll_region_ડાબું + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - અંક_લીટીઓ + 1));

    gui_clear_block(gui.scroll_region_bot - અંક_લીટીઓ + 1,
						       gui.scroll_region_ડાબું,
		    gui.scroll_region_bot, gui.scroll_region_જમણું);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    શૂન્ય
gui_mch_ઉમેરો_લીટીઓ(પૂર્ણાંક row, પૂર્ણાંક અંક_લીટીઓ)
{
#જો GTK_CHECK_VERSION(3,0,0)
    અચલ પૂર્ણાંક ncols = gui.scroll_region_જમણું - gui.scroll_region_ડાબું + 1;
    અચલ પૂર્ણાંક nrows = gui.scroll_region_bot - row + 1;
    અચલ પૂર્ણાંક src_nrows = nrows - અંક_લીટીઓ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row + અંક_લીટીઓ),
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    row,		 gui.scroll_region_ડાબું,
	    row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);
    queue_draw_area(
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#નહીંતર
    જો (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	પાછા;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row + અંક_લીટીઓ),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_ડાબું), FILL_Y(row),
	    gui.char_width * (gui.scroll_region_જમણું
					    - gui.scroll_region_ડાબું + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - અંક_લીટીઓ + 1));

    gui_clear_block(row, gui.scroll_region_ડાબું,
				row + અંક_લીટીઓ - 1, gui.scroll_region_જમણું);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    શૂન્ય
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	target;
    unsigned	i;
    time_t	start;

    માટે (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	જો (!clip_html && selection_targets[i].info == TARGET_HTML)
	    ચાલુ;
	received_selection = RS_NONE;
	target = gdk_atom_intern(selection_targets[i].target, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, target,
			      (guint32)GDK_CURRENT_TIME);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	start = time(ખાલી);
	જ્યારે (received_selection == RS_NONE && time(ખાલી) < start + 3)
	    g_main_context_iteration(ખાલી, TRUE);	// wait for selection_received_cb

	જો (received_selection != RS_FAIL)
	    પાછા;
    }

    જો (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    શૂન્ય
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    જો (in_selection_clear_event)
	પાછા;

    gtk_selection_owner_set(ખાલી, cbd->gtk_sel_atom, gui.event_time);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    પૂર્ણાંક
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    જો (is_autocmd_blocked())
	પાછા OK;

    પૂર્ણાંક success;

    success = gtk_selection_owner_set(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_time);
    // don't update on every visual selection change
    જો (!(cbd->owned && VIsual_active))
	gui_gtk_set_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    પાછા (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    શૂન્ય
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

#જો defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    પૂર્ણાંક
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    પાછા gdk_selection_owner_get(cbd->gtk_sel_atom) != ખાલી;
}
#endif


#જો defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    શૂન્ય
gui_mch_menu_grey(vimmenu_T *menu, પૂર્ણાંક grey)
{
    જો (menu->id == ખાલી)
	પાછા;

    જો (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    જો (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_set_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    શૂન્ય
gui_mch_menu_hidden(vimmenu_T *menu, પૂર્ણાંક hidden)
{
    જો (menu->id == 0)
	પાછા;

    જો (hidden)
    {
	જો (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    નહીંતર
    {
	જો (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    શૂન્ય
gui_mch_draw_menubar(શૂન્ય)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#endif // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    શૂન્ય
gui_mch_enable_scrollbar(scrollbar_T *sb, પૂર્ણાંક flag)
{
    જો (sb->id == ખાલી)
	પાછા;

    gtk_widget_set_visible(sb->id, flag);
    update_window_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#જો GTK_CHECK_VERSION(3,0,0)
    પાછા (long_u)pixel;
#નહીંતર
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (unsigned long)pixel, &color);

    પાછા (guicolor_T)(
	    (((unsigned)color.red   & 0xff00) << 8)
	 |  ((unsigned)color.green & 0xff00)
	 | (((unsigned)color.blue  & 0xff00) >> 8));
#endif
}

/*
 * Get current mouse coordinates in text window.
 */
    શૂન્ય
gui_mch_getmouse(પૂર્ણાંક *x, પૂર્ણાંક *y)
{
    gui_gtk_get_pointer(gui.drawarea, x, y, ખાલી);
}

    શૂન્ય
gui_mch_setmouse(પૂર્ણાંક x, પૂર્ણાંક y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    જો (dpy)
	XWarpPointer(dpy, (Window)0,
		GDK_WINDOW_XID(gtk_widget_get_window(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
સ્થિર પૂર્ણાંક last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    શૂન્ય
gui_mch_mousehide(પૂર્ણાંક hide)
{
    જો (gui.pointer_hidden == hide)
	પાછા;

    gui.pointer_hidden = hide;
    જો (gtk_widget_get_window(gui.drawarea) && gui.blank_pointer != ખાલી)
    {
	જો (hide)
	    gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		    gui.blank_pointer);
	નહીંતર
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#નહીંતર
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), ખાલી);
#endif
    }
}

#જો defined(FEAT_MOUSESHAPE)

# જો GTK_CHECK_VERSION(3,0,0)
સ્થિર અચલ અક્ષર * mshape_css_names[] =
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
# નહીંતર
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
સ્થિર અચલ પૂર્ણાંક mshape_ids[] =
{
    GDK_ડાબું_PTR,		// arrow
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
    GDK_જમણું_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_ડાબું_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    શૂન્ય
mch_set_mouse_shape(પૂર્ણાંક shape)
{
    GdkCursor	   *c;
    પૂર્ણાંક		   id;                    // Only id or css_name is used.
# જો GTK_CHECK_VERSION(3,0,0)
    અચલ અક્ષર     *css_name = "default";
    GdkDisplay     *disp;
# endif

    જો (gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા;

    જો (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);
    નહીંતર
    {
	જો (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    જો (id >= GDK_LAST_CURSOR)
		id = GDK_ડાબું_PTR;
	    નહીંતર
		id &= ~1;	// they are always even (why?)
	}
# જો GTK_CHECK_VERSION(3,0,0)
	નહીંતર જો (shape < (પૂર્ણાંક)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
# નહીંતર
	નહીંતર જો (shape < (પૂર્ણાંક)ARRAY_LENGTH(mshape_ids))
	    id = mshape_ids[shape];
# endif
	નહીંતર
	    પાછા;
# જો GTK_CHECK_VERSION(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# નહીંતર
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), c);
# જો GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(c));
# નહીંતર
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    જો (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif // FEAT_MOUSESHAPE


#જો defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)
# define SIGN_ASPECT ((દ્વિગુણ)SIGN_HEIGHT / (દ્વિગુણ)SIGN_WIDTH)

    શૂન્ય
gui_mch_drawsign(પૂર્ણાંક row, પૂર્ણાંક col, પૂર્ણાંક typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    જો (sign == ખાલી || gui.drawarea == ખાલી
	    || gtk_widget_get_window(gui.drawarea) == ખાલી)
	પાછા;

    પૂર્ણાંક width;
    પૂર્ણાંક height;
    પૂર્ણાંક xoffset;
    પૂર્ણાંક yoffset;
    પૂર્ણાંક need_scale;

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
    જો (need_scale)
    {
	દ્વિગુણ  aspect;
	પૂર્ણાંક	    w = width;
	પૂર્ણાંક	    h = height;

	// Keep the original aspect ratio
	aspect = (દ્વિગુણ)height / (દ્વિગુણ)width;
	width  = (દ્વિગુણ)SIGN_WIDTH * SIGN_ASPECT / aspect;
	width  = MIN(width, SIGN_WIDTH);
	જો (((દ્વિગુણ)(MAX(height, SIGN_HEIGHT)) /
		    (દ્વિગુણ)(MIN(height, SIGN_HEIGHT))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    height = (દ્વિગુણ)SIGN_HEIGHT * SIGN_ASPECT / aspect;
	    height = MIN(height, SIGN_HEIGHT);
	}
	નહીંતર
	    height = (દ્વિગુણ)width * aspect;

	જો (w == width && h == height)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	નહીંતર
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, width, height,
		    GDK_INTERP_BILINEAR);
	    જો (sign == ખાલી)
		પાછા; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (width  - SIGN_WIDTH)  / 2;
    yoffset = (height - SIGN_HEIGHT) / 2;

# જો GTK_CHECK_VERSION(3,0,0)
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
# નહીંતર // !GTK_CHECK_VERSION(3,0,0)
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
    જો (need_scale)
	g_object_unref(sign);
}

    શૂન્ય *
gui_mch_register_sign(char_u *signfile)
{
    જો (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = ખાલી;
	char_u	    *message;

	sign = gdk_pixbuf_new_from_file((અચલ અક્ષર *)signfile, &error);

	જો (error == ખાલી)
	    પાછા sign;

	message = (char_u *)error->message;

	જો (message != ખાલી && આગત_conv.vc_type != CONV_NONE)
	    message = string_convert(&આગત_conv, message, ખાલી);

	જો (message != ખાલી)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", message);

	    જો (આગત_conv.vc_type != CONV_NONE)
		vim_free(message);
	}
	g_error_free(error);
    }

    પાછા ખાલી;
}

    શૂન્ય
gui_mch_destroy_sign(શૂન્ય *sign)
{
    જો (sign != ખાલી)
	g_object_unref(sign);
}

#endif // FEAT_SIGN_ICONS

#જો defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

પ્રકારવ્યાખ્યા સંરચના
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
    સ્થિર શૂન્ય
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->data;
    uint8_t *dst;
    પૂર્ણાંક	    w = img->width;
    પૂર્ણાંક	    h = img->height;
    પૂર્ણાંક	    stride;
    પૂર્ણાંક	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_data(surf);
    stride = cairo_image_surface_get_stride(surf);

    જો (img->fmt == IMAGE_FORMAT_RGBA)
    {
	માટે (y = 0; y < h; ++y)
	{
	    unsigned પૂર્ણાંક *p = (unsigned પૂર્ણાંક *)(dst + y * stride);

	    માટે (x = 0; x < w; ++x)
	    {
		પૂર્ણાંક	idx = (y * w + x) * 4;
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
    નહીંતર
    {
	માટે (y = 0; y < h; ++y)
	{
	    unsigned પૂર્ણાંક *p = (unsigned પૂર્ણાંક *)(dst + y * stride);

	    માટે (x = 0; x < w; ++x)
	    {
		પૂર્ણાંક	idx = (y * w + x) * 3;
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

    પૂર્ણાંક
image_gui_init(image_T *img)
{
    // We only support GTK3
# જો GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    જો (ctx == ખાલી)
	પાછા FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->width, img->height);

    જો (cairo_surface_status(ctx->surf) != CAIRO_STATUS_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	જાઓ fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_data = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    પાછા OK;
fail:
    vim_free(ctx);
    પાછા FAIL;
# નહીંતર
    પાછા FAIL;
# endif
}

    શૂન્ય
image_gui_uninit(image_T *img)
{
# જો GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = img->backend_data;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    પૂર્ણાંક
image_placement_gui_init(image_placement_T *place UNUSED)
{
# જો GTK_CHECK_VERSION(3,0,0)
    પાછા OK;
# નહીંતર
    પાછા FAIL;
# endif
}

    શૂન્ય
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    શૂન્ય
image_placement_gui_draw(image_placement_T *place)
{
# જો GTK_CHECK_VERSION(3,0,0)
    image_T		    *img = place->img;
    image_cairo_T	    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    પૂર્ણાંક			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    જો (rects == ખાલી || n_rects == 0)
	પાછા;

    cairo_surface_set_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    માટે (પૂર્ણાંક i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	પૂર્ણાંક		row, col;
	પૂર્ણાંક		posx, posy;
	પૂર્ણાંક		x, y, w, h;

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

    શૂન્ય
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE
