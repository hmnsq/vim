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

#അന്തര്ഭാവ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#അന്തര്ഭാവ "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
സ്ഥിതം പൂർണ്ണസംഖ്യ selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != ശൂന്യ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ശൂന്യ ? *dll_gpm_fd   : -1)

സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_Gpm_Open)     (Gpm_Connect *, പൂർണ്ണസംഖ്യ);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_Gpm_Close)    (ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ (*dll_Gpm_GetEvent) (Gpm_Event *);
സ്ഥിതം പൂർണ്ണസംഖ്യ *dll_gpm_flag;
സ്ഥിതം പൂർണ്ണസംഖ്യ *dll_gpm_fd;

സ്ഥിതം ശൂന്യം *libgpm_hinst;
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

സ്ഥിതം ശൂന്യം gpm_close(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ gpm_open(ശൂന്യം);
സ്ഥിതം പൂർണ്ണസംഖ്യ mch_gpm_process(ശൂന്യം);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

സ്ഥിതം പൂർണ്ണസംഖ്യ sysmouse_open(ശൂന്യം);
സ്ഥിതം ശൂന്യം sysmouse_close(ശൂന്യം);
സ്ഥിതം ശൂന്യം sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#എങ്കിൽ defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#എങ്കിൽ defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
സ്ഥിതം Widget	xterm_Shell = (Widget)0;
സ്ഥിതം ശൂന്യം clip_update(ശൂന്യം);
# endif

Window	    x11_window = 0;
Display	    *x11_display = ശൂന്യ;
#endif

സ്ഥിതം പൂർണ്ണസംഖ്യ ignore_sigtstp = FALSE;

സ്ഥിതം പൂർണ്ണസംഖ്യ get_x11_title(പൂർണ്ണസംഖ്യ);

സ്ഥിതം char_u	*oldtitle = ശൂന്യ;
സ്ഥിതം അസ്ഥിര sig_atomic_t oldtitle_outdated = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ	unix_did_set_title = FALSE;
സ്ഥിതം char_u	*oldicon = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	did_set_icon = FALSE;

സ്ഥിതം ശൂന്യം may_core_dump(ശൂന്യം);

തരനിർവചനം പൂർണ്ണസംഖ്യ waitstatus;
സ്ഥിതം പൂർണ്ണസംഖ്യ  WaitForChar(long msec, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം);
സ്ഥിതം പൂർണ്ണസംഖ്യ  WaitForCharOrMouse(long msec, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം);
#ifdef VMS
പൂർണ്ണസംഖ്യ  RealWaitForChar(പൂർണ്ണസംഖ്യ, long, പൂർണ്ണസംഖ്യ *, പൂർണ്ണസംഖ്യ *interrupted);
#അല്ലെങ്കിൽ
സ്ഥിതം പൂർണ്ണസംഖ്യ  RealWaitForChar(പൂർണ്ണസംഖ്യ, long, പൂർണ്ണസംഖ്യ *, പൂർണ്ണസംഖ്യ *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
സ്ഥിതം പൂർണ്ണസംഖ്യ do_xterm_trace(ശൂന്യം);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

സ്ഥിതം ശൂന്യം handle_resize(ശൂന്യം);

#എങ്കിൽ defined(SIGWINCH)
സ്ഥിതം ശൂന്യം sig_winch SIGPROTOARG;
#endif
#എങ്കിൽ defined(SIGTSTP)
സ്ഥിതം ശൂന്യം sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
സ്ഥിതം അസ്ഥിര sig_atomic_t in_mch_suspend = FALSE;
#endif
#എങ്കിൽ defined(SIGINT)
സ്ഥിതം ശൂന്യം catch_sigint SIGPROTOARG;
#endif
#എങ്കിൽ defined(SIGUSR1)
സ്ഥിതം ശൂന്യം catch_sigusr1 SIGPROTOARG;
#endif
#എങ്കിൽ defined(SIGPWR)
സ്ഥിതം ശൂന്യം catch_sigpwr SIGPROTOARG;
#endif
#എങ്കിൽ defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
സ്ഥിതം ശൂന്യം sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
സ്ഥിതം അസ്ഥിര sig_atomic_t sig_alarm_called;
#endif
സ്ഥിതം ശൂന്യം deathtrap SIGPROTOARG;

സ്ഥിതം ശൂന്യം catch_int_signal(ശൂന്യം);
സ്ഥിതം ശൂന്യം set_signals(ശൂന്യം);
സ്ഥിതം ശൂന്യം catch_signals(ശൂന്യം (*func_deadly)(പൂർണ്ണസംഖ്യ), ശൂന്യം (*func_other)(പൂർണ്ണസംഖ്യ));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#അല്ലെങ്കിൽ
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	ചെയ്യുക { /**/ } ആകുമ്പോൾ (0)
# define UNBLOCK_SIGNALS(set)	ചെയ്യുക { /**/ } ആകുമ്പോൾ (0)
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ  have_wildcard(പൂർണ്ണസംഖ്യ, char_u **);
സ്ഥിതം പൂർണ്ണസംഖ്യ  have_dollars(പൂർണ്ണസംഖ്യ, char_u **);

സ്ഥിതം പൂർണ്ണസംഖ്യ save_patterns(പൂർണ്ണസംഖ്യ അക്കം_pat, char_u **pat, പൂർണ്ണസംഖ്യ *അക്കം_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
സ്ഥിതം അസ്ഥിര sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
സ്ഥിതം അസ്ഥിര sig_atomic_t got_tstp = FALSE;
സ്ഥിതം char_u	*extra_shell_arg = ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
സ്ഥിതം അസ്ഥിര sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
സ്ഥിതം അസ്ഥിര sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#എങ്കിൽ defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
സ്ഥിതം പൂർണ്ണസംഖ്യ dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
സ്ഥിതം tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
തരനിർവചനം ഘടന
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    അക്ഷരം *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

സ്ഥിതം xsmp_config_T xsmp;
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

സ്ഥിതം ഘടന signalinfo
{
    പൂർണ്ണസംഖ്യ	    sig;	// Signal number, eg. SIGSEGV etc
    അക്ഷരം    *name;	// Signal name (not char_u!).
    അക്ഷരം    deadly;	// Catch as a deadly signal?
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
#എങ്കിൽ defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#എങ്കിൽ defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#എങ്കിൽ defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#എങ്കിൽ defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(പൂർണ്ണസംഖ്യ sig, sighandler_T func)
{
#എങ്കിൽ defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ഘടന sigaction	sa, old;
    sigset_t		curset;
    പൂർണ്ണസംഖ്യ			blocked;

    എങ്കിൽ (sigprocmask(SIG_BLOCK, ശൂന്യ, &curset) == -1)
	തിരികെ SIG_ERR;

    blocked = sigismember(&curset, sig);

    എങ്കിൽ (func == SIG_HOLD)
    {
	എങ്കിൽ (blocked)
	    തിരികെ SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	എങ്കിൽ (sigaction(sig, ശൂന്യ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ശൂന്യ) == -1)
	    തിരികെ SIG_ERR;
	തിരികെ old.sa_handler;
    }

    എങ്കിൽ (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	എങ്കിൽ (sigprocmask(SIG_UNBLOCK, &curset, ശൂന്യ) == -1)
	    തിരികെ SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# അല്ലെങ്കിൽ
    sa.sa_flags = 0;
# endif
    എങ്കിൽ (sigaction(sig, &sa, &old) == -1)
	തിരികെ SIG_ERR;
    തിരികെ blocked ? SIG_HOLD: old.sa_handler;
#അല്ലെങ്കിലെങ്കിൽ defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    തിരികെ sigset(sig, func);
#അല്ലെങ്കിൽ
    // Oldest and most compatible solution.
    തിരികെ സൂചന(sig, func);
#endif
}

    പൂർണ്ണസംഖ്യ
mch_chdir(അക്ഷരം *path)
{
    എങ്കിൽ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    തിരികെ chdir(vms_fixfilename(path));
#അല്ലെങ്കിൽ
    തിരികെ chdir(path);
#endif
}

#എങ്കിൽ defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    ശൂന്യം
mch_write(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    vim_ignored = (പൂർണ്ണസംഖ്യ)ലിഖ(1, (അക്ഷരം *)s, len);
    എങ്കിൽ (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ശൂന്യ, ശൂന്യ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
resize_func(പൂർണ്ണസംഖ്യ check_only)
{
    എങ്കിൽ (check_only)
	തിരികെ do_resize;
    ആകുമ്പോൾ (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(ശൂന്യ, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    തിരികെ FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    പൂർണ്ണസംഖ്യ
mch_inchar(
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    പൂർണ്ണസംഖ്യ		tb_change_cnt)
{
    തിരികെ inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    സ്ഥിതം ശൂന്യം
handle_resize(ശൂന്യം)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    പൂർണ്ണസംഖ്യ
mch_char_avail(ശൂന്യം)
{
    തിരികെ WaitForChar(0L, ശൂന്യ, FALSE);
}

#എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    പൂർണ്ണസംഖ്യ
mch_check_messages(ശൂന്യം)
{
    തിരികെ WaitForChar(0L, ശൂന്യ, TRUE);
}
#endif

#എങ്കിൽ defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# എങ്കിൽ defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# എങ്കിൽ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(പൂർണ്ണസംഖ്യ special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ഘടന vm_statistics64	vm_stat;
	natural_t		എണ്ണം = HOST_VM_INFO64_എണ്ണം;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &എണ്ണം);
#  അല്ലെങ്കിൽ
	ഘടന vm_statistics	vm_stat;
	natural_t		എണ്ണം = HOST_VM_INFO_എണ്ണം;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &എണ്ണം);
#  endif
	എങ്കിൽ (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_എണ്ണം + vm_stat.active_എണ്ണം
					    + vm_stat.inactive_എണ്ണം
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_എണ്ണം
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    എങ്കിൽ (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	പൂർണ്ണസംഖ്യ		mib[2];
	size_t		len = വലുപ്പം(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  അല്ലെങ്കിൽ
	// sysctl() may return 32 bit or 64 bit, accept both
	സംഘം {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  അല്ലെങ്കിൽ
	mib[1] = HW_USERMEM;
#  endif
	എങ്കിൽ (sysctl(mib, 2, &physmem, &len, ശൂന്യ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  അല്ലെങ്കിൽ
	    എങ്കിൽ (len == വലുപ്പം(physmem.u64))
		mem = (long_u)physmem.u64;
	    അല്ലെങ്കിൽ
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# എങ്കിൽ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    എങ്കിൽ (mem == 0)
    {
	ഘടന sysinfo sinfo;

	// Linux way of getting amount of RAM available
	എങ്കിൽ (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    ആകുമ്പോൾ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  അല്ലെങ്കിൽ
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    എങ്കിൽ (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	എങ്കിൽ (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    ആകുമ്പോൾ (shiftright > 0 && (pagesize & 1) == 0)
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
# എങ്കിൽ defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ഘടന rlimit	rlp;

	എങ്കിൽ (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (വലുപ്പം(long_u) * 8 - 1))
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

    എങ്കിൽ (mem > 0)
	തിരികെ mem >> shiftright;
    തിരികെ (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    ശൂന്യം
mch_delay(long msec, പൂർണ്ണസംഖ്യ flags)
{
    tmode_T	old_tmode;
    പൂർണ്ണസംഖ്യ		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    എങ്കിൽ (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	എങ്കിൽ (call_settmode)
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
	ചെയ്യുക
	{
	    // if total is large enough, wait by portions in p_mzq
	    എങ്കിൽ (total > p_mzq)
		msec = p_mzq;
	    അല്ലെങ്കിൽ
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    ഘടന timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (ശൂന്യം)nanosleep(&ts, ശൂന്യ);
	}
#അല്ലെങ്കിൽ
# ifdef HAVE_USLEEP
	ആകുമ്പോൾ (msec >= 1000)
	{
	    usleep((unsigned പൂർണ്ണസംഖ്യ)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned പൂർണ്ണസംഖ്യ)(msec * 1000));
# അല്ലെങ്കിൽ
#  ifndef HAVE_SELECT
	poll(ശൂന്യ, 0, (പൂർണ്ണസംഖ്യ)msec);
#  അല്ലെങ്കിൽ
	{
	    ഘടന timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ശൂന്യ, ശൂന്യ, ശൂന്യ, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	ആകുമ്പോൾ (total > 0);
#endif

	എങ്കിൽ (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    അല്ലെങ്കിൽ
	WaitForChar(msec, ശൂന്യ, FALSE);
}

#എങ്കിൽ defined(HAVE_അട്ടി_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_അട്ടി_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ അട്ടി_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    സ്ഥിതം ശൂന്യം
check_അട്ടി_growth(അക്ഷരം *p)
{
    പൂർണ്ണസംഖ്യ		i;

    അട്ടി_grows_downwards = (p > (അക്ഷരം *)&i);
}
#endif

#എങ്കിൽ defined(HAVE_അട്ടി_LIMIT)
സ്ഥിതം അക്ഷരം *അട്ടി_limit = ശൂന്യ;

# എങ്കിൽ defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    സ്ഥിതം ശൂന്യം
get_അട്ടി_limit(ശൂന്യം)
{
    ഘടന rlimit	rlp;
    പൂർണ്ണസംഖ്യ			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    എങ്കിൽ (getrlimit(RLIMIT_അട്ടി, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (വലുപ്പം(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# എങ്കിൽ defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    എങ്കിൽ (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		എങ്കിൽ (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	എങ്കിൽ (അട്ടി_grows_downwards)
	{
	    അട്ടി_limit = (അക്ഷരം *)((long)&i - (lim / 16L * 15L));
	    എങ്കിൽ (അട്ടി_limit >= (അക്ഷരം *)&i)
		// overflow, set to 1/16 of current stack position
		അട്ടി_limit = (അക്ഷരം *)((long)&i / 16L);
	}
	അല്ലെങ്കിൽ
	{
	    അട്ടി_limit = (അക്ഷരം *)((long)&i + (lim / 16L * 15L));
	    എങ്കിൽ (അട്ടി_limit <= (അക്ഷരം *)&i)
		അട്ടി_limit = ശൂന്യ;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    പൂർണ്ണസംഖ്യ
mch_stackcheck(അക്ഷരം *p)
{
    എങ്കിൽ (അട്ടി_limit == ശൂന്യ)
	തിരികെ OK;

    എങ്കിൽ (അട്ടി_grows_downwards)
    {
	എങ്കിൽ (p < അട്ടി_limit)
	    തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p > അട്ടി_limit)
	തിരികെ FAIL;
    തിരികെ OK;
}
#endif

#എങ്കിൽ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
സ്ഥിതം stack_t sigstk;			// for sigaltstack()
# അല്ലെങ്കിൽ
സ്ഥിതം ഘടന sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
സ്ഥിതം long പൂർണ്ണസംഖ്യ get_signal_അട്ടി_size(ശൂന്യം)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long പൂർണ്ണസംഖ്യ size = -1;

    // return size only if sysconf doesn't return an error
    എങ്കിൽ ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	തിരികെ size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    തിരികെ SIGSTKSZ;
# endif

    // otherwise guess the size
    തിരികെ 8000;
}

സ്ഥിതം അക്ഷരം *signal_അട്ടി;

    സ്ഥിതം ശൂന്യം
init_signal_അട്ടി(ശൂന്യം)
{
    എങ്കിൽ (signal_അട്ടി == ശൂന്യ)
	തിരികെ;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_അട്ടി;
#  അല്ലെങ്കിൽ
    sigstk.ss_sp = signal_അട്ടി;
#  endif
    sigstk.ss_size = get_signal_അട്ടി_size();
    sigstk.ss_flags = 0;
    (ശൂന്യം)sigaltstack(&sigstk, ശൂന്യ);
# അല്ലെങ്കിൽ
    sigstk.ss_sp = signal_അട്ടി;
    എങ്കിൽ (അട്ടി_grows_downwards)
	sigstk.ss_sp += get_signal_അട്ടി_size() - 1;
    sigstk.ss_onstack = 0;
    (ശൂന്യം)sigstack(&sigstk, ശൂന്യ);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#എങ്കിൽ defined(SIGWINCH)
    സ്ഥിതം ശൂന്യം
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#എങ്കിൽ defined(SIGTSTP)
    സ്ഥിതം ശൂന്യം
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    എങ്കിൽ (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ഉയർത്തുക(sigarg);
    }
    അല്ലെങ്കിൽ
	got_tstp = TRUE;

# എങ്കിൽ !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#എങ്കിൽ defined(SIGINT)
    സ്ഥിതം ശൂന്യം
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#എങ്കിൽ defined(SIGUSR1)
    സ്ഥിതം ശൂന്യം
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#എങ്കിൽ defined(SIGPWR)
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#എങ്കിൽ defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
സ്ഥിതം JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
സ്ഥിതം അസ്ഥിര sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
സ്ഥിതം അസ്ഥിര sig_atomic_t lc_active = FALSE;

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
    സ്ഥിതം ശൂന്യം
mch_startjmp(ശൂന്യം)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    സ്ഥിതം ശൂന്യം
mch_endjmp(ശൂന്യം)
{
    lc_active = FALSE;
}

    സ്ഥിതം ശൂന്യം
mch_didjmp(ശൂന്യം)
{
# എങ്കിൽ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_അട്ടി();
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
    സ്ഥിതം ശൂന്യം
deathtrap SIGDEFARG(sigarg)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    പൂർണ്ണസംഖ്യ		i;
#endif

#എങ്കിൽ defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    എങ്കിൽ (lc_active)
    {
# എങ്കിൽ defined(SIGHASARG)
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
    എങ്കിൽ (in_mch_delay && sigarg == SIGQUIT)
	തിരികെ;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    എങ്കിൽ (entered == 0
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
	തിരികെ;
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

#ifdef HAVE_അട്ടി_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_അട്ടി_limit();
#endif

#എങ്കിൽ 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	എങ്കിൽ (fp)
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
    വേണ്ടി (i = 0; signal_info[i].sig != -1; i++)
	എങ്കിൽ (sigarg == signal_info[i].sig)
	    നിർത്തുക;
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
    എങ്കിൽ (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	എങ്കിൽ (entered >= 4)
	    _exit(8);
	exit(7);
    }
    എങ്കിൽ (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((അക്ഷരം *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#അല്ലെങ്കിൽ
    sprintf((അക്ഷരം *)IObuff, "Vim: Caught deadly signal\r\n");
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
    സ്ഥിതം ശൂന്യം
after_sigcont(ശൂന്യം)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#എങ്കിൽ defined(SIGCONT)
സ്ഥിതം ശൂന്യം sigcont_handler SIGPROTOARG;

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
സ്ഥിതം അസ്ഥിര sig_atomic_t sigcont_received;
സ്ഥിതം ശൂന്യം sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    സ്ഥിതം ശൂന്യം
sigcont_handler SIGDEFARG(sigarg)
{
    എങ്കിൽ (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    അല്ലെങ്കിൽ
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

#എങ്കിൽ defined(FEAT_CLIPBOARD)
# എങ്കിൽ defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
സ്ഥിതം ശൂന്യം *clip_star_save = ശൂന്യ;
സ്ഥിതം ശൂന്യം *clip_plus_save = ശൂന്യ;
# endif

# എങ്കിൽ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    സ്ഥിതം ശൂന്യം
loose_clipboard(ശൂന്യം)
{
    എങ്കിൽ (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	എങ്കിൽ (clip_star.owned)
	    clip_lose_selection(&clip_star);
	എങ്കിൽ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	എങ്കിൽ (x11_display != ശൂന്യ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# എങ്കിൽ defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    സ്ഥിതം ശൂന്യം
save_clipboard(ശൂന്യം)
{
    എങ്കിൽ (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    എങ്കിൽ (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    സ്ഥിതം ശൂന്യം
restore_clipboard(ശൂന്യം)
{
    എങ്കിൽ (clip_star_save != ശൂന്യ)
    {
	എങ്കിൽ (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	അല്ലെങ്കിൽ
	    free_register(clip_star_save);
	clip_star_save = ശൂന്യ;
    }
    എങ്കിൽ (clip_plus_save != ശൂന്യ)
    {
	എങ്കിൽ (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	അല്ലെങ്കിൽ
	    free_register(clip_plus_save);
	clip_plus_save = ശൂന്യ;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    ശൂന്യം
mch_suspend(ശൂന്യം)
{
    എങ്കിൽ (ignore_sigtstp)
	തിരികെ;

#എങ്കിൽ defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# എങ്കിൽ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# എങ്കിൽ defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# എങ്കിൽ defined(SIGCONT)
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

	വേണ്ടി (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#അല്ലെങ്കിൽ
    suspend_shell();
#endif
}

    ശൂന്യം
mch_init(ശൂന്യം)
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

    ശൂന്യം
set_sigwinch_handler(ശൂന്യം)
{
#എങ്കിൽ defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    സ്ഥിതം ശൂന്യം
set_signals(ശൂന്യം)
{
#എങ്കിൽ defined(SIGWINCH)
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
#എങ്കിൽ defined(SIGCONT)
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

#എങ്കിൽ defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    എങ്കിൽ (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#എങ്കിൽ defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    സ്ഥിതം ശൂന്യം
catch_int_signal(ശൂന്യം)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    ശൂന്യം
reset_signals(ശൂന്യം)
{
    catch_signals(SIG_DFL, SIG_DFL);
#എങ്കിൽ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    സ്ഥിതം ശൂന്യം
catch_signals(
    ശൂന്യം (*func_deadly)(പൂർണ്ണസംഖ്യ),
    ശൂന്യം (*func_other)(പൂർണ്ണസംഖ്യ))
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; signal_info[i].sig != -1; i++)
    {
	എങ്കിൽ (signal_info[i].deadly)
	{
#എങ്കിൽ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ഘടന sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# എങ്കിൽ defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# അല്ലെങ്കിൽ
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ശൂന്യ);
#അല്ലെങ്കിൽ
# എങ്കിൽ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ഘടന sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ശൂന്യ);
# അല്ലെങ്കിൽ
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	അല്ലെങ്കിൽ എങ്കിൽ (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#അല്ലെങ്കിൽ
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    സ്ഥിതം ശൂന്യം
block_signals(sigset_t *set)
{
    sigset_t	newset;
    പൂർണ്ണസംഖ്യ		i;

    sigemptyset(&newset);

    വേണ്ടി (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# എങ്കിൽ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    സ്ഥിതം ശൂന്യം
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, ശൂന്യ);
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
    പൂർണ്ണസംഖ്യ
vim_handle_signal(പൂർണ്ണസംഖ്യ sig)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ got_signal = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ blocked = TRUE;

    തിരഞ്ഞെടുപ്പ് (sig)
    {
	അവസ്ഥ SIGNAL_BLOCK:   blocked = TRUE;
			     നിർത്തുക;

	അവസ്ഥ SIGNAL_UNBLOCK: blocked = FALSE;
			     എങ്കിൽ (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     നിർത്തുക;

	സ്ഥിരസ്ഥിതി:	     എങ്കിൽ (!blocked)
				 തിരികെ TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     എങ്കിൽ (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     നിർത്തുക;
    }
    തിരികെ FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    പൂർണ്ണസംഖ്യ
mch_check_win(പൂർണ്ണസംഖ്യ argc UNUSED, അക്ഷരം **argv UNUSED)
{
    എങ്കിൽ (isatty(1))
	തിരികെ OK;
    തിരികെ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_നിവേശം_isatty(ശൂന്യം)
{
    എങ്കിൽ (isatty(read_cmd_fd))
	തിരികെ TRUE;
    തിരികെ FALSE;
}

#ifdef FEAT_X11

# എങ്കിൽ defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    സ്ഥിതം ശൂന്യം
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#എങ്കിൽ defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

സ്ഥിതം പൂർണ്ണസംഖ്യ	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (അക്ഷരം *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    എങ്കിൽ (!gui.in_use)
# endif
	സന്ദേശം((അക്ഷരം *)IObuff);

    തിരികെ 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    തിരികെ 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_connect_to_server(ശൂന്യം)
{
    // No point in connecting if we are exiting or dying.
    എങ്കിൽ (exiting || v_dying)
	തിരികെ FALSE;

# എങ്കിൽ defined(FEAT_CLIENTSERVER)
    എങ്കിൽ (x_force_connect)
	തിരികെ TRUE;
# endif
    എങ്കിൽ (x_no_connect)
	തിരികെ FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    എങ്കിൽ (clip_exclude_prog != ശൂന്യ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	എങ്കിൽ (regprog_in_use(clip_exclude_prog))
	    തിരികെ FALSE;

	എങ്കിൽ (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    തിരികെ FALSE;
    }
    തിരികെ TRUE;
}

# എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  എങ്കിൽ defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   എങ്കിൽ defined(VMS) || defined(__CYGWIN__)
    തിരികെ 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
സ്ഥിതം പൂർണ്ണസംഖ്യ xterm_dpy_retry_എണ്ണം = 0;

    സ്ഥിതം പൂർണ്ണസംഖ്യ
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = ശൂന്യ;
    xterm_dpy_retry_എണ്ണം = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = ശൂന്യ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  എങ്കിൽ defined(VMS) || defined(__CYGWIN__)
    തിരികെ 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    സ്ഥിതം ശൂന്യം
may_restore_x11_clipboard(ശൂന്യം)
{
    // No point in restoring the connecting if we are exiting or dying.
    എങ്കിൽ (!exiting && !v_dying && xterm_dpy_retry_എണ്ണം > 0)
    {
	--xterm_dpy_retry_എണ്ണം;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	എങ്കിൽ (app_context != (XtAppContext)ശൂന്യ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ശൂന്യ;
	    x11_display = ശൂന്യ; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    ശൂന്യം
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    എങ്കിൽ (eap->arg != ശൂന്യ && (arglen = STRLEN(eap->arg)) > 0)
    {
	എങ്കിൽ (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (അക്ഷരം *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ശൂന്യ
		    ? (അക്ഷരം *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_എണ്ണം = 5;  // Try reconnecting five times
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
test_x11_window(Display *dpy)
{
    പൂർണ്ണസംഖ്യ			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    എങ്കിൽ (XGetWMName(dpy, x11_window, &text_prop))
	XFree((ശൂന്യം *)text_prop.മൂല്യം);
    XSync(dpy, False);
    (ശൂന്യം)XSetErrorHandler(old_handler);

    എങ്കിൽ (p_verbose > 0 && got_x_error)
	verb_സന്ദേശം(_("Testing the X display failed"));

    തിരികെ (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

സ്ഥിതം പൂർണ്ണസംഖ്യ get_x11_thing(പൂർണ്ണസംഖ്യ get_title, പൂർണ്ണസംഖ്യ test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_windis(ശൂന്യം)
{
    അക്ഷരം	    *winid;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    x11_display_from = XD_NONE;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	    did_set_error_handler = FALSE;

    എങ്കിൽ (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(ശൂന്യം)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# എങ്കിൽ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    എങ്കിൽ (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	എങ്കിൽ (x11_display_from == XD_HERE && x11_display != ശൂന്യ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	എങ്കിൽ (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    തിരികെ OK;
	}
	x11_display = ശൂന്യ;
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = ശൂന്യ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    എങ്കിൽ (!x_connect_to_server())
	തിരികെ FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    എങ്കിൽ (x11_window == 0 && (winid = getenv("WINDOWID")) != ശൂന്യ)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    എങ്കിൽ (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    എങ്കിൽ (xterm_dpy != ശൂന്യ && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	എങ്കിൽ (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    എങ്കിൽ (x11_display_from == XD_HERE && x11_display != ശൂന്യ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	എങ്കിൽ (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = ശൂന്യ;
	    x11_display_from = XD_NONE;
	    തിരികെ FAIL;
	}
	തിരികെ OK;
    }
# endif

    എങ്കിൽ (x11_window == 0 || x11_display == ശൂന്യ)
	result = -1;

    എങ്കിൽ (result != -1)	    // Have already been here and set this
	തിരികെ result;	    // Don't do all these X calls again

    എങ്കിൽ (x11_window != 0 && x11_display == ശൂന്യ)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	എങ്കിൽ (p_verbose > 0)
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
	x11_display = XOpenDisplay(ശൂന്യ);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	എങ്കിൽ (p_verbose > 0 && sig_alarm_called)
	    verb_സന്ദേശം(_("Opening the X display timed out"));
# endif
	എങ്കിൽ (x11_display != ശൂന്യ)
	{
# ifdef ELAPSED_FUNC
	    എങ്കിൽ (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    എങ്കിൽ (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = ശൂന്യ;
	    }
	    അല്ലെങ്കിൽ
		x11_display_from = XD_HERE;
	}
    }
    എങ്കിൽ (x11_window == 0 || x11_display == ശൂന്യ)
	തിരികെ (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    തിരികെ (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_title(പൂർണ്ണസംഖ്യ test_only)
{
    തിരികെ get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_icon(പൂർണ്ണസംഖ്യ test_only)
{
    പൂർണ്ണസംഖ്യ		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    എങ്കിൽ (oldicon == ശൂന്യ && !test_only)
    {
	എങ്കിൽ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	അല്ലെങ്കിൽ
	    oldicon = vim_strsave(T_NAME);
    }

    തിരികെ retval;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_thing(
    പൂർണ്ണസംഖ്യ		get_title,	// get title string
    പൂർണ്ണസംഖ്യ		test_only)
{
    XTextProperty	text_prop;
    പൂർണ്ണസംഖ്യ			retval = FALSE;
    Status		status;

    എങ്കിൽ (get_x11_windis() != OK)
	തിരികെ FALSE;

    // Get window/icon name if any
    എങ്കിൽ (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    അല്ലെങ്കിൽ
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
    Window	    വേര്;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned പൂർണ്ണസംഖ്യ    അക്കം_children;

    ആകുമ്പോൾ (!status || text_prop.മൂല്യം == ശൂന്യ)
    {
	എങ്കിൽ (!XQuery_വൃക്ഷം(x11_display, win, &വേര്, &parent, &children,
		    &അക്കം_children))
	    നിർത്തുക;
	എങ്കിൽ (children)
	    XFree((ശൂന്യം *)children);
	എങ്കിൽ (parent == വേര് || parent == 0)
	    നിർത്തുക;

	win = parent;
	എങ്കിൽ (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	അല്ലെങ്കിൽ
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    എങ്കിൽ (status && text_prop.മൂല്യം != ശൂന്യ)
    {
	retval = TRUE;
	എങ്കിൽ (!test_only)
	{
	    എങ്കിൽ (get_title)
		vim_free(oldtitle);
	    അല്ലെങ്കിൽ
		vim_free(oldicon);
	    എങ്കിൽ (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		എങ്കിൽ (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.മൂല്യം);
		അല്ലെങ്കിൽ
		    oldicon = vim_strsave((char_u *)text_prop.മൂല്യം);
	    }
	    അല്ലെങ്കിൽ
	    {
		അക്ഷരം    **cl;
		Status  transform_status;
		പൂർണ്ണസംഖ്യ	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		എങ്കിൽ (transform_status >= Success && n > 0 && cl[0])
		{
		    എങ്കിൽ (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    അല്ലെങ്കിൽ
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		അല്ലെങ്കിൽ
		{
		    എങ്കിൽ (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.മൂല്യം);
		    അല്ലെങ്കിൽ
			oldicon = vim_strsave((char_u *)text_prop.മൂല്യം);
		}
	    }
	}
	XFree((ശൂന്യം *)text_prop.മൂല്യം);
    }
    തിരികെ retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# എങ്കിൽ defined(X_HAVE_UTF8_STRING)
#  എങ്കിൽ X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    സ്ഥിതം ശൂന്യം
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    എങ്കിൽ (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (സ്ഥിരം അക്ഷരം *)title,
					     ശൂന്യ, ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    അല്ലെങ്കിൽ
# endif
    {
# എങ്കിൽ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (സ്ഥിരം അക്ഷരം *)title,
					     ശൂന്യ, ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#  അല്ലെങ്കിൽ
	XTextProperty	text_prop;
	അക്ഷരം		*c_title = (അക്ഷരം *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(ശൂന്യം)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     ശൂന്യ, ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#  endif
# അല്ലെങ്കിൽ
	XStoreName(x11_display, x11_window, (അക്ഷരം *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    സ്ഥിതം ശൂന്യം
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    എങ്കിൽ (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, ശൂന്യ, (സ്ഥിരം അക്ഷരം *)icon,
						   ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
    അല്ലെങ്കിൽ
# endif
    {
# എങ്കിൽ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, ശൂന്യ, (സ്ഥിരം അക്ഷരം *)icon,
						   ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#  അല്ലെങ്കിൽ
	XTextProperty	text_prop;
	അക്ഷരം		*c_icon = (അക്ഷരം *)icon;

	(ശൂന്യം)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, ശൂന്യ, &text_prop,
						   ശൂന്യ, 0, ശൂന്യ, ശൂന്യ, ശൂന്യ);
#  endif
# അല്ലെങ്കിൽ
	XSetIconName(x11_display, x11_window, (അക്ഷരം *)icon);
# endif
    }
    XFlush(x11_display);
}

#അല്ലെങ്കിൽ  // FEAT_X11

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_title(പൂർണ്ണസംഖ്യ test_only UNUSED)
{
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_x11_icon(പൂർണ്ണസംഖ്യ test_only)
{
    എങ്കിൽ (!test_only)
    {
	എങ്കിൽ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	അല്ലെങ്കിൽ
	    oldicon = vim_strsave(T_NAME);
    }
    തിരികെ FALSE;
}

#endif // FEAT_X11

    പൂർണ്ണസംഖ്യ
mch_can_restore_title(ശൂന്യം)
{
    തിരികെ get_x11_title(TRUE);
}

    പൂർണ്ണസംഖ്യ
mch_can_restore_icon(ശൂന്യം)
{
    തിരികെ get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    ശൂന്യം
mch_settitle(char_u *title, char_u *icon)
{
    പൂർണ്ണസംഖ്യ		type = 0;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	recursive = 0;

    എങ്കിൽ (T_NAME == ശൂന്യ)	    // no terminal name (yet)
	തിരികെ;
    എങ്കിൽ (title == ശൂന്യ && icon == ശൂന്യ)	    // nothing to do
	തിരികെ;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    എങ്കിൽ (recursive)
	തിരികെ;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    എങ്കിൽ (get_x11_windis() == OK)
	type = 1;
#endif
#എങ്കിൽ defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    എങ്കിൽ (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    എങ്കിൽ ((type || *T_TS != NUL) && title != ശൂന്യ)
    {
	എങ്കിൽ (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	എങ്കിൽ (oldtitle == ശൂന്യ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (ശൂന്യം)get_x11_title(FALSE);

	എങ്കിൽ (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	അല്ലെങ്കിൽ
# ifdef FEAT_GUI_GTK
	എങ്കിൽ (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#എങ്കിൽ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	അല്ലെങ്കിൽ
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    എങ്കിൽ ((type || *T_CIS != NUL) && icon != ശൂന്യ)
    {
	എങ്കിൽ (oldicon == ശൂന്യ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	എങ്കിൽ (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	അല്ലെങ്കിൽ
# ifdef FEAT_GUI_GTK
	എങ്കിൽ (!gui.in_use)		// don't do this if GTK+ is running
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
    ശൂന്യം
mch_restore_title(പൂർണ്ണസംഖ്യ which)
{
    പൂർണ്ണസംഖ്യ	do_തള്ളുക_എടുക്കുക = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : ശൂന്യ;
    char_u *tofree = ശൂന്യ;
    എങ്കിൽ (title == oldtitle && oldtitle != ശൂന്യ)
    {
	tofree = vim_strsave(title);
	എങ്കിൽ (tofree != ശൂന്യ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : ശൂന്യ);
    vim_free(tofree);

    എങ്കിൽ (do_തള്ളുക_എടുക്കുക)
    {
	// pop and push from/to the stack
	term_എടുക്കുക_title(which);
	term_തള്ളുക_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    പൂർണ്ണസംഖ്യ
vim_is_xterm(char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ FALSE;
    തിരികെ ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#എങ്കിൽ defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    പൂർണ്ണസംഖ്യ
use_xterm_like_mouse(char_u *name)
{
    തിരികെ (name != ശൂന്യ
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
    പൂർണ്ണസംഖ്യ
use_xterm_mouse(ശൂന്യം)
{
    എങ്കിൽ (ttym_flags == TTYM_SGR)
	തിരികെ 4;
    എങ്കിൽ (ttym_flags == TTYM_URXVT)
	തിരികെ 3;
    എങ്കിൽ (ttym_flags == TTYM_XTERM2)
	തിരികെ 2;
    എങ്കിൽ (ttym_flags == TTYM_XTERM)
	തിരികെ 1;
    തിരികെ 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    പൂർണ്ണസംഖ്യ
vim_is_iris(char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ FALSE;
    തിരികെ (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#എങ്കിൽ defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    പൂർണ്ണസംഖ്യ
vim_is_vt300(char_u *name)
{
    എങ്കിൽ (name == ശൂന്യ)
	തിരികെ FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    തിരികെ ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ശൂന്യ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    പൂർണ്ണസംഖ്യ
mch_get_user_name(char_u *s, പൂർണ്ണസംഖ്യ len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(ശൂന്യ), len - 1);
    തിരികെ OK;
#അല്ലെങ്കിൽ
    തിരികെ mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    പൂർണ്ണസംഖ്യ
mch_get_uname(uid_t uid, char_u *s, പൂർണ്ണസംഖ്യ len)
{
#എങ്കിൽ defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ഘടന passwd   *pw;

    എങ്കിൽ ((pw = getpwuid(uid)) != ശൂന്യ
	    && pw->pw_name != ശൂന്യ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	തിരികെ OK;
    }
#endif
    sprintf((അക്ഷരം *)s, "%d", (പൂർണ്ണസംഖ്യ)uid);	    // assumes s is long enough
    തിരികെ FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    ശൂന്യം
mch_get_host_name(char_u *s, പൂർണ്ണസംഖ്യ len)
{
    ഘടന utsname vutsname;

    എങ്കിൽ (uname(&vutsname) < 0)
	*s = NUL;
    അല്ലെങ്കിൽ
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#അല്ലെങ്കിൽ // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    ശൂന്യം
mch_get_host_name(char_u *s, പൂർണ്ണസംഖ്യ len)
{
# ifdef VAXC
    vaxc$gethostname((അക്ഷരം *)s, len);
# അല്ലെങ്കിൽ
    gethostname((അക്ഷരം *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(ശൂന്യം)
{
    തിരികെ (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    പൂർണ്ണസംഖ്യ
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    എങ്കിൽ (kill(pid, 0) == 0)
	തിരികെ TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    എങ്കിൽ (errno == ESRCH)
	തിരികെ FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    തിരികെ TRUE;
}

#എങ്കിൽ !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    സ്ഥിതം അക്ഷരം *
strerror(പൂർണ്ണസംഖ്യ err)
{
    ബാഹ്യ പൂർണ്ണസംഖ്യ	    sys_nerr;
    ബാഹ്യ അക്ഷരം	    *sys_errlist[];
    സ്ഥിതം അക്ഷരം	    er[20];

    എങ്കിൽ (err > 0 && err < sys_nerr)
	തിരികെ (sys_errlist[err]);
    snprintf(er, വലുപ്പം(er), "Error %d", err);
    തിരികെ er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    പൂർണ്ണസംഖ്യ
mch_dirname(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
#എങ്കിൽ defined(USE_GETCWD)
    എങ്കിൽ (getcwd((അക്ഷരം *)buf, len) == ശൂന്യ)
    {
	STRCPY(buf, strerror(errno));
	തിരികെ FAIL;
    }
    തിരികെ OK;
#അല്ലെങ്കിൽ
    തിരികെ (getwd((അക്ഷരം *)buf) != ശൂന്യ ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    പൂർണ്ണസംഖ്യ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		force)		// also expand when already absolute path
{
    പൂർണ്ണസംഖ്യ		buflen = 0;
#ifdef HAVE_FCHDIR
    പൂർണ്ണസംഖ്യ		fd = -1;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    പൂർണ്ണസംഖ്യ		retval = OK;
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
# എങ്കിൽ CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# അല്ലെങ്കിൽ
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    എങ്കിൽ ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == ശൂന്യ || p != fname))
    {
	എങ്കിൽ (p == ശൂന്യ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	എങ്കിൽ (p != ശൂന്യ)
	{
	    എങ്കിൽ (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    എങ്കിൽ (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		എങ്കിൽ (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    എങ്കിൽ (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((അക്ഷരം *)olddir) != 0))
	    {
		p = ശൂന്യ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    അല്ലെങ്കിൽ
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		എങ്കിൽ (p - fname >= len)
		    retval = FAIL;
		അല്ലെങ്കിൽ
		{
		    vim_strncpy(buf, fname, p - fname);
		    എങ്കിൽ (mch_chdir((അക്ഷരം *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			എങ്കിൽ (mch_isFullName(fname))
			    retval = FAIL;
			അല്ലെങ്കിൽ
			    p = ശൂന്യ;
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (*p == '/')
			fname = p + 1;
		    അല്ലെങ്കിൽ
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	എങ്കിൽ (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	എങ്കിൽ (p != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ	l;

#ifdef HAVE_FCHDIR
	    എങ്കിൽ (fd >= 0)
	    {
		എങ്കിൽ (p_verbose >= 5)
		{
		    verbose_enter();
		    സന്ദേശം("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    അല്ലെങ്കിൽ
#endif
		l = mch_chdir((അക്ഷരം *)olddir);
	    എങ്കിൽ (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	എങ്കിൽ (fd >= 0)
	    close(fd);
#endif

	buflen = (പൂർണ്ണസംഖ്യ)STRLEN(buf);
	എങ്കിൽ (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	അല്ലെങ്കിൽ എങ്കിൽ (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += വലുപ്പം(PATHSEP);
	}
#endif
    }

    എങ്കിൽ (buflen == 0)
	buflen = (പൂർണ്ണസംഖ്യ)STRLEN(buf);

    // Catch file names which are too long.
    എങ്കിൽ (retval == FAIL || (പൂർണ്ണസംഖ്യ)(buflen + STRLEN(fname)) >= len)
	തിരികെ FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    എങ്കിൽ (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    തിരികെ OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    പൂർണ്ണസംഖ്യ
mch_isFullName(char_u *fname)
{
#ifdef VMS
    തിരികെ ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((അക്ഷരം *)fname,':') || strchr((അക്ഷരം *)fname,'"') ||
	    (strchr((അക്ഷരം *)fname,'[') && strchr((അക്ഷരം *)fname,']'))||
	    (strchr((അക്ഷരം *)fname,'<') && strchr((അക്ഷരം *)fname,'>'))   );
#അല്ലെങ്കിൽ
    തിരികെ (*fname == '/' || *fname == '~');
#endif
}

#എങ്കിൽ defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    ശൂന്യം
fname_case(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		len UNUSED)  // buffer size, only used when name gets longer
{
    ഘടന stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    ഘടന dirent *dp;

    എങ്കിൽ (mch_lstat((അക്ഷരം *)name, &st) < 0)
	തിരികെ;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    എങ്കിൽ (slash == ശൂന്യ)
    {
	dirp = opendir(".");
	tail = name;
    }
    അല്ലെങ്കിൽ
    {
	*slash = NUL;
	dirp = opendir((അക്ഷരം *)name);
	*slash = '/';
	tail = slash + 1;
    }

    എങ്കിൽ (dirp == ശൂന്യ)
	തിരികെ;

    taillen = STRLEN(tail);
    ആകുമ്പോൾ ((dp = readdir(dirp)) != ശൂന്യ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	എങ്കിൽ (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ഘടന stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    എങ്കിൽ (mch_lstat((അക്ഷരം *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		നിർത്തുക;
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
    ഘടന stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    എങ്കിൽ (stat((അക്ഷരം *)vms_fixfilename(name), &statb))
#അല്ലെങ്കിൽ
    എങ്കിൽ (stat((അക്ഷരം *)name, &statb))
#endif
	തിരികെ -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    തിരികെ statb.st_mode & ~S_ADDACE;
#അല്ലെങ്കിൽ
    തിരികെ statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_setperm(char_u *name, long perm)
{
    തിരികെ (chmod((അക്ഷരം *)
#ifdef VMS
		    vms_fixfilename(name),
#അല്ലെങ്കിൽ
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#എങ്കിൽ defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    പൂർണ്ണസംഖ്യ
mch_fsetperm(പൂർണ്ണസംഖ്യ fd, long perm)
{
    തിരികെ (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#എങ്കിൽ defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
തരനിർവചനം ഘടന vim_acl_solaris_T {
    പൂർണ്ണസംഖ്യ acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# എങ്കിൽ defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ശൂന്യം
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    എങ്കിൽ (from_file == ശൂന്യ)
	തിരികെ;

    എങ്കിൽ (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    എങ്കിൽ (selinux_enabled <= 0)
	തിരികെ;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    അക്ഷരം *from_context = ശൂന്യ;
    അക്ഷരം *to_context = ശൂന്യ;

    എങ്കിൽ (getfilecon((അക്ഷരം *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	എങ്കിൽ (errno == EOPNOTSUPP)
	    തിരികെ;

	സന്ദേശം_puts(_("\nCould not get security context for "));
	സന്ദേശം_outtrans(from_file);
	സന്ദേശം_putchar('\n');
	തിരികെ;
    }
    എങ്കിൽ (getfilecon((അക്ഷരം *)to_file, &to_context) < 0)
    {
	സന്ദേശം_puts(_("\nCould not get security context for "));
	സന്ദേശം_outtrans(to_file);
	സന്ദേശം_putchar('\n');
	freecon (from_context);
	തിരികെ ;
    }
    എങ്കിൽ (strcmp(from_context, to_context) != 0)
    {
	എങ്കിൽ (setfilecon((അക്ഷരം *)to_file, from_context) < 0)
	{
	    സന്ദേശം_puts(_("\nCould not set security context for "));
	    സന്ദേശം_outtrans(to_file);
	    സന്ദേശം_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# എങ്കിൽ defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ശൂന്യം
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    സ്ഥിതം സ്ഥിരം അക്ഷരം * സ്ഥിരം smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    അക്ഷരം	buffer[SMACK_LABEL_LEN];
    സ്ഥിരം അക്ഷരം	*name;
    പൂർണ്ണസംഖ്യ		index;
    പൂർണ്ണസംഖ്യ		ret;
    ssize_t	size;

    എങ്കിൽ (from_file == ശൂന്യ)
	തിരികെ;

    size = listxattr((അക്ഷരം *)from_file, ശൂന്യ, 0);
    // not supported or no attributes to copy
    എങ്കിൽ (size <= 0)
	തിരികെ;

    വേണ്ടി (index = 0 ; index < (പൂർണ്ണസംഖ്യ)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((അക്ഷരം*)from_file, name, buffer, വലുപ്പം(buffer));
	എങ്കിൽ (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((അക്ഷരം*)to_file, name, buffer, (size_t)size, 0);
	    എങ്കിൽ (ret < 0)
	    {
		vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		സന്ദേശം_outtrans(IObuff);
		സന്ദേശം_putchar('\n');
	    }
	}
	അല്ലെങ്കിൽ
	{
	    // what reason of not having the attribute value?
	    തിരഞ്ഞെടുപ്പ് (errno)
	    {
		അവസ്ഥ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    തിരികെ; // leave because it isn't useful to continue

		അവസ്ഥ ERANGE:
		സ്ഥിരസ്ഥിതി:
		    // no enough size OR unexpected error
		     vim_snprintf((അക്ഷരം *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    സന്ദേശം_puts((അക്ഷരം *)IObuff);
		    സന്ദേശം_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		അവസ്ഥ ENODATA:
		    // no attribute of this name
		    ret = removexattr((അക്ഷരം*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    നിർത്തുക;
	    }
	}
    }
}
# endif // HAVE_SMACK

# എങ്കിൽ defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    ശൂന്യം
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    അക്ഷരം	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    അക്ഷരം	*താക്കോൽ;
    അക്ഷരം	*val = ശൂന്യ;
    അക്ഷരം	*errmsg = ശൂന്യ;

    എങ്കിൽ (from_file == ശൂന്യ)
	തിരികെ;

    // get the length of the extended attributes
    size = listxattr((അക്ഷരം *)from_file, ശൂന്യ, 0);
    // not supported or no attributes to copy
    എങ്കിൽ (size <= 0)
	തിരികെ;
    xattr_buf = (അക്ഷരം*)alloc(size);
    എങ്കിൽ (xattr_buf == ശൂന്യ)
	തിരികെ;
    size = listxattr((അക്ഷരം *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    വേണ്ടി (പൂർണ്ണസംഖ്യ round = 0; round < 2; round++)
    {

	താക്കോൽ = xattr_buf;
	എങ്കിൽ (round == 1)
	    size = tsize;

	ആകുമ്പോൾ (size > 0)
	{
	    vallen = getxattr((അക്ഷരം *)from_file, താക്കോൽ,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    എങ്കിൽ (vallen >= 0 && round &&
		setxattr((അക്ഷരം *)to_file, താക്കോൽ, val, vallen, 0) == 0)
		;
	    അല്ലെങ്കിൽ എങ്കിൽ (errno)
	    {
		തിരഞ്ഞെടുപ്പ് (errno)
		{
		    അവസ്ഥ E2BIG:
			errmsg = e_xattr_e2big;
			പോകുക error_exit;
		    അവസ്ഥ ENOTSUP:
		    അവസ്ഥ EACCES:
		    അവസ്ഥ EPERM:
			നിർത്തുക;
		    അവസ്ഥ ERANGE:
			errmsg = e_xattr_erange;
			പോകുക error_exit;
		    സ്ഥിരസ്ഥിതി:
			errmsg = e_xattr_other;
			പോകുക error_exit;
		}
	    }

	    എങ്കിൽ (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(താക്കോൽ) + 1;
	    size -= keylen;
	    താക്കോൽ += keylen;
	}
	എങ്കിൽ (round)
	    നിർത്തുക;

	val = (അക്ഷരം*)alloc(max_vallen + 1);
	എങ്കിൽ (val == ശൂന്യ)
	    പോകുക error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    എങ്കിൽ (errmsg != ശൂന്യ)
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
    vim_acl_T	ret = ശൂന്യ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((അക്ഷരം *)fname, ACL_TYPE_ACCESS);
# അല്ലെങ്കിൽ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    എങ്കിൽ (acl_get((അക്ഷരം *)fname, 0, &aclent) < 0)
	തിരികെ ശൂന്യ;
    ret = (vim_acl_T)aclent;
#  അല്ലെങ്കിൽ
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(വലുപ്പം(vim_acl_solaris_T));
    എങ്കിൽ (aclent == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ ((aclent->acl_cnt = acl((അക്ഷരം *)fname, GETACLCNT, 0, ശൂന്യ)) < 0)
    {
	സ്വതന്ത്രം(aclent);
	തിരികെ ശൂന്യ;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * വലുപ്പം(aclent_t));
    എങ്കിൽ (aclent->acl_entry == ശൂന്യ)
    {
	സ്വതന്ത്രം(aclent);
	തിരികെ ശൂന്യ;
    }
    എങ്കിൽ (acl((അക്ഷരം *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	സ്വതന്ത്രം(aclent->acl_entry);
	സ്വതന്ത്രം(aclent);
	തിരികെ ശൂന്യ;
    }
    ret = (vim_acl_T)aclent;
#   അല്ലെങ്കിൽ
#    എങ്കിൽ defined(HAVE_AIX_ACL)
    പൂർണ്ണസംഖ്യ		aclsize;
    ഘടന acl *aclent;

    aclsize = വലുപ്പം(ഘടന acl);
    aclent = malloc(aclsize);
    എങ്കിൽ (aclent == ശൂന്യ)
	തിരികെ ശൂന്യ;
    എങ്കിൽ (statacl((അക്ഷരം *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	എങ്കിൽ (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    എങ്കിൽ (aclent == ശൂന്യ || statacl((അക്ഷരം *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		സ്വതന്ത്രം(aclent);
		തിരികെ ശൂന്യ;
	    }
	}
	അല്ലെങ്കിൽ
	{
	    സ്വതന്ത്രം(aclent);
	    തിരികെ ശൂന്യ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    തിരികെ ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    ശൂന്യം
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    എങ്കിൽ (aclent == ശൂന്യ)
	തിരികെ;
# ifdef HAVE_POSIX_ACL
    acl_set_file((അക്ഷരം *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# അല്ലെങ്കിൽ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((അക്ഷരം *)fname, (acl_t *)aclent);
#  അല്ലെങ്കിൽ
#   ifdef HAVE_SOLARIS_ACL
    acl((അക്ഷരം *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   അല്ലെങ്കിൽ
#    ifdef HAVE_AIX_ACL
    chacl((അക്ഷരം *)fname, aclent, ((ഘടന acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    ശൂന്യം
mch_free_acl(vim_acl_T aclent)
{
    എങ്കിൽ (aclent == ശൂന്യ)
	തിരികെ;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# അല്ലെങ്കിൽ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  അല്ലെങ്കിൽ
#   ifdef HAVE_SOLARIS_ACL
    സ്വതന്ത്രം(((vim_acl_solaris_T *)aclent)->acl_entry);
    സ്വതന്ത്രം(aclent);
#   അല്ലെങ്കിൽ
#    ifdef HAVE_AIX_ACL
    സ്വതന്ത്രം(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    ശൂന്യം
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    പൂർണ്ണസംഖ്യ
mch_isdir(char_u *name)
{
    ഘടന stat statb;

    എങ്കിൽ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	തിരികെ FALSE;
    എങ്കിൽ (stat((അക്ഷരം *)name, &statb))
	തിരികെ FALSE;
    തിരികെ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    പൂർണ്ണസംഖ്യ
mch_isrealdir(char_u *name)
{
    ഘടന stat statb;

    എങ്കിൽ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	തിരികെ FALSE;
    എങ്കിൽ (mch_lstat((അക്ഷരം *)name, &statb))
	തിരികെ FALSE;
    തിരികെ (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
executable_file(char_u *name)
{
    ഘടന stat	st;

    എങ്കിൽ (stat((അക്ഷരം *)name, &st))
	തിരികെ 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    പൂർണ്ണസംഖ്യ vms_executable = 0;
    എങ്കിൽ (S_ISREG(st.st_mode) && mch_access((അക്ഷരം *)name, X_OK) == 0)
    {
	എങ്കിൽ (strstr(vms_tolower((അക്ഷരം*)name),".exe") != ശൂന്യ
		|| strstr(vms_tolower((അക്ഷരം*)name),".com")!= ശൂന്യ)
	    vms_executable = 1;
    }
    തിരികെ vms_executable;
#അല്ലെങ്കിൽ
    തിരികെ S_ISREG(st.st_mode) && mch_access((അക്ഷരം *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    പൂർണ്ണസംഖ്യ
mch_can_exe(char_u *name, char_u **path, പൂർണ്ണസംഖ്യ use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    പൂർണ്ണസംഖ്യ		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    എങ്കിൽ (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	എങ്കിൽ ((use_path || gettail(name) != name) && executable_file(name))
	{
	    എങ്കിൽ (path != ശൂന്യ)
	    {
		എങ്കിൽ (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		അല്ലെങ്കിൽ
		    *path = vim_strsave(name);
	    }
	    തിരികെ TRUE;
	}
	തിരികെ FALSE;
    }

    p = (char_u *)getenv("PATH");
    എങ്കിൽ (p == ശൂന്യ || *p == NUL)
	തിരികെ -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    എങ്കിൽ (buf == ശൂന്യ)
	തിരികെ -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    വേണ്ടി (;;)
    {
	e = (char_u *)strchr((അക്ഷരം *)p, ':');
	എങ്കിൽ (e == ശൂന്യ)
	    e = p_end;
	elen = (size_t)(e - p);
	എങ്കിൽ (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((അക്ഷരം *)buf, bufsize, "%.*s%s%s",
		(പൂർണ്ണസംഖ്യ)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	എങ്കിൽ (retval == 1)
	{
	    എങ്കിൽ (path != ശൂന്യ)
	    {
		എങ്കിൽ (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		അല്ലെങ്കിൽ
		    *path = vim_strnsave(buf, buflen);
	    }
	    നിർത്തുക;
	}

	എങ്കിൽ (*e != ':')
	    നിർത്തുക;
	p = e + 1;
    }

    vim_free(buf);
    തിരികെ retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    പൂർണ്ണസംഖ്യ
mch_nodetype(char_u *name)
{
    ഘടന stat	st;

    എങ്കിൽ (stat((അക്ഷരം *)name, &st))
	തിരികെ കണ്ണി_NORMAL;
    എങ്കിൽ (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	തിരികെ കണ്ണി_NORMAL;
    എങ്കിൽ (S_ISBLK(st.st_mode))	// block device isn't writable
	തിരികെ കണ്ണി_OTHER;
    // Everything else is writable?
    തിരികെ കണ്ണി_WRITABLE;
}

    ശൂന്യം
mch_early_init(ശൂന്യം)
{
#ifdef HAVE_CHECK_അട്ടി_GROWTH
    പൂർണ്ണസംഖ്യ			i;

    check_അട്ടി_growth((അക്ഷരം *)&i);

# ifdef HAVE_അട്ടി_LIMIT
    get_അട്ടി_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#എങ്കിൽ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_അട്ടി = alloc(get_signal_അട്ടി_size());
    init_signal_അട്ടി();
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

#എങ്കിൽ defined(EXITFREE)
    ശൂന്യം
mch_free_mem(ശൂന്യം)
{
# എങ്കിൽ defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    എങ്കിൽ (clip_star.owned)
	clip_lose_selection(&clip_star);
    എങ്കിൽ (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    എങ്കിൽ (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    എങ്കിൽ (xterm_dpy != ശൂന്യ)
	XtCloseDisplay(xterm_dpy);
    എങ്കിൽ (app_context != (XtAppContext)ശൂന്യ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ശൂന്യ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# എങ്കിൽ defined(FEAT_X11)
    എങ്കിൽ (x11_display != ശൂന്യ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# എങ്കിൽ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_അട്ടി);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    സ്ഥിതം ശൂന്യം
exit_scroll(ശൂന്യം)
{
    എങ്കിൽ (silent_mode)
	തിരികെ;
    എങ്കിൽ (newline_on_exit || സന്ദേശം_didout)
    {
	എങ്കിൽ (സന്ദേശം_use_printf())
	{
	    എങ്കിൽ (info_message)
		mch_സന്ദേശം("\n");
	    അല്ലെങ്കിൽ
		mch_errmsg("\r\n");
	}
	അല്ലെങ്കിൽ
	    out_char('\n');
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	സന്ദേശം_clr_eos_force();		// clear the rest of the display
	windgoto((പൂർണ്ണസംഖ്യ)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# എങ്കിൽ (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
ബാഹ്യ ശൂന്യം __gcov_dump(ശൂന്യം);
ബാഹ്യ ശൂന്യം __gcov_reset(ശൂന്യം);
#  define __gcov_flush() ചെയ്യുക { __gcov_dump(); __gcov_reset(); } ആകുമ്പോൾ (0)
# അല്ലെങ്കിൽ
ബാഹ്യ ശൂന്യം __gcov_flush(ശൂന്യം);
# endif
#endif

    ശൂന്യം
mch_exit(പൂർണ്ണസംഖ്യ r)
{
    exiting = TRUE;

#എങ്കിൽ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    എങ്കിൽ (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	എങ്കിൽ (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_എടുക്കുക_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	എങ്കിൽ (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	എങ്കിൽ (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	എങ്കിൽ (full_screen)
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
    എങ്കിൽ (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    എങ്കിൽ (deadly_signal != 0)
	തിരികെ;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    സ്ഥിതം ശൂന്യം
may_core_dump(ശൂന്യം)
{
    എങ്കിൽ (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
get_tty_fd(പൂർണ്ണസംഖ്യ fd)
{
    പൂർണ്ണസംഖ്യ		tty_fd = fd;

# എങ്കിൽ defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    എങ്കിൽ (mch_isatty(fd) > 1)
    {
	അക്ഷരം *name;

	name = ptsname(fd);
	എങ്കിൽ (name == ശൂന്യ)
	    തിരികെ -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	എങ്കിൽ (tty_fd < 0)
	    തിരികെ -1;
    }
# endif
    തിരികെ tty_fd;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_tcgetattr(പൂർണ്ണസംഖ്യ fd, ശൂന്യം *term)
{
    പൂർണ്ണസംഖ്യ		tty_fd;
    പൂർണ്ണസംഖ്യ		retval = -1;

    tty_fd = get_tty_fd(fd);
    എങ്കിൽ (tty_fd < 0)
	തിരികെ -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ഘടന termios *)term);
#  അല്ലെങ്കിൽ
    retval = ioctl(tty_fd, TCGETA, (ഘടന termio *)term);
#  endif
# അല്ലെങ്കിൽ
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ഘടന sgttyb *)term);
# endif
    എങ്കിൽ (tty_fd != fd)
	close(tty_fd);
    തിരികെ retval;
}

    ശൂന്യം
mch_settmode(tmode_T tmode)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    സ്ഥിതം ഘടന termios told;
	   ഘടന termios tnew;
#  അല്ലെങ്കിൽ
    സ്ഥിതം ഘടന termio told;
	   ഘടന termio tnew;
#  endif

    എങ്കിൽ (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    എങ്കിൽ (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ശൂന്യ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  എങ്കിൽ defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   അല്ലെങ്കിൽ
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    അല്ലെങ്കിൽ
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  എങ്കിൽ defined(HAVE_TERMIOS_H)
    {
	പൂർണ്ണസംഖ്യ	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	ആകുമ്പോൾ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  അല്ലെങ്കിൽ
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# അല്ലെങ്കിൽ
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    സ്ഥിതം ഘടന sgttyb ttybold;
	   ഘടന sgttyb ttybnew;

    എങ്കിൽ (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    എങ്കിൽ (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    അല്ലെങ്കിൽ എങ്കിൽ (tmode == TMODE_SLEEP)
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
    ശൂന്യം
get_stty(ശൂന്യം)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    എങ്കിൽ (get_tty_info(read_cmd_fd, &info) != OK)
	തിരികെ;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    എങ്കിൽ (p != ശൂന്യ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ശൂന്യ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    പൂർണ്ണസംഖ്യ
get_tty_info(പൂർണ്ണസംഖ്യ fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ഘടന termios താക്കോലുകൾ;
#  അല്ലെങ്കിൽ
    ഘടന termio താക്കോലുകൾ;
#  endif

    എങ്കിൽ (mch_tcgetattr(fd, &താക്കോലുകൾ) != -1)
    {
	info->backspace = താക്കോലുകൾ.c_cc[VERASE];
	info->interrupt = താക്കോലുകൾ.c_cc[VINTR];
	എങ്കിൽ (താക്കോലുകൾ.c_iflag & ICRNL)
	    info->enter = NL;
	അല്ലെങ്കിൽ
	    info->enter = CAR;
	എങ്കിൽ (താക്കോലുകൾ.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	അല്ലെങ്കിൽ
	    info->nl_does_cr = FALSE;
	തിരികെ OK;
    }
# അല്ലെങ്കിൽ
    // for "old" tty systems
    ഘടന sgttyb താക്കോലുകൾ;

    എങ്കിൽ (mch_tcgetattr(fd, &താക്കോലുകൾ) != -1)
    {
	info->backspace = താക്കോലുകൾ.sg_erase;
	info->interrupt = താക്കോലുകൾ.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	തിരികെ OK;
    }
# endif
    തിരികെ FAIL;
}

#endif // VMS

സ്ഥിതം പൂർണ്ണസംഖ്യ	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    ശൂന്യം
mch_setmouse(പൂർണ്ണസംഖ്യ on)
{
#ifdef FEAT_BEVAL_TERM
    സ്ഥിതം പൂർണ്ണസംഖ്യ	bevalterm_ison = FALSE;
#endif
    പൂർണ്ണസംഖ്യ		xterm_mouse_vers;

#എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    എങ്കിൽ (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    എങ്കിൽ (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	തിരികെ;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    എങ്കിൽ (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    എങ്കിൽ (T_CXM != ശൂന്യ && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    എങ്കിൽ (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	എങ്കിൽ (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    എങ്കിൽ (xterm_mouse_vers > 0)
    {
	എങ്കിൽ (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	അല്ലെങ്കിൽ	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    അല്ലെങ്കിൽ എങ്കിൽ (ttym_flags == TTYM_DEC)
    {
	എങ്കിൽ (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	അല്ലെങ്കിൽ	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (on)
	{
	    എങ്കിൽ (gpm_open())
		mouse_ison = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (on)
	{
	    എങ്കിൽ (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (on)
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
# അല്ലെങ്കിൽ
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	അല്ലെങ്കിൽ
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    അല്ലെങ്കിൽ
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	എങ്കിൽ (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	അല്ലെങ്കിൽ
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#എങ്കിൽ defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    ശൂന്യം
mch_bevalterm_changed(ശൂന്യം)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    ശൂന്യം
check_mouse_termcode(ശൂന്യം)
{
#ifdef FEAT_MOUSE_XTERM
    എങ്കിൽ (use_xterm_mouse()
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
	എങ്കിൽ (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    എങ്കിൽ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    എങ്കിൽ (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	എങ്കിൽ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    അല്ലെങ്കിൽ
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    എങ്കിൽ (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	എങ്കിൽ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    അല്ലെങ്കിൽ
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
    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
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
	ഘടന winsize	ws;
	പൂർണ്ണസംഖ്യ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	എങ്കിൽ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	എങ്കിൽ (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(ശൂന്യ, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# അല്ലെങ്കിൽ // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ഘടന ttysize	ts;
	പൂർണ്ണസംഖ്യ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	എങ്കിൽ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	എങ്കിൽ (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_വരികൾ;
#   ifdef FEAT_EVAL
	    ch_log(ശൂന്യ, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    എങ്കിൽ (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ശൂന്യ)
    {
	എങ്കിൽ ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((അക്ഷരം *)p);
# ifdef FEAT_EVAL
	    ch_log(ശൂന്യ, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	എങ്കിൽ ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((അക്ഷരം *)p);
# ifdef FEAT_EVAL
	    ch_log(ശൂന്യ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    എങ്കിൽ (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(ശൂന്യ, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    എങ്കിൽ (columns <= 0 || rows <= 0)
	തിരികെ FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    തിരികെ OK;
}

# എങ്കിൽ defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    ശൂന്യം
mch_calc_cell_size(ഘടന cellsize *cs_out)
{
   ഘടന winsize   ws;
   പൂർണ്ണസംഖ്യ		    retval;
   പൂർണ്ണസംഖ്യ		    x_cell_size;
   പൂർണ്ണസംഖ്യ		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ശൂന്യ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   എങ്കിൽ (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	തിരികെ;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   എങ്കിൽ (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# എങ്കിൽ defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    പൂർണ്ണസംഖ്യ
mch_report_winsize(പൂർണ്ണസംഖ്യ fd, പൂർണ്ണസംഖ്യ rows, പൂർണ്ണസംഖ്യ cols)
{
    പൂർണ്ണസംഖ്യ		tty_fd;
    പൂർണ്ണസംഖ്യ		retval = -1;

    tty_fd = get_tty_fd(fd);
    എങ്കിൽ (tty_fd < 0)
	തിരികെ FAIL;

#  എങ്കിൽ defined(TIOCSWINSZ)
    ഘടന winsize ws;

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
    ch_log(ശൂന്യ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ഘടന ttysize ts;

    ts.ts_cols = cols;
    ts.ts_വരികൾ = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ശൂന്യ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    എങ്കിൽ (tty_fd != fd)
	close(tty_fd);
    തിരികെ retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    ശൂന്യം
mch_set_shellsize(ശൂന്യം)
{
    എങ്കിൽ (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((പൂർണ്ണസംഖ്യ)Rows, (പൂർണ്ണസംഖ്യ)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    ശൂന്യം
mch_new_shellsize(ശൂന്യം)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    സ്ഥിതം pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    ആകുമ്പോൾ (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	എങ്കിൽ (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    എങ്കിൽ (++delay_msec > 10)
		delay_msec = 10;
	    തുടരുക;
	}
	എങ്കിൽ (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    നിർത്തുക;
    }
    തിരികെ wait_pid;
}

#എങ്കിൽ !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    സ്ഥിതം ശൂന്യം
set_child_environment(
	long	rows,
	long	columns,
	അക്ഷരം	*term,
	പൂർണ്ണസംഖ്യ	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    അക്ഷരം	envbuf[50];
# അല്ലെങ്കിൽ
    സ്ഥിതം അക്ഷരം	envbuf_Term[30];
    സ്ഥിതം അക്ഷരം	envbuf_Rows[20];
    സ്ഥിതം അക്ഷരം	envbuf_വരികൾ[20];
    സ്ഥിതം അക്ഷരം	envbuf_Columns[20];
    സ്ഥിതം അക്ഷരം	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    സ്ഥിതം അക്ഷരം	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    സ്ഥിതം അക്ഷരം	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((അക്ഷരം *)envbuf, "%ld", rows);
    setenv("ROWS", (അക്ഷരം *)envbuf, 1);
    sprintf((അക്ഷരം *)envbuf, "%ld", rows);
    setenv("LINES", (അക്ഷരം *)envbuf, 1);
    sprintf((അക്ഷരം *)envbuf, "%ld", columns);
    setenv("COLUMNS", (അക്ഷരം *)envbuf, 1);
    sprintf((അക്ഷരം *)envbuf, "%d", t_colors);
    setenv("COLORS", (അക്ഷരം *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    എങ്കിൽ (is_terminal)
    {
	sprintf((അക്ഷരം *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (അക്ഷരം *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ശൂന്യ ? "" : (അക്ഷരം *)serverName, 1);
#  endif
# അല്ലെങ്കിൽ
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, വലുപ്പം(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, വലുപ്പം(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_വരികൾ, വലുപ്പം(envbuf_വരികൾ), "LINES=%ld", rows);
    putenv(envbuf_വരികൾ);
    vim_snprintf(envbuf_Columns, വലുപ്പം(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, വലുപ്പം(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    എങ്കിൽ (is_terminal)
    {
	vim_snprintf(envbuf_Version, വലുപ്പം(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, വലുപ്പം(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ശൂന്യ ? "" : (അക്ഷരം *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    സ്ഥിതം ശൂന്യം
set_default_child_environment(പൂർണ്ണസംഖ്യ is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    സ്ഥിതം ശൂന്യം
open_pty(പൂർണ്ണസംഖ്യ *pty_master_fd, പൂർണ്ണസംഖ്യ *pty_slave_fd, char_u **name1, char_u **name2)
{
    അക്ഷരം	*tty_name;

    എങ്കിൽ (name1 != ശൂന്യ)
	*name1 = ശൂന്യ;
    എങ്കിൽ (name2 != ശൂന്യ)
	*name2 = ശൂന്യ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    എങ്കിൽ (*pty_master_fd < 0)
	തിരികെ;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# അല്ലെങ്കിൽ
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    എങ്കിൽ (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (name1 != ശൂന്യ)
	    *name1 = vim_strsave((char_u *)tty_name);
	എങ്കിൽ (name2 != ശൂന്യ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    സ്ഥിതം ശൂന്യം
may_send_sigint(പൂർണ്ണസംഖ്യ c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    എങ്കിൽ (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# അല്ലെങ്കിൽ
	kill(0, SIGINT);
# endif
	എങ്കിൽ (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#എങ്കിൽ !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    പൂർണ്ണസംഖ്യ
unix_build_argv(
	char_u *cmd,
	അക്ഷരം ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    അക്ഷരം	**argv = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		argc;

    *sh_tofree = vim_strsave(p_sh);
    എങ്കിൽ (*sh_tofree == ശൂന്യ)		// out of memory
	തിരികെ FAIL;

    എങ്കിൽ (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	തിരികെ FAIL;
    *argvp = argv;

    എങ്കിൽ (cmd != ശൂന്യ)
    {
	char_u	*s;
	char_u	*p;

	എങ്കിൽ (extra_shell_arg != ശൂന്യ)
	    argv[argc++] = (അക്ഷരം *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	എങ്കിൽ (*shcf_tofree == ശൂന്യ)    // out of memory
	    തിരികെ FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	ആകുമ്പോൾ (*p != NUL)
	{
	    argv[argc++] = (അക്ഷരം *)s;
	    ആകുമ്പോൾ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (അക്ഷരം *)cmd;
    }
    argv[argc] = ശൂന്യ;
    തിരികെ OK;
}
#endif

#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_call_shell_terminal(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    അക്ഷരം	**argv = ശൂന്യ;
    char_u	*tofree1 = ശൂന്യ;
    char_u	*tofree2 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    എങ്കിൽ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	പോകുക theend;

    init_job_options(&opt);
    ch_log(ശൂന്യ, "starting terminal for system command '%s'", cmd);
    buf = term_start(ശൂന്യ, argv, &opt, TERM_START_SYSTEM);
    എങ്കിൽ (buf == ശൂന്യ)
	പോകുക theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    എങ്കിൽ (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	ആകുമ്പോൾ (term_use_loop())
	{
	    എങ്കിൽ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		എങ്കിൽ (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    അല്ലെങ്കിൽ
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ശൂന്യ, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    എങ്കിൽ (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    തിരികെ retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_call_shell_system(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    അക്ഷരം	*ifn = ശൂന്യ;
    അക്ഷരം	*ofn = ശൂന്യ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    പൂർണ്ണസംഖ്യ		x;

    out_flush();

    എങ്കിൽ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# എങ്കിൽ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  എങ്കിൽ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    എങ്കിൽ (cmd == ശൂന്യ)
	x = system((അക്ഷരം *)p_sh);
    അല്ലെങ്കിൽ
    {
# ifdef VMS
	എങ്കിൽ (ofn = strchr((അക്ഷരം *)cmd, '>'))
	    *ofn++ = '\0';
	എങ്കിൽ (ifn = strchr((അക്ഷരം *)cmd, '<'))
	{
	    അക്ഷരം *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    എങ്കിൽ (p)
		*p = '\0';
	}
	എങ്കിൽ (ofn)
	    x = vms_sys((അക്ഷരം *)cmd, ofn, ifn);
	അല്ലെങ്കിൽ
	    x = system((അക്ഷരം *)cmd);
# അല്ലെങ്കിൽ
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ശൂന്യ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	എങ്കിൽ (newcmd == ശൂന്യ)
	    x = 0;
	അല്ലെങ്കിൽ
	{
	    sprintf((അക്ഷരം *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ശൂന്യ ? "" : (അക്ഷരം *)extra_shell_arg,
		    (അക്ഷരം *)p_shcf,
		    (അക്ഷരം *)cmd);
	    x = system((അക്ഷരം *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    എങ്കിൽ (emsg_silent)
	;
    അല്ലെങ്കിൽ എങ്കിൽ (x == 127)
	സന്ദേശം_puts(_("\nCannot execute shell sh\n"));
    അല്ലെങ്കിൽ എങ്കിൽ (x && !(options & SHELL_SILENT))
    {
	സന്ദേശം_puts(_("\nshell returned "));
	സന്ദേശം_outnum((long)x);
	സന്ദേശം_putchar('\n');
    }

    എങ്കിൽ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# എങ്കിൽ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    തിരികെ x;
}

#അല്ലെങ്കിൽ // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_call_shell_fork(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    പൂർണ്ണസംഖ്യ		status = -1;
    പൂർണ്ണസംഖ്യ		retval = -1;
    അക്ഷരം	**argv = ശൂന്യ;
    char_u	*tofree1 = ശൂന്യ;
    char_u	*tofree2 = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		i;
    പൂർണ്ണസംഖ്യ		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    പൂർണ്ണസംഖ്യ		pty_slave_fd = -1;
# endif
    പൂർണ്ണസംഖ്യ		fd_toshell[2];		// for pipes
    പൂർണ്ണസംഖ്യ		fd_fromshell[2];
    പൂർണ്ണസംഖ്യ		pipe_error = FALSE;
    പൂർണ്ണസംഖ്യ		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    എങ്കിൽ (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    എങ്കിൽ (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    എങ്കിൽ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	പോകുക error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    എങ്കിൽ ((options & (SHELL_READ|SHELL_WRITE))
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
	എങ്കിൽ (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ശൂന്യ, ശൂന്യ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	എങ്കിൽ (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    എങ്കിൽ (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		എങ്കിൽ (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    എങ്കിൽ (pipe_error)
	    {
		സന്ദേശം_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    എങ്കിൽ (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	എങ്കിൽ (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    സന്ദേശം_puts(_("\nCannot fork\n"));
	    എങ്കിൽ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		എങ്കിൽ (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		അല്ലെങ്കിൽ				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	അല്ലെങ്കിൽ എങ്കിൽ (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    എങ്കിൽ (ch_log_active())
	    {
		ch_log(ശൂന്യ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    എങ്കിൽ (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		പൂർണ്ണസംഖ്യ fd;

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
		എങ്കിൽ (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		എങ്കിൽ (p_stmp)
		{
		    (ശൂന്യം)setsid();
#  എങ്കിൽ defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		എങ്കിൽ (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    എങ്കിൽ (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (അക്ഷരം *)ശൂന്യ);
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
		എങ്കിൽ (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    എങ്കിൽ (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		അല്ലെങ്കിൽ
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
		    എങ്കിൽ (gui.in_use)
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
	അല്ലെങ്കിൽ			// parent
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
	    എങ്കിൽ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		പൂർണ്ണസംഖ്യ	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		പൂർണ്ണസംഖ്യ	    ta_len = 0;		// valid bytes in ta_buf[]
		പൂർണ്ണസംഖ്യ	    len;
		പൂർണ്ണസംഖ്യ	    p_more_save;
		പൂർണ്ണസംഖ്യ	    old_State;
		പൂർണ്ണസംഖ്യ	    c;
		പൂർണ്ണസംഖ്യ	    toshell_fd;
		പൂർണ്ണസംഖ്യ	    fromshell_fd;
		garray_T    ga;
		പൂർണ്ണസംഖ്യ	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		എങ്കിൽ (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		അല്ലെങ്കിൽ
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

		എങ്കിൽ ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    എങ്കിൽ ((wpid = fork()) == -1)
		    {
			സന്ദേശം_puts(_("\nCannot fork\n"));
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			വേണ്ടി (;;)
			{
			    എങ്കിൽ (lplen == 0)
				len = 0;
			    അല്ലെങ്കിൽ എങ്കിൽ (lp[written] == NL)
				// NL -> NUL translation
				len = ലിഖ(toshell_fd, "", (size_t)1);
			    അല്ലെങ്കിൽ
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = ലിഖ(toshell_fd, (അക്ഷരം *)lp + written,
					   s == ശൂന്യ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    എങ്കിൽ (len == (പൂർണ്ണസംഖ്യ)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				എങ്കിൽ (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_വരി_എണ്ണം
						    || curbuf->b_p_eol)))
				    vim_ignored = ലിഖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				എങ്കിൽ (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    നിർത്തുക;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    അല്ലെങ്കിൽ എങ്കിൽ (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    അല്ലെങ്കിൽ // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		എങ്കിൽ (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		വേണ്ടി (;;)
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
		    എങ്കിൽ (!(options & SHELL_EXPAND)
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
		      എങ്കിൽ (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      എങ്കിൽ (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			എങ്കിൽ (len == 1 && (pty_master_fd < 0 || cmd != ശൂന്യ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    എങ്കിൽ (pty_master_fd < 0 && toshell_fd >= 0
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
			എങ്കിൽ (pty_master_fd < 0)
			{
			    വേണ്ടി (i = ta_len; i < ta_len + len; ++i)
			    {
				എങ്കിൽ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    സന്ദേശം_putchar(ta_buf[i]);
				അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
				{
				    പൂർണ്ണസംഖ്യ l = (*mb_ptr2len)(ta_buf + i);

				    സന്ദേശം_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				അല്ലെങ്കിൽ
				    സന്ദേശം_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
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
			എങ്കിൽ (options & SHELL_WRITE)
			    ta_len = 0;
			അല്ലെങ്കിൽ എങ്കിൽ (toshell_fd >= 0)
			{
			    len = ലിഖ(toshell_fd, (അക്ഷരം *)ta_buf, (size_t)1);
			    എങ്കിൽ (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    എങ്കിൽ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# അല്ലെങ്കിൽ
			kill(0, SIGINT);
# endif
			എങ്കിൽ (wpid > 0)
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
		    ആകുമ്പോൾ (RealWaitForChar(fromshell_fd, 10L, ശൂന്യ, ശൂന്യ))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			എങ്കിൽ (len <= 0)		    // end of file or error
			    പോകുക finished;

			noread_cnt = 0;
			എങ്കിൽ (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    വേണ്ടി (i = 0; i < len; ++i)
			    {
				എങ്കിൽ (buffer[i] == NL)
				    append_ga_വരി(&ga);
				അല്ലെങ്കിൽ എങ്കിൽ (buffer[i] == NUL)
				    ga_append(&ga, NL);
				അല്ലെങ്കിൽ
				    ga_append(&ga, buffer[i]);
			    }
			}
			അല്ലെങ്കിൽ എങ്കിൽ (has_mbyte)
			{
			    പൂർണ്ണസംഖ്യ		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    വേണ്ടി (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				എങ്കിൽ (l == 0)
				    l = 1;  // NUL byte?
				അല്ലെങ്കിൽ എങ്കിൽ (MB_BYTE2LEN(*p) != l)
				    നിർത്തുക;
			    }
			    എങ്കിൽ (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				എങ്കിൽ (len >= 12)
				    ++p;
				അല്ലെങ്കിൽ
				{
				    buffer_off = len;
				    തുടരുക;
				}
			    }
			    c = *p;
			    *p = NUL;
			    സന്ദേശം_puts((അക്ഷരം *)buffer);
			    എങ്കിൽ (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				തുടരുക;
			    }
			    buffer_off = 0;
			}
			അല്ലെങ്കിൽ
			{
			    buffer[len] = NUL;
			    സന്ദേശം_puts((അക്ഷരം *)buffer);
			}

			windgoto(സന്ദേശം_row, cmdline_col_off + സന്ദേശം_col);
			cursor_on();
			out_flush();
			എങ്കിൽ (got_int)
			    നിർത്തുക;

# ifdef ELAPSED_FUNC
			എങ്കിൽ (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    എങ്കിൽ (msec > 2000)
			    {
				noread_cnt = 5;
				നിർത്തുക;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    എങ്കിൽ (wait_pid == pid)
		    {
			എങ്കിൽ (noread_cnt < 5)
			    തുടരുക;
			നിർത്തുക;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    എങ്കിൽ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    അല്ലെങ്കിൽ
			wait_pid = 0;

# എങ്കിൽ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		എങ്കിൽ (options & SHELL_READ)
		{
		    എങ്കിൽ (ga.ga_len > 0)
		    {
			append_ga_വരി(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    അല്ലെങ്കിൽ
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		എങ്കിൽ (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		എങ്കിൽ (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# എങ്കിൽ (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    അല്ലെങ്കിൽ
	    {
		long delay_msec = 1;

		എങ്കിൽ (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		വേണ്ടി (;;)
		{
		    എങ്കിൽ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  അല്ലെങ്കിൽ
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    എങ്കിൽ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			നിർത്തുക;
		    }

#  എങ്കിൽ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    എങ്കിൽ (++delay_msec > 10)
			delay_msec = 10;
		}

		എങ്കിൽ (tmode == TMODE_RAW)
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
	    എങ്കിൽ (wait_pid != pid)
		(ശൂന്യം)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    എങ്കിൽ (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    എങ്കിൽ (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ശൂന്യ);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    എങ്കിൽ (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    എങ്കിൽ (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		എങ്കിൽ (retval != 0 && !emsg_silent)
		{
		    എങ്കിൽ (retval == EXEC_FAILED)
		    {
			സന്ദേശം_puts(_("\nCannot execute shell "));
			സന്ദേശം_outtrans(p_sh);
			സന്ദേശം_putchar('\n');
		    }
		    അല്ലെങ്കിൽ എങ്കിൽ (!(options & SHELL_SILENT))
		    {
			സന്ദേശം_puts(_("\nshell returned "));
			സന്ദേശം_outnum((long)retval);
			സന്ദേശം_putchar('\n');
		    }
		}
	    }
	    അല്ലെങ്കിൽ
		സന്ദേശം_puts(_("\nCommand terminated\n"));
	}
    }

error:
    എങ്കിൽ (!did_settmode)
	എങ്കിൽ (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    തിരികെ retval;
}
#endif // USE_SYSTEM

    പൂർണ്ണസംഖ്യ
mch_call_shell(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(ശൂന്യ, "executing shell command: %s", cmd);
#endif
#എങ്കിൽ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    എങ്കിൽ (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ശൂന്യ
					      && (options & SHELL_SILENT) == 0)
	തിരികെ mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    തിരികെ mch_call_shell_system(cmd, options);
#അല്ലെങ്കിൽ
    തിരികെ mch_call_shell_fork(cmd, options);
#endif
}

#എങ്കിൽ defined(FEAT_EVAL)
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
    അക്ഷരം	**argv,
    char_u	*infile,
    പൂർണ്ണസംഖ്യ		flags UNUSED,
    പൂർണ്ണസംഖ്യ		*ret_len)
{
    pid_t	pid;
    പൂർണ്ണസംഖ്യ		fd_out[2] = {-1, -1};
    പൂർണ്ണസംഖ്യ		status = -1;
    char_u	*buffer = ശൂന്യ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ശൂന്യ, "directly executing: %s", argv[0]);

    എങ്കിൽ (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	തിരികെ ശൂന്യ;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# അല്ലെങ്കിൽ
    pid = fork();
# endif
    എങ്കിൽ (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	തിരികെ ശൂന്യ;
    }

    എങ്കിൽ (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	എങ്കിൽ (ch_log_active())
	{
	    ch_log(ശൂന്യ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	എങ്കിൽ (infile != ശൂന്യ)
	{
	    പൂർണ്ണസംഖ്യ fd_in = open((അക്ഷരം *)infile, O_RDONLY);
	    എങ്കിൽ (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	അല്ലെങ്കിൽ
	{
	    പൂർണ്ണസംഖ്യ nullfd = open("/dev/null", O_RDONLY);
	    എങ്കിൽ (nullfd >= 0)
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
    വേണ്ടി (;;)
    {
	അക്ഷരം	buf[4096];
	പൂർണ്ണസംഖ്യ	n;

	n = (പൂർണ്ണസംഖ്യ)പഠ(fd_out[0], buf, വലുപ്പം(buf));
	എങ്കിൽ (n <= 0)
	    നിർത്തുക;
	എങ്കിൽ (ga_grow(&ga, n) == FAIL)
	    നിർത്തുക;
	mch_memmove((അക്ഷരം *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (ശൂന്യം)waitpid(pid, &status, 0);
    എങ്കിൽ (WIFEXITED(status))
	status = WEXITSTATUS(status);
    അല്ലെങ്കിൽ
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    എങ്കിൽ (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	എങ്കിൽ (buffer != ശൂന്യ)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    എങ്കിൽ (ret_len == ശൂന്യ)
	    {
		പൂർണ്ണസംഖ്യ	i;

		// Change NUL into SOH, otherwise the string is truncated.
		വേണ്ടി (i = 0; i < ga.ga_len; ++i)
		    എങ്കിൽ (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    അല്ലെങ്കിൽ
		*ret_len = ga.ga_len;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (ret_len != ശൂന്യ)
	*ret_len = 0;

    ga_clear(&ga);
    തിരികെ buffer;
}
#endif

#എങ്കിൽ defined(FEAT_JOB_CHANNEL)
    ശൂന്യം
mch_job_start(അക്ഷരം **argv, job_T *job, jobopt_T *options, പൂർണ്ണസംഖ്യ is_terminal)
{
    pid_t	pid;
    പൂർണ്ണസംഖ്യ		fd_in[2] = {-1, -1};	// for stdin
    പൂർണ്ണസംഖ്യ		fd_out[2] = {-1, -1};	// for stdout
    പൂർണ്ണസംഖ്യ		fd_err[2] = {-1, -1};	// for stderr
    പൂർണ്ണസംഖ്യ		pty_master_fd = -1;
    പൂർണ്ണസംഖ്യ		pty_slave_fd = -1;
    channel_T	*channel = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    പൂർണ്ണസംഖ്യ		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    പൂർണ്ണസംഖ്യ		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    പൂർണ്ണസംഖ്യ		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    എങ്കിൽ (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    എങ്കിൽ (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    എങ്കിൽ (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((അക്ഷരം *)fname, O_RDONLY, 0);
	എങ്കിൽ (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    അല്ലെങ്കിൽ
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	എങ്കിൽ (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    പോകുക failed;

    എങ്കിൽ (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((അക്ഷരം *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	എങ്കിൽ (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    അല്ലെങ്കിൽ എങ്കിൽ (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	പോകുക failed;

    എങ്കിൽ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((അക്ഷരം *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	എങ്കിൽ (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    പോകുക failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    അല്ലെങ്കിൽ എങ്കിൽ (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	പോകുക failed;

    എങ്കിൽ (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	എങ്കിൽ (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    എങ്കിൽ (channel != ശൂന്യ)
		++channel->ch_refcount;
	}
	അല്ലെങ്കിൽ
	    channel = add_channel();
	എങ്കിൽ (channel == ശൂന്യ)
	    പോകുക failed;
	എങ്കിൽ (job->jv_tty_out != ശൂന്യ)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    എങ്കിൽ (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	പോകുക failed;
    }
    എങ്കിൽ (pid == 0)
    {
	പൂർണ്ണസംഖ്യ	null_fd = -1;
	പൂർണ്ണസംഖ്യ	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	എങ്കിൽ (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(ശൂന്യം)setsid();
# endif

# ifdef FEAT_TERMINAL
	എങ്കിൽ (options->jo_term_rows > 0)
	{
	    അക്ഷരം *term = (അക്ഷരം *)T_NAME;

#  ifdef FEAT_GUI
	    എങ്കിൽ (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    എങ്കിൽ (term == ശൂന്യ || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		എങ്കിൽ (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		അല്ലെങ്കിൽ എങ്കിൽ (t_colors > 16)
		    term = "xterm-color";
		അല്ലെങ്കിൽ
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	അല്ലെങ്കിൽ
# endif
	    set_default_child_environment(is_terminal);

	എങ്കിൽ (options->jo_env != ശൂന്യ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    പൂർണ്ണസംഖ്യ		todo = (പൂർണ്ണസംഖ്യ)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_ഇനങ്ങൾ(&dict->dv_hashtab, hi, todo)
		എങ്കിൽ (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_താക്കോൽ, tv_get_string(item));
		    --todo;
		}
	}

	എങ്കിൽ (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    എങ്കിൽ (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	എങ്കിൽ (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (അക്ഷരം *)ശൂന്യ);
# endif
	}

	// set up stdin for the child
	close(0);
	എങ്കിൽ (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	അല്ലെങ്കിൽ എങ്കിൽ (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	അല്ലെങ്കിൽ
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	എങ്കിൽ (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	അല്ലെങ്കിൽ എങ്കിൽ (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	അല്ലെങ്കിൽ എങ്കിൽ (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	അല്ലെങ്കിൽ
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	എങ്കിൽ (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	അല്ലെങ്കിൽ എങ്കിൽ (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	അല്ലെങ്കിൽ
	    vim_ignored = dup(fd_out[1]);

	എങ്കിൽ (fd_in[0] >= 0)
	    close(fd_in[0]);
	എങ്കിൽ (fd_in[1] >= 0)
	    close(fd_in[1]);
	എങ്കിൽ (fd_out[0] >= 0)
	    close(fd_out[0]);
	എങ്കിൽ (fd_out[1] >= 0)
	    close(fd_out[1]);
	എങ്കിൽ (fd_err[0] >= 0)
	    close(fd_err[0]);
	എങ്കിൽ (fd_err[1] >= 0)
	    close(fd_err[1]);
	എങ്കിൽ (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	എങ്കിൽ (null_fd >= 0)
	    close(null_fd);

	എങ്കിൽ (options->jo_cwd != ശൂന്യ && mch_chdir((അക്ഷരം *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	എങ്കിൽ (stderr_works)
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

    എങ്കിൽ (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    എങ്കിൽ (fd_in[0] >= 0)
	close(fd_in[0]);
    എങ്കിൽ (fd_out[1] >= 0)
	close(fd_out[1]);
    എങ്കിൽ (fd_err[1] >= 0)
	close(fd_err[1]);
    എങ്കിൽ (channel != ശൂന്യ)
    {
	പൂർണ്ണസംഖ്യ in_fd = INVALID_FD;
	പൂർണ്ണസംഖ്യ out_fd = INVALID_FD;
	പൂർണ്ണസംഖ്യ err_fd = INVALID_FD;

	എങ്കിൽ (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	എങ്കിൽ (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	എങ്കിൽ (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    എങ്കിൽ (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    അല്ലെങ്കിൽ എങ്കിൽ (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (fd_in[1] >= 0)
	    close(fd_in[1]);
	എങ്കിൽ (fd_out[0] >= 0)
	    close(fd_out[0]);
	എങ്കിൽ (fd_err[0] >= 0)
	    close(fd_err[0]);
	എങ്കിൽ (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    തിരികെ;

failed:
    channel_unref(channel);
    എങ്കിൽ (fd_in[0] >= 0)
	close(fd_in[0]);
    എങ്കിൽ (fd_in[1] >= 0)
	close(fd_in[1]);
    എങ്കിൽ (fd_out[0] >= 0)
	close(fd_out[0]);
    എങ്കിൽ (fd_out[1] >= 0)
	close(fd_out[1]);
    എങ്കിൽ (fd_err[0] >= 0)
	close(fd_err[0]);
    എങ്കിൽ (fd_err[1] >= 0)
	close(fd_err[1]);
    എങ്കിൽ (pty_master_fd >= 0)
	close(pty_master_fd);
    എങ്കിൽ (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    സ്ഥിതം char_u *
get_signal_name(പൂർണ്ണസംഖ്യ sig)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	numbuf[NUMBUFLEN];

    എങ്കിൽ (sig == SIGKILL)
	തിരികെ vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    വേണ്ടി (i = 0; signal_info[i].sig != -1; i++)
	എങ്കിൽ (sig == signal_info[i].sig)
	    തിരികെ strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((അക്ഷരം *)numbuf, NUMBUFLEN, "%d", sig);
    തിരികെ vim_strnsave(numbuf, i);
}

    അക്ഷരം *
mch_job_status(job_T *job)
{
    പൂർണ്ണസംഖ്യ		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    എങ്കിൽ (wait_pid == -1)
    {
	പൂർണ്ണസംഖ്യ waitpid_errno = errno;
	എങ്കിൽ (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    തിരികെ "run";

	// process must have exited
	എങ്കിൽ (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	പോകുക return_dead;
    }
    എങ്കിൽ (wait_pid == 0)
	തിരികെ "run";
    എങ്കിൽ (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	എങ്കിൽ (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	പോകുക return_dead;
    }
    എങ്കിൽ (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	എങ്കിൽ (job->jv_status < JOB_ENDED && job->jv_termsig != ശൂന്യ)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	പോകുക return_dead;
    }
    തിരികെ "run";

return_dead:
    എങ്കിൽ (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    തിരികെ "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    പൂർണ്ണസംഖ്യ		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    എങ്കിൽ (dont_check_job_ended > 0)
	തിരികെ ശൂന്യ;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    എങ്കിൽ (wait_pid <= 0)
	// no process ended
	തിരികെ ശൂന്യ;
    വേണ്ടി (job = job_list; job != ശൂന്യ; job = job->jv_next)
    {
	എങ്കിൽ (job->jv_pid == wait_pid)
	{
	    എങ്കിൽ (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    അല്ലെങ്കിൽ എങ്കിൽ (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    എങ്കിൽ (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    തിരികെ job;
	}
    }
    തിരികെ ശൂന്യ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    പൂർണ്ണസംഖ്യ
mch_signal_job(job_T *job, char_u *how)
{
    പൂർണ്ണസംഖ്യ	    sig = -1;

    എങ്കിൽ (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(how, "int") == 0)
	sig = SIGINT;
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    അല്ലെങ്കിൽ എങ്കിൽ (SAFE_isdigit(*how))
	sig = atoi((അക്ഷരം *)how);
    അല്ലെങ്കിൽ
	തിരികെ FAIL;

    // Never kill ourselves!
    എങ്കിൽ (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    തിരികെ OK;
}

/*
 * Clear the data related to "job".
 */
    ശൂന്യം
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (ശൂന്യം)waitpid(job->jv_pid, ശൂന്യ, WNOHANG);
}
#endif

#എങ്കിൽ defined(FEAT_TERMINAL)
    പൂർണ്ണസംഖ്യ
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    പൂർണ്ണസംഖ്യ		pty_master_fd = -1;
    പൂർണ്ണസംഖ്യ		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    എങ്കിൽ (pty_master_fd < 0 || pty_slave_fd < 0)
	തിരികെ FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    എങ്കിൽ (channel == ശൂന്യ)
    {
	close(pty_master_fd);
	തിരികെ FAIL;
    }
    എങ്കിൽ (job->jv_tty_out != ശൂന്യ)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    തിരികെ OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    ശൂന്യം
mch_breakcheck(പൂർണ്ണസംഖ്യ force)
{
    എങ്കിൽ ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ശൂന്യ, ശൂന്യ))
	fill_നിവേശം_buf(FALSE);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
WaitForChar(long msec, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം)
{
#ifdef FEAT_TIMERS
    തിരികെ ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_നിവേശം) == OK;
#അല്ലെങ്കിൽ
    തിരികെ WaitForCharOrMouse(msec, interrupted, ignore_നിവേശം);
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
WaitForCharOrMouse(long msec, പൂർണ്ണസംഖ്യ *interrupted, പൂർണ്ണസംഖ്യ ignore_നിവേശം)
{
#ifdef FEAT_MOUSE_GPM
    പൂർണ്ണസംഖ്യ		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    പൂർണ്ണസംഖ്യ		rest;
#endif
    പൂർണ്ണസംഖ്യ		avail;

    എങ്കിൽ (!ignore_നിവേശം && നിവേശം_available())	    // something in inbuf[]
	തിരികെ 1;

#എങ്കിൽ defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    എങ്കിൽ (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	എങ്കിൽ (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#എങ്കിൽ defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    എങ്കിൽ (do_xterm_trace())
	rest = msec;
# endif
    ചെയ്യുക
    {
# ifdef FEAT_XCLIPBOARD
	എങ്കിൽ (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    എങ്കിൽ (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    എങ്കിൽ (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	എങ്കിൽ (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	എങ്കിൽ (!avail && !gpm_process_wanted)
# അല്ലെങ്കിൽ
	avail = RealWaitForChar(read_cmd_fd, msec, ശൂന്യ, interrupted);
	എങ്കിൽ (!avail)
# endif
	{
	    എങ്കിൽ (!ignore_നിവേശം && നിവേശം_available())
		തിരികെ 1;
# ifdef FEAT_XCLIPBOARD
	    എങ്കിൽ (rest == 0 || !do_xterm_trace())
# endif
		നിർത്തുക;
	}
    }
    ആകുമ്പോൾ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#അല്ലെങ്കിൽ
    avail = RealWaitForChar(read_cmd_fd, msec, ശൂന്യ, interrupted);
#endif
    തിരികെ avail;
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
    സ്ഥിതം പൂർണ്ണസംഖ്യ
RealWaitForChar(പൂർണ്ണസംഖ്യ fd, long msec, പൂർണ്ണസംഖ്യ *check_for_gpm UNUSED, പൂർണ്ണസംഖ്യ *interrupted)
{
    പൂർണ്ണസംഖ്യ		ret;
    പൂർണ്ണസംഖ്യ		result;
# എങ്കിൽ defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    സ്ഥിതം പൂർണ്ണസംഖ്യ	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    എങ്കിൽ (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    എങ്കിൽ (busy)
	തിരികെ 0;
# endif

# ifdef MAY_LOOP
    വേണ്ടി (;;)
# endif
    {
# ifdef MAY_LOOP
	പൂർണ്ണസംഖ്യ		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	പൂർണ്ണസംഖ്യ		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	പൂർണ്ണസംഖ്യ		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ഘടന pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	പൂർണ്ണസംഖ്യ		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	പൂർണ്ണസംഖ്യ             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	പൂർണ്ണസംഖ്യ		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	പൂർണ്ണസംഖ്യ		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	പൂർണ്ണസംഖ്യ		xsmp_idx = -1;
#  endif
	പൂർണ്ണസംഖ്യ		towait = (പൂർണ്ണസംഖ്യ)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	എങ്കിൽ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (പൂർണ്ണസംഖ്യ)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	എങ്കിൽ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	എങ്കിൽ (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_സംഖ്യ(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	എങ്കിൽ (check_for_gpm != ശൂന്യ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	എങ്കിൽ (xsmp_icefd != -1)
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
	എങ്കിൽ (interrupted != ശൂന്യ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	എങ്കിൽ (result == 0 && interrupted != ശൂന്യ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	എങ്കിൽ (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	എങ്കിൽ (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	എങ്കിൽ (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    എങ്കിൽ (--ret == 0 && !നിവേശം_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	എങ്കിൽ (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	എങ്കിൽ (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    എങ്കിൽ (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (fds[xsmp_idx].revents & POLLHUP)
	    {
		എങ്കിൽ (p_verbose > 0)
		    verb_സന്ദേശം(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    എങ്കിൽ (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	എങ്കിൽ (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# അല്ലെങ്കിൽ // HAVE_SELECT

	ഘടന timeval  tv;
	ഘടന timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	സ്ഥിതം fd_set	rfds, wfds, efds;
	പൂർണ്ണസംഖ്യ		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	എങ്കിൽ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	എങ്കിൽ (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	അല്ലെങ്കിൽ
	    tvp = ശൂന്യ;

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
	എങ്കിൽ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    എങ്കിൽ (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	എങ്കിൽ (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_സംഖ്യ(xterm_dpy), &rfds);
	    എങ്കിൽ (maxfd < Connection_സംഖ്യ(xterm_dpy))
		maxfd = Connection_സംഖ്യ(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	എങ്കിൽ (check_for_gpm != ശൂന്യ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    എങ്കിൽ (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	എങ്കിൽ (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    എങ്കിൽ (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	എങ്കിൽ (interrupted != ശൂന്യ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	എങ്കിൽ (result)
	    --ret;
	അല്ലെങ്കിൽ എങ്കിൽ (interrupted != ശൂന്യ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	എങ്കിൽ (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    എങ്കിൽ (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    എങ്കിൽ (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ശൂന്യ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    പോകുക select_eintr;
	}
#  endif
#  ifdef __TANDEM
	എങ്കിൽ (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	എങ്കിൽ (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	എങ്കിൽ (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	എങ്കിൽ (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_സംഖ്യ(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    എങ്കിൽ (--ret == 0 && !നിവേശം_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	എങ്കിൽ (ret > 0 && check_for_gpm != ശൂന്യ && gpm_flag && gpm_fd >= 0)
	{
	    എങ്കിൽ (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    അല്ലെങ്കിൽ എങ്കിൽ (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	എങ്കിൽ (ret > 0 && xsmp_icefd != -1)
	{
	    എങ്കിൽ (FD_ISSET(xsmp_icefd, &efds))
	    {
		എങ്കിൽ (p_verbose > 0)
		    verb_സന്ദേശം(_("XSMP lost ICE connection"));
		xsmp_close();
		എങ്കിൽ (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    അല്ലെങ്കിൽ എങ്കിൽ (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		എങ്കിൽ (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	എങ്കിൽ (ret >= 0)
	    (ശൂന്യം)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	എങ്കിൽ (finished || msec == 0)
	    നിർത്തുക;

#  എങ്കിൽ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	എങ്കിൽ (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    നിർത്തുക;
#  endif

	// We're going to loop around again, find out for how long
	എങ്കിൽ (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  അല്ലെങ്കിൽ
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    എങ്കിൽ (msec <= 0)
		നിർത്തുക;	// waited long enough
	}
# endif
    }

    തിരികെ result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    പൂർണ്ണസംഖ്യ
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    പൂർണ്ണസംഖ്യ		flags)		// EW_* flags
{
    തിരികെ unix_expandpath(gap, path, 0, flags, FALSE);
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

    പൂർണ്ണസംഖ്യ
mch_expand_wildcards(
    പൂർണ്ണസംഖ്യ		   അക്കം_pat,
    char_u	 **pat,
    പൂർണ്ണസംഖ്യ		  *അക്കം_file,
    char_u	***file,
    പൂർണ്ണസംഖ്യ		   flags)	// EW_* flags
{
    പൂർണ്ണസംഖ്യ		i;
    size_t	len;
    long	llen;
    char_u	*p;
    പൂർണ്ണസംഖ്യ		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    പൂർണ്ണസംഖ്യ		j;
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
    പൂർണ്ണസംഖ്യ		shell_style = STYLE_ECHO;
    പൂർണ്ണസംഖ്യ		check_spaces;
    സ്ഥിതം പൂർണ്ണസംഖ്യ	did_find_nul = FALSE;
    പൂർണ്ണസംഖ്യ		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    സ്ഥിതം string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    സ്ഥിതം string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *അക്കം_file = 0;	// default: no files found
    *file = ശൂന്യ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    എങ്കിൽ (!have_wildcard(അക്കം_pat, pat))
	തിരികെ save_patterns(അക്കം_pat, pat, അക്കം_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    എങ്കിൽ (sandbox != 0 && check_secure())
	തിരികെ FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    എങ്കിൽ (secure || restricted)
	വേണ്ടി (i = 0; i < അക്കം_pat; ++i)
	    എങ്കിൽ (vim_strchr(pat[i], '`') != ശൂന്യ
		    && (check_restricted() || check_secure()))
		തിരികെ FAIL;

    /*
     * get a name for the temp file
     */
    എങ്കിൽ ((tempname = vim_tempname('o', FALSE)) == ശൂന്യ)
    {
	emsg(_(e_cant_get_temp_file_name));
	തിരികെ FAIL;
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
    എങ്കിൽ (അക്കം_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    അല്ലെങ്കിൽ എങ്കിൽ ((len = STRLEN(p_sh)) >= 3)
    {
	എങ്കിൽ (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	അല്ലെങ്കിൽ എങ്കിൽ (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    എങ്കിൽ (shell_style == STYLE_ECHO)
    {
	എങ്കിൽ (strstr((അക്ഷരം *)gettail(p_sh), "bash") != ശൂന്യ)
	    shell_style = STYLE_GLOBSTAR;
	അല്ലെങ്കിൽ എങ്കിൽ (strstr((അക്ഷരം *)gettail(p_sh), "sh") != ശൂന്യ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    എങ്കിൽ (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    വേണ്ടി (i = 0; i < അക്കം_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# അല്ലെങ്കിൽ
	++len;				// add space
	വേണ്ടി (j = 0; pat[i][j] != NUL; ++j)
	{
	    എങ്കിൽ (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ശൂന്യ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    എങ്കിൽ (command == ശൂന്യ)
    {
	// out of memory
	vim_free(tempname);
	തിരികെ FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    എങ്കിൽ (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((അക്ഷരം *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((അക്ഷരം *)command, ")>");
	എങ്കിൽ (p == ശൂന്യ)
	{
	    ;					    // can't happen ;-)
	}
	അല്ലെങ്കിൽ
	{
	    --p;
	    ആകുമ്പോൾ (p > command && VIM_ISWHITE(*p))
		--p;
	    എങ്കിൽ (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    ആകുമ്പോൾ (p > command && VIM_ISWHITE(*p))
		--p;
	    എങ്കിൽ (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((അക്ഷരം *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((അക്ഷരം *)command, len, "print -N >%s", tempname);
	അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((അക്ഷരം *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((അക്ഷരം *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	അല്ലെങ്കിൽ
	    commandlen = vim_snprintf((അക്ഷരം *)command, len, "echo >%s", tempname);

	വേണ്ടി (i = 0; i < അക്കം_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((അക്ഷരം *)command + commandlen, len, " \"%s\"", pat[i]);
# അല്ലെങ്കിൽ
	    പൂർണ്ണസംഖ്യ intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    വേണ്ടി (j = 0; pat[i][j] != NUL; ++j)
	    {
		എങ്കിൽ (pat[i][j] == '`')
		    intick = !intick;
		അല്ലെങ്കിൽ എങ്കിൽ (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    എങ്കിൽ (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ശൂന്യ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		അല്ലെങ്കിൽ എങ്കിൽ (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ശൂന്യ)
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
    എങ്കിൽ (flags & EW_SILENT)
	show_shell_mess = FALSE;
    എങ്കിൽ (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    എങ്കിൽ (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_GLOB && !have_dollars(അക്കം_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    എങ്കിൽ (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ശൂന്യ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    എങ്കിൽ (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	എങ്കിൽ (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    സന്ദേശം_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    എങ്കിൽ (!(flags & EW_SILENT))
# endif
	    {
		സന്ദേശം(_(e_cannot_expand_wildcards));
		സന്ദേശം_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	എങ്കിൽ (shell_style == STYLE_BT)
	    തിരികെ FAIL;
	പോകുക notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((അക്ഷരം *)tempname, READBIN);
    എങ്കിൽ (fd == ശൂന്യ)
    {
	// Something went wrong, perhaps a file name with a special char.
	എങ്കിൽ (!(flags & EW_SILENT))
	{
	    സന്ദേശം(_(e_cannot_expand_wildcards));
	    സന്ദേശം_start();		// don't overwrite this message
	}
	vim_free(tempname);
	പോകുക notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    എങ്കിൽ (llen < 0)
	// just in case ftell() would fail
	buffer = ശൂന്യ;
    അല്ലെങ്കിൽ
	buffer = alloc(llen + 1);
    എങ്കിൽ (buffer == ശൂന്യ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	തിരികെ FAIL;
    }
    len = llen;
    i = fread((അക്ഷരം *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    എങ്കിൽ (i != (പൂർണ്ണസംഖ്യ)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	തിരികെ FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    വേണ്ടി (i = 0; i < (പൂർണ്ണസംഖ്യ)len; ++i)
	എങ്കിൽ (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    എങ്കിൽ (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	വേണ്ടി (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    ആകുമ്പോൾ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    അല്ലെങ്കിൽ എങ്കിൽ (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	വേണ്ടി (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    ആകുമ്പോൾ (*p != '\n' && *p != NUL)
		++p;
	    എങ്കിൽ (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    അല്ലെങ്കിൽ
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
	എങ്കിൽ (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    എങ്കിൽ (len && (പൂർണ്ണസംഖ്യ)STRLEN(buffer) < (പൂർണ്ണസംഖ്യ)len)
		did_find_nul = TRUE;
	    അല്ലെങ്കിൽ
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	എങ്കിൽ (len && buffer[len - 1] == NUL)
	    --len;
	അല്ലെങ്കിൽ
	    buffer[len] = NUL;
	i = 0;
	വേണ്ടി (p = buffer; p < buffer + len; ++p)
	    എങ്കിൽ (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	എങ്കിൽ (len)
	    ++i;			// count last entry
    }
    എങ്കിൽ (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	പോകുക notfound;
    }
    *അക്കം_file = i;
    *file = ALLOC_MULT(char_u *, i);
    എങ്കിൽ (*file == ശൂന്യ)
    {
	// out of memory
	vim_free(buffer);
	തിരികെ FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    വേണ്ടി (i = 0; i < *അക്കം_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	എങ്കിൽ (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    ആകുമ്പോൾ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    എങ്കിൽ (p == buffer + len)		// last entry
		*p = NUL;
	    അല്ലെങ്കിൽ
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	അല്ലെങ്കിൽ		// NUL separates
	{
	    ആകുമ്പോൾ (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    വേണ്ടി (j = 0, i = 0; i < *അക്കം_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	എങ്കിൽ (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    തുടരുക;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	എങ്കിൽ ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    തുടരുക;

	// Skip files that are not executable if we check for that.
	എങ്കിൽ (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ശൂന്യ, !(flags & EW_SHELLCMD)))
	    തുടരുക;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	എങ്കിൽ (p)
	{
	    STRCPY(p, (*file)[i]);
	    എങ്കിൽ (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *അക്കം_file = j;

    എങ്കിൽ (*അക്കം_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	പോകുക notfound;
    }

    തിരികെ OK;

notfound:
    എങ്കിൽ (flags & EW_NOTFOUND)
	തിരികെ save_patterns(അക്കം_pat, pat, അക്കം_file, file);
    തിരികെ FAIL;
}

#endif // VMS

    സ്ഥിതം പൂർണ്ണസംഖ്യ
save_patterns(
    പൂർണ്ണസംഖ്യ		അക്കം_pat,
    char_u	**pat,
    പൂർണ്ണസംഖ്യ		*അക്കം_file,
    char_u	***file)
{
    പൂർണ്ണസംഖ്യ		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, അക്കം_pat);
    എങ്കിൽ (*file == ശൂന്യ)
	തിരികെ FAIL;
    വേണ്ടി (i = 0; i < അക്കം_pat; i++)
    {
	s = vim_strsave(pat[i]);
	എങ്കിൽ (s != ശൂന്യ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *അക്കം_file = അക്കം_pat;
    തിരികെ OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    പൂർണ്ണസംഖ്യ
mch_has_exp_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ
	    എങ്കിൽ (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#അല്ലെങ്കിൽ
				    "*?[{'"
#endif
						, *p) != ശൂന്യ)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    പൂർണ്ണസംഖ്യ
mch_has_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ
	    എങ്കിൽ (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#അല്ലെങ്കിൽ
				    "*?[{`'$"
#endif
						, *p) != ശൂന്യ
		|| (*p == '~' && p[1] != NUL))
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
have_wildcard(പൂർണ്ണസംഖ്യ അക്കം, char_u **file)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; i < അക്കം; i++)
	എങ്കിൽ (mch_has_wildcard(file[i]))
	    തിരികെ 1;
    തിരികെ 0;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
have_dollars(പൂർണ്ണസംഖ്യ അക്കം, char_u **file)
{
    പൂർണ്ണസംഖ്യ	    i;

    വേണ്ടി (i = 0; i < അക്കം; i++)
	എങ്കിൽ (vim_strchr(file[i], '$') != ശൂന്യ)
	    തിരികെ TRUE;
    തിരികെ FALSE;
}

#എങ്കിൽ !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    പൂർണ്ണസംഖ്യ
mch_rename(സ്ഥിരം അക്ഷരം *src, സ്ഥിരം അക്ഷരം *dest)
{
    ഘടന stat	    st;

    എങ്കിൽ (stat(dest, &st) >= 0)	    // fail if destination exists
	തിരികെ -1;
    എങ്കിൽ (link(src, dest) != 0)	    // link file to new name
	തിരികെ -1;
    എങ്കിൽ (mch_remove(src) == 0)	    // delete link to old name
	തിരികെ 0;
    തിരികെ -1;
}
#endif // !HAVE_RENAME

#എങ്കിൽ defined(FEAT_MOUSE_GPM)
# എങ്കിൽ defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
load_libgpm(ശൂന്യം)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    എങ്കിൽ (libgpm_hinst == ശൂന്യ)
    {
	എങ്കിൽ (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	തിരികെ FAIL;
    }

    എങ്കിൽ (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ശൂന്യ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ശൂന്യ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ശൂന്യ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ശൂന്യ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ശൂന്യ
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ശൂന്യ;
	dll_gpm_flag = ശൂന്യ;
	dll_gpm_fd   = ശൂന്യ;
	തിരികെ FAIL;
    }
    തിരികെ OK;
}

    പൂർണ്ണസംഖ്യ
gpm_available(ശൂന്യം)
{
    തിരികെ libgpm_hinst != ശൂന്യ || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
gpm_open(ശൂന്യം)
{
    സ്ഥിതം Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    എങ്കിൽ (!gpm_available())
	തിരികെ 0;
# endif

    എങ്കിൽ (gpm_flag)
	തിരികെ 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    എങ്കിൽ (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	തിരികെ 1; // succeed
    }
    എങ്കിൽ (gpm_fd == -2)
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
    തിരികെ 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    പൂർണ്ണസംഖ്യ
gpm_enabled(ശൂന്യം)
{
    തിരികെ gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    സ്ഥിതം ശൂന്യം
gpm_close(ശൂന്യം)
{
    എങ്കിൽ (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
mch_gpm_process(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ			button;
    സ്ഥിതം Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    പൂർണ്ണസംഖ്യ			row,col;
    unsigned അക്ഷരം	buttons_mask;
    unsigned അക്ഷരം	gpm_modifiers;
    സ്ഥിതം unsigned അക്ഷരം old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    തിരഞ്ഞെടുപ്പ് (GPM_BARE_EVENTS(gpm_event.type))
    {
	അവസ്ഥ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    നിർത്തുക;
	അവസ്ഥ GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    തിരഞ്ഞെടുപ്പ് (buttons_mask)
	    {
		അവസ്ഥ GPM_B_ഇടത്:
		    button = MOUSE_ഇടത്;
		    നിർത്തുക;
		അവസ്ഥ GPM_B_നടുവ്:
		    button = MOUSE_നടുവ്;
		    നിർത്തുക;
		അവസ്ഥ GPM_B_വലത്:
		    button = MOUSE_വലത്;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    തിരികെ 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_അക്കം_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    നിർത്തുക;
	അവസ്ഥ GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    നിർത്തുക;
	സ്ഥിരസ്ഥിതി:
	    തിരികെ 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    എങ്കിൽ (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    എങ്കിൽ (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    എങ്കിൽ (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_നിവേശം_buf(string, 6);
    തിരികെ 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
sysmouse_open(ശൂന്യം)
{
    ഘടന mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.സൂചന = SIGUSR2;
    എങ്കിൽ (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	തിരികെ FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    തിരികെ OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    സ്ഥിതം ശൂന്യം
sysmouse_close(ശൂന്യം)
{
    ഘടന mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.സൂചന = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    സ്ഥിതം ശൂന്യം
sig_sysmouse SIGDEFARG(sigarg)
{
    ഘടന mouse_info	mouse;
    ഘടന video_info	video;
    char_u		string[6];
    പൂർണ്ണസംഖ്യ			row, col;
    പൂർണ്ണസംഖ്യ			button;
    പൂർണ്ണസംഖ്യ			buttons;
    സ്ഥിതം പൂർണ്ണസംഖ്യ		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    എങ്കിൽ (hold_gui_events)
	തിരികെ;
# endif

    mouse.operation = MOUSE_GETINFO;
    എങ്കിൽ (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	എങ്കിൽ (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	അല്ലെങ്കിൽ
	{
	    തിരഞ്ഞെടുപ്പ് (buttons)
	    {
		അവസ്ഥ 0:
		    button = MOUSE_RELEASE;
		    നിർത്തുക;
		അവസ്ഥ 1:
		    button = MOUSE_ഇടത്;
		    നിർത്തുക;
		അവസ്ഥ 2:
		    button = MOUSE_നടുവ്;
		    നിർത്തുക;
		അവസ്ഥ 4:
		    button = MOUSE_വലത്;
		    നിർത്തുക;
		സ്ഥിരസ്ഥിതി:
		    തിരികെ;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_നിവേശം_buf(string, 6);
    }
    തിരികെ;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    പൂർണ്ണസംഖ്യ
mch_get_random(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    സ്ഥിതം പൂർണ്ണസംഖ്യ dev_urandom_state = NOTDONE;

    എങ്കിൽ (dev_urandom_state == FAIL)
	തിരികെ FAIL;

    പൂർണ്ണസംഖ്യ fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    എങ്കിൽ (fd == -1)
	dev_urandom_state = FAIL;
    അല്ലെങ്കിൽ എങ്കിൽ (പഠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    അല്ലെങ്കിൽ
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    തിരികെ dev_urandom_state;
}

#എങ്കിൽ defined(FEAT_LIBCALL)
തരനിർവചനം char_u * (*STRPROCSTR)(char_u *);
തരനിർവചനം char_u * (*INTPROCSTR)(പൂർണ്ണസംഖ്യ);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*STRPROCINT)(char_u *);
തരനിർവചനം പൂർണ്ണസംഖ്യ (*INTPROCINT)(പൂർണ്ണസംഖ്യ);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    പൂർണ്ണസംഖ്യ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    പൂർണ്ണസംഖ്യ		argint,
    char_u	**string_result, // NULL when using number_result
    പൂർണ്ണസംഖ്യ		*സംഖ്യ_result)
{
# എങ്കിൽ defined(USE_DLOPEN)
    ശൂന്യം	*hinstLib;
    അക്ഷരം	*dlerr = ശൂന്യ;
# അല്ലെങ്കിൽ
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		retval_int = 0;
    പൂർണ്ണസംഖ്യ		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# എങ്കിൽ defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (ശൂന്യം)dlerror();

    hinstLib = dlopen((അക്ഷരം *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    എങ്കിൽ (hinstLib == ശൂന്യ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	എങ്കിൽ (dlerr != ശൂന്യ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# അല്ലെങ്കിൽ
    hinstLib = shl_load((സ്ഥിരം അക്ഷരം*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    എങ്കിൽ (hinstLib != ശൂന്യ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	എങ്കിൽ (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  എങ്കിൽ defined(USE_DLOPEN)
	    dlerr = ശൂന്യ;
#  endif
	    mch_didjmp();
	}
	അല്ലെങ്കിൽ
# endif
	{
	    retval_str = ശൂന്യ;
	    retval_int = 0;

	    എങ്കിൽ (argstring != ശൂന്യ)
	    {
# എങ്കിൽ defined(USE_DLOPEN)
		*(ശൂന്യം **)(&ProcAdd) = dlsym(hinstLib, (സ്ഥിരം അക്ഷരം *)funcname);
		dlerr = dlerror();
# അല്ലെങ്കിൽ
		എങ്കിൽ (shl_findsym(&hinstLib, (സ്ഥിരം അക്ഷരം *)funcname,
					TYPE_PROCEDURE, (ശൂന്യം *)&ProcAdd) < 0)
		    ProcAdd = ശൂന്യ;
# endif
		എങ്കിൽ ((success = (ProcAdd != ശൂന്യ
# എങ്കിൽ defined(USE_DLOPEN)
			    && dlerr == ശൂന്യ
# endif
			    )))
		{
		    എങ്കിൽ (string_result == ശൂന്യ)
			retval_int = ((STRPROCINT)(ശൂന്യം *)ProcAdd)(argstring);
		    അല്ലെങ്കിൽ
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    അല്ലെങ്കിൽ
	    {
# എങ്കിൽ defined(USE_DLOPEN)
		*(ശൂന്യം **)(&ProcAddI) = dlsym(hinstLib, (സ്ഥിരം അക്ഷരം *)funcname);
		dlerr = dlerror();
# അല്ലെങ്കിൽ
		എങ്കിൽ (shl_findsym(&hinstLib, (സ്ഥിരം അക്ഷരം *)funcname,
				       TYPE_PROCEDURE, (ശൂന്യം *)&ProcAddI) < 0)
		    ProcAddI = ശൂന്യ;
# endif
		എങ്കിൽ ((success = (ProcAddI != ശൂന്യ
# എങ്കിൽ defined(USE_DLOPEN)
			    && dlerr == ശൂന്യ
# endif
			    )))
		{
		    എങ്കിൽ (string_result == ശൂന്യ)
			retval_int = ((INTPROCINT)(ശൂന്യം *)ProcAddI)(argint);
		    അല്ലെങ്കിൽ
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    എങ്കിൽ (string_result == ശൂന്യ)
		*സംഖ്യ_result = retval_int;
	    അല്ലെങ്കിൽ എങ്കിൽ (retval_str != ശൂന്യ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	എങ്കിൽ (lc_signal != 0)
	{
	    പൂർണ്ണസംഖ്യ i;

	    // try to find the name of this signal
	    വേണ്ടി (i = 0; signal_info[i].sig != -1; i++)
		എങ്കിൽ (lc_signal == signal_info[i].sig)
		    നിർത്തുക;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# എങ്കിൽ defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	എങ്കിൽ (dlerr != ശൂന്യ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(ശൂന്യം)dlclose(hinstLib);
# അല്ലെങ്കിൽ
	(ശൂന്യം)shl_unload(hinstLib);
# endif
    }

    എങ്കിൽ (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	തിരികെ FAIL;
    }

    തിരികെ OK;
}
#endif

#എങ്കിൽ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
സ്ഥിതം പൂർണ്ണസംഖ്യ	xterm_trace = -1;	// default: disabled
സ്ഥിതം പൂർണ്ണസംഖ്യ	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    ശൂന്യം
setup_term_clip(ശൂന്യം)
{
    പൂർണ്ണസംഖ്യ		z = 0;
    അക്ഷരം	*strp = "";
    Widget	AppShell;

    എങ്കിൽ (!x_connect_to_server())
	തിരികെ;

    open_app_context();
    എങ്കിൽ (app_context != ശൂന്യ && xterm_Shell == (Widget)0)
    {
	പൂർണ്ണസംഖ്യ (*oldhandler)(Display*, XErrorEvent*);
# എങ്കിൽ defined(USING_SETJMP)
	പൂർണ്ണസംഖ്യ (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	എങ്കിൽ (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# എങ്കിൽ defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	എങ്കിൽ (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ശൂന്യ;
	}
	അല്ലെങ്കിൽ
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ശൂന്യ, 0, &z, &strp);
	    എങ്കിൽ (xterm_dpy != ശൂന്യ)
		xterm_dpy_retry_എണ്ണം = 0;
# എങ്കിൽ defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# എങ്കിൽ defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(ശൂന്യം)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(ശൂന്യം)XSetErrorHandler(oldhandler);

	എങ്കിൽ (xterm_dpy == ശൂന്യ)
	{
	    എങ്കിൽ (p_verbose > 0)
		verb_സന്ദേശം(_("Opening the X display failed"));
	    തിരികെ;
	}

	// Catch terminating error of the X server connection.
	(ശൂന്യം)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	എങ്കിൽ (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ശൂന്യ);
	എങ്കിൽ (AppShell == (Widget)0)
	    തിരികെ;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		മുകൾ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ശൂന്യ);
	എങ്കിൽ (xterm_Shell == (Widget)0)
	    തിരികെ;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	എങ്കിൽ (x11_display == ശൂന്യ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    എങ്കിൽ (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	എങ്കിൽ (x11_window == 0 && (strp = getenv("WINDOWID")) != ശൂന്യ)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	എങ്കിൽ (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	എങ്കിൽ (x11_window != 0)
	    xterm_trace = 0;
    }
}

    ശൂന്യം
start_xterm_trace(പൂർണ്ണസംഖ്യ button)
{
    എങ്കിൽ (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	തിരികെ;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    ശൂന്യം
stop_xterm_trace(ശൂന്യം)
{
    എങ്കിൽ (xterm_trace < 0)
	തിരികെ;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
do_xterm_trace(ശൂന്യം)
{
    Window		വേര്, child;
    പൂർണ്ണസംഖ്യ			വേര്_x, വേര്_y;
    പൂർണ്ണസംഖ്യ			win_x, win_y;
    പൂർണ്ണസംഖ്യ			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    സ്ഥിതം char_u	*mouse_code = ശൂന്യ;
    സ്ഥിതം size_t	mouse_codelen = 0;
    സ്ഥിതം char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    സ്ഥിതം പൂർണ്ണസംഖ്യ		prev_row = 0, prev_col = 0;
    സ്ഥിതം XSizeHints	xterm_hints;

    എങ്കിൽ (xterm_trace <= 0)
	തിരികെ FALSE;

    എങ്കിൽ (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	എങ്കിൽ (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    തിരികെ FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	എങ്കിൽ (mouse_code != ശൂന്യ)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &വേര്, &child, &വേര്_x, &വേര്_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	എങ്കിൽ (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	എങ്കിൽ (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	തിരികെ TRUE;
    }

    എങ്കിൽ (mouse_code == ശൂന്യ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	തിരികെ FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &വേര്, &child, &വേര്_x, &വേര്_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    എങ്കിൽ (row == prev_row && col == prev_col)
	തിരികെ TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_നിവേശം_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    തിരികെ TRUE;
}

# എങ്കിൽ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    ശൂന്യം
clear_xterm_clip(ശൂന്യം)
{
    എങ്കിൽ (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    എങ്കിൽ (xterm_dpy != ശൂന്യ)
    {
#  എങ്കിൽ 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	എങ്കിൽ (x11_display == xterm_dpy)
	    x11_display = ശൂന്യ;
	xterm_dpy = ശൂന്യ;
    }
#  എങ്കിൽ 0
    എങ്കിൽ (app_context != (XtAppContext)ശൂന്യ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ശൂന്യ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    സ്ഥിതം ശൂന്യം
clip_update(ശൂന്യം)
{
# ifdef FEAT_GUI
    എങ്കിൽ (gui.in_use)
	gui_mch_update();
    അല്ലെങ്കിൽ
# endif
    എങ്കിൽ (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ശൂന്യം
xterm_update(ശൂന്യം)
{
    XEvent event;

    വേണ്ടി (;;)
    {
	Xt_നിവേശം_Mask mask = XtAppPending(app_context);

	എങ്കിൽ (mask == 0 || vim_is_നിവേശം_buf_full())
	    നിർത്തുക;

	എങ്കിൽ (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		എങ്കിൽ (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_മൂല്യം)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	അല്ലെങ്കിൽ
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    പൂർണ്ണസംഖ്യ
clip_xterm_own_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (xterm_Shell != (Widget)0)
	തിരികെ clip_x11_own_selection(xterm_Shell, cbd);
    തിരികെ FAIL;
}

    ശൂന്യം
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    ശൂന്യം
clip_xterm_request_selection(Clipboard_T *cbd)
{
    എങ്കിൽ (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    ശൂന്യം
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#എങ്കിൽ defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# എങ്കിൽ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    സ്ഥിതം ശൂന്യം
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    പൂർണ്ണസംഖ്യ		save_cmod_flags;
    പൂർണ്ണസംഖ്യ		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    എങ്കിൽ (check_changed_any(FALSE, FALSE))
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
    എങ്കിൽ (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    സ്ഥിതം ശൂന്യം
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    പൂർണ്ണസംഖ്യ		save_type UNUSED,
    Bool	shutdown,
    പൂർണ്ണസംഖ്യ		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    എങ്കിൽ (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    എങ്കിൽ (p_verbose > 0)
	verb_സന്ദേശം(_("XSMP handling save-yourself request"));

# എങ്കിൽ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    എങ്കിൽ (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    അല്ലെങ്കിൽ
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
    സ്ഥിതം ശൂന്യം
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    സ്ഥിതം ശൂന്യം
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
    സ്ഥിതം ശൂന്യം
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    എങ്കിൽ (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    സ്ഥിതം ശൂന്യം
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    എങ്കിൽ (opening)
    {
	xsmp_icefd = Ice_Connection_സംഖ്യ(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ശൂന്യ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    പൂർണ്ണസംഖ്യ
xsmp_handle_requests(ശൂന്യം)
{
    Bool rep;

    എങ്കിൽ (IceProcessMessages(xsmp.iceconn, ശൂന്യ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	എങ്കിൽ (p_verbose > 0)
	    verb_സന്ദേശം(_("XSMP lost ICE connection"));
	xsmp_close();
	തിരികെ FAIL;
    }
    അല്ലെങ്കിൽ
	തിരികെ OK;
}

സ്ഥിതം പൂർണ്ണസംഖ്യ dummy;

// Set up X Session Management Protocol
    ശൂന്യം
xsmp_init(ശൂന്യം)
{
    അക്ഷരം		errorstring[80];
    SmcCallbacks	smcallbacks;
# എങ്കിൽ 0
    Sm_Prop_മൂല്യം		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    എങ്കിൽ (p_verbose > 0)
	verb_സന്ദേശം(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = ശൂന്യ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = ശൂന്യ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = ശൂന്യ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = ശൂന്യ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    എങ്കിൽ (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	എങ്കിൽ (p_verbose > 0)
	    verb_സന്ദേശം(_("XSMP ICE connection watch failed"));
	തിരികെ;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ശൂന്യ,
	    ശൂന്യ,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    ശൂന്യ,
	    &xsmp.clientid,
	    വലുപ്പം(errorstring) - 1,
	    errorstring);
    എങ്കിൽ (xsmp.smcconn == ശൂന്യ)
    {
	എങ്കിൽ (p_verbose > 0)
	{
	    അക്ഷരം errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[വലുപ്പം(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, വലുപ്പം(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_സന്ദേശം(errorreport);
	}
	തിരികെ;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# എങ്കിൽ 0
    // ID ourselves
    smname.മൂല്യം = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.അക്കം_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    ശൂന്യം
xsmp_close(ശൂന്യം)
{
    എങ്കിൽ (xsmp_icefd == -1)
	തിരികെ;

    SmcCloseConnection(xsmp.smcconn, 0, ശൂന്യ);
    എങ്കിൽ (xsmp.clientid != ശൂന്യ)
	സ്വതന്ത്രം(xsmp.clientid);
    xsmp.clientid = ശൂന്യ;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#എങ്കിൽ defined(FEAT_RELTIME)
# എങ്കിൽ defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
സ്ഥിതം അസ്ഥിര sig_atomic_t timeout_flag = FALSE;
സ്ഥിതം timer_t		     timer_id;
സ്ഥിതം പൂർണ്ണസംഖ്യ		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    സ്ഥിതം ശൂന്യം
set_flag(സംഘം sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ശൂന്യം
stop_timeout(ശൂന്യം)
{
    സ്ഥിതം ഘടന itimerspec disarm = {{0, 0}, {0, 0}};

    എങ്കിൽ (timer_created)
    {
	പൂർണ്ണസംഖ്യ ret = timer_settime(timer_id, 0, &disarm, ശൂന്യ);

	എങ്കിൽ (ret < 0)
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
    അസ്ഥിര sig_atomic_t *
start_timeout(long msec)
{
    ഘടന itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    പൂർണ്ണസംഖ്യ ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    എങ്കിൽ (!timer_created)
    {
	ഘടന sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	എങ്കിൽ (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    തിരികെ &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ശൂന്യ, "setting timeout timer to %d sec %ld nsec",
	       (പൂർണ്ണസംഖ്യ)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ശൂന്യ);
    എങ്കിൽ (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    തിരികെ &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    ശൂന്യം
delete_timer(ശൂന്യം)
{
    എങ്കിൽ (!timer_created)
	തിരികെ;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# അല്ലെങ്കിൽ // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
സ്ഥിതം ഘടന sigaction		prev_sigaction;
സ്ഥിതം അസ്ഥിര sig_atomic_t	timeout_flag	     = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ			timer_active	     = FALSE;
സ്ഥിതം പൂർണ്ണസംഖ്യ			timer_handler_active = FALSE;
സ്ഥിതം അസ്ഥിര sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    സ്ഥിതം ശൂന്യം
set_flag SIGDEFARG(sigarg)
{
    എങ്കിൽ (alarm_pending)
	alarm_pending = FALSE;
    അല്ലെങ്കിൽ
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ശൂന്യം
stop_timeout(ശൂന്യം)
{
    സ്ഥിതം ഘടന itimerval disarm = {{0, 0}, {0, 0}};
    പൂർണ്ണസംഖ്യ			    ret;

    എങ്കിൽ (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ശൂന്യ);
	എങ്കിൽ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    എങ്കിൽ (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ശൂന്യ);
	എങ്കിൽ (ret < 0)
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
    അസ്ഥിര sig_atomic_t *
start_timeout(long msec)
{
    ഘടന itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ഘടന sigaction	handle_alarm;
    പൂർണ്ണസംഖ്യ			ret;
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
    എങ്കിൽ (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ശൂന്യ);
    }
    എങ്കിൽ (unlikely(ret != 0 || alarm_pending < 0))
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
    എങ്കിൽ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	തിരികെ &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ശൂന്യ);
    എങ്കിൽ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	തിരികെ &timeout_flag;
    }

    timer_active = TRUE;
    തിരികെ &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
