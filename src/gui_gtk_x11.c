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

#అన్తర్భావ "vim.h"
#నిర్వచితమైతే USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GNOME
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
# ఒకవేళ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
బాహ్య శూన్య bonobo_dock_అంశం_సమితి_behavior(Bonobo_Dock_అంశం *dock_అంశం, Bonobo_Dock_అంశం_Behavior beh);
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI_GTK)
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# లేకపోతే
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# లేకపోతే
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#అయితే_ముగింపు

/*
 * Easy-to-use macro for multihead support.
 */
#నిర్వచించు GET_X_ATOM(atom)	gdk_x11_atom_గమ్యం_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
గణన
{
    లక్ష్యం_TYPE_NONE,
    లక్ష్యం_UTF8_STRING,
    లక్ష్యం_STRING,
    లక్ష్యం_COMPOUND_పాఠ్యం,
    లక్ష్యం_HTML,
    లక్ష్యం_పాఠ్యం,
    లక్ష్యం_పాఠ్యం_URI_LIST,
    లక్ష్యం_పాఠ్యం_PLAIN,
    లక్ష్యం_పాఠ్యం_PLAIN_UTF8,
    లక్ష్యం_VIM_MT,
    లక్ష్యం_VIM,
    లక్ష్యం_VIMENC_MT,
    లక్ష్యం_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
స్థిర స్థిరరాశి Gtk_లక్ష్యం_నమోదు selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, లక్ష్యం_VIMENC},
    {VIM_ATOM_NAME,	0, లక్ష్యం_VIM},
    {VIMENC_MIMETYPE_NAME,  0, లక్ష్యం_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, లక్ష్యం_VIM_MT},
    {"text/html",	0, లక్ష్యం_HTML},
    {"UTF8_STRING",	0, లక్ష్యం_UTF8_STRING},
    {"COMPOUND_TEXT",	0, లక్ష్యం_COMPOUND_పాఠ్యం},
    {"TEXT",		0, లక్ష్యం_పాఠ్యం},
    {"STRING",		0, లక్ష్యం_STRING},
    {"text/plain;charset=utf-8", 0, లక్ష్యం_పాఠ్యం_PLAIN_UTF8},
    {"text/plain",	0, లక్ష్యం_పాఠ్యం_PLAIN}
};
#నిర్వచించు N_SELECTION_TARGETS ARRAY_నిడివి(selection_targets)

#నిర్వచితమైతే FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
స్థిర స్థిరరాశి Gtk_లక్ష్యం_నమోదు dnd_targets[] =
{
    {"text/uri-list",	0, లక్ష్యం_పాఠ్యం_URI_LIST},
    {"text/html",	0, లక్ష్యం_HTML},
    {"UTF8_STRING",	0, లక్ష్యం_UTF8_STRING},
    {"STRING",		0, లక్ష్యం_STRING},
    {"text/plain",	0, లక్ష్యం_పాఠ్యం_PLAIN}
};
# define N_DND_TARGETS ARRAY_నిడివి(dnd_targets)
#అయితే_ముగింపు


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#నిర్వచించు DEFAULT_FONT	"Monospace 12"

#ఒకవేళ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#అయితే_ముగింపు

#ఒకవేళ !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
స్థిర GdkAtom wm_protocols_atom = GDK_NONE;
స్థిర GdkAtom save_yourself_atom = GDK_NONE;
#అయితే_ముగింపు

/*
 * Atoms used to control/reference X11 selections.
 */
స్థిర GdkAtom html_atom = GDK_NONE;
స్థిర GdkAtom utf8_string_atom = GDK_NONE;
స్థిర GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
స్థిర GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
స్థిర GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
స్థిర GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
స్థిర నిర్మాణం special_కీలకం
{
    guint కీలకం_sym;
    char_u code0;
    char_u code1;
}
స్థిరరాశి special_కీలకాలు[] =
{
    {GDK_పైకి,		'k', 'u'},
    {GDK_కిందికి,		'k', 'd'},
    {GDK_ఎడమ,		'k', 'l'},
    {GDK_కుడి,		'k', 'r'},
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
#నిర్వచితమైతే SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#అయితే_ముగింపు
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_వెనుక_Space,	'k', 'b'},
    {GDK_చొప్పించు,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_వెనుక_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_చివర,		'@', '7'},
    {GDK_పూర్వ,		'k', 'P'},
    {GDK_తదుపరి,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_ఎడమ,	'k', 'l'},
    {GDK_KP_కుడి,	'k', 'r'},
    {GDK_KP_పైకి,		'k', 'u'},
    {GDK_KP_కిందికి,	'k', 'd'},
    {GDK_KP_చొప్పించు,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_చివర,	'K', '4'},
    {GDK_KP_పూర్వ,	'K', '3'},  // page up
    {GDK_KP_తదుపరి,	'K', '5'},  // page down

    {GDK_KP_కూడు,	'K', '6'},
    {GDK_KP_వ్యవకలనం,	'K', '7'},
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
#నిర్వచించు ARG_FONT	1
#నిర్వచించు ARG_GEOMETRY	2
#నిర్వచించు ARG_తిరగేయి	3
#నిర్వచించు ARG_NOREVERSE	4
#నిర్వచించు ARG_BACKGROUND	5
#నిర్వచించు ARG_FOREGROUND	6
#నిర్వచించు ARG_ICONIC	7
#నిర్వచించు ARG_ROLE	8
#నిర్వచించు ARG_NETBEANS	9
#నిర్వచించు ARG_XRM		10	// ignored
#నిర్వచించు ARG_MENUFONT	11	// ignored
#నిర్వచించు ARG_సూచీ_ముసుగు	0x00ff
#నిర్వచించు ARG_HAS_విలువ	0x0100	// a value is expected after the argument
#నిర్వచించు ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#నిర్వచించు ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#నిర్వచించు ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#నిర్వచించు ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
రకనిర్వచనం నిర్మాణం
{
    స్థిరరాశి అక్షరం	    *name;
    గుర్తులేని పూర్ణాంకం    flags;
}
cmdline_option_T;

స్థిర స్థిరరాశి cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_విలువ},
    {"-font",		ARG_FONT|ARG_HAS_విలువ},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_విలువ},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_విలువ},
    {"-rv",		ARG_తిరగేయి},
    {"-reverse",	ARG_తిరగేయి},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_విలువ},
    {"-background",	ARG_BACKGROUND|ARG_HAS_విలువ},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_విలువ},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_విలువ},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_విలువ},
#నిర్వచితమైతే FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_విలువ},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_విలువ},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_విలువ},	// not implemented
#అయితే_ముగింపు
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_విలువ|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_విలువ|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_విలువ|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_విలువ},
#నిర్వచితమైతే FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_విలువ},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_విలువ},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# ఒకవేళ 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#అయితే_ముగింపు
    {ఖాళీ, 0}
};

స్థిర పూర్ణాంకం    gui_argc = 0;
స్థిర అక్షరం **gui_argv = ఖాళీ;

స్థిర స్థిరరాశి అక్షరం *role_వాదం = ఖాళీ;
#ఒకవేళ defined(USE_GNOME_SESSION)
స్థిర స్థిరరాశి అక్షరం *restart_command = ఖాళీ;
స్థిర       అక్షరం *abs_restart_command = ఖాళీ;
#అయితే_ముగింపు
స్థిర పూర్ణాంకం దొరికింది_iconic_arg = FALSE;

#నిర్వచితమైతే FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
స్థిర పూర్ణాంకం using_gnome = 0;
#లేకపోతే
# define using_gnome 0
#అయితే_ముగింపు

// Comment out the following line to ignore code for resize history tracking.
#నిర్వచించు TRACK_RESIZE_HISTORY
#నిర్వచితమైతే TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# ఒకవేళ 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
రకనిర్వచనం నిర్మాణం resize_history {
    పూర్ణాంకం వాడిన;		// If true, can't match for discard. Only matches once.
    పూర్ణాంకం వెడల్పు;
    పూర్ణాంకం ఎత్తు;
    పూర్ణాంకం from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    పూర్ణాంకం seq;		// for ch_log messages
# endif
    నిర్మాణం resize_history *తదుపరి;
} resize_hist_T;

