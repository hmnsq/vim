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

#અન્તર્ભાવ "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#અન્તર્ભાવ "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
સ્થિર પૂર્ણાંક selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != ખાલી ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ખાલી ? *dll_gpm_fd   : -1)

સ્થિર પૂર્ણાંક (*dll_Gpm_Open)     (Gpm_Connect *, પૂર્ણાંક);
સ્થિર પૂર્ણાંક (*dll_Gpm_Close)    (શૂન્ય);
સ્થિર પૂર્ણાંક (*dll_Gpm_GetEvent) (Gpm_Event *);
સ્થિર પૂર્ણાંક *dll_gpm_flag;
સ્થિર પૂર્ણાંક *dll_gpm_fd;

સ્થિર શૂન્ય *libgpm_hinst;
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

સ્થિર શૂન્ય gpm_close(શૂન્ય);
સ્થિર પૂર્ણાંક gpm_open(શૂન્ય);
સ્થિર પૂર્ણાંક mch_gpm_process(શૂન્ય);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

સ્થિર પૂર્ણાંક sysmouse_open(શૂન્ય);
સ્થિર શૂન્ય sysmouse_close(શૂન્ય);
સ્થિર શૂન્ય sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#જો defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#જો defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
સ્થિર Widget	xterm_Shell = (Widget)0;
સ્થિર શૂન્ય clip_update(શૂન્ય);
# endif

Window	    x11_window = 0;
Display	    *x11_display = ખાલી;
#endif

સ્થિર પૂર્ણાંક ignore_sigtstp = FALSE;

સ્થિર પૂર્ણાંક get_x11_title(પૂર્ણાંક);

સ્થિર char_u	*oldtitle = ખાલી;
સ્થિર અસ્થિર sig_atomic_t oldtitle_outdated = FALSE;
સ્થિર પૂર્ણાંક	unix_did_set_title = FALSE;
સ્થિર char_u	*oldicon = ખાલી;
સ્થિર પૂર્ણાંક	did_set_icon = FALSE;

સ્થિર શૂન્ય may_core_dump(શૂન્ય);

