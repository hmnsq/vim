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
#إذا defined(FEAT_SMALL)
# undef FEAT_SMALL
# إذا !defined(FEAT_TINY)
#  define FEAT_TINY
# endif
#د_که_پای

/*
 * +big is now an alias for +normal
 */
#إذا defined(FEAT_BIG)
# undef FEAT_BIG
# إذا !defined(FEAT_نورمال)
#  define FEAT_نورمال
# endif
#د_که_پای

/*
 * Uncomment one of these to override the default.  For unix use a configure
 * argument, see Makefile.
 */
#إذا !defined(FEAT_TINY) && !defined(FEAT_نورمال) && !defined(FEAT_HUGE)
// #define FEAT_TINY
// #define FEAT_NORMAL
// #define FEAT_HUGE
#د_که_پای

/*
 * For Unix, Mac and Win32 use +huge by default.  These days CPUs are fast and
 * Memory is cheap.
 * Otherwise use +normal
 */
#إذا !defined(FEAT_TINY) && !defined(FEAT_نورمال) && !defined(FEAT_HUGE)
# إذا defined(UNIX) || defined(MSWIN) || defined(MACOS_X)
#  define FEAT_HUGE
# وإلا
#  define FEAT_نورمال
# endif
#د_که_پای

/*
 * Each feature implies including the "smaller" ones.
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_نورمال
#د_که_پای
#که_تعریف_شوی FEAT_نورمال
# define FEAT_TINY
#د_که_پای

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
#که_تعریف_شوی FEAT_نورمال
# define FEAT_FOLDING
#د_که_پای

/*
 * +digraphs		Digraphs.
 *			In insert mode and on the command line you will be
 *			able to use digraphs. The CTRL-K command will work.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_DIGRAPHS
#د_که_پای

/*
 * +langmap		'langmap' option.  Only useful when you put your
 *			keyboard in a special language mode, e.g. for typing
 *			greek.
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_LANGMAP
#د_که_پای

/*
 * +keymap		'keymap' option.  Allows you to map typed keys in
 *			Insert mode for a special language.
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_KEYMAP
#د_که_پای

/*
 * +linebreak		'showbreak', 'breakat' and 'linebreak' options.
 *			Also 'numberwidth'.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_LINEBREAK
#د_که_پای

/*
 * +extra_search	'hlsearch' and 'incsearch' options.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_پلټنه_EXTRA
#د_که_پای

/*
 * +quickfix		Quickfix commands.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_QUICKFIX
#د_که_پای

/*
 * +find_in_path	"[I" ":isearch" "^W^I", ":checkpath", etc.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_پیدا_کړه_ID
#د_که_پای

/*
 * +rightleft		Right-to-left editing/typing support.
 *			Note that this isn't perfect, but enough users say they
 *			use it to keep supporting it.
 */
#إذا defined(FEAT_HUGE) && !defined(DISABLE_RIGHTLEFT)
# define FEAT_RIGHTLEFT
#د_که_پای

/*
 * +arabic		Arabic keymap and shaping support.
 *			Requires FEAT_RIGHTLEFT
 */
#إذا defined(FEAT_HUGE) && !defined(DISABLE_ARABIC)
# define FEAT_ARABIC
#د_که_پای
#که_تعریف_شوی FEAT_ARABIC
# ifndef FEAT_RIGHTLEFT
#  define FEAT_RIGHTLEFT
# endif
#د_که_پای

/*
 * +emacs_tags		When FEAT_EMACS_TAGS defined: Include support for
 *			emacs style TAGS file.
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_EMACS_نښانونه
#د_که_پای

/*
 * +cscope		Unix only: Cscope support.
 */
#إذا defined(UNIX) && defined(FEAT_HUGE) && defined(ENABLE_CSCOPE)
# define FEAT_CSCOPE
#د_که_پای

/*
 * +eval		Built-in script language and expression evaluation,
 *			":let", ":if", etc.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_EVAL
#د_که_پای

#که_تعریف_شوی FEAT_EVAL
# define HAVE_SANDBOX
#د_که_پای

/*
 * +profile		Profiling for functions and scripts.
 */
