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

#جيڪڏهن_وصف_نه_ٿيل SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM\\vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE
# define USR_VIMRC_FILE		"$HOME\\_vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"$HOME\\vimfiles\\vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE3
# define USR_VIMRC_FILE3	"$VIM\\_vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE	"$VIMRUNTIME\\defaults.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل EVIM_FILE
# define EVIM_FILE		"$VIMRUNTIME\\evim.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_EXRC_FILE
# define USR_EXRC_FILE		"$HOME\\_exrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_EXRC_FILE2
# define USR_EXRC_FILE2		"$VIM\\_exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME\\optwin.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE		"$HOME\\_viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2		"$VIM\\_viminfo"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل EXRC_FILE
# define EXRC_FILE	"_exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME\\doc\\help.txt"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME\\syntax\\%s.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_BDIR
# define DFLT_BDIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'backupdir'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_VDIR
# define DFLT_VDIR	"$HOME/vimfiles/view"	// default for 'viewdir'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_DIR
# define DFLT_DIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'directory'
#جيڪڏهن_پورو

#وصف_ڪريو DFLT_ERRORFILE		"errors.err"
#وصف_ڪريو DFLT_RUNTIMEPATH	"$HOME/vimfiles,$VIM/vimfiles,$VIMRUNTIME,$HOME/vimfiles/after,$VIM/vimfiles/after"
#وصف_ڪريو CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"

#وصف_ڪريو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#وصف_ڪريو BACKSLASH_اندريون_FILENAME
#وصف_ڪريو USE_CRNL		// lines end in CR-NL instead of NL
#وصف_ڪريو HAVE_DUP		// have dup()
#وصف_ڪريو HAVE_ST_گهڻائي		// have stat.st_mode
