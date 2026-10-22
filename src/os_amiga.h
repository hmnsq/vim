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

#تعریف_کړه CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کړه USE_FNAME_CASE		    // adjust case of file names
#تعریف_کړه USE_غړی_CONSOLE
#تعریف_کړه HAVE_AVAIL_MEM

#که_نه_تعریف_شوی HAVE_CONFIG_H
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
#  define HAVE_DATE_وخت
# endif

#د_که_پای // HAVE_CONFIG_H

#که_نه_تعریف_شوی	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#د_که_پای

#که_نه_تعریف_شوی	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#د_که_پای
#که_نه_تعریف_شوی	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#د_که_پای

#که_نه_تعریف_شوی	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#د_که_پای

#که_نه_تعریف_شوی	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#د_که_پای

#ضمن <exec/types.h>
#ضمن <libraries/dos.h>
#ضمن <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#که_نه_تعریف_شوی AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#د_که_پای

#تعریف_کړه FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#که_تعریف_شوی AZTEC_C
تعريفنوع اوږد off_t;
#د_که_پای

#که_تعریف_شوی LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#د_که_پای

#که_تعریف_شوی __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#د_که_پای

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#إذا defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, ډېرپېښ) (0)
# define ftruncate(fd, len) (0)
#د_که_پای

#ضمن <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#که_تعریف_شوی FEAT_ARP
# include <libraries/arpbase.h>
#د_که_پای

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ضمن <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#که_نه_تعریف_شوی SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#د_که_پای
#که_نه_تعریف_شوی SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#د_که_پای
#که_نه_تعریف_شوی SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#د_که_پای
#که_نه_تعریف_شوی DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#د_که_پای
#که_نه_تعریف_شوی SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#د_که_پای

#که_نه_تعریف_شوی USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#د_که_پای
#که_نه_تعریف_شوی USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#د_که_پای

#که_نه_تعریف_شوی USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#د_که_پای
#که_نه_تعریف_شوی VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#د_که_پای
#که_نه_تعریف_شوی EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#د_که_پای

#که_نه_تعریف_شوی USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#د_که_پای
#که_نه_تعریف_شوی USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#د_که_پای

#که_نه_تعریف_شوی EXRC_FILE
# define EXRC_FILE	".exrc"
#د_که_پای

#که_نه_تعریف_شوی VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#د_که_پای

#که_نه_تعریف_شوی GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#د_که_پای

#که_نه_تعریف_شوی DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#د_که_پای

#که_نه_تعریف_شوی DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#د_که_پای

#که_نه_تعریف_شوی DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#د_که_پای

#که_نه_تعریف_شوی DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#د_که_پای
#که_نه_تعریف_شوی DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#د_که_پای

#إذا defined(SASC)
صحيح setenv(ثابت حرف *, ثابت حرف *);
#د_که_پای

#تعریف_کړه mch_remove(x) remove((حرف *)(x))
#تعریف_کړه mch_rename(src, dst) rename(src, dst)
#تعریف_کړه mch_chdir(s) chdir(s)
#تعریف_کړه vim_mkdir(x, y) mch_mkdir(x)
