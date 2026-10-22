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

#அந்தர்பாவ "vim.h"

#எனில் defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#அந்தர்பாவ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#எனில் HAVE_STROPTS_H
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
#  எனில் defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#எனில் HAVE_TERMIO_H
# include <termio.h>
#இல்லையெனில்
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#எனில் HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#எனில் HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#எனில் !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#எனில் defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#எனில் defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#எனில் defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
எழுத்து *ptsname(முழு);
முழு unlockpt(முழு);
முழு grantpt(முழு);
முழு posix_openpt(முழு flags);
#endif

    நிலையான வெறுமை
initmaster(முழு f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# இல்லையெனில்
#  ifdef TIOCFLUSH
    (வெறுமை)ioctl(f, TIOCFLUSH, (எழுத்து *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (வெறுமை)ioctl(f, TIOCEXCL, (எழுத்து *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    முழு
setup_slavepty(முழு fd)
{
    எனில் (fd < 0)
	திரும்பு 0;
#எனில் defined(I_தள்ளு) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# எனில் defined(HAVE_SYS_PTEM_H) || defined(hpux)
    எனில் (ioctl(fd, I_தள்ளு, "ptem") != 0)
	திரும்பு -1;
# endif
    எனில் (ioctl(fd, I_தள்ளு, "ldterm") != 0)
	திரும்பு -1;
# ifdef SUN_SYSTEM
    எனில் (ioctl(fd, I_தள்ளு, "ttcompat") != 0)
	திரும்பு -1;
# endif
#endif
    திரும்பு 0;
}

#எனில் defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    முழு
mch_openpty(எழுத்து **ttyn)
{
    முழு			f;
    எழுத்து		*m;
    sighandler_T	sigcld;
    நிலையான எழுத்து TtyName[32];  // used for opening a new pty-pair

    எனில் ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	திரும்பு -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    எனில் ((m = ptsname(f)) == வெற்று || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	திரும்பு -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, அளவு(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#எனில் defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    முழு
mch_openpty(எழுத்து **ttyn)
{
    முழு		f;
    நிலையான எழுத்து TtyName[32];

    எனில் ((f = open_controlling_pty(TtyName)) < 0)
	திரும்பு -1;
    initmaster(f);
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#எனில் (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    முழு
mch_openpty(எழுத்து **ttyn)
{
    எழுத்து	*m, *s;
    முழு		f;
    // used for opening a new pty-pair:
    நிலையான எழுத்து PtyName[32];
    நிலையான எழுத்து TtyName[32];

    எனில் ((f = getpseudotty(&s, &m)) < 0)
	திரும்பு -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, அளவு(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, அளவு(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#எனில் defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    முழு
mch_openpty(எழுத்து **ttyn)
{
    முழு		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    நிலையான எழுத்து TtyName[32];

    எனில் ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	திரும்பு -1;
    எனில் (mch_fstat(f, &buf) < 0)
    {
	close(f);
	திரும்பு -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#எனில் defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    முழு
mch_openpty(எழுத்து **ttyn)
{
    முழு			f;
    எழுத்து		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    நிலையான எழுத்து TtyName[32];

    எனில் ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	திரும்பு -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    எனில் ((m = ptsname(f)) == வெற்று || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	திரும்பு -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, அளவு(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#எனில் defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
நிலையான முழு aixhack = -1;
# endif

    முழு
mch_openpty(எழுத்து **ttyn)
{
    முழு		f;
    // used for opening a new pty-pair:
    நிலையான எழுத்து TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    எனில் ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	திரும்பு -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), அளவு(TtyName) - 1);
    எனில் (geteuid() != வேர்_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	திரும்பு -1;
    }
    initmaster(f);
# ifdef _IBMR2
    எனில் (aixhack >= 0)
	close(aixhack);
    எனில் ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	திரும்பு -1;
    }
# endif
    *ttyn = TtyName;
    திரும்பு f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
நிலையான மாறிலி எழுத்து PtyProto[] = "/dev/ptym/ptyXY";
நிலையான மாறிலி எழுத்து TtyProto[] = "/dev/pty/ttyXY";
# இல்லையெனில்
#  ifdef __HAIKU__
நிலையான மாறிலி எழுத்து PtyProto[] = "/dev/pt/XY";
நிலையான மாறிலி எழுத்து TtyProto[] = "/dev/tt/XY";
#  இல்லையெனில்
நிலையான மாறிலி எழுத்து PtyProto[] = "/dev/ptyXY";
நிலையான மாறிலி எழுத்து TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    முழு
mch_openpty(எழுத்து **ttyn)
{
    எழுத்து	*p, *q, *l, *d;
    முழு		f;
    // used for opening a new pty-pair:
    நிலையான எழுத்து PtyName[32];
    நிலையான எழுத்து TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, அளவு(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, அளவு(TtyName) - 1);
    க்கு (p = PtyName; *p != 'X'; p++)
	;
    க்கு (q = TtyName; *q != 'X'; q++)
	;
    க்கு (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	க்கு (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    எனில் ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		தொடர்;
	    q[0] = *l;
	    q[1] = *d;
	    எனில் (geteuid() != வேர்_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		தொடர்;
	    }
# எனில் defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		முழு pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		எனில் (ioctl(f, TIOCGPGRP, (எழுத்து *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    தொடர்;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    திரும்பு f;
	}
    }
    திரும்பு -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    முழு
mch_isatty(முழு fd)
{
#எனில் defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    கட்டமைப்பு strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = வெற்று;
    istr.ic_len = 0;

    எனில் (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	திரும்பு 2;
#endif
    திரும்பு isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
