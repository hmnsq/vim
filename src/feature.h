/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */
/*
 * feature.h: Defines for optional code and preferences
 *
 * Edit this file to include/exclude parts of Vim, before compiling.
 * The only other file that may be edited is Makefile, it contains machine
 * specific options.
 *
 * To include specific options, change the "#if*" and "#endif" into comments,
 * or uncomment the "#define".
 * To exclude specific options, change the "#define" into a comment.
 */

/*
 * When adding a new feature:
 * - Add a #define below.
 * - Add a message in the table above ex_version().
 * - Add a string to f_has().
 * - Add a feature to ":help feature-list" in doc/builtin.txt.
 * - Add feature to ":help +feature-list" in doc/various.txt.
 * - Add comment for the documentation of commands that use the feature.
 */

/*
 * Basic choices:
 * ==============
 *
 * +tiny		no optional features enabled, not even +eval
 * +normal		a default selection of features enabled
 * +huge		all possible features enabled.
 *
 * When +normal is used, +tiny is also included.  +huge implies +normal, etc.
 */

/*
 * +small is now an alias for +tiny
 */
#ಆದರೆ defined(FEAT_SMALL)
# undef FEAT_SMALL
# ಆದರೆ !defined(FEAT_TINY)
#  define FEAT_TINY
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +big is now an alias for +normal
 */
#ಆದರೆ defined(FEAT_BIG)
# undef FEAT_BIG
# ಆದರೆ !defined(FEAT_ಸಾಮಾನ್ಯ)
#  define FEAT_ಸಾಮಾನ್ಯ
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Uncomment one of these to override the default.  For unix use a configure
 * argument, see Makefile.
 */
#ಆದರೆ !defined(FEAT_TINY) && !defined(FEAT_ಸಾಮಾನ್ಯ) && !defined(FEAT_HUGE)
// #define FEAT_TINY
// #define FEAT_NORMAL
// #define FEAT_HUGE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * For Unix, Mac and Win32 use +huge by default.  These days CPUs are fast and
 * Memory is cheap.
 * Otherwise use +normal
 */
#ಆದರೆ !defined(FEAT_TINY) && !defined(FEAT_ಸಾಮಾನ್ಯ) && !defined(FEAT_HUGE)
# ಆದರೆ defined(UNIX) || defined(MSWIN) || defined(MACOS_X)
#  define FEAT_HUGE
# ಇಲ್ಲದಿದ್ದರೆ
#  define FEAT_ಸಾಮಾನ್ಯ
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Each feature implies including the "smaller" ones.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_ಸಾಮಾನ್ಯ
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_TINY
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Optional code (see ":help +feature-list")
 * =============
 */

/*
 * These features used to be optional but are now always enabled:
 * +windows		Multiple windows.  Without this there is no help
 *			window and no status lines.
 * +autocmd		Automatic commands
 * +vertsplit		Vertically split windows.
 * +cmdhist		Command line history.
 * +localmap		Mappings and abbreviations local to a buffer.
 * +visual		Visual mode
 * +visualextra		Extra features for Visual mode (mostly block operators).
 * +virtualedit		'virtualedit' option and its implementation
 * +user_commands	Allow the user to define his own commands.
 * +multi_byte		Generic multi-byte character handling.
 * +cmdline_compl	completion of mappings/abbreviations in cmdline mode.
 * +insert_expand	CTRL-N/CTRL-P/CTRL-X in insert mode.
 * +modify_fname	modifiers for file name.  E.g., "%:p:h".
 * +comments		'comments' option.
 * +title		'title' and 'icon' options
 * +jumplist		Jumplist, CTRL-O and CTRL-I commands.
 * +lispindent		lisp indenting (From Eric Fischer).
 * +cindent		C code indenting (From Eric Fischer).
 * +smartindent		smart C code indenting when the 'si' option is set.
 * +textobjects		Text objects: "vaw", "das", etc.
 * +file_in_path	"gf" and "<cfile>" commands.
 * +path_extra		up/downwards searching in 'path' and 'tags'.
 * +wildignore		'wildignore' and 'backupskip' options
 * +wildmenu		'wildmenu' option
 * +builtin_terms	all builtin termcap entries included
 * +float		Floating point variables.
 * +cmdwin		Command line window.
 * +cmdline_info	'showcmd' and 'ruler' options.
 *
 * Obsolete:
 * +tag_old_static	Old style static tags: "file:tag  file  ..".
 *			Support was removed in 8.1.1093.
 * +farsi		Farsi (Persian language) Keymap support.
 *			Removed in patch 8.1.0932
 * +footer		Motif only: Add a message area at the bottom of the
 *			main window area.
 */

