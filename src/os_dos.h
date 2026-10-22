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

#جے_بنا_تعریف SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM\\vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE
# define USR_VIMRC_FILE		"$HOME\\_vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"$HOME\\vimfiles\\vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE3
# define USR_VIMRC_FILE3	"$VIM\\_vimrc"
#جے_انت
#جے_بنا_تعریف VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE	"$VIMRUNTIME\\defaults.vim"
#جے_انت
#جے_بنا_تعریف EVIM_FILE
# define EVIM_FILE		"$VIMRUNTIME\\evim.vim"
#جے_انت

#جے_بنا_تعریف USR_EXRC_FILE
# define USR_EXRC_FILE		"$HOME\\_exrc"
#جے_انت
#جے_بنا_تعریف USR_EXRC_FILE2
# define USR_EXRC_FILE2		"$VIM\\_exrc"
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
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
#جے_انت

#جے_بنا_تعریف SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME\\optwin.vim"
#جے_انت

#جے_تعریف_شدہ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE		"$HOME\\_viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2		"$VIM\\_viminfo"
# endif
#جے_انت

#جے_بنا_تعریف VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#جے_انت

#جے_بنا_تعریف EXRC_FILE
# define EXRC_FILE	"_exrc"
#جے_انت

#جے_تعریف_شدہ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#جے_انت

#جے_بنا_تعریف DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME\\doc\\help.txt"
#جے_انت

#جے_بنا_تعریف SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME\\syntax\\%s.vim"
#جے_انت

#جے_بنا_تعریف DFLT_BDIR
# define DFLT_BDIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'backupdir'
#جے_انت

#جے_بنا_تعریف DFLT_VDIR
# define DFLT_VDIR	"$HOME/vimfiles/view"	// default for 'viewdir'
#جے_انت

#جے_بنا_تعریف DFLT_DIR
# define DFLT_DIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'directory'
#جے_انت

#تعریف_کرو DFLT_ERRORFILE		"errors.err"
#تعریف_کرو DFLT_RUNTIMEPATH	"$HOME/vimfiles,$VIM/vimfiles,$VIMRUNTIME,$HOME/vimfiles/after,$VIM/vimfiles/after"
#تعریف_کرو CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"

#تعریف_کرو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کرو BACKSLASH_وچ_FILENAME
#تعریف_کرو USE_CRNL		// lines end in CR-NL instead of NL
#تعریف_کرو HAVE_DUP		// have dup()
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode
