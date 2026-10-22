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

#ವ್ಯಾಖ್ಯಾನಿಸು CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#ವ್ಯಾಖ್ಯಾನಿಸು USE_FNAME_CASE		    // adjust case of file names
#ವ್ಯಾಖ್ಯಾನಿಸು USE_ಪದ_CONSOLE
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_AVAIL_MEM

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_CONFIG_H
# ಆದರೆ defined(AZTEC_C) || defined(__amigaos4__)
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
# ಆದರೆ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_ಸಮಯ
# endif

#ಆದರೆ_ಅಂತ್ಯ // HAVE_CONFIG_H

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <exec/types.h>
#ಅನ್ತರ್ಭಾವ <libraries/dos.h>
#ಅನ್ತರ್ಭಾವ <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AZTEC_C
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ದೀರ್ಘ off_t;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#ಆದರೆ_ಅಂತ್ಯ

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#ಆದರೆ defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, ಬಹುಲಕ) (0)
# define ftruncate(fd, len) (0)
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
# include <libraries/arpbase.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#ಅನ್ತರ್ಭಾವ <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EXRC_FILE
# define EXRC_FILE	".exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SASC)
ಪೂರ್ಣಾಂಕ setenv(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು mch_remove(x) remove((ಅಕ್ಷರ *)(x))
#ವ್ಯಾಖ್ಯಾನಿಸು mch_rename(src, dst) rename(src, dst)
#ವ್ಯಾಖ್ಯಾನಿಸು mch_chdir(s) chdir(s)
#ವ್ಯಾಖ್ಯಾನಿಸು vim_mkdir(x, y) mch_mkdir(x)