/*
 * +folding		Fold lines.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_FOLDING
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +digraphs		Digraphs.
 *			In insert mode and on the command line you will be
 *			able to use digraphs. The CTRL-K command will work.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_DIGRAPHS
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +langmap		'langmap' option.  Only useful when you put your
 *			keyboard in a special language mode, e.g. for typing
 *			greek.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_LANGMAP
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +keymap		'keymap' option.  Allows you to map typed keys in
 *			Insert mode for a special language.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_KEYMAP
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +linebreak		'showbreak', 'breakat' and 'linebreak' options.
 *			Also 'numberwidth'.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_LINEBREAK
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +extra_search	'hlsearch' and 'incsearch' options.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_ಹುಡುಕು_EXTRA
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +quickfix		Quickfix commands.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_QUICKFIX
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +find_in_path	"[I" ":isearch" "^W^I", ":checkpath", etc.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_ಕಂಡುಹಿಡಿ_ID
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +rightleft		Right-to-left editing/typing support.
 *			Note that this isn't perfect, but enough users say they
 *			use it to keep supporting it.
 */
#ಆದರೆ defined(FEAT_HUGE) && !defined(DISABLE_RIGHTLEFT)
# define FEAT_RIGHTLEFT
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +arabic		Arabic keymap and shaping support.
 *			Requires FEAT_RIGHTLEFT
 */
#ಆದರೆ defined(FEAT_HUGE) && !defined(DISABLE_ARABIC)
# define FEAT_ARABIC
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARABIC
# ifndef FEAT_RIGHTLEFT
#  define FEAT_RIGHTLEFT
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +emacs_tags		When FEAT_EMACS_TAGS defined: Include support for
 *			emacs style TAGS file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_EMACS_ಟ್ಯಾಗುಗಳು
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +cscope		Unix only: Cscope support.
 */
#ಆದರೆ defined(UNIX) && defined(FEAT_HUGE) && defined(ENABLE_CSCOPE)
# define FEAT_CSCOPE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +eval		Built-in script language and expression evaluation,
 *			":let", ":if", etc.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_EVAL
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
# define HAVE_SANDBOX
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +profile		Profiling for functions and scripts.
 */
#ಆದರೆ defined(FEAT_HUGE) \
	&& defined(FEAT_EVAL) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ಸಮಯ_H)) \
		|| defined(MSWIN))
# define FEAT_PROFILE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +reltime		reltime() function
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) \
	&& defined(FEAT_EVAL) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ಸಮಯ_H) \
		&& (!defined(MACOS_X) || defined(HAVE_DISPATCH_DISPATCH_H))) \
	    || defined(MSWIN))
# define FEAT_RELTIME
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +timers		timer_start()
 */
#ಆದರೆ defined(FEAT_RELTIME) && (defined(UNIX) || defined(MSWIN) || defined(VMS))
# define FEAT_TIMERS
#ಆದರೆ_ಅಂತ್ಯ

/*
 *			Insert mode completion with 'completefunc'.
 */
#ಆದರೆ defined(FEAT_EVAL)
# define FEAT_COMPL_FUNC
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +printer		":hardcopy" command
 * +postscript		Printing uses PostScript file output.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && (defined(MSWIN) || defined(FEAT_EVAL)) \
	&& !defined(AMIGA)
# define FEAT_PRINTER
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_PRINTER) && !defined(FEAT_PRINT_PANGO) \
    && ((defined(MSWIN) && defined(MSWINPS)) \
	    || (!defined(MSWIN) && defined(FEAT_EVAL)))
# define FEAT_POSTSCRIPT
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_PRINTER) && defined(FEAT_PRINT_PANGO)
# undef FEAT_PRINT_PANGO
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +diff		Displaying diffs in a nice way.
 *			Can be enabled in autoconf already.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && !defined(FEAT_DIFF)
# define FEAT_DIFF
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +statusline		'statusline', 'statuslineopt', 'rulerformat' and
 *			special format of 'titlestring' and 'iconstring'
 *			options.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_STL_OPT
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +byte_offset		'%o' in 'statusline' and builtin functions line2byte()
 *			and byte2line().
 *			Note: Required for Macintosh.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_BYTEOFF
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +viminfo		reading/writing the viminfo file. Takes about 8Kbyte
 *			of code.
 * VIMINFO_FILE		Location of user .viminfo file (should start with $).
 * VIMINFO_FILE2	Location of alternate user .viminfo file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_VIMINFO
// #define VIMINFO_FILE	"$HOME/foo/.viminfo"
// #define VIMINFO_FILE2 "~/bar/.viminfo"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +syntax		syntax highlighting.  When using this, it's a good
 *			idea to have +eval too.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ)
