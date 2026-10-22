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

#అన్తర్భావ "vim.h"

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_పని_CHANNEL)

#అన్తర్భావ <signal.h>

#నిర్వచితమైతే HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_STROPTS_H
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
#  ఒకవేళ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_UNISTD_H
# include <unistd.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_TERMIO_H
# include <termio.h>
#లేకపోతే
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#అయితే_ముగింపు

#ఒకవేళ HAVE_SYS_ప్రవాహం_H
# include <sys/stream.h>
#అయితే_ముగింపు

#ఒకవేళ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#అయితే_ముగింపు

#ఒకవేళ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#అయితే_ముగింపు

#ఒకవేళ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
# include <sys/ttold.h>
#అయితే_ముగింపు

#నిర్వచితమైతే ISC
# include <sys/tty.h>
# include <sys/sioctl.h>
# include <sys/pty.h>
#అయితే_ముగింపు

#నిర్వచితమైతే sgi
# include <sys/sysmacros.h>
#అయితే_ముగింపు

#ఒకవేళ defined(INCLUDE_HPUX_ఆధారం) && !defined(hpux)
# define hpux
#అయితే_ముగింపు

/*
 * if no PTYRANGE[01] is in the config file, we pick a default
 */
#నిర్వచితం_కాకపోతే PTYRANGE0
# define PTYRANGE0 "qprs"
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే PTYRANGE1
# define PTYRANGE1 "0123456789abcdef"
#అయితే_ముగింపు

// SVR4 pseudo ttys don't seem to work with SCO-5
#నిర్వచితమైతే M_UNIX
# undef HAVE_SVR4_PTYS
#అయితే_ముగింపు

/*
 *  Open all ptys with O_NOCTTY, just to be on the safe side.
 */
#నిర్వచితం_కాకపోతే O_NOCTTY
# define O_NOCTTY 0
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
అక్షరం *ptsname(పూర్ణాంకం);
పూర్ణాంకం unlockpt(పూర్ణాంకం);
పూర్ణాంకం grantpt(పూర్ణాంకం);
పూర్ణాంకం posix_openpt(పూర్ణాంకం flags);
#అయితే_ముగింపు

    స్థిర శూన్య
