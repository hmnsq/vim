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

#અન્તર્ભાવ "vim.h"

#જો defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)

#અન્તર્ભાવ <signal.h>

#ifdef HAVE_SYS_IOCTL_H
# include <sys/ioctl.h>
#endif

#જો HAVE_STROPTS_H
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
#  જો defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS)
#   include <sys/ptms.h>
#  endif
# endif
#endif

#ifdef HAVE_UNISTD_H
# include <unistd.h>
#endif

#જો HAVE_TERMIO_H
# include <termio.h>
#નહીંતર
# ifdef HAVE_TERMIOS_H
#  include <termios.h>
# endif
#endif

#જો HAVE_SYS_STREAM_H
# include <sys/stream.h>
#endif

#જો HAVE_SYS_PTEM_H
# include <sys/ptem.h>
#endif

#જો !defined(SUN_SYSTEM) && !defined(VMS)
# include <sys/ioctl.h>
#endif

#જો defined(SUN_SYSTEM) && defined(LOCKPTY) && !defined(TIOCEXCL)
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

#જો defined(_INCLUDE_HPUX_SOURCE) && !defined(hpux)
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

#જો defined(HAVE_SVR4_PTYS) || defined(HAVE_POSIX_OPENPT)
// These should be in stdlib.h, but it depends on _XOPEN_SOURCE.
અક્ષર *ptsname(પૂર્ણાંક);
પૂર્ણાંક unlockpt(પૂર્ણાંક);
પૂર્ણાંક grantpt(પૂર્ણાંક);
પૂર્ણાંક posix_openpt(પૂર્ણાંક flags);
#endif

    સ્થિર શૂન્ય
initmaster(પૂર્ણાંક f UNUSED)
{
#ifndef VMS
# ifdef POSIX
    tcflush(f, TCIOFLUSH);
# નહીંતર
#  ifdef TIOCFLUSH
    (શૂન્ય)ioctl(f, TIOCFLUSH, (અક્ષર *) 0);
#  endif
# endif
# ifdef LOCKPTY
    (શૂન્ય)ioctl(f, TIOCEXCL, (અક્ષર *) 0);
# endif
#endif
}

/*
 * This causes a hang on some systems, but is required for a properly working
 * pty on others.  Needs to be tuned...
 */
    પૂર્ણાંક
setup_slavepty(પૂર્ણાંક fd)
{
    જો (fd < 0)
	પાછા 0;
#જો defined(I_ધકેલો) && defined(HAVE_SVR4_PTYS) && !defined(sgi) \
	&& !defined(linux) && !defined(__osf__) && !defined(M_UNIX)
# જો defined(HAVE_SYS_PTEM_H) || defined(hpux)
    જો (ioctl(fd, I_ધકેલો, "ptem") != 0)
	પાછા -1;
# endif
    જો (ioctl(fd, I_ધકેલો, "ldterm") != 0)
	પાછા -1;
# ifdef SUN_SYSTEM
    જો (ioctl(fd, I_ધકેલો, "ttcompat") != 0)
	પાછા -1;
# endif
#endif
    પાછા 0;
}

#જો defined(HAVE_POSIX_OPENPT) && !defined(PTY_DONE)
# define PTY_DONE
    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    પૂર્ણાંક			f;
    અક્ષર		*m;
    sighandler_T	sigcld;
    સ્થિર અક્ષર TtyName[32];  // used for opening a new pty-pair

    જો ((f = posix_openpt(O_RDWR | O_NOCTTY | O_EXTRA)) == -1)
	પાછા -1;

    // SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
    // exec()s pt_chmod
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    જો ((m = ptsname(f)) == ખાલી || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	પાછા -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, કદ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    પાછા f;
}
#endif

#જો defined(OSX) && !defined(PTY_DONE)
# define PTY_DONE
    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    પૂર્ણાંક		f;
    સ્થિર અક્ષર TtyName[32];

    જો ((f = open_controlling_pty(TtyName)) < 0)
	પાછા -1;
    initmaster(f);
    *ttyn = TtyName;
    પાછા f;
}
#endif

#જો (defined(sequent) || defined(_SEQUENT_)) && defined(HAVE_GETPSEUDOTTY) \
	&& !defined(PTY_DONE)
# define PTY_DONE
    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    અક્ષર	*m, *s;
    પૂર્ણાંક		f;
    // used for opening a new pty-pair:
    સ્થિર અક્ષર PtyName[32];
    સ્થિર અક્ષર TtyName[32];

    જો ((f = getpseudotty(&s, &m)) < 0)
	પાછા -1;
# ifdef _SEQUENT_
    fvhangup(s);
# endif
    vim_strncpy((char_u *)PtyName, (char_u *)m, કદ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)s, કદ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    પાછા f;
}
#endif

#જો defined(MIPS) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE
    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    પૂર્ણાંક		f;
    stat_T	buf;
    // used for opening a new pty-pair:
    સ્થિર અક્ષર TtyName[32];

    જો ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_NONBLOCK | O_EXTRA, 0)) < 0)
	પાછા -1;
    જો (mch_fstat(f, &buf) < 0)
    {
	close(f);
	પાછા -1;
    }
    sprintf(TtyName, "/dev/ttyq%d", minor(buf.st_rdev));
    initmaster(f);
    *ttyn = TtyName;
    પાછા f;
}
#endif

#જો defined(HAVE_SVR4_PTYS) && !defined(PTY_DONE) && !defined(hpux) \
	    && !(defined(MACOS_X) && !defined(MAC_OS_X_VERSION_10_6))

