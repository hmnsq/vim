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

#include "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#include "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
static පූර්ණ_සංඛ්‍යා selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != NULL ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != NULL ? *dll_gpm_fd   : -1)

static පූර්ණ_සංඛ්‍යා (*dll_Gpm_Open)     (Gpm_Connect *, පූර්ණ_සංඛ්‍යා);
static පූර්ණ_සංඛ්‍යා (*dll_Gpm_Close)    (void);
static පූර්ණ_සංඛ්‍යා (*dll_Gpm_GetEvent) (Gpm_Event *);
static පූර්ණ_සංඛ්‍යා *dll_gpm_flag;
static පූර්ණ_සංඛ්‍යා *dll_gpm_fd;

static void *libgpm_hinst;
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

static void gpm_close(void);
static පූර්ණ_සංඛ්‍යා gpm_open(void);
static පූර්ණ_සංඛ්‍යා mch_gpm_process(void);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

static පූර්ණ_සංඛ්‍යා sysmouse_open(void);
static void sysmouse_close(void);
static void sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#නම් defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#නම් defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
static Widget	xterm_Shell = (Widget)0;
static void clip_update(void);
# endif

Window	    x11_window = 0;
Display	    *x11_display = NULL;
#endif

static පූර්ණ_සංඛ්‍යා ignore_sigtstp = FALSE;

static පූර්ණ_සංඛ්‍යා get_x11_title(පූර්ණ_සංඛ්‍යා);

static char_u	*oldtitle = NULL;
static ca_ncala sig_atomic_t oldtitle_outdated = FALSE;
static පූර්ණ_සංඛ්‍යා	unix_did_set_title = FALSE;
static char_u	*oldicon = NULL;
static පූර්ණ_සංඛ්‍යා	did_set_icon = FALSE;

static void may_core_dump(void);

