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

#నిర్వచించు CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names
#నిర్వచించు USE_FNAME_CASE		    // adjust case of file names
#నిర్వచించు USE_పదం_CONSOLE
#నిర్వచించు HAVE_AVAIL_MEM

#నిర్వచితం_కాకపోతే HAVE_CONFIG_H
# ఒకవేళ defined(AZTEC_C) || defined(__amigaos4__)
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
# ఒకవేళ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_సమయం
# endif

#అయితే_ముగింపు // HAVE_CONFIG_H

#నిర్వచితం_కాకపోతే	DFLT_ERRORFILE
# define DFLT_ERRORFILE		"AztecC.Err"	// Should this change?
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే	DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే	CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH "VIM:vimfiles,VIM:,VIM:vimfiles/after"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే	BASENAMELEN
# define BASENAMELEN	26	// Amiga
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే	TEMPNAME
# define TEMPNAME	"t:v?XXXXXX"
# define TEMPNAMELEN	12
#అయితే_ముగింపు

#అన్తర్భావ <exec/types.h>
#అన్తర్భావ <libraries/dos.h>
#అన్తర్భావ <libraries/dosextens.h>

// Currently, all Amiga compilers except AZTEC C have these...
#నిర్వచితం_కాకపోతే AZTEC_C
# include <proto/exec.h>
# include <proto/dos.h>
# include <proto/intuition.h>
#అయితే_ముగింపు

#నిర్వచించు FNAME_ILLEGAL ";*?`#%" // illegal characters in a file name

/*
 * Manx doesn't have off_t, define it here.
 */
#నిర్వచితమైతే AZTEC_C
రకనిర్వచనం దీర్ఘ off_t;
#అయితే_ముగింపు

#నిర్వచితమైతే LATTICE
# define USE_TMPNAM	// use tmpnam() instead of mktemp()
#అయితే_ముగింపు

#నిర్వచితమైతే __GNUC__
# include <sys/stat.h>
# include <unistd.h>
# include <limits.h>
# include <errno.h>
# include <pwd.h>
# include <grp.h>
# include <dirent.h>
#అయితే_ముగింపు

// Classic AmigaOS 3.x with GCC/libnix does not provide fchown, fchmod, or
// ftruncate.  Stub them as no-ops.  (OS4 has these via clib2; MorphOS and
// AROS provide them in their respective C libraries.)
#ఒకవేళ defined(__GNUC__) && defined(AMIGA) && !defined(__amigaos4__) \
	&& !defined(__AROS__) && !defined(__MORPHOS__)
# define fchown(fd, uid, gid) (0)
# define fchmod(fd, బహుళకం) (0)
# define ftruncate(fd, len) (0)
#అయితే_ముగింపు

#అన్తర్భావ <time.h>	// for strftime() and others

/*
 * arpbase.h must be included before functions.h
 */
#నిర్వచితమైతే FEAT_ARP
# include <libraries/arpbase.h>
#అయితే_ముగింపు

/*
 * This won't be needed if you have a version of Lattice 4.01 without broken
 * break signal handling.
 */
#అన్తర్భావ <signal.h>

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */
#నిర్వచితం_కాకపోతే SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "VIM:vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "VIM:gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_MENU_FILE
# define SYS_MENU_FILE	"VIM:menu.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే DFLT_HELPFILE
# define DFLT_HELPFILE	"VIM:doc/help.txt"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYNTAX_FNAME
# define SYNTAX_FNAME	"VIM:syntax/%s.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_EXRC_FILE
# define USR_EXRC_FILE	"HOME:.exrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_EXRC_FILE2
# define USR_EXRC_FILE2	"VIM:.exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_VIMRC_FILE
# define USR_VIMRC_FILE	"HOME:.vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE2
# define USR_VIMRC_FILE2 "VIM:.vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE3
# define USR_VIMRC_FILE3 "HOME:vimfiles/vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE4
# define USR_VIMRC_FILE4 "S:.vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "VIM:defaults.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే EVIM_FILE
# define EVIM_FILE	"VIM:evim.vim"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE
# define USR_GVIMRC_FILE "HOME:.gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE2
# define USR_GVIMRC_FILE2 "VIM:.gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE3
# define USR_GVIMRC_FILE3 "HOME:vimfiles/gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_GVIMRC_FILE4
# define USR_GVIMRC_FILE4 "S:.gvimrc"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"VIM:.viminfo"
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే EXRC_FILE
# define EXRC_FILE	".exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే VIMRC_FILE
# define VIMRC_FILE	".vimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే GVIMRC_FILE
# define GVIMRC_FILE	".gvimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_BDIR
# define DFLT_BDIR	"T:"		// default for 'backupdir'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_DIR
# define DFLT_DIR	"T:"		// default for 'directory'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_VDIR
# define DFLT_VDIR	"HOME:vimfiles/view"	// default for 'viewdir'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_MAXMEM
# define DFLT_MAXMEM	256		// use up to 256Kbyte for buffer
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	0		// decide in set_init
#అయితే_ముగింపు

#ఒకవేళ defined(SASC)
పూర్ణాంకం setenv(స్థిరరాశి అక్షరం *, స్థిరరాశి అక్షరం *);
#అయితే_ముగింపు

#నిర్వచించు mch_remove(x) remove((అక్షరం *)(x))
#నిర్వచించు mch_rename(src, dst) rename(src, dst)
#నిర్వచించు mch_chdir(s) chdir(s)
#నిర్వచించు vim_mkdir(x, y) mch_mkdir(x)
