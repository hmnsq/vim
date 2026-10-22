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

#अन्तर्भाव "vim.h"

#जइ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#अन्तर्भाव <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#जइ HAVE_STROPTS_H
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
#  जइ defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#जइ HAVE_TERMIO_H
# include <termio.h>
#अण्णहा
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#जइ HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#जइ HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#जइ !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#जइ defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#जइ defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#जइ defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
वर्ण *ptsname(पूर्णाङ्क);
पूर्णाङ्क unlockpt(पूर्णाङ्क);
पूर्णाङ्क grantpt(पूर्णाङ्क);
पूर्णाङ्क posix_openpt(पूर्णाङ्क flags);
#endif

    स्थिर शून्य
initmaster(पूर्णाङ्क f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# अण्णहा
#  ifdef TIOCFLUSH
    (शून्य)ioctl(f, TIOCFLUSH, (वर्ण *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (शून्य)ioctl(f, TIOCEXCL, (वर्ण *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    पूर्णाङ्क
setup_slavepty(पूर्णाङ्क fd)
{
    जइ (fd < 0)
	प्रत्यागम 0;
#जइ defined(I_प्रक्षिप) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# जइ defined(HAVE_SYS_PTEM_H) || defined(hpux)
    जइ (ioctl(fd, I_प्रक्षिप, "ptem") != 0)
	प्रत्यागम -1;
# endif
    जइ (ioctl(fd, I_प्रक्षिप, "ldterm") != 0)
	प्रत्यागम -1;
# ifdef SUN_SYSTEM
    जइ (ioctl(fd, I_प्रक्षिप, "ttcompat") != 0)
	प्रत्यागम -1;
# endif
#endif
    प्रत्यागम 0;
}

#जइ defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    पूर्णाङ्क			f;
    वर्ण		*m;
    sighandler_T	sigcld;
    स्थिर वर्ण TtyName[32];  // used for opening a new pty-pair

    जइ ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	प्रत्यागम -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    जइ ((m = ptsname(f)) == रिक्त || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	प्रत्यागम -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, परिमाण(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#जइ defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    पूर्णाङ्क		f;
    स्थिर वर्ण TtyName[32];

    जइ ((f = open_controlling_pty(TtyName)) < 0)
	प्रत्यागम -1;
    initmaster(f);
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#जइ (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    वर्ण	*m, *s;
    पूर्णाङ्क		f;
    // used for opening a new pty-pair:
    स्थिर वर्ण PtyName[32];
    स्थिर वर्ण TtyName[32];

    जइ ((f = getpseudotty(&s, &m)) < 0)
	प्रत्यागम -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, परिमाण(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, परिमाण(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#जइ defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    पूर्णाङ्क		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    स्थिर वर्ण TtyName[32];

    जइ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	प्रत्यागम -1;
    जइ (mch_fstat(f, &buf) < 0)
    {
	close(f);
	प्रत्यागम -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#जइ defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    पूर्णाङ्क			f;
    वर्ण		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    स्थिर वर्ण TtyName[32];

    जइ ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	प्रत्यागम -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    जइ ((m = ptsname(f)) == रिक्त || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	प्रत्यागम -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, परिमाण(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#जइ defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
स्थिर पूर्णाङ्क aixhack = -1;
# endif

    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    पूर्णाङ्क		f;
    // used for opening a new pty-pair:
    स्थिर वर्ण TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    जइ ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	प्रत्यागम -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), परिमाण(TtyName) - 1);
    जइ (geteuid() != मूलम्_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	प्रत्यागम -1;
    }
    initmaster(f);
# ifdef _IBMR2
    जइ (aixhack >= 0)
	close(aixhack);
    जइ ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	प्रत्यागम -1;
    }
# endif
    *ttyn = TtyName;
    प्रत्यागम f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
स्थिर ध्रुव वर्ण PtyProto[] = "/dev/ptym/ptyXY";
स्थिर ध्रुव वर्ण TtyProto[] = "/dev/pty/ttyXY";
# अण्णहा
#  ifdef __HAIKU__
स्थिर ध्रुव वर्ण PtyProto[] = "/dev/pt/XY";
स्थिर ध्रुव वर्ण TtyProto[] = "/dev/tt/XY";
#  अण्णहा
स्थिर ध्रुव वर्ण PtyProto[] = "/dev/ptyXY";
स्थिर ध्रुव वर्ण TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    पूर्णाङ्क
mch_openpty(वर्ण **ttyn)
{
    वर्ण	*p, *q, *l, *d;
    पूर्णाङ्क		f;
    // used for opening a new pty-pair:
    स्थिर वर्ण PtyName[32];
    स्थिर वर्ण TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, परिमाण(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, परिमाण(TtyName) - 1);
    प्रति (p = PtyName; *p != 'X'; p++)
	;
    प्रति (q = TtyName; *q != 'X'; q++)
	;
    प्रति (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	प्रति (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    जइ ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		अनुवर्त;
	    q[0] = *l;
	    q[1] = *d;
	    जइ (geteuid() != मूलम्_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		अनुवर्त;
	    }
# जइ defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		पूर्णाङ्क pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		जइ (ioctl(f, TIOCGPGRP, (वर्ण *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    अनुवर्त;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    प्रत्यागम f;
	}
    }
    प्रत्यागम -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    पूर्णाङ्क
mch_isatty(पूर्णाङ्क fd)
{
#जइ defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    संरचना strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = रिक्त;
    istr.ic_len = 0;

    जइ (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	प्रत्यागम 2;
#endif
    प्रत्यागम isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