#إذا defined(FEAT_HUGE) \
	&& defined(FEAT_EVAL) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وخت_H)) \
		|| defined(MSWIN))
# define FEAT_PROFILE
#د_که_پای

/*
 * +reltime		reltime() function
 */
#إذا defined(FEAT_نورمال) \
	&& defined(FEAT_EVAL) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وخت_H) \
		&& (!defined(MACOS_X) || defined(HAVE_DISPATCH_DISPATCH_H))) \
	    || defined(MSWIN))
# define FEAT_RELTIME
#د_که_پای

/*
 * +timers		timer_start()
 */
#إذا defined(FEAT_RELTIME) && (defined(UNIX) || defined(MSWIN) || defined(VMS))
# define FEAT_TIMERS
#د_که_پای

/*
 *			Insert mode completion with 'completefunc'.
 */
#إذا defined(FEAT_EVAL)
# define FEAT_COMPL_FUNC
#د_که_پای

/*
 * +printer		":hardcopy" command
 * +postscript		Printing uses PostScript file output.
 */
#إذا defined(FEAT_نورمال) && (defined(MSWIN) || defined(FEAT_EVAL)) \
	&& !defined(AMIGA)
# define FEAT_PRINTER
#د_که_پای
#إذا defined(FEAT_PRINTER) && !defined(FEAT_PRINT_PANGO) \
    && ((defined(MSWIN) && defined(MSWINPS)) \
	    || (!defined(MSWIN) && defined(FEAT_EVAL)))
# define FEAT_POSTSCRIPT
#د_که_پای
#إذا !defined(FEAT_PRINTER) && defined(FEAT_PRINT_PANGO)
# undef FEAT_PRINT_PANGO
#د_که_پای

/*
 * +diff		Displaying diffs in a nice way.
 *			Can be enabled in autoconf already.
 */
#إذا defined(FEAT_نورمال) && !defined(FEAT_DIFF)
# define FEAT_DIFF
#د_که_پای

/*
 * +statusline		'statusline', 'statuslineopt', 'rulerformat' and
 *			special format of 'titlestring' and 'iconstring'
 *			options.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_STL_OPT
#د_که_پای

/*
 * +byte_offset		'%o' in 'statusline' and builtin functions line2byte()
 *			and byte2line().
 *			Note: Required for Macintosh.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_BYTEOFF
#د_که_پای

/*
 * +viminfo		reading/writing the viminfo file. Takes about 8Kbyte
 *			of code.
 * VIMINFO_FILE		Location of user .viminfo file (should start with $).
 * VIMINFO_FILE2	Location of alternate user .viminfo file.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_VIMINFO
// #define VIMINFO_FILE	"$HOME/foo/.viminfo"
// #define VIMINFO_FILE2 "~/bar/.viminfo"
#د_که_پای

/*
 * +syntax		syntax highlighting.  When using this, it's a good
 *			idea to have +eval too.
 */
#إذا defined(FEAT_نورمال)
# define FEAT_SYN_HL
#د_که_پای

/*
 * +conceal		'conceal' option.  Depends on syntax highlighting
 *			as this is how the concealed text is defined.
 */
#إذا defined(FEAT_نورمال) && defined(FEAT_SYN_HL)
# define FEAT_CONCEAL
#د_که_پای

/*
 * +spell		spell checking
 */
#إذا defined(FEAT_نورمال)
# define FEAT_SPELL
#د_که_پای

/*
 * +cryptv		Encryption (originally by Mohsin Ahmed <mosh@sasi.com>).
 */
#إذا defined(FEAT_نورمال) && !defined(FEAT_CRYPT)
# define FEAT_CRYPT
#د_که_پای

/*
 * libsodium - add advanced cryptography support
 */
#إذا defined(HAVE_SODIUM) && defined(FEAT_CRYPT)
# define FEAT_SODIUM
#د_که_پای

/*
 * +mksession		":mksession" command.
 *			fully depends on +eval
 */
