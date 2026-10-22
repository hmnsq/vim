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

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_ಕೆಲಸ_CHANNEL)

#ಅನ್ತರ್ಭಾವ <signal.h>

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_STROPTS_H
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
#  ಆದರೆ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_UNISTD_H
# include <unistd.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_TERMIO_H
# include <termio.h>
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_SYS_ಹರಿವು_H
# include <sys/stream.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
# include <sys/ttold.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ISC
# include <sys/tty.h>
# include <sys/sioctl.h>
# include <sys/pty.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ sgi
# include <sys/sysmacros.h>
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(INCLUDE_HPUX_ಮೂಲ) && !defined(hpux)
# define hpux
#ಆದರೆ_ಅಂತ್ಯ

/*
 * if no PTYRANGE[01] is in the config file, we pick a default
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PTYRANGE0
# define PTYRANGE0 "qprs"
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PTYRANGE1
# define PTYRANGE1 "0123456789abcdef"
#ಆದರೆ_ಅಂತ್ಯ

// SVR4 pseudo ttys don't seem to work with SCO-5
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ M_UNIX
# undef HAVE_SVR4_PTYS
#ಆದರೆ_ಅಂತ್ಯ

/*
 *  Open all ptys with O_NOCTTY, just to be on the safe side.
 */
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ O_NOCTTY
# define O_NOCTTY 0
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
ಅಕ್ಷರ *ptsname(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ unlockpt(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ grantpt(ಪೂರ್ಣಾಂಕ);
ಪೂರ್ಣಾಂಕ posix_openpt(ಪೂರ್ಣಾಂಕ flags);
#ಆದರೆ_ಅಂತ್ಯ

    ಸ್ಥಿರ ಶೂನ್ಯ
initmaster(ಪೂರ್ಣಾಂಕ f ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef TIOCFLUSH
    (ಶೂನ್ಯ)ioctl(f, TIOCFLUSH, (ಅಕ್ಷರ *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (ಶೂನ್ಯ)ioctl(f, TIOCEXCL, (ಅಕ್ಷರ *) 0);
# endif
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    ಪೂರ್ಣಾಂಕ
ಸಿದ್ಧತೆ_slavepty(ಪೂರ್ಣಾಂಕ fd)
{
    ಆದರೆ (fd < 0)
	ಹಿಂತಿರುಗಿ 0;
#ಆದರೆ defined(I_ತಳ್ಳು) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ಆದರೆ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ಆದರೆ (ioctl(fd, I_ತಳ್ಳು, "ptem") != 0)
	ಹಿಂತಿರುಗಿ -1;
# endif
    ಆದರೆ (ioctl(fd, I_ತಳ್ಳು, "ldterm") != 0)
	ಹಿಂತಿರುಗಿ -1;
# ifdef SUN_SYSTEM
    ಆದರೆ (ioctl(fd, I_ತಳ್ಳು, "ttcompat") != 0)
	ಹಿಂತಿರುಗಿ -1;
# endif
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ 0;
}

#ಆದರೆ defined(HAVE_POSIX_OPENPT) && !defined(PTY_ಮುಗಿದ)
# define PTY_ಮುಗಿದ
    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಪೂರ್ಣಾಂಕ			f;
    ಅಕ್ಷರ		*m;
    sighandler_T	sigcld;
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];  // used for opening a new pty-pair

    ಆದರೆ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ಹಿಂತಿರುಗಿ -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ಆದರೆ ((m = ptsname(f)) == ಖಾಲಿ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ಹಿಂತಿರುಗಿ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ಗಾತ್ರ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(OSX) && !defined(PTY_ಮುಗಿದ)
# define PTY_ಮುಗಿದ
    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಪೂರ್ಣಾಂಕ		f;
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    ಆದರೆ ((f = open_controlling_pty(TtyName)) < 0)
	ಹಿಂತಿರುಗಿ -1;
    initmaster(f);
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_ಮುಗಿದ)
# define PTY_ಮುಗಿದ
    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಅಕ್ಷರ	*m, *s;
    ಪೂರ್ಣಾಂಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ಅಕ್ಷರ PtyName[32];
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    ಆದರೆ ((f = getpseudotty(&s, &m)) < 0)
	ಹಿಂತಿರುಗಿ -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, ಗಾತ್ರ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, ಗಾತ್ರ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_ಮುಗಿದ)
# define PTY_ಮುಗಿದ
    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಪೂರ್ಣಾಂಕ		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    ಆದರೆ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ಹಿಂತಿರುಗಿ -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SVR4_PTYS) && !defined(PTY_ಮುಗಿದ) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_ಆವೃತ್ತಿ_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_ಮುಗಿದ
    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಪೂರ್ಣಾಂಕ			f;
    ಅಕ್ಷರ		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    ಆದರೆ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ಹಿಂತಿರುಗಿ -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ಆದರೆ ((m = ptsname(f)) == ಖಾಲಿ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ಹಿಂತಿರುಗಿ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ಗಾತ್ರ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_ಮುಗಿದ)
# define PTY_ಮುಗಿದ

# ifdef _IBMR2
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ aixhack = -1;
# endif

    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಪೂರ್ಣಾಂಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ಆದರೆ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ಹಿಂತಿರುಗಿ -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), ಗಾತ್ರ(TtyName) - 1);
    ಆದರೆ (geteuid() != ಬೇರು_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ಹಿಂತಿರುಗಿ -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ಆದರೆ (aixhack >= 0)
	close(aixhack);
    ಆದರೆ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ಹಿಂತಿರುಗಿ -1;
    }
