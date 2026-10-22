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

#གལ་སྲིད defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#include <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#གལ་སྲིད HAVE_STROPTS_H
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
#  གལ་སྲིད defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#གལ་སྲིད HAVE_TERMIO_H
# include <termio.h>
#དེ་མིན
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#གལ་སྲིད HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#གལ་སྲིད HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#གལ་སྲིད !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#གལ་སྲིད defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#གལ་སྲིད defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#གལ་སྲིད defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
var_na *ptsname(ཧྲིལ);
ཧྲིལ unlockpt(ཧྲིལ);
ཧྲིལ grantpt(ཧྲིལ);
ཧྲིལ posix_openpt(ཧྲིལ flags);
#endif

    static void
initmaster(ཧྲིལ f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# དེ་མིན
#  ifdef TIOCFLUSH
    (void)ioctl(f, TIOCFLUSH, (var_na *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (void)ioctl(f, TIOCEXCL, (var_na *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    ཧྲིལ
setup_slavepty(ཧྲིལ fd)
{
    གལ་སྲིད (fd < 0)
	ལོག 0;
#གལ་སྲིད defined(I_PUSH) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# གལ་སྲིད defined(HAVE_SYS_PTEM_H) || defined(hpux)
    གལ་སྲིད (ioctl(fd, I_PUSH, "ptem") != 0)
	ལོག -1;
# endif
    གལ་སྲིད (ioctl(fd, I_PUSH, "ldterm") != 0)
	ལོག -1;
# ifdef SUN_SYSTEM
    གལ་སྲིད (ioctl(fd, I_PUSH, "ttcompat") != 0)
	ལོག -1;
# endif
#endif
    ལོག 0;
}

#གལ་སྲིད defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    ཧྲིལ			f;
    var_na		*m;
    sighandler_T	sigcld;
    static var_na TtyName[32];  // used for opening a new pty-pair

    གལ་སྲིད ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ལོག -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    གལ་སྲིད ((m = ptsname(f)) == NULL || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ལོག -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, parimaa_na(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ལོག f;
}
#endif

#གལ་སྲིད defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    ཧྲིལ		f;
    static var_na TtyName[32];

    གལ་སྲིད ((f = open_controlling_pty(TtyName)) < 0)
	ལོག -1;
    initmaster(f);
    *ttyn = TtyName;
    ལོག f;
}
#endif

#གལ་སྲིད (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    var_na	*m, *s;
    ཧྲིལ		f;
    // used for opening a new pty-pair:
    static var_na PtyName[32];
    static var_na TtyName[32];

    གལ་སྲིད ((f = getpseudotty(&s, &m)) < 0)
	ལོག -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, parimaa_na(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, parimaa_na(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ལོག f;
}
#endif

#གལ་སྲིད defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    ཧྲིལ		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    static var_na TtyName[32];

    གལ་སྲིད ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ལོག -1;
    གལ་སྲིད (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ལོག -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ལོག f;
}
#endif

#གལ་སྲིད defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    ཧྲིལ			f;
    var_na		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    static var_na TtyName[32];

    གལ་སྲིད ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ལོག -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    གལ་སྲིད ((m = ptsname(f)) == NULL || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ལོག -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, parimaa_na(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ལོག f;
}
#endif

#གལ་སྲིད defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
static ཧྲིལ aixhack = -1;
# endif

    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    ཧྲིལ		f;
    // used for opening a new pty-pair:
    static var_na TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    གལ་སྲིད ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ལོག -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), parimaa_na(TtyName) - 1);
    གལ་སྲིད (geteuid() != ROOT_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ལོག -1;
    }
    initmaster(f);
# ifdef _IBMR2
    གལ་སྲིད (aixhack >= 0)
	close(aixhack);
    གལ་སྲིད ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ལོག -1;
    }
# endif
    *ttyn = TtyName;
    ལོག f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
static const var_na PtyProto[] = "/dev/ptym/ptyXY";
static const var_na TtyProto[] = "/dev/pty/ttyXY";
# དེ་མིན
#  ifdef __HAIKU__
static const var_na PtyProto[] = "/dev/pt/XY";
static const var_na TtyProto[] = "/dev/tt/XY";
#  དེ་མིན
static const var_na PtyProto[] = "/dev/ptyXY";
static const var_na TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    ཧྲིལ
mch_openpty(var_na **ttyn)
{
    var_na	*p, *q, *l, *d;
    ཧྲིལ		f;
    // used for opening a new pty-pair:
    static var_na PtyName[32];
    static var_na TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, parimaa_na(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, parimaa_na(TtyName) - 1);
    དོན་ལུ (p = PtyName; *p != 'X'; p++)
	;
    དོན་ལུ (q = TtyName; *q != 'X'; q++)
	;
    དོན་ལུ (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	དོན་ལུ (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    གལ་སྲིད ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		continue;
	    q[0] = *l;
	    q[1] = *d;
	    གལ་སྲིད (geteuid() != ROOT_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		continue;
	    }
# གལ་སྲིད defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		ཧྲིལ pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		གལ་སྲིད (ioctl(f, TIOCGPGRP, (var_na *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    continue;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ལོག f;
	}
    }
    ལོག -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    ཧྲིལ
mch_isatty(ཧྲིལ fd)
{
#གལ་སྲིད defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    sa_nracanaa strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = NULL;
    istr.ic_len = 0;

    གལ་སྲིད (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ལོག 2;
#endif
    ལོག isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