# define FEAT_SYN_HL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +conceal		'conceal' option.  Depends on syntax highlighting
 *			as this is how the concealed text is defined.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(FEAT_SYN_HL)
# define FEAT_CONCEAL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +spell		spell checking
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ)
# define FEAT_SPELL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +cryptv		Encryption (originally by Mohsin Ahmed <mosh@sasi.com>).
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && !defined(FEAT_CRYPT)
# define FEAT_CRYPT
#ಆದರೆ_ಅಂತ್ಯ

/*
 * libsodium - add advanced cryptography support
 */
#ಆದರೆ defined(HAVE_SODIUM) && defined(FEAT_CRYPT)
# define FEAT_SODIUM
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +mksession		":mksession" command.
 *			fully depends on +eval
 */
#ಆದರೆ defined(FEAT_EVAL)
# define FEAT_SESSION
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +multi_lang		Multi language support. ":menutrans", ":language", etc.
 * +gettext		Message translations (requires +multi_lang)
 *			(only when "lang" archive unpacked)
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_MULTI_LANG
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(HAVE_GETTEXT) && defined(FEAT_MULTI_LANG) \
	&& (defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define FEAT_GETTEXT
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +multi_byte_ime	Win32 IME input method.  Only for far-east Windows, so
 *			IME can be used to input chars.  Not tested much!
 */
#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(FEAT_MBYTE_IME)
// #define FEAT_MBYTE_IME
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_HUGE) && defined(FEAT_GUI_HAIKU) && !defined(FEAT_MBYTE_IME)
# define FEAT_MBYTE_IME
#ಆದರೆ_ಅಂತ್ಯ

// Use iconv() when it's available.
#ಆದರೆ (defined(HAVE_ICONV_H) && defined(HAVE_ICONV)) || defined(DYNAMIC_ICONV)
# define USE_ICONV
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +xim			X Input Method.  For entering special languages like
 *			chinese and Japanese.
 *			this is for Unix and VMS only.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_XIM
// #define FEAT_XIM
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
# define USE_XIM 1		// needed for GTK include files
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_XIM)
// # define X_LOCALE			// for OS with incomplete locale
					// support, like old linux versions.
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +xfontset		X fontset support.  For outputting wide characters.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FEAT_XFONTSET
# ಆದರೆ defined(HAVE_X11) && !defined(FEAT_GUI_GTK)
#  define FEAT_XFONTSET
# ಇಲ್ಲದಿದ್ದರೆ
// #  define FEAT_XFONTSET
# endif
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(USE_GTK4)
#  undef FEAT_XFONTSET
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +libcall		libcall() function
 */
// Using dlopen() also requires dlsym() to be available.
#ಆದರೆ defined(HAVE_DLOPEN) && defined(HAVE_DLSYM)
# define USE_DLOPEN
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_EVAL) && (defined(MSWIN) || ((defined(UNIX) || defined(VMS)) \
	&& (defined(USE_DLOPEN) || defined(HAVE_SHL_ತುಂಬು))))
# define FEAT_LIBCALL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +menu		":menu" command
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_MENU
# ifdef FEAT_GUI_MSWIN
#  define FEAT_TEAROFF
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * popup menu in a terminal
 */
#ಆದರೆ defined(FEAT_MENU) && !defined(ALWAYS_USE_GUI)
# define FEAT_ಪದ_POPUP_MENU
#ಆದರೆ_ಅಂತ್ಯ

/*
 * sound
 */
#ಆದರೆ !defined(FEAT_SOUND) && defined(HAVE_CANBERRA)
# define FEAT_SOUND
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_SOUND) && defined(HAVE_CANBERRA)
# define FEAT_SOUND_CANBERRA
#ಆದರೆ_ಅಂತ್ಯ

// There are two ways to use XPM.
#ಆದರೆ (defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF)) \
		|| defined(HAVE_X11_XPM_H)
# define HAVE_XPM 1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +toolbar		Include code for a toolbar (for the Win32 GUI, GTK
 *			always has it).  But only if menus are enabled.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(FEAT_MENU) \
	&& (defined(FEAT_GUI_GTK) \
		|| defined(FEAT_GUI_MSWIN) \
		|| (defined(FEAT_GUI_MOTIF) && defined(HAVE_XPM)) \
		|| defined(FEAT_GUI_PHOTON) \
		|| defined(FEAT_GUI_HAIKU))

# define FEAT_TOOLBAR
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(FEAT_TOOLBAR) && !defined(FEAT_MENU)
# define FEAT_MENU
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GUI dark theme variant
 */
#ಆದರೆ (defined(FEAT_GUI_GTK) && (defined(USE_GTK3) || defined(USE_GTK4))) \
	|| defined(FEAT_GUI_MSWIN)
