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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GRESOURCE
# include "auto/gui_gtk_gresources.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
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
# ಆದರೆ defined(FEAT_GETTEXT) && !defined(ENABLE_NLS)
#  define ENABLE_NLS	// so the texts in the dialog boxes are translated
# endif
# include <gnome.h>
# include "version.h"
// missing prototype in bonobo-dock-item.h
ಬಾಹ್ಯ ಶೂನ್ಯ bonobo_dock_ಅಂಶ_ಗಣ_behavior(Bonobo_Dock_ಅಂಶ *dock_ಅಂಶ, Bonobo_Dock_ಅಂಶ_Behavior beh);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK)
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
#  include <gdk/gdkkeysyms-compat.h>
#  ifdef GDK_WINDOWING_WAYLAND
#   include <gdk/gdkwayland.h>
#  endif
#  include <gtk/gtkx.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gdk/gdkkeysyms.h>
# endif
# include <gdk/gdk.h>
# ifdef MSWIN
#  include <gdk/gdkwin32.h>
# ಇಲ್ಲದಿದ್ದರೆ
#  include <gdk/gdkx.h>
# endif
# include <gtk/gtk.h>
# include "gui_gtk_f.h"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_X11_SUNKEYSYM_H
# include <X11/Sunkeysym.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Easy-to-use macro for multihead support.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು GET_X_ATOM(atom)	gdk_x11_atom_ಗಮ್ಯ_xatom_for_display( \
				    gtk_widget_get_display(gui.mainwin), atom)

// Selection type distinguishers
ಎಣಿಕೆ
{
    ಗುರಿ_TYPE_NONE,
    ಗುರಿ_UTF8_STRING,
    ಗುರಿ_STRING,
    ಗುರಿ_COMPOUND_ಪಠ್ಯ,
    ಗುರಿ_HTML,
    ಗುರಿ_ಪಠ್ಯ,
    ಗುರಿ_ಪಠ್ಯ_URI_LIST,
    ಗುರಿ_ಪಠ್ಯ_PLAIN,
    ಗುರಿ_ಪಠ್ಯ_PLAIN_UTF8,
    ಗುರಿ_VIM_MT,
    ಗುರಿ_VIM,
    ಗುರಿ_VIMENC_MT,
    ಗುರಿ_VIMENC
};

/*
 * Table of selection targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Gtk_ಗುರಿ_ನಮೂದು selection_targets[] =
{
    {VIMENC_ATOM_NAME,	0, ಗುರಿ_VIMENC},
    {VIM_ATOM_NAME,	0, ಗುರಿ_VIM},
    {VIMENC_MIMETYPE_NAME,  0, ಗುರಿ_VIMENC_MT},
    {VIM_MIMETYPE_NAME,	    0, ಗುರಿ_VIM_MT},
    {"text/html",	0, ಗುರಿ_HTML},
    {"UTF8_STRING",	0, ಗುರಿ_UTF8_STRING},
    {"COMPOUND_TEXT",	0, ಗುರಿ_COMPOUND_ಪಠ್ಯ},
    {"TEXT",		0, ಗುರಿ_ಪಠ್ಯ},
    {"STRING",		0, ಗುರಿ_STRING},
    {"text/plain;charset=utf-8", 0, ಗುರಿ_ಪಠ್ಯ_PLAIN_UTF8},
    {"text/plain",	0, ಗುರಿ_ಪಠ್ಯ_PLAIN}
};
#ವ್ಯಾಖ್ಯಾನಿಸು N_SELECTION_TARGETS ARRAY_ಉದ್ದಳತೆ(selection_targets)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
/*
 * Table of DnD targets supported by Vim.
 * Note: Order matters, preferred types should come first.
 */
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ Gtk_ಗುರಿ_ನಮೂದು dnd_targets[] =
{
    {"text/uri-list",	0, ಗುರಿ_ಪಠ್ಯ_URI_LIST},
    {"text/html",	0, ಗುರಿ_HTML},
    {"UTF8_STRING",	0, ಗುರಿ_UTF8_STRING},
    {"STRING",		0, ಗುರಿ_STRING},
    {"text/plain",	0, ಗುರಿ_ಪಠ್ಯ_PLAIN}
};
# define N_DND_TARGETS ARRAY_ಉದ್ದಳತೆ(dnd_targets)
#ಆದರೆ_ಅಂತ್ಯ


/*
 * "Monospace" is a standard font alias that should be present
 * on all proper Pango/fontconfig installations.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು DEFAULT_FONT	"Monospace 12"

#ಆದರೆ defined(FEAT_GUI_GNOME) && defined(FEAT_SESSION)
# define USE_GNOME_SESSION
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(FEAT_GUI_GNOME)
/*
 * Atoms used to communicate save-yourself from the X11 session manager. There
 * is no need to move them into the GUI struct, since they should be constant.
 */
ಸ್ಥಿರ GdkAtom wm_protocols_atom = GDK_NONE;
ಸ್ಥಿರ GdkAtom save_yourself_atom = GDK_NONE;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Atoms used to control/reference X11 selections.
 */
ಸ್ಥಿರ GdkAtom html_atom = GDK_NONE;
ಸ್ಥಿರ GdkAtom utf8_string_atom = GDK_NONE;
ಸ್ಥಿರ GdkAtom vim_atom = GDK_NONE;	// Vim's own special selection format
ಸ್ಥಿರ GdkAtom vimenc_atom = GDK_NONE;	// Vim's extended selection format
ಸ್ಥಿರ GdkAtom vim_mt_atom = GDK_NONE;	// Vim's own special selection format
					// (in mime type format)
ಸ್ಥಿರ GdkAtom vimenc_mt_atom = GDK_NONE;	// Vim's extended selection
						// format (in mime type format)

/*
 * Keycodes recognized by vim.
 * NOTE: when changing this, the table in gui_x11.c probably needs the same
 * change!
 */
