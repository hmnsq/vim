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

#অন্তর্ভাব "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#অন্তর্ভাব "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
স্থির পূর্ণাঙ্ক selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != রিক্ত ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != রিক্ত ? *dll_gpm_fd   : -1)

স্থির পূর্ণাঙ্ক (*dll_Gpm_Open)     (Gpm_Connect *, পূর্ণাঙ্ক);
স্থির পূর্ণাঙ্ক (*dll_Gpm_Close)    (শূন্য);
স্থির পূর্ণাঙ্ক (*dll_Gpm_GetEvent) (Gpm_Event *);
স্থির পূর্ণাঙ্ক *dll_gpm_flag;
স্থির পূর্ণাঙ্ক *dll_gpm_fd;

স্থির শূন্য *libgpm_hinst;
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

স্থির শূন্য gpm_close(শূন্য);
স্থির পূর্ণাঙ্ক gpm_open(শূন্য);
স্থির পূর্ণাঙ্ক mch_gpm_process(শূন্য);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

স্থির পূর্ণাঙ্ক sysmouse_open(শূন্য);
স্থির শূন্য sysmouse_close(শূন্য);
স্থির শূন্য sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#যদি defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#যদি defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
স্থির Widget	xterm_Shell = (Widget)0;
স্থির শূন্য clip_update(শূন্য);
# endif

Window	    x11_window = 0;
Display	    *x11_display = রিক্ত;
#endif

স্থির পূর্ণাঙ্ক ignore_sigtstp = FALSE;

স্থির পূর্ণাঙ্ক get_x11_title(পূর্ণাঙ্ক);

স্থির char_u	*oldtitle = রিক্ত;
স্থির চঞ্চল sig_atomic_t oldtitle_outdated = FALSE;
স্থির পূর্ণাঙ্ক	unix_did_set_title = FALSE;
স্থির char_u	*oldicon = রিক্ত;
স্থির পূর্ণাঙ্ক	did_set_icon = FALSE;

স্থির শূন্য may_core_dump(শূন্য);