# define FEAT_GUI_DARKTHEME
#ಆದರೆ_ಅಂತ್ಯ

/*
 * GUI tabline
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) \
    && (defined(FEAT_GUI_GTK) \
	|| (defined(FEAT_GUI_MOTIF) && defined(HAVE_XM_NOTEBOOK_H)) \
	|| defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_MSWIN))
# define FEAT_GUI_TABLINE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +tabpanel		Tab SideBar
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_TABPANEL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +browse		":browse" command.
 *			or just the ":browse" command modifier
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ)
# define FEAT_BROWSE_CMD
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_PHOTON)
#  define FEAT_BROWSE
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * On some systems, when we compile with the GUI, we always use it.  On Mac
 * there is no terminal version, and on Windows we can't figure out how to
 * fork one off with :gui.
 */
#ಆದರೆ defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define ALWAYS_USE_GUI
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +dialog_gui		Use GUI dialog.
 * +dialog_con		May use Console dialog.
 *			When none of these defined there is no dialog support.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# ಆದರೆ (defined(FEAT_GUI_MOTIF) && defined(HAVE_X11_XPM_H)) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON) \
	|| defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_MSWIN)
#  define FEAT_CON_DIALOG
#  define FEAT_GUI_DIALOG
# ಇಲ್ಲದಿದ್ದರೆ
#  define FEAT_CON_DIALOG
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ !defined(FEAT_GUI_DIALOG) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN))
// need a dialog to show error messages when starting from the desktop
# define FEAT_GUI_DIALOG
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_DIALOG) && \
	(defined(FEAT_GUI_MOTIF) \
	 || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN) \
	 || defined(FEAT_GUI_PHOTON) \
	 || defined(FEAT_GUI_HAIKU))
# define FEAT_GUI_TEXTDIALOG
# ifndef ALWAYS_USE_GUI
#  define FEAT_CON_DIALOG
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +termguicolors	'termguicolors' option.
 */
#ಆದರೆ (defined(FEAT_ಸಾಮಾನ್ಯ) && defined(FEAT_SYN_HL)) && !defined(ALWAYS_USE_GUI)
# define FEAT_TERMGUICOLORS
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +vartabs		'vartabstop' and 'varsofttabstop' options.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_HUGE
# define FEAT_VARTABS
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Preferences:
 * ============
 */

/*
 * +writebackup		'writebackup' is default on:
 *			Use a backup file while overwriting a file.  But it's
 *			deleted again when 'backup' is not set.  Changing this
 *			is strongly discouraged: You can lose all your
 *			changes when the computer crashes while writing the
 *			file.
 *			VMS note: It does work on VMS as well, but because of
 *			version handling it does not have any purpose.
 *			Overwrite will write to the new version.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS
# define FEAT_WRITEBACKUP
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +xterm_save		The t_ti and t_te entries for the builtin xterm will
 *			be set to save the screen when starting Vim and
 *			restoring it when exiting.
 */
// #define FEAT_XTERM_SAVE

/*
 * DEBUG		Output a lot of debugging garbage.
 */
// #define DEBUG

/*
 * STARTUPTIME		Time the startup process.  Writes a file with
 *			timestamps.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_ಸಮಯ_H)) \
		|| defined(MSWIN))
# define STARTUPTIME 1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * MEM_PROFILE		Debugging of memory allocation and freeing.
 */
// #define MEM_PROFILE

/*
 * VIMRC_FILE		Name of the .vimrc file in current dir.
 */
// #define VIMRC_FILE	".vimrc"

/*
 * EXRC_FILE		Name of the .exrc file in current dir.
 */
// #define EXRC_FILE	".exrc"

/*
 * GVIMRC_FILE		Name of the .gvimrc file in current dir.
 */
// #define GVIMRC_FILE	".gvimrc"

/*
 * SESSION_FILE		Name of the default ":mksession" file.
 */
#ವ್ಯಾಖ್ಯಾನಿಸು SESSION_FILE	"Session.vim"

/*
 * USR_VIMRC_FILE	Name of the user .vimrc file.
 * USR_VIMRC_FILE2	Name of alternate user .vimrc file.
 * USR_VIMRC_FILE3	Name of alternate user .vimrc file.
 */
// #define USR_VIMRC_FILE	"~/foo/.vimrc"
// #define USR_VIMRC_FILE2	"~/bar/.vimrc"
// #define USR_VIMRC_FILE3	"$VIM/.vimrc"

/*
 * VIM_DEFAULTS_FILE	Name of the defaults.vim script file
 */
// #define VIM_DEFAULTS_FILE	"$VIMRUNTIME/defaults.vim"

/*
 * EVIM_FILE		Name of the evim.vim script file
 */
// #define EVIM_FILE		"$VIMRUNTIME/evim.vim"