ಸ್ಥಿರ ರಚನೆ special_ಕೀಲಿ
{
    guint ಕೀಲಿ_sym;
    char_u code0;
    char_u code1;
}
ಸ್ಥಿರಾಂಕ special_ಕೀಲಿಗಳು[] =
{
    {GDK_ಮೇಲೆ,		'k', 'u'},
    {GDK_ಕೆಳಕ್ಕೆ,		'k', 'd'},
    {GDK_ಎಡ,		'k', 'l'},
    {GDK_ಬಲ,		'k', 'r'},
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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SunXK_F36
    {SunXK_F36,		'F', 'Q'},
    {SunXK_F37,		'F', 'R'},
#ಆದರೆ_ಅಂತ್ಯ
    {GDK_Help,		'%', '1'},
    {GDK_Undo,		'&', '8'},
    {GDK_ಹಿಂಭಾಗ_Space,	'k', 'b'},
    {GDK_ಸೇರಿಸು,	'k', 'I'},
    {GDK_Delete,	'k', 'D'},
    {GDK_3270_ಹಿಂಭಾಗ_Tab,	'k', 'B'},
    {GDK_Clear,		'k', 'C'},
    {GDK_Home,		'k', 'h'},
    {GDK_ಕೊನೆ,		'@', '7'},
    {GDK_ಪೂರ್ವ,		'k', 'P'},
    {GDK_ಮುಂದಿನ,		'k', 'N'},
    {GDK_Print,		'%', '9'},
    // Keypad keys:
    {GDK_KP_ಎಡ,	'k', 'l'},
    {GDK_KP_ಬಲ,	'k', 'r'},
    {GDK_KP_ಮೇಲೆ,		'k', 'u'},
    {GDK_KP_ಕೆಳಕ್ಕೆ,	'k', 'd'},
    {GDK_KP_ಸೇರಿಸು,	KS_EXTRA, (char_u)KE_KINS},
    {GDK_KP_Delete,	KS_EXTRA, (char_u)KE_KDEL},
    {GDK_KP_Home,	'K', '1'},
    {GDK_KP_ಕೊನೆ,	'K', '4'},
    {GDK_KP_ಪೂರ್ವ,	'K', '3'},  // page up
    {GDK_KP_ಮುಂದಿನ,	'K', '5'},  // page down

    {GDK_KP_ಕೂಡಿಸು,	'K', '6'},
    {GDK_KP_ಕಳೆ,	'K', '7'},
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
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_FONT	1
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_GEOMETRY	2
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_ಹಿಮ್ಮುಖ	3
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_NOREVERSE	4
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_BACKGROUND	5
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_FOREGROUND	6
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_ICONIC	7
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_ROLE	8
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_NETBEANS	9
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_XRM		10	// ignored
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_MENUFONT	11	// ignored
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ	0x00ff
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_HAS_ಮೌಲ್ಯ	0x0100	// a value is expected after the argument
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_NEEDS_GUI	0x0200	// need to initialize the GUI for this
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_FOR_GTK	0x0400	// argument is handled by GTK+ or GNOME
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_COMPAT_LONG	0x0800	// accept -foo but substitute with --foo
#ವ್ಯಾಖ್ಯಾನಿಸು ARG_KEEP	0x1000	// don't remove argument from argv[]

/*
 * This table holds all the X GUI command line options allowed.  This includes
 * the standard ones so that we can skip them when Vim is started without the
 * GUI (but the GUI might start up later).
 *
 * When changing this, also update doc/gui_x11.txt and the usage message!!!
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	    *name;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    flags;
}
cmdline_option_T;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ cmdline_option_T cmdline_options[] =
{
    // We handle these options ourselves
    {"-fn",		ARG_FONT|ARG_HAS_ಮೌಲ್ಯ},
    {"-font",		ARG_FONT|ARG_HAS_ಮೌಲ್ಯ},
    {"-geom",		ARG_GEOMETRY|ARG_HAS_ಮೌಲ್ಯ},
    {"-geometry",	ARG_GEOMETRY|ARG_HAS_ಮೌಲ್ಯ},
    {"-rv",		ARG_ಹಿಮ್ಮುಖ},
    {"-reverse",	ARG_ಹಿಮ್ಮುಖ},
    {"+rv",		ARG_NOREVERSE},
    {"+reverse",	ARG_NOREVERSE},
    {"-bg",		ARG_BACKGROUND|ARG_HAS_ಮೌಲ್ಯ},
    {"-background",	ARG_BACKGROUND|ARG_HAS_ಮೌಲ್ಯ},
    {"-fg",		ARG_FOREGROUND|ARG_HAS_ಮೌಲ್ಯ},
    {"-foreground",	ARG_FOREGROUND|ARG_HAS_ಮೌಲ್ಯ},
    {"-iconic",		ARG_ICONIC},
    {"--role",		ARG_ROLE|ARG_HAS_ಮೌಲ್ಯ},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    {"-nb",		ARG_NETBEANS},	      // non-standard value format
    {"-xrm",		ARG_XRM|ARG_HAS_ಮೌಲ್ಯ},		// not implemented
    {"-mf",		ARG_MENUFONT|ARG_HAS_ಮೌಲ್ಯ},	// not implemented
    {"-menufont",	ARG_MENUFONT|ARG_HAS_ಮೌಲ್ಯ},	// not implemented
#ಆದರೆ_ಅಂತ್ಯ
    // Arguments handled by GTK (and GNOME) internally.
    {"--g-fatal-warnings",	ARG_FOR_GTK},
    {"--gdk-debug",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gdk-no-debug",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gtk-debug",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gtk-no-debug",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gtk-module",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--sync",			ARG_FOR_GTK},
    {"--display",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ|ARG_COMPAT_LONG},
    {"--name",			ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ|ARG_COMPAT_LONG},
    {"--class",			ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ|ARG_COMPAT_LONG},
    {"--screen",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gxid-host",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--gxid-port",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
    {"--load-modules",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--sm-client-id",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--sm-config-prefix",	ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--sm-disable",		ARG_FOR_GTK},
    {"--oaf-ior-fd",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--oaf-activate-iid",	ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"--oaf-private",		ARG_FOR_GTK},
    {"--enable-sound",		ARG_FOR_GTK},
    {"--disable-sound",		ARG_FOR_GTK},
    {"--espeaker",		ARG_FOR_GTK|ARG_HAS_ಮೌಲ್ಯ},
    {"-?",			ARG_FOR_GTK|ARG_NEEDS_GUI},
    {"--help",			ARG_FOR_GTK|ARG_NEEDS_GUI|ARG_KEEP},
    {"--usage",			ARG_FOR_GTK|ARG_NEEDS_GUI},
# ಆದರೆ 0 // conflicts with Vim's own --version argument
    {"--version",		ARG_FOR_GTK|ARG_NEEDS_GUI},
# endif
    {"--disable-crash-dialog",	ARG_FOR_GTK},
#ಆದರೆ_ಅಂತ್ಯ
    {ಖಾಲಿ, 0}
};

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ    gui_argc = 0;
ಸ್ಥಿರ ಅಕ್ಷರ **gui_argv = ಖಾಲಿ;

ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *role_ವಾದ = ಖಾಲಿ;
#ಆದರೆ defined(USE_GNOME_SESSION)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *restart_command = ಖಾಲಿ;
ಸ್ಥಿರ       ಅಕ್ಷರ *abs_restart_command = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಸಿಕ್ಕಿತು_iconic_arg = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
/*
 * Can't use Gnome if --socketid given
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ using_gnome = 0;
#ಇಲ್ಲದಿದ್ದರೆ
# define using_gnome 0
#ಆದರೆ_ಅಂತ್ಯ

// Comment out the following line to ignore code for resize history tracking.
#ವ್ಯಾಖ್ಯಾನಿಸು TRACK_RESIZE_HISTORY
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_RESIZE_HISTORY
/*
 * Keep a short term resize history so that stale gtk responses can be
 * discarded.
 * When a gtk_window_resize() request is sent to gtk, the width/height of
 * the request is saved. Recent stale requests are kept around in a list.
 * See https://github.com/vim/vim/issues/10123
 */
# ಆದರೆ 0  // Change to 1 to enable ch_log() calls for debugging.
#  ifdef FEAT_EVAL
#   define ENABLE_RESIZE_HISTORY_LOG
#  endif
# endif

/*
 * History item of a resize request.
 * Width and height are of gui.mainwin.
 */
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ resize_history {
    ಪೂರ್ಣಾಂಕ ಬಳಸಿದ;		// If true, can't match for discard. Only matches once.
    ಪೂರ್ಣಾಂಕ ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ from_shellsize;	// TRUE if recorded by gui_mch_set_shellsize (not
			// a pre-record from gui_mch_show_tabline).
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ಪೂರ್ಣಾಂಕ seq;		// for ch_log messages
# endif
    ರಚನೆ resize_history *ಮುಂದಿನ;
} resize_hist_T;

// never NULL during execution
ಸ್ಥಿರ resize_hist_T *latest_resize_hist;
// list of stale resize requests
ಸ್ಥಿರ resize_hist_T *old_resize_hists;

// On Wayland (and GTK3 with CSD), gtk_window_resize(w, h) results in
// gtk_window_get_size() returning (w - csd_w, h - csd_h).  These offsets are
// computed once from the first confirmed resize response and applied to all
// subsequent gtk_window_resize() calls so the form gets the intended size.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mch_csd_ಅಗಲ = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mch_csd_ಎತ್ತರ = 0;

// Expected form widget width for the most recent gui_mch_set_shellsize()
// call (set only after mch_csd_width is known).  Used in
// form_configure_event() to clamp slightly-narrow configure responses that
// result from CSD under-compensation, preventing column loss.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mch_pending_form_w = 0;

/*
 * Used when calling gtk_window_resize().
 * Create a resize request history item, put previous request on stale list.
 * Width/height are the size of the request for the gui.mainwin.
 * from_shellsize is TRUE when called from gui_mch_set_shellsize (vs a
 * stale pre-record from gui_mch_show_tabline).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
alloc_resize_hist(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ, ಪೂರ್ಣಾಂಕ from_shellsize)
{
    // alloc a new resize hist, save current in list of old history
    resize_hist_T *prev_hist = latest_resize_hist;
    resize_hist_T *new_hist = ALLOC_CLEAR_ONE(resize_hist_T);

    new_hist->ಅಗಲ = ಅಗಲ;
    new_hist->ಎತ್ತರ = ಎತ್ತರ;
    new_hist->from_shellsize = from_shellsize;
    latest_resize_hist = new_hist;

    // previous hist item becomes head of list
    prev_hist->ಮುಂದಿನ = old_resize_hists;
    old_resize_hists = prev_hist;

# ifdef ENABLE_RESIZE_HISTORY_LOG
    new_hist->seq = prev_hist->seq + 1;
    ch_log(ಖಾಲಿ, "gui_gtk: New resize seq %d (%d, %d) [%d, %d]",
	   new_hist->seq, ಅಗಲ, ಎತ್ತರ, (ಪೂರ್ಣಾಂಕ)Columns, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು);
# endif
}

/*
 * Free everything on the stale resize history list.
 * This list is empty when there are no outstanding resize requests.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clear_resize_hists(ಶೂನ್ಯ)
{
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ಪೂರ್ಣಾಂಕ		    i = 0;
# endif

    ಆದರೆ (latest_resize_hist)
	latest_resize_hist->ಬಳಸಿದ = TRUE;
    ಆಗ (old_resize_hists != ಖಾಲಿ)
    {
	resize_hist_T *ಮುಂದಿನ_hist = old_resize_hists->ಮುಂದಿನ;

	vim_free(old_resize_hists);
	old_resize_hists = ಮುಂದಿನ_hist;
# ifdef ENABLE_RESIZE_HISTORY_LOG
	i++;
# endif
    }
# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ಖಾಲಿ, "gui_gtk: free %d hists", i);
# endif
}

// true if hist item is unused and matches w,h
# define ಹೊಂದಿಕೆ_ಅಗಲ_ಎತ್ತರ(hist, w, h) \
	  (!hist->ಬಳಸಿದ && hist->ಅಗಲ == w && hist->ಎತ್ತರ == h)

/*
 * Search the resize hist list.
 * Return true if the specified width,height match an item in the list that
 * has never matched before. Mark the matching item as used so it will
 * not match again.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಹೊಂದಿಕೆ_stale_ಅಗಲ_ಎತ್ತರ(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    resize_hist_T *hist = old_resize_hists;

    ಗಾಗಿ (hist = old_resize_hists; hist != ಖಾಲಿ; hist = hist->ಮುಂದಿನ)
	ಆದರೆ (ಹೊಂದಿಕೆ_ಅಗಲ_ಎತ್ತರ(hist, ಅಗಲ, ಎತ್ತರ))
	{
# ifdef ENABLE_RESIZE_HISTORY_LOG
	    ch_log(ಖಾಲಿ, "gui_gtk: discard seq %d, cur seq %d",
		   hist->seq, latest_resize_hist->seq);
# endif
	    hist->ಬಳಸಿದ = TRUE;
	    ಹಿಂತಿರುಗಿ TRUE;
	}
    ಹಿಂತಿರುಗಿ FALSE;
}

# ಆದರೆ defined(EXITFREE)
    ಸ್ಥಿರ ಶೂನ್ಯ
free_all_resize_hist(ಶೂನ್ಯ)
{
    clear_resize_hists();
    vim_free(latest_resize_hist);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GTK doesn't set the GDK_BUTTON1_MASK state when dragging a touch. Add this
 * state when dragging.
 */
ಸ್ಥಿರ guint dragging_button_state = 0;

/*
 * Parse the GUI related command-line arguments.  Any arguments used are
 * deleted from argv, and *argc is decremented accordingly.  This is called
 * when vim is started, whether or not the GUI has been started.
 */
    ಶೂನ್ಯ
gui_mch_prepare(ಪೂರ್ಣಾಂಕ *argc, ಅಕ್ಷರ **argv)
{
    ಸ್ಥಿರಾಂಕ cmdline_option_T  *option;
    ಪೂರ್ಣಾಂಕ			    i	= 0;
    ಪೂರ್ಣಾಂಕ			    len = 0;

#ಆದರೆ defined(USE_GNOME_SESSION)
    /*
     * Determine the command used to invoke Vim, to be passed as restart
     * command to the session manager.	If argv[0] contains any directory
     * components try building an absolute path, otherwise leave it as is.
     */
    restart_command = argv[0];

    ಆದರೆ (strchr(argv[0], G_DIR_SEPARATOR) != ಖಾಲಿ)
    {
	char_u buf[MAXPATHL];

	ಆದರೆ (mch_ತುಂಬಿದ_Name((char_u *)argv[0], buf, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(buf), TRUE) == OK)
	{
	    abs_restart_command = (ಅಕ್ಷರ *)vim_strsave(buf);
	    restart_command = abs_restart_command;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Move all the entries in argv which are relevant to GTK+ and GNOME
     * into gui_argv.  Freed later in gui_mch_init().
     */
    gui_argc = 0;
    gui_argv = ALLOC_MULT(ಅಕ್ಷರ *, *argc + 1);

    g_return_if_fail(gui_argv != ಖಾಲಿ);

    gui_argv[gui_argc++] = argv[i++];

    ಆಗ (i < *argc)
    {
	// Don't waste CPU cycles on non-option arguments.
	ಆದರೆ (argv[i][0] != '-' && argv[i][0] != '+')
	{
	    ++i;
	    ಮುಂದುವರಿ;
	}

	// Look for argv[i] in cmdline_options[] table.
	ಗಾಗಿ (option = &cmdline_options[0]; option->name != ಖಾಲಿ; ++option)
	{
	    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(option->name);

	    ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹೋಲಿಕೆ(argv[i], option->name, len) == 0)
	    {
		ಆದರೆ (argv[i][len] == '\0')
		    ನಿಲ್ಲಿಸು;
		// allow --foo=bar style
		ಆದರೆ (argv[i][len] == '=' && (option->flags & ARG_HAS_ಮೌಲ್ಯ))
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
		// darn, -nb has non-standard syntax
		ಆದರೆ (vim_strchr((char_u *)":=", argv[i][len]) != ಖಾಲಿ
			&& (option->flags & ARG_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ) == ARG_NETBEANS)
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((option->flags & ARG_COMPAT_LONG)
			&& strcmp(argv[i], option->name + 1) == 0)
	    {
		// Replace the standard X arguments "-name" and "-display"
		// with their GNU-style long option counterparts.
		argv[i] = (ಅಕ್ಷರ *)option->name;
		ನಿಲ್ಲಿಸು;
	    }
	}
	ಆದರೆ (option->name == ಖಾಲಿ) // no match
	{
	    ++i;
	    ಮುಂದುವರಿ;
	}

	ಆದರೆ (option->flags & ARG_FOR_GTK)
	{
	    // Move the argument into gui_argv, which
	    // will later be passed to gtk_init_check()
	    gui_argv[gui_argc++] = argv[i];
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಅಕ್ಷರ *ಮೌಲ್ಯ = ಖಾಲಿ;

	    // Extract the option's value if there is one.
	    // Accept both "--foo bar" and "--foo=bar" style.
	    ಆದರೆ (option->flags & ARG_HAS_ಮೌಲ್ಯ)
	    {
		ಆದರೆ (argv[i][len] == '=')
		    ಮೌಲ್ಯ = &argv[i][len + 1];
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i + 1 < *argc && strcmp(argv[i + 1], "--") != 0)
		    ಮೌಲ್ಯ = argv[i + 1];
	    }

	    // Check for options handled by Vim itself
	    ಆಯ್ಕೆ (option->flags & ARG_ಸೂಚ್ಯಂಕ_ಮುಖವಾಡ)
	    {
		ಸ್ಥಿತಿ ARG_ಹಿಮ್ಮುಖ:
		    ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg = TRUE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_NOREVERSE:
		    ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg = FALSE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_FONT:
		    font_ವಾದ = ಮೌಲ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_GEOMETRY:
		    ಆದರೆ (ಮೌಲ್ಯ != ಖಾಲಿ)
			gui.geom = vim_strsave((char_u *)ಮೌಲ್ಯ);
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_BACKGROUND:
		    background_ವಾದ = ಮೌಲ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_FOREGROUND:
		    foreground_ವಾದ = ಮೌಲ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_ICONIC:
		    ಸಿಕ್ಕಿತು_iconic_arg = TRUE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ ARG_ROLE:
		    role_ವಾದ = ಮೌಲ್ಯ; // used later in gui_mch_open()
		    ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
		ಸ್ಥಿತಿ ARG_NETBEANS:
		    gui.dofork = false; // don't fork() when starting GUI
		    netbeansArg = argv[i];
		    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ನಿಲ್ಲಿಸು;
	    }
	}

	// These arguments make gnome_program_init() print a message and exit.
	// Must start the GUI for this, otherwise ":gui" will exit later!
	// Only when the GUI can start.
	ಆದರೆ ((option->flags & ARG_NEEDS_GUI)
				      && gui_mch_early_init_check(FALSE) == OK)
	    gui.starting = true;

	ಆದರೆ (option->flags & ARG_KEEP)
	    ++i;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // Remove the flag from the argument vector.
	    ಆದರೆ (--*argc > i)
	    {
		ಪೂರ್ಣಾಂಕ n_strip = 1;

		// Move the argument's value as well, if there is one.
		ಆದರೆ ((option->flags & ARG_HAS_ಮೌಲ್ಯ)
			&& argv[i][len] != '='
			&& strcmp(argv[i + 1], "--") != 0)
		{
		    ++n_strip;
		    --*argc;
		    ಆದರೆ (option->flags & ARG_FOR_GTK)
			gui_argv[gui_argc++] = argv[i + 1];
		}

		ಆದರೆ (*argc > i)
		    mch_memmove(&argv[i], &argv[i + n_strip],
						(*argc - i) * ಗಾತ್ರ(ಅಕ್ಷರ *));
	    }
	    argv[*argc] = ಖಾಲಿ;
	}
    }

    gui_argv[gui_argc] = ಖಾಲಿ;
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
gui_mch_free_all(ಶೂನ್ಯ)
{
    vim_free(gui_argv);
# ಆದರೆ defined(USE_GNOME_SESSION)
    vim_free(abs_restart_command);
# endif
# ifdef TRACK_RESIZE_HISTORY
    free_all_resize_hist();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
/*
 * This should be maybe completely removed.
 * Doesn't seem possible, since check_copy_area() relies on
 * this information.  --danielk
 */
    ಸ್ಥಿರ gint
visibility_event(GtkWidget *widget ಬಳಸದ,
		 GdkEventVisibility *event,
		 gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui.visibility = event->state;
    /*
     * When we do an gdk_window_copy_area(), and the window is partially
     * obscured, we want to receive an event to tell us whether it worked
     * or not.
     */
    ಆದರೆ (gui.ಪಠ್ಯ_gc != ಖಾಲಿ)
	gdk_gc_ಗಣ_exposures(gui.ಪಠ್ಯ_gc,
			     gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

/*
 * Redraw the corresponding portions of the screen.
 */
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ gboolean
draw_event(GtkWidget *widget ಬಳಸದ,
	   cairo_t   *cr,
	   gpointer   user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ಆದರೆ (gui.starting)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹೊರಹೋಗು_flush();		// make sure all output has been processed
				// for GTK+ 3, may induce other draw events.

    cairo_ಗಣ_ಮೂಲ_surface(cr, gui.surface, 0, 0);
    cairo_paint(cr);

    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * On Wayland an alpha-less surface avoids needless alpha compositing; on X11
 * the ARGB (Render) path is the accelerated one, so keep the alpha there.
 */
# ifdef GDK_WINDOWING_WAYLAND
#  define GUI_GTK_SURFACE_CONTENT \
	(gui.is_wayland ? CAIRO_CONTENT_COLOR : CAIRO_CONTENT_COLOR_ALPHA)
# ಇಲ್ಲದಿದ್ದರೆ
#  define GUI_GTK_SURFACE_CONTENT CAIRO_CONTENT_COLOR_ALPHA
# endif

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    ಸ್ಥಿರ gboolean
scale_ಗುಣಕ_event(GtkWidget *widget,
		   GParamSpec* pspec ಬಳಸದ,
		   gpointer   user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (gui.surface != ಖಾಲಿ)
	cairo_surface_destroy(gui.surface);

    ಪೂರ್ಣಾಂಕ	    w, h;
    gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin), &w, &h);
    gui.surface = gdk_ಕಿಟಕಿ_create_similar_surface(
	    gtk_widget_get_ಕಿಟಕಿ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    w, h);

    ಪೂರ್ಣಾಂಕ	    usable_ಎತ್ತರ = h;
    ಆದರೆ (gtk_socket_id != 0)
	usable_ಎತ್ತರ -= (gui.char_ಎತ್ತರ - (gui.char_ಎತ್ತರ/2)); // sic.

    gui_gtk_form_freeze(GTK_FORM(gui.formwin));
    gui.force_redraw = 1;
    gui_resize_shell(w, usable_ಎತ್ತರ);
    gui_gtk_form_thaw(GTK_FORM(gui.formwin));

#  ಆದರೆ defined(FEAT_IMAGE) && GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    gui.scale = gtk_widget_get_scale_ಗುಣಕ(widget);
#  endif

    ಹಿಂತಿರುಗಿ TRUE;
}
# endif // GTK_CHECK_VERSION(3,10,0)

#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
    ಸ್ಥಿರ gint
expose_event(GtkWidget *widget ಬಳಸದ,
	     GdkEventExpose *event,
	     gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Skip this when the GUI isn't set up yet, will redraw later.
    ಆದರೆ (gui.starting)
	ಹಿಂತಿರುಗಿ FALSE;

    ಹೊರಹೋಗು_flush();		// make sure all output has been processed
    gui_redraw(event->area.x, event->area.y,
	       event->area.ಅಗಲ, event->area.ಎತ್ತರ);

    // Clear the border areas if needed
    ಆದರೆ (event->area.x < FILL_X(0))
	gdk_ಕಿಟಕಿ_clear_area(gui.drawarea->ಕಿಟಕಿ_2, 0, 0, FILL_X(0), 0);
    ಆದರೆ (event->area.y < FILL_Y(0))
	gdk_ಕಿಟಕಿ_clear_area(gui.drawarea->ಕಿಟಕಿ_2, 0, 0, 0, FILL_Y(0));
    ಆದರೆ (event->area.x > FILL_X(Columns))
	gdk_ಕಿಟಕಿ_clear_area(gui.drawarea->ಕಿಟಕಿ_2,
			      FILL_X((ಪೂರ್ಣಾಂಕ)Columns), 0, 0, 0);
    ಆದರೆ (event->area.y > FILL_Y(ಅಡ್ಡಸಾಲುಗಳು))
	gdk_ಕಿಟಕಿ_clear_area(gui.drawarea->ಕಿಟಕಿ_2, 0, FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು), 0, 0);

    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
/*
 * Handle changes to the "Comm" property
 */
    ಸ್ಥಿರ gint
property_event(GtkWidget *widget,
	       GdkEventProperty *event,
	       gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (event->type == GDK_PROPERTY_NOTIFY
	    && event->state == (ಪೂರ್ಣಾಂಕ)GDK_PROPERTY_NEW_ಮೌಲ್ಯ
	    && GDK_ಕಿಟಕಿ_XID(event->ಕಿಟಕಿ_2) == comm_ಕಿಟಕಿ
	    && GET_X_ATOM(event->atom) == commProperty)
    {
	XEvent xev;

	// Translate to XLib
	xev.xproperty.type = PropertyNotify;
	xev.xproperty.atom = commProperty;
	xev.xproperty.ಕಿಟಕಿ_2 = comm_ಕಿಟಕಿ;
	xev.xproperty.state = Property_New_ಮೌಲ್ಯ;
	serverEventProc(GDK_ಕಿಟಕಿ_XDISPLAY(gtk_widget_get_ಕಿಟಕಿ(widget)),
								      &xev, 0);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}
#ಆದರೆ_ಅಂತ್ಯ // defined(FEAT_CLIENTSERVER)

/*
 * Handle changes to the "Xft/DPI" setting
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gtk_settings_xft_dpi_ಬದಲಾಯಿತು_cb(GtkSettings *gtk_settings ಬಳಸದ,
				    GParamSpec *pspec ಬಳಸದ,
				    gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Create a new PangoContext for this screen, and initialize it
    // with the current font if necessary.
    ಆದರೆ (gui.ಪಠ್ಯ_context != ಖಾಲಿ)
	g_object_unref(gui.ಪಠ್ಯ_context);

    gui.ಪಠ್ಯ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ಗಣ_ತಳ_dir(gui.ಪಠ್ಯ_context, PANGO_DIRECTION_LTR);

    ಆದರೆ (gui.norm_font != ಖಾಲಿ)
    {
	// force default font
	gui_mch_init_font(*p_guifont == NUL ? ಖಾಲಿ : p_guifont, FALSE);
	gui_ಗಣ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ gboolean timeout_cb_type;

/*
 * Start a timer that will invoke the specified callback.
 * Returns the ID of the timer.
 */
    ಸ್ಥಿರ guint
timeout_ಕೂಡಿಸು(ಪೂರ್ಣಾಂಕ ಸಮಯ, timeout_cb_type (*callback)(gpointer), ಪೂರ್ಣಾಂಕ *flagp)
{
    ಹಿಂತಿರುಗಿ g_timeout_ಕೂಡಿಸು((guint)ಸಮಯ, (GSourceFunc)callback, flagp);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
timeout_remove(guint timer)
{
    g_ಮೂಲ_remove(timer);
}


/////////////////////////////////////////////////////////////////////////////
// Focus handlers:


/*
 * This is a simple state machine:
 * BLINK_NONE	not blinking at all
 * BLINK_OFF	blinking, cursor is not shown
 * BLINK_ON	blinking, cursor is shown
 */

#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_NONE  0
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_OFF   1
#ವ್ಯಾಖ್ಯಾನಿಸು BLINK_ON    2

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ blink_state = BLINK_NONE;
ಸ್ಥಿರ long_u blink_waittime = 700;
ಸ್ಥಿರ long_u blink_ontime = 400;
ಸ್ಥಿರ long_u blink_offtime = 250;
ಸ್ಥಿರ guint blink_timer = 0;

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blinking(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state != BLINK_NONE;
}

    ಪೂರ್ಣಾಂಕ
gui_mch_is_blink_off(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ blink_state == BLINK_OFF;
}

    ಶೂನ್ಯ
gui_mch_ಗಣ_blinking(ದೀರ್ಘ waittime, ದೀರ್ಘ on, ದೀರ್ಘ off)
{
    blink_waittime = waittime;
    blink_ontime = on;
    blink_offtime = off;
}

/*
 * Stop the cursor blinking.  Show the cursor if it wasn't shown.
 */
    ಶೂನ್ಯ
gui_mch_stop_blink(ಪೂರ್ಣಾಂಕ may_call_gui_update_cursor)
{
    ಆದರೆ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    ಆದರೆ (blink_state == BLINK_OFF && may_call_gui_update_cursor)
    {
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
    blink_state = BLINK_NONE;
}

    ಸ್ಥಿರ timeout_cb_type
blink_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (blink_state == BLINK_ON)
    {
	gui_undraw_cursor();
	blink_state = BLINK_OFF;
	blink_timer = timeout_ಕೂಡಿಸು(blink_offtime, blink_cb, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui_update_cursor(TRUE, FALSE);
	blink_state = BLINK_ON;
	blink_timer = timeout_ಕೂಡಿಸು(blink_ontime, blink_cb, ಖಾಲಿ);
    }
    gui_may_flush();

    ಹಿಂತಿರುಗಿ FALSE;		// don't happen again
}

/*
 * Start the cursor blinking.  If it was already blinking, this restarts the
 * waiting time and shows the cursor.
 */
    ಶೂನ್ಯ
gui_mch_ಆರಂಭ_blink(ಶೂನ್ಯ)
{
    ಆದರೆ (blink_timer)
    {
	timeout_remove(blink_timer);
	blink_timer = 0;
    }
    // Only switch blinking on if none of the times is zero
    ಆದರೆ (blink_waittime && blink_ontime && blink_offtime && gui.ಒಳ_focus)
    {
	blink_timer = timeout_ಕೂಡಿಸು(blink_waittime, blink_cb, ಖಾಲಿ);
	blink_state = BLINK_ON;
	gui_update_cursor(TRUE, FALSE);
	gui_may_flush();
    }
}

    ಸ್ಥಿರ gint
enter_notify_event(GtkWidget *widget ಬಳಸದ,
		   Gdk_Event_ದಾಟುವಿಕೆ *event ಬಳಸದ,
		   gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (blink_state == BLINK_NONE)
	gui_mch_ಆರಂಭ_blink();

    // make sure keyboard input goes there
    ಆದರೆ (gtk_socket_id == 0 || !gtk_widget_has_focus(gui.drawarea))
	gtk_widget_grab_focus(gui.drawarea);

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ gint
leave_notify_event(GtkWidget *widget ಬಳಸದ,
		   Gdk_Event_ದಾಟುವಿಕೆ *event ಬಳಸದ,
		   gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ gint
focus_ಒಳ_event(GtkWidget *widget,
	       GdkEventFocus *event ಬಳಸದ,
	       gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DIALOG
    ಆದರೆ (gui.dialog_focus_pending > 0)
    {
	--gui.dialog_focus_pending;
	++hold_gui_events;
	gui_focus_change(TRUE);
	--hold_gui_events;
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	gui_focus_change(TRUE);

    ಆದರೆ (blink_state == BLINK_NONE)
	gui_mch_ಆರಂಭ_blink();

    // make sure keyboard input goes to the draw area (if this is focus for a
    // window)
    ಆದರೆ (widget != gui.drawarea)
	gtk_widget_grab_focus(gui.drawarea);

    ಹಿಂತಿರುಗಿ TRUE;
}

    ಸ್ಥಿರ gint
focus_ಹೊರಹೋಗು_event(GtkWidget *widget ಬಳಸದ,
		GdkEventFocus *event ಬಳಸದ,
		gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_DIALOG
    ಆದರೆ (gui.dialogs_ಸಕ್ರಿಯ > 0)
	++gui.dialog_focus_pending;
#ಆದರೆ_ಅಂತ್ಯ

    gui_focus_change(FALSE);

    ಆದರೆ (blink_state != BLINK_NONE)
	gui_mch_stop_blink(TRUE);

    ಹಿಂತಿರುಗಿ TRUE;
}


/*
 * Translate a GDK key value to UTF-8 independently of the current locale.
 * The output is written to string, which must have room for at least 6 bytes
 * plus the NUL terminator.  Returns the length in bytes.
 *
 * event->string is evil; see here why:
 * https://www.manpagez.com/html/gdk2/gdk2-2.24.24/gdk2-Event-Structures.php#GdkEventKey
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
keyval_ಗಮ್ಯ_string(ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ keyval, char_u *string)
{
    ಪೂರ್ಣಾಂಕ	    len;
    guint32 uc;

    uc = gdk_keyval_ಗಮ್ಯ_unicode(keyval);
    ಆದರೆ (uc != 0)
    {
	// Translate a normal key to UTF-8.  This doesn't work for dead
	// keys of course, you _have_ to use an input method for that.
	len = utf_char2bytes((ಪೂರ್ಣಾಂಕ)uc, string);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Translate keys which are represented by ASCII control codes in Vim.
	// There are only a few of those; most control keys are translated to
	// special terminal-like control sequences.
	len = 1;
	ಆಯ್ಕೆ (keyval)
	{
	    ಸ್ಥಿತಿ GDK_Tab: ಸ್ಥಿತಿ GDK_KP_Tab: ಸ್ಥಿತಿ GDK_ISO_ಎಡ_Tab:
		string[0] = TAB;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_Linefeed:
		string[0] = NL;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_Return: ಸ್ಥಿತಿ GDK_ISO_Enter: ಸ್ಥಿತಿ GDK_3270_Enter:
		string[0] = CAR;
		ನಿಲ್ಲಿಸು;
	    ಸ್ಥಿತಿ GDK_Escape:
		string[0] = ESC;
		ನಿಲ್ಲಿಸು;
	    ಪೂರ್ವನಿಯೋಜಿತ:
		len = 0;
		ನಿಲ್ಲಿಸು;
	}
    }
    string[len] = NUL;

    ಹಿಂತಿರುಗಿ len;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
modifiers_gdk2vim(guint state)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (state & GDK_ಸರಿಸು_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ಸರಿಸು;
    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_CTRL;
    ಆದರೆ (state & GDK_MOD1_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_ALT;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,10,0)
    ಆದರೆ (state & GDK_META_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_META;
    ಆದರೆ (state & GDK_SUPER_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_CMD;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (state & GDK_MOD4_ಮುಖವಾಡ)
	modifiers |= MOD_ಮುಖವಾಡ_CMD;
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ modifiers;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
modifiers_gdk2mouse(guint state)
{
    ಪೂರ್ಣಾಂಕ modifiers = 0;

    ಆದರೆ (state & GDK_ಸರಿಸು_ಮುಖವಾಡ)
	modifiers |= MOUSE_ಸರಿಸು;
    ಆದರೆ (state & GDK_CONTROL_ಮುಖವಾಡ)
	modifiers |= MOUSE_CTRL;
    ಆದರೆ (state & GDK_MOD1_ಮುಖವಾಡ)
	modifiers |= MOUSE_ALT;

    ಹಿಂತಿರುಗಿ modifiers;
}

/*
 * Main keyboard handler:
 */
    ಸ್ಥಿರ gint
ಕೀಲಿ_press_event(GtkWidget *widget ಬಳಸದ,
		Gdk_Event_ಕೀಲಿ *event,
		gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // For GTK+ 2 we know for sure how large the string might get.
    // (That is, up to 6 bytes + NUL + CSI escapes + safety measure.)
    char_u	string[32], string2[32];
    guint	ಕೀಲಿ_sym;
    ಪೂರ್ಣಾಂಕ		len;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		modifiers;
    ಪೂರ್ಣಾಂಕ		ಕೀಲಿ;
    guint	state;
    char_u	*s, *d;
    ಪೂರ್ಣಾಂಕ		ctrl_ಪೂರ್ವಪ್ರತ್ಯಯ_added = 0;

    gui.event_ಸಮಯ = event->ಸಮಯ;
    ಕೀಲಿ_sym = event->keyval;
    state = event->state;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    ಆದರೆ (xim_ಸರತಿ_ಕೀಲಿ_press_event(event, TRUE))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SunXK_F36
    /*
     * These keys have bogus lookup strings, and trapping them here is
     * easier than trying to XRebindKeysym() on them with every possible
     * combination of modifiers.
     */
    ಆದರೆ (ಕೀಲಿ_sym == SunXK_F36 || ಕೀಲಿ_sym == SunXK_F37)
	len = 0;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	len = keyval_ಗಮ್ಯ_string(ಕೀಲಿ_sym, string2);

	// Careful: convert_input() doesn't handle the NUL character.
	// No need to convert pure ASCII anyway, thus the len > 1 check.
	ಆದರೆ (len > 1 && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    len = convert_ಒಳಹರಿವು(string2, len, ಗಾತ್ರ(string2));

	s = string2;
	d = string;
	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    *d++ = s[i];
	    ಆದರೆ (d[-1] == CSI && d + 2 < string + ಗಾತ್ರ(string))
	    {
		// Turn CSI into K_CSI.
		*d++ = KS_EXTRA;
		*d++ = (ಪೂರ್ಣಾಂಕ)KE_CSI;
	    }
	}
	len = d - string;
    }

    // Shift-Tab results in Left_Tab, but we want <S-Tab>
    ಆದರೆ (ಕೀಲಿ_sym == GDK_ISO_ಎಡ_Tab)
    {
	ಕೀಲಿ_sym = GDK_Tab;
	state |= GDK_ಸರಿಸು_ಮುಖವಾಡ;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    // If there is a menu and 'wak' is "yes", or 'wak' is "menu" and the key
    // is a menu shortcut, we ignore everything with the ALT modifier.
    ಆದರೆ ((state & GDK_MOD1_ಮುಖವಾಡ)
	    && gui.menu_is_ಸಕ್ರಿಯ
	    && (*p_wak == 'y'
		|| (*p_wak == 'm'
		    && len == 1
		    && gui_is_menu_shortcut(string[0]))))
	// For GTK2 we return false to signify that we haven't handled the
	// keypress, so that gtk will handle the mnemonic or accelerator.
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ

    // We used to apply Alt/Meta to the key here (Mod1Mask), but that is now
    // done later, the same as it happens for the terminal.  Hopefully that
    // works for everybody...

    // Check for special keys.	Also do this when len == 1 (key has an ASCII
    // value) to detect backspace, delete and keypad keys.
    ಆದರೆ (len == 0 || len == 1)
    {
	ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	{
	    ಆದರೆ (special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym == ಕೀಲಿ_sym)
	    {
		string[0] = CSI;
		string[1] = special_ಕೀಲಿಗಳು[i].code0;
		string[2] = special_ಕೀಲಿಗಳು[i].code1;
		len = -3;
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GDK_ಕೀಲಿ_dead_circumflex
    // Belgian Ctrl+[ workaround
    ಆದರೆ (len == 0 && ಕೀಲಿ_sym == GDK_ಕೀಲಿ_dead_circumflex)
    {
	string[0] = CSI;
	string[1] = KS_MODIFIER;
	string[2] = MOD_ಮುಖವಾಡ_CTRL;
	string[3] = '[';
	len = 4;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);
	// workaround has to return here, otherwise our fake string[] entries
	// are confusing code downstream
	ಹಿಂತಿರುಗಿ TRUE;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (len == 0)   // Unrecognized key
	ಹಿಂತಿರುಗಿ TRUE;

    // For some keys a shift modifier is translated into another key code.
    ಆದರೆ (len == -3)
	ಕೀಲಿ = ಗಮ್ಯ_SPECIAL(string[1], string[2]);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	string[len] = NUL;
	ಕೀಲಿ = mb_ptr2char(string);
    }

    // Handle modifiers.
    modifiers = modifiers_gdk2vim(state);

    // Recognize special keys.
    ಕೀಲಿ = simplify_ಕೀಲಿ(ಕೀಲಿ, &modifiers);
    ಆದರೆ (ಕೀಲಿ == CSI)
	ಕೀಲಿ = K_CSI;
    ಆದರೆ (IS_SPECIAL(ಕೀಲಿ))
    {
	string[0] = CSI;
	string[1] = K_ಎರಡನೇ(ಕೀಲಿ);
	string[2] = K_ಮೂರನೇ(ಕೀಲಿ);
	len = 3;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Some keys need adjustment when the Ctrl modifier is used.
	ಕೀಲಿ = may_adjust_ಕೀಲಿ_for_ctrl(modifiers, ಕೀಲಿ);

	// May remove the Shift modifier if it's included in the key.
	modifiers = may_remove_ಸರಿಸು_modifier(modifiers, ಕೀಲಿ);

	len = mb_char2bytes(ಕೀಲಿ, string);
    }

    ಆದರೆ (modifiers != 0)
    {
	string2[0] = CSI;
	string2[1] = KS_MODIFIER;
	string2[2] = modifiers;
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string2, 3);
	ಆದರೆ (modifiers == 0x4)
	    ctrl_ಪೂರ್ವಪ್ರತ್ಯಯ_added = 1;
    }

    // Check if the key interrupts.
    {
	ಪೂರ್ಣಾಂಕ int_ch = check_for_ಅಡಚಣೆ(ಕೀಲಿ, modifiers);

	ಆದರೆ (int_ch != NUL)
	{
	    trash_ಒಳಹರಿವು_buf();
	    string[0] = int_ch;
	    len = 1;
	}
    }

    // workaround for German keyboard, where instead of '[' char we have code
    // sequence of bytes 195, 188 (UTF-8 for "u-umlaut")
    ಆದರೆ (ctrl_ಪೂರ್ವಪ್ರತ್ಯಯ_added && len == 2
		    && ((ಪೂರ್ಣಾಂಕ)string[0]) == 195
		    && ((ಪೂರ್ಣಾಂಕ)string[1]) == 188)
    {
	string[0] = 91; // ASCII('[')
	len = 1;
    }
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, len);

    // blank out the pointer if necessary
    ಆದರೆ (p_mh)
	gui_mch_mousehide(TRUE);

    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_XIM) || GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ gboolean
ಕೀಲಿ_release_event(GtkWidget *widget ಬಳಸದ,
		  Gdk_Event_ಕೀಲಿ *event ಬಳಸದ,
		  gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
# ಆದರೆ defined(FEAT_XIM)
    gui.event_ಸಮಯ = event->ಸಮಯ;
    /*
     * GTK+ 2 input methods may do fancy stuff on key release events too.
     * With the default IM for instance, you can enter any UCS code point
     * by holding down CTRL-SHIFT and typing hexadecimal digits.
     */
    ಹಿಂತಿರುಗಿ xim_ಸರತಿ_ಕೀಲಿ_press_event(event, FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ TRUE;
# endif
}
#ಆದರೆ_ಅಂತ್ಯ


/////////////////////////////////////////////////////////////////////////////
// Selection handlers:

// Remember when clip_lose_selection was called from here, we must not call
// gtk_selection_owner_set() then.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಒಳ_selection_clear_event = FALSE;

    ಸ್ಥಿರ gint
selection_clear_event(GtkWidget		*widget ಬಳಸದ,
		      GdkEventSelection	*event,
		      gpointer		user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಒಳ_selection_clear_event = TRUE;
    ಆದರೆ (event->selection == clip_plus.gtk_sel_atom)
	clip_lose_selection(&clip_plus);
    ಇಲ್ಲದಿದ್ದರೆ
	clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
    ಒಳ_selection_clear_event = FALSE;

    ಹಿಂತಿರುಗಿ TRUE;
}

#ವ್ಯಾಖ್ಯಾನಿಸು RS_NONE	0	// selection_received_cb() not called yet
#ವ್ಯಾಖ್ಯಾನಿಸು RS_OK	1	// selection_received_cb() called and OK
#ವ್ಯಾಖ್ಯಾನಿಸು RS_FAIL	2	// selection_received_cb() called and failed
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ received_selection = RS_NONE;

    ಸ್ಥಿರ ಶೂನ್ಯ
selection_received_cb(GtkWidget		*widget ಬಳಸದ,
		      Gtk_Selection_ದತ್ತಾಂಶ	*ದತ್ತಾಂಶ,
		      guint		ಸಮಯ_2 ಬಳಸದ,
		      gpointer		user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    Clipboard_T	    *cbd;
    char_u	    *ಪಠ್ಯ;
    char_u	    *tmpbuf = ಖಾಲಿ;
    guchar	    *tmpbuf_utf8 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    len;
    ಪೂರ್ಣಾಂಕ		    motion_type = MAUTO;

    ಆದರೆ (gtk_selection_ದತ್ತಾಂಶ_get_selection(ದತ್ತಾಂಶ) == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ಇಲ್ಲದಿದ್ದರೆ
	cbd = &clip_ನಕ್ಷತ್ರ;

    ಪಠ್ಯ = (char_u *)gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ);
    len = gtk_selection_ದತ್ತಾಂಶ_get_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);

    ಆದರೆ (ಪಠ್ಯ == ಖಾಲಿ || len <= 0)
    {
	received_selection = RS_FAIL;
	// clip_free_selection(cbd); ???

	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(ದತ್ತಾಂಶ) == vim_atom
	    || gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(ದತ್ತಾಂಶ) == vim_mt_atom)
    {
	motion_type = *ಪಠ್ಯ++;
	--len;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(ದತ್ತಾಂಶ) == vimenc_atom
	    || gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(ದತ್ತಾಂಶ) == vimenc_mt_atom)
    {
	char_u		*enc;
	vimconv_T	conv;

	motion_type = *ಪಠ್ಯ++;
	--len;

	enc = ಪಠ್ಯ;
	ಪಠ್ಯ += STRLEN(ಪಠ್ಯ) + 1;
	len -= ಪಠ್ಯ - enc;

	// If the encoding of the text is different from 'encoding', attempt
	// converting it.
	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ(&conv, enc, p_enc);
	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, ಪಠ್ಯ, &len);
	    ಆದರೆ (tmpbuf != ಖಾಲಿ)
		ಪಠ್ಯ = tmpbuf;
	    convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	}
    }

    // gtk_selection_data_get_text() handles all the nasty details
    // and targets and encodings etc.  This rocks so hard.
    ಇಲ್ಲದಿದ್ದರೆ
    {
	tmpbuf_utf8 = gtk_selection_ದತ್ತಾಂಶ_get_ಪಠ್ಯ(ದತ್ತಾಂಶ);
	ಆದರೆ (tmpbuf_utf8 != ಖಾಲಿ)
	{
	    len = STRLEN(tmpbuf_utf8);
	    ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    {
		tmpbuf = string_convert(&ಒಳಹರಿವು_conv, tmpbuf_utf8, &len);
		ಆದರೆ (tmpbuf != ಖಾಲಿ)
		    ಪಠ್ಯ = tmpbuf;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಪಠ್ಯ = tmpbuf_utf8;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len >= 2 && ಪಠ್ಯ[0] == 0xff && ಪಠ್ಯ[1] == 0xfe)
	{
	    vimconv_T conv;

	    // UTF-16, we get this for HTML
	    conv.vc_type = CONV_NONE;
	    convert_ಸಿದ್ಧತೆ_ext(&conv, (char_u *)"utf-16le", FALSE, p_enc, TRUE);

	    ಆದರೆ (conv.vc_type != CONV_NONE)
	    {
		ಪಠ್ಯ += 2;
		len -= 2;
		tmpbuf = string_convert(&conv, ಪಠ್ಯ, &len);
		convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	    }
	    ಆದರೆ (tmpbuf != ಖಾಲಿ)
		ಪಠ್ಯ = tmpbuf;
	}
    }

    // Chop off any trailing NUL bytes.  OpenOffice sends these.
    ಆಗ (len > 0 && ಪಠ್ಯ[len - 1] == NUL)
	--len;

    clip_yank_selection(motion_type, ಪಠ್ಯ, (ದೀರ್ಘ)len, cbd);
    received_selection = RS_OK;
    vim_free(tmpbuf);
    g_free(tmpbuf_utf8);
}

/*
 * Prepare our selection data for passing it to the external selection
 * client.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
selection_get_cb(GtkWidget	    *widget ಬಳಸದ,
		 Gtk_Selection_ದತ್ತಾಂಶ   *selection_ದತ್ತಾಂಶ,
		 guint		    info,
		 guint		    ಸಮಯ_2 ಬಳಸದ,
		 gpointer	    user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    char_u	    *string;
    char_u	    *tmpbuf;
    long_u	    tmplen;
    ಪೂರ್ಣಾಂಕ		    ಉದ್ದಳತೆ;
    ಪೂರ್ಣಾಂಕ		    motion_type;
    GdkAtom	    type;
    Clipboard_T    *cbd;

    ಆದರೆ (gtk_selection_ದತ್ತಾಂಶ_get_selection(selection_ದತ್ತಾಂಶ)
	    == clip_plus.gtk_sel_atom)
	cbd = &clip_plus;
    ಇಲ್ಲದಿದ್ದರೆ
	cbd = &clip_ನಕ್ಷತ್ರ;

    ಆದರೆ (!cbd->owned)
	ಹಿಂತಿರುಗಿ;			// Shouldn't ever happen

    ಆದರೆ (info != (guint)ಗುರಿ_STRING
	    && (!clip_html || info != (guint)ಗುರಿ_HTML)
	    && info != (guint)ಗುರಿ_UTF8_STRING
	    && info != (guint)ಗುರಿ_VIMENC
	    && info != (guint)ಗುರಿ_VIM
	    && info != (guint)ಗುರಿ_VIMENC_MT
	    && info != (guint)ಗುರಿ_VIM_MT
	    && info != (guint)ಗುರಿ_COMPOUND_ಪಠ್ಯ
	    && info != (guint)ಗುರಿ_ಪಠ್ಯ_PLAIN
	    && info != (guint)ಗುರಿ_ಪಠ್ಯ_PLAIN_UTF8
	    && info != (guint)ಗುರಿ_ಪಠ್ಯ)
	ಹಿಂತಿರುಗಿ;

    // get the selection from the '*'/'+' register
    clip_get_selection(cbd);

    motion_type = clip_convert_selection(&string, &tmplen, cbd);
    ಆದರೆ (motion_type < 0 || string == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    // Due to int arguments we can't handle more than G_MAXINT.  Also
    // reserve one extra byte for NUL or the motion type; just in case.
    // (Not that pasting 2G of text is ever going to work, but... ;-)
    ಉದ್ದಳತೆ = MIN(tmplen, (long_u)(G_MAXINT - 1));

    ಆದರೆ (info == (guint)ಗುರಿ_VIM || info == (guint)ಗುರಿ_VIM_MT)
    {
	tmpbuf = alloc(ಉದ್ದಳತೆ + 1);
	ಆದರೆ (tmpbuf != ಖಾಲಿ)
	{
	    tmpbuf[0] = motion_type;
	    mch_memmove(tmpbuf + 1, string, (size_t)ಉದ್ದಳತೆ);
	}
	// For our own format, the first byte contains the motion type
	++ಉದ್ದಳತೆ;
	vim_free(string);
	string = tmpbuf;
	ಆದರೆ (info == (guint)ಗುರಿ_VIM)
	    type = vim_atom;
	ಇಲ್ಲದಿದ್ದರೆ
	    type = vim_mt_atom;
    }

    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (info == (guint)ಗುರಿ_HTML)
    {
	vimconv_T conv;

	// Since we get utf-16, we probably should set it as well.
	conv.vc_type = CONV_NONE;
	convert_ಸಿದ್ಧತೆ_ext(&conv, p_enc, TRUE, (char_u *)"utf-16le", FALSE);
	ಆದರೆ (conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&conv, string, &ಉದ್ದಳತೆ);
	    convert_ಸಿದ್ಧತೆ(&conv, ಖಾಲಿ, ಖಾಲಿ);
	    vim_free(string);
	    string = tmpbuf;
	}

	// Prepend the BOM: "fffe"
	ಆದರೆ (string != ಖಾಲಿ)
	{
	    tmpbuf = alloc(ಉದ್ದಳತೆ + 2);
	    ಆದರೆ (tmpbuf != ಖಾಲಿ)
	    {
		tmpbuf[0] = 0xff;
		tmpbuf[1] = 0xfe;
		mch_memmove(tmpbuf + 2, string, (size_t)ಉದ್ದಳತೆ);
		vim_free(string);
		string = tmpbuf;
		ಉದ್ದಳತೆ += 2;
	    }

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    // Looks redundant even for GTK2 because these values are
	    // overwritten by gtk_selection_data_set() that follows.
	    selection_ದತ್ತಾಂಶ->type = selection_ದತ್ತಾಂಶ->ಗುರಿ;
	    selection_ದತ್ತಾಂಶ->format = 16;	// 16 bits per char
#ಆದರೆ_ಅಂತ್ಯ
	    gtk_selection_ದತ್ತಾಂಶ_ಗಣ(selection_ದತ್ತಾಂಶ, html_atom, 16,
							      string, ಉದ್ದಳತೆ);
	    vim_free(string);
	}
	ಹಿಂತಿರುಗಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (info == (guint)ಗುರಿ_VIMENC || info == (guint)ಗುರಿ_VIMENC_MT)
    {
	ಪೂರ್ಣಾಂಕ l = STRLEN(p_enc);

	// contents: motion_type 'encoding' NUL text
	tmpbuf = alloc(ಉದ್ದಳತೆ + l + 2);
	ಆದರೆ (tmpbuf != ಖಾಲಿ)
	{
	    tmpbuf[0] = motion_type;
	    STRCPY(tmpbuf + 1, p_enc);
	    mch_memmove(tmpbuf + l + 2, string, (size_t)ಉದ್ದಳತೆ);
	    ಉದ್ದಳತೆ += l + 2;
	    vim_free(string);
	    string = tmpbuf;
	}
	ಆದರೆ (info == (guint)ಗುರಿ_VIMENC)
	    type = vimenc_atom;
	ಇಲ್ಲದಿದ್ದರೆ
	    type = vimenc_mt_atom;
    }

    // gtk_selection_data_set_text() handles everything for us.  This is
    // so easy and simple and cool, it'd be insane not to use it.
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	{
	    tmpbuf = string_convert(&output_conv, string, &ಉದ್ದಳತೆ);
	    vim_free(string);
	    ಆದರೆ (tmpbuf == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ;
	    string = tmpbuf;
	}
	// Validate the string to avoid runtime warnings
	ಆದರೆ (g_utf8_validate((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)string, (gssize)ಉದ್ದಳತೆ, ಖಾಲಿ))
	{
	    gtk_selection_ದತ್ತಾಂಶ_ಗಣ_ಪಠ್ಯ(selection_ದತ್ತಾಂಶ,
					(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)string, ಉದ್ದಳತೆ);
	}
	vim_free(string);
	ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (string != ಖಾಲಿ)
    {
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	// Looks redundant even for GTK2 because these values are
	// overwritten by gtk_selection_data_set() that follows.
	selection_ದತ್ತಾಂಶ->type = selection_ದತ್ತಾಂಶ->ಗುರಿ;
	selection_ದತ್ತಾಂಶ->format = 8;	// 8 bits per char
#ಆದರೆ_ಅಂತ್ಯ
	gtk_selection_ದತ್ತಾಂಶ_ಗಣ(selection_ದತ್ತಾಂಶ, type, 8, string, ಉದ್ದಳತೆ);
	vim_free(string);
    }
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced and
 * before fork().
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_early_init_check(ಪೂರ್ಣಾಂಕ give_ಸಂದೇಶ)
{
    char_u *p, *q;

    // Guess that when $DISPLAY isn't set the GUI can't start.
    p = mch_getenv((char_u *)"DISPLAY");
    q = mch_getenv((char_u *)"WAYLAND_DISPLAY");
    ಆದರೆ ((p == ಖಾಲಿ || *p == NUL) && (q == ಖಾಲಿ || *q == NUL))
    {
	gui.dying = true;
	ಆದರೆ (give_ಸಂದೇಶ)
	    emsg(_((ಅಕ್ಷರ *)e_cannot_open_display));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Check if the GUI can be started.  Called before gvimrc is sourced but after
 * fork().
 * Return OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init_check(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GRESOURCE
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ res_registered = FALSE;

    ಆದರೆ (!res_registered)
    {
	// Call this function in the GUI process; otherwise, the resources
	// won't be available.  Don't call it twice.
	res_registered = TRUE;
	gui_gtk_ರಿಜಿಸ್ಟರ್_resource();
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
    ಆದರೆ (gtk_socket_id == 0)
	using_gnome = 1;
#ಆದರೆ_ಅಂತ್ಯ

    // This defaults to argv[0], but we want it to match the name of the
    // shipped gvim.desktop so that Vim's windows can be associated with this
    // file.
    g_ಗಣ_prgname("gvim");

    // Don't use gtk_init() or gnome_init(), it exits on failure.
    ಆದರೆ (!gtk_init_check(&gui_argc, &gui_argv))
    {
	gui.dying = true;
	emsg(_((ಅಕ್ಷರ *)e_cannot_open_display));
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}

/////////////////////////////////////////////////////////////////////////////
// Mouse handling callbacks


ಸ್ಥಿರ guint mouse_click_timer = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mouse_timed_ಹೊರಹೋಗು = TRUE;

/*
 * Timer used to recognize multiple clicks of the mouse button
 */
    ಸ್ಥಿರ timeout_cb_type
mouse_click_timer_cb(gpointer ದತ್ತಾಂಶ)
{
    // we don't use this information currently
    ಪೂರ್ಣಾಂಕ *timed_ಹೊರಹೋಗು = (ಪೂರ್ಣಾಂಕ *) ದತ್ತಾಂಶ;

    *timed_ಹೊರಹೋಗು = TRUE;
    ಹಿಂತಿರುಗಿ FALSE;		// don't happen again
}

ಸ್ಥಿರ guint		motion_repeat_timer  = 0;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		motion_repeat_ಸ್ಥಾನಾಂತರ = FALSE;
ಸ್ಥಿರ timeout_cb_type	motion_repeat_timer_cb(gpointer);

    ಸ್ಥಿರ ಶೂನ್ಯ
process_motion_notify(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, GdkModifierType state)
{
    ಪೂರ್ಣಾಂಕ	    button;
    int_u   vim_modifiers;
    Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;

    // Need to add GDK_BUTTON1_MASK state when dragging a touch.
    state |= dragging_button_state;

    button = (state & (GDK_BUTTON1_ಮುಖವಾಡ | GDK_BUTTON2_ಮುಖವಾಡ |
		       GDK_BUTTON3_ಮುಖವಾಡ | GDK_BUTTON4_ಮುಖವಾಡ |
		       GDK_BUTTON5_ಮುಖವಾಡ))
	      ? MOUSE_DRAG : ' ';

    // If our pointer is currently hidden, then we should show it.
    gui_mch_mousehide(FALSE);

    // Just moving the rodent above the drawing area without any button
    // being pressed.
    ಆದರೆ (button != MOUSE_DRAG)
    {
	gui_mouse_moved(x, y);
	ಹಿಂತಿರುಗಿ;
    }

    // translate modifier coding between the main engine and GTK
    vim_modifiers = modifiers_gdk2mouse(state);

    // inform the editor engine about the occurrence of this event
    gui_send_mouse_event(button, x, y, FALSE, vim_modifiers);

    /*
     * Auto repeat timer handling.
     */
    gtk_widget_get_ಹಂಚಿಕೆ(gui.drawarea, &ಹಂಚಿಕೆ);

    ಆದರೆ (x < 0 || y < 0
	    || x >= ಹಂಚಿಕೆ.ಅಗಲ
	    || y >= ಹಂಚಿಕೆ.ಎತ್ತರ)
    {

	ಪೂರ್ಣಾಂಕ dx;
	ಪೂರ್ಣಾಂಕ dy;
	ಪೂರ್ಣಾಂಕ offshoot;
	ಪೂರ್ಣಾಂಕ delay = 10;

	// Calculate the maximal distance of the cursor from the drawing area.
	// (offshoot can't become negative here!).
	dx = x < 0 ? -x : x - ಹಂಚಿಕೆ.ಅಗಲ;
	dy = y < 0 ? -y : y - ಹಂಚಿಕೆ.ಎತ್ತರ;

	offshoot = dx > dy ? dx : dy;

	// Make a linearly decaying timer delay with a threshold of 5 at a
	// distance of 127 pixels from the main window.
	//
	// One could think endlessly about the most ergonomic variant here.
	// For example it could make sense to calculate the distance from the
	// drags start instead...
	//
	// Maybe a parabolic interpolation would suite us better here too...
	ಆದರೆ (offshoot > 127)
	{
	    // 5 appears to be somehow near to my perceptual limits :-).
	    delay = 5;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    delay = (130 * (127 - offshoot)) / 127 + 5;
	}

	// shoot again
	ಆದರೆ (!motion_repeat_timer)
	    motion_repeat_timer = timeout_ಕೂಡಿಸು(delay, motion_repeat_timer_cb,
									 ಖಾಲಿ);
    }
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ GdkDevice *
gui_gtk_get_ಸೂಚಕ_device(GtkWidget *widget)
{
    Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ win = gtk_widget_get_ಕಿಟಕಿ(widget);
    GdkDisplay * ಸ್ಥಿರಾಂಕ dpy = gdk_ಕಿಟಕಿ_get_display(win);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,20,0)
    GdkSeat * ಸ್ಥಿರಾಂಕ seat = gdk_display_get_default_seat(dpy);
    ಹಿಂತಿರುಗಿ gdk_seat_get_ಸೂಚಕ(seat);
# ಇಲ್ಲದಿದ್ದರೆ
    GdkDeviceManager * ಸ್ಥಿರಾಂಕ mngr = gdk_display_get_device_manager(dpy);
    ಹಿಂತಿರುಗಿ gdk_device_manager_get_client_ಸೂಚಕ(mngr);
# endif
}

    ಸ್ಥಿರ Gdk_ಕಿಟಕಿ *
gui_gtk_get_ಸೂಚಕ(GtkWidget       *widget,
		    gint	    *x,
		    gint	    *y,
		    GdkModifierType *state)
{
    Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ win = gtk_widget_get_ಕಿಟಕಿ(widget);
    GdkDevice * ಸ್ಥಿರಾಂಕ dev = gui_gtk_get_ಸೂಚಕ_device(widget);
    ಹಿಂತಿರುಗಿ gdk_ಕಿಟಕಿ_get_device_position(win, dev , x, y, state);
}

# ಆದರೆ defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ Gdk_ಕಿಟಕಿ *
gui_gtk_ಕಿಟಕಿ_at_position(GtkWidget *widget,
			   gint      *x,
			   gint      *y)
{
    GdkDevice * ಸ್ಥಿರಾಂಕ dev = gui_gtk_get_ಸೂಚಕ_device(widget);
    ಹಿಂತಿರುಗಿ gdk_device_get_ಕಿಟಕಿ_at_position(dev, x, y);
}
# endif
#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
# define gui_gtk_get_ಸೂಚಕ(wid, x, y, s) \
    gdk_ಕಿಟಕಿ_get_ಸೂಚಕ((wid)->ಕಿಟಕಿ_2, x, y, s)
# define gui_gtk_ಕಿಟಕಿ_at_position(wid, x, y)	gdk_ಕಿಟಕಿ_at_ಸೂಚಕ(x, y)
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Timer used to recognize multiple clicks of the mouse button.
 */
    ಸ್ಥಿರ timeout_cb_type
motion_repeat_timer_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		    x;
    ಪೂರ್ಣಾಂಕ		    y;
    GdkModifierType state;

    gui_gtk_get_ಸೂಚಕ(gui.drawarea, &x, &y, &state);

    ಆದರೆ (!(state & (GDK_BUTTON1_ಮುಖವಾಡ | GDK_BUTTON2_ಮುಖವಾಡ |
		   GDK_BUTTON3_ಮುಖವಾಡ | GDK_BUTTON4_ಮುಖವಾಡ |
		   GDK_BUTTON5_ಮುಖವಾಡ)))
    {
	motion_repeat_timer = 0;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    // If there already is a mouse click in the input buffer, wait another
    // time (otherwise we would create a backlog of clicks)
    ಆದರೆ (vim_ಬಳಸಿದ_ಒಳ_ಒಳಹರಿವು_buf() > 10)
	ಹಿಂತಿರುಗಿ TRUE;

    motion_repeat_timer = 0;

    /*
     * Fake a motion event.
     * Trick: Pretend the mouse moved to the next character on every other
     * event, otherwise drag events will be discarded, because they are still
     * in the same character.
     */
    ಆದರೆ (motion_repeat_ಸ್ಥಾನಾಂತರ)
	x += gui.char_ಅಗಲ;

    motion_repeat_ಸ್ಥಾನಾಂತರ = !motion_repeat_ಸ್ಥಾನಾಂತರ;
    process_motion_notify(x, y, state);

    // Don't happen again.  We will get reinstalled in the synthetic event
    // if needed -- thus repeating should still work.
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ gint
motion_notify_event(GtkWidget *widget,
		    GdkEventMotion *event,
		    gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (event->is_hint)
    {
	ಪೂರ್ಣಾಂಕ		x;
	ಪೂರ್ಣಾಂಕ		y;
	GdkModifierType	state;

	gui_gtk_get_ಸೂಚಕ(widget, &x, &y, &state);
	process_motion_notify(x, y, state);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	process_motion_notify((ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
			      (GdkModifierType)event->state);
    }

    ಹಿಂತಿರುಗಿ TRUE; // handled
}


/*
 * Mouse button handling.  Note please that we are capturing multiple click's
 * by our own timeout mechanism instead of the one provided by GTK+ itself.
 * This is due to the way the generic VIM code is recognizing multiple clicks.
 */
    ಸ್ಥಿರ gint
button_press_event(GtkWidget *widget,
		   GdkEventButton *event,
		   gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ button;
    ಪೂರ್ಣಾಂಕ repeated_click = FALSE;
    ಪೂರ್ಣಾಂಕ x, y;
    int_u vim_modifiers;

    gui.event_ಸಮಯ = event->ಸಮಯ;

    // Make sure we have focus now we've been selected
    ಆದರೆ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    /*
     * Don't let additional events about multiple clicks send by GTK to us
     * after the initial button press event confuse us.
     */
    ಆದರೆ (event->type != GDK_BUTTON_PRESS)
	ಹಿಂತಿರುಗಿ FALSE;

    x = event->x;
    y = event->y;

    // Handle multiple clicks
    ಆದರೆ (!mouse_timed_ಹೊರಹೋಗು && mouse_click_timer)
    {
	timeout_remove(mouse_click_timer);
	mouse_click_timer = 0;
	repeated_click = TRUE;
    }

    mouse_timed_ಹೊರಹೋಗು = FALSE;
    mouse_click_timer = timeout_ಕೂಡಿಸು(p_mouset, mouse_click_timer_cb,
							     &mouse_timed_ಹೊರಹೋಗು);

    ಆಯ್ಕೆ (event->button)
    {
	// Keep in sync with gui_x11.c.
	// Buttons 4-7 are handled in scroll_event()
	ಸ್ಥಿತಿ 1:
		button = MOUSE_ಎಡ;
		// needed for touch-drag
		dragging_button_state |= GDK_BUTTON1_ಮುಖವಾಡ;
		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 2: button = MOUSE_ಮಧ್ಯ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 3: button = MOUSE_ಬಲ; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 8: button = MOUSE_X1; ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ 9: button = MOUSE_X2; ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ FALSE;		// Unknown button
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    // cancel any preediting
    ಆದರೆ (im_is_preediting())
	xim_reset();
#ಆದರೆ_ಅಂತ್ಯ

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(button, x, y, repeated_click, vim_modifiers);

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * GTK+ abstracts scrolling via the GdkEventScroll.
 */
    ಸ್ಥಿರ gboolean
scroll_event(GtkWidget *widget,
	     GdkEventScroll *event,
	     gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    button = 0;  // silence gcc
    int_u   vim_modifiers;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    ಸ್ಥಿರ ದ್ವಿಗುಣ  acc_x, acc_y;
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,22,0)
    ಸ್ಥಿರ guint32 ಕೊನೆಯ_smooth_event_ಸಮಯ;
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (gtk_socket_id != 0 && !gtk_widget_has_focus(widget))
	gtk_widget_grab_focus(widget);

    ಆಯ್ಕೆ (event->direction)
    {
	ಸ್ಥಿತಿ GDK_SCROLL_ಮೇಲೆ:
	    button = MOUSE_4;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_SCROLL_ಕೆಳಕ್ಕೆ:
	    button = MOUSE_5;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_SCROLL_ಎಡ:
	    button = MOUSE_7;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GDK_SCROLL_ಬಲ:
	    button = MOUSE_6;
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
	ಸ್ಥಿತಿ GDK_SCROLL_SMOOTH:
	    ಆದರೆ (event->is_stop)
	    {
		acc_x = acc_y = 0;
		// this event tells us to stop, without an actual moving
		ಹಿಂತಿರುಗಿ FALSE;
	    }
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,0)
	    ಆದರೆ (gdk_device_get_axes(event->device) & GDK_AXIS_FLAG_WHEEL)
		// this is from a wheel (as oppose to a touchpad / trackpoint)
# ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (event->ಸಮಯ - ಕೊನೆಯ_smooth_event_ಸಮಯ > 50)
		// reset our accumulations after 50ms of silence
# endif
		acc_x = acc_y = 0;
	    acc_x += event->delta_x;
	    acc_y += event->delta_y;
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,22,0)
	    ಕೊನೆಯ_smooth_event_ಸಮಯ = event->ಸಮಯ;
# endif
	    ನಿಲ್ಲಿಸು;
#ಆದರೆ_ಅಂತ್ಯ
	ಪೂರ್ವನಿಯೋಜಿತ: // This shouldn't happen
	    ಹಿಂತಿರುಗಿ FALSE;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    // cancel any preediting
    ಆದರೆ (im_is_preediting())
	xim_reset();
#ಆದರೆ_ಅಂತ್ಯ

    vim_modifiers = modifiers_gdk2mouse(event->state);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
# ifdef GDK_WINDOWING_WAYLAND
    ಆದರೆ (event->direction == GDK_SCROLL_SMOOTH && gui.is_wayland)
    {
	ಆಗ (acc_x >= 1.0)
	{ //ಬಲ
	    acc_x = MAX(0.0, acc_x - 1.0);
	    gui_send_mouse_event(MOUSE_6, (ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
		    FALSE, vim_modifiers);
	}
	ಆಗ (acc_x <= -1.0)
	{ //ಎಡ
	    acc_x = MIN(0.0, acc_x + 1.0);
	    gui_send_mouse_event(MOUSE_7, (ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
		    FALSE, vim_modifiers);
	}
	ಆಗ (acc_y >= 1.0)
	{ //ಕೆಳಕ್ಕೆ
	    acc_y = MAX(0.0, acc_y - 1.0);
	    gui_send_mouse_event(MOUSE_5, (ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
		    FALSE, vim_modifiers);
	}
	ಆಗ (acc_y <= -1.0)
	{ //ಮೇಲೆ
	    acc_y = MIN(0.0, acc_y + 1.0);
	    gui_send_mouse_event(MOUSE_4, (ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
		    FALSE, vim_modifiers);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    // on x11, despite not requested, when we copy into primary clipboard,
    // we'll get smooth events. Unsmooth ones will also come along.
    ಆದರೆ (event->direction == GDK_SCROLL_SMOOTH && X_DISPLAY)
	// for X11 we deal with unsmooth events, and so ignore the smooth ones
	;
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	gui_send_mouse_event(button, (ಪೂರ್ಣಾಂಕ)event->x, (ಪೂರ್ಣಾಂಕ)event->y,
		FALSE, vim_modifiers);

    ಹಿಂತಿರುಗಿ TRUE;
}


    ಸ್ಥಿರ gint
button_release_event(GtkWidget *widget ಬಳಸದ,
		     GdkEventButton *event,
		     gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ x, y;
    int_u vim_modifiers;

    gui.event_ಸಮಯ = event->ಸಮಯ;

    // Remove any motion "machine gun" timers used for automatic further
    // extension of allocation areas if outside of the applications window
    // area .
    ಆದರೆ (motion_repeat_timer)
    {
	timeout_remove(motion_repeat_timer);
	motion_repeat_timer = 0;
    }

    x = event->x;
    y = event->y;

    vim_modifiers = modifiers_gdk2mouse(event->state);

    gui_send_mouse_event(MOUSE_RELEASE, x, y, FALSE, vim_modifiers);

    ಆಯ್ಕೆ (event->button)
    {
	ಸ್ಥಿತಿ 1:  // MOUSE_LEFT
	    dragging_button_state = 0;
	    ನಿಲ್ಲಿಸು;
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Another widget, e.g. a modal dialog, was shadowing us with a GTK grab. The
 * button release went to that widget, thus forget about the pressed button to
 * avoid that the next mouse move is taken for a drag.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
grab_notify_event(GtkWidget *widget ಬಳಸದ,
		  gboolean  was_grabbed,
		  gpointer  ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (!was_grabbed)
	dragging_button_state = 0;
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
/////////////////////////////////////////////////////////////////////////////
// Drag aNd Drop support handlers.

/*
 * Count how many items there may be and separate them with a NUL.
 * Apparently the items are separated with \r\n.  This is not documented,
 * thus be careful not to go past the end.	Also allow separation with
 * NUL characters.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_and_decode_uri_list(char_u *ಹೊರಹೋಗು, char_u *raw, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*p = ಹೊರಹೋಗು;
    ಪೂರ್ಣಾಂಕ		ಎಣಿಕೆ_2 = 0;

    ಗಾಗಿ (i = 0; i < len; ++i)
    {
	ಆದರೆ (raw[i] == NUL || raw[i] == '\n' || raw[i] == '\r')
	{
	    ಆದರೆ (p > ಹೊರಹೋಗು && p[-1] != NUL)
	    {
		++ಎಣಿಕೆ_2;
		*p++ = NUL;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (raw[i] == '%' && i + 2 < len && hexhex2nr(raw + i + 1) > 0)
	{
	    *p++ = hexhex2nr(raw + i + 1);
	    i += 2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    *p++ = raw[i];
    }
    ಆದರೆ (p > ಹೊರಹೋಗು && p[-1] != NUL)
    {
	*p = NUL;	// last item didn't have \r or \n
	++ಎಣಿಕೆ_2;
    }
    ಹಿಂತಿರುಗಿ ಎಣಿಕೆ_2;
}

/*
 * Parse NUL separated "src" strings.  Make it an array "outlist" form.  On
 * this process, URI which protocol is not "file:" are removed.  Return
 * length of array (less than "max").
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
filter_uri_list(char_u **outlist, ಪೂರ್ಣಾಂಕ max, char_u *src)
{
    ಪೂರ್ಣಾಂಕ	i, j;

    ಗಾಗಿ (i = j = 0; i < max; ++i)
    {
	outlist[i] = ಖಾಲಿ;
	ಆದರೆ (STRNCMP(src, "file:", 5) == 0)
	{
	    src += 5;
	    ಆದರೆ (STRNCMP(src, "//localhost", 11) == 0)
		src += 11;
	    ಆಗ (src[0] == '/' && src[1] == '/')
		++src;
	    outlist[j++] = vim_strsave(src);
	}
	src += STRLEN(src) + 1;
    }
    ಹಿಂತಿರುಗಿ j;
}

    ಸ್ಥಿರ char_u **
parse_uri_list(ಪೂರ್ಣಾಂಕ *ಎಣಿಕೆ_2, char_u *ದತ್ತಾಂಶ, ಪೂರ್ಣಾಂಕ len)
{
    ಪೂರ್ಣಾಂಕ	    n	    = 0;
    char_u  *tmp    = ಖಾಲಿ;
    char_u  **array = ಖಾಲಿ;

    ಆದರೆ (ದತ್ತಾಂಶ != ಖಾಲಿ && len > 0 && (tmp = alloc(len + 1)) != ಖಾಲಿ)
    {
	n = ಎಣಿಕೆ_and_decode_uri_list(tmp, ದತ್ತಾಂಶ, len);
	ಆದರೆ (n > 0 && (array = ALLOC_MULT(char_u *, n)) != ಖಾಲಿ)
	    n = filter_uri_list(array, n, tmp);
    }
    vim_free(tmp);
    *ಎಣಿಕೆ_2 = n;
    ಹಿಂತಿರುಗಿ array;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drag_handle_uri_list(GdkDragContext	*context,
		     Gtk_Selection_ದತ್ತಾಂಶ	*ದತ್ತಾಂಶ,
		     guint		ಸಮಯ_2,
		     GdkModifierType	state,
		     gint		x,
		     gint		y)
{
    char_u  **fnames;
    ಪೂರ್ಣಾಂಕ	    nfiles = 0;

    fnames = parse_uri_list(&nfiles,
	    (char_u *)gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ),
	    gtk_selection_ದತ್ತಾಂಶ_get_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ));

    ಆದರೆ (fnames != ಖಾಲಿ && nfiles > 0)
    {
	int_u   modifiers;

	gtk_drag_ಮುಕ್ತಾಯ(context, TRUE, FALSE, ಸಮಯ_2); // accept

	modifiers = modifiers_gdk2mouse(state);

	gui_handle_drop(x, y, modifiers, fnames, nfiles);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	vim_free(fnames);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
drag_handle_ಪಠ್ಯ(GdkDragContext	    *context,
		 Gtk_Selection_ದತ್ತಾಂಶ   *ದತ್ತಾಂಶ,
		 guint		    ಸಮಯ_2,
		 GdkModifierType    state)
{
    char_u  dropkey[6] = {CSI, KS_MODIFIER, 0, CSI, KS_EXTRA, (char_u)KE_DROP};
    char_u  *ಪಠ್ಯ;
    ಪೂರ್ಣಾಂಕ	    len;
    char_u  *tmpbuf = ಖಾಲಿ;

    ಪಠ್ಯ = (char_u *)gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ);
    len = gtk_selection_ದತ್ತಾಂಶ_get_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);

    ಆದರೆ (gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ_type(ದತ್ತಾಂಶ) == utf8_string_atom)
    {
	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    tmpbuf = string_convert(&ಒಳಹರಿವು_conv, ಪಠ್ಯ, &len);
	ಆದರೆ (tmpbuf != ಖಾಲಿ)
	    ಪಠ್ಯ = tmpbuf;
    }

    dnd_yank_drag_ದತ್ತಾಂಶ(ಪಠ್ಯ, (ದೀರ್ಘ)len);
    gtk_drag_ಮುಕ್ತಾಯ(context, TRUE, FALSE, ಸಮಯ_2); // accept
    vim_free(tmpbuf);

    dropkey[2] = modifiers_gdk2vim(state);

    ಆದರೆ (dropkey[2] != 0)
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(dropkey, (ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ(dropkey));
    ಇಲ್ಲದಿದ್ದರೆ
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(dropkey + 3, (ಪೂರ್ಣಾಂಕ)(ಗಾತ್ರ(dropkey) - 3));
}

/*
 * DND receiver.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drag_ದತ್ತಾಂಶ_received_cb(GtkWidget		*widget,
		      GdkDragContext	*context,
		      gint		x,
		      gint		y,
		      Gtk_Selection_ದತ್ತಾಂಶ	*ದತ್ತಾಂಶ,
		      guint		info,
		      guint		ಸಮಯ_2,
		      gpointer		user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    GdkModifierType state;

    // Guard against trash
    ಸ್ಥಿರಾಂಕ guchar * ಸ್ಥಿರಾಂಕ ದತ್ತಾಂಶ_ದತ್ತಾಂಶ = gtk_selection_ದತ್ತಾಂಶ_get_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ);
    ಸ್ಥಿರಾಂಕ gint ದತ್ತಾಂಶ_ಉದ್ದಳತೆ = gtk_selection_ದತ್ತಾಂಶ_get_ಉದ್ದಳತೆ(ದತ್ತಾಂಶ);
    ಸ್ಥಿರಾಂಕ gint ದತ್ತಾಂಶ_format = gtk_selection_ದತ್ತಾಂಶ_get_format(ದತ್ತಾಂಶ);

    ಆದರೆ (ದತ್ತಾಂಶ_ದತ್ತಾಂಶ == ಖಾಲಿ
	    || ದತ್ತಾಂಶ_ಉದ್ದಳತೆ <= 0
	    || ದತ್ತಾಂಶ_format != 8
	    || ದತ್ತಾಂಶ_ದತ್ತಾಂಶ[ದತ್ತಾಂಶ_ಉದ್ದಳತೆ] != '\0')
    {
	gtk_drag_ಮುಕ್ತಾಯ(context, FALSE, FALSE, ಸಮಯ_2);
	ಹಿಂತಿರುಗಿ;
    }

    // Get the current modifier state for proper distinguishment between
    // different operations later.
    gui_gtk_get_ಸೂಚಕ(widget, ಖಾಲಿ, ಖಾಲಿ, &state);

    // Not sure about the role of "text/plain" here...
    ಆದರೆ (info == (guint)ಗುರಿ_ಪಠ್ಯ_URI_LIST)
	drag_handle_uri_list(context, ದತ್ತಾಂಶ, ಸಮಯ_2, state, x, y);
    ಇಲ್ಲದಿದ್ದರೆ
	drag_handle_ಪಠ್ಯ(context, ದತ್ತಾಂಶ, ಸಮಯ_2, state);

}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_DND


#ಆದರೆ defined(USE_GNOME_SESSION)
/*
 * GnomeClient interact callback.  Check for unsaved buffers that cannot
 * be abandoned and pop up a dialog asking the user for confirmation if
 * necessary.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sm_client_check_ಬದಲಾಯಿತು_any(GnomeClient	    *client ಬಳಸದ,
			    gint	    ಕೀಲಿ,
			    GnomeDialogType type ಬಳಸದ,
			    gpointer	    ದತ್ತಾಂಶ ಬಳಸದ)
{
    cmdmod_T	save_cmdmod;
    gboolean	shutdown_cancelled;

    save_cmdmod = cmdmod;

# ifdef FEAT_BROWSE
    cmdmod.cmod_flags |= CMOD_BROWSE;
# endif
# ಆದರೆ defined(FEAT_GUI_DIALOG) || defined(FEAT_CON_DIALOG)
    cmdmod.cmod_flags |= CMOD_CONFIRM;
# endif
    /*
     * If there are changed buffers, present the user with
     * a dialog if possible, otherwise give an error message.
     */
    shutdown_cancelled = check_ಬದಲಾಯಿತು_any(FALSE, FALSE);

    exiting = FALSE;
    cmdmod = save_cmdmod;
    setcursor(); // position the cursor
    ಹೊರಹೋಗು_flush();
    /*
     * If the user hit the [Cancel] button the whole shutdown
     * will be cancelled.  Wow, quite powerful feature (:
     */
    gnome_interaction_ಕೀಲಿ_return(ಕೀಲಿ, shutdown_cancelled);
}

/*
 * "save_yourself" signal handler.  Initiate an interaction to ask the user
 * for confirmation if necessary.  Save the current editing session and tell
 * the session manager how to restart Vim.
 */
    ಸ್ಥಿರ gboolean
sm_client_save_yourself(GnomeClient	    *client,
			gint		    phase ಬಳಸದ,
			GnomeSaveStyle	    save_style ಬಳಸದ,
			gboolean	    shutdown ಬಳಸದ,
			GnomeInteractStyle  interact_style,
			gboolean	    fast ಬಳಸದ,
			gpointer	    ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	suffix[] = "-session.vim";
    ಅಕ್ಷರ		*session_file;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	len;
    gboolean		success;

    // Always request an interaction if possible.  check_changed_any()
    // won't actually show a dialog unless any buffers have been modified.
    // There doesn't seem to be an obvious way to check that without
    // automatically firing the dialog.  Anyway, it works just fine.
    ಆದರೆ (interact_style == GNOME_INTERACT_ANY)
	gnome_client_request_interaction(client, GNOME_DIALOG_ಸಾಮಾನ್ಯ,
					 &sm_client_check_ಬದಲಾಯಿತು_any,
					 ಖಾಲಿ);
    ಹೊರಹೋಗು_flush();
    ml_sync_all(FALSE, FALSE); // preserve all swap files

    // The path is unique for each session save.  We do neither know nor care
    // which session script will actually be used later.  This decision is in
    // the domain of the session manager.
    session_file = gnome_config_get_real_ಮಾರ್ಗ(
			gnome_client_get_config_ಪೂರ್ವಪ್ರತ್ಯಯ(client));
    len = ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(session_file);

    ಆದರೆ (len > 0 && session_file[len-1] == G_DIR_SEPARATOR)
	--len; // get rid of the superfluous trailing '/'

    session_file = g_renew(ಅಕ್ಷರ, session_file, len + ಗಾತ್ರ(suffix));
    ಸ್ಮರಣೆ_ನಕಲು(session_file + len, suffix, ಗಾತ್ರ(suffix));

    success = write_session_file((char_u *)session_file);

    ಆದರೆ (success)
    {
	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ  *argv[8];
	ಪೂರ್ಣಾಂಕ	    i;

	// Tell the session manager how to wipe out the stored session data.
	// This isn't as dangerous as it looks, don't worry :)	session_file
	// is a unique absolute filename.  Usually it'll be something like
	// `/home/user/.gnome2/vim-XXXXXX-session.vim'.
	i = 0;
	argv[i++] = "rm";
	argv[i++] = session_file;
	argv[i] = ಖಾಲಿ;

	gnome_client_ಗಣ_discard_command(client, i, (ಅಕ್ಷರ **)argv);

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
	argv[i++] = gtk_ಕಿಟಕಿ_get_role(GTK_ಕಿಟಕಿ(gui.mainwin));
	argv[i++] = "-S";
	argv[i++] = session_file;
	argv[i] = ಖಾಲಿ;

	gnome_client_ಗಣ_restart_command(client, i, (ಅಕ್ಷರ **)argv);
	gnome_client_ಗಣ_clone_command(client, 0, ಖಾಲಿ);
    }

    g_free(session_file);

    ಹಿಂತಿರುಗಿ success;
}

/*
 * Called when the session manager wants us to die.  There isn't much to save
 * here since "save_yourself" has been emitted before (unless serious trouble
 * is happening).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sm_client_die(GnomeClient *client ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Don't write messages to the GUI anymore
    ತುಂಬಿದ_screen = FALSE;

    vim_strncpy(IObuff, (char_u *)
		    _("Vim: Received \"die\" request from session manager\n"),
		    IOSIZE - 1);
    preserve_exit();
}

/*
 * Connect our signal handlers to be notified on session save and shutdown.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_save_yourself(ಶೂನ್ಯ)
{
    GnomeClient *client;

    client = gnome_master_client();
    ಆದರೆ (client == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // Must use the deprecated gtk_signal_connect() for compatibility
    // with GNOME 1.  Arrgh, zombies!
    gtk_signal_connect(GTK_OBJECT(client), "save_yourself",
	    GTK_SIGNAL_FUNC(&sm_client_save_yourself), ಖಾಲಿ);
    gtk_signal_connect(GTK_OBJECT(client), "die",
	    GTK_SIGNAL_FUNC(&sm_client_die), ಖಾಲಿ);
}

#ಇಲ್ಲದಿದ್ದರೆ // !USE_GNOME_SESSION

# ifdef USE_XSMP
/*
 * GTK tells us that XSMP needs attention
 */
    ಸ್ಥಿರ gboolean
local_xsmp_handle_ವಿನಂತಿಗಳು(
    GIOChannel		*ಮೂಲ ಬಳಸದ,
    GIOCondition	condition,
    gpointer		ದತ್ತಾಂಶ)
{
    ಆದರೆ (condition == G_IO_ಒಳ)
    {
	// Do stuff; maybe close connection
	ಆದರೆ (xsmp_handle_ವಿನಂತಿಗಳು() == FAIL)
	    g_io_channel_unref((GIOChannel *)ದತ್ತಾಂಶ);
	ಹಿಂತಿರುಗಿ TRUE;
    }
    // Error
    g_io_channel_unref((GIOChannel *)ದತ್ತಾಂಶ);
    xsmp_close();
    ಹಿಂತಿರುಗಿ TRUE;
}
# endif // USE_XSMP

/*
 * Setup the WM_PROTOCOLS to indicate we want the WM_SAVE_YOURSELF event.
 * This is an ugly use of X functions.	GTK doesn't offer an alternative.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_save_yourself(ಶೂನ್ಯ)
{
    Atom    *existing_atoms = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ	    ಎಣಿಕೆ_2 = 0;

# ifdef USE_XSMP
    ಆದರೆ (xsmp_icefd != -1)
    {
	/*
	 * Use XSMP is preference to legacy WM_SAVE_YOURSELF;
	 * set up GTK IO monitor
	 */
	GIOChannel *g_io = g_io_channel_unix_new(xsmp_icefd);

	g_io_ಕೂಡಿಸು_watch(g_io, G_IO_ಒಳ | G_IO_ERR | G_IO_HUP,
				  local_xsmp_handle_ವಿನಂತಿಗಳು, (gpointer)g_io);
	g_io_channel_unref(g_io);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	// Fall back to old method

	// first get the existing value
	Display * dpy = gui_mch_get_display();
	ಆದರೆ (!dpy)
	    ಹಿಂತಿರುಗಿ;

	Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ mainwin_win = gtk_widget_get_ಕಿಟಕಿ(gui.mainwin);
	ಆದರೆ (XGetWMProtocols(dpy, GDK_ಕಿಟಕಿ_XID(mainwin_win),
		    &existing_atoms, &ಎಣಿಕೆ_2))
	{
	    Atom	*new_atoms;
	    Atom	save_yourself_xatom;
	    ಪೂರ್ಣಾಂಕ	i;

	    save_yourself_xatom = GET_X_ATOM(save_yourself_atom);

	    // check if WM_SAVE_YOURSELF isn't there yet
	    ಗಾಗಿ (i = 0; i < ಎಣಿಕೆ_2; ++i)
		ಆದರೆ (existing_atoms[i] == save_yourself_xatom)
		    ನಿಲ್ಲಿಸು;

	    ಆದರೆ (i == ಎಣಿಕೆ_2)
	    {
		// allocate an Atoms array which is one item longer
		new_atoms = ALLOC_MULT(Atom, ಎಣಿಕೆ_2 + 1);
		ಆದರೆ (new_atoms != ಖಾಲಿ)
		{
		    ಸ್ಮರಣೆ_ನಕಲು(new_atoms, existing_atoms, ಎಣಿಕೆ_2 * ಗಾತ್ರ(Atom));
		    new_atoms[ಎಣಿಕೆ_2] = save_yourself_xatom;
		    XSetWMProtocols(GDK_ಕಿಟಕಿ_XDISPLAY(mainwin_win),
			    GDK_ಕಿಟಕಿ_XID(mainwin_win),
			    new_atoms, ಎಣಿಕೆ_2 + 1);
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
    ಸ್ಥಿರ GdkFilterReturn
global_event_filter(GdkXEvent *xev,
		    GdkEvent *event ಬಳಸದ,
		    gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    XEvent *xevent = (XEvent *)xev;

    ಆದರೆ (xevent != ಖಾಲಿ
	    && xevent->type == Client_ಸಂದೇಶ
	    && xevent->xclient.ಸಂದೇಶ_type == GET_X_ATOM(wm_protocols_atom)
	    && (long_u)xevent->xclient.ದತ್ತಾಂಶ.l[0]
					    == GET_X_ATOM(save_yourself_atom))
    {
	ಹೊರಹೋಗು_flush();
	ml_sync_all(FALSE, FALSE); // preserve all swap files
	/*
	 * Set the window's WM_COMMAND property, to let the window manager
	 * know we are done saving ourselves.  We don't want to be
	 * restarted, thus set argv to NULL.
	 */
	XSetCommand(GDK_ಕಿಟಕಿ_XDISPLAY(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin)),
		    GDK_ಕಿಟಕಿ_XID(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin)),
		    ಖಾಲಿ, 0);
	ಹಿಂತಿರುಗಿ GDK_FILTER_REMOVE;
    }

    ಹಿಂತಿರುಗಿ GDK_FILTER_CONTINUE;
}
#ಆದರೆ_ಅಂತ್ಯ // !USE_GNOME_SESSION

    ಸ್ಥಿರ GdkPixbuf *
pixbuf_new_from_png_ದತ್ತಾಂಶ(ಸ್ಥಿರಾಂಕ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ *ದತ್ತಾಂಶ, ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ len)
{
    GInput_ಹರಿವು *ಹರಿವು;
    GdkPixbuf    *pixbuf;

    ಹರಿವು = g_ಸ್ಮರಣೆ_ಒಳಹರಿವು_ಹರಿವು_new_from_ದತ್ತಾಂಶ(ದತ್ತಾಂಶ, len, ಖಾಲಿ);
    pixbuf = gdk_pixbuf_new_from_ಹರಿವು(ಹರಿವು, ಖಾಲಿ, ಖಾಲಿ);
    g_object_unref(ಹರಿವು);
    ಹಿಂತಿರುಗಿ pixbuf;
}

/*
 * Setup the window icon & xcmdsrv comm after the main window has been realized.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_realize(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
#ಅನ್ತರ್ಭಾವ "../runtime/vim16x16_png.h"
#ಅನ್ತರ್ಭಾವ "../runtime/vim32x32_png.h"
#ಅನ್ತರ್ಭಾವ "../runtime/vim48x48_png.h"

    Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ mainwin_win = gtk_widget_get_ಕಿಟಕಿ(gui.mainwin);

    // When started with "--echo-wid" argument, write window ID on stdout.
    ಆದರೆ (echo_wid_arg)
    {
	ಆದರೆ (gui_mch_get_display())
	    ಬರೆ("WID: %ld\n", (ದೀರ್ಘ)GDK_ಕಿಟಕಿ_XID(mainwin_win));
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬರೆ("WID: 0\n");
	fflush(stdout);
    }

    ಆದರೆ (vim_strchr(p_go, GO_ICON) != ಖಾಲಿ)
    {
	GtkIconTheme *icon_theme;

	icon_theme = gtk_icon_theme_get_default();

	ಆದರೆ (icon_theme && gtk_icon_theme_has_icon(icon_theme, "gvim"))
	    gtk_ಕಿಟಕಿ_ಗಣ_icon_name(GTK_ಕಿಟಕಿ(gui.mainwin), "gvim");
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	     * Add an icon to the main window. For fun and convenience of the user.
	     */
	    GList *icons = ಖಾಲಿ;

	    icons = g_list_prepend(icons, pixbuf_new_from_png_ದತ್ತಾಂಶ(vim16x16_png, vim16x16_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ದತ್ತಾಂಶ(vim32x32_png, vim32x32_png_len));
	    icons = g_list_prepend(icons, pixbuf_new_from_png_ದತ್ತಾಂಶ(vim48x48_png, vim48x48_png_len));

	    gtk_ಕಿಟಕಿ_ಗಣ_icon_list(GTK_ಕಿಟಕಿ(gui.mainwin), icons);

	    // TODO: is this type cast OK?
	    g_list_foreach(icons, (GFunc)(ಶೂನ್ಯ *)&g_object_unref, ಖಾಲಿ);
	    g_list_free(icons);
	}
    }

#ಆದರೆ !defined(USE_GNOME_SESSION)
    // Register a handler for WM_SAVE_YOURSELF with GDK's low-level X I/F
    gdk_ಕಿಟಕಿ_ಕೂಡಿಸು_filter(ಖಾಲಿ, &global_event_filter, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    // Setup to indicate to the window manager that we want to catch the
    // WM_SAVE_YOURSELF event.	For GNOME, this connects to the session
    // manager instead.
#ಆದರೆ defined(USE_GNOME_SESSION)
    ಆದರೆ (using_gnome)
#ಆದರೆ_ಅಂತ್ಯ
	ಸಿದ್ಧತೆ_save_yourself();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CLIENTSERVER
    ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 && gui_mch_get_display())
    {
	ಆದರೆ (serverName == ಖಾಲಿ && server_Delayed_ಆರಂಭ_Name != ಖಾಲಿ)
	{
	    // This is a :gui command in a plain vim with no previous server
	    comm_ಕಿಟಕಿ = GDK_ಕಿಟಕಿ_XID(mainwin_win);

	    (ಶೂನ್ಯ)server_ರಿಜಿಸ್ಟರ್_Name(GDK_ಕಿಟಕಿ_XDISPLAY(mainwin_win),
				    server_Delayed_ಆರಂಭ_Name);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    /*
	    * Cannot handle "XLib-only" windows with gtk event routines, we'll
	    * have to change the "server" registration to that of the main window
	    * If we have not registered a name yet, remember the window.
	    */
	    server_Change_Registered_ಕಿಟಕಿ(GDK_ಕಿಟಕಿ_XDISPLAY(mainwin_win),
					GDK_ಕಿಟಕಿ_XID(mainwin_win));
	}
	gtk_widget_ಕೂಡಿಸು_events(gui.mainwin, GDK_PROPERTY_CHANGE_ಮುಖವಾಡ);
	g_signal_connect(G_OBJECT(gui.mainwin), "property-notify-event",
			G_CALLBACK(property_event), ಖಾಲಿ);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ GdkCursor *
create_blank_ಸೂಚಕ(ಶೂನ್ಯ)
{
    GdkCursor	*cursor;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    GdkDisplay  *disp = gtk_widget_get_display(gui.mainwin);
    cursor = gdk_cursor_new_from_name(disp, "none");
#ಇಲ್ಲದಿದ್ದರೆ
    // Create a pseudo blank pointer, which is in fact one pixel by one pixel
    // in size.
    Gdk_ಕಿಟಕಿ	*ಬೇರು_ಕಿಟಕಿ = ಖಾಲಿ;
    GdkColor	color = { 0, 0, 0, 0 };
    GdkPixmap	*blank_ಮುಖವಾಡ;
    ಅಕ್ಷರ	blank_ದತ್ತಾಂಶ[] = { 0x0 };
    ಬೇರು_ಕಿಟಕಿ = gtk_widget_get_ಬೇರು_ಕಿಟಕಿ(gui.mainwin);
    blank_ಮುಖವಾಡ = gdk_bitmap_create_from_ದತ್ತಾಂಶ(ಬೇರು_ಕಿಟಕಿ, blank_ದತ್ತಾಂಶ, 1, 1);
    cursor = gdk_cursor_new_from_pixmap(blank_ಮುಖವಾಡ, blank_ಮುಖವಾಡ,
					&color, &color, 0, 0);
    gdk_bitmap_unref(blank_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ cursor;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_screen_ಬದಲಾಯಿತು_cb(GtkWidget  *widget,
			  GdkScreen  *ಹಿಂದಿನ_screen ಬಳಸದ,
			  gpointer   ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (!gtk_widget_has_screen(widget))
	ಹಿಂತಿರುಗಿ;

    /*
     * Recreate the invisible mouse cursor.
     */
    ಆದರೆ (gui.blank_ಸೂಚಕ != ಖಾಲಿ)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	g_object_unref(G_OBJECT(gui.blank_ಸೂಚಕ));
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_cursor_unref(gui.blank_ಸೂಚಕ);
#ಆದರೆ_ಅಂತ್ಯ

    gui.blank_ಸೂಚಕ = create_blank_ಸೂಚಕ();

    ಆದರೆ (gui.ಸೂಚಕ_hidden && gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) != ಖಾಲಿ)
	gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea),
		gui.blank_ಸೂಚಕ);

    /*
     * Create a new PangoContext for this screen, and initialize it
     * with the current font if necessary.
     */
    ಆದರೆ (gui.ಪಠ್ಯ_context != ಖಾಲಿ)
	g_object_unref(gui.ಪಠ್ಯ_context);

    gui.ಪಠ್ಯ_context = gtk_widget_create_pango_context(widget);
    pango_context_ಗಣ_ತಳ_dir(gui.ಪಠ್ಯ_context, PANGO_DIRECTION_LTR);

    ಆದರೆ (gui.norm_font != ಖಾಲಿ)
    {
	gui_mch_init_font(p_guifont, FALSE);
	gui_ಗಣ_shellsize(TRUE, FALSE, RESIZE_BOTH);
    }
}

    ಸ್ಥಿರ gboolean
mainwin_state_event_cb(GtkWidget *widget ಬಳಸದ,
		       Gdk_Event_ಕಿಟಕಿ_State *event,
		       gpointer user_ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (event->new_ಕಿಟಕಿ_state & GDK_ಕಿಟಕಿ_STATE_FULLSCREEN)
	// To redraw the text area when switching to fullscreen mode, we will
	// reuse the `gui_focus_change` method.
	gui_focus_change(TRUE);
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * After the drawing area comes up, we calculate all colors and create the
 * dummy blank cursor.
 *
 * Don't try to set any VIM scrollbar sizes anywhere here. I'm relying on the
 * fact that the main VIM engine doesn't take them into account anywhere.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_realize_cb(GtkWidget *widget, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    GtkWidget *sbar;
    Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    xim_init();
#ಆದರೆ_ಅಂತ್ಯ
    gui_mch_new_colors();
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gui.surface = gdk_ಕಿಟಕಿ_create_similar_surface(
	    gtk_widget_get_ಕಿಟಕಿ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    gtk_widget_get_allocated_ಅಗಲ(widget),
	    gtk_widget_get_allocated_ಎತ್ತರ(widget));
#ಇಲ್ಲದಿದ್ದರೆ
    gui.ಪಠ್ಯ_gc = gdk_gc_new(gui.drawarea->ಕಿಟಕಿ_2);
#ಆದರೆ_ಅಂತ್ಯ

    gui.blank_ಸೂಚಕ = create_blank_ಸೂಚಕ();
    ಆದರೆ (gui.ಸೂಚಕ_hidden)
	gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(widget), gui.blank_ಸೂಚಕ);

    // get the actual size of the scrollbars, if they are realized
    sbar = firstwin->w_scrollbars[SBAR_ಎಡ].id;
    ಆದರೆ (!sbar || (!gui.which_scrollbars[SBAR_ಎಡ]
				    && firstwin->w_scrollbars[SBAR_ಬಲ].id))
	sbar = firstwin->w_scrollbars[SBAR_ಬಲ].id;
    gtk_widget_get_ಹಂಚಿಕೆ(sbar, &ಹಂಚಿಕೆ);
    ಆದರೆ (sbar && gtk_widget_get_realized(sbar) && ಹಂಚಿಕೆ.ಅಗಲ)
	gui.scrollbar_ಅಗಲ = ಹಂಚಿಕೆ.ಅಗಲ;

    sbar = gui.bottom_sbar.id;
    ಆದರೆ (sbar && gtk_widget_get_realized(sbar) && ಹಂಚಿಕೆ.ಎತ್ತರ)
	gui.scrollbar_ಎತ್ತರ = ಹಂಚಿಕೆ.ಎತ್ತರ;
}

/*
 * Properly clean up on shutdown.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_unrealize_cb(GtkWidget *widget ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Don't write messages to the GUI anymore
    ತುಂಬಿದ_screen = FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XIM
    im_shutdown();
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gui.ascii_glyphs != ಖಾಲಿ)
    {
	pango_glyph_string_free(gui.ascii_glyphs);
	gui.ascii_glyphs = ಖಾಲಿ;
    }
    ಆದರೆ (gui.ascii_font != ಖಾಲಿ)
    {
	g_object_unref(gui.ascii_font);
	gui.ascii_font = ಖಾಲಿ;
    }
    g_object_unref(gui.ಪಠ್ಯ_context);
    gui.ಪಠ್ಯ_context = ಖಾಲಿ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಆದರೆ (gui.surface != ಖಾಲಿ)
    {
	cairo_surface_destroy(gui.surface);
	gui.surface = ಖಾಲಿ;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    g_object_unref(gui.ಪಠ್ಯ_gc);
    gui.ಪಠ್ಯ_gc = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    g_object_unref(G_OBJECT(gui.blank_ಸೂಚಕ));
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_cursor_unref(gui.blank_ಸೂಚಕ);
#ಆದರೆ_ಅಂತ್ಯ
    gui.blank_ಸೂಚಕ = ಖಾಲಿ;
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_style_updated_cb(GtkWidget *widget ಬಳಸದ,
			 gpointer ದತ್ತಾಂಶ ಬಳಸದ)
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
drawarea_style_ಗಣ_cb(GtkWidget	*widget ಬಳಸದ,
		      GtkStyle	*ಹಿಂದಿನ_style ಬಳಸದ,
		      gpointer	ದತ್ತಾಂಶ ಬಳಸದ)
#ಆದರೆ_ಅಂತ್ಯ
{
    gui_mch_new_colors();
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ gboolean
drawarea_configure_event_cb(GtkWidget	      *widget,
			    GdkEventConfigure *event,
			    gpointer	       ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cur_ಅಗಲ = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ cur_ಎತ್ತರ = 0;

    g_return_val_if_fail(event
	    && event->ಅಗಲ >= 1 && event->ಎತ್ತರ >= 1, TRUE);

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2) && !GTK_CHECK_ಆವೃತ್ತಿ(3,22,4)
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
    ಆದರೆ (event->send_event == FALSE)
	ಹಿಂತಿರುಗಿ TRUE;
# endif

    ಆದರೆ (event->ಅಗಲ == cur_ಅಗಲ && event->ಎತ್ತರ == cur_ಎತ್ತರ)
	ಹಿಂತಿರುಗಿ TRUE;

    cur_ಅಗಲ = event->ಅಗಲ;
    cur_ಎತ್ತರ = event->ಎತ್ತರ;

    ಆದರೆ (gui.surface != ಖಾಲಿ)
	cairo_surface_destroy(gui.surface);

    gui.surface = gdk_ಕಿಟಕಿ_create_similar_surface(
	    gtk_widget_get_ಕಿಟಕಿ(widget),
	    GUI_GTK_SURFACE_CONTENT,
	    event->ಅಗಲ, event->ಎತ್ತರ);

    gtk_widget_ಸರತಿ_draw(widget);

    ಹಿಂತಿರುಗಿ TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Callback routine for the "delete_event" signal on the toplevel window.
 * Tries to vim gracefully, or refuses to exit with changed buffers.
 */
    ಸ್ಥಿರ gint
delete_event_cb(GtkWidget *widget ಬಳಸದ,
		GdkEventAny *event ಬಳಸದ,
		gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui_shell_closed();
    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ defined(FEAT_MENU) || defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_ಅಂಶ_dimensions(GtkWidget *widget, GtkOrientation orientation)
{
# ifdef FEAT_GUI_GNOME
    GtkOrientation ಅಂಶ_orientation = GTK_ORIENTATION_HORIZONTAL;

    ಆದರೆ (using_gnome && widget != ಖಾಲಿ)
    {
	GtkWidget *ಪೋಷಕ;
	Bonobo_Dock_ಅಂಶ *dockitem;

	ಪೋಷಕ	 = gtk_widget_get_ಪೋಷಕ(widget);
	ಆದರೆ (G_TYPE_FROM_INSTANCE(ಪೋಷಕ) == BONOBO_TYPE_DOCK_ಅಂಶ)
	{
	    // Only menu & toolbar are dock items.  Could tabline be?
	    // Seem to be only the 2 defined in GNOME
	    widget = ಪೋಷಕ;
	    dockitem = BONOBO_DOCK_ಅಂಶ(widget);

	    ಆದರೆ (dockitem == ಖಾಲಿ || dockitem->is_floating)
		ಹಿಂತಿರುಗಿ 0;
	    ಅಂಶ_orientation = bonobo_dock_ಅಂಶ_get_orientation(dockitem);
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ
#  define ಅಂಶ_orientation GTK_ORIENTATION_HORIZONTAL
# endif

    ಆದರೆ (widget != ಖಾಲಿ
	    && ಅಂಶ_orientation == orientation
	    && gtk_widget_get_realized(widget)
	    && gtk_widget_get_visible(widget))
    {
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0) || !defined(FEAT_GUI_GNOME)
	Gtk_ಹಂಚಿಕೆ ಹಂಚಿಕೆ;

	gtk_widget_get_ಹಂಚಿಕೆ(widget, &ಹಂಚಿಕೆ);
	ಆದರೆ (ಹಂಚಿಕೆ.ಎತ್ತರ > 1)
	    ಹಿಂತಿರುಗಿ ಹಂಚಿಕೆ.ಎತ್ತರ;

	// Allocation hasn't been updated yet (widget just became visible,
	// e.g. tab bar shown asynchronously on Wayland).  Query the preferred
	// height so the caller gets a valid value before the layout pass
	// runs.  Use the maximum of minimum and natural height: GTK may
	// allocate min_h even when natural_h is smaller (e.g. GtkNotebook
	// tab bar has min_h > natural_h due to CSS).
#  ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	{
	    gint min_h = 0, natural_h = 0;

	    gtk_widget_get_preferred_ಎತ್ತರ(widget, &min_h, &natural_h);
	    ಹಿಂತಿರುಗಿ MAX(min_h, natural_h);
	}
#  ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಹಂಚಿಕೆ.ಎತ್ತರ;
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
	ಆದರೆ (orientation == GTK_ORIENTATION_HORIZONTAL)
	    ಹಿಂತಿರುಗಿ widget->ಹಂಚಿಕೆ.ಎತ್ತರ;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ widget->ಹಂಚಿಕೆ.ಅಗಲ;
# endif
    }
    ಹಿಂತಿರುಗಿ 0;
}
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_menu_tool_ಅಗಲ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಅಗಲ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME // these are never vertical without GNOME
# ifdef FEAT_MENU
    ಅಗಲ += get_ಅಂಶ_dimensions(gui.menubar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_TOOLBAR
    ಅಗಲ += get_ಅಂಶ_dimensions(gui.toolbar, GTK_ORIENTATION_VERTICAL);
# endif
# ifdef FEAT_GUI_TABLINE
    ಆದರೆ (gui.tabline != ಖಾಲಿ)
	ಅಗಲ += get_ಅಂಶ_dimensions(gui.tabline, GTK_ORIENTATION_VERTICAL);
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಅಗಲ;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_menu_tool_ಎತ್ತರ(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಎತ್ತರ += get_ಅಂಶ_dimensions(gui.menubar, GTK_ORIENTATION_HORIZONTAL);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    ಎತ್ತರ += get_ಅಂಶ_dimensions(gui.toolbar, GTK_ORIENTATION_HORIZONTAL);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    // Only include the tabline height when tabs are actually shown.  After
    // gtk_notebook_set_show_tabs(FALSE) the widget allocation is not updated
    // until the GTK main loop runs, so reading it would give a stale value.
    ಆದರೆ (gui.tabline != ಖಾಲಿ
	    && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
	ಎತ್ತರ += get_ಅಂಶ_dimensions(gui.tabline, GTK_ORIENTATION_HORIZONTAL);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಎತ್ತರ;
}

// This controls whether we can set the real window hints at
// start-up when in a GtkPlug.
// 0 = normal processing (default)
// 1 = init. hints set, no-one's tried to reset since last check
// 2 = init. hints set, attempt made to change hints
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ init_ಕಿಟಕಿ_hints_state = 0;

    ಸ್ಥಿರ ಶೂನ್ಯ
update_ಕಿಟಕಿ_manager_hints(ಪೂರ್ಣಾಂಕ force_ಅಗಲ, ಪೂರ್ಣಾಂಕ force_ಎತ್ತರ)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_ಅಗಲ  = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_ಎತ್ತರ = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_min_ಅಗಲ  = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_min_ಎತ್ತರ = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_char_ಅಗಲ  = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ old_char_ಎತ್ತರ = 0;

    ಪೂರ್ಣಾಂಕ ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ min_ಅಗಲ;
    ಪೂರ್ಣಾಂಕ min_ಎತ್ತರ;

    // At start-up, don't try to set the hints until the initial
    // values have been used (those that dictate our initial size)
    // Let forced (i.e., correct) values through always.
    ಆದರೆ (!(force_ಅಗಲ && force_ಎತ್ತರ)  &&  init_ಕಿಟಕಿ_hints_state > 0)
    {
	// Don't do it!
	init_ಕಿಟಕಿ_hints_state = 2;
	ಹಿಂತಿರುಗಿ;
    }

    // This also needs to be done when the main window isn't there yet,
    // otherwise the hints don't work.
    ಅಗಲ  = gui_get_ತಳ_ಅಗಲ();
    ಎತ್ತರ = gui_get_ತಳ_ಎತ್ತರ();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    ಎತ್ತರ += tabline_ಎತ್ತರ() * gui.char_ಎತ್ತರ;
#ಆದರೆ_ಅಂತ್ಯ
    ಅಗಲ  += get_menu_tool_ಅಗಲ();
    ಎತ್ತರ += get_menu_tool_ಎತ್ತರ();

    // GtkSockets use GtkPlug's [gui,mainwin] min-size hints to determine
    // their actual widget size.  When we set our size ourselves (e.g.,
    // 'set columns=' or init. -geom) we briefly set the min. to the size
    // we wish to be instead of the legitimate minimum so that we actually
    // resize correctly.
    ಆದರೆ (force_ಅಗಲ && force_ಎತ್ತರ)
    {
	min_ಅಗಲ  = force_ಅಗಲ;
	min_ಎತ್ತರ = force_ಎತ್ತರ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	min_ಅಗಲ  = ಅಗಲ  + MIN_COLUMNS * gui.char_ಅಗಲ;
	min_ಎತ್ತರ = ಎತ್ತರ + MIN_ಸಾಲುಗಳು   * gui.char_ಎತ್ತರ;
    }

    // Avoid an expose event when the size didn't change.
    ಆದರೆ (ಅಗಲ != old_ಅಗಲ
	    || ಎತ್ತರ != old_ಎತ್ತರ
	    || min_ಅಗಲ != old_min_ಅಗಲ
	    || min_ಎತ್ತರ != old_min_ಎತ್ತರ
	    || gui.char_ಅಗಲ != old_char_ಅಗಲ
	    || gui.char_ಎತ್ತರ != old_char_ಎತ್ತರ)
    {
	GdkGeometry	geometry;
	Gdk_ಕಿಟಕಿ_Hints	geometry_ಮುಖವಾಡ;

	geometry.ಅಗಲ_inc   = gui.char_ಅಗಲ;
	geometry.ಎತ್ತರ_inc  = gui.char_ಎತ್ತರ;
	geometry.ತಳ_ಅಗಲ  = ಅಗಲ;
	geometry.ತಳ_ಎತ್ತರ = ಎತ್ತರ;
	geometry.min_ಅಗಲ   = min_ಅಗಲ;
	geometry.min_ಎತ್ತರ  = min_ಎತ್ತರ;
	geometry_ಮುಖವಾಡ	     = GDK_HINT_ತಳ_ಗಾತ್ರ|GDK_HINT_RESIZE_INC
			       |GDK_HINT_MIN_ಗಾತ್ರ;
	// Using gui.formwin as geometry widget doesn't work as expected
	// with GTK+ 2 -- dunno why.  Presumably all the resizing hacks
	// in Vim confuse GTK+.  For GTK 3 the second argument should be NULL
	// to make the width/height inc works, despite the docs saying
	// something else.
	gtk_ಕಿಟಕಿ_ಗಣ_geometry_hints(GTK_ಕಿಟಕಿ(gui.mainwin), ಖಾಲಿ,
				      &geometry, geometry_ಮುಖವಾಡ);
	old_ಅಗಲ       = ಅಗಲ;
	old_ಎತ್ತರ      = ಎತ್ತರ;
	old_min_ಅಗಲ   = min_ಅಗಲ;
	old_min_ಎತ್ತರ  = min_ಎತ್ತರ;
	old_char_ಅಗಲ  = gui.char_ಅಗಲ;
	old_char_ಎತ್ತರ = gui.char_ಎತ್ತರ;
    }
}

#ಆದರೆ defined(FEAT_GUI_DARKTHEME)
    ಶೂನ್ಯ
gui_mch_ಗಣ_dark_theme(ಪೂರ್ಣಾಂಕ dark)
{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    GtkSettings *gtk_settings;

    gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
    g_object_ಗಣ(gtk_settings, "gtk-application-prefer-dark-theme", (gboolean)dark, ಖಾಲಿ);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_DARKTHEME

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR

/*
 * This extra effort wouldn't be necessary if we only used stock icons in the
 * toolbar, as we do for all builtin icons.  But user-defined toolbar icons
 * shouldn't be treated differently, thus we do need this.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
icon_ಗಾತ್ರ_ಬದಲಾಯಿತು_foreach(GtkWidget *widget, gpointer user_ದತ್ತಾಂಶ)
{
    ಆದರೆ (GTK_IS_IMAGE(widget))
    {
	GtkImage *image = (GtkImage *)widget;

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
	ಆದರೆ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_NAME)
	{
	    ಸ್ಥಿರಾಂಕ Gtk_Icon_ಗಾತ್ರ icon_ಗಾತ್ರ = GPOINTER_ಗಮ್ಯ_INT(user_ದತ್ತಾಂಶ);
	    ಸ್ಥಿರಾಂಕ gchar *icon_name;

	    gtk_image_get_icon_name(image, &icon_name, ಖಾಲಿ);
	    image = (GtkImage *)gtk_image_new_from_icon_name(
							 icon_name, icon_ಗಾತ್ರ);
	}
# ಇಲ್ಲದಿದ್ದರೆ
	// User-defined icons are stored in a GtkIconSet
	ಆದರೆ (gtk_image_get_storage_type(image) == GTK_IMAGE_ICON_ಗಣ)
	{
	    Gtk_Icon_ಗಣ	*icon_ಗಣ;
	    Gtk_Icon_ಗಾತ್ರ	icon_ಗಾತ್ರ;

	    gtk_image_get_icon_ಗಣ(image, &icon_ಗಣ, &icon_ಗಾತ್ರ);
	    icon_ಗಾತ್ರ = (Gtk_Icon_ಗಾತ್ರ)(ದೀರ್ಘ)user_ದತ್ತಾಂಶ;

	    gtk_icon_ಗಣ_ref(icon_ಗಣ);
	    gtk_image_ಗಣ_from_icon_ಗಣ(image, icon_ಗಣ, icon_ಗಾತ್ರ);
	    gtk_icon_ಗಣ_unref(icon_ಗಣ);
	}
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (GTK_IS_CONTAINER(widget))
    {
	gtk_container_foreach((GtkContainer *)widget,
			      &icon_ಗಾತ್ರ_ಬದಲಾಯಿತು_foreach,
			      user_ದತ್ತಾಂಶ);
    }
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_toolbar_style(GtkToolbar *toolbar)
{
    GtkToolbarStyle style;
    Gtk_Icon_ಗಾತ್ರ	    ಗಾತ್ರ_2;
    Gtk_Icon_ಗಾತ್ರ	    oldsize;

    ಆದರೆ ((toolbar_flags & (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS | TOOLBAR_HORIZ))
		      == (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS | TOOLBAR_HORIZ))
	style = GTK_TOOLBAR_BOTH_HORIZ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((toolbar_flags & (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS))
		      == (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS))
	style = GTK_TOOLBAR_BOTH;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (toolbar_flags & TOOLBAR_ಪಠ್ಯ)
	style = GTK_TOOLBAR_ಪಠ್ಯ;
    ಇಲ್ಲದಿದ್ದರೆ
	style = GTK_TOOLBAR_ICONS;

    gtk_toolbar_ಗಣ_style(toolbar, style);
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_toolbar_ಗಣ_tooltips(toolbar, (toolbar_flags & TOOLBAR_TOOLTIPS) != 0);
# endif

    ಆಯ್ಕೆ (tbis_flags)
    {
	ಸ್ಥಿತಿ TBIS_TINY:	    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_MENU;		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TBIS_SMALL:    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_SMALL_TOOLBAR;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TBIS_MEDIUM:   ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_BUTTON;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TBIS_LARGE:    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_LARGE_TOOLBAR;	ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TBIS_HUGE:     ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_DND;		ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ TBIS_GIANT:    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_DIALOG;	ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:	    ಗಾತ್ರ_2 = GTK_ICON_ಗಾತ್ರ_INVALID;	ನಿಲ್ಲಿಸು;
    }
    oldsize = gtk_toolbar_get_icon_ಗಾತ್ರ(toolbar);

    ಆದರೆ (ಗಾತ್ರ_2 == GTK_ICON_ಗಾತ್ರ_INVALID)
    {
	// Let global user preferences decide the icon size.
	gtk_toolbar_unset_icon_ಗಾತ್ರ(toolbar);
	ಗಾತ್ರ_2 = gtk_toolbar_get_icon_ಗಾತ್ರ(toolbar);
    }
    ಆದರೆ (ಗಾತ್ರ_2 != oldsize)
    {
	gtk_container_foreach(GTK_CONTAINER(toolbar),
			      &icon_ಗಾತ್ರ_ಬದಲಾಯಿತು_foreach,
			      GINT_ಗಮ್ಯ_ಸೂಚಕ((ಪೂರ್ಣಾಂಕ)ಗಾತ್ರ_2));
    }
    gtk_toolbar_ಗಣ_icon_ಗಾತ್ರ(toolbar, ಗಾತ್ರ_2);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR

#ಆದರೆ defined(FEAT_GUI_TABLINE)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ignore_tabline_evt = FALSE;
ಸ್ಥಿರ GtkWidget *tabline_menu;
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ GtkTooltips *tabline_tooltip;
# endif
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ clicked_ಪುಟ;	    // page clicked in tab line

/*
 * Handle selecting an item in the tab line popup menu.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
tabline_menu_handler(Gtk_Menu_ಅಂಶ *ಅಂಶ ಬಳಸದ, gpointer user_ದತ್ತಾಂಶ)
{
    // Add the string cmd into input buffer
    send_tabline_menu_event(clicked_ಪುಟ, (ಪೂರ್ಣಾಂಕ)(ದೀರ್ಘ)user_ದತ್ತಾಂಶ);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಕೂಡಿಸು_tabline_menu_ಅಂಶ(GtkWidget *menu, char_u *ಪಠ್ಯ, ಪೂರ್ಣಾಂಕ resp)
{
    GtkWidget	*ಅಂಶ;
    char_u	*utf_ಪಠ್ಯ;

    utf_ಪಠ್ಯ = CONVERT_ಗಮ್ಯ_UTF8(ಪಠ್ಯ);
    ಅಂಶ = gtk_menu_ಅಂಶ_new_with_label((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)utf_ಪಠ್ಯ);
    gtk_widget_show(ಅಂಶ);
    CONVERT_ಗಮ್ಯ_UTF8_FREE(utf_ಪಠ್ಯ);

    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(menu), ಅಂಶ);
    g_signal_connect(G_OBJECT(ಅಂಶ), "activate",
	    G_CALLBACK(tabline_menu_handler),
	    GINT_ಗಮ್ಯ_ಸೂಚಕ(resp));
}

/*
 * Create a menu for the tab line.
 */
    ಸ್ಥಿರ GtkWidget *
create_tabline_menu(ಶೂನ್ಯ)
{
    GtkWidget *menu;

    menu = gtk_menu_new();
    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, (char_u *)_("Close tab"), TABLINE_MENU_CLOSE);
    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, (char_u *)_("New tab"), TABLINE_MENU_NEW);
    ಕೂಡಿಸು_tabline_menu_ಅಂಶ(menu, (char_u *)_("Open Tab..."), TABLINE_MENU_OPEN);

    ಹಿಂತಿರುಗಿ menu;
}

    ಸ್ಥಿರ gboolean
on_tabline_menu(GtkWidget *widget, GdkEvent *event)
{
    // Was this button press event ?
    ಆದರೆ (event->type == GDK_BUTTON_PRESS)
    {
	GdkEventButton *bevent = (GdkEventButton *)event;
	ಪೂರ್ಣಾಂಕ		x = bevent->x;
	ಪೂರ್ಣಾಂಕ		y = bevent->y;
	GtkWidget	*tabwidget;
	Gdk_ಕಿಟಕಿ	*tabwin;

	// When ignoring events return TRUE so that the selected page doesn't
	// change.
	ಆದರೆ (hold_gui_events || cmdwin_type != 0)
	    ಹಿಂತಿರುಗಿ TRUE;

	tabwin = gui_gtk_ಕಿಟಕಿ_at_position(gui.mainwin, &x, &y);

	gdk_ಕಿಟಕಿ_get_user_ದತ್ತಾಂಶ(tabwin, (gpointer)&tabwidget);
	clicked_ಪುಟ = GPOINTER_ಗಮ್ಯ_INT(g_object_get_ದತ್ತಾಂಶ(G_OBJECT(tabwidget),
							 "tab_num"));

	// If the event was generated for 3rd button popup the menu.
	ಆದರೆ (bevent->button == 3)
	{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
#  ifdef GDK_WINDOWING_WAYLAND
	    ಆದರೆ (gui.is_wayland)
	    {
		ಪೂರ್ಣಾಂಕ x2, y2;
		gui_gtk_get_ಸೂಚಕ(gui.mainwin, &x2, &y2, ಖಾಲಿ);
		gtk_menu_popup_at_rect(GTK_MENU(widget),
			gtk_widget_get_ಕಿಟಕಿ(gui.mainwin), &(GdkRectangle){x2, y2, 1, 1},
			GDK_GRAVITY_SOUTH_WEST, GDK_GRAVITY_NORTH_WEST, ಖಾಲಿ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#  endif
	    gtk_menu_popup_at_ಸೂಚಕ(GTK_MENU(widget), event);
# ಇಲ್ಲದಿದ್ದರೆ
	    gtk_menu_popup(GTK_MENU(widget), ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ,
						bevent->button, bevent->ಸಮಯ);
# endif
	    // We handled the event.
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bevent->button == 1)
	{
	    ಆದರೆ (clicked_ಪುಟ == 0)
	    {
		// Click after all tabs moves to next tab page.  When "x" is
		// small guess it's the left button.
		send_tabline_event(x < 50 ? -1 : 0);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (bevent->button == 2)
	{
	    ಆದರೆ (clicked_ಪುಟ != 0)
		// Middle mouse click on tabpage label closes that tab.
		send_tabline_menu_event(clicked_ಪುಟ, TABLINE_MENU_CLOSE);
	}
    }

    // We didn't handle the event.
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Handle selecting one of the tabs.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_select_tab(
	GtkNotebook	*notebook ಬಳಸದ,
	gpointer	*ಪುಟ ಬಳಸದ,
	gint		idx,
	gpointer	ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (!ignore_tabline_evt)
	send_tabline_event(idx + 1);
}

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,10,0)
/*
 * Handle reordering the tabs (using D&D).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
on_tab_reordered(
	GtkNotebook	*notebook ಬಳಸದ,
	gpointer	*ಪುಟ ಬಳಸದ,
	gint		idx,
	gpointer	ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (ignore_tabline_evt)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ ((tabpage_ಸೂಚ್ಯಂಕ(curtab) - 1) < idx)
	tabpage_move(idx + 1);
    ಇಲ್ಲದಿದ್ದರೆ
	tabpage_move(idx);
}
# endif

/*
 * Show or hide the tabline.
 */
    ಶೂನ್ಯ
gui_mch_show_tabline(ಪೂರ್ಣಾಂಕ showit)
{
    ಆದರೆ (gui.tabline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (!showit != !gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline)))
    {
# ifdef TRACK_RESIZE_HISTORY
	// When the tabline visibility changes, GTK defers the formwin
	// relayout.  The resulting configure event fires while the mainwin is
	// still at its current size.  Pre-record that size so it becomes a
	// stale (discardable) entry once gui_mch_set_shellsize() records the
	// new target size.
	{
	    ಪೂರ್ಣಾಂಕ w, h;

	    gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin), &w, &h);
	    alloc_resize_hist(w, h, FALSE);
	}
# endif
	// Note: this may cause a resize event
	gtk_notebook_ಗಣ_show_tabs(GTK_NOTEBOOK(gui.tabline), showit);
	update_ಕಿಟಕಿ_manager_hints(0, 0);
	ಆದರೆ (showit)
	    gtk_widget_ಗಣ_can_focus(GTK_WIDGET(gui.tabline), FALSE);
    }

    gui_mch_update();
}

/*
 * Return TRUE when tabline is displayed.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_showing_tabline(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gui.tabline != ಖಾಲಿ
		     && gtk_notebook_get_show_tabs(GTK_NOTEBOOK(gui.tabline));
}

/*
 * Update the labels of the tabline.
 */
    ಶೂನ್ಯ
gui_mch_update_tabline(ಶೂನ್ಯ)
{
    GtkWidget	    *ಪುಟ;
    GtkWidget	    *event_box;
    GtkWidget	    *label;
    tabpage_T	    *tp;
    ಪೂರ್ಣಾಂಕ		    nr = 0;
    ಪೂರ್ಣಾಂಕ		    tab_ಅಂಕೆ;
    ಪೂರ್ಣಾಂಕ		    curtabidx = 0;
    char_u	    *labeltext;

    ಆದರೆ (gui.tabline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ignore_tabline_evt = TRUE;

    // Add a label for each tab page.  They all contain the same text area.
    ಗಾಗಿ (tp = ಮೊದಲ_tabpage; tp != ಖಾಲಿ; tp = tp->tp_ಮುಂದಿನ, ++nr)
    {
	ಆದರೆ (tp == curtab)
	    curtabidx = nr;

	tab_ಅಂಕೆ = nr + 1;

	ಪುಟ = gtk_notebook_get_nth_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr);
	ಆದರೆ (ಪುಟ == ಖಾಲಿ)
	{
	    // Add notebook page
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	    ಪುಟ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	    gtk_box_ಗಣ_homogeneous(GTK_BOX(ಪುಟ), FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
	    ಪುಟ = gtk_vbox_new(FALSE, 0);
# endif
	    gtk_widget_show(ಪುಟ);
	    event_box = gtk_event_box_new();
	    gtk_widget_show(event_box);
	    label = gtk_label_new("-Empty-");
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
	    gtk_misc_ಗಣ_padding(GTK_MISC(label), 2, 2);
# endif
	    gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(event_box), label);
	    gtk_widget_show(label);
	    gtk_notebook_ಸೇರಿಸು_ಪುಟ(GTK_NOTEBOOK(gui.tabline),
		    ಪುಟ,
		    event_box,
		    nr);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,10,0)
	    gtk_notebook_ಗಣ_tab_reorderable(GTK_NOTEBOOK(gui.tabline),
		    ಪುಟ,
		    TRUE);
# endif
	}

	event_box = gtk_notebook_get_tab_label(GTK_NOTEBOOK(gui.tabline), ಪುಟ);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(event_box), "tab_num",
						     GINT_ಗಮ್ಯ_ಸೂಚಕ(tab_ಅಂಕೆ));
	label = gtk_bin_get_ಮಗು(GTK_BIN(event_box));
	get_tabline_label(tp, FALSE);
	labeltext = CONVERT_ಗಮ್ಯ_UTF8(NameBuff);
	gtk_label_ಗಣ_ಪಠ್ಯ(GTK_LABEL(label), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)labeltext);
	CONVERT_ಗಮ್ಯ_UTF8_FREE(labeltext);

	get_tabline_label(tp, TRUE);
	labeltext = CONVERT_ಗಮ್ಯ_UTF8(NameBuff);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	gtk_widget_ಗಣ_tooltip_ಪಠ್ಯ(event_box, (ಸ್ಥಿರಾಂಕ gchar *)labeltext);
# ಇಲ್ಲದಿದ್ದರೆ
	gtk_tooltips_ಗಣ_tip(GTK_TOOLTIPS(tabline_tooltip), event_box,
			     (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)labeltext, ಖಾಲಿ);
# endif
	CONVERT_ಗಮ್ಯ_UTF8_FREE(labeltext);
    }

    // Remove any old labels.
    ಆಗ (gtk_notebook_get_nth_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr) != ಖಾಲಿ)
	gtk_notebook_remove_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr);

    ಆದರೆ (gtk_notebook_get_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline)) != curtabidx)
	gtk_notebook_ಗಣ_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline), curtabidx);

    // Make sure everything is in place before drawing text.
    gui_mch_update();

    ignore_tabline_evt = FALSE;
}

/*
 * Set the current tab to "nr".  First tab is 1.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_curtab(ಪೂರ್ಣಾಂಕ nr)
{
    ಆದರೆ (gui.tabline == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ignore_tabline_evt = TRUE;
    ಆದರೆ (gtk_notebook_get_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline)) != nr - 1)
	gtk_notebook_ಗಣ_current_ಪುಟ(GTK_NOTEBOOK(gui.tabline), nr - 1);
    ignore_tabline_evt = FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_TABLINE

/*
 * Add selection targets for PRIMARY and CLIPBOARD selections.
 */
    ಶೂನ್ಯ
gui_gtk_ಗಣ_selection_targets(GdkAtom selection)
{
    ಪೂರ್ಣಾಂಕ		    i, j = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    n_targets = N_SELECTION_TARGETS;
    ಸ್ಥಿರ Gtk_ಗುರಿ_ನಮೂದು  targets[N_SELECTION_TARGETS];

    ಆದರೆ (targets[0].ಗುರಿ == ಖಾಲಿ)
    {
	ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)N_SELECTION_TARGETS; ++i)
	{
	    // OpenOffice tries to use TARGET_HTML and fails when we don't
	    // return something, instead of trying another target. Therefore only
	    // offer TARGET_HTML when it works.
	    ಆದರೆ (!clip_html && selection_targets[i].info == ಗುರಿ_HTML)
		n_targets--;
	    ಇಲ್ಲದಿದ್ದರೆ
		targets[j++] = selection_targets[i];
	}
    }

    gtk_selection_clear_targets(gui.drawarea, selection);
    gtk_selection_ಕೂಡಿಸು_targets(gui.drawarea, selection,
			      targets, n_targets);
}

/*
 * Set up for receiving DND items.
 */
    ಶೂನ್ಯ
gui_gtk_ಗಣ_dnd_targets(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		    i, j = 0;
    ಪೂರ್ಣಾಂಕ		    n_targets = N_DND_TARGETS;
    Gtk_ಗುರಿ_ನಮೂದು  targets[N_DND_TARGETS];

    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)N_DND_TARGETS; ++i)
    {
	ಆದರೆ (!clip_html && dnd_targets[i].info == ಗುರಿ_HTML)
	    n_targets--;
	ಇಲ್ಲದಿದ್ದರೆ
	    targets[j++] = dnd_targets[i];
    }

    gtk_drag_dest_unset(gui.drawarea);
    gtk_drag_dest_ಗಣ(gui.drawarea,
		      GTK_DEST_DEFAULT_ALL,
		      targets, n_targets,
		      GDK_ACTION_COPY | GDK_ACTION_MOVE);
}

/*
 * Initialize the GUI.	Create all the windows, set up all the callbacks etc.
 * Returns OK for success, FAIL when the GUI can't be started.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init(ಶೂನ್ಯ)
{
    GtkWidget *vbox;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
    // Initialize the GNOME libraries.	gnome_program_init()/gnome_init()
    // exits on failure, but that's a non-issue because we already called
    // gtk_init_check() in gui_mch_init_check().
    ಆದರೆ (using_gnome)
    {
	gnome_ಕಾರ್ಯಕ್ರಮ_init(VIMPACKAGE, VIM_ಆವೃತ್ತಿ_SHORT,
			   LIBGNOMEUI_MODULE, gui_argc, gui_argv, ಖಾಲಿ);
# ಆದರೆ defined(LC_NUMERIC)
	{
	    ಅಕ್ಷರ *p = setlocale(LC_NUMERIC, ಖಾಲಿ);

	    // Make sure strtod() uses a decimal point, not a comma. Gnome
	    // init may change it.
	    ಆದರೆ (p == ಖಾಲಿ || strcmp(p, "C") != 0)
	       setlocale(LC_NUMERIC, "C");
	}
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ
    VIM_CLEAR(gui_argv);

#ಆದರೆ GLIB_CHECK_ಆವೃತ್ತಿ(2,1,3)
    // Set the human-readable application name
    g_ಗಣ_application_name("Vim");
#ಆದರೆ_ಅಂತ್ಯ
    /*
     * Force UTF-8 output no matter what the value of 'encoding' is.
     * did_set_string_option() in option.c prohibits changing 'termencoding'
     * to something else than UTF-8 if the GUI is in use.
     */
    ಗಣ_option_ಮೌಲ್ಯ_give_err((char_u *)"termencoding",
						     0L, (char_u *)"utf-8", 0);

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    gui_gtk_ರಿಜಿಸ್ಟರ್_stock_icons();
#ಆದರೆ_ಅಂತ್ಯ
    // FIXME: Need to install the classic icons and a gtkrc.classic file.
    // The hard part is deciding install locations and the Makefile magic.
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
# ಆದರೆ 0
    gtk_rc_parse("gtkrc");
# endif
#ಆದರೆ_ಅಂತ್ಯ

    // Initialize values
    gui.border_ಅಗಲ = 2;
    gui.scrollbar_ಅಗಲ = SB_DEFAULT_ಅಗಲ;
    gui.scrollbar_ಎತ್ತರ = SB_DEFAULT_ಅಗಲ;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gui.fgcolor = g_new(GdkRGBA, 1);
    gui.bgcolor = g_new(GdkRGBA, 1);
    gui.spcolor = g_new(GdkRGBA, 1);
#ಇಲ್ಲದಿದ್ದರೆ
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.fgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.bgcolor = g_new0(GdkColor, 1);
    // LINTED: avoid warning: conversion to 'unsigned long'
    gui.spcolor = g_new0(GdkColor, 1);
#ಆದರೆ_ಅಂತ್ಯ

    // Initialise atoms
    html_atom = gdk_atom_intern("text/html", FALSE);
    utf8_string_atom = gdk_atom_intern("UTF8_STRING", FALSE);

    // Set default foreground and background colors.
    gui.norm_pixel = gui.def_norm_pixel;
    gui.ಹಿಂಭಾಗ_pixel = gui.def_ಹಿಂಭಾಗ_pixel;

    ಆದರೆ (gtk_socket_id != 0)
    {
	GtkWidget *plug;

	// Use GtkSocket from another app.
	plug = gtk_plug_new_for_display(gdk_display_get_default(),
					gtk_socket_id);
	ಆದರೆ (plug != ಖಾಲಿ && gtk_plug_get_socket_ಕಿಟಕಿ(GTK_PLUG(plug)) != ಖಾಲಿ)
	{
	    gui.mainwin = plug;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    g_warning("Connection to GTK+ socket (ID %u) failed",
		      (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)gtk_socket_id);
	    // Pretend we never wanted it if it failed (get own window)
	    gtk_socket_id = 0;
	}
    }

    ಆದರೆ (gtk_socket_id == 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
	ಆದರೆ (using_gnome)
	{
	    gui.mainwin = gnome_app_new("Vim", ಖಾಲಿ);
# ifdef USE_XSMP
	    // Use the GNOME save-yourself functionality now.
	    xsmp_close();
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
	    gui.mainwin = gtk_ಕಿಟಕಿ_new(GTK_ಕಿಟಕಿ_TOPLEVEL);
    }

    gtk_widget_ಗಣ_name(gui.mainwin, "vim-main-window");

    // Create the PangoContext used for drawing all text.
    gui.ಪಠ್ಯ_context = gtk_widget_create_pango_context(gui.mainwin);
    pango_context_ಗಣ_ತಳ_dir(gui.ಪಠ್ಯ_context, PANGO_DIRECTION_LTR);

    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(gui.mainwin), 0);
    gtk_widget_ಕೂಡಿಸು_events(gui.mainwin, GDK_VISIBILITY_NOTIFY_ಮುಖವಾಡ);

    g_signal_connect(G_OBJECT(gui.mainwin), "delete-event",
		     G_CALLBACK(&delete_event_cb), ಖಾಲಿ);

    g_signal_connect(G_OBJECT(gui.mainwin), "realize",
		     G_CALLBACK(&mainwin_realize), ಖಾಲಿ);

    g_signal_connect(G_OBJECT(gui.mainwin), "screen-changed",
		     G_CALLBACK(&mainwin_screen_ಬದಲಾಯಿತು_cb), ಖಾಲಿ);

    g_signal_connect(G_OBJECT(gui.mainwin), "window-state-event",
		     G_CALLBACK(&mainwin_state_event_cb), ಖಾಲಿ);

    gui.accel_ಗುಂಪು = gtk_accel_ಗುಂಪು_new();
    gtk_ಕಿಟಕಿ_ಕೂಡಿಸು_accel_ಗುಂಪು(GTK_ಕಿಟಕಿ(gui.mainwin), gui.accel_ಗುಂಪು);

    // A vertical box holds the menubar, toolbar and main text window.
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_box_ಗಣ_homogeneous(GTK_BOX(vbox), FALSE);
#ಇಲ್ಲದಿದ್ದರೆ
    vbox = gtk_vbox_new(FALSE, 0);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GNOME
    ಆದರೆ (using_gnome)
    {
# ಆದರೆ defined(FEAT_MENU)
	// automagically restore menubar/toolbar placement
	gnome_app_enable_layout_config(GNOME_APP(gui.mainwin), TRUE);
# endif
	gnome_app_ಗಣ_contents(GNOME_APP(gui.mainwin), vbox);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(gui.mainwin), vbox);
	gtk_widget_show(vbox);
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MENU
    /*
     * Create the menubar and handle
     */
    gui.menubar = gtk_menu_bar_new();
    gtk_widget_ಗಣ_name(gui.menubar, "vim-menubar");

    // Avoid that GTK takes <F10> away from us.
    {
	GtkSettings *gtk_settings;

	gtk_settings = gtk_settings_get_for_screen(gdk_screen_get_default());
	g_object_ಗಣ(gtk_settings, "gtk-menu-bar-accel", ಖಾಲಿ, ಖಾಲಿ);
    }


# ifdef FEAT_GUI_GNOME
    ಆದರೆ (using_gnome)
    {
	Bonobo_Dock_ಅಂಶ *dockitem;

	gnome_app_ಗಣ_menus(GNOME_APP(gui.mainwin), GTK_MENU_BAR(gui.menubar));
	dockitem = gnome_app_get_dock_ಅಂಶ_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_MENUBAR_NAME);
	// We don't want the menu to float.
	bonobo_dock_ಅಂಶ_ಗಣ_behavior(dockitem,
		bonobo_dock_ಅಂಶ_get_behavior(dockitem)
				       | BONOBO_DOCK_ಅಂಶ_BEH_NEVER_FLOATING);
	gui.menubar_h = GTK_WIDGET(dockitem);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif	// FEAT_GUI_GNOME
    {
	// Always show the menubar, otherwise <F10> doesn't work.  It may be
	// disabled in gui_init() later.
	gtk_widget_show(gui.menubar);
	gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), gui.menubar, FALSE, FALSE, 0);
    }
#ಆದರೆ_ಅಂತ್ಯ	// FEAT_MENU

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TOOLBAR
    /*
     * Create the toolbar and handle
     */
    // some aesthetics on the toolbar
# ifdef USE_GTK3
    // TODO: Add GTK+ 3 code here using GtkCssProvider if necessary.
    // N.B.  Since the default value of GtkToolbar::button-relief is
    // GTK_RELIEF_NONE, there's no need to specify that, probably.
# ಇಲ್ಲದಿದ್ದರೆ
    gtk_rc_parse_string(
	    "style \"vim-toolbar-style\" {\n"
	    "  GtkToolbar::button_relief = GTK_RELIEF_NONE\n"
	    "}\n"
	    "widget \"*.vim-toolbar\" style \"vim-toolbar-style\"\n");
# endif
    gui.toolbar = gtk_toolbar_new();
    gtk_widget_ಗಣ_name(gui.toolbar, "vim-toolbar");
    ಗಣ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

# ifdef FEAT_GUI_GNOME
    ಆದರೆ (using_gnome)
    {
	Bonobo_Dock_ಅಂಶ *dockitem;

	gnome_app_ಗಣ_toolbar(GNOME_APP(gui.mainwin), GTK_TOOLBAR(gui.toolbar));
	dockitem = gnome_app_get_dock_ಅಂಶ_by_name(GNOME_APP(gui.mainwin),
						   GNOME_APP_TOOLBAR_NAME);
	gui.toolbar_h = GTK_WIDGET(dockitem);
	// When the toolbar is floating it gets stuck.  So long as that isn't
	// fixed let's disallow floating.
	bonobo_dock_ಅಂಶ_ಗಣ_behavior(dockitem,
		bonobo_dock_ಅಂಶ_get_behavior(dockitem)
				       | BONOBO_DOCK_ಅಂಶ_BEH_NEVER_FLOATING);
	gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(gui.toolbar), 0);
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif	// FEAT_GUI_GNOME
    {
	ಆದರೆ (vim_strchr(p_go, GO_TOOLBAR) != ಖಾಲಿ
		&& (toolbar_flags & (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS)))
	    gtk_widget_show(gui.toolbar);
	gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), gui.toolbar, FALSE, FALSE, 0);
    }
#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
    /*
     * Use a Notebook for the tab pages labels.  The labels are hidden by
     * default.
     */
    gui.tabline = gtk_notebook_new();
    gtk_widget_show(gui.tabline);
    gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), gui.tabline, FALSE, FALSE, 0);
    gtk_notebook_ಗಣ_show_border(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ಗಣ_show_tabs(GTK_NOTEBOOK(gui.tabline), FALSE);
    gtk_notebook_ಗಣ_scrollable(GTK_NOTEBOOK(gui.tabline), TRUE);
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_notebook_ಗಣ_tab_border(GTK_NOTEBOOK(gui.tabline), FALSE);
# endif

# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    tabline_tooltip = gtk_tooltips_new();
    gtk_tooltips_enable(GTK_TOOLTIPS(tabline_tooltip));
# endif

    {
	GtkWidget *ಪುಟ, *label, *event_box;

	// Add the first tab.
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	ಪುಟ = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
	gtk_box_ಗಣ_homogeneous(GTK_BOX(ಪುಟ), FALSE);
# ಇಲ್ಲದಿದ್ದರೆ
	ಪುಟ = gtk_vbox_new(FALSE, 0);
# endif
	gtk_widget_show(ಪುಟ);
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(gui.tabline), ಪುಟ);
	label = gtk_label_new("-Empty-");
	gtk_widget_show(label);
	event_box = gtk_event_box_new();
	gtk_widget_show(event_box);
	g_object_ಗಣ_ದತ್ತಾಂಶ(G_OBJECT(event_box), "tab_num", GINT_ಗಮ್ಯ_ಸೂಚಕ(1L));
# ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,14,0)
	gtk_misc_ಗಣ_padding(GTK_MISC(label), 2, 2);
# endif
	gtk_container_ಕೂಡಿಸು(GTK_CONTAINER(event_box), label);
	gtk_notebook_ಗಣ_tab_label(GTK_NOTEBOOK(gui.tabline), ಪುಟ, event_box);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,10,0)
	gtk_notebook_ಗಣ_tab_reorderable(GTK_NOTEBOOK(gui.tabline), ಪುಟ, TRUE);
# endif
    }

    g_signal_connect(G_OBJECT(gui.tabline), "switch-page",
		     G_CALLBACK(on_select_tab), ಖಾಲಿ);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(2,10,0)
    g_signal_connect(G_OBJECT(gui.tabline), "page-reordered",
		     G_CALLBACK(on_tab_reordered), ಖಾಲಿ);