#إذا defined(FEAT_EVAL)
# define FEAT_SESSION
#د_که_پای

/*
 * +multi_lang		Multi language support. ":menutrans", ":language", etc.
 * +gettext		Message translations (requires +multi_lang)
 *			(only when "lang" archive unpacked)
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_MULTI_LANG
#د_که_پای
#إذا defined(HAVE_GETTEXT) && defined(FEAT_MULTI_LANG) \
	&& (defined(HAVE_LOCALE_H) || defined(X_LOCALE))
# define FEAT_GETTEXT
#د_که_پای

/*
 * +multi_byte_ime	Win32 IME input method.  Only for far-east Windows, so
 *			IME can be used to input chars.  Not tested much!
 */
#إذا defined(FEAT_GUI_MSWIN) && !defined(FEAT_MBYTE_IME)
// #define FEAT_MBYTE_IME
#د_که_پای

#إذا defined(FEAT_HUGE) && defined(FEAT_GUI_HAIKU) && !defined(FEAT_MBYTE_IME)
# define FEAT_MBYTE_IME
#د_که_پای

// Use iconv() when it's available.
#إذا (defined(HAVE_ICONV_H) && defined(HAVE_ICONV)) || defined(DYNAMIC_ICONV)
# define USE_ICONV
#د_که_پای

/*
 * +xim			X Input Method.  For entering special languages like
 *			chinese and Japanese.
 *			this is for Unix and VMS only.
 */
#که_نه_تعریف_شوی FEAT_XIM
// #define FEAT_XIM
#د_که_پای

#إذا defined(FEAT_XIM) && defined(FEAT_GUI_GTK)
# define USE_XIM 1		// needed for GTK include files
#د_که_پای

#إذا defined(FEAT_XIM)
// # define X_LOCALE			// for OS with incomplete locale
					// support, like old linux versions.
#د_که_پای

/*
 * +xfontset		X fontset support.  For outputting wide characters.
 */
#که_نه_تعریف_شوی FEAT_XFONTSET
# إذا defined(HAVE_X11) && !defined(FEAT_GUI_GTK)
#  define FEAT_XFONTSET
# وإلا
// #  define FEAT_XFONTSET
# endif
#وإلا
# إذا defined(USE_GTK4)
#  undef FEAT_XFONTSET
# endif
#د_که_پای

/*
 * +libcall		libcall() function
 */
// Using dlopen() also requires dlsym() to be available.
#إذا defined(HAVE_DLOPEN) && defined(HAVE_DLSYM)
# define USE_DLOPEN
#د_که_پای
#إذا defined(FEAT_EVAL) && (defined(MSWIN) || ((defined(UNIX) || defined(VMS)) \
	&& (defined(USE_DLOPEN) || defined(HAVE_SHL_بارول))))
# define FEAT_LIBCALL
#د_که_پای

/*
 * +menu		":menu" command
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_MENU
# ifdef FEAT_GUI_MSWIN
#  define FEAT_TEAROFF
# endif
#د_که_پای

/*
 * popup menu in a terminal
 */
#إذا defined(FEAT_MENU) && !defined(ALWAYS_USE_GUI)
# define FEAT_غړی_POPUP_MENU
#د_که_پای

/*
 * sound
 */
#إذا !defined(FEAT_SOUND) && defined(HAVE_CANBERRA)
# define FEAT_SOUND
#د_که_پای
#إذا defined(FEAT_SOUND) && defined(HAVE_CANBERRA)
# define FEAT_SOUND_CANBERRA
#د_که_پای

// There are two ways to use XPM.
#إذا (defined(HAVE_XM_XPMP_H) && defined(FEAT_GUI_MOTIF)) \
		|| defined(HAVE_X11_XPM_H)
# define HAVE_XPM 1
#د_که_پای

/*
 * +toolbar		Include code for a toolbar (for the Win32 GUI, GTK
 *			always has it).  But only if menus are enabled.
 */