// NOTE: Even though HPUX can have /dev/ptmx, the code below doesn't work!
// Same for Mac OS X Leopard (10.5).
# define PTY_DONE
    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    પૂર્ણાંક			f;
    અક્ષર		*m;
    sighandler_T	sigcld;
    // used for opening a new pty-pair:
    સ્થિર અક્ષર TtyName[32];

    જો ((f = open("/dev/ptmx", O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
	પાછા -1;

    /*
     * SIGCHLD set to SIG_DFL for grantpt() because it fork()s and
     * exec()s pt_chmod
     */
    sigcld = mch_signal(SIGCHLD, SIG_DFL);
    જો ((m = ptsname(f)) == ખાલી || grantpt(f) || unlockpt(f))
    {
	mch_signal(SIGCHLD, sigcld);
	close(f);
	પાછા -1;
    }
    mch_signal(SIGCHLD, sigcld);
    vim_strncpy((char_u *)TtyName, (char_u *)m, કદ(TtyName) - 1);
    initmaster(f);
    *ttyn = TtyName;
    પાછા f;
}
#endif

#જો defined(_AIX) && defined(HAVE_DEV_PTC) && !defined(PTY_DONE)
# define PTY_DONE

# ifdef _IBMR2
સ્થિર પૂર્ણાંક aixhack = -1;
# endif

    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    પૂર્ણાંક		f;
    // used for opening a new pty-pair:
    સ્થિર અક્ષર TtyName[32];

    // a dumb looking loop replaced by mycrofts code:
    જો ((f = open("/dev/ptc", O_RDWR | O_NOCTTY | O_EXTRA)) < 0)
	પાછા -1;
    vim_strncpy((char_u *)TtyName, (char_u *)ttyname(f), કદ(TtyName) - 1);
    જો (geteuid() != મૂળ_UID && mch_access(TtyName, R_OK | W_OK))
    {
	close(f);
	પાછા -1;
    }
    initmaster(f);
# ifdef _IBMR2
    જો (aixhack >= 0)
	close(aixhack);
    જો ((aixhack = open(TtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) < 0)
    {
	close(f);
	પાછા -1;
    }
# endif
    *ttyn = TtyName;
    પાછા f;
}
#endif

#ifndef PTY_DONE

# ifdef hpux
સ્થિર અચલ અક્ષર PtyProto[] = "/dev/ptym/ptyXY";
સ્થિર અચલ અક્ષર TtyProto[] = "/dev/pty/ttyXY";
# નહીંતર
#  ifdef __HAIKU__
સ્થિર અચલ અક્ષર PtyProto[] = "/dev/pt/XY";
સ્થિર અચલ અક્ષર TtyProto[] = "/dev/tt/XY";
#  નહીંતર
સ્થિર અચલ અક્ષર PtyProto[] = "/dev/ptyXY";
સ્થિર અચલ અક્ષર TtyProto[] = "/dev/ttyXY";
#  endif
# endif

    પૂર્ણાંક
mch_openpty(અક્ષર **ttyn)
{
    અક્ષર	*p, *q, *l, *d;
    પૂર્ણાંક		f;
    // used for opening a new pty-pair:
    સ્થિર અક્ષર PtyName[32];
    સ્થિર અક્ષર TtyName[32];

    vim_strncpy((char_u *)PtyName, (char_u *)PtyProto, કદ(PtyName) - 1);
    vim_strncpy((char_u *)TtyName, (char_u *)TtyProto, કદ(TtyName) - 1);
    માટે (p = PtyName; *p != 'X'; p++)
	;
    માટે (q = TtyName; *q != 'X'; q++)
	;
    માટે (l = PTYRANGE0; (*p = *l) != '\0'; l++)
    {
	માટે (d = PTYRANGE1; (p[1] = *d) != '\0'; d++)
	{
	    જો ((f = open(PtyName, O_RDWR | O_NOCTTY | O_EXTRA, 0)) == -1)
		ચાલુ;
	    q[0] = *l;
	    q[1] = *d;
	    જો (geteuid() != મૂળ_UID && mch_access(TtyName, R_OK | W_OK))
	    {
		close(f);
		ચાલુ;
	    }
# જો defined(SUN_SYSTEM) && defined(TIOCGPGRP) && !defined(SUNOS3)
	    // Hack to ensure that the slave side of the pty is
	    // unused. May not work in anything other than SunOS4.1
	    {
		પૂર્ણાંક pgrp;

		// tcgetpgrp does not work (uses TIOCGETPGRP)!
		જો (ioctl(f, TIOCGPGRP, (અક્ષર *)&pgrp) != -1 || errno != EIO)
		{
		    close(f);
		    ચાલુ;
		}
	    }
# endif
	    initmaster(f);
	    *ttyn = TtyName;
	    પાછા f;
	}
    }
    પાછા -1;
}
#endif

/*
 * Call isatty(fd), except for SunOS where it's done differently.
 */
    પૂર્ણાંક
mch_isatty(પૂર્ણાંક fd)
{
#જો defined(I_STR) && defined(HAVE_SYS_PTMS_H) && defined(HAVE_SVR4_PTYS) \
	&& defined(SUN_SYSTEM)
    // On SunOS, isatty() for /dev/ptmx returns false or sometimes can hang up
    // in the inner ioctl(), and therefore first determine whether "fd" is a
    // master device.
    સંરચના strioctl istr;

    istr.ic_cmd = ISPTM;
    istr.ic_timout = 0;
    istr.ic_dp = ખાલી;
    istr.ic_len = 0;

    જો (ioctl(fd, I_STR, &istr) == 0)
	// Trick: return 2 in order to advice the caller that "fd" is a master
	// device. cf. src/os_unix.c:get_tty_fd()
	પાછા 2;
#endif
    પાછા isatty(fd);
}

#endif // FEAT_GUI || FEAT_JOB_CHANNEL