/*
 * USR_EXRC_FILE	Name of the user .exrc file.
 * USR_EXRC_FILE2	Name of the alternate user .exrc file.
 */
// #define USR_EXRC_FILE	"~/foo/.exrc"
// #define USR_EXRC_FILE2	"~/bar/.exrc"

/*
 * USR_GVIMRC_FILE	Name of the user .gvimrc file.
 * USR_GVIMRC_FILE2	Name of the alternate user .gvimrc file.
 */
// #define USR_GVIMRC_FILE	"~/foo/.gvimrc"
// #define USR_GVIMRC_FILE2	"~/bar/.gvimrc"
// #define USR_GVIMRC_FILE3	"$VIM/.gvimrc"

/*
 * SYS_VIMRC_FILE	Name of the system-wide .vimrc file.
 */
// #define SYS_VIMRC_FILE	"/etc/vimrc"

/*
 * SYS_GVIMRC_FILE	Name of the system-wide .gvimrc file.
 */
// #define SYS_GVIMRC_FILE	"/etc/gvimrc"

/*
 * DFLT_HELPFILE	Name of the help file.
 */
// # define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt.gz"

/*
 * File names for:
 * FILETYPE_FILE	used for file type detection
 * FTPLUGIN_FILE	used for loading filetype plugin files
 * INDENT_FILE		used for loading indent files
 * FTOFF_FILE		used for file type detection
 * FTPLUGOF_FILE	used for loading settings files
 * INDOFF_FILE		used for loading indent files
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FILETYPE_FILE
# define FILETYPE_FILE		"filetype.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FTPLUGIN_FILE
# define FTPLUGIN_FILE		"ftplugin.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ INDENT_FILE
# define INDENT_FILE		"indent.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FTOFF_FILE
# define FTOFF_FILE		"ftoff.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ FTPLUGOF_FILE
# define FTPLUGOF_FILE		"ftplugof.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ INDOFF_FILE
# define INDOFF_FILE		"indoff.vim"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * SYS_MENU_FILE	Name of the default menu.vim file.
 */
// # define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"

/*
 * SYS_OPTWIN_FILE	Name of the default optwin.vim file.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME/optwin.vim"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * SYNTAX_FNAME		Name of a syntax file, where %s is the syntax name.
 */
// #define SYNTAX_FNAME	"/foo/%s.vim"

/*
 * RUNTIME_DIRNAME	Generic name for the directory of the runtime files.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ RUNTIME_DIRNAME
# define RUNTIME_DIRNAME "runtime"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * RUNTIME_GLOBAL	Comma-separated list of directory names for global Vim
 *			runtime directories.
 *			Don't define this if the preprocessor can't handle
 *			string concatenation.
 *			Also set by "--with-global-runtime" configure argument.
 */
// #define RUNTIME_GLOBAL "/etc/vim"

/*
 * RUNTIME_GLOBAL_AFTER	Comma-separated list of directory names for global Vim
 *			runtime after directories.
 *			Don't define this if the preprocessor can't handle
 *			string concatenation.
 *			Also set by "--with-global-runtime" configure argument.
 */
// #define RUNTIME_GLOBAL_AFTER "/etc/vim/after"

/*
 * MODIFIED_BY		Name of who modified Vim.  Required when distributing
 *			a modified version of Vim.
 *			Also from the "--with-modified-by" configure argument.
 */
// #define MODIFIED_BY "John Doe"

/*
 * Machine dependent:
 * ==================
 */

/*
 * +fork		Unix only: fork() support (detected by configure)
 * +system		Use system() instead of fork/exec for starting a
 *			shell.  Doesn't work for the GUI!
 */
// #define USE_SYSTEM

/*
 * +X11			Unix only.  Include code for xterm title saving and X
 *			clipboard.  Only works if HAVE_X11 is also defined.
 */
#ಆದರೆ (defined(FEAT_ಸಾಮಾನ್ಯ) || defined(FEAT_GUI_MOTIF)) && !defined(USE_GTK4)
# define WANT_X11
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +wayland		Unix only.  Include code for the Wayland protocol,
 *                      only works if HAVE_WAYLAND is defined.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(UNIX)
# define WANT_WAYLAND
#ಆದರೆ_ಅಂತ್ಯ

/*
 * XSMP - X11 Session Management Protocol
 * It may be preferred to disable this if the GUI supports it (e.g.,
 * GNOME/KDE) and implement save-yourself etc. through that, but it may also
 * be cleaner to have all SM-aware vims do the same thing (libSM does not
 * depend upon X11).
 * If your GUI wants to support SM itself, change this ifdef.
 * I'm assuming that any X11 implementation will cope with this for now.
 */