#إذا defined(FEAT_نورمال) && defined(FEAT_MENU) \
	&& (defined(FEAT_GUI_GTK) \
		|| defined(FEAT_GUI_MSWIN) \
		|| (defined(FEAT_GUI_MOTIF) && defined(HAVE_XPM)) \
		|| defined(FEAT_GUI_PHOTON) \
		|| defined(FEAT_GUI_HAIKU))

# define FEAT_TOOLBAR
#د_که_پای


#إذا defined(FEAT_TOOLBAR) && !defined(FEAT_MENU)
# define FEAT_MENU
#د_که_پای

/*
 * GUI dark theme variant
 */
#إذا (defined(FEAT_GUI_GTK) && (defined(USE_GTK3) || defined(USE_GTK4))) \
	|| defined(FEAT_GUI_MSWIN)
# define FEAT_GUI_DARKTHEME
#د_که_پای

/*
 * GUI tabline
 */
#إذا defined(FEAT_نورمال) \
    && (defined(FEAT_GUI_GTK) \
	|| (defined(FEAT_GUI_MOTIF) && defined(HAVE_XM_NOTEBOOK_H)) \
	|| defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_MSWIN))
# define FEAT_GUI_TABLINE
#د_که_پای

/*
 * +tabpanel		Tab SideBar
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_TABPANEL
#د_که_پای

/*
 * +browse		":browse" command.
 *			or just the ":browse" command modifier
 */
#إذا defined(FEAT_نورمال)
# define FEAT_BROWSE_CMD
# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) || defined(FEAT_GUI_PHOTON)
#  define FEAT_BROWSE
# endif
#د_که_پای

/*
 * On some systems, when we compile with the GUI, we always use it.  On Mac
 * there is no terminal version, and on Windows we can't figure out how to
 * fork one off with :gui.
 */
#إذا defined(FEAT_GUI_MSWIN) && !defined(VIMDLL)
# define ALWAYS_USE_GUI
#د_که_پای

/*
 * +dialog_gui		Use GUI dialog.
 * +dialog_con		May use Console dialog.
 *			When none of these defined there is no dialog support.
 */
#که_تعریف_شوی FEAT_نورمال
# إذا (defined(FEAT_GUI_MOTIF) && defined(HAVE_X11_XPM_H)) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON) \
	|| defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_MSWIN)
#  define FEAT_CON_DIALOG
#  define FEAT_GUI_DIALOG
# وإلا
#  define FEAT_CON_DIALOG
# endif
#د_که_پای
#إذا !defined(FEAT_GUI_DIALOG) && (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_MSWIN))
// need a dialog to show error messages when starting from the desktop
# define FEAT_GUI_DIALOG
#د_که_پای
#إذا defined(FEAT_GUI_DIALOG) && \
	(defined(FEAT_GUI_MOTIF) \
	 || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN) \
	 || defined(FEAT_GUI_PHOTON) \
	 || defined(FEAT_GUI_HAIKU))
# define FEAT_GUI_TEXTDIALOG
# ifndef ALWAYS_USE_GUI
#  define FEAT_CON_DIALOG
# endif
#د_که_پای

/*
 * +termguicolors	'termguicolors' option.
 */
#إذا (defined(FEAT_نورمال) && defined(FEAT_SYN_HL)) && !defined(ALWAYS_USE_GUI)
# define FEAT_TERMGUICOLORS
#د_که_پای

/*
 * +vartabs		'vartabstop' and 'varsofttabstop' options.
 */
#که_تعریف_شوی FEAT_HUGE
# define FEAT_VARTABS
#د_که_پای

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
#که_نه_تعریف_شوی VMS
# define FEAT_WRITEBACKUP
#د_که_پای

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
#إذا defined(FEAT_نورمال) \
	&& ((defined(HAVE_GETTIMEOFDAY) && defined(HAVE_SYS_وخت_H)) \
		|| defined(MSWIN))
