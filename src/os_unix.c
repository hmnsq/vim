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

#شامل "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#شامل "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
ایستا صحیح selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != پوچ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != پوچ ? *dll_gpm_fd   : -1)

ایستا صحیح (*dll_Gpm_Open)     (Gpm_Connect *, صحیح);
ایستا صحیح (*dll_Gpm_Close)    (تهی);
ایستا صحیح (*dll_Gpm_GetEvent) (Gpm_Event *);
ایستا صحیح *dll_gpm_flag;
ایستا صحیح *dll_gpm_fd;

ایستا تهی *libgpm_hinst;
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

ایستا تهی gpm_close(تهی);
ایستا صحیح gpm_open(تهی);
ایستا صحیح mch_gpm_process(تهی);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ایستا صحیح sysmouse_open(تهی);
ایستا تهی sysmouse_close(تهی);
ایستا تهی sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#اگر defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#اگر defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
ایستا Widget	xterm_Shell = (Widget)0;
ایستا تهی clip_update(تهی);
# endif

Window	    x11_window = 0;
Display	    *x11_display = پوچ;
#endif

ایستا صحیح ignore_sigtstp = FALSE;

ایستا صحیح get_x11_title(صحیح);

ایستا char_u	*oldtitle = پوچ;
ایستا فرار sig_atomic_t oldtitle_outdated = FALSE;
ایستا صحیح	unix_did_set_title = FALSE;
ایستا char_u	*oldicon = پوچ;
ایستا صحیح	did_set_icon = FALSE;

ایستا تهی may_core_dump(تهی);