#ಆದರೆ defined(HAVE_X11) && defined(WANT_X11) && defined(HAVE_X11_SM_SMLIB_H)
# define USE_XSMP
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(USE_XSMP_INTERACT) && !defined(USE_XSMP)
# undef USE_XSMP_INTERACT
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +mouse_xterm		Unix only: Include code for xterm mouse handling.
 * +mouse_dec		idem, for Dec mouse handling.
 * +mouse_jsbterm	idem, for Jsbterm mouse handling.
 * +mouse_netterm	idem, for Netterm mouse handling.
 * (none)		MS-DOS mouse support.
 * +mouse_gpm		Unix only: Include code for Linux console mouse
 *			handling.
 * +mouse_pterm		PTerm mouse support for QNX
 * +mouse_sgr		Unix only: Include code for SGR-styled mouse.
 * +mouse_sysmouse	Unix only: Include code for FreeBSD and DragonFly
 *			console mouse handling.
 * +mouse_urxvt		Unix only: Include code for urxvt mouse handling.
 * +mouse		Any mouse support (any of the above enabled).
 *			Always included, since either FEAT_MOUSE_XTERM or
 *			DOS_MOUSE is defined.
 */
// Amiga console has no mouse support
#ಆದರೆ defined(UNIX) || defined(VMS)
# define FEAT_MOUSE_XTERM
# ifdef FEAT_ಸಾಮಾನ್ಯ
#  define FEAT_MOUSE_NET
#  define FEAT_MOUSE_DEC
#  define FEAT_MOUSE_URXVT
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(MSWIN)
# define DOS_MOUSE
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(__QNX__)
# define FEAT_MOUSE_PTERM
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Note: Only one of the following may be defined:
 * FEAT_MOUSE_GPM
 * FEAT_SYSMOUSE
 * FEAT_MOUSE_JSB
 * FEAT_MOUSE_PTERM
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(HAVE_GPM)
# define FEAT_MOUSE_GPM
/*
 * +mouse_gpm/dyn   Load libgpm dynamically.
 */
# ifndef DYNAMIC_GPM
// #  define DYNAMIC_GPM
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(HAVE_SYSMOUSE)
# define FEAT_SYSMOUSE
#ಆದರೆ_ಅಂತ್ಯ

// urxvt is a small variation of mouse_xterm, and shares its code
#ಆದರೆ defined(FEAT_MOUSE_URXVT) && !defined(FEAT_MOUSE_XTERM)
# define FEAT_MOUSE_XTERM
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +clipboard		Clipboard support.  Always used for the GUI.
 * +xterm_clipboard	Unix only: Include code for handling the clipboard
 *			in an xterm like in the GUI.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CYGWIN_WIN32_CLIPBOARD
# define FEAT_CLIPBOARD
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) \
	&& (defined(UNIX) || defined(VMS)) \
	&& defined(WANT_X11) && defined(HAVE_X11) \
	&& !defined(USE_GTK4)
# define FEAT_XCLIPBOARD
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(UNIX) \
    && defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND_CLIPBOARD
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +dnd		Drag'n'drop support.  Always used for the GTK+ GUI.
 */
#ಆದರೆ defined(FEAT_CLIPBOARD) && defined(FEAT_GUI_GTK)
# define FEAT_DND
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_MSWIN)
# define MSWIN_ಕಂಡುಹಿಡಿ_REPLACE	// include code for find/replace dialog
# define MSWIN_FR_BUFSIZE 256
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MOTIF) \
	|| defined(MSWIN_ಕಂಡುಹಿಡಿ_REPLACE)
# define ಕಂಡುಹಿಡಿ_REPLACE_DIALOG 1
#ಆದರೆ_ಅಂತ್ಯ

/*
 * The +channel feature requires +eval.
 */
#ಆದರೆ !defined(FEAT_EVAL) && defined(FEAT_ಕೆಲಸ_CHANNEL)
# undef FEAT_ಕೆಲಸ_CHANNEL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +socketserver	 Use channels for clientserver communication
 */
#ಆದರೆ (defined(UNIX) || defined(MSWIN)) && defined(FEAT_ಕೆಲಸ_CHANNEL)
# define FEAT_SOCKETSERVER
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +clientserver	Remote control via the remote_send() function
 *			and the --remote argument
 */
#ಆದರೆ (defined(MSWIN) || defined(FEAT_XCLIPBOARD) || defined(FEAT_SOCKETSERVER)) \
    && defined(FEAT_EVAL)
# define FEAT_CLIENTSERVER
# ಆದರೆ defined(FEAT_SOCKETSERVER) && (defined(FEAT_XCLIPBOARD) || defined(MSWIN))
#  define FEAT_CLIENTSERVER_BACKENDS
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +autoservername	Automatically generate a servername for clientserver
 *			when --servername is not passed on the command line.
 */
