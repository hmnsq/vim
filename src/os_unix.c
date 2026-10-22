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

#ضمن "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#ضمن "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
ساكن صحيح selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != عدم ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != عدم ? *dll_gpm_fd   : -1)

ساكن صحيح (*dll_Gpm_Open)     (Gpm_Connect *, صحيح);
ساكن صحيح (*dll_Gpm_Close)    (فراغ);
ساكن صحيح (*dll_Gpm_GetEvent) (Gpm_Event *);
ساكن صحيح *dll_gpm_flag;
ساكن صحيح *dll_gpm_fd;

ساكن فراغ *libgpm_hinst;
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

ساكن فراغ gpm_close(فراغ);
ساكن صحيح gpm_open(فراغ);
ساكن صحيح mch_gpm_process(فراغ);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ساكن صحيح sysmouse_open(فراغ);
ساكن فراغ sysmouse_close(فراغ);
ساكن فراغ sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#אם defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#אם defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
ساكن Widget	xterm_Shell = (Widget)0;
ساكن فراغ clip_update(فراغ);
# endif

Window	    x11_window = 0;
Display	    *x11_display = عدم;
#endif

ساكن صحيح ignore_sigtstp = FALSE;

ساكن صحيح get_x11_title(صحيح);

ساكن char_u	*oldtitle = عدم;
ساكن متطاير sig_atomic_t oldtitle_outdated = FALSE;
ساكن صحيح	unix_did_set_title = FALSE;
ساكن char_u	*oldicon = عدم;
ساكن صحيح	did_set_icon = FALSE;

ساكن فراغ may_core_dump(فراغ);

