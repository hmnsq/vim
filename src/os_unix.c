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

#அந்தர்பாவ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#அந்தர்பாவ "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
நிலையான முழு selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != வெற்று ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != வெற்று ? *dll_gpm_fd   : -1)

நிலையான முழு (*dll_Gpm_Open)     (Gpm_Connect *, முழு);
நிலையான முழு (*dll_Gpm_Close)    (வெறுமை);
நிலையான முழு (*dll_Gpm_GetEvent) (Gpm_Event *);
நிலையான முழு *dll_gpm_flag;
நிலையான முழு *dll_gpm_fd;

நிலையான வெறுமை *libgpm_hinst;
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

நிலையான வெறுமை gpm_close(வெறுமை);
நிலையான முழு gpm_open(வெறுமை);
நிலையான முழு mch_gpm_process(வெறுமை);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

நிலையான முழு sysmouse_open(வெறுமை);
நிலையான வெறுமை sysmouse_close(வெறுமை);
நிலையான வெறுமை sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#எனில் defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#எனில் defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
நிலையான Widget	xterm_Shell = (Widget)0;
நிலையான வெறுமை clip_update(வெறுமை);
# endif

Window	    x11_window = 0;
Display	    *x11_display = வெற்று;
#endif

நிலையான முழு ignore_sigtstp = FALSE;

நிலையான முழு get_x11_title(முழு);

நிலையான char_u	*oldtitle = வெற்று;
நிலையான நிலையற்ற sig_atomic_t oldtitle_outdated = FALSE;
நிலையான முழு	unix_did_set_title = FALSE;
நிலையான char_u	*oldicon = வெற்று;
நிலையான முழு	did_set_icon = FALSE;

நிலையான வெறுமை may_core_dump(வெறுமை);

