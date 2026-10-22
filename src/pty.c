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

#অন্তর্ভাৱ "vim.h"

#যদি defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#অন্তর্ভাৱ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#যদি HAVE_STROPTS_H
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
#  যদি defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#যদি HAVE_TERMIO_H
# include <termio.h>
#অন্যথা
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#যদি HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#যদি HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#যদি !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#যদি defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#যদি defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#যদি defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
ৱর্ণ *ptsname(পূর্ণাঙ্ক);
পূর্ণাঙ্ক unlockpt(পূর্ণাঙ্ক);
পূর্ণাঙ্ক grantpt(পূর্ণাঙ্ক);
পূর্ণাঙ্ক posix_openpt(পূর্ণাঙ্ক flags);
#endif

    স্থির শূন্য
initmaster(পূর্ণাঙ্ক f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# অন্যথা
#  ifdef TIOCFLUSH
    (শূন্য)ioctl(f, TIOCFLUSH, (ৱর্ণ *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (শূন্য)ioctl(f, TIOCEXCL, (ৱর্ণ *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    পূর্ণাঙ্ক
setup_slavepty(পূর্ণাঙ্ক fd)
{
    যদি (fd < 0)
	ঘূৰাই 0;
#যদি defined(I_ঠেলো) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# যদি defined(HAVE_SYS_PTEM_H) || defined(hpux)
    যদি (ioctl(fd, I_ঠেলো, "ptem") != 0)
	ঘূৰাই -1;
# endif
    যদি (ioctl(fd, I_ঠেলো, "ldterm") != 0)
	ঘূৰাই -1;
# ifdef SUN_SYSTEM
    যদি (ioctl(fd, I_ঠেলো, "ttcompat") != 0)
	ঘূৰাই -1;
# endif
#endif
    ঘূৰাই 0;
}

#যদি defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    পূর্ণাঙ্ক			f;
    ৱর্ণ		*m;
    sighandler_T	sigcld;
    স্থির ৱর্ণ TtyName[32];  // used for opening a new pty-pair

    যদি ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ঘূৰাই -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    যদি ((m = ptsname(f)) == রিক্ত || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ঘূৰাই -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, পরিমাণ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#যদি defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    পূর্ণাঙ্ক		f;
    স্থির ৱর্ণ TtyName[32];

    যদি ((f = open_controlling_pty(TtyName)) < 0)
	ঘূৰাই -1;
    initmaster(f);
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#যদি (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    ৱর্ণ	*m, *s;
    পূর্ণাঙ্ক		f;
    // used for opening a new pty-pair:
    স্থির ৱর্ণ PtyName[32];
    স্থির ৱর্ণ TtyName[32];

    যদি ((f = getpseudotty(&s, &m)) < 0)
	ঘূৰাই -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, পরিমাণ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, পরিমাণ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#যদি defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    পূর্ণাঙ্ক		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    স্থির ৱর্ণ TtyName[32];

    যদি ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ঘূৰাই -1;
    যদি (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ঘূৰাই -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#যদি defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    পূর্ণাঙ্ক			f;
    ৱর্ণ		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    স্থির ৱর্ণ TtyName[32];

    যদি ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ঘূৰাই -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    যদি ((m = ptsname(f)) == রিক্ত || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ঘূৰাই -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, পরিমাণ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#যদি defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
স্থির পূর্ণাঙ্ক aixhack = -1;
# endif

    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    পূর্ণাঙ্ক		f;
    // used for opening a new pty-pair:
    স্থির ৱর্ণ TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    যদি ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ঘূৰাই -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), পরিমাণ(TtyName) - 1);
    যদি (geteuid() != মূল_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ঘূৰাই -1;
    }
    initmaster(f);
# ifdef _IBMR2
    যদি (aixhack >= 0)
	close(aixhack);
    যদি ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ঘূৰাই -1;
    }
# endif
    *ttyn = TtyName;
    ঘূৰাই f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
স্থির ধ্রুৱ ৱর্ণ PtyProto[] = "/dev/ptym/ptyXY";
স্থির ধ্রুৱ ৱর্ণ TtyProto[] = "/dev/pty/ttyXY";
# অন্যথা
#  ifdef __HAIKU__
স্থির ধ্রুৱ ৱর্ণ PtyProto[] = "/dev/pt/XY";
স্থির ধ্রুৱ ৱর্ণ TtyProto[] = "/dev/tt/XY";
#  অন্যথা
স্থির ধ্রুৱ ৱর্ণ PtyProto[] = "/dev/ptyXY";
স্থির ধ্রুৱ ৱর্ণ TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    পূর্ণাঙ্ক
mch_openpty(ৱর্ণ **ttyn)
{
    ৱর্ণ	*p, *q, *l, *d;
    পূর্ণাঙ্ক		f;
    // used for opening a new pty-pair:
    স্থির ৱর্ণ PtyName[32];
    স্থির ৱর্ণ TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, পরিমাণ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, পরিমাণ(TtyName) - 1);
    বাবে (p = PtyName; *p != 'X'; p++)
	;
    বাবে (q = TtyName; *q != 'X'; q++)
	;
    বাবে (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	বাবে (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    যদি ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		অনুৱর্ত;
	    q[0] = *l;
	    q[1] = *d;
	    যদি (geteuid() != মূল_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		অনুৱর্ত;
	    }
# যদি defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		পূর্ণাঙ্ক pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		যদি (ioctl(f, TIOCGPGRP, (ৱর্ণ *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    অনুৱর্ত;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ঘূৰাই f;
	}
    }
    ঘূৰাই -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    পূর্ণাঙ্ক
mch_isatty(পূর্ণাঙ্ক fd)
{
#যদি defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    সংরচনা strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = রিক্ত;
    istr.ic_len = 0;

    যদি (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ঘূৰাই 2;
#endif
    ঘূৰাই isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
