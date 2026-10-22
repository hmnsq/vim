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
# اگر defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
بیرونی تهی bonobo_dock_item_set_behavior(BonoboDockItem *dock_item, BonoboDockItemBehavior beh);
#endif

#اگر defined(FEAT_GUI_GTK)
# اگر GTK_CHECK_VERSION(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# وگرنه
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# وگرنه
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
شمارش
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
ایستا ثابت GtkTargetEntry selection_targets[] =
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
ایستا ثابت GtkTargetEntry dnd_targets[] =
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

#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#endif

#اگر !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ایستا GdkAtom wm_protocols_atom = GDK_NONE;
ایستا GdkAtom save_yourself_atom = GDK_NONE;
#endif

/*
 * Atoms used to control/reference X11 selections.
 */
ایستا GdkAtom html_atom = GDK_NONE;
ایستا GdkAtom utf8_string_atom = GDK_NONE;
ایستا GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
ایستا GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
ایستا GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
ایستا GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
ایستا ساختار special_کلید
{
    guint کلید_sym;
    char_u code0;
    char_u code1;
}
ثابت special_کلیدها[] =
{
    {GDK_Up,		'k', 'u'},
    {GDK_Down,		'k', 'd'},
    {GDK_چپ,		'k', 'l'},
    {GDK_راست,		'k', 'r'},
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
    {GDK_درج,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_BackTab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_End,		'@', '7'},
    {GDK_Prior,		'k', 'P'},
    {GDK_Next,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_چپ,	'k', 'l'},
    {GDK_KP_راست,	'k', 'r'},
    {GDK_KP_Up,		'k', 'u'},
    {GDK_KP_Down,	'k', 'd'},
    {GDK_KP_درج,	KS_EXTRA, (char_u)KE_KINS},
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
#define ARG_HAS_مقدار	0x0100	// a value is expected after the argument
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
تعریفنوع ساختار
{
    ثابت نویسه	    *name;
    unsigned صحیح    flags;
}
cmdline_option_T;

ایستا ثابت cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_مقدار},
    {"-font",		ARG_FONT|ARG_HAS_مقدار},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_مقدار},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_مقدار},
    {"-rv",		ARG_REVERSE},
    {"-reverse",	ARG_REVERSE},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_مقدار},
    {"-background",	ARG_BACKGROUND|ARG_HAS_مقدار},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_مقدار},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_مقدار},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_مقدار},
#ifdef FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_مقدار},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_مقدار},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_مقدار},	// not implemented
#endif
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_مقدار|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_مقدار|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_مقدار|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_مقدار},
#ifdef FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_مقدار},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_مقدار},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# اگر 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#endif
    {پوچ, 0}
};

ایستا صحیح    gui_argc = 0;
ایستا نویسه **gui_argv = پوچ;

ایستا ثابت نویسه *role_argument = پوچ;
#اگر defined(USE_GNOME_SESSION)
ایستا ثابت نویسه *restart_command = پوچ;
ایستا       نویسه *abs_restart_command = پوچ;
#endif
ایستا صحیح found_iconic_arg = FALSE;

#ifdef FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ایستا صحیح using_gnome = 0;
#وگرنه
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
# اگر 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
تعریفنوع ساختار resize_history {
    صحیح used;		// If true, can't match for discard. Only matches once.
    صحیح width;
    صحیح height;
    صحیح from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحیح seq;		// for ch_log messages
# endif
    ساختار resize_history *next;
} resize_hist_T;

// never NULL during execution
ایستا resize_hist_T *latest_resize_hist;
// list of stale resize requests
ایستا resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ایستا صحیح mch_csd_width = 0;
ایستا صحیح mch_csd_height = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
ایستا صحیح mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    ایستا تهی
alloc_resize_hist(صحیح width, صحیح height, صحیح from_shellsize)
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
    ch_log(پوچ, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, width, height, (صحیح)Columns, (صحیح)Rows);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    ایستا تهی
