/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#నిర్వచితం_కాకపోతే OS_MAC__H
#నిర్వచించు OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#ఒకవేళ 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#అయితే_ముగింపు

// Include MAC_OS_X_VERSION_* macros
#నిర్వచితమైతే HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#అయితే_ముగింపు

/*
 * Unix interface
 */
#ఒకవేళ defined(__APPLE_CC__) // for Project Builder and ...
# include <unistd.h>
// Get stat.h or something similar. Comment: How come some OS get in vim.h
# include <sys/stat.h>
// && defined(HAVE_CURSE)
// The curses.h from MacOS X provides by default some BACKWARD compatibility
// definition which can cause us problem later on. So we undefine a few of them.
# include <curses.h>
# undef reg
# undef ospeed
// OK defined to 0 in MacOS X 10.2 curses!  Remove it, we define it to be 1.
# undef OK
#అయితే_ముగింపు
#అన్తర్భావ <signal.h>
#అన్తర్భావ <errno.h>
#అన్తర్భావ <stdio.h>
#అన్తర్భావ <stdlib.h>
#అన్తర్భావ <time.h>
#అన్తర్భావ <dirent.h>

/*
 * Mach interface
 */
#అన్తర్భావ <mach/task.h>

/*
 * MacOS specific #define
 */

// This will go away when CMD_KEY fully tested
#నిర్వచించు USE_CMD_కీలకం
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#నిర్వచించు USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#నిర్వచించు FEAT_ఆధారం_FFS
#నిర్వచించు FEAT_ఆధారం_FF_MAC

#నిర్వచించు USE_EXE_NAME		    // to find  $VIM
#నిర్వచించు CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#నిర్వచించు USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#నిర్వచించు ద్విమాన_FILE_IO
#నిర్వచించు EOL_DEFAULT EOL_MAC
#నిర్వచించు HAVE_AVAIL_MEM

#నిర్వచితం_కాకపోతే HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_బహుళకం		// have stat.st_mode
# define HAVE_MATH_H

# ఒకవేళ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_సమయం
# endif
# define HAVE_STRFTIME
#అయితే_ముగింపు

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#నిర్వచితం_కాకపోతే SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే EXRC_FILE
# define EXRC_FILE	"_exrc"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#అయితే_ముగింపు // FEAT_VIMINFO

#నిర్వచితం_కాకపోతే DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#అయితే_ముగింపు

#నిర్వచించు DFLT_ERRORFILE		"errors.err"

#నిర్వచితం_కాకపోతే DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#అయితే_ముగింపు

/*
 * Macintosh has plenty of memory, use large buffers
 */
#నిర్వచించు CMDBUFFSIZE 1024	// size of the command processing buffer

#నిర్వచితం_కాకపోతే DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#అయితే_ముగింపు

#నిర్వచించు WILDCHAR_LIST "*?[{`$"

/**************/
#నిర్వచించు mch_rename(src, dst) rename(src, dst)
#నిర్వచించు mch_remove(x) unlink((అక్షరం *)(x))
#నిర్వచితం_కాకపోతే mch_getenv
# ఒకవేళ defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((అక్షరం *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# లేకపోతే
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define పైకి_BC_PC_EXTERN
# endif
#అయితే_ముగింపు

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#నిర్వచితం_కాకపోతే SIGPROTOARG
# define SIGPROTOARG	(పూర్ణాంకం)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SIGDEFARG
# define SIGDEFARG(s)	(s) పూర్ణాంకం s వాడని;
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SIGDUMMYARG
# define SIGDUMMYARG	0
#అయితే_ముగింపు
#నిర్వచనం_రద్దు  HAVE_AVAIL_MEM
#నిర్వచితం_కాకపోతే HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_సమయం 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#అయితే_ముగింపు

#ఒకవేళ !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#అయితే_ముగింపు

// A Mac constant causing big problem to syntax highlighting
#నిర్వచించు UNKNOWN_CREATOR '\?\?\?\?'

#నిర్వచితమైతే FEAT_RELTIME

# include <dispatch/dispatch.h>

# ఒకవేళ !defined(MAC_OS_X_సంస్కరణ_10_12) \
	|| (MAC_OS_X_సంస్కరణ_MAX_ALLOWED < MAC_OS_X_సంస్కరణ_10_12)
రకనిర్వచనం పూర్ణాంకం clockid_t;
# endif
# ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
# endif

నిర్మాణం itimerspec
{
    నిర్మాణం timespec it_interval;  // timer period
    నిర్మాణం timespec it_value;	  // initial expiration
};

నిర్మాణం sigevent;

నిర్మాణం macos_timer
{
    dispatch_వరుస_t tim_వరుస;
    dispatch_ఆధారం_t tim_timer;
    శూన్య (*tim_func)(సంఘం sigval);
    శూన్య *tim_arg;
};

రకనిర్వచనం నిర్మాణం macos_timer *timer_t;

బాహ్య పూర్ణాంకం timer_create(
    clockid_t clockid,
    నిర్మాణం sigevent *sevp,
    timer_t *timerid);

బాహ్య పూర్ణాంకం timer_delete(timer_t timerid);

బాహ్య పూర్ణాంకం timer_settime(
    timer_t timerid, పూర్ణాంకం flags,
    స్థిరరాశి నిర్మాణం itimerspec *new_విలువ_2,
    నిర్మాణం itimerspec *వాడని_2);

#అయితే_ముగింపు // FEAT_RELTIME

#అయితే_ముగింపు // OS_MAC__H