# define STARTUPTIME 1
#د_که_پای

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
#تعریف_کړه SESSION_FILE	"Session.vim"

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
#که_نه_تعریف_شوی FILETYPE_FILE
# define FILETYPE_FILE		"filetype.vim"
#د_که_پای
#که_نه_تعریف_شوی FTPLUGIN_FILE
# define FTPLUGIN_FILE		"ftplugin.vim"
#د_که_پای
#که_نه_تعریف_شوی INDENT_FILE
# define INDENT_FILE		"indent.vim"
#د_که_پای
#که_نه_تعریف_شوی FTOFF_FILE
# define FTOFF_FILE		"ftoff.vim"
#د_که_پای
#که_نه_تعریف_شوی FTPLUGOF_FILE
# define FTPLUGOF_FILE		"ftplugof.vim"
#د_که_پای
#که_نه_تعریف_شوی INDOFF_FILE
# define INDOFF_FILE		"indoff.vim"
#د_که_پای

/*
 * SYS_MENU_FILE	Name of the default menu.vim file.
 */
// # define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"

/*
 * SYS_OPTWIN_FILE	Name of the default optwin.vim file.
 */
#که_نه_تعریف_شوی SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME/optwin.vim"
#د_که_پای

/*
 * SYNTAX_FNAME		Name of a syntax file, where %s is the syntax name.
 */
// #define SYNTAX_FNAME	"/foo/%s.vim"

/*
 * RUNTIME_DIRNAME	Generic name for the directory of the runtime files.
 */
#که_نه_تعریف_شوی RUNTIME_DIRNAME
# define RUNTIME_DIRNAME "runtime"
#د_که_پای

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
#إذا (defined(FEAT_نورمال) || defined(FEAT_GUI_MOTIF)) && !defined(USE_GTK4)
# define WANT_X11
#د_که_پای

/*
 * +wayland		Unix only.  Include code for the Wayland protocol,
 *                      only works if HAVE_WAYLAND is defined.
 */
#إذا defined(FEAT_نورمال) && defined(UNIX)
# define WANT_WAYLAND
#د_که_پای

/*
 * XSMP - X11 Session Management Protocol
 * It may be preferred to disable this if the GUI supports it (e.g.,
 * GNOME/KDE) and implement save-yourself etc. through that, but it may also
 * be cleaner to have all SM-aware vims do the same thing (libSM does not
 * depend upon X11).
 * If your GUI wants to support SM itself, change this ifdef.
 * I'm assuming that any X11 implementation will cope with this for now.
 */
#إذا defined(HAVE_X11) && defined(WANT_X11) && defined(HAVE_X11_SM_SMLIB_H)
# define USE_XSMP
#د_که_پای
#إذا defined(USE_XSMP_INTERACT) && !defined(USE_XSMP)
# undef USE_XSMP_INTERACT
#د_که_پای

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
#إذا defined(UNIX) || defined(VMS)
# define FEAT_MOUSE_XTERM
# ifdef FEAT_نورمال
#  define FEAT_MOUSE_NET
#  define FEAT_MOUSE_DEC
#  define FEAT_MOUSE_URXVT
# endif
#د_که_پای
#إذا defined(MSWIN)
# define DOS_MOUSE
#د_که_پای
#إذا defined(__QNX__)
# define FEAT_MOUSE_PTERM
#د_که_پای

/*
 * Note: Only one of the following may be defined:
 * FEAT_MOUSE_GPM
 * FEAT_SYSMOUSE
 * FEAT_MOUSE_JSB
 * FEAT_MOUSE_PTERM
 */
#إذا defined(FEAT_نورمال) && defined(HAVE_GPM)
# define FEAT_MOUSE_GPM
/*
 * +mouse_gpm/dyn   Load libgpm dynamically.
 */
# ifndef DYNAMIC_GPM
// #  define DYNAMIC_GPM
# endif
#د_که_پای

#إذا defined(FEAT_نورمال) && defined(HAVE_SYSMOUSE)
# define FEAT_SYSMOUSE
#د_که_پای

// urxvt is a small variation of mouse_xterm, and shares its code
#إذا defined(FEAT_MOUSE_URXVT) && !defined(FEAT_MOUSE_XTERM)
# define FEAT_MOUSE_XTERM
#د_که_پای

