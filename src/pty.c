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

#ضمن "vim.h"

#إذا defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#ضمن <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#إذا HAVE_STROPTS_H
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
#  إذا defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#إذا HAVE_TERMIO_H
# include <termio.h>
#وإلا
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#إذا HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#إذا HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#إذا !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#إذا defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#إذا defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#إذا defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
حرف *ptsname(صحيح);
صحيح unlockpt(صحيح);
صحيح grantpt(صحيح);
صحيح posix_openpt(صحيح flags);
#endif

    ساكن فراغ
initmaster(صحيح f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# وإلا
#  ifdef TIOCFLUSH
    (فراغ)ioctl(f, TIOCFLUSH, (حرف *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (فراغ)ioctl(f, TIOCEXCL, (حرف *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    صحيح
setup_slavepty(صحيح fd)
{
    إذا (fd < 0)
	أرجع 0;
#إذا defined(I_ڈالو) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# إذا defined(HAVE_SYS_PTEM_H) || defined(hpux)
    إذا (ioctl(fd, I_ڈالو, "ptem") != 0)
	أرجع -1;
# endif
    إذا (ioctl(fd, I_ڈالو, "ldterm") != 0)
	أرجع -1;
# ifdef SUN_SYSTEM
    إذا (ioctl(fd, I_ڈالو, "ttcompat") != 0)
	أرجع -1;
# endif
#endif
    أرجع 0;
}

#إذا defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    صحيح
mch_openpty(حرف **ttyn)
{
    صحيح			f;
    حرف		*m;
    sighandler_T	sigcld;
    ساكن حرف TtyName[32];  // used for opening a new pty-pair

    إذا ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	أرجع -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    إذا ((m = ptsname(f)) == عدم || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	أرجع -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, حجم(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    أرجع f;
}
#endif

#إذا defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    صحيح
mch_openpty(حرف **ttyn)
{
    صحيح		f;
    ساكن حرف TtyName[32];

    إذا ((f = open_controlling_pty(TtyName)) < 0)
	أرجع -1;
    initmaster(f);
    *ttyn = TtyName;
    أرجع f;
}
#endif

#إذا (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    صحيح
mch_openpty(حرف **ttyn)
{
    حرف	*m, *s;
    صحيح		f;
    // used for opening a new pty-pair:
    ساكن حرف PtyName[32];
    ساكن حرف TtyName[32];

    إذا ((f = getpseudotty(&s, &m)) < 0)
	أرجع -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, حجم(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, حجم(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    أرجع f;
}
#endif

#إذا defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    صحيح
mch_openpty(حرف **ttyn)
{
    صحيح		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ساكن حرف TtyName[32];

    إذا ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	أرجع -1;
    إذا (mch_fstat(f, &buf) < 0)
    {
	close(f);
	أرجع -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    أرجع f;
}
#endif

#إذا defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    صحيح
mch_openpty(حرف **ttyn)
{
    صحيح			f;
    حرف		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ساكن حرف TtyName[32];

    إذا ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	أرجع -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    إذا ((m = ptsname(f)) == عدم || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	أرجع -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, حجم(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    أرجع f;
}
#endif

#إذا defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
ساكن صحيح aixhack = -1;
# endif

    صحيح
mch_openpty(حرف **ttyn)
{
    صحيح		f;
    // used for opening a new pty-pair:
    ساكن حرف TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    إذا ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	أرجع -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), حجم(TtyName) - 1);
    إذا (geteuid() != جڑ_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	أرجع -1;
    }
    initmaster(f);
# ifdef _IBMR2
    إذا (aixhack >= 0)
	close(aixhack);
    إذا ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	أرجع -1;
    }
# endif
    *ttyn = TtyName;
    أرجع f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
ساكن ثابت حرف PtyProto[] = "/dev/ptym/ptyXY";
ساكن ثابت حرف TtyProto[] = "/dev/pty/ttyXY";
# وإلا
#  ifdef __HAIKU__
ساكن ثابت حرف PtyProto[] = "/dev/pt/XY";
ساكن ثابت حرف TtyProto[] = "/dev/tt/XY";
#  وإلا
ساكن ثابت حرف PtyProto[] = "/dev/ptyXY";
ساكن ثابت حرف TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    صحيح
mch_openpty(حرف **ttyn)
{
    حرف	*p, *q, *l, *d;
    صحيح		f;
    // used for opening a new pty-pair:
    ساكن حرف PtyName[32];
    ساكن حرف TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, حجم(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, حجم(TtyName) - 1);
    لكل (p = PtyName; *p != 'X'; p++)
	;
    لكل (q = TtyName; *q != 'X'; q++)
	;
    لكل (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	لكل (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    إذا ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		استمر;
	    q[0] = *l;
	    q[1] = *d;
	    إذا (geteuid() != جڑ_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		استمر;
	    }
# إذا defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		صحيح pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		إذا (ioctl(f, TIOCGPGRP, (حرف *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    استمر;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    أرجع f;
	}
    }
    أرجع -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    صحيح
mch_isatty(صحيح fd)
{
#إذا defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    بنية strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = عدم;
    istr.ic_len = 0;

    إذا (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	أرجع 2;
#endif
    أرجع isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
