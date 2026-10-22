/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

#که_نه_تعریف_شوی OS_MAC__H
#تعریف_کړه OS_MAC__H

// Before Including the MacOS specific files,
// let's set the OPAQUE_TOOLBOX_STRUCTS to 0 so we
// can access the internal structures.
// (Until fully Carbon compliant)
// TODO: Can we remove this? (Dany)
#إذا 0
# define OPAQUE_TOOLBOX_STRUCTS 0
#د_که_پای

// Include MAC_OS_X_VERSION_* macros
#که_تعریف_شوی HAVE_AVAILABILITYMACROS_H
# include <AvailabilityMacros.h>
#د_که_پای

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
#د_که_پای
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
#تعریف_کړه USE_CMD_کلی
// On MacOS X use the / not the :
// TODO: Should file such as ~/.vimrc reside instead in
//       ~/Library/Vim or ~/Library/Preferences/org.vim.vim/ ? (Dany)
// When compiled under MacOS X (including CARBON version)
// we use the Unix File path style.  Also when UNIX is defined.
#تعریف_کړه USE_UNIXFILENAME


/*
 * Generic Vim #define for Mac
 */

#تعریف_کړه FEAT_سرچینه_FFS
#تعریف_کړه FEAT_سرچینه_FF_MAC

#تعریف_کړه USE_EXE_NAME		    // to find  $VIM
#تعریف_کړه CASE_INSENSITIVE_FILENAME   // ignore case when comparing file names

#تعریف_کړه USE_FNAME_CASE		// make ":e os_Mac.c" open the file in its
				// original case, as "os_mac.c"
#تعریف_کړه دوګونی_FILE_IO
#تعریف_کړه EOL_DEFAULT EOL_MAC
#تعریف_کړه HAVE_AVAIL_MEM

#که_نه_تعریف_شوی HAVE_CONFIG_H
# define HAVE_STRING_H
# define HAVE_MEMSET
# define USE_TMPNAM		// use tmpnam() instead of mktemp()
# define HAVE_FCNTL_H
# define HAVE_QSORT
# define HAVE_ST_ډېرپېښ		// have stat.st_mode
# define HAVE_MATH_H

# إذا defined(__DATE__) && defined(__TIME__)
#  define HAVE_DATE_وخت
# endif
# define HAVE_STRFTIME
#د_که_پای

/*
 * Names for the EXRC, HELP and temporary files.
 * Some of these may have been defined in the makefile.
 */

#که_نه_تعریف_شوی SYS_VIMRC_FILE
# define SYS_VIMRC_FILE "$VIM/vimrc"
#د_که_پای
#که_نه_تعریف_شوی SYS_GVIMRC_FILE
# define SYS_GVIMRC_FILE "$VIM/gvimrc"
#د_که_پای
#که_نه_تعریف_شوی SYS_MENU_FILE
# define SYS_MENU_FILE	"$VIMRUNTIME/menu.vim"
#د_که_پای
#که_نه_تعریف_شوی SYS_OPTWIN_FILE
# define SYS_OPTWIN_FILE "$VIMRUNTIME/optwin.vim"
#د_که_پای
#که_نه_تعریف_شوی VIM_DEFAULTS_FILE
# define VIM_DEFAULTS_FILE "$VIMRUNTIME/defaults.vim"
#د_که_پای
#که_نه_تعریف_شوی EVIM_FILE
# define EVIM_FILE	"$VIMRUNTIME/evim.vim"
#د_که_پای

#که_تعریف_شوی FEAT_GUI
# ifndef USR_GVIMRC_FILE
#  define USR_GVIMRC_FILE "~/.gvimrc"
# endif
# ifndef GVIMRC_FILE
#  define GVIMRC_FILE	"_gvimrc"
# endif
#د_که_پای
#که_نه_تعریف_شوی USR_VIMRC_FILE
# define USR_VIMRC_FILE	"~/.vimrc"
#د_که_پای

#که_نه_تعریف_شوی USR_EXRC_FILE
# define USR_EXRC_FILE	"~/.exrc"
#د_که_پای

#که_نه_تعریف_شوی VIMRC_FILE
# define VIMRC_FILE	"_vimrc"
#د_که_پای

#که_نه_تعریف_شوی EXRC_FILE
# define EXRC_FILE	"_exrc"
#د_که_پای

#که_نه_تعریف_شوی DFLT_HELPFILE
# define DFLT_HELPFILE	"$VIMRUNTIME/doc/help.txt"
#د_که_پای

#که_نه_تعریف_شوی SYNTAX_FNAME
# define SYNTAX_FNAME	"$VIMRUNTIME/syntax/%s.vim"
#د_که_پای