typedef පූර්ණ_සංඛ්‍යා waitstatus;
static පූර්ණ_සංඛ්‍යා  WaitForChar(long msec, පූර්ණ_සංඛ්‍යා *interrupted, පූර්ණ_සංඛ්‍යා ignore_input);
static පූර්ණ_සංඛ්‍යා  WaitForCharOrMouse(long msec, පූර්ණ_සංඛ්‍යා *interrupted, පූර්ණ_සංඛ්‍යා ignore_input);
#ifdef VMS
පූර්ණ_සංඛ්‍යා  RealWaitForChar(පූර්ණ_සංඛ්‍යා, long, පූර්ණ_සංඛ්‍යා *, පූර්ණ_සංඛ්‍යා *interrupted);
#නැතහොත්
static පූර්ණ_සංඛ්‍යා  RealWaitForChar(පූර්ණ_සංඛ්‍යා, long, පූර්ණ_සංඛ්‍යා *, පූර්ණ_සංඛ්‍යා *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
static පූර්ණ_සංඛ්‍යා do_xterm_trace(void);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

static void handle_resize(void);

#නම් defined(SIGWINCH)
static void sig_winch SIGPROTOARG;
#endif
#නම් defined(SIGTSTP)
static void sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
static ca_ncala sig_atomic_t in_mch_suspend = FALSE;
#endif
#නම් defined(SIGINT)
static void catch_sigint SIGPROTOARG;
#endif
#නම් defined(SIGUSR1)
static void catch_sigusr1 SIGPROTOARG;
#endif
#නම් defined(SIGPWR)
static void catch_sigpwr SIGPROTOARG;
#endif
#නම් defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
static void sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
static ca_ncala sig_atomic_t sig_alarm_called;
#endif
static void deathtrap SIGPROTOARG;

static void catch_int_signal(void);
static void set_signals(void);
static void catch_signals(void (*func_deadly)(පූර්ණ_සංඛ්‍යා), void (*func_other)(පූර්ණ_සංඛ්‍යා));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#නැතහොත්
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	do { /**/ } අතරතුර (0)
# define UNBLOCK_SIGNALS(set)	do { /**/ } අතරතුර (0)
#endif
static පූර්ණ_සංඛ්‍යා  have_wildcard(පූර්ණ_සංඛ්‍යා, char_u **);
static පූර්ණ_සංඛ්‍යා  have_dollars(පූර්ණ_සංඛ්‍යා, char_u **);

static පූර්ණ_සංඛ්‍යා save_patterns(පූර්ණ_සංඛ්‍යා num_pat, char_u **pat, පූර්ණ_සංඛ්‍යා *num_file, char_u ***file);

#ifndef SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
static ca_ncala sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
static ca_ncala sig_atomic_t got_tstp = FALSE;
static char_u	*extra_shell_arg = NULL;
static පූර්ණ_සංඛ්‍යා	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
static ca_ncala sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
static ca_ncala sig_atomic_t in_mch_delay = FALSE; // sleeping in mch_delay()

#නම් defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
static පූර්ණ_සංඛ්‍යා dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
static tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
typedef sa_nracanaa
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    var_na *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

static xsmp_config_T xsmp;
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

static sa_nracanaa signalinfo
{
    පූර්ණ_සංඛ්‍යා	    sig;	// Signal number, eg. SIGSEGV etc
    var_na    *name;	// Signal name (not char_u!).
    var_na    deadly;	// Catch as a deadly signal?
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
#නම් defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
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
#නම් defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#නම් defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#නම් defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
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
mch_signal(පූර්ණ_සංඛ්‍යා sig, sighandler_T func)
{
#නම් defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    sa_nracanaa sigaction	sa, old;
    sigset_t		curset;
    පූර්ණ_සංඛ්‍යා			blocked;

    නම් (sigprocmask(SIG_BLOCK, NULL, &curset) == -1)
	ආපසු SIG_ERR;

    blocked = sigismember(&curset, sig);

    නම් (func == SIG_HOLD)
    {
	නම් (blocked)
	    ආපසු SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	නම් (sigaction(sig, NULL, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, NULL) == -1)
	    ආපසු SIG_ERR;
	ආපසු old.sa_handler;
    }

    නම් (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	නම් (sigprocmask(SIG_UNBLOCK, &curset, NULL) == -1)
	    ආපසු SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# නැතහොත්
    sa.sa_flags = 0;
# endif
    නම් (sigaction(sig, &sa, &old) == -1)
	ආපසු SIG_ERR;
    ආපසු blocked ? SIG_HOLD: old.sa_handler;
#elif defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    ආපසු sigset(sig, func);
#නැතහොත්
    // Oldest and most compatible solution.
    ආපසු sa_nkeeta(sig, func);
#endif
}

    පූර්ණ_සංඛ්‍යා
mch_chdir(var_na *path)
{
    නම් (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    ආපසු chdir(vms_fixfilename(path));
#නැතහොත්
    ආපසු chdir(path);
#endif
}

#නම් defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    void
mch_write(char_u *s, පූර්ණ_සංඛ්‍යා len)
{
    vim_ignored = (පූර්ණ_සංඛ්‍යා)write(1, (var_na *)s, len);
    නම් (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, NULL, NULL);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    static පූර්ණ_සංඛ්‍යා
resize_func(පූර්ණ_සංඛ්‍යා check_only)
{
    නම් (check_only)
	ආපසු do_resize;
    අතරතුර (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(NULL, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    ආපසු FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    පූර්ණ_සංඛ්‍යා
mch_inchar(
    char_u	*buf,
    පූර්ණ_සංඛ්‍යා		maxlen,
    long	wtime,	    // don't use "time", MIPS cannot handle it
    පූර්ණ_සංඛ්‍යා		tb_change_cnt)
{
    ආපසු inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    static void
handle_resize(void)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    පූර්ණ_සංඛ්‍යා
mch_char_avail(void)
{
    ආපසු WaitForChar(0L, NULL, FALSE);
}

#නම් defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    පූර්ණ_සංඛ්‍යා
mch_check_messages(void)
{
    ආපසු WaitForChar(0L, NULL, TRUE);
}
#endif

#නම් defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# නම් defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# නම් defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(පූර්ණ_සංඛ්‍යා special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	sa_nracanaa vm_statistics64	vm_stat;
	natural_t		count = HOST_VM_INFO64_COUNT;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &count);
#  නැතහොත්
	sa_nracanaa vm_statistics	vm_stat;
	natural_t		count = HOST_VM_INFO_COUNT;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &count);
#  endif
	නම් (kret == KERN_SUCCESS)
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
    නම් (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	පූර්ණ_සංඛ්‍යා		mib[2];
	size_t		len = parimaa_na(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  නැතහොත්
	// sysctl() may return 32 bit or 64 bit, accept both
	sa_ngha {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  නැතහොත්
	mib[1] = HW_USERMEM;
#  endif
	නම් (sysctl(mib, 2, &physmem, &len, NULL, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  නැතහොත්
	    නම් (len == parimaa_na(physmem.u64))
		mem = (long_u)physmem.u64;
	    නැතහොත්
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# නම් defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    නම් (mem == 0)
    {
	sa_nracanaa sysinfo sinfo;

	// Linux way of getting amount of RAM available
	නම් (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    අතරතුර (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  නැතහොත්
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    නම් (mem == 0)
    {
	long	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	නම් (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    අතරතුර (shiftright > 0 && (pagesize & 1) == 0)
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
# නම් defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	sa_nracanaa rlimit	rlp;

	නම් (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (parimaa_na(long_u) * 8 - 1))
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

    නම් (mem > 0)
	ආපසු mem >> shiftright;
    ආපසු (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    void
mch_delay(long msec, පූර්ණ_සංඛ්‍යා flags)
{
    tmode_T	old_tmode;
    පූර්ණ_සංඛ්‍යා		call_settmode;
#ifdef FEAT_MZSCHEME
    long	total = msec; // remember original value
#endif

    නම් (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	නම් (call_settmode)
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
	do
	{
	    // if total is large enough, wait by portions in p_mzq
	    නම් (total > p_mzq)
		msec = p_mzq;
	    නැතහොත්
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    sa_nracanaa timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (void)nanosleep(&ts, NULL);
	}
#නැතහොත්
# ifdef HAVE_USLEEP
	අතරතුර (msec >= 1000)
	{
	    usleep((unsigned පූර්ණ_සංඛ්‍යා)(999 * 1000));
	    msec -= 999;
	}
	usleep((unsigned පූර්ණ_සංඛ්‍යා)(msec * 1000));
# නැතහොත්
#  ifndef HAVE_SELECT
	poll(NULL, 0, (පූර්ණ_සංඛ්‍යා)msec);
#  නැතහොත්
	{
	    sa_nracanaa timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, NULL, NULL, NULL, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	අතරතුර (total > 0);
#endif

	නම් (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    නැතහොත්
	WaitForChar(msec, NULL, FALSE);
}

#නම් defined(HAVE_STACK_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_STACK_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
static පූර්ණ_සංඛ්‍යා stack_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    static void
check_stack_growth(var_na *p)
{
    පූර්ණ_සංඛ්‍යා		i;

    stack_grows_downwards = (p > (var_na *)&i);
}
#endif

#නම් defined(HAVE_STACK_LIMIT)
static var_na *stack_limit = NULL;

# නම් defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    static void
get_stack_limit(void)
{
    sa_nracanaa rlimit	rlp;
    පූර්ණ_සංඛ්‍යා			i;
    long		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    නම් (getrlimit(RLIMIT_STACK, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (parimaa_na(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (long)rlp.rlim_cur;
# නම් defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    නම් (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		නම් (lim > (long)size)
		    lim = (long)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	නම් (stack_grows_downwards)
	{
	    stack_limit = (var_na *)((long)&i - (lim / 16L * 15L));
	    නම් (stack_limit >= (var_na *)&i)
		// overflow, set to 1/16 of current stack position
		stack_limit = (var_na *)((long)&i / 16L);
	}
	නැතහොත්
	{
	    stack_limit = (var_na *)((long)&i + (lim / 16L * 15L));
	    නම් (stack_limit <= (var_na *)&i)
		stack_limit = NULL;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    පූර්ණ_සංඛ්‍යා
mch_stackcheck(var_na *p)
{
    නම් (stack_limit == NULL)
	ආපසු OK;

    නම් (stack_grows_downwards)
    {
	නම් (p < stack_limit)
	    ආපසු FAIL;
    }
    නැතහොත් නම් (p > stack_limit)
	ආපසු FAIL;
    ආපසු OK;
}
#endif

#නම් defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
static stack_t sigstk;			// for sigaltstack()
# නැතහොත්
static sa_nracanaa sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
static long පූර්ණ_සංඛ්‍යා get_signal_stack_size(void)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    long පූර්ණ_සංඛ්‍යා size = -1;

    // return size only if sysconf doesn't return an error
    නම් ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	ආපසු size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    ආපසු SIGSTKSZ;
# endif

    // otherwise guess the size
    ආපසු 8000;
}

static var_na *signal_stack;

    static void
init_signal_stack(void)
{
    නම් (signal_stack == NULL)
	ආපසු;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_stack;
#  නැතහොත්
    sigstk.ss_sp = signal_stack;
#  endif
    sigstk.ss_size = get_signal_stack_size();
    sigstk.ss_flags = 0;
    (void)sigaltstack(&sigstk, NULL);
# නැතහොත්
    sigstk.ss_sp = signal_stack;
    නම් (stack_grows_downwards)
	sigstk.ss_sp += get_signal_stack_size() - 1;
    sigstk.ss_onstack = 0;
    (void)sigstack(&sigstk, NULL);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#නම් defined(SIGWINCH)
    static void
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#නම් defined(SIGTSTP)
    static void
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    නම් (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	raise(sigarg);
    }
    නැතහොත්
	got_tstp = TRUE;

# නම් !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#නම් defined(SIGINT)
    static void
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#endif

#නම් defined(SIGUSR1)
    static void
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#නම් defined(SIGPWR)
    static void
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
    static void
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#නම් defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
static JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
static ca_ncala sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
static ca_ncala sig_atomic_t lc_active = FALSE;

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
    static void
mch_startjmp(void)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    static void
mch_endjmp(void)
{
    lc_active = FALSE;
}

    static void
mch_didjmp(void)
{
# නම් defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
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
    static void
deathtrap SIGDEFARG(sigarg)
{
    static පූර්ණ_සංඛ්‍යා	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    පූර්ණ_සංඛ්‍යා		i;
#endif

#නම් defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    නම් (lc_active)
    {
# නම් defined(SIGHASARG)
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
    නම් (in_mch_delay && sigarg == SIGQUIT)
	ආපසු;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    නම් (entered == 0
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
	ආපසු;
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

#නම් 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	නම් (fp)
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
    සඳහා (i = 0; signal_info[i].sig != -1; i++)
	නම් (sigarg == signal_info[i].sig)
	    bha_nga;
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
    නම් (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	නම් (entered >= 4)
	    _exit(8);
	exit(7);
    }
    නම් (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    sprintf((var_na *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#නැතහොත්
    sprintf((var_na *)IObuff, "Vim: Caught deadly signal\r\n");
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
    static void
after_sigcont(void)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#නම් defined(SIGCONT)
static void sigcont_handler SIGPROTOARG;

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
static ca_ncala sig_atomic_t sigcont_received;
static void sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    static void
sigcont_handler SIGDEFARG(sigarg)
{
    නම් (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    නැතහොත්
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

#නම් defined(FEAT_CLIPBOARD)
# නම් defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
static void *clip_star_save = NULL;
static void *clip_plus_save = NULL;
# endif

# නම් defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    static void
loose_clipboard(void)
{
    නම් (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	නම් (clip_star.owned)
	    clip_lose_selection(&clip_star);
	නම් (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	නම් (x11_display != NULL)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# නම් defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    static void
save_clipboard(void)
{
    නම් (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    නම් (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    static void
restore_clipboard(void)
{
    නම් (clip_star_save != NULL)
    {
	නම් (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	නැතහොත්
	    free_register(clip_star_save);
	clip_star_save = NULL;
    }
    නම් (clip_plus_save != NULL)
    {
	නම් (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	නැතහොත්
	    free_register(clip_plus_save);
	clip_plus_save = NULL;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    void
mch_suspend(void)
{
    නම් (ignore_sigtstp)
	ආපසු;

#නම් defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# නම් defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# නම් defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# නම් defined(SIGCONT)
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

	සඳහා (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#නැතහොත්
    suspend_shell();
#endif
}

    void
mch_init(void)
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

    void
set_sigwinch_handler(void)
{
#නම් defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    static void
set_signals(void)
{
#නම් defined(SIGWINCH)
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
#නම් defined(SIGCONT)
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

#නම් defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    නම් (gui.in_use)
	mch_signal(SIGHUP, SIG_IGN);
#endif
}

#නම් defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    static void
catch_int_signal(void)
{
    mch_signal(SIGINT, catch_sigint);
}
#endif

    void
reset_signals(void)
{
    catch_signals(SIG_DFL, SIG_DFL);
#නම් defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#endif
}

    static void
catch_signals(
    void (*func_deadly)(පූර්ණ_සංඛ්‍යා),
    void (*func_other)(පූර්ණ_සංඛ්‍යා))
{
    පූර්ණ_සංඛ්‍යා	    i;

    සඳහා (i = 0; signal_info[i].sig != -1; i++)
    {
	නම් (signal_info[i].deadly)
	{
#නම් defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    sa_nracanaa sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# නම් defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# නැතහොත්
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, NULL);
#නැතහොත්
# නම් defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    sa_nracanaa sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, NULL);
# නැතහොත්
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	නැතහොත් නම් (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#නැතහොත්
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    static void
block_signals(sigset_t *set)
{
    sigset_t	newset;
    පූර්ණ_සංඛ්‍යා		i;

    sigemptyset(&newset);

    සඳහා (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# නම් defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    static void
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, NULL);
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
    පූර්ණ_සංඛ්‍යා
vim_handle_signal(පූර්ණ_සංඛ්‍යා sig)
{
    static පූර්ණ_සංඛ්‍යා got_signal = 0;
    static පූර්ණ_සංඛ්‍යා blocked = TRUE;

    switch (sig)
    {
	pak_sa SIGNAL_BLOCK:   blocked = TRUE;
			     bha_nga;

	pak_sa SIGNAL_UNBLOCK: blocked = FALSE;
			     නම් (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     bha_nga;

	default:	     නම් (!blocked)
				 ආපසු TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     නම් (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     bha_nga;
    }
    ආපසු FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    පූර්ණ_සංඛ්‍යා
mch_check_win(පූර්ණ_සංඛ්‍යා argc UNUSED, var_na **argv UNUSED)
{
    නම් (isatty(1))
	ආපසු OK;
    ආපසු FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    පූර්ණ_සංඛ්‍යා
mch_input_isatty(void)
{
    නම් (isatty(read_cmd_fd))
	ආපසු TRUE;
    ආපසු FALSE;
}

#ifdef FEAT_X11

# නම් defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    static void
xopen_message(long elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#නම් defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

static පූර්ණ_සංඛ්‍යා	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    static පූර්ණ_සංඛ්‍යා
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (var_na *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    නම් (!gui.in_use)
# endif
	msg((var_na *)IObuff);

    ආපසු 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    static පූර්ණ_සංඛ්‍යා
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    ආපසු 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    static පූර්ණ_සංඛ්‍යා
x_connect_to_server(void)
{
    // No point in connecting if we are exiting or dying.
    නම් (exiting || v_dying)
	ආපසු FALSE;

# නම් defined(FEAT_CLIENTSERVER)
    නම් (x_force_connect)
	ආපසු TRUE;
# endif
    නම් (x_no_connect)
	ආපසු FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    නම් (clip_exclude_prog != NULL)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	නම් (regprog_in_use(clip_exclude_prog))
	    ආපසු FALSE;

	නම් (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    ආපසු FALSE;
    }
    ආපසු TRUE;
}

# නම් defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  නම් defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    static පූර්ණ_සංඛ්‍යා
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   නම් defined(VMS) || defined(__CYGWIN__)
    ආපසු 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
static පූර්ණ_සංඛ්‍යා xterm_dpy_retry_count = 0;

    static පූර්ණ_සංඛ්‍යා
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = NULL;
    xterm_dpy_retry_count = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = NULL;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  නම් defined(VMS) || defined(__CYGWIN__)
    ආපසු 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    static void
may_restore_x11_clipboard(void)
{
    // No point in restoring the connecting if we are exiting or dying.
    නම් (!exiting && !v_dying && xterm_dpy_retry_count > 0)
    {
	--xterm_dpy_retry_count;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	නම් (app_context != (XtAppContext)NULL)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)NULL;
	    x11_display = NULL; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    void
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    නම් (eap->arg != NULL && (arglen = STRLEN(eap->arg)) > 0)
    {
	නම් (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (var_na *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == NULL
		    ? (var_na *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

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
    static පූර්ණ_සංඛ්‍යා
test_x11_window(Display *dpy)
{
    පූර්ණ_සංඛ්‍යා			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    නම් (XGetWMName(dpy, x11_window, &text_prop))
	XFree((void *)text_prop.value);
    XSync(dpy, False);
    (void)XSetErrorHandler(old_handler);

    නම් (p_verbose > 0 && got_x_error)
	verb_msg(_("Testing the X display failed"));

    ආපසු (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

static පූර්ණ_සංඛ්‍යා get_x11_thing(පූර්ණ_සංඛ්‍යා get_title, පූර්ණ_සංඛ්‍යා test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    static පූර්ණ_සංඛ්‍යා
get_x11_windis(void)
{
    var_na	    *winid;
    static පූර්ණ_සංඛ්‍යා	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    static පූර්ණ_සංඛ්‍යා	    x11_display_from = XD_NONE;
    static පූර්ණ_සංඛ්‍යා	    did_set_error_handler = FALSE;

    නම් (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(void)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# නම් defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    නම් (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	නම් (x11_display_from == XD_HERE && x11_display != NULL)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	නම් (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    ආපසු OK;
	}
	x11_display = NULL;
	ආපසු FAIL;
    }
    නැතහොත් නම් (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = NULL;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    නම් (!x_connect_to_server())
	ආපසු FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    නම් (x11_window == 0 && (winid = getenv("WINDOWID")) != NULL)
	x11_window = (Window)atol(winid);

# ifdef FEAT_XCLIPBOARD
    නම් (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    නම් (xterm_dpy != NULL && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	නම් (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    නම් (x11_display_from == XD_HERE && x11_display != NULL)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	නම් (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = NULL;
	    x11_display_from = XD_NONE;
	    ආපසු FAIL;
	}
	ආපසු OK;
    }
# endif

    නම් (x11_window == 0 || x11_display == NULL)
	result = -1;

    නම් (result != -1)	    // Have already been here and set this
	ආපසු result;	    // Don't do all these X calls again

    නම් (x11_window != 0 && x11_display == NULL)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	නම් (p_verbose > 0)
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
	x11_display = XOpenDisplay(NULL);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	නම් (p_verbose > 0 && sig_alarm_called)
	    verb_msg(_("Opening the X display timed out"));
# endif
	නම් (x11_display != NULL)
	{
# ifdef ELAPSED_FUNC
	    නම් (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    නම් (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = NULL;
	    }
	    නැතහොත්
		x11_display_from = XD_HERE;
	}
    }
    නම් (x11_window == 0 || x11_display == NULL)
	ආපසු (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (long)x11_window);
# endif

    ආපසු (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    static පූර්ණ_සංඛ්‍යා
get_x11_title(පූර්ණ_සංඛ්‍යා test_only)
{
    ආපසු get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    static පූර්ණ_සංඛ්‍යා
get_x11_icon(පූර්ණ_සංඛ්‍යා test_only)
{
    පූර්ණ_සංඛ්‍යා		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    නම් (oldicon == NULL && !test_only)
    {
	නම් (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	නැතහොත්
	    oldicon = vim_strsave(T_NAME);
    }

    ආපසු retval;
}

    static පූර්ණ_සංඛ්‍යා
get_x11_thing(
    පූර්ණ_සංඛ්‍යා		get_title,	// get title string
    පූර්ණ_සංඛ්‍යා		test_only)
{
    XTextProperty	text_prop;
    පූර්ණ_සංඛ්‍යා			retval = FALSE;
    Status		status;

    නම් (get_x11_windis() != OK)
	ආපසු FALSE;

    // Get window/icon name if any
    නම් (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    නැතහොත්
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
    unsigned පූර්ණ_සංඛ්‍යා    num_children;

    අතරතුර (!status || text_prop.value == NULL)
    {
	නම් (!XQueryTree(x11_display, win, &root, &parent, &children,
		    &num_children))
	    bha_nga;
	නම් (children)
	    XFree((void *)children);
	නම් (parent == root || parent == 0)
	    bha_nga;

	win = parent;
	නම් (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	නැතහොත්
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    නම් (status && text_prop.value != NULL)
    {
	retval = TRUE;
	නම් (!test_only)
	{
	    නම් (get_title)
		vim_free(oldtitle);
	    නැතහොත්
		vim_free(oldicon);
	    නම් (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		නම් (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.value);
		නැතහොත්
		    oldicon = vim_strsave((char_u *)text_prop.value);
	    }
	    නැතහොත්
	    {
		var_na    **cl;
		Status  transform_status;
		පූර්ණ_සංඛ්‍යා	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		නම් (transform_status >= Success && n > 0 && cl[0])
		{
		    නම් (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    නැතහොත්
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		නැතහොත්
		{
		    නම් (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.value);
		    නැතහොත්
			oldicon = vim_strsave((char_u *)text_prop.value);
		}
	    }
	}
	XFree((void *)text_prop.value);
    }
    ආපසු retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# නම් defined(X_HAVE_UTF8_STRING)
#  නම් X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    static void
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    නම් (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (const var_na *)title,
					     NULL, NULL, 0, NULL, NULL, NULL);
    නැතහොත්
# endif
    {
# නම් XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (const var_na *)title,
					     NULL, NULL, 0, NULL, NULL, NULL);
#  නැතහොත්
	XTextProperty	text_prop;
	var_na		*c_title = (var_na *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(void)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     NULL, NULL, 0, NULL, NULL, NULL);
#  endif
# නැතහොත්
	XStoreName(x11_display, x11_window, (var_na *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    static void
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    නම් (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, NULL, (const var_na *)icon,
						   NULL, 0, NULL, NULL, NULL);
    නැතහොත්
# endif
    {
# නම් XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, NULL, (const var_na *)icon,
						   NULL, 0, NULL, NULL, NULL);
#  නැතහොත්
	XTextProperty	text_prop;
	var_na		*c_icon = (var_na *)icon;

	(void)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, NULL, &text_prop,
						   NULL, 0, NULL, NULL, NULL);
#  endif
# නැතහොත්
	XSetIconName(x11_display, x11_window, (var_na *)icon);
# endif
    }
    XFlush(x11_display);
}

#නැතහොත්  // FEAT_X11

    static පූර්ණ_සංඛ්‍යා
get_x11_title(පූර්ණ_සංඛ්‍යා test_only UNUSED)
{
    ආපසු FALSE;
}

    static පූර්ණ_සංඛ්‍යා
get_x11_icon(පූර්ණ_සංඛ්‍යා test_only)
{
    නම් (!test_only)
    {
	නම් (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	නැතහොත්
	    oldicon = vim_strsave(T_NAME);
    }
    ආපසු FALSE;
}

#endif // FEAT_X11

    පූර්ණ_සංඛ්‍යා
mch_can_restore_title(void)
{
    ආපසු get_x11_title(TRUE);
}

    පූර්ණ_සංඛ්‍යා
mch_can_restore_icon(void)
{
    ආපසු get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    void
mch_settitle(char_u *title, char_u *icon)
{
    පූර්ණ_සංඛ්‍යා		type = 0;
    static පූර්ණ_සංඛ්‍යා	recursive = 0;

    නම් (T_NAME == NULL)	    // no terminal name (yet)
	ආපසු;
    නම් (title == NULL && icon == NULL)	    // nothing to do
	ආපසු;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    නම් (recursive)
	ආපසු;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    නම් (get_x11_windis() == OK)
	type = 1;
#endif
#නම් defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    නම් (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    නම් ((type || *T_TS != NUL) && title != NULL)
    {
	නම් (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	නම් (oldtitle == NULL
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (void)get_x11_title(FALSE);

	නම් (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	නැතහොත්
# ifdef FEAT_GUI_GTK
	නම් (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#නම් defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	නැතහොත්
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    නම් ((type || *T_CIS != NUL) && icon != NULL)
    {
	නම් (oldicon == NULL
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	නම් (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	නැතහොත්
# ifdef FEAT_GUI_GTK
	නම් (!gui.in_use)		// don't do this if GTK+ is running
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
    void
mch_restore_title(පූර්ණ_සංඛ්‍යා which)
{
    පූර්ණ_සංඛ්‍යා	do_push_pop = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : NULL;
    char_u *tofree = NULL;
    නම් (title == oldtitle && oldtitle != NULL)
    {
	tofree = vim_strsave(title);
	නම් (tofree != NULL)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : NULL);
    vim_free(tofree);

    නම් (do_push_pop)
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
    පූර්ණ_සංඛ්‍යා
vim_is_xterm(char_u *name)
{
    නම් (name == NULL)
	ආපසු FALSE;
    ආපසු ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#නම් defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    පූර්ණ_සංඛ්‍යා
use_xterm_like_mouse(char_u *name)
{
    ආපසු (name != NULL
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
    පූර්ණ_සංඛ්‍යා
use_xterm_mouse(void)
{
    නම් (ttym_flags == TTYM_SGR)
	ආපසු 4;
    නම් (ttym_flags == TTYM_URXVT)
	ආපසු 3;
    නම් (ttym_flags == TTYM_XTERM2)
	ආපසු 2;
    නම් (ttym_flags == TTYM_XTERM)
	ආපසු 1;
    ආපසු 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    පූර්ණ_සංඛ්‍යා
vim_is_iris(char_u *name)
{
    නම් (name == NULL)
	ආපසු FALSE;
    ආපසු (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#නම් defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    පූර්ණ_සංඛ්‍යා
vim_is_vt300(char_u *name)
{
    නම් (name == NULL)
	ආපසු FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    ආපසු ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != NULL)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    පූර්ණ_සංඛ්‍යා
mch_get_user_name(char_u *s, පූර්ණ_සංඛ්‍යා len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(NULL), len - 1);
    ආපසු OK;
#නැතහොත්
    ආපසු mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    පූර්ණ_සංඛ්‍යා
mch_get_uname(uid_t uid, char_u *s, පූර්ණ_සංඛ්‍යා len)
{
#නම් defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    sa_nracanaa passwd   *pw;

    නම් ((pw = getpwuid(uid)) != NULL
	    && pw->pw_name != NULL && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	ආපසු OK;
    }
#endif
    sprintf((var_na *)s, "%d", (පූර්ණ_සංඛ්‍යා)uid);	    // assumes s is long enough
    ආපසු FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    void
mch_get_host_name(char_u *s, පූර්ණ_සංඛ්‍යා len)
{
    sa_nracanaa utsname vutsname;

    නම් (uname(&vutsname) < 0)
	*s = NUL;
    නැතහොත්
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#නැතහොත් // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    void
mch_get_host_name(char_u *s, පූර්ණ_සංඛ්‍යා len)
{
# ifdef VAXC
    vaxc$gethostname((var_na *)s, len);
# නැතහොත්
    gethostname((var_na *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    long
mch_get_pid(void)
{
    ආපසු (long)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    පූර්ණ_සංඛ්‍යා
mch_process_running(long pid)
{
    // If there is no error the process must be running.
    නම් (kill(pid, 0) == 0)
	ආපසු TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    නම් (errno == ESRCH)
	ආපසු FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    ආපසු TRUE;
}

#නම් !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    static var_na *
strerror(පූර්ණ_සංඛ්‍යා err)
{
    extern පූර්ණ_සංඛ්‍යා	    sys_nerr;
    extern var_na	    *sys_errlist[];
    static var_na	    er[20];

    නම් (err > 0 && err < sys_nerr)
	ආපසු (sys_errlist[err]);
    snprintf(er, parimaa_na(er), "Error %d", err);
    ආපසු er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    පූර්ණ_සංඛ්‍යා
mch_dirname(char_u *buf, පූර්ණ_සංඛ්‍යා len)
{
#නම් defined(USE_GETCWD)
    නම් (getcwd((var_na *)buf, len) == NULL)
    {
	STRCPY(buf, strerror(errno));
	ආපසු FAIL;
    }
    ආපසු OK;
#නැතහොත්
    ආපසු (getwd((var_na *)buf) != NULL ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    පූර්ණ_සංඛ්‍යා
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    පූර්ණ_සංඛ්‍යා		len,
    පූර්ණ_සංඛ්‍යා		force)		// also expand when already absolute path
{
    පූර්ණ_සංඛ්‍යා		buflen = 0;
#ifdef HAVE_FCHDIR
    පූර්ණ_සංඛ්‍යා		fd = -1;
    static පූර්ණ_සංඛ්‍යා	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		retval = OK;
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
# නම් CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# නැතහොත්
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    නම් ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == NULL || p != fname))
    {
	නම් (p == NULL && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	නම් (p != NULL)
	{
	    නම් (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    නම් (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		නම් (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    නම් (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((var_na *)olddir) != 0))
	    {
		p = NULL;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    නැතහොත්
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		නම් (p - fname >= len)
		    retval = FAIL;
		නැතහොත්
		{
		    vim_strncpy(buf, fname, p - fname);
		    නම් (mch_chdir((var_na *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			නම් (mch_isFullName(fname))
			    retval = FAIL;
			නැතහොත්
			    p = NULL;
		    }
		    නැතහොත් නම් (*p == '/')
			fname = p + 1;
		    නැතහොත්
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	නම් (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	නම් (p != NULL)
	{
	    පූර්ණ_සංඛ්‍යා	l;

#ifdef HAVE_FCHDIR
	    නම් (fd >= 0)
	    {
		නම් (p_verbose >= 5)
		{
		    verbose_enter();
		    msg("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    නැතහොත්
#endif
		l = mch_chdir((var_na *)olddir);
	    නම් (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	නම් (fd >= 0)
	    close(fd);
#endif

	buflen = (පූර්ණ_සංඛ්‍යා)STRLEN(buf);
	නම් (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	නැතහොත් නම් (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += parimaa_na(PATHSEP);
	}
#endif
    }

    නම් (buflen == 0)
	buflen = (පූර්ණ_සංඛ්‍යා)STRLEN(buf);

    // Catch file names which are too long.
    නම් (retval == FAIL || (පූර්ණ_සංඛ්‍යා)(buflen + STRLEN(fname)) >= len)
	ආපසු FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    නම් (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    ආපසු OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    පූර්ණ_සංඛ්‍යා
mch_isFullName(char_u *fname)
{
#ifdef VMS
    ආපසු ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((var_na *)fname,':') || strchr((var_na *)fname,'"') ||
	    (strchr((var_na *)fname,'[') && strchr((var_na *)fname,']'))||
	    (strchr((var_na *)fname,'<') && strchr((var_na *)fname,'>'))   );
#නැතහොත්
    ආපසු (*fname == '/' || *fname == '~');
#endif
}

#නම් defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    void
fname_case(
    char_u	*name,
    පූර්ණ_සංඛ්‍යා		len UNUSED)  // buffer size, only used when name gets longer
{
    sa_nracanaa stat st;
    char_u	*slash, *tail;
    size_t	taillen;
    DIR		*dirp;
    sa_nracanaa dirent *dp;

    නම් (mch_lstat((var_na *)name, &st) < 0)
	ආපසු;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    නම් (slash == NULL)
    {
	dirp = opendir(".");
	tail = name;
    }
    නැතහොත්
    {
	*slash = NUL;
	dirp = opendir((var_na *)name);
	*slash = '/';
	tail = slash + 1;
    }

    නම් (dirp == NULL)
	ආපසු;

    taillen = STRLEN(tail);
    අතරතුර ((dp = readdir(dirp)) != NULL)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	නම් (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    sa_nracanaa stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    නම් (mch_lstat((var_na *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		bha_nga;
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
    sa_nracanaa stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    නම් (stat((var_na *)vms_fixfilename(name), &statb))
#නැතහොත්
    නම් (stat((var_na *)name, &statb))
#endif
	ආපසු -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    ආපසු statb.st_mode & ~S_ADDACE;
#නැතහොත්
    ආපසු statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    පූර්ණ_සංඛ්‍යා
mch_setperm(char_u *name, long perm)
{
    ආපසු (chmod((var_na *)
#ifdef VMS
		    vms_fixfilename(name),
#නැතහොත්
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#නම් defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    පූර්ණ_සංඛ්‍යා
mch_fsetperm(පූර්ණ_සංඛ්‍යා fd, long perm)
{
    ආපසු (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#නම් defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
typedef sa_nracanaa vim_acl_solaris_T {
    පූර්ණ_සංඛ්‍යා acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# නම් defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    void
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    නම් (from_file == NULL)
	ආපසු;

    නම් (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    නම් (selinux_enabled <= 0)
	ආපසු;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    var_na *from_context = NULL;
    var_na *to_context = NULL;

    නම් (getfilecon((var_na *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	නම් (errno == EOPNOTSUPP)
	    ආපසු;

	msg_puts(_("\nCould not get security context for "));
	msg_outtrans(from_file);
	msg_putchar('\n');
	ආපසු;
    }
    නම් (getfilecon((var_na *)to_file, &to_context) < 0)
    {
	msg_puts(_("\nCould not get security context for "));
	msg_outtrans(to_file);
	msg_putchar('\n');
	freecon (from_context);
	ආපසු ;
    }
    නම් (strcmp(from_context, to_context) != 0)
    {
	නම් (setfilecon((var_na *)to_file, from_context) < 0)
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

# නම් defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    void
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    static const var_na * const smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    var_na	buffer[SMACK_LABEL_LEN];
    const var_na	*name;
    පූර්ණ_සංඛ්‍යා		index;
    පූර්ණ_සංඛ්‍යා		ret;
    ssize_t	size;

    නම් (from_file == NULL)
	ආපසු;

    size = listxattr((var_na *)from_file, NULL, 0);
    // not supported or no attributes to copy
    නම් (size <= 0)
	ආපසු;

    සඳහා (index = 0 ; index < (පූර්ණ_සංඛ්‍යා)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((var_na*)from_file, name, buffer, parimaa_na(buffer));
	නම් (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((var_na*)to_file, name, buffer, (size_t)size, 0);
	    නම් (ret < 0)
	    {
		vim_snprintf((var_na *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		msg_outtrans(IObuff);
		msg_putchar('\n');
	    }
	}
	නැතහොත්
	{
	    // what reason of not having the attribute value?
	    switch (errno)
	    {
		pak_sa ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    ආපසු; // leave because it isn't useful to continue

		pak_sa ERANGE:
		default:
		    // no enough size OR unexpected error
		     vim_snprintf((var_na *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    msg_puts((var_na *)IObuff);
		    msg_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		pak_sa ENODATA:
		    // no attribute of this name
		    ret = removexattr((var_na*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    bha_nga;
	    }
	}
    }
}
# endif // HAVE_SMACK

# නම් defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    void
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    var_na	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    var_na	*key;
    var_na	*val = NULL;
    var_na	*errmsg = NULL;

    නම් (from_file == NULL)
	ආපසු;

    // get the length of the extended attributes
    size = listxattr((var_na *)from_file, NULL, 0);
    // not supported or no attributes to copy
    නම් (size <= 0)
	ආපසු;
    xattr_buf = (var_na*)alloc(size);
    නම් (xattr_buf == NULL)
	ආපසු;
    size = listxattr((var_na *)from_file, xattr_buf, size);
    tsize = size;

    errno = 0;

    සඳහා (පූර්ණ_සංඛ්‍යා round = 0; round < 2; round++)
    {

	key = xattr_buf;
	නම් (round == 1)
	    size = tsize;

	අතරතුර (size > 0)
	{
	    vallen = getxattr((var_na *)from_file, key,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    නම් (vallen >= 0 && round &&
		setxattr((var_na *)to_file, key, val, vallen, 0) == 0)
		;
	    නැතහොත් නම් (errno)
	    {
		switch (errno)
		{
		    pak_sa E2BIG:
			errmsg = e_xattr_e2big;
			goto error_exit;
		    pak_sa ENOTSUP:
		    pak_sa EACCES:
		    pak_sa EPERM:
			bha_nga;
		    pak_sa ERANGE:
			errmsg = e_xattr_erange;
			goto error_exit;
		    default:
			errmsg = e_xattr_other;
			goto error_exit;
		}
	    }

	    නම් (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(key) + 1;
	    size -= keylen;
	    key += keylen;
	}
	නම් (round)
	    bha_nga;

	val = (var_na*)alloc(max_vallen + 1);
	නම් (val == NULL)
	    goto error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    නම් (errmsg != NULL)
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
    vim_acl_T	ret = NULL;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((var_na *)fname, ACL_TYPE_ACCESS);
# නැතහොත්
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    නම් (acl_get((var_na *)fname, 0, &aclent) < 0)
	ආපසු NULL;
    ret = (vim_acl_T)aclent;
#  නැතහොත්
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = malloc(parimaa_na(vim_acl_solaris_T));
    නම් (aclent == NULL)
	ආපසු NULL;
    නම් ((aclent->acl_cnt = acl((var_na *)fname, GETACLCNT, 0, NULL)) < 0)
    {
	free(aclent);
	ආපසු NULL;
    }
    aclent->acl_entry = malloc(aclent->acl_cnt * parimaa_na(aclent_t));
    නම් (aclent->acl_entry == NULL)
    {
	free(aclent);
	ආපසු NULL;
    }
    නම් (acl((var_na *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	free(aclent->acl_entry);
	free(aclent);
	ආපසු NULL;
    }
    ret = (vim_acl_T)aclent;
#   නැතහොත්
#    නම් defined(HAVE_AIX_ACL)
    පූර්ණ_සංඛ්‍යා		aclsize;
    sa_nracanaa acl *aclent;

    aclsize = parimaa_na(sa_nracanaa acl);
    aclent = malloc(aclsize);
    නම් (aclent == NULL)
	ආපසු NULL;
    නම් (statacl((var_na *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	නම් (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    නම් (aclent == NULL || statacl((var_na *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		free(aclent);
		ආපසු NULL;
	    }
	}
	නැතහොත්
	{
	    free(aclent);
	    ආපසු NULL;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    ආපසු ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    void
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    නම් (aclent == NULL)
	ආපසු;
# ifdef HAVE_POSIX_ACL
    acl_set_file((var_na *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# නැතහොත්
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((var_na *)fname, (acl_t *)aclent);
#  නැතහොත්
#   ifdef HAVE_SOLARIS_ACL
    acl((var_na *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   නැතහොත්
#    ifdef HAVE_AIX_ACL
    chacl((var_na *)fname, aclent, ((sa_nracanaa acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    void
mch_free_acl(vim_acl_T aclent)
{
    නම් (aclent == NULL)
	ආපසු;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# නැතහොත්
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  නැතහොත්
#   ifdef HAVE_SOLARIS_ACL
    free(((vim_acl_solaris_T *)aclent)->acl_entry);
    free(aclent);
#   නැතහොත්
#    ifdef HAVE_AIX_ACL
    free(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    පූර්ණ_සංඛ්‍යා
mch_isdir(char_u *name)
{
    sa_nracanaa stat statb;

    නම් (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ආපසු FALSE;
    නම් (stat((var_na *)name, &statb))
	ආපසු FALSE;
    ආපසු (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    පූර්ණ_සංඛ්‍යා
mch_isrealdir(char_u *name)
{
    sa_nracanaa stat statb;

    නම් (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ආපසු FALSE;
    නම් (mch_lstat((var_na *)name, &statb))
	ආපසු FALSE;
    ආපසු (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    static පූර්ණ_සංඛ්‍යා
executable_file(char_u *name)
{
    sa_nracanaa stat	st;

    නම් (stat((var_na *)name, &st))
	ආපසු 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    පූර්ණ_සංඛ්‍යා vms_executable = 0;
    නම් (S_ISREG(st.st_mode) && mch_access((var_na *)name, X_OK) == 0)
    {
	නම් (strstr(vms_tolower((var_na*)name),".exe") != NULL
		|| strstr(vms_tolower((var_na*)name),".com")!= NULL)
	    vms_executable = 1;
    }
    ආපසු vms_executable;
#නැතහොත්
    ආපසු S_ISREG(st.st_mode) && mch_access((var_na *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    පූර්ණ_සංඛ්‍යා
mch_can_exe(char_u *name, char_u **path, පූර්ණ_සංඛ්‍යා use_path)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    size_t	elen;
    පූර්ණ_සංඛ්‍යා		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    නම් (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	නම් ((use_path || gettail(name) != name) && executable_file(name))
	{
	    නම් (path != NULL)
	    {
		නම් (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		නැතහොත්
		    *path = vim_strsave(name);
	    }
	    ආපසු TRUE;
	}
	ආපසු FALSE;
    }

    p = (char_u *)getenv("PATH");
    නම් (p == NULL || *p == NUL)
	ආපසු -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_end - p) + 2;
    buf = alloc(bufsize);
    නම් (buf == NULL)
	ආපසු -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    සඳහා (;;)
    {
	e = (char_u *)strchr((var_na *)p, ':');
	නම් (e == NULL)
	    e = p_end;
	elen = (size_t)(e - p);
	නම් (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((var_na *)buf, bufsize, "%.*s%s%s",
		(පූර්ණ_සංඛ්‍යා)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	නම් (retval == 1)
	{
	    නම් (path != NULL)
	    {
		නම් (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		නැතහොත්
		    *path = vim_strnsave(buf, buflen);
	    }
	    bha_nga;
	}

	නම් (*e != ':')
	    bha_nga;
	p = e + 1;
    }

    vim_free(buf);
    ආපසු retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    පූර්ණ_සංඛ්‍යා
mch_nodetype(char_u *name)
{
    sa_nracanaa stat	st;

    නම් (stat((var_na *)name, &st))
	ආපසු NODE_NORMAL;
    නම් (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	ආපසු NODE_NORMAL;
    නම් (S_ISBLK(st.st_mode))	// block device isn't writable
	ආපසු NODE_OTHER;
    // Everything else is writable?
    ආපසු NODE_WRITABLE;
}

    void
mch_early_init(void)
{
#ifdef HAVE_CHECK_STACK_GROWTH
    පූර්ණ_සංඛ්‍යා			i;

    check_stack_growth((var_na *)&i);

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
#නම් defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
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

#නම් defined(EXITFREE)
    void
mch_free_mem(void)
{
# නම් defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    නම් (clip_star.owned)
	clip_lose_selection(&clip_star);
    නම් (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# නම් defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    නම් (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    නම් (xterm_dpy != NULL)
	XtCloseDisplay(xterm_dpy);
    නම් (app_context != (XtAppContext)NULL)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = NULL; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# නම් defined(FEAT_X11)
    නම් (x11_display != NULL
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# නම් defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
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
    static void
exit_scroll(void)
{
    නම් (silent_mode)
	ආපසු;
    නම් (newline_on_exit || msg_didout)
    {
	නම් (msg_use_printf())
	{
	    නම් (info_message)
		mch_msg("\n");
	    නැතහොත්
		mch_errmsg("\r\n");
	}
	නැතහොත්
	    out_char('\n');
    }
    නැතහොත් නම් (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	msg_clr_eos_force();		// clear the rest of the display
	windgoto((පූර්ණ_සංඛ්‍යා)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# නම් (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
extern void __gcov_dump(void);
extern void __gcov_reset(void);
#  define __gcov_flush() do { __gcov_dump(); __gcov_reset(); } අතරතුර (0)
# නැතහොත්
extern void __gcov_flush(void);
# endif
#endif

    void
mch_exit(පූර්ණ_සංඛ්‍යා r)
{
    exiting = TRUE;

#නම් defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    නම් (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	නම් (!is_not_a_term())
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
	නම් (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	නම් (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	නම් (full_screen)
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
    නම් (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    නම් (deadly_signal != 0)
	ආපසු;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    exit(r);
}

    static void
may_core_dump(void)
{
    නම් (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    static පූර්ණ_සංඛ්‍යා
get_tty_fd(පූර්ණ_සංඛ්‍යා fd)
{
    පූර්ණ_සංඛ්‍යා		tty_fd = fd;

# නම් defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    නම් (mch_isatty(fd) > 1)
    {
	var_na *name;

	name = ptsname(fd);
	නම් (name == NULL)
	    ආපසු -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	නම් (tty_fd < 0)
	    ආපසු -1;
    }
# endif
    ආපසු tty_fd;
}

    static පූර්ණ_සංඛ්‍යා
mch_tcgetattr(පූර්ණ_සංඛ්‍යා fd, void *term)
{
    පූර්ණ_සංඛ්‍යා		tty_fd;
    පූර්ණ_සංඛ්‍යා		retval = -1;

    tty_fd = get_tty_fd(fd);
    නම් (tty_fd < 0)
	ආපසු -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (sa_nracanaa termios *)term);
#  නැතහොත්
    retval = ioctl(tty_fd, TCGETA, (sa_nracanaa termio *)term);
#  endif
# නැතහොත්
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (sa_nracanaa sgttyb *)term);
# endif
    නම් (tty_fd != fd)
	close(tty_fd);
    ආපසු retval;
}

    void
mch_settmode(tmode_T tmode)
{
    static පූර්ණ_සංඛ්‍යා first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    static sa_nracanaa termios told;
	   sa_nracanaa termios tnew;
#  නැතහොත්
    static sa_nracanaa termio told;
	   sa_nracanaa termio tnew;
#  endif

    නම් (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    නම් (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == NULL || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  නම් defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   නැතහොත්
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    නැතහොත්
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    නැතහොත් නම් (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  නම් defined(HAVE_TERMIOS_H)
    {
	පූර්ණ_සංඛ්‍යා	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	අතරතුර (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  නැතහොත්
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# නැතහොත්
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    static sa_nracanaa sgttyb ttybold;
	   sa_nracanaa sgttyb ttybnew;

    නම් (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    නම් (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    නැතහොත් නම් (tmode == TMODE_SLEEP)
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
    void
get_stty(void)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    නම් (get_tty_info(read_cmd_fd, &info) != OK)
	ආපසු;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    නම් (p != NULL && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(NULL);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    පූර්ණ_සංඛ්‍යා
get_tty_info(පූර්ණ_සංඛ්‍යා fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    sa_nracanaa termios keys;
#  නැතහොත්
    sa_nracanaa termio keys;
#  endif

    නම් (mch_tcgetattr(fd, &keys) != -1)
    {
	info->backspace = keys.c_cc[VERASE];
	info->interrupt = keys.c_cc[VINTR];
	නම් (keys.c_iflag & ICRNL)
	    info->enter = NL;
	නැතහොත්
	    info->enter = CAR;
	නම් (keys.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	නැතහොත්
	    info->nl_does_cr = FALSE;
	ආපසු OK;
    }
# නැතහොත්
    // for "old" tty systems
    sa_nracanaa sgttyb keys;

    නම් (mch_tcgetattr(fd, &keys) != -1)
    {
	info->backspace = keys.sg_erase;
	info->interrupt = keys.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	ආපසු OK;
    }
# endif
    ආපසු FAIL;
}

#endif // VMS

static පූර්ණ_සංඛ්‍යා	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    void
mch_setmouse(පූර්ණ_සංඛ්‍යා on)
{
#ifdef FEAT_BEVAL_TERM
    static පූර්ණ_සංඛ්‍යා	bevalterm_ison = FALSE;
#endif
    පූර්ණ_සංඛ්‍යා		xterm_mouse_vers;

#නම් defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    නම් (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    නම් (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	ආපසු;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    නම් (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    නම් (T_CXM != NULL && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    නැතහොත් නම් (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    නම් (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	නම් (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    නම් (xterm_mouse_vers > 0)
    {
	නම් (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	නැතහොත්	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    නැතහොත් නම් (ttym_flags == TTYM_DEC)
    {
	නම් (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	නැතහොත්	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    නැතහොත්
    {
	නම් (on)
	{
	    නම් (gpm_open())
		mouse_ison = TRUE;
	}
	නැතහොත්
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    නැතහොත්
    {
	නම් (on)
	{
	    නම් (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	නැතහොත්
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    නැතහොත්
    {
	නම් (on)
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
# නැතහොත්
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	නැතහොත්
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    නැතහොත්
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	නම් (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	නැතහොත්
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#නම් defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    void
mch_bevalterm_changed(void)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    void
check_mouse_termcode(void)
{
#ifdef FEAT_MOUSE_XTERM
    නම් (use_xterm_mouse()
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
	නම් (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    නැතහොත්
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    නැතහොත්
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    නැතහොත්
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    නැතහොත්
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    නැතහොත්
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    නම් (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    නැතහොත්
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    නම් (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	නම් (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    නැතහොත්
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    නම් (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	නම් (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    නැතහොත්
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
    පූර්ණ_සංඛ්‍යා
mch_get_shellsize(void)
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
	sa_nracanaa winsize	ws;
	පූර්ණ_සංඛ්‍යා fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	නම් (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	නම් (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(NULL, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# නැතහොත් // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	sa_nracanaa ttysize	ts;
	පූර්ණ_සංඛ්‍යා fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	නම් (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	නම් (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_lines;
#   ifdef FEAT_EVAL
	    ch_log(NULL, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    නම් (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != NULL)
    {
	නම් ((p = (char_u *)getenv("LINES")))
	{
	    rows = atoi((var_na *)p);
# ifdef FEAT_EVAL
	    ch_log(NULL, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	නම් ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((var_na *)p);
# ifdef FEAT_EVAL
	    ch_log(NULL, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    නම් (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(NULL, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    නම් (columns <= 0 || rows <= 0)
	ආපසු FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    ආපසු OK;
}

# නම් defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    void
mch_calc_cell_size(sa_nracanaa cellsize *cs_out)
{
   sa_nracanaa winsize   ws;
   පූර්ණ_සංඛ්‍යා		    retval;
   පූර්ණ_සංඛ්‍යා		    x_cell_size;
   පූර්ණ_සංඛ්‍යා		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(NULL, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   නම් (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	ආපසු;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   නම් (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# නම් defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    පූර්ණ_සංඛ්‍යා
mch_report_winsize(පූර්ණ_සංඛ්‍යා fd, පූර්ණ_සංඛ්‍යා rows, පූර්ණ_සංඛ්‍යා cols)
{
    පූර්ණ_සංඛ්‍යා		tty_fd;
    පූර්ණ_සංඛ්‍යා		retval = -1;

    tty_fd = get_tty_fd(fd);
    නම් (tty_fd < 0)
	ආපසු FAIL;

#  නම් defined(TIOCSWINSZ)
    sa_nracanaa winsize ws;

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
    ch_log(NULL, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    sa_nracanaa ttysize ts;

    ts.ts_cols = cols;
    ts.ts_lines = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(NULL, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    නම් (tty_fd != fd)
	close(tty_fd);
    ආපසු retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    void
mch_set_shellsize(void)
{
    නම් (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((පූර්ණ_සංඛ්‍යා)Rows, (පූර්ණ_සංඛ්‍යා)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    static pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    long delay_msec = 1;

    අතරතුර (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	නම් (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    නම් (++delay_msec > 10)
		delay_msec = 10;
	    continue;
	}
	නම් (wait_pid <= 0
#ifdef ECHILD
		&& errno == ECHILD
#endif
	   )
	    bha_nga;
    }
    ආපසු wait_pid;
}

#නම් !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    static void
set_child_environment(
	long	rows,
	long	columns,
	var_na	*term,
	පූර්ණ_සංඛ්‍යා	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    var_na	envbuf[50];
# නැතහොත්
    static var_na	envbuf_Term[30];
    static var_na	envbuf_Rows[20];
    static var_na	envbuf_Lines[20];
    static var_na	envbuf_Columns[20];
    static var_na	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    static var_na	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    static var_na	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    sprintf((var_na *)envbuf, "%ld", rows);
    setenv("ROWS", (var_na *)envbuf, 1);
    sprintf((var_na *)envbuf, "%ld", rows);
    setenv("LINES", (var_na *)envbuf, 1);
    sprintf((var_na *)envbuf, "%ld", columns);
    setenv("COLUMNS", (var_na *)envbuf, 1);
    sprintf((var_na *)envbuf, "%d", t_colors);
    setenv("COLORS", (var_na *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    නම් (is_terminal)
    {
	sprintf((var_na *)envbuf, "%ld",  (long)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (var_na *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == NULL ? "" : (var_na *)serverName, 1);
#  endif
# නැතහොත්
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, parimaa_na(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, parimaa_na(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_Lines, parimaa_na(envbuf_Lines), "LINES=%ld", rows);
    putenv(envbuf_Lines);
    vim_snprintf(envbuf_Columns, parimaa_na(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, parimaa_na(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    නම් (is_terminal)
    {
	vim_snprintf(envbuf_Version, parimaa_na(envbuf_Version),
			 "VIM_TERMINAL=%ld", (long)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, parimaa_na(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == NULL ? "" : (var_na *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    static void
set_default_child_environment(පූර්ණ_සංඛ්‍යා is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#නම් defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    static void
open_pty(පූර්ණ_සංඛ්‍යා *pty_master_fd, පූර්ණ_සංඛ්‍යා *pty_slave_fd, char_u **name1, char_u **name2)
{
    var_na	*tty_name;

    නම් (name1 != NULL)
	*name1 = NULL;
    නම් (name2 != NULL)
	*name2 = NULL;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    නම් (*pty_master_fd < 0)
	ආපසු;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# නැතහොත්
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    නම් (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    නැතහොත්
    {
	නම් (name1 != NULL)
	    *name1 = vim_strsave((char_u *)tty_name);
	නම් (name2 != NULL)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    static void
may_send_sigint(පූර්ණ_සංඛ්‍යා c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef SIGINT
    නම් (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# නැතහොත්
	kill(0, SIGINT);
# endif
	නම් (wpid > 0)
	    kill(wpid, SIGINT);
    }
#endif
}

#නම් !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    පූර්ණ_සංඛ්‍යා
unix_build_argv(
	char_u *cmd,
	var_na ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    var_na	**argv = NULL;
    පූර්ණ_සංඛ්‍යා		argc;

    *sh_tofree = vim_strsave(p_sh);
    නම් (*sh_tofree == NULL)		// out of memory
	ආපසු FAIL;

    නම් (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	ආපසු FAIL;
    *argvp = argv;

    නම් (cmd != NULL)
    {
	char_u	*s;
	char_u	*p;

	නම් (extra_shell_arg != NULL)
	    argv[argc++] = (var_na *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	නම් (*shcf_tofree == NULL)    // out of memory
	    ආපසු FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	අතරතුර (*p != NUL)
	{
	    argv[argc++] = (var_na *)s;
	    අතරතුර (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (var_na *)cmd;
    }
    argv[argc] = NULL;
    ආපසු OK;
}
#endif

#නම් defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    static පූර්ණ_සංඛ්‍යා
mch_call_shell_terminal(
    char_u	*cmd,
    පූර්ණ_සංඛ්‍යා		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    var_na	**argv = NULL;
    char_u	*tofree1 = NULL;
    char_u	*tofree2 = NULL;
    පූර්ණ_සංඛ්‍යා		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    නම් (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	goto theend;

    init_job_options(&opt);
    ch_log(NULL, "starting terminal for system command '%s'", cmd);
    buf = term_start(NULL, argv, &opt, TERM_START_SYSTEM);
    නම් (buf == NULL)
	goto theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    නම් (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	අතරතුර (term_use_loop())
	{
	    නම් (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		නම් (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    නැතහොත්
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(NULL, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    නම් (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    ආපසු retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    static පූර්ණ_සංඛ්‍යා
mch_call_shell_system(
    char_u	*cmd,
    පූර්ණ_සංඛ්‍යා		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    var_na	*ifn = NULL;
    var_na	*ofn = NULL;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    පූර්ණ_සංඛ්‍යා		x;

    out_flush();

    නම් (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# නම් defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  නම් defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    නම් (cmd == NULL)
	x = system((var_na *)p_sh);
    නැතහොත්
    {
# ifdef VMS
	නම් (ofn = strchr((var_na *)cmd, '>'))
	    *ofn++ = '\0';
	නම් (ifn = strchr((var_na *)cmd, '<'))
	{
	    var_na *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    නම් (p)
		*p = '\0';
	}
	නම් (ofn)
	    x = vms_sys((var_na *)cmd, ofn, ifn);
	නැතහොත්
	    x = system((var_na *)cmd);
# නැතහොත්
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == NULL ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	නම් (newcmd == NULL)
	    x = 0;
	නැතහොත්
	{
	    sprintf((var_na *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == NULL ? "" : (var_na *)extra_shell_arg,
		    (var_na *)p_shcf,
		    (var_na *)cmd);
	    x = system((var_na *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    නම් (emsg_silent)
	;
    නැතහොත් නම් (x == 127)
	msg_puts(_("\nCannot execute shell sh\n"));
    නැතහොත් නම් (x && !(options & SHELL_SILENT))
    {
	msg_puts(_("\nshell returned "));
	msg_outnum((long)x);
	msg_putchar('\n');
    }

    නම් (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# නම් defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    ආපසු x;
}

#නැතහොත් // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    static පූර්ණ_සංඛ්‍යා
mch_call_shell_fork(
    char_u	*cmd,
    පූර්ණ_සංඛ්‍යා		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    පූර්ණ_සංඛ්‍යා		status = -1;
    පූර්ණ_සංඛ්‍යා		retval = -1;
    var_na	**argv = NULL;
    char_u	*tofree1 = NULL;
    char_u	*tofree2 = NULL;
    පූර්ණ_සංඛ්‍යා		i;
    පූර්ණ_සංඛ්‍යා		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    පූර්ණ_සංඛ්‍යා		pty_slave_fd = -1;
# endif
    පූර්ණ_සංඛ්‍යා		fd_toshell[2];		// for pipes
    පූර්ණ_සංඛ්‍යා		fd_fromshell[2];
    පූර්ණ_සංඛ්‍යා		pipe_error = FALSE;
    පූර්ණ_සංඛ්‍යා		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    නම් (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    නම් (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    නම් (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	goto error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    නම් ((options & (SHELL_READ|SHELL_WRITE))
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
	නම් (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, NULL, NULL);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	නම් (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    නම් (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		නම් (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    නම් (pipe_error)
	    {
		msg_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    නම් (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = fork();	// maybe we should use vfork()
	නම් (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    msg_puts(_("\nCannot fork\n"));
	    නම් ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		නම් (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		නැතහොත්				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	නැතහොත් නම් (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    නම් (ch_log_active())
	    {
		ch_log(NULL, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    නම් (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		පූර්ණ_සංඛ්‍යා fd;

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
		නම් (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    නැතහොත් නම් ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		නම් (p_stmp)
		{
		    (void)setsid();
#  නම් defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		නම් (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    නම් (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (var_na *)NULL);
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
		නම් (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    නම් (gui.in_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		නැතහොත්
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
		    නම් (gui.in_use)
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
	නැතහොත්			// parent
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
	    නම් ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		පූර්ණ_සංඛ්‍යා	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		පූර්ණ_සංඛ්‍යා	    ta_len = 0;		// valid bytes in ta_buf[]
		පූර්ණ_සංඛ්‍යා	    len;
		පූර්ණ_සංඛ්‍යා	    p_more_save;
		පූර්ණ_සංඛ්‍යා	    old_State;
		පූර්ණ_සංඛ්‍යා	    c;
		පූර්ණ_සංඛ්‍යා	    toshell_fd;
		පූර්ණ_සංඛ්‍යා	    fromshell_fd;
		garray_T    ga;
		පූර්ණ_සංඛ්‍යා	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		නම් (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		නැතහොත්
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

		නම් ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    නම් ((wpid = fork()) == -1)
		    {
			msg_puts(_("\nCannot fork\n"));
		    }
		    නැතහොත් නම් (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			සඳහා (;;)
			{
			    නම් (lplen == 0)
				len = 0;
			    නැතහොත් නම් (lp[written] == NL)
				// NL -> NUL translation
				len = write(toshell_fd, "", (size_t)1);
			    නැතහොත්
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = write(toshell_fd, (var_na *)lp + written,
					   s == NULL ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    නම් (len == (පූර්ණ_සංඛ්‍යා)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				නම් (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_line_count
						    || curbuf->b_p_eol)))
				    vim_ignored = write(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				නම් (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    bha_nga;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    නැතහොත් නම් (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    නැතහොත් // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		නම් (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		සඳහා (;;)
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
		    නම් (!(options & SHELL_EXPAND)
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
		      නම් (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      නම් (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			නම් (len == 1 && (pty_master_fd < 0 || cmd != NULL))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    නම් (pty_master_fd < 0 && toshell_fd >= 0
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
			නම් (pty_master_fd < 0)
			{
			    සඳහා (i = ta_len; i < ta_len + len; ++i)
			    {
				නම් (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    msg_putchar(ta_buf[i]);
				නැතහොත් නම් (has_mbyte)
				{
				    පූර්ණ_සංඛ්‍යා l = (*mb_ptr2len)(ta_buf + i);

				    msg_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				නැතහොත්
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
			නම් (options & SHELL_WRITE)
			    ta_len = 0;
			නැතහොත් නම් (toshell_fd >= 0)
			{
			    len = write(toshell_fd, (var_na *)ta_buf, (size_t)1);
			    නම් (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    නම් (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# නැතහොත්
			kill(0, SIGINT);
# endif
			නම් (wpid > 0)
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
		    අතරතුර (RealWaitForChar(fromshell_fd, 10L, NULL, NULL))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (size_t)(BUFLEN - buffer_off)
				);
			නම් (len <= 0)		    // end of file or error
			    goto finished;

			noread_cnt = 0;
			නම් (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    සඳහා (i = 0; i < len; ++i)
			    {
				නම් (buffer[i] == NL)
				    append_ga_line(&ga);
				නැතහොත් නම් (buffer[i] == NUL)
				    ga_append(&ga, NL);
				නැතහොත්
				    ga_append(&ga, buffer[i]);
			    }
			}
			නැතහොත් නම් (has_mbyte)
			{
			    පූර්ණ_සංඛ්‍යා		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    සඳහා (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				නම් (l == 0)
				    l = 1;  // NUL byte?
				නැතහොත් නම් (MB_BYTE2LEN(*p) != l)
				    bha_nga;
			    }
			    නම් (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				නම් (len >= 12)
				    ++p;
				නැතහොත්
				{
				    buffer_off = len;
				    continue;
				}
			    }
			    c = *p;
			    *p = NUL;
			    msg_puts((var_na *)buffer);
			    නම් (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				continue;
			    }
			    buffer_off = 0;
			}
			නැතහොත්
			{
			    buffer[len] = NUL;
			    msg_puts((var_na *)buffer);
			}

			windgoto(msg_row, cmdline_col_off + msg_col);
			cursor_on();
			out_flush();
			නම් (got_int)
			    bha_nga;

# ifdef ELAPSED_FUNC
			නම් (wait_pid == 0)
			{
			    long	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    නම් (msec > 2000)
			    {
				noread_cnt = 5;
				bha_nga;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    නම් (wait_pid == pid)
		    {
			නම් (noread_cnt < 5)
			    continue;
			bha_nga;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    නම් ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    නැතහොත්
			wait_pid = 0;

# නම් defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		නම් (options & SHELL_READ)
		{
		    නම් (ga.ga_len > 0)
		    {
			append_ga_line(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    නැතහොත්
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		නම් (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		නම් (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# නම් (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    නැතහොත්
	    {
		long delay_msec = 1;

		නම් (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		සඳහා (;;)
		{
		    නම් (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  නැතහොත්
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    නම් ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			bha_nga;
		    }

#  නම් defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    නම් (++delay_msec > 10)
			delay_msec = 10;
		}

		නම් (tmode == TMODE_RAW)
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
	    නම් (wait_pid != pid)
		(void)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    නම් (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    නම් (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, NULL);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    නම් (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    නම් (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		නම් (retval != 0 && !emsg_silent)
		{
		    නම් (retval == EXEC_FAILED)
		    {
			msg_puts(_("\nCannot execute shell "));
			msg_outtrans(p_sh);
			msg_putchar('\n');
		    }
		    නැතහොත් නම් (!(options & SHELL_SILENT))
		    {
			msg_puts(_("\nshell returned "));
			msg_outnum((long)retval);
			msg_putchar('\n');
		    }
		}
	    }
	    නැතහොත්
		msg_puts(_("\nCommand terminated\n"));
	}
    }

error:
    නම් (!did_settmode)
	නම් (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    ආපසු retval;
}
#endif // USE_SYSTEM

    පූර්ණ_සංඛ්‍යා
mch_call_shell(
    char_u	*cmd,
    පූර්ණ_සංඛ්‍යා		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(NULL, "executing shell command: %s", cmd);
#endif
#නම් defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    නම් (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != NULL
					      && (options & SHELL_SILENT) == 0)
	ආපසු mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    ආපසු mch_call_shell_system(cmd, options);
#නැතහොත්
    ආපසු mch_call_shell_fork(cmd, options);
#endif
}

#නම් defined(FEAT_EVAL)
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
    var_na	**argv,
    char_u	*infile,
    පූර්ණ_සංඛ්‍යා		flags UNUSED,
    පූර්ණ_සංඛ්‍යා		*ret_len)
{
    pid_t	pid;
    පූර්ණ_සංඛ්‍යා		fd_out[2] = {-1, -1};
    පූර්ණ_සංඛ්‍යා		status = -1;
    char_u	*buffer = NULL;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(NULL, "directly executing: %s", argv[0]);

    නම් (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	ආපසු NULL;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# නැතහොත්
    pid = fork();
# endif
    නම් (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	close(fd_out[0]);
	close(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	ආපසු NULL;
    }

    නම් (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	නම් (ch_log_active())
	{
	    ch_log(NULL, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	නම් (infile != NULL)
	{
	    පූර්ණ_සංඛ්‍යා fd_in = open((var_na *)infile, O_RDONLY);
	    නම් (fd_in >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_in);
		close(fd_in);
	    }
	}
	නැතහොත්
	{
	    පූර්ණ_සංඛ්‍යා nullfd = open("/dev/null", O_RDONLY);
	    නම් (nullfd >= 0)
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
    සඳහා (;;)
    {
	var_na	buf[4096];
	පූර්ණ_සංඛ්‍යා	n;

	n = (පූර්ණ_සංඛ්‍යා)read(fd_out[0], buf, parimaa_na(buf));
	නම් (n <= 0)
	    bha_nga;
	නම් (ga_grow(&ga, n) == FAIL)
	    bha_nga;
	mch_memmove((var_na *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_out[0]);

    // Wait for child to finish.
    (void)waitpid(pid, &status, 0);
    නම් (WIFEXITED(status))
	status = WEXITSTATUS(status);
    නැතහොත්
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (long)status);

    නම් (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	නම් (buffer != NULL)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    නම් (ret_len == NULL)
	    {
		පූර්ණ_සංඛ්‍යා	i;

		// Change NUL into SOH, otherwise the string is truncated.
		සඳහා (i = 0; i < ga.ga_len; ++i)
		    නම් (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    නැතහොත්
		*ret_len = ga.ga_len;
	}
    }
    නැතහොත් නම් (ret_len != NULL)
	*ret_len = 0;

    ga_clear(&ga);
    ආපසු buffer;
}
#endif

#නම් defined(FEAT_JOB_CHANNEL)
    void
mch_job_start(var_na **argv, job_T *job, jobopt_T *options, පූර්ණ_සංඛ්‍යා is_terminal)
{
    pid_t	pid;
    පූර්ණ_සංඛ්‍යා		fd_in[2] = {-1, -1};	// for stdin
    පූර්ණ_සංඛ්‍යා		fd_out[2] = {-1, -1};	// for stdout
    පූර්ණ_සංඛ්‍යා		fd_err[2] = {-1, -1};	// for stderr
    පූර්ණ_සංඛ්‍යා		pty_master_fd = -1;
    පූර්ණ_සංඛ්‍යා		pty_slave_fd = -1;
    channel_T	*channel = NULL;
    පූර්ණ_සංඛ්‍යා		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    පූර්ණ_සංඛ්‍යා		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    පූර්ණ_සංඛ්‍යා		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    පූර්ණ_සංඛ්‍යා		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    පූර්ණ_සංඛ්‍යා		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    පූර්ණ_සංඛ්‍යා		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    පූර්ණ_සංඛ්‍යා		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    පූර්ණ_සංඛ්‍යා		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    නම් (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    නම් (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    නම් (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((var_na *)fname, O_RDONLY, 0);
	නම් (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    නැතහොත්
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	නම් (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    goto failed;

    නම් (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((var_na *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	නම් (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    නැතහොත් නම් (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	goto failed;

    නම් (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((var_na *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	නම් (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    goto failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    නැතහොත් නම් (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	goto failed;

    නම් (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	නම් (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    නම් (channel != NULL)
		++channel->ch_refcount;
	}
	නැතහොත්
	    channel = add_channel();
	නම් (channel == NULL)
	    goto failed;
	නම් (job->jv_tty_out != NULL)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = fork();	// maybe we should use vfork()
    නම් (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	goto failed;
    }
    නම් (pid == 0)
    {
	පූර්ණ_සංඛ්‍යා	null_fd = -1;
	පූර්ණ_සංඛ්‍යා	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	නම් (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(void)setsid();
# endif

# ifdef FEAT_TERMINAL
	නම් (options->jo_term_rows > 0)
	{
	    var_na *term = (var_na *)T_NAME;

#  ifdef FEAT_GUI
	    නම් (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    නම් (term == NULL || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		නම් (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		නැතහොත් නම් (t_colors > 16)
		    term = "xterm-color";
		නැතහොත්
		    term = "xterm";
	    }
	    set_child_environment(
		    (long)options->jo_term_rows,
		    (long)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	නැතහොත්
# endif
	    set_default_child_environment(is_terminal);

	නම් (options->jo_env != NULL)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    පූර්ණ_සංඛ්‍යා		todo = (පූර්ණ_සංඛ්‍යා)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_ITEMS(&dict->dv_hashtab, hi, todo)
		නම් (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_key, tv_get_string(item));
		    --todo;
		}
	}

	නම් (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    නම් (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	නම් (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (var_na *)NULL);
# endif
	}

	// set up stdin for the child
	close(0);
	නම් (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	නැතහොත් නම් (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	නැතහොත්
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	close(2);
	නම් (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	නැතහොත් නම් (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	නැතහොත් නම් (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	නැතහොත්
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	නම් (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	නැතහොත් නම් (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	නැතහොත්
	    vim_ignored = dup(fd_out[1]);

	නම් (fd_in[0] >= 0)
	    close(fd_in[0]);
	නම් (fd_in[1] >= 0)
	    close(fd_in[1]);
	නම් (fd_out[0] >= 0)
	    close(fd_out[0]);
	නම් (fd_out[1] >= 0)
	    close(fd_out[1]);
	නම් (fd_err[0] >= 0)
	    close(fd_err[0]);
	නම් (fd_err[1] >= 0)
	    close(fd_err[1]);
	නම් (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	නම් (null_fd >= 0)
	    close(null_fd);

	නම් (options->jo_cwd != NULL && mch_chdir((var_na *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	නම් (stderr_works)
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

    නම් (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    නම් (fd_in[0] >= 0)
	close(fd_in[0]);
    නම් (fd_out[1] >= 0)
	close(fd_out[1]);
    නම් (fd_err[1] >= 0)
	close(fd_err[1]);
    නම් (channel != NULL)
    {
	පූර්ණ_සංඛ්‍යා in_fd = INVALID_FD;
	පූර්ණ_සංඛ්‍යා out_fd = INVALID_FD;
	පූර්ණ_සංඛ්‍යා err_fd = INVALID_FD;

	නම් (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	නම් (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	නම් (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    නම් (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    නැතහොත් නම් (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    නැතහොත්
    {
	නම් (fd_in[1] >= 0)
	    close(fd_in[1]);
	නම් (fd_out[0] >= 0)
	    close(fd_out[0]);
	නම් (fd_err[0] >= 0)
	    close(fd_err[0]);
	නම් (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    ආපසු;

failed:
    channel_unref(channel);
    නම් (fd_in[0] >= 0)
	close(fd_in[0]);
    නම් (fd_in[1] >= 0)
	close(fd_in[1]);
    නම් (fd_out[0] >= 0)
	close(fd_out[0]);
    නම් (fd_out[1] >= 0)
	close(fd_out[1]);
    නම් (fd_err[0] >= 0)
	close(fd_err[0]);
    නම් (fd_err[1] >= 0)
	close(fd_err[1]);
    නම් (pty_master_fd >= 0)
	close(pty_master_fd);
    නම් (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    static char_u *
get_signal_name(පූර්ණ_සංඛ්‍යා sig)
{
    පූර්ණ_සංඛ්‍යා		i;
    char_u	numbuf[NUMBUFLEN];

    නම් (sig == SIGKILL)
	ආපසු vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    සඳහා (i = 0; signal_info[i].sig != -1; i++)
	නම් (sig == signal_info[i].sig)
	    ආපසු strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((var_na *)numbuf, NUMBUFLEN, "%d", sig);
    ආපසු vim_strnsave(numbuf, i);
}

    var_na *
mch_job_status(job_T *job)
{
    පූර්ණ_සංඛ්‍යා		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    නම් (wait_pid == -1)
    {
	පූර්ණ_සංඛ්‍යා waitpid_errno = errno;
	නම් (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    ආපසු "run";

	// process must have exited
	නම් (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	goto return_dead;
    }
    නම් (wait_pid == 0)
	ආපසු "run";
    නම් (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	නම් (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	goto return_dead;
    }
    නම් (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	නම් (job->jv_status < JOB_ENDED && job->jv_termsig != NULL)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	goto return_dead;
    }
    ආපසු "run";

return_dead:
    නම් (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    ආපසු "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    පූර්ණ_සංඛ්‍යා		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    නම් (dont_check_job_ended > 0)
	ආපසු NULL;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    නම් (wait_pid <= 0)
	// no process ended
	ආපසු NULL;
    සඳහා (job = job_list; job != NULL; job = job->jv_next)
    {
	නම් (job->jv_pid == wait_pid)
	{
	    නම් (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    නැතහොත් නම් (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    නම් (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    ආපසු job;
	}
    }
    ආපසු NULL;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    පූර්ණ_සංඛ්‍යා
mch_signal_job(job_T *job, char_u *how)
{
    පූර්ණ_සංඛ්‍යා	    sig = -1;

    නම් (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    නැතහොත් නම් (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    නැතහොත් නම් (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    නැතහොත් නම් (STRCMP(how, "int") == 0)
	sig = SIGINT;
    නැතහොත් නම් (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    නැතහොත් නම් (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    නැතහොත් නම් (SAFE_isdigit(*how))
	sig = atoi((var_na *)how);
    නැතහොත්
	ආපසු FAIL;

    // Never kill ourselves!
    නම් (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-job->jv_pid, sig);
	kill(job->jv_pid, sig);
    }

    ආපසු OK;
}

/*
 * Clear the data related to "job".
 */
    void
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (void)waitpid(job->jv_pid, NULL, WNOHANG);
}
#endif

#නම් defined(FEAT_TERMINAL)
    පූර්ණ_සංඛ්‍යා
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    පූර්ණ_සංඛ්‍යා		pty_master_fd = -1;
    පූර්ණ_සංඛ්‍යා		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    නම් (pty_master_fd < 0 || pty_slave_fd < 0)
	ආපසු FAIL;
    close(pty_slave_fd);

    channel = add_channel();
    නම් (channel == NULL)
    {
	close(pty_master_fd);
	ආපසු FAIL;
    }
    නම් (job->jv_tty_out != NULL)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    ආපසු OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    void
mch_breakcheck(පූර්ණ_සංඛ්‍යා force)
{
    නම් ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, NULL, NULL))
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
    static පූර්ණ_සංඛ්‍යා
WaitForChar(long msec, පූර්ණ_සංඛ්‍යා *interrupted, පූර්ණ_සංඛ්‍යා ignore_input)
{
#ifdef FEAT_TIMERS
    ආපසු ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_input) == OK;
#නැතහොත්
    ආපසු WaitForCharOrMouse(msec, interrupted, ignore_input);
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
    static පූර්ණ_සංඛ්‍යා
WaitForCharOrMouse(long msec, පූර්ණ_සංඛ්‍යා *interrupted, පූර්ණ_සංඛ්‍යා ignore_input)
{
#ifdef FEAT_MOUSE_GPM
    පූර්ණ_සංඛ්‍යා		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    පූර්ණ_සංඛ්‍යා		rest;
#endif
    පූර්ණ_සංඛ්‍යා		avail;

    නම් (!ignore_input && input_available())	    // something in inbuf[]
	ආපසු 1;

#නම් defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    නම් (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	නම් (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#නම් defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    නම් (do_xterm_trace())
	rest = msec;
# endif
    do
    {
# ifdef FEAT_XCLIPBOARD
	නම් (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    නම් (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    නම් (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	නම් (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	නම් (!avail && !gpm_process_wanted)
# නැතහොත්
	avail = RealWaitForChar(read_cmd_fd, msec, NULL, interrupted);
	නම් (!avail)
# endif
	{
	    නම් (!ignore_input && input_available())
		ආපසු 1;
# ifdef FEAT_XCLIPBOARD
	    නම් (rest == 0 || !do_xterm_trace())
# endif
		bha_nga;
	}
    }
    අතරතුර (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#නැතහොත්
    avail = RealWaitForChar(read_cmd_fd, msec, NULL, interrupted);
#endif
    ආපසු avail;
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
    static පූර්ණ_සංඛ්‍යා
RealWaitForChar(පූර්ණ_සංඛ්‍යා fd, long msec, පූර්ණ_සංඛ්‍යා *check_for_gpm UNUSED, පූර්ණ_සංඛ්‍යා *interrupted)
{
    පූර්ණ_සංඛ්‍යා		ret;
    පූර්ණ_සංඛ්‍යා		result;
# නම් defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    static පූර්ණ_සංඛ්‍යා	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    long	start_msec = msec;
    elapsed_T	start_tv;

    නම් (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    නම් (busy)
	ආපසු 0;
# endif

# ifdef MAY_LOOP
    සඳහා (;;)
# endif
    {
# ifdef MAY_LOOP
	පූර්ණ_සංඛ්‍යා		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	පූර්ණ_සංඛ්‍යා		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	පූර්ණ_සංඛ්‍යා		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	sa_nracanaa pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	පූර්ණ_සංඛ්‍යා		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	පූර්ණ_සංඛ්‍යා             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	පූර්ණ_සංඛ්‍යා		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	පූර්ණ_සංඛ්‍යා		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	පූර්ණ_සංඛ්‍යා		xsmp_idx = -1;
#  endif
	පූර්ණ_සංඛ්‍යා		towait = (පූර්ණ_සංඛ්‍යා)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	නම් (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (පූර්ණ_සංඛ්‍යා)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	නම් ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	නම් (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = ConnectionNumber(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	නම් (check_for_gpm != NULL && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	නම් (xsmp_icefd != -1)
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
	නම් (interrupted != NULL)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	නම් (result == 0 && interrupted != NULL && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	නම් (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	නම් (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	නම් (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    නම් (--ret == 0 && !input_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	නම් (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	නම් (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    නම් (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    නැතහොත් නම් (fds[xsmp_idx].revents & POLLHUP)
	    {
		නම් (p_verbose > 0)
		    verb_msg(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    නම් (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	නම් (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# නැතහොත් // HAVE_SELECT

	sa_nracanaa timeval  tv;
	sa_nracanaa timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	static fd_set	rfds, wfds, efds;
	පූර්ණ_සංඛ්‍යා		maxfd;
	long		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	නම් (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	නම් (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	නැතහොත්
	    tvp = NULL;

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
	නම් ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    නම් (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	නම් (xterm_Shell != (Widget)0)
	{
	    FD_SET(ConnectionNumber(xterm_dpy), &rfds);
	    නම් (maxfd < ConnectionNumber(xterm_dpy))
		maxfd = ConnectionNumber(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	නම් (check_for_gpm != NULL && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    නම් (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	නම් (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    නම් (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	නම් (interrupted != NULL)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	නම් (result)
	    --ret;
	නැතහොත් නම් (interrupted != NULL && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	නම් (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    නම් (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    නම් (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(NULL, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    goto select_eintr;
	}
#  endif
#  ifdef __TANDEM
	නම් (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	නම් (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	නම් (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	නම් (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(ConnectionNumber(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    නම් (--ret == 0 && !input_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	නම් (ret > 0 && check_for_gpm != NULL && gpm_flag && gpm_fd >= 0)
	{
	    නම් (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    නැතහොත් නම් (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	නම් (ret > 0 && xsmp_icefd != -1)
	{
	    නම් (FD_ISSET(xsmp_icefd, &efds))
	    {
		නම් (p_verbose > 0)
		    verb_msg(_("XSMP lost ICE connection"));
		xsmp_close();
		නම් (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    නැතහොත් නම් (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		නම් (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	නම් (ret >= 0)
	    (void)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	නම් (finished || msec == 0)
	    bha_nga;

#  නම් defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	නම් (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    bha_nga;
#  endif

	// We're going to loop around again, find out for how long
	නම් (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  නැතහොත්
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    නම් (msec <= 0)
		bha_nga;	// waited long enough
	}
# endif
    }

    ආපසු result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    පූර්ණ_සංඛ්‍යා
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    පූර්ණ_සංඛ්‍යා		flags)		// EW_* flags
{
    ආපසු unix_expandpath(gap, path, 0, flags, FALSE);
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

    පූර්ණ_සංඛ්‍යා
mch_expand_wildcards(
    පූර්ණ_සංඛ්‍යා		   num_pat,
    char_u	 **pat,
    පූර්ණ_සංඛ්‍යා		  *num_file,
    char_u	***file,
    පූර්ණ_සංඛ්‍යා		   flags)	// EW_* flags
{
    පූර්ණ_සංඛ්‍යා		i;
    size_t	len;
    long	llen;
    char_u	*p;
    පූර්ණ_සංඛ්‍යා		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    පූර්ණ_සංඛ්‍යා		j;
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
    පූර්ණ_සංඛ්‍යා		shell_style = STYLE_ECHO;
    පූර්ණ_සංඛ්‍යා		check_spaces;
    static පූර්ණ_සංඛ්‍යා	did_find_nul = FALSE;
    පූර්ණ_සංඛ්‍යා		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    static string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    static string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *num_file = 0;	// default: no files found
    *file = NULL;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    නම් (!have_wildcard(num_pat, pat))
	ආපසු save_patterns(num_pat, pat, num_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    නම් (sandbox != 0 && check_secure())
	ආපසු FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    නම් (secure || restricted)
	සඳහා (i = 0; i < num_pat; ++i)
	    නම් (vim_strchr(pat[i], '`') != NULL
		    && (check_restricted() || check_secure()))
		ආපසු FAIL;

    /*
     * get a name for the temp file
     */
    නම් ((tempname = vim_tempname('o', FALSE)) == NULL)
    {
	emsg(_(e_cant_get_temp_file_name));
	ආපසු FAIL;
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
    නම් (num_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    නැතහොත් නම් ((len = STRLEN(p_sh)) >= 3)
    {
	නම් (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	නැතහොත් නම් (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    නම් (shell_style == STYLE_ECHO)
    {
	නම් (strstr((var_na *)gettail(p_sh), "bash") != NULL)
	    shell_style = STYLE_GLOBSTAR;
	නැතහොත් නම් (strstr((var_na *)gettail(p_sh), "sh") != NULL)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    නම් (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    නැතහොත් නම් (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    සඳහා (i = 0; i < num_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# නැතහොත්
	++len;				// add space
	සඳහා (j = 0; pat[i][j] != NUL; ++j)
	{
	    නම් (vim_strchr(SHELL_SPECIAL, pat[i][j]) != NULL)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    නම් (command == NULL)
    {
	// out of memory
	vim_free(tempname);
	ආපසු FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    නම් (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((var_na *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)strstr((var_na *)command, ")>");
	නම් (p == NULL)
	{
	    ;					    // can't happen ;-)
	}
	නැතහොත්
	{
	    --p;
	    අතරතුර (p > command && VIM_ISWHITE(*p))
		--p;
	    නම් (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    අතරතුර (p > command && VIM_ISWHITE(*p))
		--p;
	    නම් (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    නැතහොත්
    {
	නම් (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((var_na *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	නැතහොත් නම් (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((var_na *)command, len, "print -N >%s", tempname);
	නැතහොත් නම් (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((var_na *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	නැතහොත් නම් (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((var_na *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	නැතහොත්
	    commandlen = vim_snprintf((var_na *)command, len, "echo >%s", tempname);

	සඳහා (i = 0; i < num_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((var_na *)command + commandlen, len, " \"%s\"", pat[i]);
# නැතහොත්
	    පූර්ණ_සංඛ්‍යා intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    සඳහා (j = 0; pat[i][j] != NUL; ++j)
	    {
		නම් (pat[i][j] == '`')
		    intick = !intick;
		නැතහොත් නම් (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    නම් (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != NULL
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		නැතහොත් නම් (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != NULL)
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
    නම් (flags & EW_SILENT)
	show_shell_mess = FALSE;
    නම් (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    නම් (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    නැතහොත් නම් (shell_style == STYLE_GLOB && !have_dollars(num_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    නම් (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = NULL;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    නම් (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	නම් (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    msg_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    නම් (!(flags & EW_SILENT))
# endif
	    {
		msg(_(e_cannot_expand_wildcards));
		msg_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	නම් (shell_style == STYLE_BT)
	    ආපසු FAIL;
	goto notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((var_na *)tempname, READBIN);
    නම් (fd == NULL)
    {
	// Something went wrong, perhaps a file name with a special char.
	නම් (!(flags & EW_SILENT))
	{
	    msg(_(e_cannot_expand_wildcards));
	    msg_start();		// don't overwrite this message
	}
	vim_free(tempname);
	goto notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    නම් (llen < 0)
	// just in case ftell() would fail
	buffer = NULL;
    නැතහොත්
	buffer = alloc(llen + 1);
    නම් (buffer == NULL)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	ආපසු FAIL;
    }
    len = llen;
    i = fread((var_na *)buffer, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    නම් (i != (පූර්ණ_සංඛ්‍යා)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	ආපසු FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    සඳහා (i = 0; i < (පූර්ණ_සංඛ්‍යා)len; ++i)
	නම් (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    නම් (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	සඳහා (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    අතරතුර (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    නැතහොත් නම් (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	සඳහා (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    අතරතුර (*p != '\n' && *p != NUL)
		++p;
	    නම් (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    නැතහොත්
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
	නම් (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    නම් (len && (පූර්ණ_සංඛ්‍යා)STRLEN(buffer) < (පූර්ණ_සංඛ්‍යා)len)
		did_find_nul = TRUE;
	    නැතහොත්
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	නම් (len && buffer[len - 1] == NUL)
	    --len;
	නැතහොත්
	    buffer[len] = NUL;
	i = 0;
	සඳහා (p = buffer; p < buffer + len; ++p)
	    නම් (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	නම් (len)
	    ++i;			// count last entry
    }
    නම් (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	goto notfound;
    }
    *num_file = i;
    *file = ALLOC_MULT(char_u *, i);
    නම් (*file == NULL)
    {
	// out of memory
	vim_free(buffer);
	ආපසු FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    සඳහා (i = 0; i < *num_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	නම් (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    අතරතුර (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    නම් (p == buffer + len)		// last entry
		*p = NUL;
	    නැතහොත්
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	නැතහොත්		// NUL separates
	{
	    අතරතුර (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    සඳහා (j = 0, i = 0; i < *num_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	නම් (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    continue;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	නම් ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    continue;

	// Skip files that are not executable if we check for that.
	නම් (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], NULL, !(flags & EW_SHELLCMD)))
	    continue;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	නම් (p)
	{
	    STRCPY(p, (*file)[i]);
	    නම් (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *num_file = j;

    නම් (*num_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	goto notfound;
    }

    ආපසු OK;

notfound:
    නම් (flags & EW_NOTFOUND)
	ආපසු save_patterns(num_pat, pat, num_file, file);
    ආපසු FAIL;
}

#endif // VMS

    static පූර්ණ_සංඛ්‍යා
save_patterns(
    පූර්ණ_සංඛ්‍යා		num_pat,
    char_u	**pat,
    පූර්ණ_සංඛ්‍යා		*num_file,
    char_u	***file)
{
    පූර්ණ_සංඛ්‍යා		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, num_pat);
    නම් (*file == NULL)
	ආපසු FAIL;
    සඳහා (i = 0; i < num_pat; i++)
    {
	s = vim_strsave(pat[i]);
	නම් (s != NULL)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *num_file = num_pat;
    ආපසු OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    පූර්ණ_සංඛ්‍යා
mch_has_exp_wildcard(char_u *p)
{
    සඳහා ( ; *p; MB_PTR_ADV(p))
    {
	නම් (*p == '\\' && p[1] != NUL)
	    ++p;
	නැතහොත්
	    නම් (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#නැතහොත්
				    "*?[{'"
#endif
						, *p) != NULL)
	    ආපසු TRUE;
    }
    ආපසු FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    පූර්ණ_සංඛ්‍යා
mch_has_wildcard(char_u *p)
{
    සඳහා ( ; *p; MB_PTR_ADV(p))
    {
	නම් (*p == '\\' && p[1] != NUL)
	    ++p;
	නැතහොත්
	    නම් (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#නැතහොත්
				    "*?[{`'$"
#endif
						, *p) != NULL
		|| (*p == '~' && p[1] != NUL))
	    ආපසු TRUE;
    }
    ආපසු FALSE;
}

    static පූර්ණ_සංඛ්‍යා
have_wildcard(පූර්ණ_සංඛ්‍යා num, char_u **file)
{
    පූර්ණ_සංඛ්‍යා	    i;

    සඳහා (i = 0; i < num; i++)
	නම් (mch_has_wildcard(file[i]))
	    ආපසු 1;
    ආපසු 0;
}

    static පූර්ණ_සංඛ්‍යා
have_dollars(පූර්ණ_සංඛ්‍යා num, char_u **file)
{
    පූර්ණ_සංඛ්‍යා	    i;

    සඳහා (i = 0; i < num; i++)
	නම් (vim_strchr(file[i], '$') != NULL)
	    ආපසු TRUE;
    ආපසු FALSE;
}

#නම් !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    පූර්ණ_සංඛ්‍යා
mch_rename(const var_na *src, const var_na *dest)
{
    sa_nracanaa stat	    st;

    නම් (stat(dest, &st) >= 0)	    // fail if destination exists
	ආපසු -1;
    නම් (link(src, dest) != 0)	    // link file to new name
	ආපසු -1;
    නම් (mch_remove(src) == 0)	    // delete link to old name
	ආපසු 0;
    ආපසු -1;
}
#endif // !HAVE_RENAME

#නම් defined(FEAT_MOUSE_GPM)
# නම් defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    static පූර්ණ_සංඛ්‍යා
load_libgpm(void)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    නම් (libgpm_hinst == NULL)
    {
	නම් (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	ආපසු FAIL;
    }

    නම් (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == NULL
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == NULL
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == NULL
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == NULL
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == NULL
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = NULL;
	dll_gpm_flag = NULL;
	dll_gpm_fd   = NULL;
	ආපසු FAIL;
    }
    ආපසු OK;
}

    පූර්ණ_සංඛ්‍යා
gpm_available(void)
{
    ආපසු libgpm_hinst != NULL || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    static පූර්ණ_සංඛ්‍යා
gpm_open(void)
{
    static Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    නම් (!gpm_available())
	ආපසු 0;
# endif

    නම් (gpm_flag)
	ආපසු 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    නම් (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	ආපසු 1; // succeed
    }
    නම් (gpm_fd == -2)
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
    ආපසු 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    පූර්ණ_සංඛ්‍යා
gpm_enabled(void)
{
    ආපසු gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    static void
gpm_close(void)
{
    නම් (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    static පූර්ණ_සංඛ්‍යා
mch_gpm_process(void)
{
    පූර්ණ_සංඛ්‍යා			button;
    static Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    පූර්ණ_සංඛ්‍යා			row,col;
    unsigned var_na	buttons_mask;
    unsigned var_na	gpm_modifiers;
    static unsigned var_na old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    නම් (hold_gui_events)
	ආපසු 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    switch (GPM_BARE_EVENTS(gpm_event.type))
    {
	pak_sa GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    bha_nga;
	pak_sa GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    switch (buttons_mask)
	    {
		pak_sa GPM_B_LEFT:
		    button = MOUSE_LEFT;
		    bha_nga;
		pak_sa GPM_B_MIDDLE:
		    button = MOUSE_MIDDLE;
		    bha_nga;
		pak_sa GPM_B_RIGHT:
		    button = MOUSE_RIGHT;
		    bha_nga;
		default:
		    ආපසු 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_NUM_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    bha_nga;
	pak_sa GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    bha_nga;
	default:
	    ආපසු 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    නම් (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    නම් (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    නම් (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_input_buf(string, 6);
    ආපසු 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    static පූර්ණ_සංඛ්‍යා
sysmouse_open(void)
{
    sa_nracanaa mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.sa_nkeeta = SIGUSR2;
    නම් (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	ආපසු FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    ආපසු OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    static void
sysmouse_close(void)
{
    sa_nracanaa mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.sa_nkeeta = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    static void
sig_sysmouse SIGDEFARG(sigarg)
{
    sa_nracanaa mouse_info	mouse;
    sa_nracanaa video_info	video;
    char_u		string[6];
    පූර්ණ_සංඛ්‍යා			row, col;
    පූර්ණ_සංඛ්‍යා			button;
    පූර්ණ_සංඛ්‍යා			buttons;
    static පූර්ණ_සංඛ්‍යා		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    නම් (hold_gui_events)
	ආපසු;
# endif

    mouse.operation = MOUSE_GETINFO;
    නම් (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	නම් (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	නැතහොත්
	{
	    switch (buttons)
	    {
		pak_sa 0:
		    button = MOUSE_RELEASE;
		    bha_nga;
		pak_sa 1:
		    button = MOUSE_LEFT;
		    bha_nga;
		pak_sa 2:
		    button = MOUSE_MIDDLE;
		    bha_nga;
		pak_sa 4:
		    button = MOUSE_RIGHT;
		    bha_nga;
		default:
		    ආපසු;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_input_buf(string, 6);
    }
    ආපසු;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    පූර්ණ_සංඛ්‍යා
mch_get_random(char_u *buf, පූර්ණ_සංඛ්‍යා len)
{
    static පූර්ණ_සංඛ්‍යා dev_urandom_state = NOTDONE;

    නම් (dev_urandom_state == FAIL)
	ආපසු FAIL;

    පූර්ණ_සංඛ්‍යා fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    නම් (fd == -1)
	dev_urandom_state = FAIL;
    නැතහොත් නම් (read(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    නැතහොත්
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    ආපසු dev_urandom_state;
}

#නම් defined(FEAT_LIBCALL)
typedef char_u * (*STRPROCSTR)(char_u *);
typedef char_u * (*INTPROCSTR)(පූර්ණ_සංඛ්‍යා);
typedef පූර්ණ_සංඛ්‍යා (*STRPROCINT)(char_u *);
typedef පූර්ණ_සංඛ්‍යා (*INTPROCINT)(පූර්ණ_සංඛ්‍යා);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    පූර්ණ_සංඛ්‍යා
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    පූර්ණ_සංඛ්‍යා		argint,
    char_u	**string_result, // NULL when using number_result
    පූර්ණ_සංඛ්‍යා		*number_result)
{
# නම් defined(USE_DLOPEN)
    void	*hinstLib;
    var_na	*dlerr = NULL;
# නැතහොත්
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = NULL;
    පූර්ණ_සංඛ්‍යා		retval_int = 0;
    පූර්ණ_සංඛ්‍යා		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# නම් defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (void)dlerror();

    hinstLib = dlopen((var_na *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    නම් (hinstLib == NULL)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	නම් (dlerr != NULL)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# නැතහොත්
    hinstLib = shl_load((const var_na*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    නම් (hinstLib != NULL)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	නම් (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  නම් defined(USE_DLOPEN)
	    dlerr = NULL;
#  endif
	    mch_didjmp();
	}
	නැතහොත්
# endif
	{
	    retval_str = NULL;
	    retval_int = 0;

	    නම් (argstring != NULL)
	    {
# නම් defined(USE_DLOPEN)
		*(void **)(&ProcAdd) = dlsym(hinstLib, (const var_na *)funcname);
		dlerr = dlerror();
# නැතහොත්
		නම් (shl_findsym(&hinstLib, (const var_na *)funcname,
					TYPE_PROCEDURE, (void *)&ProcAdd) < 0)
		    ProcAdd = NULL;
# endif
		නම් ((success = (ProcAdd != NULL
# නම් defined(USE_DLOPEN)
			    && dlerr == NULL
# endif
			    )))
		{
		    නම් (string_result == NULL)
			retval_int = ((STRPROCINT)(void *)ProcAdd)(argstring);
		    නැතහොත්
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    නැතහොත්
	    {
# නම් defined(USE_DLOPEN)
		*(void **)(&ProcAddI) = dlsym(hinstLib, (const var_na *)funcname);
		dlerr = dlerror();
# නැතහොත්
		නම් (shl_findsym(&hinstLib, (const var_na *)funcname,
				       TYPE_PROCEDURE, (void *)&ProcAddI) < 0)
		    ProcAddI = NULL;
# endif
		නම් ((success = (ProcAddI != NULL
# නම් defined(USE_DLOPEN)
			    && dlerr == NULL
# endif
			    )))
		{
		    නම් (string_result == NULL)
			retval_int = ((INTPROCINT)(void *)ProcAddI)(argint);
		    නැතහොත්
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    නම් (string_result == NULL)
		*number_result = retval_int;
	    නැතහොත් නම් (retval_str != NULL
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	නම් (lc_signal != 0)
	{
	    පූර්ණ_සංඛ්‍යා i;

	    // try to find the name of this signal
	    සඳහා (i = 0; signal_info[i].sig != -1; i++)
		නම් (lc_signal == signal_info[i].sig)
		    bha_nga;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# නම් defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	නම් (dlerr != NULL)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(void)dlclose(hinstLib);
# නැතහොත්
	(void)shl_unload(hinstLib);
# endif
    }

    නම් (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ආපසු FAIL;
    }

    ආපසු OK;
}
#endif

#නම් defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
static පූර්ණ_සංඛ්‍යා	xterm_trace = -1;	// default: disabled
static පූර්ණ_සංඛ්‍යා	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    void
setup_term_clip(void)
{
    පූර්ණ_සංඛ්‍යා		z = 0;
    var_na	*strp = "";
    Widget	AppShell;

    නම් (!x_connect_to_server())
	ආපසු;

    open_app_context();
    නම් (app_context != NULL && xterm_Shell == (Widget)0)
    {
	පූර්ණ_සංඛ්‍යා (*oldhandler)(Display*, XErrorEvent*);
# නම් defined(USING_SETJMP)
	පූර්ණ_සංඛ්‍යා (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	නම් (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# නම් defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	නම් (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = NULL;
	}
	නැතහොත්
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", NULL, 0, &z, &strp);
	    නම් (xterm_dpy != NULL)
		xterm_dpy_retry_count = 0;
# නම් defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# නම් defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(void)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(void)XSetErrorHandler(oldhandler);

	නම් (xterm_dpy == NULL)
	{
	    නම් (p_verbose > 0)
		verb_msg(_("Opening the X display failed"));
	    ආපසු;
	}

	// Catch terminating error of the X server connection.
	(void)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	නම් (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		NULL);
	නම් (AppShell == (Widget)0)
	    ආපසු;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		topLevelShellWidgetClass, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		NULL);
	නම් (xterm_Shell == (Widget)0)
	    ආපසු;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	නම් (x11_display == NULL)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    නම් (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	නම් (x11_window == 0 && (strp = getenv("WINDOWID")) != NULL)
	    x11_window = (Window)atol(strp);
	// Check if $WINDOWID is valid.
	නම් (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	නම් (x11_window != 0)
	    xterm_trace = 0;
    }
}

    void
start_xterm_trace(පූර්ණ_සංඛ්‍යා button)
{
    නම් (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	ආපසු;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    void
stop_xterm_trace(void)
{
    නම් (xterm_trace < 0)
	ආපසු;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    static පූර්ණ_සංඛ්‍යා
do_xterm_trace(void)
{
    Window		root, child;
    පූර්ණ_සංඛ්‍යා			root_x, root_y;
    පූර්ණ_සංඛ්‍යා			win_x, win_y;
    පූර්ණ_සංඛ්‍යා			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    long		got_hints;
    static char_u	*mouse_code = NULL;
    static size_t	mouse_codelen = 0;
    static char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    static පූර්ණ_සංඛ්‍යා		prev_row = 0, prev_col = 0;
    static XSizeHints	xterm_hints;

    නම් (xterm_trace <= 0)
	ආපසු FALSE;

    නම් (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	නම් (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    ආපසු FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	නම් (mouse_code != NULL)
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
	නම් (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	නම් (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	ආපසු TRUE;
    }

    නම් (mouse_code == NULL || mouse_codelen > 45)
    {
	xterm_trace = 0;
	ආපසු FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &root, &child, &root_x, &root_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    නම් (row == prev_row && col == prev_col)
	ආපසු TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_input_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    ආපසු TRUE;
}

# නම් defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    void
clear_xterm_clip(void)
{
    නම් (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    නම් (xterm_dpy != NULL)
    {
#  නම් 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	නම් (x11_display == xterm_dpy)
	    x11_display = NULL;
	xterm_dpy = NULL;
    }
#  නම් 0
    නම් (app_context != (XtAppContext)NULL)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)NULL;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    static void
clip_update(void)
{
# ifdef FEAT_GUI
    නම් (gui.in_use)
	gui_mch_update();
    නැතහොත්
# endif
    නම් (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    void
xterm_update(void)
{
    XEvent event;

    සඳහා (;;)
    {
	XtInputMask mask = XtAppPending(app_context);

	නම් (mask == 0 || vim_is_input_buf_full())
	    bha_nga;

	නම් (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		නම් (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == PropertyNewValue)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	නැතහොත්
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    පූර්ණ_සංඛ්‍යා
clip_xterm_own_selection(Clipboard_T *cbd)
{
    නම් (xterm_Shell != (Widget)0)
	ආපසු clip_x11_own_selection(xterm_Shell, cbd);
    ආපසු FAIL;
}

    void
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    නම් (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    void
clip_xterm_request_selection(Clipboard_T *cbd)
{
    නම් (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    void
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#නම් defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# නම් defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    static void
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    පූර්ණ_සංඛ්‍යා		save_cmod_flags;
    පූර්ණ_සංඛ්‍යා		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    නම් (check_changed_any(FALSE, FALSE))
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
    නම් (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    static void
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    පූර්ණ_සංඛ්‍යා		save_type UNUSED,
    Bool	shutdown,
    පූර්ණ_සංඛ්‍යා		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    නම් (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    නම් (p_verbose > 0)
	verb_msg(_("XSMP handling save-yourself request"));

# නම් defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    නම් (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    නැතහොත්
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
    static void
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    static void
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
    static void
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    නම් (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    static void
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    නම් (opening)
    {
	xsmp_icefd = IceConnectionNumber(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, NULL);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    පූර්ණ_සංඛ්‍යා
xsmp_handle_requests(void)
{
    Bool rep;

    නම් (IceProcessMessages(xsmp.iceconn, NULL, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	නම් (p_verbose > 0)
	    verb_msg(_("XSMP lost ICE connection"));
	xsmp_close();
	ආපසු FAIL;
    }
    නැතහොත්
	ආපසු OK;
}

static පූර්ණ_සංඛ්‍යා dummy;

// Set up X Session Management Protocol
    void
xsmp_init(void)
{
    var_na		errorstring[80];
    SmcCallbacks	smcallbacks;
# නම් 0
    SmPropValue		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    නම් (p_verbose > 0)
	verb_msg(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = NULL;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = NULL;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = NULL;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = NULL;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    නම් (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	නම් (p_verbose > 0)
	    verb_msg(_("XSMP ICE connection watch failed"));
	ආපසු;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    NULL,
	    NULL,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    NULL,
	    &xsmp.clientid,
	    parimaa_na(errorstring) - 1,
	    errorstring);
    නම් (xsmp.smcconn == NULL)
    {
	නම් (p_verbose > 0)
	{
	    var_na errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[parimaa_na(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, parimaa_na(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_msg(errorreport);
	}
	ආපසු;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# නම් 0
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
    void
xsmp_close(void)
{
    නම් (xsmp_icefd == -1)
	ආපසු;

    SmcCloseConnection(xsmp.smcconn, 0, NULL);
    නම් (xsmp.clientid != NULL)
	free(xsmp.clientid);
    xsmp.clientid = NULL;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#නම් defined(FEAT_RELTIME)
# නම් defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
static ca_ncala sig_atomic_t timeout_flag = FALSE;
static timer_t		     timer_id;
static පූර්ණ_සංඛ්‍යා		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    static void
set_flag(sa_ngha sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    void
stop_timeout(void)
{
    static sa_nracanaa itimerspec disarm = {{0, 0}, {0, 0}};

    නම් (timer_created)
    {
	පූර්ණ_සංඛ්‍යා ret = timer_settime(timer_id, 0, &disarm, NULL);

	නම් (ret < 0)
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
    ca_ncala sig_atomic_t *
start_timeout(long msec)
{
    sa_nracanaa itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    පූර්ණ_සංඛ්‍යා ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    නම් (!timer_created)
    {
	sa_nracanaa sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	නම් (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), strerror(errno));
	    ආපසු &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(NULL, "setting timeout timer to %d sec %ld nsec",
	       (පූර්ණ_සංඛ්‍යා)interval.it_value.tv_sec, (long)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, NULL);
    නම් (ret < 0)
	semsg(_(e_could_not_set_timeout_str), strerror(errno));

    ආපසු &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    void
delete_timer(void)
{
    නම් (!timer_created)
	ආපසු;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# නැතහොත් // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
static sa_nracanaa sigaction		prev_sigaction;
static ca_ncala sig_atomic_t	timeout_flag	     = FALSE;
static පූර්ණ_සංඛ්‍යා			timer_active	     = FALSE;
static පූර්ණ_සංඛ්‍යා			timer_handler_active = FALSE;
static ca_ncala sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    static void
set_flag SIGDEFARG(sigarg)
{
    නම් (alarm_pending)
	alarm_pending = FALSE;
    නැතහොත්
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    void
stop_timeout(void)
{
    static sa_nracanaa itimerval disarm = {{0, 0}, {0, 0}};
    පූර්ණ_සංඛ්‍යා			    ret;

    නම් (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, NULL);
	නම් (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    නම් (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, NULL);
	නම් (ret < 0)
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
    ca_ncala sig_atomic_t *
start_timeout(long msec)
{
    sa_nracanaa itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    sa_nracanaa sigaction	handle_alarm;
    පූර්ණ_සංඛ්‍යා			ret;
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
    නම් (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, NULL);
    }
    නම් (unlikely(ret != 0 || alarm_pending < 0))
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
    නම් (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), strerror(errno));
	ආපසු &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, NULL);
    නම් (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), strerror(errno));
	stop_timeout();
	ආපසු &timeout_flag;
    }

    timer_active = TRUE;
    ආපසු &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