clear_resize_hists(تهی)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    صحیح		    i = 0;
# endif

    اگر (latest_resize_hist)
	latest_resize_hist->used = TRUE;
    وقتی (old_resize_hists != پوچ)
    {
	resize_hist_T *next_hist = old_resize_hists->next;

	vim_free(old_resize_hists);
	old_resize_hists = next_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(پوچ, "gui_gtk: free %d hists", i);
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
    ایستا صحیح
match_stale_width_height(صحیح width, صحیح height)
{
    resize_hist_T *hist = old_resize_hists;

    برای (hist = old_resize_hists; hist != پوچ; hist = hist->next)
	اگر (MATCH_WIDTH_HEIGHT(hist, width, height))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(پوچ, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->used = TRUE;
	    بازگشت TRUE;
	}
    بازگشت FALSE;
}

# اگر defined(EXITFREE)
    ایستا تهی
free_all_resize_hist(تهی)
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
ایستا guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    تهی
gui_mch_prepare(صحیح *argc, نویسه **argv)
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

    اگر (strchr(argv[0], G_DIR_SEPARATOR) != پوچ)
    {
	char_u buf[MAXPATHL];

	اگر (mch_FullName((char_u *)argv[0], buf, (صحیح)اندازه(buf), TRUE) == OK)
	{
	    abs_restart_command = (نویسه *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#endif

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(نویسه *, *argc + 1);

    g_return_if_fail(gui_argv != پوچ);

    gui_argv[gui_argc++] = argv[i++];

    وقتی (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	اگر (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    ادامه;
	}

	// Look for argv[i] in cmdline_options[] table.
	برای (option = &cmdline_options[0]; option->name != پوچ; ++option)
	{
	    len = strlen(option->name);

	    اگر (strncmp(argv[i], option->name, len) == 0)
	    {
		اگر (argv[i][len] == '\0')
		    بشکن;
		// allow --foo=bar style
		اگر (argv[i][len] == '=' && (option->flags & ARG_HAS_مقدار))
		    بشکن;
#ifdef FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		اگر (vim_strchr((char_u *)":=", argv[i][len]) != پوچ
			&& (option->flags & ARG_INDEX_MASK) == ARG_NETBEANS)
		    بشکن;
#endif
	    }
	    وگرنه اگر ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (نویسه *)option->name;
		بشکن;
	    }
	}
	اگر (option->name == پوچ) // no match
	{
	    ++i;
	    ادامه;
	}

	اگر (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	وگرنه
	{
	    نویسه *مقدار = پوچ;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    اگر (option->flags & ARG_HAS_مقدار)
	    {
		اگر (argv[i][len] == '=')
		    مقدار = &argv[i][len + 1];
		وگرنه اگر (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    مقدار = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    انتخاب (option->flags & ARG_INDEX_MASK)
	    {
		حالت ARG_REVERSE:
		    found_reverse_arg = TRUE;
		    بشکن;
		حالت ARG_NOREVERSE:
		    found_reverse_arg = FALSE;
		    بشکن;
		حالت ARG_FONT:
		    font_argument = مقدار;
		    بشکن;
		حالت ARG_GEOMETRY:
		    اگر (مقدار != پوچ)
			gui.geom = vim_strsave((char_u *)مقدار);
		    بشکن;
		حالت ARG_BACKGROUND:
		    background_argument = مقدار;
		    بشکن;
		حالت ARG_FOREGROUND:
		    foreground_argument = مقدار;
		    بشکن;
		حالت ARG_ICONIC:
		    found_iconic_arg = TRUE;
		    بشکن;
		حالت ARG_ROLE:
		    role_argument = مقدار; // used later in gui_mch_open()
		    بشکن;
#ifdef FEAT_NETBEANS_INTG
		حالت ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    بشکن;
#endif
		پیشفرض:
		    بشکن;
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
	وگرنه
	{
	    // Remove the flag from the argument vector.
	    اگر (--*argc > i)
	    {
		صحیح n_strip = 1;

		// Move the argument's value as well, if there is one.
		اگر ((option->flags & ARG_HAS_مقدار)
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
						(*argc - i) * اندازه(نویسه *));
	    }
	    argv[*argc] = پوچ;
	}
    }

    gui_argv[gui_argc] = پوچ;
}

#اگر defined(EXITFREE)
    تهی
gui_mch_free_all(تهی)
{
    vim_free(gui_argv);
# اگر defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#endif

#اگر !GTK_CHECK_VERSION(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ایستا gint
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
    اگر (gui.text_gc != پوچ)
	gdk_gc_set_exposures(gui.text_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    بازگشت FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا gboolean
draw_event(GtkWidget *widget UNUSED,
	   cairo_t   *cr,
	   gpointer   user_data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    اگر (gui.starting)
	بازگشت FALSE;

    out_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_set_source_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    بازگشت FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# وگرنه
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# اگر GTK_CHECK_VERSION(3,10,0)
    ایستا gboolean
scale_factor_event(GtkWidget *widget,
		   GParamSpec* pspec UNUSED,
		   gpointer   user_data UNUSED)
{
    اگر (gui.surface != پوچ)
	cairo_surface_destroy(gui.surface);

    صحیح	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    صحیح	    usable_height = h;
    اگر (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_height);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  اگر defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(widget);
#  endif

    بازگشت TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#وگرنه // !GTK_CHECK_VERSION(3,0,0)
    ایستا gint
expose_event(GtkWidget *widget UNUSED,
	     GdkEventExpose *event,
	     gpointer data UNUSED)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    اگر (gui.starting)
	بازگشت FALSE;

    out_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.width, event->area.height);

    // Clear the border areas if needed
    اگر (event->area.x < FILL_X(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, FILL_X(0), 0);
    اگر (event->area.y < FILL_Y(0))
	gdk_window_clear_area(gui.drawarea->window, 0, 0, 0, FILL_Y(0));
    اگر (event->area.x > FILL_X(Columns))
	gdk_window_clear_area(gui.drawarea->window,
			      FILL_X((صحیح)Columns), 0, 0, 0);
    اگر (event->area.y > FILL_Y(Rows))
	gdk_window_clear_area(gui.drawarea->window, 0, FILL_Y((صحیح)Rows), 0, 0);

    بازگشت FALSE;
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#ifdef FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ایستا gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer data UNUSED)
{
    اگر (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (صحیح)GDK_PROPERTY_NEW_مقدار
	    && GDK_WINDOW_XID(event->window) == commWindow
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.window = commWindow;
	xev.xproperty.state = Property_New_مقدار;
	serverEventProc(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(widget)),
								      &xev, 0);
    }
    بازگشت FALSE;
}
#endif // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ایستا تهی
gtk_settings_xft_dpi_changed_cb(GtkSettings *gtk_settings UNUSED,
				    GParamSpec *pspec UNUSED,
				    gpointer data UNUSED)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    اگر (gui.text_context != پوچ)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    اگر (gui.norm_font != پوچ)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? پوچ : p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

تعریفنوع gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ایستا guint
timeout_add(صحیح time, timeout_cb_type (*callback)(gpointer), صحیح *flagp)
{
    بازگشت g_timeout_add((guint)time, (GSourceFunc)callback, flagp);
}

    ایستا تهی
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

ایستا صحیح blink_state = BLINK_NONE;
ایستا long_u blink_waittime = 700;
ایستا long_u blink_ontime = 400;
ایستا long_u blink_offtime = 250;
ایستا guint blink_timer = 0;

    صحیح
gui_mch_is_blinking(تهی)
{
    بازگشت blink_state != BLINK_NONE;
}

    صحیح
gui_mch_is_blink_off(تهی)
{
    بازگشت blink_state == BLINK_OFF;
}

    تهی
gui_mch_set_blinking(long waittime, long on, long off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    تهی
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

    ایستا timeout_cb_type
blink_cb(gpointer data UNUSED)
{
    اگر (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_add(blink_offtime, blink_cb, پوچ);
    }
    وگرنه
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_add(blink_ontime, blink_cb, پوچ);
    }
    gui_may_flush();

    بازگشت FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    تهی
gui_mch_start_blink(تهی)
{
    اگر (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    اگر (blink_waittime && blink_ontime && blink_offtime && gui.in_focus)
    {
	blink_timer = timeout_add(blink_waittime, blink_cb, پوچ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ایستا gint
enter_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    اگر (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes there
    اگر (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    بازگشت FALSE;
}

    ایستا gint
leave_notify_event(GtkWidget *widget UNUSED,
		   GdkEventCrossing *event UNUSED,
		   gpointer data UNUSED)
{
    اگر (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    بازگشت FALSE;
}

    ایستا gint
focus_in_event(GtkWidget *widget,
	       GdkEventFocus *event UNUSED,
	       gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    اگر (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    وگرنه
#endif
	gui_focus_change(TRUE);

    اگر (blink_state == BLINK_NONE)
	gui_mch_start_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    اگر (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    بازگشت TRUE;
}

    ایستا gint
focus_out_event(GtkWidget *widget UNUSED,
		GdkEventFocus *event UNUSED,
		gpointer data UNUSED)
{
#ifdef FEAT_GUI_DIALOG
    اگر (gui.dialogs_active > 0)
	++gui.dialog_focus_pending;
#endif

    gui_focus_change(FALSE);

    اگر (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    بازگشت TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    ایستا صحیح
keyval_to_string(unsigned صحیح keyval, char_u *string)
{
    صحیح	    len;
    guint32 uc;

    uc = gdk_keyval_to_unicode(keyval);
    اگر (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((صحیح)uc, string);
    }
    وگرنه
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	انتخاب (keyval)
	{
	    حالت GDK_Tab: حالت GDK_KP_Tab: حالت GDK_ISO_چپ_Tab:
		string[0] = TAB;
		بشکن;
	    حالت GDK_Linefeed:
		string[0] = NL;
		بشکن;
	    حالت GDK_Return: حالت GDK_ISO_Enter: حالت GDK_3270_Enter:
		string[0] = CAR;
		بشکن;
	    حالت GDK_Escape:
		string[0] = ESC;
		بشکن;
	    پیشفرض:
		len = 0;
		بشکن;
	}
    }
    string[len] = NUL;

    بازگشت len;
}

    ایستا صحیح
modifiers_gdk2vim(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_SHIFT_MASK)
	modifiers |= MOD_MASK_SHIFT;
    اگر (state & GDK_CONTROL_MASK)
	modifiers |= MOD_MASK_CTRL;
    اگر (state & GDK_MOD1_MASK)
	modifiers |= MOD_MASK_ALT;
#اگر GTK_CHECK_VERSION(2,10,0)
    اگر (state & GDK_META_MASK)
	modifiers |= MOD_MASK_META;
    اگر (state & GDK_SUPER_MASK)
	modifiers |= MOD_MASK_CMD;
#وگرنه
    اگر (state & GDK_MOD4_MASK)
	modifiers |= MOD_MASK_CMD;
#endif

    بازگشت modifiers;
}

    ایستا صحیح
modifiers_gdk2mouse(guint state)
{
    صحیح modifiers = 0;

    اگر (state & GDK_SHIFT_MASK)
	modifiers |= MOUSE_SHIFT;
    اگر (state & GDK_CONTROL_MASK)
	modifiers |= MOUSE_CTRL;
    اگر (state & GDK_MOD1_MASK)
	modifiers |= MOUSE_ALT;

    بازگشت modifiers;
}

/*
 * Main keyboard handler:
 */
    ایستا gint
کلید_press_event(GtkWidget *widget UNUSED,
		Gdk_Event_کلید *event,
		gpointer data UNUSED)
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
    صحیح		ctrl_prefix_added = 0;

    gui.event_time = event->time;
    کلید_sym = event->keyval;
    state = event->state;

#ifdef FEAT_XIM
    اگر (xim_queue_کلید_press_event(event, TRUE))
	بازگشت TRUE;
#endif

#ifdef SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    اگر (کلید_sym == SunXK_F36 || کلید_sym == SunXK_F37)
	len = 0;
    وگرنه
#endif
    {
	len = keyval_to_string(کلید_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	اگر (len > 1 && درون_داد_conv.vc_type != CONV_NONE)
	    len = convert_درون_داد(string2, len, اندازه(string2));

	s = string2;
	d = string;
	برای (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    اگر (d[-1] == CSI && d + 2 < string + اندازه(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (صحیح)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    اگر (کلید_sym == GDK_ISO_چپ_Tab)
    {
	کلید_sym = GDK_Tab;
	state |= GDK_SHIFT_MASK;
    }

#ifdef FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    اگر ((state & GDK_MOD1_MASK)
	    && gui.menu_is_active
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	بازگشت FALSE;
#endif

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    اگر (len == 0 || len == 1)
    {
	برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	{
	    اگر (special_کلیدها[i].کلید_sym == کلید_sym)
	    {
		string[0] = CSI;
		string[1] = special_کلیدها[i].code0;
		string[2] = special_کلیدها[i].code1;
		len = -3;
		بشکن;
	    }
	}
    }

#ifdef GDK_کلید_dead_circumflex
    // Belgian Ctrl+[ workaround
    اگر (len == 0 && کلید_sym == GDK_کلید_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_MASK_CTRL;
	string[3] = '[';
	len = 4;
	add_to_درون_داد_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	بازگشت TRUE;
    }
#endif

    اگر (len == 0)   // Unrecognized key
	بازگشت TRUE;

    // For some keys a shift modifier is translated into another key code.
    اگر (len == -3)
	کلید = TO_SPECIAL(string[1], string[2]);
    وگرنه
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
	string[1] = K_SECOND(کلید);
	string[2] = K_THIRD(کلید);
	len = 3;
    }
    وگرنه
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	کلید = may_adjust_کلید_for_ctrl(modifiers, کلید);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_shift_modifier(modifiers, کلید);

	len = mb_char2bytes(کلید, string);
    }

    اگر (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	add_to_درون_داد_buf(string2, 3);
	اگر (modifiers == 0x4)
	    ctrl_prefix_added = 1;
    }

    // Check if the key interrupts.
    {
	صحیح int_ch = check_for_interrupt(کلید, modifiers);

	اگر (int_ch != NUL)
	{
	    trash_درون_داد_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    اگر (ctrl_prefix_added && len == 2
		    && ((صحیح)string[0]) == 195
		    && ((صحیح)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    add_to_درون_داد_buf(string, len);

    // blank out the pointer if necessary
    اگر (p_mh)
	gui_mch_mousehide(TRUE);

    بازگشت TRUE;
}

#اگر defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    ایستا gboolean
کلید_release_event(GtkWidget *widget UNUSED,
		  Gdk_Event_کلید *event UNUSED,
		  gpointer data UNUSED)
{
# اگر defined(FEAT_XIM)
    gui.event_time = event->time;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    بازگشت xim_queue_کلید_press_event(event, FALSE);
# وگرنه
    بازگشت TRUE;
# endif
}
#endif


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ایستا صحیح in_selection_clear_event = FALSE;

    ایستا gint
selection_clear_event(GtkWidget		*widget UNUSED,
		      GdkEventSelection	*event,
		      gpointer		user_data UNUSED)
{
    in_selection_clear_event = TRUE;
    اگر (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    وگرنه
	clip_lose_selection(&clip_star);
    in_selection_clear_event = FALSE;

    بازگشت TRUE;
}

#define RS_NONE	0	// selection_received_cb() not called yet
#define RS_OK	1	// selection_received_cb() called and OK
#define RS_FAIL	2	// selection_received_cb() called and failed
ایستا صحیح received_selection = RS_NONE;

    ایستا تهی
selection_received_cb(GtkWidget		*widget UNUSED,
		      GtkSelectionData	*data,
		      guint		time_ UNUSED,
		      gpointer		user_data UNUSED)
{
    Clipboard_T	    *cbd;
    char_u	    *text;
    char_u	    *tmpbuf = پوچ;
    guchar	    *tmpbuf_utf8 = پوچ;
    صحیح		    len;
    صحیح		    motion_type = MAUTO;

    اگر (gtk_selection_data_get_selection(data) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وگرنه
	cbd = &clip_star;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    اگر (text == پوچ || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	بازگشت;
    }

    اگر (gtk_selection_data_get_data_type(data) == vim_atom
	    || gtk_selection_data_get_data_type(data) == vim_mt_atom)
    {
	motion_type = *text++;
	--len;
    }
    وگرنه اگر (gtk_selection_data_get_data_type(data) == vimenc_atom
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
	اگر (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, text, &len);
	    اگر (tmpbuf != پوچ)
		text = tmpbuf;
	    convert_setup(&conv, پوچ, پوچ);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    وگرنه
    {
	tmpbuf_utf8 = gtk_selection_data_get_text(data);
	اگر (tmpbuf_utf8 != پوچ)
	{
	    len = STRLEN(tmpbuf_utf8);
	    اگر (درون_داد_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&درون_داد_conv, tmpbuf_utf8, &len);
		اگر (tmpbuf != پوچ)
		    text = tmpbuf;
	    }
	    وگرنه
		text = tmpbuf_utf8;
	}
	وگرنه اگر (len >= 2 && text[0] == 0xff && text[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_setup_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    اگر (conv.vc_type != CONV_NONE)
	    {
		text += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, text, &len);
		convert_setup(&conv, پوچ, پوچ);
	    }
	    اگر (tmpbuf != پوچ)
		text = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    وقتی (len > 0 && text[len - 1] == NUL)
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
    ایستا تهی
selection_get_cb(GtkWidget	    *widget UNUSED,
		 GtkSelectionData   *selection_data,
		 guint		    info,
		 guint		    time_ UNUSED,
		 gpointer	    user_data UNUSED)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    صحیح		    length;
    صحیح		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    اگر (gtk_selection_data_get_selection(selection_data)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    وگرنه
	cbd = &clip_star;

    اگر (!cbd->owned)
	بازگشت;			// Shouldn't ever happen

    اگر (info != (guint)TARGET_STRING
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
	بازگشت;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    اگر (motion_type < 0 || string == پوچ)
	بازگشت;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    length = MIN(tmplen, (long_u)(G_MAXINT - 1));

    اگر (info == (guint)TARGET_VIM || info == (guint)TARGET_VIM_MT)
    {
	tmpbuf = alloc(length + 1);
	اگر (tmpbuf != پوچ)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)length);
	}
	// For our own format, the first byte contains the motion type
	++length;
	vim_free(string);
	string = tmpbuf;
	اگر (info == (guint)TARGET_VIM)
	    type = vim_atom;
	وگرنه
	    type = vim_mt_atom;
    }

    وگرنه اگر (info == (guint)TARGET_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_setup_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	اگر (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &length);
	    convert_setup(&conv, پوچ, پوچ);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	اگر (string != پوچ)
	{
	    tmpbuf = alloc(length + 2);
	    اگر (tmpbuf != پوچ)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)length);
		vim_free(string);
		string = tmpbuf;
		length += 2;
	    }

#اگر !GTK_CHECK_VERSION(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_data->type = selection_data->target;
	    selection_data->format = 16;	// 16 bits per char
#endif
	    gtk_selection_data_set(selection_data, html_atom, 16,
							      string, length);
	    vim_free(string);
	}
	بازگشت;
    }
    وگرنه اگر (info == (guint)TARGET_VIMENC || info == (guint)TARGET_VIMENC_MT)
    {
	صحیح l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(length + l + 2);
	اگر (tmpbuf != پوچ)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)length);
	    length += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	اگر (info == (guint)TARGET_VIMENC)
	    type = vimenc_atom;
	وگرنه
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    وگرنه
    {
	اگر (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &length);
	    vim_free(string);
	    اگر (tmpbuf == پوچ)
		بازگشت;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	اگر (g_utf8_validate((ثابت نویسه *)string, (gssize)length, پوچ))
	{
	    gtk_selection_data_set_text(selection_data,
					(ثابت نویسه *)string, length);
	}
	vim_free(string);
	بازگشت;
    }

    اگر (string != پوچ)
    {
#اگر !GTK_CHECK_VERSION(3,0,0)
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
    صحیح
gui_mch_early_init_check(صحیح give_message)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    اگر ((p == پوچ || *p == NUL) && (q == پوچ || *q == NUL))
    {
	gui.dying = true;
	اگر (give_message)
	    emsg(_((نویسه *)e_cannot_open_display));
	بازگشت FAIL;
    }
    بازگشت OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    صحیح
gui_mch_init_check(تهی)
{
#ifdef USE_GRESOURCE
    ایستا صحیح res_registered = FALSE;

    اگر (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_register_resource();
    }
#endif

#ifdef FEAT_GUI_GNOME
    اگر (gtk_socket_id == 0)
	using_gnome = 1;
#endif

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_set_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    اگر (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((نویسه *)e_cannot_open_display));
	بازگشت FAIL;
    }

    بازگشت OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


ایستا guint mouse_click_timer = 0;
ایستا صحیح mouse_timed_out = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ایستا timeout_cb_type
mouse_click_timer_cb(gpointer data)
{
    // we don't use this information currently
    صحیح *timed_out = (صحیح *) data;

    *timed_out = TRUE;
    بازگشت FALSE;		// don't happen again
}

ایستا guint		motion_repeat_timer  = 0;
ایستا صحیح		motion_repeat_offset = FALSE;
ایستا timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ایستا تهی
process_motion_notify(صحیح x, صحیح y, GdkModifierType state)
{
    صحیح	    button;
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
    اگر (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	بازگشت;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_allocation(gui.drawarea, &allocation);

    اگر (x < 0 || y < 0
	    || x >= allocation.width
	    || y >= allocation.height)
    {

	صحیح dx;
	صحیح dy;
	صحیح offshoot;
	صحیح delay = 10;

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
	اگر (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	وگرنه
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	اگر (!motion_repeat_timer)
	    motion_repeat_timer = timeout_add(delay, motion_repeat_timer_cb,
									 پوچ);
    }
}

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا GdkDevice *
gui_gtk_get_pointer_device(GtkWidget *widget)
{
    GdkWindow * ثابت win = gtk_widget_get_window(widget);
    GdkDisplay * ثابت dpy = gdk_window_get_display(win);
# اگر GTK_CHECK_VERSION(3,20,0)
    GdkSeat * ثابت seat = gdk_display_get_default_seat(dpy);
    بازگشت gdk_seat_get_pointer(seat);
# وگرنه
    GdkDeviceManager * ثابت mngr = gdk_display_get_device_manager(dpy);
    بازگشت gdk_device_manager_get_client_pointer(mngr);
# endif
}

    ایستا GdkWindow *
gui_gtk_get_pointer(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    GdkWindow * ثابت win = gtk_widget_get_window(widget);
    GdkDevice * ثابت dev = gui_gtk_get_pointer_device(widget);
    بازگشت gdk_window_get_device_position(win, dev , x, y, state);
}

# اگر defined(FEAT_GUI_TABLINE)
    ایستا GdkWindow *
gui_gtk_window_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ثابت dev = gui_gtk_get_pointer_device(widget);
    بازگشت gdk_device_get_window_at_position(dev, x, y);
}
# endif
#وگرنه // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_pointer(wid, x, y, s) \
    gdk_window_get_pointer((wid)->window, x, y, s)
# define gui_gtk_window_at_position(wid, x, y)	gdk_window_at_pointer(x, y)
#endif

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ایستا timeout_cb_type
motion_repeat_timer_cb(gpointer data UNUSED)
{
    صحیح		    x;
    صحیح		    y;
    GdkModifierType state;

    gui_gtk_get_pointer(gui.drawarea, &x, &y, &state);

    اگر (!(state & (GDK_BUTTON1_MASK | GDK_BUTTON2_MASK |
		   GDK_BUTTON3_MASK | GDK_BUTTON4_MASK |
		   GDK_BUTTON5_MASK)))
    {
	motion_repeat_timer = 0;
	بازگشت FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    اگر (vim_used_in_درون_داد_buf() > 10)
	بازگشت TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    اگر (motion_repeat_offset)
	x += gui.char_width;

    motion_repeat_offset = !motion_repeat_offset;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    بازگشت FALSE;
}

    ایستا gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer data UNUSED)
{
    اگر (event->is_hint)
    {
	صحیح		x;
	صحیح		y;
	GdkModifierType	state;

	gui_gtk_get_pointer(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    وگرنه
    {
	process_motion_notify((صحیح)event->x, (صحیح)event->y,
			      (GdkModifierType)event->state);
    }

    بازگشت TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    ایستا gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer data UNUSED)
{
    صحیح button;
    صحیح repeated_click = FALSE;
    صحیح x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

    // Make sure we have focus now we've been selected
    اگر (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    اگر (event->type != GDK_BUTTON_PRESS)
	بازگشت FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    اگر (!mouse_timed_out && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_out = FALSE;
    mouse_click_timer = timeout_add(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_out);

    انتخاب (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	حالت 1:
		button = MOUSE_چپ;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_MASK;
		بشکن;
	حالت 2: button = MOUSE_میانه; بشکن;
	حالت 3: button = MOUSE_راست; بشکن;
	حالت 8: button = MOUSE_X1; بشکن;
	حالت 9: button = MOUSE_X2; بشکن;
	پیشفرض:
	    بازگشت FALSE;		// Unknown button
    }

#ifdef FEAT_XIM
    // cancel any preediting
    اگر (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    بازگشت TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    ایستا gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer data UNUSED)
{
    صحیح	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#اگر GTK_CHECK_VERSION(3,4,0)
    ایستا دوگانه  acc_x, acc_y;
# اگر !GTK_CHECK_VERSION(3,22,0)
    ایستا guint32 last_smooth_event_time;
# endif
#endif

    اگر (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    انتخاب (event->direction)
    {
	حالت GDK_SCROLL_UP:
	    button = MOUSE_4;
	    بشکن;
	حالت GDK_SCROLL_DOWN:
	    button = MOUSE_5;
	    بشکن;
	حالت GDK_SCROLL_چپ:
	    button = MOUSE_7;
	    بشکن;
	حالت GDK_SCROLL_راست:
	    button = MOUSE_6;
	    بشکن;
#اگر GTK_CHECK_VERSION(3,4,0)
	حالت GDK_SCROLL_SMOOTH:
	    اگر (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		بازگشت FALSE;
	    }
# اگر GTK_CHECK_VERSION(3,22,0)
	    اگر (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# وگرنه
	    اگر (event->time - last_smooth_event_time > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# اگر !GTK_CHECK_VERSION(3,22,0)
	    last_smooth_event_time = event->time;
# endif
	    بشکن;
#endif
	پیشفرض: // This shouldn't happen
	    بازگشت FALSE;
    }

#ifdef FEAT_XIM
    // cancel any preediting
    اگر (im_is_preediting())
	xim_reset();
#endif

    vim_modifiers = modifiers_gdk2mouse(event->state);

#اگر GTK_CHECK_VERSION(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    اگر (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	وقتی (acc_x >= 1.0)
	{ //راست
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	وقتی (acc_x <= -1.0)
	{ //چپ
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	وقتی (acc_y >= 1.0)
	{ // down
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
	وقتی (acc_y <= -1.0)
	{ // up
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (صحیح)event->x, (صحیح)event->y,
		    FALSE, vim_modifiers);
	}
    }
    وگرنه
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    اگر (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    وگرنه
#endif
	gui_send_mouse_event(button, (صحیح)event->x, (صحیح)event->y,
		FALSE, vim_modifiers);

    بازگشت TRUE;
}


    ایستا gint
button_release_event(GtkWidget *widget UNUSED,
		     GdkEventButton *event,
		     gpointer data UNUSED)
{
    صحیح x, y;
    int_u vim_modifiers;

    gui.event_time = event->time;

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
	    بشکن;
    }

    بازگشت TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    ایستا تهی
grab_notify_event(GtkWidget *widget UNUSED,
		  gboolean  was_grabbed,
		  gpointer  data UNUSED)
{
    اگر (!was_grabbed)
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
    ایستا صحیح
شمار_and_decode_uri_list(char_u *out, char_u *raw, صحیح len)
{
    صحیح		i;
    char_u	*p = out;
    صحیح		شمار = 0;

    برای (i = 0; i < len; ++i)
    {
	اگر (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    اگر (p > out && p[-1] != NUL)
	    {
		++شمار;
		*p++ = NUL;
	    }
	}
	وگرنه اگر (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	وگرنه
	    *p++ = raw[i];
    }
    اگر (p > out && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++شمار;
    }
    بازگشت شمار;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    ایستا صحیح
filter_uri_list(char_u **outlist, صحیح max, char_u *src)
{
    صحیح	i, j;

    برای (i = j = 0; i < max; ++i)
    {
	outlist[i] = پوچ;
	اگر (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    اگر (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    وقتی (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    بازگشت j;
}

    ایستا char_u **
parse_uri_list(صحیح *شمار, char_u *data, صحیح len)
{
    صحیح	    n	    = 0;
    char_u  *tmp    = پوچ;
    char_u  **array = پوچ;

    اگر (data != پوچ && len > 0 && (tmp = alloc(len + 1)) != پوچ)
    {
	n = شمار_and_decode_uri_list(tmp, data, len);
	اگر (n > 0 && (array = ALLOC_MULT(char_u *, n)) != پوچ)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *شمار = n;
    بازگشت array;
}

    ایستا تهی
drag_handle_uri_list(GdkDragContext	*context,
		     GtkSelectionData	*data,
		     guint		time_,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    صحیح	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_data_get_data(data),
	    gtk_selection_data_get_length(data));

    اگر (fnames != پوچ && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_finish(context, TRUE, FALSE, time_); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    وگرنه
	vim_free(fnames);
}

    ایستا تهی
drag_handle_text(GdkDragContext	    *context,
		 GtkSelectionData   *data,
		 guint		    time_,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *text;
    صحیح	    len;
    char_u  *tmpbuf = پوچ;

    text = (char_u *)gtk_selection_data_get_data(data);
    len = gtk_selection_data_get_length(data);

    اگر (gtk_selection_data_get_data_type(data) == utf8_string_atom)
    {
	اگر (درون_داد_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&درون_داد_conv, text, &len);
	اگر (tmpbuf != پوچ)
	    text = tmpbuf;
    }

    dnd_yank_drag_data(text, (long)len);
    gtk_drag_finish(context, TRUE, FALSE, time_); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    اگر (dropkey[2] != 0)
	add_to_درون_داد_buf(dropkey, (صحیح)اندازه(dropkey));
    وگرنه
	add_to_درون_داد_buf(dropkey + 3, (صحیح)(اندازه(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ایستا تهی
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
    ثابت guchar * ثابت data_data = gtk_selection_data_get_data(data);
    ثابت gint data_length = gtk_selection_data_get_length(data);
    ثابت gint data_format = gtk_selection_data_get_format(data);

    اگر (data_data == پوچ
	    || data_length <= 0
	    || data_format != 8
	    || data_data[data_length] != '\0')
    {
	gtk_drag_finish(context, FALSE, FALSE, time_);
	بازگشت;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_pointer(widget, پوچ, پوچ, &state);

    // Not sure about the role of "text/plain" here...
    اگر (info == (guint)TARGET_TEXT_URI_LIST)
	drag_handle_uri_list(context, data, time_, state, x, y);
    وگرنه
	drag_handle_text(context, data, time_, state);

}
#endif // FEAT_DND


#اگر defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ایستا تهی
sm_client_check_changed_any(GnomeClient	    *client UNUSED,
			    gint	    کلید,
			    GnomeDialogType type UNUSED,
			    gpointer	    data UNUSED)
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
    shutdown_cancelled = check_changed_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    out_flush();
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
    ایستا gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase UNUSED,
			GnomeSaveStyle	    save_style UNUSED,
			gboolean	    shutdown UNUSED,
			GnomeInteractStyle  interact_style,
			gboolean	    fast UNUSED,
			gpointer	    data UNUSED)
{
    ایستا ثابت نویسه	suffix[] = "-session.vim";
    نویسه		*session_file;
    unsigned صحیح	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    اگر (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_NORMAL,
					 &sm_client_check_changed_any,
					 پوچ);
    out_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_path(
			gnome_client_get_config_prefix(client));
    len = strlen(session_file);

    اگر (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(نویسه, session_file, len + اندازه(suffix));
    memcpy(session_file + len, suffix, اندازه(suffix));

    success = write_session_file((char_u *)session_file);

    اگر (success)
    {
	ثابت نویسه  *argv[8];
	صحیح	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = پوچ;

	gnome_client_set_discard_command(client, i, (نویسه **)argv);

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
	argv[i] = پوچ;

	gnome_client_set_restart_command(client, i, (نویسه **)argv);
	gnome_client_set_clone_command(client, 0, پوچ);
    }

    g_free(session_file);

    بازگشت success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    ایستا تهی
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
    ایستا تهی
setup_save_yourself(تهی)
{
    GnomeClient *client;

    client = gnome_master_client();
    اگر (client == پوچ)
	بازگشت;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), پوچ);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), پوچ);
}

#وگرنه // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    ایستا gboolean
local_xsmp_handle_requests(
    GIOChannel		*source UNUSED,
    GIOCondition	condition,
    gpointer		data)
{
    اگر (condition == G_IO_IN)
    {
	// Do stuff; maybe close connection
	اگر (xsmp_handle_requests() == FAIL)
	    g_io_channel_unref((GIOChannel *)data);
	بازگشت TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)data);
    xsmp_close();
    بازگشت TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    ایستا تهی
setup_save_yourself(تهی)
{
    Atom    *existing_atoms = پوچ;
    صحیح	    شمار = 0;

# ifdef USE_XSMP
    اگر (xsmp_icefd != -1)
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
    وگرنه
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	اگر (!dpy)
	    بازگشت;

	GdkWindow * ثابت mainwin_win = gtk_widget_get_window(gui.mainwin);
	اگر (XGetWMProtocols(dpy, GDK_WINDOW_XID(mainwin_win),
		    &existing_atoms, &شمار))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    صحیح	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    برای (i = 0; i < شمار; ++i)
		اگر (existing_atoms[i] == save_yourself_xatom)
		    بشکن;

	    اگر (i == شمار)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, شمار + 1);
		اگر (new_atoms != پوچ)
		{
		    memcpy(new_atoms, existing_atoms, شمار * اندازه(Atom));
		    new_atoms[شمار] = save_yourself_xatom;
		    XSetWMProtocols(GDK_WINDOW_XDISPLAY(mainwin_win),
			    GDK_WINDOW_XID(mainwin_win),
			    new_atoms, شمار + 1);
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
    ایستا GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event UNUSED,
		    gpointer data UNUSED)
{
    XEvent *xevent = (XEvent *)xev;

    اگر (xevent != پوچ
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
		    پوچ, 0);
	بازگشت GDK_FILTER_REMOVE;
    }

    بازگشت GDK_FILTER_CONTINUE;
}
#endif // !USE_GNOME_SESSION

    ایستا GdkPixbuf *
pixbuf_new_from_png_data(ثابت unsigned نویسه *data, unsigned صحیح len)
{
    GInputStream *stream;
    GdkPixbuf    *pixbuf;

    stream = g_memory_درون_داد_stream_new_from_data(data, len, پوچ);
    pixbuf = gdk_pixbuf_new_from_stream(stream, پوچ, پوچ);
    g_object_unref(stream);
    بازگشت pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ایستا تهی
mainwin_realize(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
#شامل "../runtime/vim16x16_png.h"
#شامل "../runtime/vim32x32_png.h"
#شامل "../runtime/vim48x48_png.h"

    GdkWindow * ثابت mainwin_win = gtk_widget_get_window(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    اگر (echo_wid_arg)
    {
	اگر (gui_mch_get_display())
	    printf("WID: %ld\n", (long)GDK_WINDOW_XID(mainwin_win));
	وگرنه
	    printf("WID: 0\n");
	fflush(stdout);
    }

    اگر (vim_strchr(p_go, GO_ICON) != پوچ)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	اگر (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_window_set_icon_name(GTK_WINDOW(gui.mainwin), "gvim");
	وگرنه
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = پوچ;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_data(vim48x48_png, vim48x48_png_len));

	    gtk_window_set_icon_list(GTK_WINDOW(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(تهی *)&g_object_unref, پوچ);
	    g_list_free(icons);
	}
    }

#اگر !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_window_add_filter(پوچ, &global_event_filter, پوچ);
#endif
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#اگر defined(USE_GNOME_SESSION)
    اگر (using_gnome)
#endif
	setup_save_yourself();

#ifdef FEAT_CLIENTSERVER
    اگر (clientserver_method == CLIENTSERVER_METHOD_X11 && gui_mch_get_display())
    {
	اگر (serverName == پوچ && serverDelayedStartName != پوچ)
	{
	    // This is a :gui command in a plain vim with no previous server
	    commWindow = GDK_WINDOW_XID(mainwin_win);

	    (تهی)serverRegisterName(GDK_WINDOW_XDISPLAY(mainwin_win),
				    serverDelayedStartName);
	}
	وگرنه
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
			G_CALLBACK(property_event), پوچ);
    }
#endif
}

    ایستا GdkCursor *
create_blank_pointer(تهی)
{
    GdkCursor	*cursor;

#اگر GTK_CHECK_VERSION(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#وگرنه
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    GdkWindow	*ریشه_window = پوچ;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_mask;
    نویسه	blank_data[] = { 0x0 };
    ریشه_window = gtk_widget_get_ریشه_window(gui.mainwin);
    blank_mask = gdk_bitmap_create_from_data(ریشه_window, blank_data, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_mask, blank_mask,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_mask);
#endif

    بازگشت cursor;
}

    ایستا تهی
mainwin_screen_changed_cb(GtkWidget  *widget,
			  GdkScreen  *previous_screen UNUSED,
			  gpointer   data UNUSED)
{
    اگر (!gtk_widget_has_screen(widget))
	بازگشت;

    /*
     * Recreate the invisible mouse cursor.
     */
    اگر (gui.blank_pointer != پوچ)
#اگر GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_pointer));
#وگرنه
	gdk_cursor_unref(gui.blank_pointer);
#endif

    gui.blank_pointer = create_blank_pointer();

    اگر (gui.pointer_hidden && gtk_widget_get_window(gui.drawarea) != پوچ)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    اگر (gui.text_context != پوچ)
	g_object_unref(gui.text_context);

    gui.text_context = gtk_widget_create_pango_context(widget);
    pango_context_set_base_dir(gui.text_context, PANGO_DIRECTION_LTR);

    اگر (gui.norm_font != پوچ)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_set_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ایستا gboolean
mainwin_state_event_cb(GtkWidget *widget UNUSED,
		       GdkEventWindowState *event,
		       gpointer user_data UNUSED)
{
    اگر (event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    بازگشت FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    ایستا تهی
drawarea_realize_cb(GtkWidget *widget, gpointer data UNUSED)
{
    GtkWidget *sbar;
    GtkAllocation allocation;

#ifdef FEAT_XIM
    xim_init();
#endif
    gui_mch_new_colors();
#اگر GTK_CHECK_VERSION(3,0,0)
    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_width(widget),
	    gtk_widget_get_allocated_height(widget));
#وگرنه
    gui.text_gc = gdk_gc_new(gui.drawarea->window);
#endif

    gui.blank_pointer = create_blank_pointer();
    اگر (gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(widget), gui.blank_pointer);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_چپ].id;
    اگر (!sbar || (!gui.which_scrollbars[SBAR_چپ]
				    && firstwin->w_scrollbars[SBAR_راست].id))
	sbar = firstwin->w_scrollbars[SBAR_راست].id;
    gtk_widget_get_allocation(sbar, &allocation);
    اگر (sbar && gtk_widget_get_realized(sbar) && allocation.width)
	gui.scrollbar_width = allocation.width;

    sbar = gui.bottom_sbar.id;
    اگر (sbar && gtk_widget_get_realized(sbar) && allocation.height)
	gui.scrollbar_height = allocation.height;
}

/*
 * Properly clean up on shutdown.
 */
    ایستا تهی
drawarea_unrealize_cb(GtkWidget *widget UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

#ifdef FEAT_XIM
    im_shutdown();
#endif
    اگر (gui.ascii_glyphs != پوچ)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = پوچ;
    }
    اگر (gui.ascii_font != پوچ)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = پوچ;
    }
    g_object_unref(gui.text_context);
    gui.text_context = پوچ;

#اگر GTK_CHECK_VERSION(3,0,0)
    اگر (gui.surface != پوچ)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = پوچ;
    }
#وگرنه
    g_object_unref(gui.text_gc);
    gui.text_gc = پوچ;
#endif

#اگر GTK_CHECK_VERSION(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_pointer));
#وگرنه
    gdk_cursor_unref(gui.blank_pointer);
#endif
    gui.blank_pointer = پوچ;
}

#اگر GTK_CHECK_VERSION(3,22,2)
    ایستا تهی
drawarea_style_updated_cb(GtkWidget *widget UNUSED,
			 gpointer data UNUSED)
#وگرنه
    ایستا تهی
drawarea_style_set_cb(GtkWidget	*widget UNUSED,
		      GtkStyle	*previous_style UNUSED,
		      gpointer	data UNUSED)
#endif
{
    gui_mch_new_colors();
}

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       data UNUSED)
{
    ایستا صحیح cur_width = 0;
    ایستا صحیح cur_height = 0;

    g_return_val_if_fail(event
	    && event->width >= 1 && event->height >= 1, TRUE);

# اگر GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
	بازگشت TRUE;
# endif

    اگر (event->width == cur_width && event->height == cur_height)
	بازگشت TRUE;

    cur_width = event->width;
    cur_height = event->height;

    اگر (gui.surface != پوچ)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_window_create_similar_surface(
	    gtk_widget_get_window(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->width, event->height);

    gtk_widget_queue_draw(widget);

    بازگشت TRUE;
}
#endif

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ایستا gint
delete_event_cb(GtkWidget *widget UNUSED,
		GdkEventAny *event UNUSED,
		gpointer data UNUSED)
{
    gui_shell_closed();
    بازگشت TRUE;
}

#اگر defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ایستا صحیح
get_item_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation item_orientation = GTK_ORIENTATION_HORIZONTAL;

    اگر (using_gnome && widget != پوچ)
    {
	GtkWidget *parent;
	BonoboDockItem *dockitem;

	parent	 = gtk_widget_get_parent(widget);
	اگر (G_TYPE_FROM_INSTANCE(parent) == BONOBO_TYPE_DOCK_ITEM)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = parent;
	    dockitem = BONOBO_DOCK_ITEM(widget);

	    اگر (dockitem == پوچ || dockitem->is_floating)
		بازگشت 0;
	    item_orientation = bonobo_dock_item_get_orientation(dockitem);
	}
    }
# وگرنه
#  define item_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    اگر (widget != پوچ
	    && item_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# اگر GTK_CHECK_VERSION(3,0,0) || !defined(FEAT_GUI_GNOME)
	GtkAllocation allocation;

	gtk_widget_get_allocation(widget, &allocation);
	اگر (allocation.height > 1)
	    بازگشت allocation.height;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  اگر GTK_CHECK_VERSION(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_height(widget, &min_h, &natural_h);
	    بازگشت MAX(min_h, natural_h);
	}
#  وگرنه
	بازگشت allocation.height;
#  endif
# وگرنه
	اگر (orientation == GTK_ORIENTATION_HORIZONTAL)
	    بازگشت widget->allocation.height;
	وگرنه
	    بازگشت widget->allocation.width;
# endif
    }
    بازگشت 0;
}
#endif

    ایستا صحیح
get_menu_tool_width(تهی)
{
    صحیح width = 0;

#ifdef FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    width += get_item_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    width += get_item_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    اگر (gui.tabline != پوچ)
	width += get_item_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#endif

    بازگشت width;
}

    ایستا صحیح
get_menu_tool_height(تهی)
{
    صحیح height = 0;

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
    اگر (gui.tabline != پوچ
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	height += get_item_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#endif

    بازگشت height;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ایستا صحیح init_window_hints_state = 0;

    ایستا تهی
update_window_manager_hints(صحیح force_width, صحیح force_height)
{
    ایستا صحیح old_width  = 0;
    ایستا صحیح old_height = 0;
    ایستا صحیح old_min_width  = 0;
    ایستا صحیح old_min_height = 0;
    ایستا صحیح old_char_width  = 0;
    ایستا صحیح old_char_height = 0;

    صحیح width;
    صحیح height;
    صحیح min_width;
    صحیح min_height;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    اگر (!(force_width && force_height)  &&  init_window_hints_state > 0)
    {
	// Don't do it!
	init_window_hints_state = 2;
	بازگشت;
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
    اگر (force_width && force_height)
    {
	min_width  = force_width;
	min_height = force_height;
    }
    وگرنه
    {
	min_width  = width  + MIN_COLUMNS * gui.char_width;
	min_height = height + MIN_سطرها   * gui.char_height;
    }

    // Avoid an expose event when the size didn't change.
    اگر (width != old_width
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
	gtk_window_set_geometry_hints(GTK_WINDOW(gui.mainwin), پوچ,
				      &geometry, geometry_mask);
	old_width       = width;
	old_height      = height;
	old_min_width   = min_width;
	old_min_height  = min_height;
	old_char_width  = gui.char_width;
	old_char_height = gui.char_height;
    }
}

#اگر defined(FEAT_GUI_DARKTHEME)
    تهی
gui_mch_set_dark_theme(صحیح dark)
{
# اگر GTK_CHECK_VERSION(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_set(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, پوچ);
# endif
}
#endif // FEAT_GUI_DARKTHEME

#ifdef FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ایستا تهی
icon_size_changed_foreach(GtkWidget *widget, gpointer user_data)
{
    اگر (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# اگر GTK_CHECK_VERSION(3,10,0)
	اگر (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ثابت GtkIconSize icon_size = GPOINTER_TO_INT(user_data);
	    ثابت gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, پوچ);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_size);
	}
# وگرنه
	// User-defined icons are stored in a GtkIconSet
	اگر (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_SET)
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
    وگرنه اگر (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_size_changed_foreach,
			      user_data);
    }
}

    ایستا تهی
set_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    GtkIconSize	    size;
    GtkIconSize	    oldsize;

    اگر ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    وگرنه اگر ((toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS))
		      == (TOOLBAR_TEXT | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    وگرنه اگر (toolbar_flags & TOOLBAR_TEXT)
	style = GTK_TOOLBAR_TEXT;
    وگرنه
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_set_style(toolbar, style);
# اگر !GTK_CHECK_VERSION(3,0,0)
    gtk_toolbar_set_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    انتخاب (tbis_flags)
    {
	حالت TBIS_TINY:	    size = GTK_ICON_SIZE_MENU;		بشکن;
	حالت TBIS_SMALL:    size = GTK_ICON_SIZE_SMALL_TOOLBAR;	بشکن;
	حالت TBIS_MEDIUM:   size = GTK_ICON_SIZE_BUTTON;	بشکن;
	حالت TBIS_LARGE:    size = GTK_ICON_SIZE_LARGE_TOOLBAR;	بشکن;
	حالت TBIS_HUGE:     size = GTK_ICON_SIZE_DND;		بشکن;
	حالت TBIS_GIANT:    size = GTK_ICON_SIZE_DIALOG;	بشکن;
	پیشفرض:	    size = GTK_ICON_SIZE_INVALID;	بشکن;
    }
    oldsize = gtk_toolbar_get_icon_size(toolbar);

    اگر (size == GTK_ICON_SIZE_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_size(toolbar);
	size = gtk_toolbar_get_icon_size(toolbar);
    }
    اگر (size != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_size_changed_foreach,
			      GINT_TO_POINTER((صحیح)size));
    }
    gtk_toolbar_set_icon_size(toolbar, size);
}

#endif // FEAT_TOOLBAR

#اگر defined(FEAT_GUI_TABLINE)
ایستا صحیح ignore_tabline_evt = FALSE;
ایستا GtkWidget *tabline_menu;
# اگر !GTK_CHECK_VERSION(3,0,0)
ایستا GtkTooltips *tabline_tooltip;
# endif
ایستا صحیح clicked_page;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ایستا تهی
tabline_menu_handler(GtkMenuItem *item UNUSED, gpointer user_data)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_page, (صحیح)(long)user_data);
}

    ایستا تهی
add_tabline_menu_item(GtkWidget *menu, char_u *text, صحیح resp)
{
    GtkWidget	*item;
    char_u	*utf_text;

    utf_text = CONVERT_TO_UTF8(text);
    item = gtk_menu_item_new_with_label((ثابت نویسه *)utf_text);
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
    ایستا GtkWidget *
create_tabline_menu(تهی)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    add_tabline_menu_item(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    add_tabline_menu_item(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    add_tabline_menu_item(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    بازگشت menu;
}

    ایستا gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    اگر (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	صحیح		x = bevent->x;
	صحیح		y = bevent->y;
	GtkWidget	*tabwidget;
	GdkWindow	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	اگر (hold_gui_events || cmdwin_type != 0)
	    بازگشت TRUE;

	tabwin = gui_gtk_window_at_position(gui.mainwin, &x, &y);

	gdk_window_get_user_data(tabwin, (gpointer)&tabwidget);
	clicked_page = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	اگر (bevent->button == 3)
	{
# اگر GTK_CHECK_VERSION(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    اگر (gui.is_wayland)
	    {
		صحیح x2, y2;
		gui_gtk_get_pointer(gui.mainwin, &x2, &y2, پوچ);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_window(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, پوچ);
	    }
	    وگرنه
#  endif
	    gtk_menu_popup_at_pointer(GTK_MENU(widget), event);
# وگرنه
	    gtk_menu_popup(GTK_MENU(widget), پوچ, پوچ, پوچ, پوچ,
						bevent->button, bevent->time);
# endif
	    // We handled the event.
	    بازگشت TRUE;
	}
	وگرنه اگر (bevent->button == 1)
	{
	    اگر (clicked_page == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	وگرنه اگر (bevent->button == 2)
	{
	    اگر (clicked_page != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_page, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    بازگشت FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    ایستا تهی
on_select_tab(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    اگر (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# اگر GTK_CHECK_VERSION(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ایستا تهی
on_tab_reordered(
	GtkNotebook	*notebook UNUSED,
	gpointer	*page UNUSED,
	gint		idx,
	gpointer	data UNUSED)
{
    اگر (ignore_tabline_evt)
	بازگشت;

    اگر ((tabpage_index(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    وگرنه
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    تهی
gui_mch_show_tabline(صحیح showit)
{
    اگر (gui.tabline == پوچ)
	بازگشت;

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

	    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_window_manager_hints(0, 0);
	اگر (showit)
	    gtk_widget_set_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    صحیح
gui_mch_showing_tabline(تهی)
{
    بازگشت gui.tabline != پوچ
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    تهی
gui_mch_update_tabline(تهی)
{
    GtkWidget	    *page;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    صحیح		    nr = 0;
    صحیح		    tab_رقم;
    صحیح		    curtabidx = 0;
    char_u	    *labeltext;

    اگر (gui.tabline == پوچ)
	بازگشت;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    برای (tp = first_tabpage; tp != پوچ; tp = tp->tp_next, ++nr)
    {
	اگر (tp == curtab)
	    curtabidx = nr;

	tab_رقم = nr + 1;

	page = gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr);
	اگر (page == پوچ)
	{
	    // Add notebook page
# اگر GTK_CHECK_VERSION(3,2,0)
	    page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# وگرنه
	    page = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(page);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# اگر !GTK_CHECK_VERSION(3,14,0)
	    gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_add(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_درج_page(GTK_NOTEBOOK(gui.tabline),
		    page,
		    event_box,
		    nr);
# اگر GTK_CHECK_VERSION(2,10,0)
	    gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    page,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), page);
	g_object_set_data(G_OBJECT(event_box), "tab_num",
						     GINT_TO_POINTER(tab_رقم));
	label = gtk_bin_get_child(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
	gtk_label_set_text(GTK_LABEL(label), (ثابت نویسه *)labeltext);
	CONVERT_TO_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_TO_UTF8(NameBuff);
# اگر GTK_CHECK_VERSION(3,0,0)
	gtk_widget_set_tooltip_text(event_box, (ثابت gchar *)labeltext);
# وگرنه
	gtk_tooltips_set_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ثابت نویسه *)labeltext, پوچ);
# endif
	CONVERT_TO_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    وقتی (gtk_notebook_get_nth_page(GTK_NOTEBOOK(gui.tabline), nr) != پوچ)
	gtk_notebook_remove_page(GTK_NOTEBOOK(gui.tabline), nr);

    اگر (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    تهی
gui_mch_set_curtab(صحیح nr)
{
    اگر (gui.tabline == پوچ)
	بازگشت;

    ignore_tabline_evt = TRUE;
    اگر (gtk_notebook_get_current_page(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_set_current_page(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#endif // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    تهی
gui_gtk_set_selection_targets(GdkAtom selection)
{
    صحیح		    i, j = 0;
    ایستا صحیح	    n_targets = N_SELECTION_TARGETS;
    ایستا GtkTargetEntry  targets[N_SELECTION_TARGETS];

    اگر (targets[0].target == پوچ)
    {
	برای (i = 0; i < (صحیح)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    اگر (!clip_html && selection_targets[i].info == TARGET_HTML)
		n_targets--;
	    وگرنه
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
    تهی
gui_gtk_set_dnd_targets(تهی)
{
    صحیح		    i, j = 0;
    صحیح		    n_targets = N_DND_TARGETS;
    GtkTargetEntry  targets[N_DND_TARGETS];

    برای (i = 0; i < (صحیح)N_DND_TARGETS; ++i)
    {
	اگر (!clip_html && dnd_targets[i].info == TARGET_HTML)
	    n_targets--;
	وگرنه
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
    صحیح
gui_mch_init(تهی)
{
    GtkWidget *vbox;

#ifdef FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    اگر (using_gnome)
    {
	gnome_program_init(VIMPACKAGE, VIM_VERSION_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, پوچ);
# اگر defined(LC_NUMERIC)
	{
	    نویسه *p = setlocale(LC_NUMERIC, پوچ);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    اگر (p == پوچ || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#endif
    VIM_CLEAR(gui_argv);

#اگر GLIB_CHECK_VERSION(2,1,3)
    // Set the human-readable application name
    g_set_application_name("Vim");
#endif
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    set_option_مقدار_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#ifdef FEAT_TOOLBAR
    gui_gtk_register_stock_icons();
#endif
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#اگر !GTK_CHECK_VERSION(3,0,0)
# اگر 0
    gtk_rc_parse("gtkrc");
# endif
#endif

    // Initialize values
    gui.border_width = 2;
    gui.scrollbar_width = SB_DEFAULT_WIDTH;
    gui.scrollbar_height = SB_DEFAULT_WIDTH;
#اگر GTK_CHECK_VERSION(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#وگرنه
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

    اگر (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	اگر (plug != پوچ && gtk_plug_get_socket_window(GTK_PLUG(plug)) != پوچ)
	{
	    gui.mainwin = plug;
	}
	وگرنه
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (unsigned صحیح)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    اگر (gtk_socket_id == 0)
    {
#ifdef FEAT_GUI_GNOME
	اگر (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", پوچ);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	وگرنه
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
		     G_CALLBACK(&delete_event_cb), پوچ);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), پوچ);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_changed_cb), پوچ);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), پوچ);

    gui.accel_group = gtk_accel_group_new();
    gtk_window_add_accel_group(GTK_WINDOW(gui.mainwin), gui.accel_group);

    // A vertical box holds the menubar, toolbar and main text window.
#اگر GTK_CHECK_VERSION(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_set_homogeneous(GTK_BOX(vbox), FALSE);
#وگرنه
    vbox = gtk_vbox_new(FALSE, 0);
#endif

#ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
    {
# اگر defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_set_contents(GNOME_APP(gui.mainwin), vbox);
    }
    وگرنه
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
	g_object_set(gtk_settings, "gtk-menu-bar-accel", پوچ, پوچ);
    }


# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
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
    وگرنه
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
# وگرنه
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
    اگر (using_gnome)
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
    وگرنه
# endif	// FEAT_GUI_GNOME
    {
	اگر (vim_strchr(p_go, GO_TOOLBAR) != پوچ
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
# اگر !GTK_CHECK_VERSION(3,0,0)
    gtk_notebook_set_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# اگر !GTK_CHECK_VERSION(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *page, *label, *event_box;

	// Add the first tab.
# اگر GTK_CHECK_VERSION(3,2,0)
	page = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_set_homogeneous(GTK_BOX(page), FALSE);
# وگرنه
	page = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(page);
	gtk_container_add(GTK_CONTAINER(gui.tabline), page);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_set_data(G_OBJECT(event_box), "tab_num", GINT_TO_POINTER(1L));
# اگر !GTK_CHECK_VERSION(3,14,0)
	gtk_misc_set_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_add(GTK_CONTAINER(event_box), label);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(gui.tabline), page, event_box);
# اگر GTK_CHECK_VERSION(2,10,0)
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(gui.tabline), page, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), پوچ);
# اگر GTK_CHECK_VERSION(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), پوچ);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#endif // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_set_border_width(GTK_CONTAINER(gui.formwin), 0);
#اگر !GTK_CHECK_VERSION(3,0,0)
    gtk_widget_set_events(gui.formwin, GDK_EXPOSURE_MASK);
#endif
#اگر GTK_CHECK_VERSION(3,22,2)
    gtk_widget_set_name(gui.formwin, "vim-gtk-form");
#endif

    gui.drawarea = gtk_drawing_area_new();
#اگر GTK_CHECK_VERSION(3,0,0)
    gui.surface = پوچ;
#endif

#ifdef GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    اگر (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#endif

    // Determine which events we will filter.
    gint event_mask =
	GDK_EXPOSURE_MASK |
	GDK_ENTER_NOTIFY_MASK |
	GDK_LEAVE_NOTIFY_MASK |
	GDK_BUTTON_PRESS_MASK |
	GDK_BUTTON_RELEASE_MASK |
	GDK_کلید_PRESS_MASK |
	GDK_کلید_RELEASE_MASK |
	GDK_POINTER_MOTION_MASK |
	GDK_POINTER_MOTION_HINT_MASK;
#اگر GTK_CHECK_VERSION(3,4,0)
    اگر (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
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
    وگرنه
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_mask |= GDK_SMOOTH_SCROLL_MASK;
    }
#وگرنه
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
		       G_CALLBACK(کلید_press_event), پوچ);
#اگر defined(FEAT_XIM) || GTK_CHECK_VERSION(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&کلید_release_event), پوچ);
#endif
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), پوچ);
#اگر GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), پوچ);
#endif
#اگر GTK_CHECK_VERSION(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), پوچ);
#وگرنه
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_set_cb), پوچ);
#endif

#اگر !GTK_CHECK_VERSION(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#endif

#اگر !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#endif

    اگر (gtk_socket_id != 0)
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

#اگر GTK_CHECK_VERSION(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), پوچ);
#وگرنه
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), پوچ);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), پوچ);
#endif

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    اگر (vim_strchr(p_go, GO_POINTER) != پوچ)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), پوچ);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), پوچ);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    اگر (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_out_event), پوچ);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_in_event), پوچ);
    }
    وگرنه
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_out_event), پوچ);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_in_event), پوچ);
#ifdef FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_out_event), پوچ);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_in_event), پوچ);
#endif // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), پوچ);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.in_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_changed_cb), پوچ);
    }

#اگر defined(FEAT_IMAGE) && GTK_CHECK_VERSION(3,10,0)
    gui.scale = gtk_widget_get_scale_factor(gui.formwin);
#endif

    بازگشت OK;
}

#اگر defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    تهی
gui_mch_forked(تهی)
{
    اگر (!using_gnome)
	بازگشت;

    GnomeClient *client;

    client = gnome_master_client();

    اگر (client != پوچ)
	gnome_client_set_process_id(client, getpid());
}
#endif // USE_GNOME_SESSION

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا GdkRGBA
color_to_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    بازگشت rgba;
}

    ایستا تهی
set_cairo_source_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ثابت GdkRGBA rgba = color_to_rgba(color);
    cairo_set_source_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#endif // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    تهی
gui_mch_new_colors(تهی)
{
    اگر (gui.drawarea != پوچ
#اگر GTK_CHECK_VERSION(3,22,2)
	    && gui.formwin != پوچ
#endif
	    && gtk_widget_get_window(gui.drawarea) != پوچ)
    {
#اگر !GTK_CHECK_VERSION(3,22,2)
	GdkWindow * ثابت da_win = gtk_widget_get_window(gui.drawarea);
#endif
#اگر GTK_CHECK_VERSION(3,22,2)
	GtkStyleContext * ثابت context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ثابت provider = gtk_css_provider_new();
	gchar * ثابت css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.back_pixel >> 16) & 0xff,
		 (gui.back_pixel >> 8) & 0xff,
		 gui.back_pixel & 0xff);

	gtk_css_provider_load_from_data(provider, css, -1, پوچ);
	gtk_style_context_add_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#وگرنهاگر GTK_CHECK_VERSION(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_to_rgba(gui.back_pixel);
	{
	    cairo_pattern_t * ثابت pat = cairo_pattern_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    اگر (pat != پوچ)
	    {
		gdk_window_set_background_pattern(da_win, pat);
		cairo_pattern_destroy(pat);
	    }
	    وگرنه
		gdk_window_set_background_rgba(da_win, &rgba);
	}
#وگرنه // !GTK_CHECK_VERSION(3,4,0)
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
    ایستا gboolean
startup_resize_correction_cb(gpointer data UNUSED)
{
    gui_set_shellsize(FALSE, FALSE, RESIZE_BOTH);
    بازگشت FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ایستا gint
form_configure_event(GtkWidget *widget UNUSED,
		     GdkEventConfigure *event,
		     gpointer data UNUSED)
{
    صحیح	    usable_height = event->height;
#ifdef TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    صحیح	    w, h;
    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(پوچ, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (صحیح)Columns, (صحیح)Rows);
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

    اگر (!MATCH_WIDTH_HEIGHT(latest_resize_hist, w, h)
					     && match_stale_width_height(w, h))
	// discard stale event
	بازگشت TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    اگر ((mch_csd_height == 0 || mch_csd_width == 0)
	    && latest_resize_hist != پوچ
	    && !latest_resize_hist->used
	    && latest_resize_hist->from_shellsize
	    && gui.char_height > 0)
    {
	صحیح pot_csd_w = latest_resize_hist->width - w;
	صحیح pot_csd_h = latest_resize_hist->height - h;

	اگر (pot_csd_w > 0 && pot_csd_w < gui.char_width)
	{
	    mch_csd_width = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (صحیح)Columns * gui.char_width
						    + gui_get_base_width();
	}
	اگر (pot_csd_h > 0 && pot_csd_h < gui.char_height)
	{
	    mch_csd_height = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_height += mch_csd_height;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	اگر ((mch_csd_height > 0 || mch_csd_width > 0) && gtk_socket_id == 0)
	    g_idle_add_full(GTK_PRIORITY_RESIZE, startup_resize_correction_cb, پوچ, پوچ);
    }
    clear_resize_hists();
#endif

#اگر GTK_CHECK_VERSION(3,22,2) && !GTK_CHECK_VERSION(3,22,4)
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
    اگر (event->window != gtk_widget_get_window(gui.formwin))
	بازگشت TRUE;
#endif

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    اگر (gtk_socket_id != 0)
	usable_height -= (gui.char_height - (gui.char_height/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	صحیح use_width = event->width;

#ifdef TRACK_RESIZE_HISTORY
	اگر (mch_pending_form_w > event->width
		&& mch_pending_form_w - event->width < gui.char_width)
	    use_width = mch_pending_form_w;
#endif
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_width, usable_height);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    بازگشت TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    ایستا تهی
mainwin_destroy_cb(GObject *object UNUSED, gpointer data UNUSED)
{
    // Don't write messages to the GUI anymore
    full_screen = FALSE;

    gui.mainwin  = پوچ;
    gui.drawarea = پوچ;

    اگر (!exiting) // only do anything if the destroy was unexpected
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

    تهی
gui_gtk_get_screen_geom_of_win(
	GtkWidget *wid,
	صحیح point_x,	    // x position of window if not initialized
	صحیح point_y,	    // y position of window if not initialized
	صحیح *screen_x,
	صحیح *screen_y,
	صحیح *width,
	صحیح *height)
{
    GdkRectangle geometry;
    GdkWindow *win = gtk_widget_get_window(wid);
#اگر GTK_CHECK_VERSION(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    اگر (wid != پوچ && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    وگرنه
	dpy = gdk_display_get_default();
    اگر (win != پوچ)
	monitor = gdk_display_get_monitor_at_window(dpy, win);
    وگرنه
	monitor = gdk_display_get_monitor_at_point(dpy, point_x, point_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#وگرنه
    GdkScreen* screen;
    صحیح monitor;

    اگر (wid != پوچ && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    وگرنه
	screen = gdk_screen_get_default();
    اگر (win != پوچ)
	monitor = gdk_screen_get_monitor_at_window(screen, win);
    وگرنه
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
    ایستا تهی
gui_gtk_get_screen_dimensions(
	صحیح point_x,
	صحیح point_y,
	صحیح *screen_w,
	صحیح *screen_h)
{
    صحیح	    x, y;

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

    تهی
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
    ایستا timeout_cb_type
check_startup_plug_hints(gpointer data UNUSED)
{
    اگر (init_window_hints_state == 1)
    {
	// Safe to use normal hints now
	init_window_hints_state = 0;
	update_window_manager_hints(0, 0);
	بازگشت FALSE;   // stop timer
    }

    // Keep on trying
    init_window_hints_state = 1;
    بازگشت TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    صحیح
gui_mch_open(تهی)
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
    اگر (role_argument != پوچ)
    {
	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role_argument);
    }
    وگرنه
    {
	نویسه *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (unsigned)mch_get_pid(),
			       (unsigned)g_random_int(),
			       (unsigned)time(پوچ));

	gtk_window_set_role(GTK_WINDOW(gui.mainwin), role);
	g_free(role);
    }

    اگر (gui_win_x != -1 && gui_win_y != -1)
	gtk_window_move(GTK_WINDOW(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    اگر (gui.geom != پوچ)
    {
	صحیح		mask;
	unsigned صحیح	w, h;
	صحیح		x = 0;
	صحیح		y = 0;

	mask = XParseGeometry((نویسه *)gui.geom, &x, &y, &w, &h);

	اگر (mask & Width_مقدار)
	    Columns = w;
	اگر (mask & Height_مقدار)
	{
	    اگر (p_window > (long)h - 1 || !option_was_set((char_u *)"window"))
		p_window = h - 1;
	    Rows = h;
	}
	limit_screen_size();

	pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
	pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

	pixel_width  += get_menu_tool_width();
	pixel_height += get_menu_tool_height();

	اگر (mask & (XValue | YValue))
	{
	    صحیح ww, hh;

#ifdef FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#وگرنه
	    gui_mch_get_screen_dimensions(&ww, &hh);
#endif
	    hh += p_ghr + get_menu_tool_height();
	    ww += get_menu_tool_width();
	    اگر (mask & XNegative)
		x += ww - pixel_width;
	    اگر (mask & YNegative)
		y += hh - pixel_height;
	    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	اگر (gtk_socket_id != 0  &&  (mask & Width_مقدار || mask & Height_مقدار))
	{
	    update_window_manager_hints(pixel_width, pixel_height);
	    init_window_hints_state = 1;
	    timeout_add(1000, check_startup_plug_hints, پوچ);
	}
    }

    pixel_width = (guint)(gui_get_base_width() + Columns * gui.char_width);
    pixel_height = (guint)(gui_get_base_height() + Rows * gui.char_height);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    اگر (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), pixel_width, pixel_height);
    update_window_manager_hints(0, 0);

    اگر (foreground_argument != پوچ)
	fg_pixel = gui_get_color((char_u *)foreground_argument);
    اگر (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    اگر (background_argument != پوچ)
	bg_pixel = gui_get_color((char_u *)background_argument);
    اگر (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    اگر (found_reverse_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_back_pixel = fg_pixel;
    }
    وگرنه
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
		     G_CALLBACK(mainwin_destroy_cb), پوچ);

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
				       G_CALLBACK(form_configure_event), پوچ);
#اگر GTK_CHECK_VERSION(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_factor_event), پوچ);
#endif

#ifdef FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_set_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_data_received_cb), پوچ);
#endif

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	اگر (found_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
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
	اگر (using_gnome && vim_strchr(p_go, GO_MENUS) == پوچ)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  پوچ);
# ifdef FEAT_TOOLBAR
	اگر (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == پوچ
		&& (toolbar_flags & (TOOLBAR_TEXT | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  پوچ);
# endif
#endif
	gtk_widget_show(gui.mainwin);

#اگر defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	اگر (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	اگر (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#endif
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), پوچ);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), پوچ);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), پوچ);

    بازگشت OK;
}

/*
 * Clean up for when exiting Vim.
 */
    تهی
gui_mch_exit(صحیح rc UNUSED)
{
    // Clean up, unless we don't want to invoke free().
    اگر (gui.mainwin != پوچ && !really_exiting)
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
	gtk_window_get_position(GTK_WINDOW(gui.mainwin), x, y);
	بازگشت OK;
    }
    بازگشت FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    تهی
gui_mch_set_winpos(صحیح x, صحیح y)
{
    gtk_window_move(GTK_WINDOW(gui.mainwin), x, y);
}

#اگر !GTK_CHECK_VERSION(3,0,0)
# اگر 0
ایستا صحیح resize_idle_installed = FALSE;
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
    ایستا gboolean
force_shell_resize_idle(gpointer data)
{
    اگر (gui.mainwin != پوچ
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	صحیح width;
	صحیح height;

	gtk_window_get_size(GTK_WINDOW(gui.mainwin), &width, &height);

	width  -= get_menu_tool_width();
	height -= get_menu_tool_height();

	gui_resize_shell(width, height);
    }

    resize_idle_installed = FALSE;
    بازگشت FALSE; // don't call me again
}
# endif
#endif // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    صحیح
gui_mch_maximized(تهی)
{
    بازگشت (gui.mainwin != پوچ && gtk_widget_get_window(gui.mainwin) != پوچ
	    && (gdk_window_get_state(gtk_widget_get_window(gui.mainwin))
					       & GDK_WINDOW_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    تهی
gui_mch_unmaximize(تهی)
{
    اگر (gui.mainwin != پوچ)
	gtk_window_unmaximize(GTK_WINDOW(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    تهی
gui_mch_newfont(تهی)
{
    صحیح w, h;

    gtk_window_get_size(GTK_WINDOW(gui.mainwin), &w, &h);
    w -= get_menu_tool_width();
    h -= get_menu_tool_height();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    تهی
gui_mch_set_shellsize(صحیح width, صحیح height,
		      صحیح min_width UNUSED,  صحیح min_height UNUSED,
		      صحیح base_width UNUSED, صحیح base_height UNUSED,
		      صحیح direction UNUSED)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    اگر (gtk_socket_id == 0)
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
    اگر (gtk_socket_id == 0)
	gtk_window_resize(GTK_WINDOW(gui.mainwin), width, height);
    وگرنه
	update_window_manager_hints(width, height);

#اگر !GTK_CHECK_VERSION(3,0,0)
# اگر 0
    اگر (!resize_idle_installed)
    {
	g_idle_add_full(GDK_PRIORITY_EVENTS + 10,
			&force_shell_resize_idle, پوچ, پوچ);
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

    تهی
gui_mch_settitle(char_u *title, char_u *icon UNUSED)
{
    اگر (title != پوچ && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, پوچ);

    gtk_window_set_title(GTK_WINDOW(gui.mainwin), (ثابت نویسه *)title);

    اگر (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#اگر defined(FEAT_MENU)
    تهی
gui_mch_enable_menu(صحیح showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
	widget = gui.menubar_h;
    وگرنه
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    اگر (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	اگر (showit)
	    gtk_widget_show(widget);
	وگرنه
	    gtk_widget_hide(widget);

	update_window_manager_hints(0, 0);
    }
}
#endif // FEAT_MENU

#اگر defined(FEAT_TOOLBAR)
    تهی
gui_mch_show_toolbar(صحیح showit)
{
    GtkWidget *widget;

    اگر (gui.toolbar == پوچ)
	بازگشت;

# ifdef FEAT_GUI_GNOME
    اگر (using_gnome)
	widget = gui.toolbar_h;
    وگرنه
# endif
	widget = gui.toolbar;

    اگر (showit)
	set_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    اگر (!showit != !gtk_widget_get_visible(widget))
    {
	اگر (showit)
	    gtk_widget_show(widget);
	وگرنه
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
    ایستا صحیح
is_cjk_font(PangoFontDescription *font_desc)
{
    ایستا ثابت نویسه * ثابت cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    unsigned	i;
    صحیح		is_cjk = FALSE;

    font = pango_context_load_font(gui.text_context, font_desc);

    اگر (font == پوچ)
	بازگشت FALSE;

    برای (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	اگر (coverage != پوچ)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#اگر PANGO_VERSION_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#وگرنه
	    pango_coverage_unref(coverage);
#endif
	}
    }

    g_object_unref(font);

    بازگشت is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    صحیح
gui_mch_adjust_charheight(تهی)
{
    PangoFontMetrics	*metrics;
    صحیح			ascent;
    صحیح			descent;

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

    بازگشت OK;
}

#اگر GTK_CHECK_VERSION(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ایستا gboolean
font_filter(ثابت PangoFontFamily *family,
	    ثابت PangoFontFace   *face UNUSED,
	    gpointer		   data UNUSED)
{
    بازگشت pango_font_family_is_monospace((PangoFontFamily *)family);
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
    صحیح		response;
    char_u	*fontname = پوچ;
    char_u	*oldname;

#اگر GTK_CHECK_VERSION(3,2,0)
    dialog = gtk_font_chooser_dialog_new(پوچ, پوچ);
    gtk_font_chooser_set_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    پوچ, پوچ);
#وگرنه
    dialog = gtk_font_selection_dialog_new(پوچ);
#endif

    gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(gui.mainwin));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(dialog), TRUE);

    اگر (oldval != پوچ && oldval[0] != NUL)
    {
	اگر (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, پوچ);
	وگرنه
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	اگر (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    اگر (p != پوچ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		اگر (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#اگر GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), (ثابت gchar *)oldname);
#وگرنه
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ثابت نویسه *)oldname);
#endif

	اگر (oldname != oldval)
	    vim_free(oldname);
    }
    وگرنه
#اگر GTK_CHECK_VERSION(3,2,0)
	gtk_font_chooser_set_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#وگرنه
	gtk_font_selection_dialog_set_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#endif

    response = gtk_dialog_run(GTK_DIALOG(dialog));

    اگر (response == GTK_RESPONSE_OK)
    {
	نویسه *name;

#اگر GTK_CHECK_VERSION(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#وگرنه
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#endif
	اگر (name != پوچ)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    اگر (p != پوچ && درون_داد_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&درون_داد_conv, p, پوچ);
		vim_free(p);
	    }
	    وگرنه
		fontname = p;
	}
    }

    اگر (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    بازگشت fontname;
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
    ایستا تهی
get_styled_font_variants(تهی)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_load_font(gui.text_context, gui.norm_font);

    اگر (plain_font == پوچ)
	بازگشت;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_set_weight(bold_font_desc, PANGO_WEIGHT_BOLD);

    bold_font = pango_context_load_font(gui.text_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    اگر (bold_font != پوچ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    اگر (bold_font != پوچ && gui.font_can_bold)
	g_object_unref(plain_font);
}

ایستا PangoEngineShape *default_shape_engine = پوچ;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    ایستا تهی
ascii_glyph_table_init(تهی)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *item_list;
    صحیح		    i;

    اگر (gui.ascii_glyphs != پوچ)
	pango_glyph_string_free(gui.ascii_glyphs);
    اگر (gui.ascii_font != پوچ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = پوچ;
    gui.ascii_font   = پوچ;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    برای (i = 0; i < 128; ++i)
    {
	اگر (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	وگرنه
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    item_list = pango_itemize(gui.text_context, (ثابت نویسه *)ascii_chars,
			      0, اندازه(ascii_chars), attr_list, پوچ);

    اگر (item_list != پوچ && item_list->next == پوچ) // play safe
    {
	PangoItem   *item;
	صحیح	    width;

	item  = (PangoItem *)item_list->data;
	width = gui.char_width * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = item->analysis.shape_engine;

	gui.ascii_font = item->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ثابت نویسه *)ascii_chars, اندازه(ascii_chars),
		    &item->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->رقم_glyphs == اندازه(ascii_chars));

	برای (i = 0; i < gui.ascii_glyphs->رقم_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_offset += MAX(0, width - geom->width) / 2;
	    geom->width = width;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(item_list, (GFunc)(تهی *)&pango_item_free, پوچ);
    g_list_free(item_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    صحیح
gui_mch_init_font(char_u *font_name, صحیح fontset UNUSED)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    صحیح			    width;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    اگر (font_name == پوچ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    اگر (font_desc == پوچ)
	بازگشت FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_set_font_description(gui.text_context, font_desc);

    layout = pango_layout_new(gui.text_context);
    pango_layout_set_text(layout, "MW", 2);
    pango_layout_get_size(layout, &width, پوچ);
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
	صحیح cjk_width;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_set_text(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_size(layout, &cjk_width, پوچ);

	اگر (width == cjk_width)  // Xft not patched
	    width /= 2;
    }
    g_object_unref(layout);

    gui.char_width = (width / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    اگر (gui.char_width <= 0)
	gui.char_width = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_set_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_table_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    اگر (gui.wide_font != پوچ
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = پوچ;
    }

    اگر (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    وگرنه
    {
	// Preserve the logical dimensions of the screen.
	update_window_manager_hints(0, 0);
    }

    بازگشت OK;
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
    اگر (!gui.in_use || name == پوچ)
	بازگشت پوچ;

    اگر (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, پوچ);
	اگر (buf != پوچ)
	{
	    font = pango_font_description_from_string((ثابت نویسه *)buf);
	    vim_free(buf);
	}
	وگرنه
	    font = پوچ;
    }
    وگرنه
	font = pango_font_description_from_string((ثابت نویسه *)name);

    اگر (font != پوچ)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	اگر (pango_font_description_get_size(font) <= 0)
	    pango_font_description_set_size(font, 10 * PANGO_SCALE);

	real_font = pango_context_load_font(gui.text_context, font);

	اگر (real_font == پوچ)
	{
	    pango_font_description_free(font);
	    font = پوچ;
	}
	وگرنه
	    g_object_unref(real_font);
    }

    اگر (font == پوچ)
    {
	اگر (report_error)
	    semsg(_((نویسه *)e_unknown_font_str), name);
	بازگشت پوچ;
    }

    بازگشت font;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name UNUSED)
{
    اگر (font != NOFONT)
    {
	نویسه	*pangoname = pango_font_description_to_string(font);

	اگر (pangoname != پوچ)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    بازگشت s;
	}
    }
    بازگشت پوچ;
}
#endif

/*
 * If a font is not going to be used, free its structure.
 */
    تهی
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
    تهی
gui_mch_expand_font(
    optexpand_T	*args,
    تهی	*param,
    صحیح		(*add_match)(char_u *val))
{
    PangoFontFamily	**font_families = پوچ;
    صحیح			n_families = 0;
    صحیح			wide = *(صحیح *)param;

    اگر (args->oe_include_orig_val && *args->oe_opt_مقدار == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	اگر (add_match((char_u *)DEFAULT_FONT) != OK)
	    بازگشت;
    }

    pango_context_list_families(
	    gui.text_context,
	    &font_families,
	    &n_families);

    برای (صحیح i = 0; i < n_families; i++)
    {
	اگر (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ادامه;

	ثابت نویسه* fam_name = pango_font_family_get_name(font_families[i]);
	اگر (درون_داد_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&درون_داد_conv, (char_u*)fam_name, پوچ);
	    اگر (buf != پوچ)
	    {
		اگر (add_match(buf) != OK)
		{
		    vim_free(buf);
		    بشکن;
		}
		vim_free(buf);
	    }
	    وگرنه
		بشکن;
	}
	وگرنه
	{
	    اگر (add_match((char_u *)fam_name) != OK)
		بشکن;
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

    اگر (!gui.in_use)		// can't do this when GUI not running
	بازگشت color;

    اگر (name != پوچ)
	color = gui_get_color_cmn(name);

#اگر GTK_CHECK_VERSION(3,0,0)
    بازگشت color;
#وگرنه
    اگر (color == INVALCOLOR)
	بازگشت INVALCOLOR;

    بازگشت gui_mch_get_rgb_color(
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
gui_mch_get_rgb_color(صحیح r, صحیح g, صحیح b)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    بازگشت gui_get_rgb_color_cmn(r, g, b);
#وگرنه
    GdkColor gcolor;
    صحیح ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    بازگشت ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#endif
}

/*
 * Set the current text foreground color.
 */
    تهی
gui_mch_set_fg_color(guicolor_T color)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    *gui.fgcolor = color_to_rgba(color);
#وگرنه
    gui.fgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text background color.
 */
    تهی
gui_mch_set_bg_color(guicolor_T color)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    *gui.bgcolor = color_to_rgba(color);
#وگرنه
    gui.bgcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Set the current text special color.
 */
    تهی
gui_mch_set_sp_color(guicolor_T color)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    *gui.spcolor = color_to_rgba(color);
#وگرنه
    gui.spcolor->pixel = (unsigned long)color;
#endif
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#define درج_PANGO_ATTR(Attribute, AttrList, Start, End)  \
    G_STMT_START{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->start_index = (Start);		    \
	tmp_attr_->end_index = (End);			    \
	pango_attr_list_درج((AttrList), tmp_attr_);	    \
    }G_STMT_END

    ایستا تهی
apply_wide_font_attr(char_u *s, صحیح len, PangoAttrList *attr_list)
{
    char_u  *start = پوچ;
    char_u  *p;
    صحیح	    uc;

    برای (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	اگر (start == پوچ)
	{
	    اگر (uc >= 0x80 && utf_char2cells(uc) == 2)
		start = p;
	}
	وگرنه اگر (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, start - s, p - s);
	    start = پوچ;
	}
    }

    اگر (start != پوچ)
	درج_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, start - s, len);
}

    ایستا صحیح
شمار_cluster_cells(char_u *s, PangoItem *item,
		    PangoGlyphString* glyphs, صحیح i,
		    صحیح *cluster_width,
		    صحیح *last_glyph_rbearing)
{
    char_u  *p;
    صحیح	    next;	// glyph start index of next cluster
    صحیح	    start, end; // string segment of current cluster
    صحیح	    width;	// real cluster width in Pango units
    صحیح	    uc;
    صحیح	    cellcount = 0;

    width = glyphs->glyphs[i].geometry.width;

    برای (next = i + 1; next < glyphs->رقم_glyphs; ++next)
    {
	اگر (glyphs->glyphs[next].attr.is_cluster_start)
	    بشکن;
	وگرنه اگر (glyphs->glyphs[next].geometry.width > width)
	    width = glyphs->glyphs[next].geometry.width;
    }

    start = item->offset + glyphs->log_clusters[i];
    end   = item->offset + ((next < glyphs->رقم_glyphs) ?
			    glyphs->log_clusters[next] : item->length);

    برای (p = s + start; p < s + end; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	اگر (uc < 0x80)
	    ++cellcount;
	وگرنه اگر (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    اگر (last_glyph_rbearing != پوچ
	    && cellcount > 0 && next == glyphs->رقم_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(item->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, پوچ);

	اگر (PANGO_RBEARING(ink_rect) > 0)
	    *last_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    اگر (cellcount > 0)
	*cluster_width = width;

    بازگشت cellcount;
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
    ایستا تهی
setup_zero_width_cluster(PangoItem *item, PangoGlyphInfo *glyph,
			 صحیح last_cellcount, صحیح last_cluster_width,
			 صحیح last_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    صحیح		    width;

    width = last_cellcount * gui.char_width * PANGO_SCALE;
    glyph->geometry.x_offset = -width + MAX(0, width - last_cluster_width) / 2;
    glyph->geometry.width = 0;

    pango_font_get_glyph_extents(item->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    اگر (ink_rect.x < 0)
    {
	glyph->geometry.x_offset += last_glyph_rbearing;
	glyph->geometry.y_offset  = logical_rect.height
		- (gui.char_height - p_linespace) * PANGO_SCALE;
    }
    وگرنه
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_offset = -width + MAX(0, width - ink_rect.width) / 2;
}

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا تهی
draw_glyph_string(صحیح row, صحیح col, صحیح رقم_cells, صحیح flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#وگرنه
    ایستا تهی
draw_glyph_string(صحیح row, صحیح col, صحیح رقم_cells, صحیح flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#endif
{
    اگر (!(flags & DRAW_TRANSP))
    {
#اگر GTK_CHECK_VERSION(3,0,0)
	cairo_set_source_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(col), FILL_Y(row),
			رقم_cells * gui.char_width, gui.char_height);
	cairo_fill(cr);
#وگرنه
	gdk_gc_set_foreground(gui.text_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->window,
			   gui.text_gc,
			   TRUE,
			   FILL_X(col),
			   FILL_Y(row),
			   رقم_cells * gui.char_width,
			   gui.char_height);
#endif
    }

#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_to(cr, TEXT_X(col), TEXT_Y(row));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#وگرنه
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->window,
		    gui.text_gc,
		    font,
		    TEXT_X(col),
		    TEXT_Y(row),
		    glyphs);
#endif

    // redraw the contents with an offset of 1 to emulate bold
    اگر ((flags & DRAW_BOLD) && !gui.font_can_bold)
#اگر GTK_CHECK_VERSION(3,0,0)
    {
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, TEXT_X(col) + 1, TEXT_Y(row));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#وگرنه
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
#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا تهی
draw_under(صحیح flags, صحیح row, صحیح col, صحیح cells, cairo_t *cr)
#وگرنه
    ایستا تهی
draw_under(صحیح flags, صحیح row, صحیح col, صحیح cells)
#endif
{
    صحیح			i;
    صحیح			offset;
    ایستا ثابت صحیح	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    صحیح			y = FILL_Y(row + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    اگر (flags & DRAW_UNDERC)
    {
#اگر GTK_CHECK_VERSION(3,0,0)
	cairo_set_سطر_width(cr, 1.0);
	cairo_set_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col) + 1, y - 2 + 0.5);
	برای (i = FILL_X(col) + 1; i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    cairo_سطر_to(cr, i, y - offset + 0.5);
	}
	cairo_stroke(cr);
#وگرنه
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	برای (i = FILL_X(col); i < FILL_X(col + cells); ++i)
	{
	    offset = val[i % 8];
	    gdk_draw_point(gui.drawarea->window, gui.text_gc, i, y - offset);
	}
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Draw a strikethrough line
    اگر (flags & DRAW_STRIKE)
    {
#اگر GTK_CHECK_VERSION(3,0,0)
	cairo_set_سطر_width(cr, 1.0);
	cairo_set_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 1 - gui.char_height/2 + 0.5);
	cairo_سطر_to(cr, FILL_X(col + cells), y + 1 - gui.char_height/2 + 0.5);
	cairo_stroke(cr);
#وگرنه
	gdk_gc_set_foreground(gui.text_gc, gui.spcolor);
	gdk_draw_سطر(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y + 1 - gui.char_height/2,
		      FILL_X(col + cells), y + 1 - gui.char_height/2);
	gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    }

    // Underline: draw a line at the bottom of the character cell.
    اگر (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	اگر (p_linespace > 1)
	    y -= p_linespace - 1;
#اگر GTK_CHECK_VERSION(3,0,0)
	cairo_set_سطر_width(cr, 1.0);
	cairo_set_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_to(cr, FILL_X(col), y + 0.5);
	cairo_سطر_to(cr, FILL_X(col + cells), y + 0.5);
	cairo_stroke(cr);
#وگرنه
	gdk_draw_سطر(gui.drawarea->window, gui.text_gc,
		      FILL_X(col), y,
		      FILL_X(col + cells) - 1, y);
#endif
    }
}

    صحیح
gui_gtk_draw_string(صحیح row, صحیح col, char_u *s, صحیح len, صحیح flags)
{
    char_u	*conv_buf = پوچ;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    صحیح		convlen;
    char_u	*sp, *bp;
    صحیح		plen;
    صحیح		len_sum;	// return value needs to add up since we are
				// printing substrings
    صحیح		byte_sum;	// byte position in string
    char_u	*cs;		// current *s pointer
    صحیح		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    صحیح		should_need_pango = FALSE;
    صحیح		slen;
    صحیح		is_ligature;
    صحیح		is_utf8;
    char_u	backup_ch;

    اگر (gui.text_context == پوچ || gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت len;

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
	g_return_val_if_fail(conv_buf != پوچ, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	برای (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    اگر ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		اگر (new_conv_buf == پوچ)
		    بازگشت len;
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
    اگر (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    اگر (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    وقتی (cs < s + len)
    {
	slen = 0;
	وقتی (slen < (len - byte_sum))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    اگر (is_ligature && !needs_pango)
	    {
		اگر ((slen + 1) < (len - byte_sum))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		وگرنه
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    اگر ((!is_utf8) && ((slen + 1) < (len - byte_sum)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    اگر (needs_pango != should_need_pango) // mode switch
		بشکن;
	    اگر (needs_pango)
	    {
		اگر (is_ligature)
		{
		    slen++; // ligature char by char
		}
		وگرنه // is_utf8
		{
		    اگر ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			وقتی ((slen < (len - byte_sum))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    وگرنه اگر ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    وگرنه اگر ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    وگرنه اگر ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    وگرنه
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    وگرنه
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
	len_sum += gui_gtk_draw_string_ext(row, col + len_sum, cs, slen, flags,
					    needs_pango);
	اگر (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_sum += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    بازگشت len_sum;
}

#اگر GTK_CHECK_VERSION(3,0,0)
ایستا cairo_region_t *dirty_region = پوچ;

    ایستا تهی
queue_draw_area(صحیح x, صحیح y, صحیح width, صحیح height)
{
    cairo_rectangle_int_t rect;

    اگر (width <= 0 || height <= 0 || gui.drawarea == پوچ)
	بازگشت;

    rect.x = x;
    rect.y = y;
    rect.width = width;
    rect.height = height;

    اگر (dirty_region == پوچ)
	dirty_region = cairo_region_create_rectangle(&rect);
    وگرنه
	cairo_region_union_rectangle(dirty_region, &rect);
}
#endif

    صحیح
gui_gtk_draw_string_ext(
	صحیح	row,
	صحیح	col,
	char_u	*s,
	صحیح	len,
	صحیح	flags,
	صحیح	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    صحیح			column_offset = 0;  // column offset in cells
    صحیح			i;
#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_t		*cr;
#endif

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_offset;
    area.y = FILL_Y(row);
    area.width	= gui.رقم_cols * gui.char_width;
    area.height = gui.char_height;

#اگر GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.width, area.height);
    cairo_clip(cr);
#وگرنه
    gdk_gc_set_clip_origin(gui.text_gc, 0, 0);
    gdk_gc_set_clip_rectangle(gui.text_gc, &area);
#endif

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    اگر (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != پوچ
	    && !force_pango)
    {
	char_u *p;

	برای (p = s; p < s + len; ++p)
	    اگر (*p & 0x80)
		برو not_ascii;

	pango_glyph_string_set_size(glyphs, len);

	برای (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#اگر GTK_CHECK_VERSION(3,0,0)
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs, cr);
#وگرنه
	draw_glyph_string(row, col, len, flags, gui.ascii_font, glyphs);
#endif

	column_offset = len;
    }
    وگرنه
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*item_list;
	صحیح		cluster_width;
	صحیح		last_glyph_rbearing;
	صحیح		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	اگر (!utf_valid_string(s, s + len))
	{
	    column_offset = len;
	    برو skipitall;
	}

	// original width of the current cluster
	cluster_width = PANGO_SCALE * gui.char_width;

	// right bearing of the last non-composing glyph
	last_glyph_rbearing = PANGO_SCALE * gui.char_width;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	اگر (gui.wide_font != پوچ)
	    apply_wide_font_attr(s, len, attr_list);

	اگر ((flags & DRAW_BOLD) && gui.font_can_bold)
	    درج_PANGO_ATTR(pango_attr_weight_new(PANGO_WEIGHT_BOLD),
			      attr_list, 0, len);
	اگر (flags & DRAW_ITALIC)
	    درج_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	item_list = pango_itemize(gui.text_context,
				  (ثابت نویسه *)s, 0, len, attr_list, پوچ);

	وقتی (item_list != پوچ)
	{
	    PangoItem	*item;
	    صحیح		item_cells = 0; // item length in cells

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
	    pango_shape_full((ثابت نویسه *)s + item->offset, item->length,
		    (ثابت نویسه *)s, len, &item->analysis, glyphs);
#وگرنه
	    pango_shape((ثابت نویسه *)s + item->offset, item->length,
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
	    برای (i = 0; i < glyphs->رقم_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		اگر (glyph->attr.is_cluster_start)
		{
		    صحیح cellcount;

		    cellcount = شمار_cluster_cells(
			s, item, glyphs, i, &cluster_width,
			(item_list != پوچ) ? &last_glyph_rbearing : پوچ);

		    اگر (cellcount > 0)
		    {
			صحیح width;

			width = cellcount * gui.char_width * PANGO_SCALE;
			glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
			glyph->geometry.width = width;
		    }
		    وگرنه
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
		وگرنه اگر (i > 0)
		{
		    صحیح width;

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
		    اگر (glyph->geometry.x_offset >= 0)
		    {
			glyphs->glyphs[i].geometry.width =
					 glyphs->glyphs[i - 1].geometry.width;
			glyphs->glyphs[i - 1].geometry.width = 0;
		    }
		    width = cells * gui.char_width * PANGO_SCALE;
		    glyph->geometry.x_offset +=
					    MAX(0, width - cluster_width) / 2;
		}
		وگرنه // i == 0 "cannot happen"
		{
		    glyph->geometry.width = 0;
		}
	    }

	    //// Aaaaand action! **
#اگر GTK_CHECK_VERSION(3,0,0)
	    draw_glyph_string(row, col + column_offset, item_cells,
			      flags, item->analysis.font, glyphs,
			      cr);
#وگرنه
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
#اگر GTK_CHECK_VERSION(3,0,0)
    draw_under(flags, row, col, column_offset, cr);
#وگرنه
    draw_under(flags, row, col, column_offset);
#endif

    pango_glyph_string_free(glyphs);

#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_destroy(cr);
    queue_draw_area(FILL_X(col), FILL_Y(row),
	    column_offset * gui.char_width + 1, gui.char_height);
#وگرنه
    gdk_gc_set_clip_rectangle(gui.text_gc, پوچ);
#endif

    بازگشت column_offset;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    صحیح
gui_mch_haskey(char_u *name)
{
    صحیح i;

    برای (i = 0; special_کلیدها[i].کلید_sym != 0; i++)
	اگر (name[0] == special_کلیدها[i].code0
		&& name[1] == special_کلیدها[i].code1)
	    بازگشت OK;
    بازگشت FAIL;
}

#اگر defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    صحیح
gui_get_x11_windis(Window *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    اگر (dpy)
    {
	*dis = dpy;
	*win = GDK_WINDOW_XID(gtk_widget_get_window(gui.mainwin));
	بازگشت OK;
    }

    *dis = پوچ;
    *win = 0;
    بازگشت FAIL;
}
#endif

    Display *
gui_mch_get_display(تهی)
{
    اگر (gui.mainwin != پوچ && gtk_widget_get_window(gui.mainwin) != پوچ
#اگر GTK_CHECK_VERSION(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#endif
	)
	بازگشت GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin));
    وگرنه
	بازگشت پوچ;
}

    تهی
gui_mch_beep(تهی)
{
    GdkDisplay *display;

    اگر (gui.mainwin != پوچ && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    وگرنه
	display = gdk_display_get_default();

    اگر (display != پوچ)
	gdk_display_beep(display);
}

    تهی
gui_mch_flash(صحیح msec)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    // TODO Replace GdkGC with Cairo
    (تهی)msec;
#وگرنه
    GdkGCValues	مقادیر;
    GdkGC	*invert_gc;

    اگر (gui.drawarea->window == پوچ)
	بازگشت;

    مقادیر.foreground.pixel = gui.norm_pixel ^ gui.back_pixel;
    مقادیر.background.pixel = gui.norm_pixel ^ gui.back_pixel;
    مقادیر.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_مقادیر(gui.drawarea->window,
				       &مقادیر,
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
		       FILL_X((صحیح)Columns) + gui.border_offset,
		       FILL_Y((صحیح)Rows) + gui.border_offset);

    gui_mch_flush();
    ui_delay((long)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->window, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((صحیح)Columns) + gui.border_offset,
		       FILL_Y((صحیح)Rows) + gui.border_offset);

    gdk_gc_destroy(invert_gc);
#endif
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    تهی
gui_mch_invert_rectangle(صحیح r, صحیح c, صحیح nr, صحیح nc)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    ثابت GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_width, nr * gui.char_height
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);

    cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# اگر CAIRO_VERSION >= CAIRO_VERSION_ENCODE(1,9,2)
    cairo_set_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# وگرنه
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);

    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
#وگرنه
    GdkGCValues مقادیر;
    GdkGC *invert_gc;

    اگر (gui.drawarea->window == پوچ)
	بازگشت;

    مقادیر.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_مقادیر(gui.drawarea->window,
				       &مقادیر,
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
    تهی
gui_mch_iconify(تهی)
{
    gtk_window_iconify(GTK_WINDOW(gui.mainwin));
}

#اگر defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    تهی
gui_mch_set_foreground(تهی)
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
    تهی
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    صحیح		i = 1;
#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_t    *cr;
#endif

    اگر (gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت;

#اگر GTK_CHECK_VERSION(3,0,0)
    cr = cairo_create(gui.surface);
#endif

    gui_mch_set_fg_color(color);

#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_set_source_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#وگرنه
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
#endif
    اگر (mb_lefthalve(gui.row, gui.col))
	i = 2;
#اگر GTK_CHECK_VERSION(3,0,0)
    cairo_set_سطر_width(cr, 1.0);
    cairo_set_سطر_cap(cr, CAIRO_سطر_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.col) + 0.5, FILL_Y(gui.row) + 0.5,
	    i * gui.char_width - 1, gui.char_height - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#وگرنه
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
    تهی
gui_mch_draw_part_cursor(صحیح w, صحیح h, guicolor_T color)
{
    اگر (gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت;

    gui_mch_set_fg_color(color);

#اگر GTK_CHECK_VERSION(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_set_source_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_راست ? FILL_X(gui.col + 1) - w :
# endif
	    FILL_X(gui.col), FILL_Y(gui.row) + gui.char_height - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#وگرنه // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->window, gui.text_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_راست ? FILL_X(gui.col + 1) - w :
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
    تهی
gui_mch_update(تهی)
{
    صحیح cnt = 0;	// prevent endless loop
    وقتی (g_main_context_pending(پوچ) && !vim_is_درون_داد_buf_full()
	    && ++cnt < 100)
    {
#اگر GTK_CHECK_VERSION(3,0,0)
	صحیح prio = 0;
	g_main_context_prepare(پوچ, &prio);
	// peek internal scheduling of redraw
	اگر (prio == GDK_PRIORITY_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#endif
	g_main_context_iteration(پوچ, TRUE);
    }
}

    ایستا timeout_cb_type
درون_داد_timer_cb(gpointer data)
{
    صحیح *timed_out = (صحیح *) data;

    // Just inform the caller about the occurrence of it
    *timed_out = TRUE;

    بازگشت FALSE;		// don't happen again
}

#ifdef FEAT_JOB_CHANNEL
    ایستا timeout_cb_type
channel_poll_cb(gpointer data UNUSED)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    بازگشت TRUE;		// repeat
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
    صحیح
gui_mch_wait_for_chars(long wtime)
{
    صحیح		focus;
    guint	timer;
    ایستا صحیح	timed_out;
    صحیح		retval = FAIL;
#ifdef FEAT_JOB_CHANNEL
    guint	channel_timer = 0;
#endif

    timed_out = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    اگر (wtime >= 0)
	timer = timeout_add(wtime == 0 ? 1L : wtime,
						   درون_داد_timer_cb, &timed_out);
    وگرنه
	timer = 0;

#ifdef FEAT_JOB_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    اگر (channel_any_keep_open())
	channel_timer = timeout_add(20, channel_poll_cb, پوچ);
#endif

    focus = gui.in_focus;

    انجامبده
    {
	// Stop or start blinking when focus changes
	اگر (gui.in_focus != focus)
	{
	    اگر (gui.in_focus)
		gui_mch_start_blink();
	    وگرنه
		gui_mch_stop_blink(TRUE);
	    focus = gui.in_focus;
	}

#ifdef MESSAGE_QUEUE
# ifdef FEAT_TIMERS
	did_add_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	اگر (did_add_timer)
	    // Need to recompute the waiting time.
	    برو theend;
# endif
#endif

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	اگر (!درون_داد_available())
	{
	    g_main_context_iteration(پوچ, TRUE);
#ifdef GDK_WINDOWING_WAYLAND
    اگر (gui.is_wayland && clip_star.state == SELECT_IN_PROGRESS)
	gui_may_flush();
#endif
	}

	// Got char, return immediately
	اگر (درون_داد_available())
	{
	    retval = OK;
	    برو theend;
	}
    } وقتی (wtime < 0 || !timed_out);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    اگر (timer != 0 && !timed_out)
	timeout_remove(timer);
#ifdef FEAT_JOB_CHANNEL
    اگر (channel_timer != 0)
	timeout_remove(channel_timer);
#endif

    بازگشت retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    تهی
gui_mch_flush(تهی)
{
    اگر (gui.mainwin == پوچ || !gtk_widget_get_realized(gui.mainwin))
	بازگشت;
#اگر GTK_CHECK_VERSION(3,0,0)
    اگر (dirty_region != پوچ && gui.drawarea != پوچ)
    {
	gtk_widget_queue_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = پوچ;
    }
#وگرنه
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       بازگشت;
#endif
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    تهی
gui_mch_clear_block(صحیح row1arg, صحیح col1arg, صحیح row2arg, صحیح col2arg)
{

    صحیح col1 = check_col(col1arg);
    صحیح col2 = check_col(col2arg);
    صحیح row1 = check_row(row1arg);
    صحیح row2 = check_row(row2arg);

#اگر GTK_CHECK_VERSION(3,0,0)
    اگر (gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت;
#وگرنه
    GdkColor color;

    اگر (gui.drawarea->window == پوچ)
	بازگشت;

    color.pixel = gui.back_pixel;
#endif

#اگر GTK_CHECK_VERSION(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ثابت GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_width + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_height
	};
	cairo_t * ثابت cr = cairo_create(gui.surface);
# اگر GTK_CHECK_VERSION(3,22,2)
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# وگرنه
	GdkWindow * ثابت win = gtk_widget_get_window(gui.drawarea);
	cairo_pattern_t * ثابت pat = gdk_window_get_background_pattern(win);
	اگر (pat != پوچ)
	    cairo_set_source(cr, pat);
	وگرنه
	    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
	cairo_fill(cr);
	cairo_destroy(cr);

	queue_draw_area(rect.x, rect.y, rect.width, rect.height);
    }
#وگرنه // !GTK_CHECK_VERSION(3,0,0)
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

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا تهی
gui_gtk_window_clear(GdkWindow *win)
{
    ثابت GdkRectangle rect = {
	0, 0, gdk_window_get_width(win), gdk_window_get_height(win)
    };
    cairo_t * ثابت cr = cairo_create(gui.surface);
# اگر GTK_CHECK_VERSION(3,22,2)
    set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# وگرنه
    cairo_pattern_t * ثابت pat = gdk_window_get_background_pattern(win);
    اگر (pat != پوچ)
	cairo_set_source(cr, pat);
    وگرنه
	set_cairo_source_rgba_from_color(cr, gui.back_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.width, rect.height);
    cairo_fill(cr);
    cairo_destroy(cr);

    queue_draw_area(rect.x, rect.y, rect.width, rect.height);
}
#وگرنه
# define gui_gtk_window_clear(win)  gdk_window_clear(win)
#endif

    تهی
gui_mch_clear_all(تهی)
{
    اگر (gtk_widget_get_window(gui.drawarea) != پوچ)
	gui_gtk_window_clear(gtk_widget_get_window(gui.drawarea));
}

#اگر !GTK_CHECK_VERSION(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ایستا تهی
check_copy_area(تهی)
{
    GdkEvent        *event;
    صحیح                expose_شمار;
    اگر (gui.visibility != GDK_VISIBILITY_PARTIAL)
	بازگشت;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    انجامبده
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->window);
	اگر (event == پوچ)
	    بشکن; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.width, event->expose.area.height);
	expose_شمار = event->expose.شمار;
	gdk_event_free(event);
    }
    وقتی (expose_شمار > 0); // more events follow
    gui_can_update_cursor();
}
#endif // !GTK_CHECK_VERSION(3,0,0)

#اگر GTK_CHECK_VERSION(3,0,0)
    ایستا تهی
gui_gtk_surface_copy_rect(صحیح dest_x, صحیح dest_y,
			  صحیح src_x,  صحیح src_y,
			  صحیح width,  صحیح height)
{
    cairo_t * ثابت cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ایستا cairo_surface_t *scroll_scratch = پوچ;
    ایستا صحیح scratch_w = 0, scratch_h = 0;
    صحیح last_row = Rows - 1, last_row_y = last_row * gui.char_height;
    bool last_row_overlap = (dest_y + height) > last_row_y;
    اگر (gui.is_wayland && ( !(State & MODE_CMDLINE) || !last_row_overlap) )
    {
	اگر (dest_y < src_y) // scroll up
	{
	    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	    cairo_set_source_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, width, height);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	وگرنه
	{   //  reuse surface when scrolling, only realloc if larger
	    اگر (scroll_scratch == پوچ || width > scratch_w || height > scratch_h)
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
    وگرنه
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, width, height);
	cairo_clip(cr);
	cairo_افزودن_group(cr);
	cairo_set_source_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_برداشت_group_to_source(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#endif

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    تهی
gui_mch_delete_سطرها(صحیح row, صحیح رقم_سطرها)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    ثابت صحیح ncols = gui.scroll_region_راست - gui.scroll_region_چپ + 1;
    ثابت صحیح nrows = gui.scroll_region_bot - row + 1;
    ثابت صحیح src_nrows = nrows - رقم_سطرها;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row + رقم_سطرها),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    gui.scroll_region_bot - رقم_سطرها + 1, gui.scroll_region_چپ,
	    gui.scroll_region_bot,		   gui.scroll_region_راست);
    queue_draw_area(
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#وگرنه
    اگر (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	بازگشت;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_چپ),
	    FILL_Y(row + رقم_سطرها),
	    gui.char_width * (gui.scroll_region_راست
					    - gui.scroll_region_چپ + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - رقم_سطرها + 1));

    gui_clear_block(gui.scroll_region_bot - رقم_سطرها + 1,
						       gui.scroll_region_چپ,
		    gui.scroll_region_bot, gui.scroll_region_راست);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    تهی
gui_mch_درج_سطرها(صحیح row, صحیح رقم_سطرها)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    ثابت صحیح ncols = gui.scroll_region_راست - gui.scroll_region_چپ + 1;
    ثابت صحیح nrows = gui.scroll_region_bot - row + 1;
    ثابت صحیح src_nrows = nrows - رقم_سطرها;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row + رقم_سطرها),
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    gui.char_width * ncols + 1,     gui.char_height * src_nrows);
    gui_clear_block(
	    row,		 gui.scroll_region_چپ,
	    row + رقم_سطرها - 1, gui.scroll_region_راست);
    queue_draw_area(
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    gui.char_width * ncols + 1,	gui.char_height * nrows);
#وگرنه
    اگر (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	بازگشت;			// Can't see the window

    gdk_gc_set_foreground(gui.text_gc, gui.fgcolor);
    gdk_gc_set_background(gui.text_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_window_copy_area(gui.drawarea->window, gui.text_gc,
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row + رقم_سطرها),
	    gui.drawarea->window,
	    FILL_X(gui.scroll_region_چپ), FILL_Y(row),
	    gui.char_width * (gui.scroll_region_راست
					    - gui.scroll_region_چپ + 1) + 1,
	    gui.char_height * (gui.scroll_region_bot - row - رقم_سطرها + 1));

    gui_clear_block(row, gui.scroll_region_چپ,
				row + رقم_سطرها - 1, gui.scroll_region_راست);
    check_copy_area();
#endif // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    تهی
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	target;
    unsigned	i;
    time_t	start;

    برای (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	اگر (!clip_html && selection_targets[i].info == TARGET_HTML)
	    ادامه;
	received_selection = RS_NONE;
	target = gdk_atom_intern(selection_targets[i].target, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, target,
			      (guint32)GDK_CURRENT_TIME);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	start = time(پوچ);
	وقتی (received_selection == RS_NONE && time(پوچ) < start + 3)
	    g_main_context_iteration(پوچ, TRUE);	// wait for selection_received_cb

	اگر (received_selection != RS_FAIL)
	    بازگشت;
    }

    اگر (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_WINDOW_XDISPLAY(gtk_widget_get_window(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    تهی
clip_mch_lose_selection(Clipboard_T *cbd UNUSED)
{
    اگر (in_selection_clear_event)
	بازگشت;

    gtk_selection_owner_set(پوچ, cbd->gtk_sel_atom, gui.event_time);
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
	بازگشت OK;

    صحیح success;

    success = gtk_selection_owner_set(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_time);
    // don't update on every visual selection change
    اگر (!(cbd->owned && VIsual_active))
	gui_gtk_set_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    بازگشت (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    تهی
clip_mch_set_selection(Clipboard_T *cbd UNUSED)
{
}

#اگر defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    صحیح
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    بازگشت gdk_selection_owner_get(cbd->gtk_sel_atom) != پوچ;
}
#endif


#اگر defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    تهی
gui_mch_menu_grey(vimmenu_T *menu, صحیح grey)
{
    اگر (menu->id == پوچ)
	بازگشت;

    اگر (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    اگر (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_set_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    تهی
gui_mch_menu_hidden(vimmenu_T *menu, صحیح hidden)
{
    اگر (menu->id == 0)
	بازگشت;

    اگر (hidden)
    {
	اگر (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    وگرنه
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
    تهی
gui_mch_draw_menubar(تهی)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#endif // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    تهی
gui_mch_enable_scrollbar(scrollbar_T *sb, صحیح flag)
{
    اگر (sb->id == پوچ)
	بازگشت;

    gtk_widget_set_visible(sb->id, flag);
    update_window_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#اگر GTK_CHECK_VERSION(3,0,0)
    بازگشت (long_u)pixel;
#وگرنه
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (unsigned long)pixel, &color);

    بازگشت (guicolor_T)(
	    (((unsigned)color.red   & 0xff00) << 8)
	 |  ((unsigned)color.green & 0xff00)
	 | (((unsigned)color.blue  & 0xff00) >> 8));
#endif
}

/*
 * Get current mouse coordinates in text window.
 */
    تهی
gui_mch_getmouse(صحیح *x, صحیح *y)
{
    gui_gtk_get_pointer(gui.drawarea, x, y, پوچ);
}

    تهی
gui_mch_setmouse(صحیح x, صحیح y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    اگر (dpy)
	XWarpPointer(dpy, (Window)0,
		GDK_WINDOW_XID(gtk_widget_get_window(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#ifdef FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ایستا صحیح last_shape = 0;
#endif

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    تهی
gui_mch_mousehide(صحیح hide)
{
    اگر (gui.pointer_hidden == hide)
	بازگشت;

    gui.pointer_hidden = hide;
    اگر (gtk_widget_get_window(gui.drawarea) && gui.blank_pointer != پوچ)
    {
	اگر (hide)
	    gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		    gui.blank_pointer);
	وگرنه
#ifdef FEAT_MOUSESHAPE
	    mch_set_mouse_shape(last_shape);
#وگرنه
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), پوچ);
#endif
    }
}

#اگر defined(FEAT_MOUSESHAPE)

# اگر GTK_CHECK_VERSION(3,0,0)
ایستا ثابت نویسه * mshape_css_names[] =
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
# وگرنه
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ایستا ثابت صحیح mshape_ids[] =
{
    GDK_چپ_PTR,		// arrow
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
    GDK_راست_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_چپ_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    تهی
mch_set_mouse_shape(صحیح shape)
{
    GdkCursor	   *c;
    صحیح		   id;                    // Only id or css_name is used.
# اگر GTK_CHECK_VERSION(3,0,0)
    ثابت نویسه     *css_name = "default";
    GdkDisplay     *disp;
# endif

    اگر (gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت;

    اگر (shape == MSHAPE_HIDE || gui.pointer_hidden)
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea),
		gui.blank_pointer);
    وگرنه
    {
	اگر (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    اگر (id >= GDK_LAST_CURSOR)
		id = GDK_چپ_PTR;
	    وگرنه
		id &= ~1;	// they are always even (why?)
	}
# اگر GTK_CHECK_VERSION(3,0,0)
	وگرنه اگر (shape < (صحیح)ARRAY_LENGTH(mshape_css_names))
	    css_name = mshape_css_names[shape];
# وگرنه
	وگرنه اگر (shape < (صحیح)ARRAY_LENGTH(mshape_ids))
	    id = mshape_ids[shape];
# endif
	وگرنه
	    بازگشت;
# اگر GTK_CHECK_VERSION(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# وگرنه
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_window_set_cursor(gtk_widget_get_window(gui.drawarea), c);
# اگر GTK_CHECK_VERSION(3,0,0)
	g_object_unref(G_OBJECT(c));
# وگرنه
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    اگر (shape != MSHAPE_HIDE)
	last_shape = shape;
}
#endif // FEAT_MOUSESHAPE


#اگر defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_WIDTH  (2 * gui.char_width)
# define SIGN_HEIGHT (gui.char_height)
# define SIGN_ASPECT ((دوگانه)SIGN_HEIGHT / (دوگانه)SIGN_WIDTH)

    تهی
gui_mch_drawsign(صحیح row, صحیح col, صحیح typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    اگر (sign == پوچ || gui.drawarea == پوچ
	    || gtk_widget_get_window(gui.drawarea) == پوچ)
	بازگشت;

    صحیح width;
    صحیح height;
    صحیح xoffset;
    صحیح yoffset;
    صحیح need_scale;

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
    اگر (need_scale)
    {
	دوگانه  aspect;
	صحیح	    w = width;
	صحیح	    h = height;

	// Keep the original aspect ratio
	aspect = (دوگانه)height / (دوگانه)width;
	width  = (دوگانه)SIGN_WIDTH * SIGN_ASPECT / aspect;
	width  = MIN(width, SIGN_WIDTH);
	اگر (((دوگانه)(MAX(height, SIGN_HEIGHT)) /
		    (دوگانه)(MIN(height, SIGN_HEIGHT))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    height = (دوگانه)SIGN_HEIGHT * SIGN_ASPECT / aspect;
	    height = MIN(height, SIGN_HEIGHT);
	}
	وگرنه
	    height = (دوگانه)width * aspect;

	اگر (w == width && h == height)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	وگرنه
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, width, height,
		    GDK_INTERP_BILINEAR);
	    اگر (sign == پوچ)
		بازگشت; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (width  - SIGN_WIDTH)  / 2;
    yoffset = (height - SIGN_HEIGHT) / 2;

# اگر GTK_CHECK_VERSION(3,0,0)
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
# وگرنه // !GTK_CHECK_VERSION(3,0,0)
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
    اگر (need_scale)
	g_object_unref(sign);
}

    تهی *
gui_mch_register_sign(char_u *signfile)
{
    اگر (signfile[0] != NUL && signfile[0] != '-' && gui.in_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = پوچ;
	char_u	    *message;

	sign = gdk_pixbuf_new_from_file((ثابت نویسه *)signfile, &error);

	اگر (error == پوچ)
	    بازگشت sign;

	message = (char_u *)error->message;

	اگر (message != پوچ && درون_داد_conv.vc_type != CONV_NONE)
	    message = string_convert(&درون_داد_conv, message, پوچ);

	اگر (message != پوچ)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", message);

	    اگر (درون_داد_conv.vc_type != CONV_NONE)
		vim_free(message);
	}
	g_error_free(error);
    }

    بازگشت پوچ;
}

    تهی
gui_mch_destroy_sign(تهی *sign)
{
    اگر (sign != پوچ)
	g_object_unref(sign);
}

#endif // FEAT_SIGN_ICONS

#اگر defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

تعریفنوع ساختار
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
    ایستا تهی
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->data;
    uint8_t *dst;
    صحیح	    w = img->width;
    صحیح	    h = img->height;
    صحیح	    stride;
    صحیح	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_data(surf);
    stride = cairo_image_surface_get_stride(surf);

    اگر (img->fmt == IMAGE_FORMAT_RGBA)
    {
	برای (y = 0; y < h; ++y)
	{
	    unsigned صحیح *p = (unsigned صحیح *)(dst + y * stride);

	    برای (x = 0; x < w; ++x)
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
    وگرنه
    {
	برای (y = 0; y < h; ++y)
	{
	    unsigned صحیح *p = (unsigned صحیح *)(dst + y * stride);

	    برای (x = 0; x < w; ++x)
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
# اگر GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    اگر (ctx == پوچ)
	بازگشت FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->width, img->height);

    اگر (cairo_surface_status(ctx->surf) != CAIRO_STATUS_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	برو fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_data = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    بازگشت OK;
fail:
    vim_free(ctx);
    بازگشت FAIL;
# وگرنه
    بازگشت FAIL;
# endif
}

    تهی
image_gui_uninit(image_T *img)
{
# اگر GTK_CHECK_VERSION(3,0,0)
    image_cairo_T *ctx = img->backend_data;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    صحیح
image_placement_gui_init(image_placement_T *place UNUSED)
{
# اگر GTK_CHECK_VERSION(3,0,0)
    بازگشت OK;
# وگرنه
    بازگشت FAIL;
# endif
}

    تهی
image_placement_gui_uninit(image_placement_T *place UNUSED)
{
}

    تهی
image_placement_gui_draw(image_placement_T *place)
{
# اگر GTK_CHECK_VERSION(3,0,0)
    image_T		    *img = place->img;
    image_cairo_T	    *ictx = img->backend_data;
    pixman_box32_t	    *rects;
    صحیح			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&place->visible, &n_rects);
    اگر (rects == پوچ || n_rects == 0)
	بازگشت;

    cairo_surface_set_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    برای (صحیح i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	صحیح		row, col;
	صحیح		posx, posy;
	صحیح		x, y, w, h;

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

    تهی
image_placement_gui_clear(image_placement_T *place UNUSED)
{
}

#endif // FEAT_IMAGE
