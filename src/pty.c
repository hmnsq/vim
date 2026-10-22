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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#ಅನ್ತರ್ಭಾವ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ಯದಿ HAVE_STROPTS_H
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
#  ಯದಿ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ಯದಿ HAVE_TERMIO_H
# include <termio.h>
#ಅನ್ಯಥಾ
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#ಯದಿ HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#ಯದಿ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#ಯದಿ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#ಯದಿ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#ಯದಿ defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#ಯದಿ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
ವರ್ಣ *ptsname(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ unlockpt(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ grantpt(ಪೂರ್ಣಾಙ್ಕ);
ಪೂರ್ಣಾಙ್ಕ posix_openpt(ಪೂರ್ಣಾಙ್ಕ flags);
#endif

    ಸ್ಥಿರ ಶೂನ್ಯ
initmaster(ಪೂರ್ಣಾಙ್ಕ f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# ಅನ್ಯಥಾ
#  ifdef TIOCFLUSH
    (ಶೂನ್ಯ)ioctl(f, TIOCFLUSH, (ವರ್ಣ *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (ಶೂನ್ಯ)ioctl(f, TIOCEXCL, (ವರ್ಣ *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    ಪೂರ್ಣಾಙ್ಕ
setup_slavepty(ಪೂರ್ಣಾಙ್ಕ fd)
{
    ಯದಿ (fd < 0)
	ಪ್ರತ್ಯಾಗಮ 0;
#ಯದಿ defined(I_ತಳ್ಳು) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ಯದಿ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ಯದಿ (ioctl(fd, I_ತಳ್ಳು, "ptem") != 0)
	ಪ್ರತ್ಯಾಗಮ -1;
# endif
    ಯದಿ (ioctl(fd, I_ತಳ್ಳು, "ldterm") != 0)
	ಪ್ರತ್ಯಾಗಮ -1;
# ifdef SUN_SYSTEM
    ಯದಿ (ioctl(fd, I_ತಳ್ಳು, "ttcompat") != 0)
	ಪ್ರತ್ಯಾಗಮ -1;
# endif
#endif
    ಪ್ರತ್ಯಾಗಮ 0;
}

#ಯದಿ defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ಪೂರ್ಣಾಙ್ಕ			f;
    ವರ್ಣ		*m;
    sighandler_T	sigcld;
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];  // used for opening a new pty-pair

    ಯದಿ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ಪ್ರತ್ಯಾಗಮ -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ಯದಿ ((m = ptsname(f)) == ರಿಕ್ತ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ಪರಿಮಾಣ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ಯದಿ defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ಪೂರ್ಣಾಙ್ಕ		f;
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    ಯದಿ ((f = open_controlling_pty(TtyName)) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    initmaster(f);
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ಯದಿ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ವರ್ಣ	*m, *s;
    ಪೂರ್ಣಾಙ್ಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ವರ್ಣ PtyName[32];
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    ಯದಿ ((f = getpseudotty(&s, &m)) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, ಪರಿಮಾಣ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, ಪರಿಮಾಣ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ಯದಿ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ಪೂರ್ಣಾಙ್ಕ		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    ಯದಿ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ಯದಿ defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ಪೂರ್ಣಾಙ್ಕ			f;
    ವರ್ಣ		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    ಯದಿ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ಪ್ರತ್ಯಾಗಮ -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ಯದಿ ((m = ptsname(f)) == ರಿಕ್ತ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ಪರಿಮಾಣ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ಯದಿ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ aixhack = -1;
# endif

    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ಪೂರ್ಣಾಙ್ಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ಯದಿ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ಪ್ರತ್ಯಾಗಮ -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), ಪರಿಮಾಣ(TtyName) - 1);
    ಯದಿ (geteuid() != ಬೇರು_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ಯದಿ (aixhack >= 0)
	close(aixhack);
    ಯದಿ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ಪ್ರತ್ಯಾಗಮ -1;
    }
# endif
    *ttyn = TtyName;
    ಪ್ರತ್ಯಾಗಮ f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ PtyProto[] = "/dev/ptym/ptyXY";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ TtyProto[] = "/dev/pty/ttyXY";
# ಅನ್ಯಥಾ
#  ifdef __HAIKU__
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ PtyProto[] = "/dev/pt/XY";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ TtyProto[] = "/dev/tt/XY";
#  ಅನ್ಯಥಾ
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ PtyProto[] = "/dev/ptyXY";
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    ಪೂರ್ಣಾಙ್ಕ
mch_openpty(ವರ್ಣ **ttyn)
{
    ವರ್ಣ	*p, *q, *l, *d;
    ಪೂರ್ಣಾಙ್ಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ವರ್ಣ PtyName[32];
    ಸ್ಥಿರ ವರ್ಣ TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, ಪರಿಮಾಣ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, ಪರಿಮಾಣ(TtyName) - 1);
    ಪ್ರತಿ (p = PtyName; *p != 'X'; p++)
	;
    ಪ್ರತಿ (q = TtyName; *q != 'X'; q++)
	;
    ಪ್ರತಿ (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	ಪ್ರತಿ (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ಯದಿ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ಅನುವರ್ತ;
	    q[0] = *l;
	    q[1] = *d;
	    ಯದಿ (geteuid() != ಬೇರು_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ಅನುವರ್ತ;
	    }
# ಯದಿ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		ಪೂರ್ಣಾಙ್ಕ pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ಯದಿ (ioctl(f, TIOCGPGRP, (ವರ್ಣ *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ಅನುವರ್ತ;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ಪ್ರತ್ಯಾಗಮ f;
	}
    }
    ಪ್ರತ್ಯಾಗಮ -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isatty(ಪೂರ್ಣಾಙ್ಕ fd)
{
#ಯದಿ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ಸಂರಚನಾ strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ರಿಕ್ತ;
    istr.ic_len = 0;

    ಯದಿ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ಪ್ರತ್ಯಾಗಮ 2;
#endif
    ಪ್ರತ್ಯಾಗಮ isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