/*
 * +clipboard		Clipboard support.  Always used for the GUI.
 * +xterm_clipboard	Unix only: Include code for handling the clipboard
 *			in an xterm like in the GUI.
 */

#که_تعریف_شوی FEAT_CYGWIN_WIN32_CLIPBOARD
# define FEAT_CLIPBOARD
#د_که_پای

#که_تعریف_شوی FEAT_GUI
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#د_که_پای

#إذا defined(FEAT_نورمال) \
	&& (defined(UNIX) || defined(VMS)) \
	&& defined(WANT_X11) && defined(HAVE_X11) \
	&& !defined(USE_GTK4)
# define FEAT_XCLIPBOARD
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#د_که_پای

#إذا defined(FEAT_نورمال) && defined(UNIX) \
    && defined(HAVE_WAYLAND) && defined(WANT_WAYLAND)
# define FEAT_WAYLAND_CLIPBOARD
# ifndef FEAT_CLIPBOARD
#  define FEAT_CLIPBOARD
# endif
#د_که_پای

/*
 * +dnd		Drag'n'drop support.  Always used for the GTK+ GUI.
 */
#إذا defined(FEAT_CLIPBOARD) && defined(FEAT_GUI_GTK)
# define FEAT_DND
#د_که_پای

#إذا defined(FEAT_GUI_MSWIN)
# define MSWIN_پیدا_کړه_REPLACE	// include code for find/replace dialog
# define MSWIN_FR_BUFSIZE 256
#د_که_پای

#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MOTIF) \
	|| defined(MSWIN_پیدا_کړه_REPLACE)
# define پیدا_کړه_REPLACE_DIALOG 1
#د_که_پای

/*
 * The +channel feature requires +eval.
 */
#إذا !defined(FEAT_EVAL) && defined(FEAT_دنده_CHANNEL)
# undef FEAT_دنده_CHANNEL
#د_که_پای

/*
 * +socketserver	 Use channels for clientserver communication
 */
#إذا (defined(UNIX) || defined(MSWIN)) && defined(FEAT_دنده_CHANNEL)
# define FEAT_SOCKETSERVER
#د_که_پای

/*
 * +clientserver	Remote control via the remote_send() function
 *			and the --remote argument
 */
#إذا (defined(MSWIN) || defined(FEAT_XCLIPBOARD) || defined(FEAT_SOCKETSERVER)) \
    && defined(FEAT_EVAL)
# define FEAT_CLIENTSERVER
# إذا defined(FEAT_SOCKETSERVER) && (defined(FEAT_XCLIPBOARD) || defined(MSWIN))
#  define FEAT_CLIENTSERVER_BACKENDS
# endif
#د_که_پای

/*
 * +autoservername	Automatically generate a servername for clientserver
 *			when --servername is not passed on the command line.
 */
#إذا defined(FEAT_CLIENTSERVER) && !defined(FEAT_AUTOSERVERNAME)
# ifdef MSWIN
    // Always enabled on MS-Windows.
#  define FEAT_AUTOSERVERNAME
# وإلا
    // Enable here if you don't use configure.
// # define FEAT_AUTOSERVERNAME
# endif
#د_که_پای

/*
 * +termresponse	send t_RV to obtain terminal response.  Used for xterm
 *			to check if mouse dragging can be used and if term
 *			codes can be obtained.
 */
#إذا defined(HAVE_TGETENT)
# define FEAT_TERMRESPONSE
#د_که_پای

/*
 * cursor shape		Adjust the shape of the cursor to the mode.
 * mouse shape		Adjust the shape of the mouse pointer to the mode.
 */
#که_تعریف_شوی FEAT_نورمال
// Win32 console can change cursor shape
# إذا defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))
#  define MCH_CURSOR_SHAPE
# endif
# إذا defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) \
	|| defined(FEAT_GUI_PHOTON)
#  define FEAT_MOUSESHAPE
# endif
#د_که_پای

// GUI and some consoles can change the shape of the cursor.  The code is also
// needed for the 'mouseshape' and 'concealcursor' options.
#إذا defined(FEAT_GUI) \
	    || defined(MCH_CURSOR_SHAPE) \
	    || defined(FEAT_MOUSESHAPE) \
	    || defined(FEAT_CONCEAL) \
	    || (defined(UNIX) && defined(FEAT_نورمال))
