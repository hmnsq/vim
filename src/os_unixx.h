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
#ఒకవేళ defined(HAVE_SYS_IOCTL_H) && !defined(SUN_SYSTEM)
# include <sys/ioctl.h>
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే USE_SYSTEM	// use fork/exec to start the shell

# ఒకవేళ defined(HAVE_SYS_WAIT_H)
#  include <sys/wait.h>
# endif

# ifndef WEXITSTATUS
#  define WEXITSTATUS(stat_val) (((stat_val) >> 8) & 0377)
# endif

# ifndef WIFEXITED
#  define WIFEXITED(stat_val) (((stat_val) & 255) == 0)
# endif

#అయితే_ముగింపు // !USE_SYSTEM

#నిర్వచితమైతే HAVE_STROPTS_H
# ifdef sinix
#  define buf_T __system_buf_t__
# endif
# include <stropts.h>
# ifdef sinix
#  undef buf_T
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_STRING_H
# include <string.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_ప్రవాహం_H
# include <sys/stream.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_UTSNAME_H
# include <sys/utsname.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_SYSTEMINFO_H
// <sys/systeminfo.h> uses SYS_NMLN but it may not be defined (CrayT3E).
# ifndef SYS_NMLN
#  define SYS_NMLN 32
# endif

# include <sys/systeminfo.h>	// for sysinfo
#అయితే_ముగింపు

/*
 * We use termios.h if both termios.h and termio.h are available.
 * Termios is supposed to be a superset of termio.h.  Don't include them both,
 * it may give problems on some systems (e.g. hpux).
 * I don't understand why we don't want termios.h for apollo.
 */
#ఒకవేళ defined(HAVE_TERMIOS_H) && !defined(apollo)
# include <termios.h>
#లేకపోతే
# ifdef HAVE_TERMIO_H
#  include <termio.h>
# లేకపోతే
#  ifdef HAVE_SGTTY_H
#   include <sgtty.h>
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SYS_PTEM_H
# include <sys/ptem.h>	// must be after termios.h for Sinix
# ifndef _IO_PTEM_H	// For UnixWare that should check for _IO_PT_PTEM_H
#  define _IO_PTEM_H
# endif
#అయితే_ముగింపు

// shared library access
#ఒకవేళ defined(HAVE_DLFCN_H) && defined(USE_DLOPEN)
# ఒకవేళ defined(__MVS__) && !defined (__SUSV3)
    // needed to define RTLD_LAZY (Anthony Giorgio)
#  define __SUSV3
# endif
# include <dlfcn.h>
#లేకపోతే
# ఒకవేళ defined(HAVE_DL_H) && defined(HAVE_SHL_ఎక్కించు)
#  include <dl.h>
# endif
#అయితే_ముగింపు
