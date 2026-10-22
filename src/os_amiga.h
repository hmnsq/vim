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

#تعریف_کرو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#تعریف_کرو USE_FNAME_CASE		    // adjust case of file names
#تعریف_کرو USE_رکن_CONSOLE
#تعریف_کرو HAVE_AVAIL_MEM

#جے_بنا_تعریف HAVE_CONFIG_H
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
#  define HAVE_DATE_ویلا
# endif

#جے_انت // HAVE_CONFIG_H

#جے_بنا_تعریف	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#جے_انت

#جے_بنا_تعریف	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#جے_انت
#جے_بنا_تعریف	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#جے_انت

#جے_بنا_تعریف	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#جے_انت

#جے_بنا_تعریف	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#جے_انت

#ضمن <exec/types.h>
#ضمن <libraries/dos.h>
#ضمن <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#جے_بنا_تعریف AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#جے_انت

#تعریف_کرو FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#جے_تعریف_شدہ AZTEC_C
تعريفنوع لما off_t;
#جے_انت

#جے_تعریف_شدہ LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#جے_انت

#جے_تعریف_شدہ __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#جے_انت

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#إذا defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, منوال) (0)
# define ftruncate(fd, len) (0)
#جے_انت

#ضمن <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#جے_تعریف_شدہ FEAT_ARP
# include <libraries/arpbase.h>
#جے_انت

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ضمن <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#جے_بنا_تعریف SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#جے_انت
#جے_بنا_تعریف SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#جے_انت
#جے_بنا_تعریف SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#جے_انت
#جے_بنا_تعریف DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#جے_انت
#جے_بنا_تعریف SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#جے_انت

#جے_بنا_تعریف USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#جے_انت
#جے_بنا_تعریف USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#جے_انت

#جے_بنا_تعریف USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#جے_انت
#جے_بنا_تعریف USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#جے_انت
#جے_بنا_تعریف VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#جے_انت
#جے_بنا_تعریف EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#جے_انت

#جے_بنا_تعریف USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#جے_انت
#جے_بنا_تعریف USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#جے_انت
#جے_بنا_تعریف USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#جے_انت
#جے_بنا_تعریف USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#جے_انت

#جے_تعریف_شدہ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#جے_انت

#جے_بنا_تعریف EXRC_FILE
# define EXRC_FILE	".exrc"
#جے_انت

#جے_بنا_تعریف VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#جے_انت

#جے_بنا_تعریف GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#جے_انت

#جے_بنا_تعریف DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#جے_انت

#جے_بنا_تعریف DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#جے_انت

#جے_بنا_تعریف DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#جے_انت

#جے_بنا_تعریف DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#جے_انت
#جے_بنا_تعریف DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#جے_انت

#إذا defined(SASC)
صحيح setenv(ثابت حرف *, ثابت حرف *);
#جے_انت

#تعریف_کرو mch_remove(x) remove((حرف *)(x))
#تعریف_کرو mch_rename(src, dst) rename(src, dst)
#تعریف_کرو mch_chdir(s) chdir(s)
#تعریف_کرو vim_mkdir(x, y) mch_mkdir(x)
