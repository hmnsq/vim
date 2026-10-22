/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved		by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */
/*
 * The stuff in this file mostly comes from the "screen" program.
 * Included with permission from Juergen Weigert.
 * Copied from "pty.c".  "putenv.c" was used for putenv() in misc2.c.
 *
 * It has been modified to work better with Vim.
 * The parts that are not used in Vim have been deleted.
 * See the "screen" sources for the complete stuff.
 *
 * This specific version is distributed under the Vim license (attribution by
 * Juergen Weigert), the GPL applies to the original version, see the
 * copyright notice below.
 */

// Copyright (c) 1993
//	Juergen Weigert (jnweiger@immd4.informatik.uni-erlangen.de)
//	Michael Schroeder (mlschroe@immd4.informatik.uni-erlangen.de)
// Copyright (c) 1987 Oliver Laumann
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program (see the file COPYING); if not, write to the
// Free Software Foundation, Inc.,
// 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA

#അന്തര്ഭാവ "vim.h"

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#അന്തര്ഭാവ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#എങ്കിൽ HAVE_STROPTS_H
# include <sys/types.h>
# ifdef sinix
#  define buf_T __system_buf_t__
# endif
# include <stropts.h>
# ifdef sinix
#  undef buf_T
# endif
# ifdef SUN_SYSTEM
#  include <sys/conf.h>
#  എങ്കിൽ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#എങ്കിൽ HAVE_TERMIO_H
# include <termio.h>
#അല്ലെങ്കിൽ
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#എങ്കിൽ HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#എങ്കിൽ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#എങ്കിൽ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#എങ്കിൽ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
# include <sys/ttold.h>
#endif

#ifdef ISC
# include <sys/tty.h>
# include <sys/sioctl.h>
# include <sys/pty.h>
#endif

#ifdef sgi
# include <sys/sysmacros.h>
#endif

#എങ്കിൽ defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
# define hpux
#endif

/*
 * if no PTYRANGE[01] is in the config file, we pick a default
 */
#ifndef PTYRANGE0
# define PTYRANGE0 "qprs"
#endif
#ifndef PTYRANGE1
# define PTYRANGE1 "0123456789abcdef"
#endif

// SVR4 pseudo ttys don't seem to work with SCO-5
#ifdef M_UNIX
# undef HAVE_SVR4_PTYS
#endif

/*
 *  Open all ptys with O_NOCTTY, just to be on the safe side.
 */
#ifndef O_NOCTTY
# define O_NOCTTY 0
#endif