#ಆದರೆ defined(FEAT_CLIENTSERVER) && !defined(FEAT_AUTOSERVERNAME)
# ifdef MSWIN
    // Always enabled on MS-Windows.
#  define FEAT_AUTOSERVERNAME
# ಇಲ್ಲದಿದ್ದರೆ
    // Enable here if you don't use configure.
// # define FEAT_AUTOSERVERNAME
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +termresponse	send t_RV to obtain terminal response.  Used for xterm
 *			to check if mouse dragging can be used and if term
 *			codes can be obtained.
 */
#ಆದರೆ defined(HAVE_TGETENT)
# define FEAT_TERMRESPONSE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * cursor shape		Adjust the shape of the cursor to the mode.
 * mouse shape		Adjust the shape of the mouse pointer to the mode.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
// Win32 console can change cursor shape
# ಆದರೆ defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  define MCH_CURSOR_SHAPE
# endif
# ಆದರೆ defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON)
#  define FEAT_MOUSESHAPE
# endif
#ಆದರೆ_ಅಂತ್ಯ

// GUI and some consoles can change the shape of the cursor.  The code is also
// needed for the 'mouseshape' and 'concealcursor' options.
#ಆದರೆ defined(FEAT_GUI) \
	    || defined(MCH_CURSOR_SHAPE) \
	    || defined(FEAT_MOUSESHAPE) \
	    || defined(FEAT_CONCEAL) \
	    || (defined(UNIX) && defined(FEAT_ಸಾಮಾನ್ಯ))
# define CURSOR_SHAPE
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_MZSCHEME) && (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)    \
	|| defined(FEAT_GUI_MOTIF))
# define MZSCHEME_GUI_ಎಳೆಗಳು
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +ARP			Amiga only. Use arp.library, DOS 2.0 is not required.
 */
#ಆದರೆ defined(AMIGA) && !defined(NO_ARP) && !defined(__amigaos4__) \
	&& !defined(__MORPHOS__) && !defined(__AROS__)
# define FEAT_ARP
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +ole			Win32 OLE automation: Use Makefile.ovc.
 */

/*
 * These features can only be included by using a configure argument.  See the
 * Makefile for a line to uncomment.
 * +lua			Lua interface: "--enable-luainterp"
 * +mzscheme		MzScheme interface: "--enable-mzscheme"
 * +perl		Perl interface: "--enable-perlinterp"
 * +python		Python interface: "--enable-pythoninterp"
 * +tcl			TCL interface: "--enable-tclinterp"
 * +netbeans_intg	Netbeans integration
 * +channel		Inter process communication
 * +GUI_Motif		Motif GUI
 */

/*
 * These features are automatically detected:
 * +terminfo
 * +tgetent
 */

/*
 * The Netbeans feature requires +eval and +job_channel
 */
#ಆದರೆ (!defined(FEAT_EVAL) || !defined(FEAT_ಕೆಲಸ_CHANNEL)) && defined(FEAT_NETBEANS_INTG)
# undef FEAT_NETBEANS_INTG
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +terminal		":terminal" command.  Runs a terminal in a window.
 *			requires +channel
 */
#ಆದರೆ defined(FEAT_TERMINAL) && !defined(FEAT_ಕೆಲಸ_CHANNEL)
# undef FEAT_TERMINAL
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TERMINAL) && !defined(CURSOR_SHAPE)
# define CURSOR_SHAPE
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_TERMINAL) && !defined(FEAT_SYN_HL)
// simplify the code a bit by enabling +syntax when +terminal is enabled
# define FEAT_SYN_HL
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +autoshelldir	    'autoshelldir' option.
 */
#ಆದರೆ defined(FEAT_TERMINAL)
# define FEAT_AUTOSHELLDIR
#ಆದರೆ_ಅಂತ್ಯ
/*
 * +textprop and +popupwin	Text PROPerties and POPUP windows
 */
#ಆದರೆ defined(FEAT_EVAL) && defined(FEAT_SYN_HL)
# define FEAT_PROP_POPUP
#ಆದರೆ_ಅಂತ್ಯ


/*
 * +image	    Render images in the terminal and GUI
 * +image_popup	    Render images in popup windows
 */
#ಆದರೆ defined(FEAT_HUGE) && defined(FEAT_PROP_POPUP) && defined(HAVE_PIXMAN)
# define ALLOW_IMAGE
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ALLOW_IMAGE) && !defined(ALWAYS_USE_GUI)
# define FEAT_IMAGE_SIXEL
# define FEAT_IMAGE_KITTY
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(ALLOW_IMAGE) && ((defined(FEAT_GUI_GTK) \
	    && (defined(USE_GTK4) || defined(USE_GTK3))) || defined(FEAT_GUI_MSWIN))
