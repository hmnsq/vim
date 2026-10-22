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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ defined(FEAT_GUI) || defined(FEAT_ਕੰਮ_CHANNEL)

#ਅਨ੍ਤਰ੍ਭਾਵ <signal.h>

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_STROPTS_H
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
#  ਜੇ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_UNISTD_H
# include <unistd.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_TERMIO_H
# include <termio.h>
#ਨਹੀਂਤਾਂ
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_SYS_ਧਾਰਾ_H
# include <sys/stream.h>
#ਜੇ_ਅੰਤ

#ਜੇ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#ਜੇ_ਅੰਤ

#ਜੇ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
# include <sys/ttold.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ISC
# include <sys/tty.h>
# include <sys/sioctl.h>
# include <sys/pty.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ sgi
# include <sys/sysmacros.h>
#ਜੇ_ਅੰਤ

#ਜੇ defined(INCLUDE_HPUX_ਸਰੋਤ) && !defined(hpux)
# define hpux
#ਜੇ_ਅੰਤ

/*
 * if no PTYRANGE[01] is in the config file, we pick a default
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PTYRANGE0
# define PTYRANGE0 "qprs"
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PTYRANGE1
# define PTYRANGE1 "0123456789abcdef"
#ਜੇ_ਅੰਤ

// SVR4 pseudo ttys don't seem to work with SCO-5
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ M_UNIX
# undef HAVE_SVR4_PTYS
#ਜੇ_ਅੰਤ

/*
 *  Open all ptys with O_NOCTTY, just to be on the safe side.
 */
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ O_NOCTTY
# define O_NOCTTY 0
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
ਅੱਖਰ *ptsname(ਪੂਰਨ);
ਪੂਰਨ unlockpt(ਪੂਰਨ);
ਪੂਰਨ grantpt(ਪੂਰਨ);
ਪੂਰਨ posix_openpt(ਪੂਰਨ flags);
#ਜੇ_ਅੰਤ

    ਸਥਿਰ ਖਾਲੀ
initmaster(ਪੂਰਨ f ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# ਨਹੀਂਤਾਂ
#  ifdef TIOCFLUSH
    (ਖਾਲੀ)ioctl(f, TIOCFLUSH, (ਅੱਖਰ *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (ਖਾਲੀ)ioctl(f, TIOCEXCL, (ਅੱਖਰ *) 0);
# endif
#ਜੇ_ਅੰਤ
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    ਪੂਰਨ
ਤਿਆਰੀ_slavepty(ਪੂਰਨ fd)
{
    ਜੇ (fd < 0)
	ਵਾਪਸ 0;
#ਜੇ defined(I_ਧੱਕੋ) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ਜੇ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ਜੇ (ioctl(fd, I_ਧੱਕੋ, "ptem") != 0)
	ਵਾਪਸ -1;
# endif
    ਜੇ (ioctl(fd, I_ਧੱਕੋ, "ldterm") != 0)
	ਵਾਪਸ -1;
# ifdef SUN_SYSTEM
    ਜੇ (ioctl(fd, I_ਧੱਕੋ, "ttcompat") != 0)
	ਵਾਪਸ -1;
# endif
#ਜੇ_ਅੰਤ
    ਵਾਪਸ 0;
}

#ਜੇ defined(HAVE_POSIX_OPENPT) && !defined(PTY_ਪੂਰਾ)
# define PTY_ਪੂਰਾ
    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਪੂਰਨ			f;
    ਅੱਖਰ		*m;
    sighandler_T	sigcld;
    ਸਥਿਰ ਅੱਖਰ TtyName[32];  // used for opening a new pty-pair

    ਜੇ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ਵਾਪਸ -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ਜੇ ((m = ptsname(f)) == ਨਿਰਾ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ਵਾਪਸ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ਆਕਾਰ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(OSX) && !defined(PTY_ਪੂਰਾ)
# define PTY_ਪੂਰਾ
    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਪੂਰਨ		f;
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    ਜੇ ((f = open_controlling_pty(TtyName)) < 0)
	ਵਾਪਸ -1;
    initmaster(f);
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_ਪੂਰਾ)
# define PTY_ਪੂਰਾ
    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਅੱਖਰ	*m, *s;
    ਪੂਰਨ		f;
    // used for opening a new pty-pair:
    ਸਥਿਰ ਅੱਖਰ PtyName[32];
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    ਜੇ ((f = getpseudotty(&s, &m)) < 0)
	ਵਾਪਸ -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, ਆਕਾਰ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, ਆਕਾਰ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_ਪੂਰਾ)
# define PTY_ਪੂਰਾ
    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਪੂਰਨ		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    ਜੇ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ਵਾਪਸ -1;
    ਜੇ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ਵਾਪਸ -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SVR4_PTYS) && !defined(PTY_ਪੂਰਾ) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_ਸੰਸਕਰਣ_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_ਪੂਰਾ
    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਪੂਰਨ			f;
    ਅੱਖਰ		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    ਜੇ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ਵਾਪਸ -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ਜੇ ((m = ptsname(f)) == ਨਿਰਾ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ਵਾਪਸ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ਆਕਾਰ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_ਪੂਰਾ)
# define PTY_ਪੂਰਾ

# ifdef _IBMR2
ਸਥਿਰ ਪੂਰਨ aixhack = -1;
# endif

    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਪੂਰਨ		f;
    // used for opening a new pty-pair:
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ਜੇ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ਵਾਪਸ -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), ਆਕਾਰ(TtyName) - 1);
    ਜੇ (geteuid() != ਜੜ੍ਹ_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ਵਾਪਸ -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ਜੇ (aixhack >= 0)
	close(aixhack);
    ਜੇ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ਵਾਪਸ -1;
    }
