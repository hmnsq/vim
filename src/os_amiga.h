/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * Amiga Machine-dependent things
 */

#تعریف_کن CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کن USE_FNAME_CASE		    // adjust case of file names
#تعریف_کن USE_جمله_CONSOLE
#تعریف_کن HAVE_AVAIL_MEM

#اگر_تعریف_نشده HAVE_CONFIG_H
# إذا defined(AZTEC_C) || defined(__amigaos4__)
#  define HAVE_STAT_H
# endif
# define HAVE_LOCALE_H
# define HAVE_STDLIB_H
# define HAVE_STRING_H
# define HAVE_FCNTL_H
# define HAVE_STRICMP
# define HAVE_STRNICMP
# define HAVE_STRFTIME	    // guessed
# define HAVE_SETENV
# define HAVE_MEMSET
# define HAVE_QSORT
# إذا defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_زمان
# endif

#پایان_اگر // HAVE_CONFIG_H

#اگر_تعریف_نشده	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#پایان_اگر

#اگر_تعریف_نشده	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#پایان_اگر
#اگر_تعریف_نشده	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#پایان_اگر

#اگر_تعریف_نشده	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#پایان_اگر

#اگر_تعریف_نشده	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#پایان_اگر

#ضمن <exec/types.h>
#ضمن <libraries/dos.h>
#ضمن <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#اگر_تعریف_نشده AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#پایان_اگر

#تعریف_کن FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#اگر_تعریف_شده AZTEC_C
تعريفنوع دراز off_t;
#پایان_اگر

#اگر_تعریف_شده LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#پایان_اگر

#اگر_تعریف_شده __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#پایان_اگر

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#إذا defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, نما) (0)
# define ftruncate(fd, len) (0)
#پایان_اگر

#ضمن <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#اگر_تعریف_شده FEAT_ARP
# include <libraries/arpbase.h>
#پایان_اگر

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ضمن <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#اگر_تعریف_نشده SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#پایان_اگر
#اگر_تعریف_نشده SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#پایان_اگر
#اگر_تعریف_نشده SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#پایان_اگر
#اگر_تعریف_نشده DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#پایان_اگر
#اگر_تعریف_نشده SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#پایان_اگر

#اگر_تعریف_نشده USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#پایان_اگر
#اگر_تعریف_نشده USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#پایان_اگر

#اگر_تعریف_نشده USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#پایان_اگر
#اگر_تعریف_نشده VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#پایان_اگر
#اگر_تعریف_نشده EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#پایان_اگر

#اگر_تعریف_نشده USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#پایان_اگر
#اگر_تعریف_نشده USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#پایان_اگر

#اگر_تعریف_شده FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#پایان_اگر

#اگر_تعریف_نشده EXRC_FILE
# define EXRC_FILE	".exrc"
#پایان_اگر

#اگر_تعریف_نشده VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#پایان_اگر

#اگر_تعریف_نشده GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#پایان_اگر

#اگر_تعریف_نشده DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#پایان_اگر

#اگر_تعریف_نشده DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#پایان_اگر

#اگر_تعریف_نشده DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#پایان_اگر

#اگر_تعریف_نشده DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#پایان_اگر
#اگر_تعریف_نشده DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#پایان_اگر

#إذا defined(SASC)
صحيح setenv(ثابت حرف *, ثابت حرف *);
#پایان_اگر

#تعریف_کن mch_remove(x) remove((حرف *)(x))
#تعریف_کن mch_rename(src, dst) rename(src, dst)
#تعریف_کن mch_chdir(s) chdir(s)
#تعریف_کن vim_mkdir(x, y) mch_mkdir(x)