# endif
    *ttyn = TtyName;
    ಹಿಂತಿರುಗಿ f;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ PTY_ಮುಗಿದ

# ifdef hpux
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ PtyProto[] = "/dev/ptym/ptyXY";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ TtyProto[] = "/dev/pty/ttyXY";
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef __HAIKU__
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ PtyProto[] = "/dev/pt/XY";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ TtyProto[] = "/dev/tt/XY";
#  ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ PtyProto[] = "/dev/ptyXY";
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    ಪೂರ್ಣಾಂಕ
mch_openpty(ಅಕ್ಷರ **ttyn)
{
    ಅಕ್ಷರ	*p, *q, *l, *d;
    ಪೂರ್ಣಾಂಕ		f;
    // used for opening a new pty-pair:
    ಸ್ಥಿರ ಅಕ್ಷರ PtyName[32];
    ಸ್ಥಿರ ಅಕ್ಷರ TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, ಗಾತ್ರ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, ಗಾತ್ರ(TtyName) - 1);
    ಗಾಗಿ (p = PtyName; *p != 'X'; p++)
	;
    ಗಾಗಿ (q = TtyName; *q != 'X'; q++)
	;
    ಗಾಗಿ (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	ಗಾಗಿ (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ಆದರೆ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ಮುಂದುವರಿ;
	    q[0] = *l;
	    q[1] = *d;
	    ಆದರೆ (geteuid() != ಬೇರು_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ಮುಂದುವರಿ;
	    }
# ಆದರೆ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		ಪೂರ್ಣಾಂಕ pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ಆದರೆ (ioctl(f, TIOCGPGRP, (ಅಕ್ಷರ *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ಮುಂದುವರಿ;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ಹಿಂತಿರುಗಿ f;
	}
    }
    ಹಿಂತಿರುಗಿ -1;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    ಪೂರ್ಣಾಂಕ
mch_isatty(ಪೂರ್ಣಾಂಕ fd)
{
#ಆದರೆ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ರಚನೆ strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ಖಾಲಿ;
    istr.ic_len = 0;

    ಆದರೆ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ಹಿಂತಿರುಗಿ 2;
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ isatty(fd);
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_GUI || FEAT_JOB_CHANNEL
