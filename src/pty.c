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

#include "vim.h"

#ако defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#include <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ако HAVE_STROPTS_H
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
#  ако defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ако HAVE_TERMIO_H
# include <termio.h>
#иначе
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#ако HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#ако HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#ако !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#ако defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#ако defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#ако defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
char *ptsname(цяло);
цяло unlockpt(цяло);
цяло grantpt(цяло);
цяло posix_openpt(цяло flags);
#endif

    static void
initmaster(цяло f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# иначе
#  ifdef TIOCFLUSH
    (void)ioctl(f, TIOCFLUSH, (char *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (void)ioctl(f, TIOCEXCL, (char *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    цяло
setup_slavepty(цяло fd)
{
    ако (fd < 0)
	върни 0;
#ако defined(I_PUSH) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ако defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ако (ioctl(fd, I_PUSH, "ptem") != 0)
	върни -1;
# endif
    ако (ioctl(fd, I_PUSH, "ldterm") != 0)
	върни -1;
# ifdef SUN_SYSTEM
    ако (ioctl(fd, I_PUSH, "ttcompat") != 0)
	върни -1;
# endif
#endif
    върни 0;
}

#ако defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    цяло
mch_openpty(char **ttyn)
{
    цяло			f;
    char		*m;
    sighandler_T	sigcld;
    static char TtyName[32];  // used for opening a new pty-pair

    ако ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	върни -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ако ((m = ptsname(f)) == NULL || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	върни -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, sizeof(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    върни f;
}
#endif

#ако defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    цяло
mch_openpty(char **ttyn)
{
    цяло		f;
    static char TtyName[32];

    ако ((f = open_controlling_pty(TtyName)) < 0)
	върни -1;
    initmaster(f);
    *ttyn = TtyName;
    върни f;
}
#endif

#ако (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    цяло
mch_openpty(char **ttyn)
{
    char	*m, *s;
    цяло		f;
    // used for opening a new pty-pair:
    static char PtyName[32];
    static char TtyName[32];

    ако ((f = getpseudotty(&s, &m)) < 0)
	върни -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, sizeof(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, sizeof(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    върни f;
}
#endif

#ако defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    цяло
mch_openpty(char **ttyn)
{
    цяло		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    static char TtyName[32];

    ако ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	върни -1;
    ако (mch_fstat(f, &buf) < 0)
    {
	close(f);
	върни -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    върни f;
}
#endif

#ако defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    цяло
mch_openpty(char **ttyn)
{
    цяло			f;
    char		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    static char TtyName[32];

    ако ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	върни -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ако ((m = ptsname(f)) == NULL || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	върни -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, sizeof(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    върни f;
}
#endif

#ако defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
static цяло aixhack = -1;
# endif

    цяло
mch_openpty(char **ttyn)
{
    цяло		f;
    // used for opening a new pty-pair:
    static char TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ако ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	върни -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), sizeof(TtyName) - 1);
    ако (geteuid() != ROOT_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	върни -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ако (aixhack >= 0)
	close(aixhack);
    ако ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	върни -1;
    }
# endif
    *ttyn = TtyName;
    върни f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
static const char PtyProto[] = "/dev/ptym/ptyXY";
static const char TtyProto[] = "/dev/pty/ttyXY";
# иначе
#  ifdef __HAIKU__
static const char PtyProto[] = "/dev/pt/XY";
static const char TtyProto[] = "/dev/tt/XY";
#  иначе
static const char PtyProto[] = "/dev/ptyXY";
static const char TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    цяло
mch_openpty(char **ttyn)
{
    char	*p, *q, *l, *d;
    цяло		f;
    // used for opening a new pty-pair:
    static char PtyName[32];
    static char TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, sizeof(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, sizeof(TtyName) - 1);
    за (p = PtyName; *p != 'X'; p++)
	;
    за (q = TtyName; *q != 'X'; q++)
	;
    за (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	за (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ако ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		continue;
	    q[0] = *l;
	    q[1] = *d;
	    ако (geteuid() != ROOT_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		continue;
	    }
# ако defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		цяло pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ако (ioctl(f, TIOCGPGRP, (char *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    continue;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    върни f;
	}
    }
    върни -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    цяло
mch_isatty(цяло fd)
{
#ако defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    struct strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = NULL;
    istr.ic_len = 0;

    ако (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	върни 2;
#endif
    върни isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
