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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
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
# ਜੇ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
ਬਾਹਰੀ ਖਾਲੀ bonobo_dock_ਵਸਤੂ_ਸੈੱਟ_behavior(Bonobo_Dock_ਵਸਤੂ *dock_ਵਸਤੂ, Bonobo_Dock_ਵਸਤੂ_Behavior beh);
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI_GTK)
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# ਨਹੀਂਤਾਂ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ਨਹੀਂਤਾਂ
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#ਜੇ_ਅੰਤ

/*
 * Easy-to-use macro for multihead support.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ GET_X_ATOM(atom)	gdk_x11_atom_ਮੰਜ਼ਿਲ_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
ਗਿਣਤੀ
{
    ਨਿਸ਼ਾਨਾ_TYPE_NONE,
    ਨਿਸ਼ਾਨਾ_UTF8_STRING,
    ਨਿਸ਼ਾਨਾ_STRING,
    ਨਿਸ਼ਾਨਾ_COMPOUND_ਪਾਠ,
    ਨਿਸ਼ਾਨਾ_HTML,
    ਨਿਸ਼ਾਨਾ_ਪਾਠ,
    ਨਿਸ਼ਾਨਾ_ਪਾਠ_URI_LIST,
    ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN,
    ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN_UTF8,
    ਨਿਸ਼ਾਨਾ_VIM_MT,
    ਨਿਸ਼ਾਨਾ_VIM,
    ਨਿਸ਼ਾਨਾ_VIMENC_MT,
    ਨਿਸ਼ਾਨਾ_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ਸਥਿਰ ਅਚਲ Gtk_ਨਿਸ਼ਾਨਾ_ਇੰਦਰਾਜ selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, ਨਿਸ਼ਾਨਾ_VIMENC},
    {VIM_ATOM_NAME,	0, ਨਿਸ਼ਾਨਾ_VIM},
    {VIMENC_MIMETYPE_NAME,  0, ਨਿਸ਼ਾਨਾ_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, ਨਿਸ਼ਾਨਾ_VIM_MT},
    {"text/html",	0, ਨਿਸ਼ਾਨਾ_HTML},
    {"UTF8_STRING",	0, ਨਿਸ਼ਾਨਾ_UTF8_STRING},
    {"COMPOUND_TEXT",	0, ਨਿਸ਼ਾਨਾ_COMPOUND_ਪਾਠ},
    {"TEXT",		0, ਨਿਸ਼ਾਨਾ_ਪਾਠ},
    {"STRING",		0, ਨਿਸ਼ਾਨਾ_STRING},
    {"text/plain;charset=utf-8", 0, ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN_UTF8},
    {"text/plain",	0, ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN}
};
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ N_SELECTION_TARGETS ARRAY_ਲੰਮਾਈ(selection_targets)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ਸਥਿਰ ਅਚਲ Gtk_ਨਿਸ਼ਾਨਾ_ਇੰਦਰਾਜ dnd_targets[] =
{
    {"text/uri-list",	0, ਨਿਸ਼ਾਨਾ_ਪਾਠ_URI_LIST},
    {"text/html",	0, ਨਿਸ਼ਾਨਾ_HTML},
    {"UTF8_STRING",	0, ਨਿਸ਼ਾਨਾ_UTF8_STRING},
    {"STRING",		0, ਨਿਸ਼ਾਨਾ_STRING},
    {"text/plain",	0, ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN}
};
# define N_DND_TARGETS ARRAY_ਲੰਮਾਈ(dnd_targets)
#ਜੇ_ਅੰਤ


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DEFAULT_FONT	"Monospace 12"

#ਜੇ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#ਜੇ_ਅੰਤ

#ਜੇ !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ਸਥਿਰ GdkAtom wm_protocols_atom = GDK_NONE;
ਸਥਿਰ GdkAtom save_yourself_atom = GDK_NONE;
#ਜੇ_ਅੰਤ

/*
 * Atoms used to control/reference X11 selections.
 */
ਸਥਿਰ GdkAtom html_atom = GDK_NONE;
ਸਥਿਰ GdkAtom utf8_string_atom = GDK_NONE;
ਸਥਿਰ GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
ਸਥਿਰ GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
ਸਥਿਰ GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
ਸਥਿਰ GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
ਸਥਿਰ ਬਣਤਰ special_ਕੁੰਜੀ
{
    guint ਕੁੰਜੀ_sym;
    char_u code0;
    char_u code1;
}
ਅਚਲ special_ਕੁੰਜੀਆਂ[] =
{
    {GDK_ਉੱਪਰ,		'k', 'u'},
    {GDK_ਥੱਲੇ,		'k', 'd'},
    {GDK_ਖੱਬਾ,		'k', 'l'},
    {GDK_ਸੱਜਾ,		'k', 'r'},
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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#ਜੇ_ਅੰਤ
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_ਪਿੱਠ_Space,	'k', 'b'},
    {GDK_ਪਾਓ,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_ਪਿੱਠ_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_ਸਿਰਾ,		'@', '7'},
    {GDK_ਪੂਰਵ,		'k', 'P'},
    {GDK_ਅਗਲਾ,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_ਖੱਬਾ,	'k', 'l'},
    {GDK_KP_ਸੱਜਾ,	'k', 'r'},
    {GDK_KP_ਉੱਪਰ,		'k', 'u'},
    {GDK_KP_ਥੱਲੇ,	'k', 'd'},
    {GDK_KP_ਪਾਓ,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_ਸਿਰਾ,	'K', '4'},
    {GDK_KP_ਪੂਰਵ,	'K', '3'},  // page up
    {GDK_KP_ਅਗਲਾ,	'K', '5'},  // page down

    {GDK_KP_ਜੋੜੋ,	'K', '6'},
    {GDK_KP_ਘਟਾਓ,	'K', '7'},
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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_FONT	1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_GEOMETRY	2
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_ਉਲਟ	3
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_NOREVERSE	4
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_BACKGROUND	5
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_FOREGROUND	6
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_ICONIC	7
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_ROLE	8
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_NETBEANS	9
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_XRM		10	// ignored
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_MENUFONT	11	// ignored
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_ਸੂਚਕਾਂਕ_ਨਕਾਬ	0x00ff
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_HAS_ਮੁੱਲ	0x0100	// a value is expected after the argument
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    ਅਚਲ ਅੱਖਰ	    *name;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    flags;
}
cmdline_option_T;

ਸਥਿਰ ਅਚਲ cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_ਮੁੱਲ},
    {"-font",		ARG_FONT|ARG_HAS_ਮੁੱਲ},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_ਮੁੱਲ},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_ਮੁੱਲ},
    {"-rv",		ARG_ਉਲਟ},
    {"-reverse",	ARG_ਉਲਟ},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_ਮੁੱਲ},
    {"-background",	ARG_BACKGROUND|ARG_HAS_ਮੁੱਲ},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_ਮੁੱਲ},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_ਮੁੱਲ},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_ਮੁੱਲ},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_ਮੁੱਲ},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_ਮੁੱਲ},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_ਮੁੱਲ},	// not implemented
#ਜੇ_ਅੰਤ
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_ਮੁੱਲ},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# ਜੇ 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#ਜੇ_ਅੰਤ
    {ਨਿਰਾ, 0}
};

ਸਥਿਰ ਪੂਰਨ    gui_argc = 0;
ਸਥਿਰ ਅੱਖਰ **gui_argv = ਨਿਰਾ;

ਸਥਿਰ ਅਚਲ ਅੱਖਰ *role_ਦਲੀਲ = ਨਿਰਾ;
#ਜੇ defined(USE_GNOME_SESSION)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ *restart_command = ਨਿਰਾ;
ਸਥਿਰ       ਅੱਖਰ *abs_restart_command = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ ਮਿਲਿਆ_iconic_arg = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ਸਥਿਰ ਪੂਰਨ using_gnome = 0;
#ਨਹੀਂਤਾਂ
# define using_gnome 0
#ਜੇ_ਅੰਤ

// Comment out the following line to ignore code for resize history tracking.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRACK_RESIZE_HISTORY
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# ਜੇ 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ resize_history {
    ਪੂਰਨ ਵਰਤਿਆ;		// If true, can't match for discard. Only matches once.
    ਪੂਰਨ ਚੌੜਾਈ;
    ਪੂਰਨ ਉਚਾਈ;
    ਪੂਰਨ from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ਪੂਰਨ seq;		// for ch_log messages
# endif
    ਬਣਤਰ resize_history *ਅਗਲਾ;
} resize_hist_T;

