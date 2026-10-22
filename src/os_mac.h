/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#جيڪڏهن_وصف_نه_ٿيل OS_MAC__H
#وصف_ڪريو OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#إذا 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#جيڪڏهن_پورو

// Include MAC_OS_X_VERSION_* macros
#جيڪڏهن_وصف_ٿيل HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#جيڪڏهن_پورو

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
#جيڪڏهن_پورو
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
#وصف_ڪريو USE_CMD_ڪنجي
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#وصف_ڪريو USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#وصف_ڪريو FEAT_ذريعو_FFS
#وصف_ڪريو FEAT_ذريعو_FF_MAC

#وصف_ڪريو USE_EXE_NAME		    // to find  $VIM
#وصف_ڪريو CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#وصف_ڪريو USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#وصف_ڪريو ٻٽو_FILE_IO
#وصف_ڪريو EOL_DEFAULT EOL_MAC
#وصف_ڪريو HAVE_AVAIL_MEM

#جيڪڏهن_وصف_نه_ٿيل HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_گهڻائي		// have stat.st_mode
# define HAVE_MATH_H

# إذا defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_وقت
# endif
# define HAVE_STRFTIME
#جيڪڏهن_پورو

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#جيڪڏهن_وصف_نه_ٿيل SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل EXRC_FILE
# define EXRC_FILE	"_exrc"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#جيڪڏهن_پورو // FEAT_VIMINFO

#جيڪڏهن_وصف_نه_ٿيل DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#جيڪڏهن_پورو

#وصف_ڪريو DFLT_ERRORFILE		"errors.err"

#جيڪڏهن_وصف_نه_ٿيل DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#جيڪڏهن_پورو

/*
 * Macintosh has plenty of memory, use large buffers
 */
#وصف_ڪريو CMDBUFFSIZE 1024	// size of the command processing buffer

#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#جيڪڏهن_پورو

#وصف_ڪريو WILDCHAR_LIST "*?[{`$"

/**************/
#وصف_ڪريو mch_rename(src, dst) rename(src, dst)
#وصف_ڪريو mch_remove(x) unlink((حرف *)(x))
#جيڪڏهن_وصف_نه_ٿيل mch_getenv
# إذا defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((حرف *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# وإلا
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_نه_ٿيل HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define مٿي_BC_PC_EXTERN
# endif
#جيڪڏهن_پورو

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#جيڪڏهن_وصف_نه_ٿيل SIGPROTOARG
# define SIGPROTOARG	(صحيح)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SIGDEFARG
# define SIGDEFARG(s)	(s) صحيح s اڻ_استعمال_ٿيل;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SIGDUMMYARG
# define SIGDUMMYARG	0
#جيڪڏهن_پورو
#وصف_رد  HAVE_AVAIL_MEM
#جيڪڏهن_وصف_نه_ٿيل HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_وقت 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#جيڪڏهن_پورو

#إذا !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#جيڪڏهن_پورو

// A Mac constant causing big problem to syntax highlighting
#وصف_ڪريو UNKNOWN_CREATOR '\?\?\?\?'

#جيڪڏهن_وصف_ٿيل FEAT_RELTIME

# include <dispatch/dispatch.h>

# إذا !defined(MAC_OS_X_نسخو_10_12) \
	|| (MAC_OS_X_نسخو_MAX_ALLOWED < MAC_OS_X_نسخو_10_12)
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
    dispatch_قطار_t tim_قطار;
    dispatch_ذريعو_t tim_timer;
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
    ثابت بنية itimerspec *new_قدر_2,
    بنية itimerspec *اڻ_استعمال_ٿيل_2);

#جيڪڏهن_پورو // FEAT_RELTIME

#جيڪڏهن_پورو // OS_MAC__H
