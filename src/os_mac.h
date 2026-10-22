/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ OS_MAC__H
#ವ್ಯಾಖ್ಯಾನಿಸು OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#ಆದರೆ 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#ಆದರೆ_ಅಂತ್ಯ

// Include MAC_OS_X_VERSION_* macros
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Unix interface
 */
#ಆದರೆ defined(__APPLE_CC__) // for Project Builder and ...
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
#ಆದರೆ_ಅಂತ್ಯ
#ಅನ್ತರ್ಭಾವ <signal.h>
#ಅನ್ತರ್ಭಾವ <errno.h>
#ಅನ್ತರ್ಭಾವ <stdio.h>
#ಅನ್ತರ್ಭಾವ <stdlib.h>
#ಅನ್ತರ್ಭಾವ <time.h>
#ಅನ್ತರ್ಭಾವ <dirent.h>

/*
 * Mach interface
 */
#ಅನ್ತರ್ಭಾವ <mach/task.h>

/*
 * MacOS specific #define
 */

// This will go away when CMD_KEY fully tested
#ವ್ಯಾಖ್ಯಾನಿಸು USE_CMD_ಕೀಲಿ
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#ವ್ಯಾಖ್ಯಾನಿಸು USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#ವ್ಯಾಖ್ಯಾನಿಸು FEAT_ಮೂಲ_FFS
#ವ್ಯಾಖ್ಯಾನಿಸು FEAT_ಮೂಲ_FF_MAC

#ವ್ಯಾಖ್ಯಾನಿಸು USE_EXE_NAME		    // to find  $VIM
#ವ್ಯಾಖ್ಯಾನಿಸು CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#ವ್ಯಾಖ್ಯಾನಿಸು USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#ವ್ಯಾಖ್ಯಾನಿಸು ದ್ವಿಮಾನ_FILE_IO
#ವ್ಯಾಖ್ಯಾನಿಸು EOL_DEFAULT EOL_MAC
#ವ್ಯಾಖ್ಯಾನಿಸು HAVE_AVAIL_MEM

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_ಬಹುಲಕ		// have stat.st_mode
# define HAVE_MATH_H

# ಆದರೆ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_ಸಮಯ
# endif
# define HAVE_STRFTIME
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ EXRC_FILE
# define EXRC_FILE	"_exrc"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#ಆದರೆ_ಅಂತ್ಯ // FEAT_VIMINFO

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು DFLT_ERRORFILE		"errors.err"

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Macintosh has plenty of memory, use large buffers
 */
#ವ್ಯಾಖ್ಯಾನಿಸು CMDBUFFSIZE 1024	// size of the command processing buffer

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿಸು WILDCHAR_LIST "*?[{`$"

/**************/
#ವ್ಯಾಖ್ಯಾನಿಸು mch_rename(src, dst) rename(src, dst)
#ವ್ಯಾಖ್ಯಾನಿಸು mch_remove(x) unlink((ಅಕ್ಷರ *)(x))
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ mch_getenv
# ಆದರೆ defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((ಅಕ್ಷರ *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# ಇಲ್ಲದಿದ್ದರೆ
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define ಮೇಲೆ_BC_PC_EXTERN
# endif
#ಆದರೆ_ಅಂತ್ಯ

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIGPROTOARG
# define SIGPROTOARG	(ಪೂರ್ಣಾಂಕ)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIGDEFARG
# define SIGDEFARG(s)	(s) ಪೂರ್ಣಾಂಕ s ಬಳಸದ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIGDUMMYARG
# define SIGDUMMYARG	0
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು  HAVE_AVAIL_MEM
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_ಸಮಯ 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#ಆದರೆ_ಅಂತ್ಯ

// A Mac constant causing big problem to syntax highlighting
#ವ್ಯಾಖ್ಯಾನಿಸು UNKNOWN_CREATOR '\?\?\?\?'

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_RELTIME

# include <dispatch/dispatch.h>

# ಆದರೆ !defined(MAC_OS_X_ಆವೃತ್ತಿ_10_12) \
	|| (MAC_OS_X_ಆವೃತ್ತಿ_MAX_ALLOWED < MAC_OS_X_ಆವೃತ್ತಿ_10_12)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ clockid_t;
# endif
# ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
# endif

ರಚನೆ itimerspec
{
    ರಚನೆ timespec it_interval;  // timer period
    ರಚನೆ timespec it_value;	  // initial expiration
};

ರಚನೆ sigevent;

ರಚನೆ macos_timer
{
    dispatch_ಸರತಿ_t tim_ಸರತಿ;
    dispatch_ಮೂಲ_t tim_timer;
    ಶೂನ್ಯ (*tim_func)(ಒಕ್ಕೂಟ sigval);
    ಶೂನ್ಯ *tim_arg;
};

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ macos_timer *timer_t;

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ timer_create(
    clockid_t clockid,
    ರಚನೆ sigevent *sevp,
    timer_t *timerid);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ timer_delete(timer_t timerid);

ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ timer_settime(
    timer_t timerid, ಪೂರ್ಣಾಂಕ flags,
    ಸ್ಥಿರಾಂಕ ರಚನೆ itimerspec *new_ಮೌಲ್ಯ_2,
    ರಚನೆ itimerspec *ಬಳಸದ_2);

#ಆದರೆ_ಅಂತ್ಯ // FEAT_RELTIME

#ಆದರೆ_ಅಂತ್ಯ // OS_MAC__H