initmaster(పూర్ణాంకం f వాడని)
{
#నిర్వచితం_కాకపోతే VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# లేకపోతే
#  ifdef TIOCFLUSH
    (శూన్య)ioctl(f, TIOCFLUSH, (అక్షరం *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (శూన్య)ioctl(f, TIOCEXCL, (అక్షరం *) 0);
# endif
#అయితే_ముగింపు
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    పూర్ణాంకం
అమరిక_slavepty(పూర్ణాంకం fd)
{
    ఒకవేళ (fd < 0)
	తిరిగి 0;
#ఒకవేళ defined(I_నెట్టు) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ఒకవేళ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ఒకవేళ (ioctl(fd, I_నెట్టు, "ptem") != 0)
	తిరిగి -1;
# endif
    ఒకవేళ (ioctl(fd, I_నెట్టు, "ldterm") != 0)
	తిరిగి -1;
# ifdef SUN_SYSTEM
    ఒకవేళ (ioctl(fd, I_నెట్టు, "ttcompat") != 0)
	తిరిగి -1;
# endif
#అయితే_ముగింపు
    తిరిగి 0;
}

#ఒకవేళ defined(HAVE_POSIX_OPENPT) && !defined(PTY_పూర్తి)
# define PTY_పూర్తి
    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    పూర్ణాంకం			f;
    అక్షరం		*m;
    sighandler_T	sigcld;
    స్థిర అక్షరం TtyName[32];  // used for opening a new pty-pair

    ఒకవేళ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	తిరిగి -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ఒకవేళ ((m = ptsname(f)) == ఖాళీ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	తిరిగి -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, పరిమాణం(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#ఒకవేళ defined(OSX) && !defined(PTY_పూర్తి)
# define PTY_పూర్తి
    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    పూర్ణాంకం		f;
    స్థిర అక్షరం TtyName[32];

    ఒకవేళ ((f = open_controlling_pty(TtyName)) < 0)
	తిరిగి -1;
    initmaster(f);
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#ఒకవేళ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_పూర్తి)
# define PTY_పూర్తి
    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    అక్షరం	*m, *s;
    పూర్ణాంకం		f;
    // used for opening a new pty-pair:
    స్థిర అక్షరం PtyName[32];
    స్థిర అక్షరం TtyName[32];

    ఒకవేళ ((f = getpseudotty(&s, &m)) < 0)
	తిరిగి -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, పరిమాణం(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, పరిమాణం(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#ఒకవేళ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_పూర్తి)
# define PTY_పూర్తి
    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    పూర్ణాంకం		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    స్థిర అక్షరం TtyName[32];

    ఒకవేళ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	తిరిగి -1;
    ఒకవేళ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	తిరిగి -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SVR4_PTYS) && !defined(PTY_పూర్తి) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_సంస్కరణ_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_పూర్తి
    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    పూర్ణాంకం			f;
    అక్షరం		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    స్థిర అక్షరం TtyName[32];

    ఒకవేళ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	తిరిగి -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ఒకవేళ ((m = ptsname(f)) == ఖాళీ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	తిరిగి -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, పరిమాణం(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#ఒకవేళ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_పూర్తి)
# define PTY_పూర్తి

# ifdef _IBMR2
స్థిర పూర్ణాంకం aixhack = -1;
# endif

    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    పూర్ణాంకం		f;
    // used for opening a new pty-pair:
    స్థిర అక్షరం TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ఒకవేళ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	తిరిగి -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), పరిమాణం(TtyName) - 1);
    ఒకవేళ (geteuid() != మూలం_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	తిరిగి -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ఒకవేళ (aixhack >= 0)
	close(aixhack);
    ఒకవేళ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	తిరిగి -1;
    }
# endif
    *ttyn = TtyName;
    తిరిగి f;
}
#అయితే_ముగింపు

#నిర్వచితం_కాకపోతే PTY_పూర్తి

# ifdef hpux
స్థిర స్థిరరాశి అక్షరం PtyProto[] = "/dev/ptym/ptyXY";
స్థిర స్థిరరాశి అక్షరం TtyProto[] = "/dev/pty/ttyXY";
# లేకపోతే
#  ifdef __HAIKU__
స్థిర స్థిరరాశి అక్షరం PtyProto[] = "/dev/pt/XY";
స్థిర స్థిరరాశి అక్షరం TtyProto[] = "/dev/tt/XY";
#  లేకపోతే
స్థిర స్థిరరాశి అక్షరం PtyProto[] = "/dev/ptyXY";
స్థిర స్థిరరాశి అక్షరం TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    పూర్ణాంకం
mch_openpty(అక్షరం **ttyn)
{
    అక్షరం	*p, *q, *l, *d;
    పూర్ణాంకం		f;
    // used for opening a new pty-pair:
    స్థిర అక్షరం PtyName[32];
    స్థిర అక్షరం TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, పరిమాణం(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, పరిమాణం(TtyName) - 1);
    కొరకు (p = PtyName; *p != 'X'; p++)
	;
    కొరకు (q = TtyName; *q != 'X'; q++)
	;
    కొరకు (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	కొరకు (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ఒకవేళ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		కొనసాగు;
	    q[0] = *l;
	    q[1] = *d;
	    ఒకవేళ (geteuid() != మూలం_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		కొనసాగు;
	    }
# ఒకవేళ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		పూర్ణాంకం pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ఒకవేళ (ioctl(f, TIOCGPGRP, (అక్షరం *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    కొనసాగు;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    తిరిగి f;
	}
    }
    తిరిగి -1;
}
#అయితే_ముగింపు

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    పూర్ణాంకం
mch_isatty(పూర్ణాంకం fd)
{
#ఒకవేళ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    నిర్మాణం strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ఖాళీ;
    istr.ic_len = 0;

    ఒకవేళ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	తిరిగి 2;
#అయితే_ముగింపు
    తిరిగి isatty(fd);
}

#అయితే_ముగింపు // FEAT_GUI || FEAT_JOB_CHANNEL