#که_تعریف_شوی FEAT_VIMINFO
# ifndef VIMINFO_FILE
#  define VIMINFO_FILE	"~/.viminfo"
# endif
#د_که_پای // FEAT_VIMINFO

#که_نه_تعریف_شوی DFLT_BDIR
# define DFLT_BDIR	"."	// default for 'backupdir'
#د_که_پای

#که_نه_تعریف_شوی DFLT_DIR
# define DFLT_DIR	"."	// default for 'directory'
#د_که_پای

#که_نه_تعریف_شوی DFLT_VDIR
# define DFLT_VDIR	"$VIM/vimfiles/view"	// default for 'viewdir'
#د_که_پای

#تعریف_کړه DFLT_ERRORFILE		"errors.err"

#که_نه_تعریف_شوی DFLT_RUNTIMEPATH
# define DFLT_RUNTIMEPATH	"~/.vim,$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after,~/.vim/after"
#د_که_پای
#که_نه_تعریف_شوی CLEAN_RUNTIMEPATH
# define CLEAN_RUNTIMEPATH	"$VIM/vimfiles,$VIMRUNTIME,$VIM/vimfiles/after"
#د_که_پای

/*
 * Macintosh has plenty of memory, use large buffers
 */
#تعریف_کړه CMDBUFFSIZE 1024	// size of the command processing buffer

#که_نه_تعریف_شوی DFLT_MAXMEM
# define DFLT_MAXMEM	512	// use up to  512 Kbyte for buffer
#د_که_پای

#که_نه_تعریف_شوی DFLT_MAXMEMTOT
# define DFLT_MAXMEMTOT	2048	// use up to 2048 Kbyte for Vim
#د_که_پای

#تعریف_کړه WILDCHAR_LIST "*?[{`$"

/**************/
#تعریف_کړه mch_rename(src, dst) rename(src, dst)
#تعریف_کړه mch_remove(x) unlink((حرف *)(x))
#که_نه_تعریف_شوی mch_getenv
# إذا defined(__APPLE_CC__)
#  define mch_getenv(name)  ((char_u *)getenv((حرف *)(name)))
#  define mch_setenv(name, val, x) setenv(name, val, x)
# وإلا
  // vim_getenv() is in pty.c
#  define USE_VIMPTY_GETENV
#  define mch_getenv(x) vimpty_getenv(x)
#  define mch_setenv(name, val, x) setenv(name, val, x)
# endif
#د_که_پای

#که_نه_تعریف_شوی HAVE_CONFIG_H
# ifdef __APPLE_CC__
// Assuming compiling for MacOS X
// Trying to take advantage of the prebinding
#  define HAVE_TGETENT
#  define OSPEED_EXTERN
#  define پورته_BC_PC_EXTERN
# endif
#د_که_پای

// Some "prep work" definition to be able to compile the MacOS X
// version with os_unix.c instead of os_mac.c. Based on the result
// of ./configure for console MacOS X.

#که_نه_تعریف_شوی SIGPROTOARG
# define SIGPROTOARG	(صحيح)
#د_که_پای
#که_نه_تعریف_شوی SIGDEFARG
# define SIGDEFARG(s)	(s) صحيح s ناکارول_شوی;
#د_که_پای
#که_نه_تعریف_شوی SIGDUMMYARG
# define SIGDUMMYARG	0
#د_که_پای
#تعریف_لغوه  HAVE_AVAIL_MEM
#که_نه_تعریف_شوی HAVE_CONFIG_H
//# define USE_SYSTEM  // Output ship do debugger :(, but not compile
# define HAVE_SYS_WAIT_H 1 // Attempt
# define HAVE_TERMIOS_H 1
# define SYS_SELECT_WITH_SYS_وخت 1
# define HAVE_SELECT 1
# define HAVE_SYS_SELECT_H 1
# define HAVE_PUTENV
# define HAVE_SETENV
# define HAVE_RENAME
#د_که_پای

#إذا !defined(HAVE_CONFIG_H)
# define HAVE_PUTENV
#د_که_پای

// A Mac constant causing big problem to syntax highlighting
#تعریف_کړه UNKNOWN_CREATOR '\?\?\?\?'

#که_تعریف_شوی FEAT_RELTIME

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
    dispatch_کتار_t tim_کتار;
    dispatch_سرچینه_t tim_timer;
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
    ثابت بنية itimerspec *new_ارزښت_2,
    بنية itimerspec *ناکارول_شوی_2);

#د_که_پای // FEAT_RELTIME

#د_که_پای // OS_MAC__H
