/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *	      OS/2 port by Paul Slootman
 *	      VMS merge by Zoltan Arpadffy
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_unix.c -- code for all flavors of Unix (BSD, SYSV, SVR4, POSIX, ...)
 *	     Also for OS/2, using the excellent EMX package!!!
 *	     Also for Atari MiNT.
 *
 * A lot of this file was originally written by Juergen Weigert and later
 * changed beyond recognition.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ಅನ್ತರ್ಭಾವ "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ selinux_enabled = -1;
#endif

#ifdef FEAT_XATTR
# include <sys/xattr.h>
#endif

#ifdef HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#endif

#ifdef __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#  include "winclip.pro"
# endif
#endif

#ifdef FEAT_MOUSE_GPM

# include <gpm.h>

# ifdef DYNAMIC_GPM
#  define Gpm_Open     (*dll_Gpm_Open)
#  define Gpm_Close    (*dll_Gpm_Close)
#  define Gpm_GetEvent (*dll_Gpm_GetEvent)
#  define gpm_flag     (dll_gpm_flag != ರಿಕ್ತ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ರಿಕ್ತ ? *dll_gpm_fd   : -1)

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_Gpm_Open)     (Gpm_Connect *, ಪೂರ್ಣಾಙ್ಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_Gpm_Close)    (ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ (*dll_Gpm_GetEvent) (Gpm_Event *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *dll_gpm_flag;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ *dll_gpm_fd;

ಸ್ಥಿರ ಶೂನ್ಯ *libgpm_hinst;
# endif

// <linux/keyboard.h> contains defines conflicting with "keymap.h",
// I just copied relevant defines here. A cleaner solution would be to put gpm
// code into separate file and include there linux/keyboard.h
// #include <linux/keyboard.h>
# define KG_SHIFT	0
# define KG_CTRL	2
# define KG_ALT		3
# define KG_ALTGR	1
# define KG_SHIFTL	4
# define KG_SHIFTR	5
# define KG_CTRLL	6
# define KG_CTRLR	7
# define KG_CAPSSHIFT	8

ಸ್ಥಿರ ಶೂನ್ಯ gpm_close(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ gpm_open(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ mch_gpm_process(ಶೂನ್ಯ);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sysmouse_open(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ sysmouse_close(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#ಯದಿ defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#ಯದಿ defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#endif

#ifdef FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
#  include <X11/Shell.h>
#  include <X11/StringDefs.h>
ಸ್ಥಿರ Widget	xterm_Shell = (Widget)0;
ಸ್ಥಿರ ಶೂನ್ಯ clip_update(ಶೂನ್ಯ);
# endif

Window	    x11_window = 0;
Display	    *x11_display = ರಿಕ್ತ;
#endif

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ignore_sigtstp = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_x11_title(ಪೂರ್ಣಾಙ್ಕ);

ಸ್ಥಿರ char_u	*oldtitle = ರಿಕ್ತ;
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t oldtitle_outdated = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	unix_did_set_title = FALSE;
ಸ್ಥಿರ char_u	*oldicon = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_set_icon = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ may_core_dump(ಶೂನ್ಯ);

ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ waitstatus;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  WaitForChar(long msec, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  WaitForCharOrMouse(long msec, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು);
#ifdef VMS
ಪೂರ್ಣಾಙ್ಕ  RealWaitForChar(ಪೂರ್ಣಾಙ್ಕ, long, ಪೂರ್ಣಾಙ್ಕ *, ಪೂರ್ಣಾಙ್ಕ *interrupted);
#ಅನ್ಯಥಾ
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  RealWaitForChar(ಪೂರ್ಣಾಙ್ಕ, long, ಪೂರ್ಣಾಙ್ಕ *, ಪೂರ್ಣಾಙ್ಕ *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ do_xterm_trace(ಶೂನ್ಯ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

ಸ್ಥಿರ ಶೂನ್ಯ handle_resize(ಶೂನ್ಯ);

#ಯದಿ defined(SIGWINCH)
ಸ್ಥಿರ ಶೂನ್ಯ sig_winch SIGPROTOARG;
#endif
#ಯದಿ defined(SIGTSTP)
ಸ್ಥಿರ ಶೂನ್ಯ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t in_mch_suspend = FALSE;
#endif
#ಯದಿ defined(SIGINT)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigint SIGPROTOARG;
#endif
#ಯದಿ defined(SIGUSR1)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigusr1 SIGPROTOARG;
#endif
#ಯದಿ defined(SIGPWR)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigpwr SIGPROTOARG;
#endif
#ಯದಿ defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
ಸ್ಥಿರ ಶೂನ್ಯ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t sig_alarm_called;
#endif
ಸ್ಥಿರ ಶೂನ್ಯ deathtrap SIGPROTOARG;

ಸ್ಥಿರ ಶೂನ್ಯ catch_int_signal(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ set_signals(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ catch_signals(ಶೂನ್ಯ (*func_deadly)(ಪೂರ್ಣಾಙ್ಕ), ಶೂನ್ಯ (*func_other)(ಪೂರ್ಣಾಙ್ಕ));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#ಅನ್ಯಥಾ
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	ಕುರು { /**/ } ಯಾವತ್ (0)
# define UNBLOCK_SIGNALS(set)	ಕುರು { /**/ } ಯಾವತ್ (0)
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  have_wildcard(ಪೂರ್ಣಾಙ್ಕ, char_u **);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ  have_dollars(ಪೂರ್ಣಾಙ್ಕ, char_u **);

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ save_patterns(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ_pat, char_u **pat, ಪೂರ್ಣಾಙ್ಕ *ಅಂಕೆ_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t got_tstp = FALSE;
ಸ್ಥಿರ char_u	*extra_shell_arg = ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#ಯದಿ defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ಸ್ಥಿರ tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    ವರ್ಣ *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ಸ್ಥಿರ xsmp_config_T xsmp;
#endif

#ifdef SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#endif

ಸ್ಥಿರ ಸಂರಚನಾ signalinfo
{
    ಪೂರ್ಣಾಙ್ಕ	    sig;	// Signal number, eg. SIGSEGV etc
    ವರ್ಣ    *name;	// Signal name (not char_u!).
    ವರ್ಣ    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#ifdef SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#endif
#ifdef SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#endif
#ifdef SIGILL
    {SIGILL,	    "ILL",	TRUE},
#endif
#ifdef SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#endif
#ifdef SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#endif
#ifdef SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#endif
#ifdef SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#endif
#ifdef SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#endif
#ಯದಿ defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#endif
#ifdef SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#endif
#ifdef SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#endif
#ifdef SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#endif
#ಯದಿ defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#ಯದಿ defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#endif
#ifdef SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#endif
#ifdef SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#endif
#ifdef SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#endif
#ಯದಿ defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#endif
#ifdef SIGINT
    {SIGINT,	    "INT",	FALSE},
#endif
#ifdef SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#endif
#ifdef SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#endif
#ifdef SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#endif
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(ಪೂರ್ಣಾಙ್ಕ sig, sighandler_T func)
{
#ಯದಿ defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ಸಂರಚನಾ sigaction	sa, old;
    sigset_t		curset;
    ಪೂರ್ಣಾಙ್ಕ			blocked;

    ಯದಿ (sigprocmask(SIG_BLOCK, ರಿಕ್ತ, &curset) == -1)
	ಪ್ರತ್ಯಾಗಮ SIG_ERR;

    blocked = sigismember(&curset, sig);

    ಯದಿ (func == SIG_HOLD)
    {
	ಯದಿ (blocked)
	    ಪ್ರತ್ಯಾಗಮ SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ಯದಿ (sigaction(sig, ರಿಕ್ತ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ರಿಕ್ತ) == -1)
	    ಪ್ರತ್ಯಾಗಮ SIG_ERR;
	ಪ್ರತ್ಯಾಗಮ old.sa_handler;
    }

    ಯದಿ (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ಯದಿ (sigprocmask(SIG_UNBLOCK, &curset, ರಿಕ್ತ) == -1)
	    ಪ್ರತ್ಯಾಗಮ SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# ಅನ್ಯಥಾ
    sa.sa_flags = 0;
# endif
    ಯದಿ (sigaction(sig, &sa, &old) == -1)
	ಪ್ರತ್ಯಾಗಮ SIG_ERR;
    ಪ್ರತ್ಯಾಗಮ blocked ? SIG_HOLD: old.sa_handler;
#ನೋಚೇದ್ಯದಿ defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    ಪ್ರತ್ಯಾಗಮ sigset(sig, func);
#ಅನ್ಯಥಾ
    // Oldest and most compatible solution.
    ಪ್ರತ್ಯಾಗಮ ಸಙ್ಕೇತ(sig, func);
#endif
}

    ಪೂರ್ಣಾಙ್ಕ
mch_chdir(ವರ್ಣ *path)
{
    ಯದಿ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    ಪ್ರತ್ಯಾಗಮ chdir(vms_fixfilename(path));
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ chdir(path);
#endif
}

#ಯದಿ defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    ಶೂನ್ಯ
mch_write(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    vim_ignored = (ಪೂರ್ಣಾಙ್ಕ)ಲಿಖ(1, (ವರ್ಣ *)s, len);
    ಯದಿ (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ರಿಕ್ತ, ರಿಕ್ತ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
resize_func(ಪೂರ್ಣಾಙ್ಕ check_only)
{
    ಯದಿ (check_only)
	ಪ್ರತ್ಯಾಗಮ do_resize;
    ಯಾವತ್ (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(ರಿಕ್ತ, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_inchar(
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಙ್ಕ		tb_change_cnt)
{
    ಪ್ರತ್ಯಾಗಮ inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_resize(ಶೂನ್ಯ)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_char_avail(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ WaitForChar(0L, ರಿಕ್ತ, FALSE);
}

#ಯದಿ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_check_messages(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ WaitForChar(0L, ರಿಕ್ತ, TRUE);
}
#endif

#ಯದಿ defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# ಯದಿ defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# ಯದಿ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
#  include <sys/sysinfo.h>
# endif
# ifdef MACOS_X
#  include <mach/mach_host.h>
#  include <mach/mach_port.h>
# endif

/*
 * Return total amount of memory available in Kbyte.
 * Doesn't change when memory has been allocated.
 */
    long_u
mch_total_mem(ಪೂರ್ಣಾಙ್ಕ special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ಸಂರಚನಾ vm_statistics64	vm_stat;
	natural_t		ಎಣಿಕೆ = HOST_VM_INFO64_ಎಣಿಕೆ;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ಎಣಿಕೆ);
#  ಅನ್ಯಥಾ
	ಸಂರಚನಾ vm_statistics	vm_stat;
	natural_t		ಎಣಿಕೆ = HOST_VM_INFO_ಎಣಿಕೆ;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ಎಣಿಕೆ);
#  endif
	ಯದಿ (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ಎಣಿಕೆ + vm_stat.active_ಎಣಿಕೆ
					    + vm_stat.inactive_ಎಣಿಕೆ
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_ಎಣಿಕೆ
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    ಯದಿ (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	ಪೂರ್ಣಾಙ್ಕ		mib[2];
	size_t		len = ಪರಿಮಾಣ(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  ಅನ್ಯಥಾ
	// sysctl() may return 32 bit or 64 bit, accept both
	ಸಙ್ಘ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  ಅನ್ಯಥಾ
	mib[1] = HW_USERMEM;
#  endif
	ಯದಿ (sysctl(mib, 2, &physmem, &len, ರಿಕ್ತ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  ಅನ್ಯಥಾ
	    ಯದಿ (len == ಪರಿಮಾಣ(physmem.u64))
		mem = (long_u)physmem.u64;
	    ಅನ್ಯಥಾ
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# ಯದಿ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    ಯದಿ (mem == 0)
    {
	ಸಂರಚನಾ sysinfo sinfo;

	// Linux way of getting amount of RAM available
	ಯದಿ (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    ಯಾವತ್ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  ಅನ್ಯಥಾ
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    ಯದಿ (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	ಯದಿ (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    ಯಾವತ್ (shiftright > 0 && (pagesize & 1) == 0)
	    {
		pagesize = (long_u)pagesize >> 1;
		--shiftright;
	    }
	    mem = (long_u)pagesize * pagecount;
	}
    }
# endif

    // Return the minimum of the physical memory and the user limit, because
    // using more than the user limit may cause Vim to be terminated.
# ಯದಿ defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ಸಂರಚನಾ rlimit	rlp;

	ಯದಿ (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (ಪರಿಮಾಣ(long_u) * 8 - 1))
#  ifdef RLIM_INFINITY
		&& rlp.rlim_cur != RLIM_INFINITY
#  endif
		&& ((long_u)rlp.rlim_cur >> 10) < (mem >> shiftright)
	   )
	{
	    mem = (long_u)rlp.rlim_cur;
	    shiftright = 10;
	}
    }
# endif

    ಯದಿ (mem > 0)
	ಪ್ರತ್ಯಾಗಮ mem >> shiftright;
    ಪ್ರತ್ಯಾಗಮ (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    ಶೂನ್ಯ
mch_delay(long msec, ಪೂರ್ಣಾಙ್ಕ flags)
{
    tmode_T	old_tmode;
    ಪೂರ್ಣಾಙ್ಕ		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    ಯದಿ (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	ಯದಿ (call_settmode)
	{
	    old_tmode = mch_cur_tmode;
	    settmode(TMODE_SLEEP);
	}

	/*
	 * Everybody sleeps in a different way...
	 * Prefer nanosleep(), some versions of usleep() can only sleep up to
	 * one second.
	 */
#ifdef FEAT_MZSCHEME
	ಕುರು
	{
	    // if total is large enough, wait by portions in p_mzq
	    ಯದಿ (total > p_mzq)
		msec = p_mzq;
	    ಅನ್ಯಥಾ
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    ಸಂರಚನಾ timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (ಶೂನ್ಯ)nanosleep(&ts, ರಿಕ್ತ);
	}
#ಅನ್ಯಥಾ
# ifdef HAVE_USLEEP
	ಯಾವತ್ (msec >= 1000)
	{
	    usleep((unsigned ಪೂರ್ಣಾಙ್ಕ)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned ಪೂರ್ಣಾಙ್ಕ)(msec * 1000));
# ಅನ್ಯಥಾ
#  ifndef HAVE_SELECT
	poll(ರಿಕ್ತ, 0, (ಪೂರ್ಣಾಙ್ಕ)msec);
#  ಅನ್ಯಥಾ
	{
	    ಸಂರಚನಾ timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	ಯಾವತ್ (total > 0);
#endif

	ಯದಿ (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    ಅನ್ಯಥಾ
	WaitForChar(msec, ರಿಕ್ತ, FALSE);
}

#ಯದಿ defined(HAVE_ರಾಶಿ_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ರಾಶಿ_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ ರಾಶಿ_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_ರಾಶಿ_growth(ವರ್ಣ *p)
{
    ಪೂರ್ಣಾಙ್ಕ		i;

    ರಾಶಿ_grows_downwards = (p > (ವರ್ಣ *)&i);
}
#endif

#ಯದಿ defined(HAVE_ರಾಶಿ_LIMIT)
ಸ್ಥಿರ ವರ್ಣ *ರಾಶಿ_limit = ರಿಕ್ತ;

# ಯದಿ defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ರಾಶಿ_limit(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ rlimit	rlp;
    ಪೂರ್ಣಾಙ್ಕ			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    ಯದಿ (getrlimit(RLIMIT_ರಾಶಿ, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (ಪರಿಮಾಣ(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# ಯದಿ defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    ಯದಿ (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		ಯದಿ (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	ಯದಿ (ರಾಶಿ_grows_downwards)
	{
	    ರಾಶಿ_limit = (ವರ್ಣ *)((long)&i - (lim / 16L * 15L));
	    ಯದಿ (ರಾಶಿ_limit >= (ವರ್ಣ *)&i)
		// overflow, set to 1/16 of current stack position
		ರಾಶಿ_limit = (ವರ್ಣ *)((long)&i / 16L);
	}
	ಅನ್ಯಥಾ
	{
	    ರಾಶಿ_limit = (ವರ್ಣ *)((long)&i + (lim / 16L * 15L));
	    ಯದಿ (ರಾಶಿ_limit <= (ವರ್ಣ *)&i)
		ರಾಶಿ_limit = ರಿಕ್ತ;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_stackcheck(ವರ್ಣ *p)
{
    ಯದಿ (ರಾಶಿ_limit == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ OK;

    ಯದಿ (ರಾಶಿ_grows_downwards)
    {
	ಯದಿ (p < ರಾಶಿ_limit)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (p > ರಾಶಿ_limit)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ಸ್ಥಿರ stack_t sigstk;			// for sigaltstack()
# ಅನ್ಯಥಾ
ಸ್ಥಿರ ಸಂರಚನಾ sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ಸ್ಥಿರ long ಪೂರ್ಣಾಙ್ಕ get_signal_ರಾಶಿ_size(ಶೂನ್ಯ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long ಪೂರ್ಣಾಙ್ಕ size = -1;

    // return size only if sysconf doesn't return an error
    ಯದಿ ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	ಪ್ರತ್ಯಾಗಮ size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    ಪ್ರತ್ಯಾಗಮ SIGSTKSZ;
# endif

    // otherwise guess the size
    ಪ್ರತ್ಯಾಗಮ 8000;
}

ಸ್ಥಿರ ವರ್ಣ *signal_ರಾಶಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
init_signal_ರಾಶಿ(ಶೂನ್ಯ)
{
    ಯದಿ (signal_ರಾಶಿ == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_ರಾಶಿ;
#  ಅನ್ಯಥಾ
    sigstk.ss_sp = signal_ರಾಶಿ;
#  endif
    sigstk.ss_size = get_signal_ರಾಶಿ_size();
    sigstk.ss_flags = 0;
    (ಶೂನ್ಯ)sigaltstack(&sigstk, ರಿಕ್ತ);
# ಅನ್ಯಥಾ
    sigstk.ss_sp = signal_ರಾಶಿ;
    ಯದಿ (ರಾಶಿ_grows_downwards)
	sigstk.ss_sp += get_signal_ರಾಶಿ_size() - 1;
    sigstk.ss_onstack = 0;
    (ಶೂನ್ಯ)sigstack(&sigstk, ರಿಕ್ತ);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#ಯದಿ defined(SIGWINCH)
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#ಯದಿ defined(SIGTSTP)
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    ಯದಿ (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ಉತ್ಥಾಪಯ(sigarg);
    }
    ಅನ್ಯಥಾ
	got_tstp = TRUE;

# ಯದಿ !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#ಯದಿ defined(SIGINT)
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#ಯದಿ defined(SIGUSR1)
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#ಯದಿ defined(SIGPWR)
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_sigpwr SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGPWR, catch_sigpwr);
    /*
     * I'm not sure we get the SIGPWR signal when the system is really going
     * down or when the batteries are almost empty.  Just preserve the swap
     * files and don't exit, that can't do any harm.
     */
    ml_sync_all(FALSE, FALSE);
}
#endif

#ifdef SET_SIG_ALARM
/*
 * signal function for alarm().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#ಯದಿ defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ಸ್ಥಿರ JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t lc_active = FALSE;

/*
 * A simplistic version of setjmp() that only allows one level of using.
 * Used to protect areas where we could crash.
 * Don't call twice before calling mch_endjmp()!.
 *
 * Usage:
 *	mch_startjmp();
 *	if (SETJMP(lc_jump_env) != 0)
 *	{
 *	    mch_didjmp();
 *	    emsg("crash!");
 *	}
 *	else
 *	{
 *	    do_the_work;
 *	    mch_endjmp();
 *	}
 * Note: Can't move SETJMP() here, because a function calling setjmp() must
 * not return before the saved environment is used.
 * Returns OK for normal return, FAIL when the protected code caused a
 * problem and LONGJMP() was used.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_startjmp(ಶೂನ್ಯ)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mch_endjmp(ಶೂನ್ಯ)
{
    lc_active = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mch_didjmp(ಶೂನ್ಯ)
{
# ಯದಿ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ರಾಶಿ();
# endif
}
#endif

/*
 * This function handles deadly signals.
 * It tries to preserve any swap files and exit properly.
 * (partly from Elvis).
 * NOTE: Avoid unsafe functions, such as allocating memory, they can result in
 * a deadlock.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
deathtrap SIGDEFARG(sigarg)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    ಪೂರ್ಣಾಙ್ಕ		i;
#endif

#ಯದಿ defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    ಯದಿ (lc_active)
    {
# ಯದಿ defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_active = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#endif

#ifdef SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    ಯದಿ (in_mch_delay && sigarg == SIGQUIT)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    ಯದಿ (entered == 0
	    && (0
# ifdef SIGHUP
		|| sigarg == SIGHUP
# endif
# ifdef SIGQUIT
		|| sigarg == SIGQUIT
# endif
# ifdef SIGTERM
		|| sigarg == SIGTERM
# endif
# ifdef SIGPWR
		|| sigarg == SIGPWR
# endif
# ifdef SIGUSR1
		|| sigarg == SIGUSR1
# endif
# ifdef SIGUSR2
		|| sigarg == SIGUSR2
# endif
		)
	    && !vim_handle_signal(sigarg))
	ಪ್ರತ್ಯಾಗಮ;
#endif

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    block_autocmds();

#ifdef FEAT_EVAL
    // Set the v:dying variable.
    set_vim_var_nr(VV_DYING, (long)entered);
#endif
    v_dying = entered;

#ifdef HAVE_ರಾಶಿ_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ರಾಶಿ_limit();
#endif

#ಯದಿ 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	ಯದಿ (fp)
	{
	    fprintf(fp,
		    "file %s\n"
		    "attach %d\n"
		    "set height 1000\n"
		    "bt full\n"
		    , VIM_NAME, getpid());
	    fclose(fp);
	    system("xterm -e gdb -x "VI_GDB_FILE);
	    unlink(VI_GDB_FILE);
	}
    }
#endif

#ifdef SIGHASARG
    // try to find the name of this signal
    ಪ್ರತಿ (i = 0; signal_info[i].sig != -1; i++)
	ಯದಿ (sigarg == signal_info[i].sig)
	    ಭಙ್ಗ;
    deadly_signal = sigarg;
#endif

    full_screen = FALSE;	// don't write message to the GUI, it might be
				// part of the problem...
    /*
     * If something goes wrong after entering here, we may get here again.
     * When this happens, give a message and try to exit nicely (resetting the
     * terminal mode, etc.)
     * When this happens twice, just exit, don't even try to give a message,
     * stack may be corrupt or something weird.
     * When this still happens again (or memory was corrupted in such a way
     * that "entered" was clobbered) use _exit(), don't try freeing resources.
     */
    ಯದಿ (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	ಯದಿ (entered >= 4)
	    _exit(8);
	exit(7);
    }
    ಯದಿ (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((ವರ್ಣ *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#ಅನ್ಯಥಾ
    sprintf((ವರ್ಣ *)IObuff, "Vim: Caught deadly signal\r\n");
#endif

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#ifdef NBDEBUG
    reset_signals();
    may_core_dump();
    abort();
#endif
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
after_sigcont(ಶೂನ್ಯ)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#ಯದಿ defined(SIGCONT)
ಸ್ಥಿರ ಶೂನ್ಯ sigcont_handler SIGPROTOARG;

/*
 * With multi-threading, suspending might not work immediately.  Catch the
 * SIGCONT signal, which will be used as an indication whether the suspending
 * has been done or not.
 *
 * On Linux, signal is not always handled immediately either.
 * See https://bugs.launchpad.net/bugs/291373
 * Probably because the signal is handled in another thread.
 *
 * volatile because it is used in signal handler sigcont_handler().
 */
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t sigcont_received;
ಸ್ಥಿರ ಶೂನ್ಯ sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sigcont_handler SIGDEFARG(sigarg)
{
    ಯದಿ (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    ಅನ್ಯಥಾ
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	out_flush();
    }
}
#endif

#ಯದಿ defined(FEAT_CLIPBOARD)
# ಯದಿ defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ಸ್ಥಿರ ಶೂನ್ಯ *clip_star_save = ರಿಕ್ತ;
ಸ್ಥಿರ ಶೂನ್ಯ *clip_plus_save = ರಿಕ್ತ;
# endif

# ಯದಿ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
loose_clipboard(ಶೂನ್ಯ)
{
    ಯದಿ (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	ಯದಿ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ಯದಿ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	ಯದಿ (x11_display != ರಿಕ್ತ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# ಯದಿ defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_clipboard(ಶೂನ್ಯ)
{
    ಯದಿ (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    ಯದಿ (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_clipboard(ಶೂನ್ಯ)
{
    ಯದಿ (clip_star_save != ರಿಕ್ತ)
    {
	ಯದಿ (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	ಅನ್ಯಥಾ
	    free_register(clip_star_save);
	clip_star_save = ರಿಕ್ತ;
    }
    ಯದಿ (clip_plus_save != ರಿಕ್ತ)
    {
	ಯದಿ (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	ಅನ್ಯಥಾ
	    free_register(clip_plus_save);
	clip_plus_save = ರಿಕ್ತ;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    ಯದಿ (ignore_sigtstp)
	ಪ್ರತ್ಯಾಗಮ;

#ಯದಿ defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# ಯದಿ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# ಯದಿ defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# ಯದಿ defined(SIGCONT)
    /*
     * Wait for the SIGCONT signal to be handled. It generally happens
     * immediately, but somehow not all the time, probably because it's handled
     * in another thread. Do not call pause() because there would be race
     * condition which would hang Vim if signal happened in between the test of
     * sigcont_received and the call to pause(). If signal is not yet received,
     * sleep 0, 1, 2, 3 ms. Don't bother waiting further if signal is not
     * received after 1+2+3 ms (not expected to happen).
     */
    {
	long wait_time;

	ಪ್ರತಿ (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#ಅನ್ಯಥಾ
    suspend_shell();
#endif
}

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
    Columns = 80;
    Rows = 24;

    out_flush();

#ifdef SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#endif
    set_signals();

#ifdef MACOS_CONVERT
    mac_conv_init();
#endif
#ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#endif
}

    ಶೂನ್ಯ
set_sigwinch_handler(ಶೂನ್ಯ)
{
#ಯದಿ defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_signals(ಶೂನ್ಯ)
{
#ಯದಿ defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#endif

#ifdef SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.in_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#endif
#ಯದಿ defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#endif
#ifdef SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#endif

#ifdef SIGINT
    catch_int_signal();
#endif

#ifdef SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#endif

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#ifdef SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#endif

#ifdef SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#endif

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_signals(deathtrap, SIG_ERR);

#ಯದಿ defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    ಯದಿ (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#ಯದಿ defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_int_signal(ಶೂನ್ಯ)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    ಶೂನ್ಯ
reset_signals(ಶೂನ್ಯ)
{
    catch_signals(SIG_DFL, SIG_DFL);
#ಯದಿ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
catch_signals(
    ಶೂನ್ಯ (*func_deadly)(ಪೂರ್ಣಾಙ್ಕ),
    ಶೂನ್ಯ (*func_other)(ಪೂರ್ಣಾಙ್ಕ))
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; signal_info[i].sig != -1; i++)
    {
	ಯದಿ (signal_info[i].deadly)
	{
#ಯದಿ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ಸಂರಚನಾ sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# ಯದಿ defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# ಅನ್ಯಥಾ
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ರಿಕ್ತ);
#ಅನ್ಯಥಾ
# ಯದಿ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ಸಂರಚನಾ sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ರಿಕ್ತ);
# ಅನ್ಯಥಾ
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	ಅನ್ಯಥಾ ಯದಿ (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#ಅನ್ಯಥಾ
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    ಸ್ಥಿರ ಶೂನ್ಯ
block_signals(sigset_t *set)
{
    sigset_t	newset;
    ಪೂರ್ಣಾಙ್ಕ		i;

    sigemptyset(&newset);

    ಪ್ರತಿ (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# ಯದಿ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, ರಿಕ್ತ);
}
#endif

/*
 * Handling of SIGHUP, SIGQUIT and SIGTERM:
 * "when" == a signal:       when busy, postpone and return FALSE, otherwise
 *			     return TRUE
 * "when" == SIGNAL_BLOCK:   Going to be busy, block signals
 * "when" == SIGNAL_UNBLOCK: Going to wait, unblock signals, use postponed
 *			     signal
 * Returns TRUE when Vim should exit.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_handle_signal(ಪೂರ್ಣಾಙ್ಕ sig)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ got_signal = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ blocked = TRUE;

    ವಿಕಲ್ಪ (sig)
    {
	ಪಕ್ಷ SIGNAL_BLOCK:   blocked = TRUE;
			     ಭಙ್ಗ;

	ಪಕ್ಷ SIGNAL_UNBLOCK: blocked = FALSE;
			     ಯದಿ (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ಭಙ್ಗ;

	ಸ್ವಭಾವ:	     ಯದಿ (!blocked)
				 ಪ್ರತ್ಯಾಗಮ TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     ಯದಿ (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_check_win(ಪೂರ್ಣಾಙ್ಕ argc UNUSED, ವರ್ಣ **argv UNUSED)
{
    ಯದಿ (isatty(1))
	ಪ್ರತ್ಯಾಗಮ OK;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
    ಯದಿ (isatty(read_cmd_fd))
	ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ifdef FEAT_X11

# ಯದಿ defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#ಯದಿ defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (ವರ್ಣ *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
# endif
	ಸಂದೇಶ((ವರ್ಣ *)IObuff);

    ಪ್ರತ್ಯಾಗಮ 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_connect_to_server(ಶೂನ್ಯ)
{
    // No point in connecting if we are exiting or dying.
    ಯದಿ (exiting || v_dying)
	ಪ್ರತ್ಯಾಗಮ FALSE;

# ಯದಿ defined(FEAT_CLIENTSERVER)
    ಯದಿ (x_force_connect)
	ಪ್ರತ್ಯಾಗಮ TRUE;
# endif
    ಯದಿ (x_no_connect)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    ಯದಿ (clip_exclude_prog != ರಿಕ್ತ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	ಯದಿ (regprog_in_use(clip_exclude_prog))
	    ಪ್ರತ್ಯಾಗಮ FALSE;

	ಯದಿ (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    ಪ್ರತ್ಯಾಗಮ FALSE;
    }
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ಯದಿ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  ಯದಿ defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   ಯದಿ defined(VMS) || defined(__CYGWIN__)
    ಪ್ರತ್ಯಾಗಮ 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ xterm_dpy_retry_ಎಣಿಕೆ = 0;

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = ರಿಕ್ತ;
    xterm_dpy_retry_ಎಣಿಕೆ = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = ರಿಕ್ತ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  ಯದಿ defined(VMS) || defined(__CYGWIN__)
    ಪ್ರತ್ಯಾಗಮ 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_restore_x11_clipboard(ಶೂನ್ಯ)
{
    // No point in restoring the connecting if we are exiting or dying.
    ಯದಿ (!exiting && !v_dying && xterm_dpy_retry_ಎಣಿಕೆ > 0)
    {
	--xterm_dpy_retry_ಎಣಿಕೆ;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	ಯದಿ (app_context != (XtAppContext)ರಿಕ್ತ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ರಿಕ್ತ;
	    x11_display = ರಿಕ್ತ; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    ಶೂನ್ಯ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    ಯದಿ (eap->arg != ರಿಕ್ತ && (arglen = STRLEN(eap->arg)) > 0)
    {
	ಯದಿ (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (ವರ್ಣ *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ರಿಕ್ತ
		    ? (ವರ್ಣ *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_ಎಣಿಕೆ = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    choose_clipmethod();
}
# endif

/*
 * Test if "dpy" and x11_window are valid by getting the window title.
 * I don't actually want it yet, so there may be a simpler call to use, but
 * this will cause the error handler x_error_check() to be called if anything
 * is wrong, such as the window pointer being invalid (as can happen when the
 * user changes his DISPLAY, but not his WINDOWID) -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
test_x11_window(Display *dpy)
{
    ಪೂರ್ಣಾಙ್ಕ			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    ಯದಿ (XGetWMName(dpy, x11_window, &text_prop))
	XFree((ಶೂನ್ಯ *)text_prop.ಮೌಲ್ಯ);
    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಯದಿ (p_verbose > 0 && got_x_error)
	verb_ಸಂದೇಶ(_("Testing the X display failed"));

    ಪ್ರತ್ಯಾಗಮ (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ get_x11_thing(ಪೂರ್ಣಾಙ್ಕ get_title, ಪೂರ್ಣಾಙ್ಕ test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_windis(ಶೂನ್ಯ)
{
    ವರ್ಣ	    *winid;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    x11_display_from = XD_NONE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	    did_set_error_handler = FALSE;

    ಯದಿ (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(ಶೂನ್ಯ)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# ಯದಿ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    ಯದಿ (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	ಯದಿ (x11_display_from == XD_HERE && x11_display != ರಿಕ್ತ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	ಯದಿ (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    ಪ್ರತ್ಯಾಗಮ OK;
	}
	x11_display = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ ಯದಿ (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = ರಿಕ್ತ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    ಯದಿ (!x_connect_to_server())
	ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    ಯದಿ (x11_window == 0 && (winid = getenv("WINDOWID")) != ರಿಕ್ತ)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    ಯದಿ (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    ಯದಿ (xterm_dpy != ರಿಕ್ತ && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	ಯದಿ (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    ಯದಿ (x11_display_from == XD_HERE && x11_display != ರಿಕ್ತ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	ಯದಿ (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = ರಿಕ್ತ;
	    x11_display_from = XD_NONE;
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	}
	ಪ್ರತ್ಯಾಗಮ OK;
    }
# endif

    ಯದಿ (x11_window == 0 || x11_display == ರಿಕ್ತ)
	result = -1;

    ಯದಿ (result != -1)	    // Have already been here and set this
	ಪ್ರತ್ಯಾಗಮ result;	    // Don't do all these X calls again

    ಯದಿ (x11_window != 0 && x11_display == ರಿಕ್ತ)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	ಯದಿ (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

# ifdef SET_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(ರಿಕ್ತ);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	ಯದಿ (p_verbose > 0 && sig_alarm_called)
	    verb_ಸಂದೇಶ(_("Opening the X display timed out"));
# endif
	ಯದಿ (x11_display != ರಿಕ್ತ)
	{
# ifdef ELAPSED_FUNC
	    ಯದಿ (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    ಯದಿ (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = ರಿಕ್ತ;
	    }
	    ಅನ್ಯಥಾ
		x11_display_from = XD_HERE;
	}
    }
    ಯದಿ (x11_window == 0 || x11_display == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    ಪ್ರತ್ಯಾಗಮ (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_title(ಪೂರ್ಣಾಙ್ಕ test_only)
{
    ಪ್ರತ್ಯಾಗಮ get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_icon(ಪೂರ್ಣಾಙ್ಕ test_only)
{
    ಪೂರ್ಣಾಙ್ಕ		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    ಯದಿ (oldicon == ರಿಕ್ತ && !test_only)
    {
	ಯದಿ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ಅನ್ಯಥಾ
	    oldicon = vim_strsave(T_NAME);
    }

    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_thing(
    ಪೂರ್ಣಾಙ್ಕ		get_title,	// get title string
    ಪೂರ್ಣಾಙ್ಕ		test_only)
{
    XTextProperty	text_prop;
    ಪೂರ್ಣಾಙ್ಕ			retval = FALSE;
    Status		status;

    ಯದಿ (get_x11_windis() != OK)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    // Get window/icon name if any
    ಯದಿ (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    ಅನ್ಯಥಾ
	status = XGetWMIconName(x11_display, x11_window, &text_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    Window	    ಬೇರು;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned ಪೂರ್ಣಾಙ್ಕ    ಅಂಕೆ_children;

    ಯಾವತ್ (!status || text_prop.ಮೌಲ್ಯ == ರಿಕ್ತ)
    {
	ಯದಿ (!XQuery_ವೃಕ್ಷ(x11_display, win, &ಬೇರು, &parent, &children,
		    &ಅಂಕೆ_children))
	    ಭಙ್ಗ;
	ಯದಿ (children)
	    XFree((ಶೂನ್ಯ *)children);
	ಯದಿ (parent == ಬೇರು || parent == 0)
	    ಭಙ್ಗ;

	win = parent;
	ಯದಿ (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	ಅನ್ಯಥಾ
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    ಯದಿ (status && text_prop.ಮೌಲ್ಯ != ರಿಕ್ತ)
    {
	retval = TRUE;
	ಯದಿ (!test_only)
	{
	    ಯದಿ (get_title)
		vim_free(oldtitle);
	    ಅನ್ಯಥಾ
		vim_free(oldicon);
	    ಯದಿ (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		ಯದಿ (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.ಮೌಲ್ಯ);
		ಅನ್ಯಥಾ
		    oldicon = vim_strsave((char_u *)text_prop.ಮೌಲ್ಯ);
	    }
	    ಅನ್ಯಥಾ
	    {
		ವರ್ಣ    **cl;
		Status  transform_status;
		ಪೂರ್ಣಾಙ್ಕ	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		ಯದಿ (transform_status >= Success && n > 0 && cl[0])
		{
		    ಯದಿ (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    ಅನ್ಯಥಾ
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		ಅನ್ಯಥಾ
		{
		    ಯದಿ (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.ಮೌಲ್ಯ);
		    ಅನ್ಯಥಾ
			oldicon = vim_strsave((char_u *)text_prop.ಮೌಲ್ಯ);
		}
	    }
	}
	XFree((ಶೂನ್ಯ *)text_prop.ಮೌಲ್ಯ);
    }
    ಪ್ರತ್ಯಾಗಮ retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# ಯದಿ defined(X_HAVE_UTF8_STRING)
#  ಯದಿ X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    ಯದಿ (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ಧ್ರುವ ವರ್ಣ *)title,
					     ರಿಕ್ತ, ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
# endif
    {
# ಯದಿ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ಧ್ರುವ ವರ್ಣ *)title,
					     ರಿಕ್ತ, ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#  ಅನ್ಯಥಾ
	XTextProperty	text_prop;
	ವರ್ಣ		*c_title = (ವರ್ಣ *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(ಶೂನ್ಯ)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     ರಿಕ್ತ, ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#  endif
# ಅನ್ಯಥಾ
	XStoreName(x11_display, x11_window, (ವರ್ಣ *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    ಯದಿ (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)icon,
						   ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
# endif
    {
# ಯದಿ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, ರಿಕ್ತ, (ಧ್ರುವ ವರ್ಣ *)icon,
						   ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#  ಅನ್ಯಥಾ
	XTextProperty	text_prop;
	ವರ್ಣ		*c_icon = (ವರ್ಣ *)icon;

	(ಶೂನ್ಯ)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, ರಿಕ್ತ, &text_prop,
						   ರಿಕ್ತ, 0, ರಿಕ್ತ, ರಿಕ್ತ, ರಿಕ್ತ);
#  endif
# ಅನ್ಯಥಾ
	XSetIconName(x11_display, x11_window, (ವರ್ಣ *)icon);
# endif
    }
    XFlush(x11_display);
}

#ಅನ್ಯಥಾ  // FEAT_X11

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_title(ಪೂರ್ಣಾಙ್ಕ test_only UNUSED)
{
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_x11_icon(ಪೂರ್ಣಾಙ್ಕ test_only)
{
    ಯದಿ (!test_only)
    {
	ಯದಿ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ಅನ್ಯಥಾ
	    oldicon = vim_strsave(T_NAME);
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#endif // FEAT_X11

    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ get_x11_title(TRUE);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    ಶೂನ್ಯ
mch_settitle(char_u *title, char_u *icon)
{
    ಪೂರ್ಣಾಙ್ಕ		type = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	recursive = 0;

    ಯದಿ (T_NAME == ರಿಕ್ತ)	    // no terminal name (yet)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (title == ರಿಕ್ತ && icon == ರಿಕ್ತ)	    // nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    ಯದಿ (recursive)
	ಪ್ರತ್ಯಾಗಮ;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    ಯದಿ (get_x11_windis() == OK)
	type = 1;
#endif
#ಯದಿ defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    ಯದಿ (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    ಯದಿ ((type || *T_TS != NUL) && title != ರಿಕ್ತ)
    {
	ಯದಿ (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	ಯದಿ (oldtitle == ರಿಕ್ತ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (ಶೂನ್ಯ)get_x11_title(FALSE);

	ಯದಿ (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	ಅನ್ಯಥಾ
# ifdef FEAT_GUI_GTK
	ಯದಿ (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#ಯದಿ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	ಅನ್ಯಥಾ
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    ಯದಿ ((type || *T_CIS != NUL) && icon != ರಿಕ್ತ)
    {
	ಯದಿ (oldicon == ರಿಕ್ತ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	ಯದಿ (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	ಅನ್ಯಥಾ
# ifdef FEAT_GUI_GTK
	ಯದಿ (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_icon(icon);			// x11
#endif
	did_set_icon = TRUE;
    }
    --recursive;
}

/*
 * Restore the window/icon title.
 * "which" is one of:
 *  SAVE_RESTORE_TITLE only restore title
 *  SAVE_RESTORE_ICON  only restore icon
 *  SAVE_RESTORE_BOTH  restore title and icon
 */
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಙ್ಕ which)
{
    ಪೂರ್ಣಾಙ್ಕ	do_ತಳ್ಳು_ತೆಗೆ = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : ರಿಕ್ತ;
    char_u *tofree = ರಿಕ್ತ;
    ಯದಿ (title == oldtitle && oldtitle != ರಿಕ್ತ)
    {
	tofree = vim_strsave(title);
	ಯದಿ (tofree != ರಿಕ್ತ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : ರಿಕ್ತ);
    vim_free(tofree);

    ಯದಿ (do_ತಳ್ಳು_ತೆಗೆ)
    {
	// pop and push from/to the stack
	term_ತೆಗೆ_title(which);
	term_ತಳ್ಳು_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_is_xterm(char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#ಯದಿ defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    ಪೂರ್ಣಾಙ್ಕ
use_xterm_like_mouse(char_u *name)
{
    ಪ್ರತ್ಯಾಗಮ (name != ರಿಕ್ತ
	    && (term_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#endif

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 * Return 3 for "urxvt".
 * Return 4 for "sgr".
 */
    ಪೂರ್ಣಾಙ್ಕ
use_xterm_mouse(ಶೂನ್ಯ)
{
    ಯದಿ (ttym_flags == TTYM_SGR)
	ಪ್ರತ್ಯಾಗಮ 4;
    ಯದಿ (ttym_flags == TTYM_URXVT)
	ಪ್ರತ್ಯಾಗಮ 3;
    ಯದಿ (ttym_flags == TTYM_XTERM2)
	ಪ್ರತ್ಯಾಗಮ 2;
    ಯದಿ (ttym_flags == TTYM_XTERM)
	ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_is_iris(char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#ಯದಿ defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    ಪೂರ್ಣಾಙ್ಕ
vim_is_vt300(char_u *name)
{
    ಯದಿ (name == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    ಪ್ರತ್ಯಾಗಮ ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ರಿಕ್ತ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_user_name(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(ರಿಕ್ತ), len - 1);
    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_uname(uid_t uid, char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
#ಯದಿ defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ಸಂರಚನಾ passwd   *pw;

    ಯದಿ ((pw = getpwuid(uid)) != ರಿಕ್ತ
	    && pw->pw_name != ರಿಕ್ತ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	ಪ್ರತ್ಯಾಗಮ OK;
    }
#endif
    sprintf((ವರ್ಣ *)s, "%d", (ಪೂರ್ಣಾಙ್ಕ)uid);	    // assumes s is long enough
    ಪ್ರತ್ಯಾಗಮ FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಸಂರಚನಾ utsname vutsname;

    ಯದಿ (uname(&vutsname) < 0)
	*s = NUL;
    ಅನ್ಯಥಾ
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#ಅನ್ಯಥಾ // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
# ifdef VAXC
    vaxc$gethostname((ವರ್ಣ *)s, len);
# ಅನ್ಯಥಾ
    gethostname((ವರ್ಣ *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    ಯದಿ (kill(pid, 0) == 0)
	ಪ್ರತ್ಯಾಗಮ TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    ಯದಿ (errno == ESRCH)
	ಪ್ರತ್ಯಾಗಮ FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

#ಯದಿ !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ಸ್ಥಿರ ವರ್ಣ *
strerror(ಪೂರ್ಣಾಙ್ಕ err)
{
    ಬಾಹ್ಯ ಪೂರ್ಣಾಙ್ಕ	    sys_nerr;
    ಬಾಹ್ಯ ವರ್ಣ	    *sys_errlist[];
    ಸ್ಥಿರ ವರ್ಣ	    er[20];

    ಯದಿ (err > 0 && err < sys_nerr)
	ಪ್ರತ್ಯಾಗಮ (sys_errlist[err]);
    snprintf(er, ಪರಿಮಾಣ(er), "Error %d", err);
    ಪ್ರತ್ಯಾಗಮ er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_dirname(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
#ಯದಿ defined(USE_GETCWD)
    ಯದಿ (getcwd((ವರ್ಣ *)buf, len) == ರಿಕ್ತ)
    {
	STRCPY(buf, strerror(errno));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (getwd((ವರ್ಣ *)buf) != ರಿಕ್ತ ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		force)		// also expand when already absolute path
{
    ಪೂರ್ಣಾಙ್ಕ		buflen = 0;
#ifdef HAVE_FCHDIR
    ಪೂರ್ಣಾಙ್ಕ		fd = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		retval = OK;
#ifdef __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#endif

#ifdef VMS
    fname = vms_fixfilename(fname);
#endif

#ifdef __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# ಯದಿ CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# ಅನ್ಯಥಾ
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    ಯದಿ ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == ರಿಕ್ತ || p != fname))
    {
	ಯದಿ (p == ರಿಕ್ತ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಯದಿ (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    ಯದಿ (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		ಯದಿ (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    ಯದಿ (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((ವರ್ಣ *)olddir) != 0))
	    {
		p = ರಿಕ್ತ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    ಅನ್ಯಥಾ
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		ಯದಿ (p - fname >= len)
		    retval = FAIL;
		ಅನ್ಯಥಾ
		{
		    vim_strncpy(buf, fname, p - fname);
		    ಯದಿ (mch_chdir((ವರ್ಣ *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			ಯದಿ (mch_isFullName(fname))
			    retval = FAIL;
			ಅನ್ಯಥಾ
			    p = ರಿಕ್ತ;
		    }
		    ಅನ್ಯಥಾ ಯದಿ (*p == '/')
			fname = p + 1;
		    ಅನ್ಯಥಾ
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	ಯದಿ (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	ಯದಿ (p != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ	l;

#ifdef HAVE_FCHDIR
	    ಯದಿ (fd >= 0)
	    {
		ಯದಿ (p_verbose >= 5)
		{
		    verbose_enter();
		    ಸಂದೇಶ("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    ಅನ್ಯಥಾ
#endif
		l = mch_chdir((ವರ್ಣ *)olddir);
	    ಯದಿ (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	ಯದಿ (fd >= 0)
	    close(fd);
#endif

	buflen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf);
	ಯದಿ (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	ಅನ್ಯಥಾ ಯದಿ (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += ಪರಿಮಾಣ(PATHSEP);
	}
#endif
    }

    ಯದಿ (buflen == 0)
	buflen = (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buf);

    // Catch file names which are too long.
    ಯದಿ (retval == FAIL || (ಪೂರ್ಣಾಙ್ಕ)(buflen + STRLEN(fname)) >= len)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    ಯದಿ (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isFullName(char_u *fname)
{
#ifdef VMS
    ಪ್ರತ್ಯಾಗಮ ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((ವರ್ಣ *)fname,':') || strchr((ವರ್ಣ *)fname,'"') ||
	    (strchr((ವರ್ಣ *)fname,'[') && strchr((ವರ್ಣ *)fname,']'))||
	    (strchr((ವರ್ಣ *)fname,'<') && strchr((ವರ್ಣ *)fname,'>'))   );
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (*fname == '/' || *fname == '~');
#endif
}

#ಯದಿ defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		len UNUSED)  // buffer size, only used when name gets longer
{
    ಸಂರಚನಾ stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    ಸಂರಚನಾ dirent *dp;

    ಯದಿ (mch_lstat((ವರ್ಣ *)name, &st) < 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    ಯದಿ (slash == ರಿಕ್ತ)
    {
	dirp = opendir(".");
	tail = name;
    }
    ಅನ್ಯಥಾ
    {
	*slash = NUL;
	dirp = opendir((ವರ್ಣ *)name);
	*slash = '/';
	tail = slash + 1;
    }

    ಯದಿ (dirp == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    taillen = STRLEN(tail);
    ಯಾವತ್ ((dp = readdir(dirp)) != ರಿಕ್ತ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	ಯದಿ (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ಸಂರಚನಾ stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    ಯದಿ (mch_lstat((ವರ್ಣ *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		ಭಙ್ಗ;
	    }
	}
    }

    closedir(dirp);
}
#endif

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    ಸಂರಚನಾ stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    ಯದಿ (stat((ವರ್ಣ *)vms_fixfilename(name), &statb))
#ಅನ್ಯಥಾ
    ಯದಿ (stat((ವರ್ಣ *)name, &statb))
#endif
	ಪ್ರತ್ಯಾಗಮ -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    ಪ್ರತ್ಯಾಗಮ statb.st_mode & ~S_ADDACE;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_setperm(char_u *name, long perm)
{
    ಪ್ರತ್ಯಾಗಮ (chmod((ವರ್ಣ *)
#ifdef VMS
		    vms_fixfilename(name),
#ಅನ್ಯಥಾ
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#ಯದಿ defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_fsetperm(ಪೂರ್ಣಾಙ್ಕ fd, long perm)
{
    ಪ್ರತ್ಯಾಗಮ (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#ಯದಿ defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
ಪ್ರಕಾರನಿರ್ದೇಶ ಸಂರಚನಾ vim_acl_solaris_T {
    ಪೂರ್ಣಾಙ್ಕ acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# ಯದಿ defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ಶೂನ್ಯ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ಯದಿ (from_file == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    ಯದಿ (selinux_enabled <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    ವರ್ಣ *from_context = ರಿಕ್ತ;
    ವರ್ಣ *to_context = ರಿಕ್ತ;

    ಯದಿ (getfilecon((ವರ್ಣ *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	ಯದಿ (errno == EOPNOTSUPP)
	    ಪ್ರತ್ಯಾಗಮ;

	ಸಂದೇಶ_puts(_("\nCould not get security context for "));
	ಸಂದೇಶ_outtrans(from_file);
	ಸಂದೇಶ_putchar('\n');
	ಪ್ರತ್ಯಾಗಮ;
    }
    ಯದಿ (getfilecon((ವರ್ಣ *)to_file, &to_context) < 0)
    {
	ಸಂದೇಶ_puts(_("\nCould not get security context for "));
	ಸಂದೇಶ_outtrans(to_file);
	ಸಂದೇಶ_putchar('\n');
	freecon (from_context);
	ಪ್ರತ್ಯಾಗಮ ;
    }
    ಯದಿ (strcmp(from_context, to_context) != 0)
    {
	ಯದಿ (setfilecon((ವರ್ಣ *)to_file, from_context) < 0)
	{
	    ಸಂದೇಶ_puts(_("\nCould not set security context for "));
	    ಸಂದೇಶ_outtrans(to_file);
	    ಸಂದೇಶ_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# ಯದಿ defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ಶೂನ್ಯ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ * ಧ್ರುವ smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    ವರ್ಣ	buffer[SMACK_LABEL_LEN];
    ಧ್ರುವ ವರ್ಣ	*name;
    ಪೂರ್ಣಾಙ್ಕ		index;
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ssize_t	size;

    ಯದಿ (from_file == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    size = listxattr((ವರ್ಣ *)from_file, ರಿಕ್ತ, 0);
    // not supported or no attributes to copy
    ಯದಿ (size <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಪ್ರತಿ (index = 0 ; index < (ಪೂರ್ಣಾಙ್ಕ)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((ವರ್ಣ*)from_file, name, buffer, ಪರಿಮಾಣ(buffer));
	ಯದಿ (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((ವರ್ಣ*)to_file, name, buffer, (size_t)size, 0);
	    ಯದಿ (ret < 0)
	    {
		vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		ಸಂದೇಶ_outtrans(IObuff);
		ಸಂದೇಶ_putchar('\n');
	    }
	}
	ಅನ್ಯಥಾ
	{
	    // what reason of not having the attribute value?
	    ವಿಕಲ್ಪ (errno)
	    {
		ಪಕ್ಷ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    ಪ್ರತ್ಯಾಗಮ; // leave because it isn't useful to continue

		ಪಕ್ಷ ERANGE:
		ಸ್ವಭಾವ:
		    // no enough size OR unexpected error
		     vim_snprintf((ವರ್ಣ *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    ಸಂದೇಶ_puts((ವರ್ಣ *)IObuff);
		    ಸಂದೇಶ_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		ಪಕ್ಷ ENODATA:
		    // no attribute of this name
		    ret = removexattr((ವರ್ಣ*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ಭಙ್ಗ;
	    }
	}
    }
}
# endif // HAVE_SMACK

# ಯದಿ defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    ಶೂನ್ಯ
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    ವರ್ಣ	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    ವರ್ಣ	*ಕೀಲಿ;
    ವರ್ಣ	*val = ರಿಕ್ತ;
    ವರ್ಣ	*errmsg = ರಿಕ್ತ;

    ಯದಿ (from_file == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    // get the length of the extended attributes
    size = listxattr((ವರ್ಣ *)from_file, ರಿಕ್ತ, 0);
    // not supported or no attributes to copy
    ಯದಿ (size <= 0)
	ಪ್ರತ್ಯಾಗಮ;
    xattr_buf = (ವರ್ಣ*)alloc(size);
    ಯದಿ (xattr_buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
    size = listxattr((ವರ್ಣ *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    ಪ್ರತಿ (ಪೂರ್ಣಾಙ್ಕ round = 0; round < 2; round++)
    {

	ಕೀಲಿ = xattr_buf;
	ಯದಿ (round == 1)
	    size = tsize;

	ಯಾವತ್ (size > 0)
	{
	    vallen = getxattr((ವರ್ಣ *)from_file, ಕೀಲಿ,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    ಯದಿ (vallen >= 0 && round &&
		setxattr((ವರ್ಣ *)to_file, ಕೀಲಿ, val, vallen, 0) == 0)
		;
	    ಅನ್ಯಥಾ ಯದಿ (errno)
	    {
		ವಿಕಲ್ಪ (errno)
		{
		    ಪಕ್ಷ E2BIG:
			errmsg = e_xattr_e2big;
			ಗಚ್ಛ error_exit;
		    ಪಕ್ಷ ENOTSUP:
		    ಪಕ್ಷ EACCES:
		    ಪಕ್ಷ EPERM:
			ಭಙ್ಗ;
		    ಪಕ್ಷ ERANGE:
			errmsg = e_xattr_erange;
			ಗಚ್ಛ error_exit;
		    ಸ್ವಭಾವ:
			errmsg = e_xattr_other;
			ಗಚ್ಛ error_exit;
		}
	    }

	    ಯದಿ (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ಕೀಲಿ) + 1;
	    size -= keylen;
	    ಕೀಲಿ += keylen;
	}
	ಯದಿ (round)
	    ಭಙ್ಗ;

	val = (ವರ್ಣ*)alloc(max_vallen + 1);
	ಯದಿ (val == ರಿಕ್ತ)
	    ಗಚ್ಛ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    ಯದಿ (errmsg != ರಿಕ್ತ)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname UNUSED)
{
    vim_acl_T	ret = ರಿಕ್ತ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((ವರ್ಣ *)fname, ACL_TYPE_ACCESS);
# ಅನ್ಯಥಾ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    ಯದಿ (acl_get((ವರ್ಣ *)fname, 0, &aclent) < 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ret = (vim_acl_T)aclent;
#  ಅನ್ಯಥಾ
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(ಪರಿಮಾಣ(vim_acl_solaris_T));
    ಯದಿ (aclent == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ ((aclent->acl_cnt = acl((ವರ್ಣ *)fname, GETACLCNT, 0, ರಿಕ್ತ)) < 0)
    {
	ಮುಕ್ತ(aclent);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * ಪರಿಮಾಣ(aclent_t));
    ಯದಿ (aclent->acl_entry == ರಿಕ್ತ)
    {
	ಮುಕ್ತ(aclent);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ಯದಿ (acl((ವರ್ಣ *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	ಮುಕ್ತ(aclent->acl_entry);
	ಮುಕ್ತ(aclent);
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }
    ret = (vim_acl_T)aclent;
#   ಅನ್ಯಥಾ
#    ಯದಿ defined(HAVE_AIX_ACL)
    ಪೂರ್ಣಾಙ್ಕ		aclsize;
    ಸಂರಚನಾ acl *aclent;

    aclsize = ಪರಿಮಾಣ(ಸಂರಚನಾ acl);
    aclent = malloc(aclsize);
    ಯದಿ (aclent == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಯದಿ (statacl((ವರ್ಣ *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	ಯದಿ (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    ಯದಿ (aclent == ರಿಕ್ತ || statacl((ವರ್ಣ *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		ಮುಕ್ತ(aclent);
		ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಮುಕ್ತ(aclent);
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    ಪ್ರತ್ಯಾಗಮ ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    ಶೂನ್ಯ
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    ಯದಿ (aclent == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
# ifdef HAVE_POSIX_ACL
    acl_set_file((ವರ್ಣ *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# ಅನ್ಯಥಾ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((ವರ್ಣ *)fname, (acl_t *)aclent);
#  ಅನ್ಯಥಾ
#   ifdef HAVE_SOLARIS_ACL
    acl((ವರ್ಣ *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   ಅನ್ಯಥಾ
#    ifdef HAVE_AIX_ACL
    chacl((ವರ್ಣ *)fname, aclent, ((ಸಂರಚನಾ acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    ಶೂನ್ಯ
mch_free_acl(vim_acl_T aclent)
{
    ಯದಿ (aclent == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# ಅನ್ಯಥಾ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  ಅನ್ಯಥಾ
#   ifdef HAVE_SOLARIS_ACL
    ಮುಕ್ತ(((vim_acl_solaris_T *)aclent)->acl_entry);
    ಮುಕ್ತ(aclent);
#   ಅನ್ಯಥಾ
#    ifdef HAVE_AIX_ACL
    ಮುಕ್ತ(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isdir(char_u *name)
{
    ಸಂರಚನಾ stat statb;

    ಯದಿ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (stat((ವರ್ಣ *)name, &statb))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isrealdir(char_u *name)
{
    ಸಂರಚನಾ stat statb;

    ಯದಿ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಯದಿ (mch_lstat((ವರ್ಣ *)name, &statb))
	ಪ್ರತ್ಯಾಗಮ FALSE;
    ಪ್ರತ್ಯಾಗಮ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
executable_file(char_u *name)
{
    ಸಂರಚನಾ stat	st;

    ಯದಿ (stat((ವರ್ಣ *)name, &st))
	ಪ್ರತ್ಯಾಗಮ 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    ಪೂರ್ಣಾಙ್ಕ vms_executable = 0;
    ಯದಿ (S_ISREG(st.st_mode) && mch_access((ವರ್ಣ *)name, X_OK) == 0)
    {
	ಯದಿ (strstr(vms_tolower((ವರ್ಣ*)name),".exe") != ರಿಕ್ತ
		|| strstr(vms_tolower((ವರ್ಣ*)name),".com")!= ರಿಕ್ತ)
	    vms_executable = 1;
    }
    ಪ್ರತ್ಯಾಗಮ vms_executable;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ S_ISREG(st.st_mode) && mch_access((ವರ್ಣ *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_can_exe(char_u *name, char_u **path, ಪೂರ್ಣಾಙ್ಕ use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    ಪೂರ್ಣಾಙ್ಕ		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    ಯದಿ (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	ಯದಿ ((use_path || gettail(name) != name) && executable_file(name))
	{
	    ಯದಿ (path != ರಿಕ್ತ)
	    {
		ಯದಿ (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		ಅನ್ಯಥಾ
		    *path = vim_strsave(name);
	    }
	    ಪ್ರತ್ಯಾಗಮ TRUE;
	}
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    p = (char_u *)getenv("PATH");
    ಯದಿ (p == ರಿಕ್ತ || *p == NUL)
	ಪ್ರತ್ಯಾಗಮ -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ಪ್ರತಿ (;;)
    {
	e = (char_u *)strchr((ವರ್ಣ *)p, ':');
	ಯದಿ (e == ರಿಕ್ತ)
	    e = p_end;
	elen = (size_t)(e - p);
	ಯದಿ (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((ವರ್ಣ *)buf, bufsize, "%.*s%s%s",
		(ಪೂರ್ಣಾಙ್ಕ)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	ಯದಿ (retval == 1)
	{
	    ಯದಿ (path != ರಿಕ್ತ)
	    {
		ಯದಿ (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		ಅನ್ಯಥಾ
		    *path = vim_strnsave(buf, buflen);
	    }
	    ಭಙ್ಗ;
	}

	ಯದಿ (*e != ':')
	    ಭಙ್ಗ;
	p = e + 1;
    }

    vim_free(buf);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_nodetype(char_u *name)
{
    ಸಂರಚನಾ stat	st;

    ಯದಿ (stat((ವರ್ಣ *)name, &st))
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;
    ಯದಿ (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;
    ಯದಿ (S_ISBLK(st.st_mode))	// block device isn't writable
	ಪ್ರತ್ಯಾಗಮ ಗಂಟು_OTHER;
    // Everything else is writable?
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು_WRITABLE;
}

    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
#ifdef HAVE_CHECK_ರಾಶಿ_GROWTH
    ಪೂರ್ಣಾಙ್ಕ			i;

    check_ರಾಶಿ_growth((ವರ್ಣ *)&i);

# ifdef HAVE_ರಾಶಿ_LIMIT
    get_ರಾಶಿ_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#ಯದಿ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ರಾಶಿ = alloc(get_signal_ರಾಶಿ_size());
    init_signal_ರಾಶಿ();
#endif

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#ifdef MACOS_X
# ifdef MAC_OS_X_VERSION_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_set(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#endif
}

#ಯದಿ defined(EXITFREE)
    ಶೂನ್ಯ
mch_free_mem(ಶೂನ್ಯ)
{
# ಯದಿ defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    ಯದಿ (clip_star.owned)
	clip_lose_selection(&clip_star);
    ಯದಿ (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# ಯದಿ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಯದಿ (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    ಯದಿ (xterm_dpy != ರಿಕ್ತ)
	XtCloseDisplay(xterm_dpy);
    ಯದಿ (app_context != (XtAppContext)ರಿಕ್ತ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ರಿಕ್ತ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# ಯದಿ defined(FEAT_X11)
    ಯದಿ (x11_display != ರಿಕ್ತ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# ಯದಿ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ರಾಶಿ);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
exit_scroll(ಶೂನ್ಯ)
{
    ಯದಿ (silent_mode)
	ಪ್ರತ್ಯಾಗಮ;
    ಯದಿ (newline_on_exit || ಸಂದೇಶ_didout)
    {
	ಯದಿ (ಸಂದೇಶ_use_printf())
	{
	    ಯದಿ (info_message)
		mch_ಸಂದೇಶ("\n");
	    ಅನ್ಯಥಾ
		mch_errmsg("\r\n");
	}
	ಅನ್ಯಥಾ
	    out_char('\n');
    }
    ಅನ್ಯಥಾ ಯದಿ (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	ಸಂದೇಶ_clr_eos_force();		// clear the rest of the display
	windgoto((ಪೂರ್ಣಾಙ್ಕ)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# ಯದಿ (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_dump(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_reset(ಶೂನ್ಯ);
#  define __gcov_flush() ಕುರು { __gcov_dump(); __gcov_reset(); } ಯಾವತ್ (0)
# ಅನ್ಯಥಾ
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_flush(ಶೂನ್ಯ);
# endif
#endif

    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಙ್ಕ r)
{
    exiting = TRUE;

#ಯದಿ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    ಯದಿ (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	ಯದಿ (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_ತೆಗೆ_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	ಯದಿ (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	ಯದಿ (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	ಯದಿ (full_screen)
	    cursor_on();
    }
    out_flush();
    ml_close_all(TRUE);		// remove all memfiles

#ifdef USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#endif

    may_core_dump();
#ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    ಯದಿ (deadly_signal != 0)
	ಪ್ರತ್ಯಾಗಮ;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_core_dump(ಶೂನ್ಯ)
{
    ಯದಿ (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
get_tty_fd(ಪೂರ್ಣಾಙ್ಕ fd)
{
    ಪೂರ್ಣಾಙ್ಕ		tty_fd = fd;

# ಯದಿ defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    ಯದಿ (mch_isatty(fd) > 1)
    {
	ವರ್ಣ *name;

	name = ptsname(fd);
	ಯದಿ (name == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	ಯದಿ (tty_fd < 0)
	    ಪ್ರತ್ಯಾಗಮ -1;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ tty_fd;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_tcgetattr(ಪೂರ್ಣಾಙ್ಕ fd, ಶೂನ್ಯ *term)
{
    ಪೂರ್ಣಾಙ್ಕ		tty_fd;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ಯದಿ (tty_fd < 0)
	ಪ್ರತ್ಯಾಗಮ -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ಸಂರಚನಾ termios *)term);
#  ಅನ್ಯಥಾ
    retval = ioctl(tty_fd, TCGETA, (ಸಂರಚನಾ termio *)term);
#  endif
# ಅನ್ಯಥಾ
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ಸಂರಚನಾ sgttyb *)term);
# endif
    ಯದಿ (tty_fd != fd)
	close(tty_fd);
    ಪ್ರತ್ಯಾಗಮ retval;
}

    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ಸ್ಥಿರ ಸಂರಚನಾ termios told;
	   ಸಂರಚನಾ termios tnew;
#  ಅನ್ಯಥಾ
    ಸ್ಥಿರ ಸಂರಚನಾ termio told;
	   ಸಂರಚನಾ termio tnew;
#  endif

    ಯದಿ (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    ಯದಿ (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ರಿಕ್ತ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  ಯದಿ defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   ಅನ್ಯಥಾ
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    ಅನ್ಯಥಾ
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    ಅನ್ಯಥಾ ಯದಿ (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  ಯದಿ defined(HAVE_TERMIOS_H)
    {
	ಪೂರ್ಣಾಙ್ಕ	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	ಯಾವತ್ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  ಅನ್ಯಥಾ
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# ಅನ್ಯಥಾ
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ಸ್ಥಿರ ಸಂರಚನಾ sgttyb ttybold;
	   ಸಂರಚನಾ sgttyb ttybnew;

    ಯದಿ (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    ಯದಿ (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    ಅನ್ಯಥಾ ಯದಿ (tmode == TMODE_SLEEP)
	ttybnew.sg_flags &= ~(ECHO);
    ioctl(read_cmd_fd, TIOCSETN, &ttybnew);
# endif
    mch_cur_tmode = tmode;
}

/*
 * Try to get the code for "t_kb" from the stty setting
 *
 * Even if termcap claims a backspace key, the user's setting *should*
 * prevail.  stty knows more about reality than termcap does, and if
 * somebody's usual erase key is DEL (which, for most BSD users, it will
 * be), they're going to get really annoyed if their erase key starts
 * doing forward deletes for no reason. (Eric Fischer)
 */
    ಶೂನ್ಯ
get_stty(ಶೂನ್ಯ)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    ಯದಿ (get_tty_info(read_cmd_fd, &info) != OK)
	ಪ್ರತ್ಯಾಗಮ;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    ಯದಿ (p != ರಿಕ್ತ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ರಿಕ್ತ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಙ್ಕ
get_tty_info(ಪೂರ್ಣಾಙ್ಕ fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ಸಂರಚನಾ termios ಕೀಲಿಗಳು;
#  ಅನ್ಯಥಾ
    ಸಂರಚನಾ termio ಕೀಲಿಗಳು;
#  endif

    ಯದಿ (mch_tcgetattr(fd, &ಕೀಲಿಗಳು) != -1)
    {
	info->backspace = ಕೀಲಿಗಳು.c_cc[VERASE];
	info->interrupt = ಕೀಲಿಗಳು.c_cc[VINTR];
	ಯದಿ (ಕೀಲಿಗಳು.c_iflag & ICRNL)
	    info->enter = NL;
	ಅನ್ಯಥಾ
	    info->enter = CAR;
	ಯದಿ (ಕೀಲಿಗಳು.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	ಅನ್ಯಥಾ
	    info->nl_does_cr = FALSE;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
# ಅನ್ಯಥಾ
    // for "old" tty systems
    ಸಂರಚನಾ sgttyb ಕೀಲಿಗಳು;

    ಯದಿ (mch_tcgetattr(fd, &ಕೀಲಿಗಳು) != -1)
    {
	info->backspace = ಕೀಲಿಗಳು.sg_erase;
	info->interrupt = ಕೀಲಿಗಳು.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	ಪ್ರತ್ಯಾಗಮ OK;
    }
# endif
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#endif // VMS

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಙ್ಕ on)
{
#ifdef FEAT_BEVAL_TERM
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	bevalterm_ison = FALSE;
#endif
    ಪೂರ್ಣಾಙ್ಕ		xterm_mouse_vers;

#ಯದಿ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಯದಿ (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    ಯದಿ (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	ಪ್ರತ್ಯಾಗಮ;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    ಯದಿ (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    ಯದಿ (T_CXM != ರಿಕ್ತ && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    ಅನ್ಯಥಾ ಯದಿ (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    ಯದಿ (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	ಯದಿ (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    ಯದಿ (xterm_mouse_vers > 0)
    {
	ಯದಿ (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	ಅನ್ಯಥಾ	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    ಅನ್ಯಥಾ ಯದಿ (ttym_flags == TTYM_DEC)
    {
	ಯದಿ (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	ಅನ್ಯಥಾ	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    ಅನ್ಯಥಾ
    {
	ಯದಿ (on)
	{
	    ಯದಿ (gpm_open())
		mouse_ison = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    ಅನ್ಯಥಾ
    {
	ಯದಿ (on)
	{
	    ಯದಿ (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    ಅನ್ಯಥಾ
    {
	ಯದಿ (on)
	{
	    // D - Enable Mouse up/down messages
	    // L - Enable Left Button Reporting
	    // M - Enable Middle Button Reporting
	    // R - Enable Right Button Reporting
	    // K - Enable SHIFT and CTRL key Reporting
	    // + - Enable Advanced messaging of mouse moves and up/down messages
	    // Q - Quiet No Ack
	    // # - Numeric value of mouse pointer required
	    //	  0 = Multiview 2000 cursor, used as standard
	    //	  1 = Windows Arrow
	    //	  2 = Windows I Beam
	    //	  3 = Windows Hour Glass
	    //	  4 = Windows Cross Hair
	    //	  5 = Windows UP Arrow
# ifdef JSBTERM_MOUSE_NONADVANCED
	    // Disables full feedback of pointer movements
	    out_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# ಅನ್ಯಥಾ
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	ಅನ್ಯಥಾ
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    ಅನ್ಯಥಾ
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	ಯದಿ (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	ಅನ್ಯಥಾ
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#ಯದಿ defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ಶೂನ್ಯ
mch_bevalterm_changed(ಶೂನ್ಯ)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    ಶೂನ್ಯ
check_mouse_termcode(ಶೂನ್ಯ)
{
#ifdef FEAT_MOUSE_XTERM
    ಯದಿ (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	ಯದಿ (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    ಯದಿ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    ಯದಿ (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	ಯದಿ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಅನ್ಯಥಾ
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    ಯದಿ (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	ಯದಿ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಅನ್ಯಥಾ
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#ifndef VMS

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 * Return OK when size could be determined, FAIL otherwise.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    long	rows = 0;
    long	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	ಸಂರಚನಾ winsize	ws;
	ಪೂರ್ಣಾಙ್ಕ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ಯದಿ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ಯದಿ (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# ಅನ್ಯಥಾ // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ಸಂರಚನಾ ttysize	ts;
	ಪೂರ್ಣಾಙ್ಕ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ಯದಿ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ಯದಿ (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_ಸಾಲುಗಳು;
#   ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
#   endif
	}
    }
#  endif // TIOCGSIZE
# endif // TIOCGWINSZ

    /*
     * 2. get size from environment
     *    When being POSIX compliant ('|' flag in 'cpoptions') this overrules
     *    the ioctl() values!
     */
    ಯದಿ (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ರಿಕ್ತ)
    {
	ಯದಿ ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((ವರ್ಣ *)p);
# ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	ಯದಿ ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((ವರ್ಣ *)p);
# ifdef FEAT_EVAL
	    ch_log(ರಿಕ್ತ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    ಯದಿ (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(ರಿಕ್ತ, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    ಯದಿ (columns <= 0 || rows <= 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    ಪ್ರತ್ಯಾಗಮ OK;
}

# ಯದಿ defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    ಶೂನ್ಯ
mch_calc_cell_size(ಸಂರಚನಾ cellsize *cs_out)
{
   ಸಂರಚನಾ winsize   ws;
   ಪೂರ್ಣಾಙ್ಕ		    retval;
   ಪೂರ್ಣಾಙ್ಕ		    x_cell_size;
   ಪೂರ್ಣಾಙ್ಕ		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ರಿಕ್ತ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   ಯದಿ (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	ಪ್ರತ್ಯಾಗಮ;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   ಯದಿ (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# ಯದಿ defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_report_winsize(ಪೂರ್ಣಾಙ್ಕ fd, ಪೂರ್ಣಾಙ್ಕ rows, ಪೂರ್ಣಾಙ್ಕ cols)
{
    ಪೂರ್ಣಾಙ್ಕ		tty_fd;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ಯದಿ (tty_fd < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;

#  ಯದಿ defined(TIOCSWINSZ)
    ಸಂರಚನಾ winsize ws;

    ws.ws_col = cols;
    ws.ws_row = rows;
    // Pixel sizes are best-effort; the child can query its own terminal
    // when needed.  Avoid calling mch_calc_cell_size() here -- on
    // terminals that do not report pixel sizes via TIOCGWINSZ, the
    // CSI 14 t fallback would synchronously block term_start() for up
    // to 200ms each time, perturbing timing-sensitive screendump tests
    // that fork a child Vim.  Setting 0 matches the pre-image-popup
    // behaviour and is what TIOCSWINSZ does when the kernel cannot
    // populate the field anyway.
    ws.ws_xpixel = 0;
    ws.ws_ypixel = 0;

    retval = ioctl(tty_fd, TIOCSWINSZ, &ws);
    ch_log(ರಿಕ್ತ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ಸಂರಚನಾ ttysize ts;

    ts.ts_cols = cols;
    ts.ts_ಸಾಲುಗಳು = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ರಿಕ್ತ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    ಯದಿ (tty_fd != fd)
	close(tty_fd);
    ಪ್ರತ್ಯಾಗಮ retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    ಶೂನ್ಯ
mch_set_shellsize(ಶೂನ್ಯ)
{
    ಯದಿ (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((ಪೂರ್ಣಾಙ್ಕ)Rows, (ಪೂರ್ಣಾಙ್ಕ)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ಸ್ಥಿರ pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    ಯಾವತ್ (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	ಯದಿ (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    ಯದಿ (++delay_msec > 10)
		delay_msec = 10;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    ಭಙ್ಗ;
    }
    ಪ್ರತ್ಯಾಗಮ wait_pid;
}

#ಯದಿ !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_child_environment(
	long	rows,
	long	columns,
	ವರ್ಣ	*term,
	ಪೂರ್ಣಾಙ್ಕ	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    ವರ್ಣ	envbuf[50];
# ಅನ್ಯಥಾ
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Term[30];
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Rows[20];
    ಸ್ಥಿರ ವರ್ಣ	envbuf_ಸಾಲುಗಳು[20];
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Columns[20];
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ಸ್ಥಿರ ವರ್ಣ	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((ವರ್ಣ *)envbuf, "%ld", rows);
    setenv("ROWS", (ವರ್ಣ *)envbuf, 1);
    sprintf((ವರ್ಣ *)envbuf, "%ld", rows);
    setenv("LINES", (ವರ್ಣ *)envbuf, 1);
    sprintf((ವರ್ಣ *)envbuf, "%ld", columns);
    setenv("COLUMNS", (ವರ್ಣ *)envbuf, 1);
    sprintf((ವರ್ಣ *)envbuf, "%d", t_colors);
    setenv("COLORS", (ವರ್ಣ *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    ಯದಿ (is_terminal)
    {
	sprintf((ವರ್ಣ *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (ವರ್ಣ *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ರಿಕ್ತ ? "" : (ವರ್ಣ *)serverName, 1);
#  endif
# ಅನ್ಯಥಾ
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, ಪರಿಮಾಣ(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, ಪರಿಮಾಣ(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_ಸಾಲುಗಳು, ಪರಿಮಾಣ(envbuf_ಸಾಲುಗಳು), "LINES=%ld", rows);
    putenv(envbuf_ಸಾಲುಗಳು);
    vim_snprintf(envbuf_Columns, ಪರಿಮಾಣ(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, ಪರಿಮಾಣ(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    ಯದಿ (is_terminal)
    {
	vim_snprintf(envbuf_Version, ಪರಿಮಾಣ(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, ಪರಿಮಾಣ(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ರಿಕ್ತ ? "" : (ವರ್ಣ *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
set_default_child_environment(ಪೂರ್ಣಾಙ್ಕ is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#ಯದಿ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
open_pty(ಪೂರ್ಣಾಙ್ಕ *pty_master_fd, ಪೂರ್ಣಾಙ್ಕ *pty_slave_fd, char_u **name1, char_u **name2)
{
    ವರ್ಣ	*tty_name;

    ಯದಿ (name1 != ರಿಕ್ತ)
	*name1 = ರಿಕ್ತ;
    ಯದಿ (name2 != ರಿಕ್ತ)
	*name2 = ರಿಕ್ತ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    ಯದಿ (*pty_master_fd < 0)
	ಪ್ರತ್ಯಾಗಮ;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# ಅನ್ಯಥಾ
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    ಯದಿ (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (name1 != ರಿಕ್ತ)
	    *name1 = vim_strsave((char_u *)tty_name);
	ಯದಿ (name2 != ರಿಕ್ತ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_send_sigint(ಪೂರ್ಣಾಙ್ಕ c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    ಯದಿ (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# ಅನ್ಯಥಾ
	kill(0, SIGINT);
# endif
	ಯದಿ (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#ಯದಿ !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    ಪೂರ್ಣಾಙ್ಕ
unix_build_argv(
	char_u *cmd,
	ವರ್ಣ ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		argc;

    *sh_tofree = vim_strsave(p_sh);
    ಯದಿ (*sh_tofree == ರಿಕ್ತ)		// out of memory
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    *argvp = argv;

    ಯದಿ (cmd != ರಿಕ್ತ)
    {
	char_u	*s;
	char_u	*p;

	ಯದಿ (extra_shell_arg != ರಿಕ್ತ)
	    argv[argc++] = (ವರ್ಣ *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	ಯದಿ (*shcf_tofree == ರಿಕ್ತ)    // out of memory
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	ಯಾವತ್ (*p != NUL)
	{
	    argv[argc++] = (ವರ್ಣ *)s;
	    ಯಾವತ್ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (ವರ್ಣ *)cmd;
    }
    argv[argc] = ರಿಕ್ತ;
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_call_shell_terminal(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    char_u	*tofree1 = ರಿಕ್ತ;
    char_u	*tofree2 = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ಯದಿ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ಗಚ್ಛ theend;

    init_job_options(&opt);
    ch_log(ರಿಕ್ತ, "starting terminal for system command '%s'", cmd);
    buf = term_start(ರಿಕ್ತ, argv, &opt, TERM_START_SYSTEM);
    ಯದಿ (buf == ರಿಕ್ತ)
	ಗಚ್ಛ theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ಯದಿ (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	ಯಾವತ್ (term_use_loop())
	{
	    ಯದಿ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ಯದಿ (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    ಅನ್ಯಥಾ
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ರಿಕ್ತ, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    ಯದಿ (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_call_shell_system(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    ವರ್ಣ	*ifn = ರಿಕ್ತ;
    ವರ್ಣ	*ofn = ರಿಕ್ತ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    ಪೂರ್ಣಾಙ್ಕ		x;

    out_flush();

    ಯದಿ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# ಯದಿ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  ಯದಿ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    ಯದಿ (cmd == ರಿಕ್ತ)
	x = system((ವರ್ಣ *)p_sh);
    ಅನ್ಯಥಾ
    {
# ifdef VMS
	ಯದಿ (ofn = strchr((ವರ್ಣ *)cmd, '>'))
	    *ofn++ = '\0';
	ಯದಿ (ifn = strchr((ವರ್ಣ *)cmd, '<'))
	{
	    ವರ್ಣ *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    ಯದಿ (p)
		*p = '\0';
	}
	ಯದಿ (ofn)
	    x = vms_sys((ವರ್ಣ *)cmd, ofn, ifn);
	ಅನ್ಯಥಾ
	    x = system((ವರ್ಣ *)cmd);
# ಅನ್ಯಥಾ
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ರಿಕ್ತ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	ಯದಿ (newcmd == ರಿಕ್ತ)
	    x = 0;
	ಅನ್ಯಥಾ
	{
	    sprintf((ವರ್ಣ *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ರಿಕ್ತ ? "" : (ವರ್ಣ *)extra_shell_arg,
		    (ವರ್ಣ *)p_shcf,
		    (ವರ್ಣ *)cmd);
	    x = system((ವರ್ಣ *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    ಯದಿ (emsg_silent)
	;
    ಅನ್ಯಥಾ ಯದಿ (x == 127)
	ಸಂದೇಶ_puts(_("\nCannot execute shell sh\n"));
    ಅನ್ಯಥಾ ಯದಿ (x && !(options & SHELL_SILENT))
    {
	ಸಂದೇಶ_puts(_("\nshell returned "));
	ಸಂದೇಶ_outnum((long)x);
	ಸಂದೇಶ_putchar('\n');
    }

    ಯದಿ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# ಯದಿ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    ಪ್ರತ್ಯಾಗಮ x;
}

#ಅನ್ಯಥಾ // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_call_shell_fork(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    ಪೂರ್ಣಾಙ್ಕ		status = -1;
    ಪೂರ್ಣಾಙ್ಕ		retval = -1;
    ವರ್ಣ	**argv = ರಿಕ್ತ;
    char_u	*tofree1 = ರಿಕ್ತ;
    char_u	*tofree2 = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		i;
    ಪೂರ್ಣಾಙ್ಕ		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    ಪೂರ್ಣಾಙ್ಕ		pty_slave_fd = -1;
# endif
    ಪೂರ್ಣಾಙ್ಕ		fd_toshell[2];		// for pipes
    ಪೂರ್ಣಾಙ್ಕ		fd_fromshell[2];
    ಪೂರ್ಣಾಙ್ಕ		pipe_error = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    ಯದಿ (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    ಯದಿ (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    ಯದಿ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ಗಚ್ಛ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    ಯದಿ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.in_use && show_shell_mess)
# endif
		    )
    {
# ifdef FEAT_GUI
	/*
	 * Try to open a master pty.
	 * If this works, open the slave pty.
	 * If the slave can't be opened, close the master pty.
	 */
	ಯದಿ (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ರಿಕ್ತ, ರಿಕ್ತ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	ಯದಿ (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    ಯದಿ (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		ಯದಿ (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    ಯದಿ (pipe_error)
	    {
		ಸಂದೇಶ_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    ಯದಿ (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	ಯದಿ (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    ಸಂದೇಶ_puts(_("\nCannot fork\n"));
	    ಯದಿ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		ಯದಿ (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		ಅನ್ಯಥಾ				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	ಅನ್ಯಥಾ ಯದಿ (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    ಯದಿ (ch_log_active())
	    {
		ch_log(ರಿಕ್ತ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    ಯದಿ (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		ಪೂರ್ಣಾಙ್ಕ fd;

		/*
		 * Don't want to show any message from the shell.  Can't just
		 * close stdout and stderr though, because some systems will
		 * break if you try to write to them after that, so we must
		 * use dup() to replace them with something else -- webb
		 * Connect stdin to /dev/null too, so ":n `cat`" doesn't hang,
		 * waiting for input.
		 */
		fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
		fclose(stdin);
		fclose(stdout);
		fclose(stderr);

		/*
		 * If any of these open()'s and dup()'s fail, we just continue
		 * anyway.  It's not fatal, and on most systems it will make
		 * no difference at all.  On a few it will cause the execvp()
		 * to exit with a non-zero status even when the completion
		 * could be done, which is nothing too serious.  If the open()
		 * or dup() failed we'd just do the same thing ourselves
		 * anyway -- webb
		 */
		ಯದಿ (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    ಅನ್ಯಥಾ ಯದಿ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		ಯದಿ (p_stmp)
		{
		    (ಶೂನ್ಯ)setsid();
#  ಯದಿ defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		ಯದಿ (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    ಯದಿ (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (ವರ್ಣ *)ರಿಕ್ತ);
#  endif
		}
# endif
		set_default_child_environment(FALSE);

		/*
		 * stderr is only redirected when using the GUI, so that a
		 * program like gpg can still access the terminal to get a
		 * passphrase using stderr.
		 */
# ifdef FEAT_GUI
		ಯದಿ (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    ಯದಿ (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		ಅನ್ಯಥಾ
# endif
		{
		    // set up stdin for the child
		    close(fd_toshell[1]);
		    close(0);
		    vim_ignored = dup(fd_toshell[0]);
		    close(fd_toshell[0]);

		    // set up stdout for the child
		    close(fd_fromshell[0]);
		    close(1);
		    vim_ignored = dup(fd_fromshell[1]);
		    close(fd_fromshell[1]);

# ifdef FEAT_GUI
		    ಯದಿ (gui.in_use)
		    {
			// set up stderr for the child
			close(2);
			vim_ignored = dup(1);
		    }
# endif
		}
	    }

	    /*
	     * There is no type cast for the argv, because the type may be
	     * different on different machines. This may cause a warning
	     * message with strict compilers, don't worry about it.
	     * Call _exit() instead of exit() to avoid closing the connection
	     * to the X/Wayland server (esp. with GTK, which uses atexit()).
	     */
	    execvp(argv[0], argv);
	    _exit(EXEC_FAILED);	    // exec failed, return failure code
	}
	ಅನ್ಯಥಾ			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_signals(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_SIGNALS(&curset);
# ifdef FEAT_JOB_CHANNEL
	    ++dont_check_job_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    ಯದಿ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		ಪೂರ್ಣಾಙ್ಕ	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		ಪೂರ್ಣಾಙ್ಕ	    ta_len = 0;		// valid bytes in ta_buf[]
		ಪೂರ್ಣಾಙ್ಕ	    len;
		ಪೂರ್ಣಾಙ್ಕ	    p_more_save;
		ಪೂರ್ಣಾಙ್ಕ	    old_State;
		ಪೂರ್ಣಾಙ್ಕ	    c;
		ಪೂರ್ಣಾಙ್ಕ	    toshell_fd;
		ಪೂರ್ಣಾಙ್ಕ	    fromshell_fd;
		garray_T    ga;
		ಪೂರ್ಣಾಙ್ಕ	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		ಯದಿ (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		ಅನ್ಯಥಾ
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_fromshell[1]);
		    toshell_fd = fd_toshell[1];
		    fromshell_fd = fd_fromshell[0];
		}

		/*
		 * Write to the child if there are typed characters.
		 * Read from the child if there are characters available.
		 *   Repeat the reading a few times if more characters are
		 *   available. Need to check for typed keys now and then, but
		 *   not too often (delays when no chars are available).
		 * This loop is quit if no characters can be read from the pty
		 * (WaitForChar detected special condition), or there are no
		 * characters available and the child has exited.
		 * Only check if the child has exited when there is no more
		 * output. The child may exit before all the output has
		 * been printed.
		 *
		 * Currently this busy loops!
		 * This can probably dead-lock when the write blocks!
		 */
		p_more_save = p_more;
		p_more = FALSE;
		old_State = State;
		State = MODE_EXTERNCMD;	// don't redraw at window resize

		ಯದಿ ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    ಯದಿ ((wpid = fork()) == -1)
		    {
			ಸಂದೇಶ_puts(_("\nCannot fork\n"));
		    }
		    ಅನ್ಯಥಾ ಯದಿ (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			ಪ್ರತಿ (;;)
			{
			    ಯದಿ (lplen == 0)
				len = 0;
			    ಅನ್ಯಥಾ ಯದಿ (lp[written] == NL)
				// NL -> NUL translation
				len = ಲಿಖ(toshell_fd, "", (size_t)1);
			    ಅನ್ಯಥಾ
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = ಲಿಖ(toshell_fd, (ವರ್ಣ *)lp + written,
					   s == ರಿಕ್ತ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    ಯದಿ (len == (ಪೂರ್ಣಾಙ್ಕ)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				ಯದಿ (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						    || curbuf->b_p_eol)))
				    vim_ignored = ಲಿಖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				ಯದಿ (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ಭಙ್ಗ;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    ಅನ್ಯಥಾ ಯದಿ (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    ಅನ್ಯಥಾ // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		ಯದಿ (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		ಪ್ರತಿ (;;)
		{
		    /*
		     * Check if keys have been typed, write them to the child
		     * if there are any.
		     * Don't do this if we are expanding wild cards (would eat
		     * typeahead).
		     * Don't do this when filtering and terminal is in cooked
		     * mode, the shell command will handle the I/O.  Avoids
		     * that a typed password is echoed for ssh or gpg command.
		     * Don't get characters when the child has already
		     * finished (wait_pid == 0).
		     * Don't read characters unless we didn't get output for a
		     * while (noread_cnt > 4), avoids that ":r !ls" eats
		     * typeahead.
		     */
		    len = 0;
		    ಯದಿ (!(options & SHELL_EXPAND)
			    && ((options &
					 (SHELL_READ|SHELL_WRITE|SHELL_COOKED))
				      != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
						    || gui.in_use
# endif
						    )
			    && wait_pid == 0
			    && (ta_len > 0 || noread_cnt > 4))
		    {
		      ಯದಿ (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      ಯದಿ (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			ಯದಿ (len == 1 && (pty_master_fd < 0 || cmd != ರಿಕ್ತ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    ಯದಿ (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				close(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = term_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			ಯದಿ (pty_master_fd < 0)
			{
			    ಪ್ರತಿ (i = ta_len; i < ta_len + len; ++i)
			    {
				ಯದಿ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    ಸಂದೇಶ_putchar(ta_buf[i]);
				ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
				{
				    ಪೂರ್ಣಾಙ್ಕ l = (*mb_ptr2len)(ta_buf + i);

				    ಸಂದೇಶ_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				ಅನ್ಯಥಾ
				    ಸಂದೇಶ_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col);
			    out_flush();
			}

			ta_len += len;

			/*
			 * Write the characters to the child, unless EOF has
			 * been typed for pipes.  Write one character at a
			 * time, to avoid losing too much typeahead.
			 * When writing buffer lines, drop the typed
			 * characters (only check for CTRL-C).
			 */
			ಯದಿ (options & SHELL_WRITE)
			    ta_len = 0;
			ಅನ್ಯಥಾ ಯದಿ (toshell_fd >= 0)
			{
			    len = ಲಿಖ(toshell_fd, (ವರ್ಣ *)ta_buf, (size_t)1);
			    ಯದಿ (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    ಯದಿ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# ಅನ್ಯಥಾ
			kill(0, SIGINT);
# endif
			ಯದಿ (wpid > 0)
			    kill(wpid, SIGINT);
			got_int = FALSE;
		    }

		    /*
		     * Check if the child has any characters to be printed.
		     * Read them and write them to our window.	Repeat this as
		     * long as there is something to do, avoid the 10ms wait
		     * for mch_inchar(), or sending typeahead characters to
		     * the external process.
		     * TODO: This should handle escape sequences, compatible
		     * to some terminal (vt52?).
		     */
		    ++noread_cnt;
		    ಯಾವತ್ (RealWaitForChar(fromshell_fd, 10L, ರಿಕ್ತ, ರಿಕ್ತ))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			ಯದಿ (len <= 0)		    // end of file or error
			    ಗಚ್ಛ finished;

			noread_cnt = 0;
			ಯದಿ (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    ಪ್ರತಿ (i = 0; i < len; ++i)
			    {
				ಯದಿ (buffer[i] == NL)
				    append_ga_ಸಾಲು(&ga);
				ಅನ್ಯಥಾ ಯದಿ (buffer[i] == NUL)
				    ga_append(&ga, NL);
				ಅನ್ಯಥಾ
				    ga_append(&ga, buffer[i]);
			    }
			}
			ಅನ್ಯಥಾ ಯದಿ (has_mbyte)
			{
			    ಪೂರ್ಣಾಙ್ಕ		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    ಪ್ರತಿ (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				ಯದಿ (l == 0)
				    l = 1;  // NUL byte?
				ಅನ್ಯಥಾ ಯದಿ (MB_BYTE2LEN(*p) != l)
				    ಭಙ್ಗ;
			    }
			    ಯದಿ (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				ಯದಿ (len >= 12)
				    ++p;
				ಅನ್ಯಥಾ
				{
				    buffer_off = len;
				    ಅನುವರ್ತ;
				}
			    }
			    c = *p;
			    *p = NUL;
			    ಸಂದೇಶ_puts((ವರ್ಣ *)buffer);
			    ಯದಿ (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				ಅನುವರ್ತ;
			    }
			    buffer_off = 0;
			}
			ಅನ್ಯಥಾ
			{
			    buffer[len] = NUL;
			    ಸಂದೇಶ_puts((ವರ್ಣ *)buffer);
			}

			windgoto(ಸಂದೇಶ_row, cmdline_col_off + ಸಂದೇಶ_col);
			cursor_on();
			out_flush();
			ಯದಿ (got_int)
			    ಭಙ್ಗ;

# ifdef ELAPSED_FUNC
			ಯದಿ (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    ಯದಿ (msec > 2000)
			    {
				noread_cnt = 5;
				ಭಙ್ಗ;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    ಯದಿ (wait_pid == pid)
		    {
			ಯದಿ (noread_cnt < 5)
			    ಅನುವರ್ತ;
			ಭಙ್ಗ;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    ಯದಿ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    ಅನ್ಯಥಾ
			wait_pid = 0;

# ಯದಿ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
# endif
# ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
# endif
		}
finished:
		p_more = p_more_save;
		ಯದಿ (options & SHELL_READ)
		{
		    ಯದಿ (ga.ga_len > 0)
		    {
			append_ga_ಸಾಲು(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    ಅನ್ಯಥಾ
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		ಯದಿ (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		ಯದಿ (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# ಯದಿ (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    ಅನ್ಯಥಾ
	    {
		long delay_msec = 1;

		ಯದಿ (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		ಪ್ರತಿ (;;)
		{
		    ಯದಿ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  ಅನ್ಯಥಾ
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    ಯದಿ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			ಭಙ್ಗ;
		    }

#  ಯದಿ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
#  endif
#  ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
#  endif

		    // Wait for 1 to 10 msec. 1 is faster but gives the child
		    // less time, gradually wait longer.
		    mch_delay(delay_msec,
				   MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
		    ಯದಿ (++delay_msec > 10)
			delay_msec = 10;
		}

		ಯದಿ (tmode == TMODE_RAW)
		    // possibly enables modifyOtherKeys again
		    out_str_t_TI();
	    }
# endif

	    /*
	     * Wait until our child has exited.
	     * Ignore wait() returning pids of other children and returning
	     * because of some signal like SIGWINCH.
	     * Don't wait if wait_pid was already set above, indicating the
	     * child already exited.
	     */
	    ಯದಿ (wait_pid != pid)
		(ಶೂನ್ಯ)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    ಯದಿ (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    ಯದಿ (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ರಿಕ್ತ);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    ಯದಿ (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    ಯದಿ (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		ಯದಿ (retval != 0 && !emsg_silent)
		{
		    ಯದಿ (retval == EXEC_FAILED)
		    {
			ಸಂದೇಶ_puts(_("\nCannot execute shell "));
			ಸಂದೇಶ_outtrans(p_sh);
			ಸಂದೇಶ_putchar('\n');
		    }
		    ಅನ್ಯಥಾ ಯದಿ (!(options & SHELL_SILENT))
		    {
			ಸಂದೇಶ_puts(_("\nshell returned "));
			ಸಂದೇಶ_outnum((long)retval);
			ಸಂದೇಶ_putchar('\n');
		    }
		}
	    }
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_puts(_("\nCommand terminated\n"));
	}
    }

error:
    ಯದಿ (!did_settmode)
	ಯದಿ (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    ಪ್ರತ್ಯಾಗಮ retval;
}
#endif // USE_SYSTEM

    ಪೂರ್ಣಾಙ್ಕ
mch_call_shell(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "executing shell command: %s", cmd);
#endif
#ಯದಿ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    ಯದಿ (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ರಿಕ್ತ
					      && (options & SHELL_SILENT) == 0)
	ಪ್ರತ್ಯಾಗಮ mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    ಪ್ರತ್ಯಾಗಮ mch_call_shell_system(cmd, options);
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ mch_call_shell_fork(cmd, options);
#endif
}

#ಯದಿ defined(FEAT_EVAL)
/*
 * Execute "argv" directly without the shell and return the output.
 * Used by system() and systemlist() when the command is a List.
 * "infile" is an optional temp file for stdin input.
 * "flags" is SHELL_SILENT etc.
 * When "ret_len" is not NULL, set it to the length of the output.
 * Returns the output in allocated memory (or NULL on error).
 * Sets v:shell_error to the exit status.
 */
    char_u *
mch_get_cmd_output_direct(
    ವರ್ಣ	**argv,
    char_u	*infile,
    ಪೂರ್ಣಾಙ್ಕ		flags UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		*ret_len)
{
    pid_t	pid;
    ಪೂರ್ಣಾಙ್ಕ		fd_out[2] = {-1, -1};
    ಪೂರ್ಣಾಙ್ಕ		status = -1;
    char_u	*buffer = ರಿಕ್ತ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ರಿಕ್ತ, "directly executing: %s", argv[0]);

    ಯದಿ (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# ಅನ್ಯಥಾ
    pid = fork();
# endif
    ಯದಿ (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    }

    ಯದಿ (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	ಯದಿ (ch_log_active())
	{
	    ch_log(ರಿಕ್ತ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	ಯದಿ (infile != ರಿಕ್ತ)
	{
	    ಪೂರ್ಣಾಙ್ಕ fd_in = open((ವರ್ಣ *)infile, O_RDONLY);
	    ಯದಿ (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	ಅನ್ಯಥಾ
	{
	    ಪೂರ್ಣಾಙ್ಕ nullfd = open("/dev/null", O_RDONLY);
	    ಯದಿ (nullfd >= 0)
	    {
		close(0);
		vim_ignored = dup(nullfd);
		close(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	close(fd_out[0]);
	close(1);
	vim_ignored = dup(fd_out[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_out[1]);
	close(fd_out[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_SIGNALS(&curset);
    close(fd_out[1]);

    // Read output from child.
    ಪ್ರತಿ (;;)
    {
	ವರ್ಣ	buf[4096];
	ಪೂರ್ಣಾಙ್ಕ	n;

	n = (ಪೂರ್ಣಾಙ್ಕ)ಪಠ(fd_out[0], buf, ಪರಿಮಾಣ(buf));
	ಯದಿ (n <= 0)
	    ಭಙ್ಗ;
	ಯದಿ (ga_grow(&ga, n) == FAIL)
	    ಭಙ್ಗ;
	mch_memmove((ವರ್ಣ *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (ಶೂನ್ಯ)waitpid(pid, &status, 0);
    ಯದಿ (WIFEXITED(status))
	status = WEXITSTATUS(status);
    ಅನ್ಯಥಾ
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    ಯದಿ (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	ಯದಿ (buffer != ರಿಕ್ತ)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    ಯದಿ (ret_len == ರಿಕ್ತ)
	    {
		ಪೂರ್ಣಾಙ್ಕ	i;

		// Change NUL into SOH, otherwise the string is truncated.
		ಪ್ರತಿ (i = 0; i < ga.ga_len; ++i)
		    ಯದಿ (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    ಅನ್ಯಥಾ
		*ret_len = ga.ga_len;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (ret_len != ರಿಕ್ತ)
	*ret_len = 0;

    ga_clear(&ga);
    ಪ್ರತ್ಯಾಗಮ buffer;
}
#endif

#ಯದಿ defined(FEAT_JOB_CHANNEL)
    ಶೂನ್ಯ
mch_job_start(ವರ್ಣ **argv, job_T *job, jobopt_T *options, ಪೂರ್ಣಾಙ್ಕ is_terminal)
{
    pid_t	pid;
    ಪೂರ್ಣಾಙ್ಕ		fd_in[2] = {-1, -1};	// for stdin
    ಪೂರ್ಣಾಙ್ಕ		fd_out[2] = {-1, -1};	// for stdout
    ಪೂರ್ಣಾಙ್ಕ		fd_err[2] = {-1, -1};	// for stderr
    ಪೂರ್ಣಾಙ್ಕ		pty_master_fd = -1;
    ಪೂರ್ಣಾಙ್ಕ		pty_slave_fd = -1;
    channel_T	*channel = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ಪೂರ್ಣಾಙ್ಕ		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    ಪೂರ್ಣಾಙ್ಕ		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    ಯದಿ (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    ಯದಿ (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    ಯದಿ (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((ವರ್ಣ *)fname, O_RDONLY, 0);
	ಯದಿ (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    ಅನ್ಯಥಾ
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	ಯದಿ (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    ಗಚ್ಛ failed;

    ಯದಿ (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((ವರ್ಣ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ಯದಿ (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    ಅನ್ಯಥಾ ಯದಿ (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	ಗಚ್ಛ failed;

    ಯದಿ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((ವರ್ಣ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ಯದಿ (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಗಚ್ಛ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    ಅನ್ಯಥಾ ಯದಿ (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	ಗಚ್ಛ failed;

    ಯದಿ (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	ಯದಿ (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ಯದಿ (channel != ರಿಕ್ತ)
		++channel->ch_refcount;
	}
	ಅನ್ಯಥಾ
	    channel = add_channel();
	ಯದಿ (channel == ರಿಕ್ತ)
	    ಗಚ್ಛ failed;
	ಯದಿ (job->jv_tty_out != ರಿಕ್ತ)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    ಯದಿ (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	ಗಚ್ಛ failed;
    }
    ಯದಿ (pid == 0)
    {
	ಪೂರ್ಣಾಙ್ಕ	null_fd = -1;
	ಪೂರ್ಣಾಙ್ಕ	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	ಯದಿ (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(ಶೂನ್ಯ)setsid();
# endif

# ifdef FEAT_TERMINAL
	ಯದಿ (options->jo_term_rows > 0)
	{
	    ವರ್ಣ *term = (ವರ್ಣ *)T_NAME;

#  ifdef FEAT_GUI
	    ಯದಿ (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    ಯದಿ (term == ರಿಕ್ತ || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		ಯದಿ (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		ಅನ್ಯಥಾ ಯದಿ (t_colors > 16)
		    term = "xterm-color";
		ಅನ್ಯಥಾ
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	ಅನ್ಯಥಾ
# endif
	    set_default_child_environment(is_terminal);

	ಯದಿ (options->jo_env != ರಿಕ್ತ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    ಪೂರ್ಣಾಙ್ಕ		todo = (ಪೂರ್ಣಾಙ್ಕ)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&dict->dv_hashtab, hi, todo)
		ಯದಿ (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ಕೀಲಿ, tv_get_string(item));
		    --todo;
		}
	}

	ಯದಿ (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    ಯದಿ (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	ಯದಿ (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (ವರ್ಣ *)ರಿಕ್ತ);
# endif
	}

	// set up stdin for the child
	close(0);
	ಯದಿ (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ಅನ್ಯಥಾ ಯದಿ (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಅನ್ಯಥಾ
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	ಯದಿ (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	ಅನ್ಯಥಾ ಯದಿ (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	ಅನ್ಯಥಾ ಯದಿ (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಅನ್ಯಥಾ
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	ಯದಿ (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ಅನ್ಯಥಾ ಯದಿ (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಅನ್ಯಥಾ
	    vim_ignored = dup(fd_out[1]);

	ಯದಿ (fd_in[0] >= 0)
	    close(fd_in[0]);
	ಯದಿ (fd_in[1] >= 0)
	    close(fd_in[1]);
	ಯದಿ (fd_out[0] >= 0)
	    close(fd_out[0]);
	ಯದಿ (fd_out[1] >= 0)
	    close(fd_out[1]);
	ಯದಿ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ಯದಿ (fd_err[1] >= 0)
	    close(fd_err[1]);
	ಯದಿ (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	ಯದಿ (null_fd >= 0)
	    close(null_fd);

	ಯದಿ (options->jo_cwd != ರಿಕ್ತ && mch_chdir((ವರ್ಣ *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	ಯದಿ (stderr_works)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_SIGNALS(&curset);

    job->jv_pid = pid;
    job->jv_status = JOB_STARTED;
    job->jv_channel = channel;  // ch_refcount was set above

    ಯದಿ (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    ಯದಿ (fd_in[0] >= 0)
	close(fd_in[0]);
    ಯದಿ (fd_out[1] >= 0)
	close(fd_out[1]);
    ಯದಿ (fd_err[1] >= 0)
	close(fd_err[1]);
    ಯದಿ (channel != ರಿಕ್ತ)
    {
	ಪೂರ್ಣಾಙ್ಕ in_fd = INVALID_FD;
	ಪೂರ್ಣಾಙ್ಕ out_fd = INVALID_FD;
	ಪೂರ್ಣಾಙ್ಕ err_fd = INVALID_FD;

	ಯದಿ (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	ಯದಿ (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	ಯದಿ (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    ಯದಿ (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    ಅನ್ಯಥಾ ಯದಿ (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (fd_in[1] >= 0)
	    close(fd_in[1]);
	ಯದಿ (fd_out[0] >= 0)
	    close(fd_out[0]);
	ಯದಿ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ಯದಿ (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    ಪ್ರತ್ಯಾಗಮ;

failed:
    channel_unref(channel);
    ಯದಿ (fd_in[0] >= 0)
	close(fd_in[0]);
    ಯದಿ (fd_in[1] >= 0)
	close(fd_in[1]);
    ಯದಿ (fd_out[0] >= 0)
	close(fd_out[0]);
    ಯದಿ (fd_out[1] >= 0)
	close(fd_out[1]);
    ಯದಿ (fd_err[0] >= 0)
	close(fd_err[0]);
    ಯದಿ (fd_err[1] >= 0)
	close(fd_err[1]);
    ಯದಿ (pty_master_fd >= 0)
	close(pty_master_fd);
    ಯದಿ (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ಸ್ಥಿರ char_u *
get_signal_name(ಪೂರ್ಣಾಙ್ಕ sig)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	numbuf[NUMBUFLEN];

    ಯದಿ (sig == SIGKILL)
	ಪ್ರತ್ಯಾಗಮ vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    ಪ್ರತಿ (i = 0; signal_info[i].sig != -1; i++)
	ಯದಿ (sig == signal_info[i].sig)
	    ಪ್ರತ್ಯಾಗಮ strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((ವರ್ಣ *)numbuf, NUMBUFLEN, "%d", sig);
    ಪ್ರತ್ಯಾಗಮ vim_strnsave(numbuf, i);
}

    ವರ್ಣ *
mch_job_status(job_T *job)
{
    ಪೂರ್ಣಾಙ್ಕ		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    ಯದಿ (wait_pid == -1)
    {
	ಪೂರ್ಣಾಙ್ಕ waitpid_errno = errno;
	ಯದಿ (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    ಪ್ರತ್ಯಾಗಮ "run";

	// process must have exited
	ಯದಿ (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	ಗಚ್ಛ return_dead;
    }
    ಯದಿ (wait_pid == 0)
	ಪ್ರತ್ಯಾಗಮ "run";
    ಯದಿ (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	ಯದಿ (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	ಗಚ್ಛ return_dead;
    }
    ಯದಿ (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	ಯದಿ (job->jv_status < JOB_ENDED && job->jv_termsig != ರಿಕ್ತ)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	ಗಚ್ಛ return_dead;
    }
    ಪ್ರತ್ಯಾಗಮ "run";

return_dead:
    ಯದಿ (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    ಪ್ರತ್ಯಾಗಮ "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    ಪೂರ್ಣಾಙ್ಕ		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    ಯದಿ (dont_check_job_ended > 0)
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    ಯದಿ (wait_pid <= 0)
	// no process ended
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
    ಪ್ರತಿ (job = job_list; job != ರಿಕ್ತ; job = job->jv_next)
    {
	ಯದಿ (job->jv_pid == wait_pid)
	{
	    ಯದಿ (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    ಅನ್ಯಥಾ ಯದಿ (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    ಯದಿ (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    ಪ್ರತ್ಯಾಗಮ job;
	}
    }
    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_signal_job(job_T *job, char_u *how)
{
    ಪೂರ್ಣಾಙ್ಕ	    sig = -1;

    ಯದಿ (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(how, "int") == 0)
	sig = SIGINT;
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    ಅನ್ಯಥಾ ಯದಿ (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    ಅನ್ಯಥಾ ಯದಿ (SAFE_isdigit(*how))
	sig = atoi((ವರ್ಣ *)how);
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ FAIL;

    // Never kill ourselves!
    ಯದಿ (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Clear the data related to "job".
 */
    ಶೂನ್ಯ
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (ಶೂನ್ಯ)waitpid(job->jv_pid, ರಿಕ್ತ, WNOHANG);
}
#endif

#ಯದಿ defined(FEAT_TERMINAL)
    ಪೂರ್ಣಾಙ್ಕ
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    ಪೂರ್ಣಾಙ್ಕ		pty_master_fd = -1;
    ಪೂರ್ಣಾಙ್ಕ		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    ಯದಿ (pty_master_fd < 0 || pty_slave_fd < 0)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    ಯದಿ (channel == ರಿಕ್ತ)
    {
	close(pty_master_fd);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಯದಿ (job->jv_tty_out != ರಿಕ್ತ)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಙ್ಕ force)
{
    ಯದಿ ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ರಿಕ್ತ, ರಿಕ್ತ))
	fill_ಒಳಹರಿವು_buf(FALSE);
}

/*
 * Wait "msec" msec until a character is available from the mouse, keyboard,
 * from inbuf[].
 * "msec" == -1 will block forever.
 * Invokes timer callbacks when needed.
 * When "ignore_input" is TRUE even check for pending input when input is
 * already available.
 * "interrupted" (if not NULL) is set to TRUE when no character is available
 * but something else needs to be done.
 * Returns TRUE when a character is available.
 * When a GUI is being used, this will never get called -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WaitForChar(long msec, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು)
{
#ifdef FEAT_TIMERS
    ಪ್ರತ್ಯಾಗಮ ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_ಒಳಹರಿವು) == OK;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ WaitForCharOrMouse(msec, interrupted, ignore_ಒಳಹರಿವು);
#endif
}

/*
 * Wait "msec" msec until a character is available from the mouse or keyboard
 * or from inbuf[].
 * "msec" == -1 will block forever.
 * for "ignore_input" see WaitForCharOr().
 * "interrupted" (if not NULL) is set to TRUE when no character is available
 * but something else needs to be done.
 * When a GUI is being used, this will never get called -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
WaitForCharOrMouse(long msec, ಪೂರ್ಣಾಙ್ಕ *interrupted, ಪೂರ್ಣಾಙ್ಕ ignore_ಒಳಹರಿವು)
{
#ifdef FEAT_MOUSE_GPM
    ಪೂರ್ಣಾಙ್ಕ		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    ಪೂರ್ಣಾಙ್ಕ		rest;
#endif
    ಪೂರ್ಣಾಙ್ಕ		avail;

    ಯದಿ (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_available())	    // something in inbuf[]
	ಪ್ರತ್ಯಾಗಮ 1;

#ಯದಿ defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    ಯದಿ (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	ಯದಿ (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#ಯದಿ defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    ಯದಿ (do_xterm_trace())
	rest = msec;
# endif
    ಕುರು
    {
# ifdef FEAT_XCLIPBOARD
	ಯದಿ (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    ಯದಿ (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    ಯದಿ (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	ಯದಿ (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	ಯದಿ (!avail && !gpm_process_wanted)
# ಅನ್ಯಥಾ
	avail = RealWaitForChar(read_cmd_fd, msec, ರಿಕ್ತ, interrupted);
	ಯದಿ (!avail)
# endif
	{
	    ಯದಿ (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_available())
		ಪ್ರತ್ಯಾಗಮ 1;
# ifdef FEAT_XCLIPBOARD
	    ಯದಿ (rest == 0 || !do_xterm_trace())
# endif
		ಭಙ್ಗ;
	}
    }
    ಯಾವತ್ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#ಅನ್ಯಥಾ
    avail = RealWaitForChar(read_cmd_fd, msec, ರಿಕ್ತ, interrupted);
#endif
    ಪ್ರತ್ಯಾಗಮ avail;
}

#ifndef VMS
/*
 * Wait "msec" msec until a character is available from file descriptor "fd".
 * "msec" == 0 will check for characters once.
 * "msec" == -1 will block until a character is available.
 * When a GUI is being used, this will not be used for input -- webb
 * Or when a Linux GPM mouse event is waiting.
 * Or when a clientserver message is on the queue.
 * "interrupted" (if not NULL) is set to TRUE when no character is available
 * but something else needs to be done.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
RealWaitForChar(ಪೂರ್ಣಾಙ್ಕ fd, long msec, ಪೂರ್ಣಾಙ್ಕ *check_for_gpm UNUSED, ಪೂರ್ಣಾಙ್ಕ *interrupted)
{
    ಪೂರ್ಣಾಙ್ಕ		ret;
    ಪೂರ್ಣಾಙ್ಕ		result;
# ಯದಿ defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    ಯದಿ (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    ಯದಿ (busy)
	ಪ್ರತ್ಯಾಗಮ 0;
# endif

# ifdef MAY_LOOP
    ಪ್ರತಿ (;;)
# endif
    {
# ifdef MAY_LOOP
	ಪೂರ್ಣಾಙ್ಕ		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	ಪೂರ್ಣಾಙ್ಕ		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	ಪೂರ್ಣಾಙ್ಕ		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ಸಂರಚನಾ pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	ಪೂರ್ಣಾಙ್ಕ		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	ಪೂರ್ಣಾಙ್ಕ             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	ಪೂರ್ಣಾಙ್ಕ		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಪೂರ್ಣಾಙ್ಕ		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	ಪೂರ್ಣಾಙ್ಕ		xsmp_idx = -1;
#  endif
	ಪೂರ್ಣಾಙ್ಕ		towait = (ಪೂರ್ಣಾಙ್ಕ)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	ಯದಿ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (ಪೂರ್ಣಾಙ್ಕ)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	ಯದಿ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	ಯದಿ (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_ಸಂಖ್ಯೆ(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಯದಿ (check_for_gpm != ರಿಕ್ತ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	ಯದಿ (xsmp_icefd != -1)
	{
	    xsmp_idx = nfd;
	    fds[nfd].fd = xsmp_icefd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	nfd = channel_poll_setup(nfd, &fds, &towait);
#  endif
	ಯದಿ (interrupted != ರಿಕ್ತ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	ಯದಿ (result == 0 && interrupted != ರಿಕ್ತ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	ಯದಿ (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ಯದಿ (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	ಯದಿ (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    ಯದಿ (--ret == 0 && !ಒಳಹರಿವು_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಯದಿ (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	ಯದಿ (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    ಯದಿ (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    ಅನ್ಯಥಾ ಯದಿ (fds[xsmp_idx].revents & POLLHUP)
	    {
		ಯದಿ (p_verbose > 0)
		    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    ಯದಿ (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ಯದಿ (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# ಅನ್ಯಥಾ // HAVE_SELECT

	ಸಂರಚನಾ timeval  tv;
	ಸಂರಚನಾ timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ಸ್ಥಿರ fd_set	rfds, wfds, efds;
	ಪೂರ್ಣಾಙ್ಕ		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	ಯದಿ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	ಯದಿ (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	ಅನ್ಯಥಾ
	    tvp = ರಿಕ್ತ;

	/*
	 * Select on ready for reading and exceptional condition (end of file).
	 */
select_eintr:
	FD_ZERO(&rfds);
	FD_ZERO(&wfds);
	FD_ZERO(&efds);
	FD_SET(fd, &rfds);
#  ifndef __QNX__
	// For QNX select() always returns 1 if this is set.  Why?
	FD_SET(fd, &efds);
#  endif
	maxfd = fd;

#  ifdef FEAT_WAYLAND
	ಯದಿ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    ಯದಿ (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	ಯದಿ (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_ಸಂಖ್ಯೆ(xterm_dpy), &rfds);
	    ಯದಿ (maxfd < Connection_ಸಂಖ್ಯೆ(xterm_dpy))
		maxfd = Connection_ಸಂಖ್ಯೆ(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಯದಿ (check_for_gpm != ರಿಕ್ತ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    ಯದಿ (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	ಯದಿ (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    ಯದಿ (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	ಯದಿ (interrupted != ರಿಕ್ತ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	ಯದಿ (result)
	    --ret;
	ಅನ್ಯಥಾ ಯದಿ (interrupted != ರಿಕ್ತ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	ಯದಿ (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    ಯದಿ (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    ಯದಿ (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ರಿಕ್ತ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    ಗಚ್ಛ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	ಯದಿ (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	ಯದಿ (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ಯದಿ (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	ಯದಿ (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_ಸಂಖ್ಯೆ(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    ಯದಿ (--ret == 0 && !ಒಳಹರಿವು_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಯದಿ (ret > 0 && check_for_gpm != ರಿಕ್ತ && gpm_flag && gpm_fd >= 0)
	{
	    ಯದಿ (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    ಅನ್ಯಥಾ ಯದಿ (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	ಯದಿ (ret > 0 && xsmp_icefd != -1)
	{
	    ಯದಿ (FD_ISSET(xsmp_icefd, &efds))
	    {
		ಯದಿ (p_verbose > 0)
		    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
		xsmp_close();
		ಯದಿ (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    ಅನ್ಯಥಾ ಯದಿ (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		ಯದಿ (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ಯದಿ (ret >= 0)
	    (ಶೂನ್ಯ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	ಯದಿ (finished || msec == 0)
	    ಭಙ್ಗ;

#  ಯದಿ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	ಯದಿ (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    ಭಙ್ಗ;
#  endif

	// We're going to loop around again, find out for how long
	ಯದಿ (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  ಅನ್ಯಥಾ
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    ಯದಿ (msec <= 0)
		ಭಙ್ಗ;	// waited long enough
	}
# endif
    }

    ಪ್ರತ್ಯಾಗಮ result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    ಪೂರ್ಣಾಙ್ಕ		flags)		// EW_* flags
{
    ಪ್ರತ್ಯಾಗಮ unix_expandpath(gap, path, 0, flags, FALSE);
}

/*
 * mch_expand_wildcards() - this code does wild-card pattern matching using
 * the shell
 *
 * return OK for success, FAIL for error (you may lose some memory) and put
 * an error message in *file.
 *
 * num_pat is number of input patterns
 * pat is array of pointers to input patterns
 * num_file is pointer to number of matched file names
 * file is pointer to array of pointers to matched file names
 */

# ifndef SEEK_SET
#  define SEEK_SET 0
# endif
# ifndef SEEK_END
#  define SEEK_END 2
# endif

# define SHELL_SPECIAL (char_u *)"\t \"&'$;<>()\\|\n"

    ಪೂರ್ಣಾಙ್ಕ
mch_expand_wildcards(
    ಪೂರ್ಣಾಙ್ಕ		   ಅಂಕೆ_pat,
    char_u	 **pat,
    ಪೂರ್ಣಾಙ್ಕ		  *ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಙ್ಕ		   flags)	// EW_* flags
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    size_t	len;
    long	llen;
    char_u	*p;
    ಪೂರ್ಣಾಙ್ಕ		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    ಪೂರ್ಣಾಙ್ಕ		j;
    char_u	*tempname;
    size_t	tempnamelen;
    char_u	*command;
    size_t	commandlen;
    FILE	*fd;
    char_u	*buffer;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    ಪೂರ್ಣಾಙ್ಕ		shell_style = STYLE_ECHO;
    ಪೂರ್ಣಾಙ್ಕ		check_spaces;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	did_find_nul = FALSE;
    ಪೂರ್ಣಾಙ್ಕ		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ಸ್ಥಿರ string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ಸ್ಥಿರ string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *ಅಂಕೆ_file = 0;	// default: no files found
    *file = ರಿಕ್ತ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    ಯದಿ (!have_wildcard(ಅಂಕೆ_pat, pat))
	ಪ್ರತ್ಯಾಗಮ save_patterns(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    ಯದಿ (sandbox != 0 && check_secure())
	ಪ್ರತ್ಯಾಗಮ FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    ಯದಿ (secure || restricted)
	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
	    ಯದಿ (vim_strchr(pat[i], '`') != ರಿಕ್ತ
		    && (check_restricted() || check_secure()))
		ಪ್ರತ್ಯಾಗಮ FAIL;

    /*
     * get a name for the temp file
     */
    ಯದಿ ((tempname = vim_tempname('o', FALSE)) == ರಿಕ್ತ)
    {
	emsg(_(e_cant_get_temp_file_name));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    /*
     * Let the shell expand the patterns and write the result into the temp
     * file.
     * STYLE_BT:	NL separated
     *	    If expanding `cmd` execute it directly.
     * STYLE_GLOB:	NUL separated
     *	    If we use *csh, "glob" will work better than "echo".
     * STYLE_PRINT:	NL or NUL separated
     *	    If we use *zsh, "print -N" will work better than "glob".
     * STYLE_VIMGLOB:	NL separated
     *	    If we use *sh*, we define "vimglob()".
     * STYLE_GLOBSTAR:	NL separated
     *	    If we use *bash*, we define "vimglob() and enable globstar option".
     * STYLE_ECHO:	space separated.
     *	    A shell we don't know, stay safe and use "echo".
     */
    ಯದಿ (ಅಂಕೆ_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    ಅನ್ಯಥಾ ಯದಿ ((len = STRLEN(p_sh)) >= 3)
    {
	ಯದಿ (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	ಅನ್ಯಥಾ ಯದಿ (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    ಯದಿ (shell_style == STYLE_ECHO)
    {
	ಯದಿ (strstr((ವರ್ಣ *)gettail(p_sh), "bash") != ರಿಕ್ತ)
	    shell_style = STYLE_GLOBSTAR;
	ಅನ್ಯಥಾ ಯದಿ (strstr((ವರ್ಣ *)gettail(p_sh), "sh") != ರಿಕ್ತ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    ಯದಿ (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# ಅನ್ಯಥಾ
	++len;				// add space
	ಪ್ರತಿ (j = 0; pat[i][j] != NUL; ++j)
	{
	    ಯದಿ (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ರಿಕ್ತ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    ಯದಿ (command == ರಿಕ್ತ)
    {
	// out of memory
	vim_free(tempname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    ಯದಿ (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((ವರ್ಣ *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((ವರ್ಣ *)command, ")>");
	ಯದಿ (p == ರಿಕ್ತ)
	{
	    ;					    // can't happen ;-)
	}
	ಅನ್ಯಥಾ
	{
	    --p;
	    ಯಾವತ್ (p > command && VIM_ISWHITE(*p))
		--p;
	    ಯದಿ (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    ಯಾವತ್ (p > command && VIM_ISWHITE(*p))
		--p;
	    ಯದಿ (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((ವರ್ಣ *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((ವರ್ಣ *)command, len, "print -N >%s", tempname);
	ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((ವರ್ಣ *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((ವರ್ಣ *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	ಅನ್ಯಥಾ
	    commandlen = vim_snprintf((ವರ್ಣ *)command, len, "echo >%s", tempname);

	ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((ವರ್ಣ *)command + commandlen, len, " \"%s\"", pat[i]);
# ಅನ್ಯಥಾ
	    ಪೂರ್ಣಾಙ್ಕ intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    ಪ್ರತಿ (j = 0; pat[i][j] != NUL; ++j)
	    {
		ಯದಿ (pat[i][j] == '`')
		    intick = !intick;
		ಅನ್ಯಥಾ ಯದಿ (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    ಯದಿ (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ರಿಕ್ತ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		ಅನ್ಯಥಾ ಯದಿ (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ರಿಕ್ತ)
		    // Put a backslash before a special character, but not
		    // when inside ``. And not for $var when EW_KEEPDOLLAR is
		    // set.
		    *p++ = '\\';

		// Copy one character.
		*p++ = pat[i][j];
	    }
	    *p = NUL;
	    commandlen = (size_t)(p - command);
# endif
	}
    }
    ಯದಿ (flags & EW_SILENT)
	show_shell_mess = FALSE;
    ಯದಿ (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    ಯದಿ (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_GLOB && !have_dollars(ಅಂಕೆ_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    ಯದಿ (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ರಿಕ್ತ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    ಯದಿ (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	ಯದಿ (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    ಸಂದೇಶ_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    ಯದಿ (!(flags & EW_SILENT))
# endif
	    {
		ಸಂದೇಶ(_(e_cannot_expand_wildcards));
		ಸಂದೇಶ_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	ಯದಿ (shell_style == STYLE_BT)
	    ಪ್ರತ್ಯಾಗಮ FAIL;
	ಗಚ್ಛ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((ವರ್ಣ *)tempname, READBIN);
    ಯದಿ (fd == ರಿಕ್ತ)
    {
	// Something went wrong, perhaps a file name with a special char.
	ಯದಿ (!(flags & EW_SILENT))
	{
	    ಸಂದೇಶ(_(e_cannot_expand_wildcards));
	    ಸಂದೇಶ_start();		// don't overwrite this message
	}
	vim_free(tempname);
	ಗಚ್ಛ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    ಯದಿ (llen < 0)
	// just in case ftell() would fail
	buffer = ರಿಕ್ತ;
    ಅನ್ಯಥಾ
	buffer = alloc(llen + 1);
    ಯದಿ (buffer == ರಿಕ್ತ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    len = llen;
    i = fread((ವರ್ಣ *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    ಯದಿ (i != (ಪೂರ್ಣಾಙ್ಕ)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    ಪ್ರತಿ (i = 0; i < (ಪೂರ್ಣಾಙ್ಕ)len; ++i)
	ಯದಿ (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    ಯದಿ (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	ಪ್ರತಿ (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    ಯಾವತ್ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    ಅನ್ಯಥಾ ಯದಿ (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	ಪ್ರತಿ (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    ಯಾವತ್ (*p != '\n' && *p != NUL)
		++p;
	    ಯದಿ (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    ಅನ್ಯಥಾ
    {
	/*
	 * Some versions of zsh use spaces instead of NULs to separate
	 * results.  Only do this when there is no NUL before the end of the
	 * buffer, otherwise we would never be able to use file names with
	 * embedded spaces when zsh does use NULs.
	 * When we found a NUL once, we know zsh is OK, set did_find_nul and
	 * don't check for spaces again.
	 */
	check_spaces = FALSE;
	ಯದಿ (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    ಯದಿ (len && (ಪೂರ್ಣಾಙ್ಕ)STRLEN(buffer) < (ಪೂರ್ಣಾಙ್ಕ)len)
		did_find_nul = TRUE;
	    ಅನ್ಯಥಾ
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	ಯದಿ (len && buffer[len - 1] == NUL)
	    --len;
	ಅನ್ಯಥಾ
	    buffer[len] = NUL;
	i = 0;
	ಪ್ರತಿ (p = buffer; p < buffer + len; ++p)
	    ಯದಿ (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	ಯದಿ (len)
	    ++i;			// count last entry
    }
    ಯದಿ (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	ಗಚ್ಛ notfound;
    }
    *ಅಂಕೆ_file = i;
    *file = ALLOC_MULT(char_u *, i);
    ಯದಿ (*file == ರಿಕ್ತ)
    {
	// out of memory
	vim_free(buffer);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    ಪ್ರತಿ (i = 0; i < *ಅಂಕೆ_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	ಯದಿ (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    ಯಾವತ್ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    ಯದಿ (p == buffer + len)		// last entry
		*p = NUL;
	    ಅನ್ಯಥಾ
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	ಅನ್ಯಥಾ		// NUL separates
	{
	    ಯಾವತ್ (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    ಪ್ರತಿ (j = 0, i = 0; i < *ಅಂಕೆ_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	ಯದಿ (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ಅನುವರ್ತ;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	ಯದಿ ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ಅನುವರ್ತ;

	// Skip files that are not executable if we check for that.
	ಯದಿ (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ರಿಕ್ತ, !(flags & EW_SHELLCMD)))
	    ಅನುವರ್ತ;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	ಯದಿ (p)
	{
	    STRCPY(p, (*file)[i]);
	    ಯದಿ (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *ಅಂಕೆ_file = j;

    ಯದಿ (*ಅಂಕೆ_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	ಗಚ್ಛ notfound;
    }

    ಪ್ರತ್ಯಾಗಮ OK;

notfound:
    ಯದಿ (flags & EW_NOTFOUND)
	ಪ್ರತ್ಯಾಗಮ save_patterns(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

#endif // VMS

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
save_patterns(
    ಪೂರ್ಣಾಙ್ಕ		ಅಂಕೆ_pat,
    char_u	**pat,
    ಪೂರ್ಣಾಙ್ಕ		*ಅಂಕೆ_file,
    char_u	***file)
{
    ಪೂರ್ಣಾಙ್ಕ		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, ಅಂಕೆ_pat);
    ಯದಿ (*file == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FAIL;
    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	s = vim_strsave(pat[i]);
	ಯದಿ (s != ರಿಕ್ತ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *ಅಂಕೆ_file = ಅಂಕೆ_pat;
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_has_exp_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ
	    ಯದಿ (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#ಅನ್ಯಥಾ
				    "*?[{'"
#endif
						, *p) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_has_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ
	    ಯದಿ (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#ಅನ್ಯಥಾ
				    "*?[{`'$"
#endif
						, *p) != ರಿಕ್ತ
		|| (*p == '~' && p[1] != NUL))
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
have_wildcard(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, char_u **file)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ; i++)
	ಯದಿ (mch_has_wildcard(file[i]))
	    ಪ್ರತ್ಯಾಗಮ 1;
    ಪ್ರತ್ಯಾಗಮ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
have_dollars(ಪೂರ್ಣಾಙ್ಕ ಅಂಕೆ, char_u **file)
{
    ಪೂರ್ಣಾಙ್ಕ	    i;

    ಪ್ರತಿ (i = 0; i < ಅಂಕೆ; i++)
	ಯದಿ (vim_strchr(file[i], '$') != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

#ಯದಿ !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_rename(ಧ್ರುವ ವರ್ಣ *src, ಧ್ರುವ ವರ್ಣ *dest)
{
    ಸಂರಚನಾ stat	    st;

    ಯದಿ (stat(dest, &st) >= 0)	    // fail if destination exists
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (link(src, dest) != 0)	    // link file to new name
	ಪ್ರತ್ಯಾಗಮ -1;
    ಯದಿ (mch_remove(src) == 0)	    // delete link to old name
	ಪ್ರತ್ಯಾಗಮ 0;
    ಪ್ರತ್ಯಾಗಮ -1;
}
#endif // !HAVE_RENAME

#ಯದಿ defined(FEAT_MOUSE_GPM)
# ಯದಿ defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
load_libgpm(ಶೂನ್ಯ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    ಯದಿ (libgpm_hinst == ರಿಕ್ತ)
    {
	ಯದಿ (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಯದಿ (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ರಿಕ್ತ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ರಿಕ್ತ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ರಿಕ್ತ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ರಿಕ್ತ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ರಿಕ್ತ
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ರಿಕ್ತ;
	dll_gpm_flag = ರಿಕ್ತ;
	dll_gpm_fd   = ರಿಕ್ತ;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಪ್ರತ್ಯಾಗಮ OK;
}

    ಪೂರ್ಣಾಙ್ಕ
gpm_available(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ libgpm_hinst != ರಿಕ್ತ || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
gpm_open(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    ಯದಿ (!gpm_available())
	ಪ್ರತ್ಯಾಗಮ 0;
# endif

    ಯದಿ (gpm_flag)
	ಪ್ರತ್ಯಾಗಮ 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    ಯದಿ (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	ಪ್ರತ್ಯಾಗಮ 1; // succeed
    }
    ಯದಿ (gpm_fd == -2)
    {
	Gpm_Close(); // We don't want to talk to xterm via gpm

	// Gpm_Close fails to properly restore the WINCH and TSTP handlers,
	// leading to Vim ignoring resize signals. We have to re-initialize
	// these handlers again here.
# ifdef SIGWINCH
	mch_signal(SIGWINCH, sig_winch);
# endif
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
    }
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    ಪೂರ್ಣಾಙ್ಕ
gpm_enabled(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gpm_close(ಶೂನ್ಯ)
{
    ಯದಿ (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
mch_gpm_process(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ			button;
    ಸ್ಥಿರ Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    ಪೂರ್ಣಾಙ್ಕ			row,col;
    unsigned ವರ್ಣ	buttons_mask;
    unsigned ವರ್ಣ	gpm_modifiers;
    ಸ್ಥಿರ unsigned ವರ್ಣ old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ಯದಿ (hold_gui_events)
	ಪ್ರತ್ಯಾಗಮ 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    ವಿಕಲ್ಪ (GPM_BARE_EVENTS(gpm_event.type))
    {
	ಪಕ್ಷ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ಭಙ್ಗ;
	ಪಕ್ಷ GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    ವಿಕಲ್ಪ (buttons_mask)
	    {
		ಪಕ್ಷ GPM_B_ಎಡ:
		    button = MOUSE_ಎಡ;
		    ಭಙ್ಗ;
		ಪಕ್ಷ GPM_B_ಮಧ್ಯ:
		    button = MOUSE_ಮಧ್ಯ;
		    ಭಙ್ಗ;
		ಪಕ್ಷ GPM_B_ಬಲ:
		    button = MOUSE_ಬಲ;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    ಪ್ರತ್ಯಾಗಮ 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_ಅಂಕೆ_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ಭಙ್ಗ;
	ಪಕ್ಷ GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ಭಙ್ಗ;
	ಸ್ವಭಾವ:
	    ಪ್ರತ್ಯಾಗಮ 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    ಯದಿ (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    ಯದಿ (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    ಯದಿ (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_ಒಳಹರಿವು_buf(string, 6);
    ಪ್ರತ್ಯಾಗಮ 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sysmouse_open(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.ಸಙ್ಕೇತ = SIGUSR2;
    ಯದಿ (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    ಪ್ರತ್ಯಾಗಮ OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sysmouse_close(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.ಸಙ್ಕೇತ = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_sysmouse SIGDEFARG(sigarg)
{
    ಸಂರಚನಾ mouse_info	mouse;
    ಸಂರಚನಾ video_info	video;
    char_u		string[6];
    ಪೂರ್ಣಾಙ್ಕ			row, col;
    ಪೂರ್ಣಾಙ್ಕ			button;
    ಪೂರ್ಣಾಙ್ಕ			buttons;
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ಯದಿ (hold_gui_events)
	ಪ್ರತ್ಯಾಗಮ;
# endif

    mouse.operation = MOUSE_GETINFO;
    ಯದಿ (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	row = mouse.u.data.y / video.vi_cheight;
	col = mouse.u.data.x / video.vi_cwidth;
	buttons = mouse.u.data.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	ಯದಿ (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	ಅನ್ಯಥಾ
	{
	    ವಿಕಲ್ಪ (buttons)
	    {
		ಪಕ್ಷ 0:
		    button = MOUSE_RELEASE;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 1:
		    button = MOUSE_ಎಡ;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 2:
		    button = MOUSE_ಮಧ್ಯ;
		    ಭಙ್ಗ;
		ಪಕ್ಷ 4:
		    button = MOUSE_ಬಲ;
		    ಭಙ್ಗ;
		ಸ್ವಭಾವ:
		    ಪ್ರತ್ಯಾಗಮ;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_ಒಳಹರಿವು_buf(string, 6);
    }
    ಪ್ರತ್ಯಾಗಮ;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dev_urandom_state = NOTDONE;

    ಯದಿ (dev_urandom_state == FAIL)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಪೂರ್ಣಾಙ್ಕ fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    ಯದಿ (fd == -1)
	dev_urandom_state = FAIL;
    ಅನ್ಯಥಾ ಯದಿ (ಪಠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    ಅನ್ಯಥಾ
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    ಪ್ರತ್ಯಾಗಮ dev_urandom_state;
}

#ಯದಿ defined(FEAT_LIBCALL)
ಪ್ರಕಾರನಿರ್ದೇಶ char_u * (*STRPROCSTR)(char_u *);
ಪ್ರಕಾರನಿರ್ದೇಶ char_u * (*INTPROCSTR)(ಪೂರ್ಣಾಙ್ಕ);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*STRPROCINT)(char_u *);
ಪ್ರಕಾರನಿರ್ದೇಶ ಪೂರ್ಣಾಙ್ಕ (*INTPROCINT)(ಪೂರ್ಣಾಙ್ಕ);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ಪೂರ್ಣಾಙ್ಕ		argint,
    char_u	**string_result, // NULL when using number_result
    ಪೂರ್ಣಾಙ್ಕ		*ಸಂಖ್ಯೆ_result)
{
# ಯದಿ defined(USE_DLOPEN)
    ಶೂನ್ಯ	*hinstLib;
    ವರ್ಣ	*dlerr = ರಿಕ್ತ;
# ಅನ್ಯಥಾ
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		retval_int = 0;
    ಪೂರ್ಣಾಙ್ಕ		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# ಯದಿ defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (ಶೂನ್ಯ)dlerror();

    hinstLib = dlopen((ವರ್ಣ *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    ಯದಿ (hinstLib == ರಿಕ್ತ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	ಯದಿ (dlerr != ರಿಕ್ತ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# ಅನ್ಯಥಾ
    hinstLib = shl_load((ಧ್ರುವ ವರ್ಣ*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    ಯದಿ (hinstLib != ರಿಕ್ತ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	ಯದಿ (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  ಯದಿ defined(USE_DLOPEN)
	    dlerr = ರಿಕ್ತ;
#  endif
	    mch_didjmp();
	}
	ಅನ್ಯಥಾ
# endif
	{
	    retval_str = ರಿಕ್ತ;
	    retval_int = 0;

	    ಯದಿ (argstring != ರಿಕ್ತ)
	    {
# ಯದಿ defined(USE_DLOPEN)
		*(ಶೂನ್ಯ **)(&ProcAdd) = dlsym(hinstLib, (ಧ್ರುವ ವರ್ಣ *)funcname);
		dlerr = dlerror();
# ಅನ್ಯಥಾ
		ಯದಿ (shl_findsym(&hinstLib, (ಧ್ರುವ ವರ್ಣ *)funcname,
					TYPE_PROCEDURE, (ಶೂನ್ಯ *)&ProcAdd) < 0)
		    ProcAdd = ರಿಕ್ತ;
# endif
		ಯದಿ ((success = (ProcAdd != ರಿಕ್ತ
# ಯದಿ defined(USE_DLOPEN)
			    && dlerr == ರಿಕ್ತ
# endif
			    )))
		{
		    ಯದಿ (string_result == ರಿಕ್ತ)
			retval_int = ((STRPROCINT)(ಶೂನ್ಯ *)ProcAdd)(argstring);
		    ಅನ್ಯಥಾ
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    ಅನ್ಯಥಾ
	    {
# ಯದಿ defined(USE_DLOPEN)
		*(ಶೂನ್ಯ **)(&ProcAddI) = dlsym(hinstLib, (ಧ್ರುವ ವರ್ಣ *)funcname);
		dlerr = dlerror();
# ಅನ್ಯಥಾ
		ಯದಿ (shl_findsym(&hinstLib, (ಧ್ರುವ ವರ್ಣ *)funcname,
				       TYPE_PROCEDURE, (ಶೂನ್ಯ *)&ProcAddI) < 0)
		    ProcAddI = ರಿಕ್ತ;
# endif
		ಯದಿ ((success = (ProcAddI != ರಿಕ್ತ
# ಯದಿ defined(USE_DLOPEN)
			    && dlerr == ರಿಕ್ತ
# endif
			    )))
		{
		    ಯದಿ (string_result == ರಿಕ್ತ)
			retval_int = ((INTPROCINT)(ಶೂನ್ಯ *)ProcAddI)(argint);
		    ಅನ್ಯಥಾ
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    ಯದಿ (string_result == ರಿಕ್ತ)
		*ಸಂಖ್ಯೆ_result = retval_int;
	    ಅನ್ಯಥಾ ಯದಿ (retval_str != ರಿಕ್ತ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	ಯದಿ (lc_signal != 0)
	{
	    ಪೂರ್ಣಾಙ್ಕ i;

	    // try to find the name of this signal
	    ಪ್ರತಿ (i = 0; signal_info[i].sig != -1; i++)
		ಯದಿ (lc_signal == signal_info[i].sig)
		    ಭಙ್ಗ;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# ಯದಿ defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	ಯದಿ (dlerr != ರಿಕ್ತ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(ಶೂನ್ಯ)dlclose(hinstLib);
# ಅನ್ಯಥಾ
	(ಶೂನ್ಯ)shl_unload(hinstLib);
# endif
    }

    ಯದಿ (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
}
#endif

#ಯದಿ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	xterm_trace = -1;	// default: disabled
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    ಶೂನ್ಯ
setup_term_clip(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಙ್ಕ		z = 0;
    ವರ್ಣ	*strp = "";
    Widget	AppShell;

    ಯದಿ (!x_connect_to_server())
	ಪ್ರತ್ಯಾಗಮ;

    open_app_context();
    ಯದಿ (app_context != ರಿಕ್ತ && xterm_Shell == (Widget)0)
    {
	ಪೂರ್ಣಾಙ್ಕ (*oldhandler)(Display*, XErrorEvent*);
# ಯದಿ defined(USING_SETJMP)
	ಪೂರ್ಣಾಙ್ಕ (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	ಯದಿ (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# ಯದಿ defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	ಯದಿ (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ರಿಕ್ತ;
	}
	ಅನ್ಯಥಾ
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ರಿಕ್ತ, 0, &z, &strp);
	    ಯದಿ (xterm_dpy != ರಿಕ್ತ)
		xterm_dpy_retry_ಎಣಿಕೆ = 0;
# ಯದಿ defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# ಯದಿ defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(ಶೂನ್ಯ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(ಶೂನ್ಯ)XSetErrorHandler(oldhandler);

	ಯದಿ (xterm_dpy == ರಿಕ್ತ)
	{
	    ಯದಿ (p_verbose > 0)
		verb_ಸಂದೇಶ(_("Opening the X display failed"));
	    ಪ್ರತ್ಯಾಗಮ;
	}

	// Catch terminating error of the X server connection.
	(ಶೂನ್ಯ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	ಯದಿ (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ರಿಕ್ತ);
	ಯದಿ (AppShell == (Widget)0)
	    ಪ್ರತ್ಯಾಗಮ;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		ತುದಿ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ರಿಕ್ತ);
	ಯದಿ (xterm_Shell == (Widget)0)
	    ಪ್ರತ್ಯಾಗಮ;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	ಯದಿ (x11_display == ರಿಕ್ತ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    ಯದಿ (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	ಯದಿ (x11_window == 0 && (strp = getenv("WINDOWID")) != ರಿಕ್ತ)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	ಯದಿ (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	ಯದಿ (x11_window != 0)
	    xterm_trace = 0;
    }
}

    ಶೂನ್ಯ
start_xterm_trace(ಪೂರ್ಣಾಙ್ಕ button)
{
    ಯದಿ (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	ಪ್ರತ್ಯಾಗಮ;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    ಶೂನ್ಯ
stop_xterm_trace(ಶೂನ್ಯ)
{
    ಯದಿ (xterm_trace < 0)
	ಪ್ರತ್ಯಾಗಮ;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
do_xterm_trace(ಶೂನ್ಯ)
{
    Window		ಬೇರು, child;
    ಪೂರ್ಣಾಙ್ಕ			ಬೇರು_x, ಬೇರು_y;
    ಪೂರ್ಣಾಙ್ಕ			win_x, win_y;
    ಪೂರ್ಣಾಙ್ಕ			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    ಸ್ಥಿರ char_u	*mouse_code = ರಿಕ್ತ;
    ಸ್ಥಿರ size_t	mouse_codelen = 0;
    ಸ್ಥಿರ char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		prev_row = 0, prev_col = 0;
    ಸ್ಥಿರ XSizeHints	xterm_hints;

    ಯದಿ (xterm_trace <= 0)
	ಪ್ರತ್ಯಾಗಮ FALSE;

    ಯದಿ (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	ಯದಿ (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    ಪ್ರತ್ಯಾಗಮ FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	ಯದಿ (mouse_code != ರಿಕ್ತ)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &ಬೇರು, &child, &ಬೇರು_x, &ಬೇರು_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	ಯದಿ (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	ಯದಿ (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	ಪ್ರತ್ಯಾಗಮ TRUE;
    }

    ಯದಿ (mouse_code == ರಿಕ್ತ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	ಪ್ರತ್ಯಾಗಮ FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &ಬೇರು, &child, &ಬೇರು_x, &ಬೇರು_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    ಯದಿ (row == prev_row && col == prev_col)
	ಪ್ರತ್ಯಾಗಮ TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_ಒಳಹರಿವು_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

# ಯದಿ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    ಶೂನ್ಯ
clear_xterm_clip(ಶೂನ್ಯ)
{
    ಯದಿ (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    ಯದಿ (xterm_dpy != ರಿಕ್ತ)
    {
#  ಯದಿ 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	ಯದಿ (x11_display == xterm_dpy)
	    x11_display = ರಿಕ್ತ;
	xterm_dpy = ರಿಕ್ತ;
    }
#  ಯದಿ 0
    ಯದಿ (app_context != (XtAppContext)ರಿಕ್ತ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ರಿಕ್ತ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_update(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    ಯದಿ (gui.in_use)
	gui_mch_update();
    ಅನ್ಯಥಾ
# endif
    ಯದಿ (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
xterm_update(ಶೂನ್ಯ)
{
    XEvent event;

    ಪ್ರತಿ (;;)
    {
	Xt_ಒಳಹರಿವು_Mask mask = XtAppPending(app_context);

	ಯದಿ (mask == 0 || vim_is_ಒಳಹರಿವು_buf_full())
	    ಭಙ್ಗ;

	ಯದಿ (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		ಯದಿ (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_ಮೌಲ್ಯ)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	ಅನ್ಯಥಾ
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    ಪೂರ್ಣಾಙ್ಕ
clip_xterm_own_selection(Clipboard_T *cbd)
{
    ಯದಿ (xterm_Shell != (Widget)0)
	ಪ್ರತ್ಯಾಗಮ clip_x11_own_selection(xterm_Shell, cbd);
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

    ಶೂನ್ಯ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    ಯದಿ (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    ಶೂನ್ಯ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    ಯದಿ (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    ಶೂನ್ಯ
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#ಯದಿ defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# ಯದಿ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ		save_cmod_flags;
    ಪೂರ್ಣಾಙ್ಕ		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    ಯದಿ (check_changed_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    out_flush();

    // Done interaction
    SmcInteractDone(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    ಯದಿ (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    ಪೂರ್ಣಾಙ್ಕ		save_type UNUSED,
    Bool	shutdown,
    ಪೂರ್ಣಾಙ್ಕ		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    ಯದಿ (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    ಯದಿ (p_verbose > 0)
	verb_ಸಂದೇಶ(_("XSMP handling save-yourself request"));

# ಯದಿ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    ಯದಿ (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    ಅನ್ಯಥಾ
# endif
    {
	// Can stop the cycle here
	SmcSaveYourselfDone(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_save_complete(
    SmcConn	smc_conn UNUSED,
    SmPointer	client_data UNUSED)
{
    xsmp.save_yourself = False;
}


/*
 * Callback to tell us that an instigated shutdown was cancelled
 * (maybe even by us)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    ಯದಿ (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    ಯದಿ (opening)
    {
	xsmp_icefd = Ice_Connection_ಸಂಖ್ಯೆ(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ರಿಕ್ತ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    ಪೂರ್ಣಾಙ್ಕ
xsmp_handle_requests(ಶೂನ್ಯ)
{
    Bool rep;

    ಯದಿ (IceProcessMessages(xsmp.iceconn, ರಿಕ್ತ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	ಯದಿ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
	xsmp_close();
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    ಅನ್ಯಥಾ
	ಪ್ರತ್ಯಾಗಮ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ dummy;

// Set up X Session Management Protocol
    ಶೂನ್ಯ
xsmp_init(ಶೂನ್ಯ)
{
    ವರ್ಣ		errorstring[80];
    SmcCallbacks	smcallbacks;
# ಯದಿ 0
    Sm_Prop_ಮೌಲ್ಯ		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    ಯದಿ (p_verbose > 0)
	verb_ಸಂದೇಶ(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = ರಿಕ್ತ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = ರಿಕ್ತ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = ರಿಕ್ತ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = ರಿಕ್ತ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    ಯದಿ (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	ಯದಿ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("XSMP ICE connection watch failed"));
	ಪ್ರತ್ಯಾಗಮ;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ರಿಕ್ತ,
	    ರಿಕ್ತ,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    ರಿಕ್ತ,
	    &xsmp.clientid,
	    ಪರಿಮಾಣ(errorstring) - 1,
	    errorstring);
    ಯದಿ (xsmp.smcconn == ರಿಕ್ತ)
    {
	ಯದಿ (p_verbose > 0)
	{
	    ವರ್ಣ errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[ಪರಿಮಾಣ(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, ಪರಿಮಾಣ(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_ಸಂದೇಶ(errorreport);
	}
	ಪ್ರತ್ಯಾಗಮ;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# ಯದಿ 0
    // ID ourselves
    smname.ಮೌಲ್ಯ = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.ಅಂಕೆ_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    ಶೂನ್ಯ
xsmp_close(ಶೂನ್ಯ)
{
    ಯದಿ (xsmp_icefd == -1)
	ಪ್ರತ್ಯಾಗಮ;

    SmcCloseConnection(xsmp.smcconn, 0, ರಿಕ್ತ);
    ಯದಿ (xsmp.clientid != ರಿಕ್ತ)
	ಮುಕ್ತ(xsmp.clientid);
    xsmp.clientid = ರಿಕ್ತ;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#ಯದಿ defined(FEAT_RELTIME)
# ಯದಿ defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t timeout_flag = FALSE;
ಸ್ಥಿರ timer_t		     timer_id;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_flag(ಸಙ್ಘ sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಸಂರಚನಾ itimerspec disarm = {{0, 0}, {0, 0}};

    ಯದಿ (timer_created)
    {
	ಪೂರ್ಣಾಙ್ಕ ret = timer_settime(timer_id, 0, &disarm, ರಿಕ್ತ);

	ಯದಿ (ret < 0)
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    // Clear the current timeout flag; any previous timeout should be
    // considered _not_ triggered.
    timeout_flag = FALSE;
}

/*
 * Start the timeout timer.
 *
 * The return value is a pointer to a flag that is initialised to FALSE. If the
 * timeout expires, the flag is set to TRUE. This will only return pointers to
 * static memory; i.e. any pointer returned by this function may always be
 * safely dereferenced.
 *
 * This function is not expected to fail, but if it does it will still return a
 * valid flag pointer; the flag will remain stuck as FALSE .
 */
    ಚಞ್ಚಲ sig_atomic_t *
start_timeout(long msec)
{
    ಸಂರಚನಾ itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    ಪೂರ್ಣಾಙ್ಕ ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    ಯದಿ (!timer_created)
    {
	ಸಂರಚನಾ sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	ಯದಿ (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    ಪ್ರತ್ಯಾಗಮ &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ರಿಕ್ತ, "setting timeout timer to %d sec %ld nsec",
	       (ಪೂರ್ಣಾಙ್ಕ)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ರಿಕ್ತ);
    ಯದಿ (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    ಪ್ರತ್ಯಾಗಮ &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    ಶೂನ್ಯ
delete_timer(ಶೂನ್ಯ)
{
    ಯದಿ (!timer_created)
	ಪ್ರತ್ಯಾಗಮ;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# ಅನ್ಯಥಾ // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ಸ್ಥಿರ ಸಂರಚನಾ sigaction		prev_sigaction;
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t	timeout_flag	     = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ			timer_active	     = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ			timer_handler_active = FALSE;
ಸ್ಥಿರ ಚಞ್ಚಲ sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
set_flag SIGDEFARG(sigarg)
{
    ಯದಿ (alarm_pending)
	alarm_pending = FALSE;
    ಅನ್ಯಥಾ
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ಸಂರಚನಾ itimerval disarm = {{0, 0}, {0, 0}};
    ಪೂರ್ಣಾಙ್ಕ			    ret;

    ಯದಿ (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ರಿಕ್ತ);
	ಯದಿ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    ಯದಿ (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ರಿಕ್ತ);
	ಯದಿ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_reset_handler_for_timeout_str),
							      strerror(errno));
    }
    timeout_flag = FALSE;
}

/*
 * Start the timeout timer.
 *
 * The return value is a pointer to a flag that is initialised to FALSE. If the
 * timeout expires, the flag is set to TRUE. This will only return pointers to
 * static memory; i.e. any pointer returned by this function may always be
 * safely dereferenced.
 *
 * This function is not expected to fail, but if it does it will still return a
 * valid flag pointer; the flag will remain stuck as FALSE .
 */
    ಚಞ್ಚಲ sig_atomic_t *
start_timeout(long msec)
{
    ಸಂರಚನಾ itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ಸಂರಚನಾ sigaction	handle_alarm;
    ಪೂರ್ಣಾಙ್ಕ			ret;
    sigset_t		sigs;
    sigset_t		saved_sigs;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    // There is a small chance that SIGALRM is pending and so the handler must
    // ignore it on the first call.
    alarm_pending = FALSE;
    ret = sigemptyset(&sigs);
    ret = ret == 0 ? sigaddset(&sigs, SIGALRM) : ret;
    ret = ret == 0 ? sigprocmask(SIG_BLOCK, &sigs, &saved_sigs) : ret;
    timeout_flag = FALSE;
    ret = ret == 0 ? sigpending(&sigs) : ret;
    ಯದಿ (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ರಿಕ್ತ);
    }
    ಯದಿ (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = set_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    ಯದಿ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	ಪ್ರತ್ಯಾಗಮ &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ರಿಕ್ತ);
    ಯದಿ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	ಪ್ರತ್ಯಾಗಮ &timeout_flag;
    }

    timer_active = TRUE;
    ಪ್ರತ್ಯಾಗಮ &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
