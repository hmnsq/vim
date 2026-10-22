/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_غیر_تعریف_شدہ OS_MAC__H
#تعریف_کرو OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#اگر 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#اگر_ختم

// Include MAC_OS_X_VERSION_* macros
#اگر_تعریف_شدہ HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#اگر_ختم

/*
 * Unix interface
 */
#اگر defined(__APPLE_CC__) // for Project Builder and ...
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
#اگر_ختم
#شامل <signal.h>
#شامل <errno.h>
#شامل <stdio.h>
#شامل <stdlib.h>
#شامل <time.h>
#شامل <dirent.h>

/*
 * Mach interface
 */
#شامل <mach/task.h>

/*
 * MacOS specific #define
 */

// This will go away when CMD_KEY fully tested
#تعریف_کرو USE_CMD_کلید
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#تعریف_کرو USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#تعریف_کرو FEAT_ماخذ_FFS
#تعریف_کرو FEAT_ماخذ_FF_MAC

#تعریف_کرو USE_EXE_NAME		    // to find  $VIM
#تعریف_کرو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#تعریف_کرو USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#تعریف_کرو ثنائی_FILE_IO
#تعریف_کرو EOL_DEFAULT EOL_MAC
#تعریف_کرو HAVE_AVAIL_MEM

#اگر_غیر_تعریف_شدہ HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_منوال		// have stat.st_mode
# define HAVE_MATH_H

# اگر defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_وقت
# endif
# define HAVE_STRFTIME
#اگر_ختم

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#اگر_غیر_تعریف_شدہ SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#اگر_ختم
#اگر_غیر_تعریف_شدہ SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#اگر_ختم
#اگر_غیر_تعریف_شدہ SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#اگر_ختم
#اگر_غیر_تعریف_شدہ VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#اگر_ختم
#اگر_غیر_تعریف_شدہ EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#اگر_ختم
#اگر_غیر_تعریف_شدہ USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ EXRC_FILE
# define EXRC_FILE	"_exrc"
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#اگر_ختم

#اگر_غیر_تعریف_شدہ SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#اگر_ختم

#اگر_تعریف_شدہ FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#اگر_ختم // FEAT_VIMINFO

#اگر_غیر_تعریف_شدہ DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#اگر_ختم

#تعریف_کرو DFLT_ERRORFILE		"errors.err"

#اگر_غیر_تعریف_شدہ DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#اگر_ختم
#اگر_غیر_تعریف_شدہ CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#اگر_ختم

/*
 * Macintosh has plenty of memory, use large buffers
 */
#تعریف_کرو CMDBUFFSIZE 1024	// size of the command processing buffer

#اگر_غیر_تعریف_شدہ DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#اگر_ختم

#اگر_غیر_تعریف_شدہ DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#اگر_ختم

#تعریف_کرو WILDCHAR_LIST "*?[{`$"

/**************/
#تعریف_کرو mch_rename(src, dst) rename(src, dst)
#تعریف_کرو mch_remove(x) unlink((حرف *)(x))
#اگر_غیر_تعریف_شدہ mch_getenv
# اگر defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((حرف *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# ورنہ
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#اگر_ختم

#اگر_غیر_تعریف_شدہ HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define اوپر_BC_PC_EXTERN
# endif
#اگر_ختم

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#اگر_غیر_تعریف_شدہ SIGPROTOARG
# define SIGPROTOARG	(صحیح)
#اگر_ختم
#اگر_غیر_تعریف_شدہ SIGDEFARG
# define SIGDEFARG(s)	(s) صحیح s غیر_مستعمل;
#اگر_ختم
#اگر_غیر_تعریف_شدہ SIGDUMMYARG
# define SIGDUMMYARG	0
#اگر_ختم
#تعریف_منسوخ  HAVE_AVAIL_MEM
#اگر_غیر_تعریف_شدہ HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_وقت 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#اگر_ختم

#اگر !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#اگر_ختم

// A Mac constant causing big problem to syntax highlighting
#تعریف_کرو UNKNOWN_CREATOR '\?\?\?\?'

#اگر_تعریف_شدہ FEAT_RELTIME

# include <dispatch/dispatch.h>

# اگر !defined(MAC_OS_X_نسخہ_10_12) \
	|| (MAC_OS_X_نسخہ_MAX_ALLOWED < MAC_OS_X_نسخہ_10_12)
قسمتعریف صحیح clockid_t;
# endif
# ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
# endif

ساخت itimerspec
{
    ساخت timespec it_interval;  // timer period
    ساخت timespec it_value;	  // initial expiration
};

ساخت sigevent;

ساخت macos_timer
{
    dispatch_قطار_t tim_قطار;
    dispatch_ماخذ_t tim_timer;
    خالی (*tim_func)(اتحاد sigval);
    خالی *tim_arg;
};

قسمتعریف ساخت macos_timer *timer_t;

خارجی صحیح timer_create(
    clockid_t clockid,
    ساخت sigevent *sevp,
    timer_t *timerid);

خارجی صحیح timer_delete(timer_t timerid);

خارجی صحیح timer_settime(
    timer_t timerid, صحیح flags,
    ثابت ساخت itimerspec *new_قدر_2,
    ساخت itimerspec *غیر_مستعمل_2);

#اگر_ختم // FEAT_RELTIME

#اگر_ختم // OS_MAC__H