تعریفنوع صحیح waitstatus;
ایستا صحیح  WaitForChar(long msec, صحیح *interrupted, صحیح ignore_درون_داد);
ایستا صحیح  WaitForCharOrMouse(long msec, صحیح *interrupted, صحیح ignore_درون_داد);
#ifdef VMS
صحیح  RealWaitForChar(صحیح, long, صحیح *, صحیح *interrupted);
#وگرنه
ایستا صحیح  RealWaitForChar(صحیح, long, صحیح *, صحیح *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
ایستا صحیح do_xterm_trace(تهی);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

ایستا تهی handle_resize(تهی);

#اگر defined(SIGWINCH)
ایستا تهی sig_winch SIGPROTOARG;
#endif
#اگر defined(SIGTSTP)
ایستا تهی sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ایستا فرار sig_atomic_t in_mch_suspend = FALSE;
#endif
#اگر defined(SIGINT)
ایستا تهی catch_sigint SIGPROTOARG;
#endif
#اگر defined(SIGUSR1)
ایستا تهی catch_sigusr1 SIGPROTOARG;
#endif
#اگر defined(SIGPWR)
ایستا تهی catch_sigpwr SIGPROTOARG;
#endif
#اگر defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
ایستا تهی sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ایستا فرار sig_atomic_t sig_alarm_called;
#endif
ایستا تهی deathtrap SIGPROTOARG;

ایستا تهی catch_int_signal(تهی);
ایستا تهی set_signals(تهی);
ایستا تهی catch_signals(تهی (*func_deadly)(صحیح), تهی (*func_other)(صحیح));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#وگرنه
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	انجامبده { /**/ } وقتی (0)
# define UNBLOCK_SIGNALS(set)	انجامبده { /**/ } وقتی (0)
#endif
ایستا صحیح  have_wildcard(صحیح, char_u **);
ایستا صحیح  have_dollars(صحیح, char_u **);

ایستا صحیح save_patterns(صحیح رقم_pat, char_u **pat, صحیح *رقم_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
ایستا فرار sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ایستا فرار sig_atomic_t got_tstp = FALSE;
ایستا char_u	*extra_shell_arg = پوچ;
ایستا صحیح	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ایستا فرار sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ایستا فرار sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#اگر defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
ایستا صحیح dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ایستا tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
تعریفنوع ساختار
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    نویسه *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ایستا xsmp_config_T xsmp;
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

ایستا ساختار signalinfo
{
    صحیح	    sig;	// Signal number, eg. SIGSEGV etc
    نویسه    *name;	// Signal name (not char_u!).
    نویسه    deadly;	// Catch as a deadly signal?
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
#اگر defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#اگر defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#اگر defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#اگر defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(صحیح sig, sighandler_T func)
{
#اگر defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ساختار sigaction	sa, old;
    sigset_t		curset;
    صحیح			blocked;

    اگر (sigprocmask(SIG_BLOCK, پوچ, &curset) == -1)
	بازگشت SIG_ERR;

    blocked = sigismember(&curset, sig);

    اگر (func == SIG_HOLD)
    {
	اگر (blocked)
	    بازگشت SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	اگر (sigaction(sig, پوچ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, پوچ) == -1)
	    بازگشت SIG_ERR;
	بازگشت old.sa_handler;
    }

    اگر (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	اگر (sigprocmask(SIG_UNBLOCK, &curset, پوچ) == -1)
	    بازگشت SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# وگرنه
    sa.sa_flags = 0;
# endif
    اگر (sigaction(sig, &sa, &old) == -1)
	بازگشت SIG_ERR;
    بازگشت blocked ? SIG_HOLD: old.sa_handler;
#وگرنهاگر defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    بازگشت sigset(sig, func);
#وگرنه
    // Oldest and most compatible solution.
    بازگشت سیگنال(sig, func);
#endif
}

    صحیح
mch_chdir(نویسه *path)
{
    اگر (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    بازگشت chdir(vms_fixfilename(path));
#وگرنه
    بازگشت chdir(path);
#endif
}

#اگر defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    تهی
mch_write(char_u *s, صحیح len)
{
    vim_ignored = (صحیح)اكتب(1, (نویسه *)s, len);
    اگر (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, پوچ, پوچ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ایستا صحیح
resize_func(صحیح check_only)
{
    اگر (check_only)
	بازگشت do_resize;
    وقتی (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(پوچ, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    بازگشت FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    صحیح
mch_inchar(
    char_u	*buf,
    صحیح		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt)
{
    بازگشت inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ایستا تهی
handle_resize(تهی)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    صحیح
mch_char_avail(تهی)
{
    بازگشت WaitForChar(0L, پوچ, FALSE);
}

#اگر defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحیح
mch_check_messages(تهی)
{
    بازگشت WaitForChar(0L, پوچ, TRUE);
}
#endif

#اگر defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# اگر defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# اگر defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(صحیح special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ساختار vm_statistics64	vm_stat;
	natural_t		شمار = HOST_VM_INFO64_شمار;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &شمار);
#  وگرنه
	ساختار vm_statistics	vm_stat;
	natural_t		شمار = HOST_VM_INFO_شمار;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &شمار);
#  endif
	اگر (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_شمار + vm_stat.active_شمار
					    + vm_stat.inactive_شمار
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_شمار
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    اگر (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	صحیح		mib[2];
	size_t		len = اندازه(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  وگرنه
	// sysctl() may return 32 bit or 64 bit, accept both
	اجتماع {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  وگرنه
	mib[1] = HW_USERMEM;
#  endif
	اگر (sysctl(mib, 2, &physmem, &len, پوچ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  وگرنه
	    اگر (len == اندازه(physmem.u64))
		mem = (long_u)physmem.u64;
	    وگرنه
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# اگر defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    اگر (mem == 0)
    {
	ساختار sysinfo sinfo;

	// Linux way of getting amount of RAM available
	اگر (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    وقتی (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  وگرنه
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    اگر (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	اگر (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    وقتی (shiftright > 0 && (pagesize & 1) == 0)
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
# اگر defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ساختار rlimit	rlp;

	اگر (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (اندازه(long_u) * 8 - 1))
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

    اگر (mem > 0)
	بازگشت mem >> shiftright;
    بازگشت (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    تهی
mch_delay(long msec, صحیح flags)
{
    tmode_T	old_tmode;
    صحیح		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    اگر (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	اگر (call_settmode)
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
	انجامبده
	{
	    // if total is large enough, wait by portions in p_mzq
	    اگر (total > p_mzq)
		msec = p_mzq;
	    وگرنه
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    ساختار timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (تهی)nanosleep(&ts, پوچ);
	}
#وگرنه
# ifdef HAVE_USLEEP
	وقتی (msec >= 1000)
	{
	    usleep((unsigned صحیح)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned صحیح)(msec * 1000));
# وگرنه
#  ifndef HAVE_SELECT
	poll(پوچ, 0, (صحیح)msec);
#  وگرنه
	{
	    ساختار timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, پوچ, پوچ, پوچ, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	وقتی (total > 0);
#endif

	اگر (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    وگرنه
	WaitForChar(msec, پوچ, FALSE);
}

#اگر defined(HAVE_پشته_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_پشته_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ایستا صحیح پشته_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ایستا تهی
check_پشته_growth(نویسه *p)
{
    صحیح		i;

    پشته_grows_downwards = (p > (نویسه *)&i);
}
#endif

#اگر defined(HAVE_پشته_LIMIT)
ایستا نویسه *پشته_limit = پوچ;

# اگر defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ایستا تهی
get_پشته_limit(تهی)
{
    ساختار rlimit	rlp;
    صحیح			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    اگر (getrlimit(RLIMIT_پشته, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (اندازه(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# اگر defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    اگر (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		اگر (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	اگر (پشته_grows_downwards)
	{
	    پشته_limit = (نویسه *)((long)&i - (lim / 16L * 15L));
	    اگر (پشته_limit >= (نویسه *)&i)
		// overflow, set to 1/16 of current stack position
		پشته_limit = (نویسه *)((long)&i / 16L);
	}
	وگرنه
	{
	    پشته_limit = (نویسه *)((long)&i + (lim / 16L * 15L));
	    اگر (پشته_limit <= (نویسه *)&i)
		پشته_limit = پوچ;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    صحیح
mch_stackcheck(نویسه *p)
{
    اگر (پشته_limit == پوچ)
	بازگشت OK;

    اگر (پشته_grows_downwards)
    {
	اگر (p < پشته_limit)
	    بازگشت FAIL;
    }
    وگرنه اگر (p > پشته_limit)
	بازگشت FAIL;
    بازگشت OK;
}
#endif

#اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ایستا stack_t sigstk;			// for sigaltstack()
# وگرنه
ایستا ساختار sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ایستا long صحیح get_signal_پشته_size(تهی)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long صحیح size = -1;

    // return size only if sysconf doesn't return an error
    اگر ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	بازگشت size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    بازگشت SIGSTKSZ;
# endif

    // otherwise guess the size
    بازگشت 8000;
}

ایستا نویسه *signal_پشته;

    ایستا تهی
init_signal_پشته(تهی)
{
    اگر (signal_پشته == پوچ)
	بازگشت;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_پشته;
#  وگرنه
    sigstk.ss_sp = signal_پشته;
#  endif
    sigstk.ss_size = get_signal_پشته_size();
    sigstk.ss_flags = 0;
    (تهی)sigaltstack(&sigstk, پوچ);
# وگرنه
    sigstk.ss_sp = signal_پشته;
    اگر (پشته_grows_downwards)
	sigstk.ss_sp += get_signal_پشته_size() - 1;
    sigstk.ss_onstack = 0;
    (تهی)sigstack(&sigstk, پوچ);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#اگر defined(SIGWINCH)
    ایستا تهی
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#اگر defined(SIGTSTP)
    ایستا تهی
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    اگر (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	بالاببر(sigarg);
    }
    وگرنه
	got_tstp = TRUE;

# اگر !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#اگر defined(SIGINT)
    ایستا تهی
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#اگر defined(SIGUSR1)
    ایستا تهی
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#اگر defined(SIGPWR)
    ایستا تهی
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
    ایستا تهی
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#اگر defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ایستا JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ایستا فرار sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ایستا فرار sig_atomic_t lc_active = FALSE;

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
    ایستا تهی
mch_startjmp(تهی)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    ایستا تهی
mch_endjmp(تهی)
{
    lc_active = FALSE;
}

    ایستا تهی
mch_didjmp(تهی)
{
# اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_پشته();
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
    ایستا تهی
deathtrap SIGDEFARG(sigarg)
{
    ایستا صحیح	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    صحیح		i;
#endif

#اگر defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    اگر (lc_active)
    {
# اگر defined(SIGHASARG)
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
    اگر (in_mch_delay && sigarg == SIGQUIT)
	بازگشت;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    اگر (entered == 0
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
	بازگشت;
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

#ifdef HAVE_پشته_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_پشته_limit();
#endif

#اگر 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	اگر (fp)
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
    برای (i = 0; signal_info[i].sig != -1; i++)
	اگر (sigarg == signal_info[i].sig)
	    بشکن;
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
    اگر (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	اگر (entered >= 4)
	    _exit(8);
	exit(7);
    }
    اگر (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((نویسه *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#وگرنه
    sprintf((نویسه *)IObuff, "Vim: Caught deadly signal\r\n");
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
    ایستا تهی
after_sigcont(تهی)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#اگر defined(SIGCONT)
ایستا تهی sigcont_handler SIGPROTOARG;

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
ایستا فرار sig_atomic_t sigcont_received;
ایستا تهی sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ایستا تهی
sigcont_handler SIGDEFARG(sigarg)
{
    اگر (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    وگرنه
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

#اگر defined(FEAT_CLIPBOARD)
# اگر defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ایستا تهی *clip_star_save = پوچ;
ایستا تهی *clip_plus_save = پوچ;
# endif

# اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ایستا تهی
loose_clipboard(تهی)
{
    اگر (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	اگر (clip_star.owned)
	    clip_lose_selection(&clip_star);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	اگر (x11_display != پوچ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# اگر defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ایستا تهی
save_clipboard(تهی)
{
    اگر (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    اگر (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ایستا تهی
restore_clipboard(تهی)
{
    اگر (clip_star_save != پوچ)
    {
	اگر (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	وگرنه
	    free_register(clip_star_save);
	clip_star_save = پوچ;
    }
    اگر (clip_plus_save != پوچ)
    {
	اگر (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	وگرنه
	    free_register(clip_plus_save);
	clip_plus_save = پوچ;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    تهی
mch_suspend(تهی)
{
    اگر (ignore_sigtstp)
	بازگشت;

#اگر defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# اگر defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# اگر defined(SIGCONT)
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

	برای (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#وگرنه
    suspend_shell();
#endif
}

    تهی
mch_init(تهی)
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

    تهی
set_sigwinch_handler(تهی)
{
#اگر defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    ایستا تهی
set_signals(تهی)
{
#اگر defined(SIGWINCH)
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
#اگر defined(SIGCONT)
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

#اگر defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    اگر (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#اگر defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ایستا تهی
catch_int_signal(تهی)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    تهی
reset_signals(تهی)
{
    catch_signals(SIG_DFL, SIG_DFL);
#اگر defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    ایستا تهی
catch_signals(
    تهی (*func_deadly)(صحیح),
    تهی (*func_other)(صحیح))
{
    صحیح	    i;

    برای (i = 0; signal_info[i].sig != -1; i++)
    {
	اگر (signal_info[i].deadly)
	{
#اگر defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ساختار sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# اگر defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# وگرنه
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, پوچ);
#وگرنه
# اگر defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ساختار sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, پوچ);
# وگرنه
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	وگرنه اگر (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#وگرنه
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    ایستا تهی
block_signals(sigset_t *set)
{
    sigset_t	newset;
    صحیح		i;

    sigemptyset(&newset);

    برای (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# اگر defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    ایستا تهی
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, پوچ);
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
    صحیح
vim_handle_signal(صحیح sig)
{
    ایستا صحیح got_signal = 0;
    ایستا صحیح blocked = TRUE;

    انتخاب (sig)
    {
	حالت SIGNAL_BLOCK:   blocked = TRUE;
			     بشکن;

	حالت SIGNAL_UNBLOCK: blocked = FALSE;
			     اگر (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     بشکن;

	پیشفرض:	     اگر (!blocked)
				 بازگشت TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     اگر (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     بشکن;
    }
    بازگشت FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    صحیح
mch_check_win(صحیح argc UNUSED, نویسه **argv UNUSED)
{
    اگر (isatty(1))
	بازگشت OK;
    بازگشت FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحیح
mch_درون_داد_isatty(تهی)
{
    اگر (isatty(read_cmd_fd))
	بازگشت TRUE;
    بازگشت FALSE;
}

#ifdef FEAT_X11

# اگر defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ایستا تهی
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#اگر defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ایستا صحیح	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ایستا صحیح
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (نویسه *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    اگر (!gui.in_use)
# endif
	پیام((نویسه *)IObuff);

    بازگشت 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ایستا صحیح
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    بازگشت 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ایستا صحیح
x_connect_to_server(تهی)
{
    // No point in connecting if we are exiting or dying.
    اگر (exiting || v_dying)
	بازگشت FALSE;

# اگر defined(FEAT_CLIENTSERVER)
    اگر (x_force_connect)
	بازگشت TRUE;
# endif
    اگر (x_no_connect)
	بازگشت FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    اگر (clip_exclude_prog != پوچ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	اگر (regprog_in_use(clip_exclude_prog))
	    بازگشت FALSE;

	اگر (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    بازگشت FALSE;
    }
    بازگشت TRUE;
}

# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  اگر defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ایستا صحیح
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   اگر defined(VMS) || defined(__CYGWIN__)
    بازگشت 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ایستا صحیح xterm_dpy_retry_شمار = 0;

    ایستا صحیح
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = پوچ;
    xterm_dpy_retry_شمار = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = پوچ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  اگر defined(VMS) || defined(__CYGWIN__)
    بازگشت 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ایستا تهی
may_restore_x11_clipboard(تهی)
{
    // No point in restoring the connecting if we are exiting or dying.
    اگر (!exiting && !v_dying && xterm_dpy_retry_شمار > 0)
    {
	--xterm_dpy_retry_شمار;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	اگر (app_context != (XtAppContext)پوچ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)پوچ;
	    x11_display = پوچ; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    تهی
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    اگر (eap->arg != پوچ && (arglen = STRLEN(eap->arg)) > 0)
    {
	اگر (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (نویسه *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == پوچ
		    ? (نویسه *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_شمار = 5;  // Try reconnecting five times
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
    ایستا صحیح
test_x11_window(Display *dpy)
{
    صحیح			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    اگر (XGetWMName(dpy, x11_window, &text_prop))
	XFree((تهی *)text_prop.مقدار);
    XSync(dpy, False);
    (تهی)XSetErrorHandler(old_handler);

    اگر (p_verbose > 0 && got_x_error)
	verb_پیام(_("Testing the X display failed"));

    بازگشت (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

ایستا صحیح get_x11_thing(صحیح get_title, صحیح test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ایستا صحیح
get_x11_windis(تهی)
{
    نویسه	    *winid;
    ایستا صحیح	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ایستا صحیح	    x11_display_from = XD_NONE;
    ایستا صحیح	    did_set_error_handler = FALSE;

    اگر (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(تهی)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    اگر (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	اگر (x11_display_from == XD_HERE && x11_display != پوچ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	اگر (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    بازگشت OK;
	}
	x11_display = پوچ;
	بازگشت FAIL;
    }
    وگرنه اگر (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = پوچ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    اگر (!x_connect_to_server())
	بازگشت FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    اگر (x11_window == 0 && (winid = getenv("WINDOWID")) != پوچ)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    اگر (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    اگر (xterm_dpy != پوچ && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	اگر (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    اگر (x11_display_from == XD_HERE && x11_display != پوچ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	اگر (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = پوچ;
	    x11_display_from = XD_NONE;
	    بازگشت FAIL;
	}
	بازگشت OK;
    }
# endif

    اگر (x11_window == 0 || x11_display == پوچ)
	result = -1;

    اگر (result != -1)	    // Have already been here and set this
	بازگشت result;	    // Don't do all these X calls again

    اگر (x11_window != 0 && x11_display == پوچ)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	اگر (p_verbose > 0)
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
	x11_display = XOpenDisplay(پوچ);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	اگر (p_verbose > 0 && sig_alarm_called)
	    verb_پیام(_("Opening the X display timed out"));
# endif
	اگر (x11_display != پوچ)
	{
# ifdef ELAPSED_FUNC
	    اگر (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    اگر (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = پوچ;
	    }
	    وگرنه
		x11_display_from = XD_HERE;
	}
    }
    اگر (x11_window == 0 || x11_display == پوچ)
	بازگشت (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    بازگشت (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    ایستا صحیح
get_x11_title(صحیح test_only)
{
    بازگشت get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ایستا صحیح
get_x11_icon(صحیح test_only)
{
    صحیح		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    اگر (oldicon == پوچ && !test_only)
    {
	اگر (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	وگرنه
	    oldicon = vim_strsave(T_NAME);
    }

    بازگشت retval;
}

    ایستا صحیح
get_x11_thing(
    صحیح		get_title,	// get title string
    صحیح		test_only)
{
    XTextProperty	text_prop;
    صحیح			retval = FALSE;
    Status		status;

    اگر (get_x11_windis() != OK)
	بازگشت FALSE;

    // Get window/icon name if any
    اگر (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    وگرنه
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
    Window	    ریشه;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned صحیح    رقم_children;

    وقتی (!status || text_prop.مقدار == پوچ)
    {
	اگر (!XQuery_درخت(x11_display, win, &ریشه, &parent, &children,
		    &رقم_children))
	    بشکن;
	اگر (children)
	    XFree((تهی *)children);
	اگر (parent == ریشه || parent == 0)
	    بشکن;

	win = parent;
	اگر (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	وگرنه
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    اگر (status && text_prop.مقدار != پوچ)
    {
	retval = TRUE;
	اگر (!test_only)
	{
	    اگر (get_title)
		vim_free(oldtitle);
	    وگرنه
		vim_free(oldicon);
	    اگر (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		اگر (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.مقدار);
		وگرنه
		    oldicon = vim_strsave((char_u *)text_prop.مقدار);
	    }
	    وگرنه
	    {
		نویسه    **cl;
		Status  transform_status;
		صحیح	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		اگر (transform_status >= Success && n > 0 && cl[0])
		{
		    اگر (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    وگرنه
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		وگرنه
		{
		    اگر (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.مقدار);
		    وگرنه
			oldicon = vim_strsave((char_u *)text_prop.مقدار);
		}
	    }
	}
	XFree((تهی *)text_prop.مقدار);
    }
    بازگشت retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# اگر defined(X_HAVE_UTF8_STRING)
#  اگر X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ایستا تهی
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    اگر (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ثابت نویسه *)title,
					     پوچ, پوچ, 0, پوچ, پوچ, پوچ);
    وگرنه
# endif
    {
# اگر XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ثابت نویسه *)title,
					     پوچ, پوچ, 0, پوچ, پوچ, پوچ);
#  وگرنه
	XTextProperty	text_prop;
	نویسه		*c_title = (نویسه *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(تهی)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     پوچ, پوچ, 0, پوچ, پوچ, پوچ);
#  endif
# وگرنه
	XStoreName(x11_display, x11_window, (نویسه *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ایستا تهی
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    اگر (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, پوچ, (ثابت نویسه *)icon,
						   پوچ, 0, پوچ, پوچ, پوچ);
    وگرنه
# endif
    {
# اگر XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, پوچ, (ثابت نویسه *)icon,
						   پوچ, 0, پوچ, پوچ, پوچ);
#  وگرنه
	XTextProperty	text_prop;
	نویسه		*c_icon = (نویسه *)icon;

	(تهی)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, پوچ, &text_prop,
						   پوچ, 0, پوچ, پوچ, پوچ);
#  endif
# وگرنه
	XSetIconName(x11_display, x11_window, (نویسه *)icon);
# endif
    }
    XFlush(x11_display);
}

#وگرنه  // FEAT_X11

    ایستا صحیح
get_x11_title(صحیح test_only UNUSED)
{
    بازگشت FALSE;
}

    ایستا صحیح
get_x11_icon(صحیح test_only)
{
    اگر (!test_only)
    {
	اگر (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	وگرنه
	    oldicon = vim_strsave(T_NAME);
    }
    بازگشت FALSE;
}

#endif // FEAT_X11

    صحیح
mch_can_restore_title(تهی)
{
    بازگشت get_x11_title(TRUE);
}

    صحیح
mch_can_restore_icon(تهی)
{
    بازگشت get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    تهی
mch_settitle(char_u *title, char_u *icon)
{
    صحیح		type = 0;
    ایستا صحیح	recursive = 0;

    اگر (T_NAME == پوچ)	    // no terminal name (yet)
	بازگشت;
    اگر (title == پوچ && icon == پوچ)	    // nothing to do
	بازگشت;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    اگر (recursive)
	بازگشت;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    اگر (get_x11_windis() == OK)
	type = 1;
#endif
#اگر defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    اگر (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    اگر ((type || *T_TS != NUL) && title != پوچ)
    {
	اگر (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	اگر (oldtitle == پوچ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (تهی)get_x11_title(FALSE);

	اگر (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	وگرنه
# ifdef FEAT_GUI_GTK
	اگر (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	وگرنه
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    اگر ((type || *T_CIS != NUL) && icon != پوچ)
    {
	اگر (oldicon == پوچ
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	اگر (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	وگرنه
# ifdef FEAT_GUI_GTK
	اگر (!gui.in_use)		// don't do this if GTK+ is running
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
    تهی
mch_restore_title(صحیح which)
{
    صحیح	do_افزودن_برداشت = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : پوچ;
    char_u *tofree = پوچ;
    اگر (title == oldtitle && oldtitle != پوچ)
    {
	tofree = vim_strsave(title);
	اگر (tofree != پوچ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : پوچ);
    vim_free(tofree);

    اگر (do_افزودن_برداشت)
    {
	// pop and push from/to the stack
	term_برداشت_title(which);
	term_افزودن_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    صحیح
vim_is_xterm(char_u *name)
{
    اگر (name == پوچ)
	بازگشت FALSE;
    بازگشت ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#اگر defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    صحیح
use_xterm_like_mouse(char_u *name)
{
    بازگشت (name != پوچ
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
    صحیح
use_xterm_mouse(تهی)
{
    اگر (ttym_flags == TTYM_SGR)
	بازگشت 4;
    اگر (ttym_flags == TTYM_URXVT)
	بازگشت 3;
    اگر (ttym_flags == TTYM_XTERM2)
	بازگشت 2;
    اگر (ttym_flags == TTYM_XTERM)
	بازگشت 1;
    بازگشت 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    صحیح
vim_is_iris(char_u *name)
{
    اگر (name == پوچ)
	بازگشت FALSE;
    بازگشت (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#اگر defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    صحیح
vim_is_vt300(char_u *name)
{
    اگر (name == پوچ)
	بازگشت FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    بازگشت ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != پوچ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    صحیح
mch_get_user_name(char_u *s, صحیح len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(پوچ), len - 1);
    بازگشت OK;
#وگرنه
    بازگشت mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    صحیح
mch_get_uname(uid_t uid, char_u *s, صحیح len)
{
#اگر defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ساختار passwd   *pw;

    اگر ((pw = getpwuid(uid)) != پوچ
	    && pw->pw_name != پوچ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	بازگشت OK;
    }
#endif
    sprintf((نویسه *)s, "%d", (صحیح)uid);	    // assumes s is long enough
    بازگشت FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    تهی
mch_get_host_name(char_u *s, صحیح len)
{
    ساختار utsname vutsname;

    اگر (uname(&vutsname) < 0)
	*s = NUL;
    وگرنه
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#وگرنه // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    تهی
mch_get_host_name(char_u *s, صحیح len)
{
# ifdef VAXC
    vaxc$gethostname((نویسه *)s, len);
# وگرنه
    gethostname((نویسه *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(تهی)
{
    بازگشت (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحیح
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    اگر (kill(pid, 0) == 0)
	بازگشت TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    اگر (errno == ESRCH)
	بازگشت FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    بازگشت TRUE;
}

#اگر !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ایستا نویسه *
strerror(صحیح err)
{
    بیرونی صحیح	    sys_nerr;
    بیرونی نویسه	    *sys_errlist[];
    ایستا نویسه	    er[20];

    اگر (err > 0 && err < sys_nerr)
	بازگشت (sys_errlist[err]);
    snprintf(er, اندازه(er), "Error %d", err);
    بازگشت er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    صحیح
mch_dirname(char_u *buf, صحیح len)
{
#اگر defined(USE_GETCWD)
    اگر (getcwd((نویسه *)buf, len) == پوچ)
    {
	STRCPY(buf, strerror(errno));
	بازگشت FAIL;
    }
    بازگشت OK;
#وگرنه
    بازگشت (getwd((نویسه *)buf) != پوچ ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    صحیح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)		// also expand when already absolute path
{
    صحیح		buflen = 0;
#ifdef HAVE_FCHDIR
    صحیح		fd = -1;
    ایستا صحیح	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    صحیح		retval = OK;
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
# اگر CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# وگرنه
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    اگر ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == پوچ || p != fname))
    {
	اگر (p == پوچ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	اگر (p != پوچ)
	{
	    اگر (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    اگر (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		اگر (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    اگر (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((نویسه *)olddir) != 0))
	    {
		p = پوچ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    وگرنه
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		اگر (p - fname >= len)
		    retval = FAIL;
		وگرنه
		{
		    vim_strncpy(buf, fname, p - fname);
		    اگر (mch_chdir((نویسه *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			اگر (mch_isFullName(fname))
			    retval = FAIL;
			وگرنه
			    p = پوچ;
		    }
		    وگرنه اگر (*p == '/')
			fname = p + 1;
		    وگرنه
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	اگر (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	اگر (p != پوچ)
	{
	    صحیح	l;

#ifdef HAVE_FCHDIR
	    اگر (fd >= 0)
	    {
		اگر (p_verbose >= 5)
		{
		    verbose_enter();
		    پیام("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    وگرنه
#endif
		l = mch_chdir((نویسه *)olddir);
	    اگر (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	اگر (fd >= 0)
	    close(fd);
#endif

	buflen = (صحیح)STRLEN(buf);
	اگر (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	وگرنه اگر (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += اندازه(PATHSEP);
	}
#endif
    }

    اگر (buflen == 0)
	buflen = (صحیح)STRLEN(buf);

    // Catch file names which are too long.
    اگر (retval == FAIL || (صحیح)(buflen + STRLEN(fname)) >= len)
	بازگشت FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    اگر (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    بازگشت OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_isFullName(char_u *fname)
{
#ifdef VMS
    بازگشت ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((نویسه *)fname,':') || strchr((نویسه *)fname,'"') ||
	    (strchr((نویسه *)fname,'[') && strchr((نویسه *)fname,']'))||
	    (strchr((نویسه *)fname,'<') && strchr((نویسه *)fname,'>'))   );
#وگرنه
    بازگشت (*fname == '/' || *fname == '~');
#endif
}

#اگر defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    تهی
fname_case(
    char_u	*name,
    صحیح		len UNUSED)  // buffer size, only used when name gets longer
{
    ساختار stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    ساختار dirent *dp;

    اگر (mch_lstat((نویسه *)name, &st) < 0)
	بازگشت;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    اگر (slash == پوچ)
    {
	dirp = opendir(".");
	tail = name;
    }
    وگرنه
    {
	*slash = NUL;
	dirp = opendir((نویسه *)name);
	*slash = '/';
	tail = slash + 1;
    }

    اگر (dirp == پوچ)
	بازگشت;

    taillen = STRLEN(tail);
    وقتی ((dp = readdir(dirp)) != پوچ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	اگر (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ساختار stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    اگر (mch_lstat((نویسه *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		بشکن;
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
    ساختار stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    اگر (stat((نویسه *)vms_fixfilename(name), &statb))
#وگرنه
    اگر (stat((نویسه *)name, &statb))
#endif
	بازگشت -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    بازگشت statb.st_mode & ~S_ADDACE;
#وگرنه
    بازگشت statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
mch_setperm(char_u *name, long perm)
{
    بازگشت (chmod((نویسه *)
#ifdef VMS
		    vms_fixfilename(name),
#وگرنه
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#اگر defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
mch_fsetperm(صحیح fd, long perm)
{
    بازگشت (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#اگر defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
تعریفنوع ساختار vim_acl_solaris_T {
    صحیح acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# اگر defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    تهی
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    اگر (from_file == پوچ)
	بازگشت;

    اگر (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    اگر (selinux_enabled <= 0)
	بازگشت;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    نویسه *from_context = پوچ;
    نویسه *to_context = پوچ;

    اگر (getfilecon((نویسه *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	اگر (errno == EOPNOTSUPP)
	    بازگشت;

	پیام_puts(_("\nCould not get security context for "));
	پیام_outtrans(from_file);
	پیام_putchar('\n');
	بازگشت;
    }
    اگر (getfilecon((نویسه *)to_file, &to_context) < 0)
    {
	پیام_puts(_("\nCould not get security context for "));
	پیام_outtrans(to_file);
	پیام_putchar('\n');
	freecon (from_context);
	بازگشت ;
    }
    اگر (strcmp(from_context, to_context) != 0)
    {
	اگر (setfilecon((نویسه *)to_file, from_context) < 0)
	{
	    پیام_puts(_("\nCould not set security context for "));
	    پیام_outtrans(to_file);
	    پیام_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# اگر defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    تهی
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ایستا ثابت نویسه * ثابت smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    نویسه	buffer[SMACK_LABEL_LEN];
    ثابت نویسه	*name;
    صحیح		index;
    صحیح		ret;
    ssize_t	size;

    اگر (from_file == پوچ)
	بازگشت;

    size = listxattr((نویسه *)from_file, پوچ, 0);
    // not supported or no attributes to copy
    اگر (size <= 0)
	بازگشت;

    برای (index = 0 ; index < (صحیح)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((نویسه*)from_file, name, buffer, اندازه(buffer));
	اگر (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((نویسه*)to_file, name, buffer, (size_t)size, 0);
	    اگر (ret < 0)
	    {
		vim_snprintf((نویسه *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		پیام_outtrans(IObuff);
		پیام_putchar('\n');
	    }
	}
	وگرنه
	{
	    // what reason of not having the attribute value?
	    انتخاب (errno)
	    {
		حالت ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    بازگشت; // leave because it isn't useful to continue

		حالت ERANGE:
		پیشفرض:
		    // no enough size OR unexpected error
		     vim_snprintf((نویسه *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    پیام_puts((نویسه *)IObuff);
		    پیام_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		حالت ENODATA:
		    // no attribute of this name
		    ret = removexattr((نویسه*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    بشکن;
	    }
	}
    }
}
# endif // HAVE_SMACK

# اگر defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    تهی
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    نویسه	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    نویسه	*کلید;
    نویسه	*val = پوچ;
    نویسه	*errmsg = پوچ;

    اگر (from_file == پوچ)
	بازگشت;

    // get the length of the extended attributes
    size = listxattr((نویسه *)from_file, پوچ, 0);
    // not supported or no attributes to copy
    اگر (size <= 0)
	بازگشت;
    xattr_buf = (نویسه*)alloc(size);
    اگر (xattr_buf == پوچ)
	بازگشت;
    size = listxattr((نویسه *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    برای (صحیح round = 0; round < 2; round++)
    {

	کلید = xattr_buf;
	اگر (round == 1)
	    size = tsize;

	وقتی (size > 0)
	{
	    vallen = getxattr((نویسه *)from_file, کلید,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    اگر (vallen >= 0 && round &&
		setxattr((نویسه *)to_file, کلید, val, vallen, 0) == 0)
		;
	    وگرنه اگر (errno)
	    {
		انتخاب (errno)
		{
		    حالت E2BIG:
			errmsg = e_xattr_e2big;
			برو error_exit;
		    حالت ENOTSUP:
		    حالت EACCES:
		    حالت EPERM:
			بشکن;
		    حالت ERANGE:
			errmsg = e_xattr_erange;
			برو error_exit;
		    پیشفرض:
			errmsg = e_xattr_other;
			برو error_exit;
		}
	    }

	    اگر (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(کلید) + 1;
	    size -= keylen;
	    کلید += keylen;
	}
	اگر (round)
	    بشکن;

	val = (نویسه*)alloc(max_vallen + 1);
	اگر (val == پوچ)
	    برو error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    اگر (errmsg != پوچ)
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
    vim_acl_T	ret = پوچ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((نویسه *)fname, ACL_TYPE_ACCESS);
# وگرنه
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    اگر (acl_get((نویسه *)fname, 0, &aclent) < 0)
	بازگشت پوچ;
    ret = (vim_acl_T)aclent;
#  وگرنه
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(اندازه(vim_acl_solaris_T));
    اگر (aclent == پوچ)
	بازگشت پوچ;
    اگر ((aclent->acl_cnt = acl((نویسه *)fname, GETACLCNT, 0, پوچ)) < 0)
    {
	آزاد(aclent);
	بازگشت پوچ;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * اندازه(aclent_t));
    اگر (aclent->acl_entry == پوچ)
    {
	آزاد(aclent);
	بازگشت پوچ;
    }
    اگر (acl((نویسه *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	آزاد(aclent->acl_entry);
	آزاد(aclent);
	بازگشت پوچ;
    }
    ret = (vim_acl_T)aclent;
#   وگرنه
#    اگر defined(HAVE_AIX_ACL)
    صحیح		aclsize;
    ساختار acl *aclent;

    aclsize = اندازه(ساختار acl);
    aclent = malloc(aclsize);
    اگر (aclent == پوچ)
	بازگشت پوچ;
    اگر (statacl((نویسه *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	اگر (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    اگر (aclent == پوچ || statacl((نویسه *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		آزاد(aclent);
		بازگشت پوچ;
	    }
	}
	وگرنه
	{
	    آزاد(aclent);
	    بازگشت پوچ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    بازگشت ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    تهی
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    اگر (aclent == پوچ)
	بازگشت;
# ifdef HAVE_POSIX_ACL
    acl_set_file((نویسه *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# وگرنه
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((نویسه *)fname, (acl_t *)aclent);
#  وگرنه
#   ifdef HAVE_SOLARIS_ACL
    acl((نویسه *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   وگرنه
#    ifdef HAVE_AIX_ACL
    chacl((نویسه *)fname, aclent, ((ساختار acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    تهی
mch_free_acl(vim_acl_T aclent)
{
    اگر (aclent == پوچ)
	بازگشت;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# وگرنه
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  وگرنه
#   ifdef HAVE_SOLARIS_ACL
    آزاد(((vim_acl_solaris_T *)aclent)->acl_entry);
    آزاد(aclent);
#   وگرنه
#    ifdef HAVE_AIX_ACL
    آزاد(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    تهی
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحیح
mch_isdir(char_u *name)
{
    ساختار stat statb;

    اگر (*name == NUL)	    // Some stat()s don't flag "" as an error.
	بازگشت FALSE;
    اگر (stat((نویسه *)name, &statb))
	بازگشت FALSE;
    بازگشت (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحیح
mch_isrealdir(char_u *name)
{
    ساختار stat statb;

    اگر (*name == NUL)	    // Some stat()s don't flag "" as an error.
	بازگشت FALSE;
    اگر (mch_lstat((نویسه *)name, &statb))
	بازگشت FALSE;
    بازگشت (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ایستا صحیح
executable_file(char_u *name)
{
    ساختار stat	st;

    اگر (stat((نویسه *)name, &st))
	بازگشت 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    صحیح vms_executable = 0;
    اگر (S_ISREG(st.st_mode) && mch_access((نویسه *)name, X_OK) == 0)
    {
	اگر (strstr(vms_tolower((نویسه*)name),".exe") != پوچ
		|| strstr(vms_tolower((نویسه*)name),".com")!= پوچ)
	    vms_executable = 1;
    }
    بازگشت vms_executable;
#وگرنه
    بازگشت S_ISREG(st.st_mode) && mch_access((نویسه *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحیح
mch_can_exe(char_u *name, char_u **path, صحیح use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    صحیح		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    اگر (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	اگر ((use_path || gettail(name) != name) && executable_file(name))
	{
	    اگر (path != پوچ)
	    {
		اگر (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		وگرنه
		    *path = vim_strsave(name);
	    }
	    بازگشت TRUE;
	}
	بازگشت FALSE;
    }

    p = (char_u *)getenv("PATH");
    اگر (p == پوچ || *p == NUL)
	بازگشت -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    اگر (buf == پوچ)
	بازگشت -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    برای (;;)
    {
	e = (char_u *)strchr((نویسه *)p, ':');
	اگر (e == پوچ)
	    e = p_end;
	elen = (size_t)(e - p);
	اگر (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((نویسه *)buf, bufsize, "%.*s%s%s",
		(صحیح)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	اگر (retval == 1)
	{
	    اگر (path != پوچ)
	    {
		اگر (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		وگرنه
		    *path = vim_strnsave(buf, buflen);
	    }
	    بشکن;
	}

	اگر (*e != ':')
	    بشکن;
	p = e + 1;
    }

    vim_free(buf);
    بازگشت retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحیح
mch_nodetype(char_u *name)
{
    ساختار stat	st;

    اگر (stat((نویسه *)name, &st))
	بازگشت گره_NORMAL;
    اگر (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	بازگشت گره_NORMAL;
    اگر (S_ISBLK(st.st_mode))	// block device isn't writable
	بازگشت گره_OTHER;
    // Everything else is writable?
    بازگشت گره_WRITABLE;
}

    تهی
mch_early_init(تهی)
{
#ifdef HAVE_CHECK_پشته_GROWTH
    صحیح			i;

    check_پشته_growth((نویسه *)&i);

# ifdef HAVE_پشته_LIMIT
    get_پشته_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_پشته = alloc(get_signal_پشته_size());
    init_signal_پشته();
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

#اگر defined(EXITFREE)
    تهی
mch_free_mem(تهی)
{
# اگر defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    اگر (clip_star.owned)
	clip_lose_selection(&clip_star);
    اگر (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    اگر (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    اگر (xterm_dpy != پوچ)
	XtCloseDisplay(xterm_dpy);
    اگر (app_context != (XtAppContext)پوچ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = پوچ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# اگر defined(FEAT_X11)
    اگر (x11_display != پوچ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_پشته);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ایستا تهی
exit_scroll(تهی)
{
    اگر (silent_mode)
	بازگشت;
    اگر (newline_on_exit || پیام_didout)
    {
	اگر (پیام_use_printf())
	{
	    اگر (info_message)
		mch_پیام("\n");
	    وگرنه
		mch_errmsg("\r\n");
	}
	وگرنه
	    out_char('\n');
    }
    وگرنه اگر (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	پیام_clr_eos_force();		// clear the rest of the display
	windgoto((صحیح)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# اگر (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
بیرونی تهی __gcov_dump(تهی);
بیرونی تهی __gcov_reset(تهی);
#  define __gcov_flush() انجامبده { __gcov_dump(); __gcov_reset(); } وقتی (0)
# وگرنه
بیرونی تهی __gcov_flush(تهی);
# endif
#endif

    تهی
mch_exit(صحیح r)
{
    exiting = TRUE;

#اگر defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    اگر (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	اگر (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_برداشت_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	اگر (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	اگر (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	اگر (full_screen)
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
    اگر (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    اگر (deadly_signal != 0)
	بازگشت;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    ایستا تهی
may_core_dump(تهی)
{
    اگر (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ایستا صحیح
get_tty_fd(صحیح fd)
{
    صحیح		tty_fd = fd;

# اگر defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    اگر (mch_isatty(fd) > 1)
    {
	نویسه *name;

	name = ptsname(fd);
	اگر (name == پوچ)
	    بازگشت -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	اگر (tty_fd < 0)
	    بازگشت -1;
    }
# endif
    بازگشت tty_fd;
}

    ایستا صحیح
mch_tcgetattr(صحیح fd, تهی *term)
{
    صحیح		tty_fd;
    صحیح		retval = -1;

    tty_fd = get_tty_fd(fd);
    اگر (tty_fd < 0)
	بازگشت -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ساختار termios *)term);
#  وگرنه
    retval = ioctl(tty_fd, TCGETA, (ساختار termio *)term);
#  endif
# وگرنه
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ساختار sgttyb *)term);
# endif
    اگر (tty_fd != fd)
	close(tty_fd);
    بازگشت retval;
}

    تهی
mch_settmode(tmode_T tmode)
{
    ایستا صحیح first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ایستا ساختار termios told;
	   ساختار termios tnew;
#  وگرنه
    ایستا ساختار termio told;
	   ساختار termio tnew;
#  endif

    اگر (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    اگر (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == پوچ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  اگر defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   وگرنه
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    وگرنه
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    وگرنه اگر (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  اگر defined(HAVE_TERMIOS_H)
    {
	صحیح	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	وقتی (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  وگرنه
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# وگرنه
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ایستا ساختار sgttyb ttybold;
	   ساختار sgttyb ttybnew;

    اگر (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    اگر (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    وگرنه اگر (tmode == TMODE_SLEEP)
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
    تهی
get_stty(تهی)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    اگر (get_tty_info(read_cmd_fd, &info) != OK)
	بازگشت;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    اگر (p != پوچ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(پوچ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    صحیح
get_tty_info(صحیح fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ساختار termios کلیدها;
#  وگرنه
    ساختار termio کلیدها;
#  endif

    اگر (mch_tcgetattr(fd, &کلیدها) != -1)
    {
	info->backspace = کلیدها.c_cc[VERASE];
	info->interrupt = کلیدها.c_cc[VINTR];
	اگر (کلیدها.c_iflag & ICRNL)
	    info->enter = NL;
	وگرنه
	    info->enter = CAR;
	اگر (کلیدها.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	وگرنه
	    info->nl_does_cr = FALSE;
	بازگشت OK;
    }
# وگرنه
    // for "old" tty systems
    ساختار sgttyb کلیدها;

    اگر (mch_tcgetattr(fd, &کلیدها) != -1)
    {
	info->backspace = کلیدها.sg_erase;
	info->interrupt = کلیدها.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	بازگشت OK;
    }
# endif
    بازگشت FAIL;
}

#endif // VMS

ایستا صحیح	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    تهی
mch_setmouse(صحیح on)
{
#ifdef FEAT_BEVAL_TERM
    ایستا صحیح	bevalterm_ison = FALSE;
#endif
    صحیح		xterm_mouse_vers;

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    اگر (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    اگر (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	بازگشت;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    اگر (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    اگر (T_CXM != پوچ && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    وگرنه اگر (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    اگر (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	اگر (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    اگر (xterm_mouse_vers > 0)
    {
	اگر (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	وگرنه	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    وگرنه اگر (ttym_flags == TTYM_DEC)
    {
	اگر (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	وگرنه	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    وگرنه
    {
	اگر (on)
	{
	    اگر (gpm_open())
		mouse_ison = TRUE;
	}
	وگرنه
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    وگرنه
    {
	اگر (on)
	{
	    اگر (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	وگرنه
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    وگرنه
    {
	اگر (on)
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
# وگرنه
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	وگرنه
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    وگرنه
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	اگر (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	وگرنه
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#اگر defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    تهی
mch_bevalterm_changed(تهی)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    تهی
check_mouse_termcode(تهی)
{
#ifdef FEAT_MOUSE_XTERM
    اگر (use_xterm_mouse()
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
	اگر (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وگرنه
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    وگرنه
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    وگرنه
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    وگرنه
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    وگرنه
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    وگرنه
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    اگر (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	اگر (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وگرنه
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    اگر (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	اگر (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وگرنه
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
    صحیح
mch_get_shellsize(تهی)
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
	ساختار winsize	ws;
	صحیح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	اگر (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	اگر (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(پوچ, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# وگرنه // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ساختار ttysize	ts;
	صحیح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	اگر (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	اگر (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_سطرها;
#   ifdef FEAT_EVAL
	    ch_log(پوچ, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    اگر (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != پوچ)
    {
	اگر ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((نویسه *)p);
# ifdef FEAT_EVAL
	    ch_log(پوچ, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	اگر ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((نویسه *)p);
# ifdef FEAT_EVAL
	    ch_log(پوچ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    اگر (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(پوچ, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    اگر (columns <= 0 || rows <= 0)
	بازگشت FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    بازگشت OK;
}

# اگر defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    تهی
mch_calc_cell_size(ساختار cellsize *cs_out)
{
   ساختار winsize   ws;
   صحیح		    retval;
   صحیح		    x_cell_size;
   صحیح		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(پوچ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   اگر (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	بازگشت;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   اگر (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# اگر defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    صحیح
mch_report_winsize(صحیح fd, صحیح rows, صحیح cols)
{
    صحیح		tty_fd;
    صحیح		retval = -1;

    tty_fd = get_tty_fd(fd);
    اگر (tty_fd < 0)
	بازگشت FAIL;

#  اگر defined(TIOCSWINSZ)
    ساختار winsize ws;

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
    ch_log(پوچ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ساختار ttysize ts;

    ts.ts_cols = cols;
    ts.ts_سطرها = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(پوچ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    اگر (tty_fd != fd)
	close(tty_fd);
    بازگشت retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    تهی
mch_set_shellsize(تهی)
{
    اگر (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((صحیح)Rows, (صحیح)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    تهی
mch_new_shellsize(تهی)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ایستا pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    وقتی (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	اگر (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    اگر (++delay_msec > 10)
		delay_msec = 10;
	    ادامه;
	}
	اگر (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    بشکن;
    }
    بازگشت wait_pid;
}

#اگر !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ایستا تهی
set_child_environment(
	long	rows,
	long	columns,
	نویسه	*term,
	صحیح	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    نویسه	envbuf[50];
# وگرنه
    ایستا نویسه	envbuf_Term[30];
    ایستا نویسه	envbuf_Rows[20];
    ایستا نویسه	envbuf_سطرها[20];
    ایستا نویسه	envbuf_Columns[20];
    ایستا نویسه	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ایستا نویسه	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ایستا نویسه	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((نویسه *)envbuf, "%ld", rows);
    setenv("ROWS", (نویسه *)envbuf, 1);
    sprintf((نویسه *)envbuf, "%ld", rows);
    setenv("LINES", (نویسه *)envbuf, 1);
    sprintf((نویسه *)envbuf, "%ld", columns);
    setenv("COLUMNS", (نویسه *)envbuf, 1);
    sprintf((نویسه *)envbuf, "%d", t_colors);
    setenv("COLORS", (نویسه *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    اگر (is_terminal)
    {
	sprintf((نویسه *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (نویسه *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == پوچ ? "" : (نویسه *)serverName, 1);
#  endif
# وگرنه
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, اندازه(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, اندازه(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_سطرها, اندازه(envbuf_سطرها), "LINES=%ld", rows);
    putenv(envbuf_سطرها);
    vim_snprintf(envbuf_Columns, اندازه(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, اندازه(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    اگر (is_terminal)
    {
	vim_snprintf(envbuf_Version, اندازه(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, اندازه(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == پوچ ? "" : (نویسه *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ایستا تهی
set_default_child_environment(صحیح is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#اگر defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ایستا تهی
open_pty(صحیح *pty_master_fd, صحیح *pty_slave_fd, char_u **name1, char_u **name2)
{
    نویسه	*tty_name;

    اگر (name1 != پوچ)
	*name1 = پوچ;
    اگر (name2 != پوچ)
	*name2 = پوچ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    اگر (*pty_master_fd < 0)
	بازگشت;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# وگرنه
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    اگر (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    وگرنه
    {
	اگر (name1 != پوچ)
	    *name1 = vim_strsave((char_u *)tty_name);
	اگر (name2 != پوچ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ایستا تهی
may_send_sigint(صحیح c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    اگر (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# وگرنه
	kill(0, SIGINT);
# endif
	اگر (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#اگر !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    صحیح
unix_build_argv(
	char_u *cmd,
	نویسه ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    نویسه	**argv = پوچ;
    صحیح		argc;

    *sh_tofree = vim_strsave(p_sh);
    اگر (*sh_tofree == پوچ)		// out of memory
	بازگشت FAIL;

    اگر (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	بازگشت FAIL;
    *argvp = argv;

    اگر (cmd != پوچ)
    {
	char_u	*s;
	char_u	*p;

	اگر (extra_shell_arg != پوچ)
	    argv[argc++] = (نویسه *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	اگر (*shcf_tofree == پوچ)    // out of memory
	    بازگشت FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	وقتی (*p != NUL)
	{
	    argv[argc++] = (نویسه *)s;
	    وقتی (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (نویسه *)cmd;
    }
    argv[argc] = پوچ;
    بازگشت OK;
}
#endif

#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ایستا صحیح
mch_call_shell_terminal(
    char_u	*cmd,
    صحیح		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    نویسه	**argv = پوچ;
    char_u	*tofree1 = پوچ;
    char_u	*tofree2 = پوچ;
    صحیح		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	برو theend;

    init_job_options(&opt);
    ch_log(پوچ, "starting terminal for system command '%s'", cmd);
    buf = term_start(پوچ, argv, &opt, TERM_START_SYSTEM);
    اگر (buf == پوچ)
	برو theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	وقتی (term_use_loop())
	{
	    اگر (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		اگر (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    وگرنه
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(پوچ, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    اگر (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    بازگشت retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ایستا صحیح
mch_call_shell_system(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    نویسه	*ifn = پوچ;
    نویسه	*ofn = پوچ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    صحیح		x;

    out_flush();

    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    اگر (cmd == پوچ)
	x = system((نویسه *)p_sh);
    وگرنه
    {
# ifdef VMS
	اگر (ofn = strchr((نویسه *)cmd, '>'))
	    *ofn++ = '\0';
	اگر (ifn = strchr((نویسه *)cmd, '<'))
	{
	    نویسه *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    اگر (p)
		*p = '\0';
	}
	اگر (ofn)
	    x = vms_sys((نویسه *)cmd, ofn, ifn);
	وگرنه
	    x = system((نویسه *)cmd);
# وگرنه
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == پوچ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	اگر (newcmd == پوچ)
	    x = 0;
	وگرنه
	{
	    sprintf((نویسه *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == پوچ ? "" : (نویسه *)extra_shell_arg,
		    (نویسه *)p_shcf,
		    (نویسه *)cmd);
	    x = system((نویسه *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    اگر (emsg_silent)
	;
    وگرنه اگر (x == 127)
	پیام_puts(_("\nCannot execute shell sh\n"));
    وگرنه اگر (x && !(options & SHELL_SILENT))
    {
	پیام_puts(_("\nshell returned "));
	پیام_outnum((long)x);
	پیام_putchar('\n');
    }

    اگر (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    بازگشت x;
}

#وگرنه // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ایستا صحیح
mch_call_shell_fork(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    صحیح		status = -1;
    صحیح		retval = -1;
    نویسه	**argv = پوچ;
    char_u	*tofree1 = پوچ;
    char_u	*tofree2 = پوچ;
    صحیح		i;
    صحیح		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    صحیح		pty_slave_fd = -1;
# endif
    صحیح		fd_toshell[2];		// for pipes
    صحیح		fd_fromshell[2];
    صحیح		pipe_error = FALSE;
    صحیح		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    اگر (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	برو error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    اگر ((options & (SHELL_READ|SHELL_WRITE))
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
	اگر (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, پوچ, پوچ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	اگر (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    اگر (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		اگر (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    اگر (pipe_error)
	    {
		پیام_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    اگر (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	اگر (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    پیام_puts(_("\nCannot fork\n"));
	    اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		اگر (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		وگرنه				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	وگرنه اگر (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    اگر (ch_log_active())
	    {
		ch_log(پوچ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    اگر (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		صحیح fd;

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
		اگر (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    وگرنه اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		اگر (p_stmp)
		{
		    (تهی)setsid();
#  اگر defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		اگر (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    اگر (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (نویسه *)پوچ);
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
		اگر (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    اگر (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		وگرنه
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
		    اگر (gui.in_use)
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
	وگرنه			// parent
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
	    اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		صحیح	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		صحیح	    ta_len = 0;		// valid bytes in ta_buf[]
		صحیح	    len;
		صحیح	    p_more_save;
		صحیح	    old_State;
		صحیح	    c;
		صحیح	    toshell_fd;
		صحیح	    fromshell_fd;
		garray_T    ga;
		صحیح	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		اگر (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		وگرنه
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

		اگر ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    اگر ((wpid = fork()) == -1)
		    {
			پیام_puts(_("\nCannot fork\n"));
		    }
		    وگرنه اگر (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			برای (;;)
			{
			    اگر (lplen == 0)
				len = 0;
			    وگرنه اگر (lp[written] == NL)
				// NL -> NUL translation
				len = اكتب(toshell_fd, "", (size_t)1);
			    وگرنه
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = اكتب(toshell_fd, (نویسه *)lp + written,
					   s == پوچ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    اگر (len == (صحیح)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				اگر (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_سطر_شمار
						    || curbuf->b_p_eol)))
				    vim_ignored = اكتب(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				اگر (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    بشکن;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    وگرنه اگر (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    وگرنه // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		اگر (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		برای (;;)
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
		    اگر (!(options & SHELL_EXPAND)
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
		      اگر (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      اگر (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			اگر (len == 1 && (pty_master_fd < 0 || cmd != پوچ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    اگر (pty_master_fd < 0 && toshell_fd >= 0
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
			اگر (pty_master_fd < 0)
			{
			    برای (i = ta_len; i < ta_len + len; ++i)
			    {
				اگر (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    پیام_putchar(ta_buf[i]);
				وگرنه اگر (has_mbyte)
				{
				    صحیح l = (*mb_ptr2len)(ta_buf + i);

				    پیام_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				وگرنه
				    پیام_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(پیام_row, cmdline_col_off + پیام_col);
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
			اگر (options & SHELL_WRITE)
			    ta_len = 0;
			وگرنه اگر (toshell_fd >= 0)
			{
			    len = اكتب(toshell_fd, (نویسه *)ta_buf, (size_t)1);
			    اگر (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    اگر (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# وگرنه
			kill(0, SIGINT);
# endif
			اگر (wpid > 0)
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
		    وقتی (RealWaitForChar(fromshell_fd, 10L, پوچ, پوچ))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			اگر (len <= 0)		    // end of file or error
			    برو finished;

			noread_cnt = 0;
			اگر (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    برای (i = 0; i < len; ++i)
			    {
				اگر (buffer[i] == NL)
				    append_ga_سطر(&ga);
				وگرنه اگر (buffer[i] == NUL)
				    ga_append(&ga, NL);
				وگرنه
				    ga_append(&ga, buffer[i]);
			    }
			}
			وگرنه اگر (has_mbyte)
			{
			    صحیح		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    برای (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				اگر (l == 0)
				    l = 1;  // NUL byte?
				وگرنه اگر (MB_BYTE2LEN(*p) != l)
				    بشکن;
			    }
			    اگر (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				اگر (len >= 12)
				    ++p;
				وگرنه
				{
				    buffer_off = len;
				    ادامه;
				}
			    }
			    c = *p;
			    *p = NUL;
			    پیام_puts((نویسه *)buffer);
			    اگر (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				ادامه;
			    }
			    buffer_off = 0;
			}
			وگرنه
			{
			    buffer[len] = NUL;
			    پیام_puts((نویسه *)buffer);
			}

			windgoto(پیام_row, cmdline_col_off + پیام_col);
			cursor_on();
			out_flush();
			اگر (got_int)
			    بشکن;

# ifdef ELAPSED_FUNC
			اگر (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    اگر (msec > 2000)
			    {
				noread_cnt = 5;
				بشکن;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    اگر (wait_pid == pid)
		    {
			اگر (noread_cnt < 5)
			    ادامه;
			بشکن;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    اگر ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    وگرنه
			wait_pid = 0;

# اگر defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		اگر (options & SHELL_READ)
		{
		    اگر (ga.ga_len > 0)
		    {
			append_ga_سطر(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    وگرنه
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		اگر (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		اگر (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# اگر (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    وگرنه
	    {
		long delay_msec = 1;

		اگر (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		برای (;;)
		{
		    اگر (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  وگرنه
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    اگر ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			بشکن;
		    }

#  اگر defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    اگر (++delay_msec > 10)
			delay_msec = 10;
		}

		اگر (tmode == TMODE_RAW)
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
	    اگر (wait_pid != pid)
		(تهی)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    اگر (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    اگر (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, پوچ);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    اگر (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    اگر (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		اگر (retval != 0 && !emsg_silent)
		{
		    اگر (retval == EXEC_FAILED)
		    {
			پیام_puts(_("\nCannot execute shell "));
			پیام_outtrans(p_sh);
			پیام_putchar('\n');
		    }
		    وگرنه اگر (!(options & SHELL_SILENT))
		    {
			پیام_puts(_("\nshell returned "));
			پیام_outnum((long)retval);
			پیام_putchar('\n');
		    }
		}
	    }
	    وگرنه
		پیام_puts(_("\nCommand terminated\n"));
	}
    }

error:
    اگر (!did_settmode)
	اگر (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    بازگشت retval;
}
#endif // USE_SYSTEM

    صحیح
mch_call_shell(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(پوچ, "executing shell command: %s", cmd);
#endif
#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    اگر (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != پوچ
					      && (options & SHELL_SILENT) == 0)
	بازگشت mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    بازگشت mch_call_shell_system(cmd, options);
#وگرنه
    بازگشت mch_call_shell_fork(cmd, options);
#endif
}

#اگر defined(FEAT_EVAL)
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
    نویسه	**argv,
    char_u	*infile,
    صحیح		flags UNUSED,
    صحیح		*ret_len)
{
    pid_t	pid;
    صحیح		fd_out[2] = {-1, -1};
    صحیح		status = -1;
    char_u	*buffer = پوچ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(پوچ, "directly executing: %s", argv[0]);

    اگر (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	بازگشت پوچ;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# وگرنه
    pid = fork();
# endif
    اگر (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	بازگشت پوچ;
    }

    اگر (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	اگر (ch_log_active())
	{
	    ch_log(پوچ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	اگر (infile != پوچ)
	{
	    صحیح fd_in = open((نویسه *)infile, O_RDONLY);
	    اگر (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	وگرنه
	{
	    صحیح nullfd = open("/dev/null", O_RDONLY);
	    اگر (nullfd >= 0)
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
    برای (;;)
    {
	نویسه	buf[4096];
	صحیح	n;

	n = (صحیح)اقرأ(fd_out[0], buf, اندازه(buf));
	اگر (n <= 0)
	    بشکن;
	اگر (ga_grow(&ga, n) == FAIL)
	    بشکن;
	mch_memmove((نویسه *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (تهی)waitpid(pid, &status, 0);
    اگر (WIFEXITED(status))
	status = WEXITSTATUS(status);
    وگرنه
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    اگر (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	اگر (buffer != پوچ)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    اگر (ret_len == پوچ)
	    {
		صحیح	i;

		// Change NUL into SOH, otherwise the string is truncated.
		برای (i = 0; i < ga.ga_len; ++i)
		    اگر (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    وگرنه
		*ret_len = ga.ga_len;
	}
    }
    وگرنه اگر (ret_len != پوچ)
	*ret_len = 0;

    ga_clear(&ga);
    بازگشت buffer;
}
#endif

#اگر defined(FEAT_JOB_CHANNEL)
    تهی
mch_job_start(نویسه **argv, job_T *job, jobopt_T *options, صحیح is_terminal)
{
    pid_t	pid;
    صحیح		fd_in[2] = {-1, -1};	// for stdin
    صحیح		fd_out[2] = {-1, -1};	// for stdout
    صحیح		fd_err[2] = {-1, -1};	// for stderr
    صحیح		pty_master_fd = -1;
    صحیح		pty_slave_fd = -1;
    channel_T	*channel = پوچ;
    صحیح		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    صحیح		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    صحیح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحیح		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    صحیح		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    صحیح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحیح		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    صحیح		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    اگر (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    اگر (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    اگر (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((نویسه *)fname, O_RDONLY, 0);
	اگر (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    برو failed;
	}
    }
    وگرنه
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	اگر (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    برو failed;

    اگر (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((نویسه *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	اگر (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    برو failed;
	}
    }
    وگرنه اگر (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	برو failed;

    اگر (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((نویسه *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	اگر (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    برو failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    وگرنه اگر (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	برو failed;

    اگر (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	اگر (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    اگر (channel != پوچ)
		++channel->ch_refcount;
	}
	وگرنه
	    channel = add_channel();
	اگر (channel == پوچ)
	    برو failed;
	اگر (job->jv_tty_out != پوچ)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    اگر (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	برو failed;
    }
    اگر (pid == 0)
    {
	صحیح	null_fd = -1;
	صحیح	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	اگر (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(تهی)setsid();
# endif

# ifdef FEAT_TERMINAL
	اگر (options->jo_term_rows > 0)
	{
	    نویسه *term = (نویسه *)T_NAME;

#  ifdef FEAT_GUI
	    اگر (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    اگر (term == پوچ || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		اگر (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		وگرنه اگر (t_colors > 16)
		    term = "xterm-color";
		وگرنه
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	وگرنه
# endif
	    set_default_child_environment(is_terminal);

	اگر (options->jo_env != پوچ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    صحیح		todo = (صحیح)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_اقلام(&dict->dv_hashtab, hi, todo)
		اگر (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_کلید, tv_get_string(item));
		    --todo;
		}
	}

	اگر (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    اگر (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	اگر (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (نویسه *)پوچ);
# endif
	}

	// set up stdin for the child
	close(0);
	اگر (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	وگرنه اگر (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وگرنه
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	اگر (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	وگرنه اگر (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	وگرنه اگر (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وگرنه
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	اگر (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	وگرنه اگر (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وگرنه
	    vim_ignored = dup(fd_out[1]);

	اگر (fd_in[0] >= 0)
	    close(fd_in[0]);
	اگر (fd_in[1] >= 0)
	    close(fd_in[1]);
	اگر (fd_out[0] >= 0)
	    close(fd_out[0]);
	اگر (fd_out[1] >= 0)
	    close(fd_out[1]);
	اگر (fd_err[0] >= 0)
	    close(fd_err[0]);
	اگر (fd_err[1] >= 0)
	    close(fd_err[1]);
	اگر (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	اگر (null_fd >= 0)
	    close(null_fd);

	اگر (options->jo_cwd != پوچ && mch_chdir((نویسه *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	اگر (stderr_works)
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

    اگر (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    اگر (fd_in[0] >= 0)
	close(fd_in[0]);
    اگر (fd_out[1] >= 0)
	close(fd_out[1]);
    اگر (fd_err[1] >= 0)
	close(fd_err[1]);
    اگر (channel != پوچ)
    {
	صحیح in_fd = INVALID_FD;
	صحیح out_fd = INVALID_FD;
	صحیح err_fd = INVALID_FD;

	اگر (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	اگر (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	اگر (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    اگر (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    وگرنه اگر (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    وگرنه
    {
	اگر (fd_in[1] >= 0)
	    close(fd_in[1]);
	اگر (fd_out[0] >= 0)
	    close(fd_out[0]);
	اگر (fd_err[0] >= 0)
	    close(fd_err[0]);
	اگر (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    بازگشت;

failed:
    channel_unref(channel);
    اگر (fd_in[0] >= 0)
	close(fd_in[0]);
    اگر (fd_in[1] >= 0)
	close(fd_in[1]);
    اگر (fd_out[0] >= 0)
	close(fd_out[0]);
    اگر (fd_out[1] >= 0)
	close(fd_out[1]);
    اگر (fd_err[0] >= 0)
	close(fd_err[0]);
    اگر (fd_err[1] >= 0)
	close(fd_err[1]);
    اگر (pty_master_fd >= 0)
	close(pty_master_fd);
    اگر (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ایستا char_u *
get_signal_name(صحیح sig)
{
    صحیح		i;
    char_u	numbuf[NUMBUFLEN];

    اگر (sig == SIGKILL)
	بازگشت vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    برای (i = 0; signal_info[i].sig != -1; i++)
	اگر (sig == signal_info[i].sig)
	    بازگشت strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((نویسه *)numbuf, NUMBUFLEN, "%d", sig);
    بازگشت vim_strnsave(numbuf, i);
}

    نویسه *
mch_job_status(job_T *job)
{
    صحیح		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    اگر (wait_pid == -1)
    {
	صحیح waitpid_errno = errno;
	اگر (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    بازگشت "run";

	// process must have exited
	اگر (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	برو return_dead;
    }
    اگر (wait_pid == 0)
	بازگشت "run";
    اگر (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	اگر (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	برو return_dead;
    }
    اگر (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	اگر (job->jv_status < JOB_ENDED && job->jv_termsig != پوچ)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	برو return_dead;
    }
    بازگشت "run";

return_dead:
    اگر (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    بازگشت "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    صحیح		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    اگر (dont_check_job_ended > 0)
	بازگشت پوچ;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    اگر (wait_pid <= 0)
	// no process ended
	بازگشت پوچ;
    برای (job = job_list; job != پوچ; job = job->jv_next)
    {
	اگر (job->jv_pid == wait_pid)
	{
	    اگر (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    وگرنه اگر (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    اگر (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    بازگشت job;
	}
    }
    بازگشت پوچ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    صحیح
mch_signal_job(job_T *job, char_u *how)
{
    صحیح	    sig = -1;

    اگر (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    وگرنه اگر (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    وگرنه اگر (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    وگرنه اگر (STRCMP(how, "int") == 0)
	sig = SIGINT;
    وگرنه اگر (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    وگرنه اگر (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    وگرنه اگر (SAFE_isdigit(*how))
	sig = atoi((نویسه *)how);
    وگرنه
	بازگشت FAIL;

    // Never kill ourselves!
    اگر (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    بازگشت OK;
}

/*
 * Clear the data related to "job".
 */
    تهی
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (تهی)waitpid(job->jv_pid, پوچ, WNOHANG);
}
#endif

#اگر defined(FEAT_TERMINAL)
    صحیح
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    صحیح		pty_master_fd = -1;
    صحیح		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    اگر (pty_master_fd < 0 || pty_slave_fd < 0)
	بازگشت FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    اگر (channel == پوچ)
    {
	close(pty_master_fd);
	بازگشت FAIL;
    }
    اگر (job->jv_tty_out != پوچ)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    بازگشت OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    تهی
mch_breakcheck(صحیح force)
{
    اگر ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, پوچ, پوچ))
	fill_درون_داد_buf(FALSE);
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
    ایستا صحیح
WaitForChar(long msec, صحیح *interrupted, صحیح ignore_درون_داد)
{
#ifdef FEAT_TIMERS
    بازگشت ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_درون_داد) == OK;
#وگرنه
    بازگشت WaitForCharOrMouse(msec, interrupted, ignore_درون_داد);
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
    ایستا صحیح
WaitForCharOrMouse(long msec, صحیح *interrupted, صحیح ignore_درون_داد)
{
#ifdef FEAT_MOUSE_GPM
    صحیح		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    صحیح		rest;
#endif
    صحیح		avail;

    اگر (!ignore_درون_داد && درون_داد_available())	    // something in inbuf[]
	بازگشت 1;

#اگر defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    اگر (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	اگر (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#اگر defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    اگر (do_xterm_trace())
	rest = msec;
# endif
    انجامبده
    {
# ifdef FEAT_XCLIPBOARD
	اگر (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    اگر (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    اگر (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	اگر (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	اگر (!avail && !gpm_process_wanted)
# وگرنه
	avail = RealWaitForChar(read_cmd_fd, msec, پوچ, interrupted);
	اگر (!avail)
# endif
	{
	    اگر (!ignore_درون_داد && درون_داد_available())
		بازگشت 1;
# ifdef FEAT_XCLIPBOARD
	    اگر (rest == 0 || !do_xterm_trace())
# endif
		بشکن;
	}
    }
    وقتی (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#وگرنه
    avail = RealWaitForChar(read_cmd_fd, msec, پوچ, interrupted);
#endif
    بازگشت avail;
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
    ایستا صحیح
RealWaitForChar(صحیح fd, long msec, صحیح *check_for_gpm UNUSED, صحیح *interrupted)
{
    صحیح		ret;
    صحیح		result;
# اگر defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ایستا صحیح	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    اگر (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    اگر (busy)
	بازگشت 0;
# endif

# ifdef MAY_LOOP
    برای (;;)
# endif
    {
# ifdef MAY_LOOP
	صحیح		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	صحیح		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	صحیح		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ساختار pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	صحیح		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	صحیح             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	صحیح		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	صحیح		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	صحیح		xsmp_idx = -1;
#  endif
	صحیح		towait = (صحیح)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	اگر (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (صحیح)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	اگر ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	اگر (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_عدد(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (check_for_gpm != پوچ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	اگر (xsmp_icefd != -1)
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
	اگر (interrupted != پوچ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	اگر (result == 0 && interrupted != پوچ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	اگر (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	اگر (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	اگر (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    اگر (--ret == 0 && !درون_داد_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	اگر (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    اگر (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    وگرنه اگر (fds[xsmp_idx].revents & POLLHUP)
	    {
		اگر (p_verbose > 0)
		    verb_پیام(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    اگر (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	اگر (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# وگرنه // HAVE_SELECT

	ساختار timeval  tv;
	ساختار timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ایستا fd_set	rfds, wfds, efds;
	صحیح		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	اگر (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	اگر (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	وگرنه
	    tvp = پوچ;

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
	اگر ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    اگر (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	اگر (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_عدد(xterm_dpy), &rfds);
	    اگر (maxfd < Connection_عدد(xterm_dpy))
		maxfd = Connection_عدد(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (check_for_gpm != پوچ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    اگر (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	اگر (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    اگر (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	اگر (interrupted != پوچ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	اگر (result)
	    --ret;
	وگرنه اگر (interrupted != پوچ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	اگر (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    اگر (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    اگر (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(پوچ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    برو select_eintr;
	}
#  endif
#  ifdef __TANDEM
	اگر (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	اگر (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	اگر (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	اگر (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_عدد(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    اگر (--ret == 0 && !درون_داد_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (ret > 0 && check_for_gpm != پوچ && gpm_flag && gpm_fd >= 0)
	{
	    اگر (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    وگرنه اگر (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	اگر (ret > 0 && xsmp_icefd != -1)
	{
	    اگر (FD_ISSET(xsmp_icefd, &efds))
	    {
		اگر (p_verbose > 0)
		    verb_پیام(_("XSMP lost ICE connection"));
		xsmp_close();
		اگر (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    وگرنه اگر (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		اگر (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	اگر (ret >= 0)
	    (تهی)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	اگر (finished || msec == 0)
	    بشکن;

#  اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	اگر (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    بشکن;
#  endif

	// We're going to loop around again, find out for how long
	اگر (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  وگرنه
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    اگر (msec <= 0)
		بشکن;	// waited long enough
	}
# endif
    }

    بازگشت result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    صحیح
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    صحیح		flags)		// EW_* flags
{
    بازگشت unix_expandpath(gap, path, 0, flags, FALSE);
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

    صحیح
mch_expand_wildcards(
    صحیح		   رقم_pat,
    char_u	 **pat,
    صحیح		  *رقم_file,
    char_u	***file,
    صحیح		   flags)	// EW_* flags
{
    صحیح		i;
    size_t	len;
    long	llen;
    char_u	*p;
    صحیح		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    صحیح		j;
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
    صحیح		shell_style = STYLE_ECHO;
    صحیح		check_spaces;
    ایستا صحیح	did_find_nul = FALSE;
    صحیح		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ایستا string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ایستا string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *رقم_file = 0;	// default: no files found
    *file = پوچ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    اگر (!have_wildcard(رقم_pat, pat))
	بازگشت save_patterns(رقم_pat, pat, رقم_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    اگر (sandbox != 0 && check_secure())
	بازگشت FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    اگر (secure || restricted)
	برای (i = 0; i < رقم_pat; ++i)
	    اگر (vim_strchr(pat[i], '`') != پوچ
		    && (check_restricted() || check_secure()))
		بازگشت FAIL;

    /*
     * get a name for the temp file
     */
    اگر ((tempname = vim_tempname('o', FALSE)) == پوچ)
    {
	emsg(_(e_cant_get_temp_file_name));
	بازگشت FAIL;
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
    اگر (رقم_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    وگرنه اگر ((len = STRLEN(p_sh)) >= 3)
    {
	اگر (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	وگرنه اگر (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    اگر (shell_style == STYLE_ECHO)
    {
	اگر (strstr((نویسه *)gettail(p_sh), "bash") != پوچ)
	    shell_style = STYLE_GLOBSTAR;
	وگرنه اگر (strstr((نویسه *)gettail(p_sh), "sh") != پوچ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    اگر (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    وگرنه اگر (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    برای (i = 0; i < رقم_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# وگرنه
	++len;				// add space
	برای (j = 0; pat[i][j] != NUL; ++j)
	{
	    اگر (vim_strchr(SHELL_SPECIAL, pat[i][j]) != پوچ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    اگر (command == پوچ)
    {
	// out of memory
	vim_free(tempname);
	بازگشت FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    اگر (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((نویسه *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((نویسه *)command, ")>");
	اگر (p == پوچ)
	{
	    ;					    // can't happen ;-)
	}
	وگرنه
	{
	    --p;
	    وقتی (p > command && VIM_ISWHITE(*p))
		--p;
	    اگر (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    وقتی (p > command && VIM_ISWHITE(*p))
		--p;
	    اگر (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    وگرنه
    {
	اگر (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((نویسه *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	وگرنه اگر (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((نویسه *)command, len, "print -N >%s", tempname);
	وگرنه اگر (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((نویسه *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	وگرنه اگر (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((نویسه *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	وگرنه
	    commandlen = vim_snprintf((نویسه *)command, len, "echo >%s", tempname);

	برای (i = 0; i < رقم_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((نویسه *)command + commandlen, len, " \"%s\"", pat[i]);
# وگرنه
	    صحیح intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    برای (j = 0; pat[i][j] != NUL; ++j)
	    {
		اگر (pat[i][j] == '`')
		    intick = !intick;
		وگرنه اگر (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    اگر (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != پوچ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		وگرنه اگر (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != پوچ)
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
    اگر (flags & EW_SILENT)
	show_shell_mess = FALSE;
    اگر (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    اگر (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    وگرنه اگر (shell_style == STYLE_GLOB && !have_dollars(رقم_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    اگر (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = پوچ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    اگر (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	اگر (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    پیام_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    اگر (!(flags & EW_SILENT))
# endif
	    {
		پیام(_(e_cannot_expand_wildcards));
		پیام_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	اگر (shell_style == STYLE_BT)
	    بازگشت FAIL;
	برو notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((نویسه *)tempname, READBIN);
    اگر (fd == پوچ)
    {
	// Something went wrong, perhaps a file name with a special char.
	اگر (!(flags & EW_SILENT))
	{
	    پیام(_(e_cannot_expand_wildcards));
	    پیام_start();		// don't overwrite this message
	}
	vim_free(tempname);
	برو notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    اگر (llen < 0)
	// just in case ftell() would fail
	buffer = پوچ;
    وگرنه
	buffer = alloc(llen + 1);
    اگر (buffer == پوچ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	بازگشت FAIL;
    }
    len = llen;
    i = fread((نویسه *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    اگر (i != (صحیح)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	بازگشت FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    برای (i = 0; i < (صحیح)len; ++i)
	اگر (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    اگر (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	برای (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    وقتی (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    وگرنه اگر (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	برای (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    وقتی (*p != '\n' && *p != NUL)
		++p;
	    اگر (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    وگرنه
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
	اگر (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    اگر (len && (صحیح)STRLEN(buffer) < (صحیح)len)
		did_find_nul = TRUE;
	    وگرنه
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	اگر (len && buffer[len - 1] == NUL)
	    --len;
	وگرنه
	    buffer[len] = NUL;
	i = 0;
	برای (p = buffer; p < buffer + len; ++p)
	    اگر (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	اگر (len)
	    ++i;			// count last entry
    }
    اگر (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	برو notfound;
    }
    *رقم_file = i;
    *file = ALLOC_MULT(char_u *, i);
    اگر (*file == پوچ)
    {
	// out of memory
	vim_free(buffer);
	بازگشت FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    برای (i = 0; i < *رقم_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	اگر (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    وقتی (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    اگر (p == buffer + len)		// last entry
		*p = NUL;
	    وگرنه
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	وگرنه		// NUL separates
	{
	    وقتی (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    برای (j = 0, i = 0; i < *رقم_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	اگر (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ادامه;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	اگر ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ادامه;

	// Skip files that are not executable if we check for that.
	اگر (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], پوچ, !(flags & EW_SHELLCMD)))
	    ادامه;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	اگر (p)
	{
	    STRCPY(p, (*file)[i]);
	    اگر (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *رقم_file = j;

    اگر (*رقم_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	برو notfound;
    }

    بازگشت OK;

notfound:
    اگر (flags & EW_NOTFOUND)
	بازگشت save_patterns(رقم_pat, pat, رقم_file, file);
    بازگشت FAIL;
}

#endif // VMS

    ایستا صحیح
save_patterns(
    صحیح		رقم_pat,
    char_u	**pat,
    صحیح		*رقم_file,
    char_u	***file)
{
    صحیح		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, رقم_pat);
    اگر (*file == پوچ)
	بازگشت FAIL;
    برای (i = 0; i < رقم_pat; i++)
    {
	s = vim_strsave(pat[i]);
	اگر (s != پوچ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *رقم_file = رقم_pat;
    بازگشت OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	وگرنه
	    اگر (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#وگرنه
				    "*?[{'"
#endif
						, *p) != پوچ)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    صحیح
mch_has_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	وگرنه
	    اگر (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#وگرنه
				    "*?[{`'$"
#endif
						, *p) != پوچ
		|| (*p == '~' && p[1] != NUL))
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

    ایستا صحیح
have_wildcard(صحیح رقم, char_u **file)
{
    صحیح	    i;

    برای (i = 0; i < رقم; i++)
	اگر (mch_has_wildcard(file[i]))
	    بازگشت 1;
    بازگشت 0;
}

    ایستا صحیح
have_dollars(صحیح رقم, char_u **file)
{
    صحیح	    i;

    برای (i = 0; i < رقم; i++)
	اگر (vim_strchr(file[i], '$') != پوچ)
	    بازگشت TRUE;
    بازگشت FALSE;
}

#اگر !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    صحیح
mch_rename(ثابت نویسه *src, ثابت نویسه *dest)
{
    ساختار stat	    st;

    اگر (stat(dest, &st) >= 0)	    // fail if destination exists
	بازگشت -1;
    اگر (link(src, dest) != 0)	    // link file to new name
	بازگشت -1;
    اگر (mch_remove(src) == 0)	    // delete link to old name
	بازگشت 0;
    بازگشت -1;
}
#endif // !HAVE_RENAME

#اگر defined(FEAT_MOUSE_GPM)
# اگر defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ایستا صحیح
load_libgpm(تهی)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    اگر (libgpm_hinst == پوچ)
    {
	اگر (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	بازگشت FAIL;
    }

    اگر (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == پوچ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == پوچ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == پوچ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == پوچ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == پوچ
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = پوچ;
	dll_gpm_flag = پوچ;
	dll_gpm_fd   = پوچ;
	بازگشت FAIL;
    }
    بازگشت OK;
}

    صحیح
gpm_available(تهی)
{
    بازگشت libgpm_hinst != پوچ || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ایستا صحیح
gpm_open(تهی)
{
    ایستا Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    اگر (!gpm_available())
	بازگشت 0;
# endif

    اگر (gpm_flag)
	بازگشت 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    اگر (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	بازگشت 1; // succeed
    }
    اگر (gpm_fd == -2)
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
    بازگشت 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    صحیح
gpm_enabled(تهی)
{
    بازگشت gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ایستا تهی
gpm_close(تهی)
{
    اگر (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ایستا صحیح
mch_gpm_process(تهی)
{
    صحیح			button;
    ایستا Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    صحیح			row,col;
    unsigned نویسه	buttons_mask;
    unsigned نویسه	gpm_modifiers;
    ایستا unsigned نویسه old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    انتخاب (GPM_BARE_EVENTS(gpm_event.type))
    {
	حالت GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    بشکن;
	حالت GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    انتخاب (buttons_mask)
	    {
		حالت GPM_B_چپ:
		    button = MOUSE_چپ;
		    بشکن;
		حالت GPM_B_میانه:
		    button = MOUSE_میانه;
		    بشکن;
		حالت GPM_B_راست:
		    button = MOUSE_راست;
		    بشکن;
		پیشفرض:
		    بازگشت 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_رقم_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    بشکن;
	حالت GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    بشکن;
	پیشفرض:
	    بازگشت 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    اگر (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    اگر (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    اگر (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_درون_داد_buf(string, 6);
    بازگشت 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ایستا صحیح
sysmouse_open(تهی)
{
    ساختار mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.سیگنال = SIGUSR2;
    اگر (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	بازگشت FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    بازگشت OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ایستا تهی
sysmouse_close(تهی)
{
    ساختار mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.سیگنال = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ایستا تهی
sig_sysmouse SIGDEFARG(sigarg)
{
    ساختار mouse_info	mouse;
    ساختار video_info	video;
    char_u		string[6];
    صحیح			row, col;
    صحیح			button;
    صحیح			buttons;
    ایستا صحیح		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	بازگشت;
# endif

    mouse.operation = MOUSE_GETINFO;
    اگر (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	اگر (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	وگرنه
	{
	    انتخاب (buttons)
	    {
		حالت 0:
		    button = MOUSE_RELEASE;
		    بشکن;
		حالت 1:
		    button = MOUSE_چپ;
		    بشکن;
		حالت 2:
		    button = MOUSE_میانه;
		    بشکن;
		حالت 4:
		    button = MOUSE_راست;
		    بشکن;
		پیشفرض:
		    بازگشت;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_درون_داد_buf(string, 6);
    }
    بازگشت;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ایستا صحیح dev_urandom_state = NOTDONE;

    اگر (dev_urandom_state == FAIL)
	بازگشت FAIL;

    صحیح fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    اگر (fd == -1)
	dev_urandom_state = FAIL;
    وگرنه اگر (اقرأ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    وگرنه
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    بازگشت dev_urandom_state;
}

#اگر defined(FEAT_LIBCALL)
تعریفنوع char_u * (*STRPROCSTR)(char_u *);
تعریفنوع char_u * (*INTPROCSTR)(صحیح);
تعریفنوع صحیح (*STRPROCINT)(char_u *);
تعریفنوع صحیح (*INTPROCINT)(صحیح);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    صحیح
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    صحیح		argint,
    char_u	**string_result, // NULL when using number_result
    صحیح		*عدد_result)
{
# اگر defined(USE_DLOPEN)
    تهی	*hinstLib;
    نویسه	*dlerr = پوچ;
# وگرنه
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = پوچ;
    صحیح		retval_int = 0;
    صحیح		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# اگر defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (تهی)dlerror();

    hinstLib = dlopen((نویسه *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    اگر (hinstLib == پوچ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	اگر (dlerr != پوچ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# وگرنه
    hinstLib = shl_load((ثابت نویسه*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    اگر (hinstLib != پوچ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	اگر (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  اگر defined(USE_DLOPEN)
	    dlerr = پوچ;
#  endif
	    mch_didjmp();
	}
	وگرنه
# endif
	{
	    retval_str = پوچ;
	    retval_int = 0;

	    اگر (argstring != پوچ)
	    {
# اگر defined(USE_DLOPEN)
		*(تهی **)(&ProcAdd) = dlsym(hinstLib, (ثابت نویسه *)funcname);
		dlerr = dlerror();
# وگرنه
		اگر (shl_findsym(&hinstLib, (ثابت نویسه *)funcname,
					TYPE_PROCEDURE, (تهی *)&ProcAdd) < 0)
		    ProcAdd = پوچ;
# endif
		اگر ((success = (ProcAdd != پوچ
# اگر defined(USE_DLOPEN)
			    && dlerr == پوچ
# endif
			    )))
		{
		    اگر (string_result == پوچ)
			retval_int = ((STRPROCINT)(تهی *)ProcAdd)(argstring);
		    وگرنه
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    وگرنه
	    {
# اگر defined(USE_DLOPEN)
		*(تهی **)(&ProcAddI) = dlsym(hinstLib, (ثابت نویسه *)funcname);
		dlerr = dlerror();
# وگرنه
		اگر (shl_findsym(&hinstLib, (ثابت نویسه *)funcname,
				       TYPE_PROCEDURE, (تهی *)&ProcAddI) < 0)
		    ProcAddI = پوچ;
# endif
		اگر ((success = (ProcAddI != پوچ
# اگر defined(USE_DLOPEN)
			    && dlerr == پوچ
# endif
			    )))
		{
		    اگر (string_result == پوچ)
			retval_int = ((INTPROCINT)(تهی *)ProcAddI)(argint);
		    وگرنه
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    اگر (string_result == پوچ)
		*عدد_result = retval_int;
	    وگرنه اگر (retval_str != پوچ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	اگر (lc_signal != 0)
	{
	    صحیح i;

	    // try to find the name of this signal
	    برای (i = 0; signal_info[i].sig != -1; i++)
		اگر (lc_signal == signal_info[i].sig)
		    بشکن;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# اگر defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	اگر (dlerr != پوچ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(تهی)dlclose(hinstLib);
# وگرنه
	(تهی)shl_unload(hinstLib);
# endif
    }

    اگر (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	بازگشت FAIL;
    }

    بازگشت OK;
}
#endif

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ایستا صحیح	xterm_trace = -1;	// default: disabled
ایستا صحیح	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    تهی
setup_term_clip(تهی)
{
    صحیح		z = 0;
    نویسه	*strp = "";
    Widget	AppShell;

    اگر (!x_connect_to_server())
	بازگشت;

    open_app_context();
    اگر (app_context != پوچ && xterm_Shell == (Widget)0)
    {
	صحیح (*oldhandler)(Display*, XErrorEvent*);
# اگر defined(USING_SETJMP)
	صحیح (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	اگر (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# اگر defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	اگر (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = پوچ;
	}
	وگرنه
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", پوچ, 0, &z, &strp);
	    اگر (xterm_dpy != پوچ)
		xterm_dpy_retry_شمار = 0;
# اگر defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# اگر defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(تهی)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(تهی)XSetErrorHandler(oldhandler);

	اگر (xterm_dpy == پوچ)
	{
	    اگر (p_verbose > 0)
		verb_پیام(_("Opening the X display failed"));
	    بازگشت;
	}

	// Catch terminating error of the X server connection.
	(تهی)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		پوچ);
	اگر (AppShell == (Widget)0)
	    بازگشت;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		بالاترین_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		پوچ);
	اگر (xterm_Shell == (Widget)0)
	    بازگشت;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	اگر (x11_display == پوچ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    اگر (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	اگر (x11_window == 0 && (strp = getenv("WINDOWID")) != پوچ)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	اگر (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	اگر (x11_window != 0)
	    xterm_trace = 0;
    }
}

    تهی
start_xterm_trace(صحیح button)
{
    اگر (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	بازگشت;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    تهی
stop_xterm_trace(تهی)
{
    اگر (xterm_trace < 0)
	بازگشت;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ایستا صحیح
do_xterm_trace(تهی)
{
    Window		ریشه, child;
    صحیح			ریشه_x, ریشه_y;
    صحیح			win_x, win_y;
    صحیح			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    ایستا char_u	*mouse_code = پوچ;
    ایستا size_t	mouse_codelen = 0;
    ایستا char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ایستا صحیح		prev_row = 0, prev_col = 0;
    ایستا XSizeHints	xterm_hints;

    اگر (xterm_trace <= 0)
	بازگشت FALSE;

    اگر (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	اگر (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    بازگشت FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	اگر (mouse_code != پوچ)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &ریشه, &child, &ریشه_x, &ریشه_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	اگر (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	اگر (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	بازگشت TRUE;
    }

    اگر (mouse_code == پوچ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	بازگشت FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &ریشه, &child, &ریشه_x, &ریشه_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    اگر (row == prev_row && col == prev_col)
	بازگشت TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_درون_داد_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    بازگشت TRUE;
}

# اگر defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    تهی
clear_xterm_clip(تهی)
{
    اگر (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    اگر (xterm_dpy != پوچ)
    {
#  اگر 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	اگر (x11_display == xterm_dpy)
	    x11_display = پوچ;
	xterm_dpy = پوچ;
    }
#  اگر 0
    اگر (app_context != (XtAppContext)پوچ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)پوچ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ایستا تهی
clip_update(تهی)
{
# ifdef FEAT_GUI
    اگر (gui.in_use)
	gui_mch_update();
    وگرنه
# endif
    اگر (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    تهی
xterm_update(تهی)
{
    XEvent event;

    برای (;;)
    {
	Xt_درون_داد_Mask mask = XtAppPending(app_context);

	اگر (mask == 0 || vim_is_درون_داد_buf_full())
	    بشکن;

	اگر (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		اگر (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_مقدار)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	وگرنه
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    صحیح
clip_xterm_own_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	بازگشت clip_x11_own_selection(xterm_Shell, cbd);
    بازگشت FAIL;
}

    تهی
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    تهی
clip_xterm_request_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    تهی
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#اگر defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# اگر defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ایستا تهی
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    صحیح		save_cmod_flags;
    صحیح		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    اگر (check_changed_any(FALSE, FALSE))
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
    اگر (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ایستا تهی
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    صحیح		save_type UNUSED,
    Bool	shutdown,
    صحیح		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    اگر (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    اگر (p_verbose > 0)
	verb_پیام(_("XSMP handling save-yourself request"));

# اگر defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    اگر (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    وگرنه
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
    ایستا تهی
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ایستا تهی
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
    ایستا تهی
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    اگر (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ایستا تهی
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    اگر (opening)
    {
	xsmp_icefd = Ice_Connection_عدد(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, پوچ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    صحیح
xsmp_handle_requests(تهی)
{
    Bool rep;

    اگر (IceProcessMessages(xsmp.iceconn, پوچ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	اگر (p_verbose > 0)
	    verb_پیام(_("XSMP lost ICE connection"));
	xsmp_close();
	بازگشت FAIL;
    }
    وگرنه
	بازگشت OK;
}

ایستا صحیح dummy;

// Set up X Session Management Protocol
    تهی
xsmp_init(تهی)
{
    نویسه		errorstring[80];
    SmcCallbacks	smcallbacks;
# اگر 0
    Sm_Prop_مقدار		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    اگر (p_verbose > 0)
	verb_پیام(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = پوچ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = پوچ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = پوچ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = پوچ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    اگر (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	اگر (p_verbose > 0)
	    verb_پیام(_("XSMP ICE connection watch failed"));
	بازگشت;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    پوچ,
	    پوچ,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    پوچ,
	    &xsmp.clientid,
	    اندازه(errorstring) - 1,
	    errorstring);
    اگر (xsmp.smcconn == پوچ)
    {
	اگر (p_verbose > 0)
	{
	    نویسه errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[اندازه(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, اندازه(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_پیام(errorreport);
	}
	بازگشت;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# اگر 0
    // ID ourselves
    smname.مقدار = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.رقم_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    تهی
xsmp_close(تهی)
{
    اگر (xsmp_icefd == -1)
	بازگشت;

    SmcCloseConnection(xsmp.smcconn, 0, پوچ);
    اگر (xsmp.clientid != پوچ)
	آزاد(xsmp.clientid);
    xsmp.clientid = پوچ;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#اگر defined(FEAT_RELTIME)
# اگر defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ایستا فرار sig_atomic_t timeout_flag = FALSE;
ایستا timer_t		     timer_id;
ایستا صحیح		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ایستا تهی
set_flag(اجتماع sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    تهی
stop_timeout(تهی)
{
    ایستا ساختار itimerspec disarm = {{0, 0}, {0, 0}};

    اگر (timer_created)
    {
	صحیح ret = timer_settime(timer_id, 0, &disarm, پوچ);

	اگر (ret < 0)
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
    فرار sig_atomic_t *
start_timeout(long msec)
{
    ساختار itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    صحیح ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    اگر (!timer_created)
    {
	ساختار sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	اگر (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    بازگشت &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(پوچ, "setting timeout timer to %d sec %ld nsec",
	       (صحیح)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, پوچ);
    اگر (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    بازگشت &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    تهی
delete_timer(تهی)
{
    اگر (!timer_created)
	بازگشت;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# وگرنه // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ایستا ساختار sigaction		prev_sigaction;
ایستا فرار sig_atomic_t	timeout_flag	     = FALSE;
ایستا صحیح			timer_active	     = FALSE;
ایستا صحیح			timer_handler_active = FALSE;
ایستا فرار sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ایستا تهی
set_flag SIGDEFARG(sigarg)
{
    اگر (alarm_pending)
	alarm_pending = FALSE;
    وگرنه
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    تهی
stop_timeout(تهی)
{
    ایستا ساختار itimerval disarm = {{0, 0}, {0, 0}};
    صحیح			    ret;

    اگر (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, پوچ);
	اگر (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    اگر (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, پوچ);
	اگر (ret < 0)
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
    فرار sig_atomic_t *
start_timeout(long msec)
{
    ساختار itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ساختار sigaction	handle_alarm;
    صحیح			ret;
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
    اگر (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, پوچ);
    }
    اگر (unlikely(ret != 0 || alarm_pending < 0))
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
    اگر (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	بازگشت &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, پوچ);
    اگر (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	بازگشت &timeout_flag;
    }

    timer_active = TRUE;
    بازگشت &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
