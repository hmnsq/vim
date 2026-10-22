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

#اگر_غیر_تعریف_شدہ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM\\vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ USR_VIMRC_FILE
# define USR_VIMRC_FILE		"$HOME\\_vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"$HOME\\vimfiles\\vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ USR_VIMRC_FILE3
# define USR_VIMRC_FILE3	"$VIM\\_vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE	"$VIMRUNTIME\\defaults.vim"
#اگر_ختم
#اگر_غیر_تعریف_شدہ EVIM_FILE
# define EVIM_FILE		"$VIMRUNTIME\\evim.vim"
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_EXRC_FILE
# define USR_EXRC_FILE		"$HOME\\_exrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ USR_EXRC_FILE2
# define USR_EXRC_FILE2		"$VIM\\_exrc"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
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
#اگر_ختم

#اگر_غیر_تعریف_شدہ SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME\\optwin.vim"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE		"$HOME\\_viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2		"$VIM\\_viminfo"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ EXRC_FILE
# define EXRC_FILE	"_exrc"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME\\doc\\help.txt"
#اگر_ختم

#اگر_غیر_تعریف_شدہ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME\\syntax\\%s.vim"
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_BDIR
# define DFLT_BDIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'backupdir'
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_VDIR
# define DFLT_VDIR	"$HOME/vimfiles/view"	// default for 'viewdir'
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_DIR
# define DFLT_DIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'directory'
#اگر_ختم

#تعریف_کرو DFLT_ERRORFILE		"errors.err"
#تعریف_کرو DFLT_RUNTIMEPATH	"$HOME/vimfiles,$VIM/vimfiles,$VIMRUNTIME,$HOME/vimfiles/after,$VIM/vimfiles/after"
#تعریف_کرو CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"

#تعریف_کرو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کرو BACKSLASH_اندر_FILENAME
#تعریف_کرو USE_CRNL		// lines end in CR-NL instead of NL
#تعریف_کرو HAVE_DUP		// have dup()
#تعریف_کرو HAVE_ST_منوال		// have stat.st_mode