પ્રકારવ્યાખ્યા પૂર્ણાંક waitstatus;
સ્થિર પૂર્ણાંક  WaitForChar(long msec, પૂર્ણાંક *interrupted, પૂર્ણાંક ignore_આગત);
સ્થિર પૂર્ણાંક  WaitForCharOrMouse(long msec, પૂર્ણાંક *interrupted, પૂર્ણાંક ignore_આગત);
#ifdef VMS
પૂર્ણાંક  RealWaitForChar(પૂર્ણાંક, long, પૂર્ણાંક *, પૂર્ણાંક *interrupted);
#નહીંતર
સ્થિર પૂર્ણાંક  RealWaitForChar(પૂર્ણાંક, long, પૂર્ણાંક *, પૂર્ણાંક *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
સ્થિર પૂર્ણાંક do_xterm_trace(શૂન્ય);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

સ્થિર શૂન્ય handle_resize(શૂન્ય);

#જો defined(SIGWINCH)
સ્થિર શૂન્ય sig_winch SIGPROTOARG;
#endif
#જો defined(SIGTSTP)
સ્થિર શૂન્ય sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
સ્થિર અસ્થિર sig_atomic_t in_mch_suspend = FALSE;
#endif
#જો defined(SIGINT)
સ્થિર શૂન્ય catch_sigint SIGPROTOARG;
#endif
#જો defined(SIGUSR1)
સ્થિર શૂન્ય catch_sigusr1 SIGPROTOARG;
#endif
#જો defined(SIGPWR)
સ્થિર શૂન્ય catch_sigpwr SIGPROTOARG;
#endif
#જો defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
સ્થિર શૂન્ય sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
સ્થિર અસ્થિર sig_atomic_t sig_alarm_called;
#endif
સ્થિર શૂન્ય deathtrap SIGPROTOARG;

સ્થિર શૂન્ય catch_int_signal(શૂન્ય);
સ્થિર શૂન્ય set_signals(શૂન્ય);
સ્થિર શૂન્ય catch_signals(શૂન્ય (*func_deadly)(પૂર્ણાંક), શૂન્ય (*func_other)(પૂર્ણાંક));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#નહીંતર
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	કરો { /**/ } જ્યારે (0)
# define UNBLOCK_SIGNALS(set)	કરો { /**/ } જ્યારે (0)
#endif
સ્થિર પૂર્ણાંક  have_wildcard(પૂર્ણાંક, char_u **);
સ્થિર પૂર્ણાંક  have_dollars(પૂર્ણાંક, char_u **);

સ્થિર પૂર્ણાંક save_patterns(પૂર્ણાંક અંક_pat, char_u **pat, પૂર્ણાંક *અંક_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
સ્થિર અસ્થિર sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
સ્થિર અસ્થિર sig_atomic_t got_tstp = FALSE;
સ્થિર char_u	*extra_shell_arg = ખાલી;
સ્થિર પૂર્ણાંક	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
સ્થિર અસ્થિર sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
સ્થિર અસ્થિર sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#જો defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
સ્થિર પૂર્ણાંક dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
સ્થિર tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
પ્રકારવ્યાખ્યા સંરચના
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    અક્ષર *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

સ્થિર xsmp_config_T xsmp;
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

સ્થિર સંરચના signalinfo
{
    પૂર્ણાંક	    sig;	// Signal number, eg. SIGSEGV etc
    અક્ષર    *name;	// Signal name (not char_u!).
    અક્ષર    deadly;	// Catch as a deadly signal?
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
#જો defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#જો defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#જો defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#જો defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(પૂર્ણાંક sig, sighandler_T func)
{
#જો defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    સંરચના sigaction	sa, old;
    sigset_t		curset;
    પૂર્ણાંક			blocked;

    જો (sigprocmask(SIG_BLOCK, ખાલી, &curset) == -1)
	પાછા SIG_ERR;

    blocked = sigismember(&curset, sig);

    જો (func == SIG_HOLD)
    {
	જો (blocked)
	    પાછા SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	જો (sigaction(sig, ખાલી, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ખાલી) == -1)
	    પાછા SIG_ERR;
	પાછા old.sa_handler;
    }

    જો (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	જો (sigprocmask(SIG_UNBLOCK, &curset, ખાલી) == -1)
	    પાછા SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# નહીંતર
    sa.sa_flags = 0;
# endif
    જો (sigaction(sig, &sa, &old) == -1)
	પાછા SIG_ERR;
    પાછા blocked ? SIG_HOLD: old.sa_handler;
#અથવાજો defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    પાછા sigset(sig, func);
#નહીંતર
    // Oldest and most compatible solution.
    પાછા સંકેત(sig, func);
#endif
}

    પૂર્ણાંક
mch_chdir(અક્ષર *path)
{
    જો (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    પાછા chdir(vms_fixfilename(path));
#નહીંતર
    પાછા chdir(path);
#endif
}

#જો defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    શૂન્ય
mch_write(char_u *s, પૂર્ણાંક len)
{
    vim_ignored = (પૂર્ણાંક)લિખ(1, (અક્ષર *)s, len);
    જો (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ખાલી, ખાલી);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    સ્થિર પૂર્ણાંક
resize_func(પૂર્ણાંક check_only)
{
    જો (check_only)
	પાછા do_resize;
    જ્યારે (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(ખાલી, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    પાછા FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    પૂર્ણાંક
mch_inchar(
    char_u	*buf,
    પૂર્ણાંક		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    પૂર્ણાંક		tb_change_cnt)
{
    પાછા inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    સ્થિર શૂન્ય
handle_resize(શૂન્ય)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    પૂર્ણાંક
mch_char_avail(શૂન્ય)
{
    પાછા WaitForChar(0L, ખાલી, FALSE);
}

#જો defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    પૂર્ણાંક
mch_check_messages(શૂન્ય)
{
    પાછા WaitForChar(0L, ખાલી, TRUE);
}
#endif

#જો defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# જો defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# જો defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(પૂર્ણાંક special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	સંરચના vm_statistics64	vm_stat;
	natural_t		ગણતરી = HOST_VM_INFO64_ગણતરી;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ગણતરી);
#  નહીંતર
	સંરચના vm_statistics	vm_stat;
	natural_t		ગણતરી = HOST_VM_INFO_ગણતરી;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ગણતરી);
#  endif
	જો (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ગણતરી + vm_stat.active_ગણતરી
					    + vm_stat.inactive_ગણતરી
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_ગણતરી
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    જો (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	પૂર્ણાંક		mib[2];
	size_t		len = કદ(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  નહીંતર
	// sysctl() may return 32 bit or 64 bit, accept both
	સંઘ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  નહીંતર
	mib[1] = HW_USERMEM;
#  endif
	જો (sysctl(mib, 2, &physmem, &len, ખાલી, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  નહીંતર
	    જો (len == કદ(physmem.u64))
		mem = (long_u)physmem.u64;
	    નહીંતર
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# જો defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    જો (mem == 0)
    {
	સંરચના sysinfo sinfo;

	// Linux way of getting amount of RAM available
	જો (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    જ્યારે (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  નહીંતર
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    જો (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	જો (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    જ્યારે (shiftright > 0 && (pagesize & 1) == 0)
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
# જો defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	સંરચના rlimit	rlp;

	જો (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (કદ(long_u) * 8 - 1))
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

    જો (mem > 0)
	પાછા mem >> shiftright;
    પાછા (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    શૂન્ય
mch_delay(long msec, પૂર્ણાંક flags)
{
    tmode_T	old_tmode;
    પૂર્ણાંક		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    જો (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	જો (call_settmode)
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
	કરો
	{
	    // if total is large enough, wait by portions in p_mzq
	    જો (total > p_mzq)
		msec = p_mzq;
	    નહીંતર
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    સંરચના timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (શૂન્ય)nanosleep(&ts, ખાલી);
	}
#નહીંતર
# ifdef HAVE_USLEEP
	જ્યારે (msec >= 1000)
	{
	    usleep((unsigned પૂર્ણાંક)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned પૂર્ણાંક)(msec * 1000));
# નહીંતર
#  ifndef HAVE_SELECT
	poll(ખાલી, 0, (પૂર્ણાંક)msec);
#  નહીંતર
	{
	    સંરચના timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ખાલી, ખાલી, ખાલી, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	જ્યારે (total > 0);
#endif

	જો (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    નહીંતર
	WaitForChar(msec, ખાલી, FALSE);
}

#જો defined(HAVE_ઢગલો_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ઢગલો_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
સ્થિર પૂર્ણાંક ઢગલો_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    સ્થિર શૂન્ય
check_ઢગલો_growth(અક્ષર *p)
{
    પૂર્ણાંક		i;

    ઢગલો_grows_downwards = (p > (અક્ષર *)&i);
}
#endif

#જો defined(HAVE_ઢગલો_LIMIT)
સ્થિર અક્ષર *ઢગલો_limit = ખાલી;

# જો defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    સ્થિર શૂન્ય
get_ઢગલો_limit(શૂન્ય)
{
    સંરચના rlimit	rlp;
    પૂર્ણાંક			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    જો (getrlimit(RLIMIT_ઢગલો, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (કદ(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# જો defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    જો (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		જો (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	જો (ઢગલો_grows_downwards)
	{
	    ઢગલો_limit = (અક્ષર *)((long)&i - (lim / 16L * 15L));
	    જો (ઢગલો_limit >= (અક્ષર *)&i)
		// overflow, set to 1/16 of current stack position
		ઢગલો_limit = (અક્ષર *)((long)&i / 16L);
	}
	નહીંતર
	{
	    ઢગલો_limit = (અક્ષર *)((long)&i + (lim / 16L * 15L));
	    જો (ઢગલો_limit <= (અક્ષર *)&i)
		ઢગલો_limit = ખાલી;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    પૂર્ણાંક
mch_stackcheck(અક્ષર *p)
{
    જો (ઢગલો_limit == ખાલી)
	પાછા OK;

    જો (ઢગલો_grows_downwards)
    {
	જો (p < ઢગલો_limit)
	    પાછા FAIL;
    }
    નહીંતર જો (p > ઢગલો_limit)
	પાછા FAIL;
    પાછા OK;
}
#endif

#જો defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
સ્થિર stack_t sigstk;			// for sigaltstack()
# નહીંતર
સ્થિર સંરચના sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
સ્થિર long પૂર્ણાંક get_signal_ઢગલો_size(શૂન્ય)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long પૂર્ણાંક size = -1;

    // return size only if sysconf doesn't return an error
    જો ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	પાછા size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    પાછા SIGSTKSZ;
# endif

    // otherwise guess the size
    પાછા 8000;
}

સ્થિર અક્ષર *signal_ઢગલો;

    સ્થિર શૂન્ય
init_signal_ઢગલો(શૂન્ય)
{
    જો (signal_ઢગલો == ખાલી)
	પાછા;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_ઢગલો;
#  નહીંતર
    sigstk.ss_sp = signal_ઢગલો;
#  endif
    sigstk.ss_size = get_signal_ઢગલો_size();
    sigstk.ss_flags = 0;
    (શૂન્ય)sigaltstack(&sigstk, ખાલી);
# નહીંતર
    sigstk.ss_sp = signal_ઢગલો;
    જો (ઢગલો_grows_downwards)
	sigstk.ss_sp += get_signal_ઢગલો_size() - 1;
    sigstk.ss_onstack = 0;
    (શૂન્ય)sigstack(&sigstk, ખાલી);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#જો defined(SIGWINCH)
    સ્થિર શૂન્ય
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#જો defined(SIGTSTP)
    સ્થિર શૂન્ય
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    જો (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ઉઠાવો(sigarg);
    }
    નહીંતર
	got_tstp = TRUE;

# જો !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#જો defined(SIGINT)
    સ્થિર શૂન્ય
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#જો defined(SIGUSR1)
    સ્થિર શૂન્ય
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#જો defined(SIGPWR)
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#જો defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
સ્થિર JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
સ્થિર અસ્થિર sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
સ્થિર અસ્થિર sig_atomic_t lc_active = FALSE;

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
    સ્થિર શૂન્ય
mch_startjmp(શૂન્ય)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    સ્થિર શૂન્ય
mch_endjmp(શૂન્ય)
{
    lc_active = FALSE;
}

    સ્થિર શૂન્ય
mch_didjmp(શૂન્ય)
{
# જો defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ઢગલો();
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
    સ્થિર શૂન્ય
deathtrap SIGDEFARG(sigarg)
{
    સ્થિર પૂર્ણાંક	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    પૂર્ણાંક		i;
#endif

#જો defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    જો (lc_active)
    {
# જો defined(SIGHASARG)
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
    જો (in_mch_delay && sigarg == SIGQUIT)
	પાછા;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    જો (entered == 0
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
	પાછા;
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

#ifdef HAVE_ઢગલો_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ઢગલો_limit();
#endif

#જો 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	જો (fp)
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
    માટે (i = 0; signal_info[i].sig != -1; i++)
	જો (sigarg == signal_info[i].sig)
	    તોડો;
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
    જો (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	જો (entered >= 4)
	    _exit(8);
	exit(7);
    }
    જો (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((અક્ષર *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#નહીંતર
    sprintf((અક્ષર *)IObuff, "Vim: Caught deadly signal\r\n");
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
    સ્થિર શૂન્ય
after_sigcont(શૂન્ય)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#જો defined(SIGCONT)
સ્થિર શૂન્ય sigcont_handler SIGPROTOARG;

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
સ્થિર અસ્થિર sig_atomic_t sigcont_received;
સ્થિર શૂન્ય sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    સ્થિર શૂન્ય
sigcont_handler SIGDEFARG(sigarg)
{
    જો (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    નહીંતર
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

#જો defined(FEAT_CLIPBOARD)
# જો defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
સ્થિર શૂન્ય *clip_star_save = ખાલી;
સ્થિર શૂન્ય *clip_plus_save = ખાલી;
# endif

# જો defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    સ્થિર શૂન્ય
loose_clipboard(શૂન્ય)
{
    જો (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	જો (clip_star.owned)
	    clip_lose_selection(&clip_star);
	જો (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	જો (x11_display != ખાલી)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# જો defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    સ્થિર શૂન્ય
save_clipboard(શૂન્ય)
{
    જો (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    જો (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    સ્થિર શૂન્ય
restore_clipboard(શૂન્ય)
{
    જો (clip_star_save != ખાલી)
    {
	જો (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	નહીંતર
	    free_register(clip_star_save);
	clip_star_save = ખાલી;
    }
    જો (clip_plus_save != ખાલી)
    {
	જો (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	નહીંતર
	    free_register(clip_plus_save);
	clip_plus_save = ખાલી;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    શૂન્ય
mch_suspend(શૂન્ય)
{
    જો (ignore_sigtstp)
	પાછા;

#જો defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# જો defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# જો defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# જો defined(SIGCONT)
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

	માટે (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#નહીંતર
    suspend_shell();
#endif
}

    શૂન્ય
mch_init(શૂન્ય)
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

    શૂન્ય
set_sigwinch_handler(શૂન્ય)
{
#જો defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    સ્થિર શૂન્ય
set_signals(શૂન્ય)
{
#જો defined(SIGWINCH)
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
#જો defined(SIGCONT)
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

#જો defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    જો (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#જો defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    સ્થિર શૂન્ય
catch_int_signal(શૂન્ય)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    શૂન્ય
reset_signals(શૂન્ય)
{
    catch_signals(SIG_DFL, SIG_DFL);
#જો defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    સ્થિર શૂન્ય
catch_signals(
    શૂન્ય (*func_deadly)(પૂર્ણાંક),
    શૂન્ય (*func_other)(પૂર્ણાંક))
{
    પૂર્ણાંક	    i;

    માટે (i = 0; signal_info[i].sig != -1; i++)
    {
	જો (signal_info[i].deadly)
	{
#જો defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    સંરચના sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# જો defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# નહીંતર
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ખાલી);
#નહીંતર
# જો defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    સંરચના sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ખાલી);
# નહીંતર
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	નહીંતર જો (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#નહીંતર
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    સ્થિર શૂન્ય
block_signals(sigset_t *set)
{
    sigset_t	newset;
    પૂર્ણાંક		i;

    sigemptyset(&newset);

    માટે (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# જો defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    સ્થિર શૂન્ય
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, ખાલી);
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
    પૂર્ણાંક
vim_handle_signal(પૂર્ણાંક sig)
{
    સ્થિર પૂર્ણાંક got_signal = 0;
    સ્થિર પૂર્ણાંક blocked = TRUE;

    પસંદગી (sig)
    {
	સ્થિતિ SIGNAL_BLOCK:   blocked = TRUE;
			     તોડો;

	સ્થિતિ SIGNAL_UNBLOCK: blocked = FALSE;
			     જો (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     તોડો;

	મૂળભૂત:	     જો (!blocked)
				 પાછા TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     જો (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     તોડો;
    }
    પાછા FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    પૂર્ણાંક
mch_check_win(પૂર્ણાંક argc UNUSED, અક્ષર **argv UNUSED)
{
    જો (isatty(1))
	પાછા OK;
    પાછા FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    પૂર્ણાંક
mch_આગત_isatty(શૂન્ય)
{
    જો (isatty(read_cmd_fd))
	પાછા TRUE;
    પાછા FALSE;
}

#ifdef FEAT_X11

# જો defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    સ્થિર શૂન્ય
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#જો defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

સ્થિર પૂર્ણાંક	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    સ્થિર પૂર્ણાંક
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (અક્ષર *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    જો (!gui.in_use)
# endif
	સંદેશ((અક્ષર *)IObuff);

    પાછા 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    સ્થિર પૂર્ણાંક
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    પાછા 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    સ્થિર પૂર્ણાંક
x_connect_to_server(શૂન્ય)
{
    // No point in connecting if we are exiting or dying.
    જો (exiting || v_dying)
	પાછા FALSE;

# જો defined(FEAT_CLIENTSERVER)
    જો (x_force_connect)
	પાછા TRUE;
# endif
    જો (x_no_connect)
	પાછા FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    જો (clip_exclude_prog != ખાલી)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	જો (regprog_in_use(clip_exclude_prog))
	    પાછા FALSE;

	જો (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    પાછા FALSE;
    }
    પાછા TRUE;
}

# જો defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  જો defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    સ્થિર પૂર્ણાંક
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   જો defined(VMS) || defined(__CYGWIN__)
    પાછા 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
સ્થિર પૂર્ણાંક xterm_dpy_retry_ગણતરી = 0;

    સ્થિર પૂર્ણાંક
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = ખાલી;
    xterm_dpy_retry_ગણતરી = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = ખાલી;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  જો defined(VMS) || defined(__CYGWIN__)
    પાછા 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    સ્થિર શૂન્ય
may_restore_x11_clipboard(શૂન્ય)
{
    // No point in restoring the connecting if we are exiting or dying.
    જો (!exiting && !v_dying && xterm_dpy_retry_ગણતરી > 0)
    {
	--xterm_dpy_retry_ગણતરી;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	જો (app_context != (XtAppContext)ખાલી)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ખાલી;
	    x11_display = ખાલી; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    શૂન્ય
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    જો (eap->arg != ખાલી && (arglen = STRLEN(eap->arg)) > 0)
    {
	જો (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (અક્ષર *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ખાલી
		    ? (અક્ષર *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_ગણતરી = 5;  // Try reconnecting five times
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
    સ્થિર પૂર્ણાંક
test_x11_window(Display *dpy)
{
    પૂર્ણાંક			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    જો (XGetWMName(dpy, x11_window, &text_prop))
	XFree((શૂન્ય *)text_prop.કિંમત);
    XSync(dpy, False);
    (શૂન્ય)XSetErrorHandler(old_handler);

    જો (p_verbose > 0 && got_x_error)
	verb_સંદેશ(_("Testing the X display failed"));

    પાછા (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

સ્થિર પૂર્ણાંક get_x11_thing(પૂર્ણાંક get_title, પૂર્ણાંક test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    સ્થિર પૂર્ણાંક
get_x11_windis(શૂન્ય)
{
    અક્ષર	    *winid;
    સ્થિર પૂર્ણાંક	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    સ્થિર પૂર્ણાંક	    x11_display_from = XD_NONE;
    સ્થિર પૂર્ણાંક	    did_set_error_handler = FALSE;

    જો (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(શૂન્ય)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# જો defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    જો (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	જો (x11_display_from == XD_HERE && x11_display != ખાલી)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	જો (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    પાછા OK;
	}
	x11_display = ખાલી;
	પાછા FAIL;
    }
    નહીંતર જો (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = ખાલી;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    જો (!x_connect_to_server())
	પાછા FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    જો (x11_window == 0 && (winid = getenv("WINDOWID")) != ખાલી)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    જો (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    જો (xterm_dpy != ખાલી && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	જો (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    જો (x11_display_from == XD_HERE && x11_display != ખાલી)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	જો (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = ખાલી;
	    x11_display_from = XD_NONE;
	    પાછા FAIL;
	}
	પાછા OK;
    }
# endif

    જો (x11_window == 0 || x11_display == ખાલી)
	result = -1;

    જો (result != -1)	    // Have already been here and set this
	પાછા result;	    // Don't do all these X calls again

    જો (x11_window != 0 && x11_display == ખાલી)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	જો (p_verbose > 0)
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
	x11_display = XOpenDisplay(ખાલી);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	જો (p_verbose > 0 && sig_alarm_called)
	    verb_સંદેશ(_("Opening the X display timed out"));
# endif
	જો (x11_display != ખાલી)
	{
# ifdef ELAPSED_FUNC
	    જો (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    જો (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = ખાલી;
	    }
	    નહીંતર
		x11_display_from = XD_HERE;
	}
    }
    જો (x11_window == 0 || x11_display == ખાલી)
	પાછા (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    પાછા (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    સ્થિર પૂર્ણાંક
get_x11_title(પૂર્ણાંક test_only)
{
    પાછા get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    સ્થિર પૂર્ણાંક
get_x11_icon(પૂર્ણાંક test_only)
{
    પૂર્ણાંક		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    જો (oldicon == ખાલી && !test_only)
    {
	જો (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	નહીંતર
	    oldicon = vim_strsave(T_NAME);
    }

    પાછા retval;
}

    સ્થિર પૂર્ણાંક
get_x11_thing(
    પૂર્ણાંક		get_title,	// get title string
    પૂર્ણાંક		test_only)
{
    XTextProperty	text_prop;
    પૂર્ણાંક			retval = FALSE;
    Status		status;

    જો (get_x11_windis() != OK)
	પાછા FALSE;

    // Get window/icon name if any
    જો (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    નહીંતર
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
    Window	    મૂળ;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    unsigned પૂર્ણાંક    અંક_children;

    જ્યારે (!status || text_prop.કિંમત == ખાલી)
    {
	જો (!XQuery_વૃક્ષ(x11_display, win, &મૂળ, &parent, &children,
		    &અંક_children))
	    તોડો;
	જો (children)
	    XFree((શૂન્ય *)children);
	જો (parent == મૂળ || parent == 0)
	    તોડો;

	win = parent;
	જો (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	નહીંતર
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    જો (status && text_prop.કિંમત != ખાલી)
    {
	retval = TRUE;
	જો (!test_only)
	{
	    જો (get_title)
		vim_free(oldtitle);
	    નહીંતર
		vim_free(oldicon);
	    જો (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		જો (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.કિંમત);
		નહીંતર
		    oldicon = vim_strsave((char_u *)text_prop.કિંમત);
	    }
	    નહીંતર
	    {
		અક્ષર    **cl;
		Status  transform_status;
		પૂર્ણાંક	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		જો (transform_status >= Success && n > 0 && cl[0])
		{
		    જો (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    નહીંતર
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		નહીંતર
		{
		    જો (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.કિંમત);
		    નહીંતર
			oldicon = vim_strsave((char_u *)text_prop.કિંમત);
		}
	    }
	}
	XFree((શૂન્ય *)text_prop.કિંમત);
    }
    પાછા retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# જો defined(X_HAVE_UTF8_STRING)
#  જો X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    સ્થિર શૂન્ય
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    જો (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (અચલ અક્ષર *)title,
					     ખાલી, ખાલી, 0, ખાલી, ખાલી, ખાલી);
    નહીંતર
# endif
    {
# જો XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (અચલ અક્ષર *)title,
					     ખાલી, ખાલી, 0, ખાલી, ખાલી, ખાલી);
#  નહીંતર
	XTextProperty	text_prop;
	અક્ષર		*c_title = (અક્ષર *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(શૂન્ય)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     ખાલી, ખાલી, 0, ખાલી, ખાલી, ખાલી);
#  endif
# નહીંતર
	XStoreName(x11_display, x11_window, (અક્ષર *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    સ્થિર શૂન્ય
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    જો (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, ખાલી, (અચલ અક્ષર *)icon,
						   ખાલી, 0, ખાલી, ખાલી, ખાલી);
    નહીંતર
# endif
    {
# જો XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, ખાલી, (અચલ અક્ષર *)icon,
						   ખાલી, 0, ખાલી, ખાલી, ખાલી);
#  નહીંતર
	XTextProperty	text_prop;
	અક્ષર		*c_icon = (અક્ષર *)icon;

	(શૂન્ય)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, ખાલી, &text_prop,
						   ખાલી, 0, ખાલી, ખાલી, ખાલી);
#  endif
# નહીંતર
	XSetIconName(x11_display, x11_window, (અક્ષર *)icon);
# endif
    }
    XFlush(x11_display);
}

#નહીંતર  // FEAT_X11

    સ્થિર પૂર્ણાંક
get_x11_title(પૂર્ણાંક test_only UNUSED)
{
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
get_x11_icon(પૂર્ણાંક test_only)
{
    જો (!test_only)
    {
	જો (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	નહીંતર
	    oldicon = vim_strsave(T_NAME);
    }
    પાછા FALSE;
}

#endif // FEAT_X11

    પૂર્ણાંક
mch_can_restore_title(શૂન્ય)
{
    પાછા get_x11_title(TRUE);
}

    પૂર્ણાંક
mch_can_restore_icon(શૂન્ય)
{
    પાછા get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    શૂન્ય
mch_settitle(char_u *title, char_u *icon)
{
    પૂર્ણાંક		type = 0;
    સ્થિર પૂર્ણાંક	recursive = 0;

    જો (T_NAME == ખાલી)	    // no terminal name (yet)
	પાછા;
    જો (title == ખાલી && icon == ખાલી)	    // nothing to do
	પાછા;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    જો (recursive)
	પાછા;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    જો (get_x11_windis() == OK)
	type = 1;
#endif
#જો defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    જો (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    જો ((type || *T_TS != NUL) && title != ખાલી)
    {
	જો (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	જો (oldtitle == ખાલી
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (શૂન્ય)get_x11_title(FALSE);

	જો (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	નહીંતર
# ifdef FEAT_GUI_GTK
	જો (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#જો defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	નહીંતર
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    જો ((type || *T_CIS != NUL) && icon != ખાલી)
    {
	જો (oldicon == ખાલી
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	જો (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	નહીંતર
# ifdef FEAT_GUI_GTK
	જો (!gui.in_use)		// don't do this if GTK+ is running
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
    શૂન્ય
mch_restore_title(પૂર્ણાંક which)
{
    પૂર્ણાંક	do_ધકેલો_કાઢો = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : ખાલી;
    char_u *tofree = ખાલી;
    જો (title == oldtitle && oldtitle != ખાલી)
    {
	tofree = vim_strsave(title);
	જો (tofree != ખાલી)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : ખાલી);
    vim_free(tofree);

    જો (do_ધકેલો_કાઢો)
    {
	// pop and push from/to the stack
	term_કાઢો_title(which);
	term_ધકેલો_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    પૂર્ણાંક
vim_is_xterm(char_u *name)
{
    જો (name == ખાલી)
	પાછા FALSE;
    પાછા ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#જો defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    પૂર્ણાંક
use_xterm_like_mouse(char_u *name)
{
    પાછા (name != ખાલી
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
    પૂર્ણાંક
use_xterm_mouse(શૂન્ય)
{
    જો (ttym_flags == TTYM_SGR)
	પાછા 4;
    જો (ttym_flags == TTYM_URXVT)
	પાછા 3;
    જો (ttym_flags == TTYM_XTERM2)
	પાછા 2;
    જો (ttym_flags == TTYM_XTERM)
	પાછા 1;
    પાછા 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    પૂર્ણાંક
vim_is_iris(char_u *name)
{
    જો (name == ખાલી)
	પાછા FALSE;
    પાછા (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#જો defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    પૂર્ણાંક
vim_is_vt300(char_u *name)
{
    જો (name == ખાલી)
	પાછા FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    પાછા ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ખાલી)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    પૂર્ણાંક
mch_get_user_name(char_u *s, પૂર્ણાંક len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(ખાલી), len - 1);
    પાછા OK;
#નહીંતર
    પાછા mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    પૂર્ણાંક
mch_get_uname(uid_t uid, char_u *s, પૂર્ણાંક len)
{
#જો defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    સંરચના passwd   *pw;

    જો ((pw = getpwuid(uid)) != ખાલી
	    && pw->pw_name != ખાલી && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	પાછા OK;
    }
#endif
    sprintf((અક્ષર *)s, "%d", (પૂર્ણાંક)uid);	    // assumes s is long enough
    પાછા FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    શૂન્ય
mch_get_host_name(char_u *s, પૂર્ણાંક len)
{
    સંરચના utsname vutsname;

    જો (uname(&vutsname) < 0)
	*s = NUL;
    નહીંતર
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#નહીંતર // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    શૂન્ય
mch_get_host_name(char_u *s, પૂર્ણાંક len)
{
# ifdef VAXC
    vaxc$gethostname((અક્ષર *)s, len);
# નહીંતર
    gethostname((અક્ષર *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(શૂન્ય)
{
    પાછા (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    પૂર્ણાંક
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    જો (kill(pid, 0) == 0)
	પાછા TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    જો (errno == ESRCH)
	પાછા FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    પાછા TRUE;
}

#જો !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    સ્થિર અક્ષર *
strerror(પૂર્ણાંક err)
{
    બાહ્ય પૂર્ણાંક	    sys_nerr;
    બાહ્ય અક્ષર	    *sys_errlist[];
    સ્થિર અક્ષર	    er[20];

    જો (err > 0 && err < sys_nerr)
	પાછા (sys_errlist[err]);
    snprintf(er, કદ(er), "Error %d", err);
    પાછા er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    પૂર્ણાંક
mch_dirname(char_u *buf, પૂર્ણાંક len)
{
#જો defined(USE_GETCWD)
    જો (getcwd((અક્ષર *)buf, len) == ખાલી)
    {
	STRCPY(buf, strerror(errno));
	પાછા FAIL;
    }
    પાછા OK;
#નહીંતર
    પાછા (getwd((અક્ષર *)buf) != ખાલી ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    પૂર્ણાંક
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    પૂર્ણાંક		len,
    પૂર્ણાંક		force)		// also expand when already absolute path
{
    પૂર્ણાંક		buflen = 0;
#ifdef HAVE_FCHDIR
    પૂર્ણાંક		fd = -1;
    સ્થિર પૂર્ણાંક	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    પૂર્ણાંક		retval = OK;
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
# જો CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# નહીંતર
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    જો ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == ખાલી || p != fname))
    {
	જો (p == ખાલી && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	જો (p != ખાલી)
	{
	    જો (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    જો (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		જો (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    જો (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((અક્ષર *)olddir) != 0))
	    {
		p = ખાલી;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    નહીંતર
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		જો (p - fname >= len)
		    retval = FAIL;
		નહીંતર
		{
		    vim_strncpy(buf, fname, p - fname);
		    જો (mch_chdir((અક્ષર *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			જો (mch_isFullName(fname))
			    retval = FAIL;
			નહીંતર
			    p = ખાલી;
		    }
		    નહીંતર જો (*p == '/')
			fname = p + 1;
		    નહીંતર
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	જો (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	જો (p != ખાલી)
	{
	    પૂર્ણાંક	l;

#ifdef HAVE_FCHDIR
	    જો (fd >= 0)
	    {
		જો (p_verbose >= 5)
		{
		    verbose_enter();
		    સંદેશ("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    નહીંતર
#endif
		l = mch_chdir((અક્ષર *)olddir);
	    જો (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	જો (fd >= 0)
	    close(fd);
#endif

	buflen = (પૂર્ણાંક)STRLEN(buf);
	જો (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	નહીંતર જો (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += કદ(PATHSEP);
	}
#endif
    }

    જો (buflen == 0)
	buflen = (પૂર્ણાંક)STRLEN(buf);

    // Catch file names which are too long.
    જો (retval == FAIL || (પૂર્ણાંક)(buflen + STRLEN(fname)) >= len)
	પાછા FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    જો (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    પાછા OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    પૂર્ણાંક
mch_isFullName(char_u *fname)
{
#ifdef VMS
    પાછા ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((અક્ષર *)fname,':') || strchr((અક્ષર *)fname,'"') ||
	    (strchr((અક્ષર *)fname,'[') && strchr((અક્ષર *)fname,']'))||
	    (strchr((અક્ષર *)fname,'<') && strchr((અક્ષર *)fname,'>'))   );
#નહીંતર
    પાછા (*fname == '/' || *fname == '~');
#endif
}

#જો defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    શૂન્ય
fname_case(
    char_u	*name,
    પૂર્ણાંક		len UNUSED)  // buffer size, only used when name gets longer
{
    સંરચના stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    સંરચના dirent *dp;

    જો (mch_lstat((અક્ષર *)name, &st) < 0)
	પાછા;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    જો (slash == ખાલી)
    {
	dirp = opendir(".");
	tail = name;
    }
    નહીંતર
    {
	*slash = NUL;
	dirp = opendir((અક્ષર *)name);
	*slash = '/';
	tail = slash + 1;
    }

    જો (dirp == ખાલી)
	પાછા;

    taillen = STRLEN(tail);
    જ્યારે ((dp = readdir(dirp)) != ખાલી)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	જો (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    સંરચના stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    જો (mch_lstat((અક્ષર *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		તોડો;
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
    સંરચના stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    જો (stat((અક્ષર *)vms_fixfilename(name), &statb))
#નહીંતર
    જો (stat((અક્ષર *)name, &statb))
#endif
	પાછા -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    પાછા statb.st_mode & ~S_ADDACE;
#નહીંતર
    પાછા statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
mch_setperm(char_u *name, long perm)
{
    પાછા (chmod((અક્ષર *)
#ifdef VMS
		    vms_fixfilename(name),
#નહીંતર
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#જો defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    પૂર્ણાંક
mch_fsetperm(પૂર્ણાંક fd, long perm)
{
    પાછા (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#જો defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
પ્રકારવ્યાખ્યા સંરચના vim_acl_solaris_T {
    પૂર્ણાંક acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# જો defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    શૂન્ય
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    જો (from_file == ખાલી)
	પાછા;

    જો (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    જો (selinux_enabled <= 0)
	પાછા;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    અક્ષર *from_context = ખાલી;
    અક્ષર *to_context = ખાલી;

    જો (getfilecon((અક્ષર *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	જો (errno == EOPNOTSUPP)
	    પાછા;

	સંદેશ_puts(_("\nCould not get security context for "));
	સંદેશ_outtrans(from_file);
	સંદેશ_putchar('\n');
	પાછા;
    }
    જો (getfilecon((અક્ષર *)to_file, &to_context) < 0)
    {
	સંદેશ_puts(_("\nCould not get security context for "));
	સંદેશ_outtrans(to_file);
	સંદેશ_putchar('\n');
	freecon (from_context);
	પાછા ;
    }
    જો (strcmp(from_context, to_context) != 0)
    {
	જો (setfilecon((અક્ષર *)to_file, from_context) < 0)
	{
	    સંદેશ_puts(_("\nCould not set security context for "));
	    સંદેશ_outtrans(to_file);
	    સંદેશ_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# જો defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    શૂન્ય
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    સ્થિર અચલ અક્ષર * અચલ smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    અક્ષર	buffer[SMACK_LABEL_LEN];
    અચલ અક્ષર	*name;
    પૂર્ણાંક		index;
    પૂર્ણાંક		ret;
    ssize_t	size;

    જો (from_file == ખાલી)
	પાછા;

    size = listxattr((અક્ષર *)from_file, ખાલી, 0);
    // not supported or no attributes to copy
    જો (size <= 0)
	પાછા;

    માટે (index = 0 ; index < (પૂર્ણાંક)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((અક્ષર*)from_file, name, buffer, કદ(buffer));
	જો (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((અક્ષર*)to_file, name, buffer, (size_t)size, 0);
	    જો (ret < 0)
	    {
		vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		સંદેશ_outtrans(IObuff);
		સંદેશ_putchar('\n');
	    }
	}
	નહીંતર
	{
	    // what reason of not having the attribute value?
	    પસંદગી (errno)
	    {
		સ્થિતિ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    પાછા; // leave because it isn't useful to continue

		સ્થિતિ ERANGE:
		મૂળભૂત:
		    // no enough size OR unexpected error
		     vim_snprintf((અક્ષર *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    સંદેશ_puts((અક્ષર *)IObuff);
		    સંદેશ_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		સ્થિતિ ENODATA:
		    // no attribute of this name
		    ret = removexattr((અક્ષર*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    તોડો;
	    }
	}
    }
}
# endif // HAVE_SMACK

# જો defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    શૂન્ય
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    અક્ષર	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    અક્ષર	*ચાવી;
    અક્ષર	*val = ખાલી;
    અક્ષર	*errmsg = ખાલી;

    જો (from_file == ખાલી)
	પાછા;

    // get the length of the extended attributes
    size = listxattr((અક્ષર *)from_file, ખાલી, 0);
    // not supported or no attributes to copy
    જો (size <= 0)
	પાછા;
    xattr_buf = (અક્ષર*)alloc(size);
    જો (xattr_buf == ખાલી)
	પાછા;
    size = listxattr((અક્ષર *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    માટે (પૂર્ણાંક round = 0; round < 2; round++)
    {

	ચાવી = xattr_buf;
	જો (round == 1)
	    size = tsize;

	જ્યારે (size > 0)
	{
	    vallen = getxattr((અક્ષર *)from_file, ચાવી,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    જો (vallen >= 0 && round &&
		setxattr((અક્ષર *)to_file, ચાવી, val, vallen, 0) == 0)
		;
	    નહીંતર જો (errno)
	    {
		પસંદગી (errno)
		{
		    સ્થિતિ E2BIG:
			errmsg = e_xattr_e2big;
			જાઓ error_exit;
		    સ્થિતિ ENOTSUP:
		    સ્થિતિ EACCES:
		    સ્થિતિ EPERM:
			તોડો;
		    સ્થિતિ ERANGE:
			errmsg = e_xattr_erange;
			જાઓ error_exit;
		    મૂળભૂત:
			errmsg = e_xattr_other;
			જાઓ error_exit;
		}
	    }

	    જો (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ચાવી) + 1;
	    size -= keylen;
	    ચાવી += keylen;
	}
	જો (round)
	    તોડો;

	val = (અક્ષર*)alloc(max_vallen + 1);
	જો (val == ખાલી)
	    જાઓ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    જો (errmsg != ખાલી)
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
    vim_acl_T	ret = ખાલી;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((અક્ષર *)fname, ACL_TYPE_ACCESS);
# નહીંતર
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    જો (acl_get((અક્ષર *)fname, 0, &aclent) < 0)
	પાછા ખાલી;
    ret = (vim_acl_T)aclent;
#  નહીંતર
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(કદ(vim_acl_solaris_T));
    જો (aclent == ખાલી)
	પાછા ખાલી;
    જો ((aclent->acl_cnt = acl((અક્ષર *)fname, GETACLCNT, 0, ખાલી)) < 0)
    {
	મુક્ત(aclent);
	પાછા ખાલી;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * કદ(aclent_t));
    જો (aclent->acl_entry == ખાલી)
    {
	મુક્ત(aclent);
	પાછા ખાલી;
    }
    જો (acl((અક્ષર *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	મુક્ત(aclent->acl_entry);
	મુક્ત(aclent);
	પાછા ખાલી;
    }
    ret = (vim_acl_T)aclent;
#   નહીંતર
#    જો defined(HAVE_AIX_ACL)
    પૂર્ણાંક		aclsize;
    સંરચના acl *aclent;

    aclsize = કદ(સંરચના acl);
    aclent = malloc(aclsize);
    જો (aclent == ખાલી)
	પાછા ખાલી;
    જો (statacl((અક્ષર *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	જો (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    જો (aclent == ખાલી || statacl((અક્ષર *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		મુક્ત(aclent);
		પાછા ખાલી;
	    }
	}
	નહીંતર
	{
	    મુક્ત(aclent);
	    પાછા ખાલી;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    પાછા ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    શૂન્ય
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    જો (aclent == ખાલી)
	પાછા;
# ifdef HAVE_POSIX_ACL
    acl_set_file((અક્ષર *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# નહીંતર
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((અક્ષર *)fname, (acl_t *)aclent);
#  નહીંતર
#   ifdef HAVE_SOLARIS_ACL
    acl((અક્ષર *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   નહીંતર
#    ifdef HAVE_AIX_ACL
    chacl((અક્ષર *)fname, aclent, ((સંરચના acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    શૂન્ય
mch_free_acl(vim_acl_T aclent)
{
    જો (aclent == ખાલી)
	પાછા;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# નહીંતર
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  નહીંતર
#   ifdef HAVE_SOLARIS_ACL
    મુક્ત(((vim_acl_solaris_T *)aclent)->acl_entry);
    મુક્ત(aclent);
#   નહીંતર
#    ifdef HAVE_AIX_ACL
    મુક્ત(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    શૂન્ય
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    પૂર્ણાંક
mch_isdir(char_u *name)
{
    સંરચના stat statb;

    જો (*name == NUL)	    // Some stat()s don't flag "" as an error.
	પાછા FALSE;
    જો (stat((અક્ષર *)name, &statb))
	પાછા FALSE;
    પાછા (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    પૂર્ણાંક
mch_isrealdir(char_u *name)
{
    સંરચના stat statb;

    જો (*name == NUL)	    // Some stat()s don't flag "" as an error.
	પાછા FALSE;
    જો (mch_lstat((અક્ષર *)name, &statb))
	પાછા FALSE;
    પાછા (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    સ્થિર પૂર્ણાંક
executable_file(char_u *name)
{
    સંરચના stat	st;

    જો (stat((અક્ષર *)name, &st))
	પાછા 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    પૂર્ણાંક vms_executable = 0;
    જો (S_ISREG(st.st_mode) && mch_access((અક્ષર *)name, X_OK) == 0)
    {
	જો (strstr(vms_tolower((અક્ષર*)name),".exe") != ખાલી
		|| strstr(vms_tolower((અક્ષર*)name),".com")!= ખાલી)
	    vms_executable = 1;
    }
    પાછા vms_executable;
#નહીંતર
    પાછા S_ISREG(st.st_mode) && mch_access((અક્ષર *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    પૂર્ણાંક
mch_can_exe(char_u *name, char_u **path, પૂર્ણાંક use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    પૂર્ણાંક		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    જો (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	જો ((use_path || gettail(name) != name) && executable_file(name))
	{
	    જો (path != ખાલી)
	    {
		જો (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		નહીંતર
		    *path = vim_strsave(name);
	    }
	    પાછા TRUE;
	}
	પાછા FALSE;
    }

    p = (char_u *)getenv("PATH");
    જો (p == ખાલી || *p == NUL)
	પાછા -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    જો (buf == ખાલી)
	પાછા -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    માટે (;;)
    {
	e = (char_u *)strchr((અક્ષર *)p, ':');
	જો (e == ખાલી)
	    e = p_end;
	elen = (size_t)(e - p);
	જો (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((અક્ષર *)buf, bufsize, "%.*s%s%s",
		(પૂર્ણાંક)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	જો (retval == 1)
	{
	    જો (path != ખાલી)
	    {
		જો (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		નહીંતર
		    *path = vim_strnsave(buf, buflen);
	    }
	    તોડો;
	}

	જો (*e != ':')
	    તોડો;
	p = e + 1;
    }

    vim_free(buf);
    પાછા retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    પૂર્ણાંક
mch_nodetype(char_u *name)
{
    સંરચના stat	st;

    જો (stat((અક્ષર *)name, &st))
	પાછા ગાંઠ_NORMAL;
    જો (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	પાછા ગાંઠ_NORMAL;
    જો (S_ISBLK(st.st_mode))	// block device isn't writable
	પાછા ગાંઠ_OTHER;
    // Everything else is writable?
    પાછા ગાંઠ_WRITABLE;
}

    શૂન્ય
mch_early_init(શૂન્ય)
{
#ifdef HAVE_CHECK_ઢગલો_GROWTH
    પૂર્ણાંક			i;

    check_ઢગલો_growth((અક્ષર *)&i);

# ifdef HAVE_ઢગલો_LIMIT
    get_ઢગલો_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#જો defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ઢગલો = alloc(get_signal_ઢગલો_size());
    init_signal_ઢગલો();
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

#જો defined(EXITFREE)
    શૂન્ય
mch_free_mem(શૂન્ય)
{
# જો defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    જો (clip_star.owned)
	clip_lose_selection(&clip_star);
    જો (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# જો defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    જો (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    જો (xterm_dpy != ખાલી)
	XtCloseDisplay(xterm_dpy);
    જો (app_context != (XtAppContext)ખાલી)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ખાલી; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# જો defined(FEAT_X11)
    જો (x11_display != ખાલી
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# જો defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ઢગલો);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    સ્થિર શૂન્ય
exit_scroll(શૂન્ય)
{
    જો (silent_mode)
	પાછા;
    જો (newline_on_exit || સંદેશ_didout)
    {
	જો (સંદેશ_use_printf())
	{
	    જો (info_message)
		mch_સંદેશ("\n");
	    નહીંતર
		mch_errmsg("\r\n");
	}
	નહીંતર
	    out_char('\n');
    }
    નહીંતર જો (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	સંદેશ_clr_eos_force();		// clear the rest of the display
	windgoto((પૂર્ણાંક)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# જો (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
બાહ્ય શૂન્ય __gcov_dump(શૂન્ય);
બાહ્ય શૂન્ય __gcov_reset(શૂન્ય);
#  define __gcov_flush() કરો { __gcov_dump(); __gcov_reset(); } જ્યારે (0)
# નહીંતર
બાહ્ય શૂન્ય __gcov_flush(શૂન્ય);
# endif
#endif

    શૂન્ય
mch_exit(પૂર્ણાંક r)
{
    exiting = TRUE;

#જો defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    જો (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	જો (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_કાઢો_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	જો (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	જો (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	જો (full_screen)
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
    જો (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    જો (deadly_signal != 0)
	પાછા;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    સ્થિર શૂન્ય
may_core_dump(શૂન્ય)
{
    જો (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    સ્થિર પૂર્ણાંક
get_tty_fd(પૂર્ણાંક fd)
{
    પૂર્ણાંક		tty_fd = fd;

# જો defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    જો (mch_isatty(fd) > 1)
    {
	અક્ષર *name;

	name = ptsname(fd);
	જો (name == ખાલી)
	    પાછા -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	જો (tty_fd < 0)
	    પાછા -1;
    }
# endif
    પાછા tty_fd;
}

    સ્થિર પૂર્ણાંક
mch_tcgetattr(પૂર્ણાંક fd, શૂન્ય *term)
{
    પૂર્ણાંક		tty_fd;
    પૂર્ણાંક		retval = -1;

    tty_fd = get_tty_fd(fd);
    જો (tty_fd < 0)
	પાછા -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (સંરચના termios *)term);
#  નહીંતર
    retval = ioctl(tty_fd, TCGETA, (સંરચના termio *)term);
#  endif
# નહીંતર
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (સંરચના sgttyb *)term);
# endif
    જો (tty_fd != fd)
	close(tty_fd);
    પાછા retval;
}

    શૂન્ય
mch_settmode(tmode_T tmode)
{
    સ્થિર પૂર્ણાંક first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    સ્થિર સંરચના termios told;
	   સંરચના termios tnew;
#  નહીંતર
    સ્થિર સંરચના termio told;
	   સંરચના termio tnew;
#  endif

    જો (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    જો (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ખાલી || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  જો defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   નહીંતર
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    નહીંતર
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    નહીંતર જો (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  જો defined(HAVE_TERMIOS_H)
    {
	પૂર્ણાંક	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	જ્યારે (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  નહીંતર
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# નહીંતર
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    સ્થિર સંરચના sgttyb ttybold;
	   સંરચના sgttyb ttybnew;

    જો (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    જો (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    નહીંતર જો (tmode == TMODE_SLEEP)
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
    શૂન્ય
get_stty(શૂન્ય)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    જો (get_tty_info(read_cmd_fd, &info) != OK)
	પાછા;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    જો (p != ખાલી && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ખાલી);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    પૂર્ણાંક
get_tty_info(પૂર્ણાંક fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    સંરચના termios ચાવીઓ;
#  નહીંતર
    સંરચના termio ચાવીઓ;
#  endif

    જો (mch_tcgetattr(fd, &ચાવીઓ) != -1)
    {
	info->backspace = ચાવીઓ.c_cc[VERASE];
	info->interrupt = ચાવીઓ.c_cc[VINTR];
	જો (ચાવીઓ.c_iflag & ICRNL)
	    info->enter = NL;
	નહીંતર
	    info->enter = CAR;
	જો (ચાવીઓ.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	નહીંતર
	    info->nl_does_cr = FALSE;
	પાછા OK;
    }
# નહીંતર
    // for "old" tty systems
    સંરચના sgttyb ચાવીઓ;

    જો (mch_tcgetattr(fd, &ચાવીઓ) != -1)
    {
	info->backspace = ચાવીઓ.sg_erase;
	info->interrupt = ચાવીઓ.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	પાછા OK;
    }
# endif
    પાછા FAIL;
}

#endif // VMS

સ્થિર પૂર્ણાંક	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    શૂન્ય
mch_setmouse(પૂર્ણાંક on)
{
#ifdef FEAT_BEVAL_TERM
    સ્થિર પૂર્ણાંક	bevalterm_ison = FALSE;
#endif
    પૂર્ણાંક		xterm_mouse_vers;

#જો defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    જો (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    જો (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	પાછા;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    જો (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    જો (T_CXM != ખાલી && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    નહીંતર જો (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    જો (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	જો (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    જો (xterm_mouse_vers > 0)
    {
	જો (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	નહીંતર	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    નહીંતર જો (ttym_flags == TTYM_DEC)
    {
	જો (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	નહીંતર	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    નહીંતર
    {
	જો (on)
	{
	    જો (gpm_open())
		mouse_ison = TRUE;
	}
	નહીંતર
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    નહીંતર
    {
	જો (on)
	{
	    જો (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	નહીંતર
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    નહીંતર
    {
	જો (on)
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
# નહીંતર
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	નહીંતર
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    નહીંતર
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	જો (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	નહીંતર
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#જો defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    શૂન્ય
mch_bevalterm_changed(શૂન્ય)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    શૂન્ય
check_mouse_termcode(શૂન્ય)
{
#ifdef FEAT_MOUSE_XTERM
    જો (use_xterm_mouse()
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
	જો (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    નહીંતર
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    નહીંતર
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    નહીંતર
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    નહીંતર
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    નહીંતર
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    જો (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    નહીંતર
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    જો (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	જો (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    નહીંતર
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    જો (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	જો (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    નહીંતર
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
    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
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
	સંરચના winsize	ws;
	પૂર્ણાંક fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	જો (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	જો (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(ખાલી, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# નહીંતર // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	સંરચના ttysize	ts;
	પૂર્ણાંક fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	જો (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	જો (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_લીટીઓ;
#   ifdef FEAT_EVAL
	    ch_log(ખાલી, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    જો (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ખાલી)
    {
	જો ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((અક્ષર *)p);
# ifdef FEAT_EVAL
	    ch_log(ખાલી, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	જો ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((અક્ષર *)p);
# ifdef FEAT_EVAL
	    ch_log(ખાલી, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    જો (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(ખાલી, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    જો (columns <= 0 || rows <= 0)
	પાછા FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    પાછા OK;
}

# જો defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    શૂન્ય
mch_calc_cell_size(સંરચના cellsize *cs_out)
{
   સંરચના winsize   ws;
   પૂર્ણાંક		    retval;
   પૂર્ણાંક		    x_cell_size;
   પૂર્ણાંક		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ખાલી, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   જો (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	પાછા;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   જો (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# જો defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    પૂર્ણાંક
mch_report_winsize(પૂર્ણાંક fd, પૂર્ણાંક rows, પૂર્ણાંક cols)
{
    પૂર્ણાંક		tty_fd;
    પૂર્ણાંક		retval = -1;

    tty_fd = get_tty_fd(fd);
    જો (tty_fd < 0)
	પાછા FAIL;

#  જો defined(TIOCSWINSZ)
    સંરચના winsize ws;

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
    ch_log(ખાલી, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    સંરચના ttysize ts;

    ts.ts_cols = cols;
    ts.ts_લીટીઓ = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ખાલી, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    જો (tty_fd != fd)
	close(tty_fd);
    પાછા retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    શૂન્ય
mch_set_shellsize(શૂન્ય)
{
    જો (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((પૂર્ણાંક)Rows, (પૂર્ણાંક)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    શૂન્ય
mch_new_shellsize(શૂન્ય)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    સ્થિર pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    જ્યારે (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	જો (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    જો (++delay_msec > 10)
		delay_msec = 10;
	    ચાલુ;
	}
	જો (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    તોડો;
    }
    પાછા wait_pid;
}

#જો !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    સ્થિર શૂન્ય
set_child_environment(
	long	rows,
	long	columns,
	અક્ષર	*term,
	પૂર્ણાંક	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    અક્ષર	envbuf[50];
# નહીંતર
    સ્થિર અક્ષર	envbuf_Term[30];
    સ્થિર અક્ષર	envbuf_Rows[20];
    સ્થિર અક્ષર	envbuf_લીટીઓ[20];
    સ્થિર અક્ષર	envbuf_Columns[20];
    સ્થિર અક્ષર	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    સ્થિર અક્ષર	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    સ્થિર અક્ષર	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((અક્ષર *)envbuf, "%ld", rows);
    setenv("ROWS", (અક્ષર *)envbuf, 1);
    sprintf((અક્ષર *)envbuf, "%ld", rows);
    setenv("LINES", (અક્ષર *)envbuf, 1);
    sprintf((અક્ષર *)envbuf, "%ld", columns);
    setenv("COLUMNS", (અક્ષર *)envbuf, 1);
    sprintf((અક્ષર *)envbuf, "%d", t_colors);
    setenv("COLORS", (અક્ષર *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    જો (is_terminal)
    {
	sprintf((અક્ષર *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (અક્ષર *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ખાલી ? "" : (અક્ષર *)serverName, 1);
#  endif
# નહીંતર
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, કદ(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, કદ(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_લીટીઓ, કદ(envbuf_લીટીઓ), "LINES=%ld", rows);
    putenv(envbuf_લીટીઓ);
    vim_snprintf(envbuf_Columns, કદ(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, કદ(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    જો (is_terminal)
    {
	vim_snprintf(envbuf_Version, કદ(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, કદ(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ખાલી ? "" : (અક્ષર *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    સ્થિર શૂન્ય
set_default_child_environment(પૂર્ણાંક is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#જો defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    સ્થિર શૂન્ય
open_pty(પૂર્ણાંક *pty_master_fd, પૂર્ણાંક *pty_slave_fd, char_u **name1, char_u **name2)
{
    અક્ષર	*tty_name;

    જો (name1 != ખાલી)
	*name1 = ખાલી;
    જો (name2 != ખાલી)
	*name2 = ખાલી;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    જો (*pty_master_fd < 0)
	પાછા;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# નહીંતર
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    જો (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    નહીંતર
    {
	જો (name1 != ખાલી)
	    *name1 = vim_strsave((char_u *)tty_name);
	જો (name2 != ખાલી)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    સ્થિર શૂન્ય
may_send_sigint(પૂર્ણાંક c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    જો (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# નહીંતર
	kill(0, SIGINT);
# endif
	જો (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#જો !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    પૂર્ણાંક
unix_build_argv(
	char_u *cmd,
	અક્ષર ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    અક્ષર	**argv = ખાલી;
    પૂર્ણાંક		argc;

    *sh_tofree = vim_strsave(p_sh);
    જો (*sh_tofree == ખાલી)		// out of memory
	પાછા FAIL;

    જો (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	પાછા FAIL;
    *argvp = argv;

    જો (cmd != ખાલી)
    {
	char_u	*s;
	char_u	*p;

	જો (extra_shell_arg != ખાલી)
	    argv[argc++] = (અક્ષર *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	જો (*shcf_tofree == ખાલી)    // out of memory
	    પાછા FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	જ્યારે (*p != NUL)
	{
	    argv[argc++] = (અક્ષર *)s;
	    જ્યારે (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (અક્ષર *)cmd;
    }
    argv[argc] = ખાલી;
    પાછા OK;
}
#endif

#જો defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    સ્થિર પૂર્ણાંક
mch_call_shell_terminal(
    char_u	*cmd,
    પૂર્ણાંક		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    અક્ષર	**argv = ખાલી;
    char_u	*tofree1 = ખાલી;
    char_u	*tofree2 = ખાલી;
    પૂર્ણાંક		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    જો (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	જાઓ theend;

    init_job_options(&opt);
    ch_log(ખાલી, "starting terminal for system command '%s'", cmd);
    buf = term_start(ખાલી, argv, &opt, TERM_START_SYSTEM);
    જો (buf == ખાલી)
	જાઓ theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    જો (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	જ્યારે (term_use_loop())
	{
	    જો (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		જો (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    નહીંતર
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(ખાલી, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    જો (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    પાછા retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    સ્થિર પૂર્ણાંક
mch_call_shell_system(
    char_u	*cmd,
    પૂર્ણાંક		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    અક્ષર	*ifn = ખાલી;
    અક્ષર	*ofn = ખાલી;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    પૂર્ણાંક		x;

    out_flush();

    જો (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# જો defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  જો defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    જો (cmd == ખાલી)
	x = system((અક્ષર *)p_sh);
    નહીંતર
    {
# ifdef VMS
	જો (ofn = strchr((અક્ષર *)cmd, '>'))
	    *ofn++ = '\0';
	જો (ifn = strchr((અક્ષર *)cmd, '<'))
	{
	    અક્ષર *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    જો (p)
		*p = '\0';
	}
	જો (ofn)
	    x = vms_sys((અક્ષર *)cmd, ofn, ifn);
	નહીંતર
	    x = system((અક્ષર *)cmd);
# નહીંતર
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ખાલી ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	જો (newcmd == ખાલી)
	    x = 0;
	નહીંતર
	{
	    sprintf((અક્ષર *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ખાલી ? "" : (અક્ષર *)extra_shell_arg,
		    (અક્ષર *)p_shcf,
		    (અક્ષર *)cmd);
	    x = system((અક્ષર *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    જો (emsg_silent)
	;
    નહીંતર જો (x == 127)
	સંદેશ_puts(_("\nCannot execute shell sh\n"));
    નહીંતર જો (x && !(options & SHELL_SILENT))
    {
	સંદેશ_puts(_("\nshell returned "));
	સંદેશ_outnum((long)x);
	સંદેશ_putchar('\n');
    }

    જો (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# જો defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    પાછા x;
}

#નહીંતર // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    સ્થિર પૂર્ણાંક
mch_call_shell_fork(
    char_u	*cmd,
    પૂર્ણાંક		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    પૂર્ણાંક		status = -1;
    પૂર્ણાંક		retval = -1;
    અક્ષર	**argv = ખાલી;
    char_u	*tofree1 = ખાલી;
    char_u	*tofree2 = ખાલી;
    પૂર્ણાંક		i;
    પૂર્ણાંક		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    પૂર્ણાંક		pty_slave_fd = -1;
# endif
    પૂર્ણાંક		fd_toshell[2];		// for pipes
    પૂર્ણાંક		fd_fromshell[2];
    પૂર્ણાંક		pipe_error = FALSE;
    પૂર્ણાંક		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    જો (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    જો (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    જો (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	જાઓ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    જો ((options & (SHELL_READ|SHELL_WRITE))
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
	જો (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ખાલી, ખાલી);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	જો (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    જો (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		જો (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    જો (pipe_error)
	    {
		સંદેશ_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    જો (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	જો (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    સંદેશ_puts(_("\nCannot fork\n"));
	    જો ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		જો (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		નહીંતર				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	નહીંતર જો (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    જો (ch_log_active())
	    {
		ch_log(ખાલી, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    જો (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		પૂર્ણાંક fd;

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
		જો (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    નહીંતર જો ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		જો (p_stmp)
		{
		    (શૂન્ય)setsid();
#  જો defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		જો (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    જો (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (અક્ષર *)ખાલી);
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
		જો (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    જો (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		નહીંતર
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
		    જો (gui.in_use)
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
	નહીંતર			// parent
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
	    જો ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		પૂર્ણાંક	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		પૂર્ણાંક	    ta_len = 0;		// valid bytes in ta_buf[]
		પૂર્ણાંક	    len;
		પૂર્ણાંક	    p_more_save;
		પૂર્ણાંક	    old_State;
		પૂર્ણાંક	    c;
		પૂર્ણાંક	    toshell_fd;
		પૂર્ણાંક	    fromshell_fd;
		garray_T    ga;
		પૂર્ણાંક	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		જો (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		નહીંતર
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

		જો ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    જો ((wpid = fork()) == -1)
		    {
			સંદેશ_puts(_("\nCannot fork\n"));
		    }
		    નહીંતર જો (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			માટે (;;)
			{
			    જો (lplen == 0)
				len = 0;
			    નહીંતર જો (lp[written] == NL)
				// NL -> NUL translation
				len = લિખ(toshell_fd, "", (size_t)1);
			    નહીંતર
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = લિખ(toshell_fd, (અક્ષર *)lp + written,
					   s == ખાલી ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    જો (len == (પૂર્ણાંક)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				જો (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_લીટી_ગણતરી
						    || curbuf->b_p_eol)))
				    vim_ignored = લિખ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				જો (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    તોડો;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    નહીંતર જો (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    નહીંતર // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		જો (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		માટે (;;)
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
		    જો (!(options & SHELL_EXPAND)
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
		      જો (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      જો (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			જો (len == 1 && (pty_master_fd < 0 || cmd != ખાલી))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    જો (pty_master_fd < 0 && toshell_fd >= 0
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
			જો (pty_master_fd < 0)
			{
			    માટે (i = ta_len; i < ta_len + len; ++i)
			    {
				જો (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    સંદેશ_putchar(ta_buf[i]);
				નહીંતર જો (has_mbyte)
				{
				    પૂર્ણાંક l = (*mb_ptr2len)(ta_buf + i);

				    સંદેશ_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				નહીંતર
				    સંદેશ_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
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
			જો (options & SHELL_WRITE)
			    ta_len = 0;
			નહીંતર જો (toshell_fd >= 0)
			{
			    len = લિખ(toshell_fd, (અક્ષર *)ta_buf, (size_t)1);
			    જો (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    જો (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# નહીંતર
			kill(0, SIGINT);
# endif
			જો (wpid > 0)
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
		    જ્યારે (RealWaitForChar(fromshell_fd, 10L, ખાલી, ખાલી))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			જો (len <= 0)		    // end of file or error
			    જાઓ finished;

			noread_cnt = 0;
			જો (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    માટે (i = 0; i < len; ++i)
			    {
				જો (buffer[i] == NL)
				    append_ga_લીટી(&ga);
				નહીંતર જો (buffer[i] == NUL)
				    ga_append(&ga, NL);
				નહીંતર
				    ga_append(&ga, buffer[i]);
			    }
			}
			નહીંતર જો (has_mbyte)
			{
			    પૂર્ણાંક		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    માટે (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				જો (l == 0)
				    l = 1;  // NUL byte?
				નહીંતર જો (MB_BYTE2LEN(*p) != l)
				    તોડો;
			    }
			    જો (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				જો (len >= 12)
				    ++p;
				નહીંતર
				{
				    buffer_off = len;
				    ચાલુ;
				}
			    }
			    c = *p;
			    *p = NUL;
			    સંદેશ_puts((અક્ષર *)buffer);
			    જો (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				ચાલુ;
			    }
			    buffer_off = 0;
			}
			નહીંતર
			{
			    buffer[len] = NUL;
			    સંદેશ_puts((અક્ષર *)buffer);
			}

			windgoto(સંદેશ_row, cmdline_col_off + સંદેશ_col);
			cursor_on();
			out_flush();
			જો (got_int)
			    તોડો;

# ifdef ELAPSED_FUNC
			જો (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    જો (msec > 2000)
			    {
				noread_cnt = 5;
				તોડો;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    જો (wait_pid == pid)
		    {
			જો (noread_cnt < 5)
			    ચાલુ;
			તોડો;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    જો ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    નહીંતર
			wait_pid = 0;

# જો defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		જો (options & SHELL_READ)
		{
		    જો (ga.ga_len > 0)
		    {
			append_ga_લીટી(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    નહીંતર
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		જો (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		જો (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# જો (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    નહીંતર
	    {
		long delay_msec = 1;

		જો (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		માટે (;;)
		{
		    જો (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  નહીંતર
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    જો ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			તોડો;
		    }

#  જો defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    જો (++delay_msec > 10)
			delay_msec = 10;
		}

		જો (tmode == TMODE_RAW)
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
	    જો (wait_pid != pid)
		(શૂન્ય)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    જો (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    જો (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ખાલી);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    જો (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    જો (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		જો (retval != 0 && !emsg_silent)
		{
		    જો (retval == EXEC_FAILED)
		    {
			સંદેશ_puts(_("\nCannot execute shell "));
			સંદેશ_outtrans(p_sh);
			સંદેશ_putchar('\n');
		    }
		    નહીંતર જો (!(options & SHELL_SILENT))
		    {
			સંદેશ_puts(_("\nshell returned "));
			સંદેશ_outnum((long)retval);
			સંદેશ_putchar('\n');
		    }
		}
	    }
	    નહીંતર
		સંદેશ_puts(_("\nCommand terminated\n"));
	}
    }

error:
    જો (!did_settmode)
	જો (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    પાછા retval;
}
#endif // USE_SYSTEM

    પૂર્ણાંક
mch_call_shell(
    char_u	*cmd,
    પૂર્ણાંક		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(ખાલી, "executing shell command: %s", cmd);
#endif
#જો defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    જો (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != ખાલી
					      && (options & SHELL_SILENT) == 0)
	પાછા mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    પાછા mch_call_shell_system(cmd, options);
#નહીંતર
    પાછા mch_call_shell_fork(cmd, options);
#endif
}

#જો defined(FEAT_EVAL)
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
    અક્ષર	**argv,
    char_u	*infile,
    પૂર્ણાંક		flags UNUSED,
    પૂર્ણાંક		*ret_len)
{
    pid_t	pid;
    પૂર્ણાંક		fd_out[2] = {-1, -1};
    પૂર્ણાંક		status = -1;
    char_u	*buffer = ખાલી;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ખાલી, "directly executing: %s", argv[0]);

    જો (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	પાછા ખાલી;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# નહીંતર
    pid = fork();
# endif
    જો (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	પાછા ખાલી;
    }

    જો (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	જો (ch_log_active())
	{
	    ch_log(ખાલી, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	જો (infile != ખાલી)
	{
	    પૂર્ણાંક fd_in = open((અક્ષર *)infile, O_RDONLY);
	    જો (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	નહીંતર
	{
	    પૂર્ણાંક nullfd = open("/dev/null", O_RDONLY);
	    જો (nullfd >= 0)
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
    માટે (;;)
    {
	અક્ષર	buf[4096];
	પૂર્ણાંક	n;

	n = (પૂર્ણાંક)પઠ(fd_out[0], buf, કદ(buf));
	જો (n <= 0)
	    તોડો;
	જો (ga_grow(&ga, n) == FAIL)
	    તોડો;
	mch_memmove((અક્ષર *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (શૂન્ય)waitpid(pid, &status, 0);
    જો (WIFEXITED(status))
	status = WEXITSTATUS(status);
    નહીંતર
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    જો (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	જો (buffer != ખાલી)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    જો (ret_len == ખાલી)
	    {
		પૂર્ણાંક	i;

		// Change NUL into SOH, otherwise the string is truncated.
		માટે (i = 0; i < ga.ga_len; ++i)
		    જો (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    નહીંતર
		*ret_len = ga.ga_len;
	}
    }
    નહીંતર જો (ret_len != ખાલી)
	*ret_len = 0;

    ga_clear(&ga);
    પાછા buffer;
}
#endif

#જો defined(FEAT_JOB_CHANNEL)
    શૂન્ય
mch_job_start(અક્ષર **argv, job_T *job, jobopt_T *options, પૂર્ણાંક is_terminal)
{
    pid_t	pid;
    પૂર્ણાંક		fd_in[2] = {-1, -1};	// for stdin
    પૂર્ણાંક		fd_out[2] = {-1, -1};	// for stdout
    પૂર્ણાંક		fd_err[2] = {-1, -1};	// for stderr
    પૂર્ણાંક		pty_master_fd = -1;
    પૂર્ણાંક		pty_slave_fd = -1;
    channel_T	*channel = ખાલી;
    પૂર્ણાંક		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    પૂર્ણાંક		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    પૂર્ણાંક		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    પૂર્ણાંક		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    પૂર્ણાંક		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    પૂર્ણાંક		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    પૂર્ણાંક		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    પૂર્ણાંક		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    જો (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    જો (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    જો (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((અક્ષર *)fname, O_RDONLY, 0);
	જો (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    નહીંતર
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	જો (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    જાઓ failed;

    જો (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((અક્ષર *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	જો (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    નહીંતર જો (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	જાઓ failed;

    જો (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((અક્ષર *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	જો (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    જાઓ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    નહીંતર જો (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	જાઓ failed;

    જો (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	જો (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    જો (channel != ખાલી)
		++channel->ch_refcount;
	}
	નહીંતર
	    channel = add_channel();
	જો (channel == ખાલી)
	    જાઓ failed;
	જો (job->jv_tty_out != ખાલી)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    જો (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	જાઓ failed;
    }
    જો (pid == 0)
    {
	પૂર્ણાંક	null_fd = -1;
	પૂર્ણાંક	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	જો (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(શૂન્ય)setsid();
# endif

# ifdef FEAT_TERMINAL
	જો (options->jo_term_rows > 0)
	{
	    અક્ષર *term = (અક્ષર *)T_NAME;

#  ifdef FEAT_GUI
	    જો (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    જો (term == ખાલી || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		જો (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		નહીંતર જો (t_colors > 16)
		    term = "xterm-color";
		નહીંતર
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	નહીંતર
# endif
	    set_default_child_environment(is_terminal);

	જો (options->jo_env != ખાલી)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    પૂર્ણાંક		todo = (પૂર્ણાંક)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_વસ્તુઓ(&dict->dv_hashtab, hi, todo)
		જો (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ચાવી, tv_get_string(item));
		    --todo;
		}
	}

	જો (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    જો (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	જો (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (અક્ષર *)ખાલી);
# endif
	}

	// set up stdin for the child
	close(0);
	જો (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	નહીંતર જો (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	નહીંતર
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	જો (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	નહીંતર જો (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	નહીંતર જો (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	નહીંતર
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	જો (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	નહીંતર જો (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	નહીંતર
	    vim_ignored = dup(fd_out[1]);

	જો (fd_in[0] >= 0)
	    close(fd_in[0]);
	જો (fd_in[1] >= 0)
	    close(fd_in[1]);
	જો (fd_out[0] >= 0)
	    close(fd_out[0]);
	જો (fd_out[1] >= 0)
	    close(fd_out[1]);
	જો (fd_err[0] >= 0)
	    close(fd_err[0]);
	જો (fd_err[1] >= 0)
	    close(fd_err[1]);
	જો (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	જો (null_fd >= 0)
	    close(null_fd);

	જો (options->jo_cwd != ખાલી && mch_chdir((અક્ષર *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	જો (stderr_works)
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

    જો (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    જો (fd_in[0] >= 0)
	close(fd_in[0]);
    જો (fd_out[1] >= 0)
	close(fd_out[1]);
    જો (fd_err[1] >= 0)
	close(fd_err[1]);
    જો (channel != ખાલી)
    {
	પૂર્ણાંક in_fd = INVALID_FD;
	પૂર્ણાંક out_fd = INVALID_FD;
	પૂર્ણાંક err_fd = INVALID_FD;

	જો (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	જો (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	જો (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    જો (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    નહીંતર જો (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    નહીંતર
    {
	જો (fd_in[1] >= 0)
	    close(fd_in[1]);
	જો (fd_out[0] >= 0)
	    close(fd_out[0]);
	જો (fd_err[0] >= 0)
	    close(fd_err[0]);
	જો (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    પાછા;

failed:
    channel_unref(channel);
    જો (fd_in[0] >= 0)
	close(fd_in[0]);
    જો (fd_in[1] >= 0)
	close(fd_in[1]);
    જો (fd_out[0] >= 0)
	close(fd_out[0]);
    જો (fd_out[1] >= 0)
	close(fd_out[1]);
    જો (fd_err[0] >= 0)
	close(fd_err[0]);
    જો (fd_err[1] >= 0)
	close(fd_err[1]);
    જો (pty_master_fd >= 0)
	close(pty_master_fd);
    જો (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    સ્થિર char_u *
get_signal_name(પૂર્ણાંક sig)
{
    પૂર્ણાંક		i;
    char_u	numbuf[NUMBUFLEN];

    જો (sig == SIGKILL)
	પાછા vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    માટે (i = 0; signal_info[i].sig != -1; i++)
	જો (sig == signal_info[i].sig)
	    પાછા strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((અક્ષર *)numbuf, NUMBUFLEN, "%d", sig);
    પાછા vim_strnsave(numbuf, i);
}

    અક્ષર *
mch_job_status(job_T *job)
{
    પૂર્ણાંક		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    જો (wait_pid == -1)
    {
	પૂર્ણાંક waitpid_errno = errno;
	જો (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    પાછા "run";

	// process must have exited
	જો (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	જાઓ return_dead;
    }
    જો (wait_pid == 0)
	પાછા "run";
    જો (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	જો (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	જાઓ return_dead;
    }
    જો (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	જો (job->jv_status < JOB_ENDED && job->jv_termsig != ખાલી)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	જાઓ return_dead;
    }
    પાછા "run";

return_dead:
    જો (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    પાછા "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    પૂર્ણાંક		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    જો (dont_check_job_ended > 0)
	પાછા ખાલી;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    જો (wait_pid <= 0)
	// no process ended
	પાછા ખાલી;
    માટે (job = job_list; job != ખાલી; job = job->jv_next)
    {
	જો (job->jv_pid == wait_pid)
	{
	    જો (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    નહીંતર જો (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    જો (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    પાછા job;
	}
    }
    પાછા ખાલી;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    પૂર્ણાંક
mch_signal_job(job_T *job, char_u *how)
{
    પૂર્ણાંક	    sig = -1;

    જો (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    નહીંતર જો (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    નહીંતર જો (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    નહીંતર જો (STRCMP(how, "int") == 0)
	sig = SIGINT;
    નહીંતર જો (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    નહીંતર જો (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    નહીંતર જો (SAFE_isdigit(*how))
	sig = atoi((અક્ષર *)how);
    નહીંતર
	પાછા FAIL;

    // Never kill ourselves!
    જો (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    પાછા OK;
}

/*
 * Clear the data related to "job".
 */
    શૂન્ય
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (શૂન્ય)waitpid(job->jv_pid, ખાલી, WNOHANG);
}
#endif

#જો defined(FEAT_TERMINAL)
    પૂર્ણાંક
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    પૂર્ણાંક		pty_master_fd = -1;
    પૂર્ણાંક		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    જો (pty_master_fd < 0 || pty_slave_fd < 0)
	પાછા FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    જો (channel == ખાલી)
    {
	close(pty_master_fd);
	પાછા FAIL;
    }
    જો (job->jv_tty_out != ખાલી)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    પાછા OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    શૂન્ય
mch_breakcheck(પૂર્ણાંક force)
{
    જો ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ખાલી, ખાલી))
	fill_આગત_buf(FALSE);
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
    સ્થિર પૂર્ણાંક
WaitForChar(long msec, પૂર્ણાંક *interrupted, પૂર્ણાંક ignore_આગત)
{
#ifdef FEAT_TIMERS
    પાછા ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_આગત) == OK;
#નહીંતર
    પાછા WaitForCharOrMouse(msec, interrupted, ignore_આગત);
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
    સ્થિર પૂર્ણાંક
WaitForCharOrMouse(long msec, પૂર્ણાંક *interrupted, પૂર્ણાંક ignore_આગત)
{
#ifdef FEAT_MOUSE_GPM
    પૂર્ણાંક		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    પૂર્ણાંક		rest;
#endif
    પૂર્ણાંક		avail;

    જો (!ignore_આગત && આગત_available())	    // something in inbuf[]
	પાછા 1;

#જો defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    જો (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	જો (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#જો defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    જો (do_xterm_trace())
	rest = msec;
# endif
    કરો
    {
# ifdef FEAT_XCLIPBOARD
	જો (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    જો (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    જો (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	જો (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	જો (!avail && !gpm_process_wanted)
# નહીંતર
	avail = RealWaitForChar(read_cmd_fd, msec, ખાલી, interrupted);
	જો (!avail)
# endif
	{
	    જો (!ignore_આગત && આગત_available())
		પાછા 1;
# ifdef FEAT_XCLIPBOARD
	    જો (rest == 0 || !do_xterm_trace())
# endif
		તોડો;
	}
    }
    જ્યારે (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#નહીંતર
    avail = RealWaitForChar(read_cmd_fd, msec, ખાલી, interrupted);
#endif
    પાછા avail;
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
    સ્થિર પૂર્ણાંક
RealWaitForChar(પૂર્ણાંક fd, long msec, પૂર્ણાંક *check_for_gpm UNUSED, પૂર્ણાંક *interrupted)
{
    પૂર્ણાંક		ret;
    પૂર્ણાંક		result;
# જો defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    સ્થિર પૂર્ણાંક	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    જો (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    જો (busy)
	પાછા 0;
# endif

# ifdef MAY_LOOP
    માટે (;;)
# endif
    {
# ifdef MAY_LOOP
	પૂર્ણાંક		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	પૂર્ણાંક		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	પૂર્ણાંક		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	સંરચના pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	પૂર્ણાંક		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	પૂર્ણાંક             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	પૂર્ણાંક		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	પૂર્ણાંક		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	પૂર્ણાંક		xsmp_idx = -1;
#  endif
	પૂર્ણાંક		towait = (પૂર્ણાંક)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	જો (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (પૂર્ણાંક)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	જો ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	જો (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_સંખ્યા(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	જો (check_for_gpm != ખાલી && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	જો (xsmp_icefd != -1)
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
	જો (interrupted != ખાલી)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	જો (result == 0 && interrupted != ખાલી && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	જો (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	જો (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	જો (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    જો (--ret == 0 && !આગત_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	જો (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	જો (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    જો (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    નહીંતર જો (fds[xsmp_idx].revents & POLLHUP)
	    {
		જો (p_verbose > 0)
		    verb_સંદેશ(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    જો (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	જો (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# નહીંતર // HAVE_SELECT

	સંરચના timeval  tv;
	સંરચના timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	સ્થિર fd_set	rfds, wfds, efds;
	પૂર્ણાંક		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	જો (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	જો (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	નહીંતર
	    tvp = ખાલી;

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
	જો ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    જો (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	જો (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_સંખ્યા(xterm_dpy), &rfds);
	    જો (maxfd < Connection_સંખ્યા(xterm_dpy))
		maxfd = Connection_સંખ્યા(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	જો (check_for_gpm != ખાલી && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    જો (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	જો (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    જો (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	જો (interrupted != ખાલી)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	જો (result)
	    --ret;
	નહીંતર જો (interrupted != ખાલી && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	જો (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    જો (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    જો (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ખાલી, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    જાઓ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	જો (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	જો (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	જો (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	જો (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_સંખ્યા(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    જો (--ret == 0 && !આગત_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	જો (ret > 0 && check_for_gpm != ખાલી && gpm_flag && gpm_fd >= 0)
	{
	    જો (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    નહીંતર જો (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	જો (ret > 0 && xsmp_icefd != -1)
	{
	    જો (FD_ISSET(xsmp_icefd, &efds))
	    {
		જો (p_verbose > 0)
		    verb_સંદેશ(_("XSMP lost ICE connection"));
		xsmp_close();
		જો (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    નહીંતર જો (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		જો (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	જો (ret >= 0)
	    (શૂન્ય)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	જો (finished || msec == 0)
	    તોડો;

#  જો defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	જો (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    તોડો;
#  endif

	// We're going to loop around again, find out for how long
	જો (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  નહીંતર
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    જો (msec <= 0)
		તોડો;	// waited long enough
	}
# endif
    }

    પાછા result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    પૂર્ણાંક
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    પૂર્ણાંક		flags)		// EW_* flags
{
    પાછા unix_expandpath(gap, path, 0, flags, FALSE);
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

    પૂર્ણાંક
mch_expand_wildcards(
    પૂર્ણાંક		   અંક_pat,
    char_u	 **pat,
    પૂર્ણાંક		  *અંક_file,
    char_u	***file,
    પૂર્ણાંક		   flags)	// EW_* flags
{
    પૂર્ણાંક		i;
    size_t	len;
    long	llen;
    char_u	*p;
    પૂર્ણાંક		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    પૂર્ણાંક		j;
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
    પૂર્ણાંક		shell_style = STYLE_ECHO;
    પૂર્ણાંક		check_spaces;
    સ્થિર પૂર્ણાંક	did_find_nul = FALSE;
    પૂર્ણાંક		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    સ્થિર string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    સ્થિર string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *અંક_file = 0;	// default: no files found
    *file = ખાલી;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    જો (!have_wildcard(અંક_pat, pat))
	પાછા save_patterns(અંક_pat, pat, અંક_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    જો (sandbox != 0 && check_secure())
	પાછા FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    જો (secure || restricted)
	માટે (i = 0; i < અંક_pat; ++i)
	    જો (vim_strchr(pat[i], '`') != ખાલી
		    && (check_restricted() || check_secure()))
		પાછા FAIL;

    /*
     * get a name for the temp file
     */
    જો ((tempname = vim_tempname('o', FALSE)) == ખાલી)
    {
	emsg(_(e_cant_get_temp_file_name));
	પાછા FAIL;
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
    જો (અંક_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    નહીંતર જો ((len = STRLEN(p_sh)) >= 3)
    {
	જો (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	નહીંતર જો (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    જો (shell_style == STYLE_ECHO)
    {
	જો (strstr((અક્ષર *)gettail(p_sh), "bash") != ખાલી)
	    shell_style = STYLE_GLOBSTAR;
	નહીંતર જો (strstr((અક્ષર *)gettail(p_sh), "sh") != ખાલી)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    જો (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    નહીંતર જો (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    માટે (i = 0; i < અંક_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# નહીંતર
	++len;				// add space
	માટે (j = 0; pat[i][j] != NUL; ++j)
	{
	    જો (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ખાલી)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    જો (command == ખાલી)
    {
	// out of memory
	vim_free(tempname);
	પાછા FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    જો (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((અક્ષર *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((અક્ષર *)command, ")>");
	જો (p == ખાલી)
	{
	    ;					    // can't happen ;-)
	}
	નહીંતર
	{
	    --p;
	    જ્યારે (p > command && VIM_ISWHITE(*p))
		--p;
	    જો (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    જ્યારે (p > command && VIM_ISWHITE(*p))
		--p;
	    જો (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    નહીંતર
    {
	જો (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((અક્ષર *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	નહીંતર જો (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((અક્ષર *)command, len, "print -N >%s", tempname);
	નહીંતર જો (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((અક્ષર *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	નહીંતર જો (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((અક્ષર *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	નહીંતર
	    commandlen = vim_snprintf((અક્ષર *)command, len, "echo >%s", tempname);

	માટે (i = 0; i < અંક_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((અક્ષર *)command + commandlen, len, " \"%s\"", pat[i]);
# નહીંતર
	    પૂર્ણાંક intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    માટે (j = 0; pat[i][j] != NUL; ++j)
	    {
		જો (pat[i][j] == '`')
		    intick = !intick;
		નહીંતર જો (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    જો (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ખાલી
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		નહીંતર જો (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ખાલી)
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
    જો (flags & EW_SILENT)
	show_shell_mess = FALSE;
    જો (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    જો (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    નહીંતર જો (shell_style == STYLE_GLOB && !have_dollars(અંક_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    જો (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ખાલી;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    જો (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	જો (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    સંદેશ_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    જો (!(flags & EW_SILENT))
# endif
	    {
		સંદેશ(_(e_cannot_expand_wildcards));
		સંદેશ_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	જો (shell_style == STYLE_BT)
	    પાછા FAIL;
	જાઓ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((અક્ષર *)tempname, READBIN);
    જો (fd == ખાલી)
    {
	// Something went wrong, perhaps a file name with a special char.
	જો (!(flags & EW_SILENT))
	{
	    સંદેશ(_(e_cannot_expand_wildcards));
	    સંદેશ_start();		// don't overwrite this message
	}
	vim_free(tempname);
	જાઓ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    જો (llen < 0)
	// just in case ftell() would fail
	buffer = ખાલી;
    નહીંતર
	buffer = alloc(llen + 1);
    જો (buffer == ખાલી)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	પાછા FAIL;
    }
    len = llen;
    i = fread((અક્ષર *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    જો (i != (પૂર્ણાંક)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	પાછા FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    માટે (i = 0; i < (પૂર્ણાંક)len; ++i)
	જો (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    જો (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	માટે (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    જ્યારે (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    નહીંતર જો (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	માટે (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    જ્યારે (*p != '\n' && *p != NUL)
		++p;
	    જો (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    નહીંતર
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
	જો (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    જો (len && (પૂર્ણાંક)STRLEN(buffer) < (પૂર્ણાંક)len)
		did_find_nul = TRUE;
	    નહીંતર
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	જો (len && buffer[len - 1] == NUL)
	    --len;
	નહીંતર
	    buffer[len] = NUL;
	i = 0;
	માટે (p = buffer; p < buffer + len; ++p)
	    જો (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	જો (len)
	    ++i;			// count last entry
    }
    જો (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	જાઓ notfound;
    }
    *અંક_file = i;
    *file = ALLOC_MULT(char_u *, i);
    જો (*file == ખાલી)
    {
	// out of memory
	vim_free(buffer);
	પાછા FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    માટે (i = 0; i < *અંક_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	જો (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    જ્યારે (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    જો (p == buffer + len)		// last entry
		*p = NUL;
	    નહીંતર
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	નહીંતર		// NUL separates
	{
	    જ્યારે (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    માટે (j = 0, i = 0; i < *અંક_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	જો (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ચાલુ;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	જો ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ચાલુ;

	// Skip files that are not executable if we check for that.
	જો (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ખાલી, !(flags & EW_SHELLCMD)))
	    ચાલુ;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	જો (p)
	{
	    STRCPY(p, (*file)[i]);
	    જો (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *અંક_file = j;

    જો (*અંક_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	જાઓ notfound;
    }

    પાછા OK;

notfound:
    જો (flags & EW_NOTFOUND)
	પાછા save_patterns(અંક_pat, pat, અંક_file, file);
    પાછા FAIL;
}

#endif // VMS

    સ્થિર પૂર્ણાંક
save_patterns(
    પૂર્ણાંક		અંક_pat,
    char_u	**pat,
    પૂર્ણાંક		*અંક_file,
    char_u	***file)
{
    પૂર્ણાંક		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, અંક_pat);
    જો (*file == ખાલી)
	પાછા FAIL;
    માટે (i = 0; i < અંક_pat; i++)
    {
	s = vim_strsave(pat[i]);
	જો (s != ખાલી)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *અંક_file = અંક_pat;
    પાછા OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    પૂર્ણાંક
mch_has_exp_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર
	    જો (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#નહીંતર
				    "*?[{'"
#endif
						, *p) != ખાલી)
	    પાછા TRUE;
    }
    પાછા FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    પૂર્ણાંક
mch_has_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર
	    જો (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#નહીંતર
				    "*?[{`'$"
#endif
						, *p) != ખાલી
		|| (*p == '~' && p[1] != NUL))
	    પાછા TRUE;
    }
    પાછા FALSE;
}

    સ્થિર પૂર્ણાંક
have_wildcard(પૂર્ણાંક અંક, char_u **file)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < અંક; i++)
	જો (mch_has_wildcard(file[i]))
	    પાછા 1;
    પાછા 0;
}

    સ્થિર પૂર્ણાંક
have_dollars(પૂર્ણાંક અંક, char_u **file)
{
    પૂર્ણાંક	    i;

    માટે (i = 0; i < અંક; i++)
	જો (vim_strchr(file[i], '$') != ખાલી)
	    પાછા TRUE;
    પાછા FALSE;
}

#જો !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    પૂર્ણાંક
mch_rename(અચલ અક્ષર *src, અચલ અક્ષર *dest)
{
    સંરચના stat	    st;

    જો (stat(dest, &st) >= 0)	    // fail if destination exists
	પાછા -1;
    જો (link(src, dest) != 0)	    // link file to new name
	પાછા -1;
    જો (mch_remove(src) == 0)	    // delete link to old name
	પાછા 0;
    પાછા -1;
}
#endif // !HAVE_RENAME

#જો defined(FEAT_MOUSE_GPM)
# જો defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    સ્થિર પૂર્ણાંક
load_libgpm(શૂન્ય)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    જો (libgpm_hinst == ખાલી)
    {
	જો (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	પાછા FAIL;
    }

    જો (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ખાલી
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ખાલી
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ખાલી
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ખાલી
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ખાલી
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ખાલી;
	dll_gpm_flag = ખાલી;
	dll_gpm_fd   = ખાલી;
	પાછા FAIL;
    }
    પાછા OK;
}

    પૂર્ણાંક
gpm_available(શૂન્ય)
{
    પાછા libgpm_hinst != ખાલી || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    સ્થિર પૂર્ણાંક
gpm_open(શૂન્ય)
{
    સ્થિર Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    જો (!gpm_available())
	પાછા 0;
# endif

    જો (gpm_flag)
	પાછા 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    જો (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	પાછા 1; // succeed
    }
    જો (gpm_fd == -2)
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
    પાછા 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    પૂર્ણાંક
gpm_enabled(શૂન્ય)
{
    પાછા gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    સ્થિર શૂન્ય
gpm_close(શૂન્ય)
{
    જો (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    સ્થિર પૂર્ણાંક
mch_gpm_process(શૂન્ય)
{
    પૂર્ણાંક			button;
    સ્થિર Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    પૂર્ણાંક			row,col;
    unsigned અક્ષર	buttons_mask;
    unsigned અક્ષર	gpm_modifiers;
    સ્થિર unsigned અક્ષર old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    પસંદગી (GPM_BARE_EVENTS(gpm_event.type))
    {
	સ્થિતિ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    તોડો;
	સ્થિતિ GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    પસંદગી (buttons_mask)
	    {
		સ્થિતિ GPM_B_ડાબું:
		    button = MOUSE_ડાબું;
		    તોડો;
		સ્થિતિ GPM_B_મધ્ય:
		    button = MOUSE_મધ્ય;
		    તોડો;
		સ્થિતિ GPM_B_જમણું:
		    button = MOUSE_જમણું;
		    તોડો;
		મૂળભૂત:
		    પાછા 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_અંક_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    તોડો;
	સ્થિતિ GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    તોડો;
	મૂળભૂત:
	    પાછા 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    જો (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    જો (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    જો (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_આગત_buf(string, 6);
    પાછા 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    સ્થિર પૂર્ણાંક
sysmouse_open(શૂન્ય)
{
    સંરચના mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.સંકેત = SIGUSR2;
    જો (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	પાછા FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    પાછા OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    સ્થિર શૂન્ય
sysmouse_close(શૂન્ય)
{
    સંરચના mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.સંકેત = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    સ્થિર શૂન્ય
sig_sysmouse SIGDEFARG(sigarg)
{
    સંરચના mouse_info	mouse;
    સંરચના video_info	video;
    char_u		string[6];
    પૂર્ણાંક			row, col;
    પૂર્ણાંક			button;
    પૂર્ણાંક			buttons;
    સ્થિર પૂર્ણાંક		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    જો (hold_gui_events)
	પાછા;
# endif

    mouse.operation = MOUSE_GETINFO;
    જો (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	જો (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	નહીંતર
	{
	    પસંદગી (buttons)
	    {
		સ્થિતિ 0:
		    button = MOUSE_RELEASE;
		    તોડો;
		સ્થિતિ 1:
		    button = MOUSE_ડાબું;
		    તોડો;
		સ્થિતિ 2:
		    button = MOUSE_મધ્ય;
		    તોડો;
		સ્થિતિ 4:
		    button = MOUSE_જમણું;
		    તોડો;
		મૂળભૂત:
		    પાછા;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_આગત_buf(string, 6);
    }
    પાછા;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    પૂર્ણાંક
mch_get_random(char_u *buf, પૂર્ણાંક len)
{
    સ્થિર પૂર્ણાંક dev_urandom_state = NOTDONE;

    જો (dev_urandom_state == FAIL)
	પાછા FAIL;

    પૂર્ણાંક fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    જો (fd == -1)
	dev_urandom_state = FAIL;
    નહીંતર જો (પઠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    નહીંતર
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    પાછા dev_urandom_state;
}

#જો defined(FEAT_LIBCALL)
પ્રકારવ્યાખ્યા char_u * (*STRPROCSTR)(char_u *);
પ્રકારવ્યાખ્યા char_u * (*INTPROCSTR)(પૂર્ણાંક);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*STRPROCINT)(char_u *);
પ્રકારવ્યાખ્યા પૂર્ણાંક (*INTPROCINT)(પૂર્ણાંક);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    પૂર્ણાંક
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    પૂર્ણાંક		argint,
    char_u	**string_result, // NULL when using number_result
    પૂર્ણાંક		*સંખ્યા_result)
{
# જો defined(USE_DLOPEN)
    શૂન્ય	*hinstLib;
    અક્ષર	*dlerr = ખાલી;
# નહીંતર
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = ખાલી;
    પૂર્ણાંક		retval_int = 0;
    પૂર્ણાંક		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# જો defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (શૂન્ય)dlerror();

    hinstLib = dlopen((અક્ષર *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    જો (hinstLib == ખાલી)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	જો (dlerr != ખાલી)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# નહીંતર
    hinstLib = shl_load((અચલ અક્ષર*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    જો (hinstLib != ખાલી)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	જો (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  જો defined(USE_DLOPEN)
	    dlerr = ખાલી;
#  endif
	    mch_didjmp();
	}
	નહીંતર
# endif
	{
	    retval_str = ખાલી;
	    retval_int = 0;

	    જો (argstring != ખાલી)
	    {
# જો defined(USE_DLOPEN)
		*(શૂન્ય **)(&ProcAdd) = dlsym(hinstLib, (અચલ અક્ષર *)funcname);
		dlerr = dlerror();
# નહીંતર
		જો (shl_findsym(&hinstLib, (અચલ અક્ષર *)funcname,
					TYPE_PROCEDURE, (શૂન્ય *)&ProcAdd) < 0)
		    ProcAdd = ખાલી;
# endif
		જો ((success = (ProcAdd != ખાલી
# જો defined(USE_DLOPEN)
			    && dlerr == ખાલી
# endif
			    )))
		{
		    જો (string_result == ખાલી)
			retval_int = ((STRPROCINT)(શૂન્ય *)ProcAdd)(argstring);
		    નહીંતર
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    નહીંતર
	    {
# જો defined(USE_DLOPEN)
		*(શૂન્ય **)(&ProcAddI) = dlsym(hinstLib, (અચલ અક્ષર *)funcname);
		dlerr = dlerror();
# નહીંતર
		જો (shl_findsym(&hinstLib, (અચલ અક્ષર *)funcname,
				       TYPE_PROCEDURE, (શૂન્ય *)&ProcAddI) < 0)
		    ProcAddI = ખાલી;
# endif
		જો ((success = (ProcAddI != ખાલી
# જો defined(USE_DLOPEN)
			    && dlerr == ખાલી
# endif
			    )))
		{
		    જો (string_result == ખાલી)
			retval_int = ((INTPROCINT)(શૂન્ય *)ProcAddI)(argint);
		    નહીંતર
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    જો (string_result == ખાલી)
		*સંખ્યા_result = retval_int;
	    નહીંતર જો (retval_str != ખાલી
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	જો (lc_signal != 0)
	{
	    પૂર્ણાંક i;

	    // try to find the name of this signal
	    માટે (i = 0; signal_info[i].sig != -1; i++)
		જો (lc_signal == signal_info[i].sig)
		    તોડો;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# જો defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	જો (dlerr != ખાલી)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(શૂન્ય)dlclose(hinstLib);
# નહીંતર
	(શૂન્ય)shl_unload(hinstLib);
# endif
    }

    જો (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	પાછા FAIL;
    }

    પાછા OK;
}
#endif

#જો defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
સ્થિર પૂર્ણાંક	xterm_trace = -1;	// default: disabled
સ્થિર પૂર્ણાંક	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    શૂન્ય
setup_term_clip(શૂન્ય)
{
    પૂર્ણાંક		z = 0;
    અક્ષર	*strp = "";
    Widget	AppShell;

    જો (!x_connect_to_server())
	પાછા;

    open_app_context();
    જો (app_context != ખાલી && xterm_Shell == (Widget)0)
    {
	પૂર્ણાંક (*oldhandler)(Display*, XErrorEvent*);
# જો defined(USING_SETJMP)
	પૂર્ણાંક (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	જો (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# જો defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	જો (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ખાલી;
	}
	નહીંતર
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ખાલી, 0, &z, &strp);
	    જો (xterm_dpy != ખાલી)
		xterm_dpy_retry_ગણતરી = 0;
# જો defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# જો defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(શૂન્ય)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(શૂન્ય)XSetErrorHandler(oldhandler);

	જો (xterm_dpy == ખાલી)
	{
	    જો (p_verbose > 0)
		verb_સંદેશ(_("Opening the X display failed"));
	    પાછા;
	}

	// Catch terminating error of the X server connection.
	(શૂન્ય)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	જો (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ખાલી);
	જો (AppShell == (Widget)0)
	    પાછા;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		ટોચ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ખાલી);
	જો (xterm_Shell == (Widget)0)
	    પાછા;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	જો (x11_display == ખાલી)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    જો (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	જો (x11_window == 0 && (strp = getenv("WINDOWID")) != ખાલી)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	જો (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	જો (x11_window != 0)
	    xterm_trace = 0;
    }
}

    શૂન્ય
start_xterm_trace(પૂર્ણાંક button)
{
    જો (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	પાછા;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    શૂન્ય
stop_xterm_trace(શૂન્ય)
{
    જો (xterm_trace < 0)
	પાછા;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    સ્થિર પૂર્ણાંક
do_xterm_trace(શૂન્ય)
{
    Window		મૂળ, child;
    પૂર્ણાંક			મૂળ_x, મૂળ_y;
    પૂર્ણાંક			win_x, win_y;
    પૂર્ણાંક			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    સ્થિર char_u	*mouse_code = ખાલી;
    સ્થિર size_t	mouse_codelen = 0;
    સ્થિર char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    સ્થિર પૂર્ણાંક		prev_row = 0, prev_col = 0;
    સ્થિર XSizeHints	xterm_hints;

    જો (xterm_trace <= 0)
	પાછા FALSE;

    જો (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	જો (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    પાછા FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	જો (mouse_code != ખાલી)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &મૂળ, &child, &મૂળ_x, &મૂળ_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	જો (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	જો (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	પાછા TRUE;
    }

    જો (mouse_code == ખાલી || mouse_codelen > 45)
    {
	xterm_trace = 0;
	પાછા FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &મૂળ, &child, &મૂળ_x, &મૂળ_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    જો (row == prev_row && col == prev_col)
	પાછા TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_આગત_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    પાછા TRUE;
}

# જો defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    શૂન્ય
clear_xterm_clip(શૂન્ય)
{
    જો (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    જો (xterm_dpy != ખાલી)
    {
#  જો 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	જો (x11_display == xterm_dpy)
	    x11_display = ખાલી;
	xterm_dpy = ખાલી;
    }
#  જો 0
    જો (app_context != (XtAppContext)ખાલી)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ખાલી;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    સ્થિર શૂન્ય
clip_update(શૂન્ય)
{
# ifdef FEAT_GUI
    જો (gui.in_use)
	gui_mch_update();
    નહીંતર
# endif
    જો (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    શૂન્ય
xterm_update(શૂન્ય)
{
    XEvent event;

    માટે (;;)
    {
	Xt_આગત_Mask mask = XtAppPending(app_context);

	જો (mask == 0 || vim_is_આગત_buf_full())
	    તોડો;

	જો (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		જો (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_કિંમત)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	નહીંતર
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    પૂર્ણાંક
clip_xterm_own_selection(Clipboard_T *cbd)
{
    જો (xterm_Shell != (Widget)0)
	પાછા clip_x11_own_selection(xterm_Shell, cbd);
    પાછા FAIL;
}

    શૂન્ય
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    જો (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    શૂન્ય
clip_xterm_request_selection(Clipboard_T *cbd)
{
    જો (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    શૂન્ય
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#જો defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# જો defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    સ્થિર શૂન્ય
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    પૂર્ણાંક		save_cmod_flags;
    પૂર્ણાંક		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    જો (check_changed_any(FALSE, FALSE))
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
    જો (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    સ્થિર શૂન્ય
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    પૂર્ણાંક		save_type UNUSED,
    Bool	shutdown,
    પૂર્ણાંક		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    જો (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    જો (p_verbose > 0)
	verb_સંદેશ(_("XSMP handling save-yourself request"));

# જો defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    જો (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    નહીંતર
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
    સ્થિર શૂન્ય
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    સ્થિર શૂન્ય
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
    સ્થિર શૂન્ય
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    જો (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    સ્થિર શૂન્ય
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    જો (opening)
    {
	xsmp_icefd = Ice_Connection_સંખ્યા(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ખાલી);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    પૂર્ણાંક
xsmp_handle_requests(શૂન્ય)
{
    Bool rep;

    જો (IceProcessMessages(xsmp.iceconn, ખાલી, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	જો (p_verbose > 0)
	    verb_સંદેશ(_("XSMP lost ICE connection"));
	xsmp_close();
	પાછા FAIL;
    }
    નહીંતર
	પાછા OK;
}

સ્થિર પૂર્ણાંક dummy;

// Set up X Session Management Protocol
    શૂન્ય
xsmp_init(શૂન્ય)
{
    અક્ષર		errorstring[80];
    SmcCallbacks	smcallbacks;
# જો 0
    Sm_Prop_કિંમત		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    જો (p_verbose > 0)
	verb_સંદેશ(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = ખાલી;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = ખાલી;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = ખાલી;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = ખાલી;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    જો (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	જો (p_verbose > 0)
	    verb_સંદેશ(_("XSMP ICE connection watch failed"));
	પાછા;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ખાલી,
	    ખાલી,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    ખાલી,
	    &xsmp.clientid,
	    કદ(errorstring) - 1,
	    errorstring);
    જો (xsmp.smcconn == ખાલી)
    {
	જો (p_verbose > 0)
	{
	    અક્ષર errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[કદ(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, કદ(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_સંદેશ(errorreport);
	}
	પાછા;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# જો 0
    // ID ourselves
    smname.કિંમત = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.અંક_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    શૂન્ય
xsmp_close(શૂન્ય)
{
    જો (xsmp_icefd == -1)
	પાછા;

    SmcCloseConnection(xsmp.smcconn, 0, ખાલી);
    જો (xsmp.clientid != ખાલી)
	મુક્ત(xsmp.clientid);
    xsmp.clientid = ખાલી;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#જો defined(FEAT_RELTIME)
# જો defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
સ્થિર અસ્થિર sig_atomic_t timeout_flag = FALSE;
સ્થિર timer_t		     timer_id;
સ્થિર પૂર્ણાંક		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    સ્થિર શૂન્ય
set_flag(સંઘ sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    શૂન્ય
stop_timeout(શૂન્ય)
{
    સ્થિર સંરચના itimerspec disarm = {{0, 0}, {0, 0}};

    જો (timer_created)
    {
	પૂર્ણાંક ret = timer_settime(timer_id, 0, &disarm, ખાલી);

	જો (ret < 0)
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
    અસ્થિર sig_atomic_t *
start_timeout(long msec)
{
    સંરચના itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    પૂર્ણાંક ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    જો (!timer_created)
    {
	સંરચના sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	જો (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    પાછા &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ખાલી, "setting timeout timer to %d sec %ld nsec",
	       (પૂર્ણાંક)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ખાલી);
    જો (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    પાછા &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    શૂન્ય
delete_timer(શૂન્ય)
{
    જો (!timer_created)
	પાછા;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# નહીંતર // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
સ્થિર સંરચના sigaction		prev_sigaction;
સ્થિર અસ્થિર sig_atomic_t	timeout_flag	     = FALSE;
સ્થિર પૂર્ણાંક			timer_active	     = FALSE;
સ્થિર પૂર્ણાંક			timer_handler_active = FALSE;
સ્થિર અસ્થિર sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    સ્થિર શૂન્ય
set_flag SIGDEFARG(sigarg)
{
    જો (alarm_pending)
	alarm_pending = FALSE;
    નહીંતર
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    શૂન્ય
stop_timeout(શૂન્ય)
{
    સ્થિર સંરચના itimerval disarm = {{0, 0}, {0, 0}};
    પૂર્ણાંક			    ret;

    જો (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ખાલી);
	જો (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    જો (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ખાલી);
	જો (ret < 0)
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
    અસ્થિર sig_atomic_t *
start_timeout(long msec)
{
    સંરચના itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    સંરચના sigaction	handle_alarm;
    પૂર્ણાંક			ret;
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
    જો (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ખાલી);
    }
    જો (unlikely(ret != 0 || alarm_pending < 0))
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
    જો (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	પાછા &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ખાલી);
    જો (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	પાછા &timeout_flag;
    }

    timer_active = TRUE;
    પાછા &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