# endif

    // Create a popup menu for the tab line and connect it.
    tabline_menu = create_tabline_menu();
    g_signal_connect_swapped(G_OBJECT(gui.tabline), "button-press-event",
	    G_CALLBACK(on_tabline_menu), G_OBJECT(tabline_menu));
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_TABLINE

    gui.formwin = gui_gtk_form_new();
    gtk_container_ಗಣ_border_ಅಗಲ(GTK_CONTAINER(gui.formwin), 0);
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gtk_widget_ಗಣ_events(gui.formwin, GDK_EXPOSURE_ಮುಖವಾಡ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    gtk_widget_ಗಣ_name(gui.formwin, "vim-gtk-form");
#ಆದರೆ_ಅಂತ್ಯ

    gui.drawarea = gtk_drawing_area_new();
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gui.surface = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GDK_WINDOWING_WAYLAND
    GdkDisplay *d = gdk_display_get_default();
    ಆದರೆ (GDK_IS_WAYLAND_DISPLAY(d))
	gui.is_wayland = true;
#ಆದರೆ_ಅಂತ್ಯ

    // Determine which events we will filter.
    gint event_ಮುಖವಾಡ =
	GDK_EXPOSURE_ಮುಖವಾಡ |
	GDK_ENTER_NOTIFY_ಮುಖವಾಡ |
	GDK_LEAVE_NOTIFY_ಮುಖವಾಡ |
	GDK_BUTTON_PRESS_ಮುಖವಾಡ |
	GDK_BUTTON_RELEASE_ಮುಖವಾಡ |
	GDK_ಕೀಲಿ_PRESS_ಮುಖವಾಡ |
	GDK_ಕೀಲಿ_RELEASE_ಮುಖವಾಡ |
	GDK_ಸೂಚಕ_MOTION_ಮುಖವಾಡ |
	GDK_ಸೂಚಕ_MOTION_HINT_ಮುಖವಾಡ;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0)
    ಆದರೆ (GDK_IS_X11_DISPLAY(gdk_display_get_default()))
    {
	gui.is_x11 = true;
	// for X11, if we were using smooth scroll events, we
	// would get an scroll without deltas on the very first user scroll* and
	// get both "unsmooth" scroll and smooth scroll events after
	// copying into the primary selection
	//
	// *: https://bugzilla.gnome.org/show_bug.cgi?id=675959
	event_ಮುಖವಾಡ |= GDK_SCROLL_ಮುಖವಾಡ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// for Wayland, touchpads don't generate "unsmooth" scroll events. Both
	// touchpads and wheels generate smooth scroll events expectedly.
	event_ಮುಖವಾಡ |= GDK_SMOOTH_SCROLL_ಮುಖವಾಡ;
    }