// never NULL during execution
స్థిర resize_hist_T *latest_resize_hist;
// list of stale resize requests
స్థిర resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
స్థిర పూర్ణాంకం mch_csd_వెడల్పు = 0;
స్థిర పూర్ణాంకం mch_csd_ఎత్తు = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
స్థిర పూర్ణాంకం mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    స్థిర శూన్య
alloc_resize_hist(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు, పూర్ణాంకం from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->వెడల్పు = వెడల్పు;
    new_hist->ఎత్తు = ఎత్తు;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->తదుపరి = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(ఖాళీ, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, వెడల్పు, ఎత్తు, (పూర్ణాంకం)Columns, (పూర్ణాంకం)అడ్డువరుసలు);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    స్థిర శూన్య
clear_resize_hists(శూన్య)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    పూర్ణాంకం		    i = 0;
# endif

    ఒకవేళ (latest_resize_hist)
	latest_resize_hist->వాడిన = TRUE;
    అయితే (old_resize_hists != ఖాళీ)
    {
	resize_hist_T *తదుపరి_hist = old_resize_hists->తదుపరి;

	vim_free(old_resize_hists);
	old_resize_hists = తదుపరి_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ఖాళీ, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define సరిపోలిక_వెడల్పు_ఎత్తు(hist, w, h) \
	  (!hist->వాడిన && hist->వెడల్పు == w && hist->ఎత్తు == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    స్థిర పూర్ణాంకం
సరిపోలిక_stale_వెడల్పు_ఎత్తు(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    resize_hist_T *hist = old_resize_hists;

    కొరకు (hist = old_resize_hists; hist != ఖాళీ; hist = hist->తదుపరి)
	ఒకవేళ (సరిపోలిక_వెడల్పు_ఎత్తు(hist, వెడల్పు, ఎత్తు))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(ఖాళీ, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->వాడిన = TRUE;
	    తిరిగి TRUE;
	}
    తిరిగి FALSE;
}

# ఒకవేళ defined(EXITFREE)
    స్థిర శూన్య
free_all_resize_hist(శూన్య)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#అయితే_ముగింపు

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
స్థిర guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    శూన్య
gui_mch_prepare(పూర్ణాంకం *argc, అక్షరం **argv)
{
    స్థిరరాశి cmdline_option_T  *option;
    పూర్ణాంకం			    i	= 0;
    పూర్ణాంకం			    len = 0;

#ఒకవేళ defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    ఒకవేళ (strchr(argv[0], G_DIR_SEPARATOR) != ఖాళీ)
    {
	char_u buf[MAXPATHL];

	ఒకవేళ (mch_నిండిన_Name((char_u *)argv[0], buf, (పూర్ణాంకం)పరిమాణం(buf), TRUE) == OK)
	{
	    abs_restart_command = (అక్షరం *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#అయితే_ముగింపు

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(అక్షరం *, *argc + 1);

    g_return_if_fail(gui_argv != ఖాళీ);

    gui_argv[gui_argc++] = argv[i++];

    అయితే (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	ఒకవేళ (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    కొనసాగు;
	}

	// Look for argv[i] in cmdline_options[] table.
	కొరకు (option = &cmdline_options[0]; option->name != ఖాళీ; ++option)
	{
	    len = అక్షరశ్రేణి_పొడవు(option->name);

	    ఒకవేళ (అక్షరశ్రేణి_పోలిక(argv[i], option->name, len) == 0)
	    {
		ఒకవేళ (argv[i][len] == '\0')
		    ఆపు;
		// allow --foo=bar style
		ఒకవేళ (argv[i][len] == '=' && (option->flags & ARG_HAS_విలువ))
		    ఆపు;
#నిర్వచితమైతే FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		ఒకవేళ (vim_strchr((char_u *)":=", argv[i][len]) != ఖాళీ
			&& (option->flags & ARG_సూచీ_ముసుగు) == ARG_NETBEANS)
		    ఆపు;
#అయితే_ముగింపు
	    }
	    లేకపోతే ఒకవేళ ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (అక్షరం *)option->name;
		ఆపు;
	    }
	}
	ఒకవేళ (option->name == ఖాళీ) // no match
	{
	    ++i;
	    కొనసాగు;
	}

	ఒకవేళ (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	లేకపోతే
	{
	    అక్షరం *విలువ = ఖాళీ;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    ఒకవేళ (option->flags & ARG_HAS_విలువ)
	    {
		ఒకవేళ (argv[i][len] == '=')
		    విలువ = &argv[i][len + 1];
		లేకపోతే ఒకవేళ (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    విలువ = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    ఎంపిక (option->flags & ARG_సూచీ_ముసుగు)
	    {
		స్థితి ARG_తిరగేయి:
		    దొరికింది_తిరగేయి_arg = TRUE;
		    ఆపు;
		స్థితి ARG_NOREVERSE:
		    దొరికింది_తిరగేయి_arg = FALSE;
		    ఆపు;
		స్థితి ARG_FONT:
		    font_వాదం = విలువ;
		    ఆపు;
		స్థితి ARG_GEOMETRY:
		    ఒకవేళ (విలువ != ఖాళీ)
			gui.geom = vim_strsave((char_u *)విలువ);
		    ఆపు;
		స్థితి ARG_BACKGROUND:
		    background_వాదం = విలువ;
		    ఆపు;
		స్థితి ARG_FOREGROUND:
		    foreground_వాదం = విలువ;
		    ఆపు;
		స్థితి ARG_ICONIC:
		    దొరికింది_iconic_arg = TRUE;
		    ఆపు;
		స్థితి ARG_ROLE:
		    role_వాదం = విలువ; // used later in gui_mch_open()
		    ఆపు;
#నిర్వచితమైతే FEAT_NETBEANS_INTG
		స్థితి ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    ఆపు;
#అయితే_ముగింపు
		అప్రమేయ:
		    ఆపు;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	ఒకవేళ ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	ఒకవేళ (option->flags & ARG_KEEP)
	    ++i;
	లేకపోతే
	{
	    // Remove the flag from the argument vector.
	    ఒకవేళ (--*argc > i)
	    {
		పూర్ణాంకం n_strip = 1;

		// Move the argument's value as well, if there is one.
		ఒకవేళ ((option->flags & ARG_HAS_విలువ)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    ఒకవేళ (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		ఒకవేళ (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * పరిమాణం(అక్షరం *));
	    }
	    argv[*argc] = ఖాళీ;
	}
    }

    gui_argv[gui_argc] = ఖాళీ;
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
gui_mch_free_all(శూన్య)
{
    vim_free(gui_argv);
# ఒకవేళ defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#అయితే_ముగింపు

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    స్థిర gint
visibility_event(GtkWidget *widget వాడని,
		 GdkEventVisibility *event,
		 gpointer దత్తాంశం వాడని)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    ఒకవేళ (gui.పాఠ్యం_gc != ఖాళీ)
	gdk_gc_సమితి_exposures(gui.పాఠ్యం_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    తిరిగి FALSE;
}
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర gboolean
draw_event(GtkWidget *widget వాడని,
	   cairo_t   *cr,
	   gpointer   user_దత్తాంశం వాడని)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ఒకవేళ (gui.starting)
	తిరిగి FALSE;

    బహిర్గతం_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_సమితి_ఆధారం_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    తిరిగి FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# లేకపోతే
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
    స్థిర gboolean
scale_గుణకం_event(GtkWidget *widget,
		   GParamSpec* pspec వాడని,
		   gpointer   user_దత్తాంశం వాడని)
{
    ఒకవేళ (gui.surface != ఖాళీ)
	cairo_surface_destroy(gui.surface);

    పూర్ణాంకం	    w, h;
    gtk_కిటికీ_get_కొలత(GTK_కిటికీ(gui.mainwin), &w, &h);
    gui.surface = gdk_కిటికీ_create_similar_surface(
	    gtk_widget_get_కిటికీ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    పూర్ణాంకం	    usable_ఎత్తు = h;
    ఒకవేళ (gtk_socket_id != 0)
	usable_ఎత్తు -= (gui.char_ఎత్తు - (gui.char_ఎత్తు/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_ఎత్తు);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  ఒకవేళ defined(FEAT_IMAGE) && GTK_CHECK_సంస్కరణ(3,10,0)
    gui.scale = gtk_widget_get_scale_గుణకం(widget);
#  endif

    తిరిగి TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
    స్థిర gint
expose_event(GtkWidget *widget వాడని,
	     GdkEventExpose *event,
	     gpointer దత్తాంశం వాడని)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ఒకవేళ (gui.starting)
	తిరిగి FALSE;

    బహిర్గతం_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.వెడల్పు, event->area.ఎత్తు);

    // Clear the border areas if needed
    ఒకవేళ (event->area.x < FILL_X(0))
	gdk_కిటికీ_clear_area(gui.drawarea->కిటికీ_2, 0, 0, FILL_X(0), 0);
    ఒకవేళ (event->area.y < FILL_Y(0))
	gdk_కిటికీ_clear_area(gui.drawarea->కిటికీ_2, 0, 0, 0, FILL_Y(0));
    ఒకవేళ (event->area.x > FILL_X(Columns))
	gdk_కిటికీ_clear_area(gui.drawarea->కిటికీ_2,
			      FILL_X((పూర్ణాంకం)Columns), 0, 0, 0);
    ఒకవేళ (event->area.y > FILL_Y(అడ్డువరుసలు))
	gdk_కిటికీ_clear_area(gui.drawarea->కిటికీ_2, 0, FILL_Y((పూర్ణాంకం)అడ్డువరుసలు), 0, 0);

    తిరిగి FALSE;
}
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

#నిర్వచితమైతే FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    స్థిర gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (పూర్ణాంకం)GDK_PROPERTY_NEW_విలువ
	    && GDK_కిటికీ_XID(event->కిటికీ_2) == comm_కిటికీ
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.కిటికీ_2 = comm_కిటికీ;
	xev.xproperty.state = Property_New_విలువ;
	serverEventProc(GDK_కిటికీ_XDISPLAY(gtk_widget_get_కిటికీ(widget)),
								      &xev, 0);
    }
    తిరిగి FALSE;
}
#అయితే_ముగింపు // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    స్థిర శూన్య
gtk_settings_xft_dpi_మారింది_cb(GtkSettings *gtk_settings వాడని,
				    GParamSpec *pspec వాడని,
				    gpointer దత్తాంశం వాడని)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    ఒకవేళ (gui.పాఠ్యం_context != ఖాళీ)
	g_object_unref(gui.పాఠ్యం_context);

    gui.పాఠ్యం_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_సమితి_పునాది_dir(gui.పాఠ్యం_context, PANGO_DIRECTION_LTR);

    ఒకవేళ (gui.norm_font != ఖాళీ)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? ఖాళీ : p_guifont, FALSE);
	gui_సమితి_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

రకనిర్వచనం gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
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


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#నిర్వచించు BLINK_NONE  0
#నిర్వచించు BLINK_OFF   1
#నిర్వచించు BLINK_ON    2

స్థిర పూర్ణాంకం blink_state = BLINK_NONE;
స్థిర long_u blink_waittime = 700;
స్థిర long_u blink_ontime = 400;
స్థిర long_u blink_offtime = 250;
స్థిర guint blink_timer = 0;

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

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    శూన్య
gui_mch_stop_blink(పూర్ణాంకం may_call_gui_update_cursor)
{
    ఒకవేళ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ఒకవేళ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

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
    gui_may_flush();

    తిరిగి FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    శూన్య
gui_mch_ప్రారంభం_blink(శూన్య)
{
    ఒకవేళ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    ఒకవేళ (blink_waittime && blink_ontime && blink_offtime && gui.లోపల_focus)
    {
	blink_timer = timeout_కూడు(blink_waittime, blink_cb, ఖాళీ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    స్థిర gint
enter_notify_event(GtkWidget *widget వాడని,
		   Gdk_Event_దాటడం *event వాడని,
		   gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (blink_state == BLINK_NONE)
	gui_mch_ప్రారంభం_blink();

    // make sure keyboard input goes there
    ఒకవేళ (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    తిరిగి FALSE;
}

    స్థిర gint
leave_notify_event(GtkWidget *widget వాడని,
		   Gdk_Event_దాటడం *event వాడని,
		   gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    తిరిగి FALSE;
}

    స్థిర gint
focus_లోపల_event(GtkWidget *widget,
	       GdkEventFocus *event వాడని,
	       gpointer దత్తాంశం వాడని)
{
#నిర్వచితమైతే FEAT_GUI_DIALOG
    ఒకవేళ (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    లేకపోతే
#అయితే_ముగింపు
	gui_focus_change(TRUE);

    ఒకవేళ (blink_state == BLINK_NONE)
	gui_mch_ప్రారంభం_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    ఒకవేళ (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    తిరిగి TRUE;
}

    స్థిర gint
focus_బహిర్గతం_event(GtkWidget *widget వాడని,
		GdkEventFocus *event వాడని,
		gpointer దత్తాంశం వాడని)
{
#నిర్వచితమైతే FEAT_GUI_DIALOG
    ఒకవేళ (gui.dialogs_చురుకైన > 0)
	++gui.dialog_focus_pending;
#అయితే_ముగింపు

    gui_focus_change(FALSE);

    ఒకవేళ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    తిరిగి TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    స్థిర పూర్ణాంకం
keyval_గమ్యం_string(గుర్తులేని పూర్ణాంకం keyval, char_u *string)
{
    పూర్ణాంకం	    len;
    guint32 uc;

    uc = gdk_keyval_గమ్యం_unicode(keyval);
    ఒకవేళ (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((పూర్ణాంకం)uc, string);
    }
    లేకపోతే
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	ఎంపిక (keyval)
	{
	    స్థితి GDK_Tab: స్థితి GDK_KP_Tab: స్థితి GDK_ISO_ఎడమ_Tab:
		string[0] = TAB;
		ఆపు;
	    స్థితి GDK_Linefeed:
		string[0] = NL;
		ఆపు;
	    స్థితి GDK_Return: స్థితి GDK_ISO_Enter: స్థితి GDK_3270_Enter:
		string[0] = CAR;
		ఆపు;
	    స్థితి GDK_Escape:
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
    ఒకవేళ (state & GDK_MOD1_ముసుగు)
	modifiers |= MOD_ముసుగు_ALT;
#ఒకవేళ GTK_CHECK_సంస్కరణ(2,10,0)
    ఒకవేళ (state & GDK_META_ముసుగు)
	modifiers |= MOD_ముసుగు_META;
    ఒకవేళ (state & GDK_SUPER_ముసుగు)
	modifiers |= MOD_ముసుగు_CMD;
#లేకపోతే
    ఒకవేళ (state & GDK_MOD4_ముసుగు)
	modifiers |= MOD_ముసుగు_CMD;
#అయితే_ముగింపు

    తిరిగి modifiers;
}

    స్థిర పూర్ణాంకం
modifiers_gdk2mouse(guint state)
{
    పూర్ణాంకం modifiers = 0;

    ఒకవేళ (state & GDK_జరుపు_ముసుగు)
	modifiers |= MOUSE_జరుపు;
    ఒకవేళ (state & GDK_CONTROL_ముసుగు)
	modifiers |= MOUSE_CTRL;
    ఒకవేళ (state & GDK_MOD1_ముసుగు)
	modifiers |= MOUSE_ALT;

    తిరిగి modifiers;
}

/*
 * Main keyboard handler:
 */
    స్థిర gint
కీలకం_press_event(GtkWidget *widget వాడని,
		Gdk_Event_కీలకం *event,
		gpointer దత్తాంశం వాడని)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	కీలకం_sym;
    పూర్ణాంకం		len;
    పూర్ణాంకం		i;
    పూర్ణాంకం		modifiers;
    పూర్ణాంకం		కీలకం;
    guint	state;
    char_u	*s, *d;
    పూర్ణాంకం		ctrl_ఉపసర్గ_added = 0;

    gui.event_సమయం = event->సమయం;
    కీలకం_sym = event->keyval;
    state = event->state;

#నిర్వచితమైతే FEAT_XIM
    ఒకవేళ (xim_వరుస_కీలకం_press_event(event, TRUE))
	తిరిగి TRUE;
#అయితే_ముగింపు

#నిర్వచితమైతే SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    ఒకవేళ (కీలకం_sym == SunXK_F36 || కీలకం_sym == SunXK_F37)
	len = 0;
    లేకపోతే
#అయితే_ముగింపు
    {
	len = keyval_గమ్యం_string(కీలకం_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	ఒకవేళ (len > 1 && ఆదానం_conv.vc_type != CONV_NONE)
	    len = convert_ఆదానం(string2, len, పరిమాణం(string2));

	s = string2;
	d = string;
	కొరకు (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    ఒకవేళ (d[-1] == CSI && d + 2 < string + పరిమాణం(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (పూర్ణాంకం)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    ఒకవేళ (కీలకం_sym == GDK_ISO_ఎడమ_Tab)
    {
	కీలకం_sym = GDK_Tab;
	state |= GDK_జరుపు_ముసుగు;
    }

#నిర్వచితమైతే FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ఒకవేళ ((state & GDK_MOD1_ముసుగు)
	    && gui.menu_is_చురుకైన
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	తిరిగి FALSE;
#అయితే_ముగింపు

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
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

#నిర్వచితమైతే GDK_కీలకం_dead_circumflex
    // Belgian Ctrl+[ workaround
    ఒకవేళ (len == 0 && కీలకం_sym == GDK_కీలకం_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_ముసుగు_CTRL;
	string[3] = '[';
	len = 4;
	కూడు_గమ్యం_ఆదానం_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	తిరిగి TRUE;
    }
#అయితే_ముగింపు

    ఒకవేళ (len == 0)   // Unrecognized key
	తిరిగి TRUE;

    // For some keys a shift modifier is translated into another key code.
    ఒకవేళ (len == -3)
	కీలకం = గమ్యం_SPECIAL(string[1], string[2]);
    లేకపోతే
    {
	string[len] = NUL;
	కీలకం = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
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
	// Some keys need adjustment when the Ctrl modifier is used.
	కీలకం = may_adjust_కీలకం_for_ctrl(modifiers, కీలకం);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_జరుపు_modifier(modifiers, కీలకం);

	len = mb_char2bytes(కీలకం, string);
    }

    ఒకవేళ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	కూడు_గమ్యం_ఆదానం_buf(string2, 3);
	ఒకవేళ (modifiers == 0x4)
	    ctrl_ఉపసర్గ_added = 1;
    }

    // Check if the key interrupts.
    {
	పూర్ణాంకం int_ch = check_for_అంతరాయం(కీలకం, modifiers);

	ఒకవేళ (int_ch != NUL)
	{
	    trash_ఆదానం_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    ఒకవేళ (ctrl_ఉపసర్గ_added && len == 2
		    && ((పూర్ణాంకం)string[0]) == 195
		    && ((పూర్ణాంకం)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    కూడు_గమ్యం_ఆదానం_buf(string, len);

    // blank out the pointer if necessary
    ఒకవేళ (p_mh)
	gui_mch_mousehide(TRUE);

    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_XIM) || GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర gboolean
కీలకం_release_event(GtkWidget *widget వాడని,
		  Gdk_Event_కీలకం *event వాడని,
		  gpointer దత్తాంశం వాడని)
{
# ఒకవేళ defined(FEAT_XIM)
    gui.event_సమయం = event->సమయం;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    తిరిగి xim_వరుస_కీలకం_press_event(event, FALSE);
# లేకపోతే
    తిరిగి TRUE;
# endif
}
#అయితే_ముగింపు


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
స్థిర పూర్ణాంకం లోపల_selection_clear_event = FALSE;

    స్థిర gint
selection_clear_event(GtkWidget		*widget వాడని,
		      GdkEventSelection	*event,
		      gpointer		user_దత్తాంశం వాడని)
{
    లోపల_selection_clear_event = TRUE;
    ఒకవేళ (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    లేకపోతే
	clip_lose_selection(&clip_నక్షత్రం);
    లోపల_selection_clear_event = FALSE;

    తిరిగి TRUE;
}

#నిర్వచించు RS_NONE	0	// selection_received_cb() not called yet
#నిర్వచించు RS_OK	1	// selection_received_cb() called and OK
#నిర్వచించు RS_FAIL	2	// selection_received_cb() called and failed
స్థిర పూర్ణాంకం received_selection = RS_NONE;

    స్థిర శూన్య
selection_received_cb(GtkWidget		*widget వాడని,
		      Gtk_Selection_దత్తాంశం	*దత్తాంశం,
		      guint		సమయం_2 వాడని,
		      gpointer		user_దత్తాంశం వాడని)
{
    Clipboard_T	    *cbd;
    char_u	    *పాఠ్యం;
    char_u	    *tmpbuf = ఖాళీ;
    guchar	    *tmpbuf_utf8 = ఖాళీ;
    పూర్ణాంకం		    len;
    పూర్ణాంకం		    motion_type = MAUTO;

    ఒకవేళ (gtk_selection_దత్తాంశం_get_selection(దత్తాంశం) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    లేకపోతే
	cbd = &clip_నక్షత్రం;

    పాఠ్యం = (char_u *)gtk_selection_దత్తాంశం_get_దత్తాంశం(దత్తాంశం);
    len = gtk_selection_దత్తాంశం_get_నిడివి(దత్తాంశం);

    ఒకవేళ (పాఠ్యం == ఖాళీ || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	తిరిగి;
    }

    ఒకవేళ (gtk_selection_దత్తాంశం_get_దత్తాంశం_type(దత్తాంశం) == vim_atom
	    || gtk_selection_దత్తాంశం_get_దత్తాంశం_type(దత్తాంశం) == vim_mt_atom)
    {
	motion_type = *పాఠ్యం++;
	--len;
    }
    లేకపోతే ఒకవేళ (gtk_selection_దత్తాంశం_get_దత్తాంశం_type(దత్తాంశం) == vimenc_atom
	    || gtk_selection_దత్తాంశం_get_దత్తాంశం_type(దత్తాంశం) == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;

	motion_type = *పాఠ్యం++;
	--len;

	enc = పాఠ్యం;
	పాఠ్యం += STRLEN(పాఠ్యం) + 1;
	len -= పాఠ్యం - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_అమరిక(&conv, enc, p_enc);
	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, పాఠ్యం, &len);
	    ఒకవేళ (tmpbuf != ఖాళీ)
		పాఠ్యం = tmpbuf;
	    convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    లేకపోతే
    {
	tmpbuf_utf8 = gtk_selection_దత్తాంశం_get_పాఠ్యం(దత్తాంశం);
	ఒకవేళ (tmpbuf_utf8 != ఖాళీ)
	{
	    len = STRLEN(tmpbuf_utf8);
	    ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&ఆదానం_conv, tmpbuf_utf8, &len);
		ఒకవేళ (tmpbuf != ఖాళీ)
		    పాఠ్యం = tmpbuf;
	    }
	    లేకపోతే
		పాఠ్యం = tmpbuf_utf8;
	}
	లేకపోతే ఒకవేళ (len >= 2 && పాఠ్యం[0] == 0xff && పాఠ్యం[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_అమరిక_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    ఒకవేళ (conv.vc_type != CONV_NONE)
	    {
		పాఠ్యం += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, పాఠ్యం, &len);
		convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	    }
	    ఒకవేళ (tmpbuf != ఖాళీ)
		పాఠ్యం = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    అయితే (len > 0 && పాఠ్యం[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, పాఠ్యం, (దీర్ఘ)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    స్థిర శూన్య
selection_get_cb(GtkWidget	    *widget వాడని,
		 Gtk_Selection_దత్తాంశం   *selection_దత్తాంశం,
		 guint		    info,
		 guint		    సమయం_2 వాడని,
		 gpointer	    user_దత్తాంశం వాడని)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    పూర్ణాంకం		    నిడివి;
    పూర్ణాంకం		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    ఒకవేళ (gtk_selection_దత్తాంశం_get_selection(selection_దత్తాంశం)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    లేకపోతే
	cbd = &clip_నక్షత్రం;

    ఒకవేళ (!cbd->owned)
	తిరిగి;			// Shouldn't ever happen

    ఒకవేళ (info != (guint)లక్ష్యం_STRING
	    && (!clip_html || info != (guint)లక్ష్యం_HTML)
	    && info != (guint)లక్ష్యం_UTF8_STRING
	    && info != (guint)లక్ష్యం_VIMENC
	    && info != (guint)లక్ష్యం_VIM
	    && info != (guint)లక్ష్యం_VIMENC_MT
	    && info != (guint)లక్ష్యం_VIM_MT
	    && info != (guint)లక్ష్యం_COMPOUND_పాఠ్యం
	    && info != (guint)లక్ష్యం_పాఠ్యం_PLAIN
	    && info != (guint)లక్ష్యం_పాఠ్యం_PLAIN_UTF8
	    && info != (guint)లక్ష్యం_పాఠ్యం)
	తిరిగి;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    ఒకవేళ (motion_type < 0 || string == ఖాళీ)
	తిరిగి;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    నిడివి = MIN(tmplen, (long_u)(G_MAXINT - 1));

    ఒకవేళ (info == (guint)లక్ష్యం_VIM || info == (guint)లక్ష్యం_VIM_MT)
    {
	tmpbuf = alloc(నిడివి + 1);
	ఒకవేళ (tmpbuf != ఖాళీ)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)నిడివి);
	}
	// For our own format, the first byte contains the motion type
	++నిడివి;
	vim_free(string);
	string = tmpbuf;
	ఒకవేళ (info == (guint)లక్ష్యం_VIM)
	    type = vim_atom;
	లేకపోతే
	    type = vim_mt_atom;
    }

    లేకపోతే ఒకవేళ (info == (guint)లక్ష్యం_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_అమరిక_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	ఒకవేళ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &నిడివి);
	    convert_అమరిక(&conv, ఖాళీ, ఖాళీ);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	ఒకవేళ (string != ఖాళీ)
	{
	    tmpbuf = alloc(నిడివి + 2);
	    ఒకవేళ (tmpbuf != ఖాళీ)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)నిడివి);
		vim_free(string);
		string = tmpbuf;
		నిడివి += 2;
	    }

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_దత్తాంశం->type = selection_దత్తాంశం->లక్ష్యం;
	    selection_దత్తాంశం->format = 16;	// 16 bits per char
#అయితే_ముగింపు
	    gtk_selection_దత్తాంశం_సమితి(selection_దత్తాంశం, html_atom, 16,
							      string, నిడివి);
	    vim_free(string);
	}
	తిరిగి;
    }
    లేకపోతే ఒకవేళ (info == (guint)లక్ష్యం_VIMENC || info == (guint)లక్ష్యం_VIMENC_MT)
    {
	పూర్ణాంకం l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(నిడివి + l + 2);
	ఒకవేళ (tmpbuf != ఖాళీ)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)నిడివి);
	    నిడివి += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	ఒకవేళ (info == (guint)లక్ష్యం_VIMENC)
	    type = vimenc_atom;
	లేకపోతే
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    లేకపోతే
    {
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &నిడివి);
	    vim_free(string);
	    ఒకవేళ (tmpbuf == ఖాళీ)
		తిరిగి;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	ఒకవేళ (g_utf8_validate((స్థిరరాశి అక్షరం *)string, (gssize)నిడివి, ఖాళీ))
	{
	    gtk_selection_దత్తాంశం_సమితి_పాఠ్యం(selection_దత్తాంశం,
					(స్థిరరాశి అక్షరం *)string, నిడివి);
	}
	vim_free(string);
	తిరిగి;
    }

    ఒకవేళ (string != ఖాళీ)
    {
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_దత్తాంశం->type = selection_దత్తాంశం->లక్ష్యం;
	selection_దత్తాంశం->format = 8;	// 8 bits per char
#అయితే_ముగింపు
	gtk_selection_దత్తాంశం_సమితి(selection_దత్తాంశం, type, 8, string, నిడివి);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    పూర్ణాంకం
gui_mch_early_init_check(పూర్ణాంకం give_సందేశం)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    ఒకవేళ ((p == ఖాళీ || *p == NUL) && (q == ఖాళీ || *q == NUL))
    {
	gui.dying = true;
	ఒకవేళ (give_సందేశం)
	    emsg(_((అక్షరం *)e_cannot_open_display));
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    పూర్ణాంకం
gui_mch_init_check(శూన్య)
{
#నిర్వచితమైతే USE_GRESOURCE
    స్థిర పూర్ణాంకం res_registered = FALSE;

    ఒకవేళ (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_రిజిస్టరు_resource();
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GNOME
    ఒకవేళ (gtk_socket_id == 0)
	using_gnome = 1;
#అయితే_ముగింపు

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_సమితి_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    ఒకవేళ (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((అక్షరం *)e_cannot_open_display));
	తిరిగి FAIL;
    }

    తిరిగి OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


స్థిర guint mouse_click_timer = 0;
స్థిర పూర్ణాంకం mouse_timed_బహిర్గతం = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    స్థిర timeout_cb_type
mouse_click_timer_cb(gpointer దత్తాంశం)
{
    // we don't use this information currently
    పూర్ణాంకం *timed_బహిర్గతం = (పూర్ణాంకం *) దత్తాంశం;

    *timed_బహిర్గతం = TRUE;
    తిరిగి FALSE;		// don't happen again
}

స్థిర guint		motion_repeat_timer  = 0;
స్థిర పూర్ణాంకం		motion_repeat_స్థానభ్రంశం = FALSE;
స్థిర timeout_cb_type	motion_repeat_timer_cb(gpointer);

    స్థిర శూన్య
process_motion_notify(పూర్ణాంకం x, పూర్ణాంకం y, GdkModifierType state)
{
    పూర్ణాంకం	    button;
    int_u   vim_modifiers;
    Gtk_కేటాయింపు కేటాయింపు;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_ముసుగు | GDK_BUTTON2_ముసుగు |
		       GDK_BUTTON3_ముసుగు | GDK_BUTTON4_ముసుగు |
		       GDK_BUTTON5_ముసుగు))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    ఒకవేళ (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	తిరిగి;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_కేటాయింపు(gui.drawarea, &కేటాయింపు);

    ఒకవేళ (x < 0 || y < 0
	    || x >= కేటాయింపు.వెడల్పు
	    || y >= కేటాయింపు.ఎత్తు)
    {

	పూర్ణాంకం dx;
	పూర్ణాంకం dy;
	పూర్ణాంకం offshoot;
	పూర్ణాంకం delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - కేటాయింపు.వెడల్పు;
	dy = y < 0 ? -y : y - కేటాయింపు.ఎత్తు;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	ఒకవేళ (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	లేకపోతే
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	ఒకవేళ (!motion_repeat_timer)
	    motion_repeat_timer = timeout_కూడు(delay, motion_repeat_timer_cb,
									 ఖాళీ);
    }
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర GdkDevice *
gui_gtk_get_సూచిక_device(GtkWidget *widget)
{
    Gdk_కిటికీ * స్థిరరాశి win = gtk_widget_get_కిటికీ(widget);
    GdkDisplay * స్థిరరాశి dpy = gdk_కిటికీ_get_display(win);
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,20,0)
    GdkSeat * స్థిరరాశి seat = gdk_display_get_default_seat(dpy);
    తిరిగి gdk_seat_get_సూచిక(seat);
# లేకపోతే
    GdkDeviceManager * స్థిరరాశి mngr = gdk_display_get_device_manager(dpy);
    తిరిగి gdk_device_manager_get_client_సూచిక(mngr);
# endif
}

    స్థిర Gdk_కిటికీ *
gui_gtk_get_సూచిక(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_కిటికీ * స్థిరరాశి win = gtk_widget_get_కిటికీ(widget);
    GdkDevice * స్థిరరాశి dev = gui_gtk_get_సూచిక_device(widget);
    తిరిగి gdk_కిటికీ_get_device_position(win, dev , x, y, state);
}

# ఒకవేళ defined(FEAT_GUI_TABLINE)
    స్థిర Gdk_కిటికీ *
gui_gtk_కిటికీ_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * స్థిరరాశి dev = gui_gtk_get_సూచిక_device(widget);
    తిరిగి gdk_device_get_కిటికీ_at_position(dev, x, y);
}
# endif
#లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_సూచిక(wid, x, y, s) \
    gdk_కిటికీ_get_సూచిక((wid)->కిటికీ_2, x, y, s)
# define gui_gtk_కిటికీ_at_position(wid, x, y)	gdk_కిటికీ_at_సూచిక(x, y)
#అయితే_ముగింపు

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    స్థిర timeout_cb_type
motion_repeat_timer_cb(gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం		    x;
    పూర్ణాంకం		    y;
    GdkModifierType state;

    gui_gtk_get_సూచిక(gui.drawarea, &x, &y, &state);

    ఒకవేళ (!(state & (GDK_BUTTON1_ముసుగు | GDK_BUTTON2_ముసుగు |
		   GDK_BUTTON3_ముసుగు | GDK_BUTTON4_ముసుగు |
		   GDK_BUTTON5_ముసుగు)))
    {
	motion_repeat_timer = 0;
	తిరిగి FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ఒకవేళ (vim_వాడిన_లోపల_ఆదానం_buf() > 10)
	తిరిగి TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    ఒకవేళ (motion_repeat_స్థానభ్రంశం)
	x += gui.char_వెడల్పు;

    motion_repeat_స్థానభ్రంశం = !motion_repeat_స్థానభ్రంశం;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    తిరిగి FALSE;
}

    స్థిర gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (event->is_hint)
    {
	పూర్ణాంకం		x;
	పూర్ణాంకం		y;
	GdkModifierType	state;

	gui_gtk_get_సూచిక(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    లేకపోతే
    {
	process_motion_notify((పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
			      (GdkModifierType)event->state);
    }

    తిరిగి TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    స్థిర gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం button;
    పూర్ణాంకం repeated_click = FALSE;
    పూర్ణాంకం x, y;
    int_u vim_modifiers;

    gui.event_సమయం = event->సమయం;

    // Make sure we have focus now we've been selected
    ఒకవేళ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    ఒకవేళ (event->type != GDK_BUTTON_PRESS)
	తిరిగి FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    ఒకవేళ (!mouse_timed_బహిర్గతం && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_బహిర్గతం = FALSE;
    mouse_click_timer = timeout_కూడు(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_బహిర్గతం);

    ఎంపిక (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	స్థితి 1:
		button = MOUSE_ఎడమ;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_ముసుగు;
		ఆపు;
	స్థితి 2: button = MOUSE_మధ్య; ఆపు;
	స్థితి 3: button = MOUSE_కుడి; ఆపు;
	స్థితి 8: button = MOUSE_X1; ఆపు;
	స్థితి 9: button = MOUSE_X2; ఆపు;
	అప్రమేయ:
	    తిరిగి FALSE;		// Unknown button
    }

#నిర్వచితమైతే FEAT_XIM
    // cancel any preediting
    ఒకవేళ (im_is_preediting())
	xim_reset();
#అయితే_ముగింపు

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    తిరిగి TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    స్థిర gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
    స్థిర ద్విగుణ  acc_x, acc_y;
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,22,0)
    స్థిర guint32 చివరి_smooth_event_సమయం;
# endif
#అయితే_ముగింపు

    ఒకవేళ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    ఎంపిక (event->direction)
    {
	స్థితి GDK_SCROLL_పైకి:
	    button = MOUSE_4;
	    ఆపు;
	స్థితి GDK_SCROLL_కిందికి:
	    button = MOUSE_5;
	    ఆపు;
	స్థితి GDK_SCROLL_ఎడమ:
	    button = MOUSE_7;
	    ఆపు;
	స్థితి GDK_SCROLL_కుడి:
	    button = MOUSE_6;
	    ఆపు;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
	స్థితి GDK_SCROLL_SMOOTH:
	    ఒకవేళ (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		తిరిగి FALSE;
	    }
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,0)
	    ఒకవేళ (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# లేకపోతే
	    ఒకవేళ (event->సమయం - చివరి_smooth_event_సమయం > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,22,0)
	    చివరి_smooth_event_సమయం = event->సమయం;
# endif
	    ఆపు;
#అయితే_ముగింపు
	అప్రమేయ: // This shouldn't happen
	    తిరిగి FALSE;
    }

#నిర్వచితమైతే FEAT_XIM
    // cancel any preediting
    ఒకవేళ (im_is_preediting())
	xim_reset();
#అయితే_ముగింపు

    vim_modifiers = modifiers_gdk2mouse(event->state);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    ఒకవేళ (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	అయితే (acc_x >= 1.0)
	{ //కుడి
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
		    FALSE, vim_modifiers);
	}
	అయితే (acc_x <= -1.0)
	{ //ఎడమ
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
		    FALSE, vim_modifiers);
	}
	అయితే (acc_y >= 1.0)
	{ //కిందికి
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
		    FALSE, vim_modifiers);
	}
	అయితే (acc_y <= -1.0)
	{ //పైకి
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
		    FALSE, vim_modifiers);
	}
    }
    లేకపోతే
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    ఒకవేళ (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    లేకపోతే
#అయితే_ముగింపు
	gui_send_mouse_event(button, (పూర్ణాంకం)event->x, (పూర్ణాంకం)event->y,
		FALSE, vim_modifiers);

    తిరిగి TRUE;
}


    స్థిర gint
button_release_event(GtkWidget *widget వాడని,
		     GdkEventButton *event,
		     gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం x, y;
    int_u vim_modifiers;

    gui.event_సమయం = event->సమయం;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    ఒకవేళ (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    ఎంపిక (event->button)
    {
	స్థితి 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    ఆపు;
    }

    తిరిగి TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    స్థిర శూన్య
grab_notify_event(GtkWidget *widget వాడని,
		  gboolean  was_grabbed,
		  gpointer  దత్తాంశం వాడని)
{
    ఒకవేళ (!was_grabbed)
	dragging_button_state = 0;
}


#నిర్వచితమైతే FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    స్థిర పూర్ణాంకం
లెక్క_and_decode_uri_list(char_u *బహిర్గతం, char_u *raw, పూర్ణాంకం len)
{
    పూర్ణాంకం		i;
    char_u	*p = బహిర్గతం;
    పూర్ణాంకం		లెక్క = 0;

    కొరకు (i = 0; i < len; ++i)
    {
	ఒకవేళ (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    ఒకవేళ (p > బహిర్గతం && p[-1] != NUL)
	    {
		++లెక్క;
		*p++ = NUL;
	    }
	}
	లేకపోతే ఒకవేళ (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	లేకపోతే
	    *p++ = raw[i];
    }
    ఒకవేళ (p > బహిర్గతం && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++లెక్క;
    }
    తిరిగి లెక్క;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    స్థిర పూర్ణాంకం
filter_uri_list(char_u **outlist, పూర్ణాంకం max, char_u *src)
{
    పూర్ణాంకం	i, j;

    కొరకు (i = j = 0; i < max; ++i)
    {
	outlist[i] = ఖాళీ;
	ఒకవేళ (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    ఒకవేళ (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    అయితే (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    తిరిగి j;
}

    స్థిర char_u **
parse_uri_list(పూర్ణాంకం *లెక్క, char_u *దత్తాంశం, పూర్ణాంకం len)
{
    పూర్ణాంకం	    n	    = 0;
    char_u  *tmp    = ఖాళీ;
    char_u  **array = ఖాళీ;

    ఒకవేళ (దత్తాంశం != ఖాళీ && len > 0 && (tmp = alloc(len + 1)) != ఖాళీ)
    {
	n = లెక్క_and_decode_uri_list(tmp, దత్తాంశం, len);
	ఒకవేళ (n > 0 && (array = ALLOC_MULT(char_u *, n)) != ఖాళీ)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *లెక్క = n;
    తిరిగి array;
}

    స్థిర శూన్య
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_దత్తాంశం	*దత్తాంశం,
		     guint		సమయం_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    పూర్ణాంకం	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_దత్తాంశం_get_దత్తాంశం(దత్తాంశం),
	    gtk_selection_దత్తాంశం_get_నిడివి(దత్తాంశం));

    ఒకవేళ (fnames != ఖాళీ && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_ముగింపు(context, TRUE, FALSE, సమయం_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    లేకపోతే
	vim_free(fnames);
}

    స్థిర శూన్య
drag_handle_పాఠ్యం(GdkDragContext	    *context,
		 Gtk_Selection_దత్తాంశం   *దత్తాంశం,
		 guint		    సమయం_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *పాఠ్యం;
    పూర్ణాంకం	    len;
    char_u  *tmpbuf = ఖాళీ;

    పాఠ్యం = (char_u *)gtk_selection_దత్తాంశం_get_దత్తాంశం(దత్తాంశం);
    len = gtk_selection_దత్తాంశం_get_నిడివి(దత్తాంశం);

    ఒకవేళ (gtk_selection_దత్తాంశం_get_దత్తాంశం_type(దత్తాంశం) == utf8_string_atom)
    {
	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&ఆదానం_conv, పాఠ్యం, &len);
	ఒకవేళ (tmpbuf != ఖాళీ)
	    పాఠ్యం = tmpbuf;
    }

    dnd_yank_drag_దత్తాంశం(పాఠ్యం, (దీర్ఘ)len);
    gtk_drag_ముగింపు(context, TRUE, FALSE, సమయం_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    ఒకవేళ (dropkey[2] != 0)
	కూడు_గమ్యం_ఆదానం_buf(dropkey, (పూర్ణాంకం)పరిమాణం(dropkey));
    లేకపోతే
	కూడు_గమ్యం_ఆదానం_buf(dropkey + 3, (పూర్ణాంకం)(పరిమాణం(dropkey) - 3));
}

/*
 * DND receiver.
 */
    స్థిర శూన్య
drag_దత్తాంశం_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_దత్తాంశం	*దత్తాంశం,
		      guint		info,
		      guint		సమయం_2,
		      gpointer		user_దత్తాంశం వాడని)
{
    GdkModifierType state;

    // Guard against trash
    స్థిరరాశి guchar * స్థిరరాశి దత్తాంశం_దత్తాంశం = gtk_selection_దత్తాంశం_get_దత్తాంశం(దత్తాంశం);
    స్థిరరాశి gint దత్తాంశం_నిడివి = gtk_selection_దత్తాంశం_get_నిడివి(దత్తాంశం);
    స్థిరరాశి gint దత్తాంశం_format = gtk_selection_దత్తాంశం_get_format(దత్తాంశం);

    ఒకవేళ (దత్తాంశం_దత్తాంశం == ఖాళీ
	    || దత్తాంశం_నిడివి <= 0
	    || దత్తాంశం_format != 8
	    || దత్తాంశం_దత్తాంశం[దత్తాంశం_నిడివి] != '\0')
    {
	gtk_drag_ముగింపు(context, FALSE, FALSE, సమయం_2);
	తిరిగి;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_సూచిక(widget, ఖాళీ, ఖాళీ, &state);

    // Not sure about the role of "text/plain" here...
    ఒకవేళ (info == (guint)లక్ష్యం_పాఠ్యం_URI_LIST)
	drag_handle_uri_list(context, దత్తాంశం, సమయం_2, state, x, y);
    లేకపోతే
	drag_handle_పాఠ్యం(context, దత్తాంశం, సమయం_2, state);

}
#అయితే_ముగింపు // FEAT_DND


#ఒకవేళ defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    స్థిర శూన్య
sm_client_check_మారింది_any(GnomeClient	    *client వాడని,
			    gint	    కీలకం,
			    GnomeDialogType type వాడని,
			    gpointer	    దత్తాంశం వాడని)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ఒకవేళ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_మారింది_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    బహిర్గతం_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_కీలకం_return(కీలకం, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    స్థిర gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase వాడని,
			GnomeSaveStyle	    save_style వాడని,
			gboolean	    shutdown వాడని,
			GnomeInteractStyle  interact_style,
			gboolean	    fast వాడని,
			gpointer	    దత్తాంశం వాడని)
{
    స్థిర స్థిరరాశి అక్షరం	suffix[] = "-session.vim";
    అక్షరం		*session_file;
    గుర్తులేని పూర్ణాంకం	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    ఒకవేళ (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_సాధారణ,
					 &sm_client_check_మారింది_any,
					 ఖాళీ);
    బహిర్గతం_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_మార్గం(
			gnome_client_get_config_ఉపసర్గ(client));
    len = అక్షరశ్రేణి_పొడవు(session_file);

    ఒకవేళ (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(అక్షరం, session_file, len + పరిమాణం(suffix));
    జ్ఞాపకం_నకలు(session_file + len, suffix, పరిమాణం(suffix));

    success = write_session_file((char_u *)session_file);

    ఒకవేళ (success)
    {
	స్థిరరాశి అక్షరం  *argv[8];
	పూర్ణాంకం	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = ఖాళీ;

	gnome_client_సమితి_discard_command(client, i, (అక్షరం **)argv);

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
	argv[i++] = gtk_కిటికీ_get_role(GTK_కిటికీ(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = ఖాళీ;

	gnome_client_సమితి_restart_command(client, i, (అక్షరం **)argv);
	gnome_client_సమితి_clone_command(client, 0, ఖాళీ);
    }

    g_free(session_file);

    తిరిగి success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    స్థిర శూన్య
sm_client_die(GnomeClient *client వాడని, gpointer దత్తాంశం వాడని)
{
    // Don't write messages to the GUI anymore
    నిండిన_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    స్థిర శూన్య
అమరిక_save_yourself(శూన్య)
{
    GnomeClient *client;

    client = gnome_master_client();
    ఒకవేళ (client == ఖాళీ)
	తిరిగి;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), ఖాళీ);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), ఖాళీ);
}

#లేకపోతే // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    స్థిర gboolean
local_xsmp_handle_అభ్యర్థనలు(
    GIOChannel		*ఆధారం వాడని,
    GIOCondition	condition,
    gpointer		దత్తాంశం)
{
    ఒకవేళ (condition == G_IO_లోపల)
    {
	// Do stuff; maybe close connection
	ఒకవేళ (xsmp_handle_అభ్యర్థనలు() == FAIL)
	    g_io_channel_unref((GIOChannel *)దత్తాంశం);
	తిరిగి TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)దత్తాంశం);
    xsmp_close();
    తిరిగి TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    స్థిర శూన్య
అమరిక_save_yourself(శూన్య)
{
    Atom    *existing_atoms = ఖాళీ;
    పూర్ణాంకం	    లెక్క = 0;

# ifdef USE_XSMP
    ఒకవేళ (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_కూడు_watch(g_io, G_IO_లోపల | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_అభ్యర్థనలు, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    లేకపోతే
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	ఒకవేళ (!dpy)
	    తిరిగి;

	Gdk_కిటికీ * స్థిరరాశి mainwin_win = gtk_widget_get_కిటికీ(gui.mainwin);
	ఒకవేళ (XGetWMProtocols(dpy, GDK_కిటికీ_XID(mainwin_win),
		    &existing_atoms, &లెక్క))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    పూర్ణాంకం	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    కొరకు (i = 0; i < లెక్క; ++i)
		ఒకవేళ (existing_atoms[i] == save_yourself_xatom)
		    ఆపు;

	    ఒకవేళ (i == లెక్క)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, లెక్క + 1);
		ఒకవేళ (new_atoms != ఖాళీ)
		{
		    జ్ఞాపకం_నకలు(new_atoms, existing_atoms, లెక్క * పరిమాణం(Atom));
		    new_atoms[లెక్క] = save_yourself_xatom;
		    XSetWMProtocols(GDK_కిటికీ_XDISPLAY(mainwin_win),
			    GDK_కిటికీ_XID(mainwin_win),
			    new_atoms, లెక్క + 1);
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
    స్థిర GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event వాడని,
		    gpointer దత్తాంశం వాడని)
{
    XEvent *xevent = (XEvent *)xev;

    ఒకవేళ (xevent != ఖాళీ
	    && xevent->type == Client_సందేశం
	    && xevent->xclient.సందేశం_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.దత్తాంశం.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	బహిర్గతం_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_కిటికీ_XDISPLAY(gtk_widget_get_కిటికీ(gui.mainwin)),
		    GDK_కిటికీ_XID(gtk_widget_get_కిటికీ(gui.mainwin)),
		    ఖాళీ, 0);
	తిరిగి GDK_FILTER_REMOVE;
    }

    తిరిగి GDK_FILTER_CONTINUE;
}
#అయితే_ముగింపు // !USE_GNOME_SESSION

    స్థిర GdkPixbuf *
pixbuf_new_from_png_దత్తాంశం(స్థిరరాశి గుర్తులేని అక్షరం *దత్తాంశం, గుర్తులేని పూర్ణాంకం len)
{
    GInput_ప్రవాహం *ప్రవాహం;
    GdkPixbuf    *pixbuf;

    ప్రవాహం = g_జ్ఞాపకం_ఆదానం_ప్రవాహం_new_from_దత్తాంశం(దత్తాంశం, len, ఖాళీ);
    pixbuf = gdk_pixbuf_new_from_ప్రవాహం(ప్రవాహం, ఖాళీ, ఖాళీ);
    g_object_unref(ప్రవాహం);
    తిరిగి pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    స్థిర శూన్య
mainwin_realize(GtkWidget *widget వాడని, gpointer దత్తాంశం వాడని)
{
#అన్తర్భావ "../runtime/vim16x16_png.h"
#అన్తర్భావ "../runtime/vim32x32_png.h"
#అన్తర్భావ "../runtime/vim48x48_png.h"

    Gdk_కిటికీ * స్థిరరాశి mainwin_win = gtk_widget_get_కిటికీ(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    ఒకవేళ (echo_wid_arg)
    {
	ఒకవేళ (gui_mch_get_display())
	    రాయి("WID: %ld\n", (దీర్ఘ)GDK_కిటికీ_XID(mainwin_win));
	లేకపోతే
	    రాయి("WID: 0\n");
	fflush(stdout);
    }

    ఒకవేళ (vim_strchr(p_go, GO_ICON) != ఖాళీ)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	ఒకవేళ (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_కిటికీ_సమితి_icon_name(GTK_కిటికీ(gui.mainwin), "gvim");
	లేకపోతే
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = ఖాళీ;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_దత్తాంశం(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_దత్తాంశం(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_దత్తాంశం(vim48x48_png, vim48x48_png_len));

	    gtk_కిటికీ_సమితి_icon_list(GTK_కిటికీ(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(శూన్య *)&g_object_unref, ఖాళీ);
	    g_list_free(icons);
	}
    }

#ఒకవేళ !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_కిటికీ_కూడు_filter(ఖాళీ, &global_event_filter, ఖాళీ);
#అయితే_ముగింపు
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#ఒకవేళ defined(USE_GNOME_SESSION)
    ఒకవేళ (using_gnome)
#అయితే_ముగింపు
	అమరిక_save_yourself();

#నిర్వచితమైతే FEAT_CLIENTSERVER
    ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 && gui_mch_get_display())
    {
	ఒకవేళ (serverName == ఖాళీ && server_Delayed_ప్రారంభం_Name != ఖాళీ)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_కిటికీ = GDK_కిటికీ_XID(mainwin_win);

	    (శూన్య)server_రిజిస్టరు_Name(GDK_కిటికీ_XDISPLAY(mainwin_win),
				    server_Delayed_ప్రారంభం_Name);
	}
	లేకపోతే
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_కిటికీ(GDK_కిటికీ_XDISPLAY(mainwin_win),
					GDK_కిటికీ_XID(mainwin_win));
	}
	gtk_widget_కూడు_events(gui.mainwin, GDK_PROPERTY_CHANGE_ముసుగు);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), ఖాళీ);
    }
#అయితే_ముగింపు
}

    స్థిర GdkCursor *
create_blank_సూచిక(శూన్య)
{
    GdkCursor	*cursor;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#లేకపోతే
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_కిటికీ	*మూలం_కిటికీ = ఖాళీ;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_ముసుగు;
    అక్షరం	blank_దత్తాంశం[] = { 0x0 };
    మూలం_కిటికీ = gtk_widget_get_మూలం_కిటికీ(gui.mainwin);
    blank_ముసుగు = gdk_bitmap_create_from_దత్తాంశం(మూలం_కిటికీ, blank_దత్తాంశం, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_ముసుగు, blank_ముసుగు,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_ముసుగు);
#అయితే_ముగింపు

    తిరిగి cursor;
}

    స్థిర శూన్య
mainwin_screen_మారింది_cb(GtkWidget  *widget,
			  GdkScreen  *మునుపటి_screen వాడని,
			  gpointer   దత్తాంశం వాడని)
{
    ఒకవేళ (!gtk_widget_has_screen(widget))
	తిరిగి;

    /*
     * Recreate the invisible mouse cursor.
     */
    ఒకవేళ (gui.blank_సూచిక != ఖాళీ)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_సూచిక));
#లేకపోతే
	gdk_cursor_unref(gui.blank_సూచిక);
#అయితే_ముగింపు

    gui.blank_సూచిక = create_blank_సూచిక();

    ఒకవేళ (gui.సూచిక_hidden && gtk_widget_get_కిటికీ(gui.drawarea) != ఖాళీ)
	gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(gui.drawarea),
		gui.blank_సూచిక);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    ఒకవేళ (gui.పాఠ్యం_context != ఖాళీ)
	g_object_unref(gui.పాఠ్యం_context);

    gui.పాఠ్యం_context = gtk_widget_create_pango_context(widget);
    pango_context_సమితి_పునాది_dir(gui.పాఠ్యం_context, PANGO_DIRECTION_LTR);

    ఒకవేళ (gui.norm_font != ఖాళీ)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_సమితి_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    స్థిర gboolean
mainwin_state_event_cb(GtkWidget *widget వాడని,
		       Gdk_Event_కిటికీ_State *event,
		       gpointer user_దత్తాంశం వాడని)
{
    ఒకవేళ (event->new_కిటికీ_state & GDK_కిటికీ_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    తిరిగి FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    స్థిర శూన్య
drawarea_realize_cb(GtkWidget *widget, gpointer దత్తాంశం వాడని)
{
    GtkWidget *sbar;
    Gtk_కేటాయింపు కేటాయింపు;

#నిర్వచితమైతే FEAT_XIM
    xim_init();
#అయితే_ముగింపు
    gui_mch_new_colors();
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gui.surface = gdk_కిటికీ_create_similar_surface(
	    gtk_widget_get_కిటికీ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_వెడల్పు(widget),
	    gtk_widget_get_allocated_ఎత్తు(widget));
#లేకపోతే
    gui.పాఠ్యం_gc = gdk_gc_new(gui.drawarea->కిటికీ_2);
#అయితే_ముగింపు

    gui.blank_సూచిక = create_blank_సూచిక();
    ఒకవేళ (gui.సూచిక_hidden)
	gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(widget), gui.blank_సూచిక);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_ఎడమ].id;
    ఒకవేళ (!sbar || (!gui.which_scrollbars[SBAR_ఎడమ]
				    && firstwin->w_scrollbars[SBAR_కుడి].id))
	sbar = firstwin->w_scrollbars[SBAR_కుడి].id;
    gtk_widget_get_కేటాయింపు(sbar, &కేటాయింపు);
    ఒకవేళ (sbar && gtk_widget_get_realized(sbar) && కేటాయింపు.వెడల్పు)
	gui.scrollbar_వెడల్పు = కేటాయింపు.వెడల్పు;

    sbar = gui.bottom_sbar.id;
    ఒకవేళ (sbar && gtk_widget_get_realized(sbar) && కేటాయింపు.ఎత్తు)
	gui.scrollbar_ఎత్తు = కేటాయింపు.ఎత్తు;
}

/*
 * Properly clean up on shutdown.
 */
    స్థిర శూన్య
drawarea_unrealize_cb(GtkWidget *widget వాడని, gpointer దత్తాంశం వాడని)
{
    // Don't write messages to the GUI anymore
    నిండిన_screen = FALSE;

#నిర్వచితమైతే FEAT_XIM
    im_shutdown();
#అయితే_ముగింపు
    ఒకవేళ (gui.ascii_glyphs != ఖాళీ)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = ఖాళీ;
    }
    ఒకవేళ (gui.ascii_font != ఖాళీ)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = ఖాళీ;
    }
    g_object_unref(gui.పాఠ్యం_context);
    gui.పాఠ్యం_context = ఖాళీ;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ఒకవేళ (gui.surface != ఖాళీ)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = ఖాళీ;
    }
#లేకపోతే
    g_object_unref(gui.పాఠ్యం_gc);
    gui.పాఠ్యం_gc = ఖాళీ;
#అయితే_ముగింపు

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_సూచిక));
#లేకపోతే
    gdk_cursor_unref(gui.blank_సూచిక);
#అయితే_ముగింపు
    gui.blank_సూచిక = ఖాళీ;
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    స్థిర శూన్య
drawarea_style_updated_cb(GtkWidget *widget వాడని,
			 gpointer దత్తాంశం వాడని)
#లేకపోతే
    స్థిర శూన్య
drawarea_style_సమితి_cb(GtkWidget	*widget వాడని,
		      GtkStyle	*మునుపటి_style వాడని,
		      gpointer	దత్తాంశం వాడని)
#అయితే_ముగింపు
{
    gui_mch_new_colors();
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       దత్తాంశం వాడని)
{
    స్థిర పూర్ణాంకం cur_వెడల్పు = 0;
    స్థిర పూర్ణాంకం cur_ఎత్తు = 0;

    g_return_val_if_fail(event
	    && event->వెడల్పు >= 1 && event->ఎత్తు >= 1, TRUE);

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2) && !GTK_CHECK_సంస్కరణ(3,22,4)
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
    ఒకవేళ (event->send_event == FALSE)
	తిరిగి TRUE;
# endif

    ఒకవేళ (event->వెడల్పు == cur_వెడల్పు && event->ఎత్తు == cur_ఎత్తు)
	తిరిగి TRUE;

    cur_వెడల్పు = event->వెడల్పు;
    cur_ఎత్తు = event->ఎత్తు;

    ఒకవేళ (gui.surface != ఖాళీ)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_కిటికీ_create_similar_surface(
	    gtk_widget_get_కిటికీ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->వెడల్పు, event->ఎత్తు);

    gtk_widget_వరుస_draw(widget);

    తిరిగి TRUE;
}
#అయితే_ముగింపు

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    స్థిర gint
delete_event_cb(GtkWidget *widget వాడని,
		GdkEventAny *event వాడని,
		gpointer దత్తాంశం వాడని)
{
    gui_shell_closed();
    తిరిగి TRUE;
}

#ఒకవేళ defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    స్థిర పూర్ణాంకం
get_అంశం_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation అంశం_orientation = GTK_ORIENTATION_HORIZONTAL;

    ఒకవేళ (using_gnome && widget != ఖాళీ)
    {
	GtkWidget *మాతృ;
	Bonobo_Dock_అంశం *dockitem;

	మాతృ	 = gtk_widget_get_మాతృ(widget);
	ఒకవేళ (G_TYPE_FROM_INSTANCE(మాతృ) == BONOBO_TYPE_DOCK_అంశం)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = మాతృ;
	    dockitem = BONOBO_DOCK_అంశం(widget);

	    ఒకవేళ (dockitem == ఖాళీ || dockitem->is_floating)
		తిరిగి 0;
	    అంశం_orientation = bonobo_dock_అంశం_get_orientation(dockitem);
	}
    }
# లేకపోతే
#  define అంశం_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    ఒకవేళ (widget != ఖాళీ
	    && అంశం_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_కేటాయింపు కేటాయింపు;

	gtk_widget_get_కేటాయింపు(widget, &కేటాయింపు);
	ఒకవేళ (కేటాయింపు.ఎత్తు > 1)
	    తిరిగి కేటాయింపు.ఎత్తు;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_ఎత్తు(widget, &min_h, &natural_h);
	    తిరిగి MAX(min_h, natural_h);
	}
#  లేకపోతే
	తిరిగి కేటాయింపు.ఎత్తు;
#  endif
# లేకపోతే
	ఒకవేళ (orientation == GTK_ORIENTATION_HORIZONTAL)
	    తిరిగి widget->కేటాయింపు.ఎత్తు;
	లేకపోతే
	    తిరిగి widget->కేటాయింపు.వెడల్పు;
# endif
    }
    తిరిగి 0;
}
#అయితే_ముగింపు

    స్థిర పూర్ణాంకం
get_menu_tool_వెడల్పు(శూన్య)
{
    పూర్ణాంకం వెడల్పు = 0;

#నిర్వచితమైతే FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    వెడల్పు += get_అంశం_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    వెడల్పు += get_అంశం_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    ఒకవేళ (gui.tabline != ఖాళీ)
	వెడల్పు += get_అంశం_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#అయితే_ముగింపు

    తిరిగి వెడల్పు;
}

    స్థిర పూర్ణాంకం
get_menu_tool_ఎత్తు(శూన్య)
{
    పూర్ణాంకం ఎత్తు = 0;

#నిర్వచితమైతే FEAT_MENU
    ఎత్తు += get_అంశం_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_TOOLBAR
    ఎత్తు += get_అంశం_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    ఒకవేళ (gui.tabline != ఖాళీ
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	ఎత్తు += get_అంశం_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#అయితే_ముగింపు

    తిరిగి ఎత్తు;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
స్థిర పూర్ణాంకం init_కిటికీ_hints_state = 0;

    స్థిర శూన్య
update_కిటికీ_manager_hints(పూర్ణాంకం force_వెడల్పు, పూర్ణాంకం force_ఎత్తు)
{
    స్థిర పూర్ణాంకం old_వెడల్పు  = 0;
    స్థిర పూర్ణాంకం old_ఎత్తు = 0;
    స్థిర పూర్ణాంకం old_min_వెడల్పు  = 0;
    స్థిర పూర్ణాంకం old_min_ఎత్తు = 0;
    స్థిర పూర్ణాంకం old_char_వెడల్పు  = 0;
    స్థిర పూర్ణాంకం old_char_ఎత్తు = 0;

    పూర్ణాంకం వెడల్పు;
    పూర్ణాంకం ఎత్తు;
    పూర్ణాంకం min_వెడల్పు;
    పూర్ణాంకం min_ఎత్తు;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    ఒకవేళ (!(force_వెడల్పు && force_ఎత్తు)  &&  init_కిటికీ_hints_state > 0)
    {
	// Don't do it!
	init_కిటికీ_hints_state = 2;
	తిరిగి;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    వెడల్పు  = gui_get_పునాది_వెడల్పు();
    ఎత్తు = gui_get_పునాది_ఎత్తు();
#నిర్వచితమైతే FEAT_MENU
    ఎత్తు += tabline_ఎత్తు() * gui.char_ఎత్తు;
#అయితే_ముగింపు
    వెడల్పు  += get_menu_tool_వెడల్పు();
    ఎత్తు += get_menu_tool_ఎత్తు();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    ఒకవేళ (force_వెడల్పు && force_ఎత్తు)
    {
	min_వెడల్పు  = force_వెడల్పు;
	min_ఎత్తు = force_ఎత్తు;
    }
    లేకపోతే
    {
	min_వెడల్పు  = వెడల్పు  + MIN_COLUMNS * gui.char_వెడల్పు;
	min_ఎత్తు = ఎత్తు + MIN_పంక్తులు   * gui.char_ఎత్తు;
    }

    // Avoid an expose event when the size didn't change.
    ఒకవేళ (వెడల్పు != old_వెడల్పు
	    || ఎత్తు != old_ఎత్తు
	    || min_వెడల్పు != old_min_వెడల్పు
	    || min_ఎత్తు != old_min_ఎత్తు
	    || gui.char_వెడల్పు != old_char_వెడల్పు
	    || gui.char_ఎత్తు != old_char_ఎత్తు)
    {
	GdkGeometry	geometry;
	Gdk_కిటికీ_Hints	geometry_ముసుగు;

	geometry.వెడల్పు_inc   = gui.char_వెడల్పు;
	geometry.ఎత్తు_inc  = gui.char_ఎత్తు;
	geometry.పునాది_వెడల్పు  = వెడల్పు;
	geometry.పునాది_ఎత్తు = ఎత్తు;
	geometry.min_వెడల్పు   = min_వెడల్పు;
	geometry.min_ఎత్తు  = min_ఎత్తు;
	geometry_ముసుగు	     = GDK_HINT_పునాది_కొలత|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_కొలత;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_కిటికీ_సమితి_geometry_hints(GTK_కిటికీ(gui.mainwin), ఖాళీ,
				      &geometry, geometry_ముసుగు);
	old_వెడల్పు       = వెడల్పు;
	old_ఎత్తు      = ఎత్తు;
	old_min_వెడల్పు   = min_వెడల్పు;
	old_min_ఎత్తు  = min_ఎత్తు;
	old_char_వెడల్పు  = gui.char_వెడల్పు;
	old_char_ఎత్తు = gui.char_ఎత్తు;
    }
}

#ఒకవేళ defined(FEAT_GUI_DARKTHEME)
    శూన్య
gui_mch_సమితి_dark_theme(పూర్ణాంకం dark)
{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_సమితి(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, ఖాళీ);
# endif
}
#అయితే_ముగింపు // FEAT_GUI_DARKTHEME

#నిర్వచితమైతే FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    స్థిర శూన్య
icon_కొలత_మారింది_foreach(GtkWidget *widget, gpointer user_దత్తాంశం)
{
    ఒకవేళ (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
	ఒకవేళ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    స్థిరరాశి Gtk_Icon_కొలత icon_కొలత = GPOINTER_గమ్యం_INT(user_దత్తాంశం);
	    స్థిరరాశి gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, ఖాళీ);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_కొలత);
	}
# లేకపోతే
	// User-defined icons are stored in a GtkIconSet
	ఒకవేళ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_సమితి)
	{
	    Gtk_Icon_సమితి	*icon_సమితి;
	    Gtk_Icon_కొలత	icon_కొలత;

	    gtk_image_get_icon_సమితి(image, &icon_సమితి, &icon_కొలత);
	    icon_కొలత = (Gtk_Icon_కొలత)(దీర్ఘ)user_దత్తాంశం;

	    gtk_icon_సమితి_ref(icon_సమితి);
	    gtk_image_సమితి_from_icon_సమితి(image, icon_సమితి, icon_కొలత);
	    gtk_icon_సమితి_unref(icon_సమితి);
	}
# endif
    }
    లేకపోతే ఒకవేళ (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_కొలత_మారింది_foreach,
			      user_దత్తాంశం);
    }
}

    స్థిర శూన్య
సమితి_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_కొలత	    కొలత;
    Gtk_Icon_కొలత	    oldsize;

    ఒకవేళ ((toolbar_flags & (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    లేకపోతే ఒకవేళ ((toolbar_flags & (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS))
		      == (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    లేకపోతే ఒకవేళ (toolbar_flags & TOOLBAR_పాఠ్యం)
	style = GTK_TOOLBAR_పాఠ్యం;
    లేకపోతే
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_సమితి_style(toolbar, style);
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_toolbar_సమితి_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    ఎంపిక (tbis_flags)
    {
	స్థితి TBIS_TINY:	    కొలత = GTK_ICON_కొలత_MENU;		ఆపు;
	స్థితి TBIS_SMALL:    కొలత = GTK_ICON_కొలత_SMALL_TOOLBAR;	ఆపు;
	స్థితి TBIS_MEDIUM:   కొలత = GTK_ICON_కొలత_BUTTON;	ఆపు;
	స్థితి TBIS_LARGE:    కొలత = GTK_ICON_కొలత_LARGE_TOOLBAR;	ఆపు;
	స్థితి TBIS_HUGE:     కొలత = GTK_ICON_కొలత_DND;		ఆపు;
	స్థితి TBIS_GIANT:    కొలత = GTK_ICON_కొలత_DIALOG;	ఆపు;
	అప్రమేయ:	    కొలత = GTK_ICON_కొలత_INVALID;	ఆపు;
    }
    oldsize = gtk_toolbar_get_icon_కొలత(toolbar);

    ఒకవేళ (కొలత == GTK_ICON_కొలత_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_కొలత(toolbar);
	కొలత = gtk_toolbar_get_icon_కొలత(toolbar);
    }
    ఒకవేళ (కొలత != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_కొలత_మారింది_foreach,
			      GINT_గమ్యం_సూచిక((పూర్ణాంకం)కొలత));
    }
    gtk_toolbar_సమితి_icon_కొలత(toolbar, కొలత);
}

#అయితే_ముగింపు // FEAT_TOOLBAR

#ఒకవేళ defined(FEAT_GUI_TABLINE)
స్థిర పూర్ణాంకం ignore_tabline_evt = FALSE;
స్థిర GtkWidget *tabline_menu;
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర GtkTooltips *tabline_tooltip;
# endif
స్థిర పూర్ణాంకం clicked_పుట;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    స్థిర శూన్య
tabline_menu_handler(Gtk_Menu_అంశం *అంశం వాడని, gpointer user_దత్తాంశం)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_పుట, (పూర్ణాంకం)(దీర్ఘ)user_దత్తాంశం);
}

    స్థిర శూన్య
కూడు_tabline_menu_అంశం(GtkWidget *menu, char_u *పాఠ్యం, పూర్ణాంకం resp)
{
    GtkWidget	*అంశం;
    char_u	*utf_పాఠ్యం;

    utf_పాఠ్యం = CONVERT_గమ్యం_UTF8(పాఠ్యం);
    అంశం = gtk_menu_అంశం_new_with_label((స్థిరరాశి అక్షరం *)utf_పాఠ్యం);
    gtk_widget_show(అంశం);
    CONVERT_గమ్యం_UTF8_FREE(utf_పాఠ్యం);

    gtk_container_కూడు(GTK_CONTAINER(menu), అంశం);
    g_signal_connect(G_OBJECT(అంశం), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_గమ్యం_సూచిక(resp));
}

/*
 * Create a menu for the tab line.
 */
    స్థిర GtkWidget *
create_tabline_menu(శూన్య)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    కూడు_tabline_menu_అంశం(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    కూడు_tabline_menu_అంశం(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    కూడు_tabline_menu_అంశం(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    తిరిగి menu;
}

    స్థిర gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    ఒకవేళ (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	పూర్ణాంకం		x = bevent->x;
	పూర్ణాంకం		y = bevent->y;
	GtkWidget	*tabwidget;
	Gdk_కిటికీ	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	ఒకవేళ (hold_gui_events || cmdwin_type != 0)
	    తిరిగి TRUE;

	tabwin = gui_gtk_కిటికీ_at_position(gui.mainwin, &x, &y);

	gdk_కిటికీ_get_user_దత్తాంశం(tabwin, (gpointer)&tabwidget);
	clicked_పుట = GPOINTER_గమ్యం_INT(g_object_get_దత్తాంశం(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	ఒకవేళ (bevent->button == 3)
	{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    ఒకవేళ (gui.is_wayland)
	    {
		పూర్ణాంకం x2, y2;
		gui_gtk_get_సూచిక(gui.mainwin, &x2, &y2, ఖాళీ);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_కిటికీ(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, ఖాళీ);
	    }
	    లేకపోతే
#  endif
	    gtk_menu_popup_at_సూచిక(GTK_MENU(widget), event);
# లేకపోతే
	    gtk_menu_popup(GTK_MENU(widget), ఖాళీ, ఖాళీ, ఖాళీ, ఖాళీ,
						bevent->button, bevent->సమయం);
# endif
	    // We handled the event.
	    తిరిగి TRUE;
	}
	లేకపోతే ఒకవేళ (bevent->button == 1)
	{
	    ఒకవేళ (clicked_పుట == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	లేకపోతే ఒకవేళ (bevent->button == 2)
	{
	    ఒకవేళ (clicked_పుట != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_పుట, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    తిరిగి FALSE;
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

# ఒకవేళ GTK_CHECK_సంస్కరణ(2,10,0)
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
# endif

/*
 * Show or hide the tabline.
 */
    శూన్య
gui_mch_show_tabline(పూర్ణాంకం showit)
{
    ఒకవేళ (gui.tabline == ఖాళీ)
	తిరిగి;

    ఒకవేళ (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    పూర్ణాంకం w, h;

	    gtk_కిటికీ_get_కొలత(GTK_కిటికీ(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_సమితి_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_కిటికీ_manager_hints(0, 0);
	ఒకవేళ (showit)
	    gtk_widget_సమితి_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    పూర్ణాంకం
gui_mch_showing_tabline(శూన్య)
{
    తిరిగి gui.tabline != ఖాళీ
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    శూన్య
gui_mch_update_tabline(శూన్య)
{
    GtkWidget	    *పుట;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    పూర్ణాంకం		    nr = 0;
    పూర్ణాంకం		    tab_అంకె;
    పూర్ణాంకం		    curtabidx = 0;
    char_u	    *labeltext;

    ఒకవేళ (gui.tabline == ఖాళీ)
	తిరిగి;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    కొరకు (tp = మొదటి_tabpage; tp != ఖాళీ; tp = tp->tp_తదుపరి, ++nr)
    {
	ఒకవేళ (tp == curtab)
	    curtabidx = nr;

	tab_అంకె = nr + 1;

	పుట = gtk_notebook_get_nth_పుట(GTK_NOTEBOOK(gui.tabline), nr);
	ఒకవేళ (పుట == ఖాళీ)
	{
	    // Add notebook page
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	    పుట = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_సమితి_homogeneous(GTK_BOX(పుట), FALSE);
# లేకపోతే
	    పుట = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(పుట);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,14,0)
	    gtk_misc_సమితి_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_కూడు(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_చొప్పించు_పుట(GTK_NOTEBOOK(gui.tabline),
		    పుట,
		    event_box,
		    nr);
# ఒకవేళ GTK_CHECK_సంస్కరణ(2,10,0)
	    gtk_notebook_సమితి_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    పుట,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), పుట);
	g_object_సమితి_దత్తాంశం(G_OBJECT(event_box), "tab_num",
						     GINT_గమ్యం_సూచిక(tab_అంకె));
	label = gtk_bin_get_బిడ్డ(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_గమ్యం_UTF8(NameBuff);
	gtk_label_సమితి_పాఠ్యం(GTK_LABEL(label), (స్థిరరాశి అక్షరం *)labeltext);
	CONVERT_గమ్యం_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_గమ్యం_UTF8(NameBuff);
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	gtk_widget_సమితి_tooltip_పాఠ్యం(event_box, (స్థిరరాశి gchar *)labeltext);
# లేకపోతే
	gtk_tooltips_సమితి_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (స్థిరరాశి అక్షరం *)labeltext, ఖాళీ);
# endif
	CONVERT_గమ్యం_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    అయితే (gtk_notebook_get_nth_పుట(GTK_NOTEBOOK(gui.tabline), nr) != ఖాళీ)
	gtk_notebook_remove_పుట(GTK_NOTEBOOK(gui.tabline), nr);

    ఒకవేళ (gtk_notebook_get_current_పుట(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_సమితి_current_పుట(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    శూన్య
gui_mch_సమితి_curtab(పూర్ణాంకం nr)
{
    ఒకవేళ (gui.tabline == ఖాళీ)
	తిరిగి;

    ignore_tabline_evt = TRUE;
    ఒకవేళ (gtk_notebook_get_current_పుట(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_సమితి_current_పుట(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#అయితే_ముగింపు // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    శూన్య
gui_gtk_సమితి_selection_targets(GdkAtom selection)
{
    పూర్ణాంకం		    i, j = 0;
    స్థిర పూర్ణాంకం	    n_targets = N_SELECTION_TARGETS;
    స్థిర Gtk_లక్ష్యం_నమోదు  targets[N_SELECTION_TARGETS];

    ఒకవేళ (targets[0].లక్ష్యం == ఖాళీ)
    {
	కొరకు (i = 0; i < (పూర్ణాంకం)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    ఒకవేళ (!clip_html && selection_targets[i].info == లక్ష్యం_HTML)
		n_targets--;
	    లేకపోతే
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_కూడు_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    శూన్య
gui_gtk_సమితి_dnd_targets(శూన్య)
{
    పూర్ణాంకం		    i, j = 0;
    పూర్ణాంకం		    n_targets = N_DND_TARGETS;
    Gtk_లక్ష్యం_నమోదు  targets[N_DND_TARGETS];

    కొరకు (i = 0; i < (పూర్ణాంకం)N_DND_TARGETS; ++i)
    {
	ఒకవేళ (!clip_html && dnd_targets[i].info == లక్ష్యం_HTML)
	    n_targets--;
	లేకపోతే
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_సమితి(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    పూర్ణాంకం
gui_mch_init(శూన్య)
{
    GtkWidget *vbox;

#నిర్వచితమైతే FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    ఒకవేళ (using_gnome)
    {
	gnome_కార్యక్రమం_init(VIMPACKAGE, VIM_సంస్కరణ_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, ఖాళీ);
# ఒకవేళ defined(LC_NUMERIC)
	{
	    అక్షరం *p = setlocale(LC_NUMERIC, ఖాళీ);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    ఒకవేళ (p == ఖాళీ || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#అయితే_ముగింపు
    VIM_CLEAR(gui_argv);

#ఒకవేళ GLIB_CHECK_సంస్కరణ(2,1,3)
    // Set the human-readable application name
    g_సమితి_application_name("Vim");
#అయితే_ముగింపు
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    సమితి_option_విలువ_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#నిర్వచితమైతే FEAT_TOOLBAR
    gui_gtk_రిజిస్టరు_stock_icons();
#అయితే_ముగింపు
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
# ఒకవేళ 0
    gtk_rc_parse("gtkrc");
# endif
#అయితే_ముగింపు

    // Initialize values
    gui.border_వెడల్పు = 2;
    gui.scrollbar_వెడల్పు = SB_DEFAULT_వెడల్పు;
    gui.scrollbar_ఎత్తు = SB_DEFAULT_వెడల్పు;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#లేకపోతే
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#అయితే_ముగింపు

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.వెనుక_pixel = gui.def_వెనుక_pixel;

    ఒకవేళ (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	ఒకవేళ (plug != ఖాళీ && gtk_plug_get_socket_కిటికీ(GTK_PLUG(plug)) != ఖాళీ)
	{
	    gui.mainwin = plug;
	}
	లేకపోతే
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (గుర్తులేని పూర్ణాంకం)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    ఒకవేళ (gtk_socket_id == 0)
    {
#నిర్వచితమైతే FEAT_GUI_GNOME
	ఒకవేళ (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", ఖాళీ);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	లేకపోతే
#అయితే_ముగింపు
	    gui.mainwin = gtk_కిటికీ_new(GTK_కిటికీ_TOPLEVEL);
    }

    gtk_widget_సమితి_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.పాఠ్యం_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_సమితి_పునాది_dir(gui.పాఠ్యం_context, PANGO_DIRECTION_LTR);

    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_కూడు_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_ముసుగు);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), ఖాళీ);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), ఖాళీ);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_మారింది_cb), ఖాళీ);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), ఖాళీ);

    gui.accel_సమూహం = gtk_accel_సమూహం_new();
    gtk_కిటికీ_కూడు_accel_సమూహం(GTK_కిటికీ(gui.mainwin), gui.accel_సమూహం);

    // A vertical box holds the menubar, toolbar and main text window.
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_సమితి_homogeneous(GTK_BOX(vbox), FALSE);
#లేకపోతే
    vbox = gtk_vbox_new(FALSE, 0);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI_GNOME
    ఒకవేళ (using_gnome)
    {
# ఒకవేళ defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_సమితి_contents(GNOME_APP(gui.mainwin), vbox);
    }
    లేకపోతే
#అయితే_ముగింపు
    {
	gtk_container_కూడు(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#నిర్వచితమైతే FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_సమితి_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_సమితి(gtk_settings, "gtk-menu-bar-accel", ఖాళీ, ఖాళీ);
    }


# ifdef FEAT_GUI_GNOME
    ఒకవేళ (using_gnome)
    {
	Bonobo_Dock_అంశం *dockitem;

	gnome_app_సమితి_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_అంశం_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_అంశం_సమితి_behavior(dockitem,
		bonobo_dock_అంశం_get_behavior(dockitem)
				       | BONOBO_DOCK_అంశం_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    లేకపోతే
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#అయితే_ముగింపు	// FEAT_MENU

#నిర్వచితమైతే FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# లేకపోతే
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_సమితి_name(gui.toolbar, "vim-toolbar");
    సమితి_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    ఒకవేళ (using_gnome)
    {
	Bonobo_Dock_అంశం *dockitem;

	gnome_app_సమితి_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_అంశం_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_అంశం_సమితి_behavior(dockitem,
		bonobo_dock_అంశం_get_behavior(dockitem)
				       | BONOBO_DOCK_అంశం_BEH_NEVER_FLOATING);
	gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(gui.toolbar), 0);
    }
    లేకపోతే
# endif	// FEAT_GUI_GNOME
    {
	ఒకవేళ (vim_strchr(p_go, GO_TOOLBAR) != ఖాళీ
		&& (toolbar_flags & (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#అయితే_ముగింపు // FEAT_TOOLBAR

#నిర్వచితమైతే FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_సమితి_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_సమితి_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_సమితి_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_notebook_సమితి_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *పుట, *label, *event_box;

	// Add the first tab.
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	పుట = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_సమితి_homogeneous(GTK_BOX(పుట), FALSE);
# లేకపోతే
	పుట = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(పుట);
	gtk_container_కూడు(GTK_CONTAINER(gui.tabline), పుట);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_సమితి_దత్తాంశం(G_OBJECT(event_box), "tab_num", GINT_గమ్యం_సూచిక(1L));
# ఒకవేళ !GTK_CHECK_సంస్కరణ(3,14,0)
	gtk_misc_సమితి_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_కూడు(GTK_CONTAINER(event_box), label);
	gtk_notebook_సమితి_tab_label(GTK_NOTEBOOK(gui.tabline), పుట, event_box);
# ఒకవేళ GTK_CHECK_సంస్కరణ(2,10,0)
	gtk_notebook_సమితి_tab_reorderable(GTK_NOTEBOOK(gui.tabline), పుట, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ఖాళీ);
# ఒకవేళ GTK_CHECK_సంస్కరణ(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ఖాళీ);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#అయితే_ముగింపు // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_సమితి_border_వెడల్పు(GTK_CONTAINER(gui.formwin), 0);
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    gtk_widget_సమితి_events(gui.formwin, GDK_EXPOSURE_ముసుగు);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    gtk_widget_సమితి_name(gui.formwin, "vim-gtk-form");
#అయితే_ముగింపు

    gui.drawarea = gtk_drawing_area_new();
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    gui.surface = ఖాళీ;
#అయితే_ముగింపు

#నిర్వచితమైతే GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    ఒకవేళ (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#అయితే_ముగింపు

    // Determine which events we will filter.
    gint event_ముసుగు =
	GDK_EXPOSURE_ముసుగు |
	GDK_ENTER_NOTIFY_ముసుగు |
	GDK_LEAVE_NOTIFY_ముసుగు |
	GDK_BUTTON_PRESS_ముసుగు |
	GDK_BUTTON_RELEASE_ముసుగు |
	GDK_కీలకం_PRESS_ముసుగు |
	GDK_కీలకం_RELEASE_ముసుగు |
	GDK_సూచిక_MOTION_ముసుగు |
	GDK_సూచిక_MOTION_HINT_ముసుగు;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,4,0)
    ఒకవేళ (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_ముసుగు |= GDK_SCROLL_ముసుగు;
    }
    లేకపోతే
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_ముసుగు |= GDK_SMOOTH_SCROLL_ముసుగు;
    }
#లేకపోతే
    event_ముసుగు |= GDK_SCROLL_ముసుగు;
#అయితే_ముగింపు
    gtk_widget_సమితి_events(gui.drawarea, event_ముసుగు);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_ప్రారంభం(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(కీలకం_press_event), ఖాళీ);
#ఒకవేళ defined(FEAT_XIM) || GTK_CHECK_సంస్కరణ(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&కీలకం_release_event), ఖాళీ);
#అయితే_ముగింపు
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ఖాళీ);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), ఖాళీ);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), ఖాళీ);
#లేకపోతే
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_సమితి_cb), ఖాళీ);
#అయితే_ముగింపు

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#అయితే_ముగింపు

#ఒకవేళ !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#అయితే_ముగింపు

    ఒకవేళ (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_సమితి_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_నక్షత్రం.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_స్థానభ్రంశం = gui.border_వెడల్పు;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), ఖాళీ);
#లేకపోతే
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), ఖాళీ);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), ఖాళీ);
#అయితే_ముగింపు

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ఒకవేళ (vim_strchr(p_go, GO_సూచిక) != ఖాళీ)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), ఖాళీ);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), ఖాళీ);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    ఒకవేళ (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_బహిర్గతం_event), ఖాళీ);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_లోపల_event), ఖాళీ);
    }
    లేకపోతే
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_బహిర్గతం_event), ఖాళీ);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_లోపల_event), ఖాళీ);
#నిర్వచితమైతే FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_బహిర్గతం_event), ఖాళీ);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_లోపల_event), ఖాళీ);
#అయితే_ముగింపు // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), ఖాళీ);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.లోపల_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_మారింది_cb), ఖాళీ);
    }

#ఒకవేళ defined(FEAT_IMAGE) && GTK_CHECK_సంస్కరణ(3,10,0)
    gui.scale = gtk_widget_get_scale_గుణకం(gui.formwin);
#అయితే_ముగింపు

    తిరిగి OK;
}

#ఒకవేళ defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    శూన్య
gui_mch_forked(శూన్య)
{
    ఒకవేళ (!using_gnome)
	తిరిగి;

    GnomeClient *client;

    client = gnome_master_client();

    ఒకవేళ (client != ఖాళీ)
	gnome_client_సమితి_process_id(client, getpid());
}
#అయితే_ముగింపు // USE_GNOME_SESSION

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర GdkRGBA
color_గమ్యం_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    తిరిగి rgba;
}

    స్థిర శూన్య
సమితి_cairo_ఆధారం_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    స్థిరరాశి GdkRGBA rgba = color_గమ్యం_rgba(color);
    cairo_సమితి_ఆధారం_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#అయితే_ముగింపు // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    శూన్య
gui_mch_new_colors(శూన్య)
{
    ఒకవేళ (gui.drawarea != ఖాళీ
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
	    && gui.formwin != ఖాళీ
#అయితే_ముగింపు
	    && gtk_widget_get_కిటికీ(gui.drawarea) != ఖాళీ)
    {
#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,22,2)
	Gdk_కిటికీ * స్థిరరాశి da_win = gtk_widget_get_కిటికీ(gui.drawarea);
#అయితే_ముగింపు
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
	GtkStyleContext * స్థిరరాశి context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * స్థిరరాశి provider = gtk_css_provider_new();
	gchar * స్థిరరాశి css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.వెనుక_pixel >> 16) & 0xff,
		 (gui.వెనుక_pixel >> 8) & 0xff,
		 gui.వెనుక_pixel & 0xff);

	gtk_css_provider_ఎక్కించు_from_దత్తాంశం(provider, css, -1, ఖాళీ);
	gtk_style_context_కూడు_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#లేదావేళ GTK_CHECK_సంస్కరణ(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_గమ్యం_rgba(gui.వెనుక_pixel);
	{
	    cairo_నమూనా_t * స్థిరరాశి pat = cairo_నమూనా_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    ఒకవేళ (pat != ఖాళీ)
	    {
		gdk_కిటికీ_సమితి_background_నమూనా(da_win, pat);
		cairo_నమూనా_destroy(pat);
	    }
	    లేకపోతే
		gdk_కిటికీ_సమితి_background_rgba(da_win, &rgba);
	}
#లేకపోతే // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.వెనుక_pixel;
	gdk_కిటికీ_సమితి_background(da_win, &color);
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,22,2)
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
    స్థిర gboolean
startup_resize_correction_cb(gpointer దత్తాంశం వాడని)
{
    gui_సమితి_shellsize(FALSE, FALSE, RESIZE_BOTH);
    తిరిగి FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    స్థిర gint
form_configure_event(GtkWidget *widget వాడని,
		     GdkEventConfigure *event,
		     gpointer దత్తాంశం వాడని)
{
    పూర్ణాంకం	    usable_ఎత్తు = event->ఎత్తు;
#నిర్వచితమైతే TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    పూర్ణాంకం	    w, h;
    gtk_కిటికీ_get_కొలత(GTK_కిటికీ(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ఖాళీ, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (పూర్ణాంకం)Columns, (పూర్ణాంకం)అడ్డువరుసలు);
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

    ఒకవేళ (!సరిపోలిక_వెడల్పు_ఎత్తు(latest_resize_hist, w, h)
					     && సరిపోలిక_stale_వెడల్పు_ఎత్తు(w, h))
	// discard stale event
	తిరిగి TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    ఒకవేళ ((mch_csd_ఎత్తు == 0 || mch_csd_వెడల్పు == 0)
	    && latest_resize_hist != ఖాళీ
	    && !latest_resize_hist->వాడిన
	    && latest_resize_hist->from_shellsize
	    && gui.char_ఎత్తు > 0)
    {
	పూర్ణాంకం pot_csd_w = latest_resize_hist->వెడల్పు - w;
	పూర్ణాంకం pot_csd_h = latest_resize_hist->ఎత్తు - h;

	ఒకవేళ (pot_csd_w > 0 && pot_csd_w < gui.char_వెడల్పు)
	{
	    mch_csd_వెడల్పు = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (పూర్ణాంకం)Columns * gui.char_వెడల్పు
						    + gui_get_పునాది_వెడల్పు();
	}
	ఒకవేళ (pot_csd_h > 0 && pot_csd_h < gui.char_ఎత్తు)
	{
	    mch_csd_ఎత్తు = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_ఎత్తు += mch_csd_ఎత్తు;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	ఒకవేళ ((mch_csd_ఎత్తు > 0 || mch_csd_వెడల్పు > 0) && gtk_socket_id == 0)
	    g_idle_కూడు_నిండిన(GTK_ప్రాధాన్యత_RESIZE, startup_resize_correction_cb, ఖాళీ, ఖాళీ);
    }
    clear_resize_hists();
#అయితే_ముగింపు

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2) && !GTK_CHECK_సంస్కరణ(3,22,4)
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
    ఒకవేళ (event->కిటికీ_2 != gtk_widget_get_కిటికీ(gui.formwin))
	తిరిగి TRUE;
#అయితే_ముగింపు

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    ఒకవేళ (gtk_socket_id != 0)
	usable_ఎత్తు -= (gui.char_ఎత్తు - (gui.char_ఎత్తు/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	పూర్ణాంకం use_వెడల్పు = event->వెడల్పు;

#నిర్వచితమైతే TRACK_RESIZE_HISTORY
	ఒకవేళ (mch_pending_form_w > event->వెడల్పు
		&& mch_pending_form_w - event->వెడల్పు < gui.char_వెడల్పు)
	    use_వెడల్పు = mch_pending_form_w;
#అయితే_ముగింపు
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_వెడల్పు, usable_ఎత్తు);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    తిరిగి TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    స్థిర శూన్య
mainwin_destroy_cb(GObject *object వాడని, gpointer దత్తాంశం వాడని)
{
    // Don't write messages to the GUI anymore
    నిండిన_screen = FALSE;

    gui.mainwin  = ఖాళీ;
    gui.drawarea = ఖాళీ;

    ఒకవేళ (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#నిర్వచితమైతే USE_GRESOURCE
    gui_gtk_unregister_resource();
#అయితే_ముగింపు
}

    శూన్య
gui_gtk_get_screen_geom_యొక్క_win(
	GtkWidget *wid,
	పూర్ణాంకం బిందువు_x,	    // x position of window if not initialized
	పూర్ణాంకం బిందువు_y,	    // y position of window if not initialized
	పూర్ణాంకం *screen_x,
	పూర్ణాంకం *screen_y,
	పూర్ణాంకం *వెడల్పు,
	పూర్ణాంకం *ఎత్తు)
{
    GdkRectangle geometry;
    Gdk_కిటికీ *win = gtk_widget_get_కిటికీ(wid);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    ఒకవేళ (wid != ఖాళీ && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    లేకపోతే
	dpy = gdk_display_get_default();
    ఒకవేళ (win != ఖాళీ)
	monitor = gdk_display_get_monitor_at_కిటికీ(dpy, win);
    లేకపోతే
	monitor = gdk_display_get_monitor_at_బిందువు(dpy, బిందువు_x, బిందువు_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#లేకపోతే
    GdkScreen* screen;
    పూర్ణాంకం monitor;

    ఒకవేళ (wid != ఖాళీ && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    లేకపోతే
	screen = gdk_screen_get_default();
    ఒకవేళ (win != ఖాళీ)
	monitor = gdk_screen_get_monitor_at_కిటికీ(screen, win);
    లేకపోతే
	monitor = gdk_screen_get_monitor_at_బిందువు(screen, బిందువు_x, బిందువు_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#అయితే_ముగింపు
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *వెడల్పు = geometry.వెడల్పు;
    *ఎత్తు = geometry.ఎత్తు;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    స్థిర శూన్య
gui_gtk_get_screen_dimensions(
	పూర్ణాంకం బిందువు_x,
	పూర్ణాంకం బిందువు_y,
	పూర్ణాంకం *screen_w,
	పూర్ణాంకం *screen_h)
{
    పూర్ణాంకం	    x, y;

    gui_gtk_get_screen_geom_యొక్క_win(gui.mainwin, బిందువు_x, బిందువు_y,
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
    *screen_w -= get_menu_tool_వెడల్పు();
    *screen_h -= get_menu_tool_ఎత్తు();
}

    శూన్య
gui_mch_get_screen_dimensions(పూర్ణాంకం *screen_w, పూర్ణాంకం *screen_h)
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
    స్థిర timeout_cb_type
check_startup_plug_hints(gpointer దత్తాంశం వాడని)
{
    ఒకవేళ (init_కిటికీ_hints_state == 1)
    {
	// Safe to use normal hints now
	init_కిటికీ_hints_state = 0;
	update_కిటికీ_manager_hints(0, 0);
	తిరిగి FALSE;   // stop timer
    }

    // Keep on trying
    init_కిటికీ_hints_state = 1;
    తిరిగి TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    పూర్ణాంకం
gui_mch_open(శూన్య)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_వెడల్పు;
    guint		pixel_ఎత్తు;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    ఒకవేళ (role_వాదం != ఖాళీ)
    {
	gtk_కిటికీ_సమితి_role(GTK_కిటికీ(gui.mainwin), role_వాదం);
    }
    లేకపోతే
    {
	అక్షరం *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (గుర్తులేని)mch_get_pid(),
			       (గుర్తులేని)g_random_int(),
			       (గుర్తులేని)సమయం(ఖాళీ));

	gtk_కిటికీ_సమితి_role(GTK_కిటికీ(gui.mainwin), role);
	g_free(role);
    }

    ఒకవేళ (gui_win_x != -1 && gui_win_y != -1)
	gtk_కిటికీ_move(GTK_కిటికీ(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    ఒకవేళ (gui.geom != ఖాళీ)
    {
	పూర్ణాంకం		ముసుగు;
	గుర్తులేని పూర్ణాంకం	w, h;
	పూర్ణాంకం		x = 0;
	పూర్ణాంకం		y = 0;

	ముసుగు = XParseGeometry((అక్షరం *)gui.geom, &x, &y, &w, &h);

	ఒకవేళ (ముసుగు & వెడల్పు_విలువ)
	    Columns = w;
	ఒకవేళ (ముసుగు & ఎత్తు_విలువ)
	{
	    ఒకవేళ (p_కిటికీ > (దీర్ఘ)h - 1 || !option_was_సమితి((char_u *)"window"))
		p_కిటికీ = h - 1;
	    అడ్డువరుసలు = h;
	}
	హద్దు_screen_కొలత();

	pixel_వెడల్పు = (guint)(gui_get_పునాది_వెడల్పు() + Columns * gui.char_వెడల్పు);
	pixel_ఎత్తు = (guint)(gui_get_పునాది_ఎత్తు() + అడ్డువరుసలు * gui.char_ఎత్తు);

	pixel_వెడల్పు  += get_menu_tool_వెడల్పు();
	pixel_ఎత్తు += get_menu_tool_ఎత్తు();

	ఒకవేళ (ముసుగు & (XValue | YValue))
	{
	    పూర్ణాంకం ww, hh;

#నిర్వచితమైతే FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#లేకపోతే
	    gui_mch_get_screen_dimensions(&ww, &hh);
#అయితే_ముగింపు
	    hh += p_ghr + get_menu_tool_ఎత్తు();
	    ww += get_menu_tool_వెడల్పు();
	    ఒకవేళ (ముసుగు & XNegative)
		x += ww - pixel_వెడల్పు;
	    ఒకవేళ (ముసుగు & YNegative)
		y += hh - pixel_ఎత్తు;
	    gtk_కిటికీ_move(GTK_కిటికీ(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	ఒకవేళ (gtk_socket_id != 0  &&  (ముసుగు & వెడల్పు_విలువ || ముసుగు & ఎత్తు_విలువ))
	{
	    update_కిటికీ_manager_hints(pixel_వెడల్పు, pixel_ఎత్తు);
	    init_కిటికీ_hints_state = 1;
	    timeout_కూడు(1000, check_startup_plug_hints, ఖాళీ);
	}
    }

    pixel_వెడల్పు = (guint)(gui_get_పునాది_వెడల్పు() + Columns * gui.char_వెడల్పు);
    pixel_ఎత్తు = (guint)(gui_get_పునాది_ఎత్తు() + అడ్డువరుసలు * gui.char_ఎత్తు);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    ఒకవేళ (gtk_socket_id == 0)
	gtk_కిటికీ_resize(GTK_కిటికీ(gui.mainwin), pixel_వెడల్పు, pixel_ఎత్తు);
    update_కిటికీ_manager_hints(0, 0);

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

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    సమితి_సాధారణ_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ఖాళీ);

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
#నిర్వచితమైతే TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#అయితే_ముగింపు
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), ఖాళీ);
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_గుణకం_event), ఖాళీ);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_సమితి_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_దత్తాంశం_received_cb), ఖాళీ);
#అయితే_ముగింపు

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	ఒకవేళ (దొరికింది_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#ఒకవేళ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	గుర్తులేని దీర్ఘ menu_handler = 0;
# ifdef FEAT_TOOLBAR
	గుర్తులేని దీర్ఘ tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	ఒకవేళ (using_gnome && vim_strchr(p_go, GO_MENUS) == ఖాళీ)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ఖాళీ);
# ifdef FEAT_TOOLBAR
	ఒకవేళ (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == ఖాళీ
		&& (toolbar_flags & (TOOLBAR_పాఠ్యం | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ఖాళీ);
# endif
#అయితే_ముగింపు
	gtk_widget_show(gui.mainwin);

#ఒకవేళ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	ఒకవేళ (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	ఒకవేళ (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#అయితే_ముగింపు
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), ఖాళీ);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), ఖాళీ);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), ఖాళీ);

    తిరిగి OK;
}

/*
 * Clean up for when exiting Vim.
 */
    శూన్య
gui_mch_exit(పూర్ణాంకం rc వాడని)
{
    // Clean up, unless we don't want to invoke free().
    ఒకవేళ (gui.mainwin != ఖాళీ && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    పూర్ణాంకం
gui_mch_get_winpos(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    ఒకవేళ (gui_mch_get_display())
    {
	gtk_కిటికీ_get_position(GTK_కిటికీ(gui.mainwin), x, y);
	తిరిగి OK;
    }
    తిరిగి FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    శూన్య
gui_mch_సమితి_winpos(పూర్ణాంకం x, పూర్ణాంకం y)
{
    gtk_కిటికీ_move(GTK_కిటికీ(gui.mainwin), x, y);
}

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
# ఒకవేళ 0
స్థిర పూర్ణాంకం resize_idle_installed = FALSE;
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
    స్థిర gboolean
force_shell_resize_idle(gpointer దత్తాంశం)
{
    ఒకవేళ (gui.mainwin != ఖాళీ
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	పూర్ణాంకం వెడల్పు;
	పూర్ణాంకం ఎత్తు;

	gtk_కిటికీ_get_కొలత(GTK_కిటికీ(gui.mainwin), &వెడల్పు, &ఎత్తు);

	వెడల్పు  -= get_menu_tool_వెడల్పు();
	ఎత్తు -= get_menu_tool_ఎత్తు();

	gui_resize_shell(వెడల్పు, ఎత్తు);
    }

    resize_idle_installed = FALSE;
    తిరిగి FALSE; // don't call me again
}
# endif
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    పూర్ణాంకం
gui_mch_maximized(శూన్య)
{
    తిరిగి (gui.mainwin != ఖాళీ && gtk_widget_get_కిటికీ(gui.mainwin) != ఖాళీ
	    && (gdk_కిటికీ_get_state(gtk_widget_get_కిటికీ(gui.mainwin))
					       & GDK_కిటికీ_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    శూన్య
gui_mch_unmaximize(శూన్య)
{
    ఒకవేళ (gui.mainwin != ఖాళీ)
	gtk_కిటికీ_unmaximize(GTK_కిటికీ(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    శూన్య
gui_mch_newfont(శూన్య)
{
    పూర్ణాంకం w, h;

    gtk_కిటికీ_get_కొలత(GTK_కిటికీ(gui.mainwin), &w, &h);
    w -= get_menu_tool_వెడల్పు();
    h -= get_menu_tool_ఎత్తు();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    శూన్య
gui_mch_సమితి_shellsize(పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు,
		      పూర్ణాంకం min_వెడల్పు వాడని,  పూర్ణాంకం min_ఎత్తు వాడని,
		      పూర్ణాంకం పునాది_వెడల్పు వాడని, పూర్ణాంకం పునాది_ఎత్తు వాడని,
		      పూర్ణాంకం direction వాడని)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    ఒకవేళ (gtk_socket_id == 0)
	update_కిటికీ_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    వెడల్పు  += get_menu_tool_వెడల్పు();
    ఎత్తు += get_menu_tool_ఎత్తు();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    వెడల్పు  += mch_csd_వెడల్పు;
    ఎత్తు += mch_csd_ఎత్తు;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_వెడల్పు > 0)
			    ? వెడల్పు - get_menu_tool_వెడల్పు() - mch_csd_వెడల్పు : 0;

#నిర్వచితమైతే TRACK_RESIZE_HISTORY
    alloc_resize_hist(వెడల్పు, ఎత్తు, TRUE);
#అయితే_ముగింపు
    ఒకవేళ (gtk_socket_id == 0)
	gtk_కిటికీ_resize(GTK_కిటికీ(gui.mainwin), వెడల్పు, ఎత్తు);
    లేకపోతే
	update_కిటికీ_manager_hints(వెడల్పు, ఎత్తు);

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
# ఒకవేళ 0
    ఒకవేళ (!resize_idle_installed)
    {
	g_idle_కూడు_నిండిన(GDK_ప్రాధాన్యత_EVENTS + 10,
			&force_shell_resize_idle, ఖాళీ, ఖాళీ);
	resize_idle_installed = TRUE;
    }
# endif
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    శూన్య
gui_mch_settitle(char_u *title, char_u *icon వాడని)
{
    ఒకవేళ (title != ఖాళీ && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, ఖాళీ);

    gtk_కిటికీ_సమితి_title(GTK_కిటికీ(gui.mainwin), (స్థిరరాశి అక్షరం *)title);

    ఒకవేళ (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#ఒకవేళ defined(FEAT_MENU)
    శూన్య
gui_mch_enable_menu(పూర్ణాంకం showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    ఒకవేళ (using_gnome)
	widget = gui.menubar_h;
    లేకపోతే
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    ఒకవేళ (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	ఒకవేళ (showit)
	    gtk_widget_show(widget);
	లేకపోతే
	    gtk_widget_hide(widget);

	update_కిటికీ_manager_hints(0, 0);
    }
}
#అయితే_ముగింపు // FEAT_MENU

#ఒకవేళ defined(FEAT_TOOLBAR)
    శూన్య
gui_mch_show_toolbar(పూర్ణాంకం showit)
{
    GtkWidget *widget;

    ఒకవేళ (gui.toolbar == ఖాళీ)
	తిరిగి;

# ifdef FEAT_GUI_GNOME
    ఒకవేళ (using_gnome)
	widget = gui.toolbar_h;
    లేకపోతే
# endif
	widget = gui.toolbar;

    ఒకవేళ (showit)
	సమితి_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    ఒకవేళ (!showit != !gtk_widget_get_visible(widget))
    {
	ఒకవేళ (showit)
	    gtk_widget_show(widget);
	లేకపోతే
	    gtk_widget_hide(widget);

	update_కిటికీ_manager_hints(0, 0);
    }
}
#అయితే_ముగింపు // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    స్థిర పూర్ణాంకం
is_cjk_font(PangoFontDescription *font_desc)
{
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    గుర్తులేని	i;
    పూర్ణాంకం		is_cjk = FALSE;

    font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, font_desc);

    ఒకవేళ (font == ఖాళీ)
	తిరిగి FALSE;

    కొరకు (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	ఒకవేళ (coverage != ఖాళీ)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#ఒకవేళ PANGO_సంస్కరణ_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#లేకపోతే
	    pango_coverage_unref(coverage);
#అయితే_ముగింపు
	}
    }

    g_object_unref(font);

    తిరిగి is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    పూర్ణాంకం
gui_mch_adjust_charheight(శూన్య)
{
    PangoFontMetrics	*metrics;
    పూర్ణాంకం			ascent;
    పూర్ణాంకం			descent;

    metrics = pango_context_get_metrics(gui.పాఠ్యం_context, gui.norm_font,
				pango_context_get_language(gui.పాఠ్యం_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_ఎత్తు = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ఎత్తు = MAX(gui.char_ఎత్తు, gui.char_ascent + 1);

    తిరిగి OK;
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
// Callback function used in gui_mch_font_dialog()
    స్థిర gboolean
font_filter(స్థిరరాశి PangoFontFamily *family,
	    స్థిరరాశి PangoFontFace   *face వాడని,
	    gpointer		   దత్తాంశం వాడని)
{
    తిరిగి pango_font_family_is_monospace((PangoFontFamily *)family);
}
#అయితే_ముగింపు

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
    పూర్ణాంకం		response;
    char_u	*fontname = ఖాళీ;
    char_u	*oldname;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
    dialog = gtk_font_chooser_dialog_new(ఖాళీ, ఖాళీ);
    gtk_font_chooser_సమితి_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    ఖాళీ, ఖాళీ);
#లేకపోతే
    dialog = gtk_font_selection_dialog_new(ఖాళీ);
#అయితే_ముగింపు

    gtk_కిటికీ_సమితి_transient_for(GTK_కిటికీ(dialog), GTK_కిటికీ(gui.mainwin));
    gtk_కిటికీ_సమితి_destroy_with_మాతృ(GTK_కిటికీ(dialog), TRUE);

    ఒకవేళ (oldval != ఖాళీ && oldval[0] != NUL)
    {
	ఒకవేళ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ఖాళీ);
	లేకపోతే
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	ఒకవేళ (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    ఒకవేళ (p != ఖాళీ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ఒకవేళ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	gtk_font_chooser_సమితి_font(
		GTK_FONT_CHOOSER(dialog), (స్థిరరాశి gchar *)oldname);
#లేకపోతే
	gtk_font_selection_dialog_సమితి_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (స్థిరరాశి అక్షరం *)oldname);
#అయితే_ముగింపు

	ఒకవేళ (oldname != oldval)
	    vim_free(oldname);
    }
    లేకపోతే
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	gtk_font_chooser_సమితి_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#లేకపోతే
	gtk_font_selection_dialog_సమితి_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#అయితే_ముగింపు

    response = gtk_dialog_నిరంతరం(GTK_DIALOG(dialog));

    ఒకవేళ (response == GTK_RESPONSE_OK)
    {
	అక్షరం *name;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#లేకపోతే
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#అయితే_ముగింపు
	ఒకవేళ (name != ఖాళీ)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
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
    }

    ఒకవేళ (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    తిరిగి fontname;
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
    స్థిర శూన్య
get_styled_font_variants(శూన్య)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, gui.norm_font);

    ఒకవేళ (plain_font == ఖాళీ)
	తిరిగి;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_సమితి_బరువు(bold_font_desc, PANGO_బరువు_BOLD);

    bold_font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    ఒకవేళ (bold_font != ఖాళీ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    ఒకవేళ (bold_font != ఖాళీ && gui.font_can_bold)
	g_object_unref(plain_font);
}

స్థిర PangoEngineShape *default_shape_engine = ఖాళీ;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
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

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
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

    ఒకవేళ (అంశం_list != ఖాళీ && అంశం_list->తదుపరి == ఖాళీ) // play safe
    {
	Pango_అంశం   *అంశం;
	పూర్ణాంకం	    వెడల్పు;

	అంశం  = (Pango_అంశం *)అంశం_list->దత్తాంశం;
	వెడల్పు = gui.char_వెడల్పు * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = అంశం->analysis.shape_engine;

	gui.ascii_font = అంశం->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((స్థిరరాశి అక్షరం *)ascii_chars, పరిమాణం(ascii_chars),
		    &అంశం->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->అంకె_glyphs == పరిమాణం(ascii_chars));

	కొరకు (i = 0; i < gui.ascii_glyphs->అంకె_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_స్థానభ్రంశం += MAX(0, వెడల్పు - geom->వెడల్పు) / 2;
	    geom->వెడల్పు = వెడల్పు;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(అంశం_list, (GFunc)(శూన్య *)&pango_అంశం_free, ఖాళీ);
    g_list_free(అంశం_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    పూర్ణాంకం
gui_mch_init_font(char_u *font_name, పూర్ణాంకం fontset వాడని)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    పూర్ణాంకం			    వెడల్పు;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
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
    ఒకవేళ (is_cjk_font(gui.norm_font))
    {
	పూర్ణాంకం cjk_వెడల్పు;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_సమితి_పాఠ్యం(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_కొలత(layout, &cjk_వెడల్పు, ఖాళీ);

	ఒకవేళ (వెడల్పు == cjk_వెడల్పు)  // Xft not patched
	    వెడల్పు /= 2;
    }
    g_object_unref(layout);

    gui.char_వెడల్పు = (వెడల్పు / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    ఒకవేళ (gui.char_వెడల్పు <= 0)
	gui.char_వెడల్పు = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_సమితి_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_పట్టిక_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ఒకవేళ (gui.wide_font != ఖాళీ
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = ఖాళీ;
    }

    ఒకవేళ (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    లేకపోతే
    {
	// Preserve the logical dimensions of the screen.
	update_కిటికీ_manager_hints(0, 0);
    }

    తిరిగి OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, పూర్ణాంకం report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    ఒకవేళ (!gui.లోపల_use || name == ఖాళీ)
	తిరిగి ఖాళీ;

    ఒకవేళ (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, ఖాళీ);
	ఒకవేళ (buf != ఖాళీ)
	{
	    font = pango_font_description_from_string((స్థిరరాశి అక్షరం *)buf);
	    vim_free(buf);
	}
	లేకపోతే
	    font = ఖాళీ;
    }
    లేకపోతే
	font = pango_font_description_from_string((స్థిరరాశి అక్షరం *)name);

    ఒకవేళ (font != ఖాళీ)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	ఒకవేళ (pango_font_description_get_కొలత(font) <= 0)
	    pango_font_description_సమితి_కొలత(font, 10 * PANGO_SCALE);

	real_font = pango_context_ఎక్కించు_font(gui.పాఠ్యం_context, font);

	ఒకవేళ (real_font == ఖాళీ)
	{
	    pango_font_description_free(font);
	    font = ఖాళీ;
	}
	లేకపోతే
	    g_object_unref(real_font);
    }

    ఒకవేళ (font == ఖాళీ)
    {
	ఒకవేళ (report_error)
	    semsg(_((అక్షరం *)e_unknown_font_str), name);
	తిరిగి ఖాళీ;
    }

    తిరిగి font;
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name వాడని)
{
    ఒకవేళ (font != NOFONT)
    {
	అక్షరం	*pangoname = pango_font_description_గమ్యం_string(font);

	ఒకవేళ (pangoname != ఖాళీ)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    తిరిగి s;
	}
    }
    తిరిగి ఖాళీ;
}
#అయితే_ముగింపు

/*
 * If a font is not going to be used, free its structure.
 */
    శూన్య
gui_mch_free_font(GuiFont font)
{
    ఒకవేళ (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
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
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
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

	స్థిరరాశి అక్షరం* fam_name = pango_font_family_get_name(font_families[i]);
	ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ఆదానం_conv, (char_u*)fam_name, ఖాళీ);
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
 * Return the Pixel value (color) for the given color name.
 *
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_color(char_u *name)
{
    guicolor_T color = INVALCOLOR;

    ఒకవేళ (!gui.లోపల_use)		// can't do this when GUI not running
	తిరిగి color;

    ఒకవేళ (name != ఖాళీ)
	color = gui_get_color_cmn(name);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    తిరిగి color;
#లేకపోతే
    ఒకవేళ (color == INVALCOLOR)
	తిరిగి INVALCOLOR;

    తిరిగి gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#అయితే_ముగింపు
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(పూర్ణాంకం r, పూర్ణాంకం g, పూర్ణాంకం b)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    తిరిగి gui_get_rgb_color_cmn(r, g, b);
#లేకపోతే
    GdkColor gcolor;
    పూర్ణాంకం ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    తిరిగి ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#అయితే_ముగింపు
}

/*
 * Set the current text foreground color.
 */
    శూన్య
gui_mch_సమితి_fg_color(guicolor_T color)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    *gui.fgcolor = color_గమ్యం_rgba(color);
#లేకపోతే
    gui.fgcolor->pixel = (గుర్తులేని దీర్ఘ)color;
#అయితే_ముగింపు
}

/*
 * Set the current text background color.
 */
    శూన్య
gui_mch_సమితి_bg_color(guicolor_T color)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    *gui.bgcolor = color_గమ్యం_rgba(color);
#లేకపోతే
    gui.bgcolor->pixel = (గుర్తులేని దీర్ఘ)color;
#అయితే_ముగింపు
}

/*
 * Set the current text special color.
 */
    శూన్య
gui_mch_సమితి_sp_color(guicolor_T color)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    *gui.spcolor = color_గమ్యం_rgba(color);
#లేకపోతే
    gui.spcolor->pixel = (గుర్తులేని దీర్ఘ)color;
#అయితే_ముగింపు
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#నిర్వచించు చొప్పించు_PANGO_ATTR(Attribute, AttrList, ప్రారంభం_2, చివర_2)  \
    G_STMT_ప్రారంభం{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ప్రారంభం_సూచీ = (ప్రారంభం_2);		    \
	tmp_attr_->చివర_సూచీ = (చివర_2);			    \
	pango_attr_list_చొప్పించు((AttrList), tmp_attr_);	    \
    }G_STMT_చివర

    స్థిర శూన్య
apply_wide_font_attr(char_u *s, పూర్ణాంకం len, PangoAttrList *attr_list)
{
    char_u  *ప్రారంభం = ఖాళీ;
    char_u  *p;
    పూర్ణాంకం	    uc;

    కొరకు (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ఒకవేళ (ప్రారంభం == ఖాళీ)
	{
	    ఒకవేళ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ప్రారంభం = p;
	}
	లేకపోతే ఒకవేళ (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    చొప్పించు_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, ప్రారంభం - s, p - s);
	    ప్రారంభం = ఖాళీ;
	}
    }

    ఒకవేళ (ప్రారంభం != ఖాళీ)
	చొప్పించు_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, ప్రారంభం - s, len);
}

    స్థిర పూర్ణాంకం
లెక్క_cluster_cells(char_u *s, Pango_అంశం *అంశం,
		    PangoGlyphString* glyphs, పూర్ణాంకం i,
		    పూర్ణాంకం *cluster_వెడల్పు,
		    పూర్ణాంకం *చివరి_glyph_rbearing)
{
    char_u  *p;
    పూర్ణాంకం	    తదుపరి;	// glyph start index of next cluster
    పూర్ణాంకం	    ప్రారంభం, చివర; // string segment of current cluster
    పూర్ణాంకం	    వెడల్పు;	// real cluster width in Pango units
    పూర్ణాంకం	    uc;
    పూర్ణాంకం	    cellcount = 0;

    వెడల్పు = glyphs->glyphs[i].geometry.వెడల్పు;

    కొరకు (తదుపరి = i + 1; తదుపరి < glyphs->అంకె_glyphs; ++తదుపరి)
    {
	ఒకవేళ (glyphs->glyphs[తదుపరి].attr.is_cluster_ప్రారంభం)
	    ఆపు;
	లేకపోతే ఒకవేళ (glyphs->glyphs[తదుపరి].geometry.వెడల్పు > వెడల్పు)
	    వెడల్పు = glyphs->glyphs[తదుపరి].geometry.వెడల్పు;
    }

    ప్రారంభం = అంశం->స్థానభ్రంశం + glyphs->log_clusters[i];
    చివర   = అంశం->స్థానభ్రంశం + ((తదుపరి < glyphs->అంకె_glyphs) ?
			    glyphs->log_clusters[తదుపరి] : అంశం->నిడివి);

    కొరకు (p = s + ప్రారంభం; p < s + చివర; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ఒకవేళ (uc < 0x80)
	    ++cellcount;
	లేకపోతే ఒకవేళ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ఒకవేళ (చివరి_glyph_rbearing != ఖాళీ
	    && cellcount > 0 && తదుపరి == glyphs->అంకె_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(అంశం->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, ఖాళీ);

	ఒకవేళ (PANGO_RBEARING(ink_rect) > 0)
	    *చివరి_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ఒకవేళ (cellcount > 0)
	*cluster_వెడల్పు = వెడల్పు;

    తిరిగి cellcount;
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
    స్థిర శూన్య
అమరిక_zero_వెడల్పు_cluster(Pango_అంశం *అంశం, PangoGlyphInfo *glyph,
			 పూర్ణాంకం చివరి_cellcount, పూర్ణాంకం చివరి_cluster_వెడల్పు,
			 పూర్ణాంకం చివరి_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    పూర్ణాంకం		    వెడల్పు;

    వెడల్పు = చివరి_cellcount * gui.char_వెడల్పు * PANGO_SCALE;
    glyph->geometry.x_స్థానభ్రంశం = -వెడల్పు + MAX(0, వెడల్పు - చివరి_cluster_వెడల్పు) / 2;
    glyph->geometry.వెడల్పు = 0;

    pango_font_get_glyph_extents(అంశం->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    ఒకవేళ (ink_rect.x < 0)
    {
	glyph->geometry.x_స్థానభ్రంశం += చివరి_glyph_rbearing;
	glyph->geometry.y_స్థానభ్రంశం  = logical_rect.ఎత్తు
		- (gui.char_ఎత్తు - p_linespace) * PANGO_SCALE;
    }
    లేకపోతే
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_స్థానభ్రంశం = -వెడల్పు + MAX(0, వెడల్పు - ink_rect.వెడల్పు) / 2;
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
draw_glyph_string(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం అంకె_cells, పూర్ణాంకం flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#లేకపోతే
    స్థిర శూన్య
draw_glyph_string(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం అంకె_cells, పూర్ణాంకం flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#అయితే_ముగింపు
{
    ఒకవేళ (!(flags & DRAW_TRANSP))
    {
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(నిలువు), FILL_Y(అడ్డువరుస),
			అంకె_cells * gui.char_వెడల్పు, gui.char_ఎత్తు);
	cairo_fill(cr);
#లేకపోతే
	gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->కిటికీ_2,
			   gui.పాఠ్యం_gc,
			   TRUE,
			   FILL_X(నిలువు),
			   FILL_Y(అడ్డువరుస),
			   అంకె_cells * gui.char_వెడల్పు,
			   gui.char_ఎత్తు);
#అయితే_ముగింపు
    }

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_సమితి_ఆధారం_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_గమ్యం(cr, పాఠ్యం_X(నిలువు), పాఠ్యం_Y(అడ్డువరుస));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#లేకపోతే
    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->కిటికీ_2,
		    gui.పాఠ్యం_gc,
		    font,
		    పాఠ్యం_X(నిలువు),
		    పాఠ్యం_Y(అడ్డువరుస),
		    glyphs);
#అయితే_ముగింపు

    // redraw the contents with an offset of 1 to emulate bold
    ఒకవేళ ((flags & DRAW_BOLD) && !gui.font_can_bold)
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_గమ్యం(cr, పాఠ్యం_X(నిలువు) + 1, పాఠ్యం_Y(అడ్డువరుస));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#లేకపోతే
	gdk_draw_glyphs(gui.drawarea->కిటికీ_2,
			gui.పాఠ్యం_gc,
			font,
			పాఠ్యం_X(నిలువు) + 1,
			పాఠ్యం_Y(అడ్డువరుస),
			glyphs);
#అయితే_ముగింపు
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
draw_under(పూర్ణాంకం flags, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం cells, cairo_t *cr)
#లేకపోతే
    స్థిర శూన్య
draw_under(పూర్ణాంకం flags, పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం cells)
#అయితే_ముగింపు
{
    పూర్ణాంకం			i;
    పూర్ణాంకం			స్థానభ్రంశం;
    స్థిర స్థిరరాశి పూర్ణాంకం	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    పూర్ణాంకం			y = FILL_Y(అడ్డువరుస + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    ఒకవేళ (flags & DRAW_UNDERC)
    {
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	cairo_సమితి_పంక్తి_వెడల్పు(cr, 1.0);
	cairo_సమితి_పంక్తి_cap(cr, CAIRO_పంక్తి_CAP_BUTT);
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_గమ్యం(cr, FILL_X(నిలువు) + 1, y - 2 + 0.5);
	కొరకు (i = FILL_X(నిలువు) + 1; i < FILL_X(నిలువు + cells); ++i)
	{
	    స్థానభ్రంశం = val[i % 8];
	    cairo_పంక్తి_గమ్యం(cr, i, y - స్థానభ్రంశం + 0.5);
	}
	cairo_stroke(cr);
#లేకపోతే
	gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.spcolor);
	కొరకు (i = FILL_X(నిలువు); i < FILL_X(నిలువు + cells); ++i)
	{
	    స్థానభ్రంశం = val[i % 8];
	    gdk_draw_బిందువు(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc, i, y - స్థానభ్రంశం);
	}
	gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
#అయితే_ముగింపు
    }

    // Draw a strikethrough line
    ఒకవేళ (flags & DRAW_STRIKE)
    {
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	cairo_సమితి_పంక్తి_వెడల్పు(cr, 1.0);
	cairo_సమితి_పంక్తి_cap(cr, CAIRO_పంక్తి_CAP_BUTT);
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_గమ్యం(cr, FILL_X(నిలువు), y + 1 - gui.char_ఎత్తు/2 + 0.5);
	cairo_పంక్తి_గమ్యం(cr, FILL_X(నిలువు + cells), y + 1 - gui.char_ఎత్తు/2 + 0.5);
	cairo_stroke(cr);
#లేకపోతే
	gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.spcolor);
	gdk_draw_పంక్తి(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
		      FILL_X(నిలువు), y + 1 - gui.char_ఎత్తు/2,
		      FILL_X(నిలువు + cells), y + 1 - gui.char_ఎత్తు/2);
	gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
#అయితే_ముగింపు
    }

    // Underline: draw a line at the bottom of the character cell.
    ఒకవేళ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ఒకవేళ (p_linespace > 1)
	    y -= p_linespace - 1;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	cairo_సమితి_పంక్తి_వెడల్పు(cr, 1.0);
	cairo_సమితి_పంక్తి_cap(cr, CAIRO_పంక్తి_CAP_BUTT);
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_గమ్యం(cr, FILL_X(నిలువు), y + 0.5);
	cairo_పంక్తి_గమ్యం(cr, FILL_X(నిలువు + cells), y + 0.5);
	cairo_stroke(cr);
#లేకపోతే
	gdk_draw_పంక్తి(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
		      FILL_X(నిలువు), y,
		      FILL_X(నిలువు + cells) - 1, y);
#అయితే_ముగింపు
    }
}

    పూర్ణాంకం
gui_gtk_draw_string(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, char_u *s, పూర్ణాంకం len, పూర్ణాంకం flags)
{
    char_u	*conv_buf = ఖాళీ;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    పూర్ణాంకం		convlen;
    char_u	*sp, *bp;
    పూర్ణాంకం		plen;
    పూర్ణాంకం		len_కూడిక;	// return value needs to add up since we are
				// printing substrings
    పూర్ణాంకం		byte_కూడిక;	// byte position in string
    char_u	*cs;		// current *s pointer
    పూర్ణాంకం		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    పూర్ణాంకం		should_need_pango = FALSE;
    పూర్ణాంకం		slen;
    పూర్ణాంకం		is_ligature;
    పూర్ణాంకం		is_utf8;
    char_u	backup_ch;

    ఒకవేళ (gui.పాఠ్యం_context == ఖాళీ || gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి len;

    ఒకవేళ (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != ఖాళీ, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	కొరకు (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    ఒకవేళ ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		ఒకవేళ (new_conv_buf == ఖాళీ)
		    తిరిగి len;
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
    len_కూడిక = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_కూడిక = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    ఒకవేళ (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    ఒకవేళ (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    అయితే (cs < s + len)
    {
	slen = 0;
	అయితే (slen < (len - byte_కూడిక))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    ఒకవేళ (is_ligature && !needs_pango)
	    {
		ఒకవేళ ((slen + 1) < (len - byte_కూడిక))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		లేకపోతే
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    ఒకవేళ ((!is_utf8) && ((slen + 1) < (len - byte_కూడిక)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    ఒకవేళ (needs_pango != should_need_pango) // mode switch
		ఆపు;
	    ఒకవేళ (needs_pango)
	    {
		ఒకవేళ (is_ligature)
		{
		    slen++; // ligature char by char
		}
		లేకపోతే // is_utf8
		{
		    ఒకవేళ ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			అయితే ((slen < (len - byte_కూడిక))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    లేకపోతే ఒకవేళ ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    లేకపోతే ఒకవేళ ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    లేకపోతే ఒకవేళ ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    లేకపోతే
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    లేకపోతే
	    {
		slen++; // ascii
	    }
	}

	ఒకవేళ (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_కూడిక += gui_gtk_draw_string_ext(అడ్డువరుస, నిలువు + len_కూడిక, cs, slen, flags,
					    needs_pango);
	ఒకవేళ (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_కూడిక += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    తిరిగి len_కూడిక;
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర cairo_region_t *dirty_region = ఖాళీ;

    స్థిర శూన్య
వరుస_draw_area(పూర్ణాంకం x, పూర్ణాంకం y, పూర్ణాంకం వెడల్పు, పూర్ణాంకం ఎత్తు)
{
    cairo_rectangle_int_t rect;

    ఒకవేళ (వెడల్పు <= 0 || ఎత్తు <= 0 || gui.drawarea == ఖాళీ)
	తిరిగి;

    rect.x = x;
    rect.y = y;
    rect.వెడల్పు = వెడల్పు;
    rect.ఎత్తు = ఎత్తు;

    ఒకవేళ (dirty_region == ఖాళీ)
	dirty_region = cairo_region_create_rectangle(&rect);
    లేకపోతే
	cairo_region_union_rectangle(dirty_region, &rect);
}
#అయితే_ముగింపు

    పూర్ణాంకం
gui_gtk_draw_string_ext(
	పూర్ణాంకం	అడ్డువరుస,
	పూర్ణాంకం	నిలువు,
	char_u	*s,
	పూర్ణాంకం	len,
	పూర్ణాంకం	flags,
	పూర్ణాంకం	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    పూర్ణాంకం			స్తంభం_స్థానభ్రంశం = 0;  // column offset in cells
    పూర్ణాంకం			i;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_t		*cr;
#అయితే_ముగింపు

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_స్థానభ్రంశం;
    area.y = FILL_Y(అడ్డువరుస);
    area.వెడల్పు	= gui.అంకె_నిలువుల_సంఖ్య * gui.char_వెడల్పు;
    area.ఎత్తు = gui.char_ఎత్తు;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.వెడల్పు, area.ఎత్తు);
    cairo_clip(cr);
#లేకపోతే
    gdk_gc_సమితి_clip_origin(gui.పాఠ్యం_gc, 0, 0);
    gdk_gc_సమితి_clip_rectangle(gui.పాఠ్యం_gc, &area);
#అయితే_ముగింపు

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    ఒకవేళ (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ఖాళీ
	    && !force_pango)
    {
	char_u *p;

	కొరకు (p = s; p < s + len; ++p)
	    ఒకవేళ (*p & 0x80)
		వెళ్ళు not_ascii;

	pango_glyph_string_సమితి_కొలత(glyphs, len);

	కొరకు (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	draw_glyph_string(అడ్డువరుస, నిలువు, len, flags, gui.ascii_font, glyphs, cr);
#లేకపోతే
	draw_glyph_string(అడ్డువరుస, నిలువు, len, flags, gui.ascii_font, glyphs);
#అయితే_ముగింపు

	స్తంభం_స్థానభ్రంశం = len;
    }
    లేకపోతే
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*అంశం_list;
	పూర్ణాంకం		cluster_వెడల్పు;
	పూర్ణాంకం		చివరి_glyph_rbearing;
	పూర్ణాంకం		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	ఒకవేళ (!utf_valid_string(s, s + len))
	{
	    స్తంభం_స్థానభ్రంశం = len;
	    వెళ్ళు skipitall;
	}

	// original width of the current cluster
	cluster_వెడల్పు = PANGO_SCALE * gui.char_వెడల్పు;

	// right bearing of the last non-composing glyph
	చివరి_glyph_rbearing = PANGO_SCALE * gui.char_వెడల్పు;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	ఒకవేళ (gui.wide_font != ఖాళీ)
	    apply_wide_font_attr(s, len, attr_list);

	ఒకవేళ ((flags & DRAW_BOLD) && gui.font_can_bold)
	    చొప్పించు_PANGO_ATTR(pango_attr_బరువు_new(PANGO_బరువు_BOLD),
			      attr_list, 0, len);
	ఒకవేళ (flags & DRAW_ITALIC)
	    చొప్పించు_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	అంశం_list = pango_itemize(gui.పాఠ్యం_context,
				  (స్థిరరాశి అక్షరం *)s, 0, len, attr_list, ఖాళీ);

	అయితే (అంశం_list != ఖాళీ)
	{
	    Pango_అంశం	*అంశం;
	    పూర్ణాంకం		అంశం_cells = 0; // item length in cells

	    అంశం = (Pango_అంశం *)అంశం_list->దత్తాంశం;
	    అంశం_list = g_list_delete_link(అంశం_list, అంశం_list);
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
	    అంశం->analysis.level = (అంశం->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    అంశం->analysis.shape_engine = default_shape_engine;

#నిర్వచితమైతే HAVE_PANGO_SHAPE_నిండిన
	    pango_shape_నిండిన((స్థిరరాశి అక్షరం *)s + అంశం->స్థానభ్రంశం, అంశం->నిడివి,
		    (స్థిరరాశి అక్షరం *)s, len, &అంశం->analysis, glyphs);
#లేకపోతే
	    pango_shape((స్థిరరాశి అక్షరం *)s + అంశం->స్థానభ్రంశం, అంశం->నిడివి,
			&అంశం->analysis, glyphs);
#అయితే_ముగింపు
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    కొరకు (i = 0; i < glyphs->అంకె_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		ఒకవేళ (glyph->attr.is_cluster_ప్రారంభం)
		{
		    పూర్ణాంకం cellcount;

		    cellcount = లెక్క_cluster_cells(
			s, అంశం, glyphs, i, &cluster_వెడల్పు,
			(అంశం_list != ఖాళీ) ? &చివరి_glyph_rbearing : ఖాళీ);

		    ఒకవేళ (cellcount > 0)
		    {
			పూర్ణాంకం వెడల్పు;

			వెడల్పు = cellcount * gui.char_వెడల్పు * PANGO_SCALE;
			glyph->geometry.x_స్థానభ్రంశం +=
					    MAX(0, వెడల్పు - cluster_వెడల్పు) / 2;
			glyph->geometry.వెడల్పు = వెడల్పు;
		    }
		    లేకపోతే
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			అమరిక_zero_వెడల్పు_cluster(అంశం, glyph, cells,
						 cluster_వెడల్పు,
						 చివరి_glyph_rbearing);
		    }

		    అంశం_cells += cellcount;
		    cells = cellcount;
		}
		లేకపోతే ఒకవేళ (i > 0)
		{
		    పూర్ణాంకం వెడల్పు;

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
		    ఒకవేళ (glyph->geometry.x_స్థానభ్రంశం >= 0)
		    {
			glyphs->glyphs[i].geometry.వెడల్పు =
					 glyphs->glyphs[i - 1].geometry.వెడల్పు;
			glyphs->glyphs[i - 1].geometry.వెడల్పు = 0;
		    }
		    వెడల్పు = cells * gui.char_వెడల్పు * PANGO_SCALE;
		    glyph->geometry.x_స్థానభ్రంశం +=
					    MAX(0, వెడల్పు - cluster_వెడల్పు) / 2;
		}
		లేకపోతే // i == 0 "cannot happen"
		{
		    glyph->geometry.వెడల్పు = 0;
		}
	    }

	    //// Aaaaand action! **
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    draw_glyph_string(అడ్డువరుస, నిలువు + స్తంభం_స్థానభ్రంశం, అంశం_cells,
			      flags, అంశం->analysis.font, glyphs,
			      cr);
#లేకపోతే
	    draw_glyph_string(అడ్డువరుస, నిలువు + స్తంభం_స్థానభ్రంశం, అంశం_cells,
			      flags, అంశం->analysis.font, glyphs);
#అయితే_ముగింపు

	    pango_అంశం_free(అంశం);

	    స్తంభం_స్థానభ్రంశం += అంశం_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    draw_under(flags, అడ్డువరుస, నిలువు, స్తంభం_స్థానభ్రంశం, cr);
#లేకపోతే
    draw_under(flags, అడ్డువరుస, నిలువు, స్తంభం_స్థానభ్రంశం);
#అయితే_ముగింపు

    pango_glyph_string_free(glyphs);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_destroy(cr);
    వరుస_draw_area(FILL_X(నిలువు), FILL_Y(అడ్డువరుస),
	    స్తంభం_స్థానభ్రంశం * gui.char_వెడల్పు + 1, gui.char_ఎత్తు);
#లేకపోతే
    gdk_gc_సమితి_clip_rectangle(gui.పాఠ్యం_gc, ఖాళీ);
#అయితే_ముగింపు

    తిరిగి స్తంభం_స్థానభ్రంశం;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
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

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    పూర్ణాంకం
gui_get_x11_windis(కిటికీ *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    ఒకవేళ (dpy)
    {
	*dis = dpy;
	*win = GDK_కిటికీ_XID(gtk_widget_get_కిటికీ(gui.mainwin));
	తిరిగి OK;
    }

    *dis = ఖాళీ;
    *win = 0;
    తిరిగి FAIL;
}
#అయితే_ముగింపు

    Display *
gui_mch_get_display(శూన్య)
{
    ఒకవేళ (gui.mainwin != ఖాళీ && gtk_widget_get_కిటికీ(gui.mainwin) != ఖాళీ
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#అయితే_ముగింపు
	)
	తిరిగి GDK_కిటికీ_XDISPLAY(gtk_widget_get_కిటికీ(gui.mainwin));
    లేకపోతే
	తిరిగి ఖాళీ;
}

    శూన్య
gui_mch_beep(శూన్య)
{
    GdkDisplay *display;

    ఒకవేళ (gui.mainwin != ఖాళీ && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    లేకపోతే
	display = gdk_display_get_default();

    ఒకవేళ (display != ఖాళీ)
	gdk_display_beep(display);
}

    శూన్య
gui_mch_flash(పూర్ణాంకం msec)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    // TODO Replace GdkGC with Cairo
    (శూన్య)msec;
#లేకపోతే
    GdkGCValues	విలువలు;
    GdkGC	*invert_gc;

    ఒకవేళ (gui.drawarea->కిటికీ_2 == ఖాళీ)
	తిరిగి;

    విలువలు.foreground.pixel = gui.norm_pixel ^ gui.వెనుక_pixel;
    విలువలు.background.pixel = gui.norm_pixel ^ gui.వెనుక_pixel;
    విలువలు.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_విలువలు(gui.drawarea->కిటికీ_2,
				       &విలువలు,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_సమితి_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((పూర్ణాంకం)Columns) + gui.border_స్థానభ్రంశం,
		       FILL_Y((పూర్ణాంకం)అడ్డువరుసలు) + gui.border_స్థానభ్రంశం);

    gui_mch_flush();
    ui_delay((దీర్ఘ)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((పూర్ణాంకం)Columns) + gui.border_స్థానభ్రంశం,
		       FILL_Y((పూర్ణాంకం)అడ్డువరుసలు) + gui.border_స్థానభ్రంశం);

    gdk_gc_destroy(invert_gc);
#అయితే_ముగింపు
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    శూన్య
gui_mch_invert_rectangle(పూర్ణాంకం r, పూర్ణాంకం c, పూర్ణాంకం nr, పూర్ణాంకం nc)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిరరాశి GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_వెడల్పు, nr * gui.char_ఎత్తు
    };
    cairo_t * స్థిరరాశి cr = cairo_create(gui.surface);

    cairo_సమితి_ఆధారం_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# ఒకవేళ CAIRO_సంస్కరణ >= CAIRO_సంస్కరణ_ENCODE(1,9,2)
    cairo_సమితి_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# లేకపోతే
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
    cairo_fill(cr);

    cairo_destroy(cr);

    వరుస_draw_area(rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
#లేకపోతే
    GdkGCValues విలువలు;
    GdkGC *invert_gc;

    ఒకవేళ (gui.drawarea->కిటికీ_2 == ఖాళీ)
	తిరిగి;

    విలువలు.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_విలువలు(gui.drawarea->కిటికీ_2,
				       &విలువలు,
				       GDK_GC_FUNCTION);
    gdk_gc_సమితి_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_వెడల్పు, (nr) * gui.char_ఎత్తు);
    gdk_gc_destroy(invert_gc);
#అయితే_ముగింపు
}

/*
 * Iconify the GUI window.
 */
    శూన్య
gui_mch_iconify(శూన్య)
{
    gtk_కిటికీ_iconify(GTK_కిటికీ(gui.mainwin));
}

#ఒకవేళ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    శూన్య
gui_mch_సమితి_foreground(శూన్య)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_కిటికీ_సమితి_keep_above(GTK_కిటికీ(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_కిటికీ_present(GTK_కిటికీ(gui.mainwin));
    gui_may_flush();
    gtk_కిటికీ_సమితి_keep_above(GTK_కిటికీ(gui.mainwin), FALSE);
    gui_may_flush();
}
#అయితే_ముగింపు

/*
 * Draw a cursor without focus.
 */
    శూన్య
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    పూర్ణాంకం		i = 1;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_t    *cr;
#అయితే_ముగింపు

    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి;

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cr = cairo_create(gui.surface);
#అయితే_ముగింపు

    gui_mch_సమితి_fg_color(color);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_సమితి_ఆధారం_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#లేకపోతే
    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
#అయితే_ముగింపు
    ఒకవేళ (mb_lefthalve(gui.అడ్డువరుస, gui.నిలువు))
	i = 2;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    cairo_సమితి_పంక్తి_వెడల్పు(cr, 1.0);
    cairo_సమితి_పంక్తి_cap(cr, CAIRO_పంక్తి_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.నిలువు) + 0.5, FILL_Y(gui.అడ్డువరుస) + 0.5,
	    i * gui.char_వెడల్పు - 1, gui.char_ఎత్తు - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#లేకపోతే
    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
	    FALSE,
	    FILL_X(gui.నిలువు), FILL_Y(gui.అడ్డువరుస),
	    i * gui.char_వెడల్పు - 1, gui.char_ఎత్తు - 1);
#అయితే_ముగింపు
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    శూన్య
gui_mch_draw_part_cursor(పూర్ణాంకం w, పూర్ణాంకం h, guicolor_T color)
{
    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి;

    gui_mch_సమితి_fg_color(color);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_సమితి_ఆధారం_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_కుడి ? FILL_X(gui.నిలువు + 1) - w :
# endif
	    FILL_X(gui.నిలువు), FILL_Y(gui.అడ్డువరుస) + gui.char_ఎత్తు - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_కుడి ? FILL_X(gui.నిలువు + 1) - w :
# endif
	    FILL_X(gui.నిలువు),
	    FILL_Y(gui.అడ్డువరుస) + gui.char_ఎత్తు - h,
	    w, h);
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    శూన్య
gui_mch_update(శూన్య)
{
    పూర్ణాంకం cnt = 0;	// prevent endless loop
    అయితే (g_main_context_pending(ఖాళీ) && !vim_is_ఆదానం_buf_నిండిన()
	    && ++cnt < 100)
    {
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	పూర్ణాంకం prio = 0;
	g_main_context_prepare(ఖాళీ, &prio);
	// peek internal scheduling of redraw
	ఒకవేళ (prio == GDK_ప్రాధాన్యత_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#అయితే_ముగింపు
	g_main_context_iteration(ఖాళీ, TRUE);
    }
}

    స్థిర timeout_cb_type
ఆదానం_timer_cb(gpointer దత్తాంశం)
{
    పూర్ణాంకం *timed_బహిర్గతం = (పూర్ణాంకం *) దత్తాంశం;

    // Just inform the caller about the occurrence of it
    *timed_బహిర్గతం = TRUE;

    తిరిగి FALSE;		// don't happen again
}

#నిర్వచితమైతే FEAT_పని_CHANNEL
    స్థిర timeout_cb_type
channel_poll_cb(gpointer దత్తాంశం వాడని)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    తిరిగి TRUE;		// repeat
}
#అయితే_ముగింపు

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
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

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
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
	    // Need to recompute the waiting time.
	    వెళ్ళు theend;
# endif
#అయితే_ముగింపు

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	ఒకవేళ (!ఆదానం_అందుబాటు())
	{
	    g_main_context_iteration(ఖాళీ, TRUE);
#నిర్వచితమైతే GDK_WINDOWING_WAYLAND
    ఒకవేళ (gui.is_wayland && clip_నక్షత్రం.state == SELECT_లోపల_పురోగతి)
	gui_may_flush();
#అయితే_ముగింపు
	}

	// Got char, return immediately
	ఒకవేళ (ఆదానం_అందుబాటు())
	{
	    retval = OK;
	    వెళ్ళు theend;
	}
    } అయితే (wtime < 0 || !timed_బహిర్గతం);

    /*
     * Flush all eventually pending (drawing) events.
     */
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


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    శూన్య
gui_mch_flush(శూన్య)
{
    ఒకవేళ (gui.mainwin == ఖాళీ || !gtk_widget_get_realized(gui.mainwin))
	తిరిగి;
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ఒకవేళ (dirty_region != ఖాళీ && gui.drawarea != ఖాళీ)
    {
	gtk_widget_వరుస_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = ఖాళీ;
    }
#లేకపోతే
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       తిరిగి;
#అయితే_ముగింపు
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    శూన్య
gui_mch_clear_ఖండం(పూర్ణాంకం row1arg, పూర్ణాంకం col1arg, పూర్ణాంకం row2arg, పూర్ణాంకం col2arg)
{

    పూర్ణాంకం col1 = check_నిలువు(col1arg);
    పూర్ణాంకం col2 = check_నిలువు(col2arg);
    పూర్ణాంకం row1 = check_అడ్డువరుస(row1arg);
    పూర్ణాంకం row2 = check_అడ్డువరుస(row2arg);

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి;
#లేకపోతే
    GdkColor color;

    ఒకవేళ (gui.drawarea->కిటికీ_2 == ఖాళీ)
	తిరిగి;

    color.pixel = gui.వెనుక_pixel;
#అయితే_ముగింపు

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	స్థిరరాశి GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_వెడల్పు + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ఎత్తు
	};
	cairo_t * స్థిరరాశి cr = cairo_create(gui.surface);
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
	సమితి_cairo_ఆధారం_rgba_from_color(cr, gui.వెనుక_pixel);
# లేకపోతే
	Gdk_కిటికీ * స్థిరరాశి win = gtk_widget_get_కిటికీ(gui.drawarea);
	cairo_నమూనా_t * స్థిరరాశి pat = gdk_కిటికీ_get_background_నమూనా(win);
	ఒకవేళ (pat != ఖాళీ)
	    cairo_సమితి_ఆధారం(cr, pat);
	లేకపోతే
	    సమితి_cairo_ఆధారం_rgba_from_color(cr, gui.వెనుక_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
	cairo_fill(cr);
	cairo_destroy(cr);

	వరుస_draw_area(rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
    }
#లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_వెడల్పు
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_ఎత్తు);
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
}

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
gui_gtk_కిటికీ_clear(Gdk_కిటికీ *win)
{
    స్థిరరాశి GdkRectangle rect = {
	0, 0, gdk_కిటికీ_get_వెడల్పు(win), gdk_కిటికీ_get_ఎత్తు(win)
    };
    cairo_t * స్థిరరాశి cr = cairo_create(gui.surface);
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,22,2)
    సమితి_cairo_ఆధారం_rgba_from_color(cr, gui.వెనుక_pixel);
# లేకపోతే
    cairo_నమూనా_t * స్థిరరాశి pat = gdk_కిటికీ_get_background_నమూనా(win);
    ఒకవేళ (pat != ఖాళీ)
	cairo_సమితి_ఆధారం(cr, pat);
    లేకపోతే
	సమితి_cairo_ఆధారం_rgba_from_color(cr, gui.వెనుక_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
    cairo_fill(cr);
    cairo_destroy(cr);

    వరుస_draw_area(rect.x, rect.y, rect.వెడల్పు, rect.ఎత్తు);
}
#లేకపోతే
# define gui_gtk_కిటికీ_clear(win)  gdk_కిటికీ_clear(win)
#అయితే_ముగింపు

    శూన్య
gui_mch_clear_all(శూన్య)
{
    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) != ఖాళీ)
	gui_gtk_కిటికీ_clear(gtk_widget_get_కిటికీ(gui.drawarea));
}

#ఒకవేళ !GTK_CHECK_సంస్కరణ(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    స్థిర శూన్య
check_copy_area(శూన్య)
{
    GdkEvent        *event;
    పూర్ణాంకం                expose_లెక్క;
    ఒకవేళ (gui.visibility != GDK_VISIBILITY_PARTIAL)
	తిరిగి;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    చేయి
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->కిటికీ_2);
	ఒకవేళ (event == ఖాళీ)
	    ఆపు; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.వెడల్పు, event->expose.area.ఎత్తు);
	expose_లెక్క = event->expose.లెక్క;
	gdk_event_free(event);
    }
    అయితే (expose_లెక్క > 0); // more events follow
    gui_can_update_cursor();
}
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)

#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిర శూన్య
gui_gtk_surface_copy_rect(పూర్ణాంకం dest_x, పూర్ణాంకం dest_y,
			  పూర్ణాంకం src_x,  పూర్ణాంకం src_y,
			  పూర్ణాంకం వెడల్పు,  పూర్ణాంకం ఎత్తు)
{
    cairo_t * స్థిరరాశి cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    స్థిర cairo_surface_t *scroll_scratch = ఖాళీ;
    స్థిర పూర్ణాంకం scratch_w = 0, scratch_h = 0;
    పూర్ణాంకం చివరి_అడ్డువరుస = అడ్డువరుసలు - 1, చివరి_అడ్డువరుస_y = చివరి_అడ్డువరుస * gui.char_ఎత్తు;
    bool చివరి_అడ్డువరుస_overlap = (dest_y + ఎత్తు) > చివరి_అడ్డువరుస_y;
    ఒకవేళ (gui.is_wayland && ( !(State & బహుళకం_CMDLINE) || !చివరి_అడ్డువరుస_overlap) )
    {
	ఒకవేళ (dest_y < src_y) // scroll up
	{
	    cairo_సమితి_operator(cr, CAIRO_OPERATOR_ఆధారం);
	    cairo_సమితి_ఆధారం_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, వెడల్పు, ఎత్తు);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	లేకపోతే
	{   //  reuse surface when scrolling, only realloc if larger
	    ఒకవేళ (scroll_scratch == ఖాళీ || వెడల్పు > scratch_w || ఎత్తు > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), వెడల్పు, ఎత్తు);
		scratch_w = వెడల్పు, scratch_h = ఎత్తు;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_సమితి_ఆధారం_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_సమితి_operator(cr, CAIRO_OPERATOR_ఆధారం);
	    cairo_rectangle(cr, dest_x, dest_y, వెడల్పు, ఎత్తు);
	    cairo_clip(cr);
	    cairo_సమితి_ఆధారం_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    లేకపోతే
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, వెడల్పు, ఎత్తు);
	cairo_clip(cr);
	cairo_నెట్టు_సమూహం(cr);
	cairo_సమితి_ఆధారం_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_తీయి_సమూహం_గమ్యం_ఆధారం(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#అయితే_ముగింపు

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    శూన్య
gui_mch_delete_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిరరాశి పూర్ణాంకం ncols = gui.scroll_region_కుడి - gui.scroll_region_ఎడమ + 1;
    స్థిరరాశి పూర్ణాంకం nrows = gui.scroll_region_bot - అడ్డువరుస + 1;
    స్థిరరాశి పూర్ణాంకం src_nrows = nrows - అంకె_పంక్తులు;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస + అంకె_పంక్తులు),
	    gui.char_వెడల్పు * ncols + 1,     gui.char_ఎత్తు * src_nrows);
    gui_clear_ఖండం(
	    gui.scroll_region_bot - అంకె_పంక్తులు + 1, gui.scroll_region_ఎడమ,
	    gui.scroll_region_bot,		   gui.scroll_region_కుడి);
    వరుస_draw_area(
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    gui.char_వెడల్పు * ncols + 1,	gui.char_ఎత్తు * nrows);
#లేకపోతే
    ఒకవేళ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	తిరిగి;			// Can't see the window

    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
    gdk_gc_సమితి_background(gui.పాఠ్యం_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_కిటికీ_copy_area(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    gui.drawarea->కిటికీ_2,
	    FILL_X(gui.scroll_region_ఎడమ),
	    FILL_Y(అడ్డువరుస + అంకె_పంక్తులు),
	    gui.char_వెడల్పు * (gui.scroll_region_కుడి
					    - gui.scroll_region_ఎడమ + 1) + 1,
	    gui.char_ఎత్తు * (gui.scroll_region_bot - అడ్డువరుస - అంకె_పంక్తులు + 1));

    gui_clear_ఖండం(gui.scroll_region_bot - అంకె_పంక్తులు + 1,
						       gui.scroll_region_ఎడమ,
		    gui.scroll_region_bot, gui.scroll_region_కుడి);
    check_copy_area();
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    శూన్య
gui_mch_చొప్పించు_పంక్తులు(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం అంకె_పంక్తులు)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిరరాశి పూర్ణాంకం ncols = gui.scroll_region_కుడి - gui.scroll_region_ఎడమ + 1;
    స్థిరరాశి పూర్ణాంకం nrows = gui.scroll_region_bot - అడ్డువరుస + 1;
    స్థిరరాశి పూర్ణాంకం src_nrows = nrows - అంకె_పంక్తులు;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస + అంకె_పంక్తులు),
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    gui.char_వెడల్పు * ncols + 1,     gui.char_ఎత్తు * src_nrows);
    gui_clear_ఖండం(
	    అడ్డువరుస,		 gui.scroll_region_ఎడమ,
	    అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);
    వరుస_draw_area(
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    gui.char_వెడల్పు * ncols + 1,	gui.char_ఎత్తు * nrows);
#లేకపోతే
    ఒకవేళ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	తిరిగి;			// Can't see the window

    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.fgcolor);
    gdk_gc_సమితి_background(gui.పాఠ్యం_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_కిటికీ_copy_area(gui.drawarea->కిటికీ_2, gui.పాఠ్యం_gc,
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస + అంకె_పంక్తులు),
	    gui.drawarea->కిటికీ_2,
	    FILL_X(gui.scroll_region_ఎడమ), FILL_Y(అడ్డువరుస),
	    gui.char_వెడల్పు * (gui.scroll_region_కుడి
					    - gui.scroll_region_ఎడమ + 1) + 1,
	    gui.char_ఎత్తు * (gui.scroll_region_bot - అడ్డువరుస - అంకె_పంక్తులు + 1));

    gui_clear_ఖండం(అడ్డువరుస, gui.scroll_region_ఎడమ,
				అడ్డువరుస + అంకె_పంక్తులు - 1, gui.scroll_region_కుడి);
    check_copy_area();
#అయితే_ముగింపు // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    శూన్య
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	లక్ష్యం;
    గుర్తులేని	i;
    time_t	ప్రారంభం;

    కొరకు (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	ఒకవేళ (!clip_html && selection_targets[i].info == లక్ష్యం_HTML)
	    కొనసాగు;
	received_selection = RS_NONE;
	లక్ష్యం = gdk_atom_intern(selection_targets[i].లక్ష్యం, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, లక్ష్యం,
			      (guint32)GDK_CURRENT_సమయం);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	ప్రారంభం = సమయం(ఖాళీ);
	అయితే (received_selection == RS_NONE && సమయం(ఖాళీ) < ప్రారంభం + 3)
	    g_main_context_iteration(ఖాళీ, TRUE);	// wait for selection_received_cb

	ఒకవేళ (received_selection != RS_FAIL)
	    తిరిగి;
    }

    ఒకవేళ (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_కిటికీ_XDISPLAY(gtk_widget_get_కిటికీ(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    శూన్య
clip_mch_lose_selection(Clipboard_T *cbd వాడని)
{
    ఒకవేళ (లోపల_selection_clear_event)
	తిరిగి;

    gtk_selection_owner_సమితి(ఖాళీ, cbd->gtk_sel_atom, gui.event_సమయం);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    పూర్ణాంకం
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    ఒకవేళ (is_autocmd_blocked())
	తిరిగి OK;

    పూర్ణాంకం success;

    success = gtk_selection_owner_సమితి(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_సమయం);
    // don't update on every visual selection change
    ఒకవేళ (!(cbd->owned && VIsual_చురుకైన))
	gui_gtk_సమితి_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    తిరిగి (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    శూన్య
clip_mch_సమితి_selection(Clipboard_T *cbd వాడని)
{
}

#ఒకవేళ defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    పూర్ణాంకం
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    తిరిగి gdk_selection_owner_get(cbd->gtk_sel_atom) != ఖాళీ;
}
#అయితే_ముగింపు


#ఒకవేళ defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    శూన్య
gui_mch_menu_grey(vimmenu_T *menu, పూర్ణాంకం grey)
{
    ఒకవేళ (menu->id == ఖాళీ)
	తిరిగి;

    ఒకవేళ (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    ఒకవేళ (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_సమితి_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    శూన్య
gui_mch_menu_hidden(vimmenu_T *menu, పూర్ణాంకం hidden)
{
    ఒకవేళ (menu->id == 0)
	తిరిగి;

    ఒకవేళ (hidden)
    {
	ఒకవేళ (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    లేకపోతే
    {
	ఒకవేళ (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    శూన్య
gui_mch_draw_menubar(శూన్య)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#అయితే_ముగింపు // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    శూన్య
gui_mch_enable_scrollbar(scrollbar_T *sb, పూర్ణాంకం flag)
{
    ఒకవేళ (sb->id == ఖాళీ)
	తిరిగి;

    gtk_widget_సమితి_visible(sb->id, flag);
    update_కిటికీ_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    తిరిగి (long_u)pixel;
#లేకపోతే
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (గుర్తులేని దీర్ఘ)pixel, &color);

    తిరిగి (guicolor_T)(
	    (((గుర్తులేని)color.red   & 0xff00) << 8)
	 |  ((గుర్తులేని)color.green & 0xff00)
	 | (((గుర్తులేని)color.blue  & 0xff00) >> 8));
#అయితే_ముగింపు
}

/*
 * Get current mouse coordinates in text window.
 */
    శూన్య
gui_mch_getmouse(పూర్ణాంకం *x, పూర్ణాంకం *y)
{
    gui_gtk_get_సూచిక(gui.drawarea, x, y, ఖాళీ);
}

    శూన్య
gui_mch_setmouse(పూర్ణాంకం x, పూర్ణాంకం y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    ఒకవేళ (dpy)
	XWarp_సూచిక(dpy, (కిటికీ)0,
		GDK_కిటికీ_XID(gtk_widget_get_కిటికీ(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#నిర్వచితమైతే FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
స్థిర పూర్ణాంకం చివరి_shape = 0;
#అయితే_ముగింపు

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    శూన్య
gui_mch_mousehide(పూర్ణాంకం hide)
{
    ఒకవేళ (gui.సూచిక_hidden == hide)
	తిరిగి;

    gui.సూచిక_hidden = hide;
    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) && gui.blank_సూచిక != ఖాళీ)
    {
	ఒకవేళ (hide)
	    gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(gui.drawarea),
		    gui.blank_సూచిక);
	లేకపోతే
#నిర్వచితమైతే FEAT_MOUSESHAPE
	    mch_సమితి_mouse_shape(చివరి_shape);
#లేకపోతే
	gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(gui.drawarea), ఖాళీ);
#అయితే_ముగింపు
    }
}

#ఒకవేళ defined(FEAT_MOUSESHAPE)

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
స్థిర స్థిరరాశి అక్షరం * mshape_css_పేర్లు[] =
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
# లేకపోతే
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
స్థిర స్థిరరాశి పూర్ణాంకం mshape_ids[] =
{
    GDK_ఎడమ_PTR,		// arrow
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
    GDK_కుడి_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_ఎడమ_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    శూన్య
mch_సమితి_mouse_shape(పూర్ణాంకం shape)
{
    GdkCursor	   *c;
    పూర్ణాంకం		   id;                    // Only id or css_name is used.
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    స్థిరరాశి అక్షరం     *css_name = "default";
    GdkDisplay     *disp;
# endif

    ఒకవేళ (gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి;

    ఒకవేళ (shape == MSHAPE_HIDE || gui.సూచిక_hidden)
	gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(gui.drawarea),
		gui.blank_సూచిక);
    లేకపోతే
    {
	ఒకవేళ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ఒకవేళ (id >= GDK_చివరి_CURSOR)
		id = GDK_ఎడమ_PTR;
	    లేకపోతే
		id &= ~1;	// they are always even (why?)
	}
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	లేకపోతే ఒకవేళ (shape < (పూర్ణాంకం)ARRAY_నిడివి(mshape_css_పేర్లు))
	    css_name = mshape_css_పేర్లు[shape];
# లేకపోతే
	లేకపోతే ఒకవేళ (shape < (పూర్ణాంకం)ARRAY_నిడివి(mshape_ids))
	    id = mshape_ids[shape];
# endif
	లేకపోతే
	    తిరిగి;
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# లేకపోతే
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_కిటికీ_సమితి_cursor(gtk_widget_get_కిటికీ(gui.drawarea), c);
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
	g_object_unref(G_OBJECT(c));
# లేకపోతే
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    ఒకవేళ (shape != MSHAPE_HIDE)
	చివరి_shape = shape;
}
#అయితే_ముగింపు // FEAT_MOUSESHAPE


#ఒకవేళ defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_వెడల్పు  (2 * gui.char_వెడల్పు)
# define SIGN_ఎత్తు (gui.char_ఎత్తు)
# define SIGN_ASPECT ((ద్విగుణ)SIGN_ఎత్తు / (ద్విగుణ)SIGN_వెడల్పు)

    శూన్య
gui_mch_drawsign(పూర్ణాంకం అడ్డువరుస, పూర్ణాంకం నిలువు, పూర్ణాంకం typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    ఒకవేళ (sign == ఖాళీ || gui.drawarea == ఖాళీ
	    || gtk_widget_get_కిటికీ(gui.drawarea) == ఖాళీ)
	తిరిగి;

    పూర్ణాంకం వెడల్పు;
    పూర్ణాంకం ఎత్తు;
    పూర్ణాంకం xoffset;
    పూర్ణాంకం yoffset;
    పూర్ణాంకం need_scale;

    వెడల్పు  = gdk_pixbuf_get_వెడల్పు(sign);
    ఎత్తు = gdk_pixbuf_get_ఎత్తు(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (వెడల్పు > SIGN_వెడల్పు + 2
	    || ఎత్తు != SIGN_ఎత్తు
	    || (వెడల్పు < 3 * SIGN_వెడల్పు / 4
		&& ఎత్తు < 3 * SIGN_ఎత్తు / 4));
    ఒకవేళ (need_scale)
    {
	ద్విగుణ  aspect;
	పూర్ణాంకం	    w = వెడల్పు;
	పూర్ణాంకం	    h = ఎత్తు;

	// Keep the original aspect ratio
	aspect = (ద్విగుణ)ఎత్తు / (ద్విగుణ)వెడల్పు;
	వెడల్పు  = (ద్విగుణ)SIGN_వెడల్పు * SIGN_ASPECT / aspect;
	వెడల్పు  = MIN(వెడల్పు, SIGN_వెడల్పు);
	ఒకవేళ (((ద్విగుణ)(MAX(ఎత్తు, SIGN_ఎత్తు)) /
		    (ద్విగుణ)(MIN(ఎత్తు, SIGN_ఎత్తు))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    ఎత్తు = (ద్విగుణ)SIGN_ఎత్తు * SIGN_ASPECT / aspect;
	    ఎత్తు = MIN(ఎత్తు, SIGN_ఎత్తు);
	}
	లేకపోతే
	    ఎత్తు = (ద్విగుణ)వెడల్పు * aspect;

	ఒకవేళ (w == వెడల్పు && h == ఎత్తు)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	లేకపోతే
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, వెడల్పు, ఎత్తు,
		    GDK_INTERP_BILINEAR);
	    ఒకవేళ (sign == ఖాళీ)
		తిరిగి; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (వెడల్పు  - SIGN_వెడల్పు)  / 2;
    yoffset = (ఎత్తు - SIGN_ఎత్తు) / 2;

# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_వెడల్పు, SIGN_ఎత్తు);
	bg_cr = cairo_create(bg_surf);
	cairo_సమితి_ఆధారం_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_వెడల్పు, SIGN_ఎత్తు);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_సమితి_ఆధారం_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_సమితి_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_సమితి_ఆధారం_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_సమితి_ఆధారం_surface(cr, sign_surf, FILL_X(నిలువు), FILL_Y(అడ్డువరుస));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	వరుస_draw_area(
		FILL_X(నిలువు), FILL_Y(నిలువు), వెడల్పు, ఎత్తు);

    }
# లేకపోతే // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_సమితి_foreground(gui.పాఠ్యం_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->కిటికీ_2,
	    gui.పాఠ్యం_gc,
	    TRUE,
	    FILL_X(నిలువు),
	    FILL_Y(అడ్డువరుస),
	    SIGN_వెడల్పు,
	    SIGN_ఎత్తు);

    gdk_pixbuf_render_గమ్యం_drawable_alpha(sign,
	    gui.drawarea->కిటికీ_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(నిలువు) - MIN(0, xoffset),
	    FILL_Y(అడ్డువరుస) - MIN(0, yoffset),
	    MIN(వెడల్పు,	SIGN_వెడల్పు),
	    MIN(ఎత్తు, SIGN_ఎత్తు),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_సాధారణ,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    ఒకవేళ (need_scale)
	g_object_unref(sign);
}

    శూన్య *
gui_mch_రిజిస్టరు_sign(char_u *signfile)
{
    ఒకవేళ (signfile[0] != NUL && signfile[0] != '-' && gui.లోపల_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = ఖాళీ;
	char_u	    *సందేశం_2;

	sign = gdk_pixbuf_new_from_file((స్థిరరాశి అక్షరం *)signfile, &error);

	ఒకవేళ (error == ఖాళీ)
	    తిరిగి sign;

	సందేశం_2 = (char_u *)error->సందేశం_2;

	ఒకవేళ (సందేశం_2 != ఖాళీ && ఆదానం_conv.vc_type != CONV_NONE)
	    సందేశం_2 = string_convert(&ఆదానం_conv, సందేశం_2, ఖాళీ);

	ఒకవేళ (సందేశం_2 != ఖాళీ)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", సందేశం_2);

	    ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
		vim_free(సందేశం_2);
	}
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

#అయితే_ముగింపు // FEAT_SIGN_ICONS

#ఒకవేళ defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

రకనిర్వచనం నిర్మాణం
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
    స్థిర శూన్య
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->దత్తాంశం;
    uint8_t *dst;
    పూర్ణాంకం	    w = img->వెడల్పు;
    పూర్ణాంకం	    h = img->ఎత్తు;
    పూర్ణాంకం	    stride;
    పూర్ణాంకం	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_దత్తాంశం(surf);
    stride = cairo_image_surface_get_stride(surf);

    ఒకవేళ (img->fmt == IMAGE_FORMAT_RGBA)
    {
	కొరకు (y = 0; y < h; ++y)
	{
	    గుర్తులేని పూర్ణాంకం *p = (గుర్తులేని పూర్ణాంకం *)(dst + y * stride);

	    కొరకు (x = 0; x < w; ++x)
	    {
		పూర్ణాంకం	idx = (y * w + x) * 4;
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
    లేకపోతే
    {
	కొరకు (y = 0; y < h; ++y)
	{
	    గుర్తులేని పూర్ణాంకం *p = (గుర్తులేని పూర్ణాంకం *)(dst + y * stride);

	    కొరకు (x = 0; x < w; ++x)
	    {
		పూర్ణాంకం	idx = (y * w + x) * 3;
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

    పూర్ణాంకం
image_gui_init(image_T *img)
{
    // We only support GTK3
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    ఒకవేళ (ctx == ఖాళీ)
	తిరిగి FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->వెడల్పు, img->ఎత్తు);

    ఒకవేళ (cairo_surface_స్థితి(ctx->surf) != CAIRO_స్థితి_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	వెళ్ళు fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_దత్తాంశం = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    తిరిగి OK;
fail:
    vim_free(ctx);
    తిరిగి FAIL;
# లేకపోతే
    తిరిగి FAIL;
# endif
}

    శూన్య
image_gui_uninit(image_T *img)
{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    image_cairo_T *ctx = img->backend_దత్తాంశం;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    పూర్ణాంకం
image_placement_gui_init(image_placement_T *ఉంచు వాడని)
{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    తిరిగి OK;
# లేకపోతే
    తిరిగి FAIL;
# endif
}

    శూన్య
image_placement_gui_uninit(image_placement_T *ఉంచు వాడని)
{
}

    శూన్య
image_placement_gui_draw(image_placement_T *ఉంచు)
{
# ఒకవేళ GTK_CHECK_సంస్కరణ(3,0,0)
    image_T		    *img = ఉంచు->img;
    image_cairo_T	    *ictx = img->backend_దత్తాంశం;
    pixman_box32_t	    *rects;
    పూర్ణాంకం			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&ఉంచు->visible, &n_rects);
    ఒకవేళ (rects == ఖాళీ || n_rects == 0)
	తిరిగి;

    cairo_surface_సమితి_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    కొరకు (పూర్ణాంకం i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	పూర్ణాంకం		అడ్డువరుస, నిలువు;
	పూర్ణాంకం		posx, posy;
	పూర్ణాంకం		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(ఉంచు, rect, &అడ్డువరుస, &నిలువు, &x, &y, &w, &h, false);
	posx = FILL_X(నిలువు);
	posy = FILL_Y(అడ్డువరుస);

	cairo_save(cr);
	cairo_సమితి_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_సమితి_ఆధారం_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	వరుస_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    శూన్య
image_placement_gui_clear(image_placement_T *ఉంచు వాడని)
{
}

#అయితే_ముగింపు // FEAT_IMAGE