#എങ്കിൽ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
അക്ഷരം *ptsname(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ unlockpt(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ grantpt(പൂർണ്ണസംഖ്യ);
പൂർണ്ണസംഖ്യ posix_openpt(പൂർണ്ണസംഖ്യ flags);
#endif

    സ്ഥിതം ശൂന്യം
initmaster(പൂർണ്ണസംഖ്യ f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# അല്ലെങ്കിൽ
#  ifdef TIOCFLUSH
    (ശൂന്യം)ioctl(f, TIOCFLUSH, (അക്ഷരം *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (ശൂന്യം)ioctl(f, TIOCEXCL, (അക്ഷരം *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    പൂർണ്ണസംഖ്യ
setup_slavepty(പൂർണ്ണസംഖ്യ fd)
{
    എങ്കിൽ (fd < 0)
	തിരികെ 0;
#എങ്കിൽ defined(I_തള്ളുക) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# എങ്കിൽ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    എങ്കിൽ (ioctl(fd, I_തള്ളുക, "ptem") != 0)
	തിരികെ -1;
# endif
    എങ്കിൽ (ioctl(fd, I_തള്ളുക, "ldterm") != 0)
	തിരികെ -1;
# ifdef SUN_SYSTEM
    എങ്കിൽ (ioctl(fd, I_തള്ളുക, "ttcompat") != 0)
	തിരികെ -1;
# endif
#endif
    തിരികെ 0;
}

#എങ്കിൽ defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    പൂർണ്ണസംഖ്യ			f;
    അക്ഷരം		*m;
    sighandler_T	sigcld;
    സ്ഥിതം അക്ഷരം TtyName[32];  // used for opening a new pty-pair

    എങ്കിൽ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	തിരികെ -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    എങ്കിൽ ((m = ptsname(f)) == ശൂന്യ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	തിരികെ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, വലുപ്പം(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#എങ്കിൽ defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    പൂർണ്ണസംഖ്യ		f;
    സ്ഥിതം അക്ഷരം TtyName[32];

    എങ്കിൽ ((f = open_controlling_pty(TtyName)) < 0)
	തിരികെ -1;
    initmaster(f);
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#എങ്കിൽ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    അക്ഷരം	*m, *s;
    പൂർണ്ണസംഖ്യ		f;
    // used for opening a new pty-pair:
    സ്ഥിതം അക്ഷരം PtyName[32];
    സ്ഥിതം അക്ഷരം TtyName[32];

    എങ്കിൽ ((f = getpseudotty(&s, &m)) < 0)
	തിരികെ -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, വലുപ്പം(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, വലുപ്പം(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#എങ്കിൽ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    പൂർണ്ണസംഖ്യ		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    സ്ഥിതം അക്ഷരം TtyName[32];

    എങ്കിൽ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	തിരികെ -1;
    എങ്കിൽ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	തിരികെ -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#എങ്കിൽ defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    പൂർണ്ണസംഖ്യ			f;
    അക്ഷരം		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    സ്ഥിതം അക്ഷരം TtyName[32];

    എങ്കിൽ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	തിരികെ -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    എങ്കിൽ ((m = ptsname(f)) == ശൂന്യ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	തിരികെ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, വലുപ്പം(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#എങ്കിൽ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
സ്ഥിതം പൂർണ്ണസംഖ്യ aixhack = -1;
# endif

    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    പൂർണ്ണസംഖ്യ		f;
    // used for opening a new pty-pair:
    സ്ഥിതം അക്ഷരം TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    എങ്കിൽ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	തിരികെ -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), വലുപ്പം(TtyName) - 1);
    എങ്കിൽ (geteuid() != വേര്_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	തിരികെ -1;
    }
    initmaster(f);
# ifdef _IBMR2
    എങ്കിൽ (aixhack >= 0)
	close(aixhack);
    എങ്കിൽ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	തിരികെ -1;
    }
# endif
    *ttyn = TtyName;
    തിരികെ f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
സ്ഥിതം സ്ഥിരം അക്ഷരം PtyProto[] = "/dev/ptym/ptyXY";
സ്ഥിതം സ്ഥിരം അക്ഷരം TtyProto[] = "/dev/pty/ttyXY";
# അല്ലെങ്കിൽ
#  ifdef __HAIKU__
സ്ഥിതം സ്ഥിരം അക്ഷരം PtyProto[] = "/dev/pt/XY";
സ്ഥിതം സ്ഥിരം അക്ഷരം TtyProto[] = "/dev/tt/XY";
#  അല്ലെങ്കിൽ
സ്ഥിതം സ്ഥിരം അക്ഷരം PtyProto[] = "/dev/ptyXY";
സ്ഥിതം സ്ഥിരം അക്ഷരം TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    പൂർണ്ണസംഖ്യ
mch_openpty(അക്ഷരം **ttyn)
{
    അക്ഷരം	*p, *q, *l, *d;
    പൂർണ്ണസംഖ്യ		f;
    // used for opening a new pty-pair:
    സ്ഥിതം അക്ഷരം PtyName[32];
    സ്ഥിതം അക്ഷരം TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, വലുപ്പം(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, വലുപ്പം(TtyName) - 1);
    വേണ്ടി (p = PtyName; *p != 'X'; p++)
	;
    വേണ്ടി (q = TtyName; *q != 'X'; q++)
	;
    വേണ്ടി (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	വേണ്ടി (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    എങ്കിൽ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		തുടരുക;
	    q[0] = *l;
	    q[1] = *d;
	    എങ്കിൽ (geteuid() != വേര്_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		തുടരുക;
	    }
# എങ്കിൽ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		പൂർണ്ണസംഖ്യ pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		എങ്കിൽ (ioctl(f, TIOCGPGRP, (അക്ഷരം *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    തുടരുക;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    തിരികെ f;
	}
    }
    തിരികെ -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    പൂർണ്ണസംഖ്യ
mch_isatty(പൂർണ്ണസംഖ്യ fd)
{
#എങ്കിൽ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ഘടന strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ശൂന്യ;
    istr.ic_len = 0;

    എങ്കിൽ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	തിരികെ 2;
#endif
    തിരികെ isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