# define FEAT_IMAGE_GUI
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_IMAGE_SIXEL) || defined(FEAT_IMAGE_KITTY) \
    || defined(FEAT_IMAGE_GUI)
# define FEAT_IMAGE_POPUP
# define FEAT_IMAGE
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ALLOW_IMAGE
# undef ALLOW_IMAGE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +message_window	use a popup for messages when 'cmdheight' is zero
 */
#ಆದರೆ defined(FEAT_PROP_POPUP) && defined(FEAT_TIMERS)
# define HAS_ಸಂದೇಶ_ಕಿಟಕಿ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_SYN_HL) && defined(FEAT_RELTIME)
// Can limit syntax highlight time to 'redrawtime'.
# define SYN_ಸಮಯ_ಎಲ್ಲೆ 1
#ಆದರೆ_ಅಂತ್ಯ


/*
 * +signs		Allow signs to be displayed to the left of text lines.
 *			Adds the ":sign" command.
 */
#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) || defined(FEAT_NETBEANS_INTG) || defined(FEAT_PROP_POPUP)
# define FEAT_SIGNS
# ಆದರೆ (defined(FEAT_GUI_MOTIF) && defined(HAVE_X11_XPM_H)) \
	|| defined(FEAT_GUI_GTK) \
	|| (defined(MSWIN) && defined(FEAT_GUI))
#  define FEAT_SIGN_ICONS
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +balloon_eval	Allow balloon expression evaluation. Used with a
 *			debugger and for tooltips.
 *			Only for GUIs where it was implemented.
 */
#ಆದರೆ (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) \
	&& (   ((defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)) \
		&& !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)) \
	    || defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL))
# define FEAT_BEVAL_GUI
# ಆದರೆ !defined(FEAT_XFONTSET) && !defined(FEAT_GUI_GTK) \
	&& !defined(FEAT_GUI_MSWIN)
#  define FEAT_XFONTSET
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_GUI) && defined(FEAT_GUI_MOTIF)
# define FEAT_BEVAL_TIP		// balloon eval used for toolbar tooltip
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +balloon_eval_term	Allow balloon expression evaluation in the terminal.
 */
#ಆದರೆ defined(FEAT_HUGE) && defined(FEAT_TIMERS) && \
	(defined(UNIX) || defined(VMS) || \
	 (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))))
# define FEAT_BEVAL_ಪದ
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_BEVAL_GUI) || defined(FEAT_BEVAL_ಪದ)
# define FEAT_BEVAL
#ಆದರೆ_ಅಂತ್ಯ

// Motif is X11
#ಆದರೆ defined(FEAT_GUI_MOTIF)
# define FEAT_GUI_X11
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_NETBEANS_INTG)
// NetBeans uses menus.
# ಆದರೆ !defined(FEAT_MENU)
#  define FEAT_MENU
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +autochdir		'autochdir' option.
 */
#ಆದರೆ defined(FEAT_NETBEANS_INTG) || defined(FEAT_ಸಾಮಾನ್ಯ)
# define FEAT_AUTOCHDIR
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +persistent_undo	'undofile', 'undodir' options, :wundo and :rundo, and
 * implementation.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ಸಾಮಾನ್ಯ
# define FEAT_PERSISTENT_UNDO
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +filterpipe
 */
#ಆದರೆ (defined(UNIX) && !defined(USE_SYSTEM)) \
	    || (defined(MSWIN) && defined(FEAT_GUI_MSWIN))
# define FEAT_FILTERPIPE
#ಆದರೆ_ಅಂತ್ಯ

/*
 * +vtp: Win32 virtual console.
 */
#ಆದರೆ (!defined(FEAT_GUI) || defined(VIMDLL)) && defined(MSWIN)
# define FEAT_VTP
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(DYNAMIC_PERL) \
	|| defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3) \
	|| defined(DYNAMIC_RUBY) \
	|| defined(DYNAMIC_TCL) \
	|| defined(DYNAMIC_ICONV) \
	|| defined(DYNAMIC_GETTEXT) \
	|| defined(DYNAMIC_MZSCHEME) \
	|| defined(DYNAMIC_LUA) \
	|| defined(FEAT_TERMINAL)
# define USING_ತುಂಬು_LIBRARY
#ಆದರೆ_ಅಂತ್ಯ

/*
 * currently Unix only: XATTR support
 */

#ಆದರೆ defined(FEAT_ಸಾಮಾನ್ಯ) && defined(HAVE_XATTR) && !defined(MACOS_X)
# define FEAT_XATTR
#ಆದರೆ_ಅಂತ್ಯ
