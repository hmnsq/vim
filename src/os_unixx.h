/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 */

/*
 * os_unixx.h -- include files that are only used in os_unix.c
 */

// Sun's sys/ioctl.h redefines symbols from termio world
#إذا defined(HAVE_SYS_IOCTL_H) && !defined(SUN_SYSTEM)
# include <sys/ioctl.h>
#جے_انت

#جے_بنا_تعریف USE_SYSTEM	// use fork/exec to start the shell

# إذا defined(HAVE_SYS_WAIT_H)
#  include <sys/wait.h>
# endif

# ifndef WEXITSTATUS
#  define WEXITSTATUS(stat_val) (((stat_val) >> 8) & 0377)
# endif

# ifndef WIFEXITED
#  define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
# endif

#جے_انت // !USE_SYSTEM

#جے_تعریف_شدہ HAVE_STROPTS_H
# ifdef sinix
#  define buf_T __system_buf_t__
# endif
# include <stropts.h>
# ifdef sinix
#  undef buf_T
# endif
#جے_انت

#جے_تعریف_شدہ HAVE_STRING_H
# include <string.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_دھارا_H
# include <sys/stream.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_SYSTEMINFO_H
// <sys/systeminfo.h> uses SYS_NMLN but it may not be defined (CrayT3E).
# ifndef SYS_NMLN
#  define SYS_NMLN 32
# endif

# include <sys/systeminfo.h>	// for sysinfo
#جے_انت

/*
 * We use termios.h if both termios.h and termio.h are available.
 * Termios is supposed to be a superset of termio.h.  Don't include them both,
 * it may give problems on some systems (e.g. hpux).
 * I don't understand why we don't want termios.h for apollo.
 */
#إذا defined(HAVE_TERMIOS_H) && !defined(apollo)
# include <termios.h>
#وإلا
# ifdef HAVE_TERMIO_H
#  include <termio.h>
# وإلا
#  ifdef HAVE_SGTTY_H
#   include <sgtty.h>
#  endif
# endif
#جے_انت

#جے_تعریف_شدہ HAVE_SYS_PTEM_H
# include <sys/ptem.h>	// must be after termios.h for Sinix
# ifndef _IO_PTEM_H	// For UnixWare that should check for _IO_PT_PTEM_H
#  define _IO_PTEM_H
# endif
#جے_انت

// shared library access
#إذا defined(HAVE_DLFCN_H) && defined(USE_DLOPEN)
# إذا defined(__MVS__) && !defined (__SUSV3)
    // needed to define RTLD_LAZY (Anthony Giorgio)
#  define __SUSV3
# endif
# include <dlfcn.h>
#وإلا
# إذا defined(HAVE_DL_H) && defined(HAVE_SHL_لدو)
#  include <dl.h>
# endif
#جے_انت