تعريفنوع صحيح waitstatus;
ساكن صحيح  WaitForChar(long msec, صحيح *interrupted, صحيح ignore_input);
ساكن صحيح  WaitForCharOrMouse(long msec, صحيح *interrupted, صحيح ignore_input);
#ifdef VMS
صحيح  RealWaitForChar(صحيح, long, صحيح *, صحيح *interrupted);
#אחרת
ساكن صحيح  RealWaitForChar(صحيح, long, صحيح *, صحيح *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
ساكن صحيح do_xterm_trace(فراغ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

ساكن فراغ handle_resize(فراغ);

#אם defined(SIGWINCH)
ساكن فراغ sig_winch SIGPROTOARG;
#endif
#אם defined(SIGTSTP)
ساكن فراغ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ساكن متطاير sig_atomic_t in_mch_suspend = FALSE;
#endif
#אם defined(SIGINT)
ساكن فراغ catch_sigint SIGPROTOARG;
#endif
#אם defined(SIGUSR1)
ساكن فراغ catch_sigusr1 SIGPROTOARG;
#endif
#אם defined(SIGPWR)
ساكن فراغ catch_sigpwr SIGPROTOARG;
#endif
#אם defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
ساكن فراغ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ساكن متطاير sig_atomic_t sig_alarm_called;
#endif
ساكن فراغ deathtrap SIGPROTOARG;

ساكن فراغ catch_int_signal(فراغ);
ساكن فراغ set_signals(فراغ);
ساكن فراغ catch_signals(فراغ (*func_deadly)(صحيح), فراغ (*func_other)(صحيح));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#אחרת
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	עשה { /**/ } כלעוד (0)
# define UNBLOCK_SIGNALS(set)	עשה { /**/ } כלעוד (0)
#endif
ساكن صحيح  have_wildcard(صحيح, char_u **);
ساكن صحيح  have_dollars(صحيح, char_u **);

ساكن صحيح save_patterns(صحيح num_pat, char_u **pat, صحيح *num_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
ساكن متطاير sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ساكن متطاير sig_atomic_t got_tstp = FALSE;
ساكن char_u	*extra_shell_arg = عدم;
ساكن صحيح	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ساكن متطاير sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ساكن متطاير sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#אם defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
ساكن صحيح dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ساكن tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
تعريفنوع بنية
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    حرف *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ساكن xsmp_config_T xsmp;
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

ساكن بنية signalinfo
{
    صحيح	    sig;	// Signal number, eg. SIGSEGV etc
    حرف    *name;	// Signal name (not char_u!).
    حرف    deadly;	// Catch as a deadly signal?
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
#אם defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#אם defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#אם defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#אם defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(صحيح sig, sighandler_T func)
{
#אם defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    بنية sigaction	sa, old;
    sigset_t		curset;
    صحيح			blocked;

    אם (sigprocmask(SIG_BLOCK, عدم, &curset) == -1)
	החזר SIG_ERR;

    blocked = sigismember(&curset, sig);

    אם (func == SIG_HOLD)
    {
	אם (blocked)
	    החזר SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	אם (sigaction(sig, عدم, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, عدم) == -1)
	    החזר SIG_ERR;
	החזר old.sa_handler;
    }

    אם (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	אם (sigprocmask(SIG_UNBLOCK, &curset, عدم) == -1)
	    החזר SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# אחרת
    sa.sa_flags = 0;
# endif
    אם (sigaction(sig, &sa, &old) == -1)
	החזר SIG_ERR;
    החזר blocked ? SIG_HOLD: old.sa_handler;
#وإلاإذا defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    החזר sigset(sig, func);
#אחרת
    // Oldest and most compatible solution.
    החזר إشارة(sig, func);
#endif
}

    صحيح
mch_chdir(حرف *path)
{
    אם (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    החזר chdir(vms_fixfilename(path));
#אחרת
    החזר chdir(path);
#endif
}

#אם defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    فراغ
mch_write(char_u *s, صحيح len)
{
    vim_ignored = (صحيح)اكتب(1, (حرف *)s, len);
    אם (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, عدم, عدم);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ساكن صحيح
resize_func(صحيح check_only)
{
    אם (check_only)
	החזר do_resize;
    כלעוד (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(عدم, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    החזר FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    صحيح
mch_inchar(
    char_u	*buf,
    صحيح		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt)
{
    החזר inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ساكن فراغ
handle_resize(فراغ)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    صحيح
mch_char_avail(فراغ)
{
    החזר WaitForChar(0L, عدم, FALSE);
}

#אם defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحيح
mch_check_messages(فراغ)
{
    החזר WaitForChar(0L, عدم, TRUE);
}
#endif

#אם defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# אם defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# אם defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(صحيح special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	بنية vm_statistics64	vm_stat;
	natural_t		count = HOST_VM_INFO64_COUNT;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &count);
#  אחרת
	بنية vm_statistics	vm_stat;
	natural_t		count = HOST_VM_INFO_COUNT;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &count);
#  endif
	אם (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_count + vm_stat.active_count
					    + vm_stat.inactive_count
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_count
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    אם (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	صحيح		mib[2];
	size_t		len = حجم(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  אחרת
	// sysctl() may return 32 bit or 64 bit, accept both
	اتحاد {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  אחרת
	mib[1] = HW_USERMEM;
#  endif
	אם (sysctl(mib, 2, &physmem, &len, عدم, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  אחרת
	    אם (len == حجم(physmem.u64))
		mem = (long_u)physmem.u64;
	    אחרת
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# אם defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    אם (mem == 0)
    {
	بنية sysinfo sinfo;

	// Linux way of getting amount of RAM available
	אם (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    כלעוד (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  אחרת
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    אם (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	אם (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    כלעוד (shiftright > 0 && (pagesize & 1) == 0)
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
# אם defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	بنية rlimit	rlp;

	אם (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (حجم(long_u) * 8 - 1))
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

    אם (mem > 0)
	החזר mem >> shiftright;
    החזר (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    فراغ
mch_delay(long msec, صحيح flags)
{
    tmode_T	old_tmode;
    صحيح		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    אם (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	אם (call_settmode)
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
	עשה
	{
	    // if total is large enough, wait by portions in p_mzq
	    אם (total > p_mzq)
		msec = p_mzq;
	    אחרת
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    بنية timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (فراغ)nanosleep(&ts, عدم);
	}
#אחרת
# ifdef HAVE_USLEEP
	כלעוד (msec >= 1000)
	{
	    usleep((unsigned صحيح)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned صحيح)(msec * 1000));
# אחרת
#  ifndef HAVE_SELECT
	poll(عدم, 0, (صحيح)msec);
#  אחרת
	{
	    بنية timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, عدم, عدم, عدم, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	כלעוד (total > 0);
#endif

	אם (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    אחרת
	WaitForChar(msec, عدم, FALSE);
}

#אם defined(HAVE_STACK_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_STACK_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ساكن صحيح stack_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ساكن فراغ
check_stack_growth(حرف *p)
{
    صحيح		i;

    stack_grows_downwards = (p > (حرف *)&i);
}
#endif

#אם defined(HAVE_STACK_LIMIT)
ساكن حرف *stack_limit = عدم;

# אם defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ساكن فراغ
get_stack_limit(فراغ)
{
    بنية rlimit	rlp;
    صحيح			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    אם (getrlimit(RLIMIT_STACK, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (حجم(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# אם defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    אם (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		אם (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	אם (stack_grows_downwards)
	{
	    stack_limit = (حرف *)((long)&i - (lim / 16L * 15L));
	    אם (stack_limit >= (حرف *)&i)
		// overflow, set to 1/16 of current stack position
		stack_limit = (حرف *)((long)&i / 16L);
	}
	אחרת
	{
	    stack_limit = (حرف *)((long)&i + (lim / 16L * 15L));
	    אם (stack_limit <= (حرف *)&i)
		stack_limit = عدم;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    صحيح
mch_stackcheck(حرف *p)
{
    אם (stack_limit == عدم)
	החזר OK;

    אם (stack_grows_downwards)
    {
	אם (p < stack_limit)
	    החזר FAIL;
    }
    אחרת אם (p > stack_limit)
	החזר FAIL;
    החזר OK;
}
#endif

#אם defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ساكن stack_t sigstk;			// for sigaltstack()
# אחרת
ساكن بنية sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ساكن long صحيح get_signal_stack_size(فراغ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long صحيح size = -1;

    // return size only if sysconf doesn't return an error
    אם ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	החזר size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    החזר SIGSTKSZ;
# endif

    // otherwise guess the size
    החזר 8000;
}

ساكن حرف *signal_stack;

    ساكن فراغ
init_signal_stack(فراغ)
{
    אם (signal_stack == عدم)
	החזר;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_stack;
#  אחרת
    sigstk.ss_sp = signal_stack;
#  endif
    sigstk.ss_size = get_signal_stack_size();
    sigstk.ss_flags = 0;
    (فراغ)sigaltstack(&sigstk, عدم);
# אחרת
    sigstk.ss_sp = signal_stack;
    אם (stack_grows_downwards)
	sigstk.ss_sp += get_signal_stack_size() - 1;
    sigstk.ss_onstack = 0;
    (فراغ)sigstack(&sigstk, عدم);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#אם defined(SIGWINCH)
    ساكن فراغ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#אם defined(SIGTSTP)
    ساكن فراغ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    אם (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ارفع(sigarg);
    }
    אחרת
	got_tstp = TRUE;

# אם !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#אם defined(SIGINT)
    ساكن فراغ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#אם defined(SIGUSR1)
    ساكن فراغ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#אם defined(SIGPWR)
    ساكن فراغ
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
    ساكن فراغ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#אם defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ساكن JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ساكن متطاير sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ساكن متطاير sig_atomic_t lc_active = FALSE;

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
    ساكن فراغ
mch_startjmp(فراغ)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    ساكن فراغ
mch_endjmp(فراغ)
{
    lc_active = FALSE;
}

    ساكن فراغ
mch_didjmp(فراغ)
{
# אם defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_stack();
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
    ساكن فراغ
deathtrap SIGDEFARG(sigarg)
{
    ساكن صحيح	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    صحيح		i;
#endif

#אם defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    אם (lc_active)
    {
# אם defined(SIGHASARG)
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
    אם (in_mch_delay && sigarg == SIGQUIT)
	החזר;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    אם (entered == 0
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
	החזר;
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

#ifdef HAVE_STACK_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_stack_limit();
#endif

#אם 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	אם (fp)
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
    עבור (i = 0; signal_info[i].sig != -1; i++)
	אם (sigarg == signal_info[i].sig)
	    اكسر;
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
    אם (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	אם (entered >= 4)
	    _exit(8);
	exit(7);
    }
    אם (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#אחרת
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal\r\n");
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
    ساكن فراغ
after_sigcont(فراغ)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#אם defined(SIGCONT)
ساكن فراغ sigcont_handler SIGPROTOARG;

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
ساكن متطاير sig_atomic_t sigcont_received;
ساكن فراغ sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ساكن فراغ
sigcont_handler SIGDEFARG(sigarg)
{
    אם (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    אחרת
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

#אם defined(FEAT_CLIPBOARD)
# אם defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ساكن فراغ *clip_star_save = عدم;
ساكن فراغ *clip_plus_save = عدم;
# endif

# אם defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ساكن فراغ
loose_clipboard(فراغ)
{
    אם (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	אם (clip_star.owned)
	    clip_lose_selection(&clip_star);
	אם (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	אם (x11_display != عدم)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# אם defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ساكن فراغ
save_clipboard(فراغ)
{
    אם (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    אם (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ساكن فراغ
restore_clipboard(فراغ)
{
    אם (clip_star_save != عدم)
    {
	אם (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	אחרת
	    free_register(clip_star_save);
	clip_star_save = عدم;
    }
    אם (clip_plus_save != عدم)
    {
	אם (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	אחרת
	    free_register(clip_plus_save);
	clip_plus_save = عدم;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    אם (ignore_sigtstp)
	החזר;

#אם defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# אם defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# אם defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# אם defined(SIGCONT)
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

	עבור (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#אחרת
    suspend_shell();
#endif
}

    فراغ
mch_init(فراغ)
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

    فراغ
set_sigwinch_handler(فراغ)
{
#אם defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    ساكن فراغ
set_signals(فراغ)
{
#אם defined(SIGWINCH)
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
#אם defined(SIGCONT)
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

#אם defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    אם (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#אם defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ساكن فراغ
catch_int_signal(فراغ)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    فراغ
reset_signals(فراغ)
{
    catch_signals(SIG_DFL, SIG_DFL);
#אם defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    ساكن فراغ
catch_signals(
    فراغ (*func_deadly)(صحيح),
    فراغ (*func_other)(صحيح))
{
    صحيح	    i;

    עבור (i = 0; signal_info[i].sig != -1; i++)
    {
	אם (signal_info[i].deadly)
	{
#אם defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    بنية sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# אם defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# אחרת
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, عدم);
#אחרת
# אם defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    بنية sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, عدم);
# אחרת
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	אחרת אם (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#אחרת
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    ساكن فراغ
block_signals(sigset_t *set)
{
    sigset_t	newset;
    صحيح		i;

    sigemptyset(&newset);

    עבור (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# אם defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    ساكن فراغ
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, عدم);
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
    صحيح
vim_handle_signal(صحيح sig)
{
    ساكن صحيح got_signal = 0;
    ساكن صحيح blocked = TRUE;

    بدل (sig)
    {
	حالة SIGNAL_BLOCK:   blocked = TRUE;
			     اكسر;

	حالة SIGNAL_UNBLOCK: blocked = FALSE;
			     אם (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     اكسر;

	افتراضي:	     אם (!blocked)
				 החזר TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     אם (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     اكسر;
    }
    החזר FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    صحيح
mch_check_win(صحيح argc UNUSED, حرف **argv UNUSED)
{
    אם (isatty(1))
	החזר OK;
    החזר FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحيح
mch_input_isatty(فراغ)
{
    אם (isatty(read_cmd_fd))
	החזר TRUE;
    החזר FALSE;
}

#ifdef FEAT_X11

# אם defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ساكن فراغ
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#אם defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ساكن صحيح	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ساكن صحيح
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (حرف *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    אם (!gui.in_use)
# endif
	msg((حرف *)IObuff);

    החזר 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ساكن صحيح
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    החזר 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ساكن صحيح
x_connect_to_server(فراغ)
{
    // No point in connecting if we are exiting or dying.
    אם (exiting || v_dying)
	החזר FALSE;

# אם defined(FEAT_CLIENTSERVER)
    אם (x_force_connect)
	החזר TRUE;
# endif
    אם (x_no_connect)
	החזר FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    אם (clip_exclude_prog != عدم)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	אם (regprog_in_use(clip_exclude_prog))
	    החזר FALSE;

	אם (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    החזר FALSE;
    }
    החזר TRUE;
}

# אם defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  אם defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ساكن صحيح
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   אם defined(VMS) || defined(__CYGWIN__)
    החזר 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ساكن صحيح xterm_dpy_retry_count = 0;

    ساكن صحيح
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = عدم;
    xterm_dpy_retry_count = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = عدم;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  אם defined(VMS) || defined(__CYGWIN__)
    החזר 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ساكن فراغ
may_restore_x11_clipboard(فراغ)
{
    // No point in restoring the connecting if we are exiting or dying.
    אם (!exiting && !v_dying && xterm_dpy_retry_count > 0)
    {
	--xterm_dpy_retry_count;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	אם (app_context != (XtAppContext)عدم)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)عدم;
	    x11_display = عدم; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    فراغ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    אם (eap->arg != عدم && (arglen = STRLEN(eap->arg)) > 0)
    {
	אם (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (حرف *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == عدم
		    ? (حرف *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_count = 5;  // Try reconnecting five times
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
    ساكن صحيح
test_x11_window(Display *dpy)
{
    صحيح			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    אם (XGetWMName(dpy, x11_window, &text_prop))
	XFree((فراغ *)text_prop.value);
    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    אם (p_verbose > 0 && got_x_error)
	verb_msg(_("Testing the X display failed"));

    החזר (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

ساكن صحيح get_x11_thing(صحيح get_title, صحيح test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ساكن صحيح
get_x11_windis(فراغ)
{
    حرف	    *winid;
    ساكن صحيح	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ساكن صحيح	    x11_display_from = XD_NONE;
    ساكن صحيح	    did_set_error_handler = FALSE;

    אם (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(فراغ)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# אם defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    אם (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	אם (x11_display_from == XD_HERE && x11_display != عدم)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	אם (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    החזר OK;
	}
	x11_display = عدم;
	החזר FAIL;
    }
    אחרת אם (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = عدم;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    אם (!x_connect_to_server())
	החזר FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    אם (x11_window == 0 && (winid = getenv("WINDOWID")) != عدم)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    אם (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    אם (xterm_dpy != عدم && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	אם (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    אם (x11_display_from == XD_HERE && x11_display != عدم)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	אם (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = عدم;
	    x11_display_from = XD_NONE;
	    החזר FAIL;
	}
	החזר OK;
    }
# endif

    אם (x11_window == 0 || x11_display == عدم)
	result = -1;

    אם (result != -1)	    // Have already been here and set this
	החזר result;	    // Don't do all these X calls again

    אם (x11_window != 0 && x11_display == عدم)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	אם (p_verbose > 0)
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
	x11_display = XOpenDisplay(عدم);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	אם (p_verbose > 0 && sig_alarm_called)
	    verb_msg(_("Opening the X display timed out"));
# endif
	אם (x11_display != عدم)
	{
# ifdef ELAPSED_FUNC
	    אם (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    אם (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = عدم;
	    }
	    אחרת
		x11_display_from = XD_HERE;
	}
    }
    אם (x11_window == 0 || x11_display == عدم)
	החזר (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    החזר (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    ساكن صحيح
get_x11_title(صحيح test_only)
{
    החזר get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ساكن صحيح
get_x11_icon(صحيح test_only)
{
    صحيح		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    אם (oldicon == عدم && !test_only)
    {
	אם (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	אחרת
	    oldicon = vim_strsave(T_NAME);
    }

    החזר retval;
}

    ساكن صحيح
get_x11_thing(
    صحيح		get_title,	// get title string
    صحيح		test_only)
{
    XTextProperty	text_prop;
    صحيح			retval = FALSE;
    Status		status;

    אם (get_x11_windis() != OK)
	החזר FALSE;

    // Get window/icon name if any
    אם (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    אחרת
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
    Window	    root;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned صحيح    num_children;

    כלעוד (!status || text_prop.value == عدم)
    {
	אם (!XQueryTree(x11_display, win, &root, &parent, &children,
		    &num_children))
	    اكسر;
	אם (children)
	    XFree((فراغ *)children);
	אם (parent == root || parent == 0)
	    اكسر;

	win = parent;
	אם (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	אחרת
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    אם (status && text_prop.value != عدم)
    {
	retval = TRUE;
	אם (!test_only)
	{
	    אם (get_title)
		vim_free(oldtitle);
	    אחרת
		vim_free(oldicon);
	    אם (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		אם (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.value);
		אחרת
		    oldicon = vim_strsave((char_u *)text_prop.value);
	    }
	    אחרת
	    {
		حرف    **cl;
		Status  transform_status;
		صحيح	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		אם (transform_status >= Success && n > 0 && cl[0])
		{
		    אם (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    אחרת
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		אחרת
		{
		    אם (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.value);
		    אחרת
			oldicon = vim_strsave((char_u *)text_prop.value);
		}
	    }
	}
	XFree((فراغ *)text_prop.value);
    }
    החזר retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# אם defined(X_HAVE_UTF8_STRING)
#  אם X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ساكن فراغ
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    אם (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ثابت حرف *)title,
					     عدم, عدم, 0, عدم, عدم, عدم);
    אחרת
# endif
    {
# אם XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ثابت حرف *)title,
					     عدم, عدم, 0, عدم, عدم, عدم);
#  אחרת
	XTextProperty	text_prop;
	حرف		*c_title = (حرف *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(فراغ)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     عدم, عدم, 0, عدم, عدم, عدم);
#  endif
# אחרת
	XStoreName(x11_display, x11_window, (حرف *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ساكن فراغ
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    אם (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, عدم, (ثابت حرف *)icon,
						   عدم, 0, عدم, عدم, عدم);
    אחרת
# endif
    {
# אם XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, عدم, (ثابت حرف *)icon,
						   عدم, 0, عدم, عدم, عدم);
#  אחרת
	XTextProperty	text_prop;
	حرف		*c_icon = (حرف *)icon;

	(فراغ)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, عدم, &text_prop,
						   عدم, 0, عدم, عدم, عدم);
#  endif
# אחרת
	XSetIconName(x11_display, x11_window, (حرف *)icon);
# endif
    }
    XFlush(x11_display);
}

#אחרת  // FEAT_X11

    ساكن صحيح
get_x11_title(صحيح test_only UNUSED)
{
    החזר FALSE;
}

    ساكن صحيح
get_x11_icon(صحيح test_only)
{
    אם (!test_only)
    {
	אם (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	אחרת
	    oldicon = vim_strsave(T_NAME);
    }
    החזר FALSE;
}

#endif // FEAT_X11

    صحيح
mch_can_restore_title(فراغ)
{
    החזר get_x11_title(TRUE);
}

    صحيح
mch_can_restore_icon(فراغ)
{
    החזר get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    فراغ
mch_settitle(char_u *title, char_u *icon)
{
    صحيح		type = 0;
    ساكن صحيح	recursive = 0;

    אם (T_NAME == عدم)	    // no terminal name (yet)
	החזר;
    אם (title == عدم && icon == عدم)	    // nothing to do
	החזר;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    אם (recursive)
	החזר;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    אם (get_x11_windis() == OK)
	type = 1;
#endif
#אם defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    אם (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    אם ((type || *T_TS != NUL) && title != عدم)
    {
	אם (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	אם (oldtitle == عدم
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (فراغ)get_x11_title(FALSE);

	אם (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	אחרת
# ifdef FEAT_GUI_GTK
	אם (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#אם defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	אחרת
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    אם ((type || *T_CIS != NUL) && icon != عدم)
    {
	אם (oldicon == عدم
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	אם (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	אחרת
# ifdef FEAT_GUI_GTK
	אם (!gui.in_use)		// don't do this if GTK+ is running
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
    فراغ
mch_restore_title(صحيح which)
{
    صحيح	do_push_pop = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : عدم;
    char_u *tofree = عدم;
    אם (title == oldtitle && oldtitle != عدم)
    {
	tofree = vim_strsave(title);
	אם (tofree != عدم)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : عدم);
    vim_free(tofree);

    אם (do_push_pop)
    {
	// pop and push from/to the stack
	term_pop_title(which);
	term_push_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    صحيح
vim_is_xterm(char_u *name)
{
    אם (name == عدم)
	החזר FALSE;
    החזר ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#אם defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    صحيح
use_xterm_like_mouse(char_u *name)
{
    החזר (name != عدم
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
    صحيح
use_xterm_mouse(فراغ)
{
    אם (ttym_flags == TTYM_SGR)
	החזר 4;
    אם (ttym_flags == TTYM_URXVT)
	החזר 3;
    אם (ttym_flags == TTYM_XTERM2)
	החזר 2;
    אם (ttym_flags == TTYM_XTERM)
	החזר 1;
    החזר 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    صحيح
vim_is_iris(char_u *name)
{
    אם (name == عدم)
	החזר FALSE;
    החזר (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#אם defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    صحيح
vim_is_vt300(char_u *name)
{
    אם (name == عدم)
	החזר FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    החזר ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != عدم)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    صحيح
mch_get_user_name(char_u *s, صحيح len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(عدم), len - 1);
    החזר OK;
#אחרת
    החזר mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    صحيح
mch_get_uname(uid_t uid, char_u *s, صحيح len)
{
#אם defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    بنية passwd   *pw;

    אם ((pw = getpwuid(uid)) != عدم
	    && pw->pw_name != عدم && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	החזר OK;
    }
#endif
    sprintf((حرف *)s, "%d", (صحيح)uid);	    // assumes s is long enough
    החזר FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    فراغ
mch_get_host_name(char_u *s, صحيح len)
{
    بنية utsname vutsname;

    אם (uname(&vutsname) < 0)
	*s = NUL;
    אחרת
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#אחרת // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    فراغ
mch_get_host_name(char_u *s, صحيح len)
{
# ifdef VAXC
    vaxc$gethostname((حرف *)s, len);
# אחרת
    gethostname((حرف *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(فراغ)
{
    החזר (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحيح
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    אם (kill(pid, 0) == 0)
	החזר TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    אם (errno == ESRCH)
	החזר FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    החזר TRUE;
}

#אם !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ساكن حرف *
strerror(صحيح err)
{
    خارجي صحيح	    sys_nerr;
    خارجي حرف	    *sys_errlist[];
    ساكن حرف	    er[20];

    אם (err > 0 && err < sys_nerr)
	החזר (sys_errlist[err]);
    snprintf(er, حجم(er), "Error %d", err);
    החזר er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(char_u *buf, صحيح len)
{
#אם defined(USE_GETCWD)
    אם (getcwd((حرف *)buf, len) == عدم)
    {
	STRCPY(buf, strerror(errno));
	החזר FAIL;
    }
    החזר OK;
#אחרת
    החזר (getwd((حرف *)buf) != عدم ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    صحيح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)		// also expand when already absolute path
{
    صحيح		buflen = 0;
#ifdef HAVE_FCHDIR
    صحيح		fd = -1;
    ساكن صحيح	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    صحيح		retval = OK;
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
# אם CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# אחרת
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    אם ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == عدم || p != fname))
    {
	אם (p == عدم && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	אם (p != عدم)
	{
	    אם (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    אם (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		אם (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    אם (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((حرف *)olddir) != 0))
	    {
		p = عدم;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    אחרת
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		אם (p - fname >= len)
		    retval = FAIL;
		אחרת
		{
		    vim_strncpy(buf, fname, p - fname);
		    אם (mch_chdir((حرف *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			אם (mch_isFullName(fname))
			    retval = FAIL;
			אחרת
			    p = عدم;
		    }
		    אחרת אם (*p == '/')
			fname = p + 1;
		    אחרת
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	אם (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	אם (p != عدم)
	{
	    صحيح	l;

#ifdef HAVE_FCHDIR
	    אם (fd >= 0)
	    {
		אם (p_verbose >= 5)
		{
		    verbose_enter();
		    msg("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    אחרת
#endif
		l = mch_chdir((حرف *)olddir);
	    אם (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	אם (fd >= 0)
	    close(fd);
#endif

	buflen = (صحيح)STRLEN(buf);
	אם (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	אחרת אם (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += حجم(PATHSEP);
	}
#endif
    }

    אם (buflen == 0)
	buflen = (صحيح)STRLEN(buf);

    // Catch file names which are too long.
    אם (retval == FAIL || (صحيح)(buflen + STRLEN(fname)) >= len)
	החזר FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    אם (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    החזר OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحيح
mch_isFullName(char_u *fname)
{
#ifdef VMS
    החזר ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((حرف *)fname,':') || strchr((حرف *)fname,'"') ||
	    (strchr((حرف *)fname,'[') && strchr((حرف *)fname,']'))||
	    (strchr((حرف *)fname,'<') && strchr((حرف *)fname,'>'))   );
#אחרת
    החזר (*fname == '/' || *fname == '~');
#endif
}

#אם defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    فراغ
fname_case(
    char_u	*name,
    صحيح		len UNUSED)  // buffer size, only used when name gets longer
{
    بنية stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    بنية dirent *dp;

    אם (mch_lstat((حرف *)name, &st) < 0)
	החזר;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    אם (slash == عدم)
    {
	dirp = opendir(".");
	tail = name;
    }
    אחרת
    {
	*slash = NUL;
	dirp = opendir((حرف *)name);
	*slash = '/';
	tail = slash + 1;
    }

    אם (dirp == عدم)
	החזר;

    taillen = STRLEN(tail);
    כלעוד ((dp = readdir(dirp)) != عدم)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	אם (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    بنية stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    אם (mch_lstat((حرف *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		اكسر;
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
    بنية stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    אם (stat((حرف *)vms_fixfilename(name), &statb))
#אחרת
    אם (stat((حرف *)name, &statb))
#endif
	החזר -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    החזר statb.st_mode & ~S_ADDACE;
#אחרת
    החזר statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_setperm(char_u *name, long perm)
{
    החזר (chmod((حرف *)
#ifdef VMS
		    vms_fixfilename(name),
#אחרת
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#אם defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_fsetperm(صحيح fd, long perm)
{
    החזר (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#אם defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
تعريفنوع بنية vim_acl_solaris_T {
    صحيح acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# אם defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    فراغ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    אם (from_file == عدم)
	החזר;

    אם (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    אם (selinux_enabled <= 0)
	החזר;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    حرف *from_context = عدم;
    حرف *to_context = عدم;

    אם (getfilecon((حرف *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	אם (errno == EOPNOTSUPP)
	    החזר;

	msg_puts(_("\nCould not get security context for "));
	msg_outtrans(from_file);
	msg_putchar('\n');
	החזר;
    }
    אם (getfilecon((حرف *)to_file, &to_context) < 0)
    {
	msg_puts(_("\nCould not get security context for "));
	msg_outtrans(to_file);
	msg_putchar('\n');
	freecon (from_context);
	החזר ;
    }
    אם (strcmp(from_context, to_context) != 0)
    {
	אם (setfilecon((حرف *)to_file, from_context) < 0)
	{
	    msg_puts(_("\nCould not set security context for "));
	    msg_outtrans(to_file);
	    msg_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# אם defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    فراغ
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    ساكن ثابت حرف * ثابت smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    حرف	buffer[SMACK_LABEL_LEN];
    ثابت حرف	*name;
    صحيح		index;
    صحيح		ret;
    ssize_t	size;

    אם (from_file == عدم)
	החזר;

    size = listxattr((حرف *)from_file, عدم, 0);
    // not supported or no attributes to copy
    אם (size <= 0)
	החזר;

    עבור (index = 0 ; index < (صحيح)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((حرف*)from_file, name, buffer, حجم(buffer));
	אם (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((حرف*)to_file, name, buffer, (size_t)size, 0);
	    אם (ret < 0)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		msg_outtrans(IObuff);
		msg_putchar('\n');
	    }
	}
	אחרת
	{
	    // what reason of not having the attribute value?
	    بدل (errno)
	    {
		حالة ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    החזר; // leave because it isn't useful to continue

		حالة ERANGE:
		افتراضي:
		    // no enough size OR unexpected error
		     vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    msg_puts((حرف *)IObuff);
		    msg_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		حالة ENODATA:
		    // no attribute of this name
		    ret = removexattr((حرف*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    اكسر;
	    }
	}
    }
}
# endif // HAVE_SMACK

# אם defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    فراغ
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    حرف	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    حرف	*key;
    حرف	*val = عدم;
    حرف	*errmsg = عدم;

    אם (from_file == عدم)
	החזר;

    // get the length of the extended attributes
    size = listxattr((حرف *)from_file, عدم, 0);
    // not supported or no attributes to copy
    אם (size <= 0)
	החזר;
    xattr_buf = (حرف*)alloc(size);
    אם (xattr_buf == عدم)
	החזר;
    size = listxattr((حرف *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    עבור (صحيح round = 0; round < 2; round++)
    {

	key = xattr_buf;
	אם (round == 1)
	    size = tsize;

	כלעוד (size > 0)
	{
	    vallen = getxattr((حرف *)from_file, key,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    אם (vallen >= 0 && round &&
		setxattr((حرف *)to_file, key, val, vallen, 0) == 0)
		;
	    אחרת אם (errno)
	    {
		بدل (errno)
		{
		    حالة E2BIG:
			errmsg = e_xattr_e2big;
			اذهب error_exit;
		    حالة ENOTSUP:
		    حالة EACCES:
		    حالة EPERM:
			اكسر;
		    حالة ERANGE:
			errmsg = e_xattr_erange;
			اذهب error_exit;
		    افتراضي:
			errmsg = e_xattr_other;
			اذهب error_exit;
		}
	    }

	    אם (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(key) + 1;
	    size -= keylen;
	    key += keylen;
	}
	אם (round)
	    اكسر;

	val = (حرف*)alloc(max_vallen + 1);
	אם (val == عدم)
	    اذهب error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    אם (errmsg != عدم)
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
    vim_acl_T	ret = عدم;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((حرف *)fname, ACL_TYPE_ACCESS);
# אחרת
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    אם (acl_get((حرف *)fname, 0, &aclent) < 0)
	החזר عدم;
    ret = (vim_acl_T)aclent;
#  אחרת
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(حجم(vim_acl_solaris_T));
    אם (aclent == عدم)
	החזר عدم;
    אם ((aclent->acl_cnt = acl((حرف *)fname, GETACLCNT, 0, عدم)) < 0)
    {
	حرر(aclent);
	החזר عدم;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * حجم(aclent_t));
    אם (aclent->acl_entry == عدم)
    {
	حرر(aclent);
	החזר عدم;
    }
    אם (acl((حرف *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	حرر(aclent->acl_entry);
	حرر(aclent);
	החזר عدم;
    }
    ret = (vim_acl_T)aclent;
#   אחרת
#    אם defined(HAVE_AIX_ACL)
    صحيح		aclsize;
    بنية acl *aclent;

    aclsize = حجم(بنية acl);
    aclent = malloc(aclsize);
    אם (aclent == عدم)
	החזר عدم;
    אם (statacl((حرف *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	אם (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    אם (aclent == عدم || statacl((حرف *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		حرر(aclent);
		החזר عدم;
	    }
	}
	אחרת
	{
	    حرر(aclent);
	    החזר عدم;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    החזר ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    فراغ
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    אם (aclent == عدم)
	החזר;
# ifdef HAVE_POSIX_ACL
    acl_set_file((حرف *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# אחרת
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((حرف *)fname, (acl_t *)aclent);
#  אחרת
#   ifdef HAVE_SOLARIS_ACL
    acl((حرف *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   אחרת
#    ifdef HAVE_AIX_ACL
    chacl((حرف *)fname, aclent, ((بنية acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    فراغ
mch_free_acl(vim_acl_T aclent)
{
    אם (aclent == عدم)
	החזר;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# אחרת
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  אחרת
#   ifdef HAVE_SOLARIS_ACL
    حرر(((vim_acl_solaris_T *)aclent)->acl_entry);
    حرر(aclent);
#   אחרת
#    ifdef HAVE_AIX_ACL
    حرر(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحيح
mch_isdir(char_u *name)
{
    بنية stat statb;

    אם (*name == NUL)	    // Some stat()s don't flag "" as an error.
	החזר FALSE;
    אם (stat((حرف *)name, &statb))
	החזר FALSE;
    החזר (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحيح
mch_isrealdir(char_u *name)
{
    بنية stat statb;

    אם (*name == NUL)	    // Some stat()s don't flag "" as an error.
	החזר FALSE;
    אם (mch_lstat((حرف *)name, &statb))
	החזר FALSE;
    החזר (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ساكن صحيح
executable_file(char_u *name)
{
    بنية stat	st;

    אם (stat((حرف *)name, &st))
	החזר 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    صحيح vms_executable = 0;
    אם (S_ISREG(st.st_mode) && mch_access((حرف *)name, X_OK) == 0)
    {
	אם (strstr(vms_tolower((حرف*)name),".exe") != عدم
		|| strstr(vms_tolower((حرف*)name),".com")!= عدم)
	    vms_executable = 1;
    }
    החזר vms_executable;
#אחרת
    החזר S_ISREG(st.st_mode) && mch_access((حرف *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحيح
mch_can_exe(char_u *name, char_u **path, صحيح use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    صحيح		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    אם (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	אם ((use_path || gettail(name) != name) && executable_file(name))
	{
	    אם (path != عدم)
	    {
		אם (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		אחרת
		    *path = vim_strsave(name);
	    }
	    החזר TRUE;
	}
	החזר FALSE;
    }

    p = (char_u *)getenv("PATH");
    אם (p == عدم || *p == NUL)
	החזר -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    אם (buf == عدم)
	החזר -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    עבור (;;)
    {
	e = (char_u *)strchr((حرف *)p, ':');
	אם (e == عدم)
	    e = p_end;
	elen = (size_t)(e - p);
	אם (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((حرف *)buf, bufsize, "%.*s%s%s",
		(صحيح)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	אם (retval == 1)
	{
	    אם (path != عدم)
	    {
		אם (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		אחרת
		    *path = vim_strnsave(buf, buflen);
	    }
	    اكسر;
	}

	אם (*e != ':')
	    اكسر;
	p = e + 1;
    }

    vim_free(buf);
    החזר retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name)
{
    بنية stat	st;

    אם (stat((حرف *)name, &st))
	החזר NODE_NORMAL;
    אם (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	החזר NODE_NORMAL;
    אם (S_ISBLK(st.st_mode))	// block device isn't writable
	החזר NODE_OTHER;
    // Everything else is writable?
    החזר NODE_WRITABLE;
}

    فراغ
mch_early_init(فراغ)
{
#ifdef HAVE_CHECK_STACK_GROWTH
    صحيح			i;

    check_stack_growth((حرف *)&i);

# ifdef HAVE_STACK_LIMIT
    get_stack_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#אם defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_stack = alloc(get_signal_stack_size());
    init_signal_stack();
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

#אם defined(EXITFREE)
    فراغ
mch_free_mem(فراغ)
{
# אם defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    אם (clip_star.owned)
	clip_lose_selection(&clip_star);
    אם (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# אם defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    אם (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    אם (xterm_dpy != عدم)
	XtCloseDisplay(xterm_dpy);
    אם (app_context != (XtAppContext)عدم)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = عدم; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# אם defined(FEAT_X11)
    אם (x11_display != عدم
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# אם defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_stack);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ساكن فراغ
exit_scroll(فراغ)
{
    אם (silent_mode)
	החזר;
    אם (newline_on_exit || msg_didout)
    {
	אם (msg_use_printf())
	{
	    אם (info_message)
		mch_msg("\n");
	    אחרת
		mch_errmsg("\r\n");
	}
	אחרת
	    out_char('\n');
    }
    אחרת אם (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	msg_clr_eos_force();		// clear the rest of the display
	windgoto((صحيح)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# אם (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
خارجي فراغ __gcov_dump(فراغ);
خارجي فراغ __gcov_reset(فراغ);
#  define __gcov_flush() עשה { __gcov_dump(); __gcov_reset(); } כלעוד (0)
# אחרת
خارجي فراغ __gcov_flush(فراغ);
# endif
#endif

    فراغ
mch_exit(صحيح r)
{
    exiting = TRUE;

#אם defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    אם (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	אם (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_pop_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	אם (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	אם (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	אם (full_screen)
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
    אם (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    אם (deadly_signal != 0)
	החזר;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    ساكن فراغ
may_core_dump(فراغ)
{
    אם (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ساكن صحيح
get_tty_fd(صحيح fd)
{
    صحيح		tty_fd = fd;

# אם defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    אם (mch_isatty(fd) > 1)
    {
	حرف *name;

	name = ptsname(fd);
	אם (name == عدم)
	    החזר -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	אם (tty_fd < 0)
	    החזר -1;
    }
# endif
    החזר tty_fd;
}

    ساكن صحيح
mch_tcgetattr(صحيح fd, فراغ *term)
{
    صحيح		tty_fd;
    صحيح		retval = -1;

    tty_fd = get_tty_fd(fd);
    אם (tty_fd < 0)
	החזר -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (بنية termios *)term);
#  אחרת
    retval = ioctl(tty_fd, TCGETA, (بنية termio *)term);
#  endif
# אחרת
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (بنية sgttyb *)term);
# endif
    אם (tty_fd != fd)
	close(tty_fd);
    החזר retval;
}

    فراغ
mch_settmode(tmode_T tmode)
{
    ساكن صحيح first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ساكن بنية termios told;
	   بنية termios tnew;
#  אחרת
    ساكن بنية termio told;
	   بنية termio tnew;
#  endif

    אם (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    אם (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == عدم || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  אם defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   אחרת
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    אחרת
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    אחרת אם (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  אם defined(HAVE_TERMIOS_H)
    {
	صحيح	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	כלעוד (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  אחרת
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# אחרת
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ساكن بنية sgttyb ttybold;
	   بنية sgttyb ttybnew;

    אם (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    אם (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    אחרת אם (tmode == TMODE_SLEEP)
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
    فراغ
get_stty(فراغ)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    אם (get_tty_info(read_cmd_fd, &info) != OK)
	החזר;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    אם (p != عدم && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(عدم);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    صحيح
get_tty_info(صحيح fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    بنية termios keys;
#  אחרת
    بنية termio keys;
#  endif

    אם (mch_tcgetattr(fd, &keys) != -1)
    {
	info->backspace = keys.c_cc[VERASE];
	info->interrupt = keys.c_cc[VINTR];
	אם (keys.c_iflag & ICRNL)
	    info->enter = NL;
	אחרת
	    info->enter = CAR;
	אם (keys.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	אחרת
	    info->nl_does_cr = FALSE;
	החזר OK;
    }
# אחרת
    // for "old" tty systems
    بنية sgttyb keys;

    אם (mch_tcgetattr(fd, &keys) != -1)
    {
	info->backspace = keys.sg_erase;
	info->interrupt = keys.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	החזר OK;
    }
# endif
    החזר FAIL;
}

#endif // VMS

ساكن صحيح	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    فراغ
mch_setmouse(صحيح on)
{
#ifdef FEAT_BEVAL_TERM
    ساكن صحيح	bevalterm_ison = FALSE;
#endif
    صحيح		xterm_mouse_vers;

#אם defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    אם (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    אם (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	החזר;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    אם (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    אם (T_CXM != عدم && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    אחרת אם (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    אם (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	אם (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    אם (xterm_mouse_vers > 0)
    {
	אם (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	אחרת	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    אחרת אם (ttym_flags == TTYM_DEC)
    {
	אם (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	אחרת	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    אחרת
    {
	אם (on)
	{
	    אם (gpm_open())
		mouse_ison = TRUE;
	}
	אחרת
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    אחרת
    {
	אם (on)
	{
	    אם (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	אחרת
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    אחרת
    {
	אם (on)
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
# אחרת
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	אחרת
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    אחרת
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	אם (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	אחרת
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#אם defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    فراغ
mch_bevalterm_changed(فراغ)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    فراغ
check_mouse_termcode(فراغ)
{
#ifdef FEAT_MOUSE_XTERM
    אם (use_xterm_mouse()
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
	אם (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    אחרת
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    אחרת
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    אחרת
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    אחרת
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    אחרת
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    אם (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    אחרת
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    אם (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	אם (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    אחרת
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    אם (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	אם (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    אחרת
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
    صحيح
mch_get_shellsize(فراغ)
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
	بنية winsize	ws;
	صحيح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	אם (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	אם (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(عدم, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# אחרת // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	بنية ttysize	ts;
	صحيح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	אם (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	אם (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_lines;
#   ifdef FEAT_EVAL
	    ch_log(عدم, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    אם (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != عدم)
    {
	אם ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((حرف *)p);
# ifdef FEAT_EVAL
	    ch_log(عدم, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	אם ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((حرف *)p);
# ifdef FEAT_EVAL
	    ch_log(عدم, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    אם (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(عدم, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    אם (columns <= 0 || rows <= 0)
	החזר FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    החזר OK;
}

# אם defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    فراغ
mch_calc_cell_size(بنية cellsize *cs_out)
{
   بنية winsize   ws;
   صحيح		    retval;
   صحيح		    x_cell_size;
   صحيح		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(عدم, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   אם (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	החזר;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   אם (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# אם defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    صحيح
mch_report_winsize(صحيح fd, صحيح rows, صحيح cols)
{
    صحيح		tty_fd;
    صحيح		retval = -1;

    tty_fd = get_tty_fd(fd);
    אם (tty_fd < 0)
	החזר FAIL;

#  אם defined(TIOCSWINSZ)
    بنية winsize ws;

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
    ch_log(عدم, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    بنية ttysize ts;

    ts.ts_cols = cols;
    ts.ts_lines = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(عدم, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    אם (tty_fd != fd)
	close(tty_fd);
    החזר retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    فراغ
mch_set_shellsize(فراغ)
{
    אם (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((صحيح)Rows, (صحيح)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ساكن pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    כלעוד (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	אם (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    אם (++delay_msec > 10)
		delay_msec = 10;
	    استمر;
	}
	אם (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    اكسر;
    }
    החזר wait_pid;
}

#אם !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ساكن فراغ
set_child_environment(
	long	rows,
	long	columns,
	حرف	*term,
	صحيح	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    حرف	envbuf[50];
# אחרת
    ساكن حرف	envbuf_Term[30];
    ساكن حرف	envbuf_Rows[20];
    ساكن حرف	envbuf_Lines[20];
    ساكن حرف	envbuf_Columns[20];
    ساكن حرف	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ساكن حرف	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ساكن حرف	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((حرف *)envbuf, "%ld", rows);
    setenv("ROWS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", rows);
    setenv("LINES", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", columns);
    setenv("COLUMNS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%d", t_colors);
    setenv("COLORS", (حرف *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    אם (is_terminal)
    {
	sprintf((حرف *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (حرف *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == عدم ? "" : (حرف *)serverName, 1);
#  endif
# אחרת
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, حجم(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, حجم(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_Lines, حجم(envbuf_Lines), "LINES=%ld", rows);
    putenv(envbuf_Lines);
    vim_snprintf(envbuf_Columns, حجم(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, حجم(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    אם (is_terminal)
    {
	vim_snprintf(envbuf_Version, حجم(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, حجم(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == عدم ? "" : (حرف *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ساكن فراغ
set_default_child_environment(صحيح is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#אם defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ساكن فراغ
open_pty(صحيح *pty_master_fd, صحيح *pty_slave_fd, char_u **name1, char_u **name2)
{
    حرف	*tty_name;

    אם (name1 != عدم)
	*name1 = عدم;
    אם (name2 != عدم)
	*name2 = عدم;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    אם (*pty_master_fd < 0)
	החזר;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# אחרת
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    אם (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    אחרת
    {
	אם (name1 != عدم)
	    *name1 = vim_strsave((char_u *)tty_name);
	אם (name2 != عدم)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ساكن فراغ
may_send_sigint(صحيح c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    אם (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# אחרת
	kill(0, SIGINT);
# endif
	אם (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#אם !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    صحيح
unix_build_argv(
	char_u *cmd,
	حرف ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    حرف	**argv = عدم;
    صحيح		argc;

    *sh_tofree = vim_strsave(p_sh);
    אם (*sh_tofree == عدم)		// out of memory
	החזר FAIL;

    אם (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	החזר FAIL;
    *argvp = argv;

    אם (cmd != عدم)
    {
	char_u	*s;
	char_u	*p;

	אם (extra_shell_arg != عدم)
	    argv[argc++] = (حرف *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	אם (*shcf_tofree == عدم)    // out of memory
	    החזר FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	כלעוד (*p != NUL)
	{
	    argv[argc++] = (حرف *)s;
	    כלעוד (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (حرف *)cmd;
    }
    argv[argc] = عدم;
    החזר OK;
}
#endif

#אם defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساكن صحيح
mch_call_shell_terminal(
    char_u	*cmd,
    صحيح		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    حرف	**argv = عدم;
    char_u	*tofree1 = عدم;
    char_u	*tofree2 = عدم;
    صحيح		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    אם (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	اذهب theend;

    init_job_options(&opt);
    ch_log(عدم, "starting terminal for system command '%s'", cmd);
    buf = term_start(عدم, argv, &opt, TERM_START_SYSTEM);
    אם (buf == عدم)
	اذهب theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    אם (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	כלעוד (term_use_loop())
	{
	    אם (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		אם (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    אחרת
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(عدم, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    אם (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    החזר retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ساكن صحيح
mch_call_shell_system(
    char_u	*cmd,
    صحيح		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    حرف	*ifn = عدم;
    حرف	*ofn = عدم;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    صحيح		x;

    out_flush();

    אם (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# אם defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  אם defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    אם (cmd == عدم)
	x = system((حرف *)p_sh);
    אחרת
    {
# ifdef VMS
	אם (ofn = strchr((حرف *)cmd, '>'))
	    *ofn++ = '\0';
	אם (ifn = strchr((حرف *)cmd, '<'))
	{
	    حرف *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    אם (p)
		*p = '\0';
	}
	אם (ofn)
	    x = vms_sys((حرف *)cmd, ofn, ifn);
	אחרת
	    x = system((حرف *)cmd);
# אחרת
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == عدم ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	אם (newcmd == عدم)
	    x = 0;
	אחרת
	{
	    sprintf((حرف *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == عدم ? "" : (حرف *)extra_shell_arg,
		    (حرف *)p_shcf,
		    (حرف *)cmd);
	    x = system((حرف *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    אם (emsg_silent)
	;
    אחרת אם (x == 127)
	msg_puts(_("\nCannot execute shell sh\n"));
    אחרת אם (x && !(options & SHELL_SILENT))
    {
	msg_puts(_("\nshell returned "));
	msg_outnum((long)x);
	msg_putchar('\n');
    }

    אם (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# אם defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    החזר x;
}

#אחרת // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ساكن صحيح
mch_call_shell_fork(
    char_u	*cmd,
    صحيح		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    صحيح		status = -1;
    صحيح		retval = -1;
    حرف	**argv = عدم;
    char_u	*tofree1 = عدم;
    char_u	*tofree2 = عدم;
    صحيح		i;
    صحيح		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    صحيح		pty_slave_fd = -1;
# endif
    صحيح		fd_toshell[2];		// for pipes
    صحيح		fd_fromshell[2];
    صحيح		pipe_error = FALSE;
    صحيح		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    אם (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    אם (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    אם (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	اذهب error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    אם ((options & (SHELL_READ|SHELL_WRITE))
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
	אם (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, عدم, عدم);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	אם (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    אם (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		אם (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    אם (pipe_error)
	    {
		msg_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    אם (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	אם (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    msg_puts(_("\nCannot fork\n"));
	    אם ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		אם (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		אחרת				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	אחרת אם (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    אם (ch_log_active())
	    {
		ch_log(عدم, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    אם (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		صحيح fd;

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
		אם (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    אחרת אם ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		אם (p_stmp)
		{
		    (فراغ)setsid();
#  אם defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		אם (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    אם (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)عدم);
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
		אם (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    אם (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		אחרת
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
		    אם (gui.in_use)
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
	אחרת			// parent
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
	    אם ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		صحيح	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		صحيح	    ta_len = 0;		// valid bytes in ta_buf[]
		صحيح	    len;
		صحيح	    p_more_save;
		صحيح	    old_State;
		صحيح	    c;
		صحيح	    toshell_fd;
		صحيح	    fromshell_fd;
		garray_T    ga;
		صحيح	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		אם (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		אחרת
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

		אם ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    אם ((wpid = fork()) == -1)
		    {
			msg_puts(_("\nCannot fork\n"));
		    }
		    אחרת אם (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			עבור (;;)
			{
			    אם (lplen == 0)
				len = 0;
			    אחרת אם (lp[written] == NL)
				// NL -> NUL translation
				len = اكتب(toshell_fd, "", (size_t)1);
			    אחרת
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = اكتب(toshell_fd, (حرف *)lp + written,
					   s == عدم ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    אם (len == (صحيح)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				אם (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_line_count
						    || curbuf->b_p_eol)))
				    vim_ignored = اكتب(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				אם (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    اكسر;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    אחרת אם (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    אחרת // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		אם (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		עבור (;;)
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
		    אם (!(options & SHELL_EXPAND)
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
		      אם (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      אם (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			אם (len == 1 && (pty_master_fd < 0 || cmd != عدم))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    אם (pty_master_fd < 0 && toshell_fd >= 0
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
			אם (pty_master_fd < 0)
			{
			    עבור (i = ta_len; i < ta_len + len; ++i)
			    {
				אם (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    msg_putchar(ta_buf[i]);
				אחרת אם (has_mbyte)
				{
				    صحيح l = (*mb_ptr2len)(ta_buf + i);

				    msg_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				אחרת
				    msg_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(msg_row, cmdline_col_off + msg_col);
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
			אם (options & SHELL_WRITE)
			    ta_len = 0;
			אחרת אם (toshell_fd >= 0)
			{
			    len = اكتب(toshell_fd, (حرف *)ta_buf, (size_t)1);
			    אם (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    אם (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# אחרת
			kill(0, SIGINT);
# endif
			אם (wpid > 0)
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
		    כלעוד (RealWaitForChar(fromshell_fd, 10L, عدم, عدم))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			אם (len <= 0)		    // end of file or error
			    اذهب finished;

			noread_cnt = 0;
			אם (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    עבור (i = 0; i < len; ++i)
			    {
				אם (buffer[i] == NL)
				    append_ga_line(&ga);
				אחרת אם (buffer[i] == NUL)
				    ga_append(&ga, NL);
				אחרת
				    ga_append(&ga, buffer[i]);
			    }
			}
			אחרת אם (has_mbyte)
			{
			    صحيح		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    עבור (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				אם (l == 0)
				    l = 1;  // NUL byte?
				אחרת אם (MB_BYTE2LEN(*p) != l)
				    اكسر;
			    }
			    אם (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				אם (len >= 12)
				    ++p;
				אחרת
				{
				    buffer_off = len;
				    استمر;
				}
			    }
			    c = *p;
			    *p = NUL;
			    msg_puts((حرف *)buffer);
			    אם (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				استمر;
			    }
			    buffer_off = 0;
			}
			אחרת
			{
			    buffer[len] = NUL;
			    msg_puts((حرف *)buffer);
			}

			windgoto(msg_row, cmdline_col_off + msg_col);
			cursor_on();
			out_flush();
			אם (got_int)
			    اكسر;

# ifdef ELAPSED_FUNC
			אם (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    אם (msec > 2000)
			    {
				noread_cnt = 5;
				اكسر;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    אם (wait_pid == pid)
		    {
			אם (noread_cnt < 5)
			    استمر;
			اكسر;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    אם ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    אחרת
			wait_pid = 0;

# אם defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		אם (options & SHELL_READ)
		{
		    אם (ga.ga_len > 0)
		    {
			append_ga_line(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    אחרת
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		אם (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		אם (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# אם (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    אחרת
	    {
		long delay_msec = 1;

		אם (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		עבור (;;)
		{
		    אם (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  אחרת
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    אם ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			اكسر;
		    }

#  אם defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    אם (++delay_msec > 10)
			delay_msec = 10;
		}

		אם (tmode == TMODE_RAW)
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
	    אם (wait_pid != pid)
		(فراغ)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    אם (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    אם (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, عدم);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    אם (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    אם (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		אם (retval != 0 && !emsg_silent)
		{
		    אם (retval == EXEC_FAILED)
		    {
			msg_puts(_("\nCannot execute shell "));
			msg_outtrans(p_sh);
			msg_putchar('\n');
		    }
		    אחרת אם (!(options & SHELL_SILENT))
		    {
			msg_puts(_("\nshell returned "));
			msg_outnum((long)retval);
			msg_putchar('\n');
		    }
		}
	    }
	    אחרת
		msg_puts(_("\nCommand terminated\n"));
	}
    }

error:
    אם (!did_settmode)
	אם (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    החזר retval;
}
#endif // USE_SYSTEM

    صحيح
mch_call_shell(
    char_u	*cmd,
    صحيح		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(عدم, "executing shell command: %s", cmd);
#endif
#אם defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    אם (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != عدم
					      && (options & SHELL_SILENT) == 0)
	החזר mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    החזר mch_call_shell_system(cmd, options);
#אחרת
    החזר mch_call_shell_fork(cmd, options);
#endif
}

#אם defined(FEAT_EVAL)
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
    حرف	**argv,
    char_u	*infile,
    صحيح		flags UNUSED,
    صحيح		*ret_len)
{
    pid_t	pid;
    صحيح		fd_out[2] = {-1, -1};
    صحيح		status = -1;
    char_u	*buffer = عدم;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(عدم, "directly executing: %s", argv[0]);

    אם (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	החזר عدم;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# אחרת
    pid = fork();
# endif
    אם (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	החזר عدم;
    }

    אם (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	אם (ch_log_active())
	{
	    ch_log(عدم, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	אם (infile != عدم)
	{
	    صحيح fd_in = open((حرف *)infile, O_RDONLY);
	    אם (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	אחרת
	{
	    صحيح nullfd = open("/dev/null", O_RDONLY);
	    אם (nullfd >= 0)
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
    עבור (;;)
    {
	حرف	buf[4096];
	صحيح	n;

	n = (صحيح)اقرأ(fd_out[0], buf, حجم(buf));
	אם (n <= 0)
	    اكسر;
	אם (ga_grow(&ga, n) == FAIL)
	    اكسر;
	mch_memmove((حرف *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (فراغ)waitpid(pid, &status, 0);
    אם (WIFEXITED(status))
	status = WEXITSTATUS(status);
    אחרת
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    אם (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	אם (buffer != عدم)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    אם (ret_len == عدم)
	    {
		صحيح	i;

		// Change NUL into SOH, otherwise the string is truncated.
		עבור (i = 0; i < ga.ga_len; ++i)
		    אם (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    אחרת
		*ret_len = ga.ga_len;
	}
    }
    אחרת אם (ret_len != عدم)
	*ret_len = 0;

    ga_clear(&ga);
    החזר buffer;
}
#endif

#אם defined(FEAT_JOB_CHANNEL)
    فراغ
mch_job_start(حرف **argv, job_T *job, jobopt_T *options, صحيح is_terminal)
{
    pid_t	pid;
    صحيح		fd_in[2] = {-1, -1};	// for stdin
    صحيح		fd_out[2] = {-1, -1};	// for stdout
    صحيح		fd_err[2] = {-1, -1};	// for stderr
    صحيح		pty_master_fd = -1;
    صحيح		pty_slave_fd = -1;
    channel_T	*channel = عدم;
    صحيح		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    صحيح		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    صحيح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحيح		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    صحيح		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    صحيح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحيح		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    صحيح		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    אם (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    אם (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    אם (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((حرف *)fname, O_RDONLY, 0);
	אם (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    אחרת
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	אם (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    اذهب failed;

    אם (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	אם (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    אחרת אם (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	اذهب failed;

    אם (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	אם (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    אחרת אם (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	اذهب failed;

    אם (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	אם (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    אם (channel != عدم)
		++channel->ch_refcount;
	}
	אחרת
	    channel = add_channel();
	אם (channel == عدم)
	    اذهب failed;
	אם (job->jv_tty_out != عدم)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    אם (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	اذهب failed;
    }
    אם (pid == 0)
    {
	صحيح	null_fd = -1;
	صحيح	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	אם (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(فراغ)setsid();
# endif

# ifdef FEAT_TERMINAL
	אם (options->jo_term_rows > 0)
	{
	    حرف *term = (حرف *)T_NAME;

#  ifdef FEAT_GUI
	    אם (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    אם (term == عدم || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		אם (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		אחרת אם (t_colors > 16)
		    term = "xterm-color";
		אחרת
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	אחרת
# endif
	    set_default_child_environment(is_terminal);

	אם (options->jo_env != عدم)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    صحيح		todo = (صحيح)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_ITEMS(&dict->dv_hashtab, hi, todo)
		אם (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_key, tv_get_string(item));
		    --todo;
		}
	}

	אם (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    אם (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	אם (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)عدم);
# endif
	}

	// set up stdin for the child
	close(0);
	אם (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	אחרת אם (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	אחרת
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	אם (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	אחרת אם (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	אחרת אם (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	אחרת
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	אם (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	אחרת אם (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	אחרת
	    vim_ignored = dup(fd_out[1]);

	אם (fd_in[0] >= 0)
	    close(fd_in[0]);
	אם (fd_in[1] >= 0)
	    close(fd_in[1]);
	אם (fd_out[0] >= 0)
	    close(fd_out[0]);
	אם (fd_out[1] >= 0)
	    close(fd_out[1]);
	אם (fd_err[0] >= 0)
	    close(fd_err[0]);
	אם (fd_err[1] >= 0)
	    close(fd_err[1]);
	אם (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	אם (null_fd >= 0)
	    close(null_fd);

	אם (options->jo_cwd != عدم && mch_chdir((حرف *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	אם (stderr_works)
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

    אם (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    אם (fd_in[0] >= 0)
	close(fd_in[0]);
    אם (fd_out[1] >= 0)
	close(fd_out[1]);
    אם (fd_err[1] >= 0)
	close(fd_err[1]);
    אם (channel != عدم)
    {
	صحيح in_fd = INVALID_FD;
	صحيح out_fd = INVALID_FD;
	صحيح err_fd = INVALID_FD;

	אם (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	אם (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	אם (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    אם (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    אחרת אם (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    אחרת
    {
	אם (fd_in[1] >= 0)
	    close(fd_in[1]);
	אם (fd_out[0] >= 0)
	    close(fd_out[0]);
	אם (fd_err[0] >= 0)
	    close(fd_err[0]);
	אם (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    החזר;

failed:
    channel_unref(channel);
    אם (fd_in[0] >= 0)
	close(fd_in[0]);
    אם (fd_in[1] >= 0)
	close(fd_in[1]);
    אם (fd_out[0] >= 0)
	close(fd_out[0]);
    אם (fd_out[1] >= 0)
	close(fd_out[1]);
    אם (fd_err[0] >= 0)
	close(fd_err[0]);
    אם (fd_err[1] >= 0)
	close(fd_err[1]);
    אם (pty_master_fd >= 0)
	close(pty_master_fd);
    אם (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ساكن char_u *
get_signal_name(صحيح sig)
{
    صحيح		i;
    char_u	numbuf[NUMBUFLEN];

    אם (sig == SIGKILL)
	החזר vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    עבור (i = 0; signal_info[i].sig != -1; i++)
	אם (sig == signal_info[i].sig)
	    החזר strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((حرف *)numbuf, NUMBUFLEN, "%d", sig);
    החזר vim_strnsave(numbuf, i);
}

    حرف *
mch_job_status(job_T *job)
{
    صحيح		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    אם (wait_pid == -1)
    {
	صحيح waitpid_errno = errno;
	אם (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    החזר "run";

	// process must have exited
	אם (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	اذهب return_dead;
    }
    אם (wait_pid == 0)
	החזר "run";
    אם (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	אם (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	اذهب return_dead;
    }
    אם (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	אם (job->jv_status < JOB_ENDED && job->jv_termsig != عدم)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	اذهب return_dead;
    }
    החזר "run";

return_dead:
    אם (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    החזר "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    صحيح		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    אם (dont_check_job_ended > 0)
	החזר عدم;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    אם (wait_pid <= 0)
	// no process ended
	החזר عدم;
    עבור (job = job_list; job != عدم; job = job->jv_next)
    {
	אם (job->jv_pid == wait_pid)
	{
	    אם (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    אחרת אם (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    אם (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    החזר job;
	}
    }
    החזר عدم;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    صحيح
mch_signal_job(job_T *job, char_u *how)
{
    صحيح	    sig = -1;

    אם (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    אחרת אם (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    אחרת אם (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    אחרת אם (STRCMP(how, "int") == 0)
	sig = SIGINT;
    אחרת אם (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    אחרת אם (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    אחרת אם (SAFE_isdigit(*how))
	sig = atoi((حرف *)how);
    אחרת
	החזר FAIL;

    // Never kill ourselves!
    אם (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    החזר OK;
}

/*
 * Clear the data related to "job".
 */
    فراغ
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (فراغ)waitpid(job->jv_pid, عدم, WNOHANG);
}
#endif

#אם defined(FEAT_TERMINAL)
    صحيح
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    صحيح		pty_master_fd = -1;
    صحيح		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    אם (pty_master_fd < 0 || pty_slave_fd < 0)
	החזר FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    אם (channel == عدم)
    {
	close(pty_master_fd);
	החזר FAIL;
    }
    אם (job->jv_tty_out != عدم)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    החזר OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    فراغ
mch_breakcheck(صحيح force)
{
    אם ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, عدم, عدم))
	fill_input_buf(FALSE);
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
    ساكن صحيح
WaitForChar(long msec, صحيح *interrupted, صحيح ignore_input)
{
#ifdef FEAT_TIMERS
    החזר ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_input) == OK;
#אחרת
    החזר WaitForCharOrMouse(msec, interrupted, ignore_input);
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
    ساكن صحيح
WaitForCharOrMouse(long msec, صحيح *interrupted, صحيح ignore_input)
{
#ifdef FEAT_MOUSE_GPM
    صحيح		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    صحيح		rest;
#endif
    صحيح		avail;

    אם (!ignore_input && input_available())	    // something in inbuf[]
	החזר 1;

#אם defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    אם (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	אם (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#אם defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    אם (do_xterm_trace())
	rest = msec;
# endif
    עשה
    {
# ifdef FEAT_XCLIPBOARD
	אם (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    אם (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    אם (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	אם (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	אם (!avail && !gpm_process_wanted)
# אחרת
	avail = RealWaitForChar(read_cmd_fd, msec, عدم, interrupted);
	אם (!avail)
# endif
	{
	    אם (!ignore_input && input_available())
		החזר 1;
# ifdef FEAT_XCLIPBOARD
	    אם (rest == 0 || !do_xterm_trace())
# endif
		اكسر;
	}
    }
    כלעוד (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#אחרת
    avail = RealWaitForChar(read_cmd_fd, msec, عدم, interrupted);
#endif
    החזר avail;
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
    ساكن صحيح
RealWaitForChar(صحيح fd, long msec, صحيح *check_for_gpm UNUSED, صحيح *interrupted)
{
    صحيح		ret;
    صحيح		result;
# אם defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ساكن صحيح	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    אם (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    אם (busy)
	החזר 0;
# endif

# ifdef MAY_LOOP
    עבור (;;)
# endif
    {
# ifdef MAY_LOOP
	صحيح		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	صحيح		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	صحيح		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	بنية pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	صحيح		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	صحيح             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	صحيح		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	صحيح		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	صحيح		xsmp_idx = -1;
#  endif
	صحيح		towait = (صحيح)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	אם (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (صحيح)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	אם ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	אם (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = ConnectionNumber(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	אם (check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	אם (xsmp_icefd != -1)
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
	אם (interrupted != عدم)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	אם (result == 0 && interrupted != عدم && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	אם (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	אם (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	אם (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    אם (--ret == 0 && !input_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	אם (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	אם (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    אם (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    אחרת אם (fds[xsmp_idx].revents & POLLHUP)
	    {
		אם (p_verbose > 0)
		    verb_msg(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    אם (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	אם (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# אחרת // HAVE_SELECT

	بنية timeval  tv;
	بنية timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ساكن fd_set	rfds, wfds, efds;
	صحيح		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	אם (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	אם (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	אחרת
	    tvp = عدم;

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
	אם ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    אם (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	אם (xterm_Shell != (Widget)0)
	{
	    FD_SET(ConnectionNumber(xterm_dpy), &rfds);
	    אם (maxfd < ConnectionNumber(xterm_dpy))
		maxfd = ConnectionNumber(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	אם (check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    אם (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	אם (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    אם (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	אם (interrupted != عدم)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	אם (result)
	    --ret;
	אחרת אם (interrupted != عدم && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	אם (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    אם (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    אם (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(عدم, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    اذهب select_eintr;
	}
#  endif
#  ifdef __TANDEM
	אם (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	אם (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	אם (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	אם (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(ConnectionNumber(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    אם (--ret == 0 && !input_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	אם (ret > 0 && check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    אם (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    אחרת אם (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	אם (ret > 0 && xsmp_icefd != -1)
	{
	    אם (FD_ISSET(xsmp_icefd, &efds))
	    {
		אם (p_verbose > 0)
		    verb_msg(_("XSMP lost ICE connection"));
		xsmp_close();
		אם (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    אחרת אם (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		אם (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	אם (ret >= 0)
	    (فراغ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	אם (finished || msec == 0)
	    اكسر;

#  אם defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	אם (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    اكسر;
#  endif

	// We're going to loop around again, find out for how long
	אם (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  אחרת
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    אם (msec <= 0)
		اكسر;	// waited long enough
	}
# endif
    }

    החזר result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    صحيح		flags)		// EW_* flags
{
    החזר unix_expandpath(gap, path, 0, flags, FALSE);
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

    صحيح
mch_expand_wildcards(
    صحيح		   num_pat,
    char_u	 **pat,
    صحيح		  *num_file,
    char_u	***file,
    صحيح		   flags)	// EW_* flags
{
    صحيح		i;
    size_t	len;
    long	llen;
    char_u	*p;
    صحيح		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    صحيح		j;
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
    صحيح		shell_style = STYLE_ECHO;
    صحيح		check_spaces;
    ساكن صحيح	did_find_nul = FALSE;
    صحيح		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ساكن string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ساكن string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *num_file = 0;	// default: no files found
    *file = عدم;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    אם (!have_wildcard(num_pat, pat))
	החזר save_patterns(num_pat, pat, num_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    אם (sandbox != 0 && check_secure())
	החזר FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    אם (secure || restricted)
	עבור (i = 0; i < num_pat; ++i)
	    אם (vim_strchr(pat[i], '`') != عدم
		    && (check_restricted() || check_secure()))
		החזר FAIL;

    /*
     * get a name for the temp file
     */
    אם ((tempname = vim_tempname('o', FALSE)) == عدم)
    {
	emsg(_(e_cant_get_temp_file_name));
	החזר FAIL;
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
    אם (num_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    אחרת אם ((len = STRLEN(p_sh)) >= 3)
    {
	אם (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	אחרת אם (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    אם (shell_style == STYLE_ECHO)
    {
	אם (strstr((حرف *)gettail(p_sh), "bash") != عدم)
	    shell_style = STYLE_GLOBSTAR;
	אחרת אם (strstr((حرف *)gettail(p_sh), "sh") != عدم)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    אם (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    אחרת אם (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    עבור (i = 0; i < num_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# אחרת
	++len;				// add space
	עבור (j = 0; pat[i][j] != NUL; ++j)
	{
	    אם (vim_strchr(SHELL_SPECIAL, pat[i][j]) != عدم)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    אם (command == عدم)
    {
	// out of memory
	vim_free(tempname);
	החזר FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    אם (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((حرف *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((حرف *)command, ")>");
	אם (p == عدم)
	{
	    ;					    // can't happen ;-)
	}
	אחרת
	{
	    --p;
	    כלעוד (p > command && VIM_ISWHITE(*p))
		--p;
	    אם (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    כלעוד (p > command && VIM_ISWHITE(*p))
		--p;
	    אם (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    אחרת
    {
	אם (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((حرف *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	אחרת אם (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((حرف *)command, len, "print -N >%s", tempname);
	אחרת אם (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	אחרת אם (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	אחרת
	    commandlen = vim_snprintf((حرف *)command, len, "echo >%s", tempname);

	עבור (i = 0; i < num_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((حرف *)command + commandlen, len, " \"%s\"", pat[i]);
# אחרת
	    صحيح intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    עבור (j = 0; pat[i][j] != NUL; ++j)
	    {
		אם (pat[i][j] == '`')
		    intick = !intick;
		אחרת אם (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    אם (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != عدم
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		אחרת אם (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != عدم)
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
    אם (flags & EW_SILENT)
	show_shell_mess = FALSE;
    אם (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    אם (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    אחרת אם (shell_style == STYLE_GLOB && !have_dollars(num_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    אם (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = عدم;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    אם (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	אם (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    msg_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    אם (!(flags & EW_SILENT))
# endif
	    {
		msg(_(e_cannot_expand_wildcards));
		msg_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	אם (shell_style == STYLE_BT)
	    החזר FAIL;
	اذهب notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((حرف *)tempname, READBIN);
    אם (fd == عدم)
    {
	// Something went wrong, perhaps a file name with a special char.
	אם (!(flags & EW_SILENT))
	{
	    msg(_(e_cannot_expand_wildcards));
	    msg_start();		// don't overwrite this message
	}
	vim_free(tempname);
	اذهب notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    אם (llen < 0)
	// just in case ftell() would fail
	buffer = عدم;
    אחרת
	buffer = alloc(llen + 1);
    אם (buffer == عدم)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	החזר FAIL;
    }
    len = llen;
    i = fread((حرف *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    אם (i != (صحيح)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	החזר FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    עבור (i = 0; i < (صحيح)len; ++i)
	אם (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    אם (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	עבור (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    כלעוד (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    אחרת אם (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	עבור (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    כלעוד (*p != '\n' && *p != NUL)
		++p;
	    אם (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    אחרת
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
	אם (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    אם (len && (صحيح)STRLEN(buffer) < (صحيح)len)
		did_find_nul = TRUE;
	    אחרת
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	אם (len && buffer[len - 1] == NUL)
	    --len;
	אחרת
	    buffer[len] = NUL;
	i = 0;
	עבור (p = buffer; p < buffer + len; ++p)
	    אם (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	אם (len)
	    ++i;			// count last entry
    }
    אם (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	اذهب notfound;
    }
    *num_file = i;
    *file = ALLOC_MULT(char_u *, i);
    אם (*file == عدم)
    {
	// out of memory
	vim_free(buffer);
	החזר FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    עבור (i = 0; i < *num_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	אם (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    כלעוד (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    אם (p == buffer + len)		// last entry
		*p = NUL;
	    אחרת
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	אחרת		// NUL separates
	{
	    כלעוד (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    עבור (j = 0, i = 0; i < *num_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	אם (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    استمر;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	אם ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    استمر;

	// Skip files that are not executable if we check for that.
	אם (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], عدم, !(flags & EW_SHELLCMD)))
	    استمر;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	אם (p)
	{
	    STRCPY(p, (*file)[i]);
	    אם (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *num_file = j;

    אם (*num_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	اذهب notfound;
    }

    החזר OK;

notfound:
    אם (flags & EW_NOTFOUND)
	החזר save_patterns(num_pat, pat, num_file, file);
    החזר FAIL;
}

#endif // VMS

    ساكن صحيح
save_patterns(
    صحيح		num_pat,
    char_u	**pat,
    صحيح		*num_file,
    char_u	***file)
{
    صحيح		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, num_pat);
    אם (*file == عدم)
	החזר FAIL;
    עבור (i = 0; i < num_pat; i++)
    {
	s = vim_strsave(pat[i]);
	אם (s != عدم)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *num_file = num_pat;
    החזר OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    صحيح
mch_has_exp_wildcard(char_u *p)
{
    עבור ( ; *p; MB_PTR_ADV(p))
    {
	אם (*p == '\\' && p[1] != NUL)
	    ++p;
	אחרת
	    אם (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#אחרת
				    "*?[{'"
#endif
						, *p) != عدم)
	    החזר TRUE;
    }
    החזר FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    صحيح
mch_has_wildcard(char_u *p)
{
    עבור ( ; *p; MB_PTR_ADV(p))
    {
	אם (*p == '\\' && p[1] != NUL)
	    ++p;
	אחרת
	    אם (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#אחרת
				    "*?[{`'$"
#endif
						, *p) != عدم
		|| (*p == '~' && p[1] != NUL))
	    החזר TRUE;
    }
    החזר FALSE;
}

    ساكن صحيح
have_wildcard(صحيح num, char_u **file)
{
    صحيح	    i;

    עבור (i = 0; i < num; i++)
	אם (mch_has_wildcard(file[i]))
	    החזר 1;
    החזר 0;
}

    ساكن صحيح
have_dollars(صحيح num, char_u **file)
{
    صحيح	    i;

    עבור (i = 0; i < num; i++)
	אם (vim_strchr(file[i], '$') != عدم)
	    החזר TRUE;
    החזר FALSE;
}

#אם !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    صحيح
mch_rename(ثابت حرف *src, ثابت حرف *dest)
{
    بنية stat	    st;

    אם (stat(dest, &st) >= 0)	    // fail if destination exists
	החזר -1;
    אם (link(src, dest) != 0)	    // link file to new name
	החזר -1;
    אם (mch_remove(src) == 0)	    // delete link to old name
	החזר 0;
    החזר -1;
}
#endif // !HAVE_RENAME

#אם defined(FEAT_MOUSE_GPM)
# אם defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ساكن صحيح
load_libgpm(فراغ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    אם (libgpm_hinst == عدم)
    {
	אם (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	החזר FAIL;
    }

    אם (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == عدم
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == عدم
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == عدم
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == عدم
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == عدم
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = عدم;
	dll_gpm_flag = عدم;
	dll_gpm_fd   = عدم;
	החזר FAIL;
    }
    החזר OK;
}

    صحيح
gpm_available(فراغ)
{
    החזר libgpm_hinst != عدم || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ساكن صحيح
gpm_open(فراغ)
{
    ساكن Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    אם (!gpm_available())
	החזר 0;
# endif

    אם (gpm_flag)
	החזר 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    אם (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	החזר 1; // succeed
    }
    אם (gpm_fd == -2)
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
    החזר 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    صحيح
gpm_enabled(فراغ)
{
    החזר gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ساكن فراغ
gpm_close(فراغ)
{
    אם (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ساكن صحيح
mch_gpm_process(فراغ)
{
    صحيح			button;
    ساكن Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    صحيح			row,col;
    unsigned حرف	buttons_mask;
    unsigned حرف	gpm_modifiers;
    ساكن unsigned حرف old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    אם (hold_gui_events)
	החזר 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    بدل (GPM_BARE_EVENTS(gpm_event.type))
    {
	حالة GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    اكسر;
	حالة GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    بدل (buttons_mask)
	    {
		حالة GPM_B_LEFT:
		    button = MOUSE_LEFT;
		    اكسر;
		حالة GPM_B_MIDDLE:
		    button = MOUSE_MIDDLE;
		    اكسر;
		حالة GPM_B_RIGHT:
		    button = MOUSE_RIGHT;
		    اكسر;
		افتراضي:
		    החזר 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_NUM_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    اكسر;
	حالة GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    اكسر;
	افتراضي:
	    החזר 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    אם (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    אם (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    אם (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_input_buf(string, 6);
    החזר 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ساكن صحيح
sysmouse_open(فراغ)
{
    بنية mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.إشارة = SIGUSR2;
    אם (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	החזר FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    החזר OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ساكن فراغ
sysmouse_close(فراغ)
{
    بنية mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.إشارة = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ساكن فراغ
sig_sysmouse SIGDEFARG(sigarg)
{
    بنية mouse_info	mouse;
    بنية video_info	video;
    char_u		string[6];
    صحيح			row, col;
    صحيح			button;
    صحيح			buttons;
    ساكن صحيح		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    אם (hold_gui_events)
	החזר;
# endif

    mouse.operation = MOUSE_GETINFO;
    אם (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	אם (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	אחרת
	{
	    بدل (buttons)
	    {
		حالة 0:
		    button = MOUSE_RELEASE;
		    اكسر;
		حالة 1:
		    button = MOUSE_LEFT;
		    اكسر;
		حالة 2:
		    button = MOUSE_MIDDLE;
		    اكسر;
		حالة 4:
		    button = MOUSE_RIGHT;
		    اكسر;
		افتراضي:
		    החזר;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_input_buf(string, 6);
    }
    החזר;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحيح
mch_get_random(char_u *buf, صحيح len)
{
    ساكن صحيح dev_urandom_state = NOTDONE;

    אם (dev_urandom_state == FAIL)
	החזר FAIL;

    صحيح fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    אם (fd == -1)
	dev_urandom_state = FAIL;
    אחרת אם (اقرأ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    אחרת
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    החזר dev_urandom_state;
}

#אם defined(FEAT_LIBCALL)
تعريفنوع char_u * (*STRPROCSTR)(char_u *);
تعريفنوع char_u * (*INTPROCSTR)(صحيح);
تعريفنوع صحيح (*STRPROCINT)(char_u *);
تعريفنوع صحيح (*INTPROCINT)(صحيح);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    صحيح
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    صحيح		argint,
    char_u	**string_result, // NULL when using number_result
    صحيح		*number_result)
{
# אם defined(USE_DLOPEN)
    فراغ	*hinstLib;
    حرف	*dlerr = عدم;
# אחרת
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = عدم;
    صحيح		retval_int = 0;
    صحيح		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# אם defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (فراغ)dlerror();

    hinstLib = dlopen((حرف *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    אם (hinstLib == عدم)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	אם (dlerr != عدم)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# אחרת
    hinstLib = shl_load((ثابت حرف*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    אם (hinstLib != عدم)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	אם (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  אם defined(USE_DLOPEN)
	    dlerr = عدم;
#  endif
	    mch_didjmp();
	}
	אחרת
# endif
	{
	    retval_str = عدم;
	    retval_int = 0;

	    אם (argstring != عدم)
	    {
# אם defined(USE_DLOPEN)
		*(فراغ **)(&ProcAdd) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# אחרת
		אם (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
					TYPE_PROCEDURE, (فراغ *)&ProcAdd) < 0)
		    ProcAdd = عدم;
# endif
		אם ((success = (ProcAdd != عدم
# אם defined(USE_DLOPEN)
			    && dlerr == عدم
# endif
			    )))
		{
		    אם (string_result == عدم)
			retval_int = ((STRPROCINT)(فراغ *)ProcAdd)(argstring);
		    אחרת
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    אחרת
	    {
# אם defined(USE_DLOPEN)
		*(فراغ **)(&ProcAddI) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# אחרת
		אם (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
				       TYPE_PROCEDURE, (فراغ *)&ProcAddI) < 0)
		    ProcAddI = عدم;
# endif
		אם ((success = (ProcAddI != عدم
# אם defined(USE_DLOPEN)
			    && dlerr == عدم
# endif
			    )))
		{
		    אם (string_result == عدم)
			retval_int = ((INTPROCINT)(فراغ *)ProcAddI)(argint);
		    אחרת
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    אם (string_result == عدم)
		*number_result = retval_int;
	    אחרת אם (retval_str != عدم
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	אם (lc_signal != 0)
	{
	    صحيح i;

	    // try to find the name of this signal
	    עבור (i = 0; signal_info[i].sig != -1; i++)
		אם (lc_signal == signal_info[i].sig)
		    اكسر;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# אם defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	אם (dlerr != عدم)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(فراغ)dlclose(hinstLib);
# אחרת
	(فراغ)shl_unload(hinstLib);
# endif
    }

    אם (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	החזר FAIL;
    }

    החזר OK;
}
#endif

#אם defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ساكن صحيح	xterm_trace = -1;	// default: disabled
ساكن صحيح	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    فراغ
setup_term_clip(فراغ)
{
    صحيح		z = 0;
    حرف	*strp = "";
    Widget	AppShell;

    אם (!x_connect_to_server())
	החזר;

    open_app_context();
    אם (app_context != عدم && xterm_Shell == (Widget)0)
    {
	صحيح (*oldhandler)(Display*, XErrorEvent*);
# אם defined(USING_SETJMP)
	صحيح (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	אם (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# אם defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	אם (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = عدم;
	}
	אחרת
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", عدم, 0, &z, &strp);
	    אם (xterm_dpy != عدم)
		xterm_dpy_retry_count = 0;
# אם defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# אם defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(فراغ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(فراغ)XSetErrorHandler(oldhandler);

	אם (xterm_dpy == عدم)
	{
	    אם (p_verbose > 0)
		verb_msg(_("Opening the X display failed"));
	    החזר;
	}

	// Catch terminating error of the X server connection.
	(فراغ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	אם (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		عدم);
	אם (AppShell == (Widget)0)
	    החזר;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		topLevelShellWidgetClass, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		عدم);
	אם (xterm_Shell == (Widget)0)
	    החזר;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	אם (x11_display == عدم)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    אם (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	אם (x11_window == 0 && (strp = getenv("WINDOWID")) != عدم)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	אם (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	אם (x11_window != 0)
	    xterm_trace = 0;
    }
}

    فراغ
start_xterm_trace(صحيح button)
{
    אם (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	החזר;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    فراغ
stop_xterm_trace(فراغ)
{
    אם (xterm_trace < 0)
	החזר;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ساكن صحيح
do_xterm_trace(فراغ)
{
    Window		root, child;
    صحيح			root_x, root_y;
    صحيح			win_x, win_y;
    صحيح			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    ساكن char_u	*mouse_code = عدم;
    ساكن size_t	mouse_codelen = 0;
    ساكن char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ساكن صحيح		prev_row = 0, prev_col = 0;
    ساكن XSizeHints	xterm_hints;

    אם (xterm_trace <= 0)
	החזר FALSE;

    אם (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	אם (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    החזר FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	אם (mouse_code != عدم)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &root, &child, &root_x, &root_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	אם (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	אם (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	החזר TRUE;
    }

    אם (mouse_code == عدم || mouse_codelen > 45)
    {
	xterm_trace = 0;
	החזר FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &root, &child, &root_x, &root_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    אם (row == prev_row && col == prev_col)
	החזר TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_input_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    החזר TRUE;
}

# אם defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    فراغ
clear_xterm_clip(فراغ)
{
    אם (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    אם (xterm_dpy != عدم)
    {
#  אם 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	אם (x11_display == xterm_dpy)
	    x11_display = عدم;
	xterm_dpy = عدم;
    }
#  אם 0
    אם (app_context != (XtAppContext)عدم)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)عدم;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ساكن فراغ
clip_update(فراغ)
{
# ifdef FEAT_GUI
    אם (gui.in_use)
	gui_mch_update();
    אחרת
# endif
    אם (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    فراغ
xterm_update(فراغ)
{
    XEvent event;

    עבור (;;)
    {
	XtInputMask mask = XtAppPending(app_context);

	אם (mask == 0 || vim_is_input_buf_full())
	    اكسر;

	אם (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		אם (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == PropertyNewValue)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	אחרת
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    صحيح
clip_xterm_own_selection(Clipboard_T *cbd)
{
    אם (xterm_Shell != (Widget)0)
	החזר clip_x11_own_selection(xterm_Shell, cbd);
    החזר FAIL;
}

    فراغ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    אם (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    فراغ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    אם (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    فراغ
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#אם defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# אם defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ساكن فراغ
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    صحيح		save_cmod_flags;
    صحيح		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    אם (check_changed_any(FALSE, FALSE))
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
    אם (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ساكن فراغ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    صحيح		save_type UNUSED,
    Bool	shutdown,
    صحيح		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    אם (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    אם (p_verbose > 0)
	verb_msg(_("XSMP handling save-yourself request"));

# אם defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    אם (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    אחרת
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
    ساكن فراغ
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ساكن فراغ
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
    ساكن فراغ
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    אם (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ساكن فراغ
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    אם (opening)
    {
	xsmp_icefd = IceConnectionNumber(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, عدم);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    صحيح
xsmp_handle_requests(فراغ)
{
    Bool rep;

    אם (IceProcessMessages(xsmp.iceconn, عدم, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	אם (p_verbose > 0)
	    verb_msg(_("XSMP lost ICE connection"));
	xsmp_close();
	החזר FAIL;
    }
    אחרת
	החזר OK;
}

ساكن صحيح dummy;

// Set up X Session Management Protocol
    فراغ
xsmp_init(فراغ)
{
    حرف		errorstring[80];
    SmcCallbacks	smcallbacks;
# אם 0
    SmPropValue		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    אם (p_verbose > 0)
	verb_msg(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = عدم;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = عدم;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = عدم;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = عدم;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    אם (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	אם (p_verbose > 0)
	    verb_msg(_("XSMP ICE connection watch failed"));
	החזר;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    عدم,
	    عدم,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    عدم,
	    &xsmp.clientid,
	    حجم(errorstring) - 1,
	    errorstring);
    אם (xsmp.smcconn == عدم)
    {
	אם (p_verbose > 0)
	{
	    حرف errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[حجم(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, حجم(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_msg(errorreport);
	}
	החזר;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# אם 0
    // ID ourselves
    smname.value = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.num_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    فراغ
xsmp_close(فراغ)
{
    אם (xsmp_icefd == -1)
	החזר;

    SmcCloseConnection(xsmp.smcconn, 0, عدم);
    אם (xsmp.clientid != عدم)
	حرر(xsmp.clientid);
    xsmp.clientid = عدم;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#אם defined(FEAT_RELTIME)
# אם defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ساكن متطاير sig_atomic_t timeout_flag = FALSE;
ساكن timer_t		     timer_id;
ساكن صحيح		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ساكن فراغ
set_flag(اتحاد sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    فراغ
stop_timeout(فراغ)
{
    ساكن بنية itimerspec disarm = {{0, 0}, {0, 0}};

    אם (timer_created)
    {
	صحيح ret = timer_settime(timer_id, 0, &disarm, عدم);

	אם (ret < 0)
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
    متطاير sig_atomic_t *
start_timeout(long msec)
{
    بنية itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    صحيح ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    אם (!timer_created)
    {
	بنية sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	אם (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    החזר &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(عدم, "setting timeout timer to %d sec %ld nsec",
	       (صحيح)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, عدم);
    אם (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    החזר &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    فراغ
delete_timer(فراغ)
{
    אם (!timer_created)
	החזר;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# אחרת // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ساكن بنية sigaction		prev_sigaction;
ساكن متطاير sig_atomic_t	timeout_flag	     = FALSE;
ساكن صحيح			timer_active	     = FALSE;
ساكن صحيح			timer_handler_active = FALSE;
ساكن متطاير sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ساكن فراغ
set_flag SIGDEFARG(sigarg)
{
    אם (alarm_pending)
	alarm_pending = FALSE;
    אחרת
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    فراغ
stop_timeout(فراغ)
{
    ساكن بنية itimerval disarm = {{0, 0}, {0, 0}};
    صحيح			    ret;

    אם (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, عدم);
	אם (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    אם (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, عدم);
	אם (ret < 0)
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
    متطاير sig_atomic_t *
start_timeout(long msec)
{
    بنية itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    بنية sigaction	handle_alarm;
    صحيح			ret;
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
    אם (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, عدم);
    }
    אם (unlikely(ret != 0 || alarm_pending < 0))
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
    אם (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	החזר &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, عدم);
    אם (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	החזר &timeout_flag;
    }

    timer_active = TRUE;
    החזר &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
