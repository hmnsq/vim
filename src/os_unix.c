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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
# include "if_mzsch.h"
#ਜੇ_ਅੰਤ

#ਅਨ੍ਤਰ੍ਭਾਵ "os_unixx.h"	    // unix includes for os_unix.c only

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
# include <X11/SM/SMlib.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SELINUX
# include <selinux/selinux.h>
ਸਥਿਰ ਪੂਰਨ selinux_enabled = -1;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XATTR
# include <sys/xattr.h>
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_ਔਸਤ
#  include <windows.h>
#  include "winclip.pro"
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM

# include <gpm.h>

# ifdef DYNAMIC_GPM
#  define Gpm_Open     (*dll_Gpm_Open)
#  define Gpm_Close    (*dll_Gpm_Close)
#  define Gpm_GetEvent (*dll_Gpm_GetEvent)
#  define gpm_flag     (dll_gpm_flag != ਨਿਰਾ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ਨਿਰਾ ? *dll_gpm_fd   : -1)

ਸਥਿਰ ਪੂਰਨ (*dll_Gpm_Open)     (Gpm_Connect *, ਪੂਰਨ);
ਸਥਿਰ ਪੂਰਨ (*dll_Gpm_Close)    (ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ (*dll_Gpm_GetEvent) (Gpm_Event *);
ਸਥਿਰ ਪੂਰਨ *dll_gpm_flag;
ਸਥਿਰ ਪੂਰਨ *dll_gpm_fd;

ਸਥਿਰ ਖਾਲੀ *libgpm_hinst;
# endif

// <linux/keyboard.h> contains defines conflicting with "keymap.h",
// I just copied relevant defines here. A cleaner solution would be to put gpm
// code into separate file and include there linux/keyboard.h
// #include <linux/keyboard.h>
# define KG_ਖਿਸਕਾਓ	0
# define KG_CTRL	2
# define KG_ALT		3
# define KG_ALTGR	1
# define KG_SHIFTL	4
# define KG_SHIFTR	5
# define KG_CTRLL	6
# define KG_CTRLR	7
# define KG_CAPSSHIFT	8

ਸਥਿਰ ਖਾਲੀ gpm_close(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ gpm_open(ਖਾਲੀ);
ਸਥਿਰ ਪੂਰਨ mch_gpm_process(ਖਾਲੀ);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ਸਥਿਰ ਪੂਰਨ sysmouse_open(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ sysmouse_close(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ sig_sysmouse SIGPROTOARG;
#ਜੇ_ਅੰਤ

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#ਜੇ defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
#  include <X11/Shell.h>
#  include <X11/StringDefs.h>
ਸਥਿਰ Widget	xterm_Shell = (Widget)0;
ਸਥਿਰ ਖਾਲੀ clip_update(ਖਾਲੀ);
# endif

ਖਿੜਕੀ	    x11_ਖਿੜਕੀ = 0;
Display	    *x11_display = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਪੂਰਨ ignore_sigtstp = FALSE;

ਸਥਿਰ ਪੂਰਨ get_x11_title(ਪੂਰਨ);

ਸਥਿਰ char_u	*oldtitle = ਨਿਰਾ;
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t oldtitle_outdated = FALSE;
ਸਥਿਰ ਪੂਰਨ	unix_did_ਸੈੱਟ_title = FALSE;
ਸਥਿਰ char_u	*oldicon = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	did_ਸੈੱਟ_icon = FALSE;

ਸਥਿਰ ਖਾਲੀ may_core_dump(ਖਾਲੀ);

ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ waitstatus;
ਸਥਿਰ ਪੂਰਨ  WaitForChar(ਲੰਮਾ msec, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼);
ਸਥਿਰ ਪੂਰਨ  WaitForCharOrMouse(ਲੰਮਾ msec, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
ਪੂਰਨ  RealWaitForChar(ਪੂਰਨ, ਲੰਮਾ, ਪੂਰਨ *, ਪੂਰਨ *interrupted);
#ਨਹੀਂਤਾਂ
ਸਥਿਰ ਪੂਰਨ  RealWaitForChar(ਪੂਰਨ, ਲੰਮਾ, ਪੂਰਨ *, ਪੂਰਨ *interrupted);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XCLIPBOARD
ਸਥਿਰ ਪੂਰਨ do_xterm_trace(ਖਾਲੀ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਖਾਲੀ handle_resize(ਖਾਲੀ);

#ਜੇ defined(SIGWINCH)
ਸਥਿਰ ਖਾਲੀ sig_winch SIGPROTOARG;
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGTSTP)
ਸਥਿਰ ਖਾਲੀ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t ਵਿੱਚ_mch_suspend = FALSE;
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGINT)
ਸਥਿਰ ਖਾਲੀ catch_sigint SIGPROTOARG;
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGUSR1)
ਸਥਿਰ ਖਾਲੀ catch_sigusr1 SIGPROTOARG;
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGPWR)
ਸਥਿਰ ਖਾਲੀ catch_sigpwr SIGPROTOARG;
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define ਸੈੱਟ_SIG_ALARM
ਸਥਿਰ ਖਾਲੀ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t sig_alarm_called;
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਖਾਲੀ deathtrap SIGPROTOARG;

ਸਥਿਰ ਖਾਲੀ catch_int_signal(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ ਸੈੱਟ_ਸੰਕੇਤ(ਖਾਲੀ);
ਸਥਿਰ ਖਾਲੀ catch_ਸੰਕੇਤ(ਖਾਲੀ (*func_deadly)(ਪੂਰਨ), ਖਾਲੀ (*func_other)(ਪੂਰਨ));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGPROCMASK
# define SIGSET_DECL(ਸੈੱਟ_2)	sigset_t ਸੈੱਟ_2;
# define ਖੰਡ_ਸੰਕੇਤ(ਸੈੱਟ_2)	ਖੰਡ_ਸੰਕੇਤ_2(ਸੈੱਟ_2)
# define UNBLOCK_ਸੰਕੇਤ(ਸੈੱਟ_2)	unblock_ਸੰਕੇਤ(ਸੈੱਟ_2)
#ਨਹੀਂਤਾਂ
# define SIGSET_DECL(ਸੈੱਟ_2)
# define ਖੰਡ_ਸੰਕੇਤ(ਸੈੱਟ_2)	ਕਰੋ { /**/ } ਜਦੋਂ (0)
# define UNBLOCK_ਸੰਕੇਤ(ਸੈੱਟ_2)	ਕਰੋ { /**/ } ਜਦੋਂ (0)
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ  have_wildcard(ਪੂਰਨ, char_u **);
ਸਥਿਰ ਪੂਰਨ  have_dollars(ਪੂਰਨ, char_u **);

ਸਥਿਰ ਪੂਰਨ save_patterns(ਪੂਰਨ ਅੰਕ_pat, char_u **pat, ਪੂਰਨ *ਅੰਕ_file, char_u ***file);

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#ਜੇ_ਅੰਤ
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#ਜੇ_ਅੰਤ

// volatile because it is used in signal handler sig_winch().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t got_tstp = FALSE;
ਸਥਿਰ char_u	*extra_shell_arg = ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t ਵਿੱਚ_mch_delay = FALSE; // sleeping in mch_delay()

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL) && !defined(USE_SYSTEM)
ਸਥਿਰ ਪੂਰਨ dont_check_ਕੰਮ_ended = 0;
#ਜੇ_ਅੰਤ

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ਸਥਿਰ tmode_T mch_cur_tmode = TMODE_COOK;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_XSMP
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    ਅੱਖਰ *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ਸਥਿਰ xsmp_config_T xsmp;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਬਣਤਰ signalinfo
{
    ਪੂਰਨ	    sig;	// Signal number, eg. SIGSEGV etc
    ਅੱਖਰ    *name;	// Signal name (not char_u!).
    ਅੱਖਰ    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGILL
    {SIGILL,	    "ILL",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGINT
    {SIGINT,	    "INT",	FALSE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#ਜੇ_ਅੰਤ
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(ਪੂਰਨ sig, sighandler_T func)
{
#ਜੇ defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ਬਣਤਰ sigaction	sa, old;
    sigset_t		curset;
    ਪੂਰਨ			blocked;

    ਜੇ (sigprocmask(SIG_BLOCK, ਨਿਰਾ, &curset) == -1)
	ਵਾਪਸ SIG_ERR;

    blocked = sigismember(&curset, sig);

    ਜੇ (func == SIG_HOLD)
    {
	ਜੇ (blocked)
	    ਵਾਪਸ SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ਜੇ (sigaction(sig, ਨਿਰਾ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ਨਿਰਾ) == -1)
	    ਵਾਪਸ SIG_ERR;
	ਵਾਪਸ old.sa_handler;
    }

    ਜੇ (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ਜੇ (sigprocmask(SIG_UNBLOCK, &curset, ਨਿਰਾ) == -1)
	    ਵਾਪਸ SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# ਨਹੀਂਤਾਂ
    sa.sa_flags = 0;
# endif
    ਜੇ (sigaction(sig, &sa, &old) == -1)
	ਵਾਪਸ SIG_ERR;
    ਵਾਪਸ blocked ? SIG_HOLD: old.sa_handler;
#ਜਾਂਜੇ defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    ਵਾਪਸ sigset(sig, func);
#ਨਹੀਂਤਾਂ
    // Oldest and most compatible solution.
    ਵਾਪਸ ਸੰਕੇਤ(sig, func);
#ਜੇ_ਅੰਤ
}

    ਪੂਰਨ
mch_chdir(ਅੱਖਰ *ਰਸਤਾ)
{
    ਜੇ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", ਰਸਤਾ);
	verbose_leave();
    }
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਵਾਪਸ chdir(vms_fixfilename(ਰਸਤਾ));
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ chdir(ਰਸਤਾ);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#ਜੇ_ਅੰਤ

/*
 * Write s[len] to the screen (stdout).
 */
    ਖਾਲੀ
mch_write(char_u *s, ਪੂਰਨ len)
{
    vim_ignored = (ਪੂਰਨ)ਲਿਖ(1, (ਅੱਖਰ *)s, len);
    ਜੇ (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ਨਿਰਾ, ਨਿਰਾ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ਸਥਿਰ ਪੂਰਨ
resize_func(ਪੂਰਨ check_only)
{
    ਜੇ (check_only)
	ਵਾਪਸ do_resize;
    ਜਦੋਂ (do_resize)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
	ch_log(ਨਿਰਾ, "calling handle_resize() in resize_func()");
#ਜੇ_ਅੰਤ
	handle_resize();
    }
    ਵਾਪਸ FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    ਪੂਰਨ
mch_inchar(
    char_u	*buf,
    ਪੂਰਨ		maxlen,
    ਲੰਮਾ	wtime,	    // don't use "time", MIPS cannot handle it
    ਪੂਰਨ		tb_change_cnt)
{
    ਵਾਪਸ inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ਸਥਿਰ ਖਾਲੀ
handle_resize(ਖਾਲੀ)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    ਪੂਰਨ
mch_char_avail(ਖਾਲੀ)
{
    ਵਾਪਸ WaitForChar(0L, ਨਿਰਾ, FALSE);
}

#ਜੇ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ਪੂਰਨ
mch_check_messages(ਖਾਲੀ)
{
    ਵਾਪਸ WaitForChar(0L, ਨਿਰਾ, TRUE);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_ਕੁੱਲ_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# ਜੇ defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# ਜੇ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_ਕੁੱਲ_mem(ਪੂਰਨ special ਅਣਵਰਤਿਆ)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_ਪੋਰਟ_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ਬਣਤਰ vm_statistics64	vm_stat;
	natural_t		ਗਿਣਤੀ_2 = HOST_VM_INFO64_ਗਿਣਤੀ;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ਗਿਣਤੀ_2);
#  ਨਹੀਂਤਾਂ
	ਬਣਤਰ vm_statistics	vm_stat;
	natural_t		ਗਿਣਤੀ_2 = HOST_VM_INFO_ਗਿਣਤੀ;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ਗਿਣਤੀ_2);
#  endif
	ਜੇ (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ਗਿਣਤੀ + vm_stat.ਸਰਗਰਮ_ਗਿਣਤੀ
					    + vm_stat.inactive_ਗਿਣਤੀ
#  ifdef MAC_OS_X_ਸੰਸਕਰਣ_10_9
					    + vm_stat.compressor_ਪੰਨਾ_ਗਿਣਤੀ
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_ਪੋਰਟ_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    ਜੇ (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	ਪੂਰਨ		mib[2];
	size_t		len = ਆਕਾਰ(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  ਨਹੀਂਤਾਂ
	// sysctl() may return 32 bit or 64 bit, accept both
	ਸੰਘ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  ਨਹੀਂਤਾਂ
	mib[1] = HW_USERMEM;
#  endif
	ਜੇ (sysctl(mib, 2, &physmem, &len, ਨਿਰਾ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  ਨਹੀਂਤਾਂ
	    ਜੇ (len == ਆਕਾਰ(physmem.u64))
		mem = (long_u)physmem.u64;
	    ਨਹੀਂਤਾਂ
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# ਜੇ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    ਜੇ (mem == 0)
    {
	ਬਣਤਰ sysinfo sinfo;

	// Linux way of getting amount of RAM available
	ਜੇ (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    ਜਦੋਂ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  ਨਹੀਂਤਾਂ
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    ਜੇ (mem == 0)
    {
	ਲੰਮਾ	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	ਜੇ (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    ਜਦੋਂ (shiftright > 0 && (pagesize & 1) == 0)
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
# ਜੇ defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ਬਣਤਰ rlimit	rlp;

	ਜੇ (getrlimit(RLIMIT_ਅੰਕੜੇ, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (ਆਕਾਰ(long_u) * 8 - 1))
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

    ਜੇ (mem > 0)
	ਵਾਪਸ mem >> shiftright;
    ਵਾਪਸ (long_u)0x1fffff;
}
#ਜੇ_ਅੰਤ

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    ਖਾਲੀ
mch_delay(ਲੰਮਾ msec, ਪੂਰਨ flags)
{
    tmode_T	old_tmode;
    ਪੂਰਨ		call_settmode;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
    ਲੰਮਾ	ਕੁੱਲ = msec; // remember original value
#ਜੇ_ਅੰਤ

    ਜੇ (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	ਵਿੱਚ_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	ਜੇ (call_settmode)
	{
	    old_tmode = mch_cur_tmode;
	    settmode(TMODE_SLEEP);
	}

	/*
	 * Everybody sleeps in a different way...
	 * Prefer nanosleep(), some versions of usleep() can only sleep up to
	 * one second.
	 */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
	ਕਰੋ
	{
	    // if total is large enough, wait by portions in p_mzq
	    ਜੇ (ਕੁੱਲ > p_mzq)
		msec = p_mzq;
	    ਨਹੀਂਤਾਂ
		msec = ਕੁੱਲ;
	    ਕੁੱਲ -= msec;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_NANOSLEEP
	{
	    ਬਣਤਰ timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (ਖਾਲੀ)nanosleep(&ts, ਨਿਰਾ);
	}
#ਨਹੀਂਤਾਂ
# ifdef HAVE_USLEEP
	ਜਦੋਂ (msec >= 1000)
	{
	    usleep((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(999 * 1000));
	    msec -= 999;
	}
	usleep((ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ)(msec * 1000));
# ਨਹੀਂਤਾਂ
#  ifndef HAVE_SELECT
	poll(ਨਿਰਾ, 0, (ਪੂਰਨ)msec);
#  ਨਹੀਂਤਾਂ
	{
	    ਬਣਤਰ timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ, &tv);
	}
#  endif
# endif
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MZSCHEME
	}
	ਜਦੋਂ (ਕੁੱਲ > 0);
#ਜੇ_ਅੰਤ

	ਜੇ (call_settmode)
	    settmode(old_tmode);
	ਵਿੱਚ_mch_delay = FALSE;
    }
    ਨਹੀਂਤਾਂ
	WaitForChar(msec, ਨਿਰਾ, FALSE);
}

#ਜੇ defined(HAVE_ਢੇਰ_ਹੱਦਬੰਦੀ) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ਢੇਰ_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ਸਥਿਰ ਪੂਰਨ ਢੇਰ_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ਸਥਿਰ ਖਾਲੀ
check_ਢੇਰ_growth(ਅੱਖਰ *p)
{
    ਪੂਰਨ		i;

    ਢੇਰ_grows_downwards = (p > (ਅੱਖਰ *)&i);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_ਢੇਰ_ਹੱਦਬੰਦੀ)
ਸਥਿਰ ਅੱਖਰ *ਢੇਰ_ਹੱਦਬੰਦੀ = ਨਿਰਾ;

# ਜੇ defined(THREAD_ਸੁਰੱਖਿਅਤ) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ਸਥਿਰ ਖਾਲੀ
get_ਢੇਰ_ਹੱਦਬੰਦੀ(ਖਾਲੀ)
{
    ਬਣਤਰ rlimit	rlp;
    ਪੂਰਨ			i;
    ਲੰਮਾ		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    ਜੇ (getrlimit(RLIMIT_ਢੇਰ, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (ਆਕਾਰ(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (ਲੰਮਾ)rlp.rlim_cur;
# ਜੇ defined(THREAD_ਸੁਰੱਖਿਅਤ) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    ਮਾਪ;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    ਜੇ (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &ਮਾਪ);
		ਜੇ (lim > (ਲੰਮਾ)ਮਾਪ)
		    lim = (ਲੰਮਾ)ਮਾਪ;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	ਜੇ (ਢੇਰ_grows_downwards)
	{
	    ਢੇਰ_ਹੱਦਬੰਦੀ = (ਅੱਖਰ *)((ਲੰਮਾ)&i - (lim / 16L * 15L));
	    ਜੇ (ਢੇਰ_ਹੱਦਬੰਦੀ >= (ਅੱਖਰ *)&i)
		// overflow, set to 1/16 of current stack position
		ਢੇਰ_ਹੱਦਬੰਦੀ = (ਅੱਖਰ *)((ਲੰਮਾ)&i / 16L);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਢੇਰ_ਹੱਦਬੰਦੀ = (ਅੱਖਰ *)((ਲੰਮਾ)&i + (lim / 16L * 15L));
	    ਜੇ (ਢੇਰ_ਹੱਦਬੰਦੀ <= (ਅੱਖਰ *)&i)
		ਢੇਰ_ਹੱਦਬੰਦੀ = ਨਿਰਾ;	//ਉਛਾਲ
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    ਪੂਰਨ
mch_stackcheck(ਅੱਖਰ *p)
{
    ਜੇ (ਢੇਰ_ਹੱਦਬੰਦੀ == ਨਿਰਾ)
	ਵਾਪਸ OK;

    ਜੇ (ਢੇਰ_grows_downwards)
    {
	ਜੇ (p < ਢੇਰ_ਹੱਦਬੰਦੀ)
	    ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (p > ਢੇਰ_ਹੱਦਬੰਦੀ)
	ਵਾਪਸ FAIL;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ਸਥਿਰ stack_t sigstk;			// for sigaltstack()
# ਨਹੀਂਤਾਂ
ਸਥਿਰ ਬਣਤਰ sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ਸਥਿਰ ਲੰਮਾ ਪੂਰਨ get_signal_ਢੇਰ_ਮਾਪ(ਖਾਲੀ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    ਲੰਮਾ ਪੂਰਨ ਮਾਪ = -1;

    // return size only if sysconf doesn't return an error
    ਜੇ ((ਮਾਪ = sysconf(_SC_SIGSTKSZ)) > -1)
	ਵਾਪਸ ਮਾਪ;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    ਵਾਪਸ SIGSTKSZ;
# endif

    // otherwise guess the size
    ਵਾਪਸ 8000;
}

ਸਥਿਰ ਅੱਖਰ *signal_ਢੇਰ;

    ਸਥਿਰ ਖਾਲੀ
init_signal_ਢੇਰ(ਖਾਲੀ)
{
    ਜੇ (signal_ਢੇਰ == ਨਿਰਾ)
	ਵਾਪਸ;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_ਅਧਾਰ
    sigstk.ss_ਅਧਾਰ = signal_ਢੇਰ;
#  ਨਹੀਂਤਾਂ
    sigstk.ss_sp = signal_ਢੇਰ;
#  endif
    sigstk.ss_size = get_signal_ਢੇਰ_ਮਾਪ();
    sigstk.ss_flags = 0;
    (ਖਾਲੀ)sigaltstack(&sigstk, ਨਿਰਾ);
# ਨਹੀਂਤਾਂ
    sigstk.ss_sp = signal_ਢੇਰ;
    ਜੇ (ਢੇਰ_grows_downwards)
	sigstk.ss_sp += get_signal_ਢੇਰ_ਮਾਪ() - 1;
    sigstk.ss_onstack = 0;
    (ਖਾਲੀ)sigstack(&sigstk, ਨਿਰਾ);
# endif
}
#ਜੇ_ਅੰਤ

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#ਜੇ defined(SIGWINCH)
    ਸਥਿਰ ਖਾਲੀ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(SIGTSTP)
    ਸਥਿਰ ਖਾਲੀ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    ਜੇ (ਵਿੱਚ_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ਉਠਾਓ(sigarg);
    }
    ਨਹੀਂਤਾਂ
	got_tstp = TRUE;

# ਜੇ !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(SIGINT)
    ਸਥਿਰ ਖਾਲੀ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(SIGUSR1)
    ਸਥਿਰ ਖਾਲੀ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(SIGPWR)
    ਸਥਿਰ ਖਾਲੀ
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
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਸੈੱਟ_SIG_ALARM
/*
 * signal function for alarm().
 */
    ਸਥਿਰ ਖਾਲੀ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ਸਥਿਰ JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t lc_ਸਰਗਰਮ = FALSE;

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
    ਸਥਿਰ ਖਾਲੀ
mch_startjmp(ਖਾਲੀ)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_ਸਰਗਰਮ = TRUE;
}

    ਸਥਿਰ ਖਾਲੀ
mch_endjmp(ਖਾਲੀ)
{
    lc_ਸਰਗਰਮ = FALSE;
}

    ਸਥਿਰ ਖਾਲੀ
mch_didjmp(ਖਾਲੀ)
{
# ਜੇ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ਢੇਰ();
# endif
}
#ਜੇ_ਅੰਤ

/*
 * This function handles deadly signals.
 * It tries to preserve any swap files and exit properly.
 * (partly from Elvis).
 * NOTE: Avoid unsafe functions, such as allocating memory, they can result in
 * a deadlock.
 */
    ਸਥਿਰ ਖਾਲੀ
deathtrap SIGDEFARG(sigarg)
{
    ਸਥਿਰ ਪੂਰਨ	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHASARG
    ਪੂਰਨ		i;
#ਜੇ_ਅੰਤ

#ਜੇ defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    ਜੇ (lc_ਸਰਗਰਮ)
    {
# ਜੇ defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_ਸਰਗਰਮ = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    ਜੇ (ਵਿੱਚ_mch_delay && sigarg == SIGQUIT)
	ਵਾਪਸ;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    ਜੇ (entered == 0
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
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    ਖੰਡ_autocmds();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    // Set the v:dying variable.
    ਸੈੱਟ_vim_var_nr(VV_DYING, (ਲੰਮਾ)entered);
#ਜੇ_ਅੰਤ
    v_dying = entered;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_ਢੇਰ_ਹੱਦਬੰਦੀ
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ਢੇਰ_ਹੱਦਬੰਦੀ();
#ਜੇ_ਅੰਤ

#ਜੇ 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	ਜੇ (fp)
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
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHASARG
    // try to find the name of this signal
    ਲਈ (i = 0; signal_info[i].sig != -1; i++)
	ਜੇ (sigarg == signal_info[i].sig)
	    ਰੋਕੋ;
    deadly_signal = sigarg;
#ਜੇ_ਅੰਤ

    ਭਰਿਆ_screen = FALSE;	// don't write message to the GUI, it might be
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
    ਜੇ (entered >= 3)
    {
	reset_ਸੰਕੇਤ();	// don't catch any signals anymore
	may_core_dump();
	ਜੇ (entered >= 4)
	    _exit(8);
	ਬਾਹਰ_ਨਿਕਲੋ(7);
    }
    ਜੇ (entered == 2)
    {
	// No translation, it may call malloc().
	ਬਾਹਰਲਾ_STR("Vim: Double signal, exiting\n");
	ਬਾਹਰਲਾ_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGHASARG
    sprintf((ਅੱਖਰ *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#ਨਹੀਂਤਾਂ
    sprintf((ਅੱਖਰ *)IObuff, "Vim: Caught deadly signal\r\n");
#ਜੇ_ਅੰਤ

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ NBDEBUG
    reset_ਸੰਕੇਤ();
    may_core_dump();
    abort();
#ਜੇ_ਅੰਤ
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    ਸਥਿਰ ਖਾਲੀ
after_sigcont(ਖਾਲੀ)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#ਜੇ defined(SIGCONT)
ਸਥਿਰ ਖਾਲੀ sigcont_handler SIGPROTOARG;

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
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t sigcont_received;
ਸਥਿਰ ਖਾਲੀ sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ਸਥਿਰ ਖਾਲੀ
sigcont_handler SIGDEFARG(sigarg)
{
    ਜੇ (ਵਿੱਚ_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    ਨਹੀਂਤਾਂ
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	ਬਾਹਰਲਾ_flush();
    }
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_CLIPBOARD)
# ਜੇ defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ਸਥਿਰ ਖਾਲੀ *clip_ਤਾਰਾ_save = ਨਿਰਾ;
ਸਥਿਰ ਖਾਲੀ *clip_plus_save = ਨਿਰਾ;
# endif

# ਜੇ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ਸਥਿਰ ਖਾਲੀ
loose_clipboard(ਖਾਲੀ)
{
    ਜੇ (clip_ਤਾਰਾ.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	ਜੇ (clip_ਤਾਰਾ.owned)
	    clip_lose_selection(&clip_ਤਾਰਾ);
	ਜੇ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	ਜੇ (x11_display != ਨਿਰਾ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# ਜੇ defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ਸਥਿਰ ਖਾਲੀ
save_clipboard(ਖਾਲੀ)
{
    ਜੇ (clip_ਤਾਰਾ.owned)
	clip_ਤਾਰਾ_save = get_ਰਜਿਸਟਰ('*', TRUE);
    ਜੇ (clip_plus.owned)
	clip_plus_save = get_ਰਜਿਸਟਰ('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ਸਥਿਰ ਖਾਲੀ
restore_clipboard(ਖਾਲੀ)
{
    ਜੇ (clip_ਤਾਰਾ_save != ਨਿਰਾ)
    {
	ਜੇ (!clip_gen_owner_exists(&clip_ਤਾਰਾ))
	    put_ਰਜਿਸਟਰ('*', clip_ਤਾਰਾ_save);
	ਨਹੀਂਤਾਂ
	    free_ਰਜਿਸਟਰ(clip_ਤਾਰਾ_save);
	clip_ਤਾਰਾ_save = ਨਿਰਾ;
    }
    ਜੇ (clip_plus_save != ਨਿਰਾ)
    {
	ਜੇ (!clip_gen_owner_exists(&clip_plus))
	    put_ਰਜਿਸਟਰ('+', clip_plus_save);
	ਨਹੀਂਤਾਂ
	    free_ਰਜਿਸਟਰ(clip_plus_save);
	clip_plus_save = ਨਿਰਾ;
    }
}
# endif
#ਜੇ_ਅੰਤ

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    ਖਾਲੀ
mch_suspend(ਖਾਲੀ)
{
    ਜੇ (ignore_sigtstp)
	ਵਾਪਸ;

#ਜੇ defined(SIGTSTP)
    ਵਿੱਚ_mch_suspend = TRUE;

    ਬਾਹਰਲਾ_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    ਬਾਹਰਲਾ_flush();	    // needed to disable mouse on some systems

# ਜੇ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# ਜੇ defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# ਜੇ defined(SIGCONT)
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
	ਲੰਮਾ wait_ਸਮਾਂ;

	ਲਈ (wait_ਸਮਾਂ = 0; !sigcont_received && wait_ਸਮਾਂ <= 3L; wait_ਸਮਾਂ++)
	    mch_delay(wait_ਸਮਾਂ, 0);
    }
# endif
    ਵਿੱਚ_mch_suspend = FALSE;

    after_sigcont();
#ਨਹੀਂਤਾਂ
    suspend_shell();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
mch_init(ਖਾਲੀ)
{
    Columns = 80;
    ਪੰਕਤੀਆਂ = 24;

    ਬਾਹਰਲਾ_flush();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#ਜੇ_ਅੰਤ
    ਸੈੱਟ_ਸੰਕੇਤ();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
    mac_conv_init();
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#ਜੇ_ਅੰਤ
}

    ਖਾਲੀ
ਸੈੱਟ_sigwinch_handler(ਖਾਲੀ)
{
#ਜੇ defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਸੰਕੇਤ(ਖਾਲੀ)
{
#ਜੇ defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.ਵਿੱਚ_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#ਜੇ_ਅੰਤ
#ਜੇ defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGINT
    catch_int_signal();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#ਜੇ_ਅੰਤ

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#ਜੇ_ਅੰਤ

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_ਸੰਕੇਤ(deathtrap, SIG_ERR);

#ਜੇ defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    ਜੇ (gui.ਵਿੱਚ_use)
	mch_signal(SIGHUP, SIG_IGN);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ਸਥਿਰ ਖਾਲੀ
catch_int_signal(ਖਾਲੀ)
{
    mch_signal(SIGINT, catch_sigint);
}
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
reset_ਸੰਕੇਤ(ਖਾਲੀ)
{
    catch_ਸੰਕੇਤ(SIG_DFL, SIG_DFL);
#ਜੇ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#ਜੇ_ਅੰਤ
}

    ਸਥਿਰ ਖਾਲੀ
catch_ਸੰਕੇਤ(
    ਖਾਲੀ (*func_deadly)(ਪੂਰਨ),
    ਖਾਲੀ (*func_other)(ਪੂਰਨ))
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; signal_info[i].sig != -1; i++)
    {
	ਜੇ (signal_info[i].deadly)
	{
#ਜੇ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ਬਣਤਰ sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# ਜੇ defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# ਨਹੀਂਤਾਂ
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ਨਿਰਾ);
#ਨਹੀਂਤਾਂ
# ਜੇ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ਬਣਤਰ sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_ਨਕਾਬ = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ਨਿਰਾ);
# ਨਹੀਂਤਾਂ
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#ਜੇ_ਅੰਤ
	}
	ਨਹੀਂਤਾਂ ਜੇ (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#ਨਹੀਂਤਾਂ
	    mch_signal(signal_info[i].sig, func_other);
#ਜੇ_ਅੰਤ
	}
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SIGPROCMASK
    ਸਥਿਰ ਖਾਲੀ
ਖੰਡ_ਸੰਕੇਤ_2(sigset_t *ਸੈੱਟ_2)
{
    sigset_t	newset;
    ਪੂਰਨ		i;

    sigemptyset(&newset);

    ਲਈ (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# ਜੇ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, ਸੈੱਟ_2);
}

    ਸਥਿਰ ਖਾਲੀ
unblock_ਸੰਕੇਤ(sigset_t *ਸੈੱਟ_2)
{
    sigprocmask(SIG_SETMASK, ਸੈੱਟ_2, ਨਿਰਾ);
}
#ਜੇ_ਅੰਤ

/*
 * Handling of SIGHUP, SIGQUIT and SIGTERM:
 * "when" == a signal:       when busy, postpone and return FALSE, otherwise
 *			     return TRUE
 * "when" == SIGNAL_BLOCK:   Going to be busy, block signals
 * "when" == SIGNAL_UNBLOCK: Going to wait, unblock signals, use postponed
 *			     signal
 * Returns TRUE when Vim should exit.
 */
    ਪੂਰਨ
vim_handle_signal(ਪੂਰਨ sig)
{
    ਸਥਿਰ ਪੂਰਨ got_signal = 0;
    ਸਥਿਰ ਪੂਰਨ blocked = TRUE;

    ਚੋਣ (sig)
    {
	ਹਾਲਤ SIGNAL_ਖੰਡ:   blocked = TRUE;
			     ਰੋਕੋ;

	ਹਾਲਤ SIGNAL_UNBLOCK: blocked = FALSE;
			     ਜੇ (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ਰੋਕੋ;

	ਮੂਲ:	     ਜੇ (!blocked)
				 ਵਾਪਸ TRUE;	// exit!
			     got_signal = sig;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGPWR
			     ਜੇ (sig != SIGPWR)
#ਜੇ_ਅੰਤ
				 got_int = TRUE;    // break any loops
			     ਰੋਕੋ;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    ਪੂਰਨ
mch_check_win(ਪੂਰਨ argc ਅਣਵਰਤਿਆ, ਅੱਖਰ **argv ਅਣਵਰਤਿਆ)
{
    ਜੇ (isatty(1))
	ਵਾਪਸ OK;
    ਵਾਪਸ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ਪੂਰਨ
mch_ਨਿਵੇਸ਼_isatty(ਖਾਲੀ)
{
    ਜੇ (isatty(read_cmd_fd))
	ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11

# ਜੇ defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ਸਥਿਰ ਖਾਲੀ
xopen_ਸੁਨੇਹਾ(ਲੰਮਾ elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ਸਥਿਰ ਪੂਰਨ	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ਸਥਿਰ ਪੂਰਨ
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGet_Error_ਪਾਠ(dpy, error_event->error_ਕੋਡ, (ਅੱਖਰ *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
# endif
	ਸੁਨੇਹਾ((ਅੱਖਰ *)IObuff);

    ਵਾਪਸ 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ਸਥਿਰ ਪੂਰਨ
x_error_check(Display *dpy ਅਣਵਰਤਿਆ, XErrorEvent *error_event ਅਣਵਰਤਿਆ)
{
    got_x_error = TRUE;
    ਵਾਪਸ 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ਸਥਿਰ ਪੂਰਨ
x_connect_ਮੰਜ਼ਿਲ_server(ਖਾਲੀ)
{
    // No point in connecting if we are exiting or dying.
    ਜੇ (exiting || v_dying)
	ਵਾਪਸ FALSE;

# ਜੇ defined(FEAT_CLIENTSERVER)
    ਜੇ (x_force_connect)
	ਵਾਪਸ TRUE;
# endif
    ਜੇ (x_no_connect)
	ਵਾਪਸ FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    ਜੇ (clip_exclude_prog != ਨਿਰਾ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	ਜੇ (regprog_ਵਿੱਚ_use(clip_exclude_prog))
	    ਵਾਪਸ FALSE;

	ਜੇ (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    ਵਾਪਸ FALSE;
    }
    ਵਾਪਸ TRUE;
}

# ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  ਜੇ defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ਸਥਿਰ ਪੂਰਨ
x_IOerror_check(Display *dpy ਅਣਵਰਤਿਆ)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   ਜੇ defined(VMS) || defined(__CYGWIN__)
    ਵਾਪਸ 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ਸਥਿਰ ਪੂਰਨ xterm_dpy_retry_ਗਿਣਤੀ = 0;

    ਸਥਿਰ ਪੂਰਨ
x_IOerror_handler(Display *dpy ਅਣਵਰਤਿਆ)
{
    xterm_dpy = ਨਿਰਾ;
    xterm_dpy_retry_ਗਿਣਤੀ = 5;  // Try reconnecting five times
    x11_ਖਿੜਕੀ = 0;
    x11_display = ਨਿਰਾ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  ਜੇ defined(VMS) || defined(__CYGWIN__)
    ਵਾਪਸ 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ਸਥਿਰ ਖਾਲੀ
may_restore_x11_clipboard(ਖਾਲੀ)
{
    // No point in restoring the connecting if we are exiting or dying.
    ਜੇ (!exiting && !v_dying && xterm_dpy_retry_ਗਿਣਤੀ > 0)
    {
	--xterm_dpy_retry_ਗਿਣਤੀ;

#  ifndef LESSTIF_ਸੰਸਕਰਣ
	// This has been reported to avoid Vim getting stuck.
	ਜੇ (app_context != (XtAppContext)ਨਿਰਾ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ਨਿਰਾ;
	    x11_display = ਨਿਰਾ; // freed by XtDestroyApplicationContext()
	}
#  endif

	ਤਿਆਰੀ_ਪਦ_clip();
	get_x11_title(FALSE);
    }
}

    ਖਾਲੀ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    ਜੇ (eap->arg != ਨਿਰਾ && (arglen = STRLEN(eap->arg)) > 0)
    {
	ਜੇ (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (ਅੱਖਰ *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ਨਿਰਾ
		    ? (ਅੱਖਰ *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_ਖਿੜਕੀ = 0;
    xterm_dpy_retry_ਗਿਣਤੀ = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    ਚੁਣੋ_clipmethod();
}
# endif

/*
 * Test if "dpy" and x11_window are valid by getting the window title.
 * I don't actually want it yet, so there may be a simpler call to use, but
 * this will cause the error handler x_error_check() to be called if anything
 * is wrong, such as the window pointer being invalid (as can happen when the
 * user changes his DISPLAY, but not his WINDOWID) -- webb
 */
    ਸਥਿਰ ਪੂਰਨ
test_x11_ਖਿੜਕੀ(Display *dpy)
{
    ਪੂਰਨ			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	ਪਾਠ_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    ਜੇ (XGetWMName(dpy, x11_ਖਿੜਕੀ, &ਪਾਠ_prop))
	XFree((ਖਾਲੀ *)ਪਾਠ_prop.ਮੁੱਲ);
    XSync(dpy, False);
    (ਖਾਲੀ)XSetErrorHandler(old_handler);

    ਜੇ (p_verbose > 0 && got_x_error)
	verb_ਸੁਨੇਹਾ(_("Testing the X display failed"));

    ਵਾਪਸ (got_x_error ? FAIL : OK);
}
#ਜੇ_ਅੰਤ


#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11

ਸਥਿਰ ਪੂਰਨ get_x11_thing(ਪੂਰਨ get_title, ਪੂਰਨ test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
get_x11_windis(ਖਾਲੀ)
{
    ਅੱਖਰ	    *winid;
    ਸਥਿਰ ਪੂਰਨ	    ਨਤੀਜਾ = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_ਇੱਥੇ	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ਸਥਿਰ ਪੂਰਨ	    x11_display_from = XD_NONE;
    ਸਥਿਰ ਪੂਰਨ	    did_ਸੈੱਟ_error_handler = FALSE;

    ਜੇ (!did_ਸੈੱਟ_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(ਖਾਲੀ)XSetErrorHandler(x_error_handler);
	did_ਸੈੱਟ_error_handler = TRUE;
    }

# ਜੇ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    ਜੇ (gui.ਵਿੱਚ_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	ਜੇ (x11_display_from == XD_ਇੱਥੇ && x11_display != ਨਿਰਾ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	ਜੇ (gui_get_x11_windis(&x11_ਖਿੜਕੀ, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    ਵਾਪਸ OK;
	}
	x11_display = ਨਿਰਾ;
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ ਜੇ (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_ਖਿੜਕੀ = 0;
	x11_display = ਨਿਰਾ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    ਜੇ (!x_connect_ਮੰਜ਼ਿਲ_server())
	ਵਾਪਸ FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    ਜੇ (x11_ਖਿੜਕੀ == 0 && (winid = getenv("WINDOWID")) != ਨਿਰਾ)
	x11_ਖਿੜਕੀ = (ਖਿੜਕੀ)atol(winid);

# ifdef FEAT_XCLIPBOARD
    ਜੇ (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    ਜੇ (xterm_dpy != ਨਿਰਾ && x11_ਖਿੜਕੀ != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	ਜੇ (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    ਜੇ (x11_display_from == XD_ਇੱਥੇ && x11_display != ਨਿਰਾ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	ਜੇ (test_x11_ਖਿੜਕੀ(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_ਖਿੜਕੀ = 0;
	    x11_display = ਨਿਰਾ;
	    x11_display_from = XD_NONE;
	    ਵਾਪਸ FAIL;
	}
	ਵਾਪਸ OK;
    }
# endif

    ਜੇ (x11_ਖਿੜਕੀ == 0 || x11_display == ਨਿਰਾ)
	ਨਤੀਜਾ = -1;

    ਜੇ (ਨਤੀਜਾ != -1)	    // Have already been here and set this
	ਵਾਪਸ ਨਤੀਜਾ;	    // Don't do all these X calls again

    ਜੇ (x11_ਖਿੜਕੀ != 0 && x11_display == ਨਿਰਾ)
    {
# ifdef ਸੈੱਟ_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ਸ਼ੁਰੂ_tv;

	ਜੇ (p_verbose > 0)
	    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
# endif

# ifdef ਸੈੱਟ_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(ਨਿਰਾ);

# ifdef ਸੈੱਟ_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	ਜੇ (p_verbose > 0 && sig_alarm_called)
	    verb_ਸੁਨੇਹਾ(_("Opening the X display timed out"));
# endif
	ਜੇ (x11_display != ਨਿਰਾ)
	{
# ifdef ELAPSED_FUNC
	    ਜੇ (p_verbose > 0)
	    {
		verbose_enter();
		xopen_ਸੁਨੇਹਾ(ELAPSED_FUNC(ਸ਼ੁਰੂ_tv));
		verbose_leave();
	    }
# endif
	    ਜੇ (test_x11_ਖਿੜਕੀ(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_ਖਿੜਕੀ = 0;
		XCloseDisplay(x11_display);
		x11_display = ਨਿਰਾ;
	    }
	    ਨਹੀਂਤਾਂ
		x11_display_from = XD_ਇੱਥੇ;
	}
    }
    ਜੇ (x11_ਖਿੜਕੀ == 0 || x11_display == ਨਿਰਾ)
	ਵਾਪਸ (ਨਤੀਜਾ = FAIL);

# ifdef FEAT_EVAL
    ਸੈੱਟ_vim_var_nr(VV_WINDOWID, (ਲੰਮਾ)x11_ਖਿੜਕੀ);
# endif

    ਵਾਪਸ (ਨਤੀਜਾ = OK);
}

/*
 * Determine original x11 Window Title
 */
    ਸਥਿਰ ਪੂਰਨ
get_x11_title(ਪੂਰਨ test_only)
{
    ਵਾਪਸ get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ਸਥਿਰ ਪੂਰਨ
get_x11_icon(ਪੂਰਨ test_only)
{
    ਪੂਰਨ		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    ਜੇ (oldicon == ਨਿਰਾ && !test_only)
    {
	ਜੇ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ਨਹੀਂਤਾਂ
	    oldicon = vim_strsave(T_NAME);
    }

    ਵਾਪਸ retval;
}

    ਸਥਿਰ ਪੂਰਨ
get_x11_thing(
    ਪੂਰਨ		get_title,	// get title string
    ਪੂਰਨ		test_only)
{
    XTextProperty	ਪਾਠ_prop;
    ਪੂਰਨ			retval = FALSE;
    ਸਥਿਤੀ_2		ਸਥਿਤੀ;

    ਜੇ (get_x11_windis() != OK)
	ਵਾਪਸ FALSE;

    // Get window/icon name if any
    ਜੇ (get_title)
	ਸਥਿਤੀ = XGetWMName(x11_display, x11_ਖਿੜਕੀ, &ਪਾਠ_prop);
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = XGetWMIconName(x11_display, x11_ਖਿੜਕੀ, &ਪਾਠ_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    ਖਿੜਕੀ	    ਜੜ੍ਹ;
    ਖਿੜਕੀ	    ਮਾਪਾ;
    ਖਿੜਕੀ	    win = x11_ਖਿੜਕੀ;
    ਖਿੜਕੀ	   *children;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਪੂਰਨ    ਅੰਕ_children;

    ਜਦੋਂ (!ਸਥਿਤੀ || ਪਾਠ_prop.ਮੁੱਲ == ਨਿਰਾ)
    {
	ਜੇ (!XQuery_ਰੁੱਖ(x11_display, win, &ਜੜ੍ਹ, &ਮਾਪਾ, &children,
		    &ਅੰਕ_children))
	    ਰੋਕੋ;
	ਜੇ (children)
	    XFree((ਖਾਲੀ *)children);
	ਜੇ (ਮਾਪਾ == ਜੜ੍ਹ || ਮਾਪਾ == 0)
	    ਰੋਕੋ;

	win = ਮਾਪਾ;
	ਜੇ (get_title)
	    ਸਥਿਤੀ = XGetWMName(x11_display, win, &ਪਾਠ_prop);
	ਨਹੀਂਤਾਂ
	    ਸਥਿਤੀ = XGetWMIconName(x11_display, win, &ਪਾਠ_prop);
    }

    ਜੇ (ਸਥਿਤੀ && ਪਾਠ_prop.ਮੁੱਲ != ਨਿਰਾ)
    {
	retval = TRUE;
	ਜੇ (!test_only)
	{
	    ਜੇ (get_title)
		vim_free(oldtitle);
	    ਨਹੀਂਤਾਂ
		vim_free(oldicon);
	    ਜੇ (ਪਾਠ_prop.encoding == XA_STRING && !has_mbyte)
	    {
		ਜੇ (get_title)
		    oldtitle = vim_strsave((char_u *)ਪਾਠ_prop.ਮੁੱਲ);
		ਨਹੀਂਤਾਂ
		    oldicon = vim_strsave((char_u *)ਪਾਠ_prop.ਮੁੱਲ);
	    }
	    ਨਹੀਂਤਾਂ
	    {
		ਅੱਖਰ    **cl;
		ਸਥਿਤੀ_2  transform_ਸਥਿਤੀ;
		ਪੂਰਨ	    n = 0;

		transform_ਸਥਿਤੀ = Xmb_ਪਾਠ_Property_ਮੰਜ਼ਿਲ_ਪਾਠ_List(x11_display,
			&ਪਾਠ_prop,
			&cl, &n);
		ਜੇ (transform_ਸਥਿਤੀ >= Success && n > 0 && cl[0])
		{
		    ਜੇ (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    ਨਹੀਂਤਾਂ
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		ਨਹੀਂਤਾਂ
		{
		    ਜੇ (get_title)
			oldtitle = vim_strsave((char_u *)ਪਾਠ_prop.ਮੁੱਲ);
		    ਨਹੀਂਤਾਂ
			oldicon = vim_strsave((char_u *)ਪਾਠ_prop.ਮੁੱਲ);
		}
	    }
	}
	XFree((ਖਾਲੀ *)ਪਾਠ_prop.ਮੁੱਲ);
    }
    ਵਾਪਸ retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# ਜੇ defined(X_HAVE_UTF8_STRING)
#  ਜੇ X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    ਜੇ (enc_utf8)
	Xutf8_ਸੈੱਟ_WMProperties(x11_display, x11_ਖਿੜਕੀ, (ਅਚਲ ਅੱਖਰ *)title,
					     ਨਿਰਾ, ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
# endif
    {
# ਜੇ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_ਸੈੱਟ_WMProperties(x11_display, x11_ਖਿੜਕੀ, (ਅਚਲ ਅੱਖਰ *)title,
					     ਨਿਰਾ, ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#  ਨਹੀਂਤਾਂ
	XTextProperty	ਪਾਠ_prop;
	ਅੱਖਰ		*c_title = (ਅੱਖਰ *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(ਖਾਲੀ)XString_List_ਮੰਜ਼ਿਲ_ਪਾਠ_Property(&c_title, 1, &ਪਾਠ_prop);
	XSetWMProperties(x11_display, x11_ਖਿੜਕੀ, &ਪਾਠ_prop,
					     ਨਿਰਾ, ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#  endif
# ਨਹੀਂਤਾਂ
	XStoreName(x11_display, x11_ਖਿੜਕੀ, (ਅੱਖਰ *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    ਜੇ (enc_utf8)
	Xutf8_ਸੈੱਟ_WMProperties(x11_display, x11_ਖਿੜਕੀ, ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)icon,
						   ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
# endif
    {
# ਜੇ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_ਸੈੱਟ_WMProperties(x11_display, x11_ਖਿੜਕੀ, ਨਿਰਾ, (ਅਚਲ ਅੱਖਰ *)icon,
						   ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#  ਨਹੀਂਤਾਂ
	XTextProperty	ਪਾਠ_prop;
	ਅੱਖਰ		*c_icon = (ਅੱਖਰ *)icon;

	(ਖਾਲੀ)XString_List_ਮੰਜ਼ਿਲ_ਪਾਠ_Property(&c_icon, 1, &ਪਾਠ_prop);
	XSetWMProperties(x11_display, x11_ਖਿੜਕੀ, ਨਿਰਾ, &ਪਾਠ_prop,
						   ਨਿਰਾ, 0, ਨਿਰਾ, ਨਿਰਾ, ਨਿਰਾ);
#  endif
# ਨਹੀਂਤਾਂ
	XSetIconName(x11_display, x11_ਖਿੜਕੀ, (ਅੱਖਰ *)icon);
# endif
    }
    XFlush(x11_display);
}

#ਨਹੀਂਤਾਂ  // FEAT_X11

    ਸਥਿਰ ਪੂਰਨ
get_x11_title(ਪੂਰਨ test_only ਅਣਵਰਤਿਆ)
{
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
get_x11_icon(ਪੂਰਨ test_only)
{
    ਜੇ (!test_only)
    {
	ਜੇ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ਨਹੀਂਤਾਂ
	    oldicon = vim_strsave(T_NAME);
    }
    ਵਾਪਸ FALSE;
}

#ਜੇ_ਅੰਤ // FEAT_X11

    ਪੂਰਨ
mch_can_restore_title(ਖਾਲੀ)
{
    ਵਾਪਸ get_x11_title(TRUE);
}

    ਪੂਰਨ
mch_can_restore_icon(ਖਾਲੀ)
{
    ਵਾਪਸ get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    ਖਾਲੀ
mch_settitle(char_u *title, char_u *icon)
{
    ਪੂਰਨ		type = 0;
    ਸਥਿਰ ਪੂਰਨ	recursive = 0;

    ਜੇ (T_NAME == ਨਿਰਾ)	    // no terminal name (yet)
	ਵਾਪਸ;
    ਜੇ (title == ਨਿਰਾ && icon == ਨਿਰਾ)	    // nothing to do
	ਵਾਪਸ;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    ਜੇ (recursive)
	ਵਾਪਸ;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11
    ਜੇ (get_x11_windis() == OK)
	type = 1;
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    ਜੇ (gui.ਵਿੱਚ_use)
	type = 1;
#ਜੇ_ਅੰਤ

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    ਜੇ ((type || *T_TS != NUL) && title != ਨਿਰਾ)
    {
	ਜੇ (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	ਜੇ (oldtitle == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		)		// first call but not in GUI, save title
	    (ਖਾਲੀ)get_x11_title(FALSE);

	ਜੇ (*T_TS != NUL)		// it's OK if t_fs is empty
	    ਪਦ_settitle(title);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11
	ਨਹੀਂਤਾਂ
# ifdef FEAT_GUI_GTK
	ਜੇ (!gui.ਵਿੱਚ_use)		// don't do this if GTK+ is running
# endif
	    ਸੈੱਟ_x11_title(title);		// x11
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	ਨਹੀਂਤਾਂ
	    gui_mch_settitle(title, icon);
#ਜੇ_ਅੰਤ
	unix_did_ਸੈੱਟ_title = TRUE;
    }

    ਜੇ ((type || *T_CIS != NUL) && icon != ਨਿਰਾ)
    {
	ਜੇ (oldicon == ਨਿਰਾ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
		&& !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
		)		// first call, save icon
	    get_x11_icon(FALSE);

	ਜੇ (*T_CIS != NUL)
	{
	    ਬਾਹਰਲਾ_str(T_CIS);			// set icon start
	    ਬਾਹਰਲਾ_str_nf(icon);
	    ਬਾਹਰਲਾ_str(T_CIE);			// set icon end
	    ਬਾਹਰਲਾ_flush();
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_X11
	ਨਹੀਂਤਾਂ
# ifdef FEAT_GUI_GTK
	ਜੇ (!gui.ਵਿੱਚ_use)		// don't do this if GTK+ is running
# endif
	    ਸੈੱਟ_x11_icon(icon);			// x11
#ਜੇ_ਅੰਤ
	did_ਸੈੱਟ_icon = TRUE;
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
    ਖਾਲੀ
mch_restore_title(ਪੂਰਨ which)
{
    ਪੂਰਨ	do_ਧੱਕੋ_ਕੱਢੋ = unix_did_ਸੈੱਟ_title || did_ਸੈੱਟ_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_ਸੈੱਟ_title)
			? (oldtitle ? oldtitle : p_titleold) : ਨਿਰਾ;
    char_u *tofree = ਨਿਰਾ;
    ਜੇ (title == oldtitle && oldtitle != ਨਿਰਾ)
    {
	tofree = vim_strsave(title);
	ਜੇ (tofree != ਨਿਰਾ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_ਸੈੱਟ_icon) ? oldicon : ਨਿਰਾ);
    vim_free(tofree);

    ਜੇ (do_ਧੱਕੋ_ਕੱਢੋ)
    {
	// pop and push from/to the stack
	ਪਦ_ਕੱਢੋ_title(which);
	ਪਦ_ਧੱਕੋ_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    ਪੂਰਨ
vim_is_xterm(char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#ਜੇ defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    ਪੂਰਨ
use_xterm_like_mouse(char_u *name)
{
    ਵਾਪਸ (name != ਨਿਰਾ
	    && (ਪਦ_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#ਜੇ_ਅੰਤ

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 * Return 3 for "urxvt".
 * Return 4 for "sgr".
 */
    ਪੂਰਨ
use_xterm_mouse(ਖਾਲੀ)
{
    ਜੇ (ttym_flags == TTYM_SGR)
	ਵਾਪਸ 4;
    ਜੇ (ttym_flags == TTYM_URXVT)
	ਵਾਪਸ 3;
    ਜੇ (ttym_flags == TTYM_XTERM2)
	ਵਾਪਸ 2;
    ਜੇ (ttym_flags == TTYM_XTERM)
	ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    ਪੂਰਨ
vim_is_iris(char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    ਵਾਪਸ (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#ਜੇ defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    ਪੂਰਨ
vim_is_vt300(char_u *name)
{
    ਜੇ (name == ਨਿਰਾ)
	ਵਾਪਸ FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    ਵਾਪਸ ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ਨਿਰਾ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#ਜੇ_ਅੰਤ

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    ਪੂਰਨ
mch_get_user_name(char_u *s, ਪੂਰਨ len)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    vim_strncpy(s, (char_u *)cuserid(ਨਿਰਾ), len - 1);
    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ mch_get_uname(getuid(), s, len);
#ਜੇ_ਅੰਤ
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    ਪੂਰਨ
mch_get_uname(uid_t uid, char_u *s, ਪੂਰਨ len)
{
#ਜੇ defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ਬਣਤਰ passwd   *pw;

    ਜੇ ((pw = getpwuid(uid)) != ਨਿਰਾ
	    && pw->pw_name != ਨਿਰਾ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	ਵਾਪਸ OK;
    }
#ਜੇ_ਅੰਤ
    sprintf((ਅੱਖਰ *)s, "%d", (ਪੂਰਨ)uid);	    // assumes s is long enough
    ਵਾਪਸ FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_SYS_UTSNAME_H
    ਖਾਲੀ
mch_get_host_name(char_u *s, ਪੂਰਨ len)
{
    ਬਣਤਰ utsname vutsname;

    ਜੇ (uname(&vutsname) < 0)
	*s = NUL;
    ਨਹੀਂਤਾਂ
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#ਨਹੀਂਤਾਂ // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    ਖਾਲੀ
mch_get_host_name(char_u *s, ਪੂਰਨ len)
{
# ifdef VAXC
    vaxc$gethostname((ਅੱਖਰ *)s, len);
# ਨਹੀਂਤਾਂ
    gethostname((ਅੱਖਰ *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#ਜੇ_ਅੰਤ // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    ਲੰਮਾ
mch_get_pid(ਖਾਲੀ)
{
    ਵਾਪਸ (ਲੰਮਾ)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    ਪੂਰਨ
mch_process_running(ਲੰਮਾ pid)
{
    // If there is no error the process must be running.
    ਜੇ (kill(pid, 0) == 0)
	ਵਾਪਸ TRUE;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ESRCH
    // If the error is ESRCH then the process is not running.
    ਜੇ (errno == ESRCH)
	ਵਾਪਸ FALSE;
#ਜੇ_ਅੰਤ
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    ਵਾਪਸ TRUE;
}

#ਜੇ !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ਸਥਿਰ ਅੱਖਰ *
strerror(ਪੂਰਨ err)
{
    ਬਾਹਰੀ ਪੂਰਨ	    sys_nerr;
    ਬਾਹਰੀ ਅੱਖਰ	    *sys_errlist[];
    ਸਥਿਰ ਅੱਖਰ	    er[20];

    ਜੇ (err > 0 && err < sys_nerr)
	ਵਾਪਸ (sys_errlist[err]);
    snprintf(er, ਆਕਾਰ(er), "Error %d", err);
    ਵਾਪਸ er;
}
#ਜੇ_ਅੰਤ

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    ਪੂਰਨ
mch_dirname(char_u *buf, ਪੂਰਨ len)
{
#ਜੇ defined(USE_GETCWD)
    ਜੇ (getcwd((ਅੱਖਰ *)buf, len) == ਨਿਰਾ)
    {
	STRCPY(buf, strerror(errno));
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (getwd((ਅੱਖਰ *)buf) != ਨਿਰਾ ? OK : FAIL);
#ਜੇ_ਅੰਤ
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    ਪੂਰਨ
mch_ਭਰਿਆ_Name(
    char_u	*fname,
    char_u	*buf,
    ਪੂਰਨ		len,
    ਪੂਰਨ		force)		// also expand when already absolute path
{
    ਪੂਰਨ		buflen = 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHDIR
    ਪੂਰਨ		fd = -1;
    ਸਥਿਰ ਪੂਰਨ	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#ਜੇ_ਅੰਤ
    char_u	olddir[MAXPATHL];
    char_u	*p;
    ਪੂਰਨ		retval = OK;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    fname = vms_fixfilename(fname);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# ਜੇ CYGWIN_ਸੰਸਕਰਣ_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_ਰਸਤਾ(CCP_WIN_A_ਮੰਜ਼ਿਲ_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# ਨਹੀਂਤਾਂ
    cygwin_conv_ਮੰਜ਼ਿਲ_posix_ਰਸਤਾ(fname, posix_fname);
# endif
    fname = posix_fname;
#ਜੇ_ਅੰਤ

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    ਜੇ ((force || !mch_is_ਭਰਿਆ_Name(fname))
	    && ((p = vim_strrchr(fname, '/')) == ਨਿਰਾ || p != fname))
    {
	ਜੇ (p == ਨਿਰਾ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਜੇ (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    ਜੇ (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		ਜੇ (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#ਜੇ_ਅੰਤ

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    ਜੇ (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHDIR
		fd < 0 &&
#ਜੇ_ਅੰਤ
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((ਅੱਖਰ *)olddir) != 0))
	    {
		p = ਨਿਰਾ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    ਨਹੀਂਤਾਂ
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		ਜੇ (p - fname >= len)
		    retval = FAIL;
		ਨਹੀਂਤਾਂ
		{
		    vim_strncpy(buf, fname, p - fname);
		    ਜੇ (mch_chdir((ਅੱਖਰ *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			ਜੇ (mch_is_ਭਰਿਆ_Name(fname))
			    retval = FAIL;
			ਨਹੀਂਤਾਂ
			    p = ਨਿਰਾ;
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (*p == '/')
			fname = p + 1;
		    ਨਹੀਂਤਾਂ
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	ਜੇ (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	ਜੇ (p != ਨਿਰਾ)
	{
	    ਪੂਰਨ	l;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHDIR
	    ਜੇ (fd >= 0)
	    {
		ਜੇ (p_verbose >= 5)
		{
		    verbose_enter();
		    ਸੁਨੇਹਾ("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
		l = mch_chdir((ਅੱਖਰ *)olddir);
	    ਜੇ (l != 0)
		emsg(_(e_cannot_go_ਪਿੱਠ_ਮੰਜ਼ਿਲ_ਪਿਛਲਾ_directory));
	}
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_FCHDIR
	ਜੇ (fd >= 0)
	    close(fd);
#ਜੇ_ਅੰਤ

	buflen = (ਪੂਰਨ)STRLEN(buf);
	ਜੇ (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS
	ਨਹੀਂਤਾਂ ਜੇ (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += ਆਕਾਰ(PATHSEP);
	}
#ਜੇ_ਅੰਤ
    }

    ਜੇ (buflen == 0)
	buflen = (ਪੂਰਨ)STRLEN(buf);

    // Catch file names which are too long.
    ਜੇ (retval == FAIL || (ਪੂਰਨ)(buflen + STRLEN(fname)) >= len)
	ਵਾਪਸ FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    ਜੇ (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    ਵਾਪਸ OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ਪੂਰਨ
mch_is_ਭਰਿਆ_Name(char_u *fname)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਵਾਪਸ ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((ਅੱਖਰ *)fname,':') || strchr((ਅੱਖਰ *)fname,'"') ||
	    (strchr((ਅੱਖਰ *)fname,'[') && strchr((ਅੱਖਰ *)fname,']'))||
	    (strchr((ਅੱਖਰ *)fname,'<') && strchr((ਅੱਖਰ *)fname,'>'))   );
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (*fname == '/' || *fname == '~');
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    ਖਾਲੀ
fname_case(
    char_u	*name,
    ਪੂਰਨ		len ਅਣਵਰਤਿਆ)  // buffer size, only used when name gets longer
{
    ਬਣਤਰ stat st;
    char_u	*slash, *ਪੂਛ;
    size_t	taillen;
    DIR		*dirp;
    ਬਣਤਰ dirent *dp;

    ਜੇ (mch_lstat((ਅੱਖਰ *)name, &st) < 0)
	ਵਾਪਸ;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    ਜੇ (slash == ਨਿਰਾ)
    {
	dirp = opendir(".");
	ਪੂਛ = name;
    }
    ਨਹੀਂਤਾਂ
    {
	*slash = NUL;
	dirp = opendir((ਅੱਖਰ *)name);
	*slash = '/';
	ਪੂਛ = slash + 1;
    }

    ਜੇ (dirp == ਨਿਰਾ)
	ਵਾਪਸ;

    taillen = STRLEN(ਪੂਛ);
    ਜਦੋਂ ((dp = readdir(dirp)) != ਨਿਰਾ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	ਜੇ (STRICMP(ਪੂਛ, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ਬਣਤਰ stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (ਪੂਛ - name), (char_u *)dp->d_name,
		    MAXPATHL - (ਪੂਛ - name));
	    ਜੇ (mch_lstat((ਅੱਖਰ *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(ਪੂਛ, dp->d_name);
		ਰੋਕੋ;
	    }
	}
    }

    closedir(dirp);
}
#ਜੇ_ਅੰਤ

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    ਲੰਮਾ
mch_getperm(char_u *name)
{
    ਬਣਤਰ stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    ਜੇ (stat((ਅੱਖਰ *)vms_fixfilename(name), &statb))
#ਨਹੀਂਤਾਂ
    ਜੇ (stat((ਅੱਖਰ *)name, &statb))
#ਜੇ_ਅੰਤ
	ਵਾਪਸ -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    ਵਾਪਸ statb.st_ਬਹੁਲਕ & ~S_ADDACE;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ statb.st_ਬਹੁਲਕ;
#ਜੇ_ਅੰਤ
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
mch_setperm(char_u *name, ਲੰਮਾ perm)
{
    ਵਾਪਸ (chmod((ਅੱਖਰ *)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
		    vms_fixfilename(name),
#ਨਹੀਂਤਾਂ
		    name,
#ਜੇ_ਅੰਤ
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#ਜੇ defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ਪੂਰਨ
mch_fsetperm(ਪੂਰਨ fd, ਲੰਮਾ perm)
{
    ਵਾਪਸ (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਬਣਤਰ vim_acl_solaris_T {
    ਪੂਰਨ acl_cnt;
    aclent_t *acl_ਇੰਦਰਾਜ;
} vim_acl_solaris_T;
# endif

# ਜੇ defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ਖਾਲੀ
mch_copy_sec(char_u *from_file, char_u *ਮੰਜ਼ਿਲ_file)
{
    ਜੇ (from_file == ਨਿਰਾ)
	ਵਾਪਸ;

    ਜੇ (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    ਜੇ (selinux_enabled <= 0)
	ਵਾਪਸ;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    ਅੱਖਰ *from_context = ਨਿਰਾ;
    ਅੱਖਰ *ਮੰਜ਼ਿਲ_context = ਨਿਰਾ;

    ਜੇ (getfilecon((ਅੱਖਰ *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	ਜੇ (errno == EOPNOTSUPP)
	    ਵਾਪਸ;

	ਸੁਨੇਹਾ_puts(_("\nCould not get security context for "));
	ਸੁਨੇਹਾ_outtrans(from_file);
	ਸੁਨੇਹਾ_putchar('\n');
	ਵਾਪਸ;
    }
    ਜੇ (getfilecon((ਅੱਖਰ *)ਮੰਜ਼ਿਲ_file, &ਮੰਜ਼ਿਲ_context) < 0)
    {
	ਸੁਨੇਹਾ_puts(_("\nCould not get security context for "));
	ਸੁਨੇਹਾ_outtrans(ਮੰਜ਼ਿਲ_file);
	ਸੁਨੇਹਾ_putchar('\n');
	freecon (from_context);
	ਵਾਪਸ ;
    }
    ਜੇ (strcmp(from_context, ਮੰਜ਼ਿਲ_context) != 0)
    {
	ਜੇ (setfilecon((ਅੱਖਰ *)ਮੰਜ਼ਿਲ_file, from_context) < 0)
	{
	    ਸੁਨੇਹਾ_puts(_("\nCould not set security context for "));
	    ਸੁਨੇਹਾ_outtrans(ਮੰਜ਼ਿਲ_file);
	    ਸੁਨੇਹਾ_putchar('\n');
	}
    }
    freecon(ਮੰਜ਼ਿਲ_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# ਜੇ defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ਖਾਲੀ
mch_copy_sec(char_u *from_file, char_u *ਮੰਜ਼ਿਲ_file)
{
    ਸਥਿਰ ਅਚਲ ਅੱਖਰ * ਅਚਲ smack_copied_ਗੁਣ[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    ਅੱਖਰ	ਬਫਰ[SMACK_LABEL_LEN];
    ਅਚਲ ਅੱਖਰ	*name;
    ਪੂਰਨ		ਸੂਚਕਾਂਕ;
    ਪੂਰਨ		ret;
    ssize_t	ਮਾਪ;

    ਜੇ (from_file == ਨਿਰਾ)
	ਵਾਪਸ;

    ਮਾਪ = listxattr((ਅੱਖਰ *)from_file, ਨਿਰਾ, 0);
    // not supported or no attributes to copy
    ਜੇ (ਮਾਪ <= 0)
	ਵਾਪਸ;

    ਲਈ (ਸੂਚਕਾਂਕ = 0 ; ਸੂਚਕਾਂਕ < (ਪੂਰਨ)ARRAY_ਲੰਮਾਈ(smack_copied_ਗੁਣ); ਸੂਚਕਾਂਕ++)
    {
	// get the name of the attribute to copy
	name = smack_copied_ਗੁਣ[ਸੂਚਕਾਂਕ];

	// get the value of the attribute in buffer
	ਮਾਪ = getxattr((ਅੱਖਰ*)from_file, name, ਬਫਰ, ਆਕਾਰ(ਬਫਰ));
	ਜੇ (ਮਾਪ >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((ਅੱਖਰ*)ਮੰਜ਼ਿਲ_file, name, ਬਫਰ, (size_t)ਮਾਪ, 0);
	    ਜੇ (ret < 0)
	    {
		vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, ਮੰਜ਼ਿਲ_file);
		ਸੁਨੇਹਾ_outtrans(IObuff);
		ਸੁਨੇਹਾ_putchar('\n');
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    // what reason of not having the attribute value?
	    ਚੋਣ (errno)
	    {
		ਹਾਲਤ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    ਵਾਪਸ; // leave because it isn't useful to continue

		ਹਾਲਤ ERANGE:
		ਮੂਲ:
		    // no enough size OR unexpected error
		     vim_snprintf((ਅੱਖਰ *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)IObuff);
		    ਸੁਨੇਹਾ_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		ਹਾਲਤ ENODATA:
		    // no attribute of this name
		    ret = removexattr((ਅੱਖਰ*)ਮੰਜ਼ਿਲ_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ਰੋਕੋ;
	    }
	}
    }
}
# endif // HAVE_SMACK

# ਜੇ defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    ਖਾਲੀ
mch_copy_xattr(char_u *from_file, char_u *ਮੰਜ਼ਿਲ_file)
{
    ਅੱਖਰ	*xattr_buf;
    ssize_t	ਮਾਪ;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    ਅੱਖਰ	*ਕੁੰਜੀ;
    ਅੱਖਰ	*val = ਨਿਰਾ;
    ਅੱਖਰ	*errmsg = ਨਿਰਾ;

    ਜੇ (from_file == ਨਿਰਾ)
	ਵਾਪਸ;

    // get the length of the extended attributes
    ਮਾਪ = listxattr((ਅੱਖਰ *)from_file, ਨਿਰਾ, 0);
    // not supported or no attributes to copy
    ਜੇ (ਮਾਪ <= 0)
	ਵਾਪਸ;
    xattr_buf = (ਅੱਖਰ*)alloc(ਮਾਪ);
    ਜੇ (xattr_buf == ਨਿਰਾ)
	ਵਾਪਸ;
    ਮਾਪ = listxattr((ਅੱਖਰ *)from_file, xattr_buf, ਮਾਪ);
    tsize = ਮਾਪ;

    errno = 0;

    ਲਈ (ਪੂਰਨ ਗੇੜਾ = 0; ਗੇੜਾ < 2; ਗੇੜਾ++)
    {

	ਕੁੰਜੀ = xattr_buf;
	ਜੇ (ਗੇੜਾ == 1)
	    ਮਾਪ = tsize;

	ਜਦੋਂ (ਮਾਪ > 0)
	{
	    vallen = getxattr((ਅੱਖਰ *)from_file, ਕੁੰਜੀ,
		    val, ਗੇੜਾ ? max_vallen : 0);
	    // only set the attribute in the second round
	    ਜੇ (vallen >= 0 && ਗੇੜਾ &&
		setxattr((ਅੱਖਰ *)ਮੰਜ਼ਿਲ_file, ਕੁੰਜੀ, val, vallen, 0) == 0)
		;
	    ਨਹੀਂਤਾਂ ਜੇ (errno)
	    {
		ਚੋਣ (errno)
		{
		    ਹਾਲਤ E2BIG:
			errmsg = e_xattr_e2big;
			ਜਾਓ error_exit;
		    ਹਾਲਤ ENOTSUP:
		    ਹਾਲਤ EACCES:
		    ਹਾਲਤ EPERM:
			ਰੋਕੋ;
		    ਹਾਲਤ ERANGE:
			errmsg = e_xattr_erange;
			ਜਾਓ error_exit;
		    ਮੂਲ:
			errmsg = e_xattr_other;
			ਜਾਓ error_exit;
		}
	    }

	    ਜੇ (ਗੇੜਾ == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ਕੁੰਜੀ) + 1;
	    ਮਾਪ -= keylen;
	    ਕੁੰਜੀ += keylen;
	}
	ਜੇ (ਗੇੜਾ)
	    ਰੋਕੋ;

	val = (ਅੱਖਰ*)alloc(max_vallen + 1);
	ਜੇ (val == ਨਿਰਾ)
	    ਜਾਓ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    ਜੇ (errmsg != ਨਿਰਾ)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname ਅਣਵਰਤਿਆ)
{
    vim_acl_T	ret = ਨਿਰਾ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((ਅੱਖਰ *)fname, ACL_TYPE_ACCESS);
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    ਜੇ (acl_get((ਅੱਖਰ *)fname, 0, &aclent) < 0)
	ਵਾਪਸ ਨਿਰਾ;
    ret = (vim_acl_T)aclent;
#  ਨਹੀਂਤਾਂ
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = ਸਮ੍ਰਿਤੀ_ਦਿਓ(ਆਕਾਰ(vim_acl_solaris_T));
    ਜੇ (aclent == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ ((aclent->acl_cnt = acl((ਅੱਖਰ *)fname, GETACLCNT, 0, ਨਿਰਾ)) < 0)
    {
	ਮੁਕਤ(aclent);
	ਵਾਪਸ ਨਿਰਾ;
    }
    aclent->acl_ਇੰਦਰਾਜ = ਸਮ੍ਰਿਤੀ_ਦਿਓ(aclent->acl_cnt * ਆਕਾਰ(aclent_t));
    ਜੇ (aclent->acl_ਇੰਦਰਾਜ == ਨਿਰਾ)
    {
	ਮੁਕਤ(aclent);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ਜੇ (acl((ਅੱਖਰ *)fname, GETACL, aclent->acl_cnt, aclent->acl_ਇੰਦਰਾਜ) < 0)
    {
	ਮੁਕਤ(aclent->acl_ਇੰਦਰਾਜ);
	ਮੁਕਤ(aclent);
	ਵਾਪਸ ਨਿਰਾ;
    }
    ret = (vim_acl_T)aclent;
#   ਨਹੀਂਤਾਂ
#    ਜੇ defined(HAVE_AIX_ACL)
    ਪੂਰਨ		aclsize;
    ਬਣਤਰ acl *aclent;

    aclsize = ਆਕਾਰ(ਬਣਤਰ acl);
    aclent = ਸਮ੍ਰਿਤੀ_ਦਿਓ(aclsize);
    ਜੇ (aclent == ਨਿਰਾ)
	ਵਾਪਸ ਨਿਰਾ;
    ਜੇ (statacl((ਅੱਖਰ *)fname, STX_ਸਾਧਾਰਨ, aclent, aclsize) < 0)
    {
	ਜੇ (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    ਜੇ (aclent == ਨਿਰਾ || statacl((ਅੱਖਰ *)fname, STX_ਸਾਧਾਰਨ, aclent, aclsize) < 0)
	    {
		ਮੁਕਤ(aclent);
		ਵਾਪਸ ਨਿਰਾ;
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਮੁਕਤ(aclent);
	    ਵਾਪਸ ਨਿਰਾ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    ਵਾਪਸ ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_acl(char_u *fname ਅਣਵਰਤਿਆ, vim_acl_T aclent)
{
    ਜੇ (aclent == ਨਿਰਾ)
	ਵਾਪਸ;
# ifdef HAVE_POSIX_ACL
    acl_ਸੈੱਟ_file((ਅੱਖਰ *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_ਸੈੱਟ((ਅੱਖਰ *)fname, (acl_t *)aclent);
#  ਨਹੀਂਤਾਂ
#   ifdef HAVE_SOLARIS_ACL
    acl((ਅੱਖਰ *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_ਇੰਦਰਾਜ);
#   ਨਹੀਂਤਾਂ
#    ifdef HAVE_AIX_ACL
    chacl((ਅੱਖਰ *)fname, aclent, ((ਬਣਤਰ acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    ਖਾਲੀ
mch_free_acl(vim_acl_T aclent)
{
    ਜੇ (aclent == ਨਿਰਾ)
	ਵਾਪਸ;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# ਨਹੀਂਤਾਂ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  ਨਹੀਂਤਾਂ
#   ifdef HAVE_SOLARIS_ACL
    ਮੁਕਤ(((vim_acl_solaris_T *)aclent)->acl_ਇੰਦਰਾਜ);
    ਮੁਕਤ(aclent);
#   ਨਹੀਂਤਾਂ
#    ifdef HAVE_AIX_ACL
    ਮੁਕਤ(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#ਜੇ_ਅੰਤ

/*
 * Set hidden flag for "name".
 */
    ਖਾਲੀ
mch_hide(char_u *name ਅਣਵਰਤਿਆ)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ਪੂਰਨ
mch_isdir(char_u *name)
{
    ਬਣਤਰ stat statb;

    ਜੇ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ਵਾਪਸ FALSE;
    ਜੇ (stat((ਅੱਖਰ *)name, &statb))
	ਵਾਪਸ FALSE;
    ਵਾਪਸ (S_ISDIR(statb.st_ਬਹੁਲਕ) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ਪੂਰਨ
mch_isrealdir(char_u *name)
{
    ਬਣਤਰ stat statb;

    ਜੇ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ਵਾਪਸ FALSE;
    ਜੇ (mch_lstat((ਅੱਖਰ *)name, &statb))
	ਵਾਪਸ FALSE;
    ਵਾਪਸ (S_ISDIR(statb.st_ਬਹੁਲਕ) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ਸਥਿਰ ਪੂਰਨ
executable_file(char_u *name)
{
    ਬਣਤਰ stat	st;

    ਜੇ (stat((ਅੱਖਰ *)name, &st))
	ਵਾਪਸ 0;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    ਪੂਰਨ vms_executable = 0;
    ਜੇ (S_ISREG(st.st_ਬਹੁਲਕ) && mch_access((ਅੱਖਰ *)name, X_OK) == 0)
    {
	ਜੇ (ਲੜੀ_ਖੋਜ(vms_tolower((ਅੱਖਰ*)name),".exe") != ਨਿਰਾ
		|| ਲੜੀ_ਖੋਜ(vms_tolower((ਅੱਖਰ*)name),".com")!= ਨਿਰਾ)
	    vms_executable = 1;
    }
    ਵਾਪਸ vms_executable;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ S_ISREG(st.st_ਬਹੁਲਕ) && mch_access((ਅੱਖਰ *)name, X_OK) == 0;
#ਜੇ_ਅੰਤ
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ਪੂਰਨ
mch_can_exe(char_u *name, char_u **ਰਸਤਾ, ਪੂਰਨ use_ਰਸਤਾ)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_ਸਿਰਾ;
    size_t	elen;
    ਪੂਰਨ		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    ਜੇ (!use_ਰਸਤਾ || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	ਜੇ ((use_ਰਸਤਾ || gettail(name) != name) && executable_file(name))
	{
	    ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	    {
		ਜੇ (name[0] != '/')
		    *ਰਸਤਾ = ਭਰਿਆ_Name_save(name, TRUE);
		ਨਹੀਂਤਾਂ
		    *ਰਸਤਾ = vim_strsave(name);
	    }
	    ਵਾਪਸ TRUE;
	}
	ਵਾਪਸ FALSE;
    }

    p = (char_u *)getenv("PATH");
    ਜੇ (p == ਨਿਰਾ || *p == NUL)
	ਵਾਪਸ -1;
    p_ਸਿਰਾ = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_ਸਿਰਾ - p) + 2;
    buf = alloc(bufsize);
    ਜੇ (buf == ਨਿਰਾ)
	ਵਾਪਸ -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ਲਈ (;;)
    {
	e = (char_u *)strchr((ਅੱਖਰ *)p, ':');
	ਜੇ (e == ਨਿਰਾ)
	    e = p_ਸਿਰਾ;
	elen = (size_t)(e - p);
	ਜੇ (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((ਅੱਖਰ *)buf, bufsize, "%.*s%s%s",
		(ਪੂਰਨ)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	ਜੇ (retval == 1)
	{
	    ਜੇ (ਰਸਤਾ != ਨਿਰਾ)
	    {
		ਜੇ (buf[0] != '/')
		    *ਰਸਤਾ = ਭਰਿਆ_Name_save(buf, TRUE);
		ਨਹੀਂਤਾਂ
		    *ਰਸਤਾ = vim_strnsave(buf, buflen);
	    }
	    ਰੋਕੋ;
	}

	ਜੇ (*e != ':')
	    ਰੋਕੋ;
	p = e + 1;
    }

    vim_free(buf);
    ਵਾਪਸ retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ਪੂਰਨ
mch_nodetype(char_u *name)
{
    ਬਣਤਰ stat	st;

    ਜੇ (stat((ਅੱਖਰ *)name, &st))
	ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;
    ਜੇ (S_ISREG(st.st_ਬਹੁਲਕ) || S_ISDIR(st.st_ਬਹੁਲਕ))
	ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;
    ਜੇ (S_ISBLK(st.st_ਬਹੁਲਕ))	// block device isn't writable
	ਵਾਪਸ ਗੰਢ_OTHER;
    // Everything else is writable?
    ਵਾਪਸ ਗੰਢ_WRITABLE;
}

    ਖਾਲੀ
mch_early_init(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ HAVE_CHECK_ਢੇਰ_GROWTH
    ਪੂਰਨ			i;

    check_ਢੇਰ_growth((ਅੱਖਰ *)&i);

# ifdef HAVE_ਢੇਰ_ਹੱਦਬੰਦੀ
    get_ਢੇਰ_ਹੱਦਬੰਦੀ();
# endif

#ਜੇ_ਅੰਤ

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#ਜੇ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ਢੇਰ = alloc(get_signal_ਢੇਰ_ਮਾਪ());
    init_signal_ਢੇਰ();
#ਜੇ_ਅੰਤ

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_X
# ifdef MAC_OS_X_ਸੰਸਕਰਣ_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_ਸੈੱਟ(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(EXITFREE)
    ਖਾਲੀ
mch_free_mem(ਖਾਲੀ)
{
# ਜੇ defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    ਜੇ (clip_ਤਾਰਾ.owned)
	clip_lose_selection(&clip_ਤਾਰਾ);
    ਜੇ (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ਜੇ (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_ਸੰਸਕਰਣ
    // Lesstif crashes here, lose some memory
    ਜੇ (xterm_dpy != ਨਿਰਾ)
	XtCloseDisplay(xterm_dpy);
    ਜੇ (app_context != (XtAppContext)ਨਿਰਾ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ਨਿਰਾ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# ਜੇ defined(FEAT_X11)
    ਜੇ (x11_display != ਨਿਰਾ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# ਜੇ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ਢੇਰ);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#ਜੇ_ਅੰਤ

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ਸਥਿਰ ਖਾਲੀ
exit_scroll(ਖਾਲੀ)
{
    ਜੇ (silent_ਬਹੁਲਕ)
	ਵਾਪਸ;
    ਜੇ (newline_on_exit || ਸੁਨੇਹਾ_didout)
    {
	ਜੇ (ਸੁਨੇਹਾ_use_printf())
	{
	    ਜੇ (info_ਸੁਨੇਹਾ)
		mch_ਸੁਨੇਹਾ("\n");
	    ਨਹੀਂਤਾਂ
		mch_errmsg("\r\n");
	}
	ਨਹੀਂਤਾਂ
	    ਬਾਹਰਲਾ_char('\n');
    }
    ਨਹੀਂਤਾਂ ਜੇ (!is_not_a_ਪਦ())
    {
	restore_cterm_colors();		// get original colors back
	ਸੁਨੇਹਾ_clr_eos_force();		// clear the rest of the display
	windgoto((ਪੂਰਨ)ਪੰਕਤੀਆਂ - 1, cmdline_ਖਾਨਾ_off); // may have moved the cursor
    }
}

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GCOV_FLUSH
# ਜੇ (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
ਬਾਹਰੀ ਖਾਲੀ __gcov_dump(ਖਾਲੀ);
ਬਾਹਰੀ ਖਾਲੀ __gcov_reset(ਖਾਲੀ);
#  define __gcov_flush() ਕਰੋ { __gcov_dump(); __gcov_reset(); } ਜਦੋਂ (0)
# ਨਹੀਂਤਾਂ
ਬਾਹਰੀ ਖਾਲੀ __gcov_flush(ਖਾਲੀ);
# endif
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
mch_exit(ਪੂਰਨ r)
{
    exiting = TRUE;

#ਜੇ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (!gui.ਵਿੱਚ_use)
#ਜੇ_ਅੰਤ
    {
	settmode(TMODE_COOK);
	ਜੇ (!is_not_a_ਪਦ())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    ਪਦ_ਕੱਢੋ_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	ਜੇ (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	ਜੇ (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	ਜੇ (ਭਰਿਆ_screen)
	    cursor_on();
    }
    ਬਾਹਰਲਾ_flush();
    ml_close_all(TRUE);		// remove all memfiles

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#ਜੇ_ਅੰਤ

    may_core_dump();
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_exit(r);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ MACOS_CONVERT
    mac_conv_cleanup();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    ਜੇ (deadly_signal != 0)
	ਵਾਪਸ;
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ EXITFREE
    free_all_mem();
#ਜੇ_ਅੰਤ

    ਬਾਹਰ_ਨਿਕਲੋ(r);
}

    ਸਥਿਰ ਖਾਲੀ
may_core_dump(ਖਾਲੀ)
{
    ਜੇ (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ਸਥਿਰ ਪੂਰਨ
get_tty_fd(ਪੂਰਨ fd)
{
    ਪੂਰਨ		tty_fd = fd;

# ਜੇ defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    ਜੇ (mch_isatty(fd) > 1)
    {
	ਅੱਖਰ *name;

	name = ptsname(fd);
	ਜੇ (name == ਨਿਰਾ)
	    ਵਾਪਸ -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	ਜੇ (tty_fd < 0)
	    ਵਾਪਸ -1;
    }
# endif
    ਵਾਪਸ tty_fd;
}

    ਸਥਿਰ ਪੂਰਨ
mch_tcgetattr(ਪੂਰਨ fd, ਖਾਲੀ *ਪਦ)
{
    ਪੂਰਨ		tty_fd;
    ਪੂਰਨ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ਜੇ (tty_fd < 0)
	ਵਾਪਸ -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ਬਣਤਰ termios *)ਪਦ);
#  ਨਹੀਂਤਾਂ
    retval = ioctl(tty_fd, TCGETA, (ਬਣਤਰ termio *)ਪਦ);
#  endif
# ਨਹੀਂਤਾਂ
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ਬਣਤਰ sgttyb *)ਪਦ);
# endif
    ਜੇ (tty_fd != fd)
	close(tty_fd);
    ਵਾਪਸ retval;
}

    ਖਾਲੀ
mch_settmode(tmode_T tmode)
{
    ਸਥਿਰ ਪੂਰਨ ਪਹਿਲਾ = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ਸਥਿਰ ਬਣਤਰ termios told;
	   ਬਣਤਰ termios tnew;
#  ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਬਣਤਰ termio told;
	   ਬਣਤਰ termio tnew;
#  endif

    ਜੇ (ਪਹਿਲਾ)
    {
	ਪਹਿਲਾ = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    ਜੇ (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ਨਿਰਾ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  ਜੇ defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   ਨਹੀਂਤਾਂ
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    ਨਹੀਂਤਾਂ
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    ਨਹੀਂਤਾਂ ਜੇ (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  ਜੇ defined(HAVE_TERMIOS_H)
    {
	ਪੂਰਨ	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	ਜਦੋਂ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  ਨਹੀਂਤਾਂ
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# ਨਹੀਂਤਾਂ
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ਸਥਿਰ ਬਣਤਰ sgttyb ttybold;
	   ਬਣਤਰ sgttyb ttybnew;

    ਜੇ (ਪਹਿਲਾ)
    {
	ਪਹਿਲਾ = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    ਜੇ (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    ਨਹੀਂਤਾਂ ਜੇ (tmode == TMODE_SLEEP)
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
    ਖਾਲੀ
get_stty(ਖਾਲੀ)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    ਜੇ (get_tty_info(read_cmd_fd, &info) != OK)
	ਵਾਪਸ;

    intr_char = info.ਰੁਕਾਵਟ;
    buf[0] = info.backspace;
    buf[1] = NUL;
    ਜੋੜੋ_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = ਲੱਭੋ_termcode((char_u *)"kD");
    ਜੇ (p != ਨਿਰਾ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ਨਿਰਾ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    ਪੂਰਨ
get_tty_info(ਪੂਰਨ fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ਬਣਤਰ termios ਕੁੰਜੀਆਂ;
#  ਨਹੀਂਤਾਂ
    ਬਣਤਰ termio ਕੁੰਜੀਆਂ;
#  endif

    ਜੇ (mch_tcgetattr(fd, &ਕੁੰਜੀਆਂ) != -1)
    {
	info->backspace = ਕੁੰਜੀਆਂ.c_cc[VERASE];
	info->ਰੁਕਾਵਟ = ਕੁੰਜੀਆਂ.c_cc[VINTR];
	ਜੇ (ਕੁੰਜੀਆਂ.c_iflag & ICRNL)
	    info->enter = NL;
	ਨਹੀਂਤਾਂ
	    info->enter = CAR;
	ਜੇ (ਕੁੰਜੀਆਂ.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	ਨਹੀਂਤਾਂ
	    info->nl_does_cr = FALSE;
	ਵਾਪਸ OK;
    }
# ਨਹੀਂਤਾਂ
    // for "old" tty systems
    ਬਣਤਰ sgttyb ਕੁੰਜੀਆਂ;

    ਜੇ (mch_tcgetattr(fd, &ਕੁੰਜੀਆਂ) != -1)
    {
	info->backspace = ਕੁੰਜੀਆਂ.sg_erase;
	info->ਰੁਕਾਵਟ = ਕੁੰਜੀਆਂ.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	ਵਾਪਸ OK;
    }
# endif
    ਵਾਪਸ FAIL;
}

#ਜੇ_ਅੰਤ // VMS

ਸਥਿਰ ਪੂਰਨ	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    ਖਾਲੀ
mch_setmouse(ਪੂਰਨ on)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
    ਸਥਿਰ ਪੂਰਨ	bevalterm_ison = FALSE;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		xterm_mouse_vers;

#ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ਜੇ (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#ਜੇ_ਅੰਤ

    ਜੇ (on == mouse_ison
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
	    && p_bevalterm == bevalterm_ison
#ਜੇ_ਅੰਤ
	    )
	// return quickly if nothing to do
	ਵਾਪਸ;

    xterm_mouse_vers = use_xterm_mouse();

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_URXVT
    ਜੇ (ttym_flags == TTYM_URXVT)
    {
	ਬਾਹਰਲਾ_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#ਜੇ_ਅੰਤ

    ਜੇ (T_CXM != ਨਿਰਾ && *T_CXM != NUL)
    {
	ਪਦ_enable_mouse(on);
    }
    ਨਹੀਂਤਾਂ ਜੇ (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	ਬਾਹਰਲਾ_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
    ਜੇ (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	ਜੇ (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    ਬਾਹਰਲਾ_str_nf((char_u *)("\033[?1003l"));
    }
#ਜੇ_ਅੰਤ

    ਜੇ (xterm_mouse_vers > 0)
    {
	ਜੇ (on)	// enable mouse events, use mouse tracking if available
	    ਬਾਹਰਲਾ_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_BEVAL_ਪਦ
			    bevalterm_ison ? "\033[?1003h" :
#ਜੇ_ਅੰਤ
			      "\033[?1002h")
			: "\033[?1000h"));
	ਨਹੀਂਤਾਂ	// disable mouse events, could probably always send the same
	    ਬਾਹਰਲਾ_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
    ਨਹੀਂਤਾਂ ਜੇ (ttym_flags == TTYM_DEC)
    {
	ਜੇ (on)	// enable mouse events
	    ਬਾਹਰਲਾ_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	ਨਹੀਂਤਾਂ	// disable mouse events
	    ਬਾਹਰਲਾ_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (on)
	{
	    ਜੇ (gpm_open())
		mouse_ison = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYSMOUSE
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (on)
	{
	    ਜੇ (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (on)
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
	    ਬਾਹਰਲਾ_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# ਨਹੀਂਤਾਂ
	    ਬਾਹਰਲਾ_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਬਾਹਰਲਾ_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
    ਨਹੀਂਤਾਂ
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	ਜੇ (on)
	    ਬਾਹਰਲਾ_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	ਨਹੀਂਤਾਂ
	    ਬਾਹਰਲਾ_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_BEVAL_ਪਦ)
/*
 * Called when 'balloonevalterm' changed.
 */
    ਖਾਲੀ
mch_bevalterm_ਬਦਲਿਆ(ਖਾਲੀ)
{
    mch_setmouse(mouse_ison);
}
#ਜੇ_ਅੰਤ

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    ਖਾਲੀ
check_mouse_termcode(ਖਾਲੀ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_XTERM
    ਜੇ (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
    {
	ਸੈੱਟ_mouse_termcode(KS_MOUSE, (char_u *)(ਪਦ_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	ਜੇ (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_MOUSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_GPM_MOUSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYSMOUSE
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_NETTERM_MOUSE);
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_DEC_MOUSE, (char_u *)(ਪਦ_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_DEC_MOUSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    ਜੇ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
	ਸੈੱਟ_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_PTERM_MOUSE);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_URXVT
    ਜੇ (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.ਵਿੱਚ_use
# endif
	    )
    {
	ਸੈੱਟ_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(ਪਦ_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	ਜੇ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ਨਹੀਂਤਾਂ
	del_mouse_termcode(KS_URXVT_MOUSE);
#ਜੇ_ਅੰਤ
    ਜੇ (use_xterm_mouse() == 4
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_GUI
	    && !gui.ਵਿੱਚ_use
#ਜੇ_ਅੰਤ
	    )
    {
	ਸੈੱਟ_mouse_termcode(KS_SGR_MOUSE, (char_u *)(ਪਦ_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	ਸੈੱਟ_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(ਪਦ_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	ਜੇ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ਨਹੀਂਤਾਂ
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 * Return OK when size could be determined, FAIL otherwise.
 */
    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    ਲੰਮਾ	ਪੰਕਤੀਆਂ_2 = 0;
    ਲੰਮਾ	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	ਬਣਤਰ winsize	ws;
	ਪੂਰਨ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ਜੇ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ਜੇ (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_ਖਾਨਾ;
	    ਪੰਕਤੀਆਂ_2 = ws.ws_ਪੰਕਤੀ;
#  ifdef FEAT_EVAL
	    ch_log(ਨਿਰਾ, "Got size with TIOCGWINSZ: %ld x %ld", columns, ਪੰਕਤੀਆਂ_2);
#  endif
	}
    }
# ਨਹੀਂਤਾਂ // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ਬਣਤਰ ttysize	ts;
	ਪੂਰਨ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ਜੇ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ਜੇ (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_ਖਾਨੇ_ਗਿਣਤੀ;
	    ਪੰਕਤੀਆਂ_2 = ts.ts_ਸਤਰਾਂ;
#   ifdef FEAT_EVAL
	    ch_log(ਨਿਰਾ, "Got size with TIOCGSIZE: %ld x %ld", columns, ਪੰਕਤੀਆਂ_2);
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
    ਜੇ (columns == 0 || ਪੰਕਤੀਆਂ_2 == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ਨਿਰਾ)
    {
	ਜੇ ((p = (char_u *)getenv("LINES")))
	{
	    ਪੰਕਤੀਆਂ_2 = atoi((ਅੱਖਰ *)p);
# ifdef FEAT_EVAL
	    ch_log(ਨਿਰਾ, "Got 'lines' from $LINES: %ld", ਪੰਕਤੀਆਂ_2);
# endif
	}
	ਜੇ ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((ਅੱਖਰ *)p);
# ifdef FEAT_EVAL
	    ch_log(ਨਿਰਾ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    ਜੇ (columns == 0 || ਪੰਕਤੀਆਂ_2 == 0)
    {
	getlinecol(&columns, &ਪੰਕਤੀਆਂ_2);
#  ifdef FEAT_EVAL
	ch_log(ਨਿਰਾ, "Got size from termcap: %ld x %ld", columns, ਪੰਕਤੀਆਂ_2);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    ਜੇ (columns <= 0 || ਪੰਕਤੀਆਂ_2 <= 0)
	ਵਾਪਸ FAIL;

    ਪੰਕਤੀਆਂ = ਪੰਕਤੀਆਂ_2;
    Columns = columns;
    ਹੱਦਬੰਦੀ_screen_ਮਾਪ();
    ਵਾਪਸ OK;
}

# ਜੇ defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    ਖਾਲੀ
mch_calc_cell_ਮਾਪ(ਬਣਤਰ cellsize *cs_ਬਾਹਰਲਾ)
{
   ਬਣਤਰ winsize   ws;
   ਪੂਰਨ		    retval;
   ਪੂਰਨ		    x_cell_ਮਾਪ;
   ਪੂਰਨ		    y_cell_ਮਾਪ;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ਨਿਰਾ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   ਜੇ (retval == -1 || ws.ws_ਖਾਨਾ == 0 || ws.ws_ਪੰਕਤੀ == 0)
   {
	cs_ਬਾਹਰਲਾ->cs_xpixel = -1;
	cs_ਬਾਹਰਲਾ->cs_ypixel = -1;
	ਵਾਪਸ;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_ਮਾਪ = ws.ws_xpixel / ws.ws_ਖਾਨਾ;
   y_cell_ਮਾਪ = ws.ws_ypixel / ws.ws_ਪੰਕਤੀ;

   cs_ਬਾਹਰਲਾ->cs_xpixel = x_cell_ਮਾਪ > 0 ? x_cell_ਮਾਪ : -1;
   cs_ਬਾਹਰਲਾ->cs_ypixel = y_cell_ਮਾਪ > 0 ? y_cell_ਮਾਪ : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   ਜੇ (x_cell_ਮਾਪ <= 0 || y_cell_ਮਾਪ <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       ਬਾਹਰਲਾ_STR("\033[14t");
       ਬਾਹਰਲਾ_STR("\033[16t");
       ਬਾਹਰਲਾ_flush();
   }
}
# endif

# ਜੇ defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    ਪੂਰਨ
mch_report_winsize(ਪੂਰਨ fd, ਪੂਰਨ ਪੰਕਤੀਆਂ_2, ਪੂਰਨ ਖਾਨੇ_ਗਿਣਤੀ)
{
    ਪੂਰਨ		tty_fd;
    ਪੂਰਨ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ਜੇ (tty_fd < 0)
	ਵਾਪਸ FAIL;

#  ਜੇ defined(TIOCSWINSZ)
    ਬਣਤਰ winsize ws;

    ws.ws_ਖਾਨਾ = ਖਾਨੇ_ਗਿਣਤੀ;
    ws.ws_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ_2;
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
    ch_log(ਨਿਰਾ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ਬਣਤਰ ttysize ts;

    ts.ts_ਖਾਨੇ_ਗਿਣਤੀ = ਖਾਨੇ_ਗਿਣਤੀ;
    ts.ts_ਸਤਰਾਂ = ਪੰਕਤੀਆਂ_2;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ਨਿਰਾ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    ਜੇ (tty_fd != fd)
	close(tty_fd);
    ਵਾਪਸ retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_shellsize(ਖਾਲੀ)
{
    ਜੇ (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	ਪਦ_ਸੈੱਟ_winsize((ਪੂਰਨ)ਪੰਕਤੀਆਂ, (ਪੂਰਨ)Columns);
	ਬਾਹਰਲਾ_flush();
	screen_ਸ਼ੁਰੂ();			// don't know where cursor is now
    }
}

#ਜੇ_ਅੰਤ // VMS

/*
 * Rows and/or Columns has changed.
 */
    ਖਾਲੀ
mch_new_shellsize(ਖਾਲੀ)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ਸਥਿਰ pid_t
wait4pid(pid_t ਬੱਚਾ, waitstatus *ਸਥਿਤੀ)
{
    pid_t wait_pid = 0;
    ਲੰਮਾ delay_msec = 1;

    ਜਦੋਂ (wait_pid != ਬੱਚਾ)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(ਬੱਚਾ, ਸਥਿਤੀ, WNOHANG);
	ਜੇ (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    ਜੇ (++delay_msec > 10)
		delay_msec = 10;
	    ਜਾਰੀ;
	}
	ਜੇ (wait_pid <= 0
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ECHILD
		&& errno == ECHILD
#ਜੇ_ਅੰਤ
	   )
	    ਰੋਕੋ;
    }
    ਵਾਪਸ wait_pid;
}

#ਜੇ !defined(USE_SYSTEM) || defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_ਬੱਚਾ_environment(
	ਲੰਮਾ	ਪੰਕਤੀਆਂ_2,
	ਲੰਮਾ	columns,
	ਅੱਖਰ	*ਪਦ,
	ਪੂਰਨ	is_terminal ਅਣਵਰਤਿਆ)
{
# ifdef HAVE_SETENV
    ਅੱਖਰ	envbuf[50];
# ਨਹੀਂਤਾਂ
    ਸਥਿਰ ਅੱਖਰ	envbuf_ਪਦ[30];
    ਸਥਿਰ ਅੱਖਰ	envbuf_ਪੰਕਤੀਆਂ[20];
    ਸਥਿਰ ਅੱਖਰ	envbuf_ਸਤਰਾਂ[20];
    ਸਥਿਰ ਅੱਖਰ	envbuf_Columns[20];
    ਸਥਿਰ ਅੱਖਰ	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ਸਥਿਰ ਅੱਖਰ	envbuf_ਸੰਸਕਰਣ[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ਸਥਿਰ ਅੱਖਰ	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", ਪਦ, 1);
    sprintf((ਅੱਖਰ *)envbuf, "%ld", ਪੰਕਤੀਆਂ_2);
    setenv("ROWS", (ਅੱਖਰ *)envbuf, 1);
    sprintf((ਅੱਖਰ *)envbuf, "%ld", ਪੰਕਤੀਆਂ_2);
    setenv("LINES", (ਅੱਖਰ *)envbuf, 1);
    sprintf((ਅੱਖਰ *)envbuf, "%ld", columns);
    setenv("COLUMNS", (ਅੱਖਰ *)envbuf, 1);
    sprintf((ਅੱਖਰ *)envbuf, "%d", t_colors);
    setenv("COLORS", (ਅੱਖਰ *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    ਜੇ (is_terminal)
    {
	sprintf((ਅੱਖਰ *)envbuf, "%ld",  (ਲੰਮਾ)get_vim_var_nr(VV_ਸੰਸਕਰਣ));
	setenv("VIM_TERMINAL", (ਅੱਖਰ *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ਨਿਰਾ ? "" : (ਅੱਖਰ *)serverName, 1);
#  endif
# ਨਹੀਂਤਾਂ
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_ਪਦ, ਆਕਾਰ(envbuf_ਪਦ), "TERM=%s", ਪਦ);
    putenv(envbuf_ਪਦ);
    vim_snprintf(envbuf_ਪੰਕਤੀਆਂ, ਆਕਾਰ(envbuf_ਪੰਕਤੀਆਂ), "ROWS=%ld", ਪੰਕਤੀਆਂ_2);
    putenv(envbuf_ਪੰਕਤੀਆਂ);
    vim_snprintf(envbuf_ਸਤਰਾਂ, ਆਕਾਰ(envbuf_ਸਤਰਾਂ), "LINES=%ld", ਪੰਕਤੀਆਂ_2);
    putenv(envbuf_ਸਤਰਾਂ);
    vim_snprintf(envbuf_Columns, ਆਕਾਰ(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, ਆਕਾਰ(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    ਜੇ (is_terminal)
    {
	vim_snprintf(envbuf_ਸੰਸਕਰਣ, ਆਕਾਰ(envbuf_ਸੰਸਕਰਣ),
			 "VIM_TERMINAL=%ld", (ਲੰਮਾ)get_vim_var_nr(VV_ਸੰਸਕਰਣ));
	putenv(envbuf_ਸੰਸਕਰਣ);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, ਆਕਾਰ(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ਨਿਰਾ ? "" : (ਅੱਖਰ *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_default_ਬੱਚਾ_environment(ਪੂਰਨ is_terminal)
{
    ਸੈੱਟ_ਬੱਚਾ_environment(ਪੰਕਤੀਆਂ, Columns, "dumb", is_terminal);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI) || defined(FEAT_ਕੰਮ_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ਸਥਿਰ ਖਾਲੀ
open_pty(ਪੂਰਨ *pty_master_fd, ਪੂਰਨ *pty_slave_fd, char_u **name1, char_u **name2)
{
    ਅੱਖਰ	*tty_name;

    ਜੇ (name1 != ਨਿਰਾ)
	*name1 = ਨਿਰਾ;
    ਜੇ (name2 != ਨਿਰਾ)
	*name2 = ਨਿਰਾ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    ਜੇ (*pty_master_fd < 0)
	ਵਾਪਸ;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# ਨਹੀਂਤਾਂ
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    ਜੇ (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (name1 != ਨਿਰਾ)
	    *name1 = vim_strsave((char_u *)tty_name);
	ਜੇ (name2 != ਨਿਰਾ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#ਜੇ_ਅੰਤ

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ਸਥਿਰ ਖਾਲੀ
may_send_sigint(ਪੂਰਨ c ਅਣਵਰਤਿਆ, pid_t pid ਅਣਵਰਤਿਆ, pid_t wpid ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ SIGINT
    ਜੇ (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# ਨਹੀਂਤਾਂ
	kill(0, SIGINT);
# endif
	ਜੇ (wpid > 0)
	    kill(wpid, SIGINT);
    }
#ਜੇ_ਅੰਤ
}

#ਜੇ !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    ਪੂਰਨ
unix_build_argv(
	char_u *cmd,
	ਅੱਖਰ ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    ਪੂਰਨ		argc;

    *sh_tofree = vim_strsave(p_sh);
    ਜੇ (*sh_tofree == ਨਿਰਾ)		// out of memory
	ਵਾਪਸ FAIL;

    ਜੇ (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	ਵਾਪਸ FAIL;
    *argvp = argv;

    ਜੇ (cmd != ਨਿਰਾ)
    {
	char_u	*s;
	char_u	*p;

	ਜੇ (extra_shell_arg != ਨਿਰਾ)
	    argv[argc++] = (ਅੱਖਰ *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	ਜੇ (*shcf_tofree == ਨਿਰਾ)    // out of memory
	    ਵਾਪਸ FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	ਜਦੋਂ (*p != NUL)
	{
	    argv[argc++] = (ਅੱਖਰ *)s;
	    ਜਦੋਂ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (ਅੱਖਰ *)cmd;
    }
    argv[argc] = ਨਿਰਾ;
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ਸਥਿਰ ਪੂਰਨ
mch_call_shell_terminal(
    char_u	*cmd,
    ਪੂਰਨ		options ਅਣਵਰਤਿਆ)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    char_u	*tofree1 = ਨਿਰਾ;
    char_u	*tofree2 = ਨਿਰਾ;
    ਪੂਰਨ		retval = -1;
    buf_T	*buf;
    ਕੰਮ_T	*ਕੰਮ;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ਜੇ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ਜਾਓ theend;

    init_ਕੰਮ_options(&opt);
    ch_log(ਨਿਰਾ, "starting terminal for system command '%s'", cmd);
    buf = ਪਦ_ਸ਼ੁਰੂ(ਨਿਰਾ, argv, &opt, ਪਦ_ਸ਼ੁਰੂ_SYSTEM);
    ਜੇ (buf == ਨਿਰਾ)
	ਜਾਓ theend;

    ਕੰਮ = ਪਦ_getjob(buf->b_ਪਦ);
    ++ਕੰਮ->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ਜੇ (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	ਜਦੋਂ (ਪਦ_use_loop())
	{
	    ਜੇ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_ਸਰਗਰਮ)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ਜੇ (terminal_loop(TRUE) == OK)
		    ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	    }
	    ਨਹੀਂਤਾਂ
		ਸਾਧਾਰਨ_cmd(&oa, TRUE);
	}
	retval = ਕੰਮ->jv_exitval;
	ch_log(ਨਿਰਾ, "system command finished");

	ਕੰਮ_unref(ਕੰਮ);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    ਜੇ (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_ਬਫਰ(DOBUF_WIPE, DOBUF_ਪਹਿਲਾ, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ਸਥਿਰ ਪੂਰਨ
mch_call_shell_system(
    char_u	*cmd,
    ਪੂਰਨ		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    ਅੱਖਰ	*ifn = ਨਿਰਾ;
    ਅੱਖਰ	*ofn = ਨਿਰਾ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    ਪੂਰਨ		x;

    ਬਾਹਰਲਾ_flush();

    ਜੇ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# ਜੇ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  ਜੇ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    ਜੇ (cmd == ਨਿਰਾ)
	x = system((ਅੱਖਰ *)p_sh);
    ਨਹੀਂਤਾਂ
    {
# ifdef VMS
	ਜੇ (ofn = strchr((ਅੱਖਰ *)cmd, '>'))
	    *ofn++ = '\0';
	ਜੇ (ifn = strchr((ਅੱਖਰ *)cmd, '<'))
	{
	    ਅੱਖਰ *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    ਜੇ (p)
		*p = '\0';
	}
	ਜੇ (ofn)
	    x = vms_sys((ਅੱਖਰ *)cmd, ofn, ifn);
	ਨਹੀਂਤਾਂ
	    x = system((ਅੱਖਰ *)cmd);
# ਨਹੀਂਤਾਂ
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ਨਿਰਾ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	ਜੇ (newcmd == ਨਿਰਾ)
	    x = 0;
	ਨਹੀਂਤਾਂ
	{
	    sprintf((ਅੱਖਰ *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ਨਿਰਾ ? "" : (ਅੱਖਰ *)extra_shell_arg,
		    (ਅੱਖਰ *)p_shcf,
		    (ਅੱਖਰ *)cmd);
	    x = system((ਅੱਖਰ *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_ਸਥਿਤੀ(x);
# endif
    ਜੇ (emsg_silent)
	;
    ਨਹੀਂਤਾਂ ਜੇ (x == 127)
	ਸੁਨੇਹਾ_puts(_("\nCannot execute shell sh\n"));
    ਨਹੀਂਤਾਂ ਜੇ (x && !(options & SHELL_SILENT))
    {
	ਸੁਨੇਹਾ_puts(_("\nshell returned "));
	ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)x);
	ਸੁਨੇਹਾ_putchar('\n');
    }

    ਜੇ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# ਜੇ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    ਵਾਪਸ x;
}

#ਨਹੀਂਤਾਂ // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ਸਥਿਰ ਪੂਰਨ
mch_call_shell_fork(
    char_u	*cmd,
    ਪੂਰਨ		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    ਪੂਰਨ		ਸਥਿਤੀ = -1;
    ਪੂਰਨ		retval = -1;
    ਅੱਖਰ	**argv = ਨਿਰਾ;
    char_u	*tofree1 = ਨਿਰਾ;
    char_u	*tofree2 = ਨਿਰਾ;
    ਪੂਰਨ		i;
    ਪੂਰਨ		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    ਪੂਰਨ		pty_slave_fd = -1;
# endif
    ਪੂਰਨ		fd_toshell[2];		// for pipes
    ਪੂਰਨ		fd_fromshell[2];
    ਪੂਰਨ		pipe_error = FALSE;
    ਪੂਰਨ		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    ਬਾਹਰਲਾ_flush();
    ਜੇ (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    ਜੇ (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    ਜੇ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ਜਾਓ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    ਜੇ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.ਵਿੱਚ_use && show_shell_mess)
# endif
		    )
    {
# ifdef FEAT_GUI
	/*
	 * Try to open a master pty.
	 * If this works, open the slave pty.
	 * If the slave can't be opened, close the master pty.
	 */
	ਜੇ (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ਨਿਰਾ, ਨਿਰਾ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	ਜੇ (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    ਜੇ (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		ਜੇ (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    ਜੇ (pipe_error)
	    {
		ਸੁਨੇਹਾ_puts(_("\nCannot create pipes\n"));
		ਬਾਹਰਲਾ_flush();
	    }
	}
    }

    ਜੇ (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	ਖੰਡ_ਸੰਕੇਤ(&curset);
	pid = fork();	// maybe we should use vfork()
	ਜੇ (pid == -1)
	{
	    UNBLOCK_ਸੰਕੇਤ(&curset);

	    ਸੁਨੇਹਾ_puts(_("\nCannot fork\n"));
	    ਜੇ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.ਵਿੱਚ_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		ਜੇ (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		ਨਹੀਂਤਾਂ				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	ਨਹੀਂਤਾਂ ਜੇ (pid == 0)	// child
	{
	    reset_ਸੰਕੇਤ();		// handle signals normally
	    UNBLOCK_ਸੰਕੇਤ(&curset);

# ifdef FEAT_EVAL
	    ਜੇ (ch_log_ਸਰਗਰਮ())
	    {
		ch_log(ਨਿਰਾ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    ਜੇ (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		ਪੂਰਨ fd;

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
		ਜੇ (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    ਨਹੀਂਤਾਂ ਜੇ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.ਵਿੱਚ_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		ਜੇ (p_stmp)
		{
		    (ਖਾਲੀ)setsid();
#  ਜੇ defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		ਜੇ (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    ਜੇ (options & SHELL_COOKED)
			ਤਿਆਰੀ_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (ਅੱਖਰ *)ਨਿਰਾ);
#  endif
		}
# endif
		ਸੈੱਟ_default_ਬੱਚਾ_environment(FALSE);

		/*
		 * stderr is only redirected when using the GUI, so that a
		 * program like gpg can still access the terminal to get a
		 * passphrase using stderr.
		 */
# ifdef FEAT_GUI
		ਜੇ (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    ਜੇ (gui.ਵਿੱਚ_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		ਨਹੀਂਤਾਂ
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
		    ਜੇ (gui.ਵਿੱਚ_use)
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
	ਨਹੀਂਤਾਂ			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_ਸੰਕੇਤ(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_ਸੰਕੇਤ(&curset);
# ifdef FEAT_ਕੰਮ_CHANNEL
	    ++dont_check_ਕੰਮ_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    ਜੇ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.ਵਿੱਚ_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    ਬਫਰ[BUFLEN + 1];
		ਪੂਰਨ	    ਬਫਰ_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		ਪੂਰਨ	    ta_len = 0;		// valid bytes in ta_buf[]
		ਪੂਰਨ	    len;
		ਪੂਰਨ	    p_ਹੋਰ_save;
		ਪੂਰਨ	    old_State;
		ਪੂਰਨ	    c;
		ਪੂਰਨ	    toshell_fd;
		ਪੂਰਨ	    fromshell_fd;
		garray_T    ga;
		ਪੂਰਨ	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   ਸ਼ੁਰੂ_tv;
# endif

# ifdef FEAT_GUI
		ਜੇ (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		ਨਹੀਂਤਾਂ
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
		p_ਹੋਰ_save = p_ਹੋਰ;
		p_ਹੋਰ = FALSE;
		old_State = State;
		State = ਬਹੁਲਕ_EXTERNCMD;	// don't redraw at window resize

		ਜੇ ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    ਜੇ ((wpid = fork()) == -1)
		    {
			ਸੁਨੇਹਾ_puts(_("\nCannot fork\n"));
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_ਸ਼ੁਰੂ.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			ਲਈ (;;)
			{
			    ਜੇ (lplen == 0)
				len = 0;
			    ਨਹੀਂਤਾਂ ਜੇ (lp[written] == NL)
				// NL -> NUL translation
				len = ਲਿਖ(toshell_fd, "", (size_t)1);
			    ਨਹੀਂਤਾਂ
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = ਲਿਖ(toshell_fd, (ਅੱਖਰ *)lp + written,
					   s == ਨਿਰਾ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    ਜੇ (len == (ਪੂਰਨ)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				ਜੇ (lnum != curbuf->b_op_ਸਿਰਾ.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_ਸਤਰ_ਗਿਣਤੀ
						    || curbuf->b_p_eol)))
				    vim_ignored = ਲਿਖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				ਜੇ (lnum > curbuf->b_op_ਸਿਰਾ.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ਰੋਕੋ;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    ਨਹੀਂਤਾਂ ਜੇ (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    ਨਹੀਂਤਾਂ // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		ਜੇ (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
# endif
		ਲਈ (;;)
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
		    ਜੇ (!(options & SHELL_EXPAND)
			    && ((options &
					 (SHELL_READ|SHELL_WRITE|SHELL_COOKED))
				      != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
						    || gui.ਵਿੱਚ_use
# endif
						    )
			    && wait_pid == 0
			    && (ta_len > 0 || noread_cnt > 4))
		    {
		      ਜੇ (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      ਜੇ (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			ਜੇ (len == 1 && (pty_master_fd < 0 || cmd != ਨਿਰਾ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    ਜੇ (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				close(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = ਪਦ_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			ਜੇ (pty_master_fd < 0)
			{
			    ਲਈ (i = ta_len; i < ta_len + len; ++i)
			    {
				ਜੇ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    ਸੁਨੇਹਾ_putchar(ta_buf[i]);
				ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
				{
				    ਪੂਰਨ l = (*mb_ptr2len)(ta_buf + i);

				    ਸੁਨੇਹਾ_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				ਨਹੀਂਤਾਂ
				    ਸੁਨੇਹਾ_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
			    ਬਾਹਰਲਾ_flush();
			}

			ta_len += len;

			/*
			 * Write the characters to the child, unless EOF has
			 * been typed for pipes.  Write one character at a
			 * time, to avoid losing too much typeahead.
			 * When writing buffer lines, drop the typed
			 * characters (only check for CTRL-C).
			 */
			ਜੇ (options & SHELL_WRITE)
			    ta_len = 0;
			ਨਹੀਂਤਾਂ ਜੇ (toshell_fd >= 0)
			{
			    len = ਲਿਖ(toshell_fd, (ਅੱਖਰ *)ta_buf, (size_t)1);
			    ਜੇ (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    ਜੇ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# ਨਹੀਂਤਾਂ
			kill(0, SIGINT);
# endif
			ਜੇ (wpid > 0)
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
		    ਜਦੋਂ (RealWaitForChar(fromshell_fd, 10L, ਨਿਰਾ, ਨਿਰਾ))
		    {
			len = read_eintr(fromshell_fd, ਬਫਰ
				+ ਬਫਰ_off, (size_t)(BUFLEN - ਬਫਰ_off)
				);
			ਜੇ (len <= 0)		    // end of file or error
			    ਜਾਓ ਮੁਕੰਮਲ;

			noread_cnt = 0;
			ਜੇ (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    ਲਈ (i = 0; i < len; ++i)
			    {
				ਜੇ (ਬਫਰ[i] == NL)
				    append_ga_ਸਤਰ(&ga);
				ਨਹੀਂਤਾਂ ਜੇ (ਬਫਰ[i] == NUL)
				    ga_append(&ga, NL);
				ਨਹੀਂਤਾਂ
				    ga_append(&ga, ਬਫਰ[i]);
			    }
			}
			ਨਹੀਂਤਾਂ ਜੇ (has_mbyte)
			{
			    ਪੂਰਨ		l;
			    char_u	*p;

			    len += ਬਫਰ_off;
			    ਬਫਰ[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    ਲਈ (p = ਬਫਰ; p < ਬਫਰ + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				ਜੇ (l == 0)
				    l = 1;  // NUL byte?
				ਨਹੀਂਤਾਂ ਜੇ (MB_BYTE2LEN(*p) != l)
				    ਰੋਕੋ;
			    }
			    ਜੇ (p == ਬਫਰ)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				ਜੇ (len >= 12)
				    ++p;
				ਨਹੀਂਤਾਂ
				{
				    ਬਫਰ_off = len;
				    ਜਾਰੀ;
				}
			    }
			    c = *p;
			    *p = NUL;
			    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)ਬਫਰ);
			    ਜੇ (p < ਬਫਰ + len)
			    {
				*p = c;
				ਬਫਰ_off = (ਬਫਰ + len) - p;
				mch_memmove(ਬਫਰ, p, ਬਫਰ_off);
				ਜਾਰੀ;
			    }
			    ਬਫਰ_off = 0;
			}
			ਨਹੀਂਤਾਂ
			{
			    ਬਫਰ[len] = NUL;
			    ਸੁਨੇਹਾ_puts((ਅੱਖਰ *)ਬਫਰ);
			}

			windgoto(ਸੁਨੇਹਾ_ਪੰਕਤੀ, cmdline_ਖਾਨਾ_off + ਸੁਨੇਹਾ_ਖਾਨਾ);
			cursor_on();
			ਬਾਹਰਲਾ_flush();
			ਜੇ (got_int)
			    ਰੋਕੋ;

# ifdef ELAPSED_FUNC
			ਜੇ (wait_pid == 0)
			{
			    ਲੰਮਾ	msec = ELAPSED_FUNC(ਸ਼ੁਰੂ_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    ਜੇ (msec > 2000)
			    {
				noread_cnt = 5;
				ਰੋਕੋ;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    ਜੇ (wait_pid == pid)
		    {
			ਜੇ (noread_cnt < 5)
			    ਜਾਰੀ;
			ਰੋਕੋ;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &ਸਥਿਤੀ, WNOHANG);
		    ਜੇ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(ਸਥਿਤੀ)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    ਨਹੀਂਤਾਂ
			wait_pid = 0;

# ਜੇ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
# endif
# ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
# endif
		}
ਮੁਕੰਮਲ:
		p_ਹੋਰ = p_ਹੋਰ_save;
		ਜੇ (options & SHELL_READ)
		{
		    ਜੇ (ga.ga_len > 0)
		    {
			append_ga_ਸਤਰ(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    ਨਹੀਂਤਾਂ
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		ਜੇ (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		ਜੇ (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# ਜੇ (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    ਨਹੀਂਤਾਂ
	    {
		ਲੰਮਾ delay_msec = 1;

		ਜੇ (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    ਬਾਹਰਲਾ_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		ਲਈ (;;)
		{
		    ਜੇ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  ਨਹੀਂਤਾਂ
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &ਸਥਿਤੀ, WNOHANG);
		    ਜੇ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(ਸਥਿਤੀ)))
		    {
			wait_pid = pid;
			ਰੋਕੋ;
		    }

#  ਜੇ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    ਜੇ (++delay_msec > 10)
			delay_msec = 10;
		}

		ਜੇ (tmode == TMODE_RAW)
		    // possibly enables modifyOtherKeys again
		    ਬਾਹਰਲਾ_str_t_TI();
	    }
# endif

	    /*
	     * Wait until our child has exited.
	     * Ignore wait() returning pids of other children and returning
	     * because of some signal like SIGWINCH.
	     * Don't wait if wait_pid was already set above, indicating the
	     * child already exited.
	     */
	    ਜੇ (wait_pid != pid)
		(ਖਾਲੀ)wait4pid(pid, &ਸਥਿਤੀ);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    ਜੇ (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    ਜੇ (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ਨਿਰਾ);
	    }

# ifdef FEAT_ਕੰਮ_CHANNEL
	    --dont_check_ਕੰਮ_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    ਜੇ (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    ਸੈੱਟ_ਸੰਕੇਤ();

	    ਜੇ (WIFEXITED(ਸਥਿਤੀ))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(ਸਥਿਤੀ);
		ਜੇ (retval != 0 && !emsg_silent)
		{
		    ਜੇ (retval == EXEC_FAILED)
		    {
			ਸੁਨੇਹਾ_puts(_("\nCannot execute shell "));
			ਸੁਨੇਹਾ_outtrans(p_sh);
			ਸੁਨੇਹਾ_putchar('\n');
		    }
		    ਨਹੀਂਤਾਂ ਜੇ (!(options & SHELL_SILENT))
		    {
			ਸੁਨੇਹਾ_puts(_("\nshell returned "));
			ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)retval);
			ਸੁਨੇਹਾ_putchar('\n');
		    }
		}
	    }
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_puts(_("\nCommand terminated\n"));
	}
    }

error:
    ਜੇ (!did_settmode)
	ਜੇ (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    ਵਾਪਸ retval;
}
#ਜੇ_ਅੰਤ // USE_SYSTEM

    ਪੂਰਨ
mch_call_shell(
    char_u	*cmd,
    ਪੂਰਨ		options)	// SHELL_*, see vim.h
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_EVAL
    ch_log(ਨਿਰਾ, "executing shell command: %s", cmd);
#ਜੇ_ਅੰਤ
#ਜੇ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    ਜੇ (gui.ਵਿੱਚ_use && vim_strchr(p_go, GO_TERMINAL) != ਨਿਰਾ
					      && (options & SHELL_SILENT) == 0)
	ਵਾਪਸ mch_call_shell_terminal(cmd, options);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ USE_SYSTEM
    ਵਾਪਸ mch_call_shell_system(cmd, options);
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ mch_call_shell_fork(cmd, options);
#ਜੇ_ਅੰਤ
}

#ਜੇ defined(FEAT_EVAL)
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
mch_get_cmd_output_ਸਿੱਧਾ(
    ਅੱਖਰ	**argv,
    char_u	*infile,
    ਪੂਰਨ		flags ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		*ret_len)
{
    pid_t	pid;
    ਪੂਰਨ		fd_ਬਾਹਰਲਾ[2] = {-1, -1};
    ਪੂਰਨ		ਸਥਿਤੀ = -1;
    char_u	*ਬਫਰ = ਨਿਰਾ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ਨਿਰਾ, "directly executing: %s", argv[0]);

    ਜੇ (pipe(fd_ਬਾਹਰਲਾ) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਖੰਡ_ਸੰਕੇਤ(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# ਨਹੀਂਤਾਂ
    pid = fork();
# endif
    ਜੇ (pid == -1)
    {
	UNBLOCK_ਸੰਕੇਤ(&curset);
	close(fd_ਬਾਹਰਲਾ[0]);
	close(fd_ਬਾਹਰਲਾ[1]);
	emsg(_("\nCannot fork\n"));
	ਵਾਪਸ ਨਿਰਾ;
    }

    ਜੇ (pid == 0)
    {
	// child process
	reset_ਸੰਕੇਤ();
	UNBLOCK_ਸੰਕੇਤ(&curset);

	ਜੇ (ch_log_ਸਰਗਰਮ())
	{
	    ch_log(ਨਿਰਾ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	ਜੇ (infile != ਨਿਰਾ)
	{
	    ਪੂਰਨ fd_ਵਿੱਚ = open((ਅੱਖਰ *)infile, O_RDONLY);
	    ਜੇ (fd_ਵਿੱਚ >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_ਵਿੱਚ);
		close(fd_ਵਿੱਚ);
	    }
	}
	ਨਹੀਂਤਾਂ
	{
	    ਪੂਰਨ nullfd = open("/dev/null", O_RDONLY);
	    ਜੇ (nullfd >= 0)
	    {
		close(0);
		vim_ignored = dup(nullfd);
		close(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	close(fd_ਬਾਹਰਲਾ[0]);
	close(1);
	vim_ignored = dup(fd_ਬਾਹਰਲਾ[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_ਬਾਹਰਲਾ[1]);
	close(fd_ਬਾਹਰਲਾ[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_ਸੰਕੇਤ(&curset);
    close(fd_ਬਾਹਰਲਾ[1]);

    // Read output from child.
    ਲਈ (;;)
    {
	ਅੱਖਰ	buf[4096];
	ਪੂਰਨ	n;

	n = (ਪੂਰਨ)ਪਠ(fd_ਬਾਹਰਲਾ[0], buf, ਆਕਾਰ(buf));
	ਜੇ (n <= 0)
	    ਰੋਕੋ;
	ਜੇ (ga_grow(&ga, n) == FAIL)
	    ਰੋਕੋ;
	mch_memmove((ਅੱਖਰ *)ga.ga_ਅੰਕੜੇ + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_ਬਾਹਰਲਾ[0]);

    // Wait for child to finish.
    (ਖਾਲੀ)waitpid(pid, &ਸਥਿਤੀ, 0);
    ਜੇ (WIFEXITED(ਸਥਿਤੀ))
	ਸਥਿਤੀ = WEXITSTATUS(ਸਥਿਤੀ);
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = -1;
    ਸੈੱਟ_vim_var_nr(VV_SHELL_ERROR, (ਲੰਮਾ)ਸਥਿਤੀ);

    ਜੇ (ga.ga_len > 0)
    {
	ਬਫਰ = alloc(ga.ga_len + 1);
	ਜੇ (ਬਫਰ != ਨਿਰਾ)
	{
	    mch_memmove(ਬਫਰ, ga.ga_ਅੰਕੜੇ, ga.ga_len);
	    ਜੇ (ret_len == ਨਿਰਾ)
	    {
		ਪੂਰਨ	i;

		// Change NUL into SOH, otherwise the string is truncated.
		ਲਈ (i = 0; i < ga.ga_len; ++i)
		    ਜੇ (ਬਫਰ[i] == NUL)
			ਬਫਰ[i] = 1;
		ਬਫਰ[ga.ga_len] = NUL;
	    }
	    ਨਹੀਂਤਾਂ
		*ret_len = ga.ga_len;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (ret_len != ਨਿਰਾ)
	*ret_len = 0;

    ga_clear(&ga);
    ਵਾਪਸ ਬਫਰ;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_ਕੰਮ_CHANNEL)
    ਖਾਲੀ
mch_ਕੰਮ_ਸ਼ੁਰੂ(ਅੱਖਰ **argv, ਕੰਮ_T *ਕੰਮ, jobopt_T *options, ਪੂਰਨ is_terminal)
{
    pid_t	pid;
    ਪੂਰਨ		fd_ਵਿੱਚ[2] = {-1, -1};	// for stdin
    ਪੂਰਨ		fd_ਬਾਹਰਲਾ[2] = {-1, -1};	// for stdout
    ਪੂਰਨ		fd_err[2] = {-1, -1};	// for stderr
    ਪੂਰਨ		pty_master_fd = -1;
    ਪੂਰਨ		pty_slave_fd = -1;
    channel_T	*channel = ਨਿਰਾ;
    ਪੂਰਨ		use_null_for_ਵਿੱਚ = options->jo_io[PART_ਵਿੱਚ] == JIO_NULL;
    ਪੂਰਨ		use_null_for_ਬਾਹਰਲਾ = options->jo_io[PART_ਬਾਹਰਲਾ] == JIO_NULL;
    ਪੂਰਨ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ਪੂਰਨ		use_file_for_ਵਿੱਚ = options->jo_io[PART_ਵਿੱਚ] == JIO_FILE;
    ਪੂਰਨ		use_file_for_ਬਾਹਰਲਾ = options->jo_io[PART_ਬਾਹਰਲਾ] == JIO_FILE;
    ਪੂਰਨ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ਪੂਰਨ		use_ਬਫਰ_for_ਵਿੱਚ = options->jo_io[PART_ਵਿੱਚ] == JIO_ਬਫਰ;
    ਪੂਰਨ		use_ਬਾਹਰਲਾ_for_err = options->jo_io[PART_ERR] == JIO_ਬਾਹਰਲਾ;
    SIGSET_DECL(curset)

    ਜੇ (use_ਬਾਹਰਲਾ_for_err && use_null_for_ਬਾਹਰਲਾ)
	use_null_for_err = TRUE;

    // default is to fail
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_FAILED;

    ਜੇ (options->jo_pty
	    && (!(use_file_for_ਵਿੱਚ || use_null_for_ਵਿੱਚ)
		|| !(use_file_for_ਬਾਹਰਲਾ || use_null_for_ਬਾਹਰਲਾ)
		|| !(use_ਬਾਹਰਲਾ_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &ਕੰਮ->jv_tty_ਬਾਹਰਲਾ, &ਕੰਮ->jv_tty_ਵਿੱਚ);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    ਜੇ (use_file_for_ਵਿੱਚ)
    {
	char_u *fname = options->jo_io_name[PART_ਵਿੱਚ];

	fd_ਵਿੱਚ[0] = mch_open((ਅੱਖਰ *)fname, O_RDONLY, 0);
	ਜੇ (fd_ਵਿੱਚ[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    ਨਹੀਂਤਾਂ
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	ਜੇ (!use_null_for_ਵਿੱਚ && (pty_master_fd < 0 || use_ਬਫਰ_for_ਵਿੱਚ)
							    && pipe(fd_ਵਿੱਚ) < 0)
	    ਜਾਓ failed;

    ਜੇ (use_file_for_ਬਾਹਰਲਾ)
    {
	char_u *fname = options->jo_io_name[PART_ਬਾਹਰਲਾ];

	fd_ਬਾਹਰਲਾ[1] = mch_open((ਅੱਖਰ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ਜੇ (fd_ਬਾਹਰਲਾ[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    ਨਹੀਂਤਾਂ ਜੇ (!use_null_for_ਬਾਹਰਲਾ && pty_master_fd < 0 && pipe(fd_ਬਾਹਰਲਾ) < 0)
	ਜਾਓ failed;

    ਜੇ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((ਅੱਖਰ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ਜੇ (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ਜਾਓ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    ਨਹੀਂਤਾਂ ਜੇ (!use_ਬਾਹਰਲਾ_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_ਸੈੱਟ & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	ਜਾਓ failed;

    ਜੇ (!use_null_for_ਵਿੱਚ || !use_null_for_ਬਾਹਰਲਾ || !use_null_for_err)
    {
	ਜੇ (options->jo_ਸੈੱਟ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ਜੇ (channel != ਨਿਰਾ)
		++channel->ch_refcount;
	}
	ਨਹੀਂਤਾਂ
	    channel = ਜੋੜੋ_channel();
	ਜੇ (channel == ਨਿਰਾ)
	    ਜਾਓ failed;
	ਜੇ (ਕੰਮ->jv_tty_ਬਾਹਰਲਾ != ਨਿਰਾ)
	    ch_log(channel, "using pty %s on fd %d",
					       ਕੰਮ->jv_tty_ਬਾਹਰਲਾ, pty_master_fd);
    }

    ਖੰਡ_ਸੰਕੇਤ(&curset);
    pid = fork();	// maybe we should use vfork()
    ਜੇ (pid == -1)
    {
	// failed to fork
	UNBLOCK_ਸੰਕੇਤ(&curset);
	ਜਾਓ failed;
    }
    ਜੇ (pid == 0)
    {
	ਪੂਰਨ	null_fd = -1;
	ਪੂਰਨ	stderr_ਚੱਲਦਾ = TRUE;

	// child
	reset_ਸੰਕੇਤ();		// handle signals normally
	UNBLOCK_ਸੰਕੇਤ(&curset);

# ifdef FEAT_EVAL
	ਜੇ (ch_log_ਸਰਗਰਮ())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(ਖਾਲੀ)setsid();
# endif

# ifdef FEAT_TERMINAL
	ਜੇ (options->jo_ਪਦ_ਪੰਕਤੀਆਂ > 0)
	{
	    ਅੱਖਰ *ਪਦ = (ਅੱਖਰ *)T_NAME;

#  ifdef FEAT_GUI
	    ਜੇ (ਪਦ_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		ਪਦ = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    ਜੇ (ਪਦ == ਨਿਰਾ || *ਪਦ == NUL || STRNCMP(ਪਦ, "xterm", 5) != 0)
	    {
		ਜੇ (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    ਪਦ = "xterm-256color";
		ਨਹੀਂਤਾਂ ਜੇ (t_colors > 16)
		    ਪਦ = "xterm-color";
		ਨਹੀਂਤਾਂ
		    ਪਦ = "xterm";
	    }
	    ਸੈੱਟ_ਬੱਚਾ_environment(
		    (ਲੰਮਾ)options->jo_ਪਦ_ਪੰਕਤੀਆਂ,
		    (ਲੰਮਾ)options->jo_ਪਦ_ਖਾਨੇ_ਗਿਣਤੀ,
		    ਪਦ,
		    is_terminal);
	}
	ਨਹੀਂਤਾਂ
# endif
	    ਸੈੱਟ_default_ਬੱਚਾ_environment(is_terminal);

	ਜੇ (options->jo_env != ਨਿਰਾ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    ਪੂਰਨ		todo = (ਪੂਰਨ)dict->dv_hashtab.ht_ਵਰਤਿਆ;

	    FOR_ALL_HASHTAB_ਚੀਜ਼ਾਂ(&dict->dv_hashtab, hi, todo)
		ਜੇ (!HASHITEM_ਖਾਲੀ_ਥਾਵਾਂ(hi))
		{
		    typval_T *ਵਸਤੂ = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ਕੁੰਜੀ, tv_get_string(ਵਸਤੂ));
		    --todo;
		}
	}

	ਜੇ (use_null_for_ਵਿੱਚ || use_null_for_ਬਾਹਰਲਾ || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    ਜੇ (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	ਜੇ (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    ਤਿਆਰੀ_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (ਅੱਖਰ *)ਨਿਰਾ);
# endif
	}

	// set up stdin for the child
	close(0);
	ਜੇ (use_null_for_ਵਿੱਚ && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ਨਹੀਂਤਾਂ ਜੇ (fd_ਵਿੱਚ[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ਨਹੀਂਤਾਂ
	    vim_ignored = dup(fd_ਵਿੱਚ[0]);

	// set up stderr for the child
	close(2);
	ਜੇ (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_ਚੱਲਦਾ = FALSE;
	}
	ਨਹੀਂਤਾਂ ਜੇ (use_ਬਾਹਰਲਾ_for_err)
	    vim_ignored = dup(fd_ਬਾਹਰਲਾ[1]);
	ਨਹੀਂਤਾਂ ਜੇ (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ਨਹੀਂਤਾਂ
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	ਜੇ (use_null_for_ਬਾਹਰਲਾ && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ਨਹੀਂਤਾਂ ਜੇ (fd_ਬਾਹਰਲਾ[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ਨਹੀਂਤਾਂ
	    vim_ignored = dup(fd_ਬਾਹਰਲਾ[1]);

	ਜੇ (fd_ਵਿੱਚ[0] >= 0)
	    close(fd_ਵਿੱਚ[0]);
	ਜੇ (fd_ਵਿੱਚ[1] >= 0)
	    close(fd_ਵਿੱਚ[1]);
	ਜੇ (fd_ਬਾਹਰਲਾ[0] >= 0)
	    close(fd_ਬਾਹਰਲਾ[0]);
	ਜੇ (fd_ਬਾਹਰਲਾ[1] >= 0)
	    close(fd_ਬਾਹਰਲਾ[1]);
	ਜੇ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ਜੇ (fd_err[1] >= 0)
	    close(fd_err[1]);
	ਜੇ (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	ਜੇ (null_fd >= 0)
	    close(null_fd);

	ਜੇ (options->jo_cwd != ਨਿਰਾ && mch_chdir((ਅੱਖਰ *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	ਜੇ (stderr_ਚੱਲਦਾ)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_ਸੰਕੇਤ(&curset);

    ਕੰਮ->jv_pid = pid;
    ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_STARTED;
    ਕੰਮ->jv_channel = channel;  // ch_refcount was set above

    ਜੇ (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    ਜੇ (fd_ਵਿੱਚ[0] >= 0)
	close(fd_ਵਿੱਚ[0]);
    ਜੇ (fd_ਬਾਹਰਲਾ[1] >= 0)
	close(fd_ਬਾਹਰਲਾ[1]);
    ਜੇ (fd_err[1] >= 0)
	close(fd_err[1]);
    ਜੇ (channel != ਨਿਰਾ)
    {
	ਪੂਰਨ ਵਿੱਚ_fd = INVALID_FD;
	ਪੂਰਨ ਬਾਹਰਲਾ_fd = INVALID_FD;
	ਪੂਰਨ err_fd = INVALID_FD;

	ਜੇ (!(use_file_for_ਵਿੱਚ || use_null_for_ਵਿੱਚ))
	    ਵਿੱਚ_fd = fd_ਵਿੱਚ[1] >= 0 ? fd_ਵਿੱਚ[1] : pty_master_fd;

	ਜੇ (!(use_file_for_ਬਾਹਰਲਾ || use_null_for_ਬਾਹਰਲਾ))
	    ਬਾਹਰਲਾ_fd = fd_ਬਾਹਰਲਾ[0] >= 0 ? fd_ਬਾਹਰਲਾ[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	ਜੇ (!(use_ਬਾਹਰਲਾ_for_err || use_file_for_err || use_null_for_err))
	{
	    ਜੇ (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    ਨਹੀਂਤਾਂ ਜੇ (ਬਾਹਰਲਾ_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_ਸੈੱਟ_pipes(channel, ਵਿੱਚ_fd, ਬਾਹਰਲਾ_fd, err_fd);
	channel_ਸੈੱਟ_ਕੰਮ(channel, ਕੰਮ, options);
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (fd_ਵਿੱਚ[1] >= 0)
	    close(fd_ਵਿੱਚ[1]);
	ਜੇ (fd_ਬਾਹਰਲਾ[0] >= 0)
	    close(fd_ਬਾਹਰਲਾ[0]);
	ਜੇ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ਜੇ (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    ਵਾਪਸ;

failed:
    channel_unref(channel);
    ਜੇ (fd_ਵਿੱਚ[0] >= 0)
	close(fd_ਵਿੱਚ[0]);
    ਜੇ (fd_ਵਿੱਚ[1] >= 0)
	close(fd_ਵਿੱਚ[1]);
    ਜੇ (fd_ਬਾਹਰਲਾ[0] >= 0)
	close(fd_ਬਾਹਰਲਾ[0]);
    ਜੇ (fd_ਬਾਹਰਲਾ[1] >= 0)
	close(fd_ਬਾਹਰਲਾ[1]);
    ਜੇ (fd_err[0] >= 0)
	close(fd_err[0]);
    ਜੇ (fd_err[1] >= 0)
	close(fd_err[1]);
    ਜੇ (pty_master_fd >= 0)
	close(pty_master_fd);
    ਜੇ (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ਸਥਿਰ char_u *
get_signal_name(ਪੂਰਨ sig)
{
    ਪੂਰਨ		i;
    char_u	numbuf[NUMBUFLEN];

    ਜੇ (sig == SIGKILL)
	ਵਾਪਸ vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    ਲਈ (i = 0; signal_info[i].sig != -1; i++)
	ਜੇ (sig == signal_info[i].sig)
	    ਵਾਪਸ strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((ਅੱਖਰ *)numbuf, NUMBUFLEN, "%d", sig);
    ਵਾਪਸ vim_strnsave(numbuf, i);
}

    ਅੱਖਰ *
mch_ਕੰਮ_ਸਥਿਤੀ(ਕੰਮ_T *ਕੰਮ)
{
    ਪੂਰਨ		ਸਥਿਤੀ = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(ਕੰਮ->jv_pid, &ਸਥਿਤੀ, WNOHANG);
    ਜੇ (wait_pid == -1)
    {
	ਪੂਰਨ waitpid_errno = errno;
	ਜੇ (waitpid_errno == ECHILD && mch_process_running(ਕੰਮ->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    ਵਾਪਸ "run";

	// process must have exited
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED)
	    ch_log(ਕੰਮ->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	ਜਾਓ return_dead;
    }
    ਜੇ (wait_pid == 0)
	ਵਾਪਸ "run";
    ਜੇ (WIFEXITED(ਸਥਿਤੀ))
    {
	// LINTED avoid "bitwise operation on signed value"
	ਕੰਮ->jv_exitval = WEXITSTATUS(ਸਥਿਤੀ);
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED)
	    ch_log(ਕੰਮ->jv_channel, "Job exited with %d", ਕੰਮ->jv_exitval);
	ਜਾਓ return_dead;
    }
    ਜੇ (WIFSIGNALED(ਸਥਿਤੀ))
    {
	ਕੰਮ->jv_exitval = -1;
	ਕੰਮ->jv_termsig = get_signal_name(WTERMSIG(ਸਥਿਤੀ));
	ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED && ਕੰਮ->jv_termsig != ਨਿਰਾ)
	    ch_log(ਕੰਮ->jv_channel, "Job terminated by signal \"%s\"",
							      ਕੰਮ->jv_termsig);
	ਜਾਓ return_dead;
    }
    ਵਾਪਸ "run";

return_dead:
    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED)
	ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ENDED;
    ਵਾਪਸ "dead";
}

    ਕੰਮ_T *
mch_detect_ended_ਕੰਮ(ਕੰਮ_T *ਕੰਮ_list)
{
    ਪੂਰਨ		ਸਥਿਤੀ = -1;
    pid_t	wait_pid = 0;
    ਕੰਮ_T	*ਕੰਮ;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    ਜੇ (dont_check_ਕੰਮ_ended > 0)
	ਵਾਪਸ ਨਿਰਾ;
# endif

    wait_pid = waitpid(-1, &ਸਥਿਤੀ, WNOHANG);
    ਜੇ (wait_pid <= 0)
	// no process ended
	ਵਾਪਸ ਨਿਰਾ;
    ਲਈ (ਕੰਮ = ਕੰਮ_list; ਕੰਮ != ਨਿਰਾ; ਕੰਮ = ਕੰਮ->jv_ਅਗਲਾ)
    {
	ਜੇ (ਕੰਮ->jv_pid == wait_pid)
	{
	    ਜੇ (WIFEXITED(ਸਥਿਤੀ))
		// LINTED avoid "bitwise operation on signed value"
		ਕੰਮ->jv_exitval = WEXITSTATUS(ਸਥਿਤੀ);
	    ਨਹੀਂਤਾਂ ਜੇ (WIFSIGNALED(ਸਥਿਤੀ))
	    {
		ਕੰਮ->jv_exitval = -1;
		ਕੰਮ->jv_termsig = get_signal_name(WTERMSIG(ਸਥਿਤੀ));
	    }
	    ਜੇ (ਕੰਮ->jv_ਸਥਿਤੀ < ਕੰਮ_ENDED)
	    {
		ch_log(ਕੰਮ->jv_channel, "Job ended");
		ਕੰਮ->jv_ਸਥਿਤੀ = ਕੰਮ_ENDED;
	    }
	    ਵਾਪਸ ਕੰਮ;
	}
    }
    ਵਾਪਸ ਨਿਰਾ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    ਪੂਰਨ
mch_signal_ਕੰਮ(ਕੰਮ_T *ਕੰਮ, char_u *how)
{
    ਪੂਰਨ	    sig = -1;

    ਜੇ (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(how, "int") == 0)
	sig = SIGINT;
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    ਨਹੀਂਤਾਂ ਜੇ (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    ਨਹੀਂਤਾਂ ਜੇ (ਸੁਰੱਖਿਅਤ_isdigit(*how))
	sig = atoi((ਅੱਖਰ *)how);
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ FAIL;

    // Never kill ourselves!
    ਜੇ (ਕੰਮ->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-ਕੰਮ->jv_pid, sig);
	kill(ਕੰਮ->jv_pid, sig);
    }

    ਵਾਪਸ OK;
}

/*
 * Clear the data related to "job".
 */
    ਖਾਲੀ
mch_clear_ਕੰਮ(ਕੰਮ_T *ਕੰਮ)
{
    // call waitpid because child process may become zombie
    (ਖਾਲੀ)waitpid(ਕੰਮ->jv_pid, ਨਿਰਾ, WNOHANG);
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_TERMINAL)
    ਪੂਰਨ
mch_create_pty_channel(ਕੰਮ_T *ਕੰਮ, jobopt_T *options)
{
    ਪੂਰਨ		pty_master_fd = -1;
    ਪੂਰਨ		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &ਕੰਮ->jv_tty_ਬਾਹਰਲਾ, &ਕੰਮ->jv_tty_ਵਿੱਚ);
    ਜੇ (pty_master_fd < 0 || pty_slave_fd < 0)
	ਵਾਪਸ FAIL;
    close(pty_slave_fd);

    channel = ਜੋੜੋ_channel();
    ਜੇ (channel == ਨਿਰਾ)
    {
	close(pty_master_fd);
	ਵਾਪਸ FAIL;
    }
    ਜੇ (ਕੰਮ->jv_tty_ਬਾਹਰਲਾ != ਨਿਰਾ)
	ch_log(channel, "using pty %s on fd %d",
					       ਕੰਮ->jv_tty_ਬਾਹਰਲਾ, pty_master_fd);
    ਕੰਮ->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_ਸੈੱਟ_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_ਸੈੱਟ_ਕੰਮ(channel, ਕੰਮ, options);
    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    ਖਾਲੀ
mch_breakcheck(ਪੂਰਨ force)
{
    ਜੇ ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ਨਿਰਾ, ਨਿਰਾ))
	fill_ਨਿਵੇਸ਼_buf(FALSE);
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
    ਸਥਿਰ ਪੂਰਨ
WaitForChar(ਲੰਮਾ msec, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_TIMERS
    ਵਾਪਸ ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_ਨਿਵੇਸ਼) == OK;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ WaitForCharOrMouse(msec, interrupted, ignore_ਨਿਵੇਸ਼);
#ਜੇ_ਅੰਤ
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
    ਸਥਿਰ ਪੂਰਨ
WaitForCharOrMouse(ਲੰਮਾ msec, ਪੂਰਨ *interrupted, ਪੂਰਨ ignore_ਨਿਵੇਸ਼)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_MOUSE_GPM
    ਪੂਰਨ		gpm_process_wanted;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_XCLIPBOARD
    ਪੂਰਨ		rest;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		avail;

    ਜੇ (!ignore_ਨਿਵੇਸ਼ && ਨਿਵੇਸ਼_ਉਪਲਬਧ())	    // something in inbuf[]
	ਵਾਪਸ 1;

#ਜੇ defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    ਜੇ (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	ਜੇ (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#ਜੇ_ਅੰਤ

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#ਜੇ defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    ਜੇ (do_xterm_trace())
	rest = msec;
# endif
    ਕਰੋ
    {
# ifdef FEAT_XCLIPBOARD
	ਜੇ (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    ਜੇ (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    ਜੇ (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	ਜੇ (has_sound_callback_ਵਿੱਚ_ਕਤਾਰ())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	ਜੇ (!avail && !gpm_process_wanted)
# ਨਹੀਂਤਾਂ
	avail = RealWaitForChar(read_cmd_fd, msec, ਨਿਰਾ, interrupted);
	ਜੇ (!avail)
# endif
	{
	    ਜੇ (!ignore_ਨਿਵੇਸ਼ && ਨਿਵੇਸ਼_ਉਪਲਬਧ())
		ਵਾਪਸ 1;
# ifdef FEAT_XCLIPBOARD
	    ਜੇ (rest == 0 || !do_xterm_trace())
# endif
		ਰੋਕੋ;
	}
    }
    ਜਦੋਂ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#ਨਹੀਂਤਾਂ
    avail = RealWaitForChar(read_cmd_fd, msec, ਨਿਰਾ, interrupted);
#ਜੇ_ਅੰਤ
    ਵਾਪਸ avail;
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ VMS
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
    ਸਥਿਰ ਪੂਰਨ
RealWaitForChar(ਪੂਰਨ fd, ਲੰਮਾ msec, ਪੂਰਨ *check_for_gpm ਅਣਵਰਤਿਆ, ਪੂਰਨ *interrupted)
{
    ਪੂਰਨ		ret;
    ਪੂਰਨ		ਨਤੀਜਾ;
# ਜੇ defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ਸਥਿਰ ਪੂਰਨ	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    ਲੰਮਾ	ਸ਼ੁਰੂ_msec = msec;
    elapsed_T	ਸ਼ੁਰੂ_tv;

    ਜੇ (msec > 0)
	ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    ਜੇ (busy)
	ਵਾਪਸ 0;
# endif

# ifdef MAY_LOOP
    ਲਈ (;;)
# endif
    {
# ifdef MAY_LOOP
	ਪੂਰਨ		ਮੁਕੰਮਲ = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	ਪੂਰਨ		mzquantum_ਵਰਤਿਆ = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	ਪੂਰਨ		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ਬਣਤਰ pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	ਪੂਰਨ		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	ਪੂਰਨ             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	ਪੂਰਨ		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	ਪੂਰਨ		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	ਪੂਰਨ		xsmp_idx = -1;
#  endif
	ਪੂਰਨ		towait = (ਪੂਰਨ)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_ਧਾਗੇ();
	ਜੇ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (ਪੂਰਨ)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_ਵਰਤਿਆ = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	ਜੇ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	ਜੇ (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_ਸੰਖਿਆ(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ਜੇ (check_for_gpm != ਨਿਰਾ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	ਜੇ (xsmp_icefd != -1)
	{
	    xsmp_idx = nfd;
	    fds[nfd].fd = xsmp_icefd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_ਕੰਮ_CHANNEL
	nfd = channel_poll_ਤਿਆਰੀ(nfd, &fds, &towait);
#  endif
	ਜੇ (interrupted != ਨਿਰਾ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	ਨਤੀਜਾ = ret > 0 && (fds[0].revents & POLLIN);
	ਜੇ (ਨਤੀਜਾ == 0 && interrupted != ਨਿਰਾ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	ਜੇ (ret == 0 && mzquantum_ਵਰਤਿਆ)
	    // MzThreads scheduling is required and timeout occurred
	    ਮੁਕੰਮਲ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ਜੇ (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	ਜੇ (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    ਜੇ (--ret == 0 && !ਨਿਵੇਸ਼_ਉਪਲਬਧ())
		// Try again
		ਮੁਕੰਮਲ = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ਜੇ (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	ਜੇ (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    ਜੇ (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_ਬੇਨਤੀਆਂ();
		busy = FALSE;
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (fds[xsmp_idx].revents & POLLHUP)
	    {
		ਜੇ (p_verbose > 0)
		    verb_ਸੁਨੇਹਾ(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    ਜੇ (--ret == 0)
		ਮੁਕੰਮਲ = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_ਕੰਮ_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ਜੇ (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# ਨਹੀਂਤਾਂ // HAVE_SELECT

	ਬਣਤਰ timeval  tv;
	ਬਣਤਰ timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ਸਥਿਰ fd_set	rfds, wfds, efds;
	ਪੂਰਨ		maxfd;
	ਲੰਮਾ		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_ਧਾਗੇ();
	ਜੇ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_ਵਰਤਿਆ = TRUE;
	}
#  endif

	ਜੇ (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	ਨਹੀਂਤਾਂ
	    tvp = ਨਿਰਾ;

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
	ਜੇ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    ਜੇ (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	ਜੇ (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_ਸੰਖਿਆ(xterm_dpy), &rfds);
	    ਜੇ (maxfd < Connection_ਸੰਖਿਆ(xterm_dpy))
		maxfd = Connection_ਸੰਖਿਆ(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ਜੇ (check_for_gpm != ਨਿਰਾ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    ਜੇ (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	ਜੇ (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    ਜੇ (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_ਕੰਮ_CHANNEL
	maxfd = channel_select_ਤਿਆਰੀ(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	ਜੇ (interrupted != ਨਿਰਾ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	ਨਤੀਜਾ = ret > 0 && FD_ISSET(fd, &rfds);
	ਜੇ (ਨਤੀਜਾ)
	    --ret;
	ਨਹੀਂਤਾਂ ਜੇ (interrupted != ਨਿਰਾ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	ਜੇ (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    ਜੇ (got_tstp && !ਵਿੱਚ_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    ਜੇ (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ਨਿਰਾ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    ਜਾਓ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	ਜੇ (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	ਜੇ (ret == 0 && mzquantum_ਵਰਤਿਆ)
	    // loop if MzThreads must be scheduled and timeout occurred
	    ਮੁਕੰਮਲ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ਜੇ (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	ਜੇ (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_ਸੰਖਿਆ(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    ਜੇ (--ret == 0 && !ਨਿਵੇਸ਼_ਉਪਲਬਧ())
	    {
		// Try again
		ਮੁਕੰਮਲ = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ਜੇ (ret > 0 && check_for_gpm != ਨਿਰਾ && gpm_flag && gpm_fd >= 0)
	{
	    ਜੇ (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    ਨਹੀਂਤਾਂ ਜੇ (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	ਜੇ (ret > 0 && xsmp_icefd != -1)
	{
	    ਜੇ (FD_ISSET(xsmp_icefd, &efds))
	    {
		ਜੇ (p_verbose > 0)
		    verb_ਸੁਨੇਹਾ(_("XSMP lost ICE connection"));
		xsmp_close();
		ਜੇ (--ret == 0)
		    ਮੁਕੰਮਲ = FALSE;   // keep going if event was only one
	    }
	    ਨਹੀਂਤਾਂ ਜੇ (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_ਬੇਨਤੀਆਂ();
		busy = FALSE;
		ਜੇ (--ret == 0)
		    ਮੁਕੰਮਲ = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_ਕੰਮ_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ਜੇ (ret >= 0)
	    (ਖਾਲੀ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	ਜੇ (ਮੁਕੰਮਲ || msec == 0)
	    ਰੋਕੋ;

#  ਜੇ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	ਜੇ (clientserver_ਵਿਧੀ == CLIENTSERVER_ਵਿਧੀ_X11 && server_ਉਡੀਕ())
	    ਰੋਕੋ;
#  endif

	// We're going to loop around again, find out for how long
	ਜੇ (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = ਸ਼ੁਰੂ_msec - ELAPSED_FUNC(ਸ਼ੁਰੂ_tv);
#  ਨਹੀਂਤਾਂ
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    ਜੇ (msec <= 0)
		ਰੋਕੋ;	// waited long enough
	}
# endif
    }

    ਵਾਪਸ ਨਤੀਜਾ;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ਪੂਰਨ
mch_expandpath(
    garray_T	*gap,
    char_u	*ਰਸਤਾ,
    ਪੂਰਨ		flags)		// EW_* flags
{
    ਵਾਪਸ unix_expandpath(gap, ਰਸਤਾ, 0, flags, FALSE);
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

    ਪੂਰਨ
mch_expand_wildcards(
    ਪੂਰਨ		   ਅੰਕ_pat,
    char_u	 **pat,
    ਪੂਰਨ		  *ਅੰਕ_file,
    char_u	***file,
    ਪੂਰਨ		   flags)	// EW_* flags
{
    ਪੂਰਨ		i;
    size_t	len;
    ਲੰਮਾ	llen;
    char_u	*p;
    ਪੂਰਨ		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    ਪੂਰਨ		j;
    char_u	*tempname;
    size_t	tempnamelen;
    char_u	*command;
    size_t	commandlen;
    FILE	*fd;
    char_u	*ਬਫਰ;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    ਪੂਰਨ		shell_style = STYLE_ECHO;
    ਪੂਰਨ		check_spaces;
    ਸਥਿਰ ਪੂਰਨ	did_ਲੱਭੋ_nul = FALSE;
    ਪੂਰਨ		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ਸਥਿਰ string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ਸਥਿਰ string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *ਅੰਕ_file = 0;	// default: no files found
    *file = ਨਿਰਾ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    ਜੇ (!have_wildcard(ਅੰਕ_pat, pat))
	ਵਾਪਸ save_patterns(ਅੰਕ_pat, pat, ਅੰਕ_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    ਜੇ (sandbox != 0 && check_secure())
	ਵਾਪਸ FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    ਜੇ (secure || restricted)
	ਲਈ (i = 0; i < ਅੰਕ_pat; ++i)
	    ਜੇ (vim_strchr(pat[i], '`') != ਨਿਰਾ
		    && (check_restricted() || check_secure()))
		ਵਾਪਸ FAIL;

    /*
     * get a name for the temp file
     */
    ਜੇ ((tempname = vim_tempname('o', FALSE)) == ਨਿਰਾ)
    {
	emsg(_(e_cant_get_ਆਰਜ਼ੀ_file_name));
	ਵਾਪਸ FAIL;
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
    ਜੇ (ਅੰਕ_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    ਨਹੀਂਤਾਂ ਜੇ ((len = STRLEN(p_sh)) >= 3)
    {
	ਜੇ (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	ਨਹੀਂਤਾਂ ਜੇ (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    ਜੇ (shell_style == STYLE_ECHO)
    {
	ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "bash") != ਨਿਰਾ)
	    shell_style = STYLE_GLOBSTAR;
	ਨਹੀਂਤਾਂ ਜੇ (ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)gettail(p_sh), "sh") != ਨਿਰਾ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    ਜੇ (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.ਲੰਮਾਈ;
    ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.ਲੰਮਾਈ + sh_globstar_opt.ਲੰਮਾਈ;

    ਲਈ (i = 0; i < ਅੰਕ_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# ਨਹੀਂਤਾਂ
	++len;				// add space
	ਲਈ (j = 0; pat[i][j] != NUL; ++j)
	{
	    ਜੇ (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ਨਿਰਾ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    ਜੇ (command == ਨਿਰਾ)
    {
	// out of memory
	vim_free(tempname);
	ਵਾਪਸ FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    ਜੇ (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)ਲੜੀ_ਖੋਜ((ਅੱਖਰ *)command, ")>");
	ਜੇ (p == ਨਿਰਾ)
	{
	    ;					    // can't happen ;-)
	}
	ਨਹੀਂਤਾਂ
	{
	    --p;
	    ਜਦੋਂ (p > command && VIM_ISWHITE(*p))
		--p;
	    ਜੇ (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    ਜਦੋਂ (p > command && VIM_ISWHITE(*p))
		--p;
	    ਜੇ (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "print -N >%s", tempname);
	ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	ਨਹੀਂਤਾਂ
	    commandlen = vim_snprintf((ਅੱਖਰ *)command, len, "echo >%s", tempname);

	ਲਈ (i = 0; i < ਅੰਕ_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((ਅੱਖਰ *)command + commandlen, len, " \"%s\"", pat[i]);
# ਨਹੀਂਤਾਂ
	    ਪੂਰਨ intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    ਲਈ (j = 0; pat[i][j] != NUL; ++j)
	    {
		ਜੇ (pat[i][j] == '`')
		    intick = !intick;
		ਨਹੀਂਤਾਂ ਜੇ (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    ਜੇ (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ਨਿਰਾ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		ਨਹੀਂਤਾਂ ਜੇ (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ਨਿਰਾ)
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
    ਜੇ (flags & EW_SILENT)
	show_shell_mess = FALSE;
    ਜੇ (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    ਜੇ (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_GLOB && !have_dollars(ਅੰਕ_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    ਜੇ (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ਨਿਰਾ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    ਜੇ (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	ਜੇ (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    ਸੁਨੇਹਾ_putchar('\n');		// clear bottom line quickly
	    cmdline_ਪੰਕਤੀ = ਪੰਕਤੀਆਂ - 1;	// continue on last line
# ifdef USE_SYSTEM
	    ਜੇ (!(flags & EW_SILENT))
# endif
	    {
		ਸੁਨੇਹਾ(_(e_cannot_expand_wildcards));
		ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	ਜੇ (shell_style == STYLE_BT)
	    ਵਾਪਸ FAIL;
	ਜਾਓ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((ਅੱਖਰ *)tempname, READBIN);
    ਜੇ (fd == ਨਿਰਾ)
    {
	// Something went wrong, perhaps a file name with a special char.
	ਜੇ (!(flags & EW_SILENT))
	{
	    ਸੁਨੇਹਾ(_(e_cannot_expand_wildcards));
	    ਸੁਨੇਹਾ_ਸ਼ੁਰੂ();		// don't overwrite this message
	}
	vim_free(tempname);
	ਜਾਓ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    ਜੇ (llen < 0)
	// just in case ftell() would fail
	ਬਫਰ = ਨਿਰਾ;
    ਨਹੀਂਤਾਂ
	ਬਫਰ = alloc(llen + 1);
    ਜੇ (ਬਫਰ == ਨਿਰਾ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	ਵਾਪਸ FAIL;
    }
    len = llen;
    i = fread((ਅੱਖਰ *)ਬਫਰ, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    ਜੇ (i != (ਪੂਰਨ)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(ਬਫਰ);
	ਵਾਪਸ FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = ਬਫਰ;
    ਲਈ (i = 0; i < (ਪੂਰਨ)len; ++i)
	ਜੇ (!(ਬਫਰ[i] == CAR && ਬਫਰ[i + 1] == NL))
	    *p++ = ਬਫਰ[i];
    len = p - ਬਫਰ;
# endif


    // file names are separated with Space
    ਜੇ (shell_style == STYLE_ECHO)
    {
	ਬਫਰ[len] = '\n';		// make sure the buffer ends in NL
	p = ਬਫਰ;
	ਲਈ (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    ਜਦੋਂ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    ਨਹੀਂਤਾਂ ਜੇ (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	ਬਫਰ[len] = NUL;		// make sure the buffer ends in NUL
	p = ਬਫਰ;
	ਲਈ (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    ਜਦੋਂ (*p != '\n' && *p != NUL)
		++p;
	    ਜੇ (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    ਨਹੀਂਤਾਂ
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
	ਜੇ (shell_style == STYLE_PRINT && !did_ਲੱਭੋ_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    ਬਫਰ[len] = NUL;
	    ਜੇ (len && (ਪੂਰਨ)STRLEN(ਬਫਰ) < (ਪੂਰਨ)len)
		did_ਲੱਭੋ_nul = TRUE;
	    ਨਹੀਂਤਾਂ
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	ਜੇ (len && ਬਫਰ[len - 1] == NUL)
	    --len;
	ਨਹੀਂਤਾਂ
	    ਬਫਰ[len] = NUL;
	i = 0;
	ਲਈ (p = ਬਫਰ; p < ਬਫਰ + len; ++p)
	    ਜੇ (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	ਜੇ (len)
	    ++i;			// count last entry
    }
    ਜੇ (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(ਬਫਰ);
	ਜਾਓ notfound;
    }
    *ਅੰਕ_file = i;
    *file = ALLOC_MULT(char_u *, i);
    ਜੇ (*file == ਨਿਰਾ)
    {
	// out of memory
	vim_free(ਬਫਰ);
	ਵਾਪਸ FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = ਬਫਰ;
    ਲਈ (i = 0; i < *ਅੰਕ_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	ਜੇ (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    ਜਦੋਂ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    ਜੇ (p == ਬਫਰ + len)		// last entry
		*p = NUL;
	    ਨਹੀਂਤਾਂ
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	ਨਹੀਂਤਾਂ		// NUL separates
	{
	    ਜਦੋਂ (*p && p < ਬਫਰ + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    ਲਈ (j = 0, i = 0; i < *ਅੰਕ_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	ਜੇ (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ਜਾਰੀ;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	ਜੇ ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ਜਾਰੀ;

	// Skip files that are not executable if we check for that.
	ਜੇ (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ਨਿਰਾ, !(flags & EW_SHELLCMD)))
	    ਜਾਰੀ;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	ਜੇ (p)
	{
	    STRCPY(p, (*file)[i]);
	    ਜੇ (dir)
		ਜੋੜੋ_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(ਬਫਰ);
    *ਅੰਕ_file = j;

    ਜੇ (*ਅੰਕ_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	ਜਾਓ notfound;
    }

    ਵਾਪਸ OK;

notfound:
    ਜੇ (flags & EW_NOTFOUND)
	ਵਾਪਸ save_patterns(ਅੰਕ_pat, pat, ਅੰਕ_file, file);
    ਵਾਪਸ FAIL;
}

#ਜੇ_ਅੰਤ // VMS

    ਸਥਿਰ ਪੂਰਨ
save_patterns(
    ਪੂਰਨ		ਅੰਕ_pat,
    char_u	**pat,
    ਪੂਰਨ		*ਅੰਕ_file,
    char_u	***file)
{
    ਪੂਰਨ		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, ਅੰਕ_pat);
    ਜੇ (*file == ਨਿਰਾ)
	ਵਾਪਸ FAIL;
    ਲਈ (i = 0; i < ਅੰਕ_pat; i++)
    {
	s = vim_strsave(pat[i]);
	ਜੇ (s != ਨਿਰਾ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *ਅੰਕ_file = ਅੰਕ_pat;
    ਵਾਪਸ OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    ਪੂਰਨ
mch_has_exp_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ
	    ਜੇ (vim_strchr((char_u *)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
				    "*?%"
#ਨਹੀਂਤਾਂ
				    "*?[{'"
#ਜੇ_ਅੰਤ
						, *p) != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    ਪੂਰਨ
mch_has_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ
	    ਜੇ (vim_strchr((char_u *)
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ VMS
				    "*?%$"
#ਨਹੀਂਤਾਂ
				    "*?[{`'$"
#ਜੇ_ਅੰਤ
						, *p) != ਨਿਰਾ
		|| (*p == '~' && p[1] != NUL))
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਸਥਿਰ ਪੂਰਨ
have_wildcard(ਪੂਰਨ ਅੰਕ, char_u **file)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < ਅੰਕ; i++)
	ਜੇ (mch_has_wildcard(file[i]))
	    ਵਾਪਸ 1;
    ਵਾਪਸ 0;
}

    ਸਥਿਰ ਪੂਰਨ
have_dollars(ਪੂਰਨ ਅੰਕ, char_u **file)
{
    ਪੂਰਨ	    i;

    ਲਈ (i = 0; i < ਅੰਕ; i++)
	ਜੇ (vim_strchr(file[i], '$') != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    ਵਾਪਸ FALSE;
}

#ਜੇ !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    ਪੂਰਨ
mch_rename(ਅਚਲ ਅੱਖਰ *src, ਅਚਲ ਅੱਖਰ *dest)
{
    ਬਣਤਰ stat	    st;

    ਜੇ (stat(dest, &st) >= 0)	    // fail if destination exists
	ਵਾਪਸ -1;
    ਜੇ (link(src, dest) != 0)	    // link file to new name
	ਵਾਪਸ -1;
    ਜੇ (mch_remove(src) == 0)	    // delete link to old name
	ਵਾਪਸ 0;
    ਵਾਪਸ -1;
}
#ਜੇ_ਅੰਤ // !HAVE_RENAME

#ਜੇ defined(FEAT_MOUSE_GPM)
# ਜੇ defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ਸਥਿਰ ਪੂਰਨ
ਲੱਦੋ_libgpm(ਖਾਲੀ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    ਜੇ (libgpm_hinst == ਨਿਰਾ)
    {
	ਜੇ (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	ਵਾਪਸ FAIL;
    }

    ਜੇ (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ਨਿਰਾ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ਨਿਰਾ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ਨਿਰਾ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ਨਿਰਾ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ਨਿਰਾ
      )
    {
	semsg(_(e_could_not_ਲੱਦੋ_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ਨਿਰਾ;
	dll_gpm_flag = ਨਿਰਾ;
	dll_gpm_fd   = ਨਿਰਾ;
	ਵਾਪਸ FAIL;
    }
    ਵਾਪਸ OK;
}

    ਪੂਰਨ
gpm_ਉਪਲਬਧ(ਖਾਲੀ)
{
    ਵਾਪਸ libgpm_hinst != ਨਿਰਾ || ਲੱਦੋ_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ਸਥਿਰ ਪੂਰਨ
gpm_open(ਖਾਲੀ)
{
    ਸਥਿਰ Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    ਜੇ (!gpm_ਉਪਲਬਧ())
	ਵਾਪਸ 0;
# endif

    ਜੇ (gpm_flag)
	ਵਾਪਸ 1; // already open

    gpm_connect.event_ਨਕਾਬ_2 = (GPM_ਉੱਪਰ | GPM_DRAG | GPM_ਥੱਲੇ);
    gpm_connect.default_ਨਕਾਬ = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    ਜੇ (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	ਵਾਪਸ 1; // succeed
    }
    ਜੇ (gpm_fd == -2)
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
    ਵਾਪਸ 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    ਪੂਰਨ
gpm_enabled(ਖਾਲੀ)
{
    ਵਾਪਸ gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ਸਥਿਰ ਖਾਲੀ
gpm_close(ਖਾਲੀ)
{
    ਜੇ (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ਸਥਿਰ ਪੂਰਨ
mch_gpm_process(ਖਾਲੀ)
{
    ਪੂਰਨ			button;
    ਸਥਿਰ Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    ਪੂਰਨ			ਪੰਕਤੀ,ਖਾਨਾ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ	buttons_ਨਕਾਬ;
    ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ	gpm_modifiers;
    ਸਥਿਰ ਬਿਨਾਨਿਸ਼ਾਨੀ ਅੱਖਰ old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ 0;
# endif

    ਪੰਕਤੀ = gpm_event.y - 1;
    ਖਾਨਾ = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    ਚੋਣ (GPM_BARE_EVENTS(gpm_event.type))
    {
	ਹਾਲਤ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ਰੋਕੋ;
	ਹਾਲਤ GPM_ਥੱਲੇ:
	    buttons_ਨਕਾਬ = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    ਚੋਣ (buttons_ਨਕਾਬ)
	    {
		ਹਾਲਤ GPM_B_ਖੱਬਾ:
		    button = MOUSE_ਖੱਬਾ;
		    ਰੋਕੋ;
		ਹਾਲਤ GPM_B_ਵਿਚਕਾਰ:
		    button = MOUSE_ਵਿਚਕਾਰ;
		    ਰੋਕੋ;
		ਹਾਲਤ GPM_B_ਸੱਜਾ:
		    button = MOUSE_ਸੱਜਾ;
		    ਰੋਕੋ;
		ਮੂਲ:
		    ਵਾਪਸ 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    ਸੈੱਟ_ਅੰਕ_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ਰੋਕੋ;
	ਹਾਲਤ GPM_ਉੱਪਰ:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ਰੋਕੋ;
	ਮੂਲ:
	    ਵਾਪਸ 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    ਜੇ (gpm_modifiers & ((1 << KG_ਖਿਸਕਾਓ) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_ਖਿਸਕਾਓ;

    ਜੇ (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    ਜੇ (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(ਖਾਨਾ + ' ' + 1);
    string[5] = (char_u)(ਪੰਕਤੀ + ' ' + 1);
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 6);
    ਵਾਪਸ 6;
}
#ਜੇ_ਅੰਤ // FEAT_MOUSE_GPM

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ਸਥਿਰ ਪੂਰਨ
sysmouse_open(ਖਾਲੀ)
{
    ਬਣਤਰ mouse_info   mouse;

    mouse.operation = MOUSE_ਬਹੁਲਕ;
    mouse.u.ਬਹੁਲਕ.ਬਹੁਲਕ = 0;
    mouse.u.ਬਹੁਲਕ.ਸੰਕੇਤ = SIGUSR2;
    ਜੇ (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	ਵਾਪਸ FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    ਵਾਪਸ OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ਸਥਿਰ ਖਾਲੀ
sysmouse_close(ਖਾਲੀ)
{
    ਬਣਤਰ mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_ਬਹੁਲਕ;
    mouse.u.ਬਹੁਲਕ.ਬਹੁਲਕ = 0;
    mouse.u.ਬਹੁਲਕ.ਸੰਕੇਤ = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ਸਥਿਰ ਖਾਲੀ
sig_sysmouse SIGDEFARG(sigarg)
{
    ਬਣਤਰ mouse_info	mouse;
    ਬਣਤਰ video_info	video;
    char_u		string[6];
    ਪੂਰਨ			ਪੰਕਤੀ, ਖਾਨਾ;
    ਪੂਰਨ			button;
    ਪੂਰਨ			buttons;
    ਸਥਿਰ ਪੂਰਨ		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ਜੇ (hold_gui_events)
	ਵਾਪਸ;
# endif

    mouse.operation = MOUSE_GETINFO;
    ਜੇ (ioctl(1, FBIO_GETMODE, &video.vi_ਬਹੁਲਕ) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	ਪੰਕਤੀ = mouse.u.ਅੰਕੜੇ.y / video.vi_cheight;
	ਖਾਨਾ = mouse.u.ਅੰਕੜੇ.x / video.vi_cwidth;
	buttons = mouse.u.ਅੰਕੜੇ.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	ਜੇ (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	ਨਹੀਂਤਾਂ
	{
	    ਚੋਣ (buttons)
	    {
		ਹਾਲਤ 0:
		    button = MOUSE_RELEASE;
		    ਰੋਕੋ;
		ਹਾਲਤ 1:
		    button = MOUSE_ਖੱਬਾ;
		    ਰੋਕੋ;
		ਹਾਲਤ 2:
		    button = MOUSE_ਵਿਚਕਾਰ;
		    ਰੋਕੋ;
		ਹਾਲਤ 4:
		    button = MOUSE_ਸੱਜਾ;
		    ਰੋਕੋ;
		ਮੂਲ:
		    ਵਾਪਸ;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(ਖਾਨਾ + ' ' + 1);
	string[5] = (char_u)(ਪੰਕਤੀ + ' ' + 1);
	ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(string, 6);
    }
    ਵਾਪਸ;
}
#ਜੇ_ਅੰਤ // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ਪੂਰਨ
mch_get_random(char_u *buf, ਪੂਰਨ len)
{
    ਸਥਿਰ ਪੂਰਨ dev_urandom_state = NOTDONE;

    ਜੇ (dev_urandom_state == FAIL)
	ਵਾਪਸ FAIL;

    ਪੂਰਨ fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    ਜੇ (fd == -1)
	dev_urandom_state = FAIL;
    ਨਹੀਂਤਾਂ ਜੇ (ਪਠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    ਨਹੀਂਤਾਂ
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    ਵਾਪਸ dev_urandom_state;
}

#ਜੇ defined(FEAT_LIBCALL)
ਕਿਸਮਪਰਿਭਾਸ਼ਾ char_u * (*STRPROCSTR)(char_u *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ char_u * (*INTPROCSTR)(ਪੂਰਨ);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*STRPROCINT)(char_u *);
ਕਿਸਮਪਰਿਭਾਸ਼ਾ ਪੂਰਨ (*INTPROCINT)(ਪੂਰਨ);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    ਪੂਰਨ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ਪੂਰਨ		argint,
    char_u	**string_ਨਤੀਜਾ, // NULL when using number_result
    ਪੂਰਨ		*ਸੰਖਿਆ_ਨਤੀਜਾ)
{
# ਜੇ defined(USE_DLOPEN)
    ਖਾਲੀ	*hinstLib;
    ਅੱਖਰ	*dlerr = ਨਿਰਾ;
# ਨਹੀਂਤਾਂ
    shl_t	hinstLib;
# endif
    STRPROCSTR	Proc_ਜੋੜੋ;
    INTPROCSTR	Proc_ਜੋੜੋ_I;
    char_u	*retval_str = ਨਿਰਾ;
    ਪੂਰਨ		retval_int = 0;
    ਪੂਰਨ		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# ਜੇ defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (ਖਾਲੀ)dlerror();

    hinstLib = dlopen((ਅੱਖਰ *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    ਜੇ (hinstLib == ਨਿਰਾ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	ਜੇ (dlerr != ਨਿਰਾ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# ਨਹੀਂਤਾਂ
    hinstLib = shl_ਲੱਦੋ((ਅਚਲ ਅੱਖਰ*)libname, BIND_ਤੁਰੰਤ|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    ਜੇ (hinstLib != ਨਿਰਾ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	ਜੇ (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  ਜੇ defined(USE_DLOPEN)
	    dlerr = ਨਿਰਾ;
#  endif
	    mch_didjmp();
	}
	ਨਹੀਂਤਾਂ
# endif
	{
	    retval_str = ਨਿਰਾ;
	    retval_int = 0;

	    ਜੇ (argstring != ਨਿਰਾ)
	    {
# ਜੇ defined(USE_DLOPEN)
		*(ਖਾਲੀ **)(&Proc_ਜੋੜੋ) = dlsym(hinstLib, (ਅਚਲ ਅੱਖਰ *)funcname);
		dlerr = dlerror();
# ਨਹੀਂਤਾਂ
		ਜੇ (shl_findsym(&hinstLib, (ਅਚਲ ਅੱਖਰ *)funcname,
					TYPE_PROCEDURE, (ਖਾਲੀ *)&Proc_ਜੋੜੋ) < 0)
		    Proc_ਜੋੜੋ = ਨਿਰਾ;
# endif
		ਜੇ ((success = (Proc_ਜੋੜੋ != ਨਿਰਾ
# ਜੇ defined(USE_DLOPEN)
			    && dlerr == ਨਿਰਾ
# endif
			    )))
		{
		    ਜੇ (string_ਨਤੀਜਾ == ਨਿਰਾ)
			retval_int = ((STRPROCINT)(ਖਾਲੀ *)Proc_ਜੋੜੋ)(argstring);
		    ਨਹੀਂਤਾਂ
			retval_str = (Proc_ਜੋੜੋ)(argstring);
		}
	    }
	    ਨਹੀਂਤਾਂ
	    {
# ਜੇ defined(USE_DLOPEN)
		*(ਖਾਲੀ **)(&Proc_ਜੋੜੋ_I) = dlsym(hinstLib, (ਅਚਲ ਅੱਖਰ *)funcname);
		dlerr = dlerror();
# ਨਹੀਂਤਾਂ
		ਜੇ (shl_findsym(&hinstLib, (ਅਚਲ ਅੱਖਰ *)funcname,
				       TYPE_PROCEDURE, (ਖਾਲੀ *)&Proc_ਜੋੜੋ_I) < 0)
		    Proc_ਜੋੜੋ_I = ਨਿਰਾ;
# endif
		ਜੇ ((success = (Proc_ਜੋੜੋ_I != ਨਿਰਾ
# ਜੇ defined(USE_DLOPEN)
			    && dlerr == ਨਿਰਾ
# endif
			    )))
		{
		    ਜੇ (string_ਨਤੀਜਾ == ਨਿਰਾ)
			retval_int = ((INTPROCINT)(ਖਾਲੀ *)Proc_ਜੋੜੋ_I)(argint);
		    ਨਹੀਂਤਾਂ
			retval_str = (Proc_ਜੋੜੋ_I)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    ਜੇ (string_ਨਤੀਜਾ == ਨਿਰਾ)
		*ਸੰਖਿਆ_ਨਤੀਜਾ = retval_int;
	    ਨਹੀਂਤਾਂ ਜੇ (retval_str != ਨਿਰਾ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_ਨਤੀਜਾ = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	ਜੇ (lc_signal != 0)
	{
	    ਪੂਰਨ i;

	    // try to find the name of this signal
	    ਲਈ (i = 0; signal_info[i].sig != -1; i++)
		ਜੇ (lc_signal == signal_info[i].sig)
		    ਰੋਕੋ;
	    semsg(_(e_got_sig_str_ਵਿੱਚ_libcall), signal_info[i].name);
	}
#  endif
# endif

# ਜੇ defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	ਜੇ (dlerr != ਨਿਰਾ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(ਖਾਲੀ)dlclose(hinstLib);
# ਨਹੀਂਤਾਂ
	(ਖਾਲੀ)shl_unload(hinstLib);
# endif
    }

    ਜੇ (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
}
#ਜੇ_ਅੰਤ

#ਜੇ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ਸਥਿਰ ਪੂਰਨ	xterm_trace = -1;	// default: disabled
ਸਥਿਰ ਪੂਰਨ	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    ਖਾਲੀ
ਤਿਆਰੀ_ਪਦ_clip(ਖਾਲੀ)
{
    ਪੂਰਨ		z = 0;
    ਅੱਖਰ	*strp = "";
    Widget	AppShell;

    ਜੇ (!x_connect_ਮੰਜ਼ਿਲ_server())
	ਵਾਪਸ;

    open_app_context();
    ਜੇ (app_context != ਨਿਰਾ && xterm_Shell == (Widget)0)
    {
	ਪੂਰਨ (*oldhandler)(Display*, XErrorEvent*);
# ਜੇ defined(USING_SETJMP)
	ਪੂਰਨ (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ਸ਼ੁਰੂ_tv;

	ਜੇ (p_verbose > 0)
	    ELAPSED_INIT(ਸ਼ੁਰੂ_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# ਜੇ defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	ਜੇ (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ਨਿਰਾ;
	}
	ਨਹੀਂਤਾਂ
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ਨਿਰਾ, 0, &z, &strp);
	    ਜੇ (xterm_dpy != ਨਿਰਾ)
		xterm_dpy_retry_ਗਿਣਤੀ = 0;
# ਜੇ defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# ਜੇ defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(ਖਾਲੀ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(ਖਾਲੀ)XSetErrorHandler(oldhandler);

	ਜੇ (xterm_dpy == ਨਿਰਾ)
	{
	    ਜੇ (p_verbose > 0)
		verb_ਸੁਨੇਹਾ(_("Opening the X display failed"));
	    ਵਾਪਸ;
	}

	// Catch terminating error of the X server connection.
	(ਖਾਲੀ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	ਜੇ (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_ਸੁਨੇਹਾ(ELAPSED_FUNC(ਸ਼ੁਰੂ_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ਨਿਰਾ);
	ਜੇ (AppShell == (Widget)0)
	    ਵਾਪਸ;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		ਸਿਖਰ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ਨਿਰਾ);
	ਜੇ (xterm_Shell == (Widget)0)
	    ਵਾਪਸ;

	x11_ਤਿਆਰੀ_atoms(xterm_dpy);
	x11_ਤਿਆਰੀ_selection(xterm_Shell);
	ਜੇ (x11_display == ਨਿਰਾ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    ਜੇ (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	ਜੇ (x11_ਖਿੜਕੀ == 0 && (strp = getenv("WINDOWID")) != ਨਿਰਾ)
	    x11_ਖਿੜਕੀ = (ਖਿੜਕੀ)atol(strp);
	// Check if $WINDOWID is valid.
	ਜੇ (test_x11_ਖਿੜਕੀ(xterm_dpy) == FAIL)
	    x11_ਖਿੜਕੀ = 0;
	ਜੇ (x11_ਖਿੜਕੀ != 0)
	    xterm_trace = 0;
    }
}

    ਖਾਲੀ
ਸ਼ੁਰੂ_xterm_trace(ਪੂਰਨ button)
{
    ਜੇ (x11_ਖਿੜਕੀ == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	ਵਾਪਸ;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    ਖਾਲੀ
stop_xterm_trace(ਖਾਲੀ)
{
    ਜੇ (xterm_trace < 0)
	ਵਾਪਸ;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ਸਥਿਰ ਪੂਰਨ
do_xterm_trace(ਖਾਲੀ)
{
    ਖਿੜਕੀ		ਜੜ੍ਹ, ਬੱਚਾ;
    ਪੂਰਨ			ਜੜ੍ਹ_x, ਜੜ੍ਹ_y;
    ਪੂਰਨ			win_x, win_y;
    ਪੂਰਨ			ਪੰਕਤੀ, ਖਾਨਾ;
    int_u		ਨਕਾਬ_return;
    char_u		buf[50];
    char_u		*strp;
    ਲੰਮਾ		got_hints;
    ਸਥਿਰ char_u	*mouse_ਕੋਡ = ਨਿਰਾ;
    ਸਥਿਰ size_t	mouse_codelen = 0;
    ਸਥਿਰ char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ਸਥਿਰ ਪੂਰਨ		prev_ਪੰਕਤੀ = 0, prev_ਖਾਨਾ = 0;
    ਸਥਿਰ XSizeHints	xterm_hints;

    ਜੇ (xterm_trace <= 0)
	ਵਾਪਸ FALSE;

    ਜੇ (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	ਜੇ (!XGetWMNormalHints(xterm_dpy, x11_ਖਿੜਕੀ, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.ਚੌੜਾਈ_inc <= 1
		|| xterm_hints.ਉਚਾਈ_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    ਵਾਪਸ FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_ਕੋਡ = ਲੱਭੋ_termcode(mouse_name);
	ਜੇ (mouse_ਕੋਡ != ਨਿਰਾ)
	    mouse_codelen = STRLEN(mouse_ਕੋਡ);
	prev_ਪੰਕਤੀ = mouse_ਪੰਕਤੀ;
	prev_ਖਾਨਾ = mouse_ਖਾਨਾ;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQuery_ਸੂਚਕ(xterm_dpy, x11_ਖਿੜਕੀ, &ਜੜ੍ਹ, &ਬੱਚਾ, &ਜੜ੍ਹ_x, &ਜੜ੍ਹ_y,
		      &win_x, &win_y, &ਨਕਾਬ_return);
	xterm_hints.y = win_y - (xterm_hints.ਉਚਾਈ_inc * mouse_ਪੰਕਤੀ)
			      - (xterm_hints.ਉਚਾਈ_inc / 2);
	ਜੇ (xterm_hints.y <= xterm_hints.ਉਚਾਈ_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.ਚੌੜਾਈ_inc * mouse_ਖਾਨਾ)
			      - (xterm_hints.ਚੌੜਾਈ_inc / 2);
	ਜੇ (xterm_hints.x <= xterm_hints.ਚੌੜਾਈ_inc / 2)
	    xterm_hints.x = 2;
	ਵਾਪਸ TRUE;
    }

    ਜੇ (mouse_ਕੋਡ == ਨਿਰਾ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	ਵਾਪਸ FALSE;
    }

    XQuery_ਸੂਚਕ(xterm_dpy, x11_ਖਿੜਕੀ, &ਜੜ੍ਹ, &ਬੱਚਾ, &ਜੜ੍ਹ_x, &ਜੜ੍ਹ_y,
		  &win_x, &win_y, &ਨਕਾਬ_return);

    ਪੰਕਤੀ = check_ਪੰਕਤੀ((win_y - xterm_hints.y) / xterm_hints.ਉਚਾਈ_inc);
    ਖਾਨਾ = check_ਖਾਨਾ((win_x - xterm_hints.x) / xterm_hints.ਚੌੜਾਈ_inc);
    ਜੇ (ਪੰਕਤੀ == prev_ਪੰਕਤੀ && ਖਾਨਾ == prev_ਖਾਨਾ)
	ਵਾਪਸ TRUE;

    STRCPY(buf, mouse_ਕੋਡ);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(ਖਾਨਾ + ' ' + 1);
    *strp++ = (char_u)(ਪੰਕਤੀ + ' ' + 1);
    *strp = NUL;
    ਜੋੜੋ_ਮੰਜ਼ਿਲ_ਨਿਵੇਸ਼_buf(buf, strp - buf);

    prev_ਪੰਕਤੀ = ਪੰਕਤੀ;
    prev_ਖਾਨਾ = ਖਾਨਾ;
    ਵਾਪਸ TRUE;
}

# ਜੇ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    ਖਾਲੀ
clear_xterm_clip(ਖਾਲੀ)
{
    ਜੇ (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    ਜੇ (xterm_dpy != ਨਿਰਾ)
    {
#  ਜੇ 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	ਜੇ (x11_display == xterm_dpy)
	    x11_display = ਨਿਰਾ;
	xterm_dpy = ਨਿਰਾ;
    }
#  ਜੇ 0
    ਜੇ (app_context != (XtAppContext)ਨਿਰਾ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ਨਿਰਾ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ਸਥਿਰ ਖਾਲੀ
clip_update(ਖਾਲੀ)
{
# ifdef FEAT_GUI
    ਜੇ (gui.ਵਿੱਚ_use)
	gui_mch_update();
    ਨਹੀਂਤਾਂ
# endif
    ਜੇ (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ਖਾਲੀ
xterm_update(ਖਾਲੀ)
{
    XEvent event;

    ਲਈ (;;)
    {
	Xt_ਨਿਵੇਸ਼_ਨਕਾਬ ਨਕਾਬ = XtAppPending(app_context);

	ਜੇ (ਨਕਾਬ == 0 || vim_is_ਨਿਵੇਸ਼_buf_ਭਰਿਆ())
	    ਰੋਕੋ;

	ਜੇ (ਨਕਾਬ & XtIMXEvent)
	{
	    // There is an event to process.
	    Xt_App_ਅਗਲਾ_Event(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		ਜੇ (e->type == PropertyNotify && e->ਖਿੜਕੀ_2 == comm_ਖਿੜਕੀ
		   && e->atom == commProperty && e->state == Property_New_ਮੁੱਲ)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	ਨਹੀਂਤਾਂ
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, ਨਕਾਬ);
	}
    }
}

    ਪੂਰਨ
clip_xterm_own_selection(Clipboard_T *cbd)
{
    ਜੇ (xterm_Shell != (Widget)0)
	ਵਾਪਸ clip_x11_own_selection(xterm_Shell, cbd);
    ਵਾਪਸ FAIL;
}

    ਖਾਲੀ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    ਜੇ (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    ਖਾਲੀ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    ਜੇ (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    ਖਾਲੀ
clip_xterm_ਸੈੱਟ_selection(Clipboard_T *cbd)
{
    clip_x11_ਸੈੱਟ_selection(cbd);
}
#ਜੇ_ਅੰਤ


#ਜੇ defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# ਜੇ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_handle_interaction(SmcConn smc_conn, Sm_ਸੂਚਕ client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ		save_cmod_flags;
    ਪੂਰਨ		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    ਜੇ (check_ਬਦਲਿਆ_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    ਬਾਹਰਲਾ_flush();

    // Done interaction
    Smc_Interact_ਪੂਰਾ(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    ਜੇ (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	Smc_Save_Yourself_ਪੂਰਾ(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    Sm_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ,
    ਪੂਰਨ		save_type ਅਣਵਰਤਿਆ,
    Bool	shutdown,
    ਪੂਰਨ		interact_style ਅਣਵਰਤਿਆ,
    Bool	fast ਅਣਵਰਤਿਆ)
{
    // Handle already being in saveyourself
    ਜੇ (xsmp.save_yourself)
	Smc_Save_Yourself_ਪੂਰਾ(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    ਬਾਹਰਲਾ_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    ਜੇ (p_verbose > 0)
	verb_ਸੁਨੇਹਾ(_("XSMP handling save-yourself request"));

# ਜੇ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    ਜੇ (shutdown && !fast && gui.ਵਿੱਚ_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_ਅੰਕੜੇ_2);
    ਨਹੀਂਤਾਂ
# endif
    {
	// Can stop the cycle here
	Smc_Save_Yourself_ਪੂਰਾ(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_die(SmcConn smc_conn ਅਣਵਰਤਿਆ, Sm_ਸੂਚਕ client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_save_complete(
    SmcConn	smc_conn ਅਣਵਰਤਿਆ,
    Sm_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ)
{
    xsmp.save_yourself = False;
}


/*
 * Callback to tell us that an instigated shutdown was cancelled
 * (maybe even by us)
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    Sm_ਸੂਚਕ	client_ਅੰਕੜੇ_2 ਅਣਵਰਤਿਆ)
{
    ਜੇ (xsmp.save_yourself)
	Smc_Save_Yourself_ਪੂਰਾ(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ਸਥਿਰ ਖਾਲੀ
xsmp_ice_connection(
    IceConn	iceConn,
    Ice_ਸੂਚਕ	client_ਅੰਕੜੇ ਅਣਵਰਤਿਆ,
    Bool	opening,
    Ice_ਸੂਚਕ	*watch_ਅੰਕੜੇ ਅਣਵਰਤਿਆ)
{
    // Intercept creation of ICE connection fd
    ਜੇ (opening)
    {
	xsmp_icefd = Ice_Connection_ਸੰਖਿਆ(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ਨਿਰਾ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    ਪੂਰਨ
xsmp_handle_ਬੇਨਤੀਆਂ(ਖਾਲੀ)
{
    Bool rep;

    ਜੇ (IceProcessMessages(xsmp.iceconn, ਨਿਰਾ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	ਜੇ (p_verbose > 0)
	    verb_ਸੁਨੇਹਾ(_("XSMP lost ICE connection"));
	xsmp_close();
	ਵਾਪਸ FAIL;
    }
    ਨਹੀਂਤਾਂ
	ਵਾਪਸ OK;
}

ਸਥਿਰ ਪੂਰਨ dummy;

// Set up X Session Management Protocol
    ਖਾਲੀ
xsmp_init(ਖਾਲੀ)
{
    ਅੱਖਰ		errorstring[80];
    SmcCallbacks	smcallbacks;
# ਜੇ 0
    Sm_Prop_ਮੁੱਲ		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    ਜੇ (p_verbose > 0)
	verb_ਸੁਨੇਹਾ(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_ਅੰਕੜੇ_2 = ਨਿਰਾ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_ਅੰਕੜੇ_2 = ਨਿਰਾ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_ਅੰਕੜੇ_2 = ਨਿਰਾ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_ਅੰਕੜੇ_2 = ਨਿਰਾ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    ਜੇ (Ice_ਜੋੜੋ_Connection_Watch(xsmp_ice_connection, &dummy) == 0)
    {
	ਜੇ (p_verbose > 0)
	    verb_ਸੁਨੇਹਾ(_("XSMP ICE connection watch failed"));
	ਵਾਪਸ;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ਨਿਰਾ,
	    ਨਿਰਾ,
	    SmProtoMajor,
	    SmProtoMinor,
	    Smc_Save_Yourself_Proc_ਨਕਾਬ | Smc_Die_Proc_ਨਕਾਬ
		     | Smc_Save_Complete_Proc_ਨਕਾਬ | Smc_Shutdown_Cancelled_Proc_ਨਕਾਬ,
	    &smcallbacks,
	    ਨਿਰਾ,
	    &xsmp.clientid,
	    ਆਕਾਰ(errorstring) - 1,
	    errorstring);
    ਜੇ (xsmp.smcconn == ਨਿਰਾ)
    {
	ਜੇ (p_verbose > 0)
	{
	    ਅੱਖਰ errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[ਆਕਾਰ(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, ਆਕਾਰ(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_ਸੁਨੇਹਾ(errorreport);
	}
	ਵਾਪਸ;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# ਜੇ 0
    // ID ourselves
    smname.ਮੁੱਲ = "vim";
    smname.ਲੰਮਾਈ = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.ਅੰਕ_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    Smc_ਸੈੱਟ_Properties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    ਖਾਲੀ
xsmp_close(ਖਾਲੀ)
{
    ਜੇ (xsmp_icefd == -1)
	ਵਾਪਸ;

    SmcCloseConnection(xsmp.smcconn, 0, ਨਿਰਾ);
    ਜੇ (xsmp.clientid != ਨਿਰਾ)
	ਮੁਕਤ(xsmp.clientid);
    xsmp.clientid = ਨਿਰਾ;
    xsmp_icefd = -1;
}
#ਜੇ_ਅੰਤ // USE_XSMP

#ਜੇ defined(FEAT_RELTIME)
# ਜੇ defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t timeout_flag = FALSE;
ਸਥਿਰ timer_t		     timer_id;
ਸਥਿਰ ਪੂਰਨ		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_flag(ਸੰਘ sigval ਅਣਵਰਤਿਆ_3 ਅਣਵਰਤਿਆ)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ਖਾਲੀ
stop_timeout(ਖਾਲੀ)
{
    ਸਥਿਰ ਬਣਤਰ itimerspec disarm = {{0, 0}, {0, 0}};

    ਜੇ (timer_created)
    {
	ਪੂਰਨ ret = timer_settime(timer_id, 0, &disarm, ਨਿਰਾ);

	ਜੇ (ret < 0)
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
    ਅਸਥਿਰ sig_atomic_t *
ਸ਼ੁਰੂ_timeout(ਲੰਮਾ msec)
{
    ਬਣਤਰ itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    ਪੂਰਨ ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    ਜੇ (!timer_created)
    {
	ਬਣਤਰ sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = ਸੈੱਟ_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	ਜੇ (ret < 0)
	{
	    semsg(_(e_could_not_ਸੈੱਟ_timeout_str), strerror(errno));
	    ਵਾਪਸ &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ਨਿਰਾ, "setting timeout timer to %d sec %ld nsec",
	       (ਪੂਰਨ)interval.it_value.tv_sec, (ਲੰਮਾ)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ਨਿਰਾ);
    ਜੇ (ret < 0)
	semsg(_(e_could_not_ਸੈੱਟ_timeout_str), strerror(errno));

    ਵਾਪਸ &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    ਖਾਲੀ
delete_timer(ਖਾਲੀ)
{
    ਜੇ (!timer_created)
	ਵਾਪਸ;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# ਨਹੀਂਤਾਂ // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ਸਥਿਰ ਬਣਤਰ sigaction		prev_sigaction;
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t	timeout_flag	     = FALSE;
ਸਥਿਰ ਪੂਰਨ			timer_ਸਰਗਰਮ	     = FALSE;
ਸਥਿਰ ਪੂਰਨ			timer_handler_ਸਰਗਰਮ = FALSE;
ਸਥਿਰ ਅਸਥਿਰ sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ਸਥਿਰ ਖਾਲੀ
ਸੈੱਟ_flag SIGDEFARG(sigarg)
{
    ਜੇ (alarm_pending)
	alarm_pending = FALSE;
    ਨਹੀਂਤਾਂ
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ਖਾਲੀ
stop_timeout(ਖਾਲੀ)
{
    ਸਥਿਰ ਬਣਤਰ itimerval disarm = {{0, 0}, {0, 0}};
    ਪੂਰਨ			    ret;

    ਜੇ (timer_ਸਰਗਰਮ)
    {
	timer_ਸਰਗਰਮ = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ਨਿਰਾ);
	ਜੇ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    ਜੇ (timer_handler_ਸਰਗਰਮ)
    {
	timer_handler_ਸਰਗਰਮ = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ਨਿਰਾ);
	ਜੇ (ret < 0)
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
    ਅਸਥਿਰ sig_atomic_t *
ਸ਼ੁਰੂ_timeout(ਲੰਮਾ msec)
{
    ਬਣਤਰ itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ਬਣਤਰ sigaction	handle_alarm;
    ਪੂਰਨ			ret;
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
    ਜੇ (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ਨਿਰਾ);
    }
    ਜੇ (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = ਸੈੱਟ_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    ਜੇ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_ਸੈੱਟ_handler_for_timeout_str), strerror(errno));
	ਵਾਪਸ &timeout_flag;
    }
    timer_handler_ਸਰਗਰਮ = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ਨਿਰਾ);
    ਜੇ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_ਸੈੱਟ_timeout_str), strerror(errno));
	stop_timeout();
	ਵਾਪਸ &timeout_flag;
    }

    timer_ਸਰਗਰਮ = TRUE;
    ਵਾਪਸ &timeout_flag;
}
# endif // PROF_NSEC
#ਜੇ_ਅੰਤ  // FEAT_RELTIME