வகைவரை முழு waitstatus;
நிலையான முழு  WaitForChar(long msec, முழு *interrupted, முழு ignore_உள்ளீடு);
நிலையான முழு  WaitForCharOrMouse(long msec, முழு *interrupted, முழு ignore_உள்ளீடு);
#ifdef VMS
முழு  RealWaitForChar(முழு, long, முழு *, முழு *interrupted);
#இல்லையெனில்
நிலையான முழு  RealWaitForChar(முழு, long, முழு *, முழு *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
நிலையான முழு do_xterm_trace(வெறுமை);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

நிலையான வெறுமை handle_resize(வெறுமை);

#எனில் defined(SIGWINCH)
நிலையான வெறுமை sig_winch SIGPROTOARG;
#endif
#எனில் defined(SIGTSTP)
நிலையான வெறுமை sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
நிலையான நிலையற்ற sig_atomic_t in_mch_suspend = FALSE;
#endif
#எனில் defined(SIGINT)
நிலையான வெறுமை catch_sigint SIGPROTOARG;
#endif
#எனில் defined(SIGUSR1)
நிலையான வெறுமை catch_sigusr1 SIGPROTOARG;
#endif
#எனில் defined(SIGPWR)
நிலையான வெறுமை catch_sigpwr SIGPROTOARG;
#endif
#எனில் defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
நிலையான வெறுமை sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
நிலையான நிலையற்ற sig_atomic_t sig_alarm_called;
#endif
நிலையான வெறுமை deathtrap SIGPROTOARG;

நிலையான வெறுமை catch_int_signal(வெறுமை);
நிலையான வெறுமை set_signals(வெறுமை);
நிலையான வெறுமை catch_signals(வெறுமை (*func_deadly)(முழு), வெறுமை (*func_other)(முழு));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#இல்லையெனில்
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	செய் { /**/ } போது (0)
# define UNBLOCK_SIGNALS(set)	செய் { /**/ } போது (0)
#endif
நிலையான முழு  have_wildcard(முழு, char_u **);
நிலையான முழு  have_dollars(முழு, char_u **);

நிலையான முழு save_patterns(முழு இலக்கம்_pat, char_u **pat, முழு *இலக்கம்_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
நிலையான நிலையற்ற sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
நிலையான நிலையற்ற sig_atomic_t got_tstp = FALSE;
நிலையான char_u	*extra_shell_arg = வெற்று;
நிலையான முழு	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
நிலையான நிலையற்ற sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
நிலையான நிலையற்ற sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#எனில் defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
நிலையான முழு dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
நிலையான tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
வகைவரை கட்டமைப்பு
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    எழுத்து *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

நிலையான xsmp_config_T xsmp;
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

நிலையான கட்டமைப்பு signalinfo
{
    முழு	    sig;	// Signal number, eg. SIGSEGV etc
    எழுத்து    *name;	// Signal name (not char_u!).
    எழுத்து    deadly;	// Catch as a deadly signal?
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
#எனில் defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#எனில் defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#எனில் defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#எனில் defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(முழு sig, sighandler_T func)
{
#எனில் defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    கட்டமைப்பு sigaction	sa, old;
    sigset_t		curset;
    முழு			blocked;

    எனில் (sigprocmask(SIG_BLOCK, வெற்று, &curset) == -1)
	திரும்பு SIG_ERR;

    blocked = sigismember(&curset, sig);

    எனில் (func == SIG_HOLD)
    {
	எனில் (blocked)
	    திரும்பு SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	எனில் (sigaction(sig, வெற்று, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, வெற்று) == -1)
	    திரும்பு SIG_ERR;
	திரும்பு old.sa_handler;
    }

    எனில் (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	எனில் (sigprocmask(SIG_UNBLOCK, &curset, வெற்று) == -1)
	    திரும்பு SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# இல்லையெனில்
    sa.sa_flags = 0;
# endif
    எனில் (sigaction(sig, &sa, &old) == -1)
	திரும்பு SIG_ERR;
    திரும்பு blocked ? SIG_HOLD: old.sa_handler;
#அல்லதெனில் defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    திரும்பு sigset(sig, func);
#இல்லையெனில்
    // Oldest and most compatible solution.
    திரும்பு சமிக்ஞை(sig, func);
#endif
}

    முழு
mch_chdir(எழுத்து *path)
{
    எனில் (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    திரும்பு chdir(vms_fixfilename(path));
#இல்லையெனில்
    திரும்பு chdir(path);
#endif
}

#எனில் defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    வெறுமை
mch_write(char_u *s, முழு len)
{
    vim_ignored = (முழு)லிக(1, (எழுத்து *)s, len);
    எனில் (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, வெற்று, வெற்று);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    நிலையான முழு
resize_func(முழு check_only)
{
    எனில் (check_only)
	திரும்பு do_resize;
    போது (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(வெற்று, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    திரும்பு FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    முழு
mch_inchar(
    char_u	*buf,
    முழு		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    முழு		tb_change_cnt)
{
    திரும்பு inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    நிலையான வெறுமை
handle_resize(வெறுமை)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    முழு
mch_char_avail(வெறுமை)
{
    திரும்பு WaitForChar(0L, வெற்று, FALSE);
}

#எனில் defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    முழு
mch_check_messages(வெறுமை)
{
    திரும்பு WaitForChar(0L, வெற்று, TRUE);
}
#endif

#எனில் defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# எனில் defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# எனில் defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(முழு special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	கட்டமைப்பு vm_statistics64	vm_stat;
	natural_t		மொத்தம் = HOST_VM_INFO64_மொத்தம்;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &மொத்தம்);
#  இல்லையெனில்
	கட்டமைப்பு vm_statistics	vm_stat;
	natural_t		மொத்தம் = HOST_VM_INFO_மொத்தம்;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &மொத்தம்);
#  endif
	எனில் (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_மொத்தம் + vm_stat.active_மொத்தம்
					    + vm_stat.inactive_மொத்தம்
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_மொத்தம்
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    எனில் (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	முழு		mib[2];
	size_t		len = அளவு(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  இல்லையெனில்
	// sysctl() may return 32 bit or 64 bit, accept both
	ஒன்றிப்பு {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  இல்லையெனில்
	mib[1] = HW_USERMEM;
#  endif
	எனில் (sysctl(mib, 2, &physmem, &len, வெற்று, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  இல்லையெனில்
	    எனில் (len == அளவு(physmem.u64))
		mem = (long_u)physmem.u64;
	    இல்லையெனில்
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# எனில் defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    எனில் (mem == 0)
    {
	கட்டமைப்பு sysinfo sinfo;

	// Linux way of getting amount of RAM available
	எனில் (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    போது (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  இல்லையெனில்
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    எனில் (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	எனில் (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    போது (shiftright > 0 && (pagesize & 1) == 0)
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
# எனில் defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	கட்டமைப்பு rlimit	rlp;

	எனில் (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (அளவு(long_u) * 8 - 1))
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

    எனில் (mem > 0)
	திரும்பு mem >> shiftright;
    திரும்பு (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    வெறுமை
mch_delay(long msec, முழு flags)
{
    tmode_T	old_tmode;
    முழு		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    எனில் (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	எனில் (call_settmode)
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
	செய்
	{
	    // if total is large enough, wait by portions in p_mzq
	    எனில் (total > p_mzq)
		msec = p_mzq;
	    இல்லையெனில்
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    கட்டமைப்பு timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (வெறுமை)nanosleep(&ts, வெற்று);
	}
#இல்லையெனில்
# ifdef HAVE_USLEEP
	போது (msec >= 1000)
	{
	    usleep((unsigned முழு)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned முழு)(msec * 1000));
# இல்லையெனில்
#  ifndef HAVE_SELECT
	poll(வெற்று, 0, (முழு)msec);
#  இல்லையெனில்
	{
	    கட்டமைப்பு timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, வெற்று, வெற்று, வெற்று, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	போது (total > 0);
#endif

	எனில் (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    இல்லையெனில்
	WaitForChar(msec, வெற்று, FALSE);
}

#எனில் defined(HAVE_அடுக்கு_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_அடுக்கு_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
நிலையான முழு அடுக்கு_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    நிலையான வெறுமை
check_அடுக்கு_growth(எழுத்து *p)
{
    முழு		i;

    அடுக்கு_grows_downwards = (p > (எழுத்து *)&i);
}
#endif

#எனில் defined(HAVE_அடுக்கு_LIMIT)
நிலையான எழுத்து *அடுக்கு_limit = வெற்று;

# எனில் defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    நிலையான வெறுமை
get_அடுக்கு_limit(வெறுமை)
{
    கட்டமைப்பு rlimit	rlp;
    முழு			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    எனில் (getrlimit(RLIMIT_அடுக்கு, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (அளவு(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# எனில் defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    எனில் (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		எனில் (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	எனில் (அடுக்கு_grows_downwards)
	{
	    அடுக்கு_limit = (எழுத்து *)((long)&i - (lim / 16L * 15L));
	    எனில் (அடுக்கு_limit >= (எழுத்து *)&i)
		// overflow, set to 1/16 of current stack position
		அடுக்கு_limit = (எழுத்து *)((long)&i / 16L);
	}
	இல்லையெனில்
	{
	    அடுக்கு_limit = (எழுத்து *)((long)&i + (lim / 16L * 15L));
	    எனில் (அடுக்கு_limit <= (எழுத்து *)&i)
		அடுக்கு_limit = வெற்று;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    முழு
mch_stackcheck(எழுத்து *p)
{
    எனில் (அடுக்கு_limit == வெற்று)
	திரும்பு OK;

    எனில் (அடுக்கு_grows_downwards)
    {
	எனில் (p < அடுக்கு_limit)
	    திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (p > அடுக்கு_limit)
	திரும்பு FAIL;
    திரும்பு OK;
}
#endif

#எனில் defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
நிலையான stack_t sigstk;			// for sigaltstack()
# இல்லையெனில்
நிலையான கட்டமைப்பு sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
நிலையான long முழு get_signal_அடுக்கு_size(வெறுமை)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long முழு size = -1;

    // return size only if sysconf doesn't return an error
    எனில் ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	திரும்பு size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    திரும்பு SIGSTKSZ;
# endif

    // otherwise guess the size
    திரும்பு 8000;
}

நிலையான எழுத்து *signal_அடுக்கு;

    நிலையான வெறுமை
init_signal_அடுக்கு(வெறுமை)
{
    எனில் (signal_அடுக்கு == வெற்று)
	திரும்பு;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_அடுக்கு;
#  இல்லையெனில்
    sigstk.ss_sp = signal_அடுக்கு;
#  endif
    sigstk.ss_size = get_signal_அடுக்கு_size();
    sigstk.ss_flags = 0;
    (வெறுமை)sigaltstack(&sigstk, வெற்று);
# இல்லையெனில்
    sigstk.ss_sp = signal_அடுக்கு;
    எனில் (அடுக்கு_grows_downwards)
	sigstk.ss_sp += get_signal_அடுக்கு_size() - 1;
    sigstk.ss_onstack = 0;
    (வெறுமை)sigstack(&sigstk, வெற்று);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#எனில் defined(SIGWINCH)
    நிலையான வெறுமை
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#எனில் defined(SIGTSTP)
    நிலையான வெறுமை
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    எனில் (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	எழுப்பு(sigarg);
    }
    இல்லையெனில்
	got_tstp = TRUE;

# எனில் !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#எனில் defined(SIGINT)
    நிலையான வெறுமை
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#எனில் defined(SIGUSR1)
    நிலையான வெறுமை
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#எனில் defined(SIGPWR)
    நிலையான வெறுமை
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
    நிலையான வெறுமை
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#எனில் defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
நிலையான JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
நிலையான நிலையற்ற sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
நிலையான நிலையற்ற sig_atomic_t lc_active = FALSE;

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
    நிலையான வெறுமை
mch_startjmp(வெறுமை)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    நிலையான வெறுமை
mch_endjmp(வெறுமை)
{
    lc_active = FALSE;
}

    நிலையான வெறுமை
mch_didjmp(வெறுமை)
{
# எனில் defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_அடுக்கு();
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
    நிலையான வெறுமை
deathtrap SIGDEFARG(sigarg)
{
    நிலையான முழு	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    முழு		i;
#endif

#எனில் defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    எனில் (lc_active)
    {
# எனில் defined(SIGHASARG)
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
    எனில் (in_mch_delay && sigarg == SIGQUIT)
	திரும்பு;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    எனில் (entered == 0
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
	திரும்பு;
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

#ifdef HAVE_அடுக்கு_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_அடுக்கு_limit();
#endif

#எனில் 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	எனில் (fp)
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
    க்கு (i = 0; signal_info[i].sig != -1; i++)
	எனில் (sigarg == signal_info[i].sig)
	    நிறுத்து;
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
    எனில் (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	எனில் (entered >= 4)
	    _exit(8);
	exit(7);
    }
    எனில் (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((எழுத்து *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#இல்லையெனில்
    sprintf((எழுத்து *)IObuff, "Vim: Caught deadly signal\r\n");
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
    நிலையான வெறுமை
after_sigcont(வெறுமை)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#எனில் defined(SIGCONT)
நிலையான வெறுமை sigcont_handler SIGPROTOARG;

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
நிலையான நிலையற்ற sig_atomic_t sigcont_received;
நிலையான வெறுமை sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    நிலையான வெறுமை
sigcont_handler SIGDEFARG(sigarg)
{
    எனில் (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    இல்லையெனில்
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

#எனில் defined(FEAT_CLIPBOARD)
# எனில் defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
நிலையான வெறுமை *clip_star_save = வெற்று;
நிலையான வெறுமை *clip_plus_save = வெற்று;
# endif

# எனில் defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    நிலையான வெறுமை
loose_clipboard(வெறுமை)
{
    எனில் (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	எனில் (clip_star.owned)
	    clip_lose_selection(&clip_star);
	எனில் (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	எனில் (x11_display != வெற்று)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# எனில் defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    நிலையான வெறுமை
save_clipboard(வெறுமை)
{
    எனில் (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    எனில் (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    நிலையான வெறுமை
restore_clipboard(வெறுமை)
{
    எனில் (clip_star_save != வெற்று)
    {
	எனில் (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	இல்லையெனில்
	    free_register(clip_star_save);
	clip_star_save = வெற்று;
    }
    எனில் (clip_plus_save != வெற்று)
    {
	எனில் (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	இல்லையெனில்
	    free_register(clip_plus_save);
	clip_plus_save = வெற்று;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    வெறுமை
mch_suspend(வெறுமை)
{
    எனில் (ignore_sigtstp)
	திரும்பு;

#எனில் defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# எனில் defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# எனில் defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# எனில் defined(SIGCONT)
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

	க்கு (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#இல்லையெனில்
    suspend_shell();
#endif
}

    வெறுமை
mch_init(வெறுமை)
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

    வெறுமை
set_sigwinch_handler(வெறுமை)
{
#எனில் defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    நிலையான வெறுமை
set_signals(வெறுமை)
{
#எனில் defined(SIGWINCH)
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
#எனில் defined(SIGCONT)
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

#எனில் defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    எனில் (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#எனில் defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    நிலையான வெறுமை
catch_int_signal(வெறுமை)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    வெறுமை
reset_signals(வெறுமை)
{
    catch_signals(SIG_DFL, SIG_DFL);
#எனில் defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    நிலையான வெறுமை
catch_signals(
    வெறுமை (*func_deadly)(முழு),
    வெறுமை (*func_other)(முழு))
{
    முழு	    i;

    க்கு (i = 0; signal_info[i].sig != -1; i++)
    {
	எனில் (signal_info[i].deadly)
	{
#எனில் defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    கட்டமைப்பு sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# எனில் defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# இல்லையெனில்
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, வெற்று);
#இல்லையெனில்
# எனில் defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    கட்டமைப்பு sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, வெற்று);
# இல்லையெனில்
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	இல்லையெனில் எனில் (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#இல்லையெனில்
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    நிலையான வெறுமை
block_signals(sigset_t *set)
{
    sigset_t	newset;
    முழு		i;

    sigemptyset(&newset);

    க்கு (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# எனில் defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    நிலையான வெறுமை
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, வெற்று);
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
    முழு
vim_handle_signal(முழு sig)
{
    நிலையான முழு got_signal = 0;
    நிலையான முழு blocked = TRUE;

    தேர்வு (sig)
    {
	நிலை SIGNAL_BLOCK:   blocked = TRUE;
			     நிறுத்து;

	நிலை SIGNAL_UNBLOCK: blocked = FALSE;
			     எனில் (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     நிறுத்து;

	இயல்பு:	     எனில் (!blocked)
				 திரும்பு TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     எனில் (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     நிறுத்து;
    }
    திரும்பு FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    முழு
mch_check_win(முழு argc UNUSED, எழுத்து **argv UNUSED)
{
    எனில் (isatty(1))
	திரும்பு OK;
    திரும்பு FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    முழு
mch_உள்ளீடு_isatty(வெறுமை)
{
    எனில் (isatty(read_cmd_fd))
	திரும்பு TRUE;
    திரும்பு FALSE;
}

#ifdef FEAT_X11

# எனில் defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    நிலையான வெறுமை
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#எனில் defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

நிலையான முழு	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    நிலையான முழு
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (எழுத்து *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    எனில் (!gui.in_use)
# endif
	செய்தி((எழுத்து *)IObuff);

    திரும்பு 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    நிலையான முழு
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    திரும்பு 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    நிலையான முழு
x_connect_to_server(வெறுமை)
{
    // No point in connecting if we are exiting or dying.
    எனில் (exiting || v_dying)
	திரும்பு FALSE;

# எனில் defined(FEAT_CLIENTSERVER)
    எனில் (x_force_connect)
	திரும்பு TRUE;
# endif
    எனில் (x_no_connect)
	திரும்பு FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    எனில் (clip_exclude_prog != வெற்று)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	எனில் (regprog_in_use(clip_exclude_prog))
	    திரும்பு FALSE;

	எனில் (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    திரும்பு FALSE;
    }
    திரும்பு TRUE;
}

# எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  எனில் defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    நிலையான முழு
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   எனில் defined(VMS) || defined(__CYGWIN__)
    திரும்பு 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
நிலையான முழு xterm_dpy_retry_மொத்தம் = 0;

    நிலையான முழு
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = வெற்று;
    xterm_dpy_retry_மொத்தம் = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = வெற்று;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  எனில் defined(VMS) || defined(__CYGWIN__)
    திரும்பு 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    நிலையான வெறுமை
may_restore_x11_clipboard(வெறுமை)
{
    // No point in restoring the connecting if we are exiting or dying.
    எனில் (!exiting && !v_dying && xterm_dpy_retry_மொத்தம் > 0)
    {
	--xterm_dpy_retry_மொத்தம்;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	எனில் (app_context != (XtAppContext)வெற்று)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)வெற்று;
	    x11_display = வெற்று; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    வெறுமை
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    எனில் (eap->arg != வெற்று && (arglen = STRLEN(eap->arg)) > 0)
    {
	எனில் (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (எழுத்து *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == வெற்று
		    ? (எழுத்து *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_மொத்தம் = 5;  // Try reconnecting five times
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
    நிலையான முழு
test_x11_window(Display *dpy)
{
    முழு			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    எனில் (XGetWMName(dpy, x11_window, &text_prop))
	XFree((வெறுமை *)text_prop.மதிப்பு);
    XSync(dpy, False);
    (வெறுமை)XSetErrorHandler(old_handler);

    எனில் (p_verbose > 0 && got_x_error)
	verb_செய்தி(_("Testing the X display failed"));

    திரும்பு (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

நிலையான முழு get_x11_thing(முழு get_title, முழு test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    நிலையான முழு
get_x11_windis(வெறுமை)
{
    எழுத்து	    *winid;
    நிலையான முழு	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    நிலையான முழு	    x11_display_from = XD_NONE;
    நிலையான முழு	    did_set_error_handler = FALSE;

    எனில் (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(வெறுமை)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# எனில் defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    எனில் (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	எனில் (x11_display_from == XD_HERE && x11_display != வெற்று)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	எனில் (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    திரும்பு OK;
	}
	x11_display = வெற்று;
	திரும்பு FAIL;
    }
    இல்லையெனில் எனில் (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = வெற்று;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    எனில் (!x_connect_to_server())
	திரும்பு FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    எனில் (x11_window == 0 && (winid = getenv("WINDOWID")) != வெற்று)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    எனில் (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    எனில் (xterm_dpy != வெற்று && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	எனில் (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    எனில் (x11_display_from == XD_HERE && x11_display != வெற்று)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	எனில் (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = வெற்று;
	    x11_display_from = XD_NONE;
	    திரும்பு FAIL;
	}
	திரும்பு OK;
    }
# endif

    எனில் (x11_window == 0 || x11_display == வெற்று)
	result = -1;

    எனில் (result != -1)	    // Have already been here and set this
	திரும்பு result;	    // Don't do all these X calls again

    எனில் (x11_window != 0 && x11_display == வெற்று)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	எனில் (p_verbose > 0)
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
	x11_display = XOpenDisplay(வெற்று);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	எனில் (p_verbose > 0 && sig_alarm_called)
	    verb_செய்தி(_("Opening the X display timed out"));
# endif
	எனில் (x11_display != வெற்று)
	{
# ifdef ELAPSED_FUNC
	    எனில் (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    எனில் (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = வெற்று;
	    }
	    இல்லையெனில்
		x11_display_from = XD_HERE;
	}
    }
    எனில் (x11_window == 0 || x11_display == வெற்று)
	திரும்பு (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    திரும்பு (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    நிலையான முழு
get_x11_title(முழு test_only)
{
    திரும்பு get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    நிலையான முழு
get_x11_icon(முழு test_only)
{
    முழு		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    எனில் (oldicon == வெற்று && !test_only)
    {
	எனில் (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	இல்லையெனில்
	    oldicon = vim_strsave(T_NAME);
    }

    திரும்பு retval;
}

    நிலையான முழு
get_x11_thing(
    முழு		get_title,	// get title string
    முழு		test_only)
{
    XTextProperty	text_prop;
    முழு			retval = FALSE;
    Status		status;

    எனில் (get_x11_windis() != OK)
	திரும்பு FALSE;

    // Get window/icon name if any
    எனில் (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    இல்லையெனில்
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
    Window	    வேர்;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned முழு    இலக்கம்_children;

    போது (!status || text_prop.மதிப்பு == வெற்று)
    {
	எனில் (!XQuery_மரம்(x11_display, win, &வேர், &parent, &children,
		    &இலக்கம்_children))
	    நிறுத்து;
	எனில் (children)
	    XFree((வெறுமை *)children);
	எனில் (parent == வேர் || parent == 0)
	    நிறுத்து;

	win = parent;
	எனில் (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	இல்லையெனில்
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    எனில் (status && text_prop.மதிப்பு != வெற்று)
    {
	retval = TRUE;
	எனில் (!test_only)
	{
	    எனில் (get_title)
		vim_free(oldtitle);
	    இல்லையெனில்
		vim_free(oldicon);
	    எனில் (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		எனில் (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.மதிப்பு);
		இல்லையெனில்
		    oldicon = vim_strsave((char_u *)text_prop.மதிப்பு);
	    }
	    இல்லையெனில்
	    {
		எழுத்து    **cl;
		Status  transform_status;
		முழு	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		எனில் (transform_status >= Success && n > 0 && cl[0])
		{
		    எனில் (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    இல்லையெனில்
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		இல்லையெனில்
		{
		    எனில் (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.மதிப்பு);
		    இல்லையெனில்
			oldicon = vim_strsave((char_u *)text_prop.மதிப்பு);
		}
	    }
	}
	XFree((வெறுமை *)text_prop.மதிப்பு);
    }
    திரும்பு retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# எனில் defined(X_HAVE_UTF8_STRING)
#  எனில் X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    நிலையான வெறுமை
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    எனில் (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (மாறிலி எழுத்து *)title,
					     வெற்று, வெற்று, 0, வெற்று, வெற்று, வெற்று);
    இல்லையெனில்
# endif
    {
# எனில் XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (மாறிலி எழுத்து *)title,
					     வெற்று, வெற்று, 0, வெற்று, வெற்று, வெற்று);
#  இல்லையெனில்
	XTextProperty	text_prop;
	எழுத்து		*c_title = (எழுத்து *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(வெறுமை)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     வெற்று, வெற்று, 0, வெற்று, வெற்று, வெற்று);
#  endif
# இல்லையெனில்
	XStoreName(x11_display, x11_window, (எழுத்து *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    நிலையான வெறுமை
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    எனில் (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, வெற்று, (மாறிலி எழுத்து *)icon,
						   வெற்று, 0, வெற்று, வெற்று, வெற்று);
    இல்லையெனில்
# endif
    {
# எனில் XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, வெற்று, (மாறிலி எழுத்து *)icon,
						   வெற்று, 0, வெற்று, வெற்று, வெற்று);
#  இல்லையெனில்
	XTextProperty	text_prop;
	எழுத்து		*c_icon = (எழுத்து *)icon;

	(வெறுமை)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, வெற்று, &text_prop,
						   வெற்று, 0, வெற்று, வெற்று, வெற்று);
#  endif
# இல்லையெனில்
	XSetIconName(x11_display, x11_window, (எழுத்து *)icon);
# endif
    }
    XFlush(x11_display);
}

#இல்லையெனில்  // FEAT_X11

    நிலையான முழு
get_x11_title(முழு test_only UNUSED)
{
    திரும்பு FALSE;
}

    நிலையான முழு
get_x11_icon(முழு test_only)
{
    எனில் (!test_only)
    {
	எனில் (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	இல்லையெனில்
	    oldicon = vim_strsave(T_NAME);
    }
    திரும்பு FALSE;
}

#endif // FEAT_X11

    முழு
mch_can_restore_title(வெறுமை)
{
    திரும்பு get_x11_title(TRUE);
}

    முழு
mch_can_restore_icon(வெறுமை)
{
    திரும்பு get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    வெறுமை
mch_settitle(char_u *title, char_u *icon)
{
    முழு		type = 0;
    நிலையான முழு	recursive = 0;

    எனில் (T_NAME == வெற்று)	    // no terminal name (yet)
	திரும்பு;
    எனில் (title == வெற்று && icon == வெற்று)	    // nothing to do
	திரும்பு;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    எனில் (recursive)
	திரும்பு;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    எனில் (get_x11_windis() == OK)
	type = 1;
#endif
#எனில் defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    எனில் (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    எனில் ((type || *T_TS != NUL) && title != வெற்று)
    {
	எனில் (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	எனில் (oldtitle == வெற்று
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (வெறுமை)get_x11_title(FALSE);

	எனில் (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	இல்லையெனில்
# ifdef FEAT_GUI_GTK
	எனில் (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#எனில் defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	இல்லையெனில்
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    எனில் ((type || *T_CIS != NUL) && icon != வெற்று)
    {
	எனில் (oldicon == வெற்று
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	எனில் (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	இல்லையெனில்
# ifdef FEAT_GUI_GTK
	எனில் (!gui.in_use)		// don't do this if GTK+ is running
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
    வெறுமை
mch_restore_title(முழு which)
{
    முழு	do_தள்ளு_எடு = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : வெற்று;
    char_u *tofree = வெற்று;
    எனில் (title == oldtitle && oldtitle != வெற்று)
    {
	tofree = vim_strsave(title);
	எனில் (tofree != வெற்று)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : வெற்று);
    vim_free(tofree);

    எனில் (do_தள்ளு_எடு)
    {
	// pop and push from/to the stack
	term_எடு_title(which);
	term_தள்ளு_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    முழு
vim_is_xterm(char_u *name)
{
    எனில் (name == வெற்று)
	திரும்பு FALSE;
    திரும்பு ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#எனில் defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    முழு
use_xterm_like_mouse(char_u *name)
{
    திரும்பு (name != வெற்று
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
    முழு
use_xterm_mouse(வெறுமை)
{
    எனில் (ttym_flags == TTYM_SGR)
	திரும்பு 4;
    எனில் (ttym_flags == TTYM_URXVT)
	திரும்பு 3;
    எனில் (ttym_flags == TTYM_XTERM2)
	திரும்பு 2;
    எனில் (ttym_flags == TTYM_XTERM)
	திரும்பு 1;
    திரும்பு 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    முழு
vim_is_iris(char_u *name)
{
    எனில் (name == வெற்று)
	திரும்பு FALSE;
    திரும்பு (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#எனில் defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    முழு
vim_is_vt300(char_u *name)
{
    எனில் (name == வெற்று)
	திரும்பு FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    திரும்பு ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != வெற்று)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    முழு
mch_get_user_name(char_u *s, முழு len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(வெற்று), len - 1);
    திரும்பு OK;
#இல்லையெனில்
    திரும்பு mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    முழு
mch_get_uname(uid_t uid, char_u *s, முழு len)
{
#எனில் defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    கட்டமைப்பு passwd   *pw;

    எனில் ((pw = getpwuid(uid)) != வெற்று
	    && pw->pw_name != வெற்று && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	திரும்பு OK;
    }
#endif
    sprintf((எழுத்து *)s, "%d", (முழு)uid);	    // assumes s is long enough
    திரும்பு FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    வெறுமை
mch_get_host_name(char_u *s, முழு len)
{
    கட்டமைப்பு utsname vutsname;

    எனில் (uname(&vutsname) < 0)
	*s = NUL;
    இல்லையெனில்
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#இல்லையெனில் // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    வெறுமை
mch_get_host_name(char_u *s, முழு len)
{
# ifdef VAXC
    vaxc$gethostname((எழுத்து *)s, len);
# இல்லையெனில்
    gethostname((எழுத்து *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(வெறுமை)
{
    திரும்பு (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    முழு
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    எனில் (kill(pid, 0) == 0)
	திரும்பு TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    எனில் (errno == ESRCH)
	திரும்பு FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    திரும்பு TRUE;
}

#எனில் !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    நிலையான எழுத்து *
strerror(முழு err)
{
    வெளி முழு	    sys_nerr;
    வெளி எழுத்து	    *sys_errlist[];
    நிலையான எழுத்து	    er[20];

    எனில் (err > 0 && err < sys_nerr)
	திரும்பு (sys_errlist[err]);
    snprintf(er, அளவு(er), "Error %d", err);
    திரும்பு er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    முழு
mch_dirname(char_u *buf, முழு len)
{
#எனில் defined(USE_GETCWD)
    எனில் (getcwd((எழுத்து *)buf, len) == வெற்று)
    {
	STRCPY(buf, strerror(errno));
	திரும்பு FAIL;
    }
    திரும்பு OK;
#இல்லையெனில்
    திரும்பு (getwd((எழுத்து *)buf) != வெற்று ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    முழு
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    முழு		len,
    முழு		force)		// also expand when already absolute path
{
    முழு		buflen = 0;
#ifdef HAVE_FCHDIR
    முழு		fd = -1;
    நிலையான முழு	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    முழு		retval = OK;
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
# எனில் CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# இல்லையெனில்
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    எனில் ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == வெற்று || p != fname))
    {
	எனில் (p == வெற்று && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	எனில் (p != வெற்று)
	{
	    எனில் (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    எனில் (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		எனில் (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    எனில் (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((எழுத்து *)olddir) != 0))
	    {
		p = வெற்று;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    இல்லையெனில்
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		எனில் (p - fname >= len)
		    retval = FAIL;
		இல்லையெனில்
		{
		    vim_strncpy(buf, fname, p - fname);
		    எனில் (mch_chdir((எழுத்து *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			எனில் (mch_isFullName(fname))
			    retval = FAIL;
			இல்லையெனில்
			    p = வெற்று;
		    }
		    இல்லையெனில் எனில் (*p == '/')
			fname = p + 1;
		    இல்லையெனில்
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	எனில் (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	எனில் (p != வெற்று)
	{
	    முழு	l;

#ifdef HAVE_FCHDIR
	    எனில் (fd >= 0)
	    {
		எனில் (p_verbose >= 5)
		{
		    verbose_enter();
		    செய்தி("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    இல்லையெனில்
#endif
		l = mch_chdir((எழுத்து *)olddir);
	    எனில் (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	எனில் (fd >= 0)
	    close(fd);
#endif

	buflen = (முழு)STRLEN(buf);
	எனில் (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	இல்லையெனில் எனில் (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += அளவு(PATHSEP);
	}
#endif
    }

    எனில் (buflen == 0)
	buflen = (முழு)STRLEN(buf);

    // Catch file names which are too long.
    எனில் (retval == FAIL || (முழு)(buflen + STRLEN(fname)) >= len)
	திரும்பு FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    எனில் (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    திரும்பு OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    முழு
mch_isFullName(char_u *fname)
{
#ifdef VMS
    திரும்பு ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((எழுத்து *)fname,':') || strchr((எழுத்து *)fname,'"') ||
	    (strchr((எழுத்து *)fname,'[') && strchr((எழுத்து *)fname,']'))||
	    (strchr((எழுத்து *)fname,'<') && strchr((எழுத்து *)fname,'>'))   );
#இல்லையெனில்
    திரும்பு (*fname == '/' || *fname == '~');
#endif
}

#எனில் defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    வெறுமை
fname_case(
    char_u	*name,
    முழு		len UNUSED)  // buffer size, only used when name gets longer
{
    கட்டமைப்பு stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    கட்டமைப்பு dirent *dp;

    எனில் (mch_lstat((எழுத்து *)name, &st) < 0)
	திரும்பு;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    எனில் (slash == வெற்று)
    {
	dirp = opendir(".");
	tail = name;
    }
    இல்லையெனில்
    {
	*slash = NUL;
	dirp = opendir((எழுத்து *)name);
	*slash = '/';
	tail = slash + 1;
    }

    எனில் (dirp == வெற்று)
	திரும்பு;

    taillen = STRLEN(tail);
    போது ((dp = readdir(dirp)) != வெற்று)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	எனில் (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    கட்டமைப்பு stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    எனில் (mch_lstat((எழுத்து *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		நிறுத்து;
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
    கட்டமைப்பு stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    எனில் (stat((எழுத்து *)vms_fixfilename(name), &statb))
#இல்லையெனில்
    எனில் (stat((எழுத்து *)name, &statb))
#endif
	திரும்பு -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    திரும்பு statb.st_mode & ~S_ADDACE;
#இல்லையெனில்
    திரும்பு statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    முழு
mch_setperm(char_u *name, long perm)
{
    திரும்பு (chmod((எழுத்து *)
#ifdef VMS
		    vms_fixfilename(name),
#இல்லையெனில்
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#எனில் defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    முழு
mch_fsetperm(முழு fd, long perm)
{
    திரும்பு (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#எனில் defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
வகைவரை கட்டமைப்பு vim_acl_solaris_T {
    முழு acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# எனில் defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    வெறுமை
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    எனில் (from_file == வெற்று)
	திரும்பு;

    எனில் (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    எனில் (selinux_enabled <= 0)
	திரும்பு;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    எழுத்து *from_context = வெற்று;
    எழுத்து *to_context = வெற்று;

    எனில் (getfilecon((எழுத்து *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	எனில் (errno == EOPNOTSUPP)
	    திரும்பு;

	செய்தி_puts(_("\nCould not get security context for "));
	செய்தி_outtrans(from_file);
	செய்தி_putchar('\n');
	திரும்பு;
    }
    எனில் (getfilecon((எழுத்து *)to_file, &to_context) < 0)
    {
	செய்தி_puts(_("\nCould not get security context for "));
	செய்தி_outtrans(to_file);
	செய்தி_putchar('\n');
	freecon (from_context);
	திரும்பு ;
    }
    எனில் (strcmp(from_context, to_context) != 0)
    {
	எனில் (setfilecon((எழுத்து *)to_file, from_context) < 0)
	{
	    செய்தி_puts(_("\nCould not set security context for "));
	    செய்தி_outtrans(to_file);
	    செய்தி_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# எனில் defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    வெறுமை
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    நிலையான மாறிலி எழுத்து * மாறிலி smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    எழுத்து	buffer[SMACK_LABEL_LEN];
    மாறிலி எழுத்து	*name;
    முழு		index;
    முழு		ret;
    ssize_t	size;

    எனில் (from_file == வெற்று)
	திரும்பு;

    size = listxattr((எழுத்து *)from_file, வெற்று, 0);
    // not supported or no attributes to copy
    எனில் (size <= 0)
	திரும்பு;

    க்கு (index = 0 ; index < (முழு)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((எழுத்து*)from_file, name, buffer, அளவு(buffer));
	எனில் (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((எழுத்து*)to_file, name, buffer, (size_t)size, 0);
	    எனில் (ret < 0)
	    {
		vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		செய்தி_outtrans(IObuff);
		செய்தி_putchar('\n');
	    }
	}
	இல்லையெனில்
	{
	    // what reason of not having the attribute value?
	    தேர்வு (errno)
	    {
		நிலை ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    திரும்பு; // leave because it isn't useful to continue

		நிலை ERANGE:
		இயல்பு:
		    // no enough size OR unexpected error
		     vim_snprintf((எழுத்து *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    செய்தி_puts((எழுத்து *)IObuff);
		    செய்தி_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		நிலை ENODATA:
		    // no attribute of this name
		    ret = removexattr((எழுத்து*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    நிறுத்து;
	    }
	}
    }
}
# endif // HAVE_SMACK

# எனில் defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    வெறுமை
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    எழுத்து	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    எழுத்து	*சாவி;
    எழுத்து	*val = வெற்று;
    எழுத்து	*errmsg = வெற்று;

    எனில் (from_file == வெற்று)
	திரும்பு;

    // get the length of the extended attributes
    size = listxattr((எழுத்து *)from_file, வெற்று, 0);
    // not supported or no attributes to copy
    எனில் (size <= 0)
	திரும்பு;
    xattr_buf = (எழுத்து*)alloc(size);
    எனில் (xattr_buf == வெற்று)
	திரும்பு;
    size = listxattr((எழுத்து *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    க்கு (முழு round = 0; round < 2; round++)
    {

	சாவி = xattr_buf;
	எனில் (round == 1)
	    size = tsize;

	போது (size > 0)
	{
	    vallen = getxattr((எழுத்து *)from_file, சாவி,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    எனில் (vallen >= 0 && round &&
		setxattr((எழுத்து *)to_file, சாவி, val, vallen, 0) == 0)
		;
	    இல்லையெனில் எனில் (errno)
	    {
		தேர்வு (errno)
		{
		    நிலை E2BIG:
			errmsg = e_xattr_e2big;
			செல் error_exit;
		    நிலை ENOTSUP:
		    நிலை EACCES:
		    நிலை EPERM:
			நிறுத்து;
		    நிலை ERANGE:
			errmsg = e_xattr_erange;
			செல் error_exit;
		    இயல்பு:
			errmsg = e_xattr_other;
			செல் error_exit;
		}
	    }

	    எனில் (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(சாவி) + 1;
	    size -= keylen;
	    சாவி += keylen;
	}
	எனில் (round)
	    நிறுத்து;

	val = (எழுத்து*)alloc(max_vallen + 1);
	எனில் (val == வெற்று)
	    செல் error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    எனில் (errmsg != வெற்று)
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
    vim_acl_T	ret = வெற்று;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((எழுத்து *)fname, ACL_TYPE_ACCESS);
# இல்லையெனில்
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    எனில் (acl_get((எழுத்து *)fname, 0, &aclent) < 0)
	திரும்பு வெற்று;
    ret = (vim_acl_T)aclent;
#  இல்லையெனில்
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(அளவு(vim_acl_solaris_T));
    எனில் (aclent == வெற்று)
	திரும்பு வெற்று;
    எனில் ((aclent->acl_cnt = acl((எழுத்து *)fname, GETACLCNT, 0, வெற்று)) < 0)
    {
	விடுவி(aclent);
	திரும்பு வெற்று;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * அளவு(aclent_t));
    எனில் (aclent->acl_entry == வெற்று)
    {
	விடுவி(aclent);
	திரும்பு வெற்று;
    }
    எனில் (acl((எழுத்து *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	விடுவி(aclent->acl_entry);
	விடுவி(aclent);
	திரும்பு வெற்று;
    }
    ret = (vim_acl_T)aclent;
#   இல்லையெனில்
#    எனில் defined(HAVE_AIX_ACL)
    முழு		aclsize;
    கட்டமைப்பு acl *aclent;

    aclsize = அளவு(கட்டமைப்பு acl);
    aclent = malloc(aclsize);
    எனில் (aclent == வெற்று)
	திரும்பு வெற்று;
    எனில் (statacl((எழுத்து *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	எனில் (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    எனில் (aclent == வெற்று || statacl((எழுத்து *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		விடுவி(aclent);
		திரும்பு வெற்று;
	    }
	}
	இல்லையெனில்
	{
	    விடுவி(aclent);
	    திரும்பு வெற்று;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    திரும்பு ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    வெறுமை
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    எனில் (aclent == வெற்று)
	திரும்பு;
# ifdef HAVE_POSIX_ACL
    acl_set_file((எழுத்து *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# இல்லையெனில்
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((எழுத்து *)fname, (acl_t *)aclent);
#  இல்லையெனில்
#   ifdef HAVE_SOLARIS_ACL
    acl((எழுத்து *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   இல்லையெனில்
#    ifdef HAVE_AIX_ACL
    chacl((எழுத்து *)fname, aclent, ((கட்டமைப்பு acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    வெறுமை
mch_free_acl(vim_acl_T aclent)
{
    எனில் (aclent == வெற்று)
	திரும்பு;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# இல்லையெனில்
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  இல்லையெனில்
#   ifdef HAVE_SOLARIS_ACL
    விடுவி(((vim_acl_solaris_T *)aclent)->acl_entry);
    விடுவி(aclent);
#   இல்லையெனில்
#    ifdef HAVE_AIX_ACL
    விடுவி(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    வெறுமை
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    முழு
mch_isdir(char_u *name)
{
    கட்டமைப்பு stat statb;

    எனில் (*name == NUL)	    // Some stat()s don't flag "" as an error.
	திரும்பு FALSE;
    எனில் (stat((எழுத்து *)name, &statb))
	திரும்பு FALSE;
    திரும்பு (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    முழு
mch_isrealdir(char_u *name)
{
    கட்டமைப்பு stat statb;

    எனில் (*name == NUL)	    // Some stat()s don't flag "" as an error.
	திரும்பு FALSE;
    எனில் (mch_lstat((எழுத்து *)name, &statb))
	திரும்பு FALSE;
    திரும்பு (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    நிலையான முழு
executable_file(char_u *name)
{
    கட்டமைப்பு stat	st;

    எனில் (stat((எழுத்து *)name, &st))
	திரும்பு 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    முழு vms_executable = 0;
    எனில் (S_ISREG(st.st_mode) && mch_access((எழுத்து *)name, X_OK) == 0)
    {
	எனில் (strstr(vms_tolower((எழுத்து*)name),".exe") != வெற்று
		|| strstr(vms_tolower((எழுத்து*)name),".com")!= வெற்று)
	    vms_executable = 1;
    }
    திரும்பு vms_executable;
#இல்லையெனில்
    திரும்பு S_ISREG(st.st_mode) && mch_access((எழுத்து *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    முழு
mch_can_exe(char_u *name, char_u **path, முழு use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    முழு		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    எனில் (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	எனில் ((use_path || gettail(name) != name) && executable_file(name))
	{
	    எனில் (path != வெற்று)
	    {
		எனில் (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		இல்லையெனில்
		    *path = vim_strsave(name);
	    }
	    திரும்பு TRUE;
	}
	திரும்பு FALSE;
    }

    p = (char_u *)getenv("PATH");
    எனில் (p == வெற்று || *p == NUL)
	திரும்பு -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    எனில் (buf == வெற்று)
	திரும்பு -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    க்கு (;;)
    {
	e = (char_u *)strchr((எழுத்து *)p, ':');
	எனில் (e == வெற்று)
	    e = p_end;
	elen = (size_t)(e - p);
	எனில் (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((எழுத்து *)buf, bufsize, "%.*s%s%s",
		(முழு)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	எனில் (retval == 1)
	{
	    எனில் (path != வெற்று)
	    {
		எனில் (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		இல்லையெனில்
		    *path = vim_strnsave(buf, buflen);
	    }
	    நிறுத்து;
	}

	எனில் (*e != ':')
	    நிறுத்து;
	p = e + 1;
    }

    vim_free(buf);
    திரும்பு retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    முழு
mch_nodetype(char_u *name)
{
    கட்டமைப்பு stat	st;

    எனில் (stat((எழுத்து *)name, &st))
	திரும்பு முனை_NORMAL;
    எனில் (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	திரும்பு முனை_NORMAL;
    எனில் (S_ISBLK(st.st_mode))	// block device isn't writable
	திரும்பு முனை_OTHER;
    // Everything else is writable?
    திரும்பு முனை_WRITABLE;
}

    வெறுமை
mch_early_init(வெறுமை)
{
#ifdef HAVE_CHECK_அடுக்கு_GROWTH
    முழு			i;

    check_அடுக்கு_growth((எழுத்து *)&i);

# ifdef HAVE_அடுக்கு_LIMIT
    get_அடுக்கு_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#எனில் defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_அடுக்கு = alloc(get_signal_அடுக்கு_size());
    init_signal_அடுக்கு();
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

#எனில் defined(EXITFREE)
    வெறுமை
mch_free_mem(வெறுமை)
{
# எனில் defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    எனில் (clip_star.owned)
	clip_lose_selection(&clip_star);
    எனில் (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    எனில் (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    எனில் (xterm_dpy != வெற்று)
	XtCloseDisplay(xterm_dpy);
    எனில் (app_context != (XtAppContext)வெற்று)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = வெற்று; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# எனில் defined(FEAT_X11)
    எனில் (x11_display != வெற்று
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# எனில் defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_அடுக்கு);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    நிலையான வெறுமை
exit_scroll(வெறுமை)
{
    எனில் (silent_mode)
	திரும்பு;
    எனில் (newline_on_exit || செய்தி_didout)
    {
	எனில் (செய்தி_use_printf())
	{
	    எனில் (info_message)
		mch_செய்தி("\n");
	    இல்லையெனில்
		mch_errmsg("\r\n");
	}
	இல்லையெனில்
	    out_char('\n');
    }
    இல்லையெனில் எனில் (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	செய்தி_clr_eos_force();		// clear the rest of the display
	windgoto((முழு)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# எனில் (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
வெளி வெறுமை __gcov_dump(வெறுமை);
வெளி வெறுமை __gcov_reset(வெறுமை);
#  define __gcov_flush() செய் { __gcov_dump(); __gcov_reset(); } போது (0)
# இல்லையெனில்
வெளி வெறுமை __gcov_flush(வெறுமை);
# endif
#endif

    வெறுமை
mch_exit(முழு r)
{
    exiting = TRUE;

#எனில் defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    எனில் (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	எனில் (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_எடு_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	எனில் (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	எனில் (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	எனில் (full_screen)
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
    எனில் (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    எனில் (deadly_signal != 0)
	திரும்பு;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    நிலையான வெறுமை
may_core_dump(வெறுமை)
{
    எனில் (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    நிலையான முழு
get_tty_fd(முழு fd)
{
    முழு		tty_fd = fd;

# எனில் defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    எனில் (mch_isatty(fd) > 1)
    {
	எழுத்து *name;

	name = ptsname(fd);
	எனில் (name == வெற்று)
	    திரும்பு -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	எனில் (tty_fd < 0)
	    திரும்பு -1;
    }
# endif
    திரும்பு tty_fd;
}

    நிலையான முழு
mch_tcgetattr(முழு fd, வெறுமை *term)
{
    முழு		tty_fd;
    முழு		retval = -1;

    tty_fd = get_tty_fd(fd);
    எனில் (tty_fd < 0)
	திரும்பு -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (கட்டமைப்பு termios *)term);
#  இல்லையெனில்
    retval = ioctl(tty_fd, TCGETA, (கட்டமைப்பு termio *)term);
#  endif
# இல்லையெனில்
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (கட்டமைப்பு sgttyb *)term);
# endif
    எனில் (tty_fd != fd)
	close(tty_fd);
    திரும்பு retval;
}

    வெறுமை
mch_settmode(tmode_T tmode)
{
    நிலையான முழு first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    நிலையான கட்டமைப்பு termios told;
	   கட்டமைப்பு termios tnew;
#  இல்லையெனில்
    நிலையான கட்டமைப்பு termio told;
	   கட்டமைப்பு termio tnew;
#  endif

    எனில் (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    எனில் (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == வெற்று || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  எனில் defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   இல்லையெனில்
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    இல்லையெனில்
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    இல்லையெனில் எனில் (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  எனில் defined(HAVE_TERMIOS_H)
    {
	முழு	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	போது (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  இல்லையெனில்
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# இல்லையெனில்
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    நிலையான கட்டமைப்பு sgttyb ttybold;
	   கட்டமைப்பு sgttyb ttybnew;

    எனில் (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    எனில் (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    இல்லையெனில் எனில் (tmode == TMODE_SLEEP)
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
    வெறுமை
get_stty(வெறுமை)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    எனில் (get_tty_info(read_cmd_fd, &info) != OK)
	திரும்பு;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    எனில் (p != வெற்று && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(வெற்று);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    முழு
get_tty_info(முழு fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    கட்டமைப்பு termios சாவிகள்;
#  இல்லையெனில்
    கட்டமைப்பு termio சாவிகள்;
#  endif

    எனில் (mch_tcgetattr(fd, &சாவிகள்) != -1)
    {
	info->backspace = சாவிகள்.c_cc[VERASE];
	info->interrupt = சாவிகள்.c_cc[VINTR];
	எனில் (சாவிகள்.c_iflag & ICRNL)
	    info->enter = NL;
	இல்லையெனில்
	    info->enter = CAR;
	எனில் (சாவிகள்.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	இல்லையெனில்
	    info->nl_does_cr = FALSE;
	திரும்பு OK;
    }
# இல்லையெனில்
    // for "old" tty systems
    கட்டமைப்பு sgttyb சாவிகள்;

    எனில் (mch_tcgetattr(fd, &சாவிகள்) != -1)
    {
	info->backspace = சாவிகள்.sg_erase;
	info->interrupt = சாவிகள்.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	திரும்பு OK;
    }
# endif
    திரும்பு FAIL;
}

#endif // VMS

நிலையான முழு	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    வெறுமை
mch_setmouse(முழு on)
{
#ifdef FEAT_BEVAL_TERM
    நிலையான முழு	bevalterm_ison = FALSE;
#endif
    முழு		xterm_mouse_vers;

#எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    எனில் (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    எனில் (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	திரும்பு;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    எனில் (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    எனில் (T_CXM != வெற்று && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    இல்லையெனில் எனில் (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    எனில் (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	எனில் (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    எனில் (xterm_mouse_vers > 0)
    {
	எனில் (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	இல்லையெனில்	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    இல்லையெனில் எனில் (ttym_flags == TTYM_DEC)
    {
	எனில் (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	இல்லையெனில்	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    இல்லையெனில்
    {
	எனில் (on)
	{
	    எனில் (gpm_open())
		mouse_ison = TRUE;
	}
	இல்லையெனில்
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    இல்லையெனில்
    {
	எனில் (on)
	{
	    எனில் (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	இல்லையெனில்
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    இல்லையெனில்
    {
	எனில் (on)
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
# இல்லையெனில்
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	இல்லையெனில்
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    இல்லையெனில்
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	எனில் (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	இல்லையெனில்
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#எனில் defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    வெறுமை
mch_bevalterm_changed(வெறுமை)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    வெறுமை
check_mouse_termcode(வெறுமை)
{
#ifdef FEAT_MOUSE_XTERM
    எனில் (use_xterm_mouse()
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
	எனில் (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    இல்லையெனில்
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    இல்லையெனில்
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    இல்லையெனில்
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    இல்லையெனில்
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    இல்லையெனில்
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    எனில் (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    இல்லையெனில்
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    எனில் (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	எனில் (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    இல்லையெனில்
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    எனில் (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	எனில் (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    இல்லையெனில்
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
    முழு
mch_get_shellsize(வெறுமை)
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
	கட்டமைப்பு winsize	ws;
	முழு fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	எனில் (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	எனில் (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(வெற்று, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# இல்லையெனில் // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	கட்டமைப்பு ttysize	ts;
	முழு fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	எனில் (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	எனில் (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_வரிகள்;
#   ifdef FEAT_EVAL
	    ch_log(வெற்று, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    எனில் (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != வெற்று)
    {
	எனில் ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((எழுத்து *)p);
# ifdef FEAT_EVAL
	    ch_log(வெற்று, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	எனில் ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((எழுத்து *)p);
# ifdef FEAT_EVAL
	    ch_log(வெற்று, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    எனில் (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(வெற்று, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    எனில் (columns <= 0 || rows <= 0)
	திரும்பு FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    திரும்பு OK;
}

# எனில் defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    வெறுமை
mch_calc_cell_size(கட்டமைப்பு cellsize *cs_out)
{
   கட்டமைப்பு winsize   ws;
   முழு		    retval;
   முழு		    x_cell_size;
   முழு		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(வெற்று, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   எனில் (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	திரும்பு;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   எனில் (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# எனில் defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    முழு
mch_report_winsize(முழு fd, முழு rows, முழு cols)
{
    முழு		tty_fd;
    முழு		retval = -1;

    tty_fd = get_tty_fd(fd);
    எனில் (tty_fd < 0)
	திரும்பு FAIL;

#  எனில் defined(TIOCSWINSZ)
    கட்டமைப்பு winsize ws;

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
    ch_log(வெற்று, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    கட்டமைப்பு ttysize ts;

    ts.ts_cols = cols;
    ts.ts_வரிகள் = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(வெற்று, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    எனில் (tty_fd != fd)
	close(tty_fd);
    திரும்பு retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    வெறுமை
mch_set_shellsize(வெறுமை)
{
    எனில் (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((முழு)Rows, (முழு)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    வெறுமை
mch_new_shellsize(வெறுமை)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    நிலையான pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    போது (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	எனில் (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    எனில் (++delay_msec > 10)
		delay_msec = 10;
	    தொடர்;
	}
	எனில் (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    நிறுத்து;
    }
    திரும்பு wait_pid;
}

#எனில் !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    நிலையான வெறுமை
set_child_environment(
	long	rows,
	long	columns,
	எழுத்து	*term,
	முழு	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    எழுத்து	envbuf[50];
# இல்லையெனில்
    நிலையான எழுத்து	envbuf_Term[30];
    நிலையான எழுத்து	envbuf_Rows[20];
    நிலையான எழுத்து	envbuf_வரிகள்[20];
    நிலையான எழுத்து	envbuf_Columns[20];
    நிலையான எழுத்து	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    நிலையான எழுத்து	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    நிலையான எழுத்து	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((எழுத்து *)envbuf, "%ld", rows);
    setenv("ROWS", (எழுத்து *)envbuf, 1);
    sprintf((எழுத்து *)envbuf, "%ld", rows);
    setenv("LINES", (எழுத்து *)envbuf, 1);
    sprintf((எழுத்து *)envbuf, "%ld", columns);
    setenv("COLUMNS", (எழுத்து *)envbuf, 1);
    sprintf((எழுத்து *)envbuf, "%d", t_colors);
    setenv("COLORS", (எழுத்து *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    எனில் (is_terminal)
    {
	sprintf((எழுத்து *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (எழுத்து *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == வெற்று ? "" : (எழுத்து *)serverName, 1);
#  endif
# இல்லையெனில்
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, அளவு(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, அளவு(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_வரிகள், அளவு(envbuf_வரிகள்), "LINES=%ld", rows);
    putenv(envbuf_வரிகள்);
    vim_snprintf(envbuf_Columns, அளவு(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, அளவு(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    எனில் (is_terminal)
    {
	vim_snprintf(envbuf_Version, அளவு(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, அளவு(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == வெற்று ? "" : (எழுத்து *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    நிலையான வெறுமை
set_default_child_environment(முழு is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#எனில் defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    நிலையான வெறுமை
open_pty(முழு *pty_master_fd, முழு *pty_slave_fd, char_u **name1, char_u **name2)
{
    எழுத்து	*tty_name;

    எனில் (name1 != வெற்று)
	*name1 = வெற்று;
    எனில் (name2 != வெற்று)
	*name2 = வெற்று;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    எனில் (*pty_master_fd < 0)
	திரும்பு;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# இல்லையெனில்
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    எனில் (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    இல்லையெனில்
    {
	எனில் (name1 != வெற்று)
	    *name1 = vim_strsave((char_u *)tty_name);
	எனில் (name2 != வெற்று)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    நிலையான வெறுமை
may_send_sigint(முழு c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    எனில் (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# இல்லையெனில்
	kill(0, SIGINT);
# endif
	எனில் (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#எனில் !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    முழு
unix_build_argv(
	char_u *cmd,
	எழுத்து ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    எழுத்து	**argv = வெற்று;
    முழு		argc;

    *sh_tofree = vim_strsave(p_sh);
    எனில் (*sh_tofree == வெற்று)		// out of memory
	திரும்பு FAIL;

    எனில் (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	திரும்பு FAIL;
    *argvp = argv;

    எனில் (cmd != வெற்று)
    {
	char_u	*s;
	char_u	*p;

	எனில் (extra_shell_arg != வெற்று)
	    argv[argc++] = (எழுத்து *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	எனில் (*shcf_tofree == வெற்று)    // out of memory
	    திரும்பு FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	போது (*p != NUL)
	{
	    argv[argc++] = (எழுத்து *)s;
	    போது (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (எழுத்து *)cmd;
    }
    argv[argc] = வெற்று;
    திரும்பு OK;
}
#endif

#எனில் defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    நிலையான முழு
mch_call_shell_terminal(
    char_u	*cmd,
    முழு		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    எழுத்து	**argv = வெற்று;
    char_u	*tofree1 = வெற்று;
    char_u	*tofree2 = வெற்று;
    முழு		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    எனில் (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	செல் theend;

    init_job_options(&opt);
    ch_log(வெற்று, "starting terminal for system command '%s'", cmd);
    buf = term_start(வெற்று, argv, &opt, TERM_START_SYSTEM);
    எனில் (buf == வெற்று)
	செல் theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    எனில் (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	போது (term_use_loop())
	{
	    எனில் (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		எனில் (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    இல்லையெனில்
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(வெற்று, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    எனில் (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    திரும்பு retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    நிலையான முழு
mch_call_shell_system(
    char_u	*cmd,
    முழு		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    எழுத்து	*ifn = வெற்று;
    எழுத்து	*ofn = வெற்று;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    முழு		x;

    out_flush();

    எனில் (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# எனில் defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  எனில் defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    எனில் (cmd == வெற்று)
	x = system((எழுத்து *)p_sh);
    இல்லையெனில்
    {
# ifdef VMS
	எனில் (ofn = strchr((எழுத்து *)cmd, '>'))
	    *ofn++ = '\0';
	எனில் (ifn = strchr((எழுத்து *)cmd, '<'))
	{
	    எழுத்து *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    எனில் (p)
		*p = '\0';
	}
	எனில் (ofn)
	    x = vms_sys((எழுத்து *)cmd, ofn, ifn);
	இல்லையெனில்
	    x = system((எழுத்து *)cmd);
# இல்லையெனில்
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == வெற்று ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	எனில் (newcmd == வெற்று)
	    x = 0;
	இல்லையெனில்
	{
	    sprintf((எழுத்து *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == வெற்று ? "" : (எழுத்து *)extra_shell_arg,
		    (எழுத்து *)p_shcf,
		    (எழுத்து *)cmd);
	    x = system((எழுத்து *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    எனில் (emsg_silent)
	;
    இல்லையெனில் எனில் (x == 127)
	செய்தி_puts(_("\nCannot execute shell sh\n"));
    இல்லையெனில் எனில் (x && !(options & SHELL_SILENT))
    {
	செய்தி_puts(_("\nshell returned "));
	செய்தி_outnum((long)x);
	செய்தி_putchar('\n');
    }

    எனில் (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# எனில் defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    திரும்பு x;
}

#இல்லையெனில் // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    நிலையான முழு
mch_call_shell_fork(
    char_u	*cmd,
    முழு		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    முழு		status = -1;
    முழு		retval = -1;
    எழுத்து	**argv = வெற்று;
    char_u	*tofree1 = வெற்று;
    char_u	*tofree2 = வெற்று;
    முழு		i;
    முழு		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    முழு		pty_slave_fd = -1;
# endif
    முழு		fd_toshell[2];		// for pipes
    முழு		fd_fromshell[2];
    முழு		pipe_error = FALSE;
    முழு		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    எனில் (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    எனில் (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    எனில் (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	செல் error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    எனில் ((options & (SHELL_READ|SHELL_WRITE))
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
	எனில் (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, வெற்று, வெற்று);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	எனில் (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    எனில் (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		எனில் (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    எனில் (pipe_error)
	    {
		செய்தி_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    எனில் (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	எனில் (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    செய்தி_puts(_("\nCannot fork\n"));
	    எனில் ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		எனில் (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		இல்லையெனில்				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	இல்லையெனில் எனில் (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    எனில் (ch_log_active())
	    {
		ch_log(வெற்று, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    எனில் (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		முழு fd;

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
		எனில் (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    இல்லையெனில் எனில் ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		எனில் (p_stmp)
		{
		    (வெறுமை)setsid();
#  எனில் defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		எனில் (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    எனில் (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (எழுத்து *)வெற்று);
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
		எனில் (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    எனில் (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		இல்லையெனில்
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
		    எனில் (gui.in_use)
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
	இல்லையெனில்			// parent
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
	    எனில் ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		முழு	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		முழு	    ta_len = 0;		// valid bytes in ta_buf[]
		முழு	    len;
		முழு	    p_more_save;
		முழு	    old_State;
		முழு	    c;
		முழு	    toshell_fd;
		முழு	    fromshell_fd;
		garray_T    ga;
		முழு	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		எனில் (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		இல்லையெனில்
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

		எனில் ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    எனில் ((wpid = fork()) == -1)
		    {
			செய்தி_puts(_("\nCannot fork\n"));
		    }
		    இல்லையெனில் எனில் (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			க்கு (;;)
			{
			    எனில் (lplen == 0)
				len = 0;
			    இல்லையெனில் எனில் (lp[written] == NL)
				// NL -> NUL translation
				len = லிக(toshell_fd, "", (size_t)1);
			    இல்லையெனில்
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = லிக(toshell_fd, (எழுத்து *)lp + written,
					   s == வெற்று ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    எனில் (len == (முழு)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				எனில் (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_வரி_மொத்தம்
						    || curbuf->b_p_eol)))
				    vim_ignored = லிக(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				எனில் (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    நிறுத்து;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    இல்லையெனில் எனில் (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    இல்லையெனில் // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		எனில் (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		க்கு (;;)
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
		    எனில் (!(options & SHELL_EXPAND)
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
		      எனில் (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      எனில் (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			எனில் (len == 1 && (pty_master_fd < 0 || cmd != வெற்று))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    எனில் (pty_master_fd < 0 && toshell_fd >= 0
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
			எனில் (pty_master_fd < 0)
			{
			    க்கு (i = ta_len; i < ta_len + len; ++i)
			    {
				எனில் (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    செய்தி_putchar(ta_buf[i]);
				இல்லையெனில் எனில் (has_mbyte)
				{
				    முழு l = (*mb_ptr2len)(ta_buf + i);

				    செய்தி_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				இல்லையெனில்
				    செய்தி_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
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
			எனில் (options & SHELL_WRITE)
			    ta_len = 0;
			இல்லையெனில் எனில் (toshell_fd >= 0)
			{
			    len = லிக(toshell_fd, (எழுத்து *)ta_buf, (size_t)1);
			    எனில் (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    எனில் (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# இல்லையெனில்
			kill(0, SIGINT);
# endif
			எனில் (wpid > 0)
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
		    போது (RealWaitForChar(fromshell_fd, 10L, வெற்று, வெற்று))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			எனில் (len <= 0)		    // end of file or error
			    செல் finished;

			noread_cnt = 0;
			எனில் (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    க்கு (i = 0; i < len; ++i)
			    {
				எனில் (buffer[i] == NL)
				    append_ga_வரி(&ga);
				இல்லையெனில் எனில் (buffer[i] == NUL)
				    ga_append(&ga, NL);
				இல்லையெனில்
				    ga_append(&ga, buffer[i]);
			    }
			}
			இல்லையெனில் எனில் (has_mbyte)
			{
			    முழு		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    க்கு (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				எனில் (l == 0)
				    l = 1;  // NUL byte?
				இல்லையெனில் எனில் (MB_BYTE2LEN(*p) != l)
				    நிறுத்து;
			    }
			    எனில் (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				எனில் (len >= 12)
				    ++p;
				இல்லையெனில்
				{
				    buffer_off = len;
				    தொடர்;
				}
			    }
			    c = *p;
			    *p = NUL;
			    செய்தி_puts((எழுத்து *)buffer);
			    எனில் (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				தொடர்;
			    }
			    buffer_off = 0;
			}
			இல்லையெனில்
			{
			    buffer[len] = NUL;
			    செய்தி_puts((எழுத்து *)buffer);
			}

			windgoto(செய்தி_row, cmdline_col_off + செய்தி_col);
			cursor_on();
			out_flush();
			எனில் (got_int)
			    நிறுத்து;

# ifdef ELAPSED_FUNC
			எனில் (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    எனில் (msec > 2000)
			    {
				noread_cnt = 5;
				நிறுத்து;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    எனில் (wait_pid == pid)
		    {
			எனில் (noread_cnt < 5)
			    தொடர்;
			நிறுத்து;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    எனில் ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    இல்லையெனில்
			wait_pid = 0;

# எனில் defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		எனில் (options & SHELL_READ)
		{
		    எனில் (ga.ga_len > 0)
		    {
			append_ga_வரி(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    இல்லையெனில்
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		எனில் (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		எனில் (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# எனில் (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    இல்லையெனில்
	    {
		long delay_msec = 1;

		எனில் (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		க்கு (;;)
		{
		    எனில் (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  இல்லையெனில்
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    எனில் ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			நிறுத்து;
		    }

#  எனில் defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    எனில் (++delay_msec > 10)
			delay_msec = 10;
		}

		எனில் (tmode == TMODE_RAW)
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
	    எனில் (wait_pid != pid)
		(வெறுமை)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    எனில் (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    எனில் (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, வெற்று);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    எனில் (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    எனில் (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		எனில் (retval != 0 && !emsg_silent)
		{
		    எனில் (retval == EXEC_FAILED)
		    {
			செய்தி_puts(_("\nCannot execute shell "));
			செய்தி_outtrans(p_sh);
			செய்தி_putchar('\n');
		    }
		    இல்லையெனில் எனில் (!(options & SHELL_SILENT))
		    {
			செய்தி_puts(_("\nshell returned "));
			செய்தி_outnum((long)retval);
			செய்தி_putchar('\n');
		    }
		}
	    }
	    இல்லையெனில்
		செய்தி_puts(_("\nCommand terminated\n"));
	}
    }

error:
    எனில் (!did_settmode)
	எனில் (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    திரும்பு retval;
}
#endif // USE_SYSTEM

    முழு
mch_call_shell(
    char_u	*cmd,
    முழு		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(வெற்று, "executing shell command: %s", cmd);
#endif
#எனில் defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    எனில் (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != வெற்று
					      && (options & SHELL_SILENT) == 0)
	திரும்பு mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    திரும்பு mch_call_shell_system(cmd, options);
#இல்லையெனில்
    திரும்பு mch_call_shell_fork(cmd, options);
#endif
}

#எனில் defined(FEAT_EVAL)
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
    எழுத்து	**argv,
    char_u	*infile,
    முழு		flags UNUSED,
    முழு		*ret_len)
{
    pid_t	pid;
    முழு		fd_out[2] = {-1, -1};
    முழு		status = -1;
    char_u	*buffer = வெற்று;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(வெற்று, "directly executing: %s", argv[0]);

    எனில் (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	திரும்பு வெற்று;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# இல்லையெனில்
    pid = fork();
# endif
    எனில் (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	திரும்பு வெற்று;
    }

    எனில் (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	எனில் (ch_log_active())
	{
	    ch_log(வெற்று, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	எனில் (infile != வெற்று)
	{
	    முழு fd_in = open((எழுத்து *)infile, O_RDONLY);
	    எனில் (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	இல்லையெனில்
	{
	    முழு nullfd = open("/dev/null", O_RDONLY);
	    எனில் (nullfd >= 0)
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
    க்கு (;;)
    {
	எழுத்து	buf[4096];
	முழு	n;

	n = (முழு)பட(fd_out[0], buf, அளவு(buf));
	எனில் (n <= 0)
	    நிறுத்து;
	எனில் (ga_grow(&ga, n) == FAIL)
	    நிறுத்து;
	mch_memmove((எழுத்து *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (வெறுமை)waitpid(pid, &status, 0);
    எனில் (WIFEXITED(status))
	status = WEXITSTATUS(status);
    இல்லையெனில்
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    எனில் (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	எனில் (buffer != வெற்று)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    எனில் (ret_len == வெற்று)
	    {
		முழு	i;

		// Change NUL into SOH, otherwise the string is truncated.
		க்கு (i = 0; i < ga.ga_len; ++i)
		    எனில் (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    இல்லையெனில்
		*ret_len = ga.ga_len;
	}
    }
    இல்லையெனில் எனில் (ret_len != வெற்று)
	*ret_len = 0;

    ga_clear(&ga);
    திரும்பு buffer;
}
#endif

#எனில் defined(FEAT_JOB_CHANNEL)
    வெறுமை
mch_job_start(எழுத்து **argv, job_T *job, jobopt_T *options, முழு is_terminal)
{
    pid_t	pid;
    முழு		fd_in[2] = {-1, -1};	// for stdin
    முழு		fd_out[2] = {-1, -1};	// for stdout
    முழு		fd_err[2] = {-1, -1};	// for stderr
    முழு		pty_master_fd = -1;
    முழு		pty_slave_fd = -1;
    channel_T	*channel = வெற்று;
    முழு		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    முழு		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    முழு		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    முழு		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    முழு		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    முழு		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    முழு		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    முழு		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    எனில் (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    எனில் (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    எனில் (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((எழுத்து *)fname, O_RDONLY, 0);
	எனில் (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    இல்லையெனில்
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	எனில் (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    செல் failed;

    எனில் (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((எழுத்து *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	எனில் (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    இல்லையெனில் எனில் (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	செல் failed;

    எனில் (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((எழுத்து *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	எனில் (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    செல் failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    இல்லையெனில் எனில் (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	செல் failed;

    எனில் (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	எனில் (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    எனில் (channel != வெற்று)
		++channel->ch_refcount;
	}
	இல்லையெனில்
	    channel = add_channel();
	எனில் (channel == வெற்று)
	    செல் failed;
	எனில் (job->jv_tty_out != வெற்று)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    எனில் (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	செல் failed;
    }
    எனில் (pid == 0)
    {
	முழு	null_fd = -1;
	முழு	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	எனில் (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(வெறுமை)setsid();
# endif

# ifdef FEAT_TERMINAL
	எனில் (options->jo_term_rows > 0)
	{
	    எழுத்து *term = (எழுத்து *)T_NAME;

#  ifdef FEAT_GUI
	    எனில் (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    எனில் (term == வெற்று || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		எனில் (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		இல்லையெனில் எனில் (t_colors > 16)
		    term = "xterm-color";
		இல்லையெனில்
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	இல்லையெனில்
# endif
	    set_default_child_environment(is_terminal);

	எனில் (options->jo_env != வெற்று)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    முழு		todo = (முழு)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_பொருட்கள்(&dict->dv_hashtab, hi, todo)
		எனில் (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_சாவி, tv_get_string(item));
		    --todo;
		}
	}

	எனில் (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    எனில் (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	எனில் (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (எழுத்து *)வெற்று);
# endif
	}

	// set up stdin for the child
	close(0);
	எனில் (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	இல்லையெனில் எனில் (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	இல்லையெனில்
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	எனில் (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	இல்லையெனில் எனில் (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	இல்லையெனில் எனில் (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	இல்லையெனில்
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	எனில் (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	இல்லையெனில் எனில் (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	இல்லையெனில்
	    vim_ignored = dup(fd_out[1]);

	எனில் (fd_in[0] >= 0)
	    close(fd_in[0]);
	எனில் (fd_in[1] >= 0)
	    close(fd_in[1]);
	எனில் (fd_out[0] >= 0)
	    close(fd_out[0]);
	எனில் (fd_out[1] >= 0)
	    close(fd_out[1]);
	எனில் (fd_err[0] >= 0)
	    close(fd_err[0]);
	எனில் (fd_err[1] >= 0)
	    close(fd_err[1]);
	எனில் (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	எனில் (null_fd >= 0)
	    close(null_fd);

	எனில் (options->jo_cwd != வெற்று && mch_chdir((எழுத்து *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	எனில் (stderr_works)
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

    எனில் (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    எனில் (fd_in[0] >= 0)
	close(fd_in[0]);
    எனில் (fd_out[1] >= 0)
	close(fd_out[1]);
    எனில் (fd_err[1] >= 0)
	close(fd_err[1]);
    எனில் (channel != வெற்று)
    {
	முழு in_fd = INVALID_FD;
	முழு out_fd = INVALID_FD;
	முழு err_fd = INVALID_FD;

	எனில் (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	எனில் (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	எனில் (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    எனில் (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    இல்லையெனில் எனில் (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    இல்லையெனில்
    {
	எனில் (fd_in[1] >= 0)
	    close(fd_in[1]);
	எனில் (fd_out[0] >= 0)
	    close(fd_out[0]);
	எனில் (fd_err[0] >= 0)
	    close(fd_err[0]);
	எனில் (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    திரும்பு;

failed:
    channel_unref(channel);
    எனில் (fd_in[0] >= 0)
	close(fd_in[0]);
    எனில் (fd_in[1] >= 0)
	close(fd_in[1]);
    எனில் (fd_out[0] >= 0)
	close(fd_out[0]);
    எனில் (fd_out[1] >= 0)
	close(fd_out[1]);
    எனில் (fd_err[0] >= 0)
	close(fd_err[0]);
    எனில் (fd_err[1] >= 0)
	close(fd_err[1]);
    எனில் (pty_master_fd >= 0)
	close(pty_master_fd);
    எனில் (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    நிலையான char_u *
get_signal_name(முழு sig)
{
    முழு		i;
    char_u	numbuf[NUMBUFLEN];

    எனில் (sig == SIGKILL)
	திரும்பு vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    க்கு (i = 0; signal_info[i].sig != -1; i++)
	எனில் (sig == signal_info[i].sig)
	    திரும்பு strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((எழுத்து *)numbuf, NUMBUFLEN, "%d", sig);
    திரும்பு vim_strnsave(numbuf, i);
}

    எழுத்து *
mch_job_status(job_T *job)
{
    முழு		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    எனில் (wait_pid == -1)
    {
	முழு waitpid_errno = errno;
	எனில் (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    திரும்பு "run";

	// process must have exited
	எனில் (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	செல் return_dead;
    }
    எனில் (wait_pid == 0)
	திரும்பு "run";
    எனில் (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	எனில் (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	செல் return_dead;
    }
    எனில் (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	எனில் (job->jv_status < JOB_ENDED && job->jv_termsig != வெற்று)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	செல் return_dead;
    }
    திரும்பு "run";

return_dead:
    எனில் (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    திரும்பு "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    முழு		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    எனில் (dont_check_job_ended > 0)
	திரும்பு வெற்று;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    எனில் (wait_pid <= 0)
	// no process ended
	திரும்பு வெற்று;
    க்கு (job = job_list; job != வெற்று; job = job->jv_next)
    {
	எனில் (job->jv_pid == wait_pid)
	{
	    எனில் (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    இல்லையெனில் எனில் (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    எனில் (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    திரும்பு job;
	}
    }
    திரும்பு வெற்று;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    முழு
mch_signal_job(job_T *job, char_u *how)
{
    முழு	    sig = -1;

    எனில் (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    இல்லையெனில் எனில் (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    இல்லையெனில் எனில் (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    இல்லையெனில் எனில் (STRCMP(how, "int") == 0)
	sig = SIGINT;
    இல்லையெனில் எனில் (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    இல்லையெனில் எனில் (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    இல்லையெனில் எனில் (SAFE_isdigit(*how))
	sig = atoi((எழுத்து *)how);
    இல்லையெனில்
	திரும்பு FAIL;

    // Never kill ourselves!
    எனில் (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    திரும்பு OK;
}

/*
 * Clear the data related to "job".
 */
    வெறுமை
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (வெறுமை)waitpid(job->jv_pid, வெற்று, WNOHANG);
}
#endif

#எனில் defined(FEAT_TERMINAL)
    முழு
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    முழு		pty_master_fd = -1;
    முழு		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    எனில் (pty_master_fd < 0 || pty_slave_fd < 0)
	திரும்பு FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    எனில் (channel == வெற்று)
    {
	close(pty_master_fd);
	திரும்பு FAIL;
    }
    எனில் (job->jv_tty_out != வெற்று)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    திரும்பு OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    வெறுமை
mch_breakcheck(முழு force)
{
    எனில் ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, வெற்று, வெற்று))
	fill_உள்ளீடு_buf(FALSE);
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
    நிலையான முழு
WaitForChar(long msec, முழு *interrupted, முழு ignore_உள்ளீடு)
{
#ifdef FEAT_TIMERS
    திரும்பு ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_உள்ளீடு) == OK;
#இல்லையெனில்
    திரும்பு WaitForCharOrMouse(msec, interrupted, ignore_உள்ளீடு);
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
    நிலையான முழு
WaitForCharOrMouse(long msec, முழு *interrupted, முழு ignore_உள்ளீடு)
{
#ifdef FEAT_MOUSE_GPM
    முழு		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    முழு		rest;
#endif
    முழு		avail;

    எனில் (!ignore_உள்ளீடு && உள்ளீடு_available())	    // something in inbuf[]
	திரும்பு 1;

#எனில் defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    எனில் (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	எனில் (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#எனில் defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    எனில் (do_xterm_trace())
	rest = msec;
# endif
    செய்
    {
# ifdef FEAT_XCLIPBOARD
	எனில் (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    எனில் (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    எனில் (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	எனில் (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	எனில் (!avail && !gpm_process_wanted)
# இல்லையெனில்
	avail = RealWaitForChar(read_cmd_fd, msec, வெற்று, interrupted);
	எனில் (!avail)
# endif
	{
	    எனில் (!ignore_உள்ளீடு && உள்ளீடு_available())
		திரும்பு 1;
# ifdef FEAT_XCLIPBOARD
	    எனில் (rest == 0 || !do_xterm_trace())
# endif
		நிறுத்து;
	}
    }
    போது (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#இல்லையெனில்
    avail = RealWaitForChar(read_cmd_fd, msec, வெற்று, interrupted);
#endif
    திரும்பு avail;
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
    நிலையான முழு
RealWaitForChar(முழு fd, long msec, முழு *check_for_gpm UNUSED, முழு *interrupted)
{
    முழு		ret;
    முழு		result;
# எனில் defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    நிலையான முழு	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    எனில் (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    எனில் (busy)
	திரும்பு 0;
# endif

# ifdef MAY_LOOP
    க்கு (;;)
# endif
    {
# ifdef MAY_LOOP
	முழு		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	முழு		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	முழு		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	கட்டமைப்பு pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	முழு		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	முழு             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	முழு		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	முழு		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	முழு		xsmp_idx = -1;
#  endif
	முழு		towait = (முழு)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	எனில் (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (முழு)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	எனில் ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	எனில் (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_எண்(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	எனில் (check_for_gpm != வெற்று && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	எனில் (xsmp_icefd != -1)
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
	எனில் (interrupted != வெற்று)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	எனில் (result == 0 && interrupted != வெற்று && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	எனில் (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	எனில் (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	எனில் (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    எனில் (--ret == 0 && !உள்ளீடு_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	எனில் (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	எனில் (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    எனில் (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    இல்லையெனில் எனில் (fds[xsmp_idx].revents & POLLHUP)
	    {
		எனில் (p_verbose > 0)
		    verb_செய்தி(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    எனில் (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	எனில் (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# இல்லையெனில் // HAVE_SELECT

	கட்டமைப்பு timeval  tv;
	கட்டமைப்பு timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	நிலையான fd_set	rfds, wfds, efds;
	முழு		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	எனில் (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	எனில் (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	இல்லையெனில்
	    tvp = வெற்று;

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
	எனில் ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    எனில் (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	எனில் (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_எண்(xterm_dpy), &rfds);
	    எனில் (maxfd < Connection_எண்(xterm_dpy))
		maxfd = Connection_எண்(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	எனில் (check_for_gpm != வெற்று && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    எனில் (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	எனில் (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    எனில் (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	எனில் (interrupted != வெற்று)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	எனில் (result)
	    --ret;
	இல்லையெனில் எனில் (interrupted != வெற்று && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	எனில் (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    எனில் (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    எனில் (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(வெற்று, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    செல் select_eintr;
	}
#  endif
#  ifdef __TANDEM
	எனில் (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	எனில் (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	எனில் (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	எனில் (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_எண்(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    எனில் (--ret == 0 && !உள்ளீடு_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	எனில் (ret > 0 && check_for_gpm != வெற்று && gpm_flag && gpm_fd >= 0)
	{
	    எனில் (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    இல்லையெனில் எனில் (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	எனில் (ret > 0 && xsmp_icefd != -1)
	{
	    எனில் (FD_ISSET(xsmp_icefd, &efds))
	    {
		எனில் (p_verbose > 0)
		    verb_செய்தி(_("XSMP lost ICE connection"));
		xsmp_close();
		எனில் (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    இல்லையெனில் எனில் (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		எனில் (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	எனில் (ret >= 0)
	    (வெறுமை)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	எனில் (finished || msec == 0)
	    நிறுத்து;

#  எனில் defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	எனில் (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    நிறுத்து;
#  endif

	// We're going to loop around again, find out for how long
	எனில் (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  இல்லையெனில்
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    எனில் (msec <= 0)
		நிறுத்து;	// waited long enough
	}
# endif
    }

    திரும்பு result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    முழு
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    முழு		flags)		// EW_* flags
{
    திரும்பு unix_expandpath(gap, path, 0, flags, FALSE);
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

    முழு
mch_expand_wildcards(
    முழு		   இலக்கம்_pat,
    char_u	 **pat,
    முழு		  *இலக்கம்_file,
    char_u	***file,
    முழு		   flags)	// EW_* flags
{
    முழு		i;
    size_t	len;
    long	llen;
    char_u	*p;
    முழு		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    முழு		j;
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
    முழு		shell_style = STYLE_ECHO;
    முழு		check_spaces;
    நிலையான முழு	did_find_nul = FALSE;
    முழு		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    நிலையான string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    நிலையான string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *இலக்கம்_file = 0;	// default: no files found
    *file = வெற்று;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    எனில் (!have_wildcard(இலக்கம்_pat, pat))
	திரும்பு save_patterns(இலக்கம்_pat, pat, இலக்கம்_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    எனில் (sandbox != 0 && check_secure())
	திரும்பு FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    எனில் (secure || restricted)
	க்கு (i = 0; i < இலக்கம்_pat; ++i)
	    எனில் (vim_strchr(pat[i], '`') != வெற்று
		    && (check_restricted() || check_secure()))
		திரும்பு FAIL;

    /*
     * get a name for the temp file
     */
    எனில் ((tempname = vim_tempname('o', FALSE)) == வெற்று)
    {
	emsg(_(e_cant_get_temp_file_name));
	திரும்பு FAIL;
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
    எனில் (இலக்கம்_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    இல்லையெனில் எனில் ((len = STRLEN(p_sh)) >= 3)
    {
	எனில் (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	இல்லையெனில் எனில் (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    எனில் (shell_style == STYLE_ECHO)
    {
	எனில் (strstr((எழுத்து *)gettail(p_sh), "bash") != வெற்று)
	    shell_style = STYLE_GLOBSTAR;
	இல்லையெனில் எனில் (strstr((எழுத்து *)gettail(p_sh), "sh") != வெற்று)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    எனில் (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    இல்லையெனில் எனில் (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    க்கு (i = 0; i < இலக்கம்_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# இல்லையெனில்
	++len;				// add space
	க்கு (j = 0; pat[i][j] != NUL; ++j)
	{
	    எனில் (vim_strchr(SHELL_SPECIAL, pat[i][j]) != வெற்று)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    எனில் (command == வெற்று)
    {
	// out of memory
	vim_free(tempname);
	திரும்பு FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    எனில் (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((எழுத்து *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((எழுத்து *)command, ")>");
	எனில் (p == வெற்று)
	{
	    ;					    // can't happen ;-)
	}
	இல்லையெனில்
	{
	    --p;
	    போது (p > command && VIM_ISWHITE(*p))
		--p;
	    எனில் (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    போது (p > command && VIM_ISWHITE(*p))
		--p;
	    எனில் (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    இல்லையெனில்
    {
	எனில் (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((எழுத்து *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	இல்லையெனில் எனில் (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((எழுத்து *)command, len, "print -N >%s", tempname);
	இல்லையெனில் எனில் (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((எழுத்து *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	இல்லையெனில் எனில் (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((எழுத்து *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	இல்லையெனில்
	    commandlen = vim_snprintf((எழுத்து *)command, len, "echo >%s", tempname);

	க்கு (i = 0; i < இலக்கம்_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((எழுத்து *)command + commandlen, len, " \"%s\"", pat[i]);
# இல்லையெனில்
	    முழு intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    க்கு (j = 0; pat[i][j] != NUL; ++j)
	    {
		எனில் (pat[i][j] == '`')
		    intick = !intick;
		இல்லையெனில் எனில் (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    எனில் (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != வெற்று
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		இல்லையெனில் எனில் (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != வெற்று)
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
    எனில் (flags & EW_SILENT)
	show_shell_mess = FALSE;
    எனில் (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    எனில் (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    இல்லையெனில் எனில் (shell_style == STYLE_GLOB && !have_dollars(இலக்கம்_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    எனில் (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = வெற்று;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    எனில் (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	எனில் (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    செய்தி_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    எனில் (!(flags & EW_SILENT))
# endif
	    {
		செய்தி(_(e_cannot_expand_wildcards));
		செய்தி_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	எனில் (shell_style == STYLE_BT)
	    திரும்பு FAIL;
	செல் notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((எழுத்து *)tempname, READBIN);
    எனில் (fd == வெற்று)
    {
	// Something went wrong, perhaps a file name with a special char.
	எனில் (!(flags & EW_SILENT))
	{
	    செய்தி(_(e_cannot_expand_wildcards));
	    செய்தி_start();		// don't overwrite this message
	}
	vim_free(tempname);
	செல் notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    எனில் (llen < 0)
	// just in case ftell() would fail
	buffer = வெற்று;
    இல்லையெனில்
	buffer = alloc(llen + 1);
    எனில் (buffer == வெற்று)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	திரும்பு FAIL;
    }
    len = llen;
    i = fread((எழுத்து *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    எனில் (i != (முழு)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	திரும்பு FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    க்கு (i = 0; i < (முழு)len; ++i)
	எனில் (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    எனில் (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	க்கு (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    போது (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    இல்லையெனில் எனில் (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	க்கு (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    போது (*p != '\n' && *p != NUL)
		++p;
	    எனில் (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    இல்லையெனில்
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
	எனில் (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    எனில் (len && (முழு)STRLEN(buffer) < (முழு)len)
		did_find_nul = TRUE;
	    இல்லையெனில்
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	எனில் (len && buffer[len - 1] == NUL)
	    --len;
	இல்லையெனில்
	    buffer[len] = NUL;
	i = 0;
	க்கு (p = buffer; p < buffer + len; ++p)
	    எனில் (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	எனில் (len)
	    ++i;			// count last entry
    }
    எனில் (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	செல் notfound;
    }
    *இலக்கம்_file = i;
    *file = ALLOC_MULT(char_u *, i);
    எனில் (*file == வெற்று)
    {
	// out of memory
	vim_free(buffer);
	திரும்பு FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    க்கு (i = 0; i < *இலக்கம்_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	எனில் (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    போது (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    எனில் (p == buffer + len)		// last entry
		*p = NUL;
	    இல்லையெனில்
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	இல்லையெனில்		// NUL separates
	{
	    போது (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    க்கு (j = 0, i = 0; i < *இலக்கம்_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	எனில் (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    தொடர்;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	எனில் ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    தொடர்;

	// Skip files that are not executable if we check for that.
	எனில் (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], வெற்று, !(flags & EW_SHELLCMD)))
	    தொடர்;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	எனில் (p)
	{
	    STRCPY(p, (*file)[i]);
	    எனில் (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *இலக்கம்_file = j;

    எனில் (*இலக்கம்_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	செல் notfound;
    }

    திரும்பு OK;

notfound:
    எனில் (flags & EW_NOTFOUND)
	திரும்பு save_patterns(இலக்கம்_pat, pat, இலக்கம்_file, file);
    திரும்பு FAIL;
}

#endif // VMS

    நிலையான முழு
save_patterns(
    முழு		இலக்கம்_pat,
    char_u	**pat,
    முழு		*இலக்கம்_file,
    char_u	***file)
{
    முழு		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, இலக்கம்_pat);
    எனில் (*file == வெற்று)
	திரும்பு FAIL;
    க்கு (i = 0; i < இலக்கம்_pat; i++)
    {
	s = vim_strsave(pat[i]);
	எனில் (s != வெற்று)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *இலக்கம்_file = இலக்கம்_pat;
    திரும்பு OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    முழு
mch_has_exp_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில்
	    எனில் (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#இல்லையெனில்
				    "*?[{'"
#endif
						, *p) != வெற்று)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    முழு
mch_has_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில்
	    எனில் (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#இல்லையெனில்
				    "*?[{`'$"
#endif
						, *p) != வெற்று
		|| (*p == '~' && p[1] != NUL))
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    நிலையான முழு
have_wildcard(முழு இலக்கம், char_u **file)
{
    முழு	    i;

    க்கு (i = 0; i < இலக்கம்; i++)
	எனில் (mch_has_wildcard(file[i]))
	    திரும்பு 1;
    திரும்பு 0;
}

    நிலையான முழு
have_dollars(முழு இலக்கம், char_u **file)
{
    முழு	    i;

    க்கு (i = 0; i < இலக்கம்; i++)
	எனில் (vim_strchr(file[i], '$') != வெற்று)
	    திரும்பு TRUE;
    திரும்பு FALSE;
}

#எனில் !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    முழு
mch_rename(மாறிலி எழுத்து *src, மாறிலி எழுத்து *dest)
{
    கட்டமைப்பு stat	    st;

    எனில் (stat(dest, &st) >= 0)	    // fail if destination exists
	திரும்பு -1;
    எனில் (link(src, dest) != 0)	    // link file to new name
	திரும்பு -1;
    எனில் (mch_remove(src) == 0)	    // delete link to old name
	திரும்பு 0;
    திரும்பு -1;
}
#endif // !HAVE_RENAME

#எனில் defined(FEAT_MOUSE_GPM)
# எனில் defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    நிலையான முழு
load_libgpm(வெறுமை)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    எனில் (libgpm_hinst == வெற்று)
    {
	எனில் (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	திரும்பு FAIL;
    }

    எனில் (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == வெற்று
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == வெற்று
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == வெற்று
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == வெற்று
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == வெற்று
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = வெற்று;
	dll_gpm_flag = வெற்று;
	dll_gpm_fd   = வெற்று;
	திரும்பு FAIL;
    }
    திரும்பு OK;
}

    முழு
gpm_available(வெறுமை)
{
    திரும்பு libgpm_hinst != வெற்று || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    நிலையான முழு
gpm_open(வெறுமை)
{
    நிலையான Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    எனில் (!gpm_available())
	திரும்பு 0;
# endif

    எனில் (gpm_flag)
	திரும்பு 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    எனில் (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	திரும்பு 1; // succeed
    }
    எனில் (gpm_fd == -2)
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
    திரும்பு 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    முழு
gpm_enabled(வெறுமை)
{
    திரும்பு gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    நிலையான வெறுமை
gpm_close(வெறுமை)
{
    எனில் (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    நிலையான முழு
mch_gpm_process(வெறுமை)
{
    முழு			button;
    நிலையான Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    முழு			row,col;
    unsigned எழுத்து	buttons_mask;
    unsigned எழுத்து	gpm_modifiers;
    நிலையான unsigned எழுத்து old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    தேர்வு (GPM_BARE_EVENTS(gpm_event.type))
    {
	நிலை GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    நிறுத்து;
	நிலை GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    தேர்வு (buttons_mask)
	    {
		நிலை GPM_B_இடது:
		    button = MOUSE_இடது;
		    நிறுத்து;
		நிலை GPM_B_நடு:
		    button = MOUSE_நடு;
		    நிறுத்து;
		நிலை GPM_B_வலது:
		    button = MOUSE_வலது;
		    நிறுத்து;
		இயல்பு:
		    திரும்பு 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_இலக்கம்_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    நிறுத்து;
	நிலை GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    நிறுத்து;
	இயல்பு:
	    திரும்பு 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    எனில் (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    எனில் (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    எனில் (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_உள்ளீடு_buf(string, 6);
    திரும்பு 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    நிலையான முழு
sysmouse_open(வெறுமை)
{
    கட்டமைப்பு mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.சமிக்ஞை = SIGUSR2;
    எனில் (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	திரும்பு FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    திரும்பு OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    நிலையான வெறுமை
sysmouse_close(வெறுமை)
{
    கட்டமைப்பு mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.சமிக்ஞை = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    நிலையான வெறுமை
sig_sysmouse SIGDEFARG(sigarg)
{
    கட்டமைப்பு mouse_info	mouse;
    கட்டமைப்பு video_info	video;
    char_u		string[6];
    முழு			row, col;
    முழு			button;
    முழு			buttons;
    நிலையான முழு		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    எனில் (hold_gui_events)
	திரும்பு;
# endif

    mouse.operation = MOUSE_GETINFO;
    எனில் (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	எனில் (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	இல்லையெனில்
	{
	    தேர்வு (buttons)
	    {
		நிலை 0:
		    button = MOUSE_RELEASE;
		    நிறுத்து;
		நிலை 1:
		    button = MOUSE_இடது;
		    நிறுத்து;
		நிலை 2:
		    button = MOUSE_நடு;
		    நிறுத்து;
		நிலை 4:
		    button = MOUSE_வலது;
		    நிறுத்து;
		இயல்பு:
		    திரும்பு;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_உள்ளீடு_buf(string, 6);
    }
    திரும்பு;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    முழு
mch_get_random(char_u *buf, முழு len)
{
    நிலையான முழு dev_urandom_state = NOTDONE;

    எனில் (dev_urandom_state == FAIL)
	திரும்பு FAIL;

    முழு fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    எனில் (fd == -1)
	dev_urandom_state = FAIL;
    இல்லையெனில் எனில் (பட(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    இல்லையெனில்
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    திரும்பு dev_urandom_state;
}

#எனில் defined(FEAT_LIBCALL)
வகைவரை char_u * (*STRPROCSTR)(char_u *);
வகைவரை char_u * (*INTPROCSTR)(முழு);
வகைவரை முழு (*STRPROCINT)(char_u *);
வகைவரை முழு (*INTPROCINT)(முழு);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    முழு
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    முழு		argint,
    char_u	**string_result, // NULL when using number_result
    முழு		*எண்_result)
{
# எனில் defined(USE_DLOPEN)
    வெறுமை	*hinstLib;
    எழுத்து	*dlerr = வெற்று;
# இல்லையெனில்
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = வெற்று;
    முழு		retval_int = 0;
    முழு		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# எனில் defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (வெறுமை)dlerror();

    hinstLib = dlopen((எழுத்து *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    எனில் (hinstLib == வெற்று)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	எனில் (dlerr != வெற்று)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# இல்லையெனில்
    hinstLib = shl_load((மாறிலி எழுத்து*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    எனில் (hinstLib != வெற்று)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	எனில் (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  எனில் defined(USE_DLOPEN)
	    dlerr = வெற்று;
#  endif
	    mch_didjmp();
	}
	இல்லையெனில்
# endif
	{
	    retval_str = வெற்று;
	    retval_int = 0;

	    எனில் (argstring != வெற்று)
	    {
# எனில் defined(USE_DLOPEN)
		*(வெறுமை **)(&ProcAdd) = dlsym(hinstLib, (மாறிலி எழுத்து *)funcname);
		dlerr = dlerror();
# இல்லையெனில்
		எனில் (shl_findsym(&hinstLib, (மாறிலி எழுத்து *)funcname,
					TYPE_PROCEDURE, (வெறுமை *)&ProcAdd) < 0)
		    ProcAdd = வெற்று;
# endif
		எனில் ((success = (ProcAdd != வெற்று
# எனில் defined(USE_DLOPEN)
			    && dlerr == வெற்று
# endif
			    )))
		{
		    எனில் (string_result == வெற்று)
			retval_int = ((STRPROCINT)(வெறுமை *)ProcAdd)(argstring);
		    இல்லையெனில்
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    இல்லையெனில்
	    {
# எனில் defined(USE_DLOPEN)
		*(வெறுமை **)(&ProcAddI) = dlsym(hinstLib, (மாறிலி எழுத்து *)funcname);
		dlerr = dlerror();
# இல்லையெனில்
		எனில் (shl_findsym(&hinstLib, (மாறிலி எழுத்து *)funcname,
				       TYPE_PROCEDURE, (வெறுமை *)&ProcAddI) < 0)
		    ProcAddI = வெற்று;
# endif
		எனில் ((success = (ProcAddI != வெற்று
# எனில் defined(USE_DLOPEN)
			    && dlerr == வெற்று
# endif
			    )))
		{
		    எனில் (string_result == வெற்று)
			retval_int = ((INTPROCINT)(வெறுமை *)ProcAddI)(argint);
		    இல்லையெனில்
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    எனில் (string_result == வெற்று)
		*எண்_result = retval_int;
	    இல்லையெனில் எனில் (retval_str != வெற்று
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	எனில் (lc_signal != 0)
	{
	    முழு i;

	    // try to find the name of this signal
	    க்கு (i = 0; signal_info[i].sig != -1; i++)
		எனில் (lc_signal == signal_info[i].sig)
		    நிறுத்து;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# எனில் defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	எனில் (dlerr != வெற்று)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(வெறுமை)dlclose(hinstLib);
# இல்லையெனில்
	(வெறுமை)shl_unload(hinstLib);
# endif
    }

    எனில் (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	திரும்பு FAIL;
    }

    திரும்பு OK;
}
#endif

#எனில் defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
நிலையான முழு	xterm_trace = -1;	// default: disabled
நிலையான முழு	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    வெறுமை
setup_term_clip(வெறுமை)
{
    முழு		z = 0;
    எழுத்து	*strp = "";
    Widget	AppShell;

    எனில் (!x_connect_to_server())
	திரும்பு;

    open_app_context();
    எனில் (app_context != வெற்று && xterm_Shell == (Widget)0)
    {
	முழு (*oldhandler)(Display*, XErrorEvent*);
# எனில் defined(USING_SETJMP)
	முழு (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	எனில் (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# எனில் defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	எனில் (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = வெற்று;
	}
	இல்லையெனில்
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", வெற்று, 0, &z, &strp);
	    எனில் (xterm_dpy != வெற்று)
		xterm_dpy_retry_மொத்தம் = 0;
# எனில் defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# எனில் defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(வெறுமை)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(வெறுமை)XSetErrorHandler(oldhandler);

	எனில் (xterm_dpy == வெற்று)
	{
	    எனில் (p_verbose > 0)
		verb_செய்தி(_("Opening the X display failed"));
	    திரும்பு;
	}

	// Catch terminating error of the X server connection.
	(வெறுமை)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	எனில் (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		வெற்று);
	எனில் (AppShell == (Widget)0)
	    திரும்பு;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		உச்சி_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		வெற்று);
	எனில் (xterm_Shell == (Widget)0)
	    திரும்பு;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	எனில் (x11_display == வெற்று)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    எனில் (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	எனில் (x11_window == 0 && (strp = getenv("WINDOWID")) != வெற்று)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	எனில் (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	எனில் (x11_window != 0)
	    xterm_trace = 0;
    }
}

    வெறுமை
start_xterm_trace(முழு button)
{
    எனில் (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	திரும்பு;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    வெறுமை
stop_xterm_trace(வெறுமை)
{
    எனில் (xterm_trace < 0)
	திரும்பு;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    நிலையான முழு
do_xterm_trace(வெறுமை)
{
    Window		வேர், child;
    முழு			வேர்_x, வேர்_y;
    முழு			win_x, win_y;
    முழு			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    நிலையான char_u	*mouse_code = வெற்று;
    நிலையான size_t	mouse_codelen = 0;
    நிலையான char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    நிலையான முழு		prev_row = 0, prev_col = 0;
    நிலையான XSizeHints	xterm_hints;

    எனில் (xterm_trace <= 0)
	திரும்பு FALSE;

    எனில் (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	எனில் (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    திரும்பு FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	எனில் (mouse_code != வெற்று)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &வேர், &child, &வேர்_x, &வேர்_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	எனில் (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	எனில் (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	திரும்பு TRUE;
    }

    எனில் (mouse_code == வெற்று || mouse_codelen > 45)
    {
	xterm_trace = 0;
	திரும்பு FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &வேர், &child, &வேர்_x, &வேர்_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    எனில் (row == prev_row && col == prev_col)
	திரும்பு TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_உள்ளீடு_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    திரும்பு TRUE;
}

# எனில் defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    வெறுமை
clear_xterm_clip(வெறுமை)
{
    எனில் (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    எனில் (xterm_dpy != வெற்று)
    {
#  எனில் 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	எனில் (x11_display == xterm_dpy)
	    x11_display = வெற்று;
	xterm_dpy = வெற்று;
    }
#  எனில் 0
    எனில் (app_context != (XtAppContext)வெற்று)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)வெற்று;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    நிலையான வெறுமை
clip_update(வெறுமை)
{
# ifdef FEAT_GUI
    எனில் (gui.in_use)
	gui_mch_update();
    இல்லையெனில்
# endif
    எனில் (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    வெறுமை
xterm_update(வெறுமை)
{
    XEvent event;

    க்கு (;;)
    {
	Xt_உள்ளீடு_Mask mask = XtAppPending(app_context);

	எனில் (mask == 0 || vim_is_உள்ளீடு_buf_full())
	    நிறுத்து;

	எனில் (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		எனில் (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_மதிப்பு)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	இல்லையெனில்
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    முழு
clip_xterm_own_selection(Clipboard_T *cbd)
{
    எனில் (xterm_Shell != (Widget)0)
	திரும்பு clip_x11_own_selection(xterm_Shell, cbd);
    திரும்பு FAIL;
}

    வெறுமை
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    எனில் (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    வெறுமை
clip_xterm_request_selection(Clipboard_T *cbd)
{
    எனில் (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    வெறுமை
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#எனில் defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# எனில் defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    நிலையான வெறுமை
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    முழு		save_cmod_flags;
    முழு		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    எனில் (check_changed_any(FALSE, FALSE))
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
    எனில் (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    நிலையான வெறுமை
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    முழு		save_type UNUSED,
    Bool	shutdown,
    முழு		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    எனில் (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    எனில் (p_verbose > 0)
	verb_செய்தி(_("XSMP handling save-yourself request"));

# எனில் defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    எனில் (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    இல்லையெனில்
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
    நிலையான வெறுமை
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    நிலையான வெறுமை
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
    நிலையான வெறுமை
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    எனில் (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    நிலையான வெறுமை
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    எனில் (opening)
    {
	xsmp_icefd = Ice_Connection_எண்(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, வெற்று);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    முழு
xsmp_handle_requests(வெறுமை)
{
    Bool rep;

    எனில் (IceProcessMessages(xsmp.iceconn, வெற்று, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	எனில் (p_verbose > 0)
	    verb_செய்தி(_("XSMP lost ICE connection"));
	xsmp_close();
	திரும்பு FAIL;
    }
    இல்லையெனில்
	திரும்பு OK;
}

நிலையான முழு dummy;

// Set up X Session Management Protocol
    வெறுமை
xsmp_init(வெறுமை)
{
    எழுத்து		errorstring[80];
    SmcCallbacks	smcallbacks;
# எனில் 0
    Sm_Prop_மதிப்பு		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    எனில் (p_verbose > 0)
	verb_செய்தி(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = வெற்று;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = வெற்று;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = வெற்று;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = வெற்று;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    எனில் (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	எனில் (p_verbose > 0)
	    verb_செய்தி(_("XSMP ICE connection watch failed"));
	திரும்பு;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    வெற்று,
	    வெற்று,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    வெற்று,
	    &xsmp.clientid,
	    அளவு(errorstring) - 1,
	    errorstring);
    எனில் (xsmp.smcconn == வெற்று)
    {
	எனில் (p_verbose > 0)
	{
	    எழுத்து errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[அளவு(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, அளவு(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_செய்தி(errorreport);
	}
	திரும்பு;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# எனில் 0
    // ID ourselves
    smname.மதிப்பு = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.இலக்கம்_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    வெறுமை
xsmp_close(வெறுமை)
{
    எனில் (xsmp_icefd == -1)
	திரும்பு;

    SmcCloseConnection(xsmp.smcconn, 0, வெற்று);
    எனில் (xsmp.clientid != வெற்று)
	விடுவி(xsmp.clientid);
    xsmp.clientid = வெற்று;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#எனில் defined(FEAT_RELTIME)
# எனில் defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
நிலையான நிலையற்ற sig_atomic_t timeout_flag = FALSE;
நிலையான timer_t		     timer_id;
நிலையான முழு		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    நிலையான வெறுமை
set_flag(ஒன்றிப்பு sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    வெறுமை
stop_timeout(வெறுமை)
{
    நிலையான கட்டமைப்பு itimerspec disarm = {{0, 0}, {0, 0}};

    எனில் (timer_created)
    {
	முழு ret = timer_settime(timer_id, 0, &disarm, வெற்று);

	எனில் (ret < 0)
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
    நிலையற்ற sig_atomic_t *
start_timeout(long msec)
{
    கட்டமைப்பு itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    முழு ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    எனில் (!timer_created)
    {
	கட்டமைப்பு sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	எனில் (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    திரும்பு &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(வெற்று, "setting timeout timer to %d sec %ld nsec",
	       (முழு)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, வெற்று);
    எனில் (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    திரும்பு &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    வெறுமை
delete_timer(வெறுமை)
{
    எனில் (!timer_created)
	திரும்பு;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# இல்லையெனில் // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
நிலையான கட்டமைப்பு sigaction		prev_sigaction;
நிலையான நிலையற்ற sig_atomic_t	timeout_flag	     = FALSE;
நிலையான முழு			timer_active	     = FALSE;
நிலையான முழு			timer_handler_active = FALSE;
நிலையான நிலையற்ற sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    நிலையான வெறுமை
set_flag SIGDEFARG(sigarg)
{
    எனில் (alarm_pending)
	alarm_pending = FALSE;
    இல்லையெனில்
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    வெறுமை
stop_timeout(வெறுமை)
{
    நிலையான கட்டமைப்பு itimerval disarm = {{0, 0}, {0, 0}};
    முழு			    ret;

    எனில் (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, வெற்று);
	எனில் (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    எனில் (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, வெற்று);
	எனில் (ret < 0)
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
    நிலையற்ற sig_atomic_t *
start_timeout(long msec)
{
    கட்டமைப்பு itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    கட்டமைப்பு sigaction	handle_alarm;
    முழு			ret;
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
    எனில் (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, வெற்று);
    }
    எனில் (unlikely(ret != 0 || alarm_pending < 0))
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
    எனில் (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	திரும்பு &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, வெற்று);
    எனில் (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	திரும்பு &timeout_flag;
    }

    timer_active = TRUE;
    திரும்பு &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