# define CURSOR_SHAPE
#د_که_پای

#إذا defined(FEAT_MZSCHEME) && (defined(FEAT_GUI_MSWIN) || defined(FEAT_GUI_GTK)    \
	|| defined(FEAT_GUI_MOTIF))
# define MZSCHEME_GUI_تارونه
#د_که_پای

/*
 * +ARP			Amiga only. Use arp.library, DOS 2.0 is not required.
 */
#إذا defined(AMIGA) && !defined(NO_ARP) && !defined(__amigaos4__) \
	&& !defined(__MORPHOS__) && !defined(__AROS__)
# define FEAT_ARP
#د_که_پای

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
#إذا (!defined(FEAT_EVAL) || !defined(FEAT_دنده_CHANNEL)) && defined(FEAT_NETBEANS_INTG)
# undef FEAT_NETBEANS_INTG
#د_که_پای

/*
 * +terminal		":terminal" command.  Runs a terminal in a window.
 *			requires +channel
 */
#إذا defined(FEAT_TERMINAL) && !defined(FEAT_دنده_CHANNEL)
# undef FEAT_TERMINAL
#د_که_پای
#إذا defined(FEAT_TERMINAL) && !defined(CURSOR_SHAPE)
# define CURSOR_SHAPE
#د_که_پای
#إذا defined(FEAT_TERMINAL) && !defined(FEAT_SYN_HL)
// simplify the code a bit by enabling +syntax when +terminal is enabled
# define FEAT_SYN_HL
#د_که_پای

/*
 * +autoshelldir	    'autoshelldir' option.
 */
#إذا defined(FEAT_TERMINAL)
# define FEAT_AUTOSHELLDIR
#د_که_پای
/*
 * +textprop and +popupwin	Text PROPerties and POPUP windows
 */
#إذا defined(FEAT_EVAL) && defined(FEAT_SYN_HL)
# define FEAT_PROP_POPUP
#د_که_پای


/*
 * +image	    Render images in the terminal and GUI
 * +image_popup	    Render images in popup windows
 */
#إذا defined(FEAT_HUGE) && defined(FEAT_PROP_POPUP) && defined(HAVE_PIXMAN)
# define ALLOW_IMAGE
#د_که_پای

#إذا defined(ALLOW_IMAGE) && !defined(ALWAYS_USE_GUI)
# define FEAT_IMAGE_SIXEL
# define FEAT_IMAGE_KITTY
#د_که_پای

#إذا defined(ALLOW_IMAGE) && ((defined(FEAT_GUI_GTK) \
	    && (defined(USE_GTK4) || defined(USE_GTK3))) || defined(FEAT_GUI_MSWIN))
# define FEAT_IMAGE_GUI
#د_که_پای

#إذا defined(FEAT_IMAGE_SIXEL) || defined(FEAT_IMAGE_KITTY) \
    || defined(FEAT_IMAGE_GUI)
# define FEAT_IMAGE_POPUP
# define FEAT_IMAGE
#د_که_پای

#که_تعریف_شوی ALLOW_IMAGE
# undef ALLOW_IMAGE
#د_که_پای

/*
 * +message_window	use a popup for messages when 'cmdheight' is zero
 */
#إذا defined(FEAT_PROP_POPUP) && defined(FEAT_TIMERS)
# define HAS_پیغام_کړکۍ
#د_که_پای

#إذا defined(FEAT_SYN_HL) && defined(FEAT_RELTIME)
// Can limit syntax highlight time to 'redrawtime'.
# define SYN_وخت_حد 1
#د_که_پای


/*
 * +signs		Allow signs to be displayed to the left of text lines.
 *			Adds the ":sign" command.
 */
