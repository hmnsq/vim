/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Common MS-DOS and Win32 (Windows NT and Windows 95) defines.
 *
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile or feature.h.
 */

#که_نه_تعریف_شوی SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM\\vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE
# define USR_VIMRC_FILE		"$HOME\\_vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"$HOME\\vimfiles\\vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE3
# define USR_VIMRC_FILE3	"$VIM\\_vimrc"
#د_که_پای
#که_نه_تعریف_شوی VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE	"$VIMRUNTIME\\defaults.vim"
#د_که_پای
#که_نه_تعریف_شوی EVIM_FILE
# define EVIM_FILE		"$VIMRUNTIME\\evim.vim"
#د_که_پای

#که_نه_تعریف_شوی USR_EXRC_FILE
# define USR_EXRC_FILE		"$HOME\\_exrc"
#د_که_پای
#که_نه_تعریف_شوی USR_EXRC_FILE2
# define USR_EXRC_FILE2		"$VIM\\_exrc"
#د_که_پای

#که_تعریف_شوی FEAT_GUI
# ifndef SYS_GVIMRC_FILE
#  define SYS_GVIMRC_FILE	"$VIM\\gvimrc"
# endif
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE	"$HOME\\_gvimrc"
# endif
# ifndef USR_GVIMRC_FILE2
#  define USR_GVIMRC_FILE2	"$HOME\\vimfiles\\gvimrc"
# endif
# ifndef USR_GVIMRC_FILE3
#  define USR_GVIMRC_FILE3	"$VIM\\_gvimrc"
# endif
# ifndef SYS_MENU_FILE
#  define SYS_MENU_FILE		"$VIMRUNTIME\\menu.vim"
# endif
#د_که_پای

#که_نه_تعریف_شوی SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME\\optwin.vim"
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE		"$HOME\\_viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2		"$VIM\\_viminfo"
# endif
#د_که_پای

#که_نه_تعریف_شوی VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#د_که_پای

#که_نه_تعریف_شوی EXRC_FILE
# define EXRC_FILE	"_exrc"
#د_که_پای

#که_تعریف_شوی FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#د_که_پای

#که_نه_تعریف_شوی DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME\\doc\\help.txt"
#د_که_پای

#که_نه_تعریف_شوی SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME\\syntax\\%s.vim"
#د_که_پای

#که_نه_تعریف_شوی DFLT_BDIR
# define DFLT_BDIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'backupdir'
#د_که_پای

#که_نه_تعریف_شوی DFLT_VDIR
# define DFLT_VDIR	"$HOME/vimfiles/view"	// default for 'viewdir'
#د_که_پای

#که_نه_تعریف_شوی DFLT_DIR
# define DFLT_DIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'directory'
#د_که_پای

#تعریف_کړه DFLT_ERRORFILE		"errors.err"
#تعریف_کړه DFLT_RUNTIMEPATH	"$HOME/vimfiles,$VIM/vimfiles,$VIMRUNTIME,$HOME/vimfiles/after,$VIM/vimfiles/after"
#تعریف_کړه CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"

#تعریف_کړه CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کړه BACKSLASH_دننه_FILENAME
#تعریف_کړه USE_CRNL		// lines end in CR-NL instead of NL
#تعریف_کړه HAVE_DUP		// have dup()
#تعریف_کړه HAVE_ST_ډېرپېښ		// have stat.st_mode
