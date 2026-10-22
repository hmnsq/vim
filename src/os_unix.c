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

#అన్తర్భావ "vim.h"

#నిర్వచితమైతే FEAT_MZSCHEME
# include "if_mzsch.h"
#అయితే_ముగింపు

#అన్తర్భావ "os_unixx.h"	    // unix includes for os_unix.c only

#నిర్వచితమైతే USE_XSMP
# include <X11/SM/SMlib.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SELINUX
# include <selinux/selinux.h>
స్థిర పూర్ణాంకం selinux_enabled = -1;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XATTR
# include <sys/xattr.h>
#అయితే_ముగింపు

#నిర్వచితమైతే HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_సగటు
#  include <windows.h>
#  include "winclip.pro"
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_GPM

# include <gpm.h>

# ifdef DYNAMIC_GPM
#  define Gpm_Open     (*dll_Gpm_Open)
#  define Gpm_Close    (*dll_Gpm_Close)
#  define Gpm_GetEvent (*dll_Gpm_GetEvent)
#  define gpm_flag     (dll_gpm_flag != ఖాళీ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ఖాళీ ? *dll_gpm_fd   : -1)

స్థిర పూర్ణాంకం (*dll_Gpm_Open)     (Gpm_Connect *, పూర్ణాంకం);
స్థిర పూర్ణాంకం (*dll_Gpm_Close)    (శూన్య);
స్థిర పూర్ణాంకం (*dll_Gpm_GetEvent) (Gpm_Event *);
స్థిర పూర్ణాంకం *dll_gpm_flag;
స్థిర పూర్ణాంకం *dll_gpm_fd;

స్థిర శూన్య *libgpm_hinst;
# endif

// <linux/keyboard.h> contains defines conflicting with "keymap.h",
// I just copied relevant defines here. A cleaner solution would be to put gpm
// code into separate file and include there linux/keyboard.h
// #include <linux/keyboard.h>
# define KG_జరుపు	0
# define KG_CTRL	2
# define KG_ALT		3
# define KG_ALTGR	1
# define KG_SHIFTL	4
# define KG_SHIFTR	5
# define KG_CTRLL	6
# define KG_CTRLR	7
# define KG_CAPSSHIFT	8

స్థిర శూన్య gpm_close(శూన్య);
స్థిర పూర్ణాంకం gpm_open(శూన్య);
స్థిర పూర్ణాంకం mch_gpm_process(శూన్య);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

స్థిర పూర్ణాంకం sysmouse_open(శూన్య);
స్థిర శూన్య sysmouse_close(శూన్య);
స్థిర శూన్య sig_sysmouse SIGPROTOARG;
#అయితే_ముగింపు

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#ఒకవేళ defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
#  include <X11/Shell.h>
#  include <X11/StringDefs.h>
స్థిర Widget	xterm_Shell = (Widget)0;
స్థిర శూన్య clip_update(శూన్య);
# endif

కిటికీ	    x11_కిటికీ = 0;
Display	    *x11_display = ఖాళీ;
#అయితే_ముగింపు

స్థిర పూర్ణాంకం ignore_sigtstp = FALSE;

స్థిర పూర్ణాంకం get_x11_title(పూర్ణాంకం);

స్థిర char_u	*oldtitle = ఖాళీ;
స్థిర అస్థిర sig_atomic_t oldtitle_outdated = FALSE;
స్థిర పూర్ణాంకం	unix_did_సమితి_title = FALSE;
స్థిర char_u	*oldicon = ఖాళీ;
స్థిర పూర్ణాంకం	did_సమితి_icon = FALSE;

స్థిర శూన్య may_core_dump(శూన్య);

రకనిర్వచనం పూర్ణాంకం waitstatus;
స్థిర పూర్ణాంకం  WaitForChar(దీర్ఘ msec, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం);
స్థిర పూర్ణాంకం  WaitForCharOrMouse(దీర్ఘ msec, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం);
#నిర్వచితమైతే VMS
పూర్ణాంకం  RealWaitForChar(పూర్ణాంకం, దీర్ఘ, పూర్ణాంకం *, పూర్ణాంకం *interrupted);
#లేకపోతే
స్థిర పూర్ణాంకం  RealWaitForChar(పూర్ణాంకం, దీర్ఘ, పూర్ణాంకం *, పూర్ణాంకం *interrupted);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_XCLIPBOARD
స్థిర పూర్ణాంకం do_xterm_trace(శూన్య);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#అయితే_ముగింపు

స్థిర శూన్య handle_resize(శూన్య);

#ఒకవేళ defined(SIGWINCH)
స్థిర శూన్య sig_winch SIGPROTOARG;
#అయితే_ముగింపు
#ఒకవేళ defined(SIGTSTP)
స్థిర శూన్య sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
స్థిర అస్థిర sig_atomic_t లోపల_mch_suspend = FALSE;
#అయితే_ముగింపు
#ఒకవేళ defined(SIGINT)
స్థిర శూన్య catch_sigint SIGPROTOARG;
#అయితే_ముగింపు
#ఒకవేళ defined(SIGUSR1)
స్థిర శూన్య catch_sigusr1 SIGPROTOARG;
#అయితే_ముగింపు
#ఒకవేళ defined(SIGPWR)
స్థిర శూన్య catch_sigpwr SIGPROTOARG;
#అయితే_ముగింపు
#ఒకవేళ defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define సమితి_SIG_ALARM
స్థిర శూన్య sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
స్థిర అస్థిర sig_atomic_t sig_alarm_called;
#అయితే_ముగింపు
స్థిర శూన్య deathtrap SIGPROTOARG;

స్థిర శూన్య catch_int_signal(శూన్య);
స్థిర శూన్య సమితి_సంకేతాలు(శూన్య);
స్థిర శూన్య catch_సంకేతాలు(శూన్య (*func_deadly)(పూర్ణాంకం), శూన్య (*func_other)(పూర్ణాంకం));
#నిర్వచితమైతే HAVE_SIGPROCMASK
# define SIGSET_DECL(సమితి_2)	sigset_t సమితి_2;
# define ఖండం_సంకేతాలు(సమితి_2)	ఖండం_సంకేతాలు_2(సమితి_2)
# define UNBLOCK_సంకేతాలు(సమితి_2)	unblock_సంకేతాలు(సమితి_2)
#లేకపోతే
# define SIGSET_DECL(సమితి_2)
# define ఖండం_సంకేతాలు(సమితి_2)	చేయి { /**/ } అయితే (0)
# define UNBLOCK_సంకేతాలు(సమితి_2)	చేయి { /**/ } అయితే (0)
#అయితే_ముగింపు
స్థిర పూర్ణాంకం  have_wildcard(పూర్ణాంకం, char_u **);
స్థిర పూర్ణాంకం  have_dollars(పూర్ణాంకం, char_u **);

స్థిర పూర్ణాంకం save_patterns(పూర్ణాంకం అంకె_pat, char_u **pat, పూర్ణాంకం *అంకె_file, char_u ***file);

#నిర్వచితం_కాకపోతే SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#అయితే_ముగింపు
#నిర్వచితం_కాకపోతే SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#అయితే_ముగింపు

// volatile because it is used in signal handler sig_winch().
స్థిర అస్థిర sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
స్థిర అస్థిర sig_atomic_t got_tstp = FALSE;
స్థిర char_u	*extra_shell_arg = ఖాళీ;
స్థిర పూర్ణాంకం	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
స్థిర అస్థిర sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
స్థిర అస్థిర sig_atomic_t లోపల_mch_delay = FALSE; // sleeping in mch_delay()

#ఒకవేళ defined(FEAT_పని_CHANNEL) && !defined(USE_SYSTEM)
స్థిర పూర్ణాంకం dont_check_పని_ended = 0;
#అయితే_ముగింపు

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
స్థిర tmode_T mch_cur_tmode = TMODE_COOK;

#నిర్వచితమైతే USE_XSMP
రకనిర్వచనం నిర్మాణం
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    అక్షరం *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

స్థిర xsmp_config_T xsmp;
#అయితే_ముగింపు

#నిర్వచితమైతే SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#అయితే_ముగింపు