#ಇಲ್ಲದಿದ್ದರೆ
    event_ಮುಖವಾಡ |= GDK_SCROLL_ಮುಖವಾಡ;
#ಆದರೆ_ಅಂತ್ಯ
    gtk_widget_ಗಣ_events(gui.drawarea, event_ಮುಖವಾಡ);

    gtk_widget_show(gui.drawarea);
    gui_gtk_form_put(GTK_FORM(gui.formwin), gui.drawarea, 0, 0);
    gtk_widget_show(gui.formwin);
    gtk_box_pack_ಆರಂಭ(GTK_BOX(vbox), gui.formwin, TRUE, TRUE, 0);

    // For GtkSockets, key-presses must go to the focus widget (drawarea)
    // and not the window.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		       "key-press-event",
		       G_CALLBACK(ಕೀಲಿ_press_event), ಖಾಲಿ);
#ಆದರೆ defined(FEAT_XIM) || GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    // Also forward key release events for the benefit of GTK+ 2 input
    // modules.  Try CTRL-SHIFT-xdigits to enter a Unicode code point.
    g_signal_connect((gtk_socket_id == 0) ? G_OBJECT(gui.mainwin)
					  : G_OBJECT(gui.drawarea),
		     "key-release-event",
		     G_CALLBACK(&ಕೀಲಿ_release_event), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    g_signal_connect(G_OBJECT(gui.drawarea), "realize",
		     G_CALLBACK(drawarea_realize_cb), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "unrealize",
		     G_CALLBACK(drawarea_unrealize_cb), ಖಾಲಿ);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "configure-event",
	    G_CALLBACK(drawarea_configure_event_cb), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-updated",
			   G_CALLBACK(&drawarea_style_updated_cb), ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    g_signal_connect_after(G_OBJECT(gui.drawarea), "style-set",
			   G_CALLBACK(&drawarea_style_ಗಣ_cb), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    gui.visibility = GDK_VISIBILITY_UNOBSCURED;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(USE_GNOME_SESSION)
    wm_protocols_atom = gdk_atom_intern("WM_PROTOCOLS", FALSE);
    save_yourself_atom = gdk_atom_intern("WM_SAVE_YOURSELF", FALSE);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (gtk_socket_id != 0)
	// make sure keyboard input can go to the drawarea
	gtk_widget_ಗಣ_can_focus(gui.drawarea, TRUE);

    /*
     * Set clipboard specific atoms
     */
    vim_atom = gdk_atom_intern(VIM_ATOM_NAME, FALSE);
    vimenc_atom = gdk_atom_intern(VIMENC_ATOM_NAME, FALSE);
    vim_mt_atom = gdk_atom_intern(VIM_MIMETYPE_NAME, FALSE);
    vimenc_mt_atom = gdk_atom_intern(VIMENC_MIMETYPE_NAME, FALSE);
    clip_ನಕ್ಷತ್ರ.gtk_sel_atom = GDK_SELECTION_PRIMARY;
    clip_plus.gtk_sel_atom = gdk_atom_intern("CLIPBOARD", FALSE);

    /*
     * Start out by adding the configured border width into the border offset.
     */
    gui.border_ಸ್ಥಾನಾಂತರ = gui.border_ಅಗಲ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    g_signal_connect(G_OBJECT(gui.drawarea), "draw",
		     G_CALLBACK(draw_event), ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    gtk_signal_connect(GTK_OBJECT(gui.mainwin), "visibility_notify_event",
		       GTK_SIGNAL_FUNC(visibility_event), ಖಾಲಿ);
    gtk_signal_connect(GTK_OBJECT(gui.drawarea), "expose_event",
		       GTK_SIGNAL_FUNC(expose_event), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Only install these enter/leave callbacks when 'p' in 'guioptions'.
     * Only needed for some window managers.
     */
    ಆದರೆ (vim_strchr(p_go, GO_ಸೂಚಕ) != ಖಾಲಿ)
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "leave-notify-event",
			 G_CALLBACK(leave_notify_event), ಖಾಲಿ);
	g_signal_connect(G_OBJECT(gui.drawarea), "enter-notify-event",
			 G_CALLBACK(enter_notify_event), ಖಾಲಿ);
    }

    // Real windows can get focus ... GtkPlug, being a mere container can't,
    // only its widgets.  Arguably, this could be common code and we do not
    // use the window focus at all, but let's be safe.
    ಆದರೆ (gtk_socket_id == 0)
    {
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-out-event",
			 G_CALLBACK(focus_ಹೊರಹೋಗು_event), ಖಾಲಿ);
	g_signal_connect(G_OBJECT(gui.mainwin), "focus-in-event",
			 G_CALLBACK(focus_ಒಳ_event), ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-out-event",
			 G_CALLBACK(focus_ಹೊರಹೋಗು_event), ಖಾಲಿ);
	g_signal_connect(G_OBJECT(gui.drawarea), "focus-in-event",
			 G_CALLBACK(focus_ಒಳ_event), ಖಾಲಿ);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_TABLINE
	g_signal_connect(G_OBJECT(gui.tabline), "focus-out-event",
			 G_CALLBACK(focus_ಹೊರಹೋಗು_event), ಖಾಲಿ);
	g_signal_connect(G_OBJECT(gui.tabline), "focus-in-event",
			 G_CALLBACK(focus_ಒಳ_event), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI_TABLINE
    }

    g_signal_connect(G_OBJECT(gui.drawarea), "motion-notify-event",
		     G_CALLBACK(motion_notify_event), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-press-event",
		     G_CALLBACK(button_press_event), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "button-release-event",
		     G_CALLBACK(button_release_event), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "grab-notify",
		     G_CALLBACK(grab_notify_event), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "scroll-event",
		     G_CALLBACK(scroll_event), ಖಾಲಿ);

    // Pretend we don't have input focus, we will get an event if we do.
    gui.ಒಳ_focus = false;

    // Handle changes to the "Xft/DPI" setting.
    {
	GtkSettings *gtk_settings =
			 gtk_settings_get_for_screen(gdk_screen_get_default());

	g_signal_connect(gtk_settings, "notify::gtk-xft-dpi",
			   G_CALLBACK(gtk_settings_xft_dpi_ಬದಲಾಯಿತು_cb), ಖಾಲಿ);
    }

#ಆದರೆ defined(FEAT_IMAGE) && GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    gui.scale = gtk_widget_get_scale_ಗುಣಕ(gui.formwin);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ defined(USE_GNOME_SESSION)
/*
 * This is called from gui_start() after a fork() has been done.
 * We have to tell the session manager our new PID.
 */
    ಶೂನ್ಯ
gui_mch_forked(ಶೂನ್ಯ)
{
    ಆದರೆ (!using_gnome)
	ಹಿಂತಿರುಗಿ;

    GnomeClient *client;

    client = gnome_master_client();

    ಆದರೆ (client != ಖಾಲಿ)
	gnome_client_ಗಣ_process_id(client, getpid());
}
#ಆದರೆ_ಅಂತ್ಯ // USE_GNOME_SESSION

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ GdkRGBA
color_ಗಮ್ಯ_rgba(guicolor_T color)
{
    GdkRGBA rgba;
    rgba.red   = ((color & 0xff0000) >> 16) / 255.0;
    rgba.green = ((color & 0xff00) >> 8) / 255.0;
    rgba.blue  = ((color & 0xff)) / 255.0;
    rgba.alpha = 1.0;
    ಹಿಂತಿರುಗಿ rgba;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_cairo_ಮೂಲ_rgba_from_color(cairo_t *cr, guicolor_T color)
{
    ಸ್ಥಿರಾಂಕ GdkRGBA rgba = color_ಗಮ್ಯ_rgba(color);
    cairo_ಗಣ_ಮೂಲ_rgba(cr, rgba.red, rgba.green, rgba.blue, rgba.alpha);
}
#ಆದರೆ_ಅಂತ್ಯ // GTK_CHECK_VERSION(3,0,0)

/*
 * Called when the foreground or background color has been changed.
 * This used to change the graphics contexts directly but we are
 * currently manipulating them where desired.
 */
    ಶೂನ್ಯ
gui_mch_new_colors(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.drawarea != ಖಾಲಿ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
	    && gui.formwin != ಖಾಲಿ
#ಆದರೆ_ಅಂತ್ಯ
	    && gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) != ಖಾಲಿ)
    {
#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
	Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ da_win = gtk_widget_get_ಕಿಟಕಿ(gui.drawarea);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
	GtkStyleContext * ಸ್ಥಿರಾಂಕ context =
				     gtk_widget_get_style_context(gui.formwin);
	GtkCssProvider * ಸ್ಥಿರಾಂಕ provider = gtk_css_provider_new();
	gchar * ಸ್ಥಿರಾಂಕ css = g_strdup_printf(
		"widget#vim-gtk-form {\n"
		"  background-color: #%.2lx%.2lx%.2lx;\n"
		"}\n",
		 (gui.ಹಿಂಭಾಗ_pixel >> 16) & 0xff,
		 (gui.ಹಿಂಭಾಗ_pixel >> 8) & 0xff,
		 gui.ಹಿಂಭಾಗ_pixel & 0xff);

	gtk_css_provider_ತುಂಬು_from_ದತ್ತಾಂಶ(provider, css, -1, ಖಾಲಿ);
	gtk_style_context_ಕೂಡಿಸು_provider(context,
		GTK_STYLE_PROVIDER(provider), G_MAXUINT);

	g_free(css);
	g_object_unref(provider);
#ಅಥವಾಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,4,0) // !GTK_CHECK_VERSION(3,22,2)
	GdkRGBA rgba;

	rgba = color_ಗಮ್ಯ_rgba(gui.ಹಿಂಭಾಗ_pixel);
	{
	    cairo_ನಮೂನೆ_t * ಸ್ಥಿರಾಂಕ pat = cairo_ನಮೂನೆ_create_rgba(
		    rgba.red, rgba.green, rgba.blue, rgba.alpha);
	    ಆದರೆ (pat != ಖಾಲಿ)
	    {
		gdk_ಕಿಟಕಿ_ಗಣ_background_ನಮೂನೆ(da_win, pat);
		cairo_ನಮೂನೆ_destroy(pat);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		gdk_ಕಿಟಕಿ_ಗಣ_background_rgba(da_win, &rgba);
	}
#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,4,0)
	GdkColor color = { 0, 0, 0, 0 };

	color.pixel = gui.ಹಿಂಭಾಗ_pixel;
	gdk_ಕಿಟಕಿ_ಗಣ_background(da_win, &color);
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,22,2)
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
    ಸ್ಥಿರ gboolean
startup_resize_correction_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    gui_ಗಣ_shellsize(FALSE, FALSE, RESIZE_BOTH);
    ಹಿಂತಿರುಗಿ FALSE;  // one-shot
}

/*
 * This signal informs us about the need to rearrange our sub-widgets.
 */
    ಸ್ಥಿರ gint
form_configure_event(GtkWidget *widget ಬಳಸದ,
		     GdkEventConfigure *event,
		     gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    usable_ಎತ್ತರ = event->ಎತ್ತರ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_RESIZE_HISTORY
    // Resize requests are made for gui.mainwin;
    // get its dimensions for searching if this event
    // is a response to a vim request.
    ಪೂರ್ಣಾಂಕ	    w, h;
    gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin), &w, &h);

# ifdef ENABLE_RESIZE_HISTORY_LOG
    ch_log(ಖಾಲಿ, "gui_gtk: form_configure_event: (%d, %d) [%d, %d]",
	   w, h, (ಪೂರ್ಣಾಂಕ)Columns, (ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು);
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

    ಆದರೆ (!ಹೊಂದಿಕೆ_ಅಗಲ_ಎತ್ತರ(latest_resize_hist, w, h)
					     && ಹೊಂದಿಕೆ_stale_ಅಗಲ_ಎತ್ತರ(w, h))
	// discard stale event
	ಹಿಂತಿರುಗಿ TRUE;

    // On Wayland (GTK3 CSD), gtk_window_resize(w, req_h) results in
    // gtk_window_get_size() returning (req_w - csd_w, req_h - csd_h).
    // Compute the offsets once from the first confirmed resize response.
    ಆದರೆ ((mch_csd_ಎತ್ತರ == 0 || mch_csd_ಅಗಲ == 0)
	    && latest_resize_hist != ಖಾಲಿ
	    && !latest_resize_hist->ಬಳಸಿದ
	    && latest_resize_hist->from_shellsize
	    && gui.char_ಎತ್ತರ > 0)
    {
	ಪೂರ್ಣಾಂಕ pot_csd_w = latest_resize_hist->ಅಗಲ - w;
	ಪೂರ್ಣಾಂಕ pot_csd_h = latest_resize_hist->ಎತ್ತರ - h;

	ಆದರೆ (pot_csd_w > 0 && pot_csd_w < gui.char_ಅಗಲ)
	{
	    mch_csd_ಅಗಲ = pot_csd_w;
	    // The resize that triggered this startup event was issued without
	    // CSD compensation; retroactively set the expected form width so
	    // that the clamp below corrects Columns for this same event.
	    mch_pending_form_w = (ಪೂರ್ಣಾಂಕ)Columns * gui.char_ಅಗಲ
						    + gui_get_ತಳ_ಅಗಲ();
	}
	ಆದರೆ (pot_csd_h > 0 && pot_csd_h < gui.char_ಎತ್ತರ)
	{
	    mch_csd_ಎತ್ತರ = pot_csd_h;
	    // Similarly, correct the form height for this event so that Rows
	    // is computed correctly despite the missing CSD compensation.
	    usable_ಎತ್ತರ += mch_csd_ಎತ್ತರ;
	}
	// The window was resized without CSD compensation and is physically
	// too small.  Schedule a corrective resize (now that offsets are
	// known) so the window actually fits the geometry Vim has just set.
	ಆದರೆ ((mch_csd_ಎತ್ತರ > 0 || mch_csd_ಅಗಲ > 0) && gtk_socket_id == 0)
	    g_idle_ಕೂಡಿಸು_ತುಂಬಿದ(GTK_ಆದ್ಯತೆ_RESIZE, startup_resize_correction_cb, ಖಾಲಿ, ಖಾಲಿ);
    }
    clear_resize_hists();
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2) && !GTK_CHECK_ಆವೃತ್ತಿ(3,22,4)
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
    ಆದರೆ (event->ಕಿಟಕಿ_2 != gtk_widget_get_ಕಿಟಕಿ(gui.formwin))
	ಹಿಂತಿರುಗಿ TRUE;
#ಆದರೆ_ಅಂತ್ಯ

    // When in a GtkPlug, we can't guarantee valid heights (as a round
    // no. of char-heights), so we have to manually sanitise them.
    // Widths seem to sort themselves out, don't ask me why.
    ಆದರೆ (gtk_socket_id != 0)
	usable_ಎತ್ತರ -= (gui.char_ಎತ್ತರ - (gui.char_ಎತ್ತರ/2)); // sic.

    // If the configure event delivers a form width that is slightly less than
    // the width we intended (mch_pending_form_w), the difference is within
    // one char_width and is due to CSD under-compensation.  Clamp to the
    // intended width so that column count does not drift downward.
    {
	ಪೂರ್ಣಾಂಕ use_ಅಗಲ = event->ಅಗಲ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_RESIZE_HISTORY
	ಆದರೆ (mch_pending_form_w > event->ಅಗಲ
		&& mch_pending_form_w - event->ಅಗಲ < gui.char_ಅಗಲ)
	    use_ಅಗಲ = mch_pending_form_w;
#ಆದರೆ_ಅಂತ್ಯ
	gui_gtk_form_freeze(GTK_FORM(gui.formwin));
	gui_resize_shell(use_ಅಗಲ, usable_ಎತ್ತರ);
	gui_gtk_form_thaw(GTK_FORM(gui.formwin));
    }

    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Function called when window already closed.
 * We can't do much more here than to trying to preserve what had been done,
 * since the window is already inevitably going away.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mainwin_destroy_cb(GObject *object ಬಳಸದ, gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Don't write messages to the GUI anymore
    ತುಂಬಿದ_screen = FALSE;

    gui.mainwin  = ಖಾಲಿ;
    gui.drawarea = ಖಾಲಿ;

    ಆದರೆ (!exiting) // only do anything if the destroy was unexpected
    {
	vim_strncpy(IObuff,
		(char_u *)_("Vim: Main window unexpectedly destroyed\n"),
		IOSIZE - 1);
	preserve_exit();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GRESOURCE
    gui_gtk_unregister_resource();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
gui_gtk_get_screen_geom_ದ_win(
	GtkWidget *wid,
	ಪೂರ್ಣಾಂಕ ಬಿಂದು_x,	    // x position of window if not initialized
	ಪೂರ್ಣಾಂಕ ಬಿಂದು_y,	    // y position of window if not initialized
	ಪೂರ್ಣಾಂಕ *screen_x,
	ಪೂರ್ಣಾಂಕ *screen_y,
	ಪೂರ್ಣಾಂಕ *ಅಗಲ,
	ಪೂರ್ಣಾಂಕ *ಎತ್ತರ)
{
    GdkRectangle geometry;
    Gdk_ಕಿಟಕಿ *win = gtk_widget_get_ಕಿಟಕಿ(wid);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,0)
    GdkDisplay *dpy;
    GdkMonitor *monitor;

    ಆದರೆ (wid != ಖಾಲಿ && gtk_widget_get_realized(wid))
	dpy = gtk_widget_get_display(wid);
    ಇಲ್ಲದಿದ್ದರೆ
	dpy = gdk_display_get_default();
    ಆದರೆ (win != ಖಾಲಿ)
	monitor = gdk_display_get_monitor_at_ಕಿಟಕಿ(dpy, win);
    ಇಲ್ಲದಿದ್ದರೆ
	monitor = gdk_display_get_monitor_at_ಬಿಂದು(dpy, ಬಿಂದು_x, ಬಿಂದು_y);
    gdk_monitor_get_geometry(monitor, &geometry);
#ಇಲ್ಲದಿದ್ದರೆ
    GdkScreen* screen;
    ಪೂರ್ಣಾಂಕ monitor;

    ಆದರೆ (wid != ಖಾಲಿ && gtk_widget_has_screen(wid))
	screen = gtk_widget_get_screen(wid);
    ಇಲ್ಲದಿದ್ದರೆ
	screen = gdk_screen_get_default();
    ಆದರೆ (win != ಖಾಲಿ)
	monitor = gdk_screen_get_monitor_at_ಕಿಟಕಿ(screen, win);
    ಇಲ್ಲದಿದ್ದರೆ
	monitor = gdk_screen_get_monitor_at_ಬಿಂದು(screen, ಬಿಂದು_x, ಬಿಂದು_y);
    gdk_screen_get_monitor_geometry(screen, monitor, &geometry);
#ಆದರೆ_ಅಂತ್ಯ
    *screen_x = geometry.x;
    *screen_y = geometry.y;
    *ಅಗಲ = geometry.ಅಗಲ;
    *ಎತ್ತರ = geometry.ಎತ್ತರ;
}

/*
 * The screen size is used to make sure the initial window doesn't get bigger
 * than the screen.  This subtracts some room for menubar, toolbar and window
 * decorations.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_get_screen_dimensions(
	ಪೂರ್ಣಾಂಕ ಬಿಂದು_x,
	ಪೂರ್ಣಾಂಕ ಬಿಂದು_y,
	ಪೂರ್ಣಾಂಕ *screen_w,
	ಪೂರ್ಣಾಂಕ *screen_h)
{
    ಪೂರ್ಣಾಂಕ	    x, y;

    gui_gtk_get_screen_geom_ದ_win(gui.mainwin, ಬಿಂದು_x, ಬಿಂದು_y,
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
    *screen_w -= get_menu_tool_ಅಗಲ();
    *screen_h -= get_menu_tool_ಎತ್ತರ();
}

    ಶೂನ್ಯ
gui_mch_get_screen_dimensions(ಪೂರ್ಣಾಂಕ *screen_w, ಪೂರ್ಣಾಂಕ *screen_h)
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
    ಸ್ಥಿರ timeout_cb_type
check_startup_plug_hints(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಆದರೆ (init_ಕಿಟಕಿ_hints_state == 1)
    {
	// Safe to use normal hints now
	init_ಕಿಟಕಿ_hints_state = 0;
	update_ಕಿಟಕಿ_manager_hints(0, 0);
	ಹಿಂತಿರುಗಿ FALSE;   // stop timer
    }

    // Keep on trying
    init_ಕಿಟಕಿ_hints_state = 1;
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * Open the GUI window which was created by a call to gui_mch_init().
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_open(ಶೂನ್ಯ)
{
    guicolor_T fg_pixel = INVALCOLOR;
    guicolor_T bg_pixel = INVALCOLOR;
    guint		pixel_ಅಗಲ;
    guint		pixel_ಎತ್ತರ;

    /*
     * Allow setting a window role on the command line, or invent one
     * if none was specified.  This is mainly useful for GNOME session
     * support; allowing the WM to restore window placement.
     */
    ಆದರೆ (role_ವಾದ != ಖಾಲಿ)
    {
	gtk_ಕಿಟಕಿ_ಗಣ_role(GTK_ಕಿಟಕಿ(gui.mainwin), role_ವಾದ);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಅಕ್ಷರ *role;

	// Invent a unique-enough ID string for the role
	role = g_strdup_printf("vim-%u-%u-%u",
			       (ಚಿಹ್ನೆರಹಿತ)mch_get_pid(),
			       (ಚಿಹ್ನೆರಹಿತ)g_random_int(),
			       (ಚಿಹ್ನೆರಹಿತ)ಸಮಯ(ಖಾಲಿ));

	gtk_ಕಿಟಕಿ_ಗಣ_role(GTK_ಕಿಟಕಿ(gui.mainwin), role);
	g_free(role);
    }

    ಆದರೆ (gui_win_x != -1 && gui_win_y != -1)
	gtk_ಕಿಟಕಿ_move(GTK_ಕಿಟಕಿ(gui.mainwin), gui_win_x, gui_win_y);

    // Determine user specified geometry, if present.
    ಆದರೆ (gui.geom != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ		ಮುಖವಾಡ;
	ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ	w, h;
	ಪೂರ್ಣಾಂಕ		x = 0;
	ಪೂರ್ಣಾಂಕ		y = 0;

	ಮುಖವಾಡ = XParseGeometry((ಅಕ್ಷರ *)gui.geom, &x, &y, &w, &h);

	ಆದರೆ (ಮುಖವಾಡ & ಅಗಲ_ಮೌಲ್ಯ)
	    Columns = w;
	ಆದರೆ (ಮುಖವಾಡ & ಎತ್ತರ_ಮೌಲ್ಯ)
	{
	    ಆದರೆ (p_ಕಿಟಕಿ > (ದೀರ್ಘ)h - 1 || !option_was_ಗಣ((char_u *)"window"))
		p_ಕಿಟಕಿ = h - 1;
	    ಅಡ್ಡಸಾಲುಗಳು = h;
	}
	ಎಲ್ಲೆ_screen_ಗಾತ್ರ();

	pixel_ಅಗಲ = (guint)(gui_get_ತಳ_ಅಗಲ() + Columns * gui.char_ಅಗಲ);
	pixel_ಎತ್ತರ = (guint)(gui_get_ತಳ_ಎತ್ತರ() + ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ);

	pixel_ಅಗಲ  += get_menu_tool_ಅಗಲ();
	pixel_ಎತ್ತರ += get_menu_tool_ಎತ್ತರ();

	ಆದರೆ (ಮುಖವಾಡ & (XValue | YValue))
	{
	    ಪೂರ್ಣಾಂಕ ww, hh;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI_GTK
	    gui_gtk_get_screen_dimensions(x, y, &ww, &hh);
#ಇಲ್ಲದಿದ್ದರೆ
	    gui_mch_get_screen_dimensions(&ww, &hh);
#ಆದರೆ_ಅಂತ್ಯ
	    hh += p_ghr + get_menu_tool_ಎತ್ತರ();
	    ww += get_menu_tool_ಅಗಲ();
	    ಆದರೆ (ಮುಖವಾಡ & XNegative)
		x += ww - pixel_ಅಗಲ;
	    ಆದರೆ (ಮುಖವಾಡ & YNegative)
		y += hh - pixel_ಎತ್ತರ;
	    gtk_ಕಿಟಕಿ_move(GTK_ಕಿಟಕಿ(gui.mainwin), x, y);
	}
	VIM_CLEAR(gui.geom);

	// From now until everyone's stopped trying to set the window hints
	// to their correct minimum values, stop them being set as we need
	// them to remain at our required size for the parent GtkSocket to
	// give us the right initial size.
	ಆದರೆ (gtk_socket_id != 0  &&  (ಮುಖವಾಡ & ಅಗಲ_ಮೌಲ್ಯ || ಮುಖವಾಡ & ಎತ್ತರ_ಮೌಲ್ಯ))
	{
	    update_ಕಿಟಕಿ_manager_hints(pixel_ಅಗಲ, pixel_ಎತ್ತರ);
	    init_ಕಿಟಕಿ_hints_state = 1;
	    timeout_ಕೂಡಿಸು(1000, check_startup_plug_hints, ಖಾಲಿ);
	}
    }

    pixel_ಅಗಲ = (guint)(gui_get_ತಳ_ಅಗಲ() + Columns * gui.char_ಅಗಲ);
    pixel_ಎತ್ತರ = (guint)(gui_get_ತಳ_ಎತ್ತರ() + ಅಡ್ಡಸಾಲುಗಳು * gui.char_ಎತ್ತರ);

    // For GTK2 changing the size of the form widget doesn't cause window
    // resizing.
    ಆದರೆ (gtk_socket_id == 0)
	gtk_ಕಿಟಕಿ_resize(GTK_ಕಿಟಕಿ(gui.mainwin), pixel_ಅಗಲ, pixel_ಎತ್ತರ);
    update_ಕಿಟಕಿ_manager_hints(0, 0);

    ಆದರೆ (foreground_ವಾದ != ಖಾಲಿ)
	fg_pixel = gui_get_color((char_u *)foreground_ವಾದ);
    ಆದರೆ (fg_pixel == INVALCOLOR)
	fg_pixel = gui_get_color((char_u *)"Black");

    ಆದರೆ (background_ವಾದ != ಖಾಲಿ)
	bg_pixel = gui_get_color((char_u *)background_ವಾದ);
    ಆದರೆ (bg_pixel == INVALCOLOR)
	bg_pixel = gui_get_color((char_u *)"White");

    ಆದರೆ (ಸಿಕ್ಕಿತು_ಹಿಮ್ಮುಖ_arg)
    {
	gui.def_norm_pixel = bg_pixel;
	gui.def_ಹಿಂಭಾಗ_pixel = fg_pixel;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	gui.def_norm_pixel = fg_pixel;
	gui.def_ಹಿಂಭಾಗ_pixel = bg_pixel;
    }

    // Get the colors from the "Normal" and "Menu" group (set in syntax.c or
    // in a vimrc file)
    ಗಣ_ಸಾಮಾನ್ಯ_colors();

    // Check that none of the colors are the same as the background color
    gui_check_colors();

    // Get the colors for the highlight groups (gui_check_colors() might have
    // changed them).
    highlight_gui_started();	// re-init colors and fonts

    g_signal_connect(G_OBJECT(gui.mainwin), "destroy",
		     G_CALLBACK(mainwin_destroy_cb), ಖಾಲಿ);

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
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_RESIZE_HISTORY
    latest_resize_hist = ALLOC_CLEAR_ONE(resize_hist_T);
#ಆದರೆ_ಅಂತ್ಯ
    g_signal_connect(G_OBJECT(gui.formwin), "configure-event",
				       G_CALLBACK(form_configure_event), ಖಾಲಿ);
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,10,0)
    g_signal_connect(G_OBJECT(gui.formwin), "notify::scale-factor",
		     G_CALLBACK(scale_ಗುಣಕ_event), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DND
    // Set up for receiving DND items.
    gui_gtk_ಗಣ_dnd_targets();

    g_signal_connect(G_OBJECT(gui.drawarea), "drag-data-received",
				      G_CALLBACK(drag_ದತ್ತಾಂಶ_received_cb), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

	// With GTK+ 2, we need to iconify the window before calling show()
	// to avoid mapping the window for a short time.
	ಆದರೆ (ಸಿಕ್ಕಿತು_iconic_arg && gtk_socket_id == 0)
	    gui_mch_iconify();

    {
#ಆದರೆ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ menu_handler = 0;
# ifdef FEAT_TOOLBAR
	ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ tool_handler = 0;
# endif
	/*
	 * Urgh hackish :/  For some reason BonoboDockLayout always forces a
	 * show when restoring the saved layout configuration.	We can't just
	 * hide the widgets again after gtk_widget_show(gui.mainwin) since it's
	 * a toplevel window and thus will be realized immediately.  Instead,
	 * connect signal handlers to hide the widgets just after they've been
	 * marked visible, but before the main window is realized.
	 */
	ಆದರೆ (using_gnome && vim_strchr(p_go, GO_MENUS) == ಖಾಲಿ)
	    menu_handler = g_signal_connect_after(gui.menubar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ಖಾಲಿ);
# ifdef FEAT_TOOLBAR
	ಆದರೆ (using_gnome && vim_strchr(p_go, GO_TOOLBAR) == ಖಾಲಿ
		&& (toolbar_flags & (TOOLBAR_ಪಠ್ಯ | TOOLBAR_ICONS)))
	    tool_handler = g_signal_connect_after(gui.toolbar_h, "show",
						  G_CALLBACK(&gtk_widget_hide),
						  ಖಾಲಿ);
# endif
#ಆದರೆ_ಅಂತ್ಯ
	gtk_widget_show(gui.mainwin);

#ಆದರೆ defined(FEAT_GUI_GNOME) && defined(FEAT_MENU)
	ಆದರೆ (menu_handler != 0)
	    g_signal_handler_disconnect(gui.menubar_h, menu_handler);
# ifdef FEAT_TOOLBAR
	ಆದರೆ (tool_handler != 0)
	    g_signal_handler_disconnect(gui.toolbar_h, tool_handler);
# endif
#ಆದರೆ_ಅಂತ್ಯ
    }

    /*
     * Add selection handler functions.
     */
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-clear-event",
		     G_CALLBACK(selection_clear_event), ಖಾಲಿ);
    g_signal_connect(G_OBJECT(gui.drawarea), "selection-received",
		     G_CALLBACK(selection_received_cb), ಖಾಲಿ);

    g_signal_connect(G_OBJECT(gui.drawarea), "selection-get",
		     G_CALLBACK(selection_get_cb), ಖಾಲಿ);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clean up for when exiting Vim.
 */
    ಶೂನ್ಯ
gui_mch_exit(ಪೂರ್ಣಾಂಕ rc ಬಳಸದ)
{
    // Clean up, unless we don't want to invoke free().
    ಆದರೆ (gui.mainwin != ಖಾಲಿ && !really_exiting)
	gtk_widget_destroy(gui.mainwin);
}

/*
 * Get the position of the top left corner of the window.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_get_winpos(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    ಆದರೆ (gui_mch_get_display())
    {
	gtk_ಕಿಟಕಿ_get_position(GTK_ಕಿಟಕಿ(gui.mainwin), x, y);
	ಹಿಂತಿರುಗಿ OK;
    }
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Set the position of the top left corner of the window to the given
 * coordinates.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_winpos(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    gtk_ಕಿಟಕಿ_move(GTK_ಕಿಟಕಿ(gui.mainwin), x, y);
}

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
# ಆದರೆ 0
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ resize_idle_installed = FALSE;
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
    ಸ್ಥಿರ gboolean
force_shell_resize_idle(gpointer ದತ್ತಾಂಶ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ
	    && GTK_WIDGET_REALIZED(gui.mainwin)
	    && GTK_WIDGET_VISIBLE(gui.mainwin))
    {
	ಪೂರ್ಣಾಂಕ ಅಗಲ;
	ಪೂರ್ಣಾಂಕ ಎತ್ತರ;

	gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin), &ಅಗಲ, &ಎತ್ತರ);

	ಅಗಲ  -= get_menu_tool_ಅಗಲ();
	ಎತ್ತರ -= get_menu_tool_ಎತ್ತರ();

	gui_resize_shell(ಅಗಲ, ಎತ್ತರ);
    }

    resize_idle_installed = FALSE;
    ಹಿಂತಿರುಗಿ FALSE; // don't call me again
}
# endif
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

/*
 * Return TRUE if the main window is maximized.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_maximized(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_ಕಿಟಕಿ(gui.mainwin) != ಖಾಲಿ
	    && (gdk_ಕಿಟಕಿ_get_state(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin))
					       & GDK_ಕಿಟಕಿ_STATE_MAXIMIZED));
}

/*
 * Unmaximize the main window
 */
    ಶೂನ್ಯ
gui_mch_unmaximize(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ)
	gtk_ಕಿಟಕಿ_unmaximize(GTK_ಕಿಟಕಿ(gui.mainwin));
}

/*
 * Called when the font changed while the window is maximized or GO_KEEPWINSIZE
 * is set.  Compute the new Rows and Columns.  This is like resizing the
 * window.
 */
    ಶೂನ್ಯ
gui_mch_newfont(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ w, h;

    gtk_ಕಿಟಕಿ_get_ಗಾತ್ರ(GTK_ಕಿಟಕಿ(gui.mainwin), &w, &h);
    w -= get_menu_tool_ಅಗಲ();
    h -= get_menu_tool_ಎತ್ತರ();
    gui_resize_shell(w, h);
}

/*
 * Set the windows size.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_shellsize(ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ,
		      ಪೂರ್ಣಾಂಕ min_ಅಗಲ ಬಳಸದ,  ಪೂರ್ಣಾಂಕ min_ಎತ್ತರ ಬಳಸದ,
		      ಪೂರ್ಣಾಂಕ ತಳ_ಅಗಲ ಬಳಸದ, ಪೂರ್ಣಾಂಕ ತಳ_ಎತ್ತರ ಬಳಸದ,
		      ಪೂರ್ಣಾಂಕ direction ಬಳಸದ)
{
    // give GTK+ a chance to put all widget's into place
    gui_mch_update();

    // this will cause the proper resizement to happen too
    ಆದರೆ (gtk_socket_id == 0)
	update_ಕಿಟಕಿ_manager_hints(0, 0);

    // With GTK+ 2, changing the size of the form widget doesn't resize
    // the window.  So let's do it the other way around and resize the
    // main window instead.
    ಅಗಲ  += get_menu_tool_ಅಗಲ();
    ಎತ್ತರ += get_menu_tool_ಎತ್ತರ();

    // Compensate for CSD frame: on Wayland (GTK3 with CSD), the compositor
    // subtracts the decoration margin from the requested size.  mch_csd_width
    // and mch_csd_height are computed from the first startup resize response
    // and are 0 on X11.
    ಅಗಲ  += mch_csd_ಅಗಲ;
    ಎತ್ತರ += mch_csd_ಎತ್ತರ;

    // Record the form width we expect the compositor to deliver.  Used in
    // form_configure_event() to clamp configure responses that are slightly
    // narrower than intended (due to CSD startup underestimate), preventing
    // column loss.  Only meaningful once mch_csd_width has been measured.
    mch_pending_form_w = (mch_csd_ಅಗಲ > 0)
			    ? ಅಗಲ - get_menu_tool_ಅಗಲ() - mch_csd_ಅಗಲ : 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ TRACK_RESIZE_HISTORY
    alloc_resize_hist(ಅಗಲ, ಎತ್ತರ, TRUE);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (gtk_socket_id == 0)
	gtk_ಕಿಟಕಿ_resize(GTK_ಕಿಟಕಿ(gui.mainwin), ಅಗಲ, ಎತ್ತರ);
    ಇಲ್ಲದಿದ್ದರೆ
	update_ಕಿಟಕಿ_manager_hints(ಅಗಲ, ಎತ್ತರ);

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
# ಆದರೆ 0
    ಆದರೆ (!resize_idle_installed)
    {
	g_idle_ಕೂಡಿಸು_ತುಂಬಿದ(GDK_ಆದ್ಯತೆ_EVENTS + 10,
			&force_shell_resize_idle, ಖಾಲಿ, ಖಾಲಿ);
	resize_idle_installed = TRUE;
    }
# endif
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
    /*
     * Wait until all events are processed to prevent a crash because the
     * real size of the drawing area doesn't reflect Vim's internal ideas.
     *
     * This is a bit of a hack, since Vim is a terminal application with a GUI
     * on top, while the GUI expects to be the boss.
     */
    gui_mch_update();
}

    ಶೂನ್ಯ
