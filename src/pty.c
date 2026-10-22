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

#شامل "vim.h"

#اگر defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#شامل <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#اگر HAVE_STROPTS_H
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
#  اگر defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#اگر HAVE_TERMIO_H
# include <termio.h>
#وگرنه
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#اگر HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#اگر HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#اگر !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#اگر defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#اگر defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#اگر defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
نویسه *ptsname(صحیح);
صحیح unlockpt(صحیح);
صحیح grantpt(صحیح);
صحیح posix_openpt(صحیح flags);
#endif

    ایستا تهی
initmaster(صحیح f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# وگرنه
#  ifdef TIOCFLUSH
    (تهی)ioctl(f, TIOCFLUSH, (نویسه *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (تهی)ioctl(f, TIOCEXCL, (نویسه *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    صحیح
setup_slavepty(صحیح fd)
{
    اگر (fd < 0)
	بازگشت 0;
#اگر defined(I_افزودن) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# اگر defined(HAVE_SYS_PTEM_H) || defined(hpux)
    اگر (ioctl(fd, I_افزودن, "ptem") != 0)
	بازگشت -1;
# endif
    اگر (ioctl(fd, I_افزودن, "ldterm") != 0)
	بازگشت -1;
# ifdef SUN_SYSTEM
    اگر (ioctl(fd, I_افزودن, "ttcompat") != 0)
	بازگشت -1;
# endif
#endif
    بازگشت 0;
}

#اگر defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    صحیح
mch_openpty(نویسه **ttyn)
{
    صحیح			f;
    نویسه		*m;
    sighandler_T	sigcld;
    ایستا نویسه TtyName[32];  // used for opening a new pty-pair

    اگر ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	بازگشت -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    اگر ((m = ptsname(f)) == پوچ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	بازگشت -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, اندازه(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#اگر defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    صحیح
mch_openpty(نویسه **ttyn)
{
    صحیح		f;
    ایستا نویسه TtyName[32];

    اگر ((f = open_controlling_pty(TtyName)) < 0)
	بازگشت -1;
    initmaster(f);
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#اگر (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    صحیح
mch_openpty(نویسه **ttyn)
{
    نویسه	*m, *s;
    صحیح		f;
    // used for opening a new pty-pair:
    ایستا نویسه PtyName[32];
    ایستا نویسه TtyName[32];

    اگر ((f = getpseudotty(&s, &m)) < 0)
	بازگشت -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, اندازه(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, اندازه(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#اگر defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    صحیح
mch_openpty(نویسه **ttyn)
{
    صحیح		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ایستا نویسه TtyName[32];

    اگر ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	بازگشت -1;
    اگر (mch_fstat(f, &buf) < 0)
    {
	close(f);
	بازگشت -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#اگر defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    صحیح
mch_openpty(نویسه **ttyn)
{
    صحیح			f;
    نویسه		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ایستا نویسه TtyName[32];

    اگر ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	بازگشت -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    اگر ((m = ptsname(f)) == پوچ || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	بازگشت -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, اندازه(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#اگر defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
ایستا صحیح aixhack = -1;
# endif

    صحیح
mch_openpty(نویسه **ttyn)
{
    صحیح		f;
    // used for opening a new pty-pair:
    ایستا نویسه TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    اگر ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	بازگشت -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), اندازه(TtyName) - 1);
    اگر (geteuid() != ریشه_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	بازگشت -1;
    }
    initmaster(f);
# ifdef _IBMR2
    اگر (aixhack >= 0)
	close(aixhack);
    اگر ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	بازگشت -1;
    }
# endif
    *ttyn = TtyName;
    بازگشت f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
ایستا ثابت نویسه PtyProto[] = "/dev/ptym/ptyXY";
ایستا ثابت نویسه TtyProto[] = "/dev/pty/ttyXY";
# وگرنه
#  ifdef __HAIKU__
ایستا ثابت نویسه PtyProto[] = "/dev/pt/XY";
ایستا ثابت نویسه TtyProto[] = "/dev/tt/XY";
#  وگرنه
ایستا ثابت نویسه PtyProto[] = "/dev/ptyXY";
ایستا ثابت نویسه TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    صحیح
mch_openpty(نویسه **ttyn)
{
    نویسه	*p, *q, *l, *d;
    صحیح		f;
    // used for opening a new pty-pair:
    ایستا نویسه PtyName[32];
    ایستا نویسه TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, اندازه(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, اندازه(TtyName) - 1);
    برای (p = PtyName; *p != 'X'; p++)
	;
    برای (q = TtyName; *q != 'X'; q++)
	;
    برای (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	برای (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    اگر ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ادامه;
	    q[0] = *l;
	    q[1] = *d;
	    اگر (geteuid() != ریشه_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ادامه;
	    }
# اگر defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		صحیح pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		اگر (ioctl(f, TIOCGPGRP, (نویسه *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ادامه;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    بازگشت f;
	}
    }
    بازگشت -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    صحیح
mch_isatty(صحیح fd)
{
#اگر defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ساختار strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = پوچ;
    istr.ic_len = 0;

    اگر (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	بازگشت 2;
#endif
    بازگشت isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
