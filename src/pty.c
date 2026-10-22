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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ଯଦି defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#ଅନ୍ତର୍ଭାଵ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#ଯଦି HAVE_STROPTS_H
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
#  ଯଦି defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#ଯଦି HAVE_TERMIO_H
# include <termio.h>
#ଅନ୍ୟଥା
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#ଯଦି HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#ଯଦି HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#ଯଦି !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#ଯଦି defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#ଯଦି defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#ଯଦି defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
ଅକ୍ଷର *ptsname(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା unlockpt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା grantpt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପୂର୍ଣ୍ଣସଂଖ୍ୟା posix_openpt(ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags);
#endif

    ସ୍ଥିର ଶୂନ୍ୟ
initmaster(ପୂର୍ଣ୍ଣସଂଖ୍ୟା f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# ଅନ୍ୟଥା
#  ifdef TIOCFLUSH
    (ଶୂନ୍ୟ)ioctl(f, TIOCFLUSH, (ଅକ୍ଷର *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (ଶୂନ୍ୟ)ioctl(f, TIOCEXCL, (ଅକ୍ଷର *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
setup_slavepty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    ଯଦି (fd < 0)
	ଫେରନ୍ତୁ 0;
#ଯଦି defined(I_ଠେଲ) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# ଯଦି defined(HAVE_SYS_PTEM_H) || defined(hpux)
    ଯଦି (ioctl(fd, I_ଠେଲ, "ptem") != 0)
	ଫେରନ୍ତୁ -1;
# endif
    ଯଦି (ioctl(fd, I_ଠେଲ, "ldterm") != 0)
	ଫେରନ୍ତୁ -1;
# ifdef SUN_SYSTEM
    ଯଦି (ioctl(fd, I_ଠେଲ, "ttcompat") != 0)
	ଫେରନ୍ତୁ -1;
# endif
#endif
    ଫେରନ୍ତୁ 0;
}

#ଯଦି defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			f;
    ଅକ୍ଷର		*m;
    sighandler_T	sigcld;
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];  // used for opening a new pty-pair

    ଯଦି ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	ଫେରନ୍ତୁ -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ଯଦି ((m = ptsname(f)) == ଖାଲି || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ଫେରନ୍ତୁ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ଆକାର(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ଯଦି defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    ଯଦି ((f = open_controlling_pty(TtyName)) < 0)
	ଫେରନ୍ତୁ -1;
    initmaster(f);
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ଯଦି (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ଅକ୍ଷର	*m, *s;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    // used for opening a new pty-pair:
    ସ୍ଥିର ଅକ୍ଷର PtyName[32];
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    ଯଦି ((f = getpseudotty(&s, &m)) < 0)
	ଫେରନ୍ତୁ -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, ଆକାର(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, ଆକାର(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ଯଦି defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    ଯଦି ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	ଫେରନ୍ତୁ -1;
    ଯଦି (mch_fstat(f, &buf) < 0)
    {
	close(f);
	ଫେରନ୍ତୁ -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ଯଦି defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			f;
    ଅକ୍ଷର		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    ଯଦି ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	ଫେରନ୍ତୁ -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    ଯଦି ((m = ptsname(f)) == ଖାଲି || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	ଫେରନ୍ତୁ -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, ଆକାର(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ଯଦି defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା aixhack = -1;
# endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    // used for opening a new pty-pair:
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    ଯଦି ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	ଫେରନ୍ତୁ -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), ଆକାର(TtyName) - 1);
    ଯଦି (geteuid() != ମୂଳ_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	ଫେରନ୍ତୁ -1;
    }
    initmaster(f);
# ifdef _IBMR2
    ଯଦି (aixhack >= 0)
	close(aixhack);
    ଯଦି ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	ଫେରନ୍ତୁ -1;
    }
# endif
    *ttyn = TtyName;
    ଫେରନ୍ତୁ f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର PtyProto[] = "/dev/ptym/ptyXY";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର TtyProto[] = "/dev/pty/ttyXY";
# ଅନ୍ୟଥା
#  ifdef __HAIKU__
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର PtyProto[] = "/dev/pt/XY";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର TtyProto[] = "/dev/tt/XY";
#  ଅନ୍ୟଥା
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର PtyProto[] = "/dev/ptyXY";
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_openpty(ଅକ୍ଷର **ttyn)
{
    ଅକ୍ଷର	*p, *q, *l, *d;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		f;
    // used for opening a new pty-pair:
    ସ୍ଥିର ଅକ୍ଷର PtyName[32];
    ସ୍ଥିର ଅକ୍ଷର TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, ଆକାର(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, ଆକାର(TtyName) - 1);
    ପାଇଁ (p = PtyName; *p != 'X'; p++)
	;
    ପାଇଁ (q = TtyName; *q != 'X'; q++)
	;
    ପାଇଁ (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	ପାଇଁ (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    ଯଦି ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ଜାରି;
	    q[0] = *l;
	    q[1] = *d;
	    ଯଦି (geteuid() != ମୂଳ_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ଜାରି;
	    }
# ଯଦି defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		ଯଦି (ioctl(f, TIOCGPGRP, (ଅକ୍ଷର *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ଜାରି;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    ଫେରନ୍ତୁ f;
	}
    }
    ଫେରନ୍ତୁ -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isatty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
#ଯଦି defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    ସଂରଚନା strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ଖାଲି;
    istr.ic_len = 0;

    ଯଦି (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	ଫେରନ୍ତୁ 2;
#endif
    ଫେରନ୍ତୁ isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
