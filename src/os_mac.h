/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ OS_MAC__H
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#ਜੇ 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#ਜੇ_ਅੰਤ

// Include MAC_OS_X_VERSION_* macros
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#ਜੇ_ਅੰਤ

/*
 * Unix interface
 */
#ਜੇ defined(__APPLE_CC__) // for Project Builder and ...
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
#ਜੇ_ਅੰਤ
#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <errno.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdio.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <stdlib.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <time.h>
#ਅਨ੍ਤਰ੍ਭਾਵ <dirent.h>

/*
 * Mach interface
 */
#ਅਨ੍ਤਰ੍ਭਾਵ <mach/task.h>

/*
 * MacOS specific #define
 */

// This will go away when CMD_KEY fully tested
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_CMD_ਕੁੰਜੀ
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEAT_ਸਰੋਤ_FFS
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FEAT_ਸਰੋਤ_FF_MAC

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_EXE_NAME		    // to find  $VIM
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ ਦੁਆਧਾਰੀ_FILE_IO
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ EOL_DEFAULT EOL_MAC
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ HAVE_AVAIL_MEM

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_ਬਹੁਲਕ		// have stat.st_mode
# define HAVE_MATH_H

# ਜੇ defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_ਸਮਾਂ
# endif
# define HAVE_STRFTIME
#ਜੇ_ਅੰਤ

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ EXRC_FILE
# define EXRC_FILE	"_exrc"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#ਜੇ_ਅੰਤ // FEAT_VIMINFO

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ DFLT_ERRORFILE		"errors.err"

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#ਜੇ_ਅੰਤ

/*
 * Macintosh has plenty of memory, use large buffers
 */
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ CMDBUFFSIZE 1024	// size of the command processing buffer

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ WILDCHAR_LIST "*?[{`$"

/**************/
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_rename(src, dst) rename(src, dst)
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ mch_remove(x) unlink((ਅੱਖਰ *)(x))
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ mch_getenv
# ਜੇ defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((ਅੱਖਰ *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# ਨਹੀਂਤਾਂ
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define ਉੱਪਰ_BC_PC_EXTERN
# endif
#ਜੇ_ਅੰਤ

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIGPROTOARG
# define SIGPROTOARG	(ਪੂਰਨ)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIGDEFARG
# define SIGDEFARG(s)	(s) ਪੂਰਨ s ਅਣਵਰਤਿਆ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIGDUMMYARG
# define SIGDUMMYARG	0
#ਜੇ_ਅੰਤ
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ  HAVE_AVAIL_MEM
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_ਸਮਾਂ 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#ਜੇ_ਅੰਤ

#ਜੇ !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#ਜੇ_ਅੰਤ

// A Mac constant causing big problem to syntax highlighting
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ UNKNOWN_CREATOR '\?\?\?\?'

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_RELTIME

# include <dispatch/dispatch.h>

# ਜੇ !defined(MAC_OS_X_ਸੰਸਕਰਣ_10_12) \
	|| (MAC_OS_X_ਸੰਸਕਰਣ_MAX_ALLOWED < MAC_OS_X_ਸੰਸਕਰਣ_10_12)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ clockid_t;
# endif
# ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
# endif

ਬਣਤਰ itimerspec
{
    ਬਣਤਰ timespec it_interval;  // timer period
    ਬਣਤਰ timespec it_value;	  // initial expiration
};

ਬਣਤਰ sigevent;

ਬਣਤਰ macos_timer
{
    dispatch_ਕਤਾਰ_t tim_ਕਤਾਰ;
    dispatch_ਸਰੋਤ_t tim_timer;
    ਖਾਲੀ (*tim_func)(ਸੰਘ sigval);
    ਖਾਲੀ *tim_arg;
};

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ macos_timer *timer_t;

ਬਾਹਰੀ ਪੂਰਨ timer_create(
    clockid_t clockid,
    ਬਣਤਰ sigevent *sevp,
    timer_t *timerid);

ਬਾਹਰੀ ਪੂਰਨ timer_delete(timer_t timerid);

ਬਾਹਰੀ ਪੂਰਨ timer_settime(
    timer_t timerid, ਪੂਰਨ flags,
    ਅਚਲ ਬਣਤਰ itimerspec *new_ਮੁੱਲ_2,
    ਬਣਤਰ itimerspec *ਅਣਵਰਤਿਆ_2);

#ਜੇ_ਅੰਤ // FEAT_RELTIME

#ਜੇ_ਅੰਤ // OS_MAC__H
