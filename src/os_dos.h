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

#నిర్వచితం_కాకపోతే SYS_VIMRC_FILE
# define SYS_VIMRC_FILE		"$VIM\\vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE
# define USR_VIMRC_FILE		"$HOME\\_vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE2
# define USR_VIMRC_FILE2	"$HOME\\vimfiles\\vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE3
# define USR_VIMRC_FILE3	"$VIM\\_vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE	"$VIMRUNTIME\\defaults.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే EVIM_FILE
# define EVIM_FILE		"$VIMRUNTIME\\evim.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_EXRC_FILE
# define USR_EXRC_FILE		"$HOME\\_exrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_EXRC_FILE2
# define USR_EXRC_FILE2		"$VIM\\_exrc"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
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
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE	"$VIMRUNTIME\\optwin.vim"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE		"$HOME\\_viminfo"
# endif
# ifndef VIMINFO_FILE2
#  define VIMINFO_FILE2		"$VIM\\_viminfo"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే EXRC_FILE
# define EXRC_FILE	"_exrc"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME\\doc\\help.txt"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME\\syntax\\%s.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_BDIR
# define DFLT_BDIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'backupdir'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_VDIR
# define DFLT_VDIR	"$HOME/vimfiles/view"	// default for 'viewdir'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_DIR
# define DFLT_DIR	".,$TEMP,c:\\tmp,c:\\temp" // default for 'directory'
#అయితే_ముగింపు

#నిర్వచించు DFLT_ERRORFILE		"errors.err"
#నిర్వచించు DFLT_RUNTIMEPATH	"$HOME/vimfiles,$VIM/vimfiles,$VIMRUNTIME,$HOME/vimfiles/after,$VIM/vimfiles/after"
#నిర్వచించు CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"

#నిర్వచించు CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#నిర్వచించు BACKSLASH_లోపల_FILENAME
#నిర్వచించు USE_CRNL		// lines end in CR-NL instead of NL
#నిర్వచించు HAVE_DUP		// have dup()
#నిర్వచించు HAVE_ST_బహుళకం		// have stat.st_mode