# endif
    *ttyn = TtyName;
    ਵਾਪਸ f;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ PTY_ਪੂਰਾ

# ifdef hpux
ਸਥਿਰ ਅਚਲ ਅੱਖਰ PtyProto[] = "/dev/ptym/ptyXY";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ TtyProto[] = "/dev/pty/ttyXY";
# ਨਹੀਂਤਾਂ
#  ifdef __HAIKU__
ਸਥਿਰ ਅਚਲ ਅੱਖਰ PtyProto[] = "/dev/pt/XY";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ TtyProto[] = "/dev/tt/XY";
#  ਨਹੀਂਤਾਂ
ਸਥਿਰ ਅਚਲ ਅੱਖਰ PtyProto[] = "/dev/ptyXY";
ਸਥਿਰ ਅਚਲ ਅੱਖਰ TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    ਪੂਰਨ
mch_openpty(ਅੱਖਰ **ttyn)
{
    ਅੱਖਰ	*p, *q, *l, *d;
    ਪੂਰਨ		f;
    // used for opening a new pty-pair:
    ਸਥਿਰ ਅੱਖਰ PtyName[32];
    ਸਥਿਰ ਅੱਖਰ TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, ਆਕਾਰ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, ਆਕਾਰ(TtyName) - 1);
    ਲਈ (p = PtyName; *p != 'X'; p++)
	;
    ਲਈ (q = TtyName; *q != 'X'; q++)
	;
    ਲਈ (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	ਲਈ (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ਜੇ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ਜਾਰੀ;
	    q[0] = *l;
	    q[1] = *d;
	    ਜੇ (geteuid() != ਜੜ੍ਹ_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ਜਾਰੀ;
	    }
# ਜੇ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		ਪੂਰਨ pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ਜੇ (ioctl(f, TIOCGPGRP, (ਅੱਖਰ *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ਜਾਰੀ;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ਵਾਪਸ f;
	}
    }
    ਵਾਪਸ -1;
}
#ਜੇ_ਅੰਤ

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    ਪੂਰਨ
mch_isatty(ਪੂਰਨ fd)
{
#ਜੇ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ਬਣਤਰ strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ਨਿਰਾ;
    istr.ic_len = 0;

    ਜੇ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ਵਾਪਸ 2;
#ਜੇ_ਅੰਤ
    ਵਾਪਸ isatty(fd);
}

#ਜੇ_ਅੰਤ // FEAT_GUI || FEAT_JOB_CHANNEL
