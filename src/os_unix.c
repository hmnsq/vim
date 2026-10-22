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

#ଅନ୍ତର୍ଭାଵ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ଅନ୍ତର୍ଭାଵ "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != ଖାଲି ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ଖାଲି ? *dll_gpm_fd   : -1)

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_Gpm_Open)     (Gpm_Connect *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_Gpm_Close)    (ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*dll_Gpm_GetEvent) (Gpm_Event *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dll_gpm_flag;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା *dll_gpm_fd;

ସ୍ଥିର ଶୂନ୍ୟ *libgpm_hinst;
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

ସ୍ଥିର ଶୂନ୍ୟ gpm_close(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା gpm_open(ଶୂନ୍ୟ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା mch_gpm_process(ଶୂନ୍ୟ);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sysmouse_open(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ sysmouse_close(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#ଯଦି defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#ଯଦି defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
ସ୍ଥିର Widget	xterm_Shell = (Widget)0;
ସ୍ଥିର ଶୂନ୍ୟ clip_update(ଶୂନ୍ୟ);
# endif

Window	    x11_window = 0;
Display	    *x11_display = ଖାଲି;
#endif

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_sigtstp = FALSE;

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_x11_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

ସ୍ଥିର char_u	*oldtitle = ଖାଲି;
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t oldtitle_outdated = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	unix_did_set_title = FALSE;
ସ୍ଥିର char_u	*oldicon = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_set_icon = FALSE;

ସ୍ଥିର ଶୂନ୍ୟ may_core_dump(ଶୂନ୍ୟ);

ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା waitstatus;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  WaitForChar(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  WaitForCharOrMouse(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ);
#ifdef VMS
ପୂର୍ଣ୍ଣସଂଖ୍ୟା  RealWaitForChar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, long, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted);
#ଅନ୍ୟଥା
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  RealWaitForChar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, long, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା do_xterm_trace(ଶୂନ୍ୟ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

ସ୍ଥିର ଶୂନ୍ୟ handle_resize(ଶୂନ୍ୟ);

#ଯଦି defined(SIGWINCH)
ସ୍ଥିର ଶୂନ୍ୟ sig_winch SIGPROTOARG;
#endif
#ଯଦି defined(SIGTSTP)
ସ୍ଥିର ଶୂନ୍ୟ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t in_mch_suspend = FALSE;
#endif
#ଯଦି defined(SIGINT)
ସ୍ଥିର ଶୂନ୍ୟ catch_sigint SIGPROTOARG;
#endif
#ଯଦି defined(SIGUSR1)
ସ୍ଥିର ଶୂନ୍ୟ catch_sigusr1 SIGPROTOARG;
#endif
#ଯଦି defined(SIGPWR)
ସ୍ଥିର ଶୂନ୍ୟ catch_sigpwr SIGPROTOARG;
#endif
#ଯଦି defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
ସ୍ଥିର ଶୂନ୍ୟ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t sig_alarm_called;
#endif
ସ୍ଥିର ଶୂନ୍ୟ deathtrap SIGPROTOARG;

ସ୍ଥିର ଶୂନ୍ୟ catch_int_signal(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ set_signals(ଶୂନ୍ୟ);
ସ୍ଥିର ଶୂନ୍ୟ catch_signals(ଶୂନ୍ୟ (*func_deadly)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା), ଶୂନ୍ୟ (*func_other)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#ଅନ୍ୟଥା
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	କର { /**/ } ଯେତେବେଳେ (0)
# define UNBLOCK_SIGNALS(set)	କର { /**/ } ଯେତେବେଳେ (0)
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  have_wildcard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u **);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା  have_dollars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା, char_u **);

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା save_patterns(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ_pat, char_u **pat, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *ଅଙ୍କ_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t got_tstp = FALSE;
ସ୍ଥିର char_u	*extra_shell_arg = ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#ଯଦି defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ସ୍ଥିର tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    ଅକ୍ଷର *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ସ୍ଥିର xsmp_config_T xsmp;
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

ସ୍ଥିର ସଂରଚନା signalinfo
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sig;	// Signal number, eg. SIGSEGV etc
    ଅକ୍ଷର    *name;	// Signal name (not char_u!).
    ଅକ୍ଷର    deadly;	// Catch as a deadly signal?
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
#ଯଦି defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#ଯଦି defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#ଯଦି defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#ଯଦି defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig, sighandler_T func)
{
#ଯଦି defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ସଂରଚନା sigaction	sa, old;
    sigset_t		curset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			blocked;

    ଯଦି (sigprocmask(SIG_BLOCK, ଖାଲି, &curset) == -1)
	ଫେରନ୍ତୁ SIG_ERR;

    blocked = sigismember(&curset, sig);

    ଯଦି (func == SIG_HOLD)
    {
	ଯଦି (blocked)
	    ଫେରନ୍ତୁ SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ଯଦି (sigaction(sig, ଖାଲି, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ଖାଲି) == -1)
	    ଫେରନ୍ତୁ SIG_ERR;
	ଫେରନ୍ତୁ old.sa_handler;
    }

    ଯଦି (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ଯଦି (sigprocmask(SIG_UNBLOCK, &curset, ଖାଲି) == -1)
	    ଫେରନ୍ତୁ SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# ଅନ୍ୟଥା
    sa.sa_flags = 0;
# endif
    ଯଦି (sigaction(sig, &sa, &old) == -1)
	ଫେରନ୍ତୁ SIG_ERR;
    ଫେରନ୍ତୁ blocked ? SIG_HOLD: old.sa_handler;
#ନଚେତଯଦି defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    ଫେରନ୍ତୁ sigset(sig, func);
#ଅନ୍ୟଥା
    // Oldest and most compatible solution.
    ଫେରନ୍ତୁ ସଙ୍କେତ(sig, func);
#endif
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_chdir(ଅକ୍ଷର *path)
{
    ଯଦି (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    ଫେରନ୍ତୁ chdir(vms_fixfilename(path));
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ chdir(path);
#endif
}

#ଯଦି defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    ଶୂନ୍ୟ
mch_write(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    vim_ignored = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ଲିଖ(1, (ଅକ୍ଷର *)s, len);
    ଯଦି (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ଖାଲି, ଖାଲି);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
resize_func(ପୂର୍ଣ୍ଣସଂଖ୍ୟା check_only)
{
    ଯଦି (check_only)
	ଫେରନ୍ତୁ do_resize;
    ଯେତେବେଳେ (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(ଖାଲି, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_inchar(
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tb_change_cnt)
{
    ଫେରନ୍ତୁ inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ସ୍ଥିର ଶୂନ୍ୟ
handle_resize(ଶୂନ୍ୟ)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_char_avail(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ WaitForChar(0L, ଖାଲି, FALSE);
}

#ଯଦି defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_messages(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ WaitForChar(0L, ଖାଲି, TRUE);
}
#endif

#ଯଦି defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# ଯଦି defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# ଯଦି defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ସଂରଚନା vm_statistics64	vm_stat;
	natural_t		ଗଣନା_2 = HOST_VM_INFO64_ଗଣନା;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ଗଣନା_2);
#  ଅନ୍ୟଥା
	ସଂରଚନା vm_statistics	vm_stat;
	natural_t		ଗଣନା_2 = HOST_VM_INFO_ଗଣନା;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ଗଣନା_2);
#  endif
	ଯଦି (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ଗଣନା + vm_stat.active_ଗଣନା
					    + vm_stat.inactive_ଗଣନା
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_ଗଣନା
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    ଯଦି (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mib[2];
	size_t		len = ଆକାର(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  ଅନ୍ୟଥା
	// sysctl() may return 32 bit or 64 bit, accept both
	ସଂଘ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  ଅନ୍ୟଥା
	mib[1] = HW_USERMEM;
#  endif
	ଯଦି (sysctl(mib, 2, &physmem, &len, ଖାଲି, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  ଅନ୍ୟଥା
	    ଯଦି (len == ଆକାର(physmem.u64))
		mem = (long_u)physmem.u64;
	    ଅନ୍ୟଥା
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# ଯଦି defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    ଯଦି (mem == 0)
    {
	ସଂରଚନା sysinfo sinfo;

	// Linux way of getting amount of RAM available
	ଯଦି (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    ଯେତେବେଳେ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  ଅନ୍ୟଥା
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    ଯଦି (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	ଯଦି (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    ଯେତେବେଳେ (shiftright > 0 && (pagesize & 1) == 0)
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
# ଯଦି defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ସଂରଚନା rlimit	rlp;

	ଯଦି (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (ଆକାର(long_u) * 8 - 1))
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

    ଯଦି (mem > 0)
	ଫେରନ୍ତୁ mem >> shiftright;
    ଫେରନ୍ତୁ (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    ଶୂନ୍ୟ
mch_delay(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    tmode_T	old_tmode;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    ଯଦି (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	ଯଦି (call_settmode)
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
	କର
	{
	    // if total is large enough, wait by portions in p_mzq
	    ଯଦି (total > p_mzq)
		msec = p_mzq;
	    ଅନ୍ୟଥା
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    ସଂରଚନା timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (ଶୂନ୍ୟ)nanosleep(&ts, ଖାଲି);
	}
#ଅନ୍ୟଥା
# ifdef HAVE_USLEEP
	ଯେତେବେଳେ (msec >= 1000)
	{
	    usleep((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(msec * 1000));
# ଅନ୍ୟଥା
#  ifndef HAVE_SELECT
	poll(ଖାଲି, 0, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec);
#  ଅନ୍ୟଥା
	{
	    ସଂରଚନା timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ଖାଲି, ଖାଲି, ଖାଲି, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	ଯେତେବେଳେ (total > 0);
#endif

	ଯଦି (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    ଅନ୍ୟଥା
	WaitForChar(msec, ଖାଲି, FALSE);
}

#ଯଦି defined(HAVE_ଗଦା_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ଗଦା_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଗଦା_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
check_ଗଦା_growth(ଅକ୍ଷର *p)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    ଗଦା_grows_downwards = (p > (ଅକ୍ଷର *)&i);
}
#endif

#ଯଦି defined(HAVE_ଗଦା_LIMIT)
ସ୍ଥିର ଅକ୍ଷର *ଗଦା_limit = ଖାଲି;

# ଯଦି defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ସ୍ଥିର ଶୂନ୍ୟ
get_ଗଦା_limit(ଶୂନ୍ୟ)
{
    ସଂରଚନା rlimit	rlp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    ଯଦି (getrlimit(RLIMIT_ଗଦା, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (ଆକାର(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# ଯଦି defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    ଯଦି (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		ଯଦି (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	ଯଦି (ଗଦା_grows_downwards)
	{
	    ଗଦା_limit = (ଅକ୍ଷର *)((long)&i - (lim / 16L * 15L));
	    ଯଦି (ଗଦା_limit >= (ଅକ୍ଷର *)&i)
		// overflow, set to 1/16 of current stack position
		ଗଦା_limit = (ଅକ୍ଷର *)((long)&i / 16L);
	}
	ଅନ୍ୟଥା
	{
	    ଗଦା_limit = (ଅକ୍ଷର *)((long)&i + (lim / 16L * 15L));
	    ଯଦି (ଗଦା_limit <= (ଅକ୍ଷର *)&i)
		ଗଦା_limit = ଖାଲି;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_stackcheck(ଅକ୍ଷର *p)
{
    ଯଦି (ଗଦା_limit == ଖାଲି)
	ଫେରନ୍ତୁ OK;

    ଯଦି (ଗଦା_grows_downwards)
    {
	ଯଦି (p < ଗଦା_limit)
	    ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (p > ଗଦା_limit)
	ଫେରନ୍ତୁ FAIL;
    ଫେରନ୍ତୁ OK;
}
#endif

#ଯଦି defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ସ୍ଥିର stack_t sigstk;			// for sigaltstack()
# ଅନ୍ୟଥା
ସ୍ଥିର ସଂରଚନା sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ସ୍ଥିର long ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_signal_ଗଦା_size(ଶୂନ୍ୟ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long ପୂର୍ଣ୍ଣସଂଖ୍ୟା size = -1;

    // return size only if sysconf doesn't return an error
    ଯଦି ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	ଫେରନ୍ତୁ size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    ଫେରନ୍ତୁ SIGSTKSZ;
# endif

    // otherwise guess the size
    ଫେରନ୍ତୁ 8000;
}

ସ୍ଥିର ଅକ୍ଷର *signal_ଗଦା;

    ସ୍ଥିର ଶୂନ୍ୟ
init_signal_ଗଦା(ଶୂନ୍ୟ)
{
    ଯଦି (signal_ଗଦା == ଖାଲି)
	ଫେରନ୍ତୁ;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_ଗଦା;
#  ଅନ୍ୟଥା
    sigstk.ss_sp = signal_ଗଦା;
#  endif
    sigstk.ss_size = get_signal_ଗଦା_size();
    sigstk.ss_flags = 0;
    (ଶୂନ୍ୟ)sigaltstack(&sigstk, ଖାଲି);
# ଅନ୍ୟଥା
    sigstk.ss_sp = signal_ଗଦା;
    ଯଦି (ଗଦା_grows_downwards)
	sigstk.ss_sp += get_signal_ଗଦା_size() - 1;
    sigstk.ss_onstack = 0;
    (ଶୂନ୍ୟ)sigstack(&sigstk, ଖାଲି);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#ଯଦି defined(SIGWINCH)
    ସ୍ଥିର ଶୂନ୍ୟ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#ଯଦି defined(SIGTSTP)
    ସ୍ଥିର ଶୂନ୍ୟ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    ଯଦି (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ଉଠାଅ(sigarg);
    }
    ଅନ୍ୟଥା
	got_tstp = TRUE;

# ଯଦି !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#ଯଦି defined(SIGINT)
    ସ୍ଥିର ଶୂନ୍ୟ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#ଯଦି defined(SIGUSR1)
    ସ୍ଥିର ଶୂନ୍ୟ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#ଯଦି defined(SIGPWR)
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#ଯଦି defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ସ୍ଥିର JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t lc_active = FALSE;

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
    ସ୍ଥିର ଶୂନ୍ୟ
mch_startjmp(ଶୂନ୍ୟ)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
mch_endjmp(ଶୂନ୍ୟ)
{
    lc_active = FALSE;
}

    ସ୍ଥିର ଶୂନ୍ୟ
mch_didjmp(ଶୂନ୍ୟ)
{
# ଯଦି defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ଗଦା();
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
    ସ୍ଥିର ଶୂନ୍ୟ
deathtrap SIGDEFARG(sigarg)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
#endif

#ଯଦି defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    ଯଦି (lc_active)
    {
# ଯଦି defined(SIGHASARG)
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
    ଯଦି (in_mch_delay && sigarg == SIGQUIT)
	ଫେରନ୍ତୁ;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    ଯଦି (entered == 0
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
	ଫେରନ୍ତୁ;
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

#ifdef HAVE_ଗଦା_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ଗଦା_limit();
#endif

#ଯଦି 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	ଯଦି (fp)
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
    ପାଇଁ (i = 0; signal_info[i].sig != -1; i++)
	ଯଦି (sigarg == signal_info[i].sig)
	    ଭାଙ୍ଗ;
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
    ଯଦି (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	ଯଦି (entered >= 4)
	    _exit(8);
	exit(7);
    }
    ଯଦି (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((ଅକ୍ଷର *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#ଅନ୍ୟଥା
    sprintf((ଅକ୍ଷର *)IObuff, "Vim: Caught deadly signal\r\n");
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
    ସ୍ଥିର ଶୂନ୍ୟ
after_sigcont(ଶୂନ୍ୟ)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#ଯଦି defined(SIGCONT)
ସ୍ଥିର ଶୂନ୍ୟ sigcont_handler SIGPROTOARG;

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
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t sigcont_received;
ସ୍ଥିର ଶୂନ୍ୟ sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sigcont_handler SIGDEFARG(sigarg)
{
    ଯଦି (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    ଅନ୍ୟଥା
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

#ଯଦି defined(FEAT_CLIPBOARD)
# ଯଦି defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ସ୍ଥିର ଶୂନ୍ୟ *clip_star_save = ଖାଲି;
ସ୍ଥିର ଶୂନ୍ୟ *clip_plus_save = ଖାଲି;
# endif

# ଯଦି defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
loose_clipboard(ଶୂନ୍ୟ)
{
    ଯଦି (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	ଯଦି (clip_star.owned)
	    clip_lose_selection(&clip_star);
	ଯଦି (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	ଯଦି (x11_display != ଖାଲି)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# ଯଦି defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
save_clipboard(ଶୂନ୍ୟ)
{
    ଯଦି (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    ଯଦି (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
restore_clipboard(ଶୂନ୍ୟ)
{
    ଯଦି (clip_star_save != ଖାଲି)
    {
	ଯଦି (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	ଅନ୍ୟଥା
	    free_register(clip_star_save);
	clip_star_save = ଖାଲି;
    }
    ଯଦି (clip_plus_save != ଖାଲି)
    {
	ଯଦି (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	ଅନ୍ୟଥା
	    free_register(clip_plus_save);
	clip_plus_save = ଖାଲି;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    ଶୂନ୍ୟ
mch_suspend(ଶୂନ୍ୟ)
{
    ଯଦି (ignore_sigtstp)
	ଫେରନ୍ତୁ;

#ଯଦି defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# ଯଦି defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# ଯଦି defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# ଯଦି defined(SIGCONT)
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

	ପାଇଁ (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#ଅନ୍ୟଥା
    suspend_shell();
#endif
}

    ଶୂନ୍ୟ
mch_init(ଶୂନ୍ୟ)
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

    ଶୂନ୍ୟ
set_sigwinch_handler(ଶୂନ୍ୟ)
{
#ଯଦି defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_signals(ଶୂନ୍ୟ)
{
#ଯଦି defined(SIGWINCH)
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
#ଯଦି defined(SIGCONT)
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

#ଯଦି defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    ଯଦି (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#ଯଦି defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
catch_int_signal(ଶୂନ୍ୟ)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    ଶୂନ୍ୟ
reset_signals(ଶୂନ୍ୟ)
{
    catch_signals(SIG_DFL, SIG_DFL);
#ଯଦି defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
catch_signals(
    ଶୂନ୍ୟ (*func_deadly)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା),
    ଶୂନ୍ୟ (*func_other)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା))
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; signal_info[i].sig != -1; i++)
    {
	ଯଦି (signal_info[i].deadly)
	{
#ଯଦି defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ସଂରଚନା sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# ଯଦି defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# ଅନ୍ୟଥା
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ଖାଲି);
#ଅନ୍ୟଥା
# ଯଦି defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ସଂରଚନା sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ଖାଲି);
# ଅନ୍ୟଥା
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	ଅନ୍ୟଥା ଯଦି (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#ଅନ୍ୟଥା
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    ସ୍ଥିର ଶୂନ୍ୟ
block_signals(sigset_t *set)
{
    sigset_t	newset;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    sigemptyset(&newset);

    ପାଇଁ (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# ଯଦି defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    ସ୍ଥିର ଶୂନ୍ୟ
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, ଖାଲି);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_handle_signal(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା got_signal = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା blocked = TRUE;

    ବାଛ (sig)
    {
	ସ୍ଥିତି SIGNAL_BLOCK:   blocked = TRUE;
			     ଭାଙ୍ଗ;

	ସ୍ଥିତି SIGNAL_UNBLOCK: blocked = FALSE;
			     ଯଦି (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ଭାଙ୍ଗ;

	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:	     ଯଦି (!blocked)
				 ଫେରନ୍ତୁ TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     ଯଦି (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc UNUSED, ଅକ୍ଷର **argv UNUSED)
{
    ଯଦି (isatty(1))
	ଫେରନ୍ତୁ OK;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_ନିବେଶ_isatty(ଶୂନ୍ୟ)
{
    ଯଦି (isatty(read_cmd_fd))
	ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#ifdef FEAT_X11

# ଯଦି defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#ଯଦି defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (ଅକ୍ଷର *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    ଯଦି (!gui.in_use)
# endif
	ସନ୍ଦେଶ((ଅକ୍ଷର *)IObuff);

    ଫେରନ୍ତୁ 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ଫେରନ୍ତୁ 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_connect_to_server(ଶୂନ୍ୟ)
{
    // No point in connecting if we are exiting or dying.
    ଯଦି (exiting || v_dying)
	ଫେରନ୍ତୁ FALSE;

# ଯଦି defined(FEAT_CLIENTSERVER)
    ଯଦି (x_force_connect)
	ଫେରନ୍ତୁ TRUE;
# endif
    ଯଦି (x_no_connect)
	ଫେରନ୍ତୁ FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    ଯଦି (clip_exclude_prog != ଖାଲି)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	ଯଦି (regprog_in_use(clip_exclude_prog))
	    ଫେରନ୍ତୁ FALSE;

	ଯଦି (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    ଫେରନ୍ତୁ FALSE;
    }
    ଫେରନ୍ତୁ TRUE;
}

# ଯଦି defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  ଯଦି defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   ଯଦି defined(VMS) || defined(__CYGWIN__)
    ଫେରନ୍ତୁ 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା xterm_dpy_retry_ଗଣନା = 0;

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = ଖାଲି;
    xterm_dpy_retry_ଗଣନା = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = ଖାଲି;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  ଯଦି defined(VMS) || defined(__CYGWIN__)
    ଫେରନ୍ତୁ 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ସ୍ଥିର ଶୂନ୍ୟ
may_restore_x11_clipboard(ଶୂନ୍ୟ)
{
    // No point in restoring the connecting if we are exiting or dying.
    ଯଦି (!exiting && !v_dying && xterm_dpy_retry_ଗଣନା > 0)
    {
	--xterm_dpy_retry_ଗଣନା;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	ଯଦି (app_context != (XtAppContext)ଖାଲି)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ଖାଲି;
	    x11_display = ଖାଲି; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    ଶୂନ୍ୟ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    ଯଦି (eap->arg != ଖାଲି && (arglen = STRLEN(eap->arg)) > 0)
    {
	ଯଦି (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (ଅକ୍ଷର *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ଖାଲି
		    ? (ଅକ୍ଷର *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_ଗଣନା = 5;  // Try reconnecting five times
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
test_x11_window(Display *dpy)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    ଯଦି (XGetWMName(dpy, x11_window, &text_prop))
	XFree((ଶୂନ୍ୟ *)text_prop.ମୂଲ୍ୟ);
    XSync(dpy, False);
    (ଶୂନ୍ୟ)XSetErrorHandler(old_handler);

    ଯଦି (p_verbose > 0 && got_x_error)
	verb_ସନ୍ଦେଶ(_("Testing the X display failed"));

    ଫେରନ୍ତୁ (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_x11_thing(ପୂର୍ଣ୍ଣସଂଖ୍ୟା get_title, ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_windis(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର	    *winid;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    x11_display_from = XD_NONE;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    did_set_error_handler = FALSE;

    ଯଦି (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(ଶୂନ୍ୟ)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# ଯଦି defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    ଯଦି (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	ଯଦି (x11_display_from == XD_HERE && x11_display != ଖାଲି)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	ଯଦି (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    ଫେରନ୍ତୁ OK;
	}
	x11_display = ଖାଲି;
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା ଯଦି (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = ଖାଲି;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    ଯଦି (!x_connect_to_server())
	ଫେରନ୍ତୁ FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    ଯଦି (x11_window == 0 && (winid = getenv("WINDOWID")) != ଖାଲି)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    ଯଦି (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    ଯଦି (xterm_dpy != ଖାଲି && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	ଯଦି (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    ଯଦି (x11_display_from == XD_HERE && x11_display != ଖାଲି)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	ଯଦି (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = ଖାଲି;
	    x11_display_from = XD_NONE;
	    ଫେରନ୍ତୁ FAIL;
	}
	ଫେରନ୍ତୁ OK;
    }
# endif

    ଯଦି (x11_window == 0 || x11_display == ଖାଲି)
	result = -1;

    ଯଦି (result != -1)	    // Have already been here and set this
	ଫେରନ୍ତୁ result;	    // Don't do all these X calls again

    ଯଦି (x11_window != 0 && x11_display == ଖାଲି)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	ଯଦି (p_verbose > 0)
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
	x11_display = XOpenDisplay(ଖାଲି);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	ଯଦି (p_verbose > 0 && sig_alarm_called)
	    verb_ସନ୍ଦେଶ(_("Opening the X display timed out"));
# endif
	ଯଦି (x11_display != ଖାଲି)
	{
# ifdef ELAPSED_FUNC
	    ଯଦି (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    ଯଦି (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = ଖାଲି;
	    }
	    ଅନ୍ୟଥା
		x11_display_from = XD_HERE;
	}
    }
    ଯଦି (x11_window == 0 || x11_display == ଖାଲି)
	ଫେରନ୍ତୁ (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    ଫେରନ୍ତୁ (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_only)
{
    ଫେରନ୍ତୁ get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_icon(ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_only)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    ଯଦି (oldicon == ଖାଲି && !test_only)
    {
	ଯଦି (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ଅନ୍ୟଥା
	    oldicon = vim_strsave(T_NAME);
    }

    ଫେରନ୍ତୁ retval;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_thing(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		get_title,	// get title string
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		test_only)
{
    XTextProperty	text_prop;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			retval = FALSE;
    Status		status;

    ଯଦି (get_x11_windis() != OK)
	ଫେରନ୍ତୁ FALSE;

    // Get window/icon name if any
    ଯଦି (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    ଅନ୍ୟଥା
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
    Window	    ମୂଳ;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned ପୂର୍ଣ୍ଣସଂଖ୍ୟା    ଅଙ୍କ_children;

    ଯେତେବେଳେ (!status || text_prop.ମୂଲ୍ୟ == ଖାଲି)
    {
	ଯଦି (!XQuery_ବୃକ୍ଷ(x11_display, win, &ମୂଳ, &parent, &children,
		    &ଅଙ୍କ_children))
	    ଭାଙ୍ଗ;
	ଯଦି (children)
	    XFree((ଶୂନ୍ୟ *)children);
	ଯଦି (parent == ମୂଳ || parent == 0)
	    ଭାଙ୍ଗ;

	win = parent;
	ଯଦି (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	ଅନ୍ୟଥା
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    ଯଦି (status && text_prop.ମୂଲ୍ୟ != ଖାଲି)
    {
	retval = TRUE;
	ଯଦି (!test_only)
	{
	    ଯଦି (get_title)
		vim_free(oldtitle);
	    ଅନ୍ୟଥା
		vim_free(oldicon);
	    ଯଦି (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		ଯଦି (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.ମୂଲ୍ୟ);
		ଅନ୍ୟଥା
		    oldicon = vim_strsave((char_u *)text_prop.ମୂଲ୍ୟ);
	    }
	    ଅନ୍ୟଥା
	    {
		ଅକ୍ଷର    **cl;
		Status  transform_status;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		ଯଦି (transform_status >= Success && n > 0 && cl[0])
		{
		    ଯଦି (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    ଅନ୍ୟଥା
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		ଅନ୍ୟଥା
		{
		    ଯଦି (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.ମୂଲ୍ୟ);
		    ଅନ୍ୟଥା
			oldicon = vim_strsave((char_u *)text_prop.ମୂଲ୍ୟ);
		}
	    }
	}
	XFree((ଶୂନ୍ୟ *)text_prop.ମୂଲ୍ୟ);
    }
    ଫେରନ୍ତୁ retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# ଯଦି defined(X_HAVE_UTF8_STRING)
#  ଯଦି X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    ଯଦି (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ଅଚଳ ଅକ୍ଷର *)title,
					     ଖାଲି, ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
    ଅନ୍ୟଥା
# endif
    {
# ଯଦି XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ଅଚଳ ଅକ୍ଷର *)title,
					     ଖାଲି, ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
#  ଅନ୍ୟଥା
	XTextProperty	text_prop;
	ଅକ୍ଷର		*c_title = (ଅକ୍ଷର *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(ଶୂନ୍ୟ)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     ଖାଲି, ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
#  endif
# ଅନ୍ୟଥା
	XStoreName(x11_display, x11_window, (ଅକ୍ଷର *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    ଯଦି (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)icon,
						   ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
    ଅନ୍ୟଥା
# endif
    {
# ଯଦି XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, ଖାଲି, (ଅଚଳ ଅକ୍ଷର *)icon,
						   ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
#  ଅନ୍ୟଥା
	XTextProperty	text_prop;
	ଅକ୍ଷର		*c_icon = (ଅକ୍ଷର *)icon;

	(ଶୂନ୍ୟ)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, ଖାଲି, &text_prop,
						   ଖାଲି, 0, ଖାଲି, ଖାଲି, ଖାଲି);
#  endif
# ଅନ୍ୟଥା
	XSetIconName(x11_display, x11_window, (ଅକ୍ଷର *)icon);
# endif
    }
    XFlush(x11_display);
}

#ଅନ୍ୟଥା  // FEAT_X11

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_only UNUSED)
{
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_x11_icon(ପୂର୍ଣ୍ଣସଂଖ୍ୟା test_only)
{
    ଯଦି (!test_only)
    {
	ଯଦି (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ଅନ୍ୟଥା
	    oldicon = vim_strsave(T_NAME);
    }
    ଫେରନ୍ତୁ FALSE;
}

#endif // FEAT_X11

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_title(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ get_x11_title(TRUE);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_icon(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    ଶୂନ୍ୟ
mch_settitle(char_u *title, char_u *icon)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		type = 0;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	recursive = 0;

    ଯଦି (T_NAME == ଖାଲି)	    // no terminal name (yet)
	ଫେରନ୍ତୁ;
    ଯଦି (title == ଖାଲି && icon == ଖାଲି)	    // nothing to do
	ଫେରନ୍ତୁ;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    ଯଦି (recursive)
	ଫେରନ୍ତୁ;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    ଯଦି (get_x11_windis() == OK)
	type = 1;
#endif
#ଯଦି defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    ଯଦି (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    ଯଦି ((type || *T_TS != NUL) && title != ଖାଲି)
    {
	ଯଦି (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	ଯଦି (oldtitle == ଖାଲି
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (ଶୂନ୍ୟ)get_x11_title(FALSE);

	ଯଦି (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	ଅନ୍ୟଥା
# ifdef FEAT_GUI_GTK
	ଯଦି (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#ଯଦି defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	ଅନ୍ୟଥା
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    ଯଦି ((type || *T_CIS != NUL) && icon != ଖାଲି)
    {
	ଯଦି (oldicon == ଖାଲି
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	ଯଦି (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	ଅନ୍ୟଥା
# ifdef FEAT_GUI_GTK
	ଯଦି (!gui.in_use)		// don't do this if GTK+ is running
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
    ଶୂନ୍ୟ
mch_restore_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା which)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	do_ଠେଲ_କାଢ଼ = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : ଖାଲି;
    char_u *tofree = ଖାଲି;
    ଯଦି (title == oldtitle && oldtitle != ଖାଲି)
    {
	tofree = vim_strsave(title);
	ଯଦି (tofree != ଖାଲି)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : ଖାଲି);
    vim_free(tofree);

    ଯଦି (do_ଠେଲ_କାଢ଼)
    {
	// pop and push from/to the stack
	term_କାଢ଼_title(which);
	term_ଠେଲ_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_xterm(char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#ଯଦି defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_xterm_like_mouse(char_u *name)
{
    ଫେରନ୍ତୁ (name != ଖାଲି
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
use_xterm_mouse(ଶୂନ୍ୟ)
{
    ଯଦି (ttym_flags == TTYM_SGR)
	ଫେରନ୍ତୁ 4;
    ଯଦି (ttym_flags == TTYM_URXVT)
	ଫେରନ୍ତୁ 3;
    ଯଦି (ttym_flags == TTYM_XTERM2)
	ଫେରନ୍ତୁ 2;
    ଯଦି (ttym_flags == TTYM_XTERM)
	ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_iris(char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#ଯଦି defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
vim_is_vt300(char_u *name)
{
    ଯଦି (name == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    ଫେରନ୍ତୁ ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ଖାଲି)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_user_name(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(ଖାଲି), len - 1);
    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_uname(uid_t uid, char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ଯଦି defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ସଂରଚନା passwd   *pw;

    ଯଦି ((pw = getpwuid(uid)) != ଖାଲି
	    && pw->pw_name != ଖାଲି && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	ଫେରନ୍ତୁ OK;
    }
#endif
    sprintf((ଅକ୍ଷର *)s, "%d", (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)uid);	    // assumes s is long enough
    ଫେରନ୍ତୁ FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    ଶୂନ୍ୟ
mch_get_host_name(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ସଂରଚନା utsname vutsname;

    ଯଦି (uname(&vutsname) < 0)
	*s = NUL;
    ଅନ୍ୟଥା
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#ଅନ୍ୟଥା // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    ଶୂନ୍ୟ
mch_get_host_name(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
# ifdef VAXC
    vaxc$gethostname((ଅକ୍ଷର *)s, len);
# ଅନ୍ୟଥା
    gethostname((ଅକ୍ଷର *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    ଯଦି (kill(pid, 0) == 0)
	ଫେରନ୍ତୁ TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    ଯଦି (errno == ESRCH)
	ଫେରନ୍ତୁ FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    ଫେରନ୍ତୁ TRUE;
}

#ଯଦି !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ସ୍ଥିର ଅକ୍ଷର *
strerror(ପୂର୍ଣ୍ଣସଂଖ୍ୟା err)
{
    ବାହ୍ୟ ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sys_nerr;
    ବାହ୍ୟ ଅକ୍ଷର	    *sys_errlist[];
    ସ୍ଥିର ଅକ୍ଷର	    er[20];

    ଯଦି (err > 0 && err < sys_nerr)
	ଫେରନ୍ତୁ (sys_errlist[err]);
    snprintf(er, ଆକାର(er), "Error %d", err);
    ଫେରନ୍ତୁ er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_dirname(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ଯଦି defined(USE_GETCWD)
    ଯଦି (getcwd((ଅକ୍ଷର *)buf, len) == ଖାଲି)
    {
	STRCPY(buf, strerror(errno));
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (getwd((ଅକ୍ଷର *)buf) != ଖାଲି ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)		// also expand when already absolute path
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		buflen = 0;
#ifdef HAVE_FCHDIR
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd = -1;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = OK;
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
# ଯଦି CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# ଅନ୍ୟଥା
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    ଯଦି ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == ଖାଲି || p != fname))
    {
	ଯଦି (p == ଖାଲି && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	ଯଦି (p != ଖାଲି)
	{
	    ଯଦି (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    ଯଦି (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		ଯଦି (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    ଯଦି (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((ଅକ୍ଷର *)olddir) != 0))
	    {
		p = ଖାଲି;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    ଅନ୍ୟଥା
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		ଯଦି (p - fname >= len)
		    retval = FAIL;
		ଅନ୍ୟଥା
		{
		    vim_strncpy(buf, fname, p - fname);
		    ଯଦି (mch_chdir((ଅକ୍ଷର *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			ଯଦି (mch_isFullName(fname))
			    retval = FAIL;
			ଅନ୍ୟଥା
			    p = ଖାଲି;
		    }
		    ଅନ୍ୟଥା ଯଦି (*p == '/')
			fname = p + 1;
		    ଅନ୍ୟଥା
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	ଯଦି (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	ଯଦି (p != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	l;

#ifdef HAVE_FCHDIR
	    ଯଦି (fd >= 0)
	    {
		ଯଦି (p_verbose >= 5)
		{
		    verbose_enter();
		    ସନ୍ଦେଶ("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    ଅନ୍ୟଥା
#endif
		l = mch_chdir((ଅକ୍ଷର *)olddir);
	    ଯଦି (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	ଯଦି (fd >= 0)
	    close(fd);
#endif

	buflen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf);
	ଯଦି (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	ଅନ୍ୟଥା ଯଦି (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += ଆକାର(PATHSEP);
	}
#endif
    }

    ଯଦି (buflen == 0)
	buflen = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buf);

    // Catch file names which are too long.
    ଯଦି (retval == FAIL || (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(buflen + STRLEN(fname)) >= len)
	ଫେରନ୍ତୁ FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    ଯଦି (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isFullName(char_u *fname)
{
#ifdef VMS
    ଫେରନ୍ତୁ ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((ଅକ୍ଷର *)fname,':') || strchr((ଅକ୍ଷର *)fname,'"') ||
	    (strchr((ଅକ୍ଷର *)fname,'[') && strchr((ଅକ୍ଷର *)fname,']'))||
	    (strchr((ଅକ୍ଷର *)fname,'<') && strchr((ଅକ୍ଷର *)fname,'>'))   );
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (*fname == '/' || *fname == '~');
#endif
}

#ଯଦି defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    ଶୂନ୍ୟ
fname_case(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len UNUSED)  // buffer size, only used when name gets longer
{
    ସଂରଚନା stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    ସଂରଚନା dirent *dp;

    ଯଦି (mch_lstat((ଅକ୍ଷର *)name, &st) < 0)
	ଫେରନ୍ତୁ;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    ଯଦି (slash == ଖାଲି)
    {
	dirp = opendir(".");
	tail = name;
    }
    ଅନ୍ୟଥା
    {
	*slash = NUL;
	dirp = opendir((ଅକ୍ଷର *)name);
	*slash = '/';
	tail = slash + 1;
    }

    ଯଦି (dirp == ଖାଲି)
	ଫେରନ୍ତୁ;

    taillen = STRLEN(tail);
    ଯେତେବେଳେ ((dp = readdir(dirp)) != ଖାଲି)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	ଯଦି (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ସଂରଚନା stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    ଯଦି (mch_lstat((ଅକ୍ଷର *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		ଭାଙ୍ଗ;
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
    ସଂରଚନା stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    ଯଦି (stat((ଅକ୍ଷର *)vms_fixfilename(name), &statb))
#ଅନ୍ୟଥା
    ଯଦି (stat((ଅକ୍ଷର *)name, &statb))
#endif
	ଫେରନ୍ତୁ -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    ଫେରନ୍ତୁ statb.st_mode & ~S_ADDACE;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setperm(char_u *name, long perm)
{
    ଫେରନ୍ତୁ (chmod((ଅକ୍ଷର *)
#ifdef VMS
		    vms_fixfilename(name),
#ଅନ୍ୟଥା
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#ଯଦି defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_fsetperm(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, long perm)
{
    ଫେରନ୍ତୁ (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#ଯଦି defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
ପ୍ରକାରବ୍ୟାଖ୍ୟା ସଂରଚନା vim_acl_solaris_T {
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# ଯଦି defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ଶୂନ୍ୟ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ଯଦି (from_file == ଖାଲି)
	ଫେରନ୍ତୁ;

    ଯଦି (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    ଯଦି (selinux_enabled <= 0)
	ଫେରନ୍ତୁ;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    ଅକ୍ଷର *from_context = ଖାଲି;
    ଅକ୍ଷର *to_context = ଖାଲି;

    ଯଦି (getfilecon((ଅକ୍ଷର *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	ଯଦି (errno == EOPNOTSUPP)
	    ଫେରନ୍ତୁ;

	ସନ୍ଦେଶ_puts(_("\nCould not get security context for "));
	ସନ୍ଦେଶ_outtrans(from_file);
	ସନ୍ଦେଶ_putchar('\n');
	ଫେରନ୍ତୁ;
    }
    ଯଦି (getfilecon((ଅକ୍ଷର *)to_file, &to_context) < 0)
    {
	ସନ୍ଦେଶ_puts(_("\nCould not get security context for "));
	ସନ୍ଦେଶ_outtrans(to_file);
	ସନ୍ଦେଶ_putchar('\n');
	freecon (from_context);
	ଫେରନ୍ତୁ ;
    }
    ଯଦି (strcmp(from_context, to_context) != 0)
    {
	ଯଦି (setfilecon((ଅକ୍ଷର *)to_file, from_context) < 0)
	{
	    ସନ୍ଦେଶ_puts(_("\nCould not set security context for "));
	    ସନ୍ଦେଶ_outtrans(to_file);
	    ସନ୍ଦେଶ_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# ଯଦି defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ଶୂନ୍ୟ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ସ୍ଥିର ଅଚଳ ଅକ୍ଷର * ଅଚଳ smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    ଅକ୍ଷର	buffer[SMACK_LABEL_LEN];
    ଅଚଳ ଅକ୍ଷର	*name;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		index;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ssize_t	size;

    ଯଦି (from_file == ଖାଲି)
	ଫେରନ୍ତୁ;

    size = listxattr((ଅକ୍ଷର *)from_file, ଖାଲି, 0);
    // not supported or no attributes to copy
    ଯଦି (size <= 0)
	ଫେରନ୍ତୁ;

    ପାଇଁ (index = 0 ; index < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((ଅକ୍ଷର*)from_file, name, buffer, ଆକାର(buffer));
	ଯଦି (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((ଅକ୍ଷର*)to_file, name, buffer, (size_t)size, 0);
	    ଯଦି (ret < 0)
	    {
		vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		ସନ୍ଦେଶ_outtrans(IObuff);
		ସନ୍ଦେଶ_putchar('\n');
	    }
	}
	ଅନ୍ୟଥା
	{
	    // what reason of not having the attribute value?
	    ବାଛ (errno)
	    {
		ସ୍ଥିତି ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    ଫେରନ୍ତୁ; // leave because it isn't useful to continue

		ସ୍ଥିତି ERANGE:
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    // no enough size OR unexpected error
		     vim_snprintf((ଅକ୍ଷର *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)IObuff);
		    ସନ୍ଦେଶ_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		ସ୍ଥିତି ENODATA:
		    // no attribute of this name
		    ret = removexattr((ଅକ୍ଷର*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ଭାଙ୍ଗ;
	    }
	}
    }
}
# endif // HAVE_SMACK

# ଯଦି defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    ଶୂନ୍ୟ
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    ଅକ୍ଷର	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    ଅକ୍ଷର	*ଚାବି;
    ଅକ୍ଷର	*val = ଖାଲି;
    ଅକ୍ଷର	*errmsg = ଖାଲି;

    ଯଦି (from_file == ଖାଲି)
	ଫେରନ୍ତୁ;

    // get the length of the extended attributes
    size = listxattr((ଅକ୍ଷର *)from_file, ଖାଲି, 0);
    // not supported or no attributes to copy
    ଯଦି (size <= 0)
	ଫେରନ୍ତୁ;
    xattr_buf = (ଅକ୍ଷର*)alloc(size);
    ଯଦି (xattr_buf == ଖାଲି)
	ଫେରନ୍ତୁ;
    size = listxattr((ଅକ୍ଷର *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    ପାଇଁ (ପୂର୍ଣ୍ଣସଂଖ୍ୟା round = 0; round < 2; round++)
    {

	ଚାବି = xattr_buf;
	ଯଦି (round == 1)
	    size = tsize;

	ଯେତେବେଳେ (size > 0)
	{
	    vallen = getxattr((ଅକ୍ଷର *)from_file, ଚାବି,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    ଯଦି (vallen >= 0 && round &&
		setxattr((ଅକ୍ଷର *)to_file, ଚାବି, val, vallen, 0) == 0)
		;
	    ଅନ୍ୟଥା ଯଦି (errno)
	    {
		ବାଛ (errno)
		{
		    ସ୍ଥିତି E2BIG:
			errmsg = e_xattr_e2big;
			ଯାଅ error_exit;
		    ସ୍ଥିତି ENOTSUP:
		    ସ୍ଥିତି EACCES:
		    ସ୍ଥିତି EPERM:
			ଭାଙ୍ଗ;
		    ସ୍ଥିତି ERANGE:
			errmsg = e_xattr_erange;
			ଯାଅ error_exit;
		    ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
			errmsg = e_xattr_other;
			ଯାଅ error_exit;
		}
	    }

	    ଯଦି (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ଚାବି) + 1;
	    size -= keylen;
	    ଚାବି += keylen;
	}
	ଯଦି (round)
	    ଭାଙ୍ଗ;

	val = (ଅକ୍ଷର*)alloc(max_vallen + 1);
	ଯଦି (val == ଖାଲି)
	    ଯାଅ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    ଯଦି (errmsg != ଖାଲି)
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
    vim_acl_T	ret = ଖାଲି;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((ଅକ୍ଷର *)fname, ACL_TYPE_ACCESS);
# ଅନ୍ୟଥା
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    ଯଦି (acl_get((ଅକ୍ଷର *)fname, 0, &aclent) < 0)
	ଫେରନ୍ତୁ ଖାଲି;
    ret = (vim_acl_T)aclent;
#  ଅନ୍ୟଥା
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(ଆକାର(vim_acl_solaris_T));
    ଯଦି (aclent == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି ((aclent->acl_cnt = acl((ଅକ୍ଷର *)fname, GETACLCNT, 0, ଖାଲି)) < 0)
    {
	ମୁକ୍ତ(aclent);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * ଆକାର(aclent_t));
    ଯଦି (aclent->acl_entry == ଖାଲି)
    {
	ମୁକ୍ତ(aclent);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ଯଦି (acl((ଅକ୍ଷର *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	ମୁକ୍ତ(aclent->acl_entry);
	ମୁକ୍ତ(aclent);
	ଫେରନ୍ତୁ ଖାଲି;
    }
    ret = (vim_acl_T)aclent;
#   ଅନ୍ୟଥା
#    ଯଦି defined(HAVE_AIX_ACL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		aclsize;
    ସଂରଚନା acl *aclent;

    aclsize = ଆକାର(ସଂରଚନା acl);
    aclent = malloc(aclsize);
    ଯଦି (aclent == ଖାଲି)
	ଫେରନ୍ତୁ ଖାଲି;
    ଯଦି (statacl((ଅକ୍ଷର *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	ଯଦି (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    ଯଦି (aclent == ଖାଲି || statacl((ଅକ୍ଷର *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		ମୁକ୍ତ(aclent);
		ଫେରନ୍ତୁ ଖାଲି;
	    }
	}
	ଅନ୍ୟଥା
	{
	    ମୁକ୍ତ(aclent);
	    ଫେରନ୍ତୁ ଖାଲି;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    ଫେରନ୍ତୁ ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    ଶୂନ୍ୟ
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    ଯଦି (aclent == ଖାଲି)
	ଫେରନ୍ତୁ;
# ifdef HAVE_POSIX_ACL
    acl_set_file((ଅକ୍ଷର *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# ଅନ୍ୟଥା
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((ଅକ୍ଷର *)fname, (acl_t *)aclent);
#  ଅନ୍ୟଥା
#   ifdef HAVE_SOLARIS_ACL
    acl((ଅକ୍ଷର *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   ଅନ୍ୟଥା
#    ifdef HAVE_AIX_ACL
    chacl((ଅକ୍ଷର *)fname, aclent, ((ସଂରଚନା acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    ଶୂନ୍ୟ
mch_free_acl(vim_acl_T aclent)
{
    ଯଦି (aclent == ଖାଲି)
	ଫେରନ୍ତୁ;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# ଅନ୍ୟଥା
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  ଅନ୍ୟଥା
#   ifdef HAVE_SOLARIS_ACL
    ମୁକ୍ତ(((vim_acl_solaris_T *)aclent)->acl_entry);
    ମୁକ୍ତ(aclent);
#   ଅନ୍ୟଥା
#    ifdef HAVE_AIX_ACL
    ମୁକ୍ତ(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    ଶୂନ୍ୟ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isdir(char_u *name)
{
    ସଂରଚନା stat statb;

    ଯଦି (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (stat((ଅକ୍ଷର *)name, &statb))
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isrealdir(char_u *name)
{
    ସଂରଚନା stat statb;

    ଯଦି (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ଫେରନ୍ତୁ FALSE;
    ଯଦି (mch_lstat((ଅକ୍ଷର *)name, &statb))
	ଫେରନ୍ତୁ FALSE;
    ଫେରନ୍ତୁ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
executable_file(char_u *name)
{
    ସଂରଚନା stat	st;

    ଯଦି (stat((ଅକ୍ଷର *)name, &st))
	ଫେରନ୍ତୁ 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା vms_executable = 0;
    ଯଦି (S_ISREG(st.st_mode) && mch_access((ଅକ୍ଷର *)name, X_OK) == 0)
    {
	ଯଦି (strstr(vms_tolower((ଅକ୍ଷର*)name),".exe") != ଖାଲି
		|| strstr(vms_tolower((ଅକ୍ଷର*)name),".com")!= ଖାଲି)
	    vms_executable = 1;
    }
    ଫେରନ୍ତୁ vms_executable;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ S_ISREG(st.st_mode) && mch_access((ଅକ୍ଷର *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_exe(char_u *name, char_u **path, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    ଯଦି (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	ଯଦି ((use_path || gettail(name) != name) && executable_file(name))
	{
	    ଯଦି (path != ଖାଲି)
	    {
		ଯଦି (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		ଅନ୍ୟଥା
		    *path = vim_strsave(name);
	    }
	    ଫେରନ୍ତୁ TRUE;
	}
	ଫେରନ୍ତୁ FALSE;
    }

    p = (char_u *)getenv("PATH");
    ଯଦି (p == ଖାଲି || *p == NUL)
	ଫେରନ୍ତୁ -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    ଯଦି (buf == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ପାଇଁ (;;)
    {
	e = (char_u *)strchr((ଅକ୍ଷର *)p, ':');
	ଯଦି (e == ଖାଲି)
	    e = p_end;
	elen = (size_t)(e - p);
	ଯଦି (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((ଅକ୍ଷର *)buf, bufsize, "%.*s%s%s",
		(ପୂର୍ଣ୍ଣସଂଖ୍ୟା)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	ଯଦି (retval == 1)
	{
	    ଯଦି (path != ଖାଲି)
	    {
		ଯଦି (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		ଅନ୍ୟଥା
		    *path = vim_strnsave(buf, buflen);
	    }
	    ଭାଙ୍ଗ;
	}

	ଯଦି (*e != ':')
	    ଭାଙ୍ଗ;
	p = e + 1;
    }

    vim_free(buf);
    ଫେରନ୍ତୁ retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_nodetype(char_u *name)
{
    ସଂରଚନା stat	st;

    ଯଦି (stat((ଅକ୍ଷର *)name, &st))
	ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;
    ଯଦି (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;
    ଯଦି (S_ISBLK(st.st_mode))	// block device isn't writable
	ଫେରନ୍ତୁ ଗଣ୍ଠି_OTHER;
    // Everything else is writable?
    ଫେରନ୍ତୁ ଗଣ୍ଠି_WRITABLE;
}

    ଶୂନ୍ୟ
mch_early_init(ଶୂନ୍ୟ)
{
#ifdef HAVE_CHECK_ଗଦା_GROWTH
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			i;

    check_ଗଦା_growth((ଅକ୍ଷର *)&i);

# ifdef HAVE_ଗଦା_LIMIT
    get_ଗଦା_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#ଯଦି defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ଗଦା = alloc(get_signal_ଗଦା_size());
    init_signal_ଗଦା();
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

#ଯଦି defined(EXITFREE)
    ଶୂନ୍ୟ
mch_free_mem(ଶୂନ୍ୟ)
{
# ଯଦି defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    ଯଦି (clip_star.owned)
	clip_lose_selection(&clip_star);
    ଯଦି (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# ଯଦି defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ଯଦି (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    ଯଦି (xterm_dpy != ଖାଲି)
	XtCloseDisplay(xterm_dpy);
    ଯଦି (app_context != (XtAppContext)ଖାଲି)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ଖାଲି; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# ଯଦି defined(FEAT_X11)
    ଯଦି (x11_display != ଖାଲି
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# ଯଦି defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ଗଦା);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
exit_scroll(ଶୂନ୍ୟ)
{
    ଯଦି (silent_mode)
	ଫେରନ୍ତୁ;
    ଯଦି (newline_on_exit || ସନ୍ଦେଶ_didout)
    {
	ଯଦି (ସନ୍ଦେଶ_use_printf())
	{
	    ଯଦି (info_message)
		mch_ସନ୍ଦେଶ("\n");
	    ଅନ୍ୟଥା
		mch_errmsg("\r\n");
	}
	ଅନ୍ୟଥା
	    out_char('\n');
    }
    ଅନ୍ୟଥା ଯଦି (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	ସନ୍ଦେଶ_clr_eos_force();		// clear the rest of the display
	windgoto((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# ଯଦି (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
ବାହ୍ୟ ଶୂନ୍ୟ __gcov_dump(ଶୂନ୍ୟ);
ବାହ୍ୟ ଶୂନ୍ୟ __gcov_reset(ଶୂନ୍ୟ);
#  define __gcov_flush() କର { __gcov_dump(); __gcov_reset(); } ଯେତେବେଳେ (0)
# ଅନ୍ୟଥା
ବାହ୍ୟ ଶୂନ୍ୟ __gcov_flush(ଶୂନ୍ୟ);
# endif
#endif

    ଶୂନ୍ୟ
mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r)
{
    exiting = TRUE;

#ଯଦି defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    ଯଦି (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	ଯଦି (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_କାଢ଼_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	ଯଦି (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	ଯଦି (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	ଯଦି (full_screen)
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
    ଯଦି (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    ଯଦି (deadly_signal != 0)
	ଫେରନ୍ତୁ;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    ସ୍ଥିର ଶୂନ୍ୟ
may_core_dump(ଶୂନ୍ୟ)
{
    ଯଦି (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tty_fd(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tty_fd = fd;

# ଯଦି defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    ଯଦି (mch_isatty(fd) > 1)
    {
	ଅକ୍ଷର *name;

	name = ptsname(fd);
	ଯଦି (name == ଖାଲି)
	    ଫେରନ୍ତୁ -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	ଯଦି (tty_fd < 0)
	    ଫେରନ୍ତୁ -1;
    }
# endif
    ଫେରନ୍ତୁ tty_fd;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_tcgetattr(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ଶୂନ୍ୟ *term)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tty_fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;

    tty_fd = get_tty_fd(fd);
    ଯଦି (tty_fd < 0)
	ଫେରନ୍ତୁ -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ସଂରଚନା termios *)term);
#  ଅନ୍ୟଥା
    retval = ioctl(tty_fd, TCGETA, (ସଂରଚନା termio *)term);
#  endif
# ଅନ୍ୟଥା
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ସଂରଚନା sgttyb *)term);
# endif
    ଯଦି (tty_fd != fd)
	close(tty_fd);
    ଫେରନ୍ତୁ retval;
}

    ଶୂନ୍ୟ
mch_settmode(tmode_T tmode)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ସ୍ଥିର ସଂରଚନା termios told;
	   ସଂରଚନା termios tnew;
#  ଅନ୍ୟଥା
    ସ୍ଥିର ସଂରଚନା termio told;
	   ସଂରଚନା termio tnew;
#  endif

    ଯଦି (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    ଯଦି (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ଖାଲି || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  ଯଦି defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   ଅନ୍ୟଥା
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    ଅନ୍ୟଥା
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    ଅନ୍ୟଥା ଯଦି (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  ଯଦି defined(HAVE_TERMIOS_H)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	ଯେତେବେଳେ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  ଅନ୍ୟଥା
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# ଅନ୍ୟଥା
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ସ୍ଥିର ସଂରଚନା sgttyb ttybold;
	   ସଂରଚନା sgttyb ttybnew;

    ଯଦି (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    ଯଦି (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    ଅନ୍ୟଥା ଯଦି (tmode == TMODE_SLEEP)
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
    ଶୂନ୍ୟ
get_stty(ଶୂନ୍ୟ)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    ଯଦି (get_tty_info(read_cmd_fd, &info) != OK)
	ଫେରନ୍ତୁ;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    ଯଦି (p != ଖାଲି && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ଖାଲି);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
get_tty_info(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ସଂରଚନା termios ଚାବିଗୁଡ଼ିକ;
#  ଅନ୍ୟଥା
    ସଂରଚନା termio ଚାବିଗୁଡ଼ିକ;
#  endif

    ଯଦି (mch_tcgetattr(fd, &ଚାବିଗୁଡ଼ିକ) != -1)
    {
	info->backspace = ଚାବିଗୁଡ଼ିକ.c_cc[VERASE];
	info->interrupt = ଚାବିଗୁଡ଼ିକ.c_cc[VINTR];
	ଯଦି (ଚାବିଗୁଡ଼ିକ.c_iflag & ICRNL)
	    info->enter = NL;
	ଅନ୍ୟଥା
	    info->enter = CAR;
	ଯଦି (ଚାବିଗୁଡ଼ିକ.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	ଅନ୍ୟଥା
	    info->nl_does_cr = FALSE;
	ଫେରନ୍ତୁ OK;
    }
# ଅନ୍ୟଥା
    // for "old" tty systems
    ସଂରଚନା sgttyb ଚାବିଗୁଡ଼ିକ;

    ଯଦି (mch_tcgetattr(fd, &ଚାବିଗୁଡ଼ିକ) != -1)
    {
	info->backspace = ଚାବିଗୁଡ଼ିକ.sg_erase;
	info->interrupt = ଚାବିଗୁଡ଼ିକ.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	ଫେରନ୍ତୁ OK;
    }
# endif
    ଫେରନ୍ତୁ FAIL;
}

#endif // VMS

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    ଶୂନ୍ୟ
mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on)
{
#ifdef FEAT_BEVAL_TERM
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	bevalterm_ison = FALSE;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xterm_mouse_vers;

#ଯଦି defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ଯଦି (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    ଯଦି (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	ଫେରନ୍ତୁ;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    ଯଦି (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    ଯଦି (T_CXM != ଖାଲି && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    ଅନ୍ୟଥା ଯଦି (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    ଯଦି (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	ଯଦି (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    ଯଦି (xterm_mouse_vers > 0)
    {
	ଯଦି (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	ଅନ୍ୟଥା	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    ଅନ୍ୟଥା ଯଦି (ttym_flags == TTYM_DEC)
    {
	ଯଦି (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	ଅନ୍ୟଥା	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    ଅନ୍ୟଥା
    {
	ଯଦି (on)
	{
	    ଯଦି (gpm_open())
		mouse_ison = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    ଅନ୍ୟଥା
    {
	ଯଦି (on)
	{
	    ଯଦି (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    ଅନ୍ୟଥା
    {
	ଯଦି (on)
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
# ଅନ୍ୟଥା
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	ଅନ୍ୟଥା
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    ଅନ୍ୟଥା
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	ଯଦି (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	ଅନ୍ୟଥା
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#ଯଦି defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ଶୂନ୍ୟ
mch_bevalterm_changed(ଶୂନ୍ୟ)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    ଶୂନ୍ୟ
check_mouse_termcode(ଶୂନ୍ୟ)
{
#ifdef FEAT_MOUSE_XTERM
    ଯଦି (use_xterm_mouse()
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
	ଯଦି (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    ଯଦି (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    ଯଦି (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	ଯଦି (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ଅନ୍ୟଥା
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    ଯଦି (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	ଯଦି (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ଅନ୍ୟଥା
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
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
	ସଂରଚନା winsize	ws;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ଯଦି (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ଯଦି (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(ଖାଲି, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# ଅନ୍ୟଥା // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ସଂରଚନା ttysize	ts;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ଯଦି (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ଯଦି (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_ଧାଡ଼ିଗୁଡ଼ିକ;
#   ifdef FEAT_EVAL
	    ch_log(ଖାଲି, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    ଯଦି (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ଖାଲି)
    {
	ଯଦି ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((ଅକ୍ଷର *)p);
# ifdef FEAT_EVAL
	    ch_log(ଖାଲି, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	ଯଦି ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((ଅକ୍ଷର *)p);
# ifdef FEAT_EVAL
	    ch_log(ଖାଲି, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    ଯଦି (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(ଖାଲି, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    ଯଦି (columns <= 0 || rows <= 0)
	ଫେରନ୍ତୁ FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    ଫେରନ୍ତୁ OK;
}

# ଯଦି defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    ଶୂନ୍ୟ
mch_calc_cell_size(ସଂରଚନା cellsize *cs_out)
{
   ସଂରଚନା winsize   ws;
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    retval;
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    x_cell_size;
   ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ଖାଲି, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   ଯଦି (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	ଫେରନ୍ତୁ;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   ଯଦି (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# ଯଦି defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_report_winsize(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା rows, ପୂର୍ଣ୍ଣସଂଖ୍ୟା cols)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tty_fd;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;

    tty_fd = get_tty_fd(fd);
    ଯଦି (tty_fd < 0)
	ଫେରନ୍ତୁ FAIL;

#  ଯଦି defined(TIOCSWINSZ)
    ସଂରଚନା winsize ws;

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
    ch_log(ଖାଲି, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ସଂରଚନା ttysize ts;

    ts.ts_cols = cols;
    ts.ts_ଧାଡ଼ିଗୁଡ଼ିକ = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ଖାଲି, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    ଯଦି (tty_fd != fd)
	close(tty_fd);
    ଫେରନ୍ତୁ retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    ଶୂନ୍ୟ
mch_set_shellsize(ଶୂନ୍ୟ)
{
    ଯଦି (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Rows, (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    ଶୂନ୍ୟ
mch_new_shellsize(ଶୂନ୍ୟ)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ସ୍ଥିର pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    ଯେତେବେଳେ (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	ଯଦି (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    ଯଦି (++delay_msec > 10)
		delay_msec = 10;
	    ଜାରି;
	}
	ଯଦି (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    ଭାଙ୍ଗ;
    }
    ଫେରନ୍ତୁ wait_pid;
}

#ଯଦି !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_child_environment(
	long	rows,
	long	columns,
	ଅକ୍ଷର	*term,
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    ଅକ୍ଷର	envbuf[50];
# ଅନ୍ୟଥା
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Term[30];
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Rows[20];
    ସ୍ଥିର ଅକ୍ଷର	envbuf_ଧାଡ଼ିଗୁଡ଼ିକ[20];
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Columns[20];
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ସ୍ଥିର ଅକ୍ଷର	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((ଅକ୍ଷର *)envbuf, "%ld", rows);
    setenv("ROWS", (ଅକ୍ଷର *)envbuf, 1);
    sprintf((ଅକ୍ଷର *)envbuf, "%ld", rows);
    setenv("LINES", (ଅକ୍ଷର *)envbuf, 1);
    sprintf((ଅକ୍ଷର *)envbuf, "%ld", columns);
    setenv("COLUMNS", (ଅକ୍ଷର *)envbuf, 1);
    sprintf((ଅକ୍ଷର *)envbuf, "%d", t_colors);
    setenv("COLORS", (ଅକ୍ଷର *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    ଯଦି (is_terminal)
    {
	sprintf((ଅକ୍ଷର *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (ଅକ୍ଷର *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ଖାଲି ? "" : (ଅକ୍ଷର *)serverName, 1);
#  endif
# ଅନ୍ୟଥା
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, ଆକାର(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, ଆକାର(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_ଧାଡ଼ିଗୁଡ଼ିକ, ଆକାର(envbuf_ଧାଡ଼ିଗୁଡ଼ିକ), "LINES=%ld", rows);
    putenv(envbuf_ଧାଡ଼ିଗୁଡ଼ିକ);
    vim_snprintf(envbuf_Columns, ଆକାର(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, ଆକାର(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    ଯଦି (is_terminal)
    {
	vim_snprintf(envbuf_Version, ଆକାର(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, ଆକାର(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ଖାଲି ? "" : (ଅକ୍ଷର *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ସ୍ଥିର ଶୂନ୍ୟ
set_default_child_environment(ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#ଯଦି defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ସ୍ଥିର ଶୂନ୍ୟ
open_pty(ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pty_master_fd, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *pty_slave_fd, char_u **name1, char_u **name2)
{
    ଅକ୍ଷର	*tty_name;

    ଯଦି (name1 != ଖାଲି)
	*name1 = ଖାଲି;
    ଯଦି (name2 != ଖାଲି)
	*name2 = ଖାଲି;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    ଯଦି (*pty_master_fd < 0)
	ଫେରନ୍ତୁ;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# ଅନ୍ୟଥା
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    ଯଦି (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (name1 != ଖାଲି)
	    *name1 = vim_strsave((char_u *)tty_name);
	ଯଦି (name2 != ଖାଲି)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
may_send_sigint(ପୂର୍ଣ୍ଣସଂଖ୍ୟା c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    ଯଦି (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# ଅନ୍ୟଥା
	kill(0, SIGINT);
# endif
	ଯଦି (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#ଯଦି !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
unix_build_argv(
	char_u *cmd,
	ଅକ୍ଷର ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    ଅକ୍ଷର	**argv = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argc;

    *sh_tofree = vim_strsave(p_sh);
    ଯଦି (*sh_tofree == ଖାଲି)		// out of memory
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	ଫେରନ୍ତୁ FAIL;
    *argvp = argv;

    ଯଦି (cmd != ଖାଲି)
    {
	char_u	*s;
	char_u	*p;

	ଯଦି (extra_shell_arg != ଖାଲି)
	    argv[argc++] = (ଅକ୍ଷର *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	ଯଦି (*shcf_tofree == ଖାଲି)    // out of memory
	    ଫେରନ୍ତୁ FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	ଯେତେବେଳେ (*p != NUL)
	{
	    argv[argc++] = (ଅକ୍ଷର *)s;
	    ଯେତେବେଳେ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (ଅକ୍ଷର *)cmd;
    }
    argv[argc] = ଖାଲି;
    ଫେରନ୍ତୁ OK;
}
#endif

#ଯଦି defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell_terminal(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    ଅକ୍ଷର	**argv = ଖାଲି;
    char_u	*tofree1 = ଖାଲି;
    char_u	*tofree2 = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ଯଦି (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ଯାଅ theend;

    init_job_options(&opt);
    ch_log(ଖାଲି, "starting terminal for system command '%s'", cmd);
    buf = term_start(ଖାଲି, argv, &opt, TERM_START_SYSTEM);
    ଯଦି (buf == ଖାଲି)
	ଯାଅ theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ଯଦି (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	ଯେତେବେଳେ (term_use_loop())
	{
	    ଯଦି (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ଯଦି (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    ଅନ୍ୟଥା
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ଖାଲି, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    ଯଦି (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    ଫେରନ୍ତୁ retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell_system(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    ଅକ୍ଷର	*ifn = ଖାଲି;
    ଅକ୍ଷର	*ofn = ଖାଲି;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;

    out_flush();

    ଯଦି (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# ଯଦି defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  ଯଦି defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    ଯଦି (cmd == ଖାଲି)
	x = system((ଅକ୍ଷର *)p_sh);
    ଅନ୍ୟଥା
    {
# ifdef VMS
	ଯଦି (ofn = strchr((ଅକ୍ଷର *)cmd, '>'))
	    *ofn++ = '\0';
	ଯଦି (ifn = strchr((ଅକ୍ଷର *)cmd, '<'))
	{
	    ଅକ୍ଷର *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    ଯଦି (p)
		*p = '\0';
	}
	ଯଦି (ofn)
	    x = vms_sys((ଅକ୍ଷର *)cmd, ofn, ifn);
	ଅନ୍ୟଥା
	    x = system((ଅକ୍ଷର *)cmd);
# ଅନ୍ୟଥା
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ଖାଲି ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	ଯଦି (newcmd == ଖାଲି)
	    x = 0;
	ଅନ୍ୟଥା
	{
	    sprintf((ଅକ୍ଷର *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ଖାଲି ? "" : (ଅକ୍ଷର *)extra_shell_arg,
		    (ଅକ୍ଷର *)p_shcf,
		    (ଅକ୍ଷର *)cmd);
	    x = system((ଅକ୍ଷର *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    ଯଦି (emsg_silent)
	;
    ଅନ୍ୟଥା ଯଦି (x == 127)
	ସନ୍ଦେଶ_puts(_("\nCannot execute shell sh\n"));
    ଅନ୍ୟଥା ଯଦି (x && !(options & SHELL_SILENT))
    {
	ସନ୍ଦେଶ_puts(_("\nshell returned "));
	ସନ୍ଦେଶ_outnum((long)x);
	ସନ୍ଦେଶ_putchar('\n');
    }

    ଯଦି (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# ଯଦି defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    ଫେରନ୍ତୁ x;
}

#ଅନ୍ୟଥା // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell_fork(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = -1;
    ଅକ୍ଷର	**argv = ଖାଲି;
    char_u	*tofree1 = ଖାଲି;
    char_u	*tofree2 = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_slave_fd = -1;
# endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_toshell[2];		// for pipes
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_fromshell[2];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pipe_error = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    ଯଦି (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    ଯଦି (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    ଯଦି (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ଯାଅ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    ଯଦି ((options & (SHELL_READ|SHELL_WRITE))
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
	ଯଦି (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ଖାଲି, ଖାଲି);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	ଯଦି (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    ଯଦି (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		ଯଦି (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    ଯଦି (pipe_error)
	    {
		ସନ୍ଦେଶ_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    ଯଦି (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	ଯଦି (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    ସନ୍ଦେଶ_puts(_("\nCannot fork\n"));
	    ଯଦି ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		ଯଦି (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		ଅନ୍ୟଥା				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	ଅନ୍ୟଥା ଯଦି (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    ଯଦି (ch_log_active())
	    {
		ch_log(ଖାଲି, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    ଯଦି (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd;

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
		ଯଦି (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    ଅନ୍ୟଥା ଯଦି ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		ଯଦି (p_stmp)
		{
		    (ଶୂନ୍ୟ)setsid();
#  ଯଦି defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		ଯଦି (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    ଯଦି (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (ଅକ୍ଷର *)ଖାଲି);
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
		ଯଦି (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    ଯଦି (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		ଅନ୍ୟଥା
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
		    ଯଦି (gui.in_use)
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
	ଅନ୍ୟଥା			// parent
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
	    ଯଦି ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    ta_len = 0;		// valid bytes in ta_buf[]
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    p_more_save;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    old_State;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    c;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    toshell_fd;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    fromshell_fd;
		garray_T    ga;
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		ଯଦି (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		ଅନ୍ୟଥା
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

		ଯଦି ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    ଯଦି ((wpid = fork()) == -1)
		    {
			ସନ୍ଦେଶ_puts(_("\nCannot fork\n"));
		    }
		    ଅନ୍ୟଥା ଯଦି (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			ପାଇଁ (;;)
			{
			    ଯଦି (lplen == 0)
				len = 0;
			    ଅନ୍ୟଥା ଯଦି (lp[written] == NL)
				// NL -> NUL translation
				len = ଲିଖ(toshell_fd, "", (size_t)1);
			    ଅନ୍ୟଥା
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = ଲିଖ(toshell_fd, (ଅକ୍ଷର *)lp + written,
					   s == ଖାଲି ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    ଯଦି (len == (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				ଯଦି (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_ଧାଡ଼ି_ଗଣନା
						    || curbuf->b_p_eol)))
				    vim_ignored = ଲିଖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				ଯଦି (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ଭାଙ୍ଗ;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    ଅନ୍ୟଥା ଯଦି (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    ଅନ୍ୟଥା // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		ଯଦି (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		ପାଇଁ (;;)
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
		    ଯଦି (!(options & SHELL_EXPAND)
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
		      ଯଦି (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      ଯଦି (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			ଯଦି (len == 1 && (pty_master_fd < 0 || cmd != ଖାଲି))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    ଯଦି (pty_master_fd < 0 && toshell_fd >= 0
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
			ଯଦି (pty_master_fd < 0)
			{
			    ପାଇଁ (i = ta_len; i < ta_len + len; ++i)
			    {
				ଯଦି (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    ସନ୍ଦେଶ_putchar(ta_buf[i]);
				ଅନ୍ୟଥା ଯଦି (has_mbyte)
				{
				    ପୂର୍ଣ୍ଣସଂଖ୍ୟା l = (*mb_ptr2len)(ta_buf + i);

				    ସନ୍ଦେଶ_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				ଅନ୍ୟଥା
				    ସନ୍ଦେଶ_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
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
			ଯଦି (options & SHELL_WRITE)
			    ta_len = 0;
			ଅନ୍ୟଥା ଯଦି (toshell_fd >= 0)
			{
			    len = ଲିଖ(toshell_fd, (ଅକ୍ଷର *)ta_buf, (size_t)1);
			    ଯଦି (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    ଯଦି (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# ଅନ୍ୟଥା
			kill(0, SIGINT);
# endif
			ଯଦି (wpid > 0)
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
		    ଯେତେବେଳେ (RealWaitForChar(fromshell_fd, 10L, ଖାଲି, ଖାଲି))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			ଯଦି (len <= 0)		    // end of file or error
			    ଯାଅ finished;

			noread_cnt = 0;
			ଯଦି (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    ପାଇଁ (i = 0; i < len; ++i)
			    {
				ଯଦି (buffer[i] == NL)
				    append_ga_ଧାଡ଼ି(&ga);
				ଅନ୍ୟଥା ଯଦି (buffer[i] == NUL)
				    ga_append(&ga, NL);
				ଅନ୍ୟଥା
				    ga_append(&ga, buffer[i]);
			    }
			}
			ଅନ୍ୟଥା ଯଦି (has_mbyte)
			{
			    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    ପାଇଁ (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				ଯଦି (l == 0)
				    l = 1;  // NUL byte?
				ଅନ୍ୟଥା ଯଦି (MB_BYTE2LEN(*p) != l)
				    ଭାଙ୍ଗ;
			    }
			    ଯଦି (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				ଯଦି (len >= 12)
				    ++p;
				ଅନ୍ୟଥା
				{
				    buffer_off = len;
				    ଜାରି;
				}
			    }
			    c = *p;
			    *p = NUL;
			    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)buffer);
			    ଯଦି (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				ଜାରି;
			    }
			    buffer_off = 0;
			}
			ଅନ୍ୟଥା
			{
			    buffer[len] = NUL;
			    ସନ୍ଦେଶ_puts((ଅକ୍ଷର *)buffer);
			}

			windgoto(ସନ୍ଦେଶ_row, cmdline_col_off + ସନ୍ଦେଶ_col);
			cursor_on();
			out_flush();
			ଯଦି (got_int)
			    ଭାଙ୍ଗ;

# ifdef ELAPSED_FUNC
			ଯଦି (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    ଯଦି (msec > 2000)
			    {
				noread_cnt = 5;
				ଭାଙ୍ଗ;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    ଯଦି (wait_pid == pid)
		    {
			ଯଦି (noread_cnt < 5)
			    ଜାରି;
			ଭାଙ୍ଗ;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    ଯଦି ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    ଅନ୍ୟଥା
			wait_pid = 0;

# ଯଦି defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		ଯଦି (options & SHELL_READ)
		{
		    ଯଦି (ga.ga_len > 0)
		    {
			append_ga_ଧାଡ଼ି(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    ଅନ୍ୟଥା
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		ଯଦି (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		ଯଦି (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# ଯଦି (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    ଅନ୍ୟଥା
	    {
		long delay_msec = 1;

		ଯଦି (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		ପାଇଁ (;;)
		{
		    ଯଦି (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  ଅନ୍ୟଥା
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    ଯଦି ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			ଭାଙ୍ଗ;
		    }

#  ଯଦି defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    ଯଦି (++delay_msec > 10)
			delay_msec = 10;
		}

		ଯଦି (tmode == TMODE_RAW)
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
	    ଯଦି (wait_pid != pid)
		(ଶୂନ୍ୟ)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    ଯଦି (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    ଯଦି (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ଖାଲି);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    ଯଦି (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    ଯଦି (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		ଯଦି (retval != 0 && !emsg_silent)
		{
		    ଯଦି (retval == EXEC_FAILED)
		    {
			ସନ୍ଦେଶ_puts(_("\nCannot execute shell "));
			ସନ୍ଦେଶ_outtrans(p_sh);
			ସନ୍ଦେଶ_putchar('\n');
		    }
		    ଅନ୍ୟଥା ଯଦି (!(options & SHELL_SILENT))
		    {
			ସନ୍ଦେଶ_puts(_("\nshell returned "));
			ସନ୍ଦେଶ_outnum((long)retval);
			ସନ୍ଦେଶ_putchar('\n');
		    }
		}
	    }
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_puts(_("\nCommand terminated\n"));
	}
    }

error:
    ଯଦି (!did_settmode)
	ଯଦି (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    ଫେରନ୍ତୁ retval;
}
#endif // USE_SYSTEM

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(ଖାଲି, "executing shell command: %s", cmd);
#endif
#ଯଦି defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    ଯଦି (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ଖାଲି
					      && (options & SHELL_SILENT) == 0)
	ଫେରନ୍ତୁ mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    ଫେରନ୍ତୁ mch_call_shell_system(cmd, options);
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ mch_call_shell_fork(cmd, options);
#endif
}

#ଯଦି defined(FEAT_EVAL)
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
    ଅକ୍ଷର	**argv,
    char_u	*infile,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ret_len)
{
    pid_t	pid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_out[2] = {-1, -1};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = -1;
    char_u	*buffer = ଖାଲି;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ଖାଲି, "directly executing: %s", argv[0]);

    ଯଦି (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# ଅନ୍ୟଥା
    pid = fork();
# endif
    ଯଦି (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	ଫେରନ୍ତୁ ଖାଲି;
    }

    ଯଦି (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	ଯଦି (ch_log_active())
	{
	    ch_log(ଖାଲି, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	ଯଦି (infile != ଖାଲି)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd_in = open((ଅକ୍ଷର *)infile, O_RDONLY);
	    ଯଦି (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	ଅନ୍ୟଥା
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା nullfd = open("/dev/null", O_RDONLY);
	    ଯଦି (nullfd >= 0)
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
    ପାଇଁ (;;)
    {
	ଅକ୍ଷର	buf[4096];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	n;

	n = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)ପଠ(fd_out[0], buf, ଆକାର(buf));
	ଯଦି (n <= 0)
	    ଭାଙ୍ଗ;
	ଯଦି (ga_grow(&ga, n) == FAIL)
	    ଭାଙ୍ଗ;
	mch_memmove((ଅକ୍ଷର *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (ଶୂନ୍ୟ)waitpid(pid, &status, 0);
    ଯଦି (WIFEXITED(status))
	status = WEXITSTATUS(status);
    ଅନ୍ୟଥା
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    ଯଦି (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	ଯଦି (buffer != ଖାଲି)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    ଯଦି (ret_len == ଖାଲି)
	    {
		ପୂର୍ଣ୍ଣସଂଖ୍ୟା	i;

		// Change NUL into SOH, otherwise the string is truncated.
		ପାଇଁ (i = 0; i < ga.ga_len; ++i)
		    ଯଦି (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    ଅନ୍ୟଥା
		*ret_len = ga.ga_len;
	}
    }
    ଅନ୍ୟଥା ଯଦି (ret_len != ଖାଲି)
	*ret_len = 0;

    ga_clear(&ga);
    ଫେରନ୍ତୁ buffer;
}
#endif

#ଯଦି defined(FEAT_JOB_CHANNEL)
    ଶୂନ୍ୟ
mch_job_start(ଅକ୍ଷର **argv, job_T *job, jobopt_T *options, ପୂର୍ଣ୍ଣସଂଖ୍ୟା is_terminal)
{
    pid_t	pid;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_in[2] = {-1, -1};	// for stdin
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_out[2] = {-1, -1};	// for stdout
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		fd_err[2] = {-1, -1};	// for stderr
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_master_fd = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_slave_fd = -1;
    channel_T	*channel = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    ଯଦି (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    ଯଦି (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    ଯଦି (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((ଅକ୍ଷର *)fname, O_RDONLY, 0);
	ଯଦି (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    ଅନ୍ୟଥା
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	ଯଦି (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    ଯାଅ failed;

    ଯଦି (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((ଅକ୍ଷର *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ଯଦି (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    ଅନ୍ୟଥା ଯଦି (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	ଯାଅ failed;

    ଯଦି (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((ଅକ୍ଷର *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ଯଦି (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ଯାଅ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    ଅନ୍ୟଥା ଯଦି (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	ଯାଅ failed;

    ଯଦି (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	ଯଦି (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ଯଦି (channel != ଖାଲି)
		++channel->ch_refcount;
	}
	ଅନ୍ୟଥା
	    channel = add_channel();
	ଯଦି (channel == ଖାଲି)
	    ଯାଅ failed;
	ଯଦି (job->jv_tty_out != ଖାଲି)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    ଯଦି (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	ଯାଅ failed;
    }
    ଯଦି (pid == 0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	null_fd = -1;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	ଯଦି (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(ଶୂନ୍ୟ)setsid();
# endif

# ifdef FEAT_TERMINAL
	ଯଦି (options->jo_term_rows > 0)
	{
	    ଅକ୍ଷର *term = (ଅକ୍ଷର *)T_NAME;

#  ifdef FEAT_GUI
	    ଯଦି (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    ଯଦି (term == ଖାଲି || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		ଯଦି (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		ଅନ୍ୟଥା ଯଦି (t_colors > 16)
		    term = "xterm-color";
		ଅନ୍ୟଥା
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	ଅନ୍ୟଥା
# endif
	    set_default_child_environment(is_terminal);

	ଯଦି (options->jo_env != ଖାଲି)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		todo = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_ବସ୍ତୁଗୁଡ଼ିକ(&dict->dv_hashtab, hi, todo)
		ଯଦି (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ଚାବି, tv_get_string(item));
		    --todo;
		}
	}

	ଯଦି (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    ଯଦି (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	ଯଦି (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (ଅକ୍ଷର *)ଖାଲି);
# endif
	}

	// set up stdin for the child
	close(0);
	ଯଦି (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ଅନ୍ୟଥା ଯଦି (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ଅନ୍ୟଥା
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	ଯଦି (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	ଅନ୍ୟଥା ଯଦି (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	ଅନ୍ୟଥା ଯଦି (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ଅନ୍ୟଥା
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	ଯଦି (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ଅନ୍ୟଥା ଯଦି (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ଅନ୍ୟଥା
	    vim_ignored = dup(fd_out[1]);

	ଯଦି (fd_in[0] >= 0)
	    close(fd_in[0]);
	ଯଦି (fd_in[1] >= 0)
	    close(fd_in[1]);
	ଯଦି (fd_out[0] >= 0)
	    close(fd_out[0]);
	ଯଦି (fd_out[1] >= 0)
	    close(fd_out[1]);
	ଯଦି (fd_err[0] >= 0)
	    close(fd_err[0]);
	ଯଦି (fd_err[1] >= 0)
	    close(fd_err[1]);
	ଯଦି (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	ଯଦି (null_fd >= 0)
	    close(null_fd);

	ଯଦି (options->jo_cwd != ଖାଲି && mch_chdir((ଅକ୍ଷର *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	ଯଦି (stderr_works)
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

    ଯଦି (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    ଯଦି (fd_in[0] >= 0)
	close(fd_in[0]);
    ଯଦି (fd_out[1] >= 0)
	close(fd_out[1]);
    ଯଦି (fd_err[1] >= 0)
	close(fd_err[1]);
    ଯଦି (channel != ଖାଲି)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା in_fd = INVALID_FD;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା out_fd = INVALID_FD;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା err_fd = INVALID_FD;

	ଯଦି (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	ଯଦି (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	ଯଦି (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    ଯଦି (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    ଅନ୍ୟଥା ଯଦି (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (fd_in[1] >= 0)
	    close(fd_in[1]);
	ଯଦି (fd_out[0] >= 0)
	    close(fd_out[0]);
	ଯଦି (fd_err[0] >= 0)
	    close(fd_err[0]);
	ଯଦି (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    ଫେରନ୍ତୁ;

failed:
    channel_unref(channel);
    ଯଦି (fd_in[0] >= 0)
	close(fd_in[0]);
    ଯଦି (fd_in[1] >= 0)
	close(fd_in[1]);
    ଯଦି (fd_out[0] >= 0)
	close(fd_out[0]);
    ଯଦି (fd_out[1] >= 0)
	close(fd_out[1]);
    ଯଦି (fd_err[0] >= 0)
	close(fd_err[0]);
    ଯଦି (fd_err[1] >= 0)
	close(fd_err[1]);
    ଯଦି (pty_master_fd >= 0)
	close(pty_master_fd);
    ଯଦି (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ସ୍ଥିର char_u *
get_signal_name(ପୂର୍ଣ୍ଣସଂଖ୍ୟା sig)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	numbuf[NUMBUFLEN];

    ଯଦି (sig == SIGKILL)
	ଫେରନ୍ତୁ vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    ପାଇଁ (i = 0; signal_info[i].sig != -1; i++)
	ଯଦି (sig == signal_info[i].sig)
	    ଫେରନ୍ତୁ strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((ଅକ୍ଷର *)numbuf, NUMBUFLEN, "%d", sig);
    ଫେରନ୍ତୁ vim_strnsave(numbuf, i);
}

    ଅକ୍ଷର *
mch_job_status(job_T *job)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    ଯଦି (wait_pid == -1)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା waitpid_errno = errno;
	ଯଦି (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    ଫେରନ୍ତୁ "run";

	// process must have exited
	ଯଦି (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	ଯାଅ return_dead;
    }
    ଯଦି (wait_pid == 0)
	ଫେରନ୍ତୁ "run";
    ଯଦି (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	ଯଦି (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	ଯାଅ return_dead;
    }
    ଯଦି (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	ଯଦି (job->jv_status < JOB_ENDED && job->jv_termsig != ଖାଲି)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	ଯାଅ return_dead;
    }
    ଫେରନ୍ତୁ "run";

return_dead:
    ଯଦି (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    ଫେରନ୍ତୁ "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    ଯଦି (dont_check_job_ended > 0)
	ଫେରନ୍ତୁ ଖାଲି;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    ଯଦି (wait_pid <= 0)
	// no process ended
	ଫେରନ୍ତୁ ଖାଲି;
    ପାଇଁ (job = job_list; job != ଖାଲି; job = job->jv_next)
    {
	ଯଦି (job->jv_pid == wait_pid)
	{
	    ଯଦି (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    ଅନ୍ୟଥା ଯଦି (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    ଯଦି (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    ଫେରନ୍ତୁ job;
	}
    }
    ଫେରନ୍ତୁ ଖାଲି;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_signal_job(job_T *job, char_u *how)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    sig = -1;

    ଯଦି (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    ଅନ୍ୟଥା ଯଦି (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    ଅନ୍ୟଥା ଯଦି (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    ଅନ୍ୟଥା ଯଦି (STRCMP(how, "int") == 0)
	sig = SIGINT;
    ଅନ୍ୟଥା ଯଦି (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    ଅନ୍ୟଥା ଯଦି (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    ଅନ୍ୟଥା ଯଦି (SAFE_isdigit(*how))
	sig = atoi((ଅକ୍ଷର *)how);
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ FAIL;

    // Never kill ourselves!
    ଯଦି (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    ଫେରନ୍ତୁ OK;
}

/*
 * Clear the data related to "job".
 */
    ଶୂନ୍ୟ
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (ଶୂନ୍ୟ)waitpid(job->jv_pid, ଖାଲି, WNOHANG);
}
#endif

#ଯଦି defined(FEAT_TERMINAL)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_master_fd = -1;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    ଯଦି (pty_master_fd < 0 || pty_slave_fd < 0)
	ଫେରନ୍ତୁ FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    ଯଦି (channel == ଖାଲି)
    {
	close(pty_master_fd);
	ଫେରନ୍ତୁ FAIL;
    }
    ଯଦି (job->jv_tty_out != ଖାଲି)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    ଫେରନ୍ତୁ OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    ଶୂନ୍ୟ
mch_breakcheck(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force)
{
    ଯଦି ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ଖାଲି, ଖାଲି))
	fill_ନିବେଶ_buf(FALSE);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WaitForChar(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ)
{
#ifdef FEAT_TIMERS
    ଫେରନ୍ତୁ ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_ନିବେଶ) == OK;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ WaitForCharOrMouse(msec, interrupted, ignore_ନିବେଶ);
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
WaitForCharOrMouse(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted, ପୂର୍ଣ୍ଣସଂଖ୍ୟା ignore_ନିବେଶ)
{
#ifdef FEAT_MOUSE_GPM
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		rest;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		avail;

    ଯଦି (!ignore_ନିବେଶ && ନିବେଶ_available())	    // something in inbuf[]
	ଫେରନ୍ତୁ 1;

#ଯଦି defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    ଯଦି (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	ଯଦି (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#ଯଦି defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    ଯଦି (do_xterm_trace())
	rest = msec;
# endif
    କର
    {
# ifdef FEAT_XCLIPBOARD
	ଯଦି (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    ଯଦି (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    ଯଦି (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	ଯଦି (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	ଯଦି (!avail && !gpm_process_wanted)
# ଅନ୍ୟଥା
	avail = RealWaitForChar(read_cmd_fd, msec, ଖାଲି, interrupted);
	ଯଦି (!avail)
# endif
	{
	    ଯଦି (!ignore_ନିବେଶ && ନିବେଶ_available())
		ଫେରନ୍ତୁ 1;
# ifdef FEAT_XCLIPBOARD
	    ଯଦି (rest == 0 || !do_xterm_trace())
# endif
		ଭାଙ୍ଗ;
	}
    }
    ଯେତେବେଳେ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#ଅନ୍ୟଥା
    avail = RealWaitForChar(read_cmd_fd, msec, ଖାଲି, interrupted);
#endif
    ଫେରନ୍ତୁ avail;
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
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
RealWaitForChar(ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd, long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *check_for_gpm UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା *interrupted)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ret;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		result;
# ଯଦି defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    ଯଦି (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    ଯଦି (busy)
	ଫେରନ୍ତୁ 0;
# endif

# ifdef MAY_LOOP
    ପାଇଁ (;;)
# endif
    {
# ifdef MAY_LOOP
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ସଂରଚନା pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		xsmp_idx = -1;
#  endif
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		towait = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	ଯଦି (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	ଯଦି ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	ଯଦି (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_ସଂଖ୍ୟା(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ଯଦି (check_for_gpm != ଖାଲି && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	ଯଦି (xsmp_icefd != -1)
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
	ଯଦି (interrupted != ଖାଲି)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	ଯଦି (result == 0 && interrupted != ଖାଲି && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	ଯଦି (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ଯଦି (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	ଯଦି (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    ଯଦି (--ret == 0 && !ନିବେଶ_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ଯଦି (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	ଯଦି (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    ଯଦି (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    ଅନ୍ୟଥା ଯଦି (fds[xsmp_idx].revents & POLLHUP)
	    {
		ଯଦି (p_verbose > 0)
		    verb_ସନ୍ଦେଶ(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    ଯଦି (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ଯଦି (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# ଅନ୍ୟଥା // HAVE_SELECT

	ସଂରଚନା timeval  tv;
	ସଂରଚନା timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ସ୍ଥିର fd_set	rfds, wfds, efds;
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	ଯଦି (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	ଯଦି (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	ଅନ୍ୟଥା
	    tvp = ଖାଲି;

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
	ଯଦି ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    ଯଦି (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	ଯଦି (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_ସଂଖ୍ୟା(xterm_dpy), &rfds);
	    ଯଦି (maxfd < Connection_ସଂଖ୍ୟା(xterm_dpy))
		maxfd = Connection_ସଂଖ୍ୟା(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ଯଦି (check_for_gpm != ଖାଲି && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    ଯଦି (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	ଯଦି (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    ଯଦି (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	ଯଦି (interrupted != ଖାଲି)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	ଯଦି (result)
	    --ret;
	ଅନ୍ୟଥା ଯଦି (interrupted != ଖାଲି && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	ଯଦି (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    ଯଦି (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    ଯଦି (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ଖାଲି, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    ଯାଅ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	ଯଦି (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	ଯଦି (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ଯଦି (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	ଯଦି (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_ସଂଖ୍ୟା(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    ଯଦି (--ret == 0 && !ନିବେଶ_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ଯଦି (ret > 0 && check_for_gpm != ଖାଲି && gpm_flag && gpm_fd >= 0)
	{
	    ଯଦି (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    ଅନ୍ୟଥା ଯଦି (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	ଯଦି (ret > 0 && xsmp_icefd != -1)
	{
	    ଯଦି (FD_ISSET(xsmp_icefd, &efds))
	    {
		ଯଦି (p_verbose > 0)
		    verb_ସନ୍ଦେଶ(_("XSMP lost ICE connection"));
		xsmp_close();
		ଯଦି (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    ଅନ୍ୟଥା ଯଦି (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		ଯଦି (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ଯଦି (ret >= 0)
	    (ଶୂନ୍ୟ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	ଯଦି (finished || msec == 0)
	    ଭାଙ୍ଗ;

#  ଯଦି defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	ଯଦି (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    ଭାଙ୍ଗ;
#  endif

	// We're going to loop around again, find out for how long
	ଯଦି (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  ଅନ୍ୟଥା
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    ଯଦି (msec <= 0)
		ଭାଙ୍ଗ;	// waited long enough
	}
# endif
    }

    ଫେରନ୍ତୁ result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// EW_* flags
{
    ଫେରନ୍ତୁ unix_expandpath(gap, path, 0, flags, FALSE);
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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expand_wildcards(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		   ଅଙ୍କ_pat,
    char_u	 **pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		  *ଅଙ୍କ_file,
    char_u	***file,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		   flags)	// EW_* flags
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    size_t	len;
    long	llen;
    char_u	*p;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		j;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		shell_style = STYLE_ECHO;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		check_spaces;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	did_find_nul = FALSE;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ସ୍ଥିର string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ସ୍ଥିର string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *ଅଙ୍କ_file = 0;	// default: no files found
    *file = ଖାଲି;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    ଯଦି (!have_wildcard(ଅଙ୍କ_pat, pat))
	ଫେରନ୍ତୁ save_patterns(ଅଙ୍କ_pat, pat, ଅଙ୍କ_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    ଯଦି (sandbox != 0 && check_secure())
	ଫେରନ୍ତୁ FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    ଯଦି (secure || restricted)
	ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; ++i)
	    ଯଦି (vim_strchr(pat[i], '`') != ଖାଲି
		    && (check_restricted() || check_secure()))
		ଫେରନ୍ତୁ FAIL;

    /*
     * get a name for the temp file
     */
    ଯଦି ((tempname = vim_tempname('o', FALSE)) == ଖାଲି)
    {
	emsg(_(e_cant_get_temp_file_name));
	ଫେରନ୍ତୁ FAIL;
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
    ଯଦି (ଅଙ୍କ_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    ଅନ୍ୟଥା ଯଦି ((len = STRLEN(p_sh)) >= 3)
    {
	ଯଦି (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	ଅନ୍ୟଥା ଯଦି (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    ଯଦି (shell_style == STYLE_ECHO)
    {
	ଯଦି (strstr((ଅକ୍ଷର *)gettail(p_sh), "bash") != ଖାଲି)
	    shell_style = STYLE_GLOBSTAR;
	ଅନ୍ୟଥା ଯଦି (strstr((ଅକ୍ଷର *)gettail(p_sh), "sh") != ଖାଲି)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    ଯଦି (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# ଅନ୍ୟଥା
	++len;				// add space
	ପାଇଁ (j = 0; pat[i][j] != NUL; ++j)
	{
	    ଯଦି (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ଖାଲି)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    ଯଦି (command == ଖାଲି)
    {
	// out of memory
	vim_free(tempname);
	ଫେରନ୍ତୁ FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    ଯଦି (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((ଅକ୍ଷର *)command, ")>");
	ଯଦି (p == ଖାଲି)
	{
	    ;					    // can't happen ;-)
	}
	ଅନ୍ୟଥା
	{
	    --p;
	    ଯେତେବେଳେ (p > command && VIM_ISWHITE(*p))
		--p;
	    ଯଦି (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    ଯେତେବେଳେ (p > command && VIM_ISWHITE(*p))
		--p;
	    ଯଦି (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "print -N >%s", tempname);
	ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	ଅନ୍ୟଥା
	    commandlen = vim_snprintf((ଅକ୍ଷର *)command, len, "echo >%s", tempname);

	ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((ଅକ୍ଷର *)command + commandlen, len, " \"%s\"", pat[i]);
# ଅନ୍ୟଥା
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    ପାଇଁ (j = 0; pat[i][j] != NUL; ++j)
	    {
		ଯଦି (pat[i][j] == '`')
		    intick = !intick;
		ଅନ୍ୟଥା ଯଦି (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    ଯଦି (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ଖାଲି
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		ଅନ୍ୟଥା ଯଦି (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ଖାଲି)
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
    ଯଦି (flags & EW_SILENT)
	show_shell_mess = FALSE;
    ଯଦି (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    ଯଦି (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_GLOB && !have_dollars(ଅଙ୍କ_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    ଯଦି (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ଖାଲି;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    ଯଦି (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	ଯଦି (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    ସନ୍ଦେଶ_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    ଯଦି (!(flags & EW_SILENT))
# endif
	    {
		ସନ୍ଦେଶ(_(e_cannot_expand_wildcards));
		ସନ୍ଦେଶ_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	ଯଦି (shell_style == STYLE_BT)
	    ଫେରନ୍ତୁ FAIL;
	ଯାଅ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((ଅକ୍ଷର *)tempname, READBIN);
    ଯଦି (fd == ଖାଲି)
    {
	// Something went wrong, perhaps a file name with a special char.
	ଯଦି (!(flags & EW_SILENT))
	{
	    ସନ୍ଦେଶ(_(e_cannot_expand_wildcards));
	    ସନ୍ଦେଶ_start();		// don't overwrite this message
	}
	vim_free(tempname);
	ଯାଅ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    ଯଦି (llen < 0)
	// just in case ftell() would fail
	buffer = ଖାଲି;
    ଅନ୍ୟଥା
	buffer = alloc(llen + 1);
    ଯଦି (buffer == ଖାଲି)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	ଫେରନ୍ତୁ FAIL;
    }
    len = llen;
    i = fread((ଅକ୍ଷର *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    ଯଦି (i != (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	ଫେରନ୍ତୁ FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    ପାଇଁ (i = 0; i < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len; ++i)
	ଯଦି (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    ଯଦି (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	ପାଇଁ (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    ଯେତେବେଳେ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    ଅନ୍ୟଥା ଯଦି (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	ପାଇଁ (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    ଯେତେବେଳେ (*p != '\n' && *p != NUL)
		++p;
	    ଯଦି (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    ଅନ୍ୟଥା
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
	ଯଦି (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    ଯଦି (len && (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)STRLEN(buffer) < (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)len)
		did_find_nul = TRUE;
	    ଅନ୍ୟଥା
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	ଯଦି (len && buffer[len - 1] == NUL)
	    --len;
	ଅନ୍ୟଥା
	    buffer[len] = NUL;
	i = 0;
	ପାଇଁ (p = buffer; p < buffer + len; ++p)
	    ଯଦି (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	ଯଦି (len)
	    ++i;			// count last entry
    }
    ଯଦି (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	ଯାଅ notfound;
    }
    *ଅଙ୍କ_file = i;
    *file = ALLOC_MULT(char_u *, i);
    ଯଦି (*file == ଖାଲି)
    {
	// out of memory
	vim_free(buffer);
	ଫେରନ୍ତୁ FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    ପାଇଁ (i = 0; i < *ଅଙ୍କ_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	ଯଦି (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    ଯେତେବେଳେ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    ଯଦି (p == buffer + len)		// last entry
		*p = NUL;
	    ଅନ୍ୟଥା
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	ଅନ୍ୟଥା		// NUL separates
	{
	    ଯେତେବେଳେ (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    ପାଇଁ (j = 0, i = 0; i < *ଅଙ୍କ_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	ଯଦି (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ଜାରି;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	ଯଦି ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ଜାରି;

	// Skip files that are not executable if we check for that.
	ଯଦି (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ଖାଲି, !(flags & EW_SHELLCMD)))
	    ଜାରି;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	ଯଦି (p)
	{
	    STRCPY(p, (*file)[i]);
	    ଯଦି (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *ଅଙ୍କ_file = j;

    ଯଦି (*ଅଙ୍କ_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	ଯାଅ notfound;
    }

    ଫେରନ୍ତୁ OK;

notfound:
    ଯଦି (flags & EW_NOTFOUND)
	ଫେରନ୍ତୁ save_patterns(ଅଙ୍କ_pat, pat, ଅଙ୍କ_file, file);
    ଫେରନ୍ତୁ FAIL;
}

#endif // VMS

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
save_patterns(
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		ଅଙ୍କ_pat,
    char_u	**pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ଅଙ୍କ_file,
    char_u	***file)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, ଅଙ୍କ_pat);
    ଯଦି (*file == ଖାଲି)
	ଫେରନ୍ତୁ FAIL;
    ପାଇଁ (i = 0; i < ଅଙ୍କ_pat; i++)
    {
	s = vim_strsave(pat[i]);
	ଯଦି (s != ଖାଲି)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *ଅଙ୍କ_file = ଅଙ୍କ_pat;
    ଫେରନ୍ତୁ OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_exp_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା
	    ଯଦି (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#ଅନ୍ୟଥା
				    "*?[{'"
#endif
						, *p) != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା
	    ଯଦି (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#ଅନ୍ୟଥା
				    "*?[{`'$"
#endif
						, *p) != ଖାଲି
		|| (*p == '~' && p[1] != NUL))
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
have_wildcard(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, char_u **file)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < ଅଙ୍କ; i++)
	ଯଦି (mch_has_wildcard(file[i]))
	    ଫେରନ୍ତୁ 1;
    ଫେରନ୍ତୁ 0;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
have_dollars(ପୂର୍ଣ୍ଣସଂଖ୍ୟା ଅଙ୍କ, char_u **file)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    i;

    ପାଇଁ (i = 0; i < ଅଙ୍କ; i++)
	ଯଦି (vim_strchr(file[i], '$') != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    ଫେରନ୍ତୁ FALSE;
}

#ଯଦି !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_rename(ଅଚଳ ଅକ୍ଷର *src, ଅଚଳ ଅକ୍ଷର *dest)
{
    ସଂରଚନା stat	    st;

    ଯଦି (stat(dest, &st) >= 0)	    // fail if destination exists
	ଫେରନ୍ତୁ -1;
    ଯଦି (link(src, dest) != 0)	    // link file to new name
	ଫେରନ୍ତୁ -1;
    ଯଦି (mch_remove(src) == 0)	    // delete link to old name
	ଫେରନ୍ତୁ 0;
    ଫେରନ୍ତୁ -1;
}
#endif // !HAVE_RENAME

#ଯଦି defined(FEAT_MOUSE_GPM)
# ଯଦି defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
load_libgpm(ଶୂନ୍ୟ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    ଯଦି (libgpm_hinst == ଖାଲି)
    {
	ଯଦି (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	ଫେରନ୍ତୁ FAIL;
    }

    ଯଦି (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ଖାଲି
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ଖାଲି
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ଖାଲି
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ଖାଲି
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ଖାଲି
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ଖାଲି;
	dll_gpm_flag = ଖାଲି;
	dll_gpm_fd   = ଖାଲି;
	ଫେରନ୍ତୁ FAIL;
    }
    ଫେରନ୍ତୁ OK;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gpm_available(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ libgpm_hinst != ଖାଲି || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gpm_open(ଶୂନ୍ୟ)
{
    ସ୍ଥିର Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    ଯଦି (!gpm_available())
	ଫେରନ୍ତୁ 0;
# endif

    ଯଦି (gpm_flag)
	ଫେରନ୍ତୁ 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    ଯଦି (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	ଫେରନ୍ତୁ 1; // succeed
    }
    ଯଦି (gpm_fd == -2)
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
    ଫେରନ୍ତୁ 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
gpm_enabled(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ସ୍ଥିର ଶୂନ୍ୟ
gpm_close(ଶୂନ୍ୟ)
{
    ଯଦି (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_gpm_process(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			button;
    ସ୍ଥିର Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			row,col;
    unsigned ଅକ୍ଷର	buttons_mask;
    unsigned ଅକ୍ଷର	gpm_modifiers;
    ସ୍ଥିର unsigned ଅକ୍ଷର old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ଯଦି (hold_gui_events)
	ଫେରନ୍ତୁ 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    ବାଛ (GPM_BARE_EVENTS(gpm_event.type))
    {
	ସ୍ଥିତି GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    ବାଛ (buttons_mask)
	    {
		ସ୍ଥିତି GPM_B_ବାମ:
		    button = MOUSE_ବାମ;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି GPM_B_ମଧ୍ୟ:
		    button = MOUSE_ମଧ୍ୟ;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି GPM_B_ଡାହାଣ:
		    button = MOUSE_ଡାହାଣ;
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଫେରନ୍ତୁ 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_ଅଙ୍କ_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ଭାଙ୍ଗ;
	ସ୍ଥିତି GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ଭାଙ୍ଗ;
	ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
	    ଫେରନ୍ତୁ 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    ଯଦି (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    ଯଦି (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    ଯଦି (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_ନିବେଶ_buf(string, 6);
    ଫେରନ୍ତୁ 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sysmouse_open(ଶୂନ୍ୟ)
{
    ସଂରଚନା mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.ସଙ୍କେତ = SIGUSR2;
    ଯଦି (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	ଫେରନ୍ତୁ FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    ଫେରନ୍ତୁ OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sysmouse_close(ଶୂନ୍ୟ)
{
    ସଂରଚନା mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.ସଙ୍କେତ = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
sig_sysmouse SIGDEFARG(sigarg)
{
    ସଂରଚନା mouse_info	mouse;
    ସଂରଚନା video_info	video;
    char_u		string[6];
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			row, col;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			button;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			buttons;
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ଯଦି (hold_gui_events)
	ଫେରନ୍ତୁ;
# endif

    mouse.operation = MOUSE_GETINFO;
    ଯଦି (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	ଯଦି (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	ଅନ୍ୟଥା
	{
	    ବାଛ (buttons)
	    {
		ସ୍ଥିତି 0:
		    button = MOUSE_RELEASE;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି 1:
		    button = MOUSE_ବାମ;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି 2:
		    button = MOUSE_ମଧ୍ୟ;
		    ଭାଙ୍ଗ;
		ସ୍ଥିତି 4:
		    button = MOUSE_ଡାହାଣ;
		    ଭାଙ୍ଗ;
		ପୂର୍ବନିର୍ଦ୍ଧାରିତ:
		    ଫେରନ୍ତୁ;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_ନିବେଶ_buf(string, 6);
    }
    ଫେରନ୍ତୁ;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_random(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dev_urandom_state = NOTDONE;

    ଯଦି (dev_urandom_state == FAIL)
	ଫେରନ୍ତୁ FAIL;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    ଯଦି (fd == -1)
	dev_urandom_state = FAIL;
    ଅନ୍ୟଥା ଯଦି (ପଠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    ଅନ୍ୟଥା
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    ଫେରନ୍ତୁ dev_urandom_state;
}

#ଯଦି defined(FEAT_LIBCALL)
ପ୍ରକାରବ୍ୟାଖ୍ୟା char_u * (*STRPROCSTR)(char_u *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା char_u * (*INTPROCSTR)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*STRPROCINT)(char_u *);
ପ୍ରକାରବ୍ୟାଖ୍ୟା ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*INTPROCINT)(ପୂର୍ଣ୍ଣସଂଖ୍ୟା);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		argint,
    char_u	**string_result, // NULL when using number_result
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		*ସଂଖ୍ୟା_result)
{
# ଯଦି defined(USE_DLOPEN)
    ଶୂନ୍ୟ	*hinstLib;
    ଅକ୍ଷର	*dlerr = ଖାଲି;
# ଅନ୍ୟଥା
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval_int = 0;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# ଯଦି defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (ଶୂନ୍ୟ)dlerror();

    hinstLib = dlopen((ଅକ୍ଷର *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    ଯଦି (hinstLib == ଖାଲି)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	ଯଦି (dlerr != ଖାଲି)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# ଅନ୍ୟଥା
    hinstLib = shl_load((ଅଚଳ ଅକ୍ଷର*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    ଯଦି (hinstLib != ଖାଲି)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	ଯଦି (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  ଯଦି defined(USE_DLOPEN)
	    dlerr = ଖାଲି;
#  endif
	    mch_didjmp();
	}
	ଅନ୍ୟଥା
# endif
	{
	    retval_str = ଖାଲି;
	    retval_int = 0;

	    ଯଦି (argstring != ଖାଲି)
	    {
# ଯଦି defined(USE_DLOPEN)
		*(ଶୂନ୍ୟ **)(&ProcAdd) = dlsym(hinstLib, (ଅଚଳ ଅକ୍ଷର *)funcname);
		dlerr = dlerror();
# ଅନ୍ୟଥା
		ଯଦି (shl_findsym(&hinstLib, (ଅଚଳ ଅକ୍ଷର *)funcname,
					TYPE_PROCEDURE, (ଶୂନ୍ୟ *)&ProcAdd) < 0)
		    ProcAdd = ଖାଲି;
# endif
		ଯଦି ((success = (ProcAdd != ଖାଲି
# ଯଦି defined(USE_DLOPEN)
			    && dlerr == ଖାଲି
# endif
			    )))
		{
		    ଯଦି (string_result == ଖାଲି)
			retval_int = ((STRPROCINT)(ଶୂନ୍ୟ *)ProcAdd)(argstring);
		    ଅନ୍ୟଥା
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    ଅନ୍ୟଥା
	    {
# ଯଦି defined(USE_DLOPEN)
		*(ଶୂନ୍ୟ **)(&ProcAddI) = dlsym(hinstLib, (ଅଚଳ ଅକ୍ଷର *)funcname);
		dlerr = dlerror();
# ଅନ୍ୟଥା
		ଯଦି (shl_findsym(&hinstLib, (ଅଚଳ ଅକ୍ଷର *)funcname,
				       TYPE_PROCEDURE, (ଶୂନ୍ୟ *)&ProcAddI) < 0)
		    ProcAddI = ଖାଲି;
# endif
		ଯଦି ((success = (ProcAddI != ଖାଲି
# ଯଦି defined(USE_DLOPEN)
			    && dlerr == ଖାଲି
# endif
			    )))
		{
		    ଯଦି (string_result == ଖାଲି)
			retval_int = ((INTPROCINT)(ଶୂନ୍ୟ *)ProcAddI)(argint);
		    ଅନ୍ୟଥା
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    ଯଦି (string_result == ଖାଲି)
		*ସଂଖ୍ୟା_result = retval_int;
	    ଅନ୍ୟଥା ଯଦି (retval_str != ଖାଲି
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	ଯଦି (lc_signal != 0)
	{
	    ପୂର୍ଣ୍ଣସଂଖ୍ୟା i;

	    // try to find the name of this signal
	    ପାଇଁ (i = 0; signal_info[i].sig != -1; i++)
		ଯଦି (lc_signal == signal_info[i].sig)
		    ଭାଙ୍ଗ;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# ଯଦି defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	ଯଦି (dlerr != ଖାଲି)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(ଶୂନ୍ୟ)dlclose(hinstLib);
# ଅନ୍ୟଥା
	(ଶୂନ୍ୟ)shl_unload(hinstLib);
# endif
    }

    ଯଦି (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
}
#endif

#ଯଦି defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	xterm_trace = -1;	// default: disabled
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    ଶୂନ୍ୟ
setup_term_clip(ଶୂନ୍ୟ)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		z = 0;
    ଅକ୍ଷର	*strp = "";
    Widget	AppShell;

    ଯଦି (!x_connect_to_server())
	ଫେରନ୍ତୁ;

    open_app_context();
    ଯଦି (app_context != ଖାଲି && xterm_Shell == (Widget)0)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*oldhandler)(Display*, XErrorEvent*);
# ଯଦି defined(USING_SETJMP)
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	ଯଦି (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# ଯଦି defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	ଯଦି (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ଖାଲି;
	}
	ଅନ୍ୟଥା
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ଖାଲି, 0, &z, &strp);
	    ଯଦି (xterm_dpy != ଖାଲି)
		xterm_dpy_retry_ଗଣନା = 0;
# ଯଦି defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# ଯଦି defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(ଶୂନ୍ୟ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(ଶୂନ୍ୟ)XSetErrorHandler(oldhandler);

	ଯଦି (xterm_dpy == ଖାଲି)
	{
	    ଯଦି (p_verbose > 0)
		verb_ସନ୍ଦେଶ(_("Opening the X display failed"));
	    ଫେରନ୍ତୁ;
	}

	// Catch terminating error of the X server connection.
	(ଶୂନ୍ୟ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	ଯଦି (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ଖାଲି);
	ଯଦି (AppShell == (Widget)0)
	    ଫେରନ୍ତୁ;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		ଶୀର୍ଷ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ଖାଲି);
	ଯଦି (xterm_Shell == (Widget)0)
	    ଫେରନ୍ତୁ;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	ଯଦି (x11_display == ଖାଲି)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    ଯଦି (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	ଯଦି (x11_window == 0 && (strp = getenv("WINDOWID")) != ଖାଲି)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	ଯଦି (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	ଯଦି (x11_window != 0)
	    xterm_trace = 0;
    }
}

    ଶୂନ୍ୟ
start_xterm_trace(ପୂର୍ଣ୍ଣସଂଖ୍ୟା button)
{
    ଯଦି (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	ଫେରନ୍ତୁ;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    ଶୂନ୍ୟ
stop_xterm_trace(ଶୂନ୍ୟ)
{
    ଯଦି (xterm_trace < 0)
	ଫେରନ୍ତୁ;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
do_xterm_trace(ଶୂନ୍ୟ)
{
    Window		ମୂଳ, child;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ମୂଳ_x, ମୂଳ_y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			win_x, win_y;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    ସ୍ଥିର char_u	*mouse_code = ଖାଲି;
    ସ୍ଥିର size_t	mouse_codelen = 0;
    ସ୍ଥିର char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		prev_row = 0, prev_col = 0;
    ସ୍ଥିର XSizeHints	xterm_hints;

    ଯଦି (xterm_trace <= 0)
	ଫେରନ୍ତୁ FALSE;

    ଯଦି (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	ଯଦି (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    ଫେରନ୍ତୁ FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	ଯଦି (mouse_code != ଖାଲି)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &ମୂଳ, &child, &ମୂଳ_x, &ମୂଳ_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	ଯଦି (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	ଯଦି (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	ଫେରନ୍ତୁ TRUE;
    }

    ଯଦି (mouse_code == ଖାଲି || mouse_codelen > 45)
    {
	xterm_trace = 0;
	ଫେରନ୍ତୁ FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &ମୂଳ, &child, &ମୂଳ_x, &ମୂଳ_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    ଯଦି (row == prev_row && col == prev_col)
	ଫେରନ୍ତୁ TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_ନିବେଶ_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    ଫେରନ୍ତୁ TRUE;
}

# ଯଦି defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    ଶୂନ୍ୟ
clear_xterm_clip(ଶୂନ୍ୟ)
{
    ଯଦି (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    ଯଦି (xterm_dpy != ଖାଲି)
    {
#  ଯଦି 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	ଯଦି (x11_display == xterm_dpy)
	    x11_display = ଖାଲି;
	xterm_dpy = ଖାଲି;
    }
#  ଯଦି 0
    ଯଦି (app_context != (XtAppContext)ଖାଲି)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ଖାଲି;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
clip_update(ଶୂନ୍ୟ)
{
# ifdef FEAT_GUI
    ଯଦି (gui.in_use)
	gui_mch_update();
    ଅନ୍ୟଥା
# endif
    ଯଦି (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ଶୂନ୍ୟ
xterm_update(ଶୂନ୍ୟ)
{
    XEvent event;

    ପାଇଁ (;;)
    {
	Xt_ନିବେଶ_Mask mask = XtAppPending(app_context);

	ଯଦି (mask == 0 || vim_is_ନିବେଶ_buf_full())
	    ଭାଙ୍ଗ;

	ଯଦି (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		ଯଦି (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_ମୂଲ୍ୟ)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	ଅନ୍ୟଥା
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
clip_xterm_own_selection(Clipboard_T *cbd)
{
    ଯଦି (xterm_Shell != (Widget)0)
	ଫେରନ୍ତୁ clip_x11_own_selection(xterm_Shell, cbd);
    ଫେରନ୍ତୁ FAIL;
}

    ଶୂନ୍ୟ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    ଯଦି (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    ଶୂନ୍ୟ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    ଯଦି (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    ଶୂନ୍ୟ
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#ଯଦି defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# ଯଦି defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ସ୍ଥିର ଶୂନ୍ୟ
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_cmod_flags;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    ଯଦି (check_changed_any(FALSE, FALSE))
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
    ଯଦି (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		save_type UNUSED,
    Bool	shutdown,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    ଯଦି (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    ଯଦି (p_verbose > 0)
	verb_ସନ୍ଦେଶ(_("XSMP handling save-yourself request"));

# ଯଦି defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    ଯଦି (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    ଅନ୍ୟଥା
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
    ସ୍ଥିର ଶୂନ୍ୟ
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
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
    ସ୍ଥିର ଶୂନ୍ୟ
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    ଯଦି (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    ଯଦି (opening)
    {
	xsmp_icefd = Ice_Connection_ସଂଖ୍ୟା(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ଖାଲି);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
xsmp_handle_requests(ଶୂନ୍ୟ)
{
    Bool rep;

    ଯଦି (IceProcessMessages(xsmp.iceconn, ଖାଲି, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	ଯଦି (p_verbose > 0)
	    verb_ସନ୍ଦେଶ(_("XSMP lost ICE connection"));
	xsmp_close();
	ଫେରନ୍ତୁ FAIL;
    }
    ଅନ୍ୟଥା
	ଫେରନ୍ତୁ OK;
}

ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା dummy;

// Set up X Session Management Protocol
    ଶୂନ୍ୟ
xsmp_init(ଶୂନ୍ୟ)
{
    ଅକ୍ଷର		errorstring[80];
    SmcCallbacks	smcallbacks;
# ଯଦି 0
    Sm_Prop_ମୂଲ୍ୟ		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    ଯଦି (p_verbose > 0)
	verb_ସନ୍ଦେଶ(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = ଖାଲି;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = ଖାଲି;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = ଖାଲି;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = ଖାଲି;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    ଯଦି (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	ଯଦି (p_verbose > 0)
	    verb_ସନ୍ଦେଶ(_("XSMP ICE connection watch failed"));
	ଫେରନ୍ତୁ;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ଖାଲି,
	    ଖାଲି,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    ଖାଲି,
	    &xsmp.clientid,
	    ଆକାର(errorstring) - 1,
	    errorstring);
    ଯଦି (xsmp.smcconn == ଖାଲି)
    {
	ଯଦି (p_verbose > 0)
	{
	    ଅକ୍ଷର errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[ଆକାର(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, ଆକାର(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_ସନ୍ଦେଶ(errorreport);
	}
	ଫେରନ୍ତୁ;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# ଯଦି 0
    // ID ourselves
    smname.ମୂଲ୍ୟ = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.ଅଙ୍କ_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    ଶୂନ୍ୟ
xsmp_close(ଶୂନ୍ୟ)
{
    ଯଦି (xsmp_icefd == -1)
	ଫେରନ୍ତୁ;

    SmcCloseConnection(xsmp.smcconn, 0, ଖାଲି);
    ଯଦି (xsmp.clientid != ଖାଲି)
	ମୁକ୍ତ(xsmp.clientid);
    xsmp.clientid = ଖାଲି;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#ଯଦି defined(FEAT_RELTIME)
# ଯଦି defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t timeout_flag = FALSE;
ସ୍ଥିର timer_t		     timer_id;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_flag(ସଂଘ sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ଶୂନ୍ୟ
stop_timeout(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ସଂରଚନା itimerspec disarm = {{0, 0}, {0, 0}};

    ଯଦି (timer_created)
    {
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret = timer_settime(timer_id, 0, &disarm, ଖାଲି);

	ଯଦି (ret < 0)
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
    ଅସ୍ଥିର sig_atomic_t *
start_timeout(long msec)
{
    ସଂରଚନା itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    ଯଦି (!timer_created)
    {
	ସଂରଚନା sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	ଯଦି (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    ଫେରନ୍ତୁ &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ଖାଲି, "setting timeout timer to %d sec %ld nsec",
	       (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ଖାଲି);
    ଯଦି (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    ଫେରନ୍ତୁ &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    ଶୂନ୍ୟ
delete_timer(ଶୂନ୍ୟ)
{
    ଯଦି (!timer_created)
	ଫେରନ୍ତୁ;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# ଅନ୍ୟଥା // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ସ୍ଥିର ସଂରଚନା sigaction		prev_sigaction;
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t	timeout_flag	     = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା			timer_active	     = FALSE;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା			timer_handler_active = FALSE;
ସ୍ଥିର ଅସ୍ଥିର sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ସ୍ଥିର ଶୂନ୍ୟ
set_flag SIGDEFARG(sigarg)
{
    ଯଦି (alarm_pending)
	alarm_pending = FALSE;
    ଅନ୍ୟଥା
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ଶୂନ୍ୟ
stop_timeout(ଶୂନ୍ୟ)
{
    ସ୍ଥିର ସଂରଚନା itimerval disarm = {{0, 0}, {0, 0}};
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    ret;

    ଯଦି (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ଖାଲି);
	ଯଦି (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    ଯଦି (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ଖାଲି);
	ଯଦି (ret < 0)
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
    ଅସ୍ଥିର sig_atomic_t *
start_timeout(long msec)
{
    ସଂରଚନା itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ସଂରଚନା sigaction	handle_alarm;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			ret;
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
    ଯଦି (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ଖାଲି);
    }
    ଯଦି (unlikely(ret != 0 || alarm_pending < 0))
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
    ଯଦି (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	ଫେରନ୍ତୁ &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ଖାଲି);
    ଯଦି (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	ଫେରନ୍ତୁ &timeout_flag;
    }

    timer_active = TRUE;
    ଫେରନ୍ତୁ &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
