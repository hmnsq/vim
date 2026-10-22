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

#وصف_ڪريو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#وصف_ڪريو USE_FNAME_CASE		    // adjust case of file names
#وصف_ڪريو USE_رڪن_CONSOLE
#وصف_ڪريو HAVE_AVAIL_MEM

#جيڪڏهن_وصف_نه_ٿيل HAVE_CONFIG_H
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
#  define HAVE_DATE_وقت
# endif

#جيڪڏهن_پورو // HAVE_CONFIG_H

#جيڪڏهن_وصف_نه_ٿيل	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#جيڪڏهن_پورو

#ضمن <exec/types.h>
#ضمن <libraries/dos.h>
#ضمن <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#جيڪڏهن_وصف_نه_ٿيل AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#جيڪڏهن_پورو

#وصف_ڪريو FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#جيڪڏهن_وصف_ٿيل AZTEC_C
تعريفنوع ڊگهو off_t;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#جيڪڏهن_پورو

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#إذا defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, گهڻائي) (0)
# define ftruncate(fd, len) (0)
#جيڪڏهن_پورو

#ضمن <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#جيڪڏهن_وصف_ٿيل FEAT_ARP
# include <libraries/arpbase.h>
#جيڪڏهن_پورو

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ضمن <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#جيڪڏهن_وصف_نه_ٿيل SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل EXRC_FILE
# define EXRC_FILE	".exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#جيڪڏهن_پورو

#إذا defined(SASC)
صحيح setenv(ثابت حرف *, ثابت حرف *);
#جيڪڏهن_پورو

#وصف_ڪريو mch_remove(x) remove((حرف *)(x))
#وصف_ڪريو mch_rename(src, dst) rename(src, dst)
#وصف_ڪريو mch_chdir(s) chdir(s)
#وصف_ڪريو vim_mkdir(x, y) mch_mkdir(x)
