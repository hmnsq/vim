/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#اگر_تعریف_نشده OS_MAC__H
#تعریف_کن OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#إذا 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#پایان_اگر

// Include MAC_OS_X_VERSION_* macros
#اگر_تعریف_شده HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#پایان_اگر

/*
 * Unix interface
 */
#إذا defined(__APPLE_CC__) // for Project Builder and ...
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
#پایان_اگر
#ضمن <signal.h>
#ضمن <errno.h>
#ضمن <stdio.h>
#ضمن <stdlib.h>
#ضمن <time.h>
#ضمن <dirent.h>

/*
 * Mach interface
 */
#ضمن <mach/task.h>

/*
 * MacOS specific #define
 */

// This will go away when CMD_KEY fully tested
#تعریف_کن USE_CMD_کلید
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#تعریف_کن USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#تعریف_کن FEAT_منبع_FFS
#تعریف_کن FEAT_منبع_FF_MAC

#تعریف_کن USE_EXE_NAME		    // to find  $VIM
#تعریف_کن CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#تعریف_کن USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#تعریف_کن دودویی_FILE_IO
#تعریف_کن EOL_DEFAULT EOL_MAC
#تعریف_کن HAVE_AVAIL_MEM

#اگر_تعریف_نشده HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_نما		// have stat.st_mode
# define HAVE_MATH_H

# إذا defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_زمان
# endif
# define HAVE_STRFTIME
#پایان_اگر

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#اگر_تعریف_نشده SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#پایان_اگر
#اگر_تعریف_نشده SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#پایان_اگر
#اگر_تعریف_نشده SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#پایان_اگر
#اگر_تعریف_نشده SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#پایان_اگر
#اگر_تعریف_نشده VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#پایان_اگر
#اگر_تعریف_نشده EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#پایان_اگر

#اگر_تعریف_شده FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#پایان_اگر
#اگر_تعریف_نشده USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#پایان_اگر

#اگر_تعریف_نشده USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#پایان_اگر

#اگر_تعریف_نشده VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#پایان_اگر

#اگر_تعریف_نشده EXRC_FILE
# define EXRC_FILE	"_exrc"
#پایان_اگر

#اگر_تعریف_نشده DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#پایان_اگر

#اگر_تعریف_نشده SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#پایان_اگر

#اگر_تعریف_شده FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#پایان_اگر // FEAT_VIMINFO

#اگر_تعریف_نشده DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#پایان_اگر

#اگر_تعریف_نشده DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#پایان_اگر

#اگر_تعریف_نشده DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#پایان_اگر

#تعریف_کن DFLT_ERRORFILE		"errors.err"

#اگر_تعریف_نشده DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#پایان_اگر
#اگر_تعریف_نشده CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#پایان_اگر

/*
 * Macintosh has plenty of memory, use large buffers
 */
#تعریف_کن CMDBUFFSIZE 1024	// size of the command processing buffer

#اگر_تعریف_نشده DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#پایان_اگر

#اگر_تعریف_نشده DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#پایان_اگر

#تعریف_کن WILDCHAR_LIST "*?[{`$"

/**************/
#تعریف_کن mch_rename(src, dst) rename(src, dst)
#تعریف_کن mch_remove(x) unlink((حرف *)(x))
#اگر_تعریف_نشده mch_getenv
# إذا defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((حرف *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# وإلا
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#پایان_اگر

#اگر_تعریف_نشده HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define به_بالا_BC_PC_EXTERN
# endif
#پایان_اگر

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#اگر_تعریف_نشده SIGPROTOARG
# define SIGPROTOARG	(صحيح)
#پایان_اگر
#اگر_تعریف_نشده SIGDEFARG
# define SIGDEFARG(s)	(s) صحيح s استفادهنشده;
#پایان_اگر
#اگر_تعریف_نشده SIGDUMMYARG
# define SIGDUMMYARG	0
#پایان_اگر
#لغو_تعریف  HAVE_AVAIL_MEM
#اگر_تعریف_نشده HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_زمان 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#پایان_اگر

#إذا !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#پایان_اگر

// A Mac constant causing big problem to syntax highlighting
#تعریف_کن UNKNOWN_CREATOR '\?\?\?\?'

#اگر_تعریف_شده FEAT_RELTIME

# include <dispatch/dispatch.h>

# إذا !defined(MAC_OS_X_نسخه_10_12) \
	|| (MAC_OS_X_نسخه_MAX_ALLOWED < MAC_OS_X_نسخه_10_12)
تعريفنوع صحيح clockid_t;
# endif
# ifndef CLOCK_REALTIME
#  define CLOCK_REALTIME 0
# endif
# ifndef CLOCK_MONOTONIC
#  define CLOCK_MONOTONIC 1
# endif

بنية itimerspec
{
    بنية timespec it_interval;  // timer period
    بنية timespec it_value;	  // initial expiration
};

بنية sigevent;

بنية macos_timer
{
    dispatch_صف_t tim_صف;
    dispatch_منبع_t tim_timer;
    فراغ (*tim_func)(اتحاد sigval);
    فراغ *tim_arg;
};

تعريفنوع بنية macos_timer *timer_t;

خارجي صحيح timer_create(
    clockid_t clockid,
    بنية sigevent *sevp,
    timer_t *timerid);

خارجي صحيح timer_delete(timer_t timerid);

خارجي صحيح timer_settime(
    timer_t timerid, صحيح flags,
    ثابت بنية itimerspec *new_مقدار_2,
    بنية itimerspec *استفادهنشده_2);

#پایان_اگر // FEAT_RELTIME

#پایان_اگر // OS_MAC__H