#إذا defined(FEAT_نورمال) || defined(FEAT_NETBEANS_INTG) || defined(FEAT_PROP_POPUP)
# define FEAT_SIGNS
# إذا (defined(FEAT_GUI_MOTIF) && defined(HAVE_X11_XPM_H)) \
	|| defined(FEAT_GUI_GTK) \
	|| (defined(MSWIN) && defined(FEAT_GUI))
#  define FEAT_SIGN_ICONS
# endif
#د_که_پای

/*
 * +balloon_eval	Allow balloon expression evaluation. Used with a
 *			debugger and for tooltips.
 *			Only for GUIs where it was implemented.
 */
#إذا (defined(FEAT_GUI_MOTIF) \
	|| defined(FEAT_GUI_GTK) || defined(FEAT_GUI_MSWIN)) \
	&& (   ((defined(FEAT_TOOLBAR) || defined(FEAT_GUI_TABLINE)) \
		&& !defined(FEAT_GUI_GTK) && !defined(FEAT_GUI_MSWIN)) \
	    || defined(FEAT_NETBEANS_INTG) || defined(FEAT_EVAL))
# define FEAT_BEVAL_GUI
# إذا !defined(FEAT_XFONTSET) && !defined(FEAT_GUI_GTK) \
	&& !defined(FEAT_GUI_MSWIN)
#  define FEAT_XFONTSET
# endif
#د_که_پای

#إذا defined(FEAT_BEVAL_GUI) && defined(FEAT_GUI_MOTIF)
# define FEAT_BEVAL_TIP		// balloon eval used for toolbar tooltip
#د_که_پای

/*
 * +balloon_eval_term	Allow balloon expression evaluation in the terminal.
 */
#إذا defined(FEAT_HUGE) && defined(FEAT_TIMERS) && \
	(defined(UNIX) || defined(VMS) || \
	 (defined(MSWIN) && (!defined(FEAT_GUI_MSWIN) || defined(VIMDLL))))
# define FEAT_BEVAL_غړی
#د_که_پای

#إذا defined(FEAT_BEVAL_GUI) || defined(FEAT_BEVAL_غړی)
# define FEAT_BEVAL
#د_که_پای

// Motif is X11
#إذا defined(FEAT_GUI_MOTIF)
# define FEAT_GUI_X11
#د_که_پای

#إذا defined(FEAT_NETBEANS_INTG)
// NetBeans uses menus.
# إذا !defined(FEAT_MENU)
#  define FEAT_MENU
# endif
#د_که_پای

/*
 * +autochdir		'autochdir' option.
 */
#إذا defined(FEAT_NETBEANS_INTG) || defined(FEAT_نورمال)
# define FEAT_AUTOCHDIR
#د_که_پای

/*
 * +persistent_undo	'undofile', 'undodir' options, :wundo and :rundo, and
 * implementation.
 */
#که_تعریف_شوی FEAT_نورمال
# define FEAT_PERSISTENT_UNDO
#د_که_پای

/*
 * +filterpipe
 */
#إذا (defined(UNIX) && !defined(USE_SYSTEM)) \
	    || (defined(MSWIN) && defined(FEAT_GUI_MSWIN))
# define FEAT_FILTERPIPE
#د_که_پای

/*
 * +vtp: Win32 virtual console.
 */
#إذا (!defined(FEAT_GUI) || defined(VIMDLL)) && defined(MSWIN)
# define FEAT_VTP
#د_که_پای

#إذا defined(DYNAMIC_PERL) \
	|| defined(DYNAMIC_PYTHON) || defined(DYNAMIC_PYTHON3) \
	|| defined(DYNAMIC_RUBY) \
	|| defined(DYNAMIC_TCL) \
	|| defined(DYNAMIC_ICONV) \
	|| defined(DYNAMIC_GETTEXT) \
	|| defined(DYNAMIC_MZSCHEME) \
	|| defined(DYNAMIC_LUA) \
	|| defined(FEAT_TERMINAL)
# define USING_بارول_LIBRARY
#د_که_پای

/*
 * currently Unix only: XATTR support
 */

#إذا defined(FEAT_نورمال) && defined(HAVE_XATTR) && !defined(MACOS_X)
# define FEAT_XATTR
#د_که_پای