స్థిర నిర్మాణం signalinfo
{
    పూర్ణాంకం	    sig;	// Signal number, eg. SIGSEGV etc
    అక్షరం    *name;	// Signal name (not char_u!).
    అక్షరం    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#నిర్వచితమైతే SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGILL
    {SIGILL,	    "ILL",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#అయితే_ముగింపు
#ఒకవేళ defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#అయితే_ముగింపు
#నిర్వచితమైతే SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#అయితే_ముగింపు
#ఒకవేళ defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#అయితే_ముగింపు
#ఒకవేళ defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#అయితే_ముగింపు
#ఒకవేళ defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGINT
    {SIGINT,	    "INT",	FALSE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#అయితే_ముగింపు
#నిర్వచితమైతే SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#అయితే_ముగింపు
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(పూర్ణాంకం sig, sighandler_T func)
{
#ఒకవేళ defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    నిర్మాణం sigaction	sa, old;
    sigset_t		curset;
    పూర్ణాంకం			blocked;

    ఒకవేళ (sigprocmask(SIG_BLOCK, ఖాళీ, &curset) == -1)
	తిరిగి SIG_ERR;

    blocked = sigismember(&curset, sig);

    ఒకవేళ (func == SIG_HOLD)
    {
	ఒకవేళ (blocked)
	    తిరిగి SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ఒకవేళ (sigaction(sig, ఖాళీ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ఖాళీ) == -1)
	    తిరిగి SIG_ERR;
	తిరిగి old.sa_handler;
    }

    ఒకవేళ (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ఒకవేళ (sigprocmask(SIG_UNBLOCK, &curset, ఖాళీ) == -1)
	    తిరిగి SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# లేకపోతే
    sa.sa_flags = 0;
# endif
    ఒకవేళ (sigaction(sig, &sa, &old) == -1)
	తిరిగి SIG_ERR;
    తిరిగి blocked ? SIG_HOLD: old.sa_handler;
#లేదావేళ defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    తిరిగి sigset(sig, func);
#లేకపోతే
    // Oldest and most compatible solution.
    తిరిగి సంకేతం(sig, func);
#అయితే_ముగింపు
}

    పూర్ణాంకం
mch_chdir(అక్షరం *మార్గం)
{
    ఒకవేళ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", మార్గం);
	verbose_leave();
    }
#నిర్వచితమైతే VMS
    తిరిగి chdir(vms_fixfilename(మార్గం));
#లేకపోతే
    తిరిగి chdir(మార్గం);
#అయితే_ముగింపు
}

#ఒకవేళ defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#అయితే_ముగింపు

/*
 * Write s[len] to the screen (stdout).
 */
    శూన్య
mch_write(char_u *s, పూర్ణాంకం len)
{
    vim_ignored = (పూర్ణాంకం)లిఖ(1, (అక్షరం *)s, len);
    ఒకవేళ (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ఖాళీ, ఖాళీ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    స్థిర పూర్ణాంకం
resize_func(పూర్ణాంకం check_only)
{
    ఒకవేళ (check_only)
	తిరిగి do_resize;
    అయితే (do_resize)
    {
#నిర్వచితమైతే FEAT_EVAL
	ch_log(ఖాళీ, "calling handle_resize() in resize_func()");
#అయితే_ముగింపు
	handle_resize();
    }
    తిరిగి FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    పూర్ణాంకం
mch_inchar(
    char_u	*buf,
    పూర్ణాంకం		maxlen,
    దీర్ఘ	wtime,	    // don't use "time", MIPS cannot handle it
    పూర్ణాంకం		tb_change_cnt)
{
    తిరిగి inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    స్థిర శూన్య
handle_resize(శూన్య)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    పూర్ణాంకం
mch_char_avail(శూన్య)
{
    తిరిగి WaitForChar(0L, ఖాళీ, FALSE);
}

#ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    పూర్ణాంకం
mch_check_messages(శూన్య)
{
    తిరిగి WaitForChar(0L, ఖాళీ, TRUE);
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_మొత్తం_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# ఒకవేళ defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# ఒకవేళ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_మొత్తం_mem(పూర్ణాంకం special వాడని)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_పోర్టు_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	నిర్మాణం vm_statistics64	vm_stat;
	natural_t		లెక్క = HOST_VM_INFO64_లెక్క;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &లెక్క);
#  లేకపోతే
	నిర్మాణం vm_statistics	vm_stat;
	natural_t		లెక్క = HOST_VM_INFO_లెక్క;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &లెక్క);
#  endif
	ఒకవేళ (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_లెక్క + vm_stat.చురుకైన_లెక్క
					    + vm_stat.inactive_లెక్క
#  ifdef MAC_OS_X_సంస్కరణ_10_9
					    + vm_stat.compressor_పుట_లెక్క
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_పోర్టు_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    ఒకవేళ (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	పూర్ణాంకం		mib[2];
	size_t		len = పరిమాణం(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  లేకపోతే
	// sysctl() may return 32 bit or 64 bit, accept both
	సంఘం {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  లేకపోతే
	mib[1] = HW_USERMEM;
#  endif
	ఒకవేళ (sysctl(mib, 2, &physmem, &len, ఖాళీ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  లేకపోతే
	    ఒకవేళ (len == పరిమాణం(physmem.u64))
		mem = (long_u)physmem.u64;
	    లేకపోతే
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# ఒకవేళ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    ఒకవేళ (mem == 0)
    {
	నిర్మాణం sysinfo sinfo;

	// Linux way of getting amount of RAM available
	ఒకవేళ (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    అయితే (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  లేకపోతే
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    ఒకవేళ (mem == 0)
    {
	దీర్ఘ	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	ఒకవేళ (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    అయితే (shiftright > 0 && (pagesize & 1) == 0)
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
# ఒకవేళ defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	నిర్మాణం rlimit	rlp;

	ఒకవేళ (getrlimit(RLIMIT_దత్తాంశం, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (పరిమాణం(long_u) * 8 - 1))
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

    ఒకవేళ (mem > 0)
	తిరిగి mem >> shiftright;
    తిరిగి (long_u)0x1fffff;
}
#అయితే_ముగింపు

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    శూన్య
mch_delay(దీర్ఘ msec, పూర్ణాంకం flags)
{
    tmode_T	old_tmode;
    పూర్ణాంకం		call_settmode;
#నిర్వచితమైతే FEAT_MZSCHEME
    దీర్ఘ	మొత్తం = msec; // remember original value
#అయితే_ముగింపు

    ఒకవేళ (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	లోపల_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	ఒకవేళ (call_settmode)
	{
	    old_tmode = mch_cur_tmode;
	    settmode(TMODE_SLEEP);
	}

	/*
	 * Everybody sleeps in a different way...
	 * Prefer nanosleep(), some versions of usleep() can only sleep up to
	 * one second.
	 */
#నిర్వచితమైతే FEAT_MZSCHEME
	చేయి
	{
	    // if total is large enough, wait by portions in p_mzq
	    ఒకవేళ (మొత్తం > p_mzq)
		msec = p_mzq;
	    లేకపోతే
		msec = మొత్తం;
	    మొత్తం -= msec;
#అయితే_ముగింపు
#నిర్వచితమైతే HAVE_NANOSLEEP
	{
	    నిర్మాణం timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (శూన్య)nanosleep(&ts, ఖాళీ);
	}
#లేకపోతే
# ifdef HAVE_USLEEP
	అయితే (msec >= 1000)
	{
	    usleep((గుర్తులేని పూర్ణాంకం)(999 * 1000));
	    msec -= 999;
	}
	usleep((గుర్తులేని పూర్ణాంకం)(msec * 1000));
# లేకపోతే
#  ifndef HAVE_SELECT
	poll(ఖాళీ, 0, (పూర్ణాంకం)msec);
#  లేకపోతే
	{
	    నిర్మాణం timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ఖాళీ, ఖాళీ, ఖాళీ, &tv);
	}
#  endif
# endif
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MZSCHEME
	}
	అయితే (మొత్తం > 0);
#అయితే_ముగింపు

	ఒకవేళ (call_settmode)
	    settmode(old_tmode);
	లోపల_mch_delay = FALSE;
    }
    లేకపోతే
	WaitForChar(msec, ఖాళీ, FALSE);
}

#ఒకవేళ defined(HAVE_దొంతర_హద్దు) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_దొంతర_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
స్థిర పూర్ణాంకం దొంతర_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    స్థిర శూన్య
check_దొంతర_growth(అక్షరం *p)
{
    పూర్ణాంకం		i;

    దొంతర_grows_downwards = (p > (అక్షరం *)&i);
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_దొంతర_హద్దు)
స్థిర అక్షరం *దొంతర_హద్దు = ఖాళీ;

# ఒకవేళ defined(THREAD_సురక్షిత) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    స్థిర శూన్య
get_దొంతర_హద్దు(శూన్య)
{
    నిర్మాణం rlimit	rlp;
    పూర్ణాంకం			i;
    దీర్ఘ		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    ఒకవేళ (getrlimit(RLIMIT_దొంతర, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (పరిమాణం(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (దీర్ఘ)rlp.rlim_cur;
# ఒకవేళ defined(THREAD_సురక్షిత) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    కొలత;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    ఒకవేళ (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &కొలత);
		ఒకవేళ (lim > (దీర్ఘ)కొలత)
		    lim = (దీర్ఘ)కొలత;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	ఒకవేళ (దొంతర_grows_downwards)
	{
	    దొంతర_హద్దు = (అక్షరం *)((దీర్ఘ)&i - (lim / 16L * 15L));
	    ఒకవేళ (దొంతర_హద్దు >= (అక్షరం *)&i)
		// overflow, set to 1/16 of current stack position
		దొంతర_హద్దు = (అక్షరం *)((దీర్ఘ)&i / 16L);
	}
	లేకపోతే
	{
	    దొంతర_హద్దు = (అక్షరం *)((దీర్ఘ)&i + (lim / 16L * 15L));
	    ఒకవేళ (దొంతర_హద్దు <= (అక్షరం *)&i)
		దొంతర_హద్దు = ఖాళీ;	//అధికప్రవాహం
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    పూర్ణాంకం
mch_stackcheck(అక్షరం *p)
{
    ఒకవేళ (దొంతర_హద్దు == ఖాళీ)
	తిరిగి OK;

    ఒకవేళ (దొంతర_grows_downwards)
    {
	ఒకవేళ (p < దొంతర_హద్దు)
	    తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (p > దొంతర_హద్దు)
	తిరిగి FAIL;
    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
స్థిర stack_t sigstk;			// for sigaltstack()
# లేకపోతే
స్థిర నిర్మాణం sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
స్థిర దీర్ఘ పూర్ణాంకం get_signal_దొంతర_కొలత(శూన్య)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    దీర్ఘ పూర్ణాంకం కొలత = -1;

    // return size only if sysconf doesn't return an error
    ఒకవేళ ((కొలత = sysconf(_SC_SIGSTKSZ)) > -1)
	తిరిగి కొలత;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    తిరిగి SIGSTKSZ;
# endif

    // otherwise guess the size
    తిరిగి 8000;
}

స్థిర అక్షరం *signal_దొంతర;

    స్థిర శూన్య
init_signal_దొంతర(శూన్య)
{
    ఒకవేళ (signal_దొంతర == ఖాళీ)
	తిరిగి;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_పునాది
    sigstk.ss_పునాది = signal_దొంతర;
#  లేకపోతే
    sigstk.ss_sp = signal_దొంతర;
#  endif
    sigstk.ss_size = get_signal_దొంతర_కొలత();
    sigstk.ss_flags = 0;
    (శూన్య)sigaltstack(&sigstk, ఖాళీ);
# లేకపోతే
    sigstk.ss_sp = signal_దొంతర;
    ఒకవేళ (దొంతర_grows_downwards)
	sigstk.ss_sp += get_signal_దొంతర_కొలత() - 1;
    sigstk.ss_onstack = 0;
    (శూన్య)sigstack(&sigstk, ఖాళీ);
# endif
}
#అయితే_ముగింపు

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#ఒకవేళ defined(SIGWINCH)
    స్థిర శూన్య
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(SIGTSTP)
    స్థిర శూన్య
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    ఒకవేళ (లోపల_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	లేపు(sigarg);
    }
    లేకపోతే
	got_tstp = TRUE;

# ఒకవేళ !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#అయితే_ముగింపు

#ఒకవేళ defined(SIGINT)
    స్థిర శూన్య
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(SIGUSR1)
    స్థిర శూన్య
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(SIGPWR)
    స్థిర శూన్య
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
#అయితే_ముగింపు

#నిర్వచితమైతే సమితి_SIG_ALARM
/*
 * signal function for alarm().
 */
    స్థిర శూన్య
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
స్థిర JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
స్థిర అస్థిర sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
స్థిర అస్థిర sig_atomic_t lc_చురుకైన = FALSE;

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
    స్థిర శూన్య
mch_startjmp(శూన్య)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_చురుకైన = TRUE;
}

    స్థిర శూన్య
mch_endjmp(శూన్య)
{
    lc_చురుకైన = FALSE;
}

    స్థిర శూన్య
mch_didjmp(శూన్య)
{
# ఒకవేళ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_దొంతర();
# endif
}
#అయితే_ముగింపు

/*
 * This function handles deadly signals.
 * It tries to preserve any swap files and exit properly.
 * (partly from Elvis).
 * NOTE: Avoid unsafe functions, such as allocating memory, they can result in
 * a deadlock.
 */
    స్థిర శూన్య
deathtrap SIGDEFARG(sigarg)
{
    స్థిర పూర్ణాంకం	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#నిర్వచితమైతే SIGHASARG
    పూర్ణాంకం		i;
#అయితే_ముగింపు

#ఒకవేళ defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    ఒకవేళ (lc_చురుకైన)
    {
# ఒకవేళ defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_చురుకైన = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#అయితే_ముగింపు

#నిర్వచితమైతే SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    ఒకవేళ (లోపల_mch_delay && sigarg == SIGQUIT)
	తిరిగి;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    ఒకవేళ (entered == 0
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
	తిరిగి;
#అయితే_ముగింపు

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    ఖండం_autocmds();

#నిర్వచితమైతే FEAT_EVAL
    // Set the v:dying variable.
    సమితి_vim_var_nr(VV_DYING, (దీర్ఘ)entered);
#అయితే_ముగింపు
    v_dying = entered;

#నిర్వచితమైతే HAVE_దొంతర_హద్దు
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_దొంతర_హద్దు();
#అయితే_ముగింపు

#ఒకవేళ 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	ఒకవేళ (fp)
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
#అయితే_ముగింపు

#నిర్వచితమైతే SIGHASARG
    // try to find the name of this signal
    కొరకు (i = 0; signal_info[i].sig != -1; i++)
	ఒకవేళ (sigarg == signal_info[i].sig)
	    ఆపు;
    deadly_signal = sigarg;
#అయితే_ముగింపు

    నిండిన_screen = FALSE;	// don't write message to the GUI, it might be
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
    ఒకవేళ (entered >= 3)
    {
	reset_సంకేతాలు();	// don't catch any signals anymore
	may_core_dump();
	ఒకవేళ (entered >= 4)
	    _exit(8);
	నిష్క్రమించు(7);
    }
    ఒకవేళ (entered == 2)
    {
	// No translation, it may call malloc().
	బహిర్గతం_STR("Vim: Double signal, exiting\n");
	బహిర్గతం_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#నిర్వచితమైతే SIGHASARG
    sprintf((అక్షరం *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#లేకపోతే
    sprintf((అక్షరం *)IObuff, "Vim: Caught deadly signal\r\n");
#అయితే_ముగింపు

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#నిర్వచితమైతే NBDEBUG
    reset_సంకేతాలు();
    may_core_dump();
    abort();
#అయితే_ముగింపు
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    స్థిర శూన్య
after_sigcont(శూన్య)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#ఒకవేళ defined(SIGCONT)
స్థిర శూన్య sigcont_handler SIGPROTOARG;

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
స్థిర అస్థిర sig_atomic_t sigcont_received;
స్థిర శూన్య sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    స్థిర శూన్య
sigcont_handler SIGDEFARG(sigarg)
{
    ఒకవేళ (లోపల_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    లేకపోతే
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	బహిర్గతం_flush();
    }
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_CLIPBOARD)
# ఒకవేళ defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
స్థిర శూన్య *clip_నక్షత్రం_save = ఖాళీ;
స్థిర శూన్య *clip_plus_save = ఖాళీ;
# endif

# ఒకవేళ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    స్థిర శూన్య
loose_clipboard(శూన్య)
{
    ఒకవేళ (clip_నక్షత్రం.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	ఒకవేళ (clip_నక్షత్రం.owned)
	    clip_lose_selection(&clip_నక్షత్రం);
	ఒకవేళ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	ఒకవేళ (x11_display != ఖాళీ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# ఒకవేళ defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    స్థిర శూన్య
save_clipboard(శూన్య)
{
    ఒకవేళ (clip_నక్షత్రం.owned)
	clip_నక్షత్రం_save = get_రిజిస్టరు('*', TRUE);
    ఒకవేళ (clip_plus.owned)
	clip_plus_save = get_రిజిస్టరు('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    స్థిర శూన్య
restore_clipboard(శూన్య)
{
    ఒకవేళ (clip_నక్షత్రం_save != ఖాళీ)
    {
	ఒకవేళ (!clip_gen_owner_exists(&clip_నక్షత్రం))
	    put_రిజిస్టరు('*', clip_నక్షత్రం_save);
	లేకపోతే
	    free_రిజిస్టరు(clip_నక్షత్రం_save);
	clip_నక్షత్రం_save = ఖాళీ;
    }
    ఒకవేళ (clip_plus_save != ఖాళీ)
    {
	ఒకవేళ (!clip_gen_owner_exists(&clip_plus))
	    put_రిజిస్టరు('+', clip_plus_save);
	లేకపోతే
	    free_రిజిస్టరు(clip_plus_save);
	clip_plus_save = ఖాళీ;
    }
}
# endif
#అయితే_ముగింపు

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    శూన్య
mch_suspend(శూన్య)
{
    ఒకవేళ (ignore_sigtstp)
	తిరిగి;

#ఒకవేళ defined(SIGTSTP)
    లోపల_mch_suspend = TRUE;

    బహిర్గతం_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    బహిర్గతం_flush();	    // needed to disable mouse on some systems

# ఒకవేళ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# ఒకవేళ defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# ఒకవేళ defined(SIGCONT)
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
	దీర్ఘ wait_సమయం;

	కొరకు (wait_సమయం = 0; !sigcont_received && wait_సమయం <= 3L; wait_సమయం++)
	    mch_delay(wait_సమయం, 0);
    }
# endif
    లోపల_mch_suspend = FALSE;

    after_sigcont();
#లేకపోతే
    suspend_shell();
#అయితే_ముగింపు
}

    శూన్య
mch_init(శూన్య)
{
    Columns = 80;
    అడ్డువరుసలు = 24;

    బహిర్గతం_flush();

#నిర్వచితమైతే SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#అయితే_ముగింపు
    సమితి_సంకేతాలు();

#నిర్వచితమైతే MACOS_CONVERT
    mac_conv_init();
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#అయితే_ముగింపు
}

    శూన్య
సమితి_sigwinch_handler(శూన్య)
{
#ఒకవేళ defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#అయితే_ముగింపు
}

    స్థిర శూన్య
సమితి_సంకేతాలు(శూన్య)
{
#ఒకవేళ defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#అయితే_ముగింపు

#నిర్వచితమైతే SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.లోపల_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#అయితే_ముగింపు
#ఒకవేళ defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#అయితే_ముగింపు
#నిర్వచితమైతే SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#అయితే_ముగింపు

#నిర్వచితమైతే SIGINT
    catch_int_signal();
#అయితే_ముగింపు

#నిర్వచితమైతే SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#అయితే_ముగింపు

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#నిర్వచితమైతే SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#అయితే_ముగింపు

#నిర్వచితమైతే SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#అయితే_ముగింపు

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_సంకేతాలు(deathtrap, SIG_ERR);

#ఒకవేళ defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    ఒకవేళ (gui.లోపల_use)
	mch_signal(SIGHUP, SIG_IGN);
#అయితే_ముగింపు
}

#ఒకవేళ defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    స్థిర శూన్య
catch_int_signal(శూన్య)
{
    mch_signal(SIGINT, catch_sigint);
}
#అయితే_ముగింపు

    శూన్య
reset_సంకేతాలు(శూన్య)
{
    catch_సంకేతాలు(SIG_DFL, SIG_DFL);
#ఒకవేళ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#అయితే_ముగింపు
}

    స్థిర శూన్య
catch_సంకేతాలు(
    శూన్య (*func_deadly)(పూర్ణాంకం),
    శూన్య (*func_other)(పూర్ణాంకం))
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; signal_info[i].sig != -1; i++)
    {
	ఒకవేళ (signal_info[i].deadly)
	{
#ఒకవేళ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    నిర్మాణం sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# ఒకవేళ defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# లేకపోతే
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ఖాళీ);
#లేకపోతే
# ఒకవేళ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    నిర్మాణం sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_ముసుగు = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ఖాళీ);
# లేకపోతే
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#అయితే_ముగింపు
	}
	లేకపోతే ఒకవేళ (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#నిర్వచితమైతే SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#లేకపోతే
	    mch_signal(signal_info[i].sig, func_other);
#అయితే_ముగింపు
	}
    }
}

#నిర్వచితమైతే HAVE_SIGPROCMASK
    స్థిర శూన్య
ఖండం_సంకేతాలు_2(sigset_t *సమితి_2)
{
    sigset_t	newset;
    పూర్ణాంకం		i;

    sigemptyset(&newset);

    కొరకు (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# ఒకవేళ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, సమితి_2);
}

    స్థిర శూన్య
unblock_సంకేతాలు(sigset_t *సమితి_2)
{
    sigprocmask(SIG_SETMASK, సమితి_2, ఖాళీ);
}
#అయితే_ముగింపు

/*
 * Handling of SIGHUP, SIGQUIT and SIGTERM:
 * "when" == a signal:       when busy, postpone and return FALSE, otherwise
 *			     return TRUE
 * "when" == SIGNAL_BLOCK:   Going to be busy, block signals
 * "when" == SIGNAL_UNBLOCK: Going to wait, unblock signals, use postponed
 *			     signal
 * Returns TRUE when Vim should exit.
 */
    పూర్ణాంకం
vim_handle_signal(పూర్ణాంకం sig)
{
    స్థిర పూర్ణాంకం got_signal = 0;
    స్థిర పూర్ణాంకం blocked = TRUE;

    ఎంపిక (sig)
    {
	స్థితి SIGNAL_ఖండం:   blocked = TRUE;
			     ఆపు;

	స్థితి SIGNAL_UNBLOCK: blocked = FALSE;
			     ఒకవేళ (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ఆపు;

	అప్రమేయ:	     ఒకవేళ (!blocked)
				 తిరిగి TRUE;	// exit!
			     got_signal = sig;
#నిర్వచితమైతే SIGPWR
			     ఒకవేళ (sig != SIGPWR)
#అయితే_ముగింపు
				 got_int = TRUE;    // break any loops
			     ఆపు;
    }
    తిరిగి FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    పూర్ణాంకం
mch_check_win(పూర్ణాంకం argc వాడని, అక్షరం **argv వాడని)
{
    ఒకవేళ (isatty(1))
	తిరిగి OK;
    తిరిగి FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    పూర్ణాంకం
mch_ఆదానం_isatty(శూన్య)
{
    ఒకవేళ (isatty(read_cmd_fd))
	తిరిగి TRUE;
    తిరిగి FALSE;
}

#నిర్వచితమైతే FEAT_X11

# ఒకవేళ defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    స్థిర శూన్య
xopen_సందేశం(దీర్ఘ elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

స్థిర పూర్ణాంకం	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    స్థిర పూర్ణాంకం
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGet_Error_పాఠ్యం(dpy, error_event->error_సంకేతం, (అక్షరం *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
# endif
	సందేశం((అక్షరం *)IObuff);

    తిరిగి 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    స్థిర పూర్ణాంకం
x_error_check(Display *dpy వాడని, XErrorEvent *error_event వాడని)
{
    got_x_error = TRUE;
    తిరిగి 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    స్థిర పూర్ణాంకం
x_connect_గమ్యం_server(శూన్య)
{
    // No point in connecting if we are exiting or dying.
    ఒకవేళ (exiting || v_dying)
	తిరిగి FALSE;

# ఒకవేళ defined(FEAT_CLIENTSERVER)
    ఒకవేళ (x_force_connect)
	తిరిగి TRUE;
# endif
    ఒకవేళ (x_no_connect)
	తిరిగి FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    ఒకవేళ (clip_exclude_prog != ఖాళీ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	ఒకవేళ (regprog_లోపల_use(clip_exclude_prog))
	    తిరిగి FALSE;

	ఒకవేళ (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    తిరిగి FALSE;
    }
    తిరిగి TRUE;
}

# ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  ఒకవేళ defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    స్థిర పూర్ణాంకం
x_IOerror_check(Display *dpy వాడని)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   ఒకవేళ defined(VMS) || defined(__CYGWIN__)
    తిరిగి 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
స్థిర పూర్ణాంకం xterm_dpy_retry_లెక్క = 0;

    స్థిర పూర్ణాంకం
x_IOerror_handler(Display *dpy వాడని)
{
    xterm_dpy = ఖాళీ;
    xterm_dpy_retry_లెక్క = 5;  // Try reconnecting five times
    x11_కిటికీ = 0;
    x11_display = ఖాళీ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  ఒకవేళ defined(VMS) || defined(__CYGWIN__)
    తిరిగి 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    స్థిర శూన్య
may_restore_x11_clipboard(శూన్య)
{
    // No point in restoring the connecting if we are exiting or dying.
    ఒకవేళ (!exiting && !v_dying && xterm_dpy_retry_లెక్క > 0)
    {
	--xterm_dpy_retry_లెక్క;

#  ifndef LESSTIF_సంస్కరణ
	// This has been reported to avoid Vim getting stuck.
	ఒకవేళ (app_context != (XtAppContext)ఖాళీ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ఖాళీ;
	    x11_display = ఖాళీ; // freed by XtDestroyApplicationContext()
	}
#  endif

	అమరిక_పదం_clip();
	get_x11_title(FALSE);
    }
}

    శూన్య
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    ఒకవేళ (eap->arg != ఖాళీ && (arglen = STRLEN(eap->arg)) > 0)
    {
	ఒకవేళ (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (అక్షరం *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ఖాళీ
		    ? (అక్షరం *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_కిటికీ = 0;
    xterm_dpy_retry_లెక్క = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    ఎంచుకో_clipmethod();
}
# endif

/*
 * Test if "dpy" and x11_window are valid by getting the window title.
 * I don't actually want it yet, so there may be a simpler call to use, but
 * this will cause the error handler x_error_check() to be called if anything
 * is wrong, such as the window pointer being invalid (as can happen when the
 * user changes his DISPLAY, but not his WINDOWID) -- webb
 */
    స్థిర పూర్ణాంకం
test_x11_కిటికీ(Display *dpy)
{
    పూర్ణాంకం			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	పాఠ్యం_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    ఒకవేళ (XGetWMName(dpy, x11_కిటికీ, &పాఠ్యం_prop))
	XFree((శూన్య *)పాఠ్యం_prop.విలువ);
    XSync(dpy, False);
    (శూన్య)XSetErrorHandler(old_handler);

    ఒకవేళ (p_verbose > 0 && got_x_error)
	verb_సందేశం(_("Testing the X display failed"));

    తిరిగి (got_x_error ? FAIL : OK);
}
#అయితే_ముగింపు


#నిర్వచితమైతే FEAT_X11

స్థిర పూర్ణాంకం get_x11_thing(పూర్ణాంకం get_title, పూర్ణాంకం test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
get_x11_windis(శూన్య)
{
    అక్షరం	    *winid;
    స్థిర పూర్ణాంకం	    ఫలితం = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_ఇక్కడ	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    స్థిర పూర్ణాంకం	    x11_display_from = XD_NONE;
    స్థిర పూర్ణాంకం	    did_సమితి_error_handler = FALSE;

    ఒకవేళ (!did_సమితి_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(శూన్య)XSetErrorHandler(x_error_handler);
	did_సమితి_error_handler = TRUE;
    }

# ఒకవేళ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    ఒకవేళ (gui.లోపల_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	ఒకవేళ (x11_display_from == XD_ఇక్కడ && x11_display != ఖాళీ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	ఒకవేళ (gui_get_x11_windis(&x11_కిటికీ, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    తిరిగి OK;
	}
	x11_display = ఖాళీ;
	తిరిగి FAIL;
    }
    లేకపోతే ఒకవేళ (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_కిటికీ = 0;
	x11_display = ఖాళీ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    ఒకవేళ (!x_connect_గమ్యం_server())
	తిరిగి FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    ఒకవేళ (x11_కిటికీ == 0 && (winid = getenv("WINDOWID")) != ఖాళీ)
	x11_కిటికీ = (కిటికీ)atol(winid);

# ifdef FEAT_XCLIPBOARD
    ఒకవేళ (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    ఒకవేళ (xterm_dpy != ఖాళీ && x11_కిటికీ != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	ఒకవేళ (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    ఒకవేళ (x11_display_from == XD_ఇక్కడ && x11_display != ఖాళీ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	ఒకవేళ (test_x11_కిటికీ(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_కిటికీ = 0;
	    x11_display = ఖాళీ;
	    x11_display_from = XD_NONE;
	    తిరిగి FAIL;
	}
	తిరిగి OK;
    }
# endif

    ఒకవేళ (x11_కిటికీ == 0 || x11_display == ఖాళీ)
	ఫలితం = -1;

    ఒకవేళ (ఫలితం != -1)	    // Have already been here and set this
	తిరిగి ఫలితం;	    // Don't do all these X calls again

    ఒకవేళ (x11_కిటికీ != 0 && x11_display == ఖాళీ)
    {
# ifdef సమితి_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ప్రారంభం_tv;

	ఒకవేళ (p_verbose > 0)
	    ELAPSED_INIT(ప్రారంభం_tv);
# endif

# ifdef సమితి_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(ఖాళీ);

# ifdef సమితి_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	ఒకవేళ (p_verbose > 0 && sig_alarm_called)
	    verb_సందేశం(_("Opening the X display timed out"));
# endif
	ఒకవేళ (x11_display != ఖాళీ)
	{
# ifdef ELAPSED_FUNC
	    ఒకవేళ (p_verbose > 0)
	    {
		verbose_enter();
		xopen_సందేశం(ELAPSED_FUNC(ప్రారంభం_tv));
		verbose_leave();
	    }
# endif
	    ఒకవేళ (test_x11_కిటికీ(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_కిటికీ = 0;
		XCloseDisplay(x11_display);
		x11_display = ఖాళీ;
	    }
	    లేకపోతే
		x11_display_from = XD_ఇక్కడ;
	}
    }
    ఒకవేళ (x11_కిటికీ == 0 || x11_display == ఖాళీ)
	తిరిగి (ఫలితం = FAIL);

# ifdef FEAT_EVAL
    సమితి_vim_var_nr(VV_WINDOWID, (దీర్ఘ)x11_కిటికీ);
# endif

    తిరిగి (ఫలితం = OK);
}

/*
 * Determine original x11 Window Title
 */
    స్థిర పూర్ణాంకం
get_x11_title(పూర్ణాంకం test_only)
{
    తిరిగి get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    స్థిర పూర్ణాంకం
get_x11_icon(పూర్ణాంకం test_only)
{
    పూర్ణాంకం		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    ఒకవేళ (oldicon == ఖాళీ && !test_only)
    {
	ఒకవేళ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	లేకపోతే
	    oldicon = vim_strsave(T_NAME);
    }

    తిరిగి retval;
}

    స్థిర పూర్ణాంకం
get_x11_thing(
    పూర్ణాంకం		get_title,	// get title string
    పూర్ణాంకం		test_only)
{
    XTextProperty	పాఠ్యం_prop;
    పూర్ణాంకం			retval = FALSE;
    స్థితి_3		స్థితి_2;

    ఒకవేళ (get_x11_windis() != OK)
	తిరిగి FALSE;

    // Get window/icon name if any
    ఒకవేళ (get_title)
	స్థితి_2 = XGetWMName(x11_display, x11_కిటికీ, &పాఠ్యం_prop);
    లేకపోతే
	స్థితి_2 = XGetWMIconName(x11_display, x11_కిటికీ, &పాఠ్యం_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    కిటికీ	    మూలం;
    కిటికీ	    మాతృ;
    కిటికీ	    win = x11_కిటికీ;
    కిటికీ	   *children;
    గుర్తులేని పూర్ణాంకం    అంకె_children;

    అయితే (!స్థితి_2 || పాఠ్యం_prop.విలువ == ఖాళీ)
    {
	ఒకవేళ (!XQuery_వృక్షం(x11_display, win, &మూలం, &మాతృ, &children,
		    &అంకె_children))
	    ఆపు;
	ఒకవేళ (children)
	    XFree((శూన్య *)children);
	ఒకవేళ (మాతృ == మూలం || మాతృ == 0)
	    ఆపు;

	win = మాతృ;
	ఒకవేళ (get_title)
	    స్థితి_2 = XGetWMName(x11_display, win, &పాఠ్యం_prop);
	లేకపోతే
	    స్థితి_2 = XGetWMIconName(x11_display, win, &పాఠ్యం_prop);
    }

    ఒకవేళ (స్థితి_2 && పాఠ్యం_prop.విలువ != ఖాళీ)
    {
	retval = TRUE;
	ఒకవేళ (!test_only)
	{
	    ఒకవేళ (get_title)
		vim_free(oldtitle);
	    లేకపోతే
		vim_free(oldicon);
	    ఒకవేళ (పాఠ్యం_prop.encoding == XA_STRING && !has_mbyte)
	    {
		ఒకవేళ (get_title)
		    oldtitle = vim_strsave((char_u *)పాఠ్యం_prop.విలువ);
		లేకపోతే
		    oldicon = vim_strsave((char_u *)పాఠ్యం_prop.విలువ);
	    }
	    లేకపోతే
	    {
		అక్షరం    **cl;
		స్థితి_3  transform_స్థితి;
		పూర్ణాంకం	    n = 0;

		transform_స్థితి = Xmb_పాఠ్యం_Property_గమ్యం_పాఠ్యం_List(x11_display,
			&పాఠ్యం_prop,
			&cl, &n);
		ఒకవేళ (transform_స్థితి >= Success && n > 0 && cl[0])
		{
		    ఒకవేళ (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    లేకపోతే
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		లేకపోతే
		{
		    ఒకవేళ (get_title)
			oldtitle = vim_strsave((char_u *)పాఠ్యం_prop.విలువ);
		    లేకపోతే
			oldicon = vim_strsave((char_u *)పాఠ్యం_prop.విలువ);
		}
	    }
	}
	XFree((శూన్య *)పాఠ్యం_prop.విలువ);
    }
    తిరిగి retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# ఒకవేళ defined(X_HAVE_UTF8_STRING)
#  ఒకవేళ X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    స్థిర శూన్య
సమితి_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    ఒకవేళ (enc_utf8)
	Xutf8_సమితి_WMProperties(x11_display, x11_కిటికీ, (స్థిరరాశి అక్షరం *)title,
					     ఖాళీ, ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
    లేకపోతే
# endif
    {
# ఒకవేళ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_సమితి_WMProperties(x11_display, x11_కిటికీ, (స్థిరరాశి అక్షరం *)title,
					     ఖాళీ, ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
#  లేకపోతే
	XTextProperty	పాఠ్యం_prop;
	అక్షరం		*c_title = (అక్షరం *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(శూన్య)XString_List_గమ్యం_పాఠ్యం_Property(&c_title, 1, &పాఠ్యం_prop);
	XSetWMProperties(x11_display, x11_కిటికీ, &పాఠ్యం_prop,
					     ఖాళీ, ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
#  endif
# లేకపోతే
	XStoreName(x11_display, x11_కిటికీ, (అక్షరం *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    స్థిర శూన్య
సమితి_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    ఒకవేళ (enc_utf8)
	Xutf8_సమితి_WMProperties(x11_display, x11_కిటికీ, ఖాళీ, (స్థిరరాశి అక్షరం *)icon,
						   ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
    లేకపోతే
# endif
    {
# ఒకవేళ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_సమితి_WMProperties(x11_display, x11_కిటికీ, ఖాళీ, (స్థిరరాశి అక్షరం *)icon,
						   ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
#  లేకపోతే
	XTextProperty	పాఠ్యం_prop;
	అక్షరం		*c_icon = (అక్షరం *)icon;

	(శూన్య)XString_List_గమ్యం_పాఠ్యం_Property(&c_icon, 1, &పాఠ్యం_prop);
	XSetWMProperties(x11_display, x11_కిటికీ, ఖాళీ, &పాఠ్యం_prop,
						   ఖాళీ, 0, ఖాళీ, ఖాళీ, ఖాళీ);
#  endif
# లేకపోతే
	XSetIconName(x11_display, x11_కిటికీ, (అక్షరం *)icon);
# endif
    }
    XFlush(x11_display);
}

#లేకపోతే  // FEAT_X11

    స్థిర పూర్ణాంకం
get_x11_title(పూర్ణాంకం test_only వాడని)
{
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
get_x11_icon(పూర్ణాంకం test_only)
{
    ఒకవేళ (!test_only)
    {
	ఒకవేళ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	లేకపోతే
	    oldicon = vim_strsave(T_NAME);
    }
    తిరిగి FALSE;
}

#అయితే_ముగింపు // FEAT_X11

    పూర్ణాంకం
mch_can_restore_title(శూన్య)
{
    తిరిగి get_x11_title(TRUE);
}

    పూర్ణాంకం
mch_can_restore_icon(శూన్య)
{
    తిరిగి get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    శూన్య
mch_settitle(char_u *title, char_u *icon)
{
    పూర్ణాంకం		type = 0;
    స్థిర పూర్ణాంకం	recursive = 0;

    ఒకవేళ (T_NAME == ఖాళీ)	    // no terminal name (yet)
	తిరిగి;
    ఒకవేళ (title == ఖాళీ && icon == ఖాళీ)	    // nothing to do
	తిరిగి;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    ఒకవేళ (recursive)
	తిరిగి;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#నిర్వచితమైతే FEAT_X11
    ఒకవేళ (get_x11_windis() == OK)
	type = 1;
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    ఒకవేళ (gui.లోపల_use)
	type = 1;
#అయితే_ముగింపు

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    ఒకవేళ ((type || *T_TS != NUL) && title != ఖాళీ)
    {
	ఒకవేళ (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	ఒకవేళ (oldtitle == ఖాళీ
#నిర్వచితమైతే FEAT_GUI
		&& !gui.లోపల_use
#అయితే_ముగింపు
		)		// first call but not in GUI, save title
	    (శూన్య)get_x11_title(FALSE);

	ఒకవేళ (*T_TS != NUL)		// it's OK if t_fs is empty
	    పదం_settitle(title);
#నిర్వచితమైతే FEAT_X11
	లేకపోతే
# ifdef FEAT_GUI_GTK
	ఒకవేళ (!gui.లోపల_use)		// don't do this if GTK+ is running
# endif
	    సమితి_x11_title(title);		// x11
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	లేకపోతే
	    gui_mch_settitle(title, icon);
#అయితే_ముగింపు
	unix_did_సమితి_title = TRUE;
    }

    ఒకవేళ ((type || *T_CIS != NUL) && icon != ఖాళీ)
    {
	ఒకవేళ (oldicon == ఖాళీ
#నిర్వచితమైతే FEAT_GUI
		&& !gui.లోపల_use
#అయితే_ముగింపు
		)		// first call, save icon
	    get_x11_icon(FALSE);

	ఒకవేళ (*T_CIS != NUL)
	{
	    బహిర్గతం_str(T_CIS);			// set icon start
	    బహిర్గతం_str_nf(icon);
	    బహిర్గతం_str(T_CIE);			// set icon end
	    బహిర్గతం_flush();
	}
#నిర్వచితమైతే FEAT_X11
	లేకపోతే
# ifdef FEAT_GUI_GTK
	ఒకవేళ (!gui.లోపల_use)		// don't do this if GTK+ is running
# endif
	    సమితి_x11_icon(icon);			// x11
#అయితే_ముగింపు
	did_సమితి_icon = TRUE;
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
    శూన్య
mch_restore_title(పూర్ణాంకం which)
{
    పూర్ణాంకం	do_నెట్టు_తీయి = unix_did_సమితి_title || did_సమితి_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_సమితి_title)
			? (oldtitle ? oldtitle : p_titleold) : ఖాళీ;
    char_u *tofree = ఖాళీ;
    ఒకవేళ (title == oldtitle && oldtitle != ఖాళీ)
    {
	tofree = vim_strsave(title);
	ఒకవేళ (tofree != ఖాళీ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_సమితి_icon) ? oldicon : ఖాళీ);
    vim_free(tofree);

    ఒకవేళ (do_నెట్టు_తీయి)
    {
	// pop and push from/to the stack
	పదం_తీయి_title(which);
	పదం_నెట్టు_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    పూర్ణాంకం
vim_is_xterm(char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి FALSE;
    తిరిగి ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#ఒకవేళ defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    పూర్ణాంకం
use_xterm_like_mouse(char_u *name)
{
    తిరిగి (name != ఖాళీ
	    && (పదం_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#అయితే_ముగింపు

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 * Return 3 for "urxvt".
 * Return 4 for "sgr".
 */
    పూర్ణాంకం
use_xterm_mouse(శూన్య)
{
    ఒకవేళ (ttym_flags == TTYM_SGR)
	తిరిగి 4;
    ఒకవేళ (ttym_flags == TTYM_URXVT)
	తిరిగి 3;
    ఒకవేళ (ttym_flags == TTYM_XTERM2)
	తిరిగి 2;
    ఒకవేళ (ttym_flags == TTYM_XTERM)
	తిరిగి 1;
    తిరిగి 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    పూర్ణాంకం
vim_is_iris(char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి FALSE;
    తిరిగి (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#ఒకవేళ defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    పూర్ణాంకం
vim_is_vt300(char_u *name)
{
    ఒకవేళ (name == ఖాళీ)
	తిరిగి FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    తిరిగి ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ఖాళీ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#అయితే_ముగింపు

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    పూర్ణాంకం
mch_get_user_name(char_u *s, పూర్ణాంకం len)
{
#నిర్వచితమైతే VMS
    vim_strncpy(s, (char_u *)cuserid(ఖాళీ), len - 1);
    తిరిగి OK;
#లేకపోతే
    తిరిగి mch_get_uname(getuid(), s, len);
#అయితే_ముగింపు
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    పూర్ణాంకం
mch_get_uname(uid_t uid, char_u *s, పూర్ణాంకం len)
{
#ఒకవేళ defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    నిర్మాణం passwd   *pw;

    ఒకవేళ ((pw = getpwuid(uid)) != ఖాళీ
	    && pw->pw_name != ఖాళీ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	తిరిగి OK;
    }
#అయితే_ముగింపు
    sprintf((అక్షరం *)s, "%d", (పూర్ణాంకం)uid);	    // assumes s is long enough
    తిరిగి FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#నిర్వచితమైతే HAVE_SYS_UTSNAME_H
    శూన్య
mch_get_host_name(char_u *s, పూర్ణాంకం len)
{
    నిర్మాణం utsname vutsname;

    ఒకవేళ (uname(&vutsname) < 0)
	*s = NUL;
    లేకపోతే
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#లేకపోతే // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    శూన్య
mch_get_host_name(char_u *s, పూర్ణాంకం len)
{
# ifdef VAXC
    vaxc$gethostname((అక్షరం *)s, len);
# లేకపోతే
    gethostname((అక్షరం *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#అయితే_ముగింపు // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    దీర్ఘ
mch_get_pid(శూన్య)
{
    తిరిగి (దీర్ఘ)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    పూర్ణాంకం
mch_process_running(దీర్ఘ pid)
{
    // If there is no error the process must be running.
    ఒకవేళ (kill(pid, 0) == 0)
	తిరిగి TRUE;
#నిర్వచితమైతే ESRCH
    // If the error is ESRCH then the process is not running.
    ఒకవేళ (errno == ESRCH)
	తిరిగి FALSE;
#అయితే_ముగింపు
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    తిరిగి TRUE;
}

#ఒకవేళ !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    స్థిర అక్షరం *
strerror(పూర్ణాంకం err)
{
    బాహ్య పూర్ణాంకం	    sys_nerr;
    బాహ్య అక్షరం	    *sys_errlist[];
    స్థిర అక్షరం	    er[20];

    ఒకవేళ (err > 0 && err < sys_nerr)
	తిరిగి (sys_errlist[err]);
    snprintf(er, పరిమాణం(er), "Error %d", err);
    తిరిగి er;
}
#అయితే_ముగింపు

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    పూర్ణాంకం
mch_dirname(char_u *buf, పూర్ణాంకం len)
{
#ఒకవేళ defined(USE_GETCWD)
    ఒకవేళ (getcwd((అక్షరం *)buf, len) == ఖాళీ)
    {
	STRCPY(buf, strerror(errno));
	తిరిగి FAIL;
    }
    తిరిగి OK;
#లేకపోతే
    తిరిగి (getwd((అక్షరం *)buf) != ఖాళీ ? OK : FAIL);
#అయితే_ముగింపు
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    పూర్ణాంకం
mch_నిండిన_Name(
    char_u	*fname,
    char_u	*buf,
    పూర్ణాంకం		len,
    పూర్ణాంకం		force)		// also expand when already absolute path
{
    పూర్ణాంకం		buflen = 0;
#నిర్వచితమైతే HAVE_FCHDIR
    పూర్ణాంకం		fd = -1;
    స్థిర పూర్ణాంకం	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#అయితే_ముగింపు
    char_u	olddir[MAXPATHL];
    char_u	*p;
    పూర్ణాంకం		retval = OK;
#నిర్వచితమైతే __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#అయితే_ముగింపు

#నిర్వచితమైతే VMS
    fname = vms_fixfilename(fname);
#అయితే_ముగింపు

#నిర్వచితమైతే __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# ఒకవేళ CYGWIN_సంస్కరణ_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_మార్గం(CCP_WIN_A_గమ్యం_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# లేకపోతే
    cygwin_conv_గమ్యం_posix_మార్గం(fname, posix_fname);
# endif
    fname = posix_fname;
#అయితే_ముగింపు

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    ఒకవేళ ((force || !mch_is_నిండిన_Name(fname))
	    && ((p = vim_strrchr(fname, '/')) == ఖాళీ || p != fname))
    {
	ఒకవేళ (p == ఖాళీ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	ఒకవేళ (p != ఖాళీ)
	{
	    ఒకవేళ (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#నిర్వచితమైతే HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    ఒకవేళ (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		ఒకవేళ (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#అయితే_ముగింపు

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    ఒకవేళ (
#నిర్వచితమైతే HAVE_FCHDIR
		fd < 0 &&
#అయితే_ముగింపు
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((అక్షరం *)olddir) != 0))
	    {
		p = ఖాళీ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    లేకపోతే
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		ఒకవేళ (p - fname >= len)
		    retval = FAIL;
		లేకపోతే
		{
		    vim_strncpy(buf, fname, p - fname);
		    ఒకవేళ (mch_chdir((అక్షరం *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			ఒకవేళ (mch_is_నిండిన_Name(fname))
			    retval = FAIL;
			లేకపోతే
			    p = ఖాళీ;
		    }
		    లేకపోతే ఒకవేళ (*p == '/')
			fname = p + 1;
		    లేకపోతే
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	ఒకవేళ (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	ఒకవేళ (p != ఖాళీ)
	{
	    పూర్ణాంకం	l;

#నిర్వచితమైతే HAVE_FCHDIR
	    ఒకవేళ (fd >= 0)
	    {
		ఒకవేళ (p_verbose >= 5)
		{
		    verbose_enter();
		    సందేశం("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    లేకపోతే
#అయితే_ముగింపు
		l = mch_chdir((అక్షరం *)olddir);
	    ఒకవేళ (l != 0)
		emsg(_(e_cannot_go_వెనుక_గమ్యం_మునుపటి_directory));
	}
#నిర్వచితమైతే HAVE_FCHDIR
	ఒకవేళ (fd >= 0)
	    close(fd);
#అయితే_ముగింపు

	buflen = (పూర్ణాంకం)STRLEN(buf);
	ఒకవేళ (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#నిర్వచితం_కాకపోతే VMS
	లేకపోతే ఒకవేళ (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += పరిమాణం(PATHSEP);
	}
#అయితే_ముగింపు
    }

    ఒకవేళ (buflen == 0)
	buflen = (పూర్ణాంకం)STRLEN(buf);

    // Catch file names which are too long.
    ఒకవేళ (retval == FAIL || (పూర్ణాంకం)(buflen + STRLEN(fname)) >= len)
	తిరిగి FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    ఒకవేళ (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    తిరిగి OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    పూర్ణాంకం
mch_is_నిండిన_Name(char_u *fname)
{
#నిర్వచితమైతే VMS
    తిరిగి ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((అక్షరం *)fname,':') || strchr((అక్షరం *)fname,'"') ||
	    (strchr((అక్షరం *)fname,'[') && strchr((అక్షరం *)fname,']'))||
	    (strchr((అక్షరం *)fname,'<') && strchr((అక్షరం *)fname,'>'))   );
#లేకపోతే
    తిరిగి (*fname == '/' || *fname == '~');
#అయితే_ముగింపు
}

#ఒకవేళ defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    శూన్య
fname_case(
    char_u	*name,
    పూర్ణాంకం		len వాడని)  // buffer size, only used when name gets longer
{
    నిర్మాణం stat st;
    char_u	*slash, *తోక;
    size_t	taillen;
    DIR		*dirp;
    నిర్మాణం dirent *dp;

    ఒకవేళ (mch_lstat((అక్షరం *)name, &st) < 0)
	తిరిగి;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    ఒకవేళ (slash == ఖాళీ)
    {
	dirp = opendir(".");
	తోక = name;
    }
    లేకపోతే
    {
	*slash = NUL;
	dirp = opendir((అక్షరం *)name);
	*slash = '/';
	తోక = slash + 1;
    }

    ఒకవేళ (dirp == ఖాళీ)
	తిరిగి;

    taillen = STRLEN(తోక);
    అయితే ((dp = readdir(dirp)) != ఖాళీ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	ఒకవేళ (STRICMP(తోక, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    నిర్మాణం stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (తోక - name), (char_u *)dp->d_name,
		    MAXPATHL - (తోక - name));
	    ఒకవేళ (mch_lstat((అక్షరం *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(తోక, dp->d_name);
		ఆపు;
	    }
	}
    }

    closedir(dirp);
}
#అయితే_ముగింపు

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    దీర్ఘ
mch_getperm(char_u *name)
{
    నిర్మాణం stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#నిర్వచితమైతే VMS
    ఒకవేళ (stat((అక్షరం *)vms_fixfilename(name), &statb))
#లేకపోతే
    ఒకవేళ (stat((అక్షరం *)name, &statb))
#అయితే_ముగింపు
	తిరిగి -1;
#నిర్వచితమైతే __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    తిరిగి statb.st_బహుళకం & ~S_ADDACE;
#లేకపోతే
    తిరిగి statb.st_బహుళకం;
#అయితే_ముగింపు
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
mch_setperm(char_u *name, దీర్ఘ perm)
{
    తిరిగి (chmod((అక్షరం *)
#నిర్వచితమైతే VMS
		    vms_fixfilename(name),
#లేకపోతే
		    name,
#అయితే_ముగింపు
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#ఒకవేళ defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    పూర్ణాంకం
mch_fsetperm(పూర్ణాంకం fd, దీర్ఘ perm)
{
    తిరిగి (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#అయితే_ముగింపు

#ఒకవేళ defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
రకనిర్వచనం నిర్మాణం vim_acl_solaris_T {
    పూర్ణాంకం acl_cnt;
    aclent_t *acl_నమోదు;
} vim_acl_solaris_T;
# endif

# ఒకవేళ defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    శూన్య
mch_copy_sec(char_u *from_file, char_u *గమ్యం_file)
{
    ఒకవేళ (from_file == ఖాళీ)
	తిరిగి;

    ఒకవేళ (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    ఒకవేళ (selinux_enabled <= 0)
	తిరిగి;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    అక్షరం *from_context = ఖాళీ;
    అక్షరం *గమ్యం_context = ఖాళీ;

    ఒకవేళ (getfilecon((అక్షరం *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	ఒకవేళ (errno == EOPNOTSUPP)
	    తిరిగి;

	సందేశం_puts(_("\nCould not get security context for "));
	సందేశం_outtrans(from_file);
	సందేశం_putchar('\n');
	తిరిగి;
    }
    ఒకవేళ (getfilecon((అక్షరం *)గమ్యం_file, &గమ్యం_context) < 0)
    {
	సందేశం_puts(_("\nCould not get security context for "));
	సందేశం_outtrans(గమ్యం_file);
	సందేశం_putchar('\n');
	freecon (from_context);
	తిరిగి ;
    }
    ఒకవేళ (strcmp(from_context, గమ్యం_context) != 0)
    {
	ఒకవేళ (setfilecon((అక్షరం *)గమ్యం_file, from_context) < 0)
	{
	    సందేశం_puts(_("\nCould not set security context for "));
	    సందేశం_outtrans(గమ్యం_file);
	    సందేశం_putchar('\n');
	}
    }
    freecon(గమ్యం_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# ఒకవేళ defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    శూన్య
mch_copy_sec(char_u *from_file, char_u *గమ్యం_file)
{
    స్థిర స్థిరరాశి అక్షరం * స్థిరరాశి smack_copied_లక్షణాలు[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    అక్షరం	బఫరు[SMACK_LABEL_LEN];
    స్థిరరాశి అక్షరం	*name;
    పూర్ణాంకం		సూచీ;
    పూర్ణాంకం		ret;
    ssize_t	కొలత;

    ఒకవేళ (from_file == ఖాళీ)
	తిరిగి;

    కొలత = listxattr((అక్షరం *)from_file, ఖాళీ, 0);
    // not supported or no attributes to copy
    ఒకవేళ (కొలత <= 0)
	తిరిగి;

    కొరకు (సూచీ = 0 ; సూచీ < (పూర్ణాంకం)ARRAY_నిడివి(smack_copied_లక్షణాలు); సూచీ++)
    {
	// get the name of the attribute to copy
	name = smack_copied_లక్షణాలు[సూచీ];

	// get the value of the attribute in buffer
	కొలత = getxattr((అక్షరం*)from_file, name, బఫరు, పరిమాణం(బఫరు));
	ఒకవేళ (కొలత >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((అక్షరం*)గమ్యం_file, name, బఫరు, (size_t)కొలత, 0);
	    ఒకవేళ (ret < 0)
	    {
		vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, గమ్యం_file);
		సందేశం_outtrans(IObuff);
		సందేశం_putchar('\n');
	    }
	}
	లేకపోతే
	{
	    // what reason of not having the attribute value?
	    ఎంపిక (errno)
	    {
		స్థితి ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    తిరిగి; // leave because it isn't useful to continue

		స్థితి ERANGE:
		అప్రమేయ:
		    // no enough size OR unexpected error
		     vim_snprintf((అక్షరం *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    సందేశం_puts((అక్షరం *)IObuff);
		    సందేశం_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		స్థితి ENODATA:
		    // no attribute of this name
		    ret = removexattr((అక్షరం*)గమ్యం_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ఆపు;
	    }
	}
    }
}
# endif // HAVE_SMACK

# ఒకవేళ defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    శూన్య
mch_copy_xattr(char_u *from_file, char_u *గమ్యం_file)
{
    అక్షరం	*xattr_buf;
    ssize_t	కొలత;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    అక్షరం	*కీలకం;
    అక్షరం	*val = ఖాళీ;
    అక్షరం	*errmsg = ఖాళీ;

    ఒకవేళ (from_file == ఖాళీ)
	తిరిగి;

    // get the length of the extended attributes
    కొలత = listxattr((అక్షరం *)from_file, ఖాళీ, 0);
    // not supported or no attributes to copy
    ఒకవేళ (కొలత <= 0)
	తిరిగి;
    xattr_buf = (అక్షరం*)alloc(కొలత);
    ఒకవేళ (xattr_buf == ఖాళీ)
	తిరిగి;
    కొలత = listxattr((అక్షరం *)from_file, xattr_buf, కొలత);
    tsize = కొలత;

    errno = 0;

    కొరకు (పూర్ణాంకం విడత = 0; విడత < 2; విడత++)
    {

	కీలకం = xattr_buf;
	ఒకవేళ (విడత == 1)
	    కొలత = tsize;

	అయితే (కొలత > 0)
	{
	    vallen = getxattr((అక్షరం *)from_file, కీలకం,
		    val, విడత ? max_vallen : 0);
	    // only set the attribute in the second round
	    ఒకవేళ (vallen >= 0 && విడత &&
		setxattr((అక్షరం *)గమ్యం_file, కీలకం, val, vallen, 0) == 0)
		;
	    లేకపోతే ఒకవేళ (errno)
	    {
		ఎంపిక (errno)
		{
		    స్థితి E2BIG:
			errmsg = e_xattr_e2big;
			వెళ్ళు error_exit;
		    స్థితి ENOTSUP:
		    స్థితి EACCES:
		    స్థితి EPERM:
			ఆపు;
		    స్థితి ERANGE:
			errmsg = e_xattr_erange;
			వెళ్ళు error_exit;
		    అప్రమేయ:
			errmsg = e_xattr_other;
			వెళ్ళు error_exit;
		}
	    }

	    ఒకవేళ (విడత == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(కీలకం) + 1;
	    కొలత -= keylen;
	    కీలకం += keylen;
	}
	ఒకవేళ (విడత)
	    ఆపు;

	val = (అక్షరం*)alloc(max_vallen + 1);
	ఒకవేళ (val == ఖాళీ)
	    వెళ్ళు error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    ఒకవేళ (errmsg != ఖాళీ)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname వాడని)
{
    vim_acl_T	ret = ఖాళీ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((అక్షరం *)fname, ACL_TYPE_ACCESS);
# లేకపోతే
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    ఒకవేళ (acl_get((అక్షరం *)fname, 0, &aclent) < 0)
	తిరిగి ఖాళీ;
    ret = (vim_acl_T)aclent;
#  లేకపోతే
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = జ్ఞాపకం_ఇవ్వు(పరిమాణం(vim_acl_solaris_T));
    ఒకవేళ (aclent == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ ((aclent->acl_cnt = acl((అక్షరం *)fname, GETACLCNT, 0, ఖాళీ)) < 0)
    {
	విడుదల(aclent);
	తిరిగి ఖాళీ;
    }
    aclent->acl_నమోదు = జ్ఞాపకం_ఇవ్వు(aclent->acl_cnt * పరిమాణం(aclent_t));
    ఒకవేళ (aclent->acl_నమోదు == ఖాళీ)
    {
	విడుదల(aclent);
	తిరిగి ఖాళీ;
    }
    ఒకవేళ (acl((అక్షరం *)fname, GETACL, aclent->acl_cnt, aclent->acl_నమోదు) < 0)
    {
	విడుదల(aclent->acl_నమోదు);
	విడుదల(aclent);
	తిరిగి ఖాళీ;
    }
    ret = (vim_acl_T)aclent;
#   లేకపోతే
#    ఒకవేళ defined(HAVE_AIX_ACL)
    పూర్ణాంకం		aclsize;
    నిర్మాణం acl *aclent;

    aclsize = పరిమాణం(నిర్మాణం acl);
    aclent = జ్ఞాపకం_ఇవ్వు(aclsize);
    ఒకవేళ (aclent == ఖాళీ)
	తిరిగి ఖాళీ;
    ఒకవేళ (statacl((అక్షరం *)fname, STX_సాధారణ, aclent, aclsize) < 0)
    {
	ఒకవేళ (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    ఒకవేళ (aclent == ఖాళీ || statacl((అక్షరం *)fname, STX_సాధారణ, aclent, aclsize) < 0)
	    {
		విడుదల(aclent);
		తిరిగి ఖాళీ;
	    }
	}
	లేకపోతే
	{
	    విడుదల(aclent);
	    తిరిగి ఖాళీ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    తిరిగి ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    శూన్య
mch_సమితి_acl(char_u *fname వాడని, vim_acl_T aclent)
{
    ఒకవేళ (aclent == ఖాళీ)
	తిరిగి;
# ifdef HAVE_POSIX_ACL
    acl_సమితి_file((అక్షరం *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# లేకపోతే
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_సమితి((అక్షరం *)fname, (acl_t *)aclent);
#  లేకపోతే
#   ifdef HAVE_SOLARIS_ACL
    acl((అక్షరం *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_నమోదు);
#   లేకపోతే
#    ifdef HAVE_AIX_ACL
    chacl((అక్షరం *)fname, aclent, ((నిర్మాణం acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    శూన్య
mch_free_acl(vim_acl_T aclent)
{
    ఒకవేళ (aclent == ఖాళీ)
	తిరిగి;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# లేకపోతే
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  లేకపోతే
#   ifdef HAVE_SOLARIS_ACL
    విడుదల(((vim_acl_solaris_T *)aclent)->acl_నమోదు);
    విడుదల(aclent);
#   లేకపోతే
#    ifdef HAVE_AIX_ACL
    విడుదల(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#అయితే_ముగింపు

/*
 * Set hidden flag for "name".
 */
    శూన్య
mch_hide(char_u *name వాడని)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    పూర్ణాంకం
mch_isdir(char_u *name)
{
    నిర్మాణం stat statb;

    ఒకవేళ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	తిరిగి FALSE;
    ఒకవేళ (stat((అక్షరం *)name, &statb))
	తిరిగి FALSE;
    తిరిగి (S_ISDIR(statb.st_బహుళకం) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    పూర్ణాంకం
mch_isrealdir(char_u *name)
{
    నిర్మాణం stat statb;

    ఒకవేళ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	తిరిగి FALSE;
    ఒకవేళ (mch_lstat((అక్షరం *)name, &statb))
	తిరిగి FALSE;
    తిరిగి (S_ISDIR(statb.st_బహుళకం) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    స్థిర పూర్ణాంకం
executable_file(char_u *name)
{
    నిర్మాణం stat	st;

    ఒకవేళ (stat((అక్షరం *)name, &st))
	తిరిగి 0;
#నిర్వచితమైతే VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    పూర్ణాంకం vms_executable = 0;
    ఒకవేళ (S_ISREG(st.st_బహుళకం) && mch_access((అక్షరం *)name, X_OK) == 0)
    {
	ఒకవేళ (అక్షరశ్రేణి_శోధన(vms_tolower((అక్షరం*)name),".exe") != ఖాళీ
		|| అక్షరశ్రేణి_శోధన(vms_tolower((అక్షరం*)name),".com")!= ఖాళీ)
	    vms_executable = 1;
    }
    తిరిగి vms_executable;
#లేకపోతే
    తిరిగి S_ISREG(st.st_బహుళకం) && mch_access((అక్షరం *)name, X_OK) == 0;
#అయితే_ముగింపు
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    పూర్ణాంకం
mch_can_exe(char_u *name, char_u **మార్గం, పూర్ణాంకం use_మార్గం)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_చివర;
    size_t	elen;
    పూర్ణాంకం		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    ఒకవేళ (!use_మార్గం || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	ఒకవేళ ((use_మార్గం || gettail(name) != name) && executable_file(name))
	{
	    ఒకవేళ (మార్గం != ఖాళీ)
	    {
		ఒకవేళ (name[0] != '/')
		    *మార్గం = నిండిన_Name_save(name, TRUE);
		లేకపోతే
		    *మార్గం = vim_strsave(name);
	    }
	    తిరిగి TRUE;
	}
	తిరిగి FALSE;
    }

    p = (char_u *)getenv("PATH");
    ఒకవేళ (p == ఖాళీ || *p == NUL)
	తిరిగి -1;
    p_చివర = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_చివర - p) + 2;
    buf = alloc(bufsize);
    ఒకవేళ (buf == ఖాళీ)
	తిరిగి -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    కొరకు (;;)
    {
	e = (char_u *)strchr((అక్షరం *)p, ':');
	ఒకవేళ (e == ఖాళీ)
	    e = p_చివర;
	elen = (size_t)(e - p);
	ఒకవేళ (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((అక్షరం *)buf, bufsize, "%.*s%s%s",
		(పూర్ణాంకం)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	ఒకవేళ (retval == 1)
	{
	    ఒకవేళ (మార్గం != ఖాళీ)
	    {
		ఒకవేళ (buf[0] != '/')
		    *మార్గం = నిండిన_Name_save(buf, TRUE);
		లేకపోతే
		    *మార్గం = vim_strnsave(buf, buflen);
	    }
	    ఆపు;
	}

	ఒకవేళ (*e != ':')
	    ఆపు;
	p = e + 1;
    }

    vim_free(buf);
    తిరిగి retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    పూర్ణాంకం
mch_nodetype(char_u *name)
{
    నిర్మాణం stat	st;

    ఒకవేళ (stat((అక్షరం *)name, &st))
	తిరిగి కణుపు_సాధారణ;
    ఒకవేళ (S_ISREG(st.st_బహుళకం) || S_ISDIR(st.st_బహుళకం))
	తిరిగి కణుపు_సాధారణ;
    ఒకవేళ (S_ISBLK(st.st_బహుళకం))	// block device isn't writable
	తిరిగి కణుపు_OTHER;
    // Everything else is writable?
    తిరిగి కణుపు_WRITABLE;
}

    శూన్య
mch_early_init(శూన్య)
{
#నిర్వచితమైతే HAVE_CHECK_దొంతర_GROWTH
    పూర్ణాంకం			i;

    check_దొంతర_growth((అక్షరం *)&i);

# ifdef HAVE_దొంతర_హద్దు
    get_దొంతర_హద్దు();
# endif

#అయితే_ముగింపు

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#ఒకవేళ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_దొంతర = alloc(get_signal_దొంతర_కొలత());
    init_signal_దొంతర();
#అయితే_ముగింపు

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#నిర్వచితమైతే MACOS_X
# ifdef MAC_OS_X_సంస్కరణ_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_సమితి(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#అయితే_ముగింపు
}

#ఒకవేళ defined(EXITFREE)
    శూన్య
mch_free_mem(శూన్య)
{
# ఒకవేళ defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    ఒకవేళ (clip_నక్షత్రం.owned)
	clip_lose_selection(&clip_నక్షత్రం);
    ఒకవేళ (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ఒకవేళ (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_సంస్కరణ
    // Lesstif crashes here, lose some memory
    ఒకవేళ (xterm_dpy != ఖాళీ)
	XtCloseDisplay(xterm_dpy);
    ఒకవేళ (app_context != (XtAppContext)ఖాళీ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ఖాళీ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# ఒకవేళ defined(FEAT_X11)
    ఒకవేళ (x11_display != ఖాళీ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# ఒకవేళ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_దొంతర);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#అయితే_ముగింపు

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    స్థిర శూన్య
exit_scroll(శూన్య)
{
    ఒకవేళ (silent_బహుళకం)
	తిరిగి;
    ఒకవేళ (newline_on_exit || సందేశం_didout)
    {
	ఒకవేళ (సందేశం_use_printf())
	{
	    ఒకవేళ (info_సందేశం)
		mch_సందేశం("\n");
	    లేకపోతే
		mch_errmsg("\r\n");
	}
	లేకపోతే
	    బహిర్గతం_char('\n');
    }
    లేకపోతే ఒకవేళ (!is_not_a_పదం())
    {
	restore_cterm_colors();		// get original colors back
	సందేశం_clr_eos_force();		// clear the rest of the display
	windgoto((పూర్ణాంకం)అడ్డువరుసలు - 1, cmdline_నిలువు_off); // may have moved the cursor
    }
}

#నిర్వచితమైతే USE_GCOV_FLUSH
# ఒకవేళ (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
బాహ్య శూన్య __gcov_dump(శూన్య);
బాహ్య శూన్య __gcov_reset(శూన్య);
#  define __gcov_flush() చేయి { __gcov_dump(); __gcov_reset(); } అయితే (0)
# లేకపోతే
బాహ్య శూన్య __gcov_flush(శూన్య);
# endif
#అయితే_ముగింపు

    శూన్య
mch_exit(పూర్ణాంకం r)
{
    exiting = TRUE;

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (!gui.లోపల_use)
#అయితే_ముగింపు
    {
	settmode(TMODE_COOK);
	ఒకవేళ (!is_not_a_పదం())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    పదం_తీయి_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	ఒకవేళ (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	ఒకవేళ (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	ఒకవేళ (నిండిన_screen)
	    cursor_on();
    }
    బహిర్గతం_flush();
    ml_close_all(TRUE);		// remove all memfiles

#నిర్వచితమైతే USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#అయితే_ముగింపు

    may_core_dump();
#నిర్వచితమైతే FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_exit(r);
#అయితే_ముగింపు

#నిర్వచితమైతే MACOS_CONVERT
    mac_conv_cleanup();
#అయితే_ముగింపు

#నిర్వచితమైతే __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    ఒకవేళ (deadly_signal != 0)
	తిరిగి;
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#అయితే_ముగింపు

#నిర్వచితమైతే EXITFREE
    free_all_mem();
#అయితే_ముగింపు

    నిష్క్రమించు(r);
}

    స్థిర శూన్య
may_core_dump(శూన్య)
{
    ఒకవేళ (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#నిర్వచితం_కాకపోతే VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    స్థిర పూర్ణాంకం
get_tty_fd(పూర్ణాంకం fd)
{
    పూర్ణాంకం		tty_fd = fd;

# ఒకవేళ defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    ఒకవేళ (mch_isatty(fd) > 1)
    {
	అక్షరం *name;

	name = ptsname(fd);
	ఒకవేళ (name == ఖాళీ)
	    తిరిగి -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	ఒకవేళ (tty_fd < 0)
	    తిరిగి -1;
    }
# endif
    తిరిగి tty_fd;
}

    స్థిర పూర్ణాంకం
mch_tcgetattr(పూర్ణాంకం fd, శూన్య *పదం)
{
    పూర్ణాంకం		tty_fd;
    పూర్ణాంకం		retval = -1;

    tty_fd = get_tty_fd(fd);
    ఒకవేళ (tty_fd < 0)
	తిరిగి -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (నిర్మాణం termios *)పదం);
#  లేకపోతే
    retval = ioctl(tty_fd, TCGETA, (నిర్మాణం termio *)పదం);
#  endif
# లేకపోతే
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (నిర్మాణం sgttyb *)పదం);
# endif
    ఒకవేళ (tty_fd != fd)
	close(tty_fd);
    తిరిగి retval;
}

    శూన్య
mch_settmode(tmode_T tmode)
{
    స్థిర పూర్ణాంకం మొదటి = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    స్థిర నిర్మాణం termios told;
	   నిర్మాణం termios tnew;
#  లేకపోతే
    స్థిర నిర్మాణం termio told;
	   నిర్మాణం termio tnew;
#  endif

    ఒకవేళ (మొదటి)
    {
	మొదటి = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    ఒకవేళ (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ఖాళీ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  ఒకవేళ defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   లేకపోతే
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    లేకపోతే
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    లేకపోతే ఒకవేళ (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  ఒకవేళ defined(HAVE_TERMIOS_H)
    {
	పూర్ణాంకం	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	అయితే (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  లేకపోతే
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# లేకపోతే
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    స్థిర నిర్మాణం sgttyb ttybold;
	   నిర్మాణం sgttyb ttybnew;

    ఒకవేళ (మొదటి)
    {
	మొదటి = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    ఒకవేళ (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    లేకపోతే ఒకవేళ (tmode == TMODE_SLEEP)
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
    శూన్య
get_stty(శూన్య)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    ఒకవేళ (get_tty_info(read_cmd_fd, &info) != OK)
	తిరిగి;

    intr_char = info.అంతరాయం;
    buf[0] = info.backspace;
    buf[1] = NUL;
    కూడు_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = కనుగొను_termcode((char_u *)"kD");
    ఒకవేళ (p != ఖాళీ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ఖాళీ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    పూర్ణాంకం
get_tty_info(పూర్ణాంకం fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    నిర్మాణం termios కీలకాలు;
#  లేకపోతే
    నిర్మాణం termio కీలకాలు;
#  endif

    ఒకవేళ (mch_tcgetattr(fd, &కీలకాలు) != -1)
    {
	info->backspace = కీలకాలు.c_cc[VERASE];
	info->అంతరాయం = కీలకాలు.c_cc[VINTR];
	ఒకవేళ (కీలకాలు.c_iflag & ICRNL)
	    info->enter = NL;
	లేకపోతే
	    info->enter = CAR;
	ఒకవేళ (కీలకాలు.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	లేకపోతే
	    info->nl_does_cr = FALSE;
	తిరిగి OK;
    }
# లేకపోతే
    // for "old" tty systems
    నిర్మాణం sgttyb కీలకాలు;

    ఒకవేళ (mch_tcgetattr(fd, &కీలకాలు) != -1)
    {
	info->backspace = కీలకాలు.sg_erase;
	info->అంతరాయం = కీలకాలు.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	తిరిగి OK;
    }
# endif
    తిరిగి FAIL;
}

#అయితే_ముగింపు // VMS

స్థిర పూర్ణాంకం	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    శూన్య
mch_setmouse(పూర్ణాంకం on)
{
#నిర్వచితమైతే FEAT_BEVAL_పదం
    స్థిర పూర్ణాంకం	bevalterm_ison = FALSE;
#అయితే_ముగింపు
    పూర్ణాంకం		xterm_mouse_vers;

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ఒకవేళ (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#అయితే_ముగింపు

    ఒకవేళ (on == mouse_ison
#నిర్వచితమైతే FEAT_BEVAL_పదం
	    && p_bevalterm == bevalterm_ison
#అయితే_ముగింపు
	    )
	// return quickly if nothing to do
	తిరిగి;

    xterm_mouse_vers = use_xterm_mouse();

#నిర్వచితమైతే FEAT_MOUSE_URXVT
    ఒకవేళ (ttym_flags == TTYM_URXVT)
    {
	బహిర్గతం_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#అయితే_ముగింపు

    ఒకవేళ (T_CXM != ఖాళీ && *T_CXM != NUL)
    {
	పదం_enable_mouse(on);
    }
    లేకపోతే ఒకవేళ (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	బహిర్గతం_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#నిర్వచితమైతే FEAT_BEVAL_పదం
    ఒకవేళ (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	ఒకవేళ (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    బహిర్గతం_str_nf((char_u *)("\033[?1003l"));
    }
#అయితే_ముగింపు

    ఒకవేళ (xterm_mouse_vers > 0)
    {
	ఒకవేళ (on)	// enable mouse events, use mouse tracking if available
	    బహిర్గతం_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#నిర్వచితమైతే FEAT_BEVAL_పదం
			    bevalterm_ison ? "\033[?1003h" :
#అయితే_ముగింపు
			      "\033[?1002h")
			: "\033[?1000h"));
	లేకపోతే	// disable mouse events, could probably always send the same
	    బహిర్గతం_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#నిర్వచితమైతే FEAT_MOUSE_DEC
    లేకపోతే ఒకవేళ (ttym_flags == TTYM_DEC)
    {
	ఒకవేళ (on)	// enable mouse events
	    బహిర్గతం_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	లేకపోతే	// disable mouse events
	    బహిర్గతం_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_GPM
    లేకపోతే
    {
	ఒకవేళ (on)
	{
	    ఒకవేళ (gpm_open())
		mouse_ison = TRUE;
	}
	లేకపోతే
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYSMOUSE
    లేకపోతే
    {
	ఒకవేళ (on)
	{
	    ఒకవేళ (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	లేకపోతే
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_JSB
    లేకపోతే
    {
	ఒకవేళ (on)
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
	    బహిర్గతం_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# లేకపోతే
	    బహిర్గతం_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	లేకపోతే
	{
	    బహిర్గతం_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_PTERM
    లేకపోతే
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	ఒకవేళ (on)
	    బహిర్గతం_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	లేకపోతే
	    బహిర్గతం_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_BEVAL_పదం)
/*
 * Called when 'balloonevalterm' changed.
 */
    శూన్య
mch_bevalterm_మారింది(శూన్య)
{
    mch_setmouse(mouse_ison);
}
#అయితే_ముగింపు

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    శూన్య
check_mouse_termcode(శూన్య)
{
#నిర్వచితమైతే FEAT_MOUSE_XTERM
    ఒకవేళ (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
    {
	సమితి_mouse_termcode(KS_MOUSE, (char_u *)(పదం_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	ఒకవేళ (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    లేకపోతే
	del_mouse_termcode(KS_MOUSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_GPM
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    లేకపోతే
	del_mouse_termcode(KS_GPM_MOUSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_SYSMOUSE
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    లేకపోతే
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    లేకపోతే
	del_mouse_termcode(KS_NETTERM_MOUSE);
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_DEC_MOUSE, (char_u *)(పదం_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    లేకపోతే
	del_mouse_termcode(KS_DEC_MOUSE);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    ఒకవేళ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
	సమితి_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    లేకపోతే
	del_mouse_termcode(KS_PTERM_MOUSE);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_MOUSE_URXVT
    ఒకవేళ (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.లోపల_use
# endif
	    )
    {
	సమితి_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(పదం_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	ఒకవేళ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    లేకపోతే
	del_mouse_termcode(KS_URXVT_MOUSE);
#అయితే_ముగింపు
    ఒకవేళ (use_xterm_mouse() == 4
#నిర్వచితమైతే FEAT_GUI
	    && !gui.లోపల_use
#అయితే_ముగింపు
	    )
    {
	సమితి_mouse_termcode(KS_SGR_MOUSE, (char_u *)(పదం_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	సమితి_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(పదం_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	ఒకవేళ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    లేకపోతే
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#నిర్వచితం_కాకపోతే VMS

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 * Return OK when size could be determined, FAIL otherwise.
 */
    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    దీర్ఘ	అడ్డువరుసలు_2 = 0;
    దీర్ఘ	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	నిర్మాణం winsize	ws;
	పూర్ణాంకం fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ఒకవేళ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ఒకవేళ (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_నిలువు;
	    అడ్డువరుసలు_2 = ws.ws_అడ్డువరుస;
#  ifdef FEAT_EVAL
	    ch_log(ఖాళీ, "Got size with TIOCGWINSZ: %ld x %ld", columns, అడ్డువరుసలు_2);
#  endif
	}
    }
# లేకపోతే // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	నిర్మాణం ttysize	ts;
	పూర్ణాంకం fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ఒకవేళ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ఒకవేళ (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_నిలువుల_సంఖ్య;
	    అడ్డువరుసలు_2 = ts.ts_పంక్తులు;
#   ifdef FEAT_EVAL
	    ch_log(ఖాళీ, "Got size with TIOCGSIZE: %ld x %ld", columns, అడ్డువరుసలు_2);
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
    ఒకవేళ (columns == 0 || అడ్డువరుసలు_2 == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ఖాళీ)
    {
	ఒకవేళ ((p = (char_u *)getenv("LINES")))
	{
	    అడ్డువరుసలు_2 = atoi((అక్షరం *)p);
# ifdef FEAT_EVAL
	    ch_log(ఖాళీ, "Got 'lines' from $LINES: %ld", అడ్డువరుసలు_2);
# endif
	}
	ఒకవేళ ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((అక్షరం *)p);
# ifdef FEAT_EVAL
	    ch_log(ఖాళీ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    ఒకవేళ (columns == 0 || అడ్డువరుసలు_2 == 0)
    {
	getlinecol(&columns, &అడ్డువరుసలు_2);
#  ifdef FEAT_EVAL
	ch_log(ఖాళీ, "Got size from termcap: %ld x %ld", columns, అడ్డువరుసలు_2);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    ఒకవేళ (columns <= 0 || అడ్డువరుసలు_2 <= 0)
	తిరిగి FAIL;

    అడ్డువరుసలు = అడ్డువరుసలు_2;
    Columns = columns;
    హద్దు_screen_కొలత();
    తిరిగి OK;
}

# ఒకవేళ defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    శూన్య
mch_calc_cell_కొలత(నిర్మాణం cellsize *cs_బహిర్గతం)
{
   నిర్మాణం winsize   ws;
   పూర్ణాంకం		    retval;
   పూర్ణాంకం		    x_cell_కొలత;
   పూర్ణాంకం		    y_cell_కొలత;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ఖాళీ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   ఒకవేళ (retval == -1 || ws.ws_నిలువు == 0 || ws.ws_అడ్డువరుస == 0)
   {
	cs_బహిర్గతం->cs_xpixel = -1;
	cs_బహిర్గతం->cs_ypixel = -1;
	తిరిగి;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_కొలత = ws.ws_xpixel / ws.ws_నిలువు;
   y_cell_కొలత = ws.ws_ypixel / ws.ws_అడ్డువరుస;

   cs_బహిర్గతం->cs_xpixel = x_cell_కొలత > 0 ? x_cell_కొలత : -1;
   cs_బహిర్గతం->cs_ypixel = y_cell_కొలత > 0 ? y_cell_కొలత : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   ఒకవేళ (x_cell_కొలత <= 0 || y_cell_కొలత <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       బహిర్గతం_STR("\033[14t");
       బహిర్గతం_STR("\033[16t");
       బహిర్గతం_flush();
   }
}
# endif

# ఒకవేళ defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    పూర్ణాంకం
mch_report_winsize(పూర్ణాంకం fd, పూర్ణాంకం అడ్డువరుసలు_2, పూర్ణాంకం నిలువుల_సంఖ్య)
{
    పూర్ణాంకం		tty_fd;
    పూర్ణాంకం		retval = -1;

    tty_fd = get_tty_fd(fd);
    ఒకవేళ (tty_fd < 0)
	తిరిగి FAIL;

#  ఒకవేళ defined(TIOCSWINSZ)
    నిర్మాణం winsize ws;

    ws.ws_నిలువు = నిలువుల_సంఖ్య;
    ws.ws_అడ్డువరుస = అడ్డువరుసలు_2;
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
    ch_log(ఖాళీ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    నిర్మాణం ttysize ts;

    ts.ts_నిలువుల_సంఖ్య = నిలువుల_సంఖ్య;
    ts.ts_పంక్తులు = అడ్డువరుసలు_2;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ఖాళీ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    ఒకవేళ (tty_fd != fd)
	close(tty_fd);
    తిరిగి retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    శూన్య
mch_సమితి_shellsize(శూన్య)
{
    ఒకవేళ (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	పదం_సమితి_winsize((పూర్ణాంకం)అడ్డువరుసలు, (పూర్ణాంకం)Columns);
	బహిర్గతం_flush();
	screen_ప్రారంభం();			// don't know where cursor is now
    }
}

#అయితే_ముగింపు // VMS

/*
 * Rows and/or Columns has changed.
 */
    శూన్య
mch_new_shellsize(శూన్య)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    స్థిర pid_t
wait4pid(pid_t బిడ్డ, waitstatus *స్థితి_2)
{
    pid_t wait_pid = 0;
    దీర్ఘ delay_msec = 1;

    అయితే (wait_pid != బిడ్డ)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(బిడ్డ, స్థితి_2, WNOHANG);
	ఒకవేళ (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    ఒకవేళ (++delay_msec > 10)
		delay_msec = 10;
	    కొనసాగు;
	}
	ఒకవేళ (wait_pid <= 0
#నిర్వచితమైతే ECHILD
		&& errno == ECHILD
#అయితే_ముగింపు
	   )
	    ఆపు;
    }
    తిరిగి wait_pid;
}

#ఒకవేళ !defined(USE_SYSTEM) || defined(FEAT_పని_CHANNEL)
/*
 * Set the environment for a child process.
 */
    స్థిర శూన్య
సమితి_బిడ్డ_environment(
	దీర్ఘ	అడ్డువరుసలు_2,
	దీర్ఘ	columns,
	అక్షరం	*పదం,
	పూర్ణాంకం	is_terminal వాడని)
{
# ifdef HAVE_SETENV
    అక్షరం	envbuf[50];
# లేకపోతే
    స్థిర అక్షరం	envbuf_పదం[30];
    స్థిర అక్షరం	envbuf_అడ్డువరుసలు[20];
    స్థిర అక్షరం	envbuf_పంక్తులు[20];
    స్థిర అక్షరం	envbuf_Columns[20];
    స్థిర అక్షరం	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    స్థిర అక్షరం	envbuf_సంస్కరణ[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    స్థిర అక్షరం	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", పదం, 1);
    sprintf((అక్షరం *)envbuf, "%ld", అడ్డువరుసలు_2);
    setenv("ROWS", (అక్షరం *)envbuf, 1);
    sprintf((అక్షరం *)envbuf, "%ld", అడ్డువరుసలు_2);
    setenv("LINES", (అక్షరం *)envbuf, 1);
    sprintf((అక్షరం *)envbuf, "%ld", columns);
    setenv("COLUMNS", (అక్షరం *)envbuf, 1);
    sprintf((అక్షరం *)envbuf, "%d", t_colors);
    setenv("COLORS", (అక్షరం *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    ఒకవేళ (is_terminal)
    {
	sprintf((అక్షరం *)envbuf, "%ld",  (దీర్ఘ)get_vim_var_nr(VV_సంస్కరణ));
	setenv("VIM_TERMINAL", (అక్షరం *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ఖాళీ ? "" : (అక్షరం *)serverName, 1);
#  endif
# లేకపోతే
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_పదం, పరిమాణం(envbuf_పదం), "TERM=%s", పదం);
    putenv(envbuf_పదం);
    vim_snprintf(envbuf_అడ్డువరుసలు, పరిమాణం(envbuf_అడ్డువరుసలు), "ROWS=%ld", అడ్డువరుసలు_2);
    putenv(envbuf_అడ్డువరుసలు);
    vim_snprintf(envbuf_పంక్తులు, పరిమాణం(envbuf_పంక్తులు), "LINES=%ld", అడ్డువరుసలు_2);
    putenv(envbuf_పంక్తులు);
    vim_snprintf(envbuf_Columns, పరిమాణం(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, పరిమాణం(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    ఒకవేళ (is_terminal)
    {
	vim_snprintf(envbuf_సంస్కరణ, పరిమాణం(envbuf_సంస్కరణ),
			 "VIM_TERMINAL=%ld", (దీర్ఘ)get_vim_var_nr(VV_సంస్కరణ));
	putenv(envbuf_సంస్కరణ);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, పరిమాణం(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ఖాళీ ? "" : (అక్షరం *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    స్థిర శూన్య
సమితి_default_బిడ్డ_environment(పూర్ణాంకం is_terminal)
{
    సమితి_బిడ్డ_environment(అడ్డువరుసలు, Columns, "dumb", is_terminal);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI) || defined(FEAT_పని_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    స్థిర శూన్య
open_pty(పూర్ణాంకం *pty_master_fd, పూర్ణాంకం *pty_slave_fd, char_u **name1, char_u **name2)
{
    అక్షరం	*tty_name;

    ఒకవేళ (name1 != ఖాళీ)
	*name1 = ఖాళీ;
    ఒకవేళ (name2 != ఖాళీ)
	*name2 = ఖాళీ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    ఒకవేళ (*pty_master_fd < 0)
	తిరిగి;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# లేకపోతే
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    ఒకవేళ (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    లేకపోతే
    {
	ఒకవేళ (name1 != ఖాళీ)
	    *name1 = vim_strsave((char_u *)tty_name);
	ఒకవేళ (name2 != ఖాళీ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#అయితే_ముగింపు

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    స్థిర శూన్య
may_send_sigint(పూర్ణాంకం c వాడని, pid_t pid వాడని, pid_t wpid వాడని)
{
#నిర్వచితమైతే SIGINT
    ఒకవేళ (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# లేకపోతే
	kill(0, SIGINT);
# endif
	ఒకవేళ (wpid > 0)
	    kill(wpid, SIGINT);
    }
#అయితే_ముగింపు
}

#ఒకవేళ !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    పూర్ణాంకం
unix_build_argv(
	char_u *cmd,
	అక్షరం ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    అక్షరం	**argv = ఖాళీ;
    పూర్ణాంకం		argc;

    *sh_tofree = vim_strsave(p_sh);
    ఒకవేళ (*sh_tofree == ఖాళీ)		// out of memory
	తిరిగి FAIL;

    ఒకవేళ (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	తిరిగి FAIL;
    *argvp = argv;

    ఒకవేళ (cmd != ఖాళీ)
    {
	char_u	*s;
	char_u	*p;

	ఒకవేళ (extra_shell_arg != ఖాళీ)
	    argv[argc++] = (అక్షరం *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	ఒకవేళ (*shcf_tofree == ఖాళీ)    // out of memory
	    తిరిగి FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	అయితే (*p != NUL)
	{
	    argv[argc++] = (అక్షరం *)s;
	    అయితే (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (అక్షరం *)cmd;
    }
    argv[argc] = ఖాళీ;
    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    స్థిర పూర్ణాంకం
mch_call_shell_terminal(
    char_u	*cmd,
    పూర్ణాంకం		options వాడని)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    అక్షరం	**argv = ఖాళీ;
    char_u	*tofree1 = ఖాళీ;
    char_u	*tofree2 = ఖాళీ;
    పూర్ణాంకం		retval = -1;
    buf_T	*buf;
    పని_T	*పని;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ఒకవేళ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	వెళ్ళు theend;

    init_పని_options(&opt);
    ch_log(ఖాళీ, "starting terminal for system command '%s'", cmd);
    buf = పదం_ప్రారంభం(ఖాళీ, argv, &opt, పదం_ప్రారంభం_SYSTEM);
    ఒకవేళ (buf == ఖాళీ)
	వెళ్ళు theend;

    పని = పదం_getjob(buf->b_పదం);
    ++పని->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ఒకవేళ (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	అయితే (పదం_use_loop())
	{
	    ఒకవేళ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_చురుకైన)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ఒకవేళ (terminal_loop(TRUE) == OK)
		    సాధారణ_cmd(&oa, TRUE);
	    }
	    లేకపోతే
		సాధారణ_cmd(&oa, TRUE);
	}
	retval = పని->jv_exitval;
	ch_log(ఖాళీ, "system command finished");

	పని_unref(పని);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    ఒకవేళ (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_బఫరు(DOBUF_WIPE, DOBUF_మొదటి, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    తిరిగి retval;
}
#అయితే_ముగింపు

#నిర్వచితమైతే USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    స్థిర పూర్ణాంకం
mch_call_shell_system(
    char_u	*cmd,
    పూర్ణాంకం		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    అక్షరం	*ifn = ఖాళీ;
    అక్షరం	*ofn = ఖాళీ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    పూర్ణాంకం		x;

    బహిర్గతం_flush();

    ఒకవేళ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# ఒకవేళ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  ఒకవేళ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    ఒకవేళ (cmd == ఖాళీ)
	x = system((అక్షరం *)p_sh);
    లేకపోతే
    {
# ifdef VMS
	ఒకవేళ (ofn = strchr((అక్షరం *)cmd, '>'))
	    *ofn++ = '\0';
	ఒకవేళ (ifn = strchr((అక్షరం *)cmd, '<'))
	{
	    అక్షరం *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    ఒకవేళ (p)
		*p = '\0';
	}
	ఒకవేళ (ofn)
	    x = vms_sys((అక్షరం *)cmd, ofn, ifn);
	లేకపోతే
	    x = system((అక్షరం *)cmd);
# లేకపోతే
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ఖాళీ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	ఒకవేళ (newcmd == ఖాళీ)
	    x = 0;
	లేకపోతే
	{
	    sprintf((అక్షరం *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ఖాళీ ? "" : (అక్షరం *)extra_shell_arg,
		    (అక్షరం *)p_shcf,
		    (అక్షరం *)cmd);
	    x = system((అక్షరం *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_స్థితి(x);
# endif
    ఒకవేళ (emsg_silent)
	;
    లేకపోతే ఒకవేళ (x == 127)
	సందేశం_puts(_("\nCannot execute shell sh\n"));
    లేకపోతే ఒకవేళ (x && !(options & SHELL_SILENT))
    {
	సందేశం_puts(_("\nshell returned "));
	సందేశం_outnum((దీర్ఘ)x);
	సందేశం_putchar('\n');
    }

    ఒకవేళ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# ఒకవేళ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    తిరిగి x;
}

#లేకపోతే // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    స్థిర పూర్ణాంకం
mch_call_shell_fork(
    char_u	*cmd,
    పూర్ణాంకం		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    పూర్ణాంకం		స్థితి_2 = -1;
    పూర్ణాంకం		retval = -1;
    అక్షరం	**argv = ఖాళీ;
    char_u	*tofree1 = ఖాళీ;
    char_u	*tofree2 = ఖాళీ;
    పూర్ణాంకం		i;
    పూర్ణాంకం		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    పూర్ణాంకం		pty_slave_fd = -1;
# endif
    పూర్ణాంకం		fd_toshell[2];		// for pipes
    పూర్ణాంకం		fd_fromshell[2];
    పూర్ణాంకం		pipe_error = FALSE;
    పూర్ణాంకం		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    బహిర్గతం_flush();
    ఒకవేళ (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    ఒకవేళ (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    ఒకవేళ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	వెళ్ళు error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    ఒకవేళ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.లోపల_use && show_shell_mess)
# endif
		    )
    {
# ifdef FEAT_GUI
	/*
	 * Try to open a master pty.
	 * If this works, open the slave pty.
	 * If the slave can't be opened, close the master pty.
	 */
	ఒకవేళ (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ఖాళీ, ఖాళీ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	ఒకవేళ (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    ఒకవేళ (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		ఒకవేళ (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    ఒకవేళ (pipe_error)
	    {
		సందేశం_puts(_("\nCannot create pipes\n"));
		బహిర్గతం_flush();
	    }
	}
    }

    ఒకవేళ (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	ఖండం_సంకేతాలు(&curset);
	pid = fork();	// maybe we should use vfork()
	ఒకవేళ (pid == -1)
	{
	    UNBLOCK_సంకేతాలు(&curset);

	    సందేశం_puts(_("\nCannot fork\n"));
	    ఒకవేళ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.లోపల_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		ఒకవేళ (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		లేకపోతే				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	లేకపోతే ఒకవేళ (pid == 0)	// child
	{
	    reset_సంకేతాలు();		// handle signals normally
	    UNBLOCK_సంకేతాలు(&curset);

# ifdef FEAT_EVAL
	    ఒకవేళ (ch_log_చురుకైన())
	    {
		ch_log(ఖాళీ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    ఒకవేళ (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		పూర్ణాంకం fd;

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
		ఒకవేళ (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    లేకపోతే ఒకవేళ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.లోపల_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		ఒకవేళ (p_stmp)
		{
		    (శూన్య)setsid();
#  ఒకవేళ defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		ఒకవేళ (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    ఒకవేళ (options & SHELL_COOKED)
			అమరిక_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (అక్షరం *)ఖాళీ);
#  endif
		}
# endif
		సమితి_default_బిడ్డ_environment(FALSE);

		/*
		 * stderr is only redirected when using the GUI, so that a
		 * program like gpg can still access the terminal to get a
		 * passphrase using stderr.
		 */
# ifdef FEAT_GUI
		ఒకవేళ (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    ఒకవేళ (gui.లోపల_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		లేకపోతే
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
		    ఒకవేళ (gui.లోపల_use)
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
	లేకపోతే			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_సంకేతాలు(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_సంకేతాలు(&curset);
# ifdef FEAT_పని_CHANNEL
	    ++dont_check_పని_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    ఒకవేళ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.లోపల_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    బఫరు[BUFLEN + 1];
		పూర్ణాంకం	    బఫరు_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		పూర్ణాంకం	    ta_len = 0;		// valid bytes in ta_buf[]
		పూర్ణాంకం	    len;
		పూర్ణాంకం	    p_మరిన్ని_save;
		పూర్ణాంకం	    old_State;
		పూర్ణాంకం	    c;
		పూర్ణాంకం	    toshell_fd;
		పూర్ణాంకం	    fromshell_fd;
		garray_T    ga;
		పూర్ణాంకం	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   ప్రారంభం_tv;
# endif

# ifdef FEAT_GUI
		ఒకవేళ (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		లేకపోతే
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
		p_మరిన్ని_save = p_మరిన్ని;
		p_మరిన్ని = FALSE;
		old_State = State;
		State = బహుళకం_EXTERNCMD;	// don't redraw at window resize

		ఒకవేళ ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    ఒకవేళ ((wpid = fork()) == -1)
		    {
			సందేశం_puts(_("\nCannot fork\n"));
		    }
		    లేకపోతే ఒకవేళ (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_ప్రారంభం.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			కొరకు (;;)
			{
			    ఒకవేళ (lplen == 0)
				len = 0;
			    లేకపోతే ఒకవేళ (lp[written] == NL)
				// NL -> NUL translation
				len = లిఖ(toshell_fd, "", (size_t)1);
			    లేకపోతే
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = లిఖ(toshell_fd, (అక్షరం *)lp + written,
					   s == ఖాళీ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    ఒకవేళ (len == (పూర్ణాంకం)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				ఒకవేళ (lnum != curbuf->b_op_చివర.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_పంక్తి_లెక్క
						    || curbuf->b_p_eol)))
				    vim_ignored = లిఖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				ఒకవేళ (lnum > curbuf->b_op_చివర.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ఆపు;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    లేకపోతే ఒకవేళ (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    లేకపోతే // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		ఒకవేళ (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(ప్రారంభం_tv);
# endif
		కొరకు (;;)
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
		    ఒకవేళ (!(options & SHELL_EXPAND)
			    && ((options &
					 (SHELL_READ|SHELL_WRITE|SHELL_COOKED))
				      != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
						    || gui.లోపల_use
# endif
						    )
			    && wait_pid == 0
			    && (ta_len > 0 || noread_cnt > 4))
		    {
		      ఒకవేళ (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(ప్రారంభం_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      ఒకవేళ (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			ఒకవేళ (len == 1 && (pty_master_fd < 0 || cmd != ఖాళీ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    ఒకవేళ (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				close(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = పదం_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			ఒకవేళ (pty_master_fd < 0)
			{
			    కొరకు (i = ta_len; i < ta_len + len; ++i)
			    {
				ఒకవేళ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    సందేశం_putchar(ta_buf[i]);
				లేకపోతే ఒకవేళ (has_mbyte)
				{
				    పూర్ణాంకం l = (*mb_ptr2len)(ta_buf + i);

				    సందేశం_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				లేకపోతే
				    సందేశం_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
			    బహిర్గతం_flush();
			}

			ta_len += len;

			/*
			 * Write the characters to the child, unless EOF has
			 * been typed for pipes.  Write one character at a
			 * time, to avoid losing too much typeahead.
			 * When writing buffer lines, drop the typed
			 * characters (only check for CTRL-C).
			 */
			ఒకవేళ (options & SHELL_WRITE)
			    ta_len = 0;
			లేకపోతే ఒకవేళ (toshell_fd >= 0)
			{
			    len = లిఖ(toshell_fd, (అక్షరం *)ta_buf, (size_t)1);
			    ఒకవేళ (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    ఒకవేళ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# లేకపోతే
			kill(0, SIGINT);
# endif
			ఒకవేళ (wpid > 0)
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
		    అయితే (RealWaitForChar(fromshell_fd, 10L, ఖాళీ, ఖాళీ))
		    {
			len = read_eintr(fromshell_fd, బఫరు
				+ బఫరు_off, (size_t)(BUFLEN - బఫరు_off)
				);
			ఒకవేళ (len <= 0)		    // end of file or error
			    వెళ్ళు ముగిసిన;

			noread_cnt = 0;
			ఒకవేళ (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    కొరకు (i = 0; i < len; ++i)
			    {
				ఒకవేళ (బఫరు[i] == NL)
				    append_ga_పంక్తి(&ga);
				లేకపోతే ఒకవేళ (బఫరు[i] == NUL)
				    ga_append(&ga, NL);
				లేకపోతే
				    ga_append(&ga, బఫరు[i]);
			    }
			}
			లేకపోతే ఒకవేళ (has_mbyte)
			{
			    పూర్ణాంకం		l;
			    char_u	*p;

			    len += బఫరు_off;
			    బఫరు[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    కొరకు (p = బఫరు; p < బఫరు + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				ఒకవేళ (l == 0)
				    l = 1;  // NUL byte?
				లేకపోతే ఒకవేళ (MB_BYTE2LEN(*p) != l)
				    ఆపు;
			    }
			    ఒకవేళ (p == బఫరు)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				ఒకవేళ (len >= 12)
				    ++p;
				లేకపోతే
				{
				    బఫరు_off = len;
				    కొనసాగు;
				}
			    }
			    c = *p;
			    *p = NUL;
			    సందేశం_puts((అక్షరం *)బఫరు);
			    ఒకవేళ (p < బఫరు + len)
			    {
				*p = c;
				బఫరు_off = (బఫరు + len) - p;
				mch_memmove(బఫరు, p, బఫరు_off);
				కొనసాగు;
			    }
			    బఫరు_off = 0;
			}
			లేకపోతే
			{
			    బఫరు[len] = NUL;
			    సందేశం_puts((అక్షరం *)బఫరు);
			}

			windgoto(సందేశం_అడ్డువరుస, cmdline_నిలువు_off + సందేశం_నిలువు);
			cursor_on();
			బహిర్గతం_flush();
			ఒకవేళ (got_int)
			    ఆపు;

# ifdef ELAPSED_FUNC
			ఒకవేళ (wait_pid == 0)
			{
			    దీర్ఘ	msec = ELAPSED_FUNC(ప్రారంభం_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    ఒకవేళ (msec > 2000)
			    {
				noread_cnt = 5;
				ఆపు;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    ఒకవేళ (wait_pid == pid)
		    {
			ఒకవేళ (noread_cnt < 5)
			    కొనసాగు;
			ఆపు;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &స్థితి_2, WNOHANG);
		    ఒకవేళ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(స్థితి_2)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    లేకపోతే
			wait_pid = 0;

# ఒకవేళ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
# endif
# ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
# endif
		}
ముగిసిన:
		p_మరిన్ని = p_మరిన్ని_save;
		ఒకవేళ (options & SHELL_READ)
		{
		    ఒకవేళ (ga.ga_len > 0)
		    {
			append_ga_పంక్తి(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    లేకపోతే
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		ఒకవేళ (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		ఒకవేళ (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# ఒకవేళ (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    లేకపోతే
	    {
		దీర్ఘ delay_msec = 1;

		ఒకవేళ (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    బహిర్గతం_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		కొరకు (;;)
		{
		    ఒకవేళ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  లేకపోతే
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &స్థితి_2, WNOHANG);
		    ఒకవేళ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(స్థితి_2)))
		    {
			wait_pid = pid;
			ఆపు;
		    }

#  ఒకవేళ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    ఒకవేళ (++delay_msec > 10)
			delay_msec = 10;
		}

		ఒకవేళ (tmode == TMODE_RAW)
		    // possibly enables modifyOtherKeys again
		    బహిర్గతం_str_t_TI();
	    }
# endif

	    /*
	     * Wait until our child has exited.
	     * Ignore wait() returning pids of other children and returning
	     * because of some signal like SIGWINCH.
	     * Don't wait if wait_pid was already set above, indicating the
	     * child already exited.
	     */
	    ఒకవేళ (wait_pid != pid)
		(శూన్య)wait4pid(pid, &స్థితి_2);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    ఒకవేళ (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    ఒకవేళ (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ఖాళీ);
	    }

# ifdef FEAT_పని_CHANNEL
	    --dont_check_పని_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    ఒకవేళ (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    సమితి_సంకేతాలు();

	    ఒకవేళ (WIFEXITED(స్థితి_2))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(స్థితి_2);
		ఒకవేళ (retval != 0 && !emsg_silent)
		{
		    ఒకవేళ (retval == EXEC_FAILED)
		    {
			సందేశం_puts(_("\nCannot execute shell "));
			సందేశం_outtrans(p_sh);
			సందేశం_putchar('\n');
		    }
		    లేకపోతే ఒకవేళ (!(options & SHELL_SILENT))
		    {
			సందేశం_puts(_("\nshell returned "));
			సందేశం_outnum((దీర్ఘ)retval);
			సందేశం_putchar('\n');
		    }
		}
	    }
	    లేకపోతే
		సందేశం_puts(_("\nCommand terminated\n"));
	}
    }

error:
    ఒకవేళ (!did_settmode)
	ఒకవేళ (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    తిరిగి retval;
}
#అయితే_ముగింపు // USE_SYSTEM

    పూర్ణాంకం
mch_call_shell(
    char_u	*cmd,
    పూర్ణాంకం		options)	// SHELL_*, see vim.h
{
#నిర్వచితమైతే FEAT_EVAL
    ch_log(ఖాళీ, "executing shell command: %s", cmd);
#అయితే_ముగింపు
#ఒకవేళ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    ఒకవేళ (gui.లోపల_use && vim_strchr(p_go, GO_TERMINAL) != ఖాళీ
					      && (options & SHELL_SILENT) == 0)
	తిరిగి mch_call_shell_terminal(cmd, options);
#అయితే_ముగింపు
#నిర్వచితమైతే USE_SYSTEM
    తిరిగి mch_call_shell_system(cmd, options);
#లేకపోతే
    తిరిగి mch_call_shell_fork(cmd, options);
#అయితే_ముగింపు
}

#ఒకవేళ defined(FEAT_EVAL)
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
mch_get_cmd_output_ప్రత్యక్ష(
    అక్షరం	**argv,
    char_u	*infile,
    పూర్ణాంకం		flags వాడని,
    పూర్ణాంకం		*ret_len)
{
    pid_t	pid;
    పూర్ణాంకం		fd_బహిర్గతం[2] = {-1, -1};
    పూర్ణాంకం		స్థితి_2 = -1;
    char_u	*బఫరు = ఖాళీ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ఖాళీ, "directly executing: %s", argv[0]);

    ఒకవేళ (pipe(fd_బహిర్గతం) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	తిరిగి ఖాళీ;
    }

    ఖండం_సంకేతాలు(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# లేకపోతే
    pid = fork();
# endif
    ఒకవేళ (pid == -1)
    {
	UNBLOCK_సంకేతాలు(&curset);
	close(fd_బహిర్గతం[0]);
	close(fd_బహిర్గతం[1]);
	emsg(_("\nCannot fork\n"));
	తిరిగి ఖాళీ;
    }

    ఒకవేళ (pid == 0)
    {
	// child process
	reset_సంకేతాలు();
	UNBLOCK_సంకేతాలు(&curset);

	ఒకవేళ (ch_log_చురుకైన())
	{
	    ch_log(ఖాళీ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	ఒకవేళ (infile != ఖాళీ)
	{
	    పూర్ణాంకం fd_లోపల = open((అక్షరం *)infile, O_RDONLY);
	    ఒకవేళ (fd_లోపల >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_లోపల);
		close(fd_లోపల);
	    }
	}
	లేకపోతే
	{
	    పూర్ణాంకం nullfd = open("/dev/null", O_RDONLY);
	    ఒకవేళ (nullfd >= 0)
	    {
		close(0);
		vim_ignored = dup(nullfd);
		close(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	close(fd_బహిర్గతం[0]);
	close(1);
	vim_ignored = dup(fd_బహిర్గతం[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_బహిర్గతం[1]);
	close(fd_బహిర్గతం[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_సంకేతాలు(&curset);
    close(fd_బహిర్గతం[1]);

    // Read output from child.
    కొరకు (;;)
    {
	అక్షరం	buf[4096];
	పూర్ణాంకం	n;

	n = (పూర్ణాంకం)పఠ(fd_బహిర్గతం[0], buf, పరిమాణం(buf));
	ఒకవేళ (n <= 0)
	    ఆపు;
	ఒకవేళ (ga_grow(&ga, n) == FAIL)
	    ఆపు;
	mch_memmove((అక్షరం *)ga.ga_దత్తాంశం + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_బహిర్గతం[0]);

    // Wait for child to finish.
    (శూన్య)waitpid(pid, &స్థితి_2, 0);
    ఒకవేళ (WIFEXITED(స్థితి_2))
	స్థితి_2 = WEXITSTATUS(స్థితి_2);
    లేకపోతే
	స్థితి_2 = -1;
    సమితి_vim_var_nr(VV_SHELL_ERROR, (దీర్ఘ)స్థితి_2);

    ఒకవేళ (ga.ga_len > 0)
    {
	బఫరు = alloc(ga.ga_len + 1);
	ఒకవేళ (బఫరు != ఖాళీ)
	{
	    mch_memmove(బఫరు, ga.ga_దత్తాంశం, ga.ga_len);
	    ఒకవేళ (ret_len == ఖాళీ)
	    {
		పూర్ణాంకం	i;

		// Change NUL into SOH, otherwise the string is truncated.
		కొరకు (i = 0; i < ga.ga_len; ++i)
		    ఒకవేళ (బఫరు[i] == NUL)
			బఫరు[i] = 1;
		బఫరు[ga.ga_len] = NUL;
	    }
	    లేకపోతే
		*ret_len = ga.ga_len;
	}
    }
    లేకపోతే ఒకవేళ (ret_len != ఖాళీ)
	*ret_len = 0;

    ga_clear(&ga);
    తిరిగి బఫరు;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_పని_CHANNEL)
    శూన్య
mch_పని_ప్రారంభం(అక్షరం **argv, పని_T *పని, jobopt_T *options, పూర్ణాంకం is_terminal)
{
    pid_t	pid;
    పూర్ణాంకం		fd_లోపల[2] = {-1, -1};	// for stdin
    పూర్ణాంకం		fd_బహిర్గతం[2] = {-1, -1};	// for stdout
    పూర్ణాంకం		fd_err[2] = {-1, -1};	// for stderr
    పూర్ణాంకం		pty_master_fd = -1;
    పూర్ణాంకం		pty_slave_fd = -1;
    channel_T	*channel = ఖాళీ;
    పూర్ణాంకం		use_null_for_లోపల = options->jo_io[PART_లోపల] == JIO_NULL;
    పూర్ణాంకం		use_null_for_బహిర్గతం = options->jo_io[PART_బహిర్గతం] == JIO_NULL;
    పూర్ణాంకం		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    పూర్ణాంకం		use_file_for_లోపల = options->jo_io[PART_లోపల] == JIO_FILE;
    పూర్ణాంకం		use_file_for_బహిర్గతం = options->jo_io[PART_బహిర్గతం] == JIO_FILE;
    పూర్ణాంకం		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    పూర్ణాంకం		use_బఫరు_for_లోపల = options->jo_io[PART_లోపల] == JIO_బఫరు;
    పూర్ణాంకం		use_బహిర్గతం_for_err = options->jo_io[PART_ERR] == JIO_బహిర్గతం;
    SIGSET_DECL(curset)

    ఒకవేళ (use_బహిర్గతం_for_err && use_null_for_బహిర్గతం)
	use_null_for_err = TRUE;

    // default is to fail
    పని->jv_స్థితి = పని_FAILED;

    ఒకవేళ (options->jo_pty
	    && (!(use_file_for_లోపల || use_null_for_లోపల)
		|| !(use_file_for_బహిర్గతం || use_null_for_బహిర్గతం)
		|| !(use_బహిర్గతం_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &పని->jv_tty_బహిర్గతం, &పని->jv_tty_లోపల);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    ఒకవేళ (use_file_for_లోపల)
    {
	char_u *fname = options->jo_io_name[PART_లోపల];

	fd_లోపల[0] = mch_open((అక్షరం *)fname, O_RDONLY, 0);
	ఒకవేళ (fd_లోపల[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    లేకపోతే
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	ఒకవేళ (!use_null_for_లోపల && (pty_master_fd < 0 || use_బఫరు_for_లోపల)
							    && pipe(fd_లోపల) < 0)
	    వెళ్ళు failed;

    ఒకవేళ (use_file_for_బహిర్గతం)
    {
	char_u *fname = options->jo_io_name[PART_బహిర్గతం];

	fd_బహిర్గతం[1] = mch_open((అక్షరం *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ఒకవేళ (fd_బహిర్గతం[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    లేకపోతే ఒకవేళ (!use_null_for_బహిర్గతం && pty_master_fd < 0 && pipe(fd_బహిర్గతం) < 0)
	వెళ్ళు failed;

    ఒకవేళ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((అక్షరం *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ఒకవేళ (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    వెళ్ళు failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    లేకపోతే ఒకవేళ (!use_బహిర్గతం_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_సమితి & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	వెళ్ళు failed;

    ఒకవేళ (!use_null_for_లోపల || !use_null_for_బహిర్గతం || !use_null_for_err)
    {
	ఒకవేళ (options->jo_సమితి & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ఒకవేళ (channel != ఖాళీ)
		++channel->ch_refcount;
	}
	లేకపోతే
	    channel = కూడు_channel();
	ఒకవేళ (channel == ఖాళీ)
	    వెళ్ళు failed;
	ఒకవేళ (పని->jv_tty_బహిర్గతం != ఖాళీ)
	    ch_log(channel, "using pty %s on fd %d",
					       పని->jv_tty_బహిర్గతం, pty_master_fd);
    }

    ఖండం_సంకేతాలు(&curset);
    pid = fork();	// maybe we should use vfork()
    ఒకవేళ (pid == -1)
    {
	// failed to fork
	UNBLOCK_సంకేతాలు(&curset);
	వెళ్ళు failed;
    }
    ఒకవేళ (pid == 0)
    {
	పూర్ణాంకం	null_fd = -1;
	పూర్ణాంకం	stderr_పనిచేస్తుంది = TRUE;

	// child
	reset_సంకేతాలు();		// handle signals normally
	UNBLOCK_సంకేతాలు(&curset);

# ifdef FEAT_EVAL
	ఒకవేళ (ch_log_చురుకైన())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(శూన్య)setsid();
# endif

# ifdef FEAT_TERMINAL
	ఒకవేళ (options->jo_పదం_అడ్డువరుసలు > 0)
	{
	    అక్షరం *పదం = (అక్షరం *)T_NAME;

#  ifdef FEAT_GUI
	    ఒకవేళ (పదం_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		పదం = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    ఒకవేళ (పదం == ఖాళీ || *పదం == NUL || STRNCMP(పదం, "xterm", 5) != 0)
	    {
		ఒకవేళ (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    పదం = "xterm-256color";
		లేకపోతే ఒకవేళ (t_colors > 16)
		    పదం = "xterm-color";
		లేకపోతే
		    పదం = "xterm";
	    }
	    సమితి_బిడ్డ_environment(
		    (దీర్ఘ)options->jo_పదం_అడ్డువరుసలు,
		    (దీర్ఘ)options->jo_పదం_నిలువుల_సంఖ్య,
		    పదం,
		    is_terminal);
	}
	లేకపోతే
# endif
	    సమితి_default_బిడ్డ_environment(is_terminal);

	ఒకవేళ (options->jo_env != ఖాళీ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    పూర్ణాంకం		todo = (పూర్ణాంకం)dict->dv_hashtab.ht_వాడిన;

	    FOR_ALL_HASHTAB_వస్తువులు(&dict->dv_hashtab, hi, todo)
		ఒకవేళ (!HASHITEM_ఖాళీలు(hi))
		{
		    typval_T *అంశం = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_కీలకం, tv_get_string(అంశం));
		    --todo;
		}
	}

	ఒకవేళ (use_null_for_లోపల || use_null_for_బహిర్గతం || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    ఒకవేళ (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	ఒకవేళ (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    అమరిక_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (అక్షరం *)ఖాళీ);
# endif
	}

	// set up stdin for the child
	close(0);
	ఒకవేళ (use_null_for_లోపల && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	లేకపోతే ఒకవేళ (fd_లోపల[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	లేకపోతే
	    vim_ignored = dup(fd_లోపల[0]);

	// set up stderr for the child
	close(2);
	ఒకవేళ (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_పనిచేస్తుంది = FALSE;
	}
	లేకపోతే ఒకవేళ (use_బహిర్గతం_for_err)
	    vim_ignored = dup(fd_బహిర్గతం[1]);
	లేకపోతే ఒకవేళ (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	లేకపోతే
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	ఒకవేళ (use_null_for_బహిర్గతం && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	లేకపోతే ఒకవేళ (fd_బహిర్గతం[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	లేకపోతే
	    vim_ignored = dup(fd_బహిర్గతం[1]);

	ఒకవేళ (fd_లోపల[0] >= 0)
	    close(fd_లోపల[0]);
	ఒకవేళ (fd_లోపల[1] >= 0)
	    close(fd_లోపల[1]);
	ఒకవేళ (fd_బహిర్గతం[0] >= 0)
	    close(fd_బహిర్గతం[0]);
	ఒకవేళ (fd_బహిర్గతం[1] >= 0)
	    close(fd_బహిర్గతం[1]);
	ఒకవేళ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ఒకవేళ (fd_err[1] >= 0)
	    close(fd_err[1]);
	ఒకవేళ (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	ఒకవేళ (null_fd >= 0)
	    close(null_fd);

	ఒకవేళ (options->jo_cwd != ఖాళీ && mch_chdir((అక్షరం *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	ఒకవేళ (stderr_పనిచేస్తుంది)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_సంకేతాలు(&curset);

    పని->jv_pid = pid;
    పని->jv_స్థితి = పని_STARTED;
    పని->jv_channel = channel;  // ch_refcount was set above

    ఒకవేళ (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    ఒకవేళ (fd_లోపల[0] >= 0)
	close(fd_లోపల[0]);
    ఒకవేళ (fd_బహిర్గతం[1] >= 0)
	close(fd_బహిర్గతం[1]);
    ఒకవేళ (fd_err[1] >= 0)
	close(fd_err[1]);
    ఒకవేళ (channel != ఖాళీ)
    {
	పూర్ణాంకం లోపల_fd = INVALID_FD;
	పూర్ణాంకం బహిర్గతం_fd = INVALID_FD;
	పూర్ణాంకం err_fd = INVALID_FD;

	ఒకవేళ (!(use_file_for_లోపల || use_null_for_లోపల))
	    లోపల_fd = fd_లోపల[1] >= 0 ? fd_లోపల[1] : pty_master_fd;

	ఒకవేళ (!(use_file_for_బహిర్గతం || use_null_for_బహిర్గతం))
	    బహిర్గతం_fd = fd_బహిర్గతం[0] >= 0 ? fd_బహిర్గతం[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	ఒకవేళ (!(use_బహిర్గతం_for_err || use_file_for_err || use_null_for_err))
	{
	    ఒకవేళ (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    లేకపోతే ఒకవేళ (బహిర్గతం_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_సమితి_pipes(channel, లోపల_fd, బహిర్గతం_fd, err_fd);
	channel_సమితి_పని(channel, పని, options);
    }
    లేకపోతే
    {
	ఒకవేళ (fd_లోపల[1] >= 0)
	    close(fd_లోపల[1]);
	ఒకవేళ (fd_బహిర్గతం[0] >= 0)
	    close(fd_బహిర్గతం[0]);
	ఒకవేళ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ఒకవేళ (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    తిరిగి;

failed:
    channel_unref(channel);
    ఒకవేళ (fd_లోపల[0] >= 0)
	close(fd_లోపల[0]);
    ఒకవేళ (fd_లోపల[1] >= 0)
	close(fd_లోపల[1]);
    ఒకవేళ (fd_బహిర్గతం[0] >= 0)
	close(fd_బహిర్గతం[0]);
    ఒకవేళ (fd_బహిర్గతం[1] >= 0)
	close(fd_బహిర్గతం[1]);
    ఒకవేళ (fd_err[0] >= 0)
	close(fd_err[0]);
    ఒకవేళ (fd_err[1] >= 0)
	close(fd_err[1]);
    ఒకవేళ (pty_master_fd >= 0)
	close(pty_master_fd);
    ఒకవేళ (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    స్థిర char_u *
get_signal_name(పూర్ణాంకం sig)
{
    పూర్ణాంకం		i;
    char_u	numbuf[NUMBUFLEN];

    ఒకవేళ (sig == SIGKILL)
	తిరిగి vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    కొరకు (i = 0; signal_info[i].sig != -1; i++)
	ఒకవేళ (sig == signal_info[i].sig)
	    తిరిగి strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((అక్షరం *)numbuf, NUMBUFLEN, "%d", sig);
    తిరిగి vim_strnsave(numbuf, i);
}

    అక్షరం *
mch_పని_స్థితి(పని_T *పని)
{
    పూర్ణాంకం		స్థితి_2 = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(పని->jv_pid, &స్థితి_2, WNOHANG);
    ఒకవేళ (wait_pid == -1)
    {
	పూర్ణాంకం waitpid_errno = errno;
	ఒకవేళ (waitpid_errno == ECHILD && mch_process_running(పని->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    తిరిగి "run";

	// process must have exited
	ఒకవేళ (పని->jv_స్థితి < పని_ENDED)
	    ch_log(పని->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	వెళ్ళు return_dead;
    }
    ఒకవేళ (wait_pid == 0)
	తిరిగి "run";
    ఒకవేళ (WIFEXITED(స్థితి_2))
    {
	// LINTED avoid "bitwise operation on signed value"
	పని->jv_exitval = WEXITSTATUS(స్థితి_2);
	ఒకవేళ (పని->jv_స్థితి < పని_ENDED)
	    ch_log(పని->jv_channel, "Job exited with %d", పని->jv_exitval);
	వెళ్ళు return_dead;
    }
    ఒకవేళ (WIFSIGNALED(స్థితి_2))
    {
	పని->jv_exitval = -1;
	పని->jv_termsig = get_signal_name(WTERMSIG(స్థితి_2));
	ఒకవేళ (పని->jv_స్థితి < పని_ENDED && పని->jv_termsig != ఖాళీ)
	    ch_log(పని->jv_channel, "Job terminated by signal \"%s\"",
							      పని->jv_termsig);
	వెళ్ళు return_dead;
    }
    తిరిగి "run";

return_dead:
    ఒకవేళ (పని->jv_స్థితి < పని_ENDED)
	పని->jv_స్థితి = పని_ENDED;
    తిరిగి "dead";
}

    పని_T *
mch_detect_ended_పని(పని_T *పని_list)
{
    పూర్ణాంకం		స్థితి_2 = -1;
    pid_t	wait_pid = 0;
    పని_T	*పని;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    ఒకవేళ (dont_check_పని_ended > 0)
	తిరిగి ఖాళీ;
# endif

    wait_pid = waitpid(-1, &స్థితి_2, WNOHANG);
    ఒకవేళ (wait_pid <= 0)
	// no process ended
	తిరిగి ఖాళీ;
    కొరకు (పని = పని_list; పని != ఖాళీ; పని = పని->jv_తదుపరి)
    {
	ఒకవేళ (పని->jv_pid == wait_pid)
	{
	    ఒకవేళ (WIFEXITED(స్థితి_2))
		// LINTED avoid "bitwise operation on signed value"
		పని->jv_exitval = WEXITSTATUS(స్థితి_2);
	    లేకపోతే ఒకవేళ (WIFSIGNALED(స్థితి_2))
	    {
		పని->jv_exitval = -1;
		పని->jv_termsig = get_signal_name(WTERMSIG(స్థితి_2));
	    }
	    ఒకవేళ (పని->jv_స్థితి < పని_ENDED)
	    {
		ch_log(పని->jv_channel, "Job ended");
		పని->jv_స్థితి = పని_ENDED;
	    }
	    తిరిగి పని;
	}
    }
    తిరిగి ఖాళీ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    పూర్ణాంకం
mch_signal_పని(పని_T *పని, char_u *how)
{
    పూర్ణాంకం	    sig = -1;

    ఒకవేళ (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    లేకపోతే ఒకవేళ (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    లేకపోతే ఒకవేళ (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    లేకపోతే ఒకవేళ (STRCMP(how, "int") == 0)
	sig = SIGINT;
    లేకపోతే ఒకవేళ (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    లేకపోతే ఒకవేళ (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    లేకపోతే ఒకవేళ (సురక్షిత_isdigit(*how))
	sig = atoi((అక్షరం *)how);
    లేకపోతే
	తిరిగి FAIL;

    // Never kill ourselves!
    ఒకవేళ (పని->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-పని->jv_pid, sig);
	kill(పని->jv_pid, sig);
    }

    తిరిగి OK;
}

/*
 * Clear the data related to "job".
 */
    శూన్య
mch_clear_పని(పని_T *పని)
{
    // call waitpid because child process may become zombie
    (శూన్య)waitpid(పని->jv_pid, ఖాళీ, WNOHANG);
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_TERMINAL)
    పూర్ణాంకం
mch_create_pty_channel(పని_T *పని, jobopt_T *options)
{
    పూర్ణాంకం		pty_master_fd = -1;
    పూర్ణాంకం		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &పని->jv_tty_బహిర్గతం, &పని->jv_tty_లోపల);
    ఒకవేళ (pty_master_fd < 0 || pty_slave_fd < 0)
	తిరిగి FAIL;
    close(pty_slave_fd);

    channel = కూడు_channel();
    ఒకవేళ (channel == ఖాళీ)
    {
	close(pty_master_fd);
	తిరిగి FAIL;
    }
    ఒకవేళ (పని->jv_tty_బహిర్గతం != ఖాళీ)
	ch_log(channel, "using pty %s on fd %d",
					       పని->jv_tty_బహిర్గతం, pty_master_fd);
    పని->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_సమితి_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_సమితి_పని(channel, పని, options);
    తిరిగి OK;
}
#అయితే_ముగింపు

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    శూన్య
mch_breakcheck(పూర్ణాంకం force)
{
    ఒకవేళ ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ఖాళీ, ఖాళీ))
	fill_ఆదానం_buf(FALSE);
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
    స్థిర పూర్ణాంకం
WaitForChar(దీర్ఘ msec, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం)
{
#నిర్వచితమైతే FEAT_TIMERS
    తిరిగి ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_ఆదానం) == OK;
#లేకపోతే
    తిరిగి WaitForCharOrMouse(msec, interrupted, ignore_ఆదానం);
#అయితే_ముగింపు
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
    స్థిర పూర్ణాంకం
WaitForCharOrMouse(దీర్ఘ msec, పూర్ణాంకం *interrupted, పూర్ణాంకం ignore_ఆదానం)
{
#నిర్వచితమైతే FEAT_MOUSE_GPM
    పూర్ణాంకం		gpm_process_wanted;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_XCLIPBOARD
    పూర్ణాంకం		rest;
#అయితే_ముగింపు
    పూర్ణాంకం		avail;

    ఒకవేళ (!ignore_ఆదానం && ఆదానం_అందుబాటు())	    // something in inbuf[]
	తిరిగి 1;

#ఒకవేళ defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    ఒకవేళ (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	ఒకవేళ (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#అయితే_ముగింపు

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#ఒకవేళ defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    ఒకవేళ (do_xterm_trace())
	rest = msec;
# endif
    చేయి
    {
# ifdef FEAT_XCLIPBOARD
	ఒకవేళ (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    ఒకవేళ (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    ఒకవేళ (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	ఒకవేళ (has_sound_callback_లోపల_వరుస())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	ఒకవేళ (!avail && !gpm_process_wanted)
# లేకపోతే
	avail = RealWaitForChar(read_cmd_fd, msec, ఖాళీ, interrupted);
	ఒకవేళ (!avail)
# endif
	{
	    ఒకవేళ (!ignore_ఆదానం && ఆదానం_అందుబాటు())
		తిరిగి 1;
# ifdef FEAT_XCLIPBOARD
	    ఒకవేళ (rest == 0 || !do_xterm_trace())
# endif
		ఆపు;
	}
    }
    అయితే (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#లేకపోతే
    avail = RealWaitForChar(read_cmd_fd, msec, ఖాళీ, interrupted);
#అయితే_ముగింపు
    తిరిగి avail;
}

#నిర్వచితం_కాకపోతే VMS
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
    స్థిర పూర్ణాంకం
RealWaitForChar(పూర్ణాంకం fd, దీర్ఘ msec, పూర్ణాంకం *check_for_gpm వాడని, పూర్ణాంకం *interrupted)
{
    పూర్ణాంకం		ret;
    పూర్ణాంకం		ఫలితం;
# ఒకవేళ defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    స్థిర పూర్ణాంకం	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    దీర్ఘ	ప్రారంభం_msec = msec;
    elapsed_T	ప్రారంభం_tv;

    ఒకవేళ (msec > 0)
	ELAPSED_INIT(ప్రారంభం_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    ఒకవేళ (busy)
	తిరిగి 0;
# endif

# ifdef MAY_LOOP
    కొరకు (;;)
# endif
    {
# ifdef MAY_LOOP
	పూర్ణాంకం		ముగిసిన = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	పూర్ణాంకం		mzquantum_వాడిన = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	పూర్ణాంకం		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	నిర్మాణం pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	పూర్ణాంకం		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	పూర్ణాంకం             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	పూర్ణాంకం		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	పూర్ణాంకం		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	పూర్ణాంకం		xsmp_idx = -1;
#  endif
	పూర్ణాంకం		towait = (పూర్ణాంకం)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_దారాలు();
	ఒకవేళ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (పూర్ణాంకం)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_వాడిన = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	ఒకవేళ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	ఒకవేళ (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_సంఖ్య(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ఒకవేళ (check_for_gpm != ఖాళీ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	ఒకవేళ (xsmp_icefd != -1)
	{
	    xsmp_idx = nfd;
	    fds[nfd].fd = xsmp_icefd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_పని_CHANNEL
	nfd = channel_poll_అమరిక(nfd, &fds, &towait);
#  endif
	ఒకవేళ (interrupted != ఖాళీ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	ఫలితం = ret > 0 && (fds[0].revents & POLLIN);
	ఒకవేళ (ఫలితం == 0 && interrupted != ఖాళీ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	ఒకవేళ (ret == 0 && mzquantum_వాడిన)
	    // MzThreads scheduling is required and timeout occurred
	    ముగిసిన = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ఒకవేళ (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	ఒకవేళ (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    ఒకవేళ (--ret == 0 && !ఆదానం_అందుబాటు())
		// Try again
		ముగిసిన = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ఒకవేళ (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	ఒకవేళ (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    ఒకవేళ (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_అభ్యర్థనలు();
		busy = FALSE;
	    }
	    లేకపోతే ఒకవేళ (fds[xsmp_idx].revents & POLLHUP)
	    {
		ఒకవేళ (p_verbose > 0)
		    verb_సందేశం(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    ఒకవేళ (--ret == 0)
		ముగిసిన = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_పని_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ఒకవేళ (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# లేకపోతే // HAVE_SELECT

	నిర్మాణం timeval  tv;
	నిర్మాణం timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	స్థిర fd_set	rfds, wfds, efds;
	పూర్ణాంకం		maxfd;
	దీర్ఘ		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_దారాలు();
	ఒకవేళ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_వాడిన = TRUE;
	}
#  endif

	ఒకవేళ (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	లేకపోతే
	    tvp = ఖాళీ;

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
	ఒకవేళ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    ఒకవేళ (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	ఒకవేళ (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_సంఖ్య(xterm_dpy), &rfds);
	    ఒకవేళ (maxfd < Connection_సంఖ్య(xterm_dpy))
		maxfd = Connection_సంఖ్య(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ఒకవేళ (check_for_gpm != ఖాళీ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    ఒకవేళ (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	ఒకవేళ (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    ఒకవేళ (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_పని_CHANNEL
	maxfd = channel_select_అమరిక(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	ఒకవేళ (interrupted != ఖాళీ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	ఫలితం = ret > 0 && FD_ISSET(fd, &rfds);
	ఒకవేళ (ఫలితం)
	    --ret;
	లేకపోతే ఒకవేళ (interrupted != ఖాళీ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	ఒకవేళ (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    ఒకవేళ (got_tstp && !లోపల_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    ఒకవేళ (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ఖాళీ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    వెళ్ళు select_eintr;
	}
#  endif
#  ifdef __TANDEM
	ఒకవేళ (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	ఒకవేళ (ret == 0 && mzquantum_వాడిన)
	    // loop if MzThreads must be scheduled and timeout occurred
	    ముగిసిన = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ఒకవేళ (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	ఒకవేళ (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_సంఖ్య(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    ఒకవేళ (--ret == 0 && !ఆదానం_అందుబాటు())
	    {
		// Try again
		ముగిసిన = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ఒకవేళ (ret > 0 && check_for_gpm != ఖాళీ && gpm_flag && gpm_fd >= 0)
	{
	    ఒకవేళ (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    లేకపోతే ఒకవేళ (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	ఒకవేళ (ret > 0 && xsmp_icefd != -1)
	{
	    ఒకవేళ (FD_ISSET(xsmp_icefd, &efds))
	    {
		ఒకవేళ (p_verbose > 0)
		    verb_సందేశం(_("XSMP lost ICE connection"));
		xsmp_close();
		ఒకవేళ (--ret == 0)
		    ముగిసిన = FALSE;   // keep going if event was only one
	    }
	    లేకపోతే ఒకవేళ (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_అభ్యర్థనలు();
		busy = FALSE;
		ఒకవేళ (--ret == 0)
		    ముగిసిన = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_పని_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ఒకవేళ (ret >= 0)
	    (శూన్య)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	ఒకవేళ (ముగిసిన || msec == 0)
	    ఆపు;

#  ఒకవేళ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	ఒకవేళ (clientserver_పద్ధతి == CLIENTSERVER_పద్ధతి_X11 && server_నిరీక్షణ())
	    ఆపు;
#  endif

	// We're going to loop around again, find out for how long
	ఒకవేళ (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = ప్రారంభం_msec - ELAPSED_FUNC(ప్రారంభం_tv);
#  లేకపోతే
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    ఒకవేళ (msec <= 0)
		ఆపు;	// waited long enough
	}
# endif
    }

    తిరిగి ఫలితం;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    పూర్ణాంకం
mch_expandpath(
    garray_T	*gap,
    char_u	*మార్గం,
    పూర్ణాంకం		flags)		// EW_* flags
{
    తిరిగి unix_expandpath(gap, మార్గం, 0, flags, FALSE);
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

    పూర్ణాంకం
mch_expand_wildcards(
    పూర్ణాంకం		   అంకె_pat,
    char_u	 **pat,
    పూర్ణాంకం		  *అంకె_file,
    char_u	***file,
    పూర్ణాంకం		   flags)	// EW_* flags
{
    పూర్ణాంకం		i;
    size_t	len;
    దీర్ఘ	llen;
    char_u	*p;
    పూర్ణాంకం		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    పూర్ణాంకం		j;
    char_u	*tempname;
    size_t	tempnamelen;
    char_u	*command;
    size_t	commandlen;
    FILE	*fd;
    char_u	*బఫరు;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    పూర్ణాంకం		shell_style = STYLE_ECHO;
    పూర్ణాంకం		check_spaces;
    స్థిర పూర్ణాంకం	did_కనుగొను_nul = FALSE;
    పూర్ణాంకం		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    స్థిర string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    స్థిర string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *అంకె_file = 0;	// default: no files found
    *file = ఖాళీ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    ఒకవేళ (!have_wildcard(అంకె_pat, pat))
	తిరిగి save_patterns(అంకె_pat, pat, అంకె_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    ఒకవేళ (sandbox != 0 && check_secure())
	తిరిగి FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    ఒకవేళ (secure || restricted)
	కొరకు (i = 0; i < అంకె_pat; ++i)
	    ఒకవేళ (vim_strchr(pat[i], '`') != ఖాళీ
		    && (check_restricted() || check_secure()))
		తిరిగి FAIL;

    /*
     * get a name for the temp file
     */
    ఒకవేళ ((tempname = vim_tempname('o', FALSE)) == ఖాళీ)
    {
	emsg(_(e_cant_get_తాత్కాలిక_file_name));
	తిరిగి FAIL;
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
    ఒకవేళ (అంకె_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    లేకపోతే ఒకవేళ ((len = STRLEN(p_sh)) >= 3)
    {
	ఒకవేళ (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	లేకపోతే ఒకవేళ (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    ఒకవేళ (shell_style == STYLE_ECHO)
    {
	ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "bash") != ఖాళీ)
	    shell_style = STYLE_GLOBSTAR;
	లేకపోతే ఒకవేళ (అక్షరశ్రేణి_శోధన((అక్షరం *)gettail(p_sh), "sh") != ఖాళీ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    ఒకవేళ (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.నిడివి;
    లేకపోతే ఒకవేళ (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.నిడివి + sh_globstar_opt.నిడివి;

    కొరకు (i = 0; i < అంకె_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# లేకపోతే
	++len;				// add space
	కొరకు (j = 0; pat[i][j] != NUL; ++j)
	{
	    ఒకవేళ (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ఖాళీ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    ఒకవేళ (command == ఖాళీ)
    {
	// out of memory
	vim_free(tempname);
	తిరిగి FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    ఒకవేళ (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((అక్షరం *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)అక్షరశ్రేణి_శోధన((అక్షరం *)command, ")>");
	ఒకవేళ (p == ఖాళీ)
	{
	    ;					    // can't happen ;-)
	}
	లేకపోతే
	{
	    --p;
	    అయితే (p > command && VIM_ISWHITE(*p))
		--p;
	    ఒకవేళ (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    అయితే (p > command && VIM_ISWHITE(*p))
		--p;
	    ఒకవేళ (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    లేకపోతే
    {
	ఒకవేళ (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((అక్షరం *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	లేకపోతే ఒకవేళ (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((అక్షరం *)command, len, "print -N >%s", tempname);
	లేకపోతే ఒకవేళ (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((అక్షరం *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	లేకపోతే ఒకవేళ (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((అక్షరం *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	లేకపోతే
	    commandlen = vim_snprintf((అక్షరం *)command, len, "echo >%s", tempname);

	కొరకు (i = 0; i < అంకె_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((అక్షరం *)command + commandlen, len, " \"%s\"", pat[i]);
# లేకపోతే
	    పూర్ణాంకం intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    కొరకు (j = 0; pat[i][j] != NUL; ++j)
	    {
		ఒకవేళ (pat[i][j] == '`')
		    intick = !intick;
		లేకపోతే ఒకవేళ (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    ఒకవేళ (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ఖాళీ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		లేకపోతే ఒకవేళ (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ఖాళీ)
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
    ఒకవేళ (flags & EW_SILENT)
	show_shell_mess = FALSE;
    ఒకవేళ (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    ఒకవేళ (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    లేకపోతే ఒకవేళ (shell_style == STYLE_GLOB && !have_dollars(అంకె_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    ఒకవేళ (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ఖాళీ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    ఒకవేళ (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	ఒకవేళ (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    సందేశం_putchar('\n');		// clear bottom line quickly
	    cmdline_అడ్డువరుస = అడ్డువరుసలు - 1;	// continue on last line
# ifdef USE_SYSTEM
	    ఒకవేళ (!(flags & EW_SILENT))
# endif
	    {
		సందేశం(_(e_cannot_expand_wildcards));
		సందేశం_ప్రారంభం();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	ఒకవేళ (shell_style == STYLE_BT)
	    తిరిగి FAIL;
	వెళ్ళు notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((అక్షరం *)tempname, READBIN);
    ఒకవేళ (fd == ఖాళీ)
    {
	// Something went wrong, perhaps a file name with a special char.
	ఒకవేళ (!(flags & EW_SILENT))
	{
	    సందేశం(_(e_cannot_expand_wildcards));
	    సందేశం_ప్రారంభం();		// don't overwrite this message
	}
	vim_free(tempname);
	వెళ్ళు notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    ఒకవేళ (llen < 0)
	// just in case ftell() would fail
	బఫరు = ఖాళీ;
    లేకపోతే
	బఫరు = alloc(llen + 1);
    ఒకవేళ (బఫరు == ఖాళీ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	తిరిగి FAIL;
    }
    len = llen;
    i = fread((అక్షరం *)బఫరు, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    ఒకవేళ (i != (పూర్ణాంకం)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(బఫరు);
	తిరిగి FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = బఫరు;
    కొరకు (i = 0; i < (పూర్ణాంకం)len; ++i)
	ఒకవేళ (!(బఫరు[i] == CAR && బఫరు[i + 1] == NL))
	    *p++ = బఫరు[i];
    len = p - బఫరు;
# endif


    // file names are separated with Space
    ఒకవేళ (shell_style == STYLE_ECHO)
    {
	బఫరు[len] = '\n';		// make sure the buffer ends in NL
	p = బఫరు;
	కొరకు (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    అయితే (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    లేకపోతే ఒకవేళ (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	బఫరు[len] = NUL;		// make sure the buffer ends in NUL
	p = బఫరు;
	కొరకు (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    అయితే (*p != '\n' && *p != NUL)
		++p;
	    ఒకవేళ (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    లేకపోతే
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
	ఒకవేళ (shell_style == STYLE_PRINT && !did_కనుగొను_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    బఫరు[len] = NUL;
	    ఒకవేళ (len && (పూర్ణాంకం)STRLEN(బఫరు) < (పూర్ణాంకం)len)
		did_కనుగొను_nul = TRUE;
	    లేకపోతే
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	ఒకవేళ (len && బఫరు[len - 1] == NUL)
	    --len;
	లేకపోతే
	    బఫరు[len] = NUL;
	i = 0;
	కొరకు (p = బఫరు; p < బఫరు + len; ++p)
	    ఒకవేళ (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	ఒకవేళ (len)
	    ++i;			// count last entry
    }
    ఒకవేళ (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(బఫరు);
	వెళ్ళు notfound;
    }
    *అంకె_file = i;
    *file = ALLOC_MULT(char_u *, i);
    ఒకవేళ (*file == ఖాళీ)
    {
	// out of memory
	vim_free(బఫరు);
	తిరిగి FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = బఫరు;
    కొరకు (i = 0; i < *అంకె_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	ఒకవేళ (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    అయితే (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    ఒకవేళ (p == బఫరు + len)		// last entry
		*p = NUL;
	    లేకపోతే
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	లేకపోతే		// NUL separates
	{
	    అయితే (*p && p < బఫరు + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    కొరకు (j = 0, i = 0; i < *అంకె_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	ఒకవేళ (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    కొనసాగు;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	ఒకవేళ ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    కొనసాగు;

	// Skip files that are not executable if we check for that.
	ఒకవేళ (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ఖాళీ, !(flags & EW_SHELLCMD)))
	    కొనసాగు;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	ఒకవేళ (p)
	{
	    STRCPY(p, (*file)[i]);
	    ఒకవేళ (dir)
		కూడు_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(బఫరు);
    *అంకె_file = j;

    ఒకవేళ (*అంకె_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	వెళ్ళు notfound;
    }

    తిరిగి OK;

notfound:
    ఒకవేళ (flags & EW_NOTFOUND)
	తిరిగి save_patterns(అంకె_pat, pat, అంకె_file, file);
    తిరిగి FAIL;
}

#అయితే_ముగింపు // VMS

    స్థిర పూర్ణాంకం
save_patterns(
    పూర్ణాంకం		అంకె_pat,
    char_u	**pat,
    పూర్ణాంకం		*అంకె_file,
    char_u	***file)
{
    పూర్ణాంకం		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, అంకె_pat);
    ఒకవేళ (*file == ఖాళీ)
	తిరిగి FAIL;
    కొరకు (i = 0; i < అంకె_pat; i++)
    {
	s = vim_strsave(pat[i]);
	ఒకవేళ (s != ఖాళీ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *అంకె_file = అంకె_pat;
    తిరిగి OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    పూర్ణాంకం
mch_has_exp_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే
	    ఒకవేళ (vim_strchr((char_u *)
#నిర్వచితమైతే VMS
				    "*?%"
#లేకపోతే
				    "*?[{'"
#అయితే_ముగింపు
						, *p) != ఖాళీ)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    పూర్ణాంకం
mch_has_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే
	    ఒకవేళ (vim_strchr((char_u *)
#నిర్వచితమైతే VMS
				    "*?%$"
#లేకపోతే
				    "*?[{`'$"
#అయితే_ముగింపు
						, *p) != ఖాళీ
		|| (*p == '~' && p[1] != NUL))
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    స్థిర పూర్ణాంకం
have_wildcard(పూర్ణాంకం అంకె, char_u **file)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < అంకె; i++)
	ఒకవేళ (mch_has_wildcard(file[i]))
	    తిరిగి 1;
    తిరిగి 0;
}

    స్థిర పూర్ణాంకం
have_dollars(పూర్ణాంకం అంకె, char_u **file)
{
    పూర్ణాంకం	    i;

    కొరకు (i = 0; i < అంకె; i++)
	ఒకవేళ (vim_strchr(file[i], '$') != ఖాళీ)
	    తిరిగి TRUE;
    తిరిగి FALSE;
}

#ఒకవేళ !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    పూర్ణాంకం
mch_rename(స్థిరరాశి అక్షరం *src, స్థిరరాశి అక్షరం *dest)
{
    నిర్మాణం stat	    st;

    ఒకవేళ (stat(dest, &st) >= 0)	    // fail if destination exists
	తిరిగి -1;
    ఒకవేళ (link(src, dest) != 0)	    // link file to new name
	తిరిగి -1;
    ఒకవేళ (mch_remove(src) == 0)	    // delete link to old name
	తిరిగి 0;
    తిరిగి -1;
}
#అయితే_ముగింపు // !HAVE_RENAME

#ఒకవేళ defined(FEAT_MOUSE_GPM)
# ఒకవేళ defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    స్థిర పూర్ణాంకం
ఎక్కించు_libgpm(శూన్య)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    ఒకవేళ (libgpm_hinst == ఖాళీ)
    {
	ఒకవేళ (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	తిరిగి FAIL;
    }

    ఒకవేళ (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ఖాళీ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ఖాళీ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ఖాళీ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ఖాళీ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ఖాళీ
      )
    {
	semsg(_(e_could_not_ఎక్కించు_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ఖాళీ;
	dll_gpm_flag = ఖాళీ;
	dll_gpm_fd   = ఖాళీ;
	తిరిగి FAIL;
    }
    తిరిగి OK;
}

    పూర్ణాంకం
gpm_అందుబాటు(శూన్య)
{
    తిరిగి libgpm_hinst != ఖాళీ || ఎక్కించు_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    స్థిర పూర్ణాంకం
gpm_open(శూన్య)
{
    స్థిర Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    ఒకవేళ (!gpm_అందుబాటు())
	తిరిగి 0;
# endif

    ఒకవేళ (gpm_flag)
	తిరిగి 1; // already open

    gpm_connect.event_ముసుగు_2 = (GPM_పైకి | GPM_DRAG | GPM_కిందికి);
    gpm_connect.default_ముసుగు = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    ఒకవేళ (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	తిరిగి 1; // succeed
    }
    ఒకవేళ (gpm_fd == -2)
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
    తిరిగి 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    పూర్ణాంకం
gpm_enabled(శూన్య)
{
    తిరిగి gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    స్థిర శూన్య
gpm_close(శూన్య)
{
    ఒకవేళ (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    స్థిర పూర్ణాంకం
mch_gpm_process(శూన్య)
{
    పూర్ణాంకం			button;
    స్థిర Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    పూర్ణాంకం			అడ్డువరుస,నిలువు;
    గుర్తులేని అక్షరం	buttons_ముసుగు;
    గుర్తులేని అక్షరం	gpm_modifiers;
    స్థిర గుర్తులేని అక్షరం old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి 0;
# endif

    అడ్డువరుస = gpm_event.y - 1;
    నిలువు = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    ఎంపిక (GPM_BARE_EVENTS(gpm_event.type))
    {
	స్థితి GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ఆపు;
	స్థితి GPM_కిందికి:
	    buttons_ముసుగు = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    ఎంపిక (buttons_ముసుగు)
	    {
		స్థితి GPM_B_ఎడమ:
		    button = MOUSE_ఎడమ;
		    ఆపు;
		స్థితి GPM_B_మధ్య:
		    button = MOUSE_మధ్య;
		    ఆపు;
		స్థితి GPM_B_కుడి:
		    button = MOUSE_కుడి;
		    ఆపు;
		అప్రమేయ:
		    తిరిగి 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    సమితి_అంకె_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ఆపు;
	స్థితి GPM_పైకి:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ఆపు;
	అప్రమేయ:
	    తిరిగి 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    ఒకవేళ (gpm_modifiers & ((1 << KG_జరుపు) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_జరుపు;

    ఒకవేళ (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    ఒకవేళ (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(నిలువు + ' ' + 1);
    string[5] = (char_u)(అడ్డువరుస + ' ' + 1);
    కూడు_గమ్యం_ఆదానం_buf(string, 6);
    తిరిగి 6;
}
#అయితే_ముగింపు // FEAT_MOUSE_GPM

#నిర్వచితమైతే FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    స్థిర పూర్ణాంకం
sysmouse_open(శూన్య)
{
    నిర్మాణం mouse_info   mouse;

    mouse.operation = MOUSE_బహుళకం;
    mouse.u.బహుళకం.బహుళకం = 0;
    mouse.u.బహుళకం.సంకేతం = SIGUSR2;
    ఒకవేళ (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	తిరిగి FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    తిరిగి OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    స్థిర శూన్య
sysmouse_close(శూన్య)
{
    నిర్మాణం mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_బహుళకం;
    mouse.u.బహుళకం.బహుళకం = 0;
    mouse.u.బహుళకం.సంకేతం = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    స్థిర శూన్య
sig_sysmouse SIGDEFARG(sigarg)
{
    నిర్మాణం mouse_info	mouse;
    నిర్మాణం video_info	video;
    char_u		string[6];
    పూర్ణాంకం			అడ్డువరుస, నిలువు;
    పూర్ణాంకం			button;
    పూర్ణాంకం			buttons;
    స్థిర పూర్ణాంకం		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ఒకవేళ (hold_gui_events)
	తిరిగి;
# endif

    mouse.operation = MOUSE_GETINFO;
    ఒకవేళ (ioctl(1, FBIO_GETMODE, &video.vi_బహుళకం) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	అడ్డువరుస = mouse.u.దత్తాంశం.y / video.vi_cheight;
	నిలువు = mouse.u.దత్తాంశం.x / video.vi_cwidth;
	buttons = mouse.u.దత్తాంశం.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	ఒకవేళ (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	లేకపోతే
	{
	    ఎంపిక (buttons)
	    {
		స్థితి 0:
		    button = MOUSE_RELEASE;
		    ఆపు;
		స్థితి 1:
		    button = MOUSE_ఎడమ;
		    ఆపు;
		స్థితి 2:
		    button = MOUSE_మధ్య;
		    ఆపు;
		స్థితి 4:
		    button = MOUSE_కుడి;
		    ఆపు;
		అప్రమేయ:
		    తిరిగి;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(నిలువు + ' ' + 1);
	string[5] = (char_u)(అడ్డువరుస + ' ' + 1);
	కూడు_గమ్యం_ఆదానం_buf(string, 6);
    }
    తిరిగి;
}
#అయితే_ముగింపు // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    పూర్ణాంకం
mch_get_random(char_u *buf, పూర్ణాంకం len)
{
    స్థిర పూర్ణాంకం dev_urandom_state = NOTDONE;

    ఒకవేళ (dev_urandom_state == FAIL)
	తిరిగి FAIL;

    పూర్ణాంకం fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    ఒకవేళ (fd == -1)
	dev_urandom_state = FAIL;
    లేకపోతే ఒకవేళ (పఠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    లేకపోతే
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    తిరిగి dev_urandom_state;
}

#ఒకవేళ defined(FEAT_LIBCALL)
రకనిర్వచనం char_u * (*STRPROCSTR)(char_u *);
రకనిర్వచనం char_u * (*INTPROCSTR)(పూర్ణాంకం);
రకనిర్వచనం పూర్ణాంకం (*STRPROCINT)(char_u *);
రకనిర్వచనం పూర్ణాంకం (*INTPROCINT)(పూర్ణాంకం);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    పూర్ణాంకం
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    పూర్ణాంకం		argint,
    char_u	**string_ఫలితం, // NULL when using number_result
    పూర్ణాంకం		*సంఖ్య_ఫలితం)
{
# ఒకవేళ defined(USE_DLOPEN)
    శూన్య	*hinstLib;
    అక్షరం	*dlerr = ఖాళీ;
# లేకపోతే
    shl_t	hinstLib;
# endif
    STRPROCSTR	Proc_కూడు;
    INTPROCSTR	Proc_కూడు_I;
    char_u	*retval_str = ఖాళీ;
    పూర్ణాంకం		retval_int = 0;
    పూర్ణాంకం		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# ఒకవేళ defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (శూన్య)dlerror();

    hinstLib = dlopen((అక్షరం *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    ఒకవేళ (hinstLib == ఖాళీ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	ఒకవేళ (dlerr != ఖాళీ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# లేకపోతే
    hinstLib = shl_ఎక్కించు((స్థిరరాశి అక్షరం*)libname, BIND_తక్షణ|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    ఒకవేళ (hinstLib != ఖాళీ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	ఒకవేళ (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  ఒకవేళ defined(USE_DLOPEN)
	    dlerr = ఖాళీ;
#  endif
	    mch_didjmp();
	}
	లేకపోతే
# endif
	{
	    retval_str = ఖాళీ;
	    retval_int = 0;

	    ఒకవేళ (argstring != ఖాళీ)
	    {
# ఒకవేళ defined(USE_DLOPEN)
		*(శూన్య **)(&Proc_కూడు) = dlsym(hinstLib, (స్థిరరాశి అక్షరం *)funcname);
		dlerr = dlerror();
# లేకపోతే
		ఒకవేళ (shl_findsym(&hinstLib, (స్థిరరాశి అక్షరం *)funcname,
					TYPE_PROCEDURE, (శూన్య *)&Proc_కూడు) < 0)
		    Proc_కూడు = ఖాళీ;
# endif
		ఒకవేళ ((success = (Proc_కూడు != ఖాళీ
# ఒకవేళ defined(USE_DLOPEN)
			    && dlerr == ఖాళీ
# endif
			    )))
		{
		    ఒకవేళ (string_ఫలితం == ఖాళీ)
			retval_int = ((STRPROCINT)(శూన్య *)Proc_కూడు)(argstring);
		    లేకపోతే
			retval_str = (Proc_కూడు)(argstring);
		}
	    }
	    లేకపోతే
	    {
# ఒకవేళ defined(USE_DLOPEN)
		*(శూన్య **)(&Proc_కూడు_I) = dlsym(hinstLib, (స్థిరరాశి అక్షరం *)funcname);
		dlerr = dlerror();
# లేకపోతే
		ఒకవేళ (shl_findsym(&hinstLib, (స్థిరరాశి అక్షరం *)funcname,
				       TYPE_PROCEDURE, (శూన్య *)&Proc_కూడు_I) < 0)
		    Proc_కూడు_I = ఖాళీ;
# endif
		ఒకవేళ ((success = (Proc_కూడు_I != ఖాళీ
# ఒకవేళ defined(USE_DLOPEN)
			    && dlerr == ఖాళీ
# endif
			    )))
		{
		    ఒకవేళ (string_ఫలితం == ఖాళీ)
			retval_int = ((INTPROCINT)(శూన్య *)Proc_కూడు_I)(argint);
		    లేకపోతే
			retval_str = (Proc_కూడు_I)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    ఒకవేళ (string_ఫలితం == ఖాళీ)
		*సంఖ్య_ఫలితం = retval_int;
	    లేకపోతే ఒకవేళ (retval_str != ఖాళీ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_ఫలితం = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	ఒకవేళ (lc_signal != 0)
	{
	    పూర్ణాంకం i;

	    // try to find the name of this signal
	    కొరకు (i = 0; signal_info[i].sig != -1; i++)
		ఒకవేళ (lc_signal == signal_info[i].sig)
		    ఆపు;
	    semsg(_(e_got_sig_str_లోపల_libcall), signal_info[i].name);
	}
#  endif
# endif

# ఒకవేళ defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	ఒకవేళ (dlerr != ఖాళీ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(శూన్య)dlclose(hinstLib);
# లేకపోతే
	(శూన్య)shl_unload(hinstLib);
# endif
    }

    ఒకవేళ (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	తిరిగి FAIL;
    }

    తిరిగి OK;
}
#అయితే_ముగింపు

#ఒకవేళ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
స్థిర పూర్ణాంకం	xterm_trace = -1;	// default: disabled
స్థిర పూర్ణాంకం	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    శూన్య
అమరిక_పదం_clip(శూన్య)
{
    పూర్ణాంకం		z = 0;
    అక్షరం	*strp = "";
    Widget	AppShell;

    ఒకవేళ (!x_connect_గమ్యం_server())
	తిరిగి;

    open_app_context();
    ఒకవేళ (app_context != ఖాళీ && xterm_Shell == (Widget)0)
    {
	పూర్ణాంకం (*oldhandler)(Display*, XErrorEvent*);
# ఒకవేళ defined(USING_SETJMP)
	పూర్ణాంకం (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ప్రారంభం_tv;

	ఒకవేళ (p_verbose > 0)
	    ELAPSED_INIT(ప్రారంభం_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# ఒకవేళ defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	ఒకవేళ (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ఖాళీ;
	}
	లేకపోతే
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ఖాళీ, 0, &z, &strp);
	    ఒకవేళ (xterm_dpy != ఖాళీ)
		xterm_dpy_retry_లెక్క = 0;
# ఒకవేళ defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# ఒకవేళ defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(శూన్య)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(శూన్య)XSetErrorHandler(oldhandler);

	ఒకవేళ (xterm_dpy == ఖాళీ)
	{
	    ఒకవేళ (p_verbose > 0)
		verb_సందేశం(_("Opening the X display failed"));
	    తిరిగి;
	}

	// Catch terminating error of the X server connection.
	(శూన్య)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	ఒకవేళ (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_సందేశం(ELAPSED_FUNC(ప్రారంభం_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ఖాళీ);
	ఒకవేళ (AppShell == (Widget)0)
	    తిరిగి;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		శిఖరం_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ఖాళీ);
	ఒకవేళ (xterm_Shell == (Widget)0)
	    తిరిగి;

	x11_అమరిక_atoms(xterm_dpy);
	x11_అమరిక_selection(xterm_Shell);
	ఒకవేళ (x11_display == ఖాళీ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    ఒకవేళ (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	ఒకవేళ (x11_కిటికీ == 0 && (strp = getenv("WINDOWID")) != ఖాళీ)
	    x11_కిటికీ = (కిటికీ)atol(strp);
	// Check if $WINDOWID is valid.
	ఒకవేళ (test_x11_కిటికీ(xterm_dpy) == FAIL)
	    x11_కిటికీ = 0;
	ఒకవేళ (x11_కిటికీ != 0)
	    xterm_trace = 0;
    }
}

    శూన్య
ప్రారంభం_xterm_trace(పూర్ణాంకం button)
{
    ఒకవేళ (x11_కిటికీ == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	తిరిగి;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    శూన్య
stop_xterm_trace(శూన్య)
{
    ఒకవేళ (xterm_trace < 0)
	తిరిగి;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    స్థిర పూర్ణాంకం
do_xterm_trace(శూన్య)
{
    కిటికీ		మూలం, బిడ్డ;
    పూర్ణాంకం			మూలం_x, మూలం_y;
    పూర్ణాంకం			win_x, win_y;
    పూర్ణాంకం			అడ్డువరుస, నిలువు;
    int_u		ముసుగు_return;
    char_u		buf[50];
    char_u		*strp;
    దీర్ఘ		got_hints;
    స్థిర char_u	*mouse_సంకేతం = ఖాళీ;
    స్థిర size_t	mouse_codelen = 0;
    స్థిర char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    స్థిర పూర్ణాంకం		prev_అడ్డువరుస = 0, prev_నిలువు = 0;
    స్థిర XSizeHints	xterm_hints;

    ఒకవేళ (xterm_trace <= 0)
	తిరిగి FALSE;

    ఒకవేళ (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	ఒకవేళ (!XGetWMNormalHints(xterm_dpy, x11_కిటికీ, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.వెడల్పు_inc <= 1
		|| xterm_hints.ఎత్తు_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    తిరిగి FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_సంకేతం = కనుగొను_termcode(mouse_name);
	ఒకవేళ (mouse_సంకేతం != ఖాళీ)
	    mouse_codelen = STRLEN(mouse_సంకేతం);
	prev_అడ్డువరుస = mouse_అడ్డువరుస;
	prev_నిలువు = mouse_నిలువు;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQuery_సూచిక(xterm_dpy, x11_కిటికీ, &మూలం, &బిడ్డ, &మూలం_x, &మూలం_y,
		      &win_x, &win_y, &ముసుగు_return);
	xterm_hints.y = win_y - (xterm_hints.ఎత్తు_inc * mouse_అడ్డువరుస)
			      - (xterm_hints.ఎత్తు_inc / 2);
	ఒకవేళ (xterm_hints.y <= xterm_hints.ఎత్తు_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.వెడల్పు_inc * mouse_నిలువు)
			      - (xterm_hints.వెడల్పు_inc / 2);
	ఒకవేళ (xterm_hints.x <= xterm_hints.వెడల్పు_inc / 2)
	    xterm_hints.x = 2;
	తిరిగి TRUE;
    }

    ఒకవేళ (mouse_సంకేతం == ఖాళీ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	తిరిగి FALSE;
    }

    XQuery_సూచిక(xterm_dpy, x11_కిటికీ, &మూలం, &బిడ్డ, &మూలం_x, &మూలం_y,
		  &win_x, &win_y, &ముసుగు_return);

    అడ్డువరుస = check_అడ్డువరుస((win_y - xterm_hints.y) / xterm_hints.ఎత్తు_inc);
    నిలువు = check_నిలువు((win_x - xterm_hints.x) / xterm_hints.వెడల్పు_inc);
    ఒకవేళ (అడ్డువరుస == prev_అడ్డువరుస && నిలువు == prev_నిలువు)
	తిరిగి TRUE;

    STRCPY(buf, mouse_సంకేతం);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(నిలువు + ' ' + 1);
    *strp++ = (char_u)(అడ్డువరుస + ' ' + 1);
    *strp = NUL;
    కూడు_గమ్యం_ఆదానం_buf(buf, strp - buf);

    prev_అడ్డువరుస = అడ్డువరుస;
    prev_నిలువు = నిలువు;
    తిరిగి TRUE;
}

# ఒకవేళ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    శూన్య
clear_xterm_clip(శూన్య)
{
    ఒకవేళ (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    ఒకవేళ (xterm_dpy != ఖాళీ)
    {
#  ఒకవేళ 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	ఒకవేళ (x11_display == xterm_dpy)
	    x11_display = ఖాళీ;
	xterm_dpy = ఖాళీ;
    }
#  ఒకవేళ 0
    ఒకవేళ (app_context != (XtAppContext)ఖాళీ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ఖాళీ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    స్థిర శూన్య
clip_update(శూన్య)
{
# ifdef FEAT_GUI
    ఒకవేళ (gui.లోపల_use)
	gui_mch_update();
    లేకపోతే
# endif
    ఒకవేళ (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    శూన్య
xterm_update(శూన్య)
{
    XEvent event;

    కొరకు (;;)
    {
	Xt_ఆదానం_ముసుగు ముసుగు = XtAppPending(app_context);

	ఒకవేళ (ముసుగు == 0 || vim_is_ఆదానం_buf_నిండిన())
	    ఆపు;

	ఒకవేళ (ముసుగు & XtIMXEvent)
	{
	    // There is an event to process.
	    Xt_App_తదుపరి_Event(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		ఒకవేళ (e->type == PropertyNotify && e->కిటికీ_2 == comm_కిటికీ
		   && e->atom == commProperty && e->state == Property_New_విలువ)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	లేకపోతే
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, ముసుగు);
	}
    }
}

    పూర్ణాంకం
clip_xterm_own_selection(Clipboard_T *cbd)
{
    ఒకవేళ (xterm_Shell != (Widget)0)
	తిరిగి clip_x11_own_selection(xterm_Shell, cbd);
    తిరిగి FAIL;
}

    శూన్య
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    ఒకవేళ (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    శూన్య
clip_xterm_request_selection(Clipboard_T *cbd)
{
    ఒకవేళ (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    శూన్య
clip_xterm_సమితి_selection(Clipboard_T *cbd)
{
    clip_x11_సమితి_selection(cbd);
}
#అయితే_ముగింపు


#ఒకవేళ defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# ఒకవేళ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    స్థిర శూన్య
xsmp_handle_interaction(SmcConn smc_conn, Sm_సూచిక client_దత్తాంశం_2 వాడని)
{
    పూర్ణాంకం		save_cmod_flags;
    పూర్ణాంకం		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    ఒకవేళ (check_మారింది_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    బహిర్గతం_flush();

    // Done interaction
    Smc_Interact_పూర్తి(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    ఒకవేళ (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	Smc_Save_Yourself_పూర్తి(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    స్థిర శూన్య
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    Sm_సూచిక	client_దత్తాంశం_2 వాడని,
    పూర్ణాంకం		save_type వాడని,
    Bool	shutdown,
    పూర్ణాంకం		interact_style వాడని,
    Bool	fast వాడని)
{
    // Handle already being in saveyourself
    ఒకవేళ (xsmp.save_yourself)
	Smc_Save_Yourself_పూర్తి(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    బహిర్గతం_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    ఒకవేళ (p_verbose > 0)
	verb_సందేశం(_("XSMP handling save-yourself request"));

# ఒకవేళ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    ఒకవేళ (shutdown && !fast && gui.లోపల_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_దత్తాంశం_2);
    లేకపోతే
# endif
    {
	// Can stop the cycle here
	Smc_Save_Yourself_పూర్తి(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    స్థిర శూన్య
xsmp_die(SmcConn smc_conn వాడని, Sm_సూచిక client_దత్తాంశం_2 వాడని)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    స్థిర శూన్య
xsmp_save_complete(
    SmcConn	smc_conn వాడని,
    Sm_సూచిక	client_దత్తాంశం_2 వాడని)
{
    xsmp.save_yourself = False;
}


/*
 * Callback to tell us that an instigated shutdown was cancelled
 * (maybe even by us)
 */
    స్థిర శూన్య
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    Sm_సూచిక	client_దత్తాంశం_2 వాడని)
{
    ఒకవేళ (xsmp.save_yourself)
	Smc_Save_Yourself_పూర్తి(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    స్థిర శూన్య
xsmp_ice_connection(
    IceConn	iceConn,
    Ice_సూచిక	client_దత్తాంశం వాడని,
    Bool	opening,
    Ice_సూచిక	*watch_దత్తాంశం వాడని)
{
    // Intercept creation of ICE connection fd
    ఒకవేళ (opening)
    {
	xsmp_icefd = Ice_Connection_సంఖ్య(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ఖాళీ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    పూర్ణాంకం
xsmp_handle_అభ్యర్థనలు(శూన్య)
{
    Bool rep;

    ఒకవేళ (IceProcessMessages(xsmp.iceconn, ఖాళీ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	ఒకవేళ (p_verbose > 0)
	    verb_సందేశం(_("XSMP lost ICE connection"));
	xsmp_close();
	తిరిగి FAIL;
    }
    లేకపోతే
	తిరిగి OK;
}

స్థిర పూర్ణాంకం dummy;

// Set up X Session Management Protocol
    శూన్య
xsmp_init(శూన్య)
{
    అక్షరం		errorstring[80];
    SmcCallbacks	smcallbacks;
# ఒకవేళ 0
    Sm_Prop_విలువ		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    ఒకవేళ (p_verbose > 0)
	verb_సందేశం(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_దత్తాంశం_2 = ఖాళీ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_దత్తాంశం_2 = ఖాళీ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_దత్తాంశం_2 = ఖాళీ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_దత్తాంశం_2 = ఖాళీ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    ఒకవేళ (Ice_కూడు_Connection_Watch(xsmp_ice_connection, &dummy) == 0)
    {
	ఒకవేళ (p_verbose > 0)
	    verb_సందేశం(_("XSMP ICE connection watch failed"));
	తిరిగి;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ఖాళీ,
	    ఖాళీ,
	    SmProtoMajor,
	    SmProtoMinor,
	    Smc_Save_Yourself_Proc_ముసుగు | Smc_Die_Proc_ముసుగు
		     | Smc_Save_Complete_Proc_ముసుగు | Smc_Shutdown_Cancelled_Proc_ముసుగు,
	    &smcallbacks,
	    ఖాళీ,
	    &xsmp.clientid,
	    పరిమాణం(errorstring) - 1,
	    errorstring);
    ఒకవేళ (xsmp.smcconn == ఖాళీ)
    {
	ఒకవేళ (p_verbose > 0)
	{
	    అక్షరం errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[పరిమాణం(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, పరిమాణం(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_సందేశం(errorreport);
	}
	తిరిగి;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# ఒకవేళ 0
    // ID ourselves
    smname.విలువ = "vim";
    smname.నిడివి = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.అంకె_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    Smc_సమితి_Properties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    శూన్య
xsmp_close(శూన్య)
{
    ఒకవేళ (xsmp_icefd == -1)
	తిరిగి;

    SmcCloseConnection(xsmp.smcconn, 0, ఖాళీ);
    ఒకవేళ (xsmp.clientid != ఖాళీ)
	విడుదల(xsmp.clientid);
    xsmp.clientid = ఖాళీ;
    xsmp_icefd = -1;
}
#అయితే_ముగింపు // USE_XSMP

#ఒకవేళ defined(FEAT_RELTIME)
# ఒకవేళ defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
స్థిర అస్థిర sig_atomic_t timeout_flag = FALSE;
స్థిర timer_t		     timer_id;
స్థిర పూర్ణాంకం		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    స్థిర శూన్య
సమితి_flag(సంఘం sigval వాడని_3 వాడని)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    శూన్య
stop_timeout(శూన్య)
{
    స్థిర నిర్మాణం itimerspec disarm = {{0, 0}, {0, 0}};

    ఒకవేళ (timer_created)
    {
	పూర్ణాంకం ret = timer_settime(timer_id, 0, &disarm, ఖాళీ);

	ఒకవేళ (ret < 0)
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
    అస్థిర sig_atomic_t *
ప్రారంభం_timeout(దీర్ఘ msec)
{
    నిర్మాణం itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    పూర్ణాంకం ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    ఒకవేళ (!timer_created)
    {
	నిర్మాణం sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = సమితి_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	ఒకవేళ (ret < 0)
	{
	    semsg(_(e_could_not_సమితి_timeout_str), strerror(errno));
	    తిరిగి &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ఖాళీ, "setting timeout timer to %d sec %ld nsec",
	       (పూర్ణాంకం)interval.it_value.tv_sec, (దీర్ఘ)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ఖాళీ);
    ఒకవేళ (ret < 0)
	semsg(_(e_could_not_సమితి_timeout_str), strerror(errno));

    తిరిగి &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    శూన్య
delete_timer(శూన్య)
{
    ఒకవేళ (!timer_created)
	తిరిగి;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# లేకపోతే // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
స్థిర నిర్మాణం sigaction		prev_sigaction;
స్థిర అస్థిర sig_atomic_t	timeout_flag	     = FALSE;
స్థిర పూర్ణాంకం			timer_చురుకైన	     = FALSE;
స్థిర పూర్ణాంకం			timer_handler_చురుకైన = FALSE;
స్థిర అస్థిర sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    స్థిర శూన్య
సమితి_flag SIGDEFARG(sigarg)
{
    ఒకవేళ (alarm_pending)
	alarm_pending = FALSE;
    లేకపోతే
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    శూన్య
stop_timeout(శూన్య)
{
    స్థిర నిర్మాణం itimerval disarm = {{0, 0}, {0, 0}};
    పూర్ణాంకం			    ret;

    ఒకవేళ (timer_చురుకైన)
    {
	timer_చురుకైన = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ఖాళీ);
	ఒకవేళ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    ఒకవేళ (timer_handler_చురుకైన)
    {
	timer_handler_చురుకైన = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ఖాళీ);
	ఒకవేళ (ret < 0)
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
    అస్థిర sig_atomic_t *
ప్రారంభం_timeout(దీర్ఘ msec)
{
    నిర్మాణం itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    నిర్మాణం sigaction	handle_alarm;
    పూర్ణాంకం			ret;
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
    ఒకవేళ (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ఖాళీ);
    }
    ఒకవేళ (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = సమితి_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    ఒకవేళ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_సమితి_handler_for_timeout_str), strerror(errno));
	తిరిగి &timeout_flag;
    }
    timer_handler_చురుకైన = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ఖాళీ);
    ఒకవేళ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_సమితి_timeout_str), strerror(errno));
	stop_timeout();
	తిరిగి &timeout_flag;
    }

    timer_చురుకైన = TRUE;
    తిరిగి &timeout_flag;
}
# endif // PROF_NSEC
#అయితే_ముగింపు  // FEAT_RELTIME
