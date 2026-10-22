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

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_FNAME_CASE		    // adjust case of file names
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_ਪਦ_CONSOLE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_AVAIL_MEM

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
# ਜੇ defined(AZTEC_C) || defined(__amigaos4__)
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
# ਜੇ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_ਸਮਾਂ
# endif

#ਜੇ_ਅੰਤ // HAVE_CONFIG_H

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <exec/types.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libraries/dos.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AZTEC_C
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਲੰਮਾ off_t;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#ਜੇ_ਅੰਤ

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#ਜੇ defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, ਬਹੁਲਕ) (0)
# define ftruncate(fd, len) (0)
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
# include <libraries/arpbase.h>
#ਜੇ_ਅੰਤ

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EXRC_FILE
# define EXRC_FILE	".exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#ਜੇ_ਅੰਤ

#ਜੇ defined(SASC)
ਪੂਰਨ setenv(ਅਚਲ ਅੱਖਰ *, ਅਚਲ ਅੱਖਰ *);
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_remove(x) remove((ਅੱਖਰ *)(x))
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_rename(src, dst) rename(src, dst)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_chdir(s) chdir(s)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ vim_mkdir(x, y) mch_mkdir(x)