প্রকারনির্দেশ পূর্ণাঙ্ক waitstatus;
স্থির পূর্ণাঙ্ক  WaitForChar(long msec, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ);
স্থির পূর্ণাঙ্ক  WaitForCharOrMouse(long msec, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ);
#ifdef VMS
পূর্ণাঙ্ক  RealWaitForChar(পূর্ণাঙ্ক, long, পূর্ণাঙ্ক *, পূর্ণাঙ্ক *interrupted);
#অন্যথা
স্থির পূর্ণাঙ্ক  RealWaitForChar(পূর্ণাঙ্ক, long, পূর্ণাঙ্ক *, পূর্ণাঙ্ক *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
স্থির পূর্ণাঙ্ক do_xterm_trace(শূন্য);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

স্থির শূন্য handle_resize(শূন্য);

#যদি defined(SIGWINCH)
স্থির শূন্য sig_winch SIGPROTOARG;
#endif
#যদি defined(SIGTSTP)
স্থির শূন্য sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
স্থির চঞ্চল sig_atomic_t in_mch_suspend = FALSE;
#endif
#যদি defined(SIGINT)
স্থির শূন্য catch_sigint SIGPROTOARG;
#endif
#যদি defined(SIGUSR1)
স্থির শূন্য catch_sigusr1 SIGPROTOARG;
#endif
#যদি defined(SIGPWR)
স্থির শূন্য catch_sigpwr SIGPROTOARG;
#endif
#যদি defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
স্থির শূন্য sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
স্থির চঞ্চল sig_atomic_t sig_alarm_called;
#endif
স্থির শূন্য deathtrap SIGPROTOARG;

স্থির শূন্য catch_int_signal(শূন্য);
স্থির শূন্য set_signals(শূন্য);
স্থির শূন্য catch_signals(শূন্য (*func_deadly)(পূর্ণাঙ্ক), শূন্য (*func_other)(পূর্ণাঙ্ক));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#অন্যথা
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	কুরু { /**/ } যাবত্ (0)
# define UNBLOCK_SIGNALS(set)	কুরু { /**/ } যাবত্ (0)
#endif
স্থির পূর্ণাঙ্ক  have_wildcard(পূর্ণাঙ্ক, char_u **);
স্থির পূর্ণাঙ্ক  have_dollars(পূর্ণাঙ্ক, char_u **);

স্থির পূর্ণাঙ্ক save_patterns(পূর্ণাঙ্ক অঙ্ক_pat, char_u **pat, পূর্ণাঙ্ক *অঙ্ক_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
স্থির চঞ্চল sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
স্থির চঞ্চল sig_atomic_t got_tstp = FALSE;
স্থির char_u	*extra_shell_arg = রিক্ত;
স্থির পূর্ণাঙ্ক	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
স্থির চঞ্চল sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
স্থির চঞ্চল sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#যদি defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
স্থির পূর্ণাঙ্ক dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
স্থির tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
প্রকারনির্দেশ সংরচনা
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    বর্ণ *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

স্থির xsmp_config_T xsmp;
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

স্থির সংরচনা signalinfo
{
    পূর্ণাঙ্ক	    sig;	// Signal number, eg. SIGSEGV etc
    বর্ণ    *name;	// Signal name (not char_u!).
    বর্ণ    deadly;	// Catch as a deadly signal?
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
#যদি defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#যদি defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#যদি defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#যদি defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(পূর্ণাঙ্ক sig, sighandler_T func)
{
#যদি defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    সংরচনা sigaction	sa, old;
    sigset_t		curset;
    পূর্ণাঙ্ক			blocked;

    যদি (sigprocmask(SIG_BLOCK, রিক্ত, &curset) == -1)
	প্রত্যাগম SIG_ERR;

    blocked = sigismember(&curset, sig);

    যদি (func == SIG_HOLD)
    {
	যদি (blocked)
	    প্রত্যাগম SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	যদি (sigaction(sig, রিক্ত, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, রিক্ত) == -1)
	    প্রত্যাগম SIG_ERR;
	প্রত্যাগম old.sa_handler;
    }

    যদি (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	যদি (sigprocmask(SIG_UNBLOCK, &curset, রিক্ত) == -1)
	    প্রত্যাগম SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# অন্যথা
    sa.sa_flags = 0;
# endif
    যদি (sigaction(sig, &sa, &old) == -1)
	প্রত্যাগম SIG_ERR;
    প্রত্যাগম blocked ? SIG_HOLD: old.sa_handler;
#নোচেদ্যদি defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    প্রত্যাগম sigset(sig, func);
#অন্যথা
    // Oldest and most compatible solution.
    প্রত্যাগম সঙ্কেত(sig, func);
#endif
}

    পূর্ণাঙ্ক
mch_chdir(বর্ণ *path)
{
    যদি (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    প্রত্যাগম chdir(vms_fixfilename(path));
#অন্যথা
    প্রত্যাগম chdir(path);
#endif
}

#যদি defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    শূন্য
mch_write(char_u *s, পূর্ণাঙ্ক len)
{
    vim_ignored = (পূর্ণাঙ্ক)লিখ(1, (বর্ণ *)s, len);
    যদি (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, রিক্ত, রিক্ত);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    স্থির পূর্ণাঙ্ক
resize_func(পূর্ণাঙ্ক check_only)
{
    যদি (check_only)
	প্রত্যাগম do_resize;
    যাবত্ (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(রিক্ত, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    প্রত্যাগম FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    পূর্ণাঙ্ক
mch_inchar(
    char_u	*buf,
    পূর্ণাঙ্ক		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    পূর্ণাঙ্ক		tb_change_cnt)
{
    প্রত্যাগম inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    স্থির শূন্য
handle_resize(শূন্য)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    পূর্ণাঙ্ক
mch_char_avail(শূন্য)
{
    প্রত্যাগম WaitForChar(0L, রিক্ত, FALSE);
}

#যদি defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    পূর্ণাঙ্ক
mch_check_messages(শূন্য)
{
    প্রত্যাগম WaitForChar(0L, রিক্ত, TRUE);
}
#endif

#যদি defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# যদি defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# যদি defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(পূর্ণাঙ্ক special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	সংরচনা vm_statistics64	vm_stat;
	natural_t		গুনতি = HOST_VM_INFO64_গুনতি;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &গুনতি);
#  অন্যথা
	সংরচনা vm_statistics	vm_stat;
	natural_t		গুনতি = HOST_VM_INFO_গুনতি;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &গুনতি);
#  endif
	যদি (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_গুনতি + vm_stat.active_গুনতি
					    + vm_stat.inactive_গুনতি
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_গুনতি
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    যদি (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	পূর্ণাঙ্ক		mib[2];
	size_t		len = পরিমাণ(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  অন্যথা
	// sysctl() may return 32 bit or 64 bit, accept both
	সঙ্ঘ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  অন্যথা
	mib[1] = HW_USERMEM;
#  endif
	যদি (sysctl(mib, 2, &physmem, &len, রিক্ত, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  অন্যথা
	    যদি (len == পরিমাণ(physmem.u64))
		mem = (long_u)physmem.u64;
	    অন্যথা
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# যদি defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    যদি (mem == 0)
    {
	সংরচনা sysinfo sinfo;

	// Linux way of getting amount of RAM available
	যদি (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    যাবত্ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  অন্যথা
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    যদি (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	যদি (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    যাবত্ (shiftright > 0 && (pagesize & 1) == 0)
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
# যদি defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	সংরচনা rlimit	rlp;

	যদি (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (পরিমাণ(long_u) * 8 - 1))
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

    যদি (mem > 0)
	প্রত্যাগম mem >> shiftright;
    প্রত্যাগম (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    শূন্য
mch_delay(long msec, পূর্ণাঙ্ক flags)
{
    tmode_T	old_tmode;
    পূর্ণাঙ্ক		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    যদি (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	যদি (call_settmode)
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
	কুরু
	{
	    // if total is large enough, wait by portions in p_mzq
	    যদি (total > p_mzq)
		msec = p_mzq;
	    অন্যথা
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    সংরচনা timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (শূন্য)nanosleep(&ts, রিক্ত);
	}
#অন্যথা
# ifdef HAVE_USLEEP
	যাবত্ (msec >= 1000)
	{
	    usleep((unsigned পূর্ণাঙ্ক)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned পূর্ণাঙ্ক)(msec * 1000));
# অন্যথা
#  ifndef HAVE_SELECT
	poll(রিক্ত, 0, (পূর্ণাঙ্ক)msec);
#  অন্যথা
	{
	    সংরচনা timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, রিক্ত, রিক্ত, রিক্ত, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	যাবত্ (total > 0);
#endif

	যদি (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    অন্যথা
	WaitForChar(msec, রিক্ত, FALSE);
}

#যদি defined(HAVE_স্তূপ_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_স্তূপ_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
স্থির পূর্ণাঙ্ক স্তূপ_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    স্থির শূন্য
check_স্তূপ_growth(বর্ণ *p)
{
    পূর্ণাঙ্ক		i;

    স্তূপ_grows_downwards = (p > (বর্ণ *)&i);
}
#endif

#যদি defined(HAVE_স্তূপ_LIMIT)
স্থির বর্ণ *স্তূপ_limit = রিক্ত;

# যদি defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    স্থির শূন্য
get_স্তূপ_limit(শূন্য)
{
    সংরচনা rlimit	rlp;
    পূর্ণাঙ্ক			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    যদি (getrlimit(RLIMIT_স্তূপ, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (পরিমাণ(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# যদি defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    যদি (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		যদি (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	যদি (স্তূপ_grows_downwards)
	{
	    স্তূপ_limit = (বর্ণ *)((long)&i - (lim / 16L * 15L));
	    যদি (স্তূপ_limit >= (বর্ণ *)&i)
		// overflow, set to 1/16 of current stack position
		স্তূপ_limit = (বর্ণ *)((long)&i / 16L);
	}
	অন্যথা
	{
	    স্তূপ_limit = (বর্ণ *)((long)&i + (lim / 16L * 15L));
	    যদি (স্তূপ_limit <= (বর্ণ *)&i)
		স্তূপ_limit = রিক্ত;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    পূর্ণাঙ্ক
mch_stackcheck(বর্ণ *p)
{
    যদি (স্তূপ_limit == রিক্ত)
	প্রত্যাগম OK;

    যদি (স্তূপ_grows_downwards)
    {
	যদি (p < স্তূপ_limit)
	    প্রত্যাগম FAIL;
    }
    অন্যথা যদি (p > স্তূপ_limit)
	প্রত্যাগম FAIL;
    প্রত্যাগম OK;
}
#endif

#যদি defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
স্থির stack_t sigstk;			// for sigaltstack()
# অন্যথা
স্থির সংরচনা sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
স্থির long পূর্ণাঙ্ক get_signal_স্তূপ_size(শূন্য)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long পূর্ণাঙ্ক size = -1;

    // return size only if sysconf doesn't return an error
    যদি ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	প্রত্যাগম size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    প্রত্যাগম SIGSTKSZ;
# endif

    // otherwise guess the size
    প্রত্যাগম 8000;
}

স্থির বর্ণ *signal_স্তূপ;

    স্থির শূন্য
init_signal_স্তূপ(শূন্য)
{
    যদি (signal_স্তূপ == রিক্ত)
	প্রত্যাগম;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_স্তূপ;
#  অন্যথা
    sigstk.ss_sp = signal_স্তূপ;
#  endif
    sigstk.ss_size = get_signal_স্তূপ_size();
    sigstk.ss_flags = 0;
    (শূন্য)sigaltstack(&sigstk, রিক্ত);
# অন্যথা
    sigstk.ss_sp = signal_স্তূপ;
    যদি (স্তূপ_grows_downwards)
	sigstk.ss_sp += get_signal_স্তূপ_size() - 1;
    sigstk.ss_onstack = 0;
    (শূন্য)sigstack(&sigstk, রিক্ত);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#যদি defined(SIGWINCH)
    স্থির শূন্য
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#যদি defined(SIGTSTP)
    স্থির শূন্য
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    যদি (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	উত্থাপয(sigarg);
    }
    অন্যথা
	got_tstp = TRUE;

# যদি !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#যদি defined(SIGINT)
    স্থির শূন্য
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#যদি defined(SIGUSR1)
    স্থির শূন্য
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#যদি defined(SIGPWR)
    স্থির শূন্য
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
    স্থির শূন্য
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#যদি defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
স্থির JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
স্থির চঞ্চল sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
স্থির চঞ্চল sig_atomic_t lc_active = FALSE;

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
    স্থির শূন্য
mch_startjmp(শূন্য)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    স্থির শূন্য
mch_endjmp(শূন্য)
{
    lc_active = FALSE;
}

    স্থির শূন্য
mch_didjmp(শূন্য)
{
# যদি defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_স্তূপ();
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
    স্থির শূন্য
deathtrap SIGDEFARG(sigarg)
{
    স্থির পূর্ণাঙ্ক	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    পূর্ণাঙ্ক		i;
#endif

#যদি defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    যদি (lc_active)
    {
# যদি defined(SIGHASARG)
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
    যদি (in_mch_delay && sigarg == SIGQUIT)
	প্রত্যাগম;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    যদি (entered == 0
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
	প্রত্যাগম;
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

#ifdef HAVE_স্তূপ_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_স্তূপ_limit();
#endif

#যদি 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	যদি (fp)
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
    প্রতি (i = 0; signal_info[i].sig != -1; i++)
	যদি (sigarg == signal_info[i].sig)
	    ভঙ্গ;
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
    যদি (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	যদি (entered >= 4)
	    _exit(8);
	exit(7);
    }
    যদি (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((বর্ণ *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#অন্যথা
    sprintf((বর্ণ *)IObuff, "Vim: Caught deadly signal\r\n");
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
    স্থির শূন্য
after_sigcont(শূন্য)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#যদি defined(SIGCONT)
স্থির শূন্য sigcont_handler SIGPROTOARG;

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
স্থির চঞ্চল sig_atomic_t sigcont_received;
স্থির শূন্য sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    স্থির শূন্য
sigcont_handler SIGDEFARG(sigarg)
{
    যদি (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    অন্যথা
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

#যদি defined(FEAT_CLIPBOARD)
# যদি defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
স্থির শূন্য *clip_star_save = রিক্ত;
স্থির শূন্য *clip_plus_save = রিক্ত;
# endif

# যদি defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    স্থির শূন্য
loose_clipboard(শূন্য)
{
    যদি (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	যদি (clip_star.owned)
	    clip_lose_selection(&clip_star);
	যদি (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	যদি (x11_display != রিক্ত)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# যদি defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    স্থির শূন্য
save_clipboard(শূন্য)
{
    যদি (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    যদি (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    স্থির শূন্য
restore_clipboard(শূন্য)
{
    যদি (clip_star_save != রিক্ত)
    {
	যদি (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	অন্যথা
	    free_register(clip_star_save);
	clip_star_save = রিক্ত;
    }
    যদি (clip_plus_save != রিক্ত)
    {
	যদি (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	অন্যথা
	    free_register(clip_plus_save);
	clip_plus_save = রিক্ত;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    শূন্য
mch_suspend(শূন্য)
{
    যদি (ignore_sigtstp)
	প্রত্যাগম;

#যদি defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# যদি defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# যদি defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# যদি defined(SIGCONT)
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

	প্রতি (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#অন্যথা
    suspend_shell();
#endif
}

    শূন্য
mch_init(শূন্য)
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

    শূন্য
set_sigwinch_handler(শূন্য)
{
#যদি defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    স্থির শূন্য
set_signals(শূন্য)
{
#যদি defined(SIGWINCH)
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
#যদি defined(SIGCONT)
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

#যদি defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    যদি (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#যদি defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    স্থির শূন্য
catch_int_signal(শূন্য)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    শূন্য
reset_signals(শূন্য)
{
    catch_signals(SIG_DFL, SIG_DFL);
#যদি defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    স্থির শূন্য
catch_signals(
    শূন্য (*func_deadly)(পূর্ণাঙ্ক),
    শূন্য (*func_other)(পূর্ণাঙ্ক))
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; signal_info[i].sig != -1; i++)
    {
	যদি (signal_info[i].deadly)
	{
#যদি defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    সংরচনা sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# যদি defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# অন্যথা
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, রিক্ত);
#অন্যথা
# যদি defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    সংরচনা sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, রিক্ত);
# অন্যথা
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	অন্যথা যদি (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#অন্যথা
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    স্থির শূন্য
block_signals(sigset_t *set)
{
    sigset_t	newset;
    পূর্ণাঙ্ক		i;

    sigemptyset(&newset);

    প্রতি (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# যদি defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    স্থির শূন্য
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, রিক্ত);
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
    পূর্ণাঙ্ক
vim_handle_signal(পূর্ণাঙ্ক sig)
{
    স্থির পূর্ণাঙ্ক got_signal = 0;
    স্থির পূর্ণাঙ্ক blocked = TRUE;

    বিকল্প (sig)
    {
	পক্ষ SIGNAL_BLOCK:   blocked = TRUE;
			     ভঙ্গ;

	পক্ষ SIGNAL_UNBLOCK: blocked = FALSE;
			     যদি (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ভঙ্গ;

	স্বভাব:	     যদি (!blocked)
				 প্রত্যাগম TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     যদি (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     ভঙ্গ;
    }
    প্রত্যাগম FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    পূর্ণাঙ্ক
mch_check_win(পূর্ণাঙ্ক argc UNUSED, বর্ণ **argv UNUSED)
{
    যদি (isatty(1))
	প্রত্যাগম OK;
    প্রত্যাগম FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    পূর্ণাঙ্ক
mch_প্রবেশ_isatty(শূন্য)
{
    যদি (isatty(read_cmd_fd))
	প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

#ifdef FEAT_X11

# যদি defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    স্থির শূন্য
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#যদি defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

স্থির পূর্ণাঙ্ক	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    স্থির পূর্ণাঙ্ক
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (বর্ণ *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    যদি (!gui.in_use)
# endif
	বার্তা((বর্ণ *)IObuff);

    প্রত্যাগম 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    স্থির পূর্ণাঙ্ক
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    প্রত্যাগম 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    স্থির পূর্ণাঙ্ক
x_connect_to_server(শূন্য)
{
    // No point in connecting if we are exiting or dying.
    যদি (exiting || v_dying)
	প্রত্যাগম FALSE;

# যদি defined(FEAT_CLIENTSERVER)
    যদি (x_force_connect)
	প্রত্যাগম TRUE;
# endif
    যদি (x_no_connect)
	প্রত্যাগম FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    যদি (clip_exclude_prog != রিক্ত)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	যদি (regprog_in_use(clip_exclude_prog))
	    প্রত্যাগম FALSE;

	যদি (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    প্রত্যাগম FALSE;
    }
    প্রত্যাগম TRUE;
}

# যদি defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  যদি defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    স্থির পূর্ণাঙ্ক
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   যদি defined(VMS) || defined(__CYGWIN__)
    প্রত্যাগম 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
স্থির পূর্ণাঙ্ক xterm_dpy_retry_গুনতি = 0;

    স্থির পূর্ণাঙ্ক
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = রিক্ত;
    xterm_dpy_retry_গুনতি = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = রিক্ত;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  যদি defined(VMS) || defined(__CYGWIN__)
    প্রত্যাগম 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    স্থির শূন্য
may_restore_x11_clipboard(শূন্য)
{
    // No point in restoring the connecting if we are exiting or dying.
    যদি (!exiting && !v_dying && xterm_dpy_retry_গুনতি > 0)
    {
	--xterm_dpy_retry_গুনতি;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	যদি (app_context != (XtAppContext)রিক্ত)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)রিক্ত;
	    x11_display = রিক্ত; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    শূন্য
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    যদি (eap->arg != রিক্ত && (arglen = STRLEN(eap->arg)) > 0)
    {
	যদি (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (বর্ণ *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == রিক্ত
		    ? (বর্ণ *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_গুনতি = 5;  // Try reconnecting five times
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
    স্থির পূর্ণাঙ্ক
test_x11_window(Display *dpy)
{
    পূর্ণাঙ্ক			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    যদি (XGetWMName(dpy, x11_window, &text_prop))
	XFree((শূন্য *)text_prop.মান);
    XSync(dpy, False);
    (শূন্য)XSetErrorHandler(old_handler);

    যদি (p_verbose > 0 && got_x_error)
	verb_বার্তা(_("Testing the X display failed"));

    প্রত্যাগম (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

স্থির পূর্ণাঙ্ক get_x11_thing(পূর্ণাঙ্ক get_title, পূর্ণাঙ্ক test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    স্থির পূর্ণাঙ্ক
get_x11_windis(শূন্য)
{
    বর্ণ	    *winid;
    স্থির পূর্ণাঙ্ক	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    স্থির পূর্ণাঙ্ক	    x11_display_from = XD_NONE;
    স্থির পূর্ণাঙ্ক	    did_set_error_handler = FALSE;

    যদি (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(শূন্য)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# যদি defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    যদি (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	যদি (x11_display_from == XD_HERE && x11_display != রিক্ত)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	যদি (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    প্রত্যাগম OK;
	}
	x11_display = রিক্ত;
	প্রত্যাগম FAIL;
    }
    অন্যথা যদি (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = রিক্ত;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    যদি (!x_connect_to_server())
	প্রত্যাগম FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    যদি (x11_window == 0 && (winid = getenv("WINDOWID")) != রিক্ত)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    যদি (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    যদি (xterm_dpy != রিক্ত && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	যদি (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    যদি (x11_display_from == XD_HERE && x11_display != রিক্ত)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	যদি (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = রিক্ত;
	    x11_display_from = XD_NONE;
	    প্রত্যাগম FAIL;
	}
	প্রত্যাগম OK;
    }
# endif

    যদি (x11_window == 0 || x11_display == রিক্ত)
	result = -1;

    যদি (result != -1)	    // Have already been here and set this
	প্রত্যাগম result;	    // Don't do all these X calls again

    যদি (x11_window != 0 && x11_display == রিক্ত)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	যদি (p_verbose > 0)
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
	x11_display = XOpenDisplay(রিক্ত);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	যদি (p_verbose > 0 && sig_alarm_called)
	    verb_বার্তা(_("Opening the X display timed out"));
# endif
	যদি (x11_display != রিক্ত)
	{
# ifdef ELAPSED_FUNC
	    যদি (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    যদি (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = রিক্ত;
	    }
	    অন্যথা
		x11_display_from = XD_HERE;
	}
    }
    যদি (x11_window == 0 || x11_display == রিক্ত)
	প্রত্যাগম (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    প্রত্যাগম (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    স্থির পূর্ণাঙ্ক
get_x11_title(পূর্ণাঙ্ক test_only)
{
    প্রত্যাগম get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    স্থির পূর্ণাঙ্ক
get_x11_icon(পূর্ণাঙ্ক test_only)
{
    পূর্ণাঙ্ক		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    যদি (oldicon == রিক্ত && !test_only)
    {
	যদি (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	অন্যথা
	    oldicon = vim_strsave(T_NAME);
    }

    প্রত্যাগম retval;
}

    স্থির পূর্ণাঙ্ক
get_x11_thing(
    পূর্ণাঙ্ক		get_title,	// get title string
    পূর্ণাঙ্ক		test_only)
{
    XTextProperty	text_prop;
    পূর্ণাঙ্ক			retval = FALSE;
    Status		status;

    যদি (get_x11_windis() != OK)
	প্রত্যাগম FALSE;

    // Get window/icon name if any
    যদি (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    অন্যথা
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
    Window	    মূল;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned পূর্ণাঙ্ক    অঙ্ক_children;

    যাবত্ (!status || text_prop.মান == রিক্ত)
    {
	যদি (!XQuery_বৃক্ষ(x11_display, win, &মূল, &parent, &children,
		    &অঙ্ক_children))
	    ভঙ্গ;
	যদি (children)
	    XFree((শূন্য *)children);
	যদি (parent == মূল || parent == 0)
	    ভঙ্গ;

	win = parent;
	যদি (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	অন্যথা
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    যদি (status && text_prop.মান != রিক্ত)
    {
	retval = TRUE;
	যদি (!test_only)
	{
	    যদি (get_title)
		vim_free(oldtitle);
	    অন্যথা
		vim_free(oldicon);
	    যদি (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		যদি (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.মান);
		অন্যথা
		    oldicon = vim_strsave((char_u *)text_prop.মান);
	    }
	    অন্যথা
	    {
		বর্ণ    **cl;
		Status  transform_status;
		পূর্ণাঙ্ক	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		যদি (transform_status >= Success && n > 0 && cl[0])
		{
		    যদি (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    অন্যথা
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		অন্যথা
		{
		    যদি (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.মান);
		    অন্যথা
			oldicon = vim_strsave((char_u *)text_prop.মান);
		}
	    }
	}
	XFree((শূন্য *)text_prop.মান);
    }
    প্রত্যাগম retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# যদি defined(X_HAVE_UTF8_STRING)
#  যদি X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    স্থির শূন্য
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    যদি (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ধ্রুব বর্ণ *)title,
					     রিক্ত, রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
    অন্যথা
# endif
    {
# যদি XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ধ্রুব বর্ণ *)title,
					     রিক্ত, রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
#  অন্যথা
	XTextProperty	text_prop;
	বর্ণ		*c_title = (বর্ণ *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(শূন্য)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     রিক্ত, রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
#  endif
# অন্যথা
	XStoreName(x11_display, x11_window, (বর্ণ *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    স্থির শূন্য
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    যদি (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, রিক্ত, (ধ্রুব বর্ণ *)icon,
						   রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
    অন্যথা
# endif
    {
# যদি XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, রিক্ত, (ধ্রুব বর্ণ *)icon,
						   রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
#  অন্যথা
	XTextProperty	text_prop;
	বর্ণ		*c_icon = (বর্ণ *)icon;

	(শূন্য)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, রিক্ত, &text_prop,
						   রিক্ত, 0, রিক্ত, রিক্ত, রিক্ত);
#  endif
# অন্যথা
	XSetIconName(x11_display, x11_window, (বর্ণ *)icon);
# endif
    }
    XFlush(x11_display);
}

#অন্যথা  // FEAT_X11

    স্থির পূর্ণাঙ্ক
get_x11_title(পূর্ণাঙ্ক test_only UNUSED)
{
    প্রত্যাগম FALSE;
}

    স্থির পূর্ণাঙ্ক
get_x11_icon(পূর্ণাঙ্ক test_only)
{
    যদি (!test_only)
    {
	যদি (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	অন্যথা
	    oldicon = vim_strsave(T_NAME);
    }
    প্রত্যাগম FALSE;
}

#endif // FEAT_X11

    পূর্ণাঙ্ক
mch_can_restore_title(শূন্য)
{
    প্রত্যাগম get_x11_title(TRUE);
}

    পূর্ণাঙ্ক
mch_can_restore_icon(শূন্য)
{
    প্রত্যাগম get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    শূন্য
mch_settitle(char_u *title, char_u *icon)
{
    পূর্ণাঙ্ক		type = 0;
    স্থির পূর্ণাঙ্ক	recursive = 0;

    যদি (T_NAME == রিক্ত)	    // no terminal name (yet)
	প্রত্যাগম;
    যদি (title == রিক্ত && icon == রিক্ত)	    // nothing to do
	প্রত্যাগম;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    যদি (recursive)
	প্রত্যাগম;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    যদি (get_x11_windis() == OK)
	type = 1;
#endif
#যদি defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    যদি (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    যদি ((type || *T_TS != NUL) && title != রিক্ত)
    {
	যদি (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	যদি (oldtitle == রিক্ত
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (শূন্য)get_x11_title(FALSE);

	যদি (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	অন্যথা
# ifdef FEAT_GUI_GTK
	যদি (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#যদি defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	অন্যথা
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    যদি ((type || *T_CIS != NUL) && icon != রিক্ত)
    {
	যদি (oldicon == রিক্ত
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	যদি (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	অন্যথা
# ifdef FEAT_GUI_GTK
	যদি (!gui.in_use)		// don't do this if GTK+ is running
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
    শূন্য
mch_restore_title(পূর্ণাঙ্ক which)
{
    পূর্ণাঙ্ক	do_ঠেলো_বের_করো = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : রিক্ত;
    char_u *tofree = রিক্ত;
    যদি (title == oldtitle && oldtitle != রিক্ত)
    {
	tofree = vim_strsave(title);
	যদি (tofree != রিক্ত)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : রিক্ত);
    vim_free(tofree);

    যদি (do_ঠেলো_বের_করো)
    {
	// pop and push from/to the stack
	term_বের_করো_title(which);
	term_ঠেলো_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    পূর্ণাঙ্ক
vim_is_xterm(char_u *name)
{
    যদি (name == রিক্ত)
	প্রত্যাগম FALSE;
    প্রত্যাগম ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#যদি defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    পূর্ণাঙ্ক
use_xterm_like_mouse(char_u *name)
{
    প্রত্যাগম (name != রিক্ত
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
    পূর্ণাঙ্ক
use_xterm_mouse(শূন্য)
{
    যদি (ttym_flags == TTYM_SGR)
	প্রত্যাগম 4;
    যদি (ttym_flags == TTYM_URXVT)
	প্রত্যাগম 3;
    যদি (ttym_flags == TTYM_XTERM2)
	প্রত্যাগম 2;
    যদি (ttym_flags == TTYM_XTERM)
	প্রত্যাগম 1;
    প্রত্যাগম 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    পূর্ণাঙ্ক
vim_is_iris(char_u *name)
{
    যদি (name == রিক্ত)
	প্রত্যাগম FALSE;
    প্রত্যাগম (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#যদি defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    পূর্ণাঙ্ক
vim_is_vt300(char_u *name)
{
    যদি (name == রিক্ত)
	প্রত্যাগম FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    প্রত্যাগম ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != রিক্ত)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    পূর্ণাঙ্ক
mch_get_user_name(char_u *s, পূর্ণাঙ্ক len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(রিক্ত), len - 1);
    প্রত্যাগম OK;
#অন্যথা
    প্রত্যাগম mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    পূর্ণাঙ্ক
mch_get_uname(uid_t uid, char_u *s, পূর্ণাঙ্ক len)
{
#যদি defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    সংরচনা passwd   *pw;

    যদি ((pw = getpwuid(uid)) != রিক্ত
	    && pw->pw_name != রিক্ত && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	প্রত্যাগম OK;
    }
#endif
    sprintf((বর্ণ *)s, "%d", (পূর্ণাঙ্ক)uid);	    // assumes s is long enough
    প্রত্যাগম FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    শূন্য
mch_get_host_name(char_u *s, পূর্ণাঙ্ক len)
{
    সংরচনা utsname vutsname;

    যদি (uname(&vutsname) < 0)
	*s = NUL;
    অন্যথা
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#অন্যথা // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    শূন্য
mch_get_host_name(char_u *s, পূর্ণাঙ্ক len)
{
# ifdef VAXC
    vaxc$gethostname((বর্ণ *)s, len);
# অন্যথা
    gethostname((বর্ণ *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(শূন্য)
{
    প্রত্যাগম (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    পূর্ণাঙ্ক
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    যদি (kill(pid, 0) == 0)
	প্রত্যাগম TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    যদি (errno == ESRCH)
	প্রত্যাগম FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    প্রত্যাগম TRUE;
}

#যদি !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    স্থির বর্ণ *
strerror(পূর্ণাঙ্ক err)
{
    বাহ্য পূর্ণাঙ্ক	    sys_nerr;
    বাহ্য বর্ণ	    *sys_errlist[];
    স্থির বর্ণ	    er[20];

    যদি (err > 0 && err < sys_nerr)
	প্রত্যাগম (sys_errlist[err]);
    snprintf(er, পরিমাণ(er), "Error %d", err);
    প্রত্যাগম er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    পূর্ণাঙ্ক
mch_dirname(char_u *buf, পূর্ণাঙ্ক len)
{
#যদি defined(USE_GETCWD)
    যদি (getcwd((বর্ণ *)buf, len) == রিক্ত)
    {
	STRCPY(buf, strerror(errno));
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
#অন্যথা
    প্রত্যাগম (getwd((বর্ণ *)buf) != রিক্ত ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    পূর্ণাঙ্ক
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		force)		// also expand when already absolute path
{
    পূর্ণাঙ্ক		buflen = 0;
#ifdef HAVE_FCHDIR
    পূর্ণাঙ্ক		fd = -1;
    স্থির পূর্ণাঙ্ক	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    পূর্ণাঙ্ক		retval = OK;
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
# যদি CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# অন্যথা
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    যদি ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == রিক্ত || p != fname))
    {
	যদি (p == রিক্ত && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	যদি (p != রিক্ত)
	{
	    যদি (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    যদি (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		যদি (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    যদি (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((বর্ণ *)olddir) != 0))
	    {
		p = রিক্ত;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    অন্যথা
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		যদি (p - fname >= len)
		    retval = FAIL;
		অন্যথা
		{
		    vim_strncpy(buf, fname, p - fname);
		    যদি (mch_chdir((বর্ণ *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			যদি (mch_isFullName(fname))
			    retval = FAIL;
			অন্যথা
			    p = রিক্ত;
		    }
		    অন্যথা যদি (*p == '/')
			fname = p + 1;
		    অন্যথা
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	যদি (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	যদি (p != রিক্ত)
	{
	    পূর্ণাঙ্ক	l;

#ifdef HAVE_FCHDIR
	    যদি (fd >= 0)
	    {
		যদি (p_verbose >= 5)
		{
		    verbose_enter();
		    বার্তা("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    অন্যথা
#endif
		l = mch_chdir((বর্ণ *)olddir);
	    যদি (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	যদি (fd >= 0)
	    close(fd);
#endif

	buflen = (পূর্ণাঙ্ক)STRLEN(buf);
	যদি (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	অন্যথা যদি (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += পরিমাণ(PATHSEP);
	}
#endif
    }

    যদি (buflen == 0)
	buflen = (পূর্ণাঙ্ক)STRLEN(buf);

    // Catch file names which are too long.
    যদি (retval == FAIL || (পূর্ণাঙ্ক)(buflen + STRLEN(fname)) >= len)
	প্রত্যাগম FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    যদি (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    প্রত্যাগম OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    পূর্ণাঙ্ক
mch_isFullName(char_u *fname)
{
#ifdef VMS
    প্রত্যাগম ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((বর্ণ *)fname,':') || strchr((বর্ণ *)fname,'"') ||
	    (strchr((বর্ণ *)fname,'[') && strchr((বর্ণ *)fname,']'))||
	    (strchr((বর্ণ *)fname,'<') && strchr((বর্ণ *)fname,'>'))   );
#অন্যথা
    প্রত্যাগম (*fname == '/' || *fname == '~');
#endif
}

#যদি defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    শূন্য
fname_case(
    char_u	*name,
    পূর্ণাঙ্ক		len UNUSED)  // buffer size, only used when name gets longer
{
    সংরচনা stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    সংরচনা dirent *dp;

    যদি (mch_lstat((বর্ণ *)name, &st) < 0)
	প্রত্যাগম;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    যদি (slash == রিক্ত)
    {
	dirp = opendir(".");
	tail = name;
    }
    অন্যথা
    {
	*slash = NUL;
	dirp = opendir((বর্ণ *)name);
	*slash = '/';
	tail = slash + 1;
    }

    যদি (dirp == রিক্ত)
	প্রত্যাগম;

    taillen = STRLEN(tail);
    যাবত্ ((dp = readdir(dirp)) != রিক্ত)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	যদি (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    সংরচনা stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    যদি (mch_lstat((বর্ণ *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		ভঙ্গ;
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
    সংরচনা stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    যদি (stat((বর্ণ *)vms_fixfilename(name), &statb))
#অন্যথা
    যদি (stat((বর্ণ *)name, &statb))
#endif
	প্রত্যাগম -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    প্রত্যাগম statb.st_mode & ~S_ADDACE;
#অন্যথা
    প্রত্যাগম statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
mch_setperm(char_u *name, long perm)
{
    প্রত্যাগম (chmod((বর্ণ *)
#ifdef VMS
		    vms_fixfilename(name),
#অন্যথা
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#যদি defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    পূর্ণাঙ্ক
mch_fsetperm(পূর্ণাঙ্ক fd, long perm)
{
    প্রত্যাগম (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#যদি defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
প্রকারনির্দেশ সংরচনা vim_acl_solaris_T {
    পূর্ণাঙ্ক acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# যদি defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    শূন্য
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    যদি (from_file == রিক্ত)
	প্রত্যাগম;

    যদি (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    যদি (selinux_enabled <= 0)
	প্রত্যাগম;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    বর্ণ *from_context = রিক্ত;
    বর্ণ *to_context = রিক্ত;

    যদি (getfilecon((বর্ণ *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	যদি (errno == EOPNOTSUPP)
	    প্রত্যাগম;

	বার্তা_puts(_("\nCould not get security context for "));
	বার্তা_outtrans(from_file);
	বার্তা_putchar('\n');
	প্রত্যাগম;
    }
    যদি (getfilecon((বর্ণ *)to_file, &to_context) < 0)
    {
	বার্তা_puts(_("\nCould not get security context for "));
	বার্তা_outtrans(to_file);
	বার্তা_putchar('\n');
	freecon (from_context);
	প্রত্যাগম ;
    }
    যদি (strcmp(from_context, to_context) != 0)
    {
	যদি (setfilecon((বর্ণ *)to_file, from_context) < 0)
	{
	    বার্তা_puts(_("\nCould not set security context for "));
	    বার্তা_outtrans(to_file);
	    বার্তা_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# যদি defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    শূন্য
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    স্থির ধ্রুব বর্ণ * ধ্রুব smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    বর্ণ	buffer[SMACK_LABEL_LEN];
    ধ্রুব বর্ণ	*name;
    পূর্ণাঙ্ক		index;
    পূর্ণাঙ্ক		ret;
    ssize_t	size;

    যদি (from_file == রিক্ত)
	প্রত্যাগম;

    size = listxattr((বর্ণ *)from_file, রিক্ত, 0);
    // not supported or no attributes to copy
    যদি (size <= 0)
	প্রত্যাগম;

    প্রতি (index = 0 ; index < (পূর্ণাঙ্ক)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((বর্ণ*)from_file, name, buffer, পরিমাণ(buffer));
	যদি (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((বর্ণ*)to_file, name, buffer, (size_t)size, 0);
	    যদি (ret < 0)
	    {
		vim_snprintf((বর্ণ *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		বার্তা_outtrans(IObuff);
		বার্তা_putchar('\n');
	    }
	}
	অন্যথা
	{
	    // what reason of not having the attribute value?
	    বিকল্প (errno)
	    {
		পক্ষ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    প্রত্যাগম; // leave because it isn't useful to continue

		পক্ষ ERANGE:
		স্বভাব:
		    // no enough size OR unexpected error
		     vim_snprintf((বর্ণ *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    বার্তা_puts((বর্ণ *)IObuff);
		    বার্তা_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		পক্ষ ENODATA:
		    // no attribute of this name
		    ret = removexattr((বর্ণ*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ভঙ্গ;
	    }
	}
    }
}
# endif // HAVE_SMACK

# যদি defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    শূন্য
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    বর্ণ	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    বর্ণ	*চাবি;
    বর্ণ	*val = রিক্ত;
    বর্ণ	*errmsg = রিক্ত;

    যদি (from_file == রিক্ত)
	প্রত্যাগম;

    // get the length of the extended attributes
    size = listxattr((বর্ণ *)from_file, রিক্ত, 0);
    // not supported or no attributes to copy
    যদি (size <= 0)
	প্রত্যাগম;
    xattr_buf = (বর্ণ*)alloc(size);
    যদি (xattr_buf == রিক্ত)
	প্রত্যাগম;
    size = listxattr((বর্ণ *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    প্রতি (পূর্ণাঙ্ক round = 0; round < 2; round++)
    {

	চাবি = xattr_buf;
	যদি (round == 1)
	    size = tsize;

	যাবত্ (size > 0)
	{
	    vallen = getxattr((বর্ণ *)from_file, চাবি,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    যদি (vallen >= 0 && round &&
		setxattr((বর্ণ *)to_file, চাবি, val, vallen, 0) == 0)
		;
	    অন্যথা যদি (errno)
	    {
		বিকল্প (errno)
		{
		    পক্ষ E2BIG:
			errmsg = e_xattr_e2big;
			গচ্ছ error_exit;
		    পক্ষ ENOTSUP:
		    পক্ষ EACCES:
		    পক্ষ EPERM:
			ভঙ্গ;
		    পক্ষ ERANGE:
			errmsg = e_xattr_erange;
			গচ্ছ error_exit;
		    স্বভাব:
			errmsg = e_xattr_other;
			গচ্ছ error_exit;
		}
	    }

	    যদি (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(চাবি) + 1;
	    size -= keylen;
	    চাবি += keylen;
	}
	যদি (round)
	    ভঙ্গ;

	val = (বর্ণ*)alloc(max_vallen + 1);
	যদি (val == রিক্ত)
	    গচ্ছ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    যদি (errmsg != রিক্ত)
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
    vim_acl_T	ret = রিক্ত;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((বর্ণ *)fname, ACL_TYPE_ACCESS);
# অন্যথা
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    যদি (acl_get((বর্ণ *)fname, 0, &aclent) < 0)
	প্রত্যাগম রিক্ত;
    ret = (vim_acl_T)aclent;
#  অন্যথা
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(পরিমাণ(vim_acl_solaris_T));
    যদি (aclent == রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি ((aclent->acl_cnt = acl((বর্ণ *)fname, GETACLCNT, 0, রিক্ত)) < 0)
    {
	মুক্ত(aclent);
	প্রত্যাগম রিক্ত;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * পরিমাণ(aclent_t));
    যদি (aclent->acl_entry == রিক্ত)
    {
	মুক্ত(aclent);
	প্রত্যাগম রিক্ত;
    }
    যদি (acl((বর্ণ *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	মুক্ত(aclent->acl_entry);
	মুক্ত(aclent);
	প্রত্যাগম রিক্ত;
    }
    ret = (vim_acl_T)aclent;
#   অন্যথা
#    যদি defined(HAVE_AIX_ACL)
    পূর্ণাঙ্ক		aclsize;
    সংরচনা acl *aclent;

    aclsize = পরিমাণ(সংরচনা acl);
    aclent = malloc(aclsize);
    যদি (aclent == রিক্ত)
	প্রত্যাগম রিক্ত;
    যদি (statacl((বর্ণ *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	যদি (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    যদি (aclent == রিক্ত || statacl((বর্ণ *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		মুক্ত(aclent);
		প্রত্যাগম রিক্ত;
	    }
	}
	অন্যথা
	{
	    মুক্ত(aclent);
	    প্রত্যাগম রিক্ত;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    প্রত্যাগম ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    শূন্য
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    যদি (aclent == রিক্ত)
	প্রত্যাগম;
# ifdef HAVE_POSIX_ACL
    acl_set_file((বর্ণ *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# অন্যথা
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((বর্ণ *)fname, (acl_t *)aclent);
#  অন্যথা
#   ifdef HAVE_SOLARIS_ACL
    acl((বর্ণ *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   অন্যথা
#    ifdef HAVE_AIX_ACL
    chacl((বর্ণ *)fname, aclent, ((সংরচনা acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    শূন্য
mch_free_acl(vim_acl_T aclent)
{
    যদি (aclent == রিক্ত)
	প্রত্যাগম;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# অন্যথা
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  অন্যথা
#   ifdef HAVE_SOLARIS_ACL
    মুক্ত(((vim_acl_solaris_T *)aclent)->acl_entry);
    মুক্ত(aclent);
#   অন্যথা
#    ifdef HAVE_AIX_ACL
    মুক্ত(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    শূন্য
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    পূর্ণাঙ্ক
mch_isdir(char_u *name)
{
    সংরচনা stat statb;

    যদি (*name == NUL)	    // Some stat()s don't flag "" as an error.
	প্রত্যাগম FALSE;
    যদি (stat((বর্ণ *)name, &statb))
	প্রত্যাগম FALSE;
    প্রত্যাগম (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    পূর্ণাঙ্ক
mch_isrealdir(char_u *name)
{
    সংরচনা stat statb;

    যদি (*name == NUL)	    // Some stat()s don't flag "" as an error.
	প্রত্যাগম FALSE;
    যদি (mch_lstat((বর্ণ *)name, &statb))
	প্রত্যাগম FALSE;
    প্রত্যাগম (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    স্থির পূর্ণাঙ্ক
executable_file(char_u *name)
{
    সংরচনা stat	st;

    যদি (stat((বর্ণ *)name, &st))
	প্রত্যাগম 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    পূর্ণাঙ্ক vms_executable = 0;
    যদি (S_ISREG(st.st_mode) && mch_access((বর্ণ *)name, X_OK) == 0)
    {
	যদি (strstr(vms_tolower((বর্ণ*)name),".exe") != রিক্ত
		|| strstr(vms_tolower((বর্ণ*)name),".com")!= রিক্ত)
	    vms_executable = 1;
    }
    প্রত্যাগম vms_executable;
#অন্যথা
    প্রত্যাগম S_ISREG(st.st_mode) && mch_access((বর্ণ *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    পূর্ণাঙ্ক
mch_can_exe(char_u *name, char_u **path, পূর্ণাঙ্ক use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    পূর্ণাঙ্ক		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    যদি (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	যদি ((use_path || gettail(name) != name) && executable_file(name))
	{
	    যদি (path != রিক্ত)
	    {
		যদি (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		অন্যথা
		    *path = vim_strsave(name);
	    }
	    প্রত্যাগম TRUE;
	}
	প্রত্যাগম FALSE;
    }

    p = (char_u *)getenv("PATH");
    যদি (p == রিক্ত || *p == NUL)
	প্রত্যাগম -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    যদি (buf == রিক্ত)
	প্রত্যাগম -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    প্রতি (;;)
    {
	e = (char_u *)strchr((বর্ণ *)p, ':');
	যদি (e == রিক্ত)
	    e = p_end;
	elen = (size_t)(e - p);
	যদি (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((বর্ণ *)buf, bufsize, "%.*s%s%s",
		(পূর্ণাঙ্ক)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	যদি (retval == 1)
	{
	    যদি (path != রিক্ত)
	    {
		যদি (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		অন্যথা
		    *path = vim_strnsave(buf, buflen);
	    }
	    ভঙ্গ;
	}

	যদি (*e != ':')
	    ভঙ্গ;
	p = e + 1;
    }

    vim_free(buf);
    প্রত্যাগম retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    পূর্ণাঙ্ক
mch_nodetype(char_u *name)
{
    সংরচনা stat	st;

    যদি (stat((বর্ণ *)name, &st))
	প্রত্যাগম গ্রন্থি_NORMAL;
    যদি (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	প্রত্যাগম গ্রন্থি_NORMAL;
    যদি (S_ISBLK(st.st_mode))	// block device isn't writable
	প্রত্যাগম গ্রন্থি_OTHER;
    // Everything else is writable?
    প্রত্যাগম গ্রন্থি_WRITABLE;
}

    শূন্য
mch_early_init(শূন্য)
{
#ifdef HAVE_CHECK_স্তূপ_GROWTH
    পূর্ণাঙ্ক			i;

    check_স্তূপ_growth((বর্ণ *)&i);

# ifdef HAVE_স্তূপ_LIMIT
    get_স্তূপ_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#যদি defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_স্তূপ = alloc(get_signal_স্তূপ_size());
    init_signal_স্তূপ();
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

#যদি defined(EXITFREE)
    শূন্য
mch_free_mem(শূন্য)
{
# যদি defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    যদি (clip_star.owned)
	clip_lose_selection(&clip_star);
    যদি (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# যদি defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    যদি (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    যদি (xterm_dpy != রিক্ত)
	XtCloseDisplay(xterm_dpy);
    যদি (app_context != (XtAppContext)রিক্ত)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = রিক্ত; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# যদি defined(FEAT_X11)
    যদি (x11_display != রিক্ত
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# যদি defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_স্তূপ);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    স্থির শূন্য
exit_scroll(শূন্য)
{
    যদি (silent_mode)
	প্রত্যাগম;
    যদি (newline_on_exit || বার্তা_didout)
    {
	যদি (বার্তা_use_printf())
	{
	    যদি (info_message)
		mch_বার্তা("\n");
	    অন্যথা
		mch_errmsg("\r\n");
	}
	অন্যথা
	    out_char('\n');
    }
    অন্যথা যদি (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	বার্তা_clr_eos_force();		// clear the rest of the display
	windgoto((পূর্ণাঙ্ক)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# যদি (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
বাহ্য শূন্য __gcov_dump(শূন্য);
বাহ্য শূন্য __gcov_reset(শূন্য);
#  define __gcov_flush() কুরু { __gcov_dump(); __gcov_reset(); } যাবত্ (0)
# অন্যথা
বাহ্য শূন্য __gcov_flush(শূন্য);
# endif
#endif

    শূন্য
mch_exit(পূর্ণাঙ্ক r)
{
    exiting = TRUE;

#যদি defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    যদি (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	যদি (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_বের_করো_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	যদি (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	যদি (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	যদি (full_screen)
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
    যদি (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    যদি (deadly_signal != 0)
	প্রত্যাগম;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    স্থির শূন্য
may_core_dump(শূন্য)
{
    যদি (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    স্থির পূর্ণাঙ্ক
get_tty_fd(পূর্ণাঙ্ক fd)
{
    পূর্ণাঙ্ক		tty_fd = fd;

# যদি defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    যদি (mch_isatty(fd) > 1)
    {
	বর্ণ *name;

	name = ptsname(fd);
	যদি (name == রিক্ত)
	    প্রত্যাগম -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	যদি (tty_fd < 0)
	    প্রত্যাগম -1;
    }
# endif
    প্রত্যাগম tty_fd;
}

    স্থির পূর্ণাঙ্ক
mch_tcgetattr(পূর্ণাঙ্ক fd, শূন্য *term)
{
    পূর্ণাঙ্ক		tty_fd;
    পূর্ণাঙ্ক		retval = -1;

    tty_fd = get_tty_fd(fd);
    যদি (tty_fd < 0)
	প্রত্যাগম -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (সংরচনা termios *)term);
#  অন্যথা
    retval = ioctl(tty_fd, TCGETA, (সংরচনা termio *)term);
#  endif
# অন্যথা
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (সংরচনা sgttyb *)term);
# endif
    যদি (tty_fd != fd)
	close(tty_fd);
    প্রত্যাগম retval;
}

    শূন্য
mch_settmode(tmode_T tmode)
{
    স্থির পূর্ণাঙ্ক first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    স্থির সংরচনা termios told;
	   সংরচনা termios tnew;
#  অন্যথা
    স্থির সংরচনা termio told;
	   সংরচনা termio tnew;
#  endif

    যদি (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    যদি (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == রিক্ত || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  যদি defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   অন্যথা
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    অন্যথা
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    অন্যথা যদি (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  যদি defined(HAVE_TERMIOS_H)
    {
	পূর্ণাঙ্ক	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	যাবত্ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  অন্যথা
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# অন্যথা
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    স্থির সংরচনা sgttyb ttybold;
	   সংরচনা sgttyb ttybnew;

    যদি (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    যদি (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    অন্যথা যদি (tmode == TMODE_SLEEP)
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
    শূন্য
get_stty(শূন্য)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    যদি (get_tty_info(read_cmd_fd, &info) != OK)
	প্রত্যাগম;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    যদি (p != রিক্ত && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(রিক্ত);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    পূর্ণাঙ্ক
get_tty_info(পূর্ণাঙ্ক fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    সংরচনা termios চাবিগুলি;
#  অন্যথা
    সংরচনা termio চাবিগুলি;
#  endif

    যদি (mch_tcgetattr(fd, &চাবিগুলি) != -1)
    {
	info->backspace = চাবিগুলি.c_cc[VERASE];
	info->interrupt = চাবিগুলি.c_cc[VINTR];
	যদি (চাবিগুলি.c_iflag & ICRNL)
	    info->enter = NL;
	অন্যথা
	    info->enter = CAR;
	যদি (চাবিগুলি.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	অন্যথা
	    info->nl_does_cr = FALSE;
	প্রত্যাগম OK;
    }
# অন্যথা
    // for "old" tty systems
    সংরচনা sgttyb চাবিগুলি;

    যদি (mch_tcgetattr(fd, &চাবিগুলি) != -1)
    {
	info->backspace = চাবিগুলি.sg_erase;
	info->interrupt = চাবিগুলি.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	প্রত্যাগম OK;
    }
# endif
    প্রত্যাগম FAIL;
}

#endif // VMS

স্থির পূর্ণাঙ্ক	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    শূন্য
mch_setmouse(পূর্ণাঙ্ক on)
{
#ifdef FEAT_BEVAL_TERM
    স্থির পূর্ণাঙ্ক	bevalterm_ison = FALSE;
#endif
    পূর্ণাঙ্ক		xterm_mouse_vers;

#যদি defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    যদি (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    যদি (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	প্রত্যাগম;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    যদি (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    যদি (T_CXM != রিক্ত && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    অন্যথা যদি (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    যদি (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	যদি (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    যদি (xterm_mouse_vers > 0)
    {
	যদি (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	অন্যথা	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    অন্যথা যদি (ttym_flags == TTYM_DEC)
    {
	যদি (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	অন্যথা	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    অন্যথা
    {
	যদি (on)
	{
	    যদি (gpm_open())
		mouse_ison = TRUE;
	}
	অন্যথা
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    অন্যথা
    {
	যদি (on)
	{
	    যদি (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	অন্যথা
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    অন্যথা
    {
	যদি (on)
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
# অন্যথা
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	অন্যথা
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    অন্যথা
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	যদি (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	অন্যথা
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#যদি defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    শূন্য
mch_bevalterm_changed(শূন্য)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    শূন্য
check_mouse_termcode(শূন্য)
{
#ifdef FEAT_MOUSE_XTERM
    যদি (use_xterm_mouse()
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
	যদি (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    অন্যথা
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    অন্যথা
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    অন্যথা
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    অন্যথা
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    অন্যথা
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    যদি (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    অন্যথা
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    যদি (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	যদি (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    অন্যথা
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    যদি (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	যদি (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    অন্যথা
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
    পূর্ণাঙ্ক
mch_get_shellsize(শূন্য)
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
	সংরচনা winsize	ws;
	পূর্ণাঙ্ক fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	যদি (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	যদি (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(রিক্ত, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# অন্যথা // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	সংরচনা ttysize	ts;
	পূর্ণাঙ্ক fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	যদি (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	যদি (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_সারিগুলি;
#   ifdef FEAT_EVAL
	    ch_log(রিক্ত, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    যদি (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != রিক্ত)
    {
	যদি ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((বর্ণ *)p);
# ifdef FEAT_EVAL
	    ch_log(রিক্ত, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	যদি ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((বর্ণ *)p);
# ifdef FEAT_EVAL
	    ch_log(রিক্ত, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    যদি (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(রিক্ত, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    যদি (columns <= 0 || rows <= 0)
	প্রত্যাগম FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    প্রত্যাগম OK;
}

# যদি defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    শূন্য
mch_calc_cell_size(সংরচনা cellsize *cs_out)
{
   সংরচনা winsize   ws;
   পূর্ণাঙ্ক		    retval;
   পূর্ণাঙ্ক		    x_cell_size;
   পূর্ণাঙ্ক		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(রিক্ত, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   যদি (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	প্রত্যাগম;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   যদি (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# যদি defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    পূর্ণাঙ্ক
mch_report_winsize(পূর্ণাঙ্ক fd, পূর্ণাঙ্ক rows, পূর্ণাঙ্ক cols)
{
    পূর্ণাঙ্ক		tty_fd;
    পূর্ণাঙ্ক		retval = -1;

    tty_fd = get_tty_fd(fd);
    যদি (tty_fd < 0)
	প্রত্যাগম FAIL;

#  যদি defined(TIOCSWINSZ)
    সংরচনা winsize ws;

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
    ch_log(রিক্ত, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    সংরচনা ttysize ts;

    ts.ts_cols = cols;
    ts.ts_সারিগুলি = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(রিক্ত, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    যদি (tty_fd != fd)
	close(tty_fd);
    প্রত্যাগম retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    শূন্য
mch_set_shellsize(শূন্য)
{
    যদি (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((পূর্ণাঙ্ক)Rows, (পূর্ণাঙ্ক)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    শূন্য
mch_new_shellsize(শূন্য)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    স্থির pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    যাবত্ (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	যদি (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    যদি (++delay_msec > 10)
		delay_msec = 10;
	    অনুবর্ত;
	}
	যদি (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    ভঙ্গ;
    }
    প্রত্যাগম wait_pid;
}

#যদি !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    স্থির শূন্য
set_child_environment(
	long	rows,
	long	columns,
	বর্ণ	*term,
	পূর্ণাঙ্ক	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    বর্ণ	envbuf[50];
# অন্যথা
    স্থির বর্ণ	envbuf_Term[30];
    স্থির বর্ণ	envbuf_Rows[20];
    স্থির বর্ণ	envbuf_সারিগুলি[20];
    স্থির বর্ণ	envbuf_Columns[20];
    স্থির বর্ণ	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    স্থির বর্ণ	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    স্থির বর্ণ	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((বর্ণ *)envbuf, "%ld", rows);
    setenv("ROWS", (বর্ণ *)envbuf, 1);
    sprintf((বর্ণ *)envbuf, "%ld", rows);
    setenv("LINES", (বর্ণ *)envbuf, 1);
    sprintf((বর্ণ *)envbuf, "%ld", columns);
    setenv("COLUMNS", (বর্ণ *)envbuf, 1);
    sprintf((বর্ণ *)envbuf, "%d", t_colors);
    setenv("COLORS", (বর্ণ *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    যদি (is_terminal)
    {
	sprintf((বর্ণ *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (বর্ণ *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == রিক্ত ? "" : (বর্ণ *)serverName, 1);
#  endif
# অন্যথা
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, পরিমাণ(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, পরিমাণ(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_সারিগুলি, পরিমাণ(envbuf_সারিগুলি), "LINES=%ld", rows);
    putenv(envbuf_সারিগুলি);
    vim_snprintf(envbuf_Columns, পরিমাণ(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, পরিমাণ(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    যদি (is_terminal)
    {
	vim_snprintf(envbuf_Version, পরিমাণ(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, পরিমাণ(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == রিক্ত ? "" : (বর্ণ *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    স্থির শূন্য
set_default_child_environment(পূর্ণাঙ্ক is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#যদি defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    স্থির শূন্য
open_pty(পূর্ণাঙ্ক *pty_master_fd, পূর্ণাঙ্ক *pty_slave_fd, char_u **name1, char_u **name2)
{
    বর্ণ	*tty_name;

    যদি (name1 != রিক্ত)
	*name1 = রিক্ত;
    যদি (name2 != রিক্ত)
	*name2 = রিক্ত;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    যদি (*pty_master_fd < 0)
	প্রত্যাগম;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# অন্যথা
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    যদি (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    অন্যথা
    {
	যদি (name1 != রিক্ত)
	    *name1 = vim_strsave((char_u *)tty_name);
	যদি (name2 != রিক্ত)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    স্থির শূন্য
may_send_sigint(পূর্ণাঙ্ক c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    যদি (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# অন্যথা
	kill(0, SIGINT);
# endif
	যদি (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#যদি !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    পূর্ণাঙ্ক
unix_build_argv(
	char_u *cmd,
	বর্ণ ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    বর্ণ	**argv = রিক্ত;
    পূর্ণাঙ্ক		argc;

    *sh_tofree = vim_strsave(p_sh);
    যদি (*sh_tofree == রিক্ত)		// out of memory
	প্রত্যাগম FAIL;

    যদি (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	প্রত্যাগম FAIL;
    *argvp = argv;

    যদি (cmd != রিক্ত)
    {
	char_u	*s;
	char_u	*p;

	যদি (extra_shell_arg != রিক্ত)
	    argv[argc++] = (বর্ণ *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	যদি (*shcf_tofree == রিক্ত)    // out of memory
	    প্রত্যাগম FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	যাবত্ (*p != NUL)
	{
	    argv[argc++] = (বর্ণ *)s;
	    যাবত্ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (বর্ণ *)cmd;
    }
    argv[argc] = রিক্ত;
    প্রত্যাগম OK;
}
#endif

#যদি defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    স্থির পূর্ণাঙ্ক
mch_call_shell_terminal(
    char_u	*cmd,
    পূর্ণাঙ্ক		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    বর্ণ	**argv = রিক্ত;
    char_u	*tofree1 = রিক্ত;
    char_u	*tofree2 = রিক্ত;
    পূর্ণাঙ্ক		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    যদি (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	গচ্ছ theend;

    init_job_options(&opt);
    ch_log(রিক্ত, "starting terminal for system command '%s'", cmd);
    buf = term_start(রিক্ত, argv, &opt, TERM_START_SYSTEM);
    যদি (buf == রিক্ত)
	গচ্ছ theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    যদি (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	যাবত্ (term_use_loop())
	{
	    যদি (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		যদি (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    অন্যথা
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(রিক্ত, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    যদি (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    প্রত্যাগম retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    স্থির পূর্ণাঙ্ক
mch_call_shell_system(
    char_u	*cmd,
    পূর্ণাঙ্ক		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    বর্ণ	*ifn = রিক্ত;
    বর্ণ	*ofn = রিক্ত;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    পূর্ণাঙ্ক		x;

    out_flush();

    যদি (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# যদি defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  যদি defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    যদি (cmd == রিক্ত)
	x = system((বর্ণ *)p_sh);
    অন্যথা
    {
# ifdef VMS
	যদি (ofn = strchr((বর্ণ *)cmd, '>'))
	    *ofn++ = '\0';
	যদি (ifn = strchr((বর্ণ *)cmd, '<'))
	{
	    বর্ণ *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    যদি (p)
		*p = '\0';
	}
	যদি (ofn)
	    x = vms_sys((বর্ণ *)cmd, ofn, ifn);
	অন্যথা
	    x = system((বর্ণ *)cmd);
# অন্যথা
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == রিক্ত ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	যদি (newcmd == রিক্ত)
	    x = 0;
	অন্যথা
	{
	    sprintf((বর্ণ *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == রিক্ত ? "" : (বর্ণ *)extra_shell_arg,
		    (বর্ণ *)p_shcf,
		    (বর্ণ *)cmd);
	    x = system((বর্ণ *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    যদি (emsg_silent)
	;
    অন্যথা যদি (x == 127)
	বার্তা_puts(_("\nCannot execute shell sh\n"));
    অন্যথা যদি (x && !(options & SHELL_SILENT))
    {
	বার্তা_puts(_("\nshell returned "));
	বার্তা_outnum((long)x);
	বার্তা_putchar('\n');
    }

    যদি (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# যদি defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    প্রত্যাগম x;
}

#অন্যথা // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    স্থির পূর্ণাঙ্ক
mch_call_shell_fork(
    char_u	*cmd,
    পূর্ণাঙ্ক		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    পূর্ণাঙ্ক		status = -1;
    পূর্ণাঙ্ক		retval = -1;
    বর্ণ	**argv = রিক্ত;
    char_u	*tofree1 = রিক্ত;
    char_u	*tofree2 = রিক্ত;
    পূর্ণাঙ্ক		i;
    পূর্ণাঙ্ক		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    পূর্ণাঙ্ক		pty_slave_fd = -1;
# endif
    পূর্ণাঙ্ক		fd_toshell[2];		// for pipes
    পূর্ণাঙ্ক		fd_fromshell[2];
    পূর্ণাঙ্ক		pipe_error = FALSE;
    পূর্ণাঙ্ক		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    যদি (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    যদি (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    যদি (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	গচ্ছ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    যদি ((options & (SHELL_READ|SHELL_WRITE))
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
	যদি (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, রিক্ত, রিক্ত);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	যদি (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    যদি (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		যদি (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    যদি (pipe_error)
	    {
		বার্তা_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    যদি (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	যদি (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    বার্তা_puts(_("\nCannot fork\n"));
	    যদি ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		যদি (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		অন্যথা				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	অন্যথা যদি (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    যদি (ch_log_active())
	    {
		ch_log(রিক্ত, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    যদি (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		পূর্ণাঙ্ক fd;

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
		যদি (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    অন্যথা যদি ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		যদি (p_stmp)
		{
		    (শূন্য)setsid();
#  যদি defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		যদি (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    যদি (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (বর্ণ *)রিক্ত);
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
		যদি (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    যদি (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		অন্যথা
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
		    যদি (gui.in_use)
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
	অন্যথা			// parent
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
	    যদি ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		পূর্ণাঙ্ক	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		পূর্ণাঙ্ক	    ta_len = 0;		// valid bytes in ta_buf[]
		পূর্ণাঙ্ক	    len;
		পূর্ণাঙ্ক	    p_more_save;
		পূর্ণাঙ্ক	    old_State;
		পূর্ণাঙ্ক	    c;
		পূর্ণাঙ্ক	    toshell_fd;
		পূর্ণাঙ্ক	    fromshell_fd;
		garray_T    ga;
		পূর্ণাঙ্ক	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		যদি (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		অন্যথা
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

		যদি ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    যদি ((wpid = fork()) == -1)
		    {
			বার্তা_puts(_("\nCannot fork\n"));
		    }
		    অন্যথা যদি (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			প্রতি (;;)
			{
			    যদি (lplen == 0)
				len = 0;
			    অন্যথা যদি (lp[written] == NL)
				// NL -> NUL translation
				len = লিখ(toshell_fd, "", (size_t)1);
			    অন্যথা
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = লিখ(toshell_fd, (বর্ণ *)lp + written,
					   s == রিক্ত ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    যদি (len == (পূর্ণাঙ্ক)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				যদি (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_সারি_গুনতি
						    || curbuf->b_p_eol)))
				    vim_ignored = লিখ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				যদি (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ভঙ্গ;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    অন্যথা যদি (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    অন্যথা // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		যদি (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		প্রতি (;;)
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
		    যদি (!(options & SHELL_EXPAND)
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
		      যদি (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      যদি (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			যদি (len == 1 && (pty_master_fd < 0 || cmd != রিক্ত))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    যদি (pty_master_fd < 0 && toshell_fd >= 0
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
			যদি (pty_master_fd < 0)
			{
			    প্রতি (i = ta_len; i < ta_len + len; ++i)
			    {
				যদি (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    বার্তা_putchar(ta_buf[i]);
				অন্যথা যদি (has_mbyte)
				{
				    পূর্ণাঙ্ক l = (*mb_ptr2len)(ta_buf + i);

				    বার্তা_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				অন্যথা
				    বার্তা_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(বার্তা_row, cmdline_col_off + বার্তা_col);
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
			যদি (options & SHELL_WRITE)
			    ta_len = 0;
			অন্যথা যদি (toshell_fd >= 0)
			{
			    len = লিখ(toshell_fd, (বর্ণ *)ta_buf, (size_t)1);
			    যদি (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    যদি (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# অন্যথা
			kill(0, SIGINT);
# endif
			যদি (wpid > 0)
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
		    যাবত্ (RealWaitForChar(fromshell_fd, 10L, রিক্ত, রিক্ত))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			যদি (len <= 0)		    // end of file or error
			    গচ্ছ finished;

			noread_cnt = 0;
			যদি (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    প্রতি (i = 0; i < len; ++i)
			    {
				যদি (buffer[i] == NL)
				    append_ga_সারি(&ga);
				অন্যথা যদি (buffer[i] == NUL)
				    ga_append(&ga, NL);
				অন্যথা
				    ga_append(&ga, buffer[i]);
			    }
			}
			অন্যথা যদি (has_mbyte)
			{
			    পূর্ণাঙ্ক		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    প্রতি (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				যদি (l == 0)
				    l = 1;  // NUL byte?
				অন্যথা যদি (MB_BYTE2LEN(*p) != l)
				    ভঙ্গ;
			    }
			    যদি (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				যদি (len >= 12)
				    ++p;
				অন্যথা
				{
				    buffer_off = len;
				    অনুবর্ত;
				}
			    }
			    c = *p;
			    *p = NUL;
			    বার্তা_puts((বর্ণ *)buffer);
			    যদি (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				অনুবর্ত;
			    }
			    buffer_off = 0;
			}
			অন্যথা
			{
			    buffer[len] = NUL;
			    বার্তা_puts((বর্ণ *)buffer);
			}

			windgoto(বার্তা_row, cmdline_col_off + বার্তা_col);
			cursor_on();
			out_flush();
			যদি (got_int)
			    ভঙ্গ;

# ifdef ELAPSED_FUNC
			যদি (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    যদি (msec > 2000)
			    {
				noread_cnt = 5;
				ভঙ্গ;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    যদি (wait_pid == pid)
		    {
			যদি (noread_cnt < 5)
			    অনুবর্ত;
			ভঙ্গ;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    যদি ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    অন্যথা
			wait_pid = 0;

# যদি defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		যদি (options & SHELL_READ)
		{
		    যদি (ga.ga_len > 0)
		    {
			append_ga_সারি(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    অন্যথা
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		যদি (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		যদি (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# যদি (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    অন্যথা
	    {
		long delay_msec = 1;

		যদি (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		প্রতি (;;)
		{
		    যদি (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  অন্যথা
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    যদি ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			ভঙ্গ;
		    }

#  যদি defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    যদি (++delay_msec > 10)
			delay_msec = 10;
		}

		যদি (tmode == TMODE_RAW)
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
	    যদি (wait_pid != pid)
		(শূন্য)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    যদি (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    যদি (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, রিক্ত);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    যদি (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    যদি (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		যদি (retval != 0 && !emsg_silent)
		{
		    যদি (retval == EXEC_FAILED)
		    {
			বার্তা_puts(_("\nCannot execute shell "));
			বার্তা_outtrans(p_sh);
			বার্তা_putchar('\n');
		    }
		    অন্যথা যদি (!(options & SHELL_SILENT))
		    {
			বার্তা_puts(_("\nshell returned "));
			বার্তা_outnum((long)retval);
			বার্তা_putchar('\n');
		    }
		}
	    }
	    অন্যথা
		বার্তা_puts(_("\nCommand terminated\n"));
	}
    }

error:
    যদি (!did_settmode)
	যদি (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    প্রত্যাগম retval;
}
#endif // USE_SYSTEM

    পূর্ণাঙ্ক
mch_call_shell(
    char_u	*cmd,
    পূর্ণাঙ্ক		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(রিক্ত, "executing shell command: %s", cmd);
#endif
#যদি defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    যদি (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != রিক্ত
					      && (options & SHELL_SILENT) == 0)
	প্রত্যাগম mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    প্রত্যাগম mch_call_shell_system(cmd, options);
#অন্যথা
    প্রত্যাগম mch_call_shell_fork(cmd, options);
#endif
}

#যদি defined(FEAT_EVAL)
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
    বর্ণ	**argv,
    char_u	*infile,
    পূর্ণাঙ্ক		flags UNUSED,
    পূর্ণাঙ্ক		*ret_len)
{
    pid_t	pid;
    পূর্ণাঙ্ক		fd_out[2] = {-1, -1};
    পূর্ণাঙ্ক		status = -1;
    char_u	*buffer = রিক্ত;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(রিক্ত, "directly executing: %s", argv[0]);

    যদি (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	প্রত্যাগম রিক্ত;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# অন্যথা
    pid = fork();
# endif
    যদি (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	প্রত্যাগম রিক্ত;
    }

    যদি (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	যদি (ch_log_active())
	{
	    ch_log(রিক্ত, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	যদি (infile != রিক্ত)
	{
	    পূর্ণাঙ্ক fd_in = open((বর্ণ *)infile, O_RDONLY);
	    যদি (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	অন্যথা
	{
	    পূর্ণাঙ্ক nullfd = open("/dev/null", O_RDONLY);
	    যদি (nullfd >= 0)
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
    প্রতি (;;)
    {
	বর্ণ	buf[4096];
	পূর্ণাঙ্ক	n;

	n = (পূর্ণাঙ্ক)পঠ(fd_out[0], buf, পরিমাণ(buf));
	যদি (n <= 0)
	    ভঙ্গ;
	যদি (ga_grow(&ga, n) == FAIL)
	    ভঙ্গ;
	mch_memmove((বর্ণ *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (শূন্য)waitpid(pid, &status, 0);
    যদি (WIFEXITED(status))
	status = WEXITSTATUS(status);
    অন্যথা
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    যদি (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	যদি (buffer != রিক্ত)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    যদি (ret_len == রিক্ত)
	    {
		পূর্ণাঙ্ক	i;

		// Change NUL into SOH, otherwise the string is truncated.
		প্রতি (i = 0; i < ga.ga_len; ++i)
		    যদি (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    অন্যথা
		*ret_len = ga.ga_len;
	}
    }
    অন্যথা যদি (ret_len != রিক্ত)
	*ret_len = 0;

    ga_clear(&ga);
    প্রত্যাগম buffer;
}
#endif

#যদি defined(FEAT_JOB_CHANNEL)
    শূন্য
mch_job_start(বর্ণ **argv, job_T *job, jobopt_T *options, পূর্ণাঙ্ক is_terminal)
{
    pid_t	pid;
    পূর্ণাঙ্ক		fd_in[2] = {-1, -1};	// for stdin
    পূর্ণাঙ্ক		fd_out[2] = {-1, -1};	// for stdout
    পূর্ণাঙ্ক		fd_err[2] = {-1, -1};	// for stderr
    পূর্ণাঙ্ক		pty_master_fd = -1;
    পূর্ণাঙ্ক		pty_slave_fd = -1;
    channel_T	*channel = রিক্ত;
    পূর্ণাঙ্ক		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    পূর্ণাঙ্ক		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    পূর্ণাঙ্ক		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    পূর্ণাঙ্ক		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    পূর্ণাঙ্ক		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    পূর্ণাঙ্ক		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    পূর্ণাঙ্ক		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    পূর্ণাঙ্ক		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    যদি (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    যদি (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    যদি (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((বর্ণ *)fname, O_RDONLY, 0);
	যদি (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    অন্যথা
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	যদি (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    গচ্ছ failed;

    যদি (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((বর্ণ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	যদি (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    অন্যথা যদি (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	গচ্ছ failed;

    যদি (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((বর্ণ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	যদি (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    গচ্ছ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    অন্যথা যদি (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	গচ্ছ failed;

    যদি (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	যদি (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    যদি (channel != রিক্ত)
		++channel->ch_refcount;
	}
	অন্যথা
	    channel = add_channel();
	যদি (channel == রিক্ত)
	    গচ্ছ failed;
	যদি (job->jv_tty_out != রিক্ত)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    যদি (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	গচ্ছ failed;
    }
    যদি (pid == 0)
    {
	পূর্ণাঙ্ক	null_fd = -1;
	পূর্ণাঙ্ক	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	যদি (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(শূন্য)setsid();
# endif

# ifdef FEAT_TERMINAL
	যদি (options->jo_term_rows > 0)
	{
	    বর্ণ *term = (বর্ণ *)T_NAME;

#  ifdef FEAT_GUI
	    যদি (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    যদি (term == রিক্ত || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		যদি (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		অন্যথা যদি (t_colors > 16)
		    term = "xterm-color";
		অন্যথা
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	অন্যথা
# endif
	    set_default_child_environment(is_terminal);

	যদি (options->jo_env != রিক্ত)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    পূর্ণাঙ্ক		todo = (পূর্ণাঙ্ক)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_বস্তুসমূহ(&dict->dv_hashtab, hi, todo)
		যদি (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_চাবি, tv_get_string(item));
		    --todo;
		}
	}

	যদি (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    যদি (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	যদি (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (বর্ণ *)রিক্ত);
# endif
	}

	// set up stdin for the child
	close(0);
	যদি (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	অন্যথা যদি (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	অন্যথা
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	যদি (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	অন্যথা যদি (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	অন্যথা যদি (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	অন্যথা
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	যদি (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	অন্যথা যদি (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	অন্যথা
	    vim_ignored = dup(fd_out[1]);

	যদি (fd_in[0] >= 0)
	    close(fd_in[0]);
	যদি (fd_in[1] >= 0)
	    close(fd_in[1]);
	যদি (fd_out[0] >= 0)
	    close(fd_out[0]);
	যদি (fd_out[1] >= 0)
	    close(fd_out[1]);
	যদি (fd_err[0] >= 0)
	    close(fd_err[0]);
	যদি (fd_err[1] >= 0)
	    close(fd_err[1]);
	যদি (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	যদি (null_fd >= 0)
	    close(null_fd);

	যদি (options->jo_cwd != রিক্ত && mch_chdir((বর্ণ *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	যদি (stderr_works)
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

    যদি (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    যদি (fd_in[0] >= 0)
	close(fd_in[0]);
    যদি (fd_out[1] >= 0)
	close(fd_out[1]);
    যদি (fd_err[1] >= 0)
	close(fd_err[1]);
    যদি (channel != রিক্ত)
    {
	পূর্ণাঙ্ক in_fd = INVALID_FD;
	পূর্ণাঙ্ক out_fd = INVALID_FD;
	পূর্ণাঙ্ক err_fd = INVALID_FD;

	যদি (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	যদি (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	যদি (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    যদি (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    অন্যথা যদি (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    অন্যথা
    {
	যদি (fd_in[1] >= 0)
	    close(fd_in[1]);
	যদি (fd_out[0] >= 0)
	    close(fd_out[0]);
	যদি (fd_err[0] >= 0)
	    close(fd_err[0]);
	যদি (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    প্রত্যাগম;

failed:
    channel_unref(channel);
    যদি (fd_in[0] >= 0)
	close(fd_in[0]);
    যদি (fd_in[1] >= 0)
	close(fd_in[1]);
    যদি (fd_out[0] >= 0)
	close(fd_out[0]);
    যদি (fd_out[1] >= 0)
	close(fd_out[1]);
    যদি (fd_err[0] >= 0)
	close(fd_err[0]);
    যদি (fd_err[1] >= 0)
	close(fd_err[1]);
    যদি (pty_master_fd >= 0)
	close(pty_master_fd);
    যদি (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    স্থির char_u *
get_signal_name(পূর্ণাঙ্ক sig)
{
    পূর্ণাঙ্ক		i;
    char_u	numbuf[NUMBUFLEN];

    যদি (sig == SIGKILL)
	প্রত্যাগম vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    প্রতি (i = 0; signal_info[i].sig != -1; i++)
	যদি (sig == signal_info[i].sig)
	    প্রত্যাগম strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((বর্ণ *)numbuf, NUMBUFLEN, "%d", sig);
    প্রত্যাগম vim_strnsave(numbuf, i);
}

    বর্ণ *
mch_job_status(job_T *job)
{
    পূর্ণাঙ্ক		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    যদি (wait_pid == -1)
    {
	পূর্ণাঙ্ক waitpid_errno = errno;
	যদি (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    প্রত্যাগম "run";

	// process must have exited
	যদি (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	গচ্ছ return_dead;
    }
    যদি (wait_pid == 0)
	প্রত্যাগম "run";
    যদি (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	যদি (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	গচ্ছ return_dead;
    }
    যদি (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	যদি (job->jv_status < JOB_ENDED && job->jv_termsig != রিক্ত)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	গচ্ছ return_dead;
    }
    প্রত্যাগম "run";

return_dead:
    যদি (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    প্রত্যাগম "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    পূর্ণাঙ্ক		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    যদি (dont_check_job_ended > 0)
	প্রত্যাগম রিক্ত;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    যদি (wait_pid <= 0)
	// no process ended
	প্রত্যাগম রিক্ত;
    প্রতি (job = job_list; job != রিক্ত; job = job->jv_next)
    {
	যদি (job->jv_pid == wait_pid)
	{
	    যদি (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    অন্যথা যদি (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    যদি (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    প্রত্যাগম job;
	}
    }
    প্রত্যাগম রিক্ত;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    পূর্ণাঙ্ক
mch_signal_job(job_T *job, char_u *how)
{
    পূর্ণাঙ্ক	    sig = -1;

    যদি (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    অন্যথা যদি (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    অন্যথা যদি (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    অন্যথা যদি (STRCMP(how, "int") == 0)
	sig = SIGINT;
    অন্যথা যদি (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    অন্যথা যদি (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    অন্যথা যদি (SAFE_isdigit(*how))
	sig = atoi((বর্ণ *)how);
    অন্যথা
	প্রত্যাগম FAIL;

    // Never kill ourselves!
    যদি (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    প্রত্যাগম OK;
}

/*
 * Clear the data related to "job".
 */
    শূন্য
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (শূন্য)waitpid(job->jv_pid, রিক্ত, WNOHANG);
}
#endif

#যদি defined(FEAT_TERMINAL)
    পূর্ণাঙ্ক
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    পূর্ণাঙ্ক		pty_master_fd = -1;
    পূর্ণাঙ্ক		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    যদি (pty_master_fd < 0 || pty_slave_fd < 0)
	প্রত্যাগম FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    যদি (channel == রিক্ত)
    {
	close(pty_master_fd);
	প্রত্যাগম FAIL;
    }
    যদি (job->jv_tty_out != রিক্ত)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    প্রত্যাগম OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    শূন্য
mch_breakcheck(পূর্ণাঙ্ক force)
{
    যদি ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, রিক্ত, রিক্ত))
	fill_প্রবেশ_buf(FALSE);
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
    স্থির পূর্ণাঙ্ক
WaitForChar(long msec, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ)
{
#ifdef FEAT_TIMERS
    প্রত্যাগম ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_প্রবেশ) == OK;
#অন্যথা
    প্রত্যাগম WaitForCharOrMouse(msec, interrupted, ignore_প্রবেশ);
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
    স্থির পূর্ণাঙ্ক
WaitForCharOrMouse(long msec, পূর্ণাঙ্ক *interrupted, পূর্ণাঙ্ক ignore_প্রবেশ)
{
#ifdef FEAT_MOUSE_GPM
    পূর্ণাঙ্ক		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    পূর্ণাঙ্ক		rest;
#endif
    পূর্ণাঙ্ক		avail;

    যদি (!ignore_প্রবেশ && প্রবেশ_available())	    // something in inbuf[]
	প্রত্যাগম 1;

#যদি defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    যদি (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	যদি (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#যদি defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    যদি (do_xterm_trace())
	rest = msec;
# endif
    কুরু
    {
# ifdef FEAT_XCLIPBOARD
	যদি (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    যদি (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    যদি (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	যদি (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	যদি (!avail && !gpm_process_wanted)
# অন্যথা
	avail = RealWaitForChar(read_cmd_fd, msec, রিক্ত, interrupted);
	যদি (!avail)
# endif
	{
	    যদি (!ignore_প্রবেশ && প্রবেশ_available())
		প্রত্যাগম 1;
# ifdef FEAT_XCLIPBOARD
	    যদি (rest == 0 || !do_xterm_trace())
# endif
		ভঙ্গ;
	}
    }
    যাবত্ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#অন্যথা
    avail = RealWaitForChar(read_cmd_fd, msec, রিক্ত, interrupted);
#endif
    প্রত্যাগম avail;
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
    স্থির পূর্ণাঙ্ক
RealWaitForChar(পূর্ণাঙ্ক fd, long msec, পূর্ণাঙ্ক *check_for_gpm UNUSED, পূর্ণাঙ্ক *interrupted)
{
    পূর্ণাঙ্ক		ret;
    পূর্ণাঙ্ক		result;
# যদি defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    স্থির পূর্ণাঙ্ক	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    যদি (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    যদি (busy)
	প্রত্যাগম 0;
# endif

# ifdef MAY_LOOP
    প্রতি (;;)
# endif
    {
# ifdef MAY_LOOP
	পূর্ণাঙ্ক		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	পূর্ণাঙ্ক		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	পূর্ণাঙ্ক		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	সংরচনা pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	পূর্ণাঙ্ক		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	পূর্ণাঙ্ক             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	পূর্ণাঙ্ক		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	পূর্ণাঙ্ক		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	পূর্ণাঙ্ক		xsmp_idx = -1;
#  endif
	পূর্ণাঙ্ক		towait = (পূর্ণাঙ্ক)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	যদি (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (পূর্ণাঙ্ক)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	যদি ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	যদি (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_সংখ্যা(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	যদি (check_for_gpm != রিক্ত && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	যদি (xsmp_icefd != -1)
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
	যদি (interrupted != রিক্ত)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	যদি (result == 0 && interrupted != রিক্ত && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	যদি (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	যদি (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	যদি (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    যদি (--ret == 0 && !প্রবেশ_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	যদি (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	যদি (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    যদি (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    অন্যথা যদি (fds[xsmp_idx].revents & POLLHUP)
	    {
		যদি (p_verbose > 0)
		    verb_বার্তা(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    যদি (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	যদি (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# অন্যথা // HAVE_SELECT

	সংরচনা timeval  tv;
	সংরচনা timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	স্থির fd_set	rfds, wfds, efds;
	পূর্ণাঙ্ক		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	যদি (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	যদি (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	অন্যথা
	    tvp = রিক্ত;

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
	যদি ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    যদি (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	যদি (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_সংখ্যা(xterm_dpy), &rfds);
	    যদি (maxfd < Connection_সংখ্যা(xterm_dpy))
		maxfd = Connection_সংখ্যা(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	যদি (check_for_gpm != রিক্ত && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    যদি (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	যদি (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    যদি (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	যদি (interrupted != রিক্ত)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	যদি (result)
	    --ret;
	অন্যথা যদি (interrupted != রিক্ত && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	যদি (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    যদি (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    যদি (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(রিক্ত, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    গচ্ছ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	যদি (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	যদি (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	যদি (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	যদি (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_সংখ্যা(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    যদি (--ret == 0 && !প্রবেশ_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	যদি (ret > 0 && check_for_gpm != রিক্ত && gpm_flag && gpm_fd >= 0)
	{
	    যদি (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    অন্যথা যদি (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	যদি (ret > 0 && xsmp_icefd != -1)
	{
	    যদি (FD_ISSET(xsmp_icefd, &efds))
	    {
		যদি (p_verbose > 0)
		    verb_বার্তা(_("XSMP lost ICE connection"));
		xsmp_close();
		যদি (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    অন্যথা যদি (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		যদি (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	যদি (ret >= 0)
	    (শূন্য)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	যদি (finished || msec == 0)
	    ভঙ্গ;

#  যদি defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	যদি (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    ভঙ্গ;
#  endif

	// We're going to loop around again, find out for how long
	যদি (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  অন্যথা
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    যদি (msec <= 0)
		ভঙ্গ;	// waited long enough
	}
# endif
    }

    প্রত্যাগম result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    পূর্ণাঙ্ক
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    পূর্ণাঙ্ক		flags)		// EW_* flags
{
    প্রত্যাগম unix_expandpath(gap, path, 0, flags, FALSE);
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

    পূর্ণাঙ্ক
mch_expand_wildcards(
    পূর্ণাঙ্ক		   অঙ্ক_pat,
    char_u	 **pat,
    পূর্ণাঙ্ক		  *অঙ্ক_file,
    char_u	***file,
    পূর্ণাঙ্ক		   flags)	// EW_* flags
{
    পূর্ণাঙ্ক		i;
    size_t	len;
    long	llen;
    char_u	*p;
    পূর্ণাঙ্ক		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    পূর্ণাঙ্ক		j;
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
    পূর্ণাঙ্ক		shell_style = STYLE_ECHO;
    পূর্ণাঙ্ক		check_spaces;
    স্থির পূর্ণাঙ্ক	did_find_nul = FALSE;
    পূর্ণাঙ্ক		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    স্থির string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    স্থির string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *অঙ্ক_file = 0;	// default: no files found
    *file = রিক্ত;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    যদি (!have_wildcard(অঙ্ক_pat, pat))
	প্রত্যাগম save_patterns(অঙ্ক_pat, pat, অঙ্ক_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    যদি (sandbox != 0 && check_secure())
	প্রত্যাগম FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    যদি (secure || restricted)
	প্রতি (i = 0; i < অঙ্ক_pat; ++i)
	    যদি (vim_strchr(pat[i], '`') != রিক্ত
		    && (check_restricted() || check_secure()))
		প্রত্যাগম FAIL;

    /*
     * get a name for the temp file
     */
    যদি ((tempname = vim_tempname('o', FALSE)) == রিক্ত)
    {
	emsg(_(e_cant_get_temp_file_name));
	প্রত্যাগম FAIL;
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
    যদি (অঙ্ক_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    অন্যথা যদি ((len = STRLEN(p_sh)) >= 3)
    {
	যদি (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	অন্যথা যদি (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    যদি (shell_style == STYLE_ECHO)
    {
	যদি (strstr((বর্ণ *)gettail(p_sh), "bash") != রিক্ত)
	    shell_style = STYLE_GLOBSTAR;
	অন্যথা যদি (strstr((বর্ণ *)gettail(p_sh), "sh") != রিক্ত)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    যদি (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    অন্যথা যদি (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    প্রতি (i = 0; i < অঙ্ক_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# অন্যথা
	++len;				// add space
	প্রতি (j = 0; pat[i][j] != NUL; ++j)
	{
	    যদি (vim_strchr(SHELL_SPECIAL, pat[i][j]) != রিক্ত)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    যদি (command == রিক্ত)
    {
	// out of memory
	vim_free(tempname);
	প্রত্যাগম FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    যদি (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((বর্ণ *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((বর্ণ *)command, ")>");
	যদি (p == রিক্ত)
	{
	    ;					    // can't happen ;-)
	}
	অন্যথা
	{
	    --p;
	    যাবত্ (p > command && VIM_ISWHITE(*p))
		--p;
	    যদি (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    যাবত্ (p > command && VIM_ISWHITE(*p))
		--p;
	    যদি (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    অন্যথা
    {
	যদি (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((বর্ণ *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	অন্যথা যদি (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((বর্ণ *)command, len, "print -N >%s", tempname);
	অন্যথা যদি (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((বর্ণ *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	অন্যথা যদি (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((বর্ণ *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	অন্যথা
	    commandlen = vim_snprintf((বর্ণ *)command, len, "echo >%s", tempname);

	প্রতি (i = 0; i < অঙ্ক_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((বর্ণ *)command + commandlen, len, " \"%s\"", pat[i]);
# অন্যথা
	    পূর্ণাঙ্ক intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    প্রতি (j = 0; pat[i][j] != NUL; ++j)
	    {
		যদি (pat[i][j] == '`')
		    intick = !intick;
		অন্যথা যদি (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    যদি (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != রিক্ত
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		অন্যথা যদি (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != রিক্ত)
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
    যদি (flags & EW_SILENT)
	show_shell_mess = FALSE;
    যদি (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    যদি (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    অন্যথা যদি (shell_style == STYLE_GLOB && !have_dollars(অঙ্ক_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    যদি (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = রিক্ত;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    যদি (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	যদি (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    বার্তা_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    যদি (!(flags & EW_SILENT))
# endif
	    {
		বার্তা(_(e_cannot_expand_wildcards));
		বার্তা_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	যদি (shell_style == STYLE_BT)
	    প্রত্যাগম FAIL;
	গচ্ছ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((বর্ণ *)tempname, READBIN);
    যদি (fd == রিক্ত)
    {
	// Something went wrong, perhaps a file name with a special char.
	যদি (!(flags & EW_SILENT))
	{
	    বার্তা(_(e_cannot_expand_wildcards));
	    বার্তা_start();		// don't overwrite this message
	}
	vim_free(tempname);
	গচ্ছ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    যদি (llen < 0)
	// just in case ftell() would fail
	buffer = রিক্ত;
    অন্যথা
	buffer = alloc(llen + 1);
    যদি (buffer == রিক্ত)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	প্রত্যাগম FAIL;
    }
    len = llen;
    i = fread((বর্ণ *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    যদি (i != (পূর্ণাঙ্ক)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	প্রত্যাগম FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    প্রতি (i = 0; i < (পূর্ণাঙ্ক)len; ++i)
	যদি (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    যদি (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	প্রতি (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    যাবত্ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    অন্যথা যদি (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	প্রতি (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    যাবত্ (*p != '\n' && *p != NUL)
		++p;
	    যদি (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    অন্যথা
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
	যদি (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    যদি (len && (পূর্ণাঙ্ক)STRLEN(buffer) < (পূর্ণাঙ্ক)len)
		did_find_nul = TRUE;
	    অন্যথা
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	যদি (len && buffer[len - 1] == NUL)
	    --len;
	অন্যথা
	    buffer[len] = NUL;
	i = 0;
	প্রতি (p = buffer; p < buffer + len; ++p)
	    যদি (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	যদি (len)
	    ++i;			// count last entry
    }
    যদি (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	গচ্ছ notfound;
    }
    *অঙ্ক_file = i;
    *file = ALLOC_MULT(char_u *, i);
    যদি (*file == রিক্ত)
    {
	// out of memory
	vim_free(buffer);
	প্রত্যাগম FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    প্রতি (i = 0; i < *অঙ্ক_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	যদি (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    যাবত্ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    যদি (p == buffer + len)		// last entry
		*p = NUL;
	    অন্যথা
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	অন্যথা		// NUL separates
	{
	    যাবত্ (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    প্রতি (j = 0, i = 0; i < *অঙ্ক_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	যদি (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    অনুবর্ত;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	যদি ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    অনুবর্ত;

	// Skip files that are not executable if we check for that.
	যদি (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], রিক্ত, !(flags & EW_SHELLCMD)))
	    অনুবর্ত;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	যদি (p)
	{
	    STRCPY(p, (*file)[i]);
	    যদি (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *অঙ্ক_file = j;

    যদি (*অঙ্ক_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	গচ্ছ notfound;
    }

    প্রত্যাগম OK;

notfound:
    যদি (flags & EW_NOTFOUND)
	প্রত্যাগম save_patterns(অঙ্ক_pat, pat, অঙ্ক_file, file);
    প্রত্যাগম FAIL;
}

#endif // VMS

    স্থির পূর্ণাঙ্ক
save_patterns(
    পূর্ণাঙ্ক		অঙ্ক_pat,
    char_u	**pat,
    পূর্ণাঙ্ক		*অঙ্ক_file,
    char_u	***file)
{
    পূর্ণাঙ্ক		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, অঙ্ক_pat);
    যদি (*file == রিক্ত)
	প্রত্যাগম FAIL;
    প্রতি (i = 0; i < অঙ্ক_pat; i++)
    {
	s = vim_strsave(pat[i]);
	যদি (s != রিক্ত)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *অঙ্ক_file = অঙ্ক_pat;
    প্রত্যাগম OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    পূর্ণাঙ্ক
mch_has_exp_wildcard(char_u *p)
{
    প্রতি ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা
	    যদি (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#অন্যথা
				    "*?[{'"
#endif
						, *p) != রিক্ত)
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    পূর্ণাঙ্ক
mch_has_wildcard(char_u *p)
{
    প্রতি ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা
	    যদি (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#অন্যথা
				    "*?[{`'$"
#endif
						, *p) != রিক্ত
		|| (*p == '~' && p[1] != NUL))
	    প্রত্যাগম TRUE;
    }
    প্রত্যাগম FALSE;
}

    স্থির পূর্ণাঙ্ক
have_wildcard(পূর্ণাঙ্ক অঙ্ক, char_u **file)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < অঙ্ক; i++)
	যদি (mch_has_wildcard(file[i]))
	    প্রত্যাগম 1;
    প্রত্যাগম 0;
}

    স্থির পূর্ণাঙ্ক
have_dollars(পূর্ণাঙ্ক অঙ্ক, char_u **file)
{
    পূর্ণাঙ্ক	    i;

    প্রতি (i = 0; i < অঙ্ক; i++)
	যদি (vim_strchr(file[i], '$') != রিক্ত)
	    প্রত্যাগম TRUE;
    প্রত্যাগম FALSE;
}

#যদি !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    পূর্ণাঙ্ক
mch_rename(ধ্রুব বর্ণ *src, ধ্রুব বর্ণ *dest)
{
    সংরচনা stat	    st;

    যদি (stat(dest, &st) >= 0)	    // fail if destination exists
	প্রত্যাগম -1;
    যদি (link(src, dest) != 0)	    // link file to new name
	প্রত্যাগম -1;
    যদি (mch_remove(src) == 0)	    // delete link to old name
	প্রত্যাগম 0;
    প্রত্যাগম -1;
}
#endif // !HAVE_RENAME

#যদি defined(FEAT_MOUSE_GPM)
# যদি defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    স্থির পূর্ণাঙ্ক
load_libgpm(শূন্য)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    যদি (libgpm_hinst == রিক্ত)
    {
	যদি (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	প্রত্যাগম FAIL;
    }

    যদি (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == রিক্ত
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == রিক্ত
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == রিক্ত
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == রিক্ত
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == রিক্ত
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = রিক্ত;
	dll_gpm_flag = রিক্ত;
	dll_gpm_fd   = রিক্ত;
	প্রত্যাগম FAIL;
    }
    প্রত্যাগম OK;
}

    পূর্ণাঙ্ক
gpm_available(শূন্য)
{
    প্রত্যাগম libgpm_hinst != রিক্ত || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    স্থির পূর্ণাঙ্ক
gpm_open(শূন্য)
{
    স্থির Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    যদি (!gpm_available())
	প্রত্যাগম 0;
# endif

    যদি (gpm_flag)
	প্রত্যাগম 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    যদি (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	প্রত্যাগম 1; // succeed
    }
    যদি (gpm_fd == -2)
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
    প্রত্যাগম 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    পূর্ণাঙ্ক
gpm_enabled(শূন্য)
{
    প্রত্যাগম gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    স্থির শূন্য
gpm_close(শূন্য)
{
    যদি (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    স্থির পূর্ণাঙ্ক
mch_gpm_process(শূন্য)
{
    পূর্ণাঙ্ক			button;
    স্থির Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    পূর্ণাঙ্ক			row,col;
    unsigned বর্ণ	buttons_mask;
    unsigned বর্ণ	gpm_modifiers;
    স্থির unsigned বর্ণ old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    যদি (hold_gui_events)
	প্রত্যাগম 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    বিকল্প (GPM_BARE_EVENTS(gpm_event.type))
    {
	পক্ষ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ভঙ্গ;
	পক্ষ GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    বিকল্প (buttons_mask)
	    {
		পক্ষ GPM_B_বাম:
		    button = MOUSE_বাম;
		    ভঙ্গ;
		পক্ষ GPM_B_মধ্য:
		    button = MOUSE_মধ্য;
		    ভঙ্গ;
		পক্ষ GPM_B_ডান:
		    button = MOUSE_ডান;
		    ভঙ্গ;
		স্বভাব:
		    প্রত্যাগম 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_অঙ্ক_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ভঙ্গ;
	পক্ষ GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ভঙ্গ;
	স্বভাব:
	    প্রত্যাগম 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    যদি (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    যদি (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    যদি (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_প্রবেশ_buf(string, 6);
    প্রত্যাগম 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    স্থির পূর্ণাঙ্ক
sysmouse_open(শূন্য)
{
    সংরচনা mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.সঙ্কেত = SIGUSR2;
    যদি (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	প্রত্যাগম FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    প্রত্যাগম OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    স্থির শূন্য
sysmouse_close(শূন্য)
{
    সংরচনা mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.সঙ্কেত = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    স্থির শূন্য
sig_sysmouse SIGDEFARG(sigarg)
{
    সংরচনা mouse_info	mouse;
    সংরচনা video_info	video;
    char_u		string[6];
    পূর্ণাঙ্ক			row, col;
    পূর্ণাঙ্ক			button;
    পূর্ণাঙ্ক			buttons;
    স্থির পূর্ণাঙ্ক		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    যদি (hold_gui_events)
	প্রত্যাগম;
# endif

    mouse.operation = MOUSE_GETINFO;
    যদি (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	যদি (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	অন্যথা
	{
	    বিকল্প (buttons)
	    {
		পক্ষ 0:
		    button = MOUSE_RELEASE;
		    ভঙ্গ;
		পক্ষ 1:
		    button = MOUSE_বাম;
		    ভঙ্গ;
		পক্ষ 2:
		    button = MOUSE_মধ্য;
		    ভঙ্গ;
		পক্ষ 4:
		    button = MOUSE_ডান;
		    ভঙ্গ;
		স্বভাব:
		    প্রত্যাগম;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_প্রবেশ_buf(string, 6);
    }
    প্রত্যাগম;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    পূর্ণাঙ্ক
mch_get_random(char_u *buf, পূর্ণাঙ্ক len)
{
    স্থির পূর্ণাঙ্ক dev_urandom_state = NOTDONE;

    যদি (dev_urandom_state == FAIL)
	প্রত্যাগম FAIL;

    পূর্ণাঙ্ক fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    যদি (fd == -1)
	dev_urandom_state = FAIL;
    অন্যথা যদি (পঠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    অন্যথা
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    প্রত্যাগম dev_urandom_state;
}

#যদি defined(FEAT_LIBCALL)
প্রকারনির্দেশ char_u * (*STRPROCSTR)(char_u *);
প্রকারনির্দেশ char_u * (*INTPROCSTR)(পূর্ণাঙ্ক);
প্রকারনির্দেশ পূর্ণাঙ্ক (*STRPROCINT)(char_u *);
প্রকারনির্দেশ পূর্ণাঙ্ক (*INTPROCINT)(পূর্ণাঙ্ক);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    পূর্ণাঙ্ক
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    পূর্ণাঙ্ক		argint,
    char_u	**string_result, // NULL when using number_result
    পূর্ণাঙ্ক		*সংখ্যা_result)
{
# যদি defined(USE_DLOPEN)
    শূন্য	*hinstLib;
    বর্ণ	*dlerr = রিক্ত;
# অন্যথা
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = রিক্ত;
    পূর্ণাঙ্ক		retval_int = 0;
    পূর্ণাঙ্ক		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# যদি defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (শূন্য)dlerror();

    hinstLib = dlopen((বর্ণ *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    যদি (hinstLib == রিক্ত)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	যদি (dlerr != রিক্ত)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# অন্যথা
    hinstLib = shl_load((ধ্রুব বর্ণ*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    যদি (hinstLib != রিক্ত)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	যদি (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  যদি defined(USE_DLOPEN)
	    dlerr = রিক্ত;
#  endif
	    mch_didjmp();
	}
	অন্যথা
# endif
	{
	    retval_str = রিক্ত;
	    retval_int = 0;

	    যদি (argstring != রিক্ত)
	    {
# যদি defined(USE_DLOPEN)
		*(শূন্য **)(&ProcAdd) = dlsym(hinstLib, (ধ্রুব বর্ণ *)funcname);
		dlerr = dlerror();
# অন্যথা
		যদি (shl_findsym(&hinstLib, (ধ্রুব বর্ণ *)funcname,
					TYPE_PROCEDURE, (শূন্য *)&ProcAdd) < 0)
		    ProcAdd = রিক্ত;
# endif
		যদি ((success = (ProcAdd != রিক্ত
# যদি defined(USE_DLOPEN)
			    && dlerr == রিক্ত
# endif
			    )))
		{
		    যদি (string_result == রিক্ত)
			retval_int = ((STRPROCINT)(শূন্য *)ProcAdd)(argstring);
		    অন্যথা
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    অন্যথা
	    {
# যদি defined(USE_DLOPEN)
		*(শূন্য **)(&ProcAddI) = dlsym(hinstLib, (ধ্রুব বর্ণ *)funcname);
		dlerr = dlerror();
# অন্যথা
		যদি (shl_findsym(&hinstLib, (ধ্রুব বর্ণ *)funcname,
				       TYPE_PROCEDURE, (শূন্য *)&ProcAddI) < 0)
		    ProcAddI = রিক্ত;
# endif
		যদি ((success = (ProcAddI != রিক্ত
# যদি defined(USE_DLOPEN)
			    && dlerr == রিক্ত
# endif
			    )))
		{
		    যদি (string_result == রিক্ত)
			retval_int = ((INTPROCINT)(শূন্য *)ProcAddI)(argint);
		    অন্যথা
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    যদি (string_result == রিক্ত)
		*সংখ্যা_result = retval_int;
	    অন্যথা যদি (retval_str != রিক্ত
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	যদি (lc_signal != 0)
	{
	    পূর্ণাঙ্ক i;

	    // try to find the name of this signal
	    প্রতি (i = 0; signal_info[i].sig != -1; i++)
		যদি (lc_signal == signal_info[i].sig)
		    ভঙ্গ;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# যদি defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	যদি (dlerr != রিক্ত)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(শূন্য)dlclose(hinstLib);
# অন্যথা
	(শূন্য)shl_unload(hinstLib);
# endif
    }

    যদি (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	প্রত্যাগম FAIL;
    }

    প্রত্যাগম OK;
}
#endif

#যদি defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
স্থির পূর্ণাঙ্ক	xterm_trace = -1;	// default: disabled
স্থির পূর্ণাঙ্ক	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    শূন্য
setup_term_clip(শূন্য)
{
    পূর্ণাঙ্ক		z = 0;
    বর্ণ	*strp = "";
    Widget	AppShell;

    যদি (!x_connect_to_server())
	প্রত্যাগম;

    open_app_context();
    যদি (app_context != রিক্ত && xterm_Shell == (Widget)0)
    {
	পূর্ণাঙ্ক (*oldhandler)(Display*, XErrorEvent*);
# যদি defined(USING_SETJMP)
	পূর্ণাঙ্ক (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	যদি (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# যদি defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	যদি (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = রিক্ত;
	}
	অন্যথা
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", রিক্ত, 0, &z, &strp);
	    যদি (xterm_dpy != রিক্ত)
		xterm_dpy_retry_গুনতি = 0;
# যদি defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# যদি defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(শূন্য)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(শূন্য)XSetErrorHandler(oldhandler);

	যদি (xterm_dpy == রিক্ত)
	{
	    যদি (p_verbose > 0)
		verb_বার্তা(_("Opening the X display failed"));
	    প্রত্যাগম;
	}

	// Catch terminating error of the X server connection.
	(শূন্য)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	যদি (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		রিক্ত);
	যদি (AppShell == (Widget)0)
	    প্রত্যাগম;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		শীর্ষ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		রিক্ত);
	যদি (xterm_Shell == (Widget)0)
	    প্রত্যাগম;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	যদি (x11_display == রিক্ত)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    যদি (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	যদি (x11_window == 0 && (strp = getenv("WINDOWID")) != রিক্ত)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	যদি (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	যদি (x11_window != 0)
	    xterm_trace = 0;
    }
}

    শূন্য
start_xterm_trace(পূর্ণাঙ্ক button)
{
    যদি (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	প্রত্যাগম;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    শূন্য
stop_xterm_trace(শূন্য)
{
    যদি (xterm_trace < 0)
	প্রত্যাগম;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    স্থির পূর্ণাঙ্ক
do_xterm_trace(শূন্য)
{
    Window		মূল, child;
    পূর্ণাঙ্ক			মূল_x, মূল_y;
    পূর্ণাঙ্ক			win_x, win_y;
    পূর্ণাঙ্ক			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    স্থির char_u	*mouse_code = রিক্ত;
    স্থির size_t	mouse_codelen = 0;
    স্থির char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    স্থির পূর্ণাঙ্ক		prev_row = 0, prev_col = 0;
    স্থির XSizeHints	xterm_hints;

    যদি (xterm_trace <= 0)
	প্রত্যাগম FALSE;

    যদি (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	যদি (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    প্রত্যাগম FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	যদি (mouse_code != রিক্ত)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &মূল, &child, &মূল_x, &মূল_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	যদি (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	যদি (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	প্রত্যাগম TRUE;
    }

    যদি (mouse_code == রিক্ত || mouse_codelen > 45)
    {
	xterm_trace = 0;
	প্রত্যাগম FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &মূল, &child, &মূল_x, &মূল_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    যদি (row == prev_row && col == prev_col)
	প্রত্যাগম TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_প্রবেশ_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    প্রত্যাগম TRUE;
}

# যদি defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    শূন্য
clear_xterm_clip(শূন্য)
{
    যদি (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    যদি (xterm_dpy != রিক্ত)
    {
#  যদি 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	যদি (x11_display == xterm_dpy)
	    x11_display = রিক্ত;
	xterm_dpy = রিক্ত;
    }
#  যদি 0
    যদি (app_context != (XtAppContext)রিক্ত)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)রিক্ত;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    স্থির শূন্য
clip_update(শূন্য)
{
# ifdef FEAT_GUI
    যদি (gui.in_use)
	gui_mch_update();
    অন্যথা
# endif
    যদি (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    শূন্য
xterm_update(শূন্য)
{
    XEvent event;

    প্রতি (;;)
    {
	Xt_প্রবেশ_Mask mask = XtAppPending(app_context);

	যদি (mask == 0 || vim_is_প্রবেশ_buf_full())
	    ভঙ্গ;

	যদি (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		যদি (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_মান)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	অন্যথা
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    পূর্ণাঙ্ক
clip_xterm_own_selection(Clipboard_T *cbd)
{
    যদি (xterm_Shell != (Widget)0)
	প্রত্যাগম clip_x11_own_selection(xterm_Shell, cbd);
    প্রত্যাগম FAIL;
}

    শূন্য
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    যদি (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    শূন্য
clip_xterm_request_selection(Clipboard_T *cbd)
{
    যদি (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    শূন্য
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#যদি defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# যদি defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    স্থির শূন্য
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    পূর্ণাঙ্ক		save_cmod_flags;
    পূর্ণাঙ্ক		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    যদি (check_changed_any(FALSE, FALSE))
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
    যদি (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    স্থির শূন্য
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    পূর্ণাঙ্ক		save_type UNUSED,
    Bool	shutdown,
    পূর্ণাঙ্ক		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    যদি (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    যদি (p_verbose > 0)
	verb_বার্তা(_("XSMP handling save-yourself request"));

# যদি defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    যদি (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    অন্যথা
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
    স্থির শূন্য
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    স্থির শূন্য
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
    স্থির শূন্য
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    যদি (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    স্থির শূন্য
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    যদি (opening)
    {
	xsmp_icefd = Ice_Connection_সংখ্যা(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, রিক্ত);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    পূর্ণাঙ্ক
xsmp_handle_requests(শূন্য)
{
    Bool rep;

    যদি (IceProcessMessages(xsmp.iceconn, রিক্ত, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	যদি (p_verbose > 0)
	    verb_বার্তা(_("XSMP lost ICE connection"));
	xsmp_close();
	প্রত্যাগম FAIL;
    }
    অন্যথা
	প্রত্যাগম OK;
}

স্থির পূর্ণাঙ্ক dummy;

// Set up X Session Management Protocol
    শূন্য
xsmp_init(শূন্য)
{
    বর্ণ		errorstring[80];
    SmcCallbacks	smcallbacks;
# যদি 0
    Sm_Prop_মান		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    যদি (p_verbose > 0)
	verb_বার্তা(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = রিক্ত;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = রিক্ত;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = রিক্ত;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = রিক্ত;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    যদি (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	যদি (p_verbose > 0)
	    verb_বার্তা(_("XSMP ICE connection watch failed"));
	প্রত্যাগম;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    রিক্ত,
	    রিক্ত,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    রিক্ত,
	    &xsmp.clientid,
	    পরিমাণ(errorstring) - 1,
	    errorstring);
    যদি (xsmp.smcconn == রিক্ত)
    {
	যদি (p_verbose > 0)
	{
	    বর্ণ errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[পরিমাণ(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, পরিমাণ(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_বার্তা(errorreport);
	}
	প্রত্যাগম;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# যদি 0
    // ID ourselves
    smname.মান = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.অঙ্ক_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    শূন্য
xsmp_close(শূন্য)
{
    যদি (xsmp_icefd == -1)
	প্রত্যাগম;

    SmcCloseConnection(xsmp.smcconn, 0, রিক্ত);
    যদি (xsmp.clientid != রিক্ত)
	মুক্ত(xsmp.clientid);
    xsmp.clientid = রিক্ত;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#যদি defined(FEAT_RELTIME)
# যদি defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
স্থির চঞ্চল sig_atomic_t timeout_flag = FALSE;
স্থির timer_t		     timer_id;
স্থির পূর্ণাঙ্ক		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    স্থির শূন্য
set_flag(সঙ্ঘ sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    শূন্য
stop_timeout(শূন্য)
{
    স্থির সংরচনা itimerspec disarm = {{0, 0}, {0, 0}};

    যদি (timer_created)
    {
	পূর্ণাঙ্ক ret = timer_settime(timer_id, 0, &disarm, রিক্ত);

	যদি (ret < 0)
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
    চঞ্চল sig_atomic_t *
start_timeout(long msec)
{
    সংরচনা itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    পূর্ণাঙ্ক ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    যদি (!timer_created)
    {
	সংরচনা sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	যদি (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    প্রত্যাগম &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(রিক্ত, "setting timeout timer to %d sec %ld nsec",
	       (পূর্ণাঙ্ক)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, রিক্ত);
    যদি (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    প্রত্যাগম &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    শূন্য
delete_timer(শূন্য)
{
    যদি (!timer_created)
	প্রত্যাগম;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# অন্যথা // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
স্থির সংরচনা sigaction		prev_sigaction;
স্থির চঞ্চল sig_atomic_t	timeout_flag	     = FALSE;
স্থির পূর্ণাঙ্ক			timer_active	     = FALSE;
স্থির পূর্ণাঙ্ক			timer_handler_active = FALSE;
স্থির চঞ্চল sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    স্থির শূন্য
set_flag SIGDEFARG(sigarg)
{
    যদি (alarm_pending)
	alarm_pending = FALSE;
    অন্যথা
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    শূন্য
stop_timeout(শূন্য)
{
    স্থির সংরচনা itimerval disarm = {{0, 0}, {0, 0}};
    পূর্ণাঙ্ক			    ret;

    যদি (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, রিক্ত);
	যদি (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    যদি (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, রিক্ত);
	যদি (ret < 0)
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
    চঞ্চল sig_atomic_t *
start_timeout(long msec)
{
    সংরচনা itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    সংরচনা sigaction	handle_alarm;
    পূর্ণাঙ্ক			ret;
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
    যদি (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, রিক্ত);
    }
    যদি (unlikely(ret != 0 || alarm_pending < 0))
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
    যদি (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	প্রত্যাগম &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, রিক্ত);
    যদি (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	প্রত্যাগম &timeout_flag;
    }

    timer_active = TRUE;
    প্রত্যাগম &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