// never NULL during execution
ਸਥਿਰ resize_hist_T *latest_resize_hist;
// list of stale resize requests
ਸਥਿਰ resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ਸਥਿਰ ਪੂਰਨ mch_csd_ਚੌੜਾਈ = 0;
ਸਥਿਰ ਪੂਰਨ mch_csd_ਉਚਾਈ = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
ਸਥਿਰ ਪੂਰਨ mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    ਸਥਿਰ ਖਾਲੀ
alloc_resize_hist(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ, ਪੂਰਨ from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->ਚੌੜਾਈ = ਚੌੜਾਈ;
    new_hist->ਉਚਾਈ = ਉਚਾਈ;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->ਅਗਲਾ = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(ਨਿਰਾ, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, ਚੌੜਾਈ, ਉਚਾਈ, (ਪੂਰਨ)Columns, (ਪੂਰਨ)ਪੰਕਤੀਆਂ);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    ਸਥਿਰ ਖਾਲੀ
clear_resize_hists(ਖਾਲੀ)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ਪੂਰਨ		    i = 0;
# endif

    ਜੇ (latest_resize_hist)
	latest_resize_hist->ਵਰਤਿਆ = TRUE;
    ਜਦੋਂ (old_resize_hists != ਨਿਰਾ)
    {
	resize_hist_T *ਅਗਲਾ_hist = old_resize_hists->ਅਗਲਾ;

	vim_free(old_resize_hists);
	old_resize_hists = ਅਗਲਾ_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ਨਿਰਾ, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define ਮੇਲ_ਚੌੜਾਈ_ਉਚਾਈ(hist, w, h) \
	  (!hist->ਵਰਤਿਆ && hist->ਚੌੜਾਈ == w && hist->ਉਚਾਈ == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ਸਥਿਰ ਪੂਰਨ
ਮੇਲ_stale_ਚੌੜਾਈ_ਉਚਾਈ(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    resize_hist_T *hist = old_resize_hists;

    ਲਈ (hist = old_resize_hists; hist != ਨਿਰਾ; hist = hist->ਅਗਲਾ)
	ਜੇ (ਮੇਲ_ਚੌੜਾਈ_ਉਚਾਈ(hist, ਚੌੜਾਈ, ਉਚਾਈ))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(ਨਿਰਾ, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->ਵਰਤਿਆ = TRUE;
	    ਵਾਪਸ TRUE;
	}
    ਵਾਪਸ FALSE;
}

# ਜੇ defined(EXITFREE)
    ਸਥਿਰ ਖਾਲੀ
free_all_resize_hist(ਖਾਲੀ)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#ਜੇ_ਅੰਤ

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
ਸਥਿਰ guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ਖਾਲੀ
gui_mch_prepare(ਪੂਰਨ *argc, ਅੱਖਰ **argv)
{
    ਅਚਲ cmdline_option_T  *option;
    ਪੂਰਨ			    i	= 0;
    ਪੂਰਨ			    len = 0;

#ਜੇ defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    ਜੇ (strchr(argv[0], G_DIR_SEPARATOR) != ਨਿਰਾ)
    {
	char_u buf[MAXPATHL];

	ਜੇ (mch_ਭਰਿਆ_Name((char_u *)argv[0], buf, (ਪੂਰਨ)ਆਕਾਰ(buf), TRUE) == OK)
	{
	    abs_restart_command = (ਅੱਖਰ *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#ਜੇ_ਅੰਤ

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(ਅੱਖਰ *, *argc + 1);

    g_return_if_fail(gui_argv != ਨਿਰਾ);

    gui_argv[gui_argc++] = argv[i++];

    ਜਦੋਂ (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	ਜੇ (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    ਜਾਰੀ;
	}

	// Look for argv[i] in cmdline_options[] table.
	ਲਈ (option = &cmdline_options[0]; option->name != ਨਿਰਾ; ++option)
	{
	    len = ਲੜੀ_ਲੰਬਾਈ(option->name);

	    ਜੇ (ਲੜੀ_ਤੁਲਨਾ(argv[i], option->name, len) == 0)
	    {
		ਜੇ (argv[i][len] == '\0')
		    ਰੋਕੋ;
		// allow --foo=bar style
		ਜੇ (argv[i][len] == '=' && (option->flags & ARG_HAS_ਮੁੱਲ))
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		ਜੇ (vim_strchr((char_u *)":=", argv[i][len]) != ਨਿਰਾ
			&& (option->flags & ARG_ਸੂਚਕਾਂਕ_ਨਕਾਬ) == ARG_NETBEANS)
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (ਅੱਖਰ *)option->name;
		ਰੋਕੋ;
	    }
	}
	ਜੇ (option->name == ਨਿਰਾ) // no match
	{
	    ++i;
	    ਜਾਰੀ;
	}

	ਜੇ (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	ਨਹੀਂਤਾਂ
	{
	    ਅੱਖਰ *ਮੁੱਲ = ਨਿਰਾ;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    ਜੇ (option->flags & ARG_HAS_ਮੁੱਲ)
	    {
		ਜੇ (argv[i][len] == '=')
		    ਮੁੱਲ = &argv[i][len + 1];
		ਨਹੀਂਤਾਂ ਜੇ (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    ਮੁੱਲ = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    ਚੋਣ (option->flags & ARG_ਸੂਚਕਾਂਕ_ਨਕਾਬ)
	    {
		ਹਾਲਤ ARG_ਉਲਟ:
		    ਮਿਲਿਆ_ਉਲਟ_arg = TRUE;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_NOREVERSE:
		    ਮਿਲਿਆ_ਉਲਟ_arg = FALSE;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_FONT:
		    font_ਦਲੀਲ = ਮੁੱਲ;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_GEOMETRY:
		    ਜੇ (ਮੁੱਲ != ਨਿਰਾ)
			gui.geom = vim_strsave((char_u *)ਮੁੱਲ);
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_BACKGROUND:
		    background_ਦਲੀਲ = ਮੁੱਲ;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_FOREGROUND:
		    foreground_ਦਲੀਲ = ਮੁੱਲ;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_ICONIC:
		    ਮਿਲਿਆ_iconic_arg = TRUE;
		    ਰੋਕੋ;
		ਹਾਲਤ ARG_ROLE:
		    role_ਦਲੀਲ = ਮੁੱਲ; // used later in gui_mch_open()
		    ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
		ਹਾਲਤ ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
		ਮੂਲ:
		    ਰੋਕੋ;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	ਜੇ ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	ਜੇ (option->flags & ARG_KEEP)
	    ++i;
	ਨਹੀਂਤਾਂ
	{
	    // Remove the flag from the argument vector.
	    ਜੇ (--*argc > i)
	    {
		ਪੂਰਨ n_strip = 1;

		// Move the argument's value as well, if there is one.
		ਜੇ ((option->flags & ARG_HAS_ਮੁੱਲ)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    ਜੇ (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		ਜੇ (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * ਆਕਾਰ(ਅੱਖਰ *));
	    }
	    argv[*argc] = ਨਿਰਾ;
	}
    }

    gui_argv[gui_argc] = ਨਿਰਾ;
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
gui_mch_free_all(ਖਾਲੀ)
{
    vim_free(gui_argv);
# ਜੇ defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ਸਥਿਰ gint
visibility_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		 GdkEventVisibility *event,
		 gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    ਜੇ (gui.ਪਾਠ_gc != ਨਿਰਾ)
	gdk_gc_ਸੈੱਟ_exposures(gui.ਪਾਠ_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ gboolean
draw_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
	   cairo_t   *cr,
	   gpointer   user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ਜੇ (gui.starting)
	ਵਾਪਸ FALSE;

    ਬਾਹਰਲਾ_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    ਵਾਪਸ FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# ਨਹੀਂਤਾਂ
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    ਸਥਿਰ gboolean
scale_ਗੁਣਕ_event(GtkWidget *widget,
		   GParamSpec* pspec ਅਣਵਰਤਿਆ,
		   gpointer   user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (gui.surface != ਨਿਰਾ)
	cairo_surface_destroy(gui.surface);

    ਪੂਰਨ	    w, h;
    gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin), &w, &h);
    gui.surface = gdk_ਖਿੜਕੀ_create_similar_surface(
	    gtk_widget_get_ਖਿੜਕੀ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    ਪੂਰਨ	    usable_ਉਚਾਈ = h;
    ਜੇ (gtk_socket_id != 0)
	usable_ਉਚਾਈ -= (gui.char_ਉਚਾਈ - (gui.char_ਉਚਾਈ/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_ਉਚਾਈ);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  ਜੇ defined(FEAT_IMAGE) && GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    gui.scale = gtk_widget_get_scale_ਗੁਣਕ(widget);
#  endif

    ਵਾਪਸ TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
    ਸਥਿਰ gint
expose_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
	     GdkEventExpose *event,
	     gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ਜੇ (gui.starting)
	ਵਾਪਸ FALSE;

    ਬਾਹਰਲਾ_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.ਚੌੜਾਈ, event->area.ਉਚਾਈ);

    // Clear the border areas if needed
    ਜੇ (event->area.x < FILL_X(0))
	gdk_ਖਿੜਕੀ_clear_area(gui.drawarea->ਖਿੜਕੀ_2, 0, 0, FILL_X(0), 0);
    ਜੇ (event->area.y < FILL_Y(0))
	gdk_ਖਿੜਕੀ_clear_area(gui.drawarea->ਖਿੜਕੀ_2, 0, 0, 0, FILL_Y(0));
    ਜੇ (event->area.x > FILL_X(Columns))
	gdk_ਖਿੜਕੀ_clear_area(gui.drawarea->ਖਿੜਕੀ_2,
			      FILL_X((ਪੂਰਨ)Columns), 0, 0, 0);
    ਜੇ (event->area.y > FILL_Y(ਪੰਕਤੀਆਂ))
	gdk_ਖਿੜਕੀ_clear_area(gui.drawarea->ਖਿੜਕੀ_2, 0, FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ), 0, 0);

    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ਸਥਿਰ gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (ਪੂਰਨ)GDK_PROPERTY_NEW_ਮੁੱਲ
	    && GDK_ਖਿੜਕੀ_XID(event->ਖਿੜਕੀ_2) == comm_ਖਿੜਕੀ
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.ਖਿੜਕੀ_2 = comm_ਖਿੜਕੀ;
	xev.xproperty.state = Property_New_ਮੁੱਲ;
	serverEventProc(GDK_ਖਿੜਕੀ_XDISPLAY(gtk_widget_get_ਖਿੜਕੀ(widget)),
								      &xev, 0);
    }
    ਵਾਪਸ FALSE;
}
#ਜੇ_ਅੰਤ // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ਸਥਿਰ ਖਾਲੀ
gtk_settings_xft_dpi_ਬਦਲਿਆ_cb(GtkSettings *gtk_settings ਅਣਵਰਤਿਆ,
				    GParamSpec *pspec ਅਣਵਰਤਿਆ,
				    gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    ਜੇ (gui.ਪਾਠ_context != ਨਿਰਾ)
	g_object_unref(gui.ਪਾਠ_context);

    gui.ਪਾਠ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ਸੈੱਟ_ਅਧਾਰ_dir(gui.ਪਾਠ_context, PANGO_DIRECTION_LTR);

    ਜੇ (gui.norm_font != ਨਿਰਾ)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? ਨਿਰਾ : p_guifont, FALSE);
	gui_ਸੈੱਟ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

ਕਿਸਮਪਰਿਭਾਸ਼ਾ gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ਸਥਿਰ guint
timeout_ਜੋੜੋ(ਪੂਰਨ ਸਮਾਂ, timeout_cb_type (*callback)(gpointer), ਪੂਰਨ *flagp)
{
    ਵਾਪਸ g_timeout_ਜੋੜੋ((guint)ਸਮਾਂ, (GSourceFunc)callback, flagp);
}

    ਸਥਿਰ ਖਾਲੀ
timeout_remove(guint timer)
{
    g_ਸਰੋਤ_remove(timer);
}


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_NONE  0
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_OFF   1
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BLINK_ON    2

ਸਥਿਰ ਪੂਰਨ blink_state = BLINK_NONE;
ਸਥਿਰ long_u blink_waittime = 700;
ਸਥਿਰ long_u blink_ontime = 400;
ਸਥਿਰ long_u blink_offtime = 250;
ਸਥਿਰ guint blink_timer = 0;

    ਪੂਰਨ
gui_mch_is_blinking(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state != BLINK_NONE;
}

    ਪੂਰਨ
gui_mch_is_blink_off(ਖਾਲੀ)
{
    ਵਾਪਸ blink_state == BLINK_OFF;
}

    ਖਾਲੀ
gui_mch_ਸੈੱਟ_blinking(ਲੰਮਾ waittime, ਲੰਮਾ on, ਲੰਮਾ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ਖਾਲੀ
gui_mch_stop_blink(ਪੂਰਨ may_call_gui_update_cursor)
{
    ਜੇ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ਜੇ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

    ਸਥਿਰ timeout_cb_type
blink_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_ਜੋੜੋ(blink_offtime, blink_cb, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_ਜੋੜੋ(blink_ontime, blink_cb, ਨਿਰਾ);
    }
    gui_may_flush();

    ਵਾਪਸ FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ਖਾਲੀ
gui_mch_ਸ਼ੁਰੂ_blink(ਖਾਲੀ)
{
    ਜੇ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    ਜੇ (blink_waittime && blink_ontime && blink_offtime && gui.ਵਿੱਚ_focus)
    {
	blink_timer = timeout_ਜੋੜੋ(blink_waittime, blink_cb, ਨਿਰਾ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ਸਥਿਰ gint
enter_notify_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		   Gdk_Event_ਪਾਰ *event ਅਣਵਰਤਿਆ,
		   gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (blink_state == BLINK_NONE)
	gui_mch_ਸ਼ੁਰੂ_blink();

    // make sure keyboard input goes there
    ਜੇ (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ gint
leave_notify_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		   Gdk_Event_ਪਾਰ *event ਅਣਵਰਤਿਆ,
		   gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ gint
focus_ਵਿੱਚ_event(GtkWidget *widget,
	       GdkEventFocus *event ਅਣਵਰਤਿਆ,
	       gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DIALOG
    ਜੇ (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	gui_focus_change(TRUE);

    ਜੇ (blink_state == BLINK_NONE)
	gui_mch_ਸ਼ੁਰੂ_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    ਜੇ (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    ਵਾਪਸ TRUE;
}

    ਸਥਿਰ gint
focus_ਬਾਹਰਲਾ_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		GdkEventFocus *event ਅਣਵਰਤਿਆ,
		gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_DIALOG
    ਜੇ (gui.dialogs_ਸਰਗਰਮ > 0)
	++gui.dialog_focus_pending;
#ਜੇ_ਅੰਤ

    gui_focus_change(FALSE);

    ਜੇ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    ਵਾਪਸ TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    ਸਥਿਰ ਪੂਰਨ
keyval_ਮੰਜ਼ਿਲ_string(ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ keyval, char_u *string)
{
    ਪੂਰਨ	    len;
    guint32 uc;

    uc = gdk_keyval_ਮੰਜ਼ਿਲ_unicode(keyval);
    ਜੇ (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((ਪੂਰਨ)uc, string);
    }
    ਨਹੀਂਤਾਂ
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	ਚੋਣ (keyval)
	{
	    ਹਾਲਤ GDK_Tab: ਹਾਲਤ GDK_KP_Tab: ਹਾਲਤ GDK_ISO_ਖੱਬਾ_Tab:
		string[0] = TAB;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_Linefeed:
		string[0] = NL;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_Return: ਹਾਲਤ GDK_ISO_Enter: ਹਾਲਤ GDK_3270_Enter:
		string[0] = CAR;
		ਰੋਕੋ;
	    ਹਾਲਤ GDK_Escape:
		string[0] = ESC;
		ਰੋਕੋ;
	    ਮੂਲ:
		len = 0;
		ਰੋਕੋ;
	}
    }
    string[len] = NUL;

    ਵਾਪਸ len;
}

    ਸਥਿਰ ਪੂਰਨ
modifiers_gdk2vim(guint state)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (state & GDK_ਖਿਸਕਾਓ_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ਖਿਸਕਾਓ;
    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_CTRL;
    ਜੇ (state & GDK_MOD1_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_ALT;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,10,0)
    ਜੇ (state & GDK_META_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_META;
    ਜੇ (state & GDK_SUPER_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_CMD;
#ਨਹੀਂਤਾਂ
    ਜੇ (state & GDK_MOD4_ਨਕਾਬ)
	modifiers |= MOD_ਨਕਾਬ_CMD;
#ਜੇ_ਅੰਤ

    ਵਾਪਸ modifiers;
}

    ਸਥਿਰ ਪੂਰਨ
modifiers_gdk2mouse(guint state)
{
    ਪੂਰਨ modifiers = 0;

    ਜੇ (state & GDK_ਖਿਸਕਾਓ_ਨਕਾਬ)
	modifiers |= MOUSE_ਖਿਸਕਾਓ;
    ਜੇ (state & GDK_CONTROL_ਨਕਾਬ)
	modifiers |= MOUSE_CTRL;
    ਜੇ (state & GDK_MOD1_ਨਕਾਬ)
	modifiers |= MOUSE_ALT;

    ਵਾਪਸ modifiers;
}

/*
 * Main keyboard handler:
 */
    ਸਥਿਰ gint
ਕੁੰਜੀ_press_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		Gdk_Event_ਕੁੰਜੀ *event,
		gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	ਕੁੰਜੀ_sym;
    ਪੂਰਨ		len;
    ਪੂਰਨ		i;
    ਪੂਰਨ		modifiers;
    ਪੂਰਨ		ਕੁੰਜੀ;
    guint	state;
    char_u	*s, *d;
    ਪੂਰਨ		ctrl_ਅਗੇਤਰ_added = 0;

    gui.event_ਸਮਾਂ = event->ਸਮਾਂ;
    ਕੁੰਜੀ_sym = event->keyval;
    state = event->state;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    ਜੇ (xim_ਕਤਾਰ_ਕੁੰਜੀ_press_event(event, TRUE))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    ਜੇ (ਕੁੰਜੀ_sym == SunXK_F36 || ਕੁੰਜੀ_sym == SunXK_F37)
	len = 0;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	len = keyval_ਮੰਜ਼ਿਲ_string(ਕੁੰਜੀ_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	ਜੇ (len > 1 && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    len = convert_ਨਿਵੇਸ਼(string2, len, ਆਕਾਰ(string2));

	s = string2;
	d = string;
	ਲਈ (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    ਜੇ (d[-1] == CSI && d + 2 < string + ਆਕਾਰ(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (ਪੂਰਨ)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    ਜੇ (ਕੁੰਜੀ_sym == GDK_ISO_ਖੱਬਾ_Tab)
    {
	ਕੁੰਜੀ_sym = GDK_Tab;
	state |= GDK_ਖਿਸਕਾਓ_ਨਕਾਬ;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ਜੇ ((state & GDK_MOD1_ਨਕਾਬ)
	    && gui.menu_is_ਸਰਗਰਮ
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ਜੇ (len == 0 || len == 1)
    {
	ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	{
	    ਜੇ (special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym == ਕੁੰਜੀ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ਕੁੰਜੀਆਂ[i].code0;
		string[2] = special_ਕੁੰਜੀਆਂ[i].code1;
		len = -3;
		ਰੋਕੋ;
	    }
	}
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GDK_ਕੁੰਜੀ_dead_circumflex
    // Belgian Ctrl+[ workaround
    ਜੇ (len == 0 && ਕੁੰਜੀ_sym == GDK_ਕੁੰਜੀ_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_ਨਕਾਬ_CTRL;
	string[3] = '[';
	len = 4;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	ਵਾਪਸ TRUE;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (len == 0)   // Unrecognized key
	ਵਾਪਸ TRUE;

    // For some keys a shift modifier is translated into another key code.
    ਜੇ (len == -3)
	ਕੁੰਜੀ = ਮੰਜ਼ਿਲ_SPECIAL(string[1], string[2]);
    ਨਹੀਂਤਾਂ
    {
	string[len] = NUL;
	ਕੁੰਜੀ = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    ਕੁੰਜੀ = simplify_ਕੁੰਜੀ(ਕੁੰਜੀ, &modifiers);
    ਜੇ (ਕੁੰਜੀ == CSI)
	ਕੁੰਜੀ = K_CSI;
    ਜੇ (IS_SPECIAL(ਕੁੰਜੀ))
    {
	string[0] = CSI;
	string[1] = K_ਦੂਜਾ(ਕੁੰਜੀ);
	string[2] = K_ਤੀਜਾ(ਕੁੰਜੀ);
	len = 3;
    }
    ਨਹੀਂਤਾਂ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ਕੁੰਜੀ = may_adjust_ਕੁੰਜੀ_for_ctrl(modifiers, ਕੁੰਜੀ);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_ਖਿਸਕਾਓ_modifier(modifiers, ਕੁੰਜੀ);

	len = mb_char2bytes(ਕੁੰਜੀ, string);
    }

    ਜੇ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string2, 3);
	ਜੇ (modifiers == 0x4)
	    ctrl_ਅਗੇਤਰ_added = 1;
    }

    // Check if the key interrupts.
    {
	ਪੂਰਨ int_ch = check_for_ਰੁਕਾਵਟ(ਕੁੰਜੀ, modifiers);

	ਜੇ (int_ch != NUL)
	{
	    trash_ਨਿਵੇਸ਼_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    ਜੇ (ctrl_ਅਗੇਤਰ_added && len == 2
		    && ((ਪੂਰਨ)string[0]) == 195
		    && ((ਪੂਰਨ)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, len);

    // blank out the pointer if necessary
    ਜੇ (p_mh)
	gui_mch_mousehide(TRUE);

    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_XIM) || GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ gboolean
ਕੁੰਜੀ_release_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		  Gdk_Event_ਕੁੰਜੀ *event ਅਣਵਰਤਿਆ,
		  gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
# ਜੇ defined(FEAT_XIM)
    gui.event_ਸਮਾਂ = event->ਸਮਾਂ;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    ਵਾਪਸ xim_ਕਤਾਰ_ਕੁੰਜੀ_press_event(event, FALSE);
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ TRUE;
# endif
}
#ਜੇ_ਅੰਤ


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ਸਥਿਰ ਪੂਰਨ ਵਿੱਚ_selection_clear_event = FALSE;

    ਸਥਿਰ gint
selection_clear_event(GtkWidget		*widget ਅਣਵਰਤਿਆ,
		      GdkEventSelection	*event,
		      gpointer		user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਵਿੱਚ_selection_clear_event = TRUE;
    ਜੇ (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    ਨਹੀਂਤਾਂ
	clip_lose_selection(&clip_ਤਾਰਾ);
    ਵਿੱਚ_selection_clear_event = FALSE;

    ਵਾਪਸ TRUE;
}

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RS_NONE	0	// selection_received_cb() not called yet
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RS_OK	1	// selection_received_cb() called and OK
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ RS_FAIL	2	// selection_received_cb() called and failed
ਸਥਿਰ ਪੂਰਨ received_selection = RS_NONE;

    ਸਥਿਰ ਖਾਲੀ
selection_received_cb(GtkWidget		*widget ਅਣਵਰਤਿਆ,
		      Gtk_Selection_ਅੰਕੜੇ	*ਅੰਕੜੇ,
		      guint		ਸਮਾਂ_2 ਅਣਵਰਤਿਆ,
		      gpointer		user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    Clipboard_T	    *cbd;
    char_u	    *ਪਾਠ;
    char_u	    *tmpbuf = ਨਿਰਾ;
    guchar	    *tmpbuf_utf8 = ਨਿਰਾ;
    ਪੂਰਨ		    len;
    ਪੂਰਨ		    motion_type = MAUTO;

    ਜੇ (gtk_selection_ਅੰਕੜੇ_get_selection(ਅੰਕੜੇ) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ਨਹੀਂਤਾਂ
	cbd = &clip_ਤਾਰਾ;

    ਪਾਠ = (char_u *)gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ(ਅੰਕੜੇ);
    len = gtk_selection_ਅੰਕੜੇ_get_ਲੰਮਾਈ(ਅੰਕੜੇ);

    ਜੇ (ਪਾਠ == ਨਿਰਾ || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	ਵਾਪਸ;
    }

    ਜੇ (gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(ਅੰਕੜੇ) == vim_atom
	    || gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(ਅੰਕੜੇ) == vim_mt_atom)
    {
	motion_type = *ਪਾਠ++;
	--len;
    }
    ਨਹੀਂਤਾਂ ਜੇ (gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(ਅੰਕੜੇ) == vimenc_atom
	    || gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(ਅੰਕੜੇ) == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;

	motion_type = *ਪਾਠ++;
	--len;

	enc = ਪਾਠ;
	ਪਾਠ += STRLEN(ਪਾਠ) + 1;
	len -= ਪਾਠ - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ(&conv, enc, p_enc);
	ਜੇ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, ਪਾਠ, &len);
	    ਜੇ (tmpbuf != ਨਿਰਾ)
		ਪਾਠ = tmpbuf;
	    convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    ਨਹੀਂਤਾਂ
    {
	tmpbuf_utf8 = gtk_selection_ਅੰਕੜੇ_get_ਪਾਠ(ਅੰਕੜੇ);
	ਜੇ (tmpbuf_utf8 != ਨਿਰਾ)
	{
	    len = STRLEN(tmpbuf_utf8);
	    ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&ਨਿਵੇਸ਼_conv, tmpbuf_utf8, &len);
		ਜੇ (tmpbuf != ਨਿਰਾ)
		    ਪਾਠ = tmpbuf;
	    }
	    ਨਹੀਂਤਾਂ
		ਪਾਠ = tmpbuf_utf8;
	}
	ਨਹੀਂਤਾਂ ਜੇ (len >= 2 && ਪਾਠ[0] == 0xff && ਪਾਠ[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_ਤਿਆਰੀ_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    ਜੇ (conv.vc_type != CONV_NONE)
	    {
		ਪਾਠ += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, ਪਾਠ, &len);
		convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	    }
	    ਜੇ (tmpbuf != ਨਿਰਾ)
		ਪਾਠ = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    ਜਦੋਂ (len > 0 && ਪਾਠ[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, ਪਾਠ, (ਲੰਮਾ)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ਸਥਿਰ ਖਾਲੀ
selection_get_cb(GtkWidget	    *widget ਅਣਵਰਤਿਆ,
		 Gtk_Selection_ਅੰਕੜੇ   *selection_ਅੰਕੜੇ,
		 guint		    info,
		 guint		    ਸਮਾਂ_2 ਅਣਵਰਤਿਆ,
		 gpointer	    user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    ਪੂਰਨ		    ਲੰਮਾਈ;
    ਪੂਰਨ		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    ਜੇ (gtk_selection_ਅੰਕੜੇ_get_selection(selection_ਅੰਕੜੇ)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ਨਹੀਂਤਾਂ
	cbd = &clip_ਤਾਰਾ;

    ਜੇ (!cbd->owned)
	ਵਾਪਸ;			// Shouldn't ever happen

    ਜੇ (info != (guint)ਨਿਸ਼ਾਨਾ_STRING
	    && (!clip_html || info != (guint)ਨਿਸ਼ਾਨਾ_HTML)
	    && info != (guint)ਨਿਸ਼ਾਨਾ_UTF8_STRING
	    && info != (guint)ਨਿਸ਼ਾਨਾ_VIMENC
	    && info != (guint)ਨਿਸ਼ਾਨਾ_VIM
	    && info != (guint)ਨਿਸ਼ਾਨਾ_VIMENC_MT
	    && info != (guint)ਨਿਸ਼ਾਨਾ_VIM_MT
	    && info != (guint)ਨਿਸ਼ਾਨਾ_COMPOUND_ਪਾਠ
	    && info != (guint)ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN
	    && info != (guint)ਨਿਸ਼ਾਨਾ_ਪਾਠ_PLAIN_UTF8
	    && info != (guint)ਨਿਸ਼ਾਨਾ_ਪਾਠ)
	ਵਾਪਸ;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    ਜੇ (motion_type < 0 || string == ਨਿਰਾ)
	ਵਾਪਸ;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    ਲੰਮਾਈ = MIN(tmplen, (long_u)(G_MAXINT - 1));

    ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_VIM || info == (guint)ਨਿਸ਼ਾਨਾ_VIM_MT)
    {
	tmpbuf = alloc(ਲੰਮਾਈ + 1);
	ਜੇ (tmpbuf != ਨਿਰਾ)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)ਲੰਮਾਈ);
	}
	// For our own format, the first byte contains the motion type
	++ਲੰਮਾਈ;
	vim_free(string);
	string = tmpbuf;
	ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_VIM)
	    type = vim_atom;
	ਨਹੀਂਤਾਂ
	    type = vim_mt_atom;
    }

    ਨਹੀਂਤਾਂ ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_ਤਿਆਰੀ_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	ਜੇ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &ਲੰਮਾਈ);
	    convert_ਤਿਆਰੀ(&conv, ਨਿਰਾ, ਨਿਰਾ);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	ਜੇ (string != ਨਿਰਾ)
	{
	    tmpbuf = alloc(ਲੰਮਾਈ + 2);
	    ਜੇ (tmpbuf != ਨਿਰਾ)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)ਲੰਮਾਈ);
		vim_free(string);
		string = tmpbuf;
		ਲੰਮਾਈ += 2;
	    }

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_ਅੰਕੜੇ->type = selection_ਅੰਕੜੇ->ਨਿਸ਼ਾਨਾ;
	    selection_ਅੰਕੜੇ->format = 16;	// 16 bits per char
#ਜੇ_ਅੰਤ
	    gtk_selection_ਅੰਕੜੇ_ਸੈੱਟ(selection_ਅੰਕੜੇ, html_atom, 16,
							      string, ਲੰਮਾਈ);
	    vim_free(string);
	}
	ਵਾਪਸ;
    }
    ਨਹੀਂਤਾਂ ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_VIMENC || info == (guint)ਨਿਸ਼ਾਨਾ_VIMENC_MT)
    {
	ਪੂਰਨ l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(ਲੰਮਾਈ + l + 2);
	ਜੇ (tmpbuf != ਨਿਰਾ)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)ਲੰਮਾਈ);
	    ਲੰਮਾਈ += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_VIMENC)
	    type = vimenc_atom;
	ਨਹੀਂਤਾਂ
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &ਲੰਮਾਈ);
	    vim_free(string);
	    ਜੇ (tmpbuf == ਨਿਰਾ)
		ਵਾਪਸ;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	ਜੇ (g_utf8_validate((ਅਚਲ ਅੱਖਰ *)string, (gssize)ਲੰਮਾਈ, ਨਿਰਾ))
	{
	    gtk_selection_ਅੰਕੜੇ_ਸੈੱਟ_ਪਾਠ(selection_ਅੰਕੜੇ,
					(ਅਚਲ ਅੱਖਰ *)string, ਲੰਮਾਈ);
	}
	vim_free(string);
	ਵਾਪਸ;
    }

    ਜੇ (string != ਨਿਰਾ)
    {
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_ਅੰਕੜੇ->type = selection_ਅੰਕੜੇ->ਨਿਸ਼ਾਨਾ;
	selection_ਅੰਕੜੇ->format = 8;	// 8 bits per char
#ਜੇ_ਅੰਤ
	gtk_selection_ਅੰਕੜੇ_ਸੈੱਟ(selection_ਅੰਕੜੇ, type, 8, string, ਲੰਮਾਈ);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    ਪੂਰਨ
gui_mch_early_init_check(ਪੂਰਨ give_ਸੁਨੇਹਾ)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    ਜੇ ((p == ਨਿਰਾ || *p == NUL) && (q == ਨਿਰਾ || *q == NUL))
    {
	gui.dying = true;
	ਜੇ (give_ਸੁਨੇਹਾ)
	    emsg(_((ਅੱਖਰ *)e_cannot_open_display));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    ਪੂਰਨ
gui_mch_init_check(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GRESOURCE
    ਸਥਿਰ ਪੂਰਨ res_registered = FALSE;

    ਜੇ (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_ਰਜਿਸਟਰ_resource();
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
    ਜੇ (gtk_socket_id == 0)
	using_gnome = 1;
#ਜੇ_ਅੰਤ

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_ਸੈੱਟ_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    ਜੇ (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((ਅੱਖਰ *)e_cannot_open_display));
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


ਸਥਿਰ guint mouse_click_timer = 0;
ਸਥਿਰ ਪੂਰਨ mouse_timed_ਬਾਹਰਲਾ = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ਸਥਿਰ timeout_cb_type
mouse_click_timer_cb(gpointer ਅੰਕੜੇ)
{
    // we don't use this information currently
    ਪੂਰਨ *timed_ਬਾਹਰਲਾ = (ਪੂਰਨ *) ਅੰਕੜੇ;

    *timed_ਬਾਹਰਲਾ = TRUE;
    ਵਾਪਸ FALSE;		// don't happen again
}

ਸਥਿਰ guint		motion_repeat_timer  = 0;
ਸਥਿਰ ਪੂਰਨ		motion_repeat_ਵਿੱਥ = FALSE;
ਸਥਿਰ timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ਸਥਿਰ ਖਾਲੀ
process_motion_notify(ਪੂਰਨ x, ਪੂਰਨ y, GdkModifierType state)
{
    ਪੂਰਨ	    button;
    int_u   vim_modifiers;
    Gtk_ਵੰਡ ਵੰਡ;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_ਨਕਾਬ | GDK_BUTTON2_ਨਕਾਬ |
		       GDK_BUTTON3_ਨਕਾਬ | GDK_BUTTON4_ਨਕਾਬ |
		       GDK_BUTTON5_ਨਕਾਬ))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    ਜੇ (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	ਵਾਪਸ;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_ਵੰਡ(gui.drawarea, &ਵੰਡ);

    ਜੇ (x < 0 || y < 0
	    || x >= ਵੰਡ.ਚੌੜਾਈ
	    || y >= ਵੰਡ.ਉਚਾਈ)
    {

	ਪੂਰਨ dx;
	ਪੂਰਨ dy;
	ਪੂਰਨ offshoot;
	ਪੂਰਨ delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - ਵੰਡ.ਚੌੜਾਈ;
	dy = y < 0 ? -y : y - ਵੰਡ.ਉਚਾਈ;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	ਜੇ (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	ਨਹੀਂਤਾਂ
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	ਜੇ (!motion_repeat_timer)
	    motion_repeat_timer = timeout_ਜੋੜੋ(delay, motion_repeat_timer_cb,
									 ਨਿਰਾ);
    }
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ GdkDevice *
gui_gtk_get_ਸੂਚਕ_device(GtkWidget *widget)
{
    Gdk_ਖਿੜਕੀ * ਅਚਲ win = gtk_widget_get_ਖਿੜਕੀ(widget);
    GdkDisplay * ਅਚਲ dpy = gdk_ਖਿੜਕੀ_get_display(win);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,20,0)
    GdkSeat * ਅਚਲ seat = gdk_display_get_default_seat(dpy);
    ਵਾਪਸ gdk_seat_get_ਸੂਚਕ(seat);
# ਨਹੀਂਤਾਂ
    GdkDeviceManager * ਅਚਲ mngr = gdk_display_get_device_manager(dpy);
    ਵਾਪਸ gdk_device_manager_get_client_ਸੂਚਕ(mngr);
# endif
}

    ਸਥਿਰ Gdk_ਖਿੜਕੀ *
gui_gtk_get_ਸੂਚਕ(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_ਖਿੜਕੀ * ਅਚਲ win = gtk_widget_get_ਖਿੜਕੀ(widget);
    GdkDevice * ਅਚਲ dev = gui_gtk_get_ਸੂਚਕ_device(widget);
    ਵਾਪਸ gdk_ਖਿੜਕੀ_get_device_position(win, dev , x, y, state);
}

# ਜੇ defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ Gdk_ਖਿੜਕੀ *
gui_gtk_ਖਿੜਕੀ_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ਅਚਲ dev = gui_gtk_get_ਸੂਚਕ_device(widget);
    ਵਾਪਸ gdk_device_get_ਖਿੜਕੀ_at_position(dev, x, y);
}
# endif
#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_ਸੂਚਕ(wid, x, y, s) \
    gdk_ਖਿੜਕੀ_get_ਸੂਚਕ((wid)->ਖਿੜਕੀ_2, x, y, s)
# define gui_gtk_ਖਿੜਕੀ_at_position(wid, x, y)	gdk_ਖਿੜਕੀ_at_ਸੂਚਕ(x, y)
#ਜੇ_ਅੰਤ

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ਸਥਿਰ timeout_cb_type
motion_repeat_timer_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		    x;
    ਪੂਰਨ		    y;
    GdkModifierType state;

    gui_gtk_get_ਸੂਚਕ(gui.drawarea, &x, &y, &state);

    ਜੇ (!(state & (GDK_BUTTON1_ਨਕਾਬ | GDK_BUTTON2_ਨਕਾਬ |
		   GDK_BUTTON3_ਨਕਾਬ | GDK_BUTTON4_ਨਕਾਬ |
		   GDK_BUTTON5_ਨਕਾਬ)))
    {
	motion_repeat_timer = 0;
	ਵਾਪਸ FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ਜੇ (vim_ਵਰਤਿਆ_ਵਿੱਚ_ਨਿਵੇਸ਼_buf() > 10)
	ਵਾਪਸ TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    ਜੇ (motion_repeat_ਵਿੱਥ)
	x += gui.char_ਚੌੜਾਈ;

    motion_repeat_ਵਿੱਥ = !motion_repeat_ਵਿੱਥ;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (event->is_hint)
    {
	ਪੂਰਨ		x;
	ਪੂਰਨ		y;
	GdkModifierType	state;

	gui_gtk_get_ਸੂਚਕ(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    ਨਹੀਂਤਾਂ
    {
	process_motion_notify((ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
			      (GdkModifierType)event->state);
    }

    ਵਾਪਸ TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    ਸਥਿਰ gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ button;
    ਪੂਰਨ repeated_click = FALSE;
    ਪੂਰਨ x, y;
    int_u vim_modifiers;

    gui.event_ਸਮਾਂ = event->ਸਮਾਂ;

    // Make sure we have focus now we've been selected
    ਜੇ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    ਜੇ (event->type != GDK_BUTTON_PRESS)
	ਵਾਪਸ FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    ਜੇ (!mouse_timed_ਬਾਹਰਲਾ && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_ਬਾਹਰਲਾ = FALSE;
    mouse_click_timer = timeout_ਜੋੜੋ(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_ਬਾਹਰਲਾ);

    ਚੋਣ (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	ਹਾਲਤ 1:
		button = MOUSE_ਖੱਬਾ;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_ਨਕਾਬ;
		ਰੋਕੋ;
	ਹਾਲਤ 2: button = MOUSE_ਵਿਚਕਾਰ; ਰੋਕੋ;
	ਹਾਲਤ 3: button = MOUSE_ਸੱਜਾ; ਰੋਕੋ;
	ਹਾਲਤ 8: button = MOUSE_X1; ਰੋਕੋ;
	ਹਾਲਤ 9: button = MOUSE_X2; ਰੋਕੋ;
	ਮੂਲ:
	    ਵਾਪਸ FALSE;		// Unknown button
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    // cancel any preediting
    ਜੇ (im_is_preediting())
	xim_reset();
#ਜੇ_ਅੰਤ

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    ਵਾਪਸ TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    ਸਥਿਰ gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    ਸਥਿਰ ਦੁੱਗਣਾ  acc_x, acc_y;
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,22,0)
    ਸਥਿਰ guint32 ਆਖਰੀ_smooth_event_ਸਮਾਂ;
# endif
#ਜੇ_ਅੰਤ

    ਜੇ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    ਚੋਣ (event->direction)
    {
	ਹਾਲਤ GDK_SCROLL_ਉੱਪਰ:
	    button = MOUSE_4;
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_SCROLL_ਥੱਲੇ:
	    button = MOUSE_5;
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_SCROLL_ਖੱਬਾ:
	    button = MOUSE_7;
	    ਰੋਕੋ;
	ਹਾਲਤ GDK_SCROLL_ਸੱਜਾ:
	    button = MOUSE_6;
	    ਰੋਕੋ;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
	ਹਾਲਤ GDK_SCROLL_SMOOTH:
	    ਜੇ (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		ਵਾਪਸ FALSE;
	    }
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,0)
	    ਜੇ (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# ਨਹੀਂਤਾਂ
	    ਜੇ (event->ਸਮਾਂ - ਆਖਰੀ_smooth_event_ਸਮਾਂ > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,22,0)
	    ਆਖਰੀ_smooth_event_ਸਮਾਂ = event->ਸਮਾਂ;
# endif
	    ਰੋਕੋ;
#ਜੇ_ਅੰਤ
	ਮੂਲ: // This shouldn't happen
	    ਵਾਪਸ FALSE;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    // cancel any preediting
    ਜੇ (im_is_preediting())
	xim_reset();
#ਜੇ_ਅੰਤ

    vim_modifiers = modifiers_gdk2mouse(event->state);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    ਜੇ (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	ਜਦੋਂ (acc_x >= 1.0)
	{ //ਸੱਜਾ
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
		    FALSE, vim_modifiers);
	}
	ਜਦੋਂ (acc_x <= -1.0)
	{ //ਖੱਬਾ
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
		    FALSE, vim_modifiers);
	}
	ਜਦੋਂ (acc_y >= 1.0)
	{ //ਥੱਲੇ
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
		    FALSE, vim_modifiers);
	}
	ਜਦੋਂ (acc_y <= -1.0)
	{ //ਉੱਪਰ
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
		    FALSE, vim_modifiers);
	}
    }
    ਨਹੀਂਤਾਂ
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    ਜੇ (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	gui_send_mouse_event(button, (ਪੂਰਨ)event->x, (ਪੂਰਨ)event->y,
		FALSE, vim_modifiers);

    ਵਾਪਸ TRUE;
}


    ਸਥਿਰ gint
button_release_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		     GdkEventButton *event,
		     gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ x, y;
    int_u vim_modifiers;

    gui.event_ਸਮਾਂ = event->ਸਮਾਂ;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    ਜੇ (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    ਚੋਣ (event->button)
    {
	ਹਾਲਤ 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    ਰੋਕੋ;
    }

    ਵਾਪਸ TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    ਸਥਿਰ ਖਾਲੀ
grab_notify_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		  gboolean  was_grabbed,
		  gpointer  ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!was_grabbed)
	dragging_button_state = 0;
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_and_decode_uri_list(char_u *ਬਾਹਰਲਾ, char_u *raw, ਪੂਰਨ len)
{
    ਪੂਰਨ		i;
    char_u	*p = ਬਾਹਰਲਾ;
    ਪੂਰਨ		ਗਿਣਤੀ_2 = 0;

    ਲਈ (i = 0; i < len; ++i)
    {
	ਜੇ (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    ਜੇ (p > ਬਾਹਰਲਾ && p[-1] != NUL)
	    {
		++ਗਿਣਤੀ_2;
		*p++ = NUL;
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	ਨਹੀਂਤਾਂ
	    *p++ = raw[i];
    }
    ਜੇ (p > ਬਾਹਰਲਾ && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++ਗਿਣਤੀ_2;
    }
    ਵਾਪਸ ਗਿਣਤੀ_2;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    ਸਥਿਰ ਪੂਰਨ
filter_uri_list(char_u **outlist, ਪੂਰਨ max, char_u *src)
{
    ਪੂਰਨ	i, j;

    ਲਈ (i = j = 0; i < max; ++i)
    {
	outlist[i] = ਨਿਰਾ;
	ਜੇ (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    ਜੇ (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    ਜਦੋਂ (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    ਵਾਪਸ j;
}

    ਸਥਿਰ char_u **
parse_uri_list(ਪੂਰਨ *ਗਿਣਤੀ_2, char_u *ਅੰਕੜੇ, ਪੂਰਨ len)
{
    ਪੂਰਨ	    n	    = 0;
    char_u  *tmp    = ਨਿਰਾ;
    char_u  **array = ਨਿਰਾ;

    ਜੇ (ਅੰਕੜੇ != ਨਿਰਾ && len > 0 && (tmp = alloc(len + 1)) != ਨਿਰਾ)
    {
	n = ਗਿਣਤੀ_and_decode_uri_list(tmp, ਅੰਕੜੇ, len);
	ਜੇ (n > 0 && (array = ALLOC_MULT(char_u *, n)) != ਨਿਰਾ)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *ਗਿਣਤੀ_2 = n;
    ਵਾਪਸ array;
}

    ਸਥਿਰ ਖਾਲੀ
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_ਅੰਕੜੇ	*ਅੰਕੜੇ,
		     guint		ਸਮਾਂ_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    ਪੂਰਨ	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ(ਅੰਕੜੇ),
	    gtk_selection_ਅੰਕੜੇ_get_ਲੰਮਾਈ(ਅੰਕੜੇ));

    ਜੇ (fnames != ਨਿਰਾ && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_ਅੰਤ(context, TRUE, FALSE, ਸਮਾਂ_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    ਨਹੀਂਤਾਂ
	vim_free(fnames);
}

    ਸਥਿਰ ਖਾਲੀ
drag_handle_ਪਾਠ(GdkDragContext	    *context,
		 Gtk_Selection_ਅੰਕੜੇ   *ਅੰਕੜੇ,
		 guint		    ਸਮਾਂ_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *ਪਾਠ;
    ਪੂਰਨ	    len;
    char_u  *tmpbuf = ਨਿਰਾ;

    ਪਾਠ = (char_u *)gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ(ਅੰਕੜੇ);
    len = gtk_selection_ਅੰਕੜੇ_get_ਲੰਮਾਈ(ਅੰਕੜੇ);

    ਜੇ (gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ_type(ਅੰਕੜੇ) == utf8_string_atom)
    {
	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&ਨਿਵੇਸ਼_conv, ਪਾਠ, &len);
	ਜੇ (tmpbuf != ਨਿਰਾ)
	    ਪਾਠ = tmpbuf;
    }

    dnd_yank_drag_ਅੰਕੜੇ(ਪਾਠ, (ਲੰਮਾ)len);
    gtk_drag_ਅੰਤ(context, TRUE, FALSE, ਸਮਾਂ_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    ਜੇ (dropkey[2] != 0)
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(dropkey, (ਪੂਰਨ)ਆਕਾਰ(dropkey));
    ਨਹੀਂਤਾਂ
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(dropkey + 3, (ਪੂਰਨ)(ਆਕਾਰ(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ਸਥਿਰ ਖਾਲੀ
drag_ਅੰਕੜੇ_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_ਅੰਕੜੇ	*ਅੰਕੜੇ,
		      guint		info,
		      guint		ਸਮਾਂ_2,
		      gpointer		user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    GdkModifierType state;

    // Guard against trash
    ਅਚਲ guchar * ਅਚਲ ਅੰਕੜੇ_ਅੰਕੜੇ = gtk_selection_ਅੰਕੜੇ_get_ਅੰਕੜੇ(ਅੰਕੜੇ);
    ਅਚਲ gint ਅੰਕੜੇ_ਲੰਮਾਈ = gtk_selection_ਅੰਕੜੇ_get_ਲੰਮਾਈ(ਅੰਕੜੇ);
    ਅਚਲ gint ਅੰਕੜੇ_format = gtk_selection_ਅੰਕੜੇ_get_format(ਅੰਕੜੇ);

    ਜੇ (ਅੰਕੜੇ_ਅੰਕੜੇ == ਨਿਰਾ
	    || ਅੰਕੜੇ_ਲੰਮਾਈ <= 0
	    || ਅੰਕੜੇ_format != 8
	    || ਅੰਕੜੇ_ਅੰਕੜੇ[ਅੰਕੜੇ_ਲੰਮਾਈ] != '\0')
    {
	gtk_drag_ਅੰਤ(context, FALSE, FALSE, ਸਮਾਂ_2);
	ਵਾਪਸ;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_ਸੂਚਕ(widget, ਨਿਰਾ, ਨਿਰਾ, &state);

    // Not sure about the role of "text/plain" here...
    ਜੇ (info == (guint)ਨਿਸ਼ਾਨਾ_ਪਾਠ_URI_LIST)
	drag_handle_uri_list(context, ਅੰਕੜੇ, ਸਮਾਂ_2, state, x, y);
    ਨਹੀਂਤਾਂ
	drag_handle_ਪਾਠ(context, ਅੰਕੜੇ, ਸਮਾਂ_2, state);

}
#ਜੇ_ਅੰਤ // FEAT_DND


#ਜੇ defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ਸਥਿਰ ਖਾਲੀ
sm_client_check_ਬਦਲਿਆ_any(GnomeClient	    *client ਅਣਵਰਤਿਆ,
			    gint	    ਕੁੰਜੀ,
			    GnomeDialogType type ਅਣਵਰਤਿਆ,
			    gpointer	    ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ਜੇ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_ਬਦਲਿਆ_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    ਬਾਹਰਲਾ_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_ਕੁੰਜੀ_return(ਕੁੰਜੀ, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    ਸਥਿਰ gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase ਅਣਵਰਤਿਆ,
			GnomeSaveStyle	    save_style ਅਣਵਰਤਿਆ,
			gboolean	    shutdown ਅਣਵਰਤਿਆ,
			GnomeInteractStyle  interact_style,
			gboolean	    fast ਅਣਵਰਤਿਆ,
			gpointer	    ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ	suffix[] = "-session.vim";
    ਅੱਖਰ		*session_file;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    ਜੇ (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_ਸਾਧਾਰਨ,
					 &sm_client_check_ਬਦਲਿਆ_any,
					 ਨਿਰਾ);
    ਬਾਹਰਲਾ_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_ਰਸਤਾ(
			gnome_client_get_config_ਅਗੇਤਰ(client));
    len = ਲੜੀ_ਲੰਬਾਈ(session_file);

    ਜੇ (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(ਅੱਖਰ, session_file, len + ਆਕਾਰ(suffix));
    ਸਮ੍ਰਿਤੀ_ਨਕਲ(session_file + len, suffix, ਆਕਾਰ(suffix));

    success = write_session_file((char_u *)session_file);

    ਜੇ (success)
    {
	ਅਚਲ ਅੱਖਰ  *argv[8];
	ਪੂਰਨ	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = ਨਿਰਾ;

	gnome_client_ਸੈੱਟ_discard_command(client, i, (ਅੱਖਰ **)argv);

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
	argv[i++] = gtk_ਖਿੜਕੀ_get_role(GTK_ਖਿੜਕੀ(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = ਨਿਰਾ;

	gnome_client_ਸੈੱਟ_restart_command(client, i, (ਅੱਖਰ **)argv);
	gnome_client_ਸੈੱਟ_clone_command(client, 0, ਨਿਰਾ);
    }

    g_free(session_file);

    ਵਾਪਸ success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    ਸਥਿਰ ਖਾਲੀ
sm_client_die(GnomeClient *client ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Don't write messages to the GUI anymore
    ਭਰਿਆ_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_save_yourself(ਖਾਲੀ)
{
    GnomeClient *client;

    client = gnome_master_client();
    ਜੇ (client == ਨਿਰਾ)
	ਵਾਪਸ;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), ਨਿਰਾ);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), ਨਿਰਾ);
}

#ਨਹੀਂਤਾਂ // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    ਸਥਿਰ gboolean
local_xsmp_handle_ਬੇਨਤੀਆਂ(
    GIOChannel		*ਸਰੋਤ ਅਣਵਰਤਿਆ,
    GIOCondition	condition,
    gpointer		ਅੰਕੜੇ)
{
    ਜੇ (condition == G_IO_ਵਿੱਚ)
    {
	// Do stuff; maybe close connection
	ਜੇ (xsmp_handle_ਬੇਨਤੀਆਂ() == FAIL)
	    g_io_channel_unref((GIOChannel *)ਅੰਕੜੇ);
	ਵਾਪਸ TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)ਅੰਕੜੇ);
    xsmp_close();
    ਵਾਪਸ TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_save_yourself(ਖਾਲੀ)
{
    Atom    *existing_atoms = ਨਿਰਾ;
    ਪੂਰਨ	    ਗਿਣਤੀ_2 = 0;

# ifdef USE_XSMP
    ਜੇ (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_ਜੋੜੋ_watch(g_io, G_IO_ਵਿੱਚ | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_ਬੇਨਤੀਆਂ, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	ਜੇ (!dpy)
	    ਵਾਪਸ;

	Gdk_ਖਿੜਕੀ * ਅਚਲ mainwin_win = gtk_widget_get_ਖਿੜਕੀ(gui.mainwin);
	ਜੇ (XGetWMProtocols(dpy, GDK_ਖਿੜਕੀ_XID(mainwin_win),
		    &existing_atoms, &ਗਿਣਤੀ_2))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    ਪੂਰਨ	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    ਲਈ (i = 0; i < ਗਿਣਤੀ_2; ++i)
		ਜੇ (existing_atoms[i] == save_yourself_xatom)
		    ਰੋਕੋ;

	    ਜੇ (i == ਗਿਣਤੀ_2)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, ਗਿਣਤੀ_2 + 1);
		ਜੇ (new_atoms != ਨਿਰਾ)
		{
		    ਸਮ੍ਰਿਤੀ_ਨਕਲ(new_atoms, existing_atoms, ਗਿਣਤੀ_2 * ਆਕਾਰ(Atom));
		    new_atoms[ਗਿਣਤੀ_2] = save_yourself_xatom;
		    XSetWMProtocols(GDK_ਖਿੜਕੀ_XDISPLAY(mainwin_win),
			    GDK_ਖਿੜਕੀ_XID(mainwin_win),
			    new_atoms, ਗਿਣਤੀ_2 + 1);
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
    ਸਥਿਰ GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event ਅਣਵਰਤਿਆ,
		    gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    XEvent *xevent = (XEvent *)xev;

    ਜੇ (xevent != ਨਿਰਾ
	    && xevent->type == Client_ਸੁਨੇਹਾ
	    && xevent->xclient.ਸੁਨੇਹਾ_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.ਅੰਕੜੇ.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	ਬਾਹਰਲਾ_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_ਖਿੜਕੀ_XDISPLAY(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin)),
		    GDK_ਖਿੜਕੀ_XID(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin)),
		    ਨਿਰਾ, 0);
	ਵਾਪਸ GDK_FILTER_REMOVE;
    }

    ਵਾਪਸ GDK_FILTER_CONTINUE;
}
#ਜੇ_ਅੰਤ // !USE_GNOME_SESSION

    ਸਥਿਰ GdkPixbuf *
pixbuf_new_from_png_ਅੰਕੜੇ(ਅਚਲ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ *ਅੰਕੜੇ, ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ len)
{
    GInput_ਧਾਰਾ *ਧਾਰਾ;
    GdkPixbuf    *pixbuf;

    ਧਾਰਾ = g_ਸਮ੍ਰਿਤੀ_ਨਿਵੇਸ਼_ਧਾਰਾ_new_from_ਅੰਕੜੇ(ਅੰਕੜੇ, len, ਨਿਰਾ);
    pixbuf = gdk_pixbuf_new_from_ਧਾਰਾ(ਧਾਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    g_object_unref(ਧਾਰਾ);
    ਵਾਪਸ pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ਸਥਿਰ ਖਾਲੀ
mainwin_realize(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
#ਅਨ੍ਤਰ੍ਭਾਵ "../runtime/vim16x16_png.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../runtime/vim32x32_png.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "../runtime/vim48x48_png.h"

    Gdk_ਖਿੜਕੀ * ਅਚਲ mainwin_win = gtk_widget_get_ਖਿੜਕੀ(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    ਜੇ (echo_wid_arg)
    {
	ਜੇ (gui_mch_get_display())
	    ਲਿਖੋ("WID: %ld\n", (ਲੰਮਾ)GDK_ਖਿੜਕੀ_XID(mainwin_win));
	ਨਹੀਂਤਾਂ
	    ਲਿਖੋ("WID: 0\n");
	fflush(stdout);
    }

    ਜੇ (vim_strchr(p_go, GO_ICON) != ਨਿਰਾ)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	ਜੇ (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_ਖਿੜਕੀ_ਸੈੱਟ_icon_name(GTK_ਖਿੜਕੀ(gui.mainwin), "gvim");
	ਨਹੀਂਤਾਂ
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = ਨਿਰਾ;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_ਅੰਕੜੇ(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ਅੰਕੜੇ(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ਅੰਕੜੇ(vim48x48_png, vim48x48_png_len));

	    gtk_ਖਿੜਕੀ_ਸੈੱਟ_icon_list(GTK_ਖਿੜਕੀ(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(ਖਾਲੀ *)&g_object_unref, ਨਿਰਾ);
	    g_list_free(icons);
	}
    }

#ਜੇ !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_ਖਿੜਕੀ_ਜੋੜੋ_filter(ਨਿਰਾ, &global_event_filter, ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#ਜੇ defined(USE_GNOME_SESSION)
    ਜੇ (using_gnome)
#ਜੇ_ਅੰਤ
	ਤਿਆਰੀ_save_yourself();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CLIENTSERVER
    ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 && gui_mch_get_display())
    {
	ਜੇ (serverName == ਨਿਰਾ && server_Delayed_ਸ਼ੁਰੂ_Name != ਨਿਰਾ)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_ਖਿੜਕੀ = GDK_ਖਿੜਕੀ_XID(mainwin_win);

	    (ਖਾਲੀ)server_ਰਜਿਸਟਰ_Name(GDK_ਖਿੜਕੀ_XDISPLAY(mainwin_win),
				    server_Delayed_ਸ਼ੁਰੂ_Name);
	}
	ਨਹੀਂਤਾਂ
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_ਖਿੜਕੀ(GDK_ਖਿੜਕੀ_XDISPLAY(mainwin_win),
					GDK_ਖਿੜਕੀ_XID(mainwin_win));
	}
	gtk_widget_ਜੋੜੋ_events(gui.mainwin, GDK_PROPERTY_CHANGE_ਨਕਾਬ);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), ਨਿਰਾ);
    }
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ GdkCursor *
create_blank_ਸੂਚਕ(ਖਾਲੀ)
{
    GdkCursor	*cursor;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#ਨਹੀਂਤਾਂ
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_ਖਿੜਕੀ	*ਜੜ੍ਹ_ਖਿੜਕੀ = ਨਿਰਾ;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_ਨਕਾਬ;
    ਅੱਖਰ	blank_ਅੰਕੜੇ[] = { 0x0 };
    ਜੜ੍ਹ_ਖਿੜਕੀ = gtk_widget_get_ਜੜ੍ਹ_ਖਿੜਕੀ(gui.mainwin);
    blank_ਨਕਾਬ = gdk_bitmap_create_from_ਅੰਕੜੇ(ਜੜ੍ਹ_ਖਿੜਕੀ, blank_ਅੰਕੜੇ, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_ਨਕਾਬ, blank_ਨਕਾਬ,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_ਨਕਾਬ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ cursor;
}

    ਸਥਿਰ ਖਾਲੀ
mainwin_screen_ਬਦਲਿਆ_cb(GtkWidget  *widget,
			  GdkScreen  *ਪਿਛਲਾ_screen ਅਣਵਰਤਿਆ,
			  gpointer   ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!gtk_widget_has_screen(widget))
	ਵਾਪਸ;

    /*
     * Recreate the invisible mouse cursor.
     */
    ਜੇ (gui.blank_ਸੂਚਕ != ਨਿਰਾ)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_ਸੂਚਕ));
#ਨਹੀਂਤਾਂ
	gdk_cursor_unref(gui.blank_ਸੂਚਕ);
#ਜੇ_ਅੰਤ

    gui.blank_ਸੂਚਕ = create_blank_ਸੂਚਕ();

    ਜੇ (gui.ਸੂਚਕ_hidden && gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) != ਨਿਰਾ)
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea),
		gui.blank_ਸੂਚਕ);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    ਜੇ (gui.ਪਾਠ_context != ਨਿਰਾ)
	g_object_unref(gui.ਪਾਠ_context);

    gui.ਪਾਠ_context = gtk_widget_create_pango_context(widget);
    pango_context_ਸੈੱਟ_ਅਧਾਰ_dir(gui.ਪਾਠ_context, PANGO_DIRECTION_LTR);

    ਜੇ (gui.norm_font != ਨਿਰਾ)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_ਸੈੱਟ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ਸਥਿਰ gboolean
mainwin_state_event_cb(GtkWidget *widget ਅਣਵਰਤਿਆ,
		       Gdk_Event_ਖਿੜਕੀ_State *event,
		       gpointer user_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (event->new_ਖਿੜਕੀ_state & GDK_ਖਿੜਕੀ_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    ਵਾਪਸ FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    ਸਥਿਰ ਖਾਲੀ
drawarea_realize_cb(GtkWidget *widget, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    GtkWidget *sbar;
    Gtk_ਵੰਡ ਵੰਡ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    xim_init();
#ਜੇ_ਅੰਤ
    gui_mch_new_colors();
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gui.surface = gdk_ਖਿੜਕੀ_create_similar_surface(
	    gtk_widget_get_ਖਿੜਕੀ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_ਚੌੜਾਈ(widget),
	    gtk_widget_get_allocated_ਉਚਾਈ(widget));
#ਨਹੀਂਤਾਂ
    gui.ਪਾਠ_gc = gdk_gc_new(gui.drawarea->ਖਿੜਕੀ_2);
#ਜੇ_ਅੰਤ

    gui.blank_ਸੂਚਕ = create_blank_ਸੂਚਕ();
    ਜੇ (gui.ਸੂਚਕ_hidden)
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(widget), gui.blank_ਸੂਚਕ);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_ਖੱਬਾ].id;
    ਜੇ (!sbar || (!gui.which_scrollbars[SBAR_ਖੱਬਾ]
				    && firstwin->w_scrollbars[SBAR_ਸੱਜਾ].id))
	sbar = firstwin->w_scrollbars[SBAR_ਸੱਜਾ].id;
    gtk_widget_get_ਵੰਡ(sbar, &ਵੰਡ);
    ਜੇ (sbar && gtk_widget_get_realized(sbar) && ਵੰਡ.ਚੌੜਾਈ)
	gui.scrollbar_ਚੌੜਾਈ = ਵੰਡ.ਚੌੜਾਈ;

    sbar = gui.bottom_sbar.id;
    ਜੇ (sbar && gtk_widget_get_realized(sbar) && ਵੰਡ.ਉਚਾਈ)
	gui.scrollbar_ਉਚਾਈ = ਵੰਡ.ਉਚਾਈ;
}

/*
 * Properly clean up on shutdown.
 */
    ਸਥਿਰ ਖਾਲੀ
drawarea_unrealize_cb(GtkWidget *widget ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Don't write messages to the GUI anymore
    ਭਰਿਆ_screen = FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XIM
    im_shutdown();
#ਜੇ_ਅੰਤ
    ਜੇ (gui.ascii_glyphs != ਨਿਰਾ)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = ਨਿਰਾ;
    }
    ਜੇ (gui.ascii_font != ਨਿਰਾ)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = ਨਿਰਾ;
    }
    g_object_unref(gui.ਪਾਠ_context);
    gui.ਪਾਠ_context = ਨਿਰਾ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਜੇ (gui.surface != ਨਿਰਾ)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = ਨਿਰਾ;
    }
#ਨਹੀਂਤਾਂ
    g_object_unref(gui.ਪਾਠ_gc);
    gui.ਪਾਠ_gc = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_ਸੂਚਕ));
#ਨਹੀਂਤਾਂ
    gdk_cursor_unref(gui.blank_ਸੂਚਕ);
#ਜੇ_ਅੰਤ
    gui.blank_ਸੂਚਕ = ਨਿਰਾ;
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    ਸਥਿਰ ਖਾਲੀ
drawarea_style_updated_cb(GtkWidget *widget ਅਣਵਰਤਿਆ,
			 gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
drawarea_style_ਸੈੱਟ_cb(GtkWidget	*widget ਅਣਵਰਤਿਆ,
		      GtkStyle	*ਪਿਛਲਾ_style ਅਣਵਰਤਿਆ,
		      gpointer	ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
#ਜੇ_ਅੰਤ
{
    gui_mch_new_colors();
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਸਥਿਰ ਪੂਰਨ cur_ਚੌੜਾਈ = 0;
    ਸਥਿਰ ਪੂਰਨ cur_ਉਚਾਈ = 0;

    g_return_val_if_fail(event
	    && event->ਚੌੜਾਈ >= 1 && event->ਉਚਾਈ >= 1, TRUE);

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2) && !GTK_CHECK_ਸੰਸਕਰਣ(3,22,4)
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
    ਜੇ (event->send_event == FALSE)
	ਵਾਪਸ TRUE;
# endif

    ਜੇ (event->ਚੌੜਾਈ == cur_ਚੌੜਾਈ && event->ਉਚਾਈ == cur_ਉਚਾਈ)
	ਵਾਪਸ TRUE;

    cur_ਚੌੜਾਈ = event->ਚੌੜਾਈ;
    cur_ਉਚਾਈ = event->ਉਚਾਈ;

    ਜੇ (gui.surface != ਨਿਰਾ)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_ਖਿੜਕੀ_create_similar_surface(
	    gtk_widget_get_ਖਿੜਕੀ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->ਚੌੜਾਈ, event->ਉਚਾਈ);

    gtk_widget_ਕਤਾਰ_draw(widget);

    ਵਾਪਸ TRUE;
}
#ਜੇ_ਅੰਤ

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ਸਥਿਰ gint
delete_event_cb(GtkWidget *widget ਅਣਵਰਤਿਆ,
		GdkEventAny *event ਅਣਵਰਤਿਆ,
		gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui_shell_closed();
    ਵਾਪਸ TRUE;
}

#ਜੇ defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ਸਥਿਰ ਪੂਰਨ
get_ਵਸਤੂ_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation ਵਸਤੂ_orientation = GTK_ORIENTATION_HORIZONTAL;

    ਜੇ (using_gnome && widget != ਨਿਰਾ)
    {
	GtkWidget *ਮਾਪਾ;
	Bonobo_Dock_ਵਸਤੂ *dockitem;

	ਮਾਪਾ	 = gtk_widget_get_ਮਾਪਾ(widget);
	ਜੇ (G_TYPE_FROM_INSTANCE(ਮਾਪਾ) == BONOBO_TYPE_DOCK_ਵਸਤੂ)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = ਮਾਪਾ;
	    dockitem = BONOBO_DOCK_ਵਸਤੂ(widget);

	    ਜੇ (dockitem == ਨਿਰਾ || dockitem->is_floating)
		ਵਾਪਸ 0;
	    ਵਸਤੂ_orientation = bonobo_dock_ਵਸਤੂ_get_orientation(dockitem);
	}
    }
# ਨਹੀਂਤਾਂ
#  define ਵਸਤੂ_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    ਜੇ (widget != ਨਿਰਾ
	    && ਵਸਤੂ_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_ਵੰਡ ਵੰਡ;

	gtk_widget_get_ਵੰਡ(widget, &ਵੰਡ);
	ਜੇ (ਵੰਡ.ਉਚਾਈ > 1)
	    ਵਾਪਸ ਵੰਡ.ਉਚਾਈ;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_ਉਚਾਈ(widget, &min_h, &natural_h);
	    ਵਾਪਸ MAX(min_h, natural_h);
	}
#  ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਵੰਡ.ਉਚਾਈ;
#  endif
# ਨਹੀਂਤਾਂ
	ਜੇ (orientation == GTK_ORIENTATION_HORIZONTAL)
	    ਵਾਪਸ widget->ਵੰਡ.ਉਚਾਈ;
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ widget->ਵੰਡ.ਚੌੜਾਈ;
# endif
    }
    ਵਾਪਸ 0;
}
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਪੂਰਨ
get_menu_tool_ਚੌੜਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ ਚੌੜਾਈ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    ਚੌੜਾਈ += get_ਵਸਤੂ_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    ਚੌੜਾਈ += get_ਵਸਤੂ_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    ਜੇ (gui.tabline != ਨਿਰਾ)
	ਚੌੜਾਈ += get_ਵਸਤੂ_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਚੌੜਾਈ;
}

    ਸਥਿਰ ਪੂਰਨ
get_menu_tool_ਉਚਾਈ(ਖਾਲੀ)
{
    ਪੂਰਨ ਉਚਾਈ = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਉਚਾਈ += get_ਵਸਤੂ_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    ਉਚਾਈ += get_ਵਸਤੂ_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    ਜੇ (gui.tabline != ਨਿਰਾ
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	ਉਚਾਈ += get_ਵਸਤੂ_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਉਚਾਈ;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ਸਥਿਰ ਪੂਰਨ init_ਖਿੜਕੀ_hints_state = 0;

    ਸਥਿਰ ਖਾਲੀ
update_ਖਿੜਕੀ_manager_hints(ਪੂਰਨ force_ਚੌੜਾਈ, ਪੂਰਨ force_ਉਚਾਈ)
{
    ਸਥਿਰ ਪੂਰਨ old_ਚੌੜਾਈ  = 0;
    ਸਥਿਰ ਪੂਰਨ old_ਉਚਾਈ = 0;
    ਸਥਿਰ ਪੂਰਨ old_min_ਚੌੜਾਈ  = 0;
    ਸਥਿਰ ਪੂਰਨ old_min_ਉਚਾਈ = 0;
    ਸਥਿਰ ਪੂਰਨ old_char_ਚੌੜਾਈ  = 0;
    ਸਥਿਰ ਪੂਰਨ old_char_ਉਚਾਈ = 0;

    ਪੂਰਨ ਚੌੜਾਈ;
    ਪੂਰਨ ਉਚਾਈ;
    ਪੂਰਨ min_ਚੌੜਾਈ;
    ਪੂਰਨ min_ਉਚਾਈ;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    ਜੇ (!(force_ਚੌੜਾਈ && force_ਉਚਾਈ)  &&  init_ਖਿੜਕੀ_hints_state > 0)
    {
	// Don't do it!
	init_ਖਿੜਕੀ_hints_state = 2;
	ਵਾਪਸ;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    ਚੌੜਾਈ  = gui_get_ਅਧਾਰ_ਚੌੜਾਈ();
    ਉਚਾਈ = gui_get_ਅਧਾਰ_ਉਚਾਈ();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    ਉਚਾਈ += tabline_ਉਚਾਈ() * gui.char_ਉਚਾਈ;
#ਜੇ_ਅੰਤ
    ਚੌੜਾਈ  += get_menu_tool_ਚੌੜਾਈ();
    ਉਚਾਈ += get_menu_tool_ਉਚਾਈ();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    ਜੇ (force_ਚੌੜਾਈ && force_ਉਚਾਈ)
    {
	min_ਚੌੜਾਈ  = force_ਚੌੜਾਈ;
	min_ਉਚਾਈ = force_ਉਚਾਈ;
    }
    ਨਹੀਂਤਾਂ
    {
	min_ਚੌੜਾਈ  = ਚੌੜਾਈ  + MIN_COLUMNS * gui.char_ਚੌੜਾਈ;
	min_ਉਚਾਈ = ਉਚਾਈ + MIN_ਸਤਰਾਂ   * gui.char_ਉਚਾਈ;
    }

    // Avoid an expose event when the size didn't change.
    ਜੇ (ਚੌੜਾਈ != old_ਚੌੜਾਈ
	    || ਉਚਾਈ != old_ਉਚਾਈ
	    || min_ਚੌੜਾਈ != old_min_ਚੌੜਾਈ
	    || min_ਉਚਾਈ != old_min_ਉਚਾਈ
	    || gui.char_ਚੌੜਾਈ != old_char_ਚੌੜਾਈ
	    || gui.char_ਉਚਾਈ != old_char_ਉਚਾਈ)
    {
	GdkGeometry	geometry;
	Gdk_ਖਿੜਕੀ_Hints	geometry_ਨਕਾਬ;

	geometry.ਚੌੜਾਈ_inc   = gui.char_ਚੌੜਾਈ;
	geometry.ਉਚਾਈ_inc  = gui.char_ਉਚਾਈ;
	geometry.ਅਧਾਰ_ਚੌੜਾਈ  = ਚੌੜਾਈ;
	geometry.ਅਧਾਰ_ਉਚਾਈ = ਉਚਾਈ;
	geometry.min_ਚੌੜਾਈ   = min_ਚੌੜਾਈ;
	geometry.min_ਉਚਾਈ  = min_ਉਚਾਈ;
	geometry_ਨਕਾਬ	     = GDK_HINT_ਅਧਾਰ_ਮਾਪ|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_ਮਾਪ;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_geometry_hints(GTK_ਖਿੜਕੀ(gui.mainwin), ਨਿਰਾ,
				      &geometry, geometry_ਨਕਾਬ);
	old_ਚੌੜਾਈ       = ਚੌੜਾਈ;
	old_ਉਚਾਈ      = ਉਚਾਈ;
	old_min_ਚੌੜਾਈ   = min_ਚੌੜਾਈ;
	old_min_ਉਚਾਈ  = min_ਉਚਾਈ;
	old_char_ਚੌੜਾਈ  = gui.char_ਚੌੜਾਈ;
	old_char_ਉਚਾਈ = gui.char_ਉਚਾਈ;
    }
}

#ਜੇ defined(FEAT_GUI_DARKTHEME)
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_dark_theme(ਪੂਰਨ dark)
{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_ਸੈੱਟ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, ਨਿਰਾ);
# endif
}
#ਜੇ_ਅੰਤ // FEAT_GUI_DARKTHEME

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ਸਥਿਰ ਖਾਲੀ
icon_ਮਾਪ_ਬਦਲਿਆ_foreach(GtkWidget *widget, gpointer user_ਅੰਕੜੇ)
{
    ਜੇ (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
	ਜੇ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ਅਚਲ Gtk_Icon_ਮਾਪ icon_ਮਾਪ = GPOINTER_ਮੰਜ਼ਿਲ_INT(user_ਅੰਕੜੇ);
	    ਅਚਲ gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, ਨਿਰਾ);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_ਮਾਪ);
	}
# ਨਹੀਂਤਾਂ
	// User-defined icons are stored in a GtkIconSet
	ਜੇ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_ਸੈੱਟ)
	{
	    Gtk_Icon_ਸੈੱਟ	*icon_ਸੈੱਟ;
	    Gtk_Icon_ਮਾਪ	icon_ਮਾਪ;

	    gtk_image_get_icon_ਸੈੱਟ(image, &icon_ਸੈੱਟ, &icon_ਮਾਪ);
	    icon_ਮਾਪ = (Gtk_Icon_ਮਾਪ)(ਲੰਮਾ)user_ਅੰਕੜੇ;

	    gtk_icon_ਸੈੱਟ_ref(icon_ਸੈੱਟ);
	    gtk_image_ਸੈੱਟ_from_icon_ਸੈੱਟ(image, icon_ਸੈੱਟ, icon_ਮਾਪ);
	    gtk_icon_ਸੈੱਟ_unref(icon_ਸੈੱਟ);
	}
# endif
    }
    ਨਹੀਂਤਾਂ ਜੇ (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_ਮਾਪ_ਬਦਲਿਆ_foreach,
			      user_ਅੰਕੜੇ);
    }
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_ਮਾਪ	    ਮਾਪ;
    Gtk_Icon_ਮਾਪ	    oldsize;

    ਜੇ ((toolbar_flags & (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    ਨਹੀਂਤਾਂ ਜੇ ((toolbar_flags & (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS))
		      == (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    ਨਹੀਂਤਾਂ ਜੇ (toolbar_flags & TOOLBAR_ਪਾਠ)
	style = GTK_TOOLBAR_ਪਾਠ;
    ਨਹੀਂਤਾਂ
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_ਸੈੱਟ_style(toolbar, style);
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_toolbar_ਸੈੱਟ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    ਚੋਣ (tbis_flags)
    {
	ਹਾਲਤ TBIS_TINY:	    ਮਾਪ = GTK_ICON_ਮਾਪ_MENU;		ਰੋਕੋ;
	ਹਾਲਤ TBIS_SMALL:    ਮਾਪ = GTK_ICON_ਮਾਪ_SMALL_TOOLBAR;	ਰੋਕੋ;
	ਹਾਲਤ TBIS_MEDIUM:   ਮਾਪ = GTK_ICON_ਮਾਪ_BUTTON;	ਰੋਕੋ;
	ਹਾਲਤ TBIS_LARGE:    ਮਾਪ = GTK_ICON_ਮਾਪ_LARGE_TOOLBAR;	ਰੋਕੋ;
	ਹਾਲਤ TBIS_HUGE:     ਮਾਪ = GTK_ICON_ਮਾਪ_DND;		ਰੋਕੋ;
	ਹਾਲਤ TBIS_GIANT:    ਮਾਪ = GTK_ICON_ਮਾਪ_DIALOG;	ਰੋਕੋ;
	ਮੂਲ:	    ਮਾਪ = GTK_ICON_ਮਾਪ_INVALID;	ਰੋਕੋ;
    }
    oldsize = gtk_toolbar_get_icon_ਮਾਪ(toolbar);

    ਜੇ (ਮਾਪ == GTK_ICON_ਮਾਪ_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_ਮਾਪ(toolbar);
	ਮਾਪ = gtk_toolbar_get_icon_ਮਾਪ(toolbar);
    }
    ਜੇ (ਮਾਪ != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_ਮਾਪ_ਬਦਲਿਆ_foreach,
			      GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ((ਪੂਰਨ)ਮਾਪ));
    }
    gtk_toolbar_ਸੈੱਟ_icon_ਮਾਪ(toolbar, ਮਾਪ);
}

#ਜੇ_ਅੰਤ // FEAT_TOOLBAR

#ਜੇ defined(FEAT_GUI_TABLINE)
ਸਥਿਰ ਪੂਰਨ ignore_tabline_evt = FALSE;
ਸਥਿਰ GtkWidget *tabline_menu;
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ GtkTooltips *tabline_tooltip;
# endif
ਸਥਿਰ ਪੂਰਨ clicked_ਪੰਨਾ;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ਸਥਿਰ ਖਾਲੀ
tabline_menu_handler(Gtk_Menu_ਵਸਤੂ *ਵਸਤੂ ਅਣਵਰਤਿਆ, gpointer user_ਅੰਕੜੇ)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_ਪੰਨਾ, (ਪੂਰਨ)(ਲੰਮਾ)user_ਅੰਕੜੇ);
}

    ਸਥਿਰ ਖਾਲੀ
ਜੋੜੋ_tabline_menu_ਵਸਤੂ(GtkWidget *menu, char_u *ਪਾਠ, ਪੂਰਨ resp)
{
    GtkWidget	*ਵਸਤੂ;
    char_u	*utf_ਪਾਠ;

    utf_ਪਾਠ = CONVERT_ਮੰਜ਼ਿਲ_UTF8(ਪਾਠ);
    ਵਸਤੂ = gtk_menu_ਵਸਤੂ_new_with_label((ਅਚਲ ਅੱਖਰ *)utf_ਪਾਠ);
    gtk_widget_show(ਵਸਤੂ);
    CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(utf_ਪਾਠ);

    gtk_container_ਜੋੜੋ(GTK_CONTAINER(menu), ਵਸਤੂ);
    g_signal_connect(G_OBJECT(ਵਸਤੂ), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(resp));
}

/*
 * Create a menu for the tab line.
 */
    ਸਥਿਰ GtkWidget *
create_tabline_menu(ਖਾਲੀ)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    ਜੋੜੋ_tabline_menu_ਵਸਤੂ(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    ਵਾਪਸ menu;
}

    ਸਥਿਰ gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    ਜੇ (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	ਪੂਰਨ		x = bevent->x;
	ਪੂਰਨ		y = bevent->y;
	GtkWidget	*tabwidget;
	Gdk_ਖਿੜਕੀ	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	ਜੇ (hold_gui_events || cmdwin_type != 0)
	    ਵਾਪਸ TRUE;

	tabwin = gui_gtk_ਖਿੜਕੀ_at_position(gui.mainwin, &x, &y);

	gdk_ਖਿੜਕੀ_get_user_ਅੰਕੜੇ(tabwin, (gpointer)&tabwidget);
	clicked_ਪੰਨਾ = GPOINTER_ਮੰਜ਼ਿਲ_INT(g_object_get_ਅੰਕੜੇ(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	ਜੇ (bevent->button == 3)
	{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    ਜੇ (gui.is_wayland)
	    {
		ਪੂਰਨ x2, y2;
		gui_gtk_get_ਸੂਚਕ(gui.mainwin, &x2, &y2, ਨਿਰਾ);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_ਖਿੜਕੀ(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, ਨਿਰਾ);
	    }
	    ਨਹੀਂਤਾਂ
#  endif
	    gtk_menu_popup_at_ਸੂਚਕ(GTK_MENU(widget), event);
# ਨਹੀਂਤਾਂ
	    gtk_menu_popup(GTK_MENU(widget), ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ,
						bevent->button, bevent->ਸਮਾਂ);
# endif
	    // We handled the event.
	    ਵਾਪਸ TRUE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (bevent->button == 1)
	{
	    ਜੇ (clicked_ਪੰਨਾ == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (bevent->button == 2)
	{
	    ਜੇ (clicked_ਪੰਨਾ != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_ਪੰਨਾ, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    ਵਾਪਸ FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    ਸਥਿਰ ਖਾਲੀ
on_select_tab(
	GtkNotebook	*notebook ਅਣਵਰਤਿਆ,
	gpointer	*ਪੰਨਾ ਅਣਵਰਤਿਆ,
	gint		idx,
	gpointer	ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ਸਥਿਰ ਖਾਲੀ
on_tab_reordered(
	GtkNotebook	*notebook ਅਣਵਰਤਿਆ,
	gpointer	*ਪੰਨਾ ਅਣਵਰਤਿਆ,
	gint		idx,
	gpointer	ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (ignore_tabline_evt)
	ਵਾਪਸ;

    ਜੇ ((tabpage_ਸੂਚਕਾਂਕ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ਨਹੀਂਤਾਂ
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    ਖਾਲੀ
gui_mch_show_tabline(ਪੂਰਨ showit)
{
    ਜੇ (gui.tabline == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    ਪੂਰਨ w, h;

	    gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_ਸੈੱਟ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_ਖਿੜਕੀ_manager_hints(0, 0);
	ਜੇ (showit)
	    gtk_widget_ਸੈੱਟ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    ਪੂਰਨ
gui_mch_showing_tabline(ਖਾਲੀ)
{
    ਵਾਪਸ gui.tabline != ਨਿਰਾ
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    ਖਾਲੀ
gui_mch_update_tabline(ਖਾਲੀ)
{
    GtkWidget	    *ਪੰਨਾ;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    ਪੂਰਨ		    nr = 0;
    ਪੂਰਨ		    tab_ਅੰਕ;
    ਪੂਰਨ		    curtabidx = 0;
    char_u	    *labeltext;

    ਜੇ (gui.tabline == ਨਿਰਾ)
	ਵਾਪਸ;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    ਲਈ (tp = ਪਹਿਲਾ_tabpage; tp != ਨਿਰਾ; tp = tp->tp_ਅਗਲਾ, ++nr)
    {
	ਜੇ (tp == curtab)
	    curtabidx = nr;

	tab_ਅੰਕ = nr + 1;

	ਪੰਨਾ = gtk_notebook_get_nth_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr);
	ਜੇ (ਪੰਨਾ == ਨਿਰਾ)
	{
	    // Add notebook page
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	    ਪੰਨਾ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(ਪੰਨਾ), FALSE);
# ਨਹੀਂਤਾਂ
	    ਪੰਨਾ = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(ਪੰਨਾ);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
	    gtk_misc_ਸੈੱਟ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_ਜੋੜੋ(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_ਪਾਓ_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline),
		    ਪੰਨਾ,
		    event_box,
		    nr);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,10,0)
	    gtk_notebook_ਸੈੱਟ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ਪੰਨਾ,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(event_box), "tab_num",
						     GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(tab_ਅੰਕ));
	label = gtk_bin_get_ਬੱਚਾ(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_ਮੰਜ਼ਿਲ_UTF8(NameBuff);
	gtk_label_ਸੈੱਟ_ਪਾਠ(GTK_LABEL(label), (ਅਚਲ ਅੱਖਰ *)labeltext);
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_ਮੰਜ਼ਿਲ_UTF8(NameBuff);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	gtk_widget_ਸੈੱਟ_tooltip_ਪਾਠ(event_box, (ਅਚਲ gchar *)labeltext);
# ਨਹੀਂਤਾਂ
	gtk_tooltips_ਸੈੱਟ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ਅਚਲ ਅੱਖਰ *)labeltext, ਨਿਰਾ);
# endif
	CONVERT_ਮੰਜ਼ਿਲ_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    ਜਦੋਂ (gtk_notebook_get_nth_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr) != ਨਿਰਾ)
	gtk_notebook_remove_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr);

    ਜੇ (gtk_notebook_get_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_ਸੈੱਟ_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_curtab(ਪੂਰਨ nr)
{
    ਜੇ (gui.tabline == ਨਿਰਾ)
	ਵਾਪਸ;

    ignore_tabline_evt = TRUE;
    ਜੇ (gtk_notebook_get_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_ਸੈੱਟ_current_ਪੰਨਾ(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    ਖਾਲੀ
gui_gtk_ਸੈੱਟ_selection_targets(GdkAtom selection)
{
    ਪੂਰਨ		    i, j = 0;
    ਸਥਿਰ ਪੂਰਨ	    n_targets = N_SELECTION_TARGETS;
    ਸਥਿਰ Gtk_ਨਿਸ਼ਾਨਾ_ਇੰਦਰਾਜ  targets[N_SELECTION_TARGETS];

    ਜੇ (targets[0].ਨਿਸ਼ਾਨਾ == ਨਿਰਾ)
    {
	ਲਈ (i = 0; i < (ਪੂਰਨ)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    ਜੇ (!clip_html && selection_targets[i].info == ਨਿਸ਼ਾਨਾ_HTML)
		n_targets--;
	    ਨਹੀਂਤਾਂ
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_ਜੋੜੋ_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    ਖਾਲੀ
gui_gtk_ਸੈੱਟ_dnd_targets(ਖਾਲੀ)
{
    ਪੂਰਨ		    i, j = 0;
    ਪੂਰਨ		    n_targets = N_DND_TARGETS;
    Gtk_ਨਿਸ਼ਾਨਾ_ਇੰਦਰਾਜ  targets[N_DND_TARGETS];

    ਲਈ (i = 0; i < (ਪੂਰਨ)N_DND_TARGETS; ++i)
    {
	ਜੇ (!clip_html && dnd_targets[i].info == ਨਿਸ਼ਾਨਾ_HTML)
	    n_targets--;
	ਨਹੀਂਤਾਂ
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_ਸੈੱਟ(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ਪੂਰਨ
gui_mch_init(ਖਾਲੀ)
{
    GtkWidget *vbox;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    ਜੇ (using_gnome)
    {
	gnome_ਪ੍ਰੋਗਰਾਮ_init(VIMPACKAGE, VIM_ਸੰਸਕਰਣ_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, ਨਿਰਾ);
# ਜੇ defined(LC_NUMERIC)
	{
	    ਅੱਖਰ *p = setlocale(LC_NUMERIC, ਨਿਰਾ);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    ਜੇ (p == ਨਿਰਾ || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#ਜੇ_ਅੰਤ
    VIM_CLEAR(gui_argv);

#ਜੇ GLIB_CHECK_ਸੰਸਕਰਣ(2,1,3)
    // Set the human-readable application name
    g_ਸੈੱਟ_application_name("Vim");
#ਜੇ_ਅੰਤ
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    ਸੈੱਟ_option_ਮੁੱਲ_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    gui_gtk_ਰਜਿਸਟਰ_stock_icons();
#ਜੇ_ਅੰਤ
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
# ਜੇ 0
    gtk_rc_parse("gtkrc");
# endif
#ਜੇ_ਅੰਤ

    // Initialize values
    gui.border_ਚੌੜਾਈ = 2;
    gui.scrollbar_ਚੌੜਾਈ = SB_DEFAULT_ਚੌੜਾਈ;
    gui.scrollbar_ਉਚਾਈ = SB_DEFAULT_ਚੌੜਾਈ;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#ਨਹੀਂਤਾਂ
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#ਜੇ_ਅੰਤ

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ਪਿੱਠ_pixel = gui.def_ਪਿੱਠ_pixel;

    ਜੇ (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	ਜੇ (plug != ਨਿਰਾ && gtk_plug_get_socket_ਖਿੜਕੀ(GTK_PLUG(plug)) != ਨਿਰਾ)
	{
	    gui.mainwin = plug;
	}
	ਨਹੀਂਤਾਂ
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    ਜੇ (gtk_socket_id == 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
	ਜੇ (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", ਨਿਰਾ);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
	    gui.mainwin = gtk_ਖਿੜਕੀ_new(GTK_ਖਿੜਕੀ_TOPLEVEL);
    }

    gtk_widget_ਸੈੱਟ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.ਪਾਠ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ਸੈੱਟ_ਅਧਾਰ_dir(gui.ਪਾਠ_context, PANGO_DIRECTION_LTR);

    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_ਜੋੜੋ_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_ਨਕਾਬ);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), ਨਿਰਾ);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), ਨਿਰਾ);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_ਬਦਲਿਆ_cb), ਨਿਰਾ);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), ਨਿਰਾ);

    gui.accel_ਸਮੂਹ = gtk_accel_ਸਮੂਹ_new();
    gtk_ਖਿੜਕੀ_ਜੋੜੋ_accel_ਸਮੂਹ(GTK_ਖਿੜਕੀ(gui.mainwin), gui.accel_ਸਮੂਹ);

    // A vertical box holds the menubar, toolbar and main text window.
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(vbox), FALSE);
#ਨਹੀਂਤਾਂ
    vbox = gtk_vbox_new(FALSE, 0);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GNOME
    ਜੇ (using_gnome)
    {
# ਜੇ defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_ਸੈੱਟ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_ਸੈੱਟ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_ਸੈੱਟ(gtk_settings, "gtk-menu-bar-accel", ਨਿਰਾ, ਨਿਰਾ);
    }


# ifdef FEAT_GUI_GNOME
    ਜੇ (using_gnome)
    {
	Bonobo_Dock_ਵਸਤੂ *dockitem;

	gnome_app_ਸੈੱਟ_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_ਵਸਤੂ_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_ਵਸਤੂ_ਸੈੱਟ_behavior(dockitem,
		bonobo_dock_ਵਸਤੂ_get_behavior(dockitem)
				       | BONOBO_DOCK_ਵਸਤੂ_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    ਨਹੀਂਤਾਂ
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#ਜੇ_ਅੰਤ	// FEAT_MENU

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# ਨਹੀਂਤਾਂ
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_ਸੈੱਟ_name(gui.toolbar, "vim-toolbar");
    ਸੈੱਟ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    ਜੇ (using_gnome)
    {
	Bonobo_Dock_ਵਸਤੂ *dockitem;

	gnome_app_ਸੈੱਟ_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_ਵਸਤੂ_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_ਵਸਤੂ_ਸੈੱਟ_behavior(dockitem,
		bonobo_dock_ਵਸਤੂ_get_behavior(dockitem)
				       | BONOBO_DOCK_ਵਸਤੂ_BEH_NEVER_FLOATING);
	gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(gui.toolbar), 0);
    }
    ਨਹੀਂਤਾਂ
# endif	// FEAT_GUI_GNOME
    {
	ਜੇ (vim_strchr(p_go, GO_TOOLBAR) != ਨਿਰਾ
		&& (toolbar_flags & (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#ਜੇ_ਅੰਤ // FEAT_TOOLBAR

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_ਸੈੱਟ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ਸੈੱਟ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ਸੈੱਟ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_notebook_ਸੈੱਟ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *ਪੰਨਾ, *label, *event_box;

	// Add the first tab.
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	ਪੰਨਾ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_ਸੈੱਟ_homogeneous(GTK_BOX(ਪੰਨਾ), FALSE);
# ਨਹੀਂਤਾਂ
	ਪੰਨਾ = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(ਪੰਨਾ);
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(gui.tabline), ਪੰਨਾ);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_ਸੈੱਟ_ਅੰਕੜੇ(G_OBJECT(event_box), "tab_num", GINT_ਮੰਜ਼ਿਲ_ਸੂਚਕ(1L));
# ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,14,0)
	gtk_misc_ਸੈੱਟ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_ਜੋੜੋ(GTK_CONTAINER(event_box), label);
	gtk_notebook_ਸੈੱਟ_tab_label(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ, event_box);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,10,0)
	gtk_notebook_ਸੈੱਟ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), ਪੰਨਾ, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ਨਿਰਾ);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ਨਿਰਾ);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#ਜੇ_ਅੰਤ // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_ਸੈੱਟ_border_ਚੌੜਾਈ(GTK_CONTAINER(gui.formwin), 0);
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gtk_widget_ਸੈੱਟ_events(gui.formwin, GDK_EXPOSURE_ਨਕਾਬ);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    gtk_widget_ਸੈੱਟ_name(gui.formwin, "vim-gtk-form");
#ਜੇ_ਅੰਤ

    gui.drawarea = gtk_drawing_area_new();
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gui.surface = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    ਜੇ (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#ਜੇ_ਅੰਤ

    // Determine which events we will filter.
    gint event_ਨਕਾਬ =
	GDK_EXPOSURE_ਨਕਾਬ |
	GDK_ENTER_NOTIFY_ਨਕਾਬ |
	GDK_LEAVE_NOTIFY_ਨਕਾਬ |
	GDK_BUTTON_PRESS_ਨਕਾਬ |
	GDK_BUTTON_RELEASE_ਨਕਾਬ |
	GDK_ਕੁੰਜੀ_PRESS_ਨਕਾਬ |
	GDK_ਕੁੰਜੀ_RELEASE_ਨਕਾਬ |
	GDK_ਸੂਚਕ_MOTION_ਨਕਾਬ |
	GDK_ਸੂਚਕ_MOTION_HINT_ਨਕਾਬ;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0)
    ਜੇ (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_ਨਕਾਬ |= GDK_SCROLL_ਨਕਾਬ;
    }
    ਨਹੀਂਤਾਂ
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_ਨਕਾਬ |= GDK_SMOOTH_SCROLL_ਨਕਾਬ;
    }
#ਨਹੀਂਤਾਂ
    event_ਨਕਾਬ |= GDK_SCROLL_ਨਕਾਬ;
#ਜੇ_ਅੰਤ
    gtk_widget_ਸੈੱਟ_events(gui.drawarea, event_ਨਕਾਬ);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_ਸ਼ੁਰੂ(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(ਕੁੰਜੀ_press_event), ਨਿਰਾ);
#ਜੇ defined(FEAT_XIM) || GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&ਕੁੰਜੀ_release_event), ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ਨਿਰਾ);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), ਨਿਰਾ);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_ਸੈੱਟ_cb), ਨਿਰਾ);
#ਜੇ_ਅੰਤ

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#ਜੇ_ਅੰਤ

#ਜੇ !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#ਜੇ_ਅੰਤ

    ਜੇ (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_ਸੈੱਟ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_ਤਾਰਾ.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_ਵਿੱਥ = gui.border_ਚੌੜਾਈ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), ਨਿਰਾ);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ਜੇ (vim_strchr(p_go, GO_ਸੂਚਕ) != ਨਿਰਾ)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), ਨਿਰਾ);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), ਨਿਰਾ);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    ਜੇ (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_ਬਾਹਰਲਾ_event), ਨਿਰਾ);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_ਵਿੱਚ_event), ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_ਬਾਹਰਲਾ_event), ਨਿਰਾ);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_ਵਿੱਚ_event), ਨਿਰਾ);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_ਬਾਹਰਲਾ_event), ਨਿਰਾ);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_ਵਿੱਚ_event), ਨਿਰਾ);
#ਜੇ_ਅੰਤ // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), ਨਿਰਾ);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.ਵਿੱਚ_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_ਬਦਲਿਆ_cb), ਨਿਰਾ);
    }

#ਜੇ defined(FEAT_IMAGE) && GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    gui.scale = gtk_widget_get_scale_ਗੁਣਕ(gui.formwin);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ OK;
}

#ਜੇ defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    ਖਾਲੀ
gui_mch_forked(ਖਾਲੀ)
{
    ਜੇ (!using_gnome)
	ਵਾਪਸ;

    GnomeClient *client;

    client = gnome_master_client();

    ਜੇ (client != ਨਿਰਾ)
	gnome_client_ਸੈੱਟ_process_id(client, getpid());
}
#ਜੇ_ਅੰਤ // USE_GNOME_SESSION

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ GdkRGBA
color_ਮੰਜ਼ਿਲ_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    ਵਾਪਸ rgba;
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_cairo_ਸਰੋਤ_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ਅਚਲ GdkRGBA rgba = color_ਮੰਜ਼ਿਲ_rgba(color);
    cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#ਜੇ_ਅੰਤ // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    ਖਾਲੀ
gui_mch_new_colors(ਖਾਲੀ)
{
    ਜੇ (gui.drawarea != ਨਿਰਾ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
	    && gui.formwin != ਨਿਰਾ
#ਜੇ_ਅੰਤ
	    && gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) != ਨਿਰਾ)
    {
#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
	Gdk_ਖਿੜਕੀ * ਅਚਲ da_win = gtk_widget_get_ਖਿੜਕੀ(gui.drawarea);
#ਜੇ_ਅੰਤ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
	GtkStyleContext * ਅਚਲ context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ਅਚਲ provider = gtk_css_provider_new();
	gchar * ਅਚਲ css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.ਪਿੱਠ_pixel >> 16) & 0xff,
		 (gui.ਪਿੱਠ_pixel >> 8) & 0xff,
		 gui.ਪਿੱਠ_pixel & 0xff);

	gtk_css_provider_ਲੱਦੋ_from_ਅੰਕੜੇ(provider, css, -1, ਨਿਰਾ);
	gtk_style_context_ਜੋੜੋ_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#ਜਾਂਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_ਮੰਜ਼ਿਲ_rgba(gui.ਪਿੱਠ_pixel);
	{
	    cairo_ਨਮੂਨਾ_t * ਅਚਲ pat = cairo_ਨਮੂਨਾ_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    ਜੇ (pat != ਨਿਰਾ)
	    {
		gdk_ਖਿੜਕੀ_ਸੈੱਟ_background_ਨਮੂਨਾ(da_win, pat);
		cairo_ਨਮੂਨਾ_destroy(pat);
	    }
	    ਨਹੀਂਤਾਂ
		gdk_ਖਿੜਕੀ_ਸੈੱਟ_background_rgba(da_win, &rgba);
	}
#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.ਪਿੱਠ_pixel;
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_background(da_win, &color);
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,22,2)
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
    ਸਥਿਰ gboolean
startup_resize_correction_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    gui_ਸੈੱਟ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ਵਾਪਸ FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ਸਥਿਰ gint
form_configure_event(GtkWidget *widget ਅਣਵਰਤਿਆ,
		     GdkEventConfigure *event,
		     gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    usable_ਉਚਾਈ = event->ਉਚਾਈ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    ਪੂਰਨ	    w, h;
    gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ਨਿਰਾ, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (ਪੂਰਨ)Columns, (ਪੂਰਨ)ਪੰਕਤੀਆਂ);
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

    ਜੇ (!ਮੇਲ_ਚੌੜਾਈ_ਉਚਾਈ(latest_resize_hist, w, h)
					     && ਮੇਲ_stale_ਚੌੜਾਈ_ਉਚਾਈ(w, h))
	// discard stale event
	ਵਾਪਸ TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    ਜੇ ((mch_csd_ਉਚਾਈ == 0 || mch_csd_ਚੌੜਾਈ == 0)
	    && latest_resize_hist != ਨਿਰਾ
	    && !latest_resize_hist->ਵਰਤਿਆ
	    && latest_resize_hist->from_shellsize
	    && gui.char_ਉਚਾਈ > 0)
    {
	ਪੂਰਨ pot_csd_w = latest_resize_hist->ਚੌੜਾਈ - w;
	ਪੂਰਨ pot_csd_h = latest_resize_hist->ਉਚਾਈ - h;

	ਜੇ (pot_csd_w > 0 && pot_csd_w < gui.char_ਚੌੜਾਈ)
	{
	    mch_csd_ਚੌੜਾਈ = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (ਪੂਰਨ)Columns * gui.char_ਚੌੜਾਈ
						    + gui_get_ਅਧਾਰ_ਚੌੜਾਈ();
	}
	ਜੇ (pot_csd_h > 0 && pot_csd_h < gui.char_ਉਚਾਈ)
	{
	    mch_csd_ਉਚਾਈ = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_ਉਚਾਈ += mch_csd_ਉਚਾਈ;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	ਜੇ ((mch_csd_ਉਚਾਈ > 0 || mch_csd_ਚੌੜਾਈ > 0) && gtk_socket_id == 0)
	    g_idle_ਜੋੜੋ_ਭਰਿਆ(GTK_ਤਰਜੀਹ_RESIZE, startup_resize_correction_cb, ਨਿਰਾ, ਨਿਰਾ);
    }
    clear_resize_hists();
#ਜੇ_ਅੰਤ

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2) && !GTK_CHECK_ਸੰਸਕਰਣ(3,22,4)
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
    ਜੇ (event->ਖਿੜਕੀ_2 != gtk_widget_get_ਖਿੜਕੀ(gui.formwin))
	ਵਾਪਸ TRUE;
#ਜੇ_ਅੰਤ

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    ਜੇ (gtk_socket_id != 0)
	usable_ਉਚਾਈ -= (gui.char_ਉਚਾਈ - (gui.char_ਉਚਾਈ/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	ਪੂਰਨ use_ਚੌੜਾਈ = event->ਚੌੜਾਈ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_RESIZE_HISTORY
	ਜੇ (mch_pending_form_w > event->ਚੌੜਾਈ
		&& mch_pending_form_w - event->ਚੌੜਾਈ < gui.char_ਚੌੜਾਈ)
	    use_ਚੌੜਾਈ = mch_pending_form_w;
#ਜੇ_ਅੰਤ
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_ਚੌੜਾਈ, usable_ਉਚਾਈ);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    ਵਾਪਸ TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    ਸਥਿਰ ਖਾਲੀ
mainwin_destroy_cb(GObject *object ਅਣਵਰਤਿਆ, gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Don't write messages to the GUI anymore
    ਭਰਿਆ_screen = FALSE;

    gui.mainwin  = ਨਿਰਾ;
    gui.drawarea = ਨਿਰਾ;

    ਜੇ (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GRESOURCE
    gui_gtk_unregister_resource();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
gui_gtk_get_screen_geom_ਦਾ_win(
	GtkWidget *wid,
	ਪੂਰਨ ਬਿੰਦੂ_x,	    // x position of window if not initialized
	ਪੂਰਨ ਬਿੰਦੂ_y,	    // y position of window if not initialized
	ਪੂਰਨ *screen_x,
	ਪੂਰਨ *screen_y,
	ਪੂਰਨ *ਚੌੜਾਈ,
	ਪੂਰਨ *ਉਚਾਈ)
{
    GdkRectangle geometry;
    Gdk_ਖਿੜਕੀ *win = gtk_widget_get_ਖਿੜਕੀ(wid);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    ਜੇ (wid != ਨਿਰਾ && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    ਨਹੀਂਤਾਂ
	dpy = gdk_display_get_default();
    ਜੇ (win != ਨਿਰਾ)
	monitor = gdk_display_get_monitor_at_ਖਿੜਕੀ(dpy, win);
    ਨਹੀਂਤਾਂ
	monitor = gdk_display_get_monitor_at_ਬਿੰਦੂ(dpy, ਬਿੰਦੂ_x, ਬਿੰਦੂ_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#ਨਹੀਂਤਾਂ
    GdkScreen* screen;
    ਪੂਰਨ monitor;

    ਜੇ (wid != ਨਿਰਾ && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    ਨਹੀਂਤਾਂ
	screen = gdk_screen_get_default();
    ਜੇ (win != ਨਿਰਾ)
	monitor = gdk_screen_get_monitor_at_ਖਿੜਕੀ(screen, win);
    ਨਹੀਂਤਾਂ
	monitor = gdk_screen_get_monitor_at_ਬਿੰਦੂ(screen, ਬਿੰਦੂ_x, ਬਿੰਦੂ_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#ਜੇ_ਅੰਤ
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *ਚੌੜਾਈ = geometry.ਚੌੜਾਈ;
    *ਉਚਾਈ = geometry.ਉਚਾਈ;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ਸਥਿਰ ਖਾਲੀ
gui_gtk_get_screen_dimensions(
	ਪੂਰਨ ਬਿੰਦੂ_x,
	ਪੂਰਨ ਬਿੰਦੂ_y,
	ਪੂਰਨ *screen_w,
	ਪੂਰਨ *screen_h)
{
    ਪੂਰਨ	    x, y;

    gui_gtk_get_screen_geom_ਦਾ_win(gui.mainwin, ਬਿੰਦੂ_x, ਬਿੰਦੂ_y,
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
    *screen_w -= get_menu_tool_ਚੌੜਾਈ();
    *screen_h -= get_menu_tool_ਉਚਾਈ();
}

    ਖਾਲੀ
gui_mch_get_screen_dimensions(ਪੂਰਨ *screen_w, ਪੂਰਨ *screen_h)
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
    ਸਥਿਰ timeout_cb_type
check_startup_plug_hints(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਜੇ (init_ਖਿੜਕੀ_hints_state == 1)
    {
	// Safe to use normal hints now
	init_ਖਿੜਕੀ_hints_state = 0;
	update_ਖਿੜਕੀ_manager_hints(0, 0);
	ਵਾਪਸ FALSE;   // stop timer
    }

    // Keep on trying
    init_ਖਿੜਕੀ_hints_state = 1;
    ਵਾਪਸ TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ਪੂਰਨ
gui_mch_open(ਖਾਲੀ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_ਚੌੜਾਈ;
    guint		pixel_ਉਚਾਈ;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    ਜੇ (role_ਦਲੀਲ != ਨਿਰਾ)
    {
	gtk_ਖਿੜਕੀ_ਸੈੱਟ_role(GTK_ਖਿੜਕੀ(gui.mainwin), role_ਦਲੀਲ);
    }
    ਨਹੀਂਤਾਂ
    {
	ਅੱਖਰ *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (ਬਿਨਾਨਿਸ਼ਾਨੀ)mch_get_pid(),
			       (ਬਿਨਾਨਿਸ਼ਾਨੀ)g_random_int(),
			       (ਬਿਨਾਨਿਸ਼ਾਨੀ)ਸਮਾਂ(ਨਿਰਾ));

	gtk_ਖਿੜਕੀ_ਸੈੱਟ_role(GTK_ਖਿੜਕੀ(gui.mainwin), role);
	g_free(role);
    }

    ਜੇ (gui_win_x != -1 && gui_win_y != -1)
	gtk_ਖਿੜਕੀ_move(GTK_ਖਿੜਕੀ(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    ਜੇ (gui.geom != ਨਿਰਾ)
    {
	ਪੂਰਨ		ਨਕਾਬ;
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ	w, h;
	ਪੂਰਨ		x = 0;
	ਪੂਰਨ		y = 0;

	ਨਕਾਬ = XParseGeometry((ਅੱਖਰ *)gui.geom, &x, &y, &w, &h);

	ਜੇ (ਨਕਾਬ & ਚੌੜਾਈ_ਮੁੱਲ)
	    Columns = w;
	ਜੇ (ਨਕਾਬ & ਉਚਾਈ_ਮੁੱਲ)
	{
	    ਜੇ (p_ਖਿੜਕੀ > (ਲੰਮਾ)h - 1 || !option_was_ਸੈੱਟ((char_u *)"window"))
		p_ਖਿੜਕੀ = h - 1;
	    ਪੰਕਤੀਆਂ = h;
	}
	ਹੱਦਬੰਦੀ_screen_ਮਾਪ();

	pixel_ਚੌੜਾਈ = (guint)(gui_get_ਅਧਾਰ_ਚੌੜਾਈ() + Columns * gui.char_ਚੌੜਾਈ);
	pixel_ਉਚਾਈ = (guint)(gui_get_ਅਧਾਰ_ਉਚਾਈ() + ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ);

	pixel_ਚੌੜਾਈ  += get_menu_tool_ਚੌੜਾਈ();
	pixel_ਉਚਾਈ += get_menu_tool_ਉਚਾਈ();

	ਜੇ (ਨਕਾਬ & (XValue | YValue))
	{
	    ਪੂਰਨ ww, hh;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#ਨਹੀਂਤਾਂ
	    gui_mch_get_screen_dimensions(&ww, &hh);
#ਜੇ_ਅੰਤ
	    hh += p_ghr + get_menu_tool_ਉਚਾਈ();
	    ww += get_menu_tool_ਚੌੜਾਈ();
	    ਜੇ (ਨਕਾਬ & XNegative)
		x += ww - pixel_ਚੌੜਾਈ;
	    ਜੇ (ਨਕਾਬ & YNegative)
		y += hh - pixel_ਉਚਾਈ;
	    gtk_ਖਿੜਕੀ_move(GTK_ਖਿੜਕੀ(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	ਜੇ (gtk_socket_id != 0  &&  (ਨਕਾਬ & ਚੌੜਾਈ_ਮੁੱਲ || ਨਕਾਬ & ਉਚਾਈ_ਮੁੱਲ))
	{
	    update_ਖਿੜਕੀ_manager_hints(pixel_ਚੌੜਾਈ, pixel_ਉਚਾਈ);
	    init_ਖਿੜਕੀ_hints_state = 1;
	    timeout_ਜੋੜੋ(1000, check_startup_plug_hints, ਨਿਰਾ);
	}
    }

    pixel_ਚੌੜਾਈ = (guint)(gui_get_ਅਧਾਰ_ਚੌੜਾਈ() + Columns * gui.char_ਚੌੜਾਈ);
    pixel_ਉਚਾਈ = (guint)(gui_get_ਅਧਾਰ_ਉਚਾਈ() + ਪੰਕਤੀਆਂ * gui.char_ਉਚਾਈ);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    ਜੇ (gtk_socket_id == 0)
	gtk_ਖਿੜਕੀ_resize(GTK_ਖਿੜਕੀ(gui.mainwin), pixel_ਚੌੜਾਈ, pixel_ਉਚਾਈ);
    update_ਖਿੜਕੀ_manager_hints(0, 0);

    ਜੇ (foreground_ਦਲੀਲ != ਨਿਰਾ)
	fg_pixel = gui_get_color((char_u *)foreground_ਦਲੀਲ);
    ਜੇ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ਜੇ (background_ਦਲੀਲ != ਨਿਰਾ)
	bg_pixel = gui_get_color((char_u *)background_ਦਲੀਲ);
    ਜੇ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ਜੇ (ਮਿਲਿਆ_ਉਲਟ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_ਪਿੱਠ_pixel = fg_pixel;
    }
    ਨਹੀਂਤਾਂ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_ਪਿੱਠ_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    ਸੈੱਟ_ਸਾਧਾਰਨ_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ਨਿਰਾ);

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
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#ਜੇ_ਅੰਤ
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), ਨਿਰਾ);
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ਗੁਣਕ_event), ਨਿਰਾ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_ਸੈੱਟ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_ਅੰਕੜੇ_received_cb), ਨਿਰਾ);
#ਜੇ_ਅੰਤ

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	ਜੇ (ਮਿਲਿਆ_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#ਜੇ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ menu_handler = 0;
# ifdef FEAT_TOOLBAR
	ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	ਜੇ (using_gnome && vim_strchr(p_go, GO_MENUS) == ਨਿਰਾ)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ਨਿਰਾ);
# ifdef FEAT_TOOLBAR
	ਜੇ (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == ਨਿਰਾ
		&& (toolbar_flags & (TOOLBAR_ਪਾਠ | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ਨਿਰਾ);
# endif
#ਜੇ_ਅੰਤ
	gtk_widget_show(gui.mainwin);

#ਜੇ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	ਜੇ (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	ਜੇ (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#ਜੇ_ਅੰਤ
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), ਨਿਰਾ);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), ਨਿਰਾ);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), ਨਿਰਾ);

    ਵਾਪਸ OK;
}

/*
 * Clean up for when exiting Vim.
 */
    ਖਾਲੀ
gui_mch_exit(ਪੂਰਨ rc ਅਣਵਰਤਿਆ)
{
    // Clean up, unless we don't want to invoke free().
    ਜੇ (gui.mainwin != ਨਿਰਾ && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    ਪੂਰਨ
gui_mch_get_winpos(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    ਜੇ (gui_mch_get_display())
    {
	gtk_ਖਿੜਕੀ_get_position(GTK_ਖਿੜਕੀ(gui.mainwin), x, y);
	ਵਾਪਸ OK;
    }
    ਵਾਪਸ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_winpos(ਪੂਰਨ x, ਪੂਰਨ y)
{
    gtk_ਖਿੜਕੀ_move(GTK_ਖਿੜਕੀ(gui.mainwin), x, y);
}

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
# ਜੇ 0
ਸਥਿਰ ਪੂਰਨ resize_idle_installed = FALSE;
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
    ਸਥਿਰ gboolean
force_shell_resize_idle(gpointer ਅੰਕੜੇ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	ਪੂਰਨ ਚੌੜਾਈ;
	ਪੂਰਨ ਉਚਾਈ;

	gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin), &ਚੌੜਾਈ, &ਉਚਾਈ);

	ਚੌੜਾਈ  -= get_menu_tool_ਚੌੜਾਈ();
	ਉਚਾਈ -= get_menu_tool_ਉਚਾਈ();

	gui_resize_shell(ਚੌੜਾਈ, ਉਚਾਈ);
    }

    resize_idle_installed = FALSE;
    ਵਾਪਸ FALSE; // don't call me again
}
# endif
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    ਪੂਰਨ
gui_mch_maximized(ਖਾਲੀ)
{
    ਵਾਪਸ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_ਖਿੜਕੀ(gui.mainwin) != ਨਿਰਾ
	    && (gdk_ਖਿੜਕੀ_get_state(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin))
					       & GDK_ਖਿੜਕੀ_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    ਖਾਲੀ
gui_mch_unmaximize(ਖਾਲੀ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ)
	gtk_ਖਿੜਕੀ_unmaximize(GTK_ਖਿੜਕੀ(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ਖਾਲੀ
gui_mch_newfont(ਖਾਲੀ)
{
    ਪੂਰਨ w, h;

    gtk_ਖਿੜਕੀ_get_ਮਾਪ(GTK_ਖਿੜਕੀ(gui.mainwin), &w, &h);
    w -= get_menu_tool_ਚੌੜਾਈ();
    h -= get_menu_tool_ਉਚਾਈ();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_shellsize(ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ,
		      ਪੂਰਨ min_ਚੌੜਾਈ ਅਣਵਰਤਿਆ,  ਪੂਰਨ min_ਉਚਾਈ ਅਣਵਰਤਿਆ,
		      ਪੂਰਨ ਅਧਾਰ_ਚੌੜਾਈ ਅਣਵਰਤਿਆ, ਪੂਰਨ ਅਧਾਰ_ਉਚਾਈ ਅਣਵਰਤਿਆ,
		      ਪੂਰਨ direction ਅਣਵਰਤਿਆ)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    ਜੇ (gtk_socket_id == 0)
	update_ਖਿੜਕੀ_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    ਚੌੜਾਈ  += get_menu_tool_ਚੌੜਾਈ();
    ਉਚਾਈ += get_menu_tool_ਉਚਾਈ();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    ਚੌੜਾਈ  += mch_csd_ਚੌੜਾਈ;
    ਉਚਾਈ += mch_csd_ਉਚਾਈ;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_ਚੌੜਾਈ > 0)
			    ? ਚੌੜਾਈ - get_menu_tool_ਚੌੜਾਈ() - mch_csd_ਚੌੜਾਈ : 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ TRACK_RESIZE_HISTORY
    alloc_resize_hist(ਚੌੜਾਈ, ਉਚਾਈ, TRUE);
#ਜੇ_ਅੰਤ
    ਜੇ (gtk_socket_id == 0)
	gtk_ਖਿੜਕੀ_resize(GTK_ਖਿੜਕੀ(gui.mainwin), ਚੌੜਾਈ, ਉਚਾਈ);
    ਨਹੀਂਤਾਂ
	update_ਖਿੜਕੀ_manager_hints(ਚੌੜਾਈ, ਉਚਾਈ);

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
# ਜੇ 0
    ਜੇ (!resize_idle_installed)
    {
	g_idle_ਜੋੜੋ_ਭਰਿਆ(GDK_ਤਰਜੀਹ_EVENTS + 10,
			&force_shell_resize_idle, ਨਿਰਾ, ਨਿਰਾ);
	resize_idle_installed = TRUE;
    }
# endif
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    ਖਾਲੀ
gui_mch_settitle(char_u *title, char_u *icon ਅਣਵਰਤਿਆ)
{
    ਜੇ (title != ਨਿਰਾ && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, ਨਿਰਾ);

    gtk_ਖਿੜਕੀ_ਸੈੱਟ_title(GTK_ਖਿੜਕੀ(gui.mainwin), (ਅਚਲ ਅੱਖਰ *)title);

    ਜੇ (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#ਜੇ defined(FEAT_MENU)
    ਖਾਲੀ
gui_mch_enable_menu(ਪੂਰਨ showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    ਜੇ (using_gnome)
	widget = gui.menubar_h;
    ਨਹੀਂਤਾਂ
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    ਜੇ (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	ਜੇ (showit)
	    gtk_widget_show(widget);
	ਨਹੀਂਤਾਂ
	    gtk_widget_hide(widget);

	update_ਖਿੜਕੀ_manager_hints(0, 0);
    }
}
#ਜੇ_ਅੰਤ // FEAT_MENU

#ਜੇ defined(FEAT_TOOLBAR)
    ਖਾਲੀ
gui_mch_show_toolbar(ਪੂਰਨ showit)
{
    GtkWidget *widget;

    ਜੇ (gui.toolbar == ਨਿਰਾ)
	ਵਾਪਸ;

# ifdef FEAT_GUI_GNOME
    ਜੇ (using_gnome)
	widget = gui.toolbar_h;
    ਨਹੀਂਤਾਂ
# endif
	widget = gui.toolbar;

    ਜੇ (showit)
	ਸੈੱਟ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    ਜੇ (!showit != !gtk_widget_get_visible(widget))
    {
	ਜੇ (showit)
	    gtk_widget_show(widget);
	ਨਹੀਂਤਾਂ
	    gtk_widget_hide(widget);

	update_ਖਿੜਕੀ_manager_hints(0, 0);
    }
}
#ਜੇ_ਅੰਤ // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    ਸਥਿਰ ਪੂਰਨ
is_cjk_font(PangoFontDescription *font_desc)
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	i;
    ਪੂਰਨ		is_cjk = FALSE;

    font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, font_desc);

    ਜੇ (font == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

    ਲਈ (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	ਜੇ (coverage != ਨਿਰਾ)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#ਜੇ PANGO_ਸੰਸਕਰਣ_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#ਨਹੀਂਤਾਂ
	    pango_coverage_unref(coverage);
#ਜੇ_ਅੰਤ
	}
    }

    g_object_unref(font);

    ਵਾਪਸ is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ਪੂਰਨ
gui_mch_adjust_charheight(ਖਾਲੀ)
{
    PangoFontMetrics	*metrics;
    ਪੂਰਨ			ascent;
    ਪੂਰਨ			descent;

    metrics = pango_context_get_metrics(gui.ਪਾਠ_context, gui.norm_font,
				pango_context_get_language(gui.ਪਾਠ_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_ਉਚਾਈ = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ਉਚਾਈ = MAX(gui.char_ਉਚਾਈ, gui.char_ascent + 1);

    ਵਾਪਸ OK;
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ਸਥਿਰ gboolean
font_filter(ਅਚਲ PangoFontFamily *family,
	    ਅਚਲ PangoFontFace   *face ਅਣਵਰਤਿਆ,
	    gpointer		   ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ pango_font_family_is_monospace((PangoFontFamily *)family);
}
#ਜੇ_ਅੰਤ

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
    ਪੂਰਨ		response;
    char_u	*fontname = ਨਿਰਾ;
    char_u	*oldname;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
    dialog = gtk_font_chooser_dialog_new(ਨਿਰਾ, ਨਿਰਾ);
    gtk_font_chooser_ਸੈੱਟ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    ਨਿਰਾ, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
    dialog = gtk_font_selection_dialog_new(ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    gtk_ਖਿੜਕੀ_ਸੈੱਟ_transient_for(GTK_ਖਿੜਕੀ(dialog), GTK_ਖਿੜਕੀ(gui.mainwin));
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_destroy_with_ਮਾਪਾ(GTK_ਖਿੜਕੀ(dialog), TRUE);

    ਜੇ (oldval != ਨਿਰਾ && oldval[0] != NUL)
    {
	ਜੇ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ਨਿਰਾ);
	ਨਹੀਂਤਾਂ
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	ਜੇ (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    ਜੇ (p != ਨਿਰਾ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ਜੇ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	gtk_font_chooser_ਸੈੱਟ_font(
		GTK_FONT_CHOOSER(dialog), (ਅਚਲ gchar *)oldname);
#ਨਹੀਂਤਾਂ
	gtk_font_selection_dialog_ਸੈੱਟ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ਅਚਲ ਅੱਖਰ *)oldname);
#ਜੇ_ਅੰਤ

	ਜੇ (oldname != oldval)
	    vim_free(oldname);
    }
    ਨਹੀਂਤਾਂ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	gtk_font_chooser_ਸੈੱਟ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#ਨਹੀਂਤਾਂ
	gtk_font_selection_dialog_ਸੈੱਟ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#ਜੇ_ਅੰਤ

    response = gtk_dialog_ਲੜੀਵਾਰ(GTK_DIALOG(dialog));

    ਜੇ (response == GTK_RESPONSE_OK)
    {
	ਅੱਖਰ *name;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#ਨਹੀਂਤਾਂ
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#ਜੇ_ਅੰਤ
	ਜੇ (name != ਨਿਰਾ)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ਜੇ (p != ਨਿਰਾ && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ਨਿਵੇਸ਼_conv, p, ਨਿਰਾ);
		vim_free(p);
	    }
	    ਨਹੀਂਤਾਂ
		fontname = p;
	}
    }

    ਜੇ (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    ਵਾਪਸ fontname;
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
    ਸਥਿਰ ਖਾਲੀ
get_styled_font_variants(ਖਾਲੀ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, gui.norm_font);

    ਜੇ (plain_font == ਨਿਰਾ)
	ਵਾਪਸ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_ਸੈੱਟ_ਭਾਰ(bold_font_desc, PANGO_ਭਾਰ_BOLD);

    bold_font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    ਜੇ (bold_font != ਨਿਰਾ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    ਜੇ (bold_font != ਨਿਰਾ && gui.font_can_bold)
	g_object_unref(plain_font);
}

ਸਥਿਰ PangoEngineShape *default_shape_engine = ਨਿਰਾ;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    ਸਥਿਰ ਖਾਲੀ
ascii_glyph_ਸਾਰਣੀ_init(ਖਾਲੀ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *ਵਸਤੂ_list;
    ਪੂਰਨ		    i;

    ਜੇ (gui.ascii_glyphs != ਨਿਰਾ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ਜੇ (gui.ascii_font != ਨਿਰਾ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ਨਿਰਾ;
    gui.ascii_font   = ਨਿਰਾ;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    ਲਈ (i = 0; i < 128; ++i)
    {
	ਜੇ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ਨਹੀਂਤਾਂ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    ਵਸਤੂ_list = pango_itemize(gui.ਪਾਠ_context, (ਅਚਲ ਅੱਖਰ *)ascii_chars,
			      0, ਆਕਾਰ(ascii_chars), attr_list, ਨਿਰਾ);

    ਜੇ (ਵਸਤੂ_list != ਨਿਰਾ && ਵਸਤੂ_list->ਅਗਲਾ == ਨਿਰਾ) // play safe
    {
	Pango_ਵਸਤੂ   *ਵਸਤੂ;
	ਪੂਰਨ	    ਚੌੜਾਈ;

	ਵਸਤੂ  = (Pango_ਵਸਤੂ *)ਵਸਤੂ_list->ਅੰਕੜੇ;
	ਚੌੜਾਈ = gui.char_ਚੌੜਾਈ * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = ਵਸਤੂ->analysis.shape_engine;

	gui.ascii_font = ਵਸਤੂ->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ਅਚਲ ਅੱਖਰ *)ascii_chars, ਆਕਾਰ(ascii_chars),
		    &ਵਸਤੂ->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->ਅੰਕ_glyphs == ਆਕਾਰ(ascii_chars));

	ਲਈ (i = 0; i < gui.ascii_glyphs->ਅੰਕ_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_ਵਿੱਥ += MAX(0, ਚੌੜਾਈ - geom->ਚੌੜਾਈ) / 2;
	    geom->ਚੌੜਾਈ = ਚੌੜਾਈ;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(ਵਸਤੂ_list, (GFunc)(ਖਾਲੀ *)&pango_ਵਸਤੂ_free, ਨਿਰਾ);
    g_list_free(ਵਸਤੂ_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    ਪੂਰਨ
gui_mch_init_font(char_u *font_name, ਪੂਰਨ fontset ਅਣਵਰਤਿਆ)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ਪੂਰਨ			    ਚੌੜਾਈ;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    ਜੇ (font_name == ਨਿਰਾ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    ਜੇ (font_desc == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_ਸੈੱਟ_font_description(gui.ਪਾਠ_context, font_desc);

    layout = pango_layout_new(gui.ਪਾਠ_context);
    pango_layout_ਸੈੱਟ_ਪਾਠ(layout, "MW", 2);
    pango_layout_get_ਮਾਪ(layout, &ਚੌੜਾਈ, ਨਿਰਾ);
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
    ਜੇ (is_cjk_font(gui.norm_font))
    {
	ਪੂਰਨ cjk_ਚੌੜਾਈ;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_ਸੈੱਟ_ਪਾਠ(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_ਮਾਪ(layout, &cjk_ਚੌੜਾਈ, ਨਿਰਾ);

	ਜੇ (ਚੌੜਾਈ == cjk_ਚੌੜਾਈ)  // Xft not patched
	    ਚੌੜਾਈ /= 2;
    }
    g_object_unref(layout);

    gui.char_ਚੌੜਾਈ = (ਚੌੜਾਈ / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    ਜੇ (gui.char_ਚੌੜਾਈ <= 0)
	gui.char_ਚੌੜਾਈ = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_ਸੈੱਟ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_ਸਾਰਣੀ_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ਜੇ (gui.wide_font != ਨਿਰਾ
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = ਨਿਰਾ;
    }

    ਜੇ (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    ਨਹੀਂਤਾਂ
    {
	// Preserve the logical dimensions of the screen.
	update_ਖਿੜਕੀ_manager_hints(0, 0);
    }

    ਵਾਪਸ OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, ਪੂਰਨ report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    ਜੇ (!gui.ਵਿੱਚ_use || name == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;

    ਜੇ (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, ਨਿਰਾ);
	ਜੇ (buf != ਨਿਰਾ)
	{
	    font = pango_font_description_from_string((ਅਚਲ ਅੱਖਰ *)buf);
	    vim_free(buf);
	}
	ਨਹੀਂਤਾਂ
	    font = ਨਿਰਾ;
    }
    ਨਹੀਂਤਾਂ
	font = pango_font_description_from_string((ਅਚਲ ਅੱਖਰ *)name);

    ਜੇ (font != ਨਿਰਾ)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	ਜੇ (pango_font_description_get_ਮਾਪ(font) <= 0)
	    pango_font_description_ਸੈੱਟ_ਮਾਪ(font, 10 * PANGO_SCALE);

	real_font = pango_context_ਲੱਦੋ_font(gui.ਪਾਠ_context, font);

	ਜੇ (real_font == ਨਿਰਾ)
	{
	    pango_font_description_free(font);
	    font = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
	    g_object_unref(real_font);
    }

    ਜੇ (font == ਨਿਰਾ)
    {
	ਜੇ (report_error)
	    semsg(_((ਅੱਖਰ *)e_unknown_font_str), name);
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਵਾਪਸ font;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name ਅਣਵਰਤਿਆ)
{
    ਜੇ (font != NOFONT)
    {
	ਅੱਖਰ	*pangoname = pango_font_description_ਮੰਜ਼ਿਲ_string(font);

	ਜੇ (pangoname != ਨਿਰਾ)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    ਵਾਪਸ s;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ

/*
 * If a font is not going to be used, free its structure.
 */
    ਖਾਲੀ
gui_mch_free_font(GuiFont font)
{
    ਜੇ (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
    ਖਾਲੀ
gui_mch_expand_font(
    optexpand_T	*ਦਲੀਲਾਂ,
    ਖਾਲੀ	*param,
    ਪੂਰਨ		(*ਜੋੜੋ_ਮੇਲ)(char_u *val))
{
    PangoFontFamily	**font_families = ਨਿਰਾ;
    ਪੂਰਨ			n_families = 0;
    ਪੂਰਨ			wide = *(ਪੂਰਨ *)param;

    ਜੇ (ਦਲੀਲਾਂ->oe_include_orig_val && *ਦਲੀਲਾਂ->oe_opt_ਮੁੱਲ == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	ਜੇ (ਜੋੜੋ_ਮੇਲ((char_u *)DEFAULT_FONT) != OK)
	    ਵਾਪਸ;
    }

    pango_context_list_families(
	    gui.ਪਾਠ_context,
	    &font_families,
	    &n_families);

    ਲਈ (ਪੂਰਨ i = 0; i < n_families; i++)
    {
	ਜੇ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ਜਾਰੀ;

	ਅਚਲ ਅੱਖਰ* fam_name = pango_font_family_get_name(font_families[i]);
	ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ਨਿਵੇਸ਼_conv, (char_u*)fam_name, ਨਿਰਾ);
	    ਜੇ (buf != ਨਿਰਾ)
	    {
		ਜੇ (ਜੋੜੋ_ਮੇਲ(buf) != OK)
		{
		    vim_free(buf);
		    ਰੋਕੋ;
		}
		vim_free(buf);
	    }
	    ਨਹੀਂਤਾਂ
		ਰੋਕੋ;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (ਜੋੜੋ_ਮੇਲ((char_u *)fam_name) != OK)
		ਰੋਕੋ;
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

    ਜੇ (!gui.ਵਿੱਚ_use)		// can't do this when GUI not running
	ਵਾਪਸ color;

    ਜੇ (name != ਨਿਰਾ)
	color = gui_get_color_cmn(name);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਵਾਪਸ color;
#ਨਹੀਂਤਾਂ
    ਜੇ (color == INVALCOLOR)
	ਵਾਪਸ INVALCOLOR;

    ਵਾਪਸ gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#ਜੇ_ਅੰਤ
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ਪੂਰਨ r, ਪੂਰਨ g, ਪੂਰਨ b)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਵਾਪਸ gui_get_rgb_color_cmn(r, g, b);
#ਨਹੀਂਤਾਂ
    GdkColor gcolor;
    ਪੂਰਨ ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    ਵਾਪਸ ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#ਜੇ_ਅੰਤ
}

/*
 * Set the current text foreground color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_fg_color(guicolor_T color)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    *gui.fgcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
#ਨਹੀਂਤਾਂ
    gui.fgcolor->pixel = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)color;
#ਜੇ_ਅੰਤ
}

/*
 * Set the current text background color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_bg_color(guicolor_T color)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    *gui.bgcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
#ਨਹੀਂਤਾਂ
    gui.bgcolor->pixel = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)color;
#ਜੇ_ਅੰਤ
}

/*
 * Set the current text special color.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_sp_color(guicolor_T color)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    *gui.spcolor = color_ਮੰਜ਼ਿਲ_rgba(color);
#ਨਹੀਂਤਾਂ
    gui.spcolor->pixel = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)color;
#ਜੇ_ਅੰਤ
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਪਾਓ_PANGO_ATTR(Attribute, AttrList, ਸ਼ੁਰੂ_2, ਸਿਰਾ_2)  \
    G_STMT_ਸ਼ੁਰੂ{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ਸ਼ੁਰੂ_ਸੂਚਕਾਂਕ = (ਸ਼ੁਰੂ_2);		    \
	tmp_attr_->ਸਿਰਾ_ਸੂਚਕਾਂਕ = (ਸਿਰਾ_2);			    \
	pango_attr_list_ਪਾਓ((AttrList), tmp_attr_);	    \
    }G_STMT_ਸਿਰਾ

    ਸਥਿਰ ਖਾਲੀ
apply_wide_font_attr(char_u *s, ਪੂਰਨ len, PangoAttrList *attr_list)
{
    char_u  *ਸ਼ੁਰੂ = ਨਿਰਾ;
    char_u  *p;
    ਪੂਰਨ	    uc;

    ਲਈ (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ਜੇ (ਸ਼ੁਰੂ == ਨਿਰਾ)
	{
	    ਜੇ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ਸ਼ੁਰੂ = p;
	}
	ਨਹੀਂਤਾਂ ਜੇ (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ਪਾਓ_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, ਸ਼ੁਰੂ - s, p - s);
	    ਸ਼ੁਰੂ = ਨਿਰਾ;
	}
    }

    ਜੇ (ਸ਼ੁਰੂ != ਨਿਰਾ)
	ਪਾਓ_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, ਸ਼ੁਰੂ - s, len);
}

    ਸਥਿਰ ਪੂਰਨ
ਗਿਣਤੀ_cluster_cells(char_u *s, Pango_ਵਸਤੂ *ਵਸਤੂ,
		    PangoGlyphString* glyphs, ਪੂਰਨ i,
		    ਪੂਰਨ *cluster_ਚੌੜਾਈ,
		    ਪੂਰਨ *ਆਖਰੀ_glyph_rbearing)
{
    char_u  *p;
    ਪੂਰਨ	    ਅਗਲਾ;	// glyph start index of next cluster
    ਪੂਰਨ	    ਸ਼ੁਰੂ, ਸਿਰਾ; // string segment of current cluster
    ਪੂਰਨ	    ਚੌੜਾਈ;	// real cluster width in Pango units
    ਪੂਰਨ	    uc;
    ਪੂਰਨ	    cellcount = 0;

    ਚੌੜਾਈ = glyphs->glyphs[i].geometry.ਚੌੜਾਈ;

    ਲਈ (ਅਗਲਾ = i + 1; ਅਗਲਾ < glyphs->ਅੰਕ_glyphs; ++ਅਗਲਾ)
    {
	ਜੇ (glyphs->glyphs[ਅਗਲਾ].attr.is_cluster_ਸ਼ੁਰੂ)
	    ਰੋਕੋ;
	ਨਹੀਂਤਾਂ ਜੇ (glyphs->glyphs[ਅਗਲਾ].geometry.ਚੌੜਾਈ > ਚੌੜਾਈ)
	    ਚੌੜਾਈ = glyphs->glyphs[ਅਗਲਾ].geometry.ਚੌੜਾਈ;
    }

    ਸ਼ੁਰੂ = ਵਸਤੂ->ਵਿੱਥ + glyphs->log_clusters[i];
    ਸਿਰਾ   = ਵਸਤੂ->ਵਿੱਥ + ((ਅਗਲਾ < glyphs->ਅੰਕ_glyphs) ?
			    glyphs->log_clusters[ਅਗਲਾ] : ਵਸਤੂ->ਲੰਮਾਈ);

    ਲਈ (p = s + ਸ਼ੁਰੂ; p < s + ਸਿਰਾ; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ਜੇ (uc < 0x80)
	    ++cellcount;
	ਨਹੀਂਤਾਂ ਜੇ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ਜੇ (ਆਖਰੀ_glyph_rbearing != ਨਿਰਾ
	    && cellcount > 0 && ਅਗਲਾ == glyphs->ਅੰਕ_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(ਵਸਤੂ->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, ਨਿਰਾ);

	ਜੇ (PANGO_RBEARING(ink_rect) > 0)
	    *ਆਖਰੀ_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ਜੇ (cellcount > 0)
	*cluster_ਚੌੜਾਈ = ਚੌੜਾਈ;

    ਵਾਪਸ cellcount;
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
    ਸਥਿਰ ਖਾਲੀ
ਤਿਆਰੀ_zero_ਚੌੜਾਈ_cluster(Pango_ਵਸਤੂ *ਵਸਤੂ, PangoGlyphInfo *glyph,
			 ਪੂਰਨ ਆਖਰੀ_cellcount, ਪੂਰਨ ਆਖਰੀ_cluster_ਚੌੜਾਈ,
			 ਪੂਰਨ ਆਖਰੀ_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    ਪੂਰਨ		    ਚੌੜਾਈ;

    ਚੌੜਾਈ = ਆਖਰੀ_cellcount * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
    glyph->geometry.x_ਵਿੱਥ = -ਚੌੜਾਈ + MAX(0, ਚੌੜਾਈ - ਆਖਰੀ_cluster_ਚੌੜਾਈ) / 2;
    glyph->geometry.ਚੌੜਾਈ = 0;

    pango_font_get_glyph_extents(ਵਸਤੂ->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    ਜੇ (ink_rect.x < 0)
    {
	glyph->geometry.x_ਵਿੱਥ += ਆਖਰੀ_glyph_rbearing;
	glyph->geometry.y_ਵਿੱਥ  = logical_rect.ਉਚਾਈ
		- (gui.char_ਉਚਾਈ - p_linespace) * PANGO_SCALE;
    }
    ਨਹੀਂਤਾਂ
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_ਵਿੱਥ = -ਚੌੜਾਈ + MAX(0, ਚੌੜਾਈ - ink_rect.ਚੌੜਾਈ) / 2;
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
draw_glyph_string(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਅੰਕ_cells, ਪੂਰਨ flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
draw_glyph_string(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ ਅੰਕ_cells, ਪੂਰਨ flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#ਜੇ_ਅੰਤ
{
    ਜੇ (!(flags & DRAW_TRANSP))
    {
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ),
			ਅੰਕ_cells * gui.char_ਚੌੜਾਈ, gui.char_ਉਚਾਈ);
	cairo_fill(cr);
#ਨਹੀਂਤਾਂ
	gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2,
			   gui.ਪਾਠ_gc,
			   TRUE,
			   FILL_X(ਖਾਨਾ),
			   FILL_Y(ਪੰਕਤੀ),
			   ਅੰਕ_cells * gui.char_ਚੌੜਾਈ,
			   gui.char_ਉਚਾਈ);
#ਜੇ_ਅੰਤ
    }

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_ਮੰਜ਼ਿਲ(cr, ਪਾਠ_X(ਖਾਨਾ), ਪਾਠ_Y(ਪੰਕਤੀ));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#ਨਹੀਂਤਾਂ
    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->ਖਿੜਕੀ_2,
		    gui.ਪਾਠ_gc,
		    font,
		    ਪਾਠ_X(ਖਾਨਾ),
		    ਪਾਠ_Y(ਪੰਕਤੀ),
		    glyphs);
#ਜੇ_ਅੰਤ

    // redraw the contents with an offset of 1 to emulate bold
    ਜੇ ((flags & DRAW_BOLD) && !gui.font_can_bold)
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_ਮੰਜ਼ਿਲ(cr, ਪਾਠ_X(ਖਾਨਾ) + 1, ਪਾਠ_Y(ਪੰਕਤੀ));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#ਨਹੀਂਤਾਂ
	gdk_draw_glyphs(gui.drawarea->ਖਿੜਕੀ_2,
			gui.ਪਾਠ_gc,
			font,
			ਪਾਠ_X(ਖਾਨਾ) + 1,
			ਪਾਠ_Y(ਪੰਕਤੀ),
			glyphs);
#ਜੇ_ਅੰਤ
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
draw_under(ਪੂਰਨ flags, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ cells, cairo_t *cr)
#ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਖਾਲੀ
draw_under(ਪੂਰਨ flags, ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ cells)
#ਜੇ_ਅੰਤ
{
    ਪੂਰਨ			i;
    ਪੂਰਨ			ਵਿੱਥ;
    ਸਥਿਰ ਅਚਲ ਪੂਰਨ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    ਪੂਰਨ			y = FILL_Y(ਪੰਕਤੀ + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    ਜੇ (flags & DRAW_UNDERC)
    {
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(cr, 1.0);
	cairo_ਸੈੱਟ_ਸਤਰ_cap(cr, CAIRO_ਸਤਰ_CAP_BUTT);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_ਮੰਜ਼ਿਲ(cr, FILL_X(ਖਾਨਾ) + 1, y - 2 + 0.5);
	ਲਈ (i = FILL_X(ਖਾਨਾ) + 1; i < FILL_X(ਖਾਨਾ + cells); ++i)
	{
	    ਵਿੱਥ = val[i % 8];
	    cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, i, y - ਵਿੱਥ + 0.5);
	}
	cairo_stroke(cr);
#ਨਹੀਂਤਾਂ
	gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.spcolor);
	ਲਈ (i = FILL_X(ਖਾਨਾ); i < FILL_X(ਖਾਨਾ + cells); ++i)
	{
	    ਵਿੱਥ = val[i % 8];
	    gdk_draw_ਬਿੰਦੂ(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc, i, y - ਵਿੱਥ);
	}
	gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
#ਜੇ_ਅੰਤ
    }

    // Draw a strikethrough line
    ਜੇ (flags & DRAW_STRIKE)
    {
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(cr, 1.0);
	cairo_ਸੈੱਟ_ਸਤਰ_cap(cr, CAIRO_ਸਤਰ_CAP_BUTT);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_ਮੰਜ਼ਿਲ(cr, FILL_X(ਖਾਨਾ), y + 1 - gui.char_ਉਚਾਈ/2 + 0.5);
	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, FILL_X(ਖਾਨਾ + cells), y + 1 - gui.char_ਉਚਾਈ/2 + 0.5);
	cairo_stroke(cr);
#ਨਹੀਂਤਾਂ
	gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.spcolor);
	gdk_draw_ਸਤਰ(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
		      FILL_X(ਖਾਨਾ), y + 1 - gui.char_ਉਚਾਈ/2,
		      FILL_X(ਖਾਨਾ + cells), y + 1 - gui.char_ਉਚਾਈ/2);
	gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
#ਜੇ_ਅੰਤ
    }

    // Underline: draw a line at the bottom of the character cell.
    ਜੇ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ਜੇ (p_linespace > 1)
	    y -= p_linespace - 1;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(cr, 1.0);
	cairo_ਸੈੱਟ_ਸਤਰ_cap(cr, CAIRO_ਸਤਰ_CAP_BUTT);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_ਮੰਜ਼ਿਲ(cr, FILL_X(ਖਾਨਾ), y + 0.5);
	cairo_ਸਤਰ_ਮੰਜ਼ਿਲ(cr, FILL_X(ਖਾਨਾ + cells), y + 0.5);
	cairo_stroke(cr);
#ਨਹੀਂਤਾਂ
	gdk_draw_ਸਤਰ(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
		      FILL_X(ਖਾਨਾ), y,
		      FILL_X(ਖਾਨਾ + cells) - 1, y);
#ਜੇ_ਅੰਤ
    }
}

    ਪੂਰਨ
gui_gtk_draw_string(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, char_u *s, ਪੂਰਨ len, ਪੂਰਨ flags)
{
    char_u	*conv_buf = ਨਿਰਾ;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    ਪੂਰਨ		convlen;
    char_u	*sp, *bp;
    ਪੂਰਨ		plen;
    ਪੂਰਨ		len_ਜੋੜ;	// return value needs to add up since we are
				// printing substrings
    ਪੂਰਨ		byte_ਜੋੜ;	// byte position in string
    char_u	*cs;		// current *s pointer
    ਪੂਰਨ		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    ਪੂਰਨ		should_need_pango = FALSE;
    ਪੂਰਨ		slen;
    ਪੂਰਨ		is_ligature;
    ਪੂਰਨ		is_utf8;
    char_u	backup_ch;

    ਜੇ (gui.ਪਾਠ_context == ਨਿਰਾ || gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ len;

    ਜੇ (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != ਨਿਰਾ, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	ਲਈ (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    ਜੇ ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		ਜੇ (new_conv_buf == ਨਿਰਾ)
		    ਵਾਪਸ len;
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
    len_ਜੋੜ = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_ਜੋੜ = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    ਜੇ (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    ਜੇ (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    ਜਦੋਂ (cs < s + len)
    {
	slen = 0;
	ਜਦੋਂ (slen < (len - byte_ਜੋੜ))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    ਜੇ (is_ligature && !needs_pango)
	    {
		ਜੇ ((slen + 1) < (len - byte_ਜੋੜ))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		ਨਹੀਂਤਾਂ
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    ਜੇ ((!is_utf8) && ((slen + 1) < (len - byte_ਜੋੜ)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    ਜੇ (needs_pango != should_need_pango) // mode switch
		ਰੋਕੋ;
	    ਜੇ (needs_pango)
	    {
		ਜੇ (is_ligature)
		{
		    slen++; // ligature char by char
		}
		ਨਹੀਂਤਾਂ // is_utf8
		{
		    ਜੇ ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			ਜਦੋਂ ((slen < (len - byte_ਜੋੜ))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    ਨਹੀਂਤਾਂ ਜੇ ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
		slen++; // ascii
	    }
	}

	ਜੇ (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_ਜੋੜ += gui_gtk_draw_string_ext(ਪੰਕਤੀ, ਖਾਨਾ + len_ਜੋੜ, cs, slen, flags,
					    needs_pango);
	ਜੇ (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_ਜੋੜ += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    ਵਾਪਸ len_ਜੋੜ;
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ cairo_region_t *dirty_region = ਨਿਰਾ;

    ਸਥਿਰ ਖਾਲੀ
ਕਤਾਰ_draw_area(ਪੂਰਨ x, ਪੂਰਨ y, ਪੂਰਨ ਚੌੜਾਈ, ਪੂਰਨ ਉਚਾਈ)
{
    cairo_rectangle_int_t rect;

    ਜੇ (ਚੌੜਾਈ <= 0 || ਉਚਾਈ <= 0 || gui.drawarea == ਨਿਰਾ)
	ਵਾਪਸ;

    rect.x = x;
    rect.y = y;
    rect.ਚੌੜਾਈ = ਚੌੜਾਈ;
    rect.ਉਚਾਈ = ਉਚਾਈ;

    ਜੇ (dirty_region == ਨਿਰਾ)
	dirty_region = cairo_region_create_rectangle(&rect);
    ਨਹੀਂਤਾਂ
	cairo_region_union_rectangle(dirty_region, &rect);
}
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
gui_gtk_draw_string_ext(
	ਪੂਰਨ	ਪੰਕਤੀ,
	ਪੂਰਨ	ਖਾਨਾ,
	char_u	*s,
	ਪੂਰਨ	len,
	ਪੂਰਨ	flags,
	ਪੂਰਨ	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    ਪੂਰਨ			ਕਾਲਮ_ਵਿੱਥ = 0;  // column offset in cells
    ਪੂਰਨ			i;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_t		*cr;
#ਜੇ_ਅੰਤ

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_ਵਿੱਥ;
    area.y = FILL_Y(ਪੰਕਤੀ);
    area.ਚੌੜਾਈ	= gui.ਅੰਕ_ਖਾਨੇ_ਗਿਣਤੀ * gui.char_ਚੌੜਾਈ;
    area.ਉਚਾਈ = gui.char_ਉਚਾਈ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.ਚੌੜਾਈ, area.ਉਚਾਈ);
    cairo_clip(cr);
#ਨਹੀਂਤਾਂ
    gdk_gc_ਸੈੱਟ_clip_origin(gui.ਪਾਠ_gc, 0, 0);
    gdk_gc_ਸੈੱਟ_clip_rectangle(gui.ਪਾਠ_gc, &area);
#ਜੇ_ਅੰਤ

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    ਜੇ (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ਨਿਰਾ
	    && !force_pango)
    {
	char_u *p;

	ਲਈ (p = s; p < s + len; ++p)
	    ਜੇ (*p & 0x80)
		ਜਾਓ not_ascii;

	pango_glyph_string_ਸੈੱਟ_ਮਾਪ(glyphs, len);

	ਲਈ (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	draw_glyph_string(ਪੰਕਤੀ, ਖਾਨਾ, len, flags, gui.ascii_font, glyphs, cr);
#ਨਹੀਂਤਾਂ
	draw_glyph_string(ਪੰਕਤੀ, ਖਾਨਾ, len, flags, gui.ascii_font, glyphs);
#ਜੇ_ਅੰਤ

	ਕਾਲਮ_ਵਿੱਥ = len;
    }
    ਨਹੀਂਤਾਂ
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*ਵਸਤੂ_list;
	ਪੂਰਨ		cluster_ਚੌੜਾਈ;
	ਪੂਰਨ		ਆਖਰੀ_glyph_rbearing;
	ਪੂਰਨ		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	ਜੇ (!utf_valid_string(s, s + len))
	{
	    ਕਾਲਮ_ਵਿੱਥ = len;
	    ਜਾਓ skipitall;
	}

	// original width of the current cluster
	cluster_ਚੌੜਾਈ = PANGO_SCALE * gui.char_ਚੌੜਾਈ;

	// right bearing of the last non-composing glyph
	ਆਖਰੀ_glyph_rbearing = PANGO_SCALE * gui.char_ਚੌੜਾਈ;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	ਜੇ (gui.wide_font != ਨਿਰਾ)
	    apply_wide_font_attr(s, len, attr_list);

	ਜੇ ((flags & DRAW_BOLD) && gui.font_can_bold)
	    ਪਾਓ_PANGO_ATTR(pango_attr_ਭਾਰ_new(PANGO_ਭਾਰ_BOLD),
			      attr_list, 0, len);
	ਜੇ (flags & DRAW_ITALIC)
	    ਪਾਓ_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	ਵਸਤੂ_list = pango_itemize(gui.ਪਾਠ_context,
				  (ਅਚਲ ਅੱਖਰ *)s, 0, len, attr_list, ਨਿਰਾ);

	ਜਦੋਂ (ਵਸਤੂ_list != ਨਿਰਾ)
	{
	    Pango_ਵਸਤੂ	*ਵਸਤੂ;
	    ਪੂਰਨ		ਵਸਤੂ_cells = 0; // item length in cells

	    ਵਸਤੂ = (Pango_ਵਸਤੂ *)ਵਸਤੂ_list->ਅੰਕੜੇ;
	    ਵਸਤੂ_list = g_list_delete_link(ਵਸਤੂ_list, ਵਸਤੂ_list);
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
	    ਵਸਤੂ->analysis.level = (ਵਸਤੂ->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    ਵਸਤੂ->analysis.shape_engine = default_shape_engine;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_PANGO_SHAPE_ਭਰਿਆ
	    pango_shape_ਭਰਿਆ((ਅਚਲ ਅੱਖਰ *)s + ਵਸਤੂ->ਵਿੱਥ, ਵਸਤੂ->ਲੰਮਾਈ,
		    (ਅਚਲ ਅੱਖਰ *)s, len, &ਵਸਤੂ->analysis, glyphs);
#ਨਹੀਂਤਾਂ
	    pango_shape((ਅਚਲ ਅੱਖਰ *)s + ਵਸਤੂ->ਵਿੱਥ, ਵਸਤੂ->ਲੰਮਾਈ,
			&ਵਸਤੂ->analysis, glyphs);
#ਜੇ_ਅੰਤ
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    ਲਈ (i = 0; i < glyphs->ਅੰਕ_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		ਜੇ (glyph->attr.is_cluster_ਸ਼ੁਰੂ)
		{
		    ਪੂਰਨ cellcount;

		    cellcount = ਗਿਣਤੀ_cluster_cells(
			s, ਵਸਤੂ, glyphs, i, &cluster_ਚੌੜਾਈ,
			(ਵਸਤੂ_list != ਨਿਰਾ) ? &ਆਖਰੀ_glyph_rbearing : ਨਿਰਾ);

		    ਜੇ (cellcount > 0)
		    {
			ਪੂਰਨ ਚੌੜਾਈ;

			ਚੌੜਾਈ = cellcount * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
			glyph->geometry.x_ਵਿੱਥ +=
					    MAX(0, ਚੌੜਾਈ - cluster_ਚੌੜਾਈ) / 2;
			glyph->geometry.ਚੌੜਾਈ = ਚੌੜਾਈ;
		    }
		    ਨਹੀਂਤਾਂ
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			ਤਿਆਰੀ_zero_ਚੌੜਾਈ_cluster(ਵਸਤੂ, glyph, cells,
						 cluster_ਚੌੜਾਈ,
						 ਆਖਰੀ_glyph_rbearing);
		    }

		    ਵਸਤੂ_cells += cellcount;
		    cells = cellcount;
		}
		ਨਹੀਂਤਾਂ ਜੇ (i > 0)
		{
		    ਪੂਰਨ ਚੌੜਾਈ;

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
		    ਜੇ (glyph->geometry.x_ਵਿੱਥ >= 0)
		    {
			glyphs->glyphs[i].geometry.ਚੌੜਾਈ =
					 glyphs->glyphs[i - 1].geometry.ਚੌੜਾਈ;
			glyphs->glyphs[i - 1].geometry.ਚੌੜਾਈ = 0;
		    }
		    ਚੌੜਾਈ = cells * gui.char_ਚੌੜਾਈ * PANGO_SCALE;
		    glyph->geometry.x_ਵਿੱਥ +=
					    MAX(0, ਚੌੜਾਈ - cluster_ਚੌੜਾਈ) / 2;
		}
		ਨਹੀਂਤਾਂ // i == 0 "cannot happen"
		{
		    glyph->geometry.ਚੌੜਾਈ = 0;
		}
	    }

	    //// Aaaaand action! **
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    draw_glyph_string(ਪੰਕਤੀ, ਖਾਨਾ + ਕਾਲਮ_ਵਿੱਥ, ਵਸਤੂ_cells,
			      flags, ਵਸਤੂ->analysis.font, glyphs,
			      cr);
#ਨਹੀਂਤਾਂ
	    draw_glyph_string(ਪੰਕਤੀ, ਖਾਨਾ + ਕਾਲਮ_ਵਿੱਥ, ਵਸਤੂ_cells,
			      flags, ਵਸਤੂ->analysis.font, glyphs);
#ਜੇ_ਅੰਤ

	    pango_ਵਸਤੂ_free(ਵਸਤੂ);

	    ਕਾਲਮ_ਵਿੱਥ += ਵਸਤੂ_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    draw_under(flags, ਪੰਕਤੀ, ਖਾਨਾ, ਕਾਲਮ_ਵਿੱਥ, cr);
#ਨਹੀਂਤਾਂ
    draw_under(flags, ਪੰਕਤੀ, ਖਾਨਾ, ਕਾਲਮ_ਵਿੱਥ);
#ਜੇ_ਅੰਤ

    pango_glyph_string_free(glyphs);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_destroy(cr);
    ਕਤਾਰ_draw_area(FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ),
	    ਕਾਲਮ_ਵਿੱਥ * gui.char_ਚੌੜਾਈ + 1, gui.char_ਉਚਾਈ);
#ਨਹੀਂਤਾਂ
    gdk_gc_ਸੈੱਟ_clip_rectangle(gui.ਪਾਠ_gc, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ ਕਾਲਮ_ਵਿੱਥ;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ਪੂਰਨ
gui_mch_haskey(char_u *name)
{
    ਪੂਰਨ i;

    ਲਈ (i = 0; special_ਕੁੰਜੀਆਂ[i].ਕੁੰਜੀ_sym != 0; i++)
	ਜੇ (name[0] == special_ਕੁੰਜੀਆਂ[i].code0
		&& name[1] == special_ਕੁੰਜੀਆਂ[i].code1)
	    ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ਪੂਰਨ
gui_get_x11_windis(ਖਿੜਕੀ *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    ਜੇ (dpy)
    {
	*dis = dpy;
	*win = GDK_ਖਿੜਕੀ_XID(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin));
	ਵਾਪਸ OK;
    }

    *dis = ਨਿਰਾ;
    *win = 0;
    ਵਾਪਸ FAIL;
}
#ਜੇ_ਅੰਤ

    Display *
gui_mch_get_display(ਖਾਲੀ)
{
    ਜੇ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_ਖਿੜਕੀ(gui.mainwin) != ਨਿਰਾ
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#ਜੇ_ਅੰਤ
	)
	ਵਾਪਸ GDK_ਖਿੜਕੀ_XDISPLAY(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin));
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_beep(ਖਾਲੀ)
{
    GdkDisplay *display;

    ਜੇ (gui.mainwin != ਨਿਰਾ && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    ਨਹੀਂਤਾਂ
	display = gdk_display_get_default();

    ਜੇ (display != ਨਿਰਾ)
	gdk_display_beep(display);
}

    ਖਾਲੀ
gui_mch_flash(ਪੂਰਨ msec)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    // TODO Replace GdkGC with Cairo
    (ਖਾਲੀ)msec;
#ਨਹੀਂਤਾਂ
    GdkGCValues	ਮੁੱਲਾਂ;
    GdkGC	*invert_gc;

    ਜੇ (gui.drawarea->ਖਿੜਕੀ_2 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਮੁੱਲਾਂ.foreground.pixel = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    ਮੁੱਲਾਂ.background.pixel = gui.norm_pixel ^ gui.ਪਿੱਠ_pixel;
    ਮੁੱਲਾਂ.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_ਮੁੱਲਾਂ(gui.drawarea->ਖਿੜਕੀ_2,
				       &ਮੁੱਲਾਂ,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_ਸੈੱਟ_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((ਪੂਰਨ)Columns) + gui.border_ਵਿੱਥ,
		       FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ) + gui.border_ਵਿੱਥ);

    gui_mch_flush();
    ui_delay((ਲੰਮਾ)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((ਪੂਰਨ)Columns) + gui.border_ਵਿੱਥ,
		       FILL_Y((ਪੂਰਨ)ਪੰਕਤੀਆਂ) + gui.border_ਵਿੱਥ);

    gdk_gc_destroy(invert_gc);
#ਜੇ_ਅੰਤ
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ਖਾਲੀ
gui_mch_invert_rectangle(ਪੂਰਨ r, ਪੂਰਨ c, ਪੂਰਨ nr, ਪੂਰਨ nc)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਅਚਲ GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_ਚੌੜਾਈ, nr * gui.char_ਉਚਾਈ
    };
    cairo_t * ਅਚਲ cr = cairo_create(gui.surface);

    cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# ਜੇ CAIRO_ਸੰਸਕਰਣ >= CAIRO_ਸੰਸਕਰਣ_ENCODE(1,9,2)
    cairo_ਸੈੱਟ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# ਨਹੀਂਤਾਂ
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
    cairo_fill(cr);

    cairo_destroy(cr);

    ਕਤਾਰ_draw_area(rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
#ਨਹੀਂਤਾਂ
    GdkGCValues ਮੁੱਲਾਂ;
    GdkGC *invert_gc;

    ਜੇ (gui.drawarea->ਖਿੜਕੀ_2 == ਨਿਰਾ)
	ਵਾਪਸ;

    ਮੁੱਲਾਂ.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_ਮੁੱਲਾਂ(gui.drawarea->ਖਿੜਕੀ_2,
				       &ਮੁੱਲਾਂ,
				       GDK_GC_FUNCTION);
    gdk_gc_ਸੈੱਟ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_ਚੌੜਾਈ, (nr) * gui.char_ਉਚਾਈ);
    gdk_gc_destroy(invert_gc);
#ਜੇ_ਅੰਤ
}

/*
 * Iconify the GUI window.
 */
    ਖਾਲੀ
gui_mch_iconify(ਖਾਲੀ)
{
    gtk_ਖਿੜਕੀ_iconify(GTK_ਖਿੜਕੀ(gui.mainwin));
}

#ਜੇ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ਖਾਲੀ
gui_mch_ਸੈੱਟ_foreground(ਖਾਲੀ)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_keep_above(GTK_ਖਿੜਕੀ(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_ਖਿੜਕੀ_present(GTK_ਖਿੜਕੀ(gui.mainwin));
    gui_may_flush();
    gtk_ਖਿੜਕੀ_ਸੈੱਟ_keep_above(GTK_ਖਿੜਕੀ(gui.mainwin), FALSE);
    gui_may_flush();
}
#ਜੇ_ਅੰਤ

/*
 * Draw a cursor without focus.
 */
    ਖਾਲੀ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ਪੂਰਨ		i = 1;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_t    *cr;
#ਜੇ_ਅੰਤ

    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ;

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cr = cairo_create(gui.surface);
#ਜੇ_ਅੰਤ

    gui_mch_ਸੈੱਟ_fg_color(color);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#ਨਹੀਂਤਾਂ
    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
#ਜੇ_ਅੰਤ
    ਜੇ (mb_lefthalve(gui.ਪੰਕਤੀ, gui.ਖਾਨਾ))
	i = 2;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    cairo_ਸੈੱਟ_ਸਤਰ_ਚੌੜਾਈ(cr, 1.0);
    cairo_ਸੈੱਟ_ਸਤਰ_cap(cr, CAIRO_ਸਤਰ_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.ਖਾਨਾ) + 0.5, FILL_Y(gui.ਪੰਕਤੀ) + 0.5,
	    i * gui.char_ਚੌੜਾਈ - 1, gui.char_ਉਚਾਈ - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#ਨਹੀਂਤਾਂ
    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
	    FALSE,
	    FILL_X(gui.ਖਾਨਾ), FILL_Y(gui.ਪੰਕਤੀ),
	    i * gui.char_ਚੌੜਾਈ - 1, gui.char_ਉਚਾਈ - 1);
#ਜੇ_ਅੰਤ
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ਖਾਲੀ
gui_mch_draw_part_cursor(ਪੂਰਨ w, ਪੂਰਨ h, guicolor_T color)
{
    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ;

    gui_mch_ਸੈੱਟ_fg_color(color);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.ਖਾਨਾ + 1) - w :
# endif
	    FILL_X(gui.ਖਾਨਾ), FILL_Y(gui.ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ਸੱਜਾ ? FILL_X(gui.ਖਾਨਾ + 1) - w :
# endif
	    FILL_X(gui.ਖਾਨਾ),
	    FILL_Y(gui.ਪੰਕਤੀ) + gui.char_ਉਚਾਈ - h,
	    w, h);
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    ਖਾਲੀ
gui_mch_update(ਖਾਲੀ)
{
    ਪੂਰਨ cnt = 0;	// prevent endless loop
    ਜਦੋਂ (g_main_context_pending(ਨਿਰਾ) && !vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ()
	    && ++cnt < 100)
    {
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	ਪੂਰਨ prio = 0;
	g_main_context_prepare(ਨਿਰਾ, &prio);
	// peek internal scheduling of redraw
	ਜੇ (prio == GDK_ਤਰਜੀਹ_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#ਜੇ_ਅੰਤ
	g_main_context_iteration(ਨਿਰਾ, TRUE);
    }
}

    ਸਥਿਰ timeout_cb_type
ਨਿਵੇਸ਼_timer_cb(gpointer ਅੰਕੜੇ)
{
    ਪੂਰਨ *timed_ਬਾਹਰਲਾ = (ਪੂਰਨ *) ਅੰਕੜੇ;

    // Just inform the caller about the occurrence of it
    *timed_ਬਾਹਰਲਾ = TRUE;

    ਵਾਪਸ FALSE;		// don't happen again
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਸਥਿਰ timeout_cb_type
channel_poll_cb(gpointer ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    ਵਾਪਸ TRUE;		// repeat
}
#ਜੇ_ਅੰਤ

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ਪੂਰਨ
gui_mch_wait_for_chars(ਲੰਮਾ wtime)
{
    ਪੂਰਨ		focus;
    guint	timer;
    ਸਥਿਰ ਪੂਰਨ	timed_ਬਾਹਰਲਾ;
    ਪੂਰਨ		retval = FAIL;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    guint	channel_timer = 0;
#ਜੇ_ਅੰਤ

    timed_ਬਾਹਰਲਾ = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    ਜੇ (wtime >= 0)
	timer = timeout_ਜੋੜੋ(wtime == 0 ? 1L : wtime,
						   ਨਿਵੇਸ਼_timer_cb, &timed_ਬਾਹਰਲਾ);
    ਨਹੀਂਤਾਂ
	timer = 0;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ਜੇ (channel_any_keep_open())
	channel_timer = timeout_ਜੋੜੋ(20, channel_poll_cb, ਨਿਰਾ);
#ਜੇ_ਅੰਤ

    focus = gui.ਵਿੱਚ_focus;

    ਕਰੋ
    {
	// Stop or start blinking when focus changes
	ਜੇ (gui.ਵਿੱਚ_focus != focus)
	{
	    ਜੇ (gui.ਵਿੱਚ_focus)
		gui_mch_ਸ਼ੁਰੂ_blink();
	    ਨਹੀਂਤਾਂ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ਵਿੱਚ_focus;
	}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੁਨੇਹਾ_ਕਤਾਰ
# ifdef FEAT_TIMERS
	did_ਜੋੜੋ_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ਜੇ (did_ਜੋੜੋ_timer)
	    // Need to recompute the waiting time.
	    ਜਾਓ theend;
# endif
#ਜੇ_ਅੰਤ

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	ਜੇ (!ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    g_main_context_iteration(ਨਿਰਾ, TRUE);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ GDK_WINDOWING_WAYLAND
    ਜੇ (gui.is_wayland && clip_ਤਾਰਾ.state == SELECT_ਵਿੱਚ_ਤਰੱਕੀ)
	gui_may_flush();
#ਜੇ_ਅੰਤ
	}

	// Got char, return immediately
	ਜੇ (ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	{
	    retval = OK;
	    ਜਾਓ theend;
	}
    } ਜਦੋਂ (wtime < 0 || !timed_ਬਾਹਰਲਾ);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    ਜੇ (timer != 0 && !timed_ਬਾਹਰਲਾ)
	timeout_remove(timer);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ਕੰਮ_CHANNEL
    ਜੇ (channel_timer != 0)
	timeout_remove(channel_timer);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    ਖਾਲੀ
gui_mch_flush(ਖਾਲੀ)
{
    ਜੇ (gui.mainwin == ਨਿਰਾ || !gtk_widget_get_realized(gui.mainwin))
	ਵਾਪਸ;
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਜੇ (dirty_region != ਨਿਰਾ && gui.drawarea != ਨਿਰਾ)
    {
	gtk_widget_ਕਤਾਰ_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = ਨਿਰਾ;
    }
#ਨਹੀਂਤਾਂ
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       ਵਾਪਸ;
#ਜੇ_ਅੰਤ
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ਖਾਲੀ
gui_mch_clear_ਖੰਡ(ਪੂਰਨ row1arg, ਪੂਰਨ col1arg, ਪੂਰਨ row2arg, ਪੂਰਨ col2arg)
{

    ਪੂਰਨ col1 = check_ਖਾਨਾ(col1arg);
    ਪੂਰਨ col2 = check_ਖਾਨਾ(col2arg);
    ਪੂਰਨ row1 = check_ਪੰਕਤੀ(row1arg);
    ਪੂਰਨ row2 = check_ਪੰਕਤੀ(row2arg);

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ;
#ਨਹੀਂਤਾਂ
    GdkColor color;

    ਜੇ (gui.drawarea->ਖਿੜਕੀ_2 == ਨਿਰਾ)
	ਵਾਪਸ;

    color.pixel = gui.ਪਿੱਠ_pixel;
#ਜੇ_ਅੰਤ

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ਅਚਲ GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_ਚੌੜਾਈ + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ਉਚਾਈ
	};
	cairo_t * ਅਚਲ cr = cairo_create(gui.surface);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
	ਸੈੱਟ_cairo_ਸਰੋਤ_rgba_from_color(cr, gui.ਪਿੱਠ_pixel);
# ਨਹੀਂਤਾਂ
	Gdk_ਖਿੜਕੀ * ਅਚਲ win = gtk_widget_get_ਖਿੜਕੀ(gui.drawarea);
	cairo_ਨਮੂਨਾ_t * ਅਚਲ pat = gdk_ਖਿੜਕੀ_get_background_ਨਮੂਨਾ(win);
	ਜੇ (pat != ਨਿਰਾ)
	    cairo_ਸੈੱਟ_ਸਰੋਤ(cr, pat);
	ਨਹੀਂਤਾਂ
	    ਸੈੱਟ_cairo_ਸਰੋਤ_rgba_from_color(cr, gui.ਪਿੱਠ_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
	cairo_fill(cr);
	cairo_destroy(cr);

	ਕਤਾਰ_draw_area(rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
    }
#ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_ਚੌੜਾਈ
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_ਉਚਾਈ);
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
}

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
gui_gtk_ਖਿੜਕੀ_clear(Gdk_ਖਿੜਕੀ *win)
{
    ਅਚਲ GdkRectangle rect = {
	0, 0, gdk_ਖਿੜਕੀ_get_ਚੌੜਾਈ(win), gdk_ਖਿੜਕੀ_get_ਉਚਾਈ(win)
    };
    cairo_t * ਅਚਲ cr = cairo_create(gui.surface);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,22,2)
    ਸੈੱਟ_cairo_ਸਰੋਤ_rgba_from_color(cr, gui.ਪਿੱਠ_pixel);
# ਨਹੀਂਤਾਂ
    cairo_ਨਮੂਨਾ_t * ਅਚਲ pat = gdk_ਖਿੜਕੀ_get_background_ਨਮੂਨਾ(win);
    ਜੇ (pat != ਨਿਰਾ)
	cairo_ਸੈੱਟ_ਸਰੋਤ(cr, pat);
    ਨਹੀਂਤਾਂ
	ਸੈੱਟ_cairo_ਸਰੋਤ_rgba_from_color(cr, gui.ਪਿੱਠ_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
    cairo_fill(cr);
    cairo_destroy(cr);

    ਕਤਾਰ_draw_area(rect.x, rect.y, rect.ਚੌੜਾਈ, rect.ਉਚਾਈ);
}
#ਨਹੀਂਤਾਂ
# define gui_gtk_ਖਿੜਕੀ_clear(win)  gdk_ਖਿੜਕੀ_clear(win)
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
gui_mch_clear_all(ਖਾਲੀ)
{
    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) != ਨਿਰਾ)
	gui_gtk_ਖਿੜਕੀ_clear(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea));
}

#ਜੇ !GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ਸਥਿਰ ਖਾਲੀ
check_copy_area(ਖਾਲੀ)
{
    GdkEvent        *event;
    ਪੂਰਨ                expose_ਗਿਣਤੀ;
    ਜੇ (gui.visibility != GDK_VISIBILITY_PARTIAL)
	ਵਾਪਸ;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    ਕਰੋ
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->ਖਿੜਕੀ_2);
	ਜੇ (event == ਨਿਰਾ)
	    ਰੋਕੋ; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.ਚੌੜਾਈ, event->expose.area.ਉਚਾਈ);
	expose_ਗਿਣਤੀ = event->expose.ਗਿਣਤੀ_2;
	gdk_event_free(event);
    }
    ਜਦੋਂ (expose_ਗਿਣਤੀ > 0); // more events follow
    gui_can_update_cursor();
}
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)

#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਸਥਿਰ ਖਾਲੀ
gui_gtk_surface_copy_rect(ਪੂਰਨ dest_x, ਪੂਰਨ dest_y,
			  ਪੂਰਨ src_x,  ਪੂਰਨ src_y,
			  ਪੂਰਨ ਚੌੜਾਈ,  ਪੂਰਨ ਉਚਾਈ)
{
    cairo_t * ਅਚਲ cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ਸਥਿਰ cairo_surface_t *scroll_scratch = ਨਿਰਾ;
    ਸਥਿਰ ਪੂਰਨ scratch_w = 0, scratch_h = 0;
    ਪੂਰਨ ਆਖਰੀ_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1, ਆਖਰੀ_ਪੰਕਤੀ_y = ਆਖਰੀ_ਪੰਕਤੀ * gui.char_ਉਚਾਈ;
    bool ਆਖਰੀ_ਪੰਕਤੀ_overlap = (dest_y + ਉਚਾਈ) > ਆਖਰੀ_ਪੰਕਤੀ_y;
    ਜੇ (gui.is_wayland && ( !(State & ਬਹੁਲਕ_CMDLINE) || !ਆਖਰੀ_ਪੰਕਤੀ_overlap) )
    {
	ਜੇ (dest_y < src_y) // scroll up
	{
	    cairo_ਸੈੱਟ_operator(cr, CAIRO_OPERATOR_ਸਰੋਤ);
	    cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, ਚੌੜਾਈ, ਉਚਾਈ);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	ਨਹੀਂਤਾਂ
	{   //  reuse surface when scrolling, only realloc if larger
	    ਜੇ (scroll_scratch == ਨਿਰਾ || ਚੌੜਾਈ > scratch_w || ਉਚਾਈ > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), ਚੌੜਾਈ, ਉਚਾਈ);
		scratch_w = ਚੌੜਾਈ, scratch_h = ਉਚਾਈ;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_ਸੈੱਟ_ਸਰੋਤ_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_ਸੈੱਟ_operator(cr, CAIRO_OPERATOR_ਸਰੋਤ);
	    cairo_rectangle(cr, dest_x, dest_y, ਚੌੜਾਈ, ਉਚਾਈ);
	    cairo_clip(cr);
	    cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    ਨਹੀਂਤਾਂ
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, ਚੌੜਾਈ, ਉਚਾਈ);
	cairo_clip(cr);
	cairo_ਧੱਕੋ_ਸਮੂਹ(cr);
	cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_ਕੱਢੋ_ਸਮੂਹ_ਮੰਜ਼ਿਲ_ਸਰੋਤ(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#ਜੇ_ਅੰਤ

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ਖਾਲੀ
gui_mch_delete_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਅਚਲ ਪੂਰਨ ncols = gui.scroll_region_ਸੱਜਾ - gui.scroll_region_ਖੱਬਾ + 1;
    ਅਚਲ ਪੂਰਨ nrows = gui.scroll_region_bot - ਪੰਕਤੀ + 1;
    ਅਚਲ ਪੂਰਨ src_nrows = nrows - ਅੰਕ_ਸਤਰਾਂ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ),
	    gui.char_ਚੌੜਾਈ * ncols + 1,     gui.char_ਉਚਾਈ * src_nrows);
    gui_clear_ਖੰਡ(
	    gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1, gui.scroll_region_ਖੱਬਾ,
	    gui.scroll_region_bot,		   gui.scroll_region_ਸੱਜਾ);
    ਕਤਾਰ_draw_area(
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    gui.char_ਚੌੜਾਈ * ncols + 1,	gui.char_ਉਚਾਈ * nrows);
#ਨਹੀਂਤਾਂ
    ਜੇ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	ਵਾਪਸ;			// Can't see the window

    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
    gdk_gc_ਸੈੱਟ_background(gui.ਪਾਠ_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_ਖਿੜਕੀ_copy_area(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    gui.drawarea->ਖਿੜਕੀ_2,
	    FILL_X(gui.scroll_region_ਖੱਬਾ),
	    FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ),
	    gui.char_ਚੌੜਾਈ * (gui.scroll_region_ਸੱਜਾ
					    - gui.scroll_region_ਖੱਬਾ + 1) + 1,
	    gui.char_ਉਚਾਈ * (gui.scroll_region_bot - ਪੰਕਤੀ - ਅੰਕ_ਸਤਰਾਂ + 1));

    gui_clear_ਖੰਡ(gui.scroll_region_bot - ਅੰਕ_ਸਤਰਾਂ + 1,
						       gui.scroll_region_ਖੱਬਾ,
		    gui.scroll_region_bot, gui.scroll_region_ਸੱਜਾ);
    check_copy_area();
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ਖਾਲੀ
gui_mch_ਪਾਓ_ਸਤਰਾਂ(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਅੰਕ_ਸਤਰਾਂ)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਅਚਲ ਪੂਰਨ ncols = gui.scroll_region_ਸੱਜਾ - gui.scroll_region_ਖੱਬਾ + 1;
    ਅਚਲ ਪੂਰਨ nrows = gui.scroll_region_bot - ਪੰਕਤੀ + 1;
    ਅਚਲ ਪੂਰਨ src_nrows = nrows - ਅੰਕ_ਸਤਰਾਂ;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ),
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    gui.char_ਚੌੜਾਈ * ncols + 1,     gui.char_ਉਚਾਈ * src_nrows);
    gui_clear_ਖੰਡ(
	    ਪੰਕਤੀ,		 gui.scroll_region_ਖੱਬਾ,
	    ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);
    ਕਤਾਰ_draw_area(
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    gui.char_ਚੌੜਾਈ * ncols + 1,	gui.char_ਉਚਾਈ * nrows);
#ਨਹੀਂਤਾਂ
    ਜੇ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	ਵਾਪਸ;			// Can't see the window

    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.fgcolor);
    gdk_gc_ਸੈੱਟ_background(gui.ਪਾਠ_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_ਖਿੜਕੀ_copy_area(gui.drawarea->ਖਿੜਕੀ_2, gui.ਪਾਠ_gc,
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ),
	    gui.drawarea->ਖਿੜਕੀ_2,
	    FILL_X(gui.scroll_region_ਖੱਬਾ), FILL_Y(ਪੰਕਤੀ),
	    gui.char_ਚੌੜਾਈ * (gui.scroll_region_ਸੱਜਾ
					    - gui.scroll_region_ਖੱਬਾ + 1) + 1,
	    gui.char_ਉਚਾਈ * (gui.scroll_region_bot - ਪੰਕਤੀ - ਅੰਕ_ਸਤਰਾਂ + 1));

    gui_clear_ਖੰਡ(ਪੰਕਤੀ, gui.scroll_region_ਖੱਬਾ,
				ਪੰਕਤੀ + ਅੰਕ_ਸਤਰਾਂ - 1, gui.scroll_region_ਸੱਜਾ);
    check_copy_area();
#ਜੇ_ਅੰਤ // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    ਖਾਲੀ
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	ਨਿਸ਼ਾਨਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ	i;
    time_t	ਸ਼ੁਰੂ;

    ਲਈ (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	ਜੇ (!clip_html && selection_targets[i].info == ਨਿਸ਼ਾਨਾ_HTML)
	    ਜਾਰੀ;
	received_selection = RS_NONE;
	ਨਿਸ਼ਾਨਾ = gdk_atom_intern(selection_targets[i].ਨਿਸ਼ਾਨਾ, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, ਨਿਸ਼ਾਨਾ,
			      (guint32)GDK_CURRENT_ਸਮਾਂ);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	ਸ਼ੁਰੂ = ਸਮਾਂ(ਨਿਰਾ);
	ਜਦੋਂ (received_selection == RS_NONE && ਸਮਾਂ(ਨਿਰਾ) < ਸ਼ੁਰੂ + 3)
	    g_main_context_iteration(ਨਿਰਾ, TRUE);	// wait for selection_received_cb

	ਜੇ (received_selection != RS_FAIL)
	    ਵਾਪਸ;
    }

    ਜੇ (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_ਖਿੜਕੀ_XDISPLAY(gtk_widget_get_ਖਿੜਕੀ(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    ਖਾਲੀ
clip_mch_lose_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
    ਜੇ (ਵਿੱਚ_selection_clear_event)
	ਵਾਪਸ;

    gtk_selection_owner_ਸੈੱਟ(ਨਿਰਾ, cbd->gtk_sel_atom, gui.event_ਸਮਾਂ);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    ਪੂਰਨ
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    ਜੇ (is_autocmd_blocked())
	ਵਾਪਸ OK;

    ਪੂਰਨ success;

    success = gtk_selection_owner_ਸੈੱਟ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_ਸਮਾਂ);
    // don't update on every visual selection change
    ਜੇ (!(cbd->owned && VIsual_ਸਰਗਰਮ))
	gui_gtk_ਸੈੱਟ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    ਵਾਪਸ (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ਖਾਲੀ
clip_mch_ਸੈੱਟ_selection(Clipboard_T *cbd ਅਣਵਰਤਿਆ)
{
}

#ਜੇ defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    ਪੂਰਨ
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    ਵਾਪਸ gdk_selection_owner_get(cbd->gtk_sel_atom) != ਨਿਰਾ;
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    ਖਾਲੀ
gui_mch_menu_grey(vimmenu_T *menu, ਪੂਰਨ grey)
{
    ਜੇ (menu->id == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    ਜੇ (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_ਸੈੱਟ_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    ਖਾਲੀ
gui_mch_menu_hidden(vimmenu_T *menu, ਪੂਰਨ hidden)
{
    ਜੇ (menu->id == 0)
	ਵਾਪਸ;

    ਜੇ (hidden)
    {
	ਜੇ (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ਖਾਲੀ
gui_mch_draw_menubar(ਖਾਲੀ)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#ਜੇ_ਅੰਤ // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    ਖਾਲੀ
gui_mch_enable_scrollbar(scrollbar_T *sb, ਪੂਰਨ flag)
{
    ਜੇ (sb->id == ਨਿਰਾ)
	ਵਾਪਸ;

    gtk_widget_ਸੈੱਟ_visible(sb->id, flag);
    update_ਖਿੜਕੀ_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਵਾਪਸ (long_u)pixel;
#ਨਹੀਂਤਾਂ
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)pixel, &color);

    ਵਾਪਸ (guicolor_T)(
	    (((ਬਿਨਾਨਿਸ਼ਾਨੀ)color.red   & 0xff00) << 8)
	 |  ((ਬਿਨਾਨਿਸ਼ਾਨੀ)color.green & 0xff00)
	 | (((ਬਿਨਾਨਿਸ਼ਾਨੀ)color.blue  & 0xff00) >> 8));
#ਜੇ_ਅੰਤ
}

/*
 * Get current mouse coordinates in text window.
 */
    ਖਾਲੀ
gui_mch_getmouse(ਪੂਰਨ *x, ਪੂਰਨ *y)
{
    gui_gtk_get_ਸੂਚਕ(gui.drawarea, x, y, ਨਿਰਾ);
}

    ਖਾਲੀ
gui_mch_setmouse(ਪੂਰਨ x, ਪੂਰਨ y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    ਜੇ (dpy)
	XWarp_ਸੂਚਕ(dpy, (ਖਿੜਕੀ)0,
		GDK_ਖਿੜਕੀ_XID(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ਸਥਿਰ ਪੂਰਨ ਆਖਰੀ_shape = 0;
#ਜੇ_ਅੰਤ

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    ਖਾਲੀ
gui_mch_mousehide(ਪੂਰਨ hide)
{
    ਜੇ (gui.ਸੂਚਕ_hidden == hide)
	ਵਾਪਸ;

    gui.ਸੂਚਕ_hidden = hide;
    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) && gui.blank_ਸੂਚਕ != ਨਿਰਾ)
    {
	ਜੇ (hide)
	    gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea),
		    gui.blank_ਸੂਚਕ);
	ਨਹੀਂਤਾਂ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSESHAPE
	    mch_ਸੈੱਟ_mouse_shape(ਆਖਰੀ_shape);
#ਨਹੀਂਤਾਂ
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea), ਨਿਰਾ);
#ਜੇ_ਅੰਤ
    }
}

#ਜੇ defined(FEAT_MOUSESHAPE)

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ * mshape_css_ਨਾਂ[] =
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
# ਨਹੀਂਤਾਂ
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ਸਥਿਰ ਅਚਲ ਪੂਰਨ mshape_ids[] =
{
    GDK_ਖੱਬਾ_PTR,		// arrow
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
    GDK_ਸੱਜਾ_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_ਖੱਬਾ_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    ਖਾਲੀ
mch_ਸੈੱਟ_mouse_shape(ਪੂਰਨ shape)
{
    GdkCursor	   *c;
    ਪੂਰਨ		   id;                    // Only id or css_name is used.
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਅਚਲ ਅੱਖਰ     *css_name = "default";
    GdkDisplay     *disp;
# endif

    ਜੇ (gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (shape == MSHAPE_HIDE || gui.ਸੂਚਕ_hidden)
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea),
		gui.blank_ਸੂਚਕ);
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ਜੇ (id >= GDK_ਆਖਰੀ_CURSOR)
		id = GDK_ਖੱਬਾ_PTR;
	    ਨਹੀਂਤਾਂ
		id &= ~1;	// they are always even (why?)
	}
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	ਨਹੀਂਤਾਂ ਜੇ (shape < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(mshape_css_ਨਾਂ))
	    css_name = mshape_css_ਨਾਂ[shape];
# ਨਹੀਂਤਾਂ
	ਨਹੀਂਤਾਂ ਜੇ (shape < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(mshape_ids))
	    id = mshape_ids[shape];
# endif
	ਨਹੀਂਤਾਂ
	    ਵਾਪਸ;
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# ਨਹੀਂਤਾਂ
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_ਖਿੜਕੀ_ਸੈੱਟ_cursor(gtk_widget_get_ਖਿੜਕੀ(gui.drawarea), c);
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
	g_object_unref(G_OBJECT(c));
# ਨਹੀਂਤਾਂ
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    ਜੇ (shape != MSHAPE_HIDE)
	ਆਖਰੀ_shape = shape;
}
#ਜੇ_ਅੰਤ // FEAT_MOUSESHAPE


#ਜੇ defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_ਚੌੜਾਈ  (2 * gui.char_ਚੌੜਾਈ)
# define SIGN_ਉਚਾਈ (gui.char_ਉਚਾਈ)
# define SIGN_ASPECT ((ਦੁੱਗਣਾ)SIGN_ਉਚਾਈ / (ਦੁੱਗਣਾ)SIGN_ਚੌੜਾਈ)

    ਖਾਲੀ
gui_mch_drawsign(ਪੂਰਨ ਪੰਕਤੀ, ਪੂਰਨ ਖਾਨਾ, ਪੂਰਨ typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    ਜੇ (sign == ਨਿਰਾ || gui.drawarea == ਨਿਰਾ
	    || gtk_widget_get_ਖਿੜਕੀ(gui.drawarea) == ਨਿਰਾ)
	ਵਾਪਸ;

    ਪੂਰਨ ਚੌੜਾਈ;
    ਪੂਰਨ ਉਚਾਈ;
    ਪੂਰਨ xoffset;
    ਪੂਰਨ yoffset;
    ਪੂਰਨ need_scale;

    ਚੌੜਾਈ  = gdk_pixbuf_get_ਚੌੜਾਈ(sign);
    ਉਚਾਈ = gdk_pixbuf_get_ਉਚਾਈ(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (ਚੌੜਾਈ > SIGN_ਚੌੜਾਈ + 2
	    || ਉਚਾਈ != SIGN_ਉਚਾਈ
	    || (ਚੌੜਾਈ < 3 * SIGN_ਚੌੜਾਈ / 4
		&& ਉਚਾਈ < 3 * SIGN_ਉਚਾਈ / 4));
    ਜੇ (need_scale)
    {
	ਦੁੱਗਣਾ  aspect;
	ਪੂਰਨ	    w = ਚੌੜਾਈ;
	ਪੂਰਨ	    h = ਉਚਾਈ;

	// Keep the original aspect ratio
	aspect = (ਦੁੱਗਣਾ)ਉਚਾਈ / (ਦੁੱਗਣਾ)ਚੌੜਾਈ;
	ਚੌੜਾਈ  = (ਦੁੱਗਣਾ)SIGN_ਚੌੜਾਈ * SIGN_ASPECT / aspect;
	ਚੌੜਾਈ  = MIN(ਚੌੜਾਈ, SIGN_ਚੌੜਾਈ);
	ਜੇ (((ਦੁੱਗਣਾ)(MAX(ਉਚਾਈ, SIGN_ਉਚਾਈ)) /
		    (ਦੁੱਗਣਾ)(MIN(ਉਚਾਈ, SIGN_ਉਚਾਈ))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    ਉਚਾਈ = (ਦੁੱਗਣਾ)SIGN_ਉਚਾਈ * SIGN_ASPECT / aspect;
	    ਉਚਾਈ = MIN(ਉਚਾਈ, SIGN_ਉਚਾਈ);
	}
	ਨਹੀਂਤਾਂ
	    ਉਚਾਈ = (ਦੁੱਗਣਾ)ਚੌੜਾਈ * aspect;

	ਜੇ (w == ਚੌੜਾਈ && h == ਉਚਾਈ)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	ਨਹੀਂਤਾਂ
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, ਚੌੜਾਈ, ਉਚਾਈ,
		    GDK_INTERP_BILINEAR);
	    ਜੇ (sign == ਨਿਰਾ)
		ਵਾਪਸ; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (ਚੌੜਾਈ  - SIGN_ਚੌੜਾਈ)  / 2;
    yoffset = (ਉਚਾਈ - SIGN_ਉਚਾਈ) / 2;

# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ਚੌੜਾਈ, SIGN_ਉਚਾਈ);
	bg_cr = cairo_create(bg_surf);
	cairo_ਸੈੱਟ_ਸਰੋਤ_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ਚੌੜਾਈ, SIGN_ਉਚਾਈ);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_ਸੈੱਟ_ਸਰੋਤ_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_ਸੈੱਟ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_ਸੈੱਟ_ਸਰੋਤ_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, sign_surf, FILL_X(ਖਾਨਾ), FILL_Y(ਪੰਕਤੀ));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	ਕਤਾਰ_draw_area(
		FILL_X(ਖਾਨਾ), FILL_Y(ਖਾਨਾ), ਚੌੜਾਈ, ਉਚਾਈ);

    }
# ਨਹੀਂਤਾਂ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ਸੈੱਟ_foreground(gui.ਪਾਠ_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->ਖਿੜਕੀ_2,
	    gui.ਪਾਠ_gc,
	    TRUE,
	    FILL_X(ਖਾਨਾ),
	    FILL_Y(ਪੰਕਤੀ),
	    SIGN_ਚੌੜਾਈ,
	    SIGN_ਉਚਾਈ);

    gdk_pixbuf_render_ਮੰਜ਼ਿਲ_drawable_alpha(sign,
	    gui.drawarea->ਖਿੜਕੀ_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(ਖਾਨਾ) - MIN(0, xoffset),
	    FILL_Y(ਪੰਕਤੀ) - MIN(0, yoffset),
	    MIN(ਚੌੜਾਈ,	SIGN_ਚੌੜਾਈ),
	    MIN(ਉਚਾਈ, SIGN_ਉਚਾਈ),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_ਸਾਧਾਰਨ,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    ਜੇ (need_scale)
	g_object_unref(sign);
}

    ਖਾਲੀ *
gui_mch_ਰਜਿਸਟਰ_sign(char_u *signfile)
{
    ਜੇ (signfile[0] != NUL && signfile[0] != '-' && gui.ਵਿੱਚ_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = ਨਿਰਾ;
	char_u	    *ਸੁਨੇਹਾ_2;

	sign = gdk_pixbuf_new_from_file((ਅਚਲ ਅੱਖਰ *)signfile, &error);

	ਜੇ (error == ਨਿਰਾ)
	    ਵਾਪਸ sign;

	ਸੁਨੇਹਾ_2 = (char_u *)error->ਸੁਨੇਹਾ_2;

	ਜੇ (ਸੁਨੇਹਾ_2 != ਨਿਰਾ && ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
	    ਸੁਨੇਹਾ_2 = string_convert(&ਨਿਵੇਸ਼_conv, ਸੁਨੇਹਾ_2, ਨਿਰਾ);

	ਜੇ (ਸੁਨੇਹਾ_2 != ਨਿਰਾ)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", ਸੁਨੇਹਾ_2);

	    ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
		vim_free(ਸੁਨੇਹਾ_2);
	}
	g_error_free(error);
    }

    ਵਾਪਸ ਨਿਰਾ;
}

    ਖਾਲੀ
gui_mch_destroy_sign(ਖਾਲੀ *sign)
{
    ਜੇ (sign != ਨਿਰਾ)
	g_object_unref(sign);
}

#ਜੇ_ਅੰਤ // FEAT_SIGN_ICONS

#ਜੇ defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
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
    ਸਥਿਰ ਖਾਲੀ
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->ਅੰਕੜੇ;
    uint8_t *dst;
    ਪੂਰਨ	    w = img->ਚੌੜਾਈ;
    ਪੂਰਨ	    h = img->ਉਚਾਈ;
    ਪੂਰਨ	    stride;
    ਪੂਰਨ	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_ਅੰਕੜੇ(surf);
    stride = cairo_image_surface_get_stride(surf);

    ਜੇ (img->fmt == IMAGE_FORMAT_RGBA)
    {
	ਲਈ (y = 0; y < h; ++y)
	{
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *)(dst + y * stride);

	    ਲਈ (x = 0; x < w; ++x)
	    {
		ਪੂਰਨ	idx = (y * w + x) * 4;
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
    ਨਹੀਂਤਾਂ
    {
	ਲਈ (y = 0; y < h; ++y)
	{
	    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *p = (ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ *)(dst + y * stride);

	    ਲਈ (x = 0; x < w; ++x)
	    {
		ਪੂਰਨ	idx = (y * w + x) * 3;
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

    ਪੂਰਨ
image_gui_init(image_T *img)
{
    // We only support GTK3
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    ਜੇ (ctx == ਨਿਰਾ)
	ਵਾਪਸ FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->ਚੌੜਾਈ, img->ਉਚਾਈ);

    ਜੇ (cairo_surface_ਸਥਿਤੀ(ctx->surf) != CAIRO_ਸਥਿਤੀ_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	ਜਾਓ fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_ਅੰਕੜੇ = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    ਵਾਪਸ OK;
fail:
    vim_free(ctx);
    ਵਾਪਸ FAIL;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
# endif
}

    ਖਾਲੀ
image_gui_uninit(image_T *img)
{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    image_cairo_T *ctx = img->backend_ਅੰਕੜੇ;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    ਪੂਰਨ
image_placement_gui_init(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    ਵਾਪਸ OK;
# ਨਹੀਂਤਾਂ
    ਵਾਪਸ FAIL;
# endif
}

    ਖਾਲੀ
image_placement_gui_uninit(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
}

    ਖਾਲੀ
image_placement_gui_draw(image_placement_T *ਰੱਖੋ)
{
# ਜੇ GTK_CHECK_ਸੰਸਕਰਣ(3,0,0)
    image_T		    *img = ਰੱਖੋ->img;
    image_cairo_T	    *ictx = img->backend_ਅੰਕੜੇ;
    pixman_box32_t	    *rects;
    ਪੂਰਨ			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&ਰੱਖੋ->visible, &n_rects);
    ਜੇ (rects == ਨਿਰਾ || n_rects == 0)
	ਵਾਪਸ;

    cairo_surface_ਸੈੱਟ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    ਲਈ (ਪੂਰਨ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ਪੂਰਨ		ਪੰਕਤੀ, ਖਾਨਾ;
	ਪੂਰਨ		posx, posy;
	ਪੂਰਨ		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(ਰੱਖੋ, rect, &ਪੰਕਤੀ, &ਖਾਨਾ, &x, &y, &w, &h, false);
	posx = FILL_X(ਖਾਨਾ);
	posy = FILL_Y(ਪੰਕਤੀ);

	cairo_save(cr);
	cairo_ਸੈੱਟ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_ਸੈੱਟ_ਸਰੋਤ_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	ਕਤਾਰ_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    ਖਾਲੀ
image_placement_gui_clear(image_placement_T *ਰੱਖੋ ਅਣਵਰਤਿਆ)
{
}

#ਜੇ_ਅੰਤ // FEAT_IMAGE