gui_mch_settitle(char_u *title, char_u *icon ಬಳಸದ)
{
    ಆದರೆ (title != ಖಾಲಿ && output_conv.vc_type != CONV_NONE)
	title = string_convert(&output_conv, title, ಖಾಲಿ);

    gtk_ಕಿಟಕಿ_ಗಣ_title(GTK_ಕಿಟಕಿ(gui.mainwin), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title);

    ಆದರೆ (output_conv.vc_type != CONV_NONE)
	vim_free(title);
}

#ಆದರೆ defined(FEAT_MENU)
    ಶೂನ್ಯ
gui_mch_enable_menu(ಪೂರ್ಣಾಂಕ showit)
{
    GtkWidget *widget;

# ifdef FEAT_GUI_GNOME
    ಆದರೆ (using_gnome)
	widget = gui.menubar_h;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	widget = gui.menubar;

    // Do not disable the menu while starting up, otherwise F10 doesn't work.
    ಆದರೆ (!showit != !gtk_widget_get_visible(widget) && !gui.starting)
    {
	ಆದರೆ (showit)
	    gtk_widget_show(widget);
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_widget_hide(widget);

	update_ಕಿಟಕಿ_manager_hints(0, 0);
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

#ಆದರೆ defined(FEAT_TOOLBAR)
    ಶೂನ್ಯ
gui_mch_show_toolbar(ಪೂರ್ಣಾಂಕ showit)
{
    GtkWidget *widget;

    ಆದರೆ (gui.toolbar == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ifdef FEAT_GUI_GNOME
    ಆದರೆ (using_gnome)
	widget = gui.toolbar_h;
    ಇಲ್ಲದಿದ್ದರೆ
# endif
	widget = gui.toolbar;

    ಆದರೆ (showit)
	ಗಣ_toolbar_style(GTK_TOOLBAR(gui.toolbar));

    ಆದರೆ (!showit != !gtk_widget_get_visible(widget))
    {
	ಆದರೆ (showit)
	    gtk_widget_show(widget);
	ಇಲ್ಲದಿದ್ದರೆ
	    gtk_widget_hide(widget);

	update_ಕಿಟಕಿ_manager_hints(0, 0);
    }
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_TOOLBAR

/*
 * Check if a given font is a CJK font. This is done in a very crude manner. It
 * just see if U+04E00 for zh and ja and U+AC00 for ko are covered in a given
 * font. Consequently, this function cannot  be used as a general purpose check
 * for CJK-ness for which fontconfig APIs should be used.  This is only used by
 * gui_mch_init_font() to deal with 'CJK fixed width fonts'.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
is_cjk_font(PangoFontDescription *font_desc)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ cjk_langs[] =
	{"zh_CN", "zh_TW", "zh_HK", "ja", "ko"};

    PangoFont	*font;
    ಚಿಹ್ನೆರಹಿತ	i;
    ಪೂರ್ಣಾಂಕ		is_cjk = FALSE;

    font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, font_desc);

    ಆದರೆ (font == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

    ಗಾಗಿ (i = 0; !is_cjk && i < G_N_ELEMENTS(cjk_langs); ++i)
    {
	PangoCoverage	*coverage;
	gunichar	uc;

	// Valgrind reports a leak for pango_language_from_string(), but the
	// documentation says "This is owned by Pango and should not be freed".
	coverage = pango_font_get_coverage(
		font, pango_language_from_string(cjk_langs[i]));

	ಆದರೆ (coverage != ಖಾಲಿ)
	{
	    uc = (cjk_langs[i][0] == 'k') ? 0xAC00 : 0x4E00;
	    is_cjk = (pango_coverage_get(coverage, uc) == PANGO_COVERAGE_EXACT);
#ಆದರೆ PANGO_ಆವೃತ್ತಿ_CHECK(1, 52, 0)
	    g_object_unref(coverage);
#ಇಲ್ಲದಿದ್ದರೆ
	    pango_coverage_unref(coverage);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }

    g_object_unref(font);

    ಹಿಂತಿರುಗಿ is_cjk;
}

/*
 * Adjust gui.char_height (after 'linespace' was changed).
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_adjust_charheight(ಶೂನ್ಯ)
{
    PangoFontMetrics	*metrics;
    ಪೂರ್ಣಾಂಕ			ascent;
    ಪೂರ್ಣಾಂಕ			descent;

    metrics = pango_context_get_metrics(gui.ಪಠ್ಯ_context, gui.norm_font,
				pango_context_get_language(gui.ಪಠ್ಯ_context));
    ascent  = pango_font_metrics_get_ascent(metrics);
    descent = pango_font_metrics_get_descent(metrics);

    pango_font_metrics_unref(metrics);

    // Round up when the value is more than about 1/16 of a pixel above a whole
    // pixel (12.0624 becomes 12, 12.07 becomes 13).  Then add 'linespace'.
    gui.char_ಎತ್ತರ = (ascent + descent + (PANGO_SCALE * 15) / 16)
						   / PANGO_SCALE + p_linespace;
    // LINTED: avoid warning: bitwise operation on signed value
    gui.char_ascent = PANGO_PIXELS(ascent + p_linespace * PANGO_SCALE / 2);

    // A not-positive value of char_height may crash Vim.  Only happens
    // if 'linespace' is negative (which does make sense sometimes).
    gui.char_ascent = MAX(gui.char_ascent, 0);
    gui.char_ಎತ್ತರ = MAX(gui.char_ಎತ್ತರ, gui.char_ascent + 1);

    ಹಿಂತಿರುಗಿ OK;
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
// Callback function used in gui_mch_font_dialog()
    ಸ್ಥಿರ gboolean
font_filter(ಸ್ಥಿರಾಂಕ PangoFontFamily *family,
	    ಸ್ಥಿರಾಂಕ PangoFontFace   *face ಬಳಸದ,
	    gpointer		   ದತ್ತಾಂಶ ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ pango_font_family_is_monospace((PangoFontFamily *)family);
}
#ಆದರೆ_ಅಂತ್ಯ

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
    ಪೂರ್ಣಾಂಕ		response;
    char_u	*fontname = ಖಾಲಿ;
    char_u	*oldname;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
    dialog = gtk_font_chooser_dialog_new(ಖಾಲಿ, ಖಾಲಿ);
    gtk_font_chooser_ಗಣ_filter_func(GTK_FONT_CHOOSER(dialog), font_filter,
	    ಖಾಲಿ, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
    dialog = gtk_font_selection_dialog_new(ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    gtk_ಕಿಟಕಿ_ಗಣ_transient_for(GTK_ಕಿಟಕಿ(dialog), GTK_ಕಿಟಕಿ(gui.mainwin));
    gtk_ಕಿಟಕಿ_ಗಣ_destroy_with_ಪೋಷಕ(GTK_ಕಿಟಕಿ(dialog), TRUE);

    ಆದರೆ (oldval != ಖಾಲಿ && oldval[0] != NUL)
    {
	ಆದರೆ (output_conv.vc_type != CONV_NONE)
	    oldname = string_convert(&output_conv, oldval, ಖಾಲಿ);
	ಇಲ್ಲದಿದ್ದರೆ
	    oldname = oldval;

	// Annoying bug in GTK (or Pango): if the font name does not include a
	// size, zero is used.  Use default point size ten.
	ಆದರೆ (!vim_isdigit(oldname[STRLEN(oldname) - 1]))
	{
	    char_u	*p = vim_strnsave(oldname, STRLEN(oldname) + 3);

	    ಆದರೆ (p != ಖಾಲಿ)
	    {
		STRCPY(p + STRLEN(p), " 10");
		ಆದರೆ (oldname != oldval)
		    vim_free(oldname);
		oldname = p;
	    }
	}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	gtk_font_chooser_ಗಣ_font(
		GTK_FONT_CHOOSER(dialog), (ಸ್ಥಿರಾಂಕ gchar *)oldname);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_font_selection_dialog_ಗಣ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)oldname);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (oldname != oldval)
	    vim_free(oldname);
    }
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	gtk_font_chooser_ಗಣ_font(
		GTK_FONT_CHOOSER(dialog), DEFAULT_FONT);
#ಇಲ್ಲದಿದ್ದರೆ
	gtk_font_selection_dialog_ಗಣ_font_name(
		GTK_FONT_SELECTION_DIALOG(dialog), DEFAULT_FONT);
#ಆದರೆ_ಅಂತ್ಯ

    response = gtk_dialog_ಸತತ(GTK_DIALOG(dialog));

    ಆದರೆ (response == GTK_RESPONSE_OK)
    {
	ಅಕ್ಷರ *name;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,2,0)
	name = gtk_font_chooser_get_font(GTK_FONT_CHOOSER(dialog));
#ಇಲ್ಲದಿದ್ದರೆ
	name = gtk_font_selection_dialog_get_font_name(
			    GTK_FONT_SELECTION_DIALOG(dialog));
#ಆದರೆ_ಅಂತ್ಯ
	ಆದರೆ (name != ಖಾಲಿ)
	{
	    char_u  *p;

	    // Apparently some font names include a comma, need to escape
	    // that, because in 'guifont' it separates names.
	    p = vim_strsave_escaped((char_u *)name, (char_u *)",");
	    g_free(name);
	    ಆದರೆ (p != ಖಾಲಿ && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    {
		fontname = string_convert(&ಒಳಹರಿವು_conv, p, ಖಾಲಿ);
		vim_free(p);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		fontname = p;
	}
    }

    ಆದರೆ (response != GTK_RESPONSE_NONE)
	gtk_widget_destroy(dialog);

    ಹಿಂತಿರುಗಿ fontname;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
get_styled_font_variants(ಶೂನ್ಯ)
{
    PangoFontDescription    *bold_font_desc;
    PangoFont		    *plain_font;
    PangoFont		    *bold_font;

    gui.font_can_bold = false;

    plain_font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, gui.norm_font);

    ಆದರೆ (plain_font == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    bold_font_desc = pango_font_description_copy_static(gui.norm_font);
    pango_font_description_ಗಣ_ತೂಕ(bold_font_desc, PANGO_ತೂಕ_BOLD);

    bold_font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, bold_font_desc);
    /*
     * The comparison relies on the unique handle nature of a PangoFont*,
     * i.e. it's assumed that a different PangoFont* won't refer to the
     * same font.  Seems to work, and failing here isn't critical anyway.
     */
    ಆದರೆ (bold_font != ಖಾಲಿ)
    {
	gui.font_can_bold = (bold_font != plain_font);
	g_object_unref(bold_font);
    }

    pango_font_description_free(bold_font_desc);
    ಆದರೆ (bold_font != ಖಾಲಿ && gui.font_can_bold)
	g_object_unref(plain_font);
}

ಸ್ಥಿರ PangoEngineShape *default_shape_engine = ಖಾಲಿ;

/*
 * Create a map from ASCII characters in the range [32,126] to glyphs
 * of the current font.  This is used by gui_gtk_draw_string() to skip
 * the itemize and shaping process for the most common case.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ascii_glyph_ಕೋಷ್ಟಕ_init(ಶೂನ್ಯ)
{
    char_u	    ascii_chars[2 * 128];
    PangoAttrList   *attr_list;
    GList	    *ಅಂಶ_list;
    ಪೂರ್ಣಾಂಕ		    i;

    ಆದರೆ (gui.ascii_glyphs != ಖಾಲಿ)
	pango_glyph_string_free(gui.ascii_glyphs);
    ಆದರೆ (gui.ascii_font != ಖಾಲಿ)
	g_object_unref(gui.ascii_font);

    gui.ascii_glyphs = ಖಾಲಿ;
    gui.ascii_font   = ಖಾಲಿ;

    // For safety, fill in question marks for the control characters.
    // Put a space between characters to avoid shaping.
    ಗಾಗಿ (i = 0; i < 128; ++i)
    {
	ಆದರೆ (i >= 32 && i < 127)
	    ascii_chars[2 * i] = i;
	ಇಲ್ಲದಿದ್ದರೆ
	    ascii_chars[2 * i] = '?';
	ascii_chars[2 * i + 1] = ' ';
    }

    attr_list = pango_attr_list_new();
    ಅಂಶ_list = pango_itemize(gui.ಪಠ್ಯ_context, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ascii_chars,
			      0, ಗಾತ್ರ(ascii_chars), attr_list, ಖಾಲಿ);

    ಆದರೆ (ಅಂಶ_list != ಖಾಲಿ && ಅಂಶ_list->ಮುಂದಿನ == ಖಾಲಿ) // play safe
    {
	Pango_ಅಂಶ   *ಅಂಶ;
	ಪೂರ್ಣಾಂಕ	    ಅಗಲ;

	ಅಂಶ  = (Pango_ಅಂಶ *)ಅಂಶ_list->ದತ್ತಾಂಶ;
	ಅಗಲ = gui.char_ಅಗಲ * PANGO_SCALE;

	// Remember the shape engine used for ASCII.
	default_shape_engine = ಅಂಶ->analysis.shape_engine;

	gui.ascii_font = ಅಂಶ->analysis.font;
	g_object_ref(gui.ascii_font);

	gui.ascii_glyphs = pango_glyph_string_new();

	pango_shape((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)ascii_chars, ಗಾತ್ರ(ascii_chars),
		    &ಅಂಶ->analysis, gui.ascii_glyphs);

	g_return_if_fail(gui.ascii_glyphs->ಅಂಕೆ_glyphs == ಗಾತ್ರ(ascii_chars));

	ಗಾಗಿ (i = 0; i < gui.ascii_glyphs->ಅಂಕೆ_glyphs; ++i)
	{
	    PangoGlyphGeometry *geom;

	    geom = &gui.ascii_glyphs->glyphs[i].geometry;
	    geom->x_ಸ್ಥಾನಾಂತರ += MAX(0, ಅಗಲ - geom->ಅಗಲ) / 2;
	    geom->ಅಗಲ = ಅಗಲ;
	}
    }

    // TODO: is this type cast OK?
    g_list_foreach(ಅಂಶ_list, (GFunc)(ಶೂನ್ಯ *)&pango_ಅಂಶ_free, ಖಾಲಿ);
    g_list_free(ಅಂಶ_list);
    pango_attr_list_unref(attr_list);
}

/*
 * Initialize Vim to use the font or fontset with the given name.
 * Return FAIL if the font could not be loaded, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_init_font(char_u *font_name, ಪೂರ್ಣಾಂಕ fontset ಬಳಸದ)
{
    PangoFontDescription    *font_desc;
    PangoLayout		    *layout;
    ಪೂರ್ಣಾಂಕ			    ಅಗಲ;

    // If font_name is NULL, this means to use the default, which should
    // be present on all proper Pango/fontconfig installations.
    ಆದರೆ (font_name == ಖಾಲಿ)
	font_name = (char_u *)DEFAULT_FONT;

    font_desc = gui_mch_get_font(font_name, FALSE);

    ಆದರೆ (font_desc == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    gui_mch_free_font(gui.norm_font);
    gui.norm_font = font_desc;

    pango_context_ಗಣ_font_description(gui.ಪಠ್ಯ_context, font_desc);

    layout = pango_layout_new(gui.ಪಠ್ಯ_context);
    pango_layout_ಗಣ_ಪಠ್ಯ(layout, "MW", 2);
    pango_layout_get_ಗಾತ್ರ(layout, &ಅಗಲ, ಖಾಲಿ);
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
    ಆದರೆ (is_cjk_font(gui.norm_font))
    {
	ಪೂರ್ಣಾಂಕ cjk_ಅಗಲ;

	// Measure the text extent of U+4E00 and U+4E8C
	pango_layout_ಗಣ_ಪಠ್ಯ(layout, "\344\270\200\344\272\214", -1);
	pango_layout_get_ಗಾತ್ರ(layout, &cjk_ಅಗಲ, ಖಾಲಿ);

	ಆದರೆ (ಅಗಲ == cjk_ಅಗಲ)  // Xft not patched
	    ಅಗಲ /= 2;
    }
    g_object_unref(layout);

    gui.char_ಅಗಲ = (ಅಗಲ / 2 + PANGO_SCALE - 1) / PANGO_SCALE;

    // A zero width may cause a crash.	Happens for semi-invalid fontsets.
    ಆದರೆ (gui.char_ಅಗಲ <= 0)
	gui.char_ಅಗಲ = 8;

    gui_mch_adjust_charheight();

    // Set the fontname, which will be used for information purposes
    hl_ಗಣ_font_name(font_name);

    get_styled_font_variants();
    ascii_glyph_ಕೋಷ್ಟಕ_init();

    // Avoid unnecessary overhead if 'guifontwide' is equal to 'guifont'.
    ಆದರೆ (gui.wide_font != ಖಾಲಿ
	&& pango_font_description_equal(gui.norm_font, gui.wide_font))
    {
	pango_font_description_free(gui.wide_font);
	gui.wide_font = ಖಾಲಿ;
    }

    ಆದರೆ (gui_mch_maximized())
    {
	// Update lines and columns in accordance with the new font, keep the
	// window maximized.
	gui_mch_newfont();
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// Preserve the logical dimensions of the screen.
	update_ಕಿಟಕಿ_manager_hints(0, 0);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Get a reference to the font "name".
 * Return zero for failure.
 */
    GuiFont
gui_mch_get_font(char_u *name, ಪೂರ್ಣಾಂಕ report_error)
{
    PangoFontDescription    *font;

    // can't do this when GUI is not running
    ಆದರೆ (!gui.ಒಳ_use || name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;

    ಆದರೆ (output_conv.vc_type != CONV_NONE)
    {
	char_u *buf;

	buf = string_convert(&output_conv, name, ಖಾಲಿ);
	ಆದರೆ (buf != ಖಾಲಿ)
	{
	    font = pango_font_description_from_string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)buf);
	    vim_free(buf);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    font = ಖಾಲಿ;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	font = pango_font_description_from_string((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)name);

    ಆದರೆ (font != ಖಾಲಿ)
    {
	PangoFont *real_font;

	// pango_context_load_font() bails out if no font size is set
	ಆದರೆ (pango_font_description_get_ಗಾತ್ರ(font) <= 0)
	    pango_font_description_ಗಣ_ಗಾತ್ರ(font, 10 * PANGO_SCALE);

	real_font = pango_context_ತುಂಬು_font(gui.ಪಠ್ಯ_context, font);

	ಆದರೆ (real_font == ಖಾಲಿ)
	{
	    pango_font_description_free(font);
	    font = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    g_object_unref(real_font);
    }

    ಆದರೆ (font == ಖಾಲಿ)
    {
	ಆದರೆ (report_error)
	    semsg(_((ಅಕ್ಷರ *)e_unknown_font_str), name);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಹಿಂತಿರುಗಿ font;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the name of font "font" in allocated memory.
 */
    char_u *
gui_mch_get_fontname(GuiFont font, char_u *name ಬಳಸದ)
{
    ಆದರೆ (font != NOFONT)
    {
	ಅಕ್ಷರ	*pangoname = pango_font_description_ಗಮ್ಯ_string(font);

	ಆದರೆ (pangoname != ಖಾಲಿ)
	{
	    char_u	*s = vim_strsave((char_u *)pangoname);

	    g_free(pangoname);
	    ಹಿಂತಿರುಗಿ s;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If a font is not going to be used, free its structure.
 */
    ಶೂನ್ಯ
gui_mch_free_font(GuiFont font)
{
    ಆದರೆ (font != NOFONT)
	pango_font_description_free(font);
}

/*
 * Cmdline expansion for setting 'guifont' / 'guifontwide'. Will enumerate
 * through all fonts for completion. When setting 'guifont' it will only show
 * monospace fonts as it's unlikely other fonts would be useful.
 */
    ಶೂನ್ಯ
gui_mch_expand_font(
    optexpand_T	*ವಾದಗಳು,
    ಶೂನ್ಯ	*param,
    ಪೂರ್ಣಾಂಕ		(*ಕೂಡಿಸು_ಹೊಂದಿಕೆ)(char_u *val))
{
    PangoFontFamily	**font_families = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ			n_families = 0;
    ಪೂರ್ಣಾಂಕ			wide = *(ಪೂರ್ಣಾಂಕ *)param;

    ಆದರೆ (ವಾದಗಳು->oe_include_orig_val && *ವಾದಗಳು->oe_opt_ಮೌಲ್ಯ == NUL && !wide)
    {
	// If guifont is empty, and we want to fill in the orig value, suggest
	// the default so the user can modify it.
	ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)DEFAULT_FONT) != OK)
	    ಹಿಂತಿರುಗಿ;
    }

    pango_context_list_families(
	    gui.ಪಠ್ಯ_context,
	    &font_families,
	    &n_families);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_families; i++)
    {
	ಆದರೆ (!wide && !pango_font_family_is_monospace(font_families[i]))
	    ಮುಂದುವರಿ;

	ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* fam_name = pango_font_family_get_name(font_families[i]);
	ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	{
	    char_u *buf = string_convert(&ಒಳಹರಿವು_conv, (char_u*)fam_name, ಖಾಲಿ);
	    ಆದರೆ (buf != ಖಾಲಿ)
	    {
		ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ(buf) != OK)
		{
		    vim_free(buf);
		    ನಿಲ್ಲಿಸು;
		}
		vim_free(buf);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ನಿಲ್ಲಿಸು;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (ಕೂಡಿಸು_ಹೊಂದಿಕೆ((char_u *)fam_name) != OK)
		ನಿಲ್ಲಿಸು;
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

    ಆದರೆ (!gui.ಒಳ_use)		// can't do this when GUI not running
	ಹಿಂತಿರುಗಿ color;

    ಆದರೆ (name != ಖಾಲಿ)
	color = gui_get_color_cmn(name);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಹಿಂತಿರುಗಿ color;
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (color == INVALCOLOR)
	ಹಿಂತಿರುಗಿ INVALCOLOR;

    ಹಿಂತಿರುಗಿ gui_mch_get_rgb_color(
	    (color & 0xff0000) >> 16,
	    (color & 0xff00) >> 8,
	    color & 0xff);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return the Pixel value (color) for the given RGB values.
 * Return INVALCOLOR for error.
 */
    guicolor_T
gui_mch_get_rgb_color(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ g, ಪೂರ್ಣಾಂಕ b)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಹಿಂತಿರುಗಿ gui_get_rgb_color_cmn(r, g, b);
#ಇಲ್ಲದಿದ್ದರೆ
    GdkColor gcolor;
    ಪೂರ್ಣಾಂಕ ret;

    gcolor.red = (guint16)(r / 255.0 * 65535 + 0.5);
    gcolor.green = (guint16)(g / 255.0 * 65535 + 0.5);
    gcolor.blue = (guint16)(b / 255.0 * 65535 + 0.5);

    ret = gdk_colormap_alloc_color(gtk_widget_get_colormap(gui.drawarea),
	    &gcolor, FALSE, TRUE);

    ಹಿಂತಿರುಗಿ ret != 0 ? (guicolor_T)gcolor.pixel : INVALCOLOR;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the current text foreground color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_fg_color(guicolor_T color)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    *gui.fgcolor = color_ಗಮ್ಯ_rgba(color);
#ಇಲ್ಲದಿದ್ದರೆ
    gui.fgcolor->pixel = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)color;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the current text background color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_bg_color(guicolor_T color)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    *gui.bgcolor = color_ಗಮ್ಯ_rgba(color);
#ಇಲ್ಲದಿದ್ದರೆ
    gui.bgcolor->pixel = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)color;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set the current text special color.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_sp_color(guicolor_T color)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    *gui.spcolor = color_ಗಮ್ಯ_rgba(color);
#ಇಲ್ಲದಿದ್ದರೆ
    gui.spcolor->pixel = (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)color;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Function-like convenience macro for the sake of efficiency.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು ಸೇರಿಸು_PANGO_ATTR(Attribute, AttrList, ಆರಂಭ_2, ಕೊನೆ_2)  \
    G_STMT_ಆರಂಭ{					    \
	PangoAttribute *tmp_attr_;			    \
	tmp_attr_ = (Attribute);			    \
	tmp_attr_->ಆರಂಭ_ಸೂಚ್ಯಂಕ = (ಆರಂಭ_2);		    \
	tmp_attr_->ಕೊನೆ_ಸೂಚ್ಯಂಕ = (ಕೊನೆ_2);			    \
	pango_attr_list_ಸೇರಿಸು((AttrList), tmp_attr_);	    \
    }G_STMT_ಕೊನೆ

    ಸ್ಥಿರ ಶೂನ್ಯ
apply_wide_font_attr(char_u *s, ಪೂರ್ಣಾಂಕ len, PangoAttrList *attr_list)
{
    char_u  *ಆರಂಭ = ಖಾಲಿ;
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    uc;

    ಗಾಗಿ (p = s; p < s + len; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);

	ಆದರೆ (ಆರಂಭ == ಖಾಲಿ)
	{
	    ಆದರೆ (uc >= 0x80 && utf_char2cells(uc) == 2)
		ಆರಂಭ = p;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (uc < 0x80 // optimization shortcut
		 || (utf_char2cells(uc) != 2 && !utf_iscomposing(uc)))
	{
	    ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			      attr_list, ಆರಂಭ - s, p - s);
	    ಆರಂಭ = ಖಾಲಿ;
	}
    }

    ಆದರೆ (ಆರಂಭ != ಖಾಲಿ)
	ಸೇರಿಸು_PANGO_ATTR(pango_attr_font_desc_new(gui.wide_font),
			  attr_list, ಆರಂಭ - s, len);
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ಎಣಿಕೆ_cluster_cells(char_u *s, Pango_ಅಂಶ *ಅಂಶ,
		    PangoGlyphString* glyphs, ಪೂರ್ಣಾಂಕ i,
		    ಪೂರ್ಣಾಂಕ *cluster_ಅಗಲ,
		    ಪೂರ್ಣಾಂಕ *ಕೊನೆಯ_glyph_rbearing)
{
    char_u  *p;
    ಪೂರ್ಣಾಂಕ	    ಮುಂದಿನ;	// glyph start index of next cluster
    ಪೂರ್ಣಾಂಕ	    ಆರಂಭ, ಕೊನೆ; // string segment of current cluster
    ಪೂರ್ಣಾಂಕ	    ಅಗಲ;	// real cluster width in Pango units
    ಪೂರ್ಣಾಂಕ	    uc;
    ಪೂರ್ಣಾಂಕ	    cellcount = 0;

    ಅಗಲ = glyphs->glyphs[i].geometry.ಅಗಲ;

    ಗಾಗಿ (ಮುಂದಿನ = i + 1; ಮುಂದಿನ < glyphs->ಅಂಕೆ_glyphs; ++ಮುಂದಿನ)
    {
	ಆದರೆ (glyphs->glyphs[ಮುಂದಿನ].attr.is_cluster_ಆರಂಭ)
	    ನಿಲ್ಲಿಸು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (glyphs->glyphs[ಮುಂದಿನ].geometry.ಅಗಲ > ಅಗಲ)
	    ಅಗಲ = glyphs->glyphs[ಮುಂದಿನ].geometry.ಅಗಲ;
    }

    ಆರಂಭ = ಅಂಶ->ಸ್ಥಾನಾಂತರ + glyphs->log_clusters[i];
    ಕೊನೆ   = ಅಂಶ->ಸ್ಥಾನಾಂತರ + ((ಮುಂದಿನ < glyphs->ಅಂಕೆ_glyphs) ?
			    glyphs->log_clusters[ಮುಂದಿನ] : ಅಂಶ->ಉದ್ದಳತೆ);

    ಗಾಗಿ (p = s + ಆರಂಭ; p < s + ಕೊನೆ; p += utf_byte2len(*p))
    {
	uc = utf_ptr2char(p);
	ಆದರೆ (uc < 0x80)
	    ++cellcount;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!utf_iscomposing(uc))
	    cellcount += utf_char2cells(uc);
    }

    ಆದರೆ (ಕೊನೆಯ_glyph_rbearing != ಖಾಲಿ
	    && cellcount > 0 && ಮುಂದಿನ == glyphs->ಅಂಕೆ_glyphs)
    {
	PangoRectangle ink_rect;
	/*
	 * If a certain combining mark had to be taken from a non-monospace
	 * font, we have to compensate manually by adapting x_offset according
	 * to the ink extents of the previous glyph.
	 */
	pango_font_get_glyph_extents(ಅಂಶ->analysis.font,
				     glyphs->glyphs[i].glyph,
				     &ink_rect, ಖಾಲಿ);

	ಆದರೆ (PANGO_RBEARING(ink_rect) > 0)
	    *ಕೊನೆಯ_glyph_rbearing = PANGO_RBEARING(ink_rect);
    }

    ಆದರೆ (cellcount > 0)
	*cluster_ಅಗಲ = ಅಗಲ;

    ಹಿಂತಿರುಗಿ cellcount;
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
    ಸ್ಥಿರ ಶೂನ್ಯ
ಸಿದ್ಧತೆ_zero_ಅಗಲ_cluster(Pango_ಅಂಶ *ಅಂಶ, PangoGlyphInfo *glyph,
			 ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_cellcount, ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_cluster_ಅಗಲ,
			 ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_glyph_rbearing)
{
    PangoRectangle  ink_rect;
    PangoRectangle  logical_rect;
    ಪೂರ್ಣಾಂಕ		    ಅಗಲ;

    ಅಗಲ = ಕೊನೆಯ_cellcount * gui.char_ಅಗಲ * PANGO_SCALE;
    glyph->geometry.x_ಸ್ಥಾನಾಂತರ = -ಅಗಲ + MAX(0, ಅಗಲ - ಕೊನೆಯ_cluster_ಅಗಲ) / 2;
    glyph->geometry.ಅಗಲ = 0;

    pango_font_get_glyph_extents(ಅಂಶ->analysis.font,
				 glyph->glyph,
				 &ink_rect, &logical_rect);
    ಆದರೆ (ink_rect.x < 0)
    {
	glyph->geometry.x_ಸ್ಥಾನಾಂತರ += ಕೊನೆಯ_glyph_rbearing;
	glyph->geometry.y_ಸ್ಥಾನಾಂತರ  = logical_rect.ಎತ್ತರ
		- (gui.char_ಎತ್ತರ - p_linespace) * PANGO_SCALE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// If the accent width is smaller than the cluster width, position it
	// in the middle.
	glyph->geometry.x_ಸ್ಥಾನಾಂತರ = -ಅಗಲ + MAX(0, ಅಗಲ - ink_rect.ಅಗಲ) / 2;
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_glyph_string(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_cells, ಪೂರ್ಣಾಂಕ flags,
		  PangoFont *font, PangoGlyphString *glyphs,
		  cairo_t *cr)
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_glyph_string(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_cells, ಪೂರ್ಣಾಂಕ flags,
		  PangoFont *font, PangoGlyphString *glyphs)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಆದರೆ (!(flags & DRAW_TRANSP))
    {
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_rectangle(cr,
			FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು),
			ಅಂಕೆ_cells * gui.char_ಅಗಲ, gui.char_ಎತ್ತರ);
	cairo_fill(cr);
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.bgcolor);

	gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2,
			   gui.ಪಠ್ಯ_gc,
			   TRUE,
			   FILL_X(ಲಂಬಸಾಲು),
			   FILL_Y(ಅಡ್ಡಸಾಲು),
			   ಅಂಕೆ_cells * gui.char_ಅಗಲ,
			   gui.char_ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ
    }

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_ಗಣ_ಮೂಲ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
    cairo_move_ಗಮ್ಯ(cr, ಪಠ್ಯ_X(ಲಂಬಸಾಲು), ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು));
    pango_cairo_show_glyph_string(cr, font, glyphs);
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);

    gdk_draw_glyphs(gui.drawarea->ಕಿಟಕಿ_2,
		    gui.ಪಠ್ಯ_gc,
		    font,
		    ಪಠ್ಯ_X(ಲಂಬಸಾಲು),
		    ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
		    glyphs);
#ಆದರೆ_ಅಂತ್ಯ

    // redraw the contents with an offset of 1 to emulate bold
    ಆದರೆ ((flags & DRAW_BOLD) && !gui.font_can_bold)
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_ಗಮ್ಯ(cr, ಪಠ್ಯ_X(ಲಂಬಸಾಲು) + 1, ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು));
	pango_cairo_show_glyph_string(cr, font, glyphs);
    }
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_draw_glyphs(gui.drawarea->ಕಿಟಕಿ_2,
			gui.ಪಠ್ಯ_gc,
			font,
			ಪಠ್ಯ_X(ಲಂಬಸಾಲು) + 1,
			ಪಠ್ಯ_Y(ಅಡ್ಡಸಾಲು),
			glyphs);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Draw underline and undercurl at the bottom of the character cell.
 */
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_under(ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ cells, cairo_t *cr)
#ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಶೂನ್ಯ
draw_under(ಪೂರ್ಣಾಂಕ flags, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ cells)
#ಆದರೆ_ಅಂತ್ಯ
{
    ಪೂರ್ಣಾಂಕ			i;
    ಪೂರ್ಣಾಂಕ			ಸ್ಥಾನಾಂತರ;
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ	val[8] = {1, 0, 0, 0, 1, 2, 2, 2 };
    ಪೂರ್ಣಾಂಕ			y = FILL_Y(ಅಡ್ಡಸಾಲು + 1) - 1;

    // Undercurl: draw curl at the bottom of the character cell.
    ಆದರೆ (flags & DRAW_UNDERC)
    {
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	cairo_ಗಣ_ಸಾಲು_ಅಗಲ(cr, 1.0);
	cairo_ಗಣ_ಸಾಲು_cap(cr, CAIRO_ಸಾಲು_CAP_BUTT);
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_ಗಮ್ಯ(cr, FILL_X(ಲಂಬಸಾಲು) + 1, y - 2 + 0.5);
	ಗಾಗಿ (i = FILL_X(ಲಂಬಸಾಲು) + 1; i < FILL_X(ಲಂಬಸಾಲು + cells); ++i)
	{
	    ಸ್ಥಾನಾಂತರ = val[i % 8];
	    cairo_ಸಾಲು_ಗಮ್ಯ(cr, i, y - ಸ್ಥಾನಾಂತರ + 0.5);
	}
	cairo_stroke(cr);
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.spcolor);
	ಗಾಗಿ (i = FILL_X(ಲಂಬಸಾಲು); i < FILL_X(ಲಂಬಸಾಲು + cells); ++i)
	{
	    ಸ್ಥಾನಾಂತರ = val[i % 8];
	    gdk_draw_ಬಿಂದು(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc, i, y - ಸ್ಥಾನಾಂತರ);
	}
	gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Draw a strikethrough line
    ಆದರೆ (flags & DRAW_STRIKE)
    {
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	cairo_ಗಣ_ಸಾಲು_ಅಗಲ(cr, 1.0);
	cairo_ಗಣ_ಸಾಲು_cap(cr, CAIRO_ಸಾಲು_CAP_BUTT);
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.spcolor->red, gui.spcolor->green, gui.spcolor->blue,
		gui.spcolor->alpha);
	cairo_move_ಗಮ್ಯ(cr, FILL_X(ಲಂಬಸಾಲು), y + 1 - gui.char_ಎತ್ತರ/2 + 0.5);
	cairo_ಸಾಲು_ಗಮ್ಯ(cr, FILL_X(ಲಂಬಸಾಲು + cells), y + 1 - gui.char_ಎತ್ತರ/2 + 0.5);
	cairo_stroke(cr);
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.spcolor);
	gdk_draw_ಸಾಲು(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
		      FILL_X(ಲಂಬಸಾಲು), y + 1 - gui.char_ಎತ್ತರ/2,
		      FILL_X(ಲಂಬಸಾಲು + cells), y + 1 - gui.char_ಎತ್ತರ/2);
	gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
#ಆದರೆ_ಅಂತ್ಯ
    }

    // Underline: draw a line at the bottom of the character cell.
    ಆದರೆ (flags & DRAW_UNDERL)
    {
	// When p_linespace is 0, overwrite the bottom row of pixels.
	// Otherwise put the line just below the character.
	ಆದರೆ (p_linespace > 1)
	    y -= p_linespace - 1;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	cairo_ಗಣ_ಸಾಲು_ಅಗಲ(cr, 1.0);
	cairo_ಗಣ_ಸಾಲು_cap(cr, CAIRO_ಸಾಲು_CAP_BUTT);
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_move_ಗಮ್ಯ(cr, FILL_X(ಲಂಬಸಾಲು), y + 0.5);
	cairo_ಸಾಲು_ಗಮ್ಯ(cr, FILL_X(ಲಂಬಸಾಲು + cells), y + 0.5);
	cairo_stroke(cr);
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_draw_ಸಾಲು(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
		      FILL_X(ಲಂಬಸಾಲು), y,
		      FILL_X(ಲಂಬಸಾಲು + cells) - 1, y);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

    ಪೂರ್ಣಾಂಕ
gui_gtk_draw_string(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, char_u *s, ಪೂರ್ಣಾಂಕ len, ಪೂರ್ಣಾಂಕ flags)
{
    char_u	*conv_buf = ಖಾಲಿ;   // result of UTF-8 conversion
    char_u	*new_conv_buf;
    ಪೂರ್ಣಾಂಕ		convlen;
    char_u	*sp, *bp;
    ಪೂರ್ಣಾಂಕ		plen;
    ಪೂರ್ಣಾಂಕ		len_ಮೊತ್ತ;	// return value needs to add up since we are
				// printing substrings
    ಪೂರ್ಣಾಂಕ		byte_ಮೊತ್ತ;	// byte position in string
    char_u	*cs;		// current *s pointer
    ಪೂರ್ಣಾಂಕ		needs_pango;	// look ahead, 0=ascii 1=unicode/ligatures
    ಪೂರ್ಣಾಂಕ		should_need_pango = FALSE;
    ಪೂರ್ಣಾಂಕ		slen;
    ಪೂರ್ಣಾಂಕ		is_ligature;
    ಪೂರ್ಣಾಂಕ		is_utf8;
    char_u	backup_ch;

    ಆದರೆ (gui.ಪಠ್ಯ_context == ಖಾಲಿ || gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ len;

    ಆದರೆ (output_conv.vc_type != CONV_NONE)
    {
	/*
	 * Convert characters from 'encoding' to 'termencoding', which is set
	 * to UTF-8 by gui_mch_init().	did_set_string_option() in option.c
	 * prohibits changing this to something else than UTF-8 if the GUI is
	 * in use.
	 */
	convlen = len;
	conv_buf = string_convert(&output_conv, s, &convlen);
	g_return_val_if_fail(conv_buf != ಖಾಲಿ, len);

	// Correct for differences in char width: some chars are
	// double-wide in 'encoding' but single-wide in utf-8.  Add a space to
	// compensate for that.
	ಗಾಗಿ (sp = s, bp = conv_buf; sp < s + len && bp < conv_buf + convlen; )
	{
	    plen = utf_ptr2len(bp);
	    ಆದರೆ ((*mb_ptr2cells)(sp) == 2 && utf_ptr2cells(bp) == 1)
	    {
		new_conv_buf = alloc(convlen + 2);
		ಆದರೆ (new_conv_buf == ಖಾಲಿ)
		    ಹಿಂತಿರುಗಿ len;
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
    len_ಮೊತ್ತ = 0;    // return value needs to add up since we are printing
		    // substrings
    byte_ಮೊತ್ತ = 0;
    cs = s;
    // First char decides starting needs_pango mode, 0=ascii 1=utf8/ligatures.
    // Even if it is ligature char, two chars or more make ligature.
    // Ascii followed by utf8 is also going through pango.
    is_utf8 = (*cs & 0x80);
    is_ligature = gui.ligatures_map[*cs] && (len > 1);
    ಆದರೆ (is_ligature)
	is_ligature = gui.ligatures_map[*(cs + 1)];
    ಆದರೆ (!is_utf8 && len > 1)
	is_utf8 = (*(cs + 1) & 0x80) != 0;
    needs_pango = is_utf8 || is_ligature;

    // split string into ascii and non-ascii (ligatures + utf-8) substrings,
    // print glyphs or use Pango
    ಆಗ (cs < s + len)
    {
	slen = 0;
	ಆಗ (slen < (len - byte_ಮೊತ್ತ))
	{
	    is_ligature = gui.ligatures_map[*(cs + slen)];
	    // look ahead, single ligature char between ascii is ascii
	    ಆದರೆ (is_ligature && !needs_pango)
	    {
		ಆದರೆ ((slen + 1) < (len - byte_ಮೊತ್ತ))
		    is_ligature = gui.ligatures_map[*(cs + slen + 1)];
		ಇಲ್ಲದಿದ್ದರೆ
		    is_ligature = 0;
	    }
	    is_utf8 = *(cs + slen) & 0x80;
	    // ascii followed by utf8 could be combining
	    // if so send it through pango
	    ಆದರೆ ((!is_utf8) && ((slen + 1) < (len - byte_ಮೊತ್ತ)))
		is_utf8 = (*(cs + slen + 1) & 0x80);
	    should_need_pango = (is_ligature || is_utf8);
	    ಆದರೆ (needs_pango != should_need_pango) // mode switch
		ನಿಲ್ಲಿಸು;
	    ಆದರೆ (needs_pango)
	    {
		ಆದರೆ (is_ligature)
		{
		    slen++; // ligature char by char
		}
		ಇಲ್ಲದಿದ್ದರೆ // is_utf8
		{
		    ಆದರೆ ((*(cs + slen) & 0xC0) == 0x80)
		    {
			// a continuation, find next 0xC0 != 0x80 but don't
			// include it
			ಆಗ ((slen < (len - byte_ಮೊತ್ತ))
					    && ((*(cs + slen) & 0xC0) == 0x80))
			{
			    slen++;
			}
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*(cs + slen) & 0xE0) == 0xC0)
		    {
			// + one byte utf8
			slen++;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*(cs + slen) & 0xF0) == 0xE0)
		    {
			// + two bytes utf8
			slen += 2;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((*(cs + slen) & 0xF8) == 0xF0)
		    {
			// + three bytes utf8
			slen += 3;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// this should not happen, try moving forward, Pango
			// will catch it
			slen++;
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		slen++; // ascii
	    }
	}

	ಆದರೆ (slen < len)
	{
	    // temporarily zero terminate substring, print, restore char, wrap
	    backup_ch = *(cs + slen);
	    *(cs + slen) = NUL;
	}
	len_ಮೊತ್ತ += gui_gtk_draw_string_ext(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + len_ಮೊತ್ತ, cs, slen, flags,
					    needs_pango);
	ಆದರೆ (slen < len)
	    *(cs + slen) = backup_ch;
	cs += slen;
	byte_ಮೊತ್ತ += slen;
	needs_pango = should_need_pango;
    }
    vim_free(conv_buf);
    ಹಿಂತಿರುಗಿ len_ಮೊತ್ತ;
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ cairo_region_t *dirty_region = ಖಾಲಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
ಸರತಿ_draw_area(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y, ಪೂರ್ಣಾಂಕ ಅಗಲ, ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    cairo_rectangle_int_t rect;

    ಆದರೆ (ಅಗಲ <= 0 || ಎತ್ತರ <= 0 || gui.drawarea == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    rect.x = x;
    rect.y = y;
    rect.ಅಗಲ = ಅಗಲ;
    rect.ಎತ್ತರ = ಎತ್ತರ;

    ಆದರೆ (dirty_region == ಖಾಲಿ)
	dirty_region = cairo_region_create_rectangle(&rect);
    ಇಲ್ಲದಿದ್ದರೆ
	cairo_region_union_rectangle(dirty_region, &rect);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
gui_gtk_draw_string_ext(
	ಪೂರ್ಣಾಂಕ	ಅಡ್ಡಸಾಲು,
	ಪೂರ್ಣಾಂಕ	ಲಂಬಸಾಲು,
	char_u	*s,
	ಪೂರ್ಣಾಂಕ	len,
	ಪೂರ್ಣಾಂಕ	flags,
	ಪೂರ್ಣಾಂಕ	force_pango)
{
    GdkRectangle	area;		    // area for clip mask
    PangoGlyphString	*glyphs;	    // glyphs of current item
    ಪೂರ್ಣಾಂಕ			ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ = 0;  // column offset in cells
    ಪೂರ್ಣಾಂಕ			i;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_t		*cr;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Restrict all drawing to the current screen line in order to prevent
     * fuzzy font lookups from messing up the screen.
     */
    area.x = gui.border_ಸ್ಥಾನಾಂತರ;
    area.y = FILL_Y(ಅಡ್ಡಸಾಲು);
    area.ಅಗಲ	= gui.ಅಂಕೆ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ * gui.char_ಅಗಲ;
    area.ಎತ್ತರ = gui.char_ಎತ್ತರ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cr = cairo_create(gui.surface);
    cairo_rectangle(cr, area.x, area.y, area.ಅಗಲ, area.ಎತ್ತರ);
    cairo_clip(cr);
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_gc_ಗಣ_clip_origin(gui.ಪಠ್ಯ_gc, 0, 0);
    gdk_gc_ಗಣ_clip_rectangle(gui.ಪಠ್ಯ_gc, &area);
#ಆದರೆ_ಅಂತ್ಯ

    glyphs = pango_glyph_string_new();

    /*
     * Optimization hack:  If possible, skip the itemize and shaping process
     * for pure ASCII strings.	This optimization is particularly effective
     * because Vim draws space characters to clear parts of the screen.
     */
    ಆದರೆ (!(flags & DRAW_ITALIC)
	    && !((flags & DRAW_BOLD) && gui.font_can_bold)
	    && gui.ascii_glyphs != ಖಾಲಿ
	    && !force_pango)
    {
	char_u *p;

	ಗಾಗಿ (p = s; p < s + len; ++p)
	    ಆದರೆ (*p & 0x80)
		ಹೋಗು not_ascii;

	pango_glyph_string_ಗಣ_ಗಾತ್ರ(glyphs, len);

	ಗಾಗಿ (i = 0; i < len; ++i)
	{
	    glyphs->glyphs[i] = gui.ascii_glyphs->glyphs[2 * s[i]];
	    glyphs->log_clusters[i] = i;
	}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	draw_glyph_string(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, len, flags, gui.ascii_font, glyphs, cr);
#ಇಲ್ಲದಿದ್ದರೆ
	draw_glyph_string(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, len, flags, gui.ascii_font, glyphs);
#ಆದರೆ_ಅಂತ್ಯ

	ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ = len;
    }
    ಇಲ್ಲದಿದ್ದರೆ
not_ascii:
    {
	PangoAttrList	*attr_list;
	GList		*ಅಂಶ_list;
	ಪೂರ್ಣಾಂಕ		cluster_ಅಗಲ;
	ಪೂರ್ಣಾಂಕ		ಕೊನೆಯ_glyph_rbearing;
	ಪೂರ್ಣಾಂಕ		cells = 0;  // cells occupied by current cluster

	// Safety check: pango crashes when invoked with invalid utf-8
	// characters.
	ಆದರೆ (!utf_valid_string(s, s + len))
	{
	    ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ = len;
	    ಹೋಗು skipitall;
	}

	// original width of the current cluster
	cluster_ಅಗಲ = PANGO_SCALE * gui.char_ಅಗಲ;

	// right bearing of the last non-composing glyph
	ಕೊನೆಯ_glyph_rbearing = PANGO_SCALE * gui.char_ಅಗಲ;

	attr_list = pango_attr_list_new();

	// If 'guifontwide' is set then use that for double-width characters.
	// Otherwise just go with 'guifont' and let Pango do its thing.
	ಆದರೆ (gui.wide_font != ಖಾಲಿ)
	    apply_wide_font_attr(s, len, attr_list);

	ಆದರೆ ((flags & DRAW_BOLD) && gui.font_can_bold)
	    ಸೇರಿಸು_PANGO_ATTR(pango_attr_ತೂಕ_new(PANGO_ತೂಕ_BOLD),
			      attr_list, 0, len);
	ಆದರೆ (flags & DRAW_ITALIC)
	    ಸೇರಿಸು_PANGO_ATTR(pango_attr_style_new(PANGO_STYLE_ITALIC),
			      attr_list, 0, len);
	/*
	 * Break the text into segments with consistent directional level
	 * and shaping engine.	Pure Latin text needs only a single segment,
	 * so there's no need to worry about the loop's efficiency.  Better
	 * try to optimize elsewhere, e.g. reducing exposes and stuff :)
	 */
	ಅಂಶ_list = pango_itemize(gui.ಪಠ್ಯ_context,
				  (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s, 0, len, attr_list, ಖಾಲಿ);

	ಆಗ (ಅಂಶ_list != ಖಾಲಿ)
	{
	    Pango_ಅಂಶ	*ಅಂಶ;
	    ಪೂರ್ಣಾಂಕ		ಅಂಶ_cells = 0; // item length in cells

	    ಅಂಶ = (Pango_ಅಂಶ *)ಅಂಶ_list->ದತ್ತಾಂಶ;
	    ಅಂಶ_list = g_list_delete_link(ಅಂಶ_list, ಅಂಶ_list);
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
	    ಅಂಶ->analysis.level = (ಅಂಶ->analysis.level + 1) & (~1U);

	    // HACK: Overrule the shape engine, we don't want shaping to be
	    // done, because drawing the cursor would change the display.
	    ಅಂಶ->analysis.shape_engine = default_shape_engine;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_PANGO_SHAPE_ತುಂಬಿದ
	    pango_shape_ತುಂಬಿದ((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s + ಅಂಶ->ಸ್ಥಾನಾಂತರ, ಅಂಶ->ಉದ್ದಳತೆ,
		    (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s, len, &ಅಂಶ->analysis, glyphs);
#ಇಲ್ಲದಿದ್ದರೆ
	    pango_shape((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)s + ಅಂಶ->ಸ್ಥಾನಾಂತರ, ಅಂಶ->ಉದ್ದಳತೆ,
			&ಅಂಶ->analysis, glyphs);
#ಆದರೆ_ಅಂತ್ಯ
	    /*
	     * Fixed-width hack: iterate over the array and assign a fixed
	     * width to each glyph, thus overriding the choice made by the
	     * shaping engine.	We use utf_char2cells() to determine the
	     * number of cells needed.
	     *
	     * Also perform all kind of dark magic to get composing
	     * characters right (and pretty too of course).
	     */
	    ಗಾಗಿ (i = 0; i < glyphs->ಅಂಕೆ_glyphs; ++i)
	    {
		PangoGlyphInfo *glyph;

		glyph = &glyphs->glyphs[i];

		ಆದರೆ (glyph->attr.is_cluster_ಆರಂಭ)
		{
		    ಪೂರ್ಣಾಂಕ cellcount;

		    cellcount = ಎಣಿಕೆ_cluster_cells(
			s, ಅಂಶ, glyphs, i, &cluster_ಅಗಲ,
			(ಅಂಶ_list != ಖಾಲಿ) ? &ಕೊನೆಯ_glyph_rbearing : ಖಾಲಿ);

		    ಆದರೆ (cellcount > 0)
		    {
			ಪೂರ್ಣಾಂಕ ಅಗಲ;

			ಅಗಲ = cellcount * gui.char_ಅಗಲ * PANGO_SCALE;
			glyph->geometry.x_ಸ್ಥಾನಾಂತರ +=
					    MAX(0, ಅಗಲ - cluster_ಅಗಲ) / 2;
			glyph->geometry.ಅಗಲ = ಅಗಲ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
		    {
			// If there are only combining characters in the
			// cluster, we cannot just change the width of the
			// previous glyph since there is none.	Therefore
			// some guesswork is needed.
			ಸಿದ್ಧತೆ_zero_ಅಗಲ_cluster(ಅಂಶ, glyph, cells,
						 cluster_ಅಗಲ,
						 ಕೊನೆಯ_glyph_rbearing);
		    }

		    ಅಂಶ_cells += cellcount;
		    cells = cellcount;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (i > 0)
		{
		    ಪೂರ್ಣಾಂಕ ಅಗಲ;

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
		    ಆದರೆ (glyph->geometry.x_ಸ್ಥಾನಾಂತರ >= 0)
		    {
			glyphs->glyphs[i].geometry.ಅಗಲ =
					 glyphs->glyphs[i - 1].geometry.ಅಗಲ;
			glyphs->glyphs[i - 1].geometry.ಅಗಲ = 0;
		    }
		    ಅಗಲ = cells * gui.char_ಅಗಲ * PANGO_SCALE;
		    glyph->geometry.x_ಸ್ಥಾನಾಂತರ +=
					    MAX(0, ಅಗಲ - cluster_ಅಗಲ) / 2;
		}
		ಇಲ್ಲದಿದ್ದರೆ // i == 0 "cannot happen"
		{
		    glyph->geometry.ಅಗಲ = 0;
		}
	    }

	    //// Aaaaand action! **
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    draw_glyph_string(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ, ಅಂಶ_cells,
			      flags, ಅಂಶ->analysis.font, glyphs,
			      cr);
#ಇಲ್ಲದಿದ್ದರೆ
	    draw_glyph_string(ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು + ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ, ಅಂಶ_cells,
			      flags, ಅಂಶ->analysis.font, glyphs);
#ಆದರೆ_ಅಂತ್ಯ

	    pango_ಅಂಶ_free(ಅಂಶ);

	    ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ += ಅಂಶ_cells;
	}

	pango_attr_list_unref(attr_list);
    }

skipitall:
    // Draw underline and undercurl.
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    draw_under(flags, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ, cr);
#ಇಲ್ಲದಿದ್ದರೆ
    draw_under(flags, ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು, ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ);
#ಆದರೆ_ಅಂತ್ಯ

    pango_glyph_string_free(glyphs);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_destroy(cr);
    ಸರತಿ_draw_area(FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು),
	    ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ * gui.char_ಅಗಲ + 1, gui.char_ಎತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_gc_ಗಣ_clip_rectangle(gui.ಪಠ್ಯ_gc, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ ಸ್ತಂಭ_ಸ್ಥಾನಾಂತರ;
}

/*
 * Return OK if the key with the termcap name "name" is supported.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_haskey(char_u *name)
{
    ಪೂರ್ಣಾಂಕ i;

    ಗಾಗಿ (i = 0; special_ಕೀಲಿಗಳು[i].ಕೀಲಿ_sym != 0; i++)
	ಆದರೆ (name[0] == special_ಕೀಲಿಗಳು[i].code0
		&& name[1] == special_ಕೀಲಿಗಳು[i].code1)
	    ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Return the text window-id and display.  Only required for X-based GUI's
 */
    ಪೂರ್ಣಾಂಕ
gui_get_x11_windis(ಕಿಟಕಿ *win, Display **dis)
{
    Display * dpy = gui_mch_get_display();
    ಆದರೆ (dpy)
    {
	*dis = dpy;
	*win = GDK_ಕಿಟಕಿ_XID(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin));
	ಹಿಂತಿರುಗಿ OK;
    }

    *dis = ಖಾಲಿ;
    *win = 0;
    ಹಿಂತಿರುಗಿ FAIL;
}
#ಆದರೆ_ಅಂತ್ಯ

    Display *
gui_mch_get_display(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_ಕಿಟಕಿ(gui.mainwin) != ಖಾಲಿ
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	    && GDK_IS_X11_DISPLAY(gtk_widget_get_display(gui.mainwin))
#ಆದರೆ_ಅಂತ್ಯ
	)
	ಹಿಂತಿರುಗಿ GDK_ಕಿಟಕಿ_XDISPLAY(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin));
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_beep(ಶೂನ್ಯ)
{
    GdkDisplay *display;

    ಆದರೆ (gui.mainwin != ಖಾಲಿ && gtk_widget_get_realized(gui.mainwin))
	display = gtk_widget_get_display(gui.mainwin);
    ಇಲ್ಲದಿದ್ದರೆ
	display = gdk_display_get_default();

    ಆದರೆ (display != ಖಾಲಿ)
	gdk_display_beep(display);
}

    ಶೂನ್ಯ
gui_mch_flash(ಪೂರ್ಣಾಂಕ msec)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    // TODO Replace GdkGC with Cairo
    (ಶೂನ್ಯ)msec;
#ಇಲ್ಲದಿದ್ದರೆ
    GdkGCValues	ಮೌಲ್ಯಗಳು;
    GdkGC	*invert_gc;

    ಆದರೆ (gui.drawarea->ಕಿಟಕಿ_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಮೌಲ್ಯಗಳು.foreground.pixel = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    ಮೌಲ್ಯಗಳು.background.pixel = gui.norm_pixel ^ gui.ಹಿಂಭಾಗ_pixel;
    ಮೌಲ್ಯಗಳು.function = GDK_XOR;
    invert_gc = gdk_gc_new_with_ಮೌಲ್ಯಗಳು(gui.drawarea->ಕಿಟಕಿ_2,
				       &ಮೌಲ್ಯಗಳು,
				       GDK_GC_FOREGROUND |
				       GDK_GC_BACKGROUND |
				       GDK_GC_FUNCTION);
    gdk_gc_ಗಣ_exposures(invert_gc,
			 gui.visibility != GDK_VISIBILITY_UNOBSCURED);
    /*
     * Do a visual beep by changing back and forth in some undetermined way,
     * the foreground and background colors.  This is due to the fact that
     * there can't be really any prediction about the effects of XOR on
     * arbitrary X11 servers. However this seems to be enough for what we
     * intend it to do.
     */
    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((ಪೂರ್ಣಾಂಕ)Columns) + gui.border_ಸ್ಥಾನಾಂತರ,
		       FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು) + gui.border_ಸ್ಥಾನಾಂತರ);

    gui_mch_flush();
    ui_delay((ದೀರ್ಘ)msec, TRUE);	// wait so many msec

    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, invert_gc,
		       TRUE,
		       0, 0,
		       FILL_X((ಪೂರ್ಣಾಂಕ)Columns) + gui.border_ಸ್ಥಾನಾಂತರ,
		       FILL_Y((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು) + gui.border_ಸ್ಥಾನಾಂತರ);

    gdk_gc_destroy(invert_gc);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Invert a rectangle from row r, column c, for nr rows and nc columns.
 */
    ಶೂನ್ಯ
gui_mch_invert_rectangle(ಪೂರ್ಣಾಂಕ r, ಪೂರ್ಣಾಂಕ c, ಪೂರ್ಣಾಂಕ nr, ಪೂರ್ಣಾಂಕ nc)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರಾಂಕ GdkRectangle rect = {
	FILL_X(c), FILL_Y(r), nc * gui.char_ಅಗಲ, nr * gui.char_ಎತ್ತರ
    };
    cairo_t * ಸ್ಥಿರಾಂಕ cr = cairo_create(gui.surface);

    cairo_ಗಣ_ಮೂಲ_rgba(cr, 1.0, 1.0, 1.0, 1.0);
# ಆದರೆ CAIRO_ಆವೃತ್ತಿ >= CAIRO_ಆವೃತ್ತಿ_ENCODE(1,9,2)
    cairo_ಗಣ_operator(cr, CAIRO_OPERATOR_DIFFERENCE);
# ಇಲ್ಲದಿದ್ದರೆ
    // Give an implementation for older cairo versions if necessary.
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
    cairo_fill(cr);

    cairo_destroy(cr);

    ಸರತಿ_draw_area(rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
#ಇಲ್ಲದಿದ್ದರೆ
    GdkGCValues ಮೌಲ್ಯಗಳು;
    GdkGC *invert_gc;

    ಆದರೆ (gui.drawarea->ಕಿಟಕಿ_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಮೌಲ್ಯಗಳು.function = GDK_INVERT;
    invert_gc = gdk_gc_new_with_ಮೌಲ್ಯಗಳು(gui.drawarea->ಕಿಟಕಿ_2,
				       &ಮೌಲ್ಯಗಳು,
				       GDK_GC_FUNCTION);
    gdk_gc_ಗಣ_exposures(invert_gc, gui.visibility !=
						   GDK_VISIBILITY_UNOBSCURED);
    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, invert_gc,
		       TRUE,
		       FILL_X(c), FILL_Y(r),
		       (nc) * gui.char_ಅಗಲ, (nr) * gui.char_ಎತ್ತರ);
    gdk_gc_destroy(invert_gc);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Iconify the GUI window.
 */
    ಶೂನ್ಯ
gui_mch_iconify(ಶೂನ್ಯ)
{
    gtk_ಕಿಟಕಿ_iconify(GTK_ಕಿಟಕಿ(gui.mainwin));
}

#ಆದರೆ defined(FEAT_EVAL)
/*
 * Bring the Vim window to the foreground.
 */
    ಶೂನ್ಯ
gui_mch_ಗಣ_foreground(ಶೂನ್ಯ)
{
    // Just calling gtk_window_present() used to work in the past, but now this
    // sequence appears to be needed:
    // - Show the window on top of others.
    // - Present the window (also shows it above others).
    // - Do not the window on top of others (otherwise it would be stuck there).
    gtk_ಕಿಟಕಿ_ಗಣ_keep_above(GTK_ಕಿಟಕಿ(gui.mainwin), TRUE);
    gui_may_flush();
    gtk_ಕಿಟಕಿ_present(GTK_ಕಿಟಕಿ(gui.mainwin));
    gui_may_flush();
    gtk_ಕಿಟಕಿ_ಗಣ_keep_above(GTK_ಕಿಟಕಿ(gui.mainwin), FALSE);
    gui_may_flush();
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Draw a cursor without focus.
 */
    ಶೂನ್ಯ
gui_mch_draw_hollow_cursor(guicolor_T color)
{
    ಪೂರ್ಣಾಂಕ		i = 1;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_t    *cr;
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cr = cairo_create(gui.surface);
#ಆದರೆ_ಅಂತ್ಯ

    gui_mch_ಗಣ_fg_color(color);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_ಗಣ_ಮೂಲ_rgba(cr,
	    gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
	    gui.fgcolor->alpha);
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (mb_lefthalve(gui.ಅಡ್ಡಸಾಲು, gui.ಲಂಬಸಾಲು))
	i = 2;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    cairo_ಗಣ_ಸಾಲು_ಅಗಲ(cr, 1.0);
    cairo_ಗಣ_ಸಾಲು_cap(cr, CAIRO_ಸಾಲು_CAP_BUTT);
    cairo_rectangle(cr,
	    FILL_X(gui.ಲಂಬಸಾಲು) + 0.5, FILL_Y(gui.ಅಡ್ಡಸಾಲು) + 0.5,
	    i * gui.char_ಅಗಲ - 1, gui.char_ಎತ್ತರ - 1);
    cairo_stroke(cr);
    cairo_destroy(cr);
#ಇಲ್ಲದಿದ್ದರೆ
    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
	    FALSE,
	    FILL_X(gui.ಲಂಬಸಾಲು), FILL_Y(gui.ಅಡ್ಡಸಾಲು),
	    i * gui.char_ಅಗಲ - 1, gui.char_ಎತ್ತರ - 1);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Draw part of a cursor, "w" pixels wide, and "h" pixels high, using
 * color "color".
 */
    ಶೂನ್ಯ
gui_mch_draw_part_cursor(ಪೂರ್ಣಾಂಕ w, ಪೂರ್ಣಾಂಕ h, guicolor_T color)
{
    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    gui_mch_ಗಣ_fg_color(color);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	cairo_t *cr;

	cr = cairo_create(gui.surface);
	cairo_ಗಣ_ಮೂಲ_rgba(cr,
		gui.fgcolor->red, gui.fgcolor->green, gui.fgcolor->blue,
		gui.fgcolor->alpha);
	cairo_rectangle(cr,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ಬಲ ? FILL_X(gui.ಲಂಬಸಾಲು + 1) - w :
# endif
	    FILL_X(gui.ಲಂಬಸಾಲು), FILL_Y(gui.ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h,
	    w, h);
	cairo_fill(cr);
	cairo_destroy(cr);
    }
#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
	    TRUE,
# ifdef FEAT_RIGHTLEFT
	    // vertical line should be on the right of current point
	    CURSOR_BAR_ಬಲ ? FILL_X(gui.ಲಂಬಸಾಲು + 1) - w :
# endif
	    FILL_X(gui.ಲಂಬಸಾಲು),
	    FILL_Y(gui.ಅಡ್ಡಸಾಲು) + gui.char_ಎತ್ತರ - h,
	    w, h);
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
}


/*
 * Catch up with any queued X11 events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X11 event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
gui_mch_update(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ cnt = 0;	// prevent endless loop
    ಆಗ (g_main_context_pending(ಖಾಲಿ) && !vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ()
	    && ++cnt < 100)
    {
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	ಪೂರ್ಣಾಂಕ prio = 0;
	g_main_context_prepare(ಖಾಲಿ, &prio);
	// peek internal scheduling of redraw
	ಆದರೆ (prio == GDK_ಆದ್ಯತೆ_REDRAW)
	    gui_may_flush(); // prepares redraw: g_main_context_iteration
#ಆದರೆ_ಅಂತ್ಯ
	g_main_context_iteration(ಖಾಲಿ, TRUE);
    }
}

    ಸ್ಥಿರ timeout_cb_type
ಒಳಹರಿವು_timer_cb(gpointer ದತ್ತಾಂಶ)
{
    ಪೂರ್ಣಾಂಕ *timed_ಹೊರಹೋಗು = (ಪೂರ್ಣಾಂಕ *) ದತ್ತಾಂಶ;

    // Just inform the caller about the occurrence of it
    *timed_ಹೊರಹೋಗು = TRUE;

    ಹಿಂತಿರುಗಿ FALSE;		// don't happen again
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಸ್ಥಿರ timeout_cb_type
channel_poll_cb(gpointer ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Using an event handler for a channel that may be disconnected does
    // not work, it hangs.  Instead poll for messages.
    channel_handle_events(TRUE);
    parse_queued_messages();

    ಹಿಂತಿರುಗಿ TRUE;		// repeat
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GUI input routine called by gui_wait_for_chars().  Waits for a character
 * from the keyboard.
 *  wtime == -1     Wait forever.
 *  wtime == 0	    This should never happen.
 *  wtime > 0	    Wait wtime milliseconds for a character.
 * Returns OK if a character was found to be available within the given time,
 * or FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
gui_mch_wait_for_chars(ದೀರ್ಘ wtime)
{
    ಪೂರ್ಣಾಂಕ		focus;
    guint	timer;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	timed_ಹೊರಹೋಗು;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    guint	channel_timer = 0;
#ಆದರೆ_ಅಂತ್ಯ

    timed_ಹೊರಹೋಗು = FALSE;

    // This timeout makes sure that we will return if no characters arrived in
    // time. If "wtime" is zero just use one.
    ಆದರೆ (wtime >= 0)
	timer = timeout_ಕೂಡಿಸು(wtime == 0 ? 1L : wtime,
						   ಒಳಹರಿವು_timer_cb, &timed_ಹೊರಹೋಗು);
    ಇಲ್ಲದಿದ್ದರೆ
	timer = 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    // If there is a channel with the keep_open flag we need to poll for input
    // on them.
    ಆದರೆ (channel_any_keep_open())
	channel_timer = timeout_ಕೂಡಿಸು(20, channel_poll_cb, ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ

    focus = gui.ಒಳ_focus;

    ಮಾಡು
    {
	// Stop or start blinking when focus changes
	ಆದರೆ (gui.ಒಳ_focus != focus)
	{
	    ಆದರೆ (gui.ಒಳ_focus)
		gui_mch_ಆರಂಭ_blink();
	    ಇಲ್ಲದಿದ್ದರೆ
		gui_mch_stop_blink(TRUE);
	    focus = gui.ಒಳ_focus;
	}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಸಂದೇಶ_ಸರತಿ
# ifdef FEAT_TIMERS
	did_ಕೂಡಿಸು_timer = FALSE;
# endif
	parse_queued_messages();
# ifdef FEAT_TIMERS
	ಆದರೆ (did_ಕೂಡಿಸು_timer)
	    // Need to recompute the waiting time.
	    ಹೋಗು theend;
# endif
#ಆದರೆ_ಅಂತ್ಯ

	/*
	 * Loop in GTK+ processing  until a timeout or input occurs.
	 * Skip this if input is available anyway (can happen in rare
	 * situations, sort of race condition).
	 */
	ಆದರೆ (!ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    g_main_context_iteration(ಖಾಲಿ, TRUE);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ GDK_WINDOWING_WAYLAND
    ಆದರೆ (gui.is_wayland && clip_ನಕ್ಷತ್ರ.state == SELECT_ಒಳ_ಪ್ರಗತಿ)
	gui_may_flush();
#ಆದರೆ_ಅಂತ್ಯ
	}

	// Got char, return immediately
	ಆದರೆ (ಒಳಹರಿವು_ಲಭ್ಯ())
	{
	    retval = OK;
	    ಹೋಗು theend;
	}
    } ಆಗ (wtime < 0 || !timed_ಹೊರಹೋಗು);

    /*
     * Flush all eventually pending (drawing) events.
     */
    gui_mch_update();

theend:
    ಆದರೆ (timer != 0 && !timed_ಹೊರಹೋಗು)
	timeout_remove(timer);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಕೆಲಸ_CHANNEL
    ಆದರೆ (channel_timer != 0)
	timeout_remove(channel_timer);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ retval;
}


/////////////////////////////////////////////////////////////////////////////
// Output drawing routines.
//


// Flush any output to the screen
    ಶೂನ್ಯ
gui_mch_flush(ಶೂನ್ಯ)
{
    ಆದರೆ (gui.mainwin == ಖಾಲಿ || !gtk_widget_get_realized(gui.mainwin))
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಆದರೆ (dirty_region != ಖಾಲಿ && gui.drawarea != ಖಾಲಿ)
    {
	gtk_widget_ಸರತಿ_draw_region(gui.drawarea, dirty_region);
	cairo_region_destroy(dirty_region);
	dirty_region = ಖಾಲಿ;
    }
#ಇಲ್ಲದಿದ್ದರೆ
       gdk_display_flush(gtk_widget_get_display(gui.mainwin));
       ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Clear a rectangular region of the screen from text pos (row1, col1) to
 * (row2, col2) inclusive.
 */
    ಶೂನ್ಯ
gui_mch_clear_ಖಂಡ(ಪೂರ್ಣಾಂಕ row1arg, ಪೂರ್ಣಾಂಕ col1arg, ಪೂರ್ಣಾಂಕ row2arg, ಪೂರ್ಣಾಂಕ col2arg)
{

    ಪೂರ್ಣಾಂಕ col1 = check_ಲಂಬಸಾಲು(col1arg);
    ಪೂರ್ಣಾಂಕ col2 = check_ಲಂಬಸಾಲು(col2arg);
    ಪೂರ್ಣಾಂಕ row1 = check_ಅಡ್ಡಸಾಲು(row1arg);
    ಪೂರ್ಣಾಂಕ row2 = check_ಅಡ್ಡಸಾಲು(row2arg);

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
#ಇಲ್ಲದಿದ್ದರೆ
    GdkColor color;

    ಆದರೆ (gui.drawarea->ಕಿಟಕಿ_2 == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    color.pixel = gui.ಹಿಂಭಾಗ_pixel;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	// Add one pixel to the far right column in case a double-stroked
	// bold glyph may sit there.
	ಸ್ಥಿರಾಂಕ GdkRectangle rect = {
	    FILL_X(col1), FILL_Y(row1),
	    (col2 - col1 + 1) * gui.char_ಅಗಲ + (col2 == Columns - 1),
	    (row2 - row1 + 1) * gui.char_ಎತ್ತರ
	};
	cairo_t * ಸ್ಥಿರಾಂಕ cr = cairo_create(gui.surface);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
	ಗಣ_cairo_ಮೂಲ_rgba_from_color(cr, gui.ಹಿಂಭಾಗ_pixel);
# ಇಲ್ಲದಿದ್ದರೆ
	Gdk_ಕಿಟಕಿ * ಸ್ಥಿರಾಂಕ win = gtk_widget_get_ಕಿಟಕಿ(gui.drawarea);
	cairo_ನಮೂನೆ_t * ಸ್ಥಿರಾಂಕ pat = gdk_ಕಿಟಕಿ_get_background_ನಮೂನೆ(win);
	ಆದರೆ (pat != ಖಾಲಿ)
	    cairo_ಗಣ_ಮೂಲ(cr, pat);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಗಣ_cairo_ಮೂಲ_rgba_from_color(cr, gui.ಹಿಂಭಾಗ_pixel);
# endif
	cairo_rectangle(cr, rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
	cairo_fill(cr);
	cairo_destroy(cr);

	ಸರತಿ_draw_area(rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
    }
#ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, &color);

    // Clear one extra pixel at the far right, for when bold characters have
    // spilled over to the window border.
    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc, TRUE,
		       FILL_X(col1), FILL_Y(row1),
		       (col2 - col1 + 1) * gui.char_ಅಗಲ
						      + (col2 == Columns - 1),
		       (row2 - row1 + 1) * gui.char_ಎತ್ತರ);
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
}

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_ಕಿಟಕಿ_clear(Gdk_ಕಿಟಕಿ *win)
{
    ಸ್ಥಿರಾಂಕ GdkRectangle rect = {
	0, 0, gdk_ಕಿಟಕಿ_get_ಅಗಲ(win), gdk_ಕಿಟಕಿ_get_ಎತ್ತರ(win)
    };
    cairo_t * ಸ್ಥಿರಾಂಕ cr = cairo_create(gui.surface);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,22,2)
    ಗಣ_cairo_ಮೂಲ_rgba_from_color(cr, gui.ಹಿಂಭಾಗ_pixel);
# ಇಲ್ಲದಿದ್ದರೆ
    cairo_ನಮೂನೆ_t * ಸ್ಥಿರಾಂಕ pat = gdk_ಕಿಟಕಿ_get_background_ನಮೂನೆ(win);
    ಆದರೆ (pat != ಖಾಲಿ)
	cairo_ಗಣ_ಮೂಲ(cr, pat);
    ಇಲ್ಲದಿದ್ದರೆ
	ಗಣ_cairo_ಮೂಲ_rgba_from_color(cr, gui.ಹಿಂಭಾಗ_pixel);
# endif
    cairo_rectangle(cr, rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
    cairo_fill(cr);
    cairo_destroy(cr);

    ಸರತಿ_draw_area(rect.x, rect.y, rect.ಅಗಲ, rect.ಎತ್ತರ);
}
#ಇಲ್ಲದಿದ್ದರೆ
# define gui_gtk_ಕಿಟಕಿ_clear(win)  gdk_ಕಿಟಕಿ_clear(win)
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
gui_mch_clear_all(ಶೂನ್ಯ)
{
    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) != ಖಾಲಿ)
	gui_gtk_ಕಿಟಕಿ_clear(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea));
}

#ಆದರೆ !GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
/*
 * Redraw any text revealed by scrolling up/down.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_copy_area(ಶೂನ್ಯ)
{
    GdkEvent        *event;
    ಪೂರ್ಣಾಂಕ                expose_ಎಣಿಕೆ;
    ಆದರೆ (gui.visibility != GDK_VISIBILITY_PARTIAL)
	ಹಿಂತಿರುಗಿ;
    // Avoid redrawing the cursor while scrolling or it'll end up where
    // we don't want it to be.        I'm not sure if it's correct to call
    // gui_dont_update_cursor() at this point but it works as a quick
    // fix for now.
    gui_dont_update_cursor(TRUE);
    ಮಾಡು
    {
	// Wait to check whether the scroll worked or not.
	event = gdk_event_get_graphics_expose(gui.drawarea->ಕಿಟಕಿ_2);
	ಆದರೆ (event == ಖಾಲಿ)
	    ನಿಲ್ಲಿಸು; // received NoExpose event
	gui_redraw(event->expose.area.x, event->expose.area.y,
		event->expose.area.ಅಗಲ, event->expose.area.ಎತ್ತರ);
	expose_ಎಣಿಕೆ = event->expose.ಎಣಿಕೆ_2;
	gdk_event_free(event);
    }
    ಆಗ (expose_ಎಣಿಕೆ > 0); // more events follow
    gui_can_update_cursor();
}
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)

#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರ ಶೂನ್ಯ
gui_gtk_surface_copy_rect(ಪೂರ್ಣಾಂಕ dest_x, ಪೂರ್ಣಾಂಕ dest_y,
			  ಪೂರ್ಣಾಂಕ src_x,  ಪೂರ್ಣಾಂಕ src_y,
			  ಪೂರ್ಣಾಂಕ ಅಗಲ,  ಪೂರ್ಣಾಂಕ ಎತ್ತರ)
{
    cairo_t * ಸ್ಥಿರಾಂಕ cr = cairo_create(gui.surface);

# ifdef GDK_WINDOWING_WAYLAND
    ಸ್ಥಿರ cairo_surface_t *scroll_scratch = ಖಾಲಿ;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ scratch_w = 0, scratch_h = 0;
    ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1, ಕೊನೆಯ_ಅಡ್ಡಸಾಲು_y = ಕೊನೆಯ_ಅಡ್ಡಸಾಲು * gui.char_ಎತ್ತರ;
    bool ಕೊನೆಯ_ಅಡ್ಡಸಾಲು_overlap = (dest_y + ಎತ್ತರ) > ಕೊನೆಯ_ಅಡ್ಡಸಾಲು_y;
    ಆದರೆ (gui.is_wayland && ( !(State & ಬಹುಲಕ_CMDLINE) || !ಕೊನೆಯ_ಅಡ್ಡಸಾಲು_overlap) )
    {
	ಆದರೆ (dest_y < src_y) // scroll up
	{
	    cairo_ಗಣ_operator(cr, CAIRO_OPERATOR_ಮೂಲ);
	    cairo_ಗಣ_ಮೂಲ_surface(cr, gui.surface, src_x - dest_x,
		    dest_y - src_y);
	    cairo_rectangle(cr, dest_x, dest_y, ಅಗಲ, ಎತ್ತರ);
	    cairo_clip(cr);
	    cairo_paint(cr);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{   //  reuse surface when scrolling, only realloc if larger
	    ಆದರೆ (scroll_scratch == ಖಾಲಿ || ಅಗಲ > scratch_w || ಎತ್ತರ > scratch_h)
	    {
		cairo_surface_destroy(scroll_scratch); // safe even if NULL
		scroll_scratch = cairo_surface_create_similar(gui.surface,
			cairo_surface_get_content(gui.surface), ಅಗಲ, ಎತ್ತರ);
		scratch_w = ಅಗಲ, scratch_h = ಎತ್ತರ;
	    }
	    // capture scroll source region
	    cairo_t *tcr = cairo_create(scroll_scratch);
	    cairo_ಗಣ_ಮೂಲ_surface(tcr, gui.surface, -src_x, -src_y);
	    cairo_paint(tcr);
	    cairo_destroy(tcr);
	    // reuse scroll source region
	    cairo_ಗಣ_operator(cr, CAIRO_OPERATOR_ಮೂಲ);
	    cairo_rectangle(cr, dest_x, dest_y, ಅಗಲ, ಎತ್ತರ);
	    cairo_clip(cr);
	    cairo_ಗಣ_ಮೂಲ_surface(cr, scroll_scratch, dest_x, dest_y);
	    cairo_paint(cr);
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	cairo_rectangle(cr, dest_x, dest_y, ಅಗಲ, ಎತ್ತರ);
	cairo_clip(cr);
	cairo_ತಳ್ಳು_ಗುಂಪು(cr);
	cairo_ಗಣ_ಮೂಲ_surface(cr, gui.surface, dest_x - src_x, dest_y - src_y);
	cairo_paint(cr);
	cairo_ತೆಗೆ_ಗುಂಪು_ಗಮ್ಯ_ಮೂಲ(cr);
	cairo_paint(cr);
    }
    cairo_destroy(cr);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Delete the given number of lines from the given row, scrolling up any
 * text further down within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_delete_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ncols = gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nrows = gui.scroll_region_bot - ಅಡ್ಡಸಾಲು + 1;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ src_nrows = nrows - ಅಂಕೆ_ಸಾಲುಗಳು;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು),
	    gui.char_ಅಗಲ * ncols + 1,     gui.char_ಎತ್ತರ * src_nrows);
    gui_clear_ಖಂಡ(
	    gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1, gui.scroll_region_ಎಡ,
	    gui.scroll_region_bot,		   gui.scroll_region_ಬಲ);
    ಸರತಿ_draw_area(
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    gui.char_ಅಗಲ * ncols + 1,	gui.char_ಎತ್ತರ * nrows);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	ಹಿಂತಿರುಗಿ;			// Can't see the window

    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
    gdk_gc_ಗಣ_background(gui.ಪಠ್ಯ_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_ಕಿಟಕಿ_copy_area(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    gui.drawarea->ಕಿಟಕಿ_2,
	    FILL_X(gui.scroll_region_ಎಡ),
	    FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು),
	    gui.char_ಅಗಲ * (gui.scroll_region_ಬಲ
					    - gui.scroll_region_ಎಡ + 1) + 1,
	    gui.char_ಎತ್ತರ * (gui.scroll_region_bot - ಅಡ್ಡಸಾಲು - ಅಂಕೆ_ಸಾಲುಗಳು + 1));

    gui_clear_ಖಂಡ(gui.scroll_region_bot - ಅಂಕೆ_ಸಾಲುಗಳು + 1,
						       gui.scroll_region_ಎಡ,
		    gui.scroll_region_bot, gui.scroll_region_ಬಲ);
    check_copy_area();
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * Insert the given number of lines before the given row, scrolling down any
 * following text within the scroll region.
 */
    ಶೂನ್ಯ
gui_mch_ಸೇರಿಸು_ಸಾಲುಗಳು(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಅಂಕೆ_ಸಾಲುಗಳು)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ ncols = gui.scroll_region_ಬಲ - gui.scroll_region_ಎಡ + 1;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ nrows = gui.scroll_region_bot - ಅಡ್ಡಸಾಲು + 1;
    ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ src_nrows = nrows - ಅಂಕೆ_ಸಾಲುಗಳು;

    gui_gtk_surface_copy_rect(
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು),
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    gui.char_ಅಗಲ * ncols + 1,     gui.char_ಎತ್ತರ * src_nrows);
    gui_clear_ಖಂಡ(
	    ಅಡ್ಡಸಾಲು,		 gui.scroll_region_ಎಡ,
	    ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
    ಸರತಿ_draw_area(
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    gui.char_ಅಗಲ * ncols + 1,	gui.char_ಎತ್ತರ * nrows);
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (gui.visibility == GDK_VISIBILITY_FULLY_OBSCURED)
	ಹಿಂತಿರುಗಿ;			// Can't see the window

    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.fgcolor);
    gdk_gc_ಗಣ_background(gui.ಪಠ್ಯ_gc, gui.bgcolor);

    // copy one extra pixel, for when bold has spilled over
    gdk_ಕಿಟಕಿ_copy_area(gui.drawarea->ಕಿಟಕಿ_2, gui.ಪಠ್ಯ_gc,
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು),
	    gui.drawarea->ಕಿಟಕಿ_2,
	    FILL_X(gui.scroll_region_ಎಡ), FILL_Y(ಅಡ್ಡಸಾಲು),
	    gui.char_ಅಗಲ * (gui.scroll_region_ಬಲ
					    - gui.scroll_region_ಎಡ + 1) + 1,
	    gui.char_ಎತ್ತರ * (gui.scroll_region_bot - ಅಡ್ಡಸಾಲು - ಅಂಕೆ_ಸಾಲುಗಳು + 1));

    gui_clear_ಖಂಡ(ಅಡ್ಡಸಾಲು, gui.scroll_region_ಎಡ,
				ಅಡ್ಡಸಾಲು + ಅಂಕೆ_ಸಾಲುಗಳು - 1, gui.scroll_region_ಬಲ);
    check_copy_area();
#ಆದರೆ_ಅಂತ್ಯ // !GTK_CHECK_VERSION(3,0,0)
}

/*
 * X Selection stuff, for cutting and pasting text to other windows.
 */
    ಶೂನ್ಯ
clip_mch_request_selection(Clipboard_T *cbd)
{
    GdkAtom	ಗುರಿ;
    ಚಿಹ್ನೆರಹಿತ	i;
    time_t	ಆರಂಭ;

    ಗಾಗಿ (i = 0; i < N_SELECTION_TARGETS; ++i)
    {
	ಆದರೆ (!clip_html && selection_targets[i].info == ಗುರಿ_HTML)
	    ಮುಂದುವರಿ;
	received_selection = RS_NONE;
	ಗುರಿ = gdk_atom_intern(selection_targets[i].ಗುರಿ, FALSE);

	gtk_selection_convert(gui.drawarea,
			      cbd->gtk_sel_atom, ಗುರಿ,
			      (guint32)GDK_CURRENT_ಸಮಯ);

	// Hack: Wait up to three seconds for the selection.  A hang was
	// noticed here when using the netrw plugin combined with ":gui"
	// during the FocusGained event.
	ಆರಂಭ = ಸಮಯ(ಖಾಲಿ);
	ಆಗ (received_selection == RS_NONE && ಸಮಯ(ಖಾಲಿ) < ಆರಂಭ + 3)
	    g_main_context_iteration(ಖಾಲಿ, TRUE);	// wait for selection_received_cb

	ಆದರೆ (received_selection != RS_FAIL)
	    ಹಿಂತಿರುಗಿ;
    }

    ಆದರೆ (gui_mch_get_display())
	// Final fallback position - use the X CUT_BUFFER0 store
	yank_cut_buffer0(GDK_ಕಿಟಕಿ_XDISPLAY(gtk_widget_get_ಕಿಟಕಿ(gui.mainwin)),
		cbd);
}

/*
 * Disown the selection.
 */
    ಶೂನ್ಯ
clip_mch_lose_selection(Clipboard_T *cbd ಬಳಸದ)
{
    ಆದರೆ (ಒಳ_selection_clear_event)
	ಹಿಂತಿರುಗಿ;

    gtk_selection_owner_ಗಣ(ಖಾಲಿ, cbd->gtk_sel_atom, gui.event_ಸಮಯ);
    gui_mch_update();
}

/*
 * Own the selection and return OK if it worked.
 */
    ಪೂರ್ಣಾಂಕ
clip_mch_own_selection(Clipboard_T *cbd)
{
    // If we're blocking autocmds, we are filling the register to offer the
    // selection (inside selection-get)
    ಆದರೆ (is_autocmd_blocked())
	ಹಿಂತಿರುಗಿ OK;

    ಪೂರ್ಣಾಂಕ success;

    success = gtk_selection_owner_ಗಣ(gui.drawarea, cbd->gtk_sel_atom,
				      gui.event_ಸಮಯ);
    // don't update on every visual selection change
    ಆದರೆ (!(cbd->owned && VIsual_ಸಕ್ರಿಯ))
	gui_gtk_ಗಣ_selection_targets(cbd->gtk_sel_atom);
    gui_mch_update();
    ಹಿಂತಿರುಗಿ (success) ? OK : FAIL;
}

/*
 * Send the current selection to the clipboard.  Do nothing for X because we
 * will fill in the selection only when requested by another app.
 */
    ಶೂನ್ಯ
clip_mch_ಗಣ_selection(Clipboard_T *cbd ಬಳಸದ)
{
}

#ಆದರೆ defined(FEAT_XCLIPBOARD) && defined(USE_SYSTEM)
    ಪೂರ್ಣಾಂಕ
clip_gtk_owner_exists(Clipboard_T *cbd)
{
    ಹಿಂತಿರುಗಿ gdk_selection_owner_get(cbd->gtk_sel_atom) != ಖಾಲಿ;
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_MENU)
/*
 * Make a menu item appear either active or not active (grey or not grey).
 */
    ಶೂನ್ಯ
gui_mch_menu_grey(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ grey)
{
    ಆದರೆ (menu->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (menu_is_separator(menu->name))
	grey = TRUE;

    gui_mch_menu_hidden(menu, FALSE);
    // Be clever about bitfields versus true booleans here!
    ಆದರೆ (!gtk_widget_get_sensitive(menu->id) == !grey)
    {
	gtk_widget_ಗಣ_sensitive(menu->id, !grey);
	gui_mch_update();
    }
}

/*
 * Make menu item hidden or not hidden.
 */
    ಶೂನ್ಯ
gui_mch_menu_hidden(vimmenu_T *menu, ಪೂರ್ಣಾಂಕ hidden)
{
    ಆದರೆ (menu->id == 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (hidden)
    {
	ಆದರೆ (gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_hide(menu->id);
	    gui_mch_update();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (!gtk_widget_get_visible(menu->id))
	{
	    gtk_widget_show(menu->id);
	    gui_mch_update();
	}
    }
}

/*
 * This is called after setting all the menus to grey/hidden or not.
 */
    ಶೂನ್ಯ
gui_mch_draw_menubar(ಶೂನ್ಯ)
{
    // just make sure that the visual changes get effect immediately
    gui_mch_update();
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MENU

/*
 * Scrollbar stuff.
 */
    ಶೂನ್ಯ
gui_mch_enable_scrollbar(scrollbar_T *sb, ಪೂರ್ಣಾಂಕ flag)
{
    ಆದರೆ (sb->id == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    gtk_widget_ಗಣ_visible(sb->id, flag);
    update_ಕಿಟಕಿ_manager_hints(0, 0);
}


/*
 * Return the RGB value of a pixel as long.
 */
    guicolor_T
gui_mch_get_rgb(guicolor_T pixel)
{
#ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಹಿಂತಿರುಗಿ (long_u)pixel;
#ಇಲ್ಲದಿದ್ದರೆ
    GdkColor color;

    gdk_colormap_query_color(gtk_widget_get_colormap(gui.drawarea),
			     (ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)pixel, &color);

    ಹಿಂತಿರುಗಿ (guicolor_T)(
	    (((ಚಿಹ್ನೆರಹಿತ)color.red   & 0xff00) << 8)
	 |  ((ಚಿಹ್ನೆರಹಿತ)color.green & 0xff00)
	 | (((ಚಿಹ್ನೆರಹಿತ)color.blue  & 0xff00) >> 8));
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Get current mouse coordinates in text window.
 */
    ಶೂನ್ಯ
gui_mch_getmouse(ಪೂರ್ಣಾಂಕ *x, ಪೂರ್ಣಾಂಕ *y)
{
    gui_gtk_get_ಸೂಚಕ(gui.drawarea, x, y, ಖಾಲಿ);
}

    ಶೂನ್ಯ
gui_mch_setmouse(ಪೂರ್ಣಾಂಕ x, ಪೂರ್ಣಾಂಕ y)
{
    // Sorry for the Xlib call, but we can't avoid it, since there is no
    // internal GDK mechanism present to accomplish this.  (and for good
    // reason...)
    Display * dpy = gui_mch_get_display();
    ಆದರೆ (dpy)
	XWarp_ಸೂಚಕ(dpy, (ಕಿಟಕಿ)0,
		GDK_ಕಿಟಕಿ_XID(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea)),
		0, 0, 0U, 0U, x, y);
}


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
// The last set mouse pointer shape is remembered, to be used when it goes
// from hidden to not hidden.
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಕೊನೆಯ_shape = 0;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Use the blank mouse pointer or not.
 *
 * hide: TRUE = use blank ptr, FALSE = use parent ptr
 */
    ಶೂನ್ಯ
gui_mch_mousehide(ಪೂರ್ಣಾಂಕ hide)
{
    ಆದರೆ (gui.ಸೂಚಕ_hidden == hide)
	ಹಿಂತಿರುಗಿ;

    gui.ಸೂಚಕ_hidden = hide;
    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) && gui.blank_ಸೂಚಕ != ಖಾಲಿ)
    {
	ಆದರೆ (hide)
	    gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea),
		    gui.blank_ಸೂಚಕ);
	ಇಲ್ಲದಿದ್ದರೆ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSESHAPE
	    mch_ಗಣ_mouse_shape(ಕೊನೆಯ_shape);
#ಇಲ್ಲದಿದ್ದರೆ
	gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea), ಖಾಲಿ);
#ಆದರೆ_ಅಂತ್ಯ
    }
}

#ಆದರೆ defined(FEAT_MOUSESHAPE)

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * mshape_css_ಹೆಸರುಗಳು[] =
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
# ಇಲ್ಲದಿದ್ದರೆ
// Table for shape IDs.  Keep in sync with the mshape_names[] table in
// misc2.c!
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಪೂರ್ಣಾಂಕ mshape_ids[] =
{
    GDK_ಎಡ_PTR,		// arrow
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
    GDK_ಬಲ_PTR,		// right-arrow
    GDK_CENTER_PTR,		// up-arrow
    GDK_ಎಡ_PTR		// last one
};
# endif // GTK_CHECK_VERSION(3,0,0)

    ಶೂನ್ಯ
mch_ಗಣ_mouse_shape(ಪೂರ್ಣಾಂಕ shape)
{
    GdkCursor	   *c;
    ಪೂರ್ಣಾಂಕ		   id;                    // Only id or css_name is used.
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ     *css_name = "default";
    GdkDisplay     *disp;
# endif

    ಆದರೆ (gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (shape == MSHAPE_HIDE || gui.ಸೂಚಕ_hidden)
	gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea),
		gui.blank_ಸೂಚಕ);
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shape >= MSHAPE_NUMBERED)
	{
	    id = shape - MSHAPE_NUMBERED;
	    ಆದರೆ (id >= GDK_ಕೊನೆಯ_CURSOR)
		id = GDK_ಎಡ_PTR;
	    ಇಲ್ಲದಿದ್ದರೆ
		id &= ~1;	// they are always even (why?)
	}
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shape < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(mshape_css_ಹೆಸರುಗಳು))
	    css_name = mshape_css_ಹೆಸರುಗಳು[shape];
# ಇಲ್ಲದಿದ್ದರೆ
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shape < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(mshape_ids))
	    id = mshape_ids[shape];
# endif
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹಿಂತಿರುಗಿ;
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	disp = gtk_widget_get_display(gui.drawarea);
	c = gdk_cursor_new_from_name(disp, css_name);
# ಇಲ್ಲದಿದ್ದರೆ
	c = gdk_cursor_new_for_display(
		gtk_widget_get_display(gui.drawarea), (GdkCursorType)id);
# endif
	gdk_ಕಿಟಕಿ_ಗಣ_cursor(gtk_widget_get_ಕಿಟಕಿ(gui.drawarea), c);
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
	g_object_unref(G_OBJECT(c));
# ಇಲ್ಲದಿದ್ದರೆ
	gdk_cursor_destroy(c); // Unref, actually.  Bloody GTK+ 1.
# endif
    }
    ಆದರೆ (shape != MSHAPE_HIDE)
	ಕೊನೆಯ_shape = shape;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSESHAPE


#ಆದರೆ defined(FEAT_SIGN_ICONS)
/*
 * Signs are currently always 2 chars wide.  With GTK+ 2, the image will be
 * scaled down if the current font is not big enough, or scaled up if the image
 * size is less than 3/4 of the maximum sign size.  With GTK+ 1, the pixmap
 * will be cut off if the current font is not big enough, or centered if it's
 * too small.
 */
# define SIGN_ಅಗಲ  (2 * gui.char_ಅಗಲ)
# define SIGN_ಎತ್ತರ (gui.char_ಎತ್ತರ)
# define SIGN_ASPECT ((ದ್ವಿಗುಣ)SIGN_ಎತ್ತರ / (ದ್ವಿಗುಣ)SIGN_ಅಗಲ)

    ಶೂನ್ಯ
gui_mch_drawsign(ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲು, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು, ಪೂರ್ಣಾಂಕ typenr)
{
    GdkPixbuf *sign;

    sign = (GdkPixbuf *)sign_get_image(typenr);

    ಆದರೆ (sign == ಖಾಲಿ || gui.drawarea == ಖಾಲಿ
	    || gtk_widget_get_ಕಿಟಕಿ(gui.drawarea) == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಪೂರ್ಣಾಂಕ ಅಗಲ;
    ಪೂರ್ಣಾಂಕ ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ xoffset;
    ಪೂರ್ಣಾಂಕ yoffset;
    ಪೂರ್ಣಾಂಕ need_scale;

    ಅಗಲ  = gdk_pixbuf_get_ಅಗಲ(sign);
    ಎತ್ತರ = gdk_pixbuf_get_ಎತ್ತರ(sign);
    /*
     * Decide whether we need to scale.  Allow one pixel of border
     * width to be cut off, in order to avoid excessive scaling for
     * tiny differences in font size.
     * Do scale to fit the height to avoid gaps because of linespacing.
     */
    need_scale = (ಅಗಲ > SIGN_ಅಗಲ + 2
	    || ಎತ್ತರ != SIGN_ಎತ್ತರ
	    || (ಅಗಲ < 3 * SIGN_ಅಗಲ / 4
		&& ಎತ್ತರ < 3 * SIGN_ಎತ್ತರ / 4));
    ಆದರೆ (need_scale)
    {
	ದ್ವಿಗುಣ  aspect;
	ಪೂರ್ಣಾಂಕ	    w = ಅಗಲ;
	ಪೂರ್ಣಾಂಕ	    h = ಎತ್ತರ;

	// Keep the original aspect ratio
	aspect = (ದ್ವಿಗುಣ)ಎತ್ತರ / (ದ್ವಿಗುಣ)ಅಗಲ;
	ಅಗಲ  = (ದ್ವಿಗುಣ)SIGN_ಅಗಲ * SIGN_ASPECT / aspect;
	ಅಗಲ  = MIN(ಅಗಲ, SIGN_ಅಗಲ);
	ಆದರೆ (((ದ್ವಿಗುಣ)(MAX(ಎತ್ತರ, SIGN_ಎತ್ತರ)) /
		    (ದ್ವಿಗುಣ)(MIN(ಎತ್ತರ, SIGN_ಎತ್ತರ))) < 1.15)
	{
	    // Change the aspect ratio by at most 15% to fill the
	    // available space completely.
	    ಎತ್ತರ = (ದ್ವಿಗುಣ)SIGN_ಎತ್ತರ * SIGN_ASPECT / aspect;
	    ಎತ್ತರ = MIN(ಎತ್ತರ, SIGN_ಎತ್ತರ);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಎತ್ತರ = (ದ್ವಿಗುಣ)ಅಗಲ * aspect;

	ಆದರೆ (w == ಅಗಲ && h == ಎತ್ತರ)
	{
	    // no change in dimensions; don't decrease reference counter
	    // (below)
	    need_scale = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // This doesn't seem to be worth caching, and doing so would
	    // complicate the code quite a bit.
	    sign = gdk_pixbuf_scale_simple(sign, ಅಗಲ, ಎತ್ತರ,
		    GDK_INTERP_BILINEAR);
	    ಆದರೆ (sign == ಖಾಲಿ)
		ಹಿಂತಿರುಗಿ; // out of memory
	}
    }

    // The origin is the upper-left corner of the pixmap.  Therefore
    // these offset may become negative if the pixmap is smaller than
    // the 2x1 cells reserved for the sign icon.
    xoffset = (ಅಗಲ  - SIGN_ಅಗಲ)  / 2;
    yoffset = (ಎತ್ತರ - SIGN_ಎತ್ತರ) / 2;

# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    {
	cairo_t	    *cr;
	cairo_surface_t *bg_surf;
	cairo_t	    *bg_cr;
	cairo_surface_t *sign_surf;
	cairo_t	    *sign_cr;

	cr = cairo_create(gui.surface);

	bg_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ಅಗಲ, SIGN_ಎತ್ತರ);
	bg_cr = cairo_create(bg_surf);
	cairo_ಗಣ_ಮೂಲ_rgba(bg_cr,
		gui.bgcolor->red, gui.bgcolor->green, gui.bgcolor->blue,
		gui.bgcolor->alpha);
	cairo_paint(bg_cr);

	sign_surf = cairo_surface_create_similar(gui.surface,
		cairo_surface_get_content(gui.surface),
		SIGN_ಅಗಲ, SIGN_ಎತ್ತರ);
	sign_cr = cairo_create(sign_surf);
	gdk_cairo_ಗಣ_ಮೂಲ_pixbuf(sign_cr, sign, -xoffset, -yoffset);
	cairo_paint(sign_cr);

	cairo_ಗಣ_operator(sign_cr, CAIRO_OPERATOR_DEST_OVER);
	cairo_ಗಣ_ಮೂಲ_surface(sign_cr, bg_surf, 0, 0);
	cairo_paint(sign_cr);

	cairo_ಗಣ_ಮೂಲ_surface(cr, sign_surf, FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಅಡ್ಡಸಾಲು));
	cairo_paint(cr);

	cairo_destroy(sign_cr);
	cairo_surface_destroy(sign_surf);
	cairo_destroy(bg_cr);
	cairo_surface_destroy(bg_surf);
	cairo_destroy(cr);

	ಸರತಿ_draw_area(
		FILL_X(ಲಂಬಸಾಲು), FILL_Y(ಲಂಬಸಾಲು), ಅಗಲ, ಎತ್ತರ);

    }
# ಇಲ್ಲದಿದ್ದರೆ // !GTK_CHECK_VERSION(3,0,0)
    gdk_gc_ಗಣ_foreground(gui.ಪಠ್ಯ_gc, gui.bgcolor);

    gdk_draw_rectangle(gui.drawarea->ಕಿಟಕಿ_2,
	    gui.ಪಠ್ಯ_gc,
	    TRUE,
	    FILL_X(ಲಂಬಸಾಲು),
	    FILL_Y(ಅಡ್ಡಸಾಲು),
	    SIGN_ಅಗಲ,
	    SIGN_ಎತ್ತರ);

    gdk_pixbuf_render_ಗಮ್ಯ_drawable_alpha(sign,
	    gui.drawarea->ಕಿಟಕಿ_2,
	    MAX(0, xoffset),
	    MAX(0, yoffset),
	    FILL_X(ಲಂಬಸಾಲು) - MIN(0, xoffset),
	    FILL_Y(ಅಡ್ಡಸಾಲು) - MIN(0, yoffset),
	    MIN(ಅಗಲ,	SIGN_ಅಗಲ),
	    MIN(ಎತ್ತರ, SIGN_ಎತ್ತರ),
	    GDK_PIXBUF_ALPHA_BILEVEL,
	    127,
	    GDK_RGB_DITHER_ಸಾಮಾನ್ಯ,
	    0, 0);
# endif // !GTK_CHECK_VERSION(3,0,0)
    ಆದರೆ (need_scale)
	g_object_unref(sign);
}

    ಶೂನ್ಯ *
gui_mch_ರಿಜಿಸ್ಟರ್_sign(char_u *signfile)
{
    ಆದರೆ (signfile[0] != NUL && signfile[0] != '-' && gui.ಒಳ_use)
    {
	GdkPixbuf   *sign;
	GError	    *error = ಖಾಲಿ;
	char_u	    *ಸಂದೇಶ_2;

	sign = gdk_pixbuf_new_from_file((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)signfile, &error);

	ಆದರೆ (error == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ sign;

	ಸಂದೇಶ_2 = (char_u *)error->ಸಂದೇಶ_2;

	ಆದರೆ (ಸಂದೇಶ_2 != ಖಾಲಿ && ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
	    ಸಂದೇಶ_2 = string_convert(&ಒಳಹರಿವು_conv, ಸಂದೇಶ_2, ಖಾಲಿ);

	ಆದರೆ (ಸಂದೇಶ_2 != ಖಾಲಿ)
	{
	    // The error message is already translated and will be more
	    // descriptive than anything we could possibly do ourselves.
	    semsg("E255: %s", ಸಂದೇಶ_2);

	    ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
		vim_free(ಸಂದೇಶ_2);
	}
	g_error_free(error);
    }

    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

    ಶೂನ್ಯ
gui_mch_destroy_sign(ಶೂನ್ಯ *sign)
{
    ಆದರೆ (sign != ಖಾಲಿ)
	g_object_unref(sign);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_SIGN_ICONS

#ಆದರೆ defined(FEAT_IMAGE) && defined(FEAT_IMAGE_GUI)

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
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
    ಸ್ಥಿರ ಶೂನ್ಯ
fill_image_surface(image_T *img, cairo_surface_t *surf)
{
    uint8_t *src = img->ದತ್ತಾಂಶ;
    uint8_t *dst;
    ಪೂರ್ಣಾಂಕ	    w = img->ಅಗಲ;
    ಪೂರ್ಣಾಂಕ	    h = img->ಎತ್ತರ;
    ಪೂರ್ಣಾಂಕ	    stride;
    ಪೂರ್ಣಾಂಕ	    x, y;

    cairo_surface_flush(surf);
    dst = cairo_image_surface_get_ದತ್ತಾಂಶ(surf);
    stride = cairo_image_surface_get_stride(surf);

    ಆದರೆ (img->fmt == IMAGE_FORMAT_RGBA)
    {
	ಗಾಗಿ (y = 0; y < h; ++y)
	{
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *p = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *)(dst + y * stride);

	    ಗಾಗಿ (x = 0; x < w; ++x)
	    {
		ಪೂರ್ಣಾಂಕ	idx = (y * w + x) * 4;
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
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಗಾಗಿ (y = 0; y < h; ++y)
	{
	    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *p = (ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ *)(dst + y * stride);

	    ಗಾಗಿ (x = 0; x < w; ++x)
	    {
		ಪೂರ್ಣಾಂಕ	idx = (y * w + x) * 3;
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

    ಪೂರ್ಣಾಂಕ
image_gui_init(image_T *img)
{
    // We only support GTK3
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    image_cairo_T *ctx = ALLOC_CLEAR_ONE(image_cairo_T);

    ಆದರೆ (ctx == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;

    ctx->surf = cairo_image_surface_create(CAIRO_FORMAT_ARGB32,
	    img->ಅಗಲ, img->ಎತ್ತರ);

    ಆದರೆ (cairo_surface_ಸ್ಥಿತಿ(ctx->surf) != CAIRO_ಸ್ಥಿತಿ_SUCCESS)
    {
	cairo_surface_destroy(ctx->surf);
	ಹೋಗು fail;
    }

    fill_image_surface(img, ctx->surf);
    img->backend_ದತ್ತಾಂಶ = ctx;

    // Set the device scale of the surface later before every redraw. This iso
    // that the GUI scale factor is used automatically.
    ಹಿಂತಿರುಗಿ OK;
fail:
    vim_free(ctx);
    ಹಿಂತಿರುಗಿ FAIL;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
# endif
}

    ಶೂನ್ಯ
image_gui_uninit(image_T *img)
{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    image_cairo_T *ctx = img->backend_ದತ್ತಾಂಶ;

    cairo_surface_destroy(ctx->surf);
    vim_free(ctx);
# endif
}

    ಪೂರ್ಣಾಂಕ
image_placement_gui_init(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    ಹಿಂತಿರುಗಿ OK;
# ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ FAIL;
# endif
}

    ಶೂನ್ಯ
image_placement_gui_uninit(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
}

    ಶೂನ್ಯ
image_placement_gui_draw(image_placement_T *ಇರಿಸು)
{
# ಆದರೆ GTK_CHECK_ಆವೃತ್ತಿ(3,0,0)
    image_T		    *img = ಇರಿಸು->img;
    image_cairo_T	    *ictx = img->backend_ದತ್ತಾಂಶ;
    pixman_box32_t	    *rects;
    ಪೂರ್ಣಾಂಕ			    n_rects;
    cairo_t		    *cr;

    rects = pixman_region32_rectangles(&ಇರಿಸು->visible, &n_rects);
    ಆದರೆ (rects == ಖಾಲಿ || n_rects == 0)
	ಹಿಂತಿರುಗಿ;

    cairo_surface_ಗಣ_device_scale(ictx->surf, gui.scale, gui.scale);
    cr = cairo_create(gui.surface);

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ i = 0; i < n_rects; i++)
    {
	pixman_box32_t	rect = rects[i];
	ಪೂರ್ಣಾಂಕ		ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
	ಪೂರ್ಣಾಂಕ		posx, posy;
	ಪೂರ್ಣಾಂಕ		x, y, w, h;

	// We set the device scale for the surface, so we need logical pixel
	// dimensions because cairo will handle the converting stuff.
	image_placement_subrect(ಇರಿಸು, rect, &ಅಡ್ಡಸಾಲು, &ಲಂಬಸಾಲು, &x, &y, &w, &h, false);
	posx = FILL_X(ಲಂಬಸಾಲು);
	posy = FILL_Y(ಅಡ್ಡಸಾಲು);

	cairo_save(cr);
	cairo_ಗಣ_operator(cr, CAIRO_OPERATOR_OVER);
	cairo_ಗಣ_ಮೂಲ_surface(cr, ictx->surf, posx - x, posy - y);
	cairo_rectangle(cr, posx, posy, w, h);
	cairo_fill(cr);
	cairo_restore(cr);

	ಸರತಿ_draw_area(posx, posy, w, h);
    }
    cairo_destroy(cr);
# endif
}

    ಶೂನ್ಯ
image_placement_gui_clear(image_placement_T *ಇರಿಸು ಬಳಸದ)
{
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_IMAGE
