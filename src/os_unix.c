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

#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
# include "if_mzsch.h"
#جيڪڏهن_پورو

#ضمن "os_unixx.h"	    // unix includes for os_unix.c only

#جيڪڏهن_وصف_ٿيل USE_XSMP
# include <X11/SM/SMlib.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SELINUX
# include <selinux/selinux.h>
ساكن صحيح selinux_enabled = -1;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_XATTR
# include <sys/xattr.h>
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_سراسري
#  include <windows.h>
#  include "winclip.pro"
# endif
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_GPM

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
# define KG_سرڪايو	0
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ساكن صحيح sysmouse_open(فراغ);
ساكن فراغ sysmouse_close(فراغ);
ساكن فراغ sig_sysmouse SIGPROTOARG;
#جيڪڏهن_پورو

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#إذا defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#جيڪڏهن_پورو

#إذا defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_X11
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

دري	    x11_دري = 0;
Display	    *x11_display = عدم;
#جيڪڏهن_پورو

ساكن صحيح ignore_sigtstp = FALSE;

ساكن صحيح get_x11_title(صحيح);

ساكن char_u	*oldtitle = عدم;
ساكن متطاير sig_atomic_t oldtitle_outdated = FALSE;
ساكن صحيح	unix_did_سيٽ_title = FALSE;
ساكن char_u	*oldicon = عدم;
ساكن صحيح	did_سيٽ_icon = FALSE;

ساكن فراغ may_core_dump(فراغ);

تعريفنوع صحيح waitstatus;
ساكن صحيح  WaitForChar(ڊگهو msec, صحيح *interrupted, صحيح ignore_داخلا);
ساكن صحيح  WaitForCharOrMouse(ڊگهو msec, صحيح *interrupted, صحيح ignore_داخلا);
#جيڪڏهن_وصف_ٿيل VMS
صحيح  RealWaitForChar(صحيح, ڊگهو, صحيح *, صحيح *interrupted);
#وإلا
ساكن صحيح  RealWaitForChar(صحيح, ڊگهو, صحيح *, صحيح *interrupted);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_XCLIPBOARD
ساكن صحيح do_xterm_trace(فراغ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#جيڪڏهن_پورو

ساكن فراغ handle_resize(فراغ);

#إذا defined(SIGWINCH)
ساكن فراغ sig_winch SIGPROTOARG;
#جيڪڏهن_پورو
#إذا defined(SIGTSTP)
ساكن فراغ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ساكن متطاير sig_atomic_t اندريون_mch_suspend = FALSE;
#جيڪڏهن_پورو
#إذا defined(SIGINT)
ساكن فراغ catch_sigint SIGPROTOARG;
#جيڪڏهن_پورو
#إذا defined(SIGUSR1)
ساكن فراغ catch_sigusr1 SIGPROTOARG;
#جيڪڏهن_پورو
#إذا defined(SIGPWR)
ساكن فراغ catch_sigpwr SIGPROTOARG;
#جيڪڏهن_پورو
#إذا defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define سيٽ_SIG_ALARM
ساكن فراغ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ساكن متطاير sig_atomic_t sig_alarm_called;
#جيڪڏهن_پورو
ساكن فراغ deathtrap SIGPROTOARG;

ساكن فراغ catch_int_signal(فراغ);
ساكن فراغ سيٽ_سگنل(فراغ);
ساكن فراغ catch_سگنل(فراغ (*func_deadly)(صحيح), فراغ (*func_other)(صحيح));
#جيڪڏهن_وصف_ٿيل HAVE_SIGPROCMASK
# define SIGSET_DECL(سيٽ_2)	sigset_t سيٽ_2;
# define بلاڪ_سگنل(سيٽ_2)	بلاڪ_سگنل_2(سيٽ_2)
# define UNBLOCK_سگنل(سيٽ_2)	unblock_سگنل(سيٽ_2)
#وإلا
# define SIGSET_DECL(سيٽ_2)
# define بلاڪ_سگنل(سيٽ_2)	افعل { /**/ } طالما (0)
# define UNBLOCK_سگنل(سيٽ_2)	افعل { /**/ } طالما (0)
#جيڪڏهن_پورو
ساكن صحيح  have_wildcard(صحيح, char_u **);
ساكن صحيح  have_dollars(صحيح, char_u **);

ساكن صحيح save_patterns(صحيح انگ_pat, char_u **pat, صحيح *انگ_file, char_u ***file);

#جيڪڏهن_وصف_نه_ٿيل SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#جيڪڏهن_پورو
#جيڪڏهن_وصف_نه_ٿيل SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#جيڪڏهن_پورو

// volatile because it is used in signal handler sig_winch().
ساكن متطاير sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ساكن متطاير sig_atomic_t got_tstp = FALSE;
ساكن char_u	*extra_shell_arg = عدم;
ساكن صحيح	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ساكن متطاير sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ساكن متطاير sig_atomic_t اندريون_mch_delay = FALSE; // sleeping in mch_delay()

#إذا defined(FEAT_ڪم_CHANNEL) && !defined(USE_SYSTEM)
ساكن صحيح dont_check_ڪم_ended = 0;
#جيڪڏهن_پورو

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ساكن tmode_T mch_cur_tmode = TMODE_COOK;

#جيڪڏهن_وصف_ٿيل USE_XSMP
تعريفنوع بنية
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    حرف *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ساكن xsmp_config_T xsmp;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#جيڪڏهن_پورو

ساكن بنية signalinfo
{
    صحيح	    sig;	// Signal number, eg. SIGSEGV etc
    حرف    *name;	// Signal name (not char_u!).
    حرف    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#جيڪڏهن_وصف_ٿيل SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGILL
    {SIGILL,	    "ILL",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#جيڪڏهن_پورو
#إذا defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#جيڪڏهن_پورو
#إذا defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#جيڪڏهن_پورو
#إذا defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#جيڪڏهن_پورو
#إذا defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGINT
    {SIGINT,	    "INT",	FALSE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#جيڪڏهن_پورو
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(صحيح sig, sighandler_T func)
{
#إذا defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    بنية sigaction	sa, old;
    sigset_t		curset;
    صحيح			blocked;

    إذا (sigprocmask(SIG_BLOCK, عدم, &curset) == -1)
	أرجع SIG_ERR;

    blocked = sigismember(&curset, sig);

    إذا (func == SIG_HOLD)
    {
	إذا (blocked)
	    أرجع SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	إذا (sigaction(sig, عدم, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, عدم) == -1)
	    أرجع SIG_ERR;
	أرجع old.sa_handler;
    }

    إذا (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	إذا (sigprocmask(SIG_UNBLOCK, &curset, عدم) == -1)
	    أرجع SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# وإلا
    sa.sa_flags = 0;
# endif
    إذا (sigaction(sig, &sa, &old) == -1)
	أرجع SIG_ERR;
    أرجع blocked ? SIG_HOLD: old.sa_handler;
#وإلاإذا defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    أرجع sigset(sig, func);
#وإلا
    // Oldest and most compatible solution.
    أرجع إشارة(sig, func);
#جيڪڏهن_پورو
}

    صحيح
mch_chdir(حرف *رستو)
{
    إذا (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", رستو);
	verbose_leave();
    }
#جيڪڏهن_وصف_ٿيل VMS
    أرجع chdir(vms_fixfilename(رستو));
#وإلا
    أرجع chdir(رستو);
#جيڪڏهن_پورو
}

#إذا defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#جيڪڏهن_پورو

/*
 * Write s[len] to the screen (stdout).
 */
    فراغ
mch_write(char_u *s, صحيح len)
{
    vim_ignored = (صحيح)اكتب(1, (حرف *)s, len);
    إذا (p_wd)		// Unix is too fast, slow down a bit more
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
    إذا (check_only)
	أرجع do_resize;
    طالما (do_resize)
    {
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
	ch_log(عدم, "calling handle_resize() in resize_func()");
#جيڪڏهن_پورو
	handle_resize();
    }
    أرجع FALSE;
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
    ڊگهو	wtime,	    // don't use "time", MIPS cannot handle it
    صحيح		tb_change_cnt)
{
    أرجع inchar_loop(buf, maxlen, wtime, tb_change_cnt,
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
    أرجع WaitForChar(0L, عدم, FALSE);
}

#إذا defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحيح
mch_check_messages(فراغ)
{
    أرجع WaitForChar(0L, عدم, TRUE);
}
#جيڪڏهن_پورو

#إذا defined(HAVE_ڪل_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# إذا defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# إذا defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_ڪل_mem(صحيح special اڻ_استعمال_ٿيل)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_پورٽ_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	بنية vm_statistics64	vm_stat;
	natural_t		ڳڻپ = HOST_VM_INFO64_ڳڻپ;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ڳڻپ);
#  وإلا
	بنية vm_statistics	vm_stat;
	natural_t		ڳڻپ = HOST_VM_INFO_ڳڻپ;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ڳڻپ);
#  endif
	إذا (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ڳڻپ + vm_stat.سرگرم_ڳڻپ
					    + vm_stat.inactive_ڳڻپ
#  ifdef MAC_OS_X_نسخو_10_9
					    + vm_stat.compressor_صفحو_ڳڻپ
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_پورٽ_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    إذا (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	صحيح		mib[2];
	size_t		len = حجم(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  وإلا
	// sysctl() may return 32 bit or 64 bit, accept both
	اتحاد {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  وإلا
	mib[1] = HW_USERMEM;
#  endif
	إذا (sysctl(mib, 2, &physmem, &len, عدم, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  وإلا
	    إذا (len == حجم(physmem.u64))
		mem = (long_u)physmem.u64;
	    وإلا
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# إذا defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    إذا (mem == 0)
    {
	بنية sysinfo sinfo;

	// Linux way of getting amount of RAM available
	إذا (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    طالما (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  وإلا
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    إذا (mem == 0)
    {
	ڊگهو	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	إذا (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    طالما (shiftright > 0 && (pagesize & 1) == 0)
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
# إذا defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	بنية rlimit	rlp;

	إذا (getrlimit(RLIMIT_معلومات, &rlp) == 0
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

    إذا (mem > 0)
	أرجع mem >> shiftright;
    أرجع (long_u)0x1fffff;
}
#جيڪڏهن_پورو

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    فراغ
mch_delay(ڊگهو msec, صحيح flags)
{
    tmode_T	old_tmode;
    صحيح		call_settmode;
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
    ڊگهو	ڪل = msec; // remember original value
#جيڪڏهن_پورو

    إذا (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	اندريون_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	إذا (call_settmode)
	{
	    old_tmode = mch_cur_tmode;
	    settmode(TMODE_SLEEP);
	}

	/*
	 * Everybody sleeps in a different way...
	 * Prefer nanosleep(), some versions of usleep() can only sleep up to
	 * one second.
	 */
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
	افعل
	{
	    // if total is large enough, wait by portions in p_mzq
	    إذا (ڪل > p_mzq)
		msec = p_mzq;
	    وإلا
		msec = ڪل;
	    ڪل -= msec;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل HAVE_NANOSLEEP
	{
	    بنية timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (فراغ)nanosleep(&ts, عدم);
	}
#وإلا
# ifdef HAVE_USLEEP
	طالما (msec >= 1000)
	{
	    usleep((بنانشان صحيح)(999 * 1000));
	    msec -= 999;
	}
	usleep((بنانشان صحيح)(msec * 1000));
# وإلا
#  ifndef HAVE_SELECT
	poll(عدم, 0, (صحيح)msec);
#  وإلا
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
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MZSCHEME
	}
	طالما (ڪل > 0);
#جيڪڏهن_پورو

	إذا (call_settmode)
	    settmode(old_tmode);
	اندريون_mch_delay = FALSE;
    }
    وإلا
	WaitForChar(msec, عدم, FALSE);
}

#إذا defined(HAVE_ڍير_حد_بندي) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ڍير_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ساكن صحيح ڍير_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ساكن فراغ
check_ڍير_growth(حرف *p)
{
    صحيح		i;

    ڍير_grows_downwards = (p > (حرف *)&i);
}
#جيڪڏهن_پورو

#إذا defined(HAVE_ڍير_حد_بندي)
ساكن حرف *ڍير_حد_بندي = عدم;

# إذا defined(THREAD_محفوظ) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ساكن فراغ
get_ڍير_حد_بندي(فراغ)
{
    بنية rlimit	rlp;
    صحيح			i;
    ڊگهو		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    إذا (getrlimit(RLIMIT_ڍير, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (حجم(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (ڊگهو)rlp.rlim_cur;
# إذا defined(THREAD_محفوظ) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    سائيز;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    إذا (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &سائيز);
		إذا (lim > (ڊگهو)سائيز)
		    lim = (ڊگهو)سائيز;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	إذا (ڍير_grows_downwards)
	{
	    ڍير_حد_بندي = (حرف *)((ڊگهو)&i - (lim / 16L * 15L));
	    إذا (ڍير_حد_بندي >= (حرف *)&i)
		// overflow, set to 1/16 of current stack position
		ڍير_حد_بندي = (حرف *)((ڊگهو)&i / 16L);
	}
	وإلا
	{
	    ڍير_حد_بندي = (حرف *)((ڊگهو)&i + (lim / 16L * 15L));
	    إذا (ڍير_حد_بندي <= (حرف *)&i)
		ڍير_حد_بندي = عدم;	//حد_کان_مٿي
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
    إذا (ڍير_حد_بندي == عدم)
	أرجع OK;

    إذا (ڍير_grows_downwards)
    {
	إذا (p < ڍير_حد_بندي)
	    أرجع FAIL;
    }
    وإلا إذا (p > ڍير_حد_بندي)
	أرجع FAIL;
    أرجع OK;
}
#جيڪڏهن_پورو

#إذا defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ساكن stack_t sigstk;			// for sigaltstack()
# وإلا
ساكن بنية sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ساكن ڊگهو صحيح get_signal_ڍير_سائيز(فراغ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    ڊگهو صحيح سائيز = -1;

    // return size only if sysconf doesn't return an error
    إذا ((سائيز = sysconf(_SC_SIGSTKSZ)) > -1)
	أرجع سائيز;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    أرجع SIGSTKSZ;
# endif

    // otherwise guess the size
    أرجع 8000;
}

ساكن حرف *signal_ڍير;

    ساكن فراغ
init_signal_ڍير(فراغ)
{
    إذا (signal_ڍير == عدم)
	أرجع;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_بنياد
    sigstk.ss_بنياد = signal_ڍير;
#  وإلا
    sigstk.ss_sp = signal_ڍير;
#  endif
    sigstk.ss_size = get_signal_ڍير_سائيز();
    sigstk.ss_flags = 0;
    (فراغ)sigaltstack(&sigstk, عدم);
# وإلا
    sigstk.ss_sp = signal_ڍير;
    إذا (ڍير_grows_downwards)
	sigstk.ss_sp += get_signal_ڍير_سائيز() - 1;
    sigstk.ss_onstack = 0;
    (فراغ)sigstack(&sigstk, عدم);
# endif
}
#جيڪڏهن_پورو

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#إذا defined(SIGWINCH)
    ساكن فراغ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#جيڪڏهن_پورو

#إذا defined(SIGTSTP)
    ساكن فراغ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    إذا (اندريون_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ارفع(sigarg);
    }
    وإلا
	got_tstp = TRUE;

# إذا !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#جيڪڏهن_پورو

#إذا defined(SIGINT)
    ساكن فراغ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#جيڪڏهن_پورو

#إذا defined(SIGUSR1)
    ساكن فراغ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#جيڪڏهن_پورو

#إذا defined(SIGPWR)
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل سيٽ_SIG_ALARM
/*
 * signal function for alarm().
 */
    ساكن فراغ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#جيڪڏهن_پورو

#إذا defined(HAVE_SETJMP_H) \
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
ساكن متطاير sig_atomic_t lc_سرگرم = FALSE;

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
    lc_سرگرم = TRUE;
}

    ساكن فراغ
mch_endjmp(فراغ)
{
    lc_سرگرم = FALSE;
}

    ساكن فراغ
mch_didjmp(فراغ)
{
# إذا defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ڍير();
# endif
}
#جيڪڏهن_پورو

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
#جيڪڏهن_وصف_ٿيل SIGHASARG
    صحيح		i;
#جيڪڏهن_پورو

#إذا defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    إذا (lc_سرگرم)
    {
# إذا defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_سرگرم = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    إذا (اندريون_mch_delay && sigarg == SIGQUIT)
	أرجع;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    إذا (entered == 0
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
	أرجع;
#جيڪڏهن_پورو

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    بلاڪ_autocmds();

#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    // Set the v:dying variable.
    سيٽ_vim_var_nr(VV_DYING, (ڊگهو)entered);
#جيڪڏهن_پورو
    v_dying = entered;

#جيڪڏهن_وصف_ٿيل HAVE_ڍير_حد_بندي
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ڍير_حد_بندي();
#جيڪڏهن_پورو

#إذا 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	إذا (fp)
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
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGHASARG
    // try to find the name of this signal
    لكل (i = 0; signal_info[i].sig != -1; i++)
	إذا (sigarg == signal_info[i].sig)
	    اكسر;
    deadly_signal = sigarg;
#جيڪڏهن_پورو

    ڀريل_screen = FALSE;	// don't write message to the GUI, it might be
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
    إذا (entered >= 3)
    {
	reset_سگنل();	// don't catch any signals anymore
	may_core_dump();
	إذا (entered >= 4)
	    _exit(8);
	ٻاهر_نڪرو(7);
    }
    إذا (entered == 2)
    {
	// No translation, it may call malloc().
	ٻاهريون_STR("Vim: Double signal, exiting\n");
	ٻاهريون_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#جيڪڏهن_وصف_ٿيل SIGHASARG
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#وإلا
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal\r\n");
#جيڪڏهن_پورو

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#جيڪڏهن_وصف_ٿيل NBDEBUG
    reset_سگنل();
    may_core_dump();
    abort();
#جيڪڏهن_پورو
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

#إذا defined(SIGCONT)
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
    إذا (اندريون_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    وإلا
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	ٻاهريون_flush();
    }
}
#جيڪڏهن_پورو

#إذا defined(FEAT_CLIPBOARD)
# إذا defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ساكن فراغ *clip_تارو_save = عدم;
ساكن فراغ *clip_plus_save = عدم;
# endif

# إذا defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
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
    إذا (clip_تارو.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	إذا (clip_تارو.owned)
	    clip_lose_selection(&clip_تارو);
	إذا (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	إذا (x11_display != عدم)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# إذا defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ساكن فراغ
save_clipboard(فراغ)
{
    إذا (clip_تارو.owned)
	clip_تارو_save = get_رجسٽر('*', TRUE);
    إذا (clip_plus.owned)
	clip_plus_save = get_رجسٽر('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ساكن فراغ
restore_clipboard(فراغ)
{
    إذا (clip_تارو_save != عدم)
    {
	إذا (!clip_gen_owner_exists(&clip_تارو))
	    put_رجسٽر('*', clip_تارو_save);
	وإلا
	    free_رجسٽر(clip_تارو_save);
	clip_تارو_save = عدم;
    }
    إذا (clip_plus_save != عدم)
    {
	إذا (!clip_gen_owner_exists(&clip_plus))
	    put_رجسٽر('+', clip_plus_save);
	وإلا
	    free_رجسٽر(clip_plus_save);
	clip_plus_save = عدم;
    }
}
# endif
#جيڪڏهن_پورو

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    إذا (ignore_sigtstp)
	أرجع;

#إذا defined(SIGTSTP)
    اندريون_mch_suspend = TRUE;

    ٻاهريون_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    ٻاهريون_flush();	    // needed to disable mouse on some systems

# إذا defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# إذا defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# إذا defined(SIGCONT)
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
	ڊگهو wait_وقت;

	لكل (wait_وقت = 0; !sigcont_received && wait_وقت <= 3L; wait_وقت++)
	    mch_delay(wait_وقت, 0);
    }
# endif
    اندريون_mch_suspend = FALSE;

    after_sigcont();
#وإلا
    suspend_shell();
#جيڪڏهن_پورو
}

    فراغ
mch_init(فراغ)
{
    Columns = 80;
    صفون = 24;

    ٻاهريون_flush();

#جيڪڏهن_وصف_ٿيل SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#جيڪڏهن_پورو
    سيٽ_سگنل();

#جيڪڏهن_وصف_ٿيل MACOS_CONVERT
    mac_conv_init();
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#جيڪڏهن_پورو
}

    فراغ
سيٽ_sigwinch_handler(فراغ)
{
#إذا defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#جيڪڏهن_پورو
}

    ساكن فراغ
سيٽ_سگنل(فراغ)
{
#إذا defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.اندريون_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#جيڪڏهن_پورو
#إذا defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGINT
    catch_int_signal();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#جيڪڏهن_پورو

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#جيڪڏهن_وصف_ٿيل SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#جيڪڏهن_پورو

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_سگنل(deathtrap, SIG_ERR);

#إذا defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    إذا (gui.اندريون_use)
	mch_signal(SIGHUP, SIG_IGN);
#جيڪڏهن_پورو
}

#إذا defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ساكن فراغ
catch_int_signal(فراغ)
{
    mch_signal(SIGINT, catch_sigint);
}
#جيڪڏهن_پورو

    فراغ
reset_سگنل(فراغ)
{
    catch_سگنل(SIG_DFL, SIG_DFL);
#إذا defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#جيڪڏهن_پورو
}

    ساكن فراغ
catch_سگنل(
    فراغ (*func_deadly)(صحيح),
    فراغ (*func_other)(صحيح))
{
    صحيح	    i;

    لكل (i = 0; signal_info[i].sig != -1; i++)
    {
	إذا (signal_info[i].deadly)
	{
#إذا defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    بنية sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# إذا defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# وإلا
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, عدم);
#وإلا
# إذا defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    بنية sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_نقاب = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, عدم);
# وإلا
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#جيڪڏهن_پورو
	}
	وإلا إذا (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#جيڪڏهن_وصف_ٿيل SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#وإلا
	    mch_signal(signal_info[i].sig, func_other);
#جيڪڏهن_پورو
	}
    }
}

#جيڪڏهن_وصف_ٿيل HAVE_SIGPROCMASK
    ساكن فراغ
بلاڪ_سگنل_2(sigset_t *سيٽ_2)
{
    sigset_t	newset;
    صحيح		i;

    sigemptyset(&newset);

    لكل (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# إذا defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, سيٽ_2);
}

    ساكن فراغ
unblock_سگنل(sigset_t *سيٽ_2)
{
    sigprocmask(SIG_SETMASK, سيٽ_2, عدم);
}
#جيڪڏهن_پورو

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
	حالة SIGNAL_بلاڪ:   blocked = TRUE;
			     اكسر;

	حالة SIGNAL_UNBLOCK: blocked = FALSE;
			     إذا (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     اكسر;

	افتراضي:	     إذا (!blocked)
				 أرجع TRUE;	// exit!
			     got_signal = sig;
#جيڪڏهن_وصف_ٿيل SIGPWR
			     إذا (sig != SIGPWR)
#جيڪڏهن_پورو
				 got_int = TRUE;    // break any loops
			     اكسر;
    }
    أرجع FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    صحيح
mch_check_win(صحيح argc اڻ_استعمال_ٿيل, حرف **argv اڻ_استعمال_ٿيل)
{
    إذا (isatty(1))
	أرجع OK;
    أرجع FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحيح
mch_داخلا_isatty(فراغ)
{
    إذا (isatty(read_cmd_fd))
	أرجع TRUE;
    أرجع FALSE;
}

#جيڪڏهن_وصف_ٿيل FEAT_X11

# إذا defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ساكن فراغ
xopen_پيغام(ڊگهو elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#جيڪڏهن_پورو

#إذا defined(FEAT_X11)
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
    XGet_Error_متن(dpy, error_event->error_ڪوڊ, (حرف *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    إذا (!gui.اندريون_use)
# endif
	نياپو((حرف *)IObuff);

    أرجع 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ساكن صحيح
x_error_check(Display *dpy اڻ_استعمال_ٿيل, XErrorEvent *error_event اڻ_استعمال_ٿيل)
{
    got_x_error = TRUE;
    أرجع 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ساكن صحيح
x_connect_منزل_server(فراغ)
{
    // No point in connecting if we are exiting or dying.
    إذا (exiting || v_dying)
	أرجع FALSE;

# إذا defined(FEAT_CLIENTSERVER)
    إذا (x_force_connect)
	أرجع TRUE;
# endif
    إذا (x_no_connect)
	أرجع FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    إذا (clip_exclude_prog != عدم)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	إذا (regprog_اندريون_use(clip_exclude_prog))
	    أرجع FALSE;

	إذا (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    أرجع FALSE;
    }
    أرجع TRUE;
}

# إذا defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  إذا defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ساكن صحيح
x_IOerror_check(Display *dpy اڻ_استعمال_ٿيل)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   إذا defined(VMS) || defined(__CYGWIN__)
    أرجع 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ساكن صحيح xterm_dpy_retry_ڳڻپ = 0;

    ساكن صحيح
x_IOerror_handler(Display *dpy اڻ_استعمال_ٿيل)
{
    xterm_dpy = عدم;
    xterm_dpy_retry_ڳڻپ = 5;  // Try reconnecting five times
    x11_دري = 0;
    x11_display = عدم;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  إذا defined(VMS) || defined(__CYGWIN__)
    أرجع 0;  // avoid the compiler complains about missing return value
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
    إذا (!exiting && !v_dying && xterm_dpy_retry_ڳڻپ > 0)
    {
	--xterm_dpy_retry_ڳڻپ;

#  ifndef LESSTIF_نسخو
	// This has been reported to avoid Vim getting stuck.
	إذا (app_context != (XtAppContext)عدم)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)عدم;
	    x11_display = عدم; // freed by XtDestroyApplicationContext()
	}
#  endif

	تياري_رڪن_clip();
	get_x11_title(FALSE);
    }
}

    فراغ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    إذا (eap->arg != عدم && (arglen = STRLEN(eap->arg)) > 0)
    {
	إذا (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (حرف *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == عدم
		    ? (حرف *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_دري = 0;
    xterm_dpy_retry_ڳڻپ = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    چونڊيو_clipmethod();
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
test_x11_دري(Display *dpy)
{
    صحيح			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	متن_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    إذا (XGetWMName(dpy, x11_دري, &متن_prop))
	XFree((فراغ *)متن_prop.قدر);
    XSync(dpy, False);
    (فراغ)XSetErrorHandler(old_handler);

    إذا (p_verbose > 0 && got_x_error)
	verb_نياپو(_("Testing the X display failed"));

    أرجع (got_x_error ? FAIL : OK);
}
#جيڪڏهن_پورو


#جيڪڏهن_وصف_ٿيل FEAT_X11

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
    ساكن صحيح	    نتيجو = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_هتي	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ساكن صحيح	    x11_display_from = XD_NONE;
    ساكن صحيح	    did_سيٽ_error_handler = FALSE;

    إذا (!did_سيٽ_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(فراغ)XSetErrorHandler(x_error_handler);
	did_سيٽ_error_handler = TRUE;
    }

# إذا defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    إذا (gui.اندريون_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	إذا (x11_display_from == XD_هتي && x11_display != عدم)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	إذا (gui_get_x11_windis(&x11_دري, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    أرجع OK;
	}
	x11_display = عدم;
	أرجع FAIL;
    }
    وإلا إذا (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_دري = 0;
	x11_display = عدم;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    إذا (!x_connect_منزل_server())
	أرجع FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    إذا (x11_دري == 0 && (winid = getenv("WINDOWID")) != عدم)
	x11_دري = (دري)atol(winid);

# ifdef FEAT_XCLIPBOARD
    إذا (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    إذا (xterm_dpy != عدم && x11_دري != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	إذا (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    إذا (x11_display_from == XD_هتي && x11_display != عدم)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	إذا (test_x11_دري(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_دري = 0;
	    x11_display = عدم;
	    x11_display_from = XD_NONE;
	    أرجع FAIL;
	}
	أرجع OK;
    }
# endif

    إذا (x11_دري == 0 || x11_display == عدم)
	نتيجو = -1;

    إذا (نتيجو != -1)	    // Have already been here and set this
	أرجع نتيجو;	    // Don't do all these X calls again

    إذا (x11_دري != 0 && x11_display == عدم)
    {
# ifdef سيٽ_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T شروعات_tv;

	إذا (p_verbose > 0)
	    ELAPSED_INIT(شروعات_tv);
# endif

# ifdef سيٽ_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(عدم);

# ifdef سيٽ_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	إذا (p_verbose > 0 && sig_alarm_called)
	    verb_نياپو(_("Opening the X display timed out"));
# endif
	إذا (x11_display != عدم)
	{
# ifdef ELAPSED_FUNC
	    إذا (p_verbose > 0)
	    {
		verbose_enter();
		xopen_پيغام(ELAPSED_FUNC(شروعات_tv));
		verbose_leave();
	    }
# endif
	    إذا (test_x11_دري(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_دري = 0;
		XCloseDisplay(x11_display);
		x11_display = عدم;
	    }
	    وإلا
		x11_display_from = XD_هتي;
	}
    }
    إذا (x11_دري == 0 || x11_display == عدم)
	أرجع (نتيجو = FAIL);

# ifdef FEAT_EVAL
    سيٽ_vim_var_nr(VV_WINDOWID, (ڊگهو)x11_دري);
# endif

    أرجع (نتيجو = OK);
}

/*
 * Determine original x11 Window Title
 */
    ساكن صحيح
get_x11_title(صحيح test_only)
{
    أرجع get_x11_thing(TRUE, test_only);
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
    إذا (oldicon == عدم && !test_only)
    {
	إذا (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	وإلا
	    oldicon = vim_strsave(T_NAME);
    }

    أرجع retval;
}

    ساكن صحيح
get_x11_thing(
    صحيح		get_title,	// get title string
    صحيح		test_only)
{
    XTextProperty	متن_prop;
    صحيح			retval = FALSE;
    حالت_2		حالت;

    إذا (get_x11_windis() != OK)
	أرجع FALSE;

    // Get window/icon name if any
    إذا (get_title)
	حالت = XGetWMName(x11_display, x11_دري, &متن_prop);
    وإلا
	حالت = XGetWMIconName(x11_display, x11_دري, &متن_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    دري	    پاڙ;
    دري	    والد;
    دري	    win = x11_دري;
    دري	   *children;
    بنانشان صحيح    انگ_children;

    طالما (!حالت || متن_prop.قدر == عدم)
    {
	إذا (!XQuery_وڻ(x11_display, win, &پاڙ, &والد, &children,
		    &انگ_children))
	    اكسر;
	إذا (children)
	    XFree((فراغ *)children);
	إذا (والد == پاڙ || والد == 0)
	    اكسر;

	win = والد;
	إذا (get_title)
	    حالت = XGetWMName(x11_display, win, &متن_prop);
	وإلا
	    حالت = XGetWMIconName(x11_display, win, &متن_prop);
    }

    إذا (حالت && متن_prop.قدر != عدم)
    {
	retval = TRUE;
	إذا (!test_only)
	{
	    إذا (get_title)
		vim_free(oldtitle);
	    وإلا
		vim_free(oldicon);
	    إذا (متن_prop.encoding == XA_STRING && !has_mbyte)
	    {
		إذا (get_title)
		    oldtitle = vim_strsave((char_u *)متن_prop.قدر);
		وإلا
		    oldicon = vim_strsave((char_u *)متن_prop.قدر);
	    }
	    وإلا
	    {
		حرف    **cl;
		حالت_2  transform_حالت;
		صحيح	    n = 0;

		transform_حالت = Xmb_متن_Property_منزل_متن_List(x11_display,
			&متن_prop,
			&cl, &n);
		إذا (transform_حالت >= Success && n > 0 && cl[0])
		{
		    إذا (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    وإلا
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		وإلا
		{
		    إذا (get_title)
			oldtitle = vim_strsave((char_u *)متن_prop.قدر);
		    وإلا
			oldicon = vim_strsave((char_u *)متن_prop.قدر);
		}
	    }
	}
	XFree((فراغ *)متن_prop.قدر);
    }
    أرجع retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# إذا defined(X_HAVE_UTF8_STRING)
#  إذا X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ساكن فراغ
سيٽ_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    إذا (enc_utf8)
	Xutf8_سيٽ_WMProperties(x11_display, x11_دري, (ثابت حرف *)title,
					     عدم, عدم, 0, عدم, عدم, عدم);
    وإلا
# endif
    {
# إذا XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_سيٽ_WMProperties(x11_display, x11_دري, (ثابت حرف *)title,
					     عدم, عدم, 0, عدم, عدم, عدم);
#  وإلا
	XTextProperty	متن_prop;
	حرف		*c_title = (حرف *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(فراغ)XString_List_منزل_متن_Property(&c_title, 1, &متن_prop);
	XSetWMProperties(x11_display, x11_دري, &متن_prop,
					     عدم, عدم, 0, عدم, عدم, عدم);
#  endif
# وإلا
	XStoreName(x11_display, x11_دري, (حرف *)title);
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
سيٽ_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    إذا (enc_utf8)
	Xutf8_سيٽ_WMProperties(x11_display, x11_دري, عدم, (ثابت حرف *)icon,
						   عدم, 0, عدم, عدم, عدم);
    وإلا
# endif
    {
# إذا XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_سيٽ_WMProperties(x11_display, x11_دري, عدم, (ثابت حرف *)icon,
						   عدم, 0, عدم, عدم, عدم);
#  وإلا
	XTextProperty	متن_prop;
	حرف		*c_icon = (حرف *)icon;

	(فراغ)XString_List_منزل_متن_Property(&c_icon, 1, &متن_prop);
	XSetWMProperties(x11_display, x11_دري, عدم, &متن_prop,
						   عدم, 0, عدم, عدم, عدم);
#  endif
# وإلا
	XSetIconName(x11_display, x11_دري, (حرف *)icon);
# endif
    }
    XFlush(x11_display);
}

#وإلا  // FEAT_X11

    ساكن صحيح
get_x11_title(صحيح test_only اڻ_استعمال_ٿيل)
{
    أرجع FALSE;
}

    ساكن صحيح
get_x11_icon(صحيح test_only)
{
    إذا (!test_only)
    {
	إذا (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	وإلا
	    oldicon = vim_strsave(T_NAME);
    }
    أرجع FALSE;
}

#جيڪڏهن_پورو // FEAT_X11

    صحيح
mch_can_restore_title(فراغ)
{
    أرجع get_x11_title(TRUE);
}

    صحيح
mch_can_restore_icon(فراغ)
{
    أرجع get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    فراغ
mch_settitle(char_u *title, char_u *icon)
{
    صحيح		type = 0;
    ساكن صحيح	recursive = 0;

    إذا (T_NAME == عدم)	    // no terminal name (yet)
	أرجع;
    إذا (title == عدم && icon == عدم)	    // nothing to do
	أرجع;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    إذا (recursive)
	أرجع;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#جيڪڏهن_وصف_ٿيل FEAT_X11
    إذا (get_x11_windis() == OK)
	type = 1;
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    إذا (gui.اندريون_use)
	type = 1;
#جيڪڏهن_پورو

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    إذا ((type || *T_TS != NUL) && title != عدم)
    {
	إذا (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	إذا (oldtitle == عدم
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		&& !gui.اندريون_use
#جيڪڏهن_پورو
		)		// first call but not in GUI, save title
	    (فراغ)get_x11_title(FALSE);

	إذا (*T_TS != NUL)		// it's OK if t_fs is empty
	    رڪن_settitle(title);
#جيڪڏهن_وصف_ٿيل FEAT_X11
	وإلا
# ifdef FEAT_GUI_GTK
	إذا (!gui.اندريون_use)		// don't do this if GTK+ is running
# endif
	    سيٽ_x11_title(title);		// x11
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	وإلا
	    gui_mch_settitle(title, icon);
#جيڪڏهن_پورو
	unix_did_سيٽ_title = TRUE;
    }

    إذا ((type || *T_CIS != NUL) && icon != عدم)
    {
	إذا (oldicon == عدم
#جيڪڏهن_وصف_ٿيل FEAT_GUI
		&& !gui.اندريون_use
#جيڪڏهن_پورو
		)		// first call, save icon
	    get_x11_icon(FALSE);

	إذا (*T_CIS != NUL)
	{
	    ٻاهريون_str(T_CIS);			// set icon start
	    ٻاهريون_str_nf(icon);
	    ٻاهريون_str(T_CIE);			// set icon end
	    ٻاهريون_flush();
	}
#جيڪڏهن_وصف_ٿيل FEAT_X11
	وإلا
# ifdef FEAT_GUI_GTK
	إذا (!gui.اندريون_use)		// don't do this if GTK+ is running
# endif
	    سيٽ_x11_icon(icon);			// x11
#جيڪڏهن_پورو
	did_سيٽ_icon = TRUE;
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
    صحيح	do_ڌڪو_ڪڍو = unix_did_سيٽ_title || did_سيٽ_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_سيٽ_title)
			? (oldtitle ? oldtitle : p_titleold) : عدم;
    char_u *tofree = عدم;
    إذا (title == oldtitle && oldtitle != عدم)
    {
	tofree = vim_strsave(title);
	إذا (tofree != عدم)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_سيٽ_icon) ? oldicon : عدم);
    vim_free(tofree);

    إذا (do_ڌڪو_ڪڍو)
    {
	// pop and push from/to the stack
	رڪن_ڪڍو_title(which);
	رڪن_ڌڪو_title(which);
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
    إذا (name == عدم)
	أرجع FALSE;
    أرجع ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#إذا defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    صحيح
use_xterm_like_mouse(char_u *name)
{
    أرجع (name != عدم
	    && (رڪن_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#جيڪڏهن_پورو

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
    إذا (ttym_flags == TTYM_SGR)
	أرجع 4;
    إذا (ttym_flags == TTYM_URXVT)
	أرجع 3;
    إذا (ttym_flags == TTYM_XTERM2)
	أرجع 2;
    إذا (ttym_flags == TTYM_XTERM)
	أرجع 1;
    أرجع 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    صحيح
vim_is_iris(char_u *name)
{
    إذا (name == عدم)
	أرجع FALSE;
    أرجع (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#إذا defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    صحيح
vim_is_vt300(char_u *name)
{
    إذا (name == عدم)
	أرجع FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    أرجع ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != عدم)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#جيڪڏهن_پورو

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    صحيح
mch_get_user_name(char_u *s, صحيح len)
{
#جيڪڏهن_وصف_ٿيل VMS
    vim_strncpy(s, (char_u *)cuserid(عدم), len - 1);
    أرجع OK;
#وإلا
    أرجع mch_get_uname(getuid(), s, len);
#جيڪڏهن_پورو
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    صحيح
mch_get_uname(uid_t uid, char_u *s, صحيح len)
{
#إذا defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    بنية passwd   *pw;

    إذا ((pw = getpwuid(uid)) != عدم
	    && pw->pw_name != عدم && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	أرجع OK;
    }
#جيڪڏهن_پورو
    sprintf((حرف *)s, "%d", (صحيح)uid);	    // assumes s is long enough
    أرجع FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#جيڪڏهن_وصف_ٿيل HAVE_SYS_UTSNAME_H
    فراغ
mch_get_host_name(char_u *s, صحيح len)
{
    بنية utsname vutsname;

    إذا (uname(&vutsname) < 0)
	*s = NUL;
    وإلا
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#وإلا // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    فراغ
mch_get_host_name(char_u *s, صحيح len)
{
# ifdef VAXC
    vaxc$gethostname((حرف *)s, len);
# وإلا
    gethostname((حرف *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#جيڪڏهن_پورو // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    ڊگهو
mch_get_pid(فراغ)
{
    أرجع (ڊگهو)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحيح
mch_process_running(ڊگهو pid)
{
    // If there is no error the process must be running.
    إذا (kill(pid, 0) == 0)
	أرجع TRUE;
#جيڪڏهن_وصف_ٿيل ESRCH
    // If the error is ESRCH then the process is not running.
    إذا (errno == ESRCH)
	أرجع FALSE;
#جيڪڏهن_پورو
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    أرجع TRUE;
}

#إذا !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ساكن حرف *
strerror(صحيح err)
{
    خارجي صحيح	    sys_nerr;
    خارجي حرف	    *sys_errlist[];
    ساكن حرف	    er[20];

    إذا (err > 0 && err < sys_nerr)
	أرجع (sys_errlist[err]);
    snprintf(er, حجم(er), "Error %d", err);
    أرجع er;
}
#جيڪڏهن_پورو

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(char_u *buf, صحيح len)
{
#إذا defined(USE_GETCWD)
    إذا (getcwd((حرف *)buf, len) == عدم)
    {
	STRCPY(buf, strerror(errno));
	أرجع FAIL;
    }
    أرجع OK;
#وإلا
    أرجع (getwd((حرف *)buf) != عدم ? OK : FAIL);
#جيڪڏهن_پورو
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    صحيح
mch_ڀريل_Name(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)		// also expand when already absolute path
{
    صحيح		buflen = 0;
#جيڪڏهن_وصف_ٿيل HAVE_FCHDIR
    صحيح		fd = -1;
    ساكن صحيح	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#جيڪڏهن_پورو
    char_u	olddir[MAXPATHL];
    char_u	*p;
    صحيح		retval = OK;
#جيڪڏهن_وصف_ٿيل __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل VMS
    fname = vms_fixfilename(fname);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# إذا CYGWIN_نسخو_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_رستو(CCP_WIN_A_منزل_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# وإلا
    cygwin_conv_منزل_posix_رستو(fname, posix_fname);
# endif
    fname = posix_fname;
#جيڪڏهن_پورو

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    إذا ((force || !mch_is_ڀريل_Name(fname))
	    && ((p = vim_strrchr(fname, '/')) == عدم || p != fname))
    {
	إذا (p == عدم && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	إذا (p != عدم)
	{
	    إذا (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#جيڪڏهن_وصف_ٿيل HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    إذا (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		إذا (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#جيڪڏهن_پورو

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    إذا (
#جيڪڏهن_وصف_ٿيل HAVE_FCHDIR
		fd < 0 &&
#جيڪڏهن_پورو
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((حرف *)olddir) != 0))
	    {
		p = عدم;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    وإلا
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		إذا (p - fname >= len)
		    retval = FAIL;
		وإلا
		{
		    vim_strncpy(buf, fname, p - fname);
		    إذا (mch_chdir((حرف *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			إذا (mch_is_ڀريل_Name(fname))
			    retval = FAIL;
			وإلا
			    p = عدم;
		    }
		    وإلا إذا (*p == '/')
			fname = p + 1;
		    وإلا
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	إذا (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	إذا (p != عدم)
	{
	    صحيح	l;

#جيڪڏهن_وصف_ٿيل HAVE_FCHDIR
	    إذا (fd >= 0)
	    {
		إذا (p_verbose >= 5)
		{
		    verbose_enter();
		    نياپو("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    وإلا
#جيڪڏهن_پورو
		l = mch_chdir((حرف *)olddir);
	    إذا (l != 0)
		emsg(_(e_cannot_go_پٺ_منزل_اڳيون_directory));
	}
#جيڪڏهن_وصف_ٿيل HAVE_FCHDIR
	إذا (fd >= 0)
	    close(fd);
#جيڪڏهن_پورو

	buflen = (صحيح)STRLEN(buf);
	إذا (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#جيڪڏهن_وصف_نه_ٿيل VMS
	وإلا إذا (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += حجم(PATHSEP);
	}
#جيڪڏهن_پورو
    }

    إذا (buflen == 0)
	buflen = (صحيح)STRLEN(buf);

    // Catch file names which are too long.
    إذا (retval == FAIL || (صحيح)(buflen + STRLEN(fname)) >= len)
	أرجع FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    إذا (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    أرجع OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحيح
mch_is_ڀريل_Name(char_u *fname)
{
#جيڪڏهن_وصف_ٿيل VMS
    أرجع ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((حرف *)fname,':') || strchr((حرف *)fname,'"') ||
	    (strchr((حرف *)fname,'[') && strchr((حرف *)fname,']'))||
	    (strchr((حرف *)fname,'<') && strchr((حرف *)fname,'>'))   );
#وإلا
    أرجع (*fname == '/' || *fname == '~');
#جيڪڏهن_پورو
}

#إذا defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    فراغ
fname_case(
    char_u	*name,
    صحيح		len اڻ_استعمال_ٿيل)  // buffer size, only used when name gets longer
{
    بنية stat st;
    char_u	*slash, *پڇ;
    size_t	taillen;
    DIR		*dirp;
    بنية dirent *dp;

    إذا (mch_lstat((حرف *)name, &st) < 0)
	أرجع;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    إذا (slash == عدم)
    {
	dirp = opendir(".");
	پڇ = name;
    }
    وإلا
    {
	*slash = NUL;
	dirp = opendir((حرف *)name);
	*slash = '/';
	پڇ = slash + 1;
    }

    إذا (dirp == عدم)
	أرجع;

    taillen = STRLEN(پڇ);
    طالما ((dp = readdir(dirp)) != عدم)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	إذا (STRICMP(پڇ, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    بنية stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (پڇ - name), (char_u *)dp->d_name,
		    MAXPATHL - (پڇ - name));
	    إذا (mch_lstat((حرف *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(پڇ, dp->d_name);
		اكسر;
	    }
	}
    }

    closedir(dirp);
}
#جيڪڏهن_پورو

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    ڊگهو
mch_getperm(char_u *name)
{
    بنية stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#جيڪڏهن_وصف_ٿيل VMS
    إذا (stat((حرف *)vms_fixfilename(name), &statb))
#وإلا
    إذا (stat((حرف *)name, &statb))
#جيڪڏهن_پورو
	أرجع -1;
#جيڪڏهن_وصف_ٿيل __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    أرجع statb.st_گهڻائي & ~S_ADDACE;
#وإلا
    أرجع statb.st_گهڻائي;
#جيڪڏهن_پورو
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_setperm(char_u *name, ڊگهو perm)
{
    أرجع (chmod((حرف *)
#جيڪڏهن_وصف_ٿيل VMS
		    vms_fixfilename(name),
#وإلا
		    name,
#جيڪڏهن_پورو
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#إذا defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحيح
mch_fsetperm(صحيح fd, ڊگهو perm)
{
    أرجع (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#جيڪڏهن_پورو

#إذا defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
تعريفنوع بنية vim_acl_solaris_T {
    صحيح acl_cnt;
    aclent_t *acl_داخلا;
} vim_acl_solaris_T;
# endif

# إذا defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    فراغ
mch_copy_sec(char_u *from_file, char_u *منزل_file)
{
    إذا (from_file == عدم)
	أرجع;

    إذا (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    إذا (selinux_enabled <= 0)
	أرجع;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    حرف *from_context = عدم;
    حرف *منزل_context = عدم;

    إذا (getfilecon((حرف *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	إذا (errno == EOPNOTSUPP)
	    أرجع;

	نياپو_puts(_("\nCould not get security context for "));
	نياپو_outtrans(from_file);
	نياپو_putchar('\n');
	أرجع;
    }
    إذا (getfilecon((حرف *)منزل_file, &منزل_context) < 0)
    {
	نياپو_puts(_("\nCould not get security context for "));
	نياپو_outtrans(منزل_file);
	نياپو_putchar('\n');
	freecon (from_context);
	أرجع ;
    }
    إذا (strcmp(from_context, منزل_context) != 0)
    {
	إذا (setfilecon((حرف *)منزل_file, from_context) < 0)
	{
	    نياپو_puts(_("\nCould not set security context for "));
	    نياپو_outtrans(منزل_file);
	    نياپو_putchar('\n');
	}
    }
    freecon(منزل_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# إذا defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    فراغ
mch_copy_sec(char_u *from_file, char_u *منزل_file)
{
    ساكن ثابت حرف * ثابت smack_copied_خاصيتون[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    حرف	بفر[SMACK_LABEL_LEN];
    ثابت حرف	*name;
    صحيح		اشاريو;
    صحيح		ret;
    ssize_t	سائيز;

    إذا (from_file == عدم)
	أرجع;

    سائيز = listxattr((حرف *)from_file, عدم, 0);
    // not supported or no attributes to copy
    إذا (سائيز <= 0)
	أرجع;

    لكل (اشاريو = 0 ; اشاريو < (صحيح)ARRAY_ڊيگهائي(smack_copied_خاصيتون); اشاريو++)
    {
	// get the name of the attribute to copy
	name = smack_copied_خاصيتون[اشاريو];

	// get the value of the attribute in buffer
	سائيز = getxattr((حرف*)from_file, name, بفر, حجم(بفر));
	إذا (سائيز >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((حرف*)منزل_file, name, بفر, (size_t)سائيز, 0);
	    إذا (ret < 0)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, منزل_file);
		نياپو_outtrans(IObuff);
		نياپو_putchar('\n');
	    }
	}
	وإلا
	{
	    // what reason of not having the attribute value?
	    بدل (errno)
	    {
		حالة ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    أرجع; // leave because it isn't useful to continue

		حالة ERANGE:
		افتراضي:
		    // no enough size OR unexpected error
		     vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    نياپو_puts((حرف *)IObuff);
		    نياپو_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		حالة ENODATA:
		    // no attribute of this name
		    ret = removexattr((حرف*)منزل_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    اكسر;
	    }
	}
    }
}
# endif // HAVE_SMACK

# إذا defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    فراغ
mch_copy_xattr(char_u *from_file, char_u *منزل_file)
{
    حرف	*xattr_buf;
    ssize_t	سائيز;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    حرف	*ڪنجي;
    حرف	*val = عدم;
    حرف	*errmsg = عدم;

    إذا (from_file == عدم)
	أرجع;

    // get the length of the extended attributes
    سائيز = listxattr((حرف *)from_file, عدم, 0);
    // not supported or no attributes to copy
    إذا (سائيز <= 0)
	أرجع;
    xattr_buf = (حرف*)alloc(سائيز);
    إذا (xattr_buf == عدم)
	أرجع;
    سائيز = listxattr((حرف *)from_file, xattr_buf, سائيز);
    tsize = سائيز;

    errno = 0;

    لكل (صحيح ڦيرو = 0; ڦيرو < 2; ڦيرو++)
    {

	ڪنجي = xattr_buf;
	إذا (ڦيرو == 1)
	    سائيز = tsize;

	طالما (سائيز > 0)
	{
	    vallen = getxattr((حرف *)from_file, ڪنجي,
		    val, ڦيرو ? max_vallen : 0);
	    // only set the attribute in the second round
	    إذا (vallen >= 0 && ڦيرو &&
		setxattr((حرف *)منزل_file, ڪنجي, val, vallen, 0) == 0)
		;
	    وإلا إذا (errno)
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

	    إذا (ڦيرو == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ڪنجي) + 1;
	    سائيز -= keylen;
	    ڪنجي += keylen;
	}
	إذا (ڦيرو)
	    اكسر;

	val = (حرف*)alloc(max_vallen + 1);
	إذا (val == عدم)
	    اذهب error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    إذا (errmsg != عدم)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname اڻ_استعمال_ٿيل)
{
    vim_acl_T	ret = عدم;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((حرف *)fname, ACL_TYPE_ACCESS);
# وإلا
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    إذا (acl_get((حرف *)fname, 0, &aclent) < 0)
	أرجع عدم;
    ret = (vim_acl_T)aclent;
#  وإلا
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = يادگيري_ڏيو(حجم(vim_acl_solaris_T));
    إذا (aclent == عدم)
	أرجع عدم;
    إذا ((aclent->acl_cnt = acl((حرف *)fname, GETACLCNT, 0, عدم)) < 0)
    {
	حرر(aclent);
	أرجع عدم;
    }
    aclent->acl_داخلا = يادگيري_ڏيو(aclent->acl_cnt * حجم(aclent_t));
    إذا (aclent->acl_داخلا == عدم)
    {
	حرر(aclent);
	أرجع عدم;
    }
    إذا (acl((حرف *)fname, GETACL, aclent->acl_cnt, aclent->acl_داخلا) < 0)
    {
	حرر(aclent->acl_داخلا);
	حرر(aclent);
	أرجع عدم;
    }
    ret = (vim_acl_T)aclent;
#   وإلا
#    إذا defined(HAVE_AIX_ACL)
    صحيح		aclsize;
    بنية acl *aclent;

    aclsize = حجم(بنية acl);
    aclent = يادگيري_ڏيو(aclsize);
    إذا (aclent == عدم)
	أرجع عدم;
    إذا (statacl((حرف *)fname, STX_عام, aclent, aclsize) < 0)
    {
	إذا (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    إذا (aclent == عدم || statacl((حرف *)fname, STX_عام, aclent, aclsize) < 0)
	    {
		حرر(aclent);
		أرجع عدم;
	    }
	}
	وإلا
	{
	    حرر(aclent);
	    أرجع عدم;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    أرجع ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    فراغ
mch_سيٽ_acl(char_u *fname اڻ_استعمال_ٿيل, vim_acl_T aclent)
{
    إذا (aclent == عدم)
	أرجع;
# ifdef HAVE_POSIX_ACL
    acl_سيٽ_file((حرف *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# وإلا
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_سيٽ((حرف *)fname, (acl_t *)aclent);
#  وإلا
#   ifdef HAVE_SOLARIS_ACL
    acl((حرف *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_داخلا);
#   وإلا
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
    إذا (aclent == عدم)
	أرجع;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# وإلا
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  وإلا
#   ifdef HAVE_SOLARIS_ACL
    حرر(((vim_acl_solaris_T *)aclent)->acl_داخلا);
    حرر(aclent);
#   وإلا
#    ifdef HAVE_AIX_ACL
    حرر(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#جيڪڏهن_پورو

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name اڻ_استعمال_ٿيل)
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

    إذا (*name == NUL)	    // Some stat()s don't flag "" as an error.
	أرجع FALSE;
    إذا (stat((حرف *)name, &statb))
	أرجع FALSE;
    أرجع (S_ISDIR(statb.st_گهڻائي) ? TRUE : FALSE);
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

    إذا (*name == NUL)	    // Some stat()s don't flag "" as an error.
	أرجع FALSE;
    إذا (mch_lstat((حرف *)name, &statb))
	أرجع FALSE;
    أرجع (S_ISDIR(statb.st_گهڻائي) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ساكن صحيح
executable_file(char_u *name)
{
    بنية stat	st;

    إذا (stat((حرف *)name, &st))
	أرجع 0;
#جيڪڏهن_وصف_ٿيل VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    صحيح vms_executable = 0;
    إذا (S_ISREG(st.st_گهڻائي) && mch_access((حرف *)name, X_OK) == 0)
    {
	إذا (لڙي_ڳولا(vms_tolower((حرف*)name),".exe") != عدم
		|| لڙي_ڳولا(vms_tolower((حرف*)name),".com")!= عدم)
	    vms_executable = 1;
    }
    أرجع vms_executable;
#وإلا
    أرجع S_ISREG(st.st_گهڻائي) && mch_access((حرف *)name, X_OK) == 0;
#جيڪڏهن_پورو
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحيح
mch_can_exe(char_u *name, char_u **رستو, صحيح use_رستو)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_پڇاڙي;
    size_t	elen;
    صحيح		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    إذا (!use_رستو || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	إذا ((use_رستو || gettail(name) != name) && executable_file(name))
	{
	    إذا (رستو != عدم)
	    {
		إذا (name[0] != '/')
		    *رستو = ڀريل_Name_save(name, TRUE);
		وإلا
		    *رستو = vim_strsave(name);
	    }
	    أرجع TRUE;
	}
	أرجع FALSE;
    }

    p = (char_u *)getenv("PATH");
    إذا (p == عدم || *p == NUL)
	أرجع -1;
    p_پڇاڙي = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_پڇاڙي - p) + 2;
    buf = alloc(bufsize);
    إذا (buf == عدم)
	أرجع -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    لكل (;;)
    {
	e = (char_u *)strchr((حرف *)p, ':');
	إذا (e == عدم)
	    e = p_پڇاڙي;
	elen = (size_t)(e - p);
	إذا (elen <= 1)		// empty entry means current dir
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
	إذا (retval == 1)
	{
	    إذا (رستو != عدم)
	    {
		إذا (buf[0] != '/')
		    *رستو = ڀريل_Name_save(buf, TRUE);
		وإلا
		    *رستو = vim_strnsave(buf, buflen);
	    }
	    اكسر;
	}

	إذا (*e != ':')
	    اكسر;
	p = e + 1;
    }

    vim_free(buf);
    أرجع retval;
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

    إذا (stat((حرف *)name, &st))
	أرجع ڳنڍ_عام;
    إذا (S_ISREG(st.st_گهڻائي) || S_ISDIR(st.st_گهڻائي))
	أرجع ڳنڍ_عام;
    إذا (S_ISBLK(st.st_گهڻائي))	// block device isn't writable
	أرجع ڳنڍ_OTHER;
    // Everything else is writable?
    أرجع ڳنڍ_WRITABLE;
}

    فراغ
mch_early_init(فراغ)
{
#جيڪڏهن_وصف_ٿيل HAVE_CHECK_ڍير_GROWTH
    صحيح			i;

    check_ڍير_growth((حرف *)&i);

# ifdef HAVE_ڍير_حد_بندي
    get_ڍير_حد_بندي();
# endif

#جيڪڏهن_پورو

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#إذا defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ڍير = alloc(get_signal_ڍير_سائيز());
    init_signal_ڍير();
#جيڪڏهن_پورو

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#جيڪڏهن_وصف_ٿيل MACOS_X
# ifdef MAC_OS_X_نسخو_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_سيٽ(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#جيڪڏهن_پورو
}

#إذا defined(EXITFREE)
    فراغ
mch_free_mem(فراغ)
{
# إذا defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    إذا (clip_تارو.owned)
	clip_lose_selection(&clip_تارو);
    إذا (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# إذا defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    إذا (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_نسخو
    // Lesstif crashes here, lose some memory
    إذا (xterm_dpy != عدم)
	XtCloseDisplay(xterm_dpy);
    إذا (app_context != (XtAppContext)عدم)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = عدم; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# إذا defined(FEAT_X11)
    إذا (x11_display != عدم
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# إذا defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ڍير);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#جيڪڏهن_پورو

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ساكن فراغ
exit_scroll(فراغ)
{
    إذا (silent_گهڻائي)
	أرجع;
    إذا (newline_on_exit || نياپو_didout)
    {
	إذا (نياپو_use_printf())
	{
	    إذا (info_پيغام)
		mch_نياپو("\n");
	    وإلا
		mch_errmsg("\r\n");
	}
	وإلا
	    ٻاهريون_char('\n');
    }
    وإلا إذا (!is_not_a_رڪن())
    {
	restore_cterm_colors();		// get original colors back
	نياپو_clr_eos_force();		// clear the rest of the display
	windgoto((صحيح)صفون - 1, cmdline_کانو_off); // may have moved the cursor
    }
}

#جيڪڏهن_وصف_ٿيل USE_GCOV_FLUSH
# إذا (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
خارجي فراغ __gcov_dump(فراغ);
خارجي فراغ __gcov_reset(فراغ);
#  define __gcov_flush() افعل { __gcov_dump(); __gcov_reset(); } طالما (0)
# وإلا
خارجي فراغ __gcov_flush(فراغ);
# endif
#جيڪڏهن_پورو

    فراغ
mch_exit(صحيح r)
{
    exiting = TRUE;

#إذا defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (!gui.اندريون_use)
#جيڪڏهن_پورو
    {
	settmode(TMODE_COOK);
	إذا (!is_not_a_رڪن())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    رڪن_ڪڍو_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	إذا (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	إذا (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	إذا (ڀريل_screen)
	    cursor_on();
    }
    ٻاهريون_flush();
    ml_close_all(TRUE);		// remove all memfiles

#جيڪڏهن_وصف_ٿيل USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#جيڪڏهن_پورو

    may_core_dump();
#جيڪڏهن_وصف_ٿيل FEAT_GUI
    إذا (gui.اندريون_use)
	gui_exit(r);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل MACOS_CONVERT
    mac_conv_cleanup();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    إذا (deadly_signal != 0)
	أرجع;
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل EXITFREE
    free_all_mem();
#جيڪڏهن_پورو

    ٻاهر_نڪرو(r);
}

    ساكن فراغ
may_core_dump(فراغ)
{
    إذا (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#جيڪڏهن_وصف_نه_ٿيل VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ساكن صحيح
get_tty_fd(صحيح fd)
{
    صحيح		tty_fd = fd;

# إذا defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    إذا (mch_isatty(fd) > 1)
    {
	حرف *name;

	name = ptsname(fd);
	إذا (name == عدم)
	    أرجع -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	إذا (tty_fd < 0)
	    أرجع -1;
    }
# endif
    أرجع tty_fd;
}

    ساكن صحيح
mch_tcgetattr(صحيح fd, فراغ *رڪن)
{
    صحيح		tty_fd;
    صحيح		retval = -1;

    tty_fd = get_tty_fd(fd);
    إذا (tty_fd < 0)
	أرجع -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (بنية termios *)رڪن);
#  وإلا
    retval = ioctl(tty_fd, TCGETA, (بنية termio *)رڪن);
#  endif
# وإلا
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (بنية sgttyb *)رڪن);
# endif
    إذا (tty_fd != fd)
	close(tty_fd);
    أرجع retval;
}

    فراغ
mch_settmode(tmode_T tmode)
{
    ساكن صحيح پهريون = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ساكن بنية termios told;
	   بنية termios tnew;
#  وإلا
    ساكن بنية termio told;
	   بنية termio tnew;
#  endif

    إذا (پهريون)
    {
	پهريون = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    إذا (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == عدم || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  إذا defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   وإلا
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    وإلا
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    وإلا إذا (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  إذا defined(HAVE_TERMIOS_H)
    {
	صحيح	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	طالما (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  وإلا
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# وإلا
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ساكن بنية sgttyb ttybold;
	   بنية sgttyb ttybnew;

    إذا (پهريون)
    {
	پهريون = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    إذا (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    وإلا إذا (tmode == TMODE_SLEEP)
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

    إذا (get_tty_info(read_cmd_fd, &info) != OK)
	أرجع;

    intr_char = info.مداخلت;
    buf[0] = info.backspace;
    buf[1] = NUL;
    جوڙيو_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = ڳوليو_termcode((char_u *)"kD");
    إذا (p != عدم && p[0] == buf[0] && p[1] == buf[1])
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
    بنية termios ڪنجيون;
#  وإلا
    بنية termio ڪنجيون;
#  endif

    إذا (mch_tcgetattr(fd, &ڪنجيون) != -1)
    {
	info->backspace = ڪنجيون.c_cc[VERASE];
	info->مداخلت = ڪنجيون.c_cc[VINTR];
	إذا (ڪنجيون.c_iflag & ICRNL)
	    info->enter = NL;
	وإلا
	    info->enter = CAR;
	إذا (ڪنجيون.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	وإلا
	    info->nl_does_cr = FALSE;
	أرجع OK;
    }
# وإلا
    // for "old" tty systems
    بنية sgttyb ڪنجيون;

    إذا (mch_tcgetattr(fd, &ڪنجيون) != -1)
    {
	info->backspace = ڪنجيون.sg_erase;
	info->مداخلت = ڪنجيون.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	أرجع OK;
    }
# endif
    أرجع FAIL;
}

#جيڪڏهن_پورو // VMS

ساكن صحيح	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    فراغ
mch_setmouse(صحيح on)
{
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_رڪن
    ساكن صحيح	bevalterm_ison = FALSE;
#جيڪڏهن_پورو
    صحيح		xterm_mouse_vers;

#إذا defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    إذا (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#جيڪڏهن_پورو

    إذا (on == mouse_ison
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_رڪن
	    && p_bevalterm == bevalterm_ison
#جيڪڏهن_پورو
	    )
	// return quickly if nothing to do
	أرجع;

    xterm_mouse_vers = use_xterm_mouse();

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_URXVT
    إذا (ttym_flags == TTYM_URXVT)
    {
	ٻاهريون_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#جيڪڏهن_پورو

    إذا (T_CXM != عدم && *T_CXM != NUL)
    {
	رڪن_enable_mouse(on);
    }
    وإلا إذا (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	ٻاهريون_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_رڪن
    إذا (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	إذا (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    ٻاهريون_str_nf((char_u *)("\033[?1003l"));
    }
#جيڪڏهن_پورو

    إذا (xterm_mouse_vers > 0)
    {
	إذا (on)	// enable mouse events, use mouse tracking if available
	    ٻاهريون_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#جيڪڏهن_وصف_ٿيل FEAT_BEVAL_رڪن
			    bevalterm_ison ? "\033[?1003h" :
#جيڪڏهن_پورو
			      "\033[?1002h")
			: "\033[?1000h"));
	وإلا	// disable mouse events, could probably always send the same
	    ٻاهريون_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_DEC
    وإلا إذا (ttym_flags == TTYM_DEC)
    {
	إذا (on)	// enable mouse events
	    ٻاهريون_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	وإلا	// disable mouse events
	    ٻاهريون_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_GPM
    وإلا
    {
	إذا (on)
	{
	    إذا (gpm_open())
		mouse_ison = TRUE;
	}
	وإلا
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SYSMOUSE
    وإلا
    {
	إذا (on)
	{
	    إذا (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	وإلا
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_JSB
    وإلا
    {
	إذا (on)
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
	    ٻاهريون_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# وإلا
	    ٻاهريون_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	وإلا
	{
	    ٻاهريون_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_PTERM
    وإلا
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	إذا (on)
	    ٻاهريون_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	وإلا
	    ٻاهريون_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#جيڪڏهن_پورو
}

#إذا defined(FEAT_BEVAL_رڪن)
/*
 * Called when 'balloonevalterm' changed.
 */
    فراغ
mch_bevalterm_بدليو(فراغ)
{
    mch_setmouse(mouse_ison);
}
#جيڪڏهن_پورو

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    فراغ
check_mouse_termcode(فراغ)
{
#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_XTERM
    إذا (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
    {
	سيٽ_mouse_termcode(KS_MOUSE, (char_u *)(رڪن_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	إذا (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وإلا
	del_mouse_termcode(KS_MOUSE);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_GPM
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    وإلا
	del_mouse_termcode(KS_GPM_MOUSE);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_SYSMOUSE
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    وإلا
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    وإلا
	del_mouse_termcode(KS_NETTERM_MOUSE);
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_DEC_MOUSE, (char_u *)(رڪن_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    وإلا
	del_mouse_termcode(KS_DEC_MOUSE);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    إذا (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
	سيٽ_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    وإلا
	del_mouse_termcode(KS_PTERM_MOUSE);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_URXVT
    إذا (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.اندريون_use
# endif
	    )
    {
	سيٽ_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(رڪن_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	إذا (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وإلا
	del_mouse_termcode(KS_URXVT_MOUSE);
#جيڪڏهن_پورو
    إذا (use_xterm_mouse() == 4
#جيڪڏهن_وصف_ٿيل FEAT_GUI
	    && !gui.اندريون_use
#جيڪڏهن_پورو
	    )
    {
	سيٽ_mouse_termcode(KS_SGR_MOUSE, (char_u *)(رڪن_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	سيٽ_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(رڪن_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	إذا (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    وإلا
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#جيڪڏهن_وصف_نه_ٿيل VMS

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
    ڊگهو	صفون_2 = 0;
    ڊگهو	columns = 0;
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
	إذا (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	إذا (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_کانو;
	    صفون_2 = ws.ws_صف;
#  ifdef FEAT_EVAL
	    ch_log(عدم, "Got size with TIOCGWINSZ: %ld x %ld", columns, صفون_2);
#  endif
	}
    }
# وإلا // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	بنية ttysize	ts;
	صحيح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	إذا (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	إذا (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_کانن_ڳڻپ;
	    صفون_2 = ts.ts_سٽون;
#   ifdef FEAT_EVAL
	    ch_log(عدم, "Got size with TIOCGSIZE: %ld x %ld", columns, صفون_2);
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
    إذا (columns == 0 || صفون_2 == 0 || vim_strchr(p_cpo, CPO_TSIZE) != عدم)
    {
	إذا ((p = (char_u *)getenv("LINES")))
	{
	    صفون_2 = atoi((حرف *)p);
# ifdef FEAT_EVAL
	    ch_log(عدم, "Got 'lines' from $LINES: %ld", صفون_2);
# endif
	}
	إذا ((p = (char_u *)getenv("COLUMNS")))
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
    إذا (columns == 0 || صفون_2 == 0)
    {
	getlinecol(&columns, &صفون_2);
#  ifdef FEAT_EVAL
	ch_log(عدم, "Got size from termcap: %ld x %ld", columns, صفون_2);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    إذا (columns <= 0 || صفون_2 <= 0)
	أرجع FAIL;

    صفون = صفون_2;
    Columns = columns;
    حد_بندي_screen_سائيز();
    أرجع OK;
}

# إذا defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    فراغ
mch_calc_cell_سائيز(بنية cellsize *cs_ٻاهريون)
{
   بنية winsize   ws;
   صحيح		    retval;
   صحيح		    x_cell_سائيز;
   صحيح		    y_cell_سائيز;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(عدم, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   إذا (retval == -1 || ws.ws_کانو == 0 || ws.ws_صف == 0)
   {
	cs_ٻاهريون->cs_xpixel = -1;
	cs_ٻاهريون->cs_ypixel = -1;
	أرجع;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_سائيز = ws.ws_xpixel / ws.ws_کانو;
   y_cell_سائيز = ws.ws_ypixel / ws.ws_صف;

   cs_ٻاهريون->cs_xpixel = x_cell_سائيز > 0 ? x_cell_سائيز : -1;
   cs_ٻاهريون->cs_ypixel = y_cell_سائيز > 0 ? y_cell_سائيز : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   إذا (x_cell_سائيز <= 0 || y_cell_سائيز <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       ٻاهريون_STR("\033[14t");
       ٻاهريون_STR("\033[16t");
       ٻاهريون_flush();
   }
}
# endif

# إذا defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    صحيح
mch_report_winsize(صحيح fd, صحيح صفون_2, صحيح کانن_ڳڻپ)
{
    صحيح		tty_fd;
    صحيح		retval = -1;

    tty_fd = get_tty_fd(fd);
    إذا (tty_fd < 0)
	أرجع FAIL;

#  إذا defined(TIOCSWINSZ)
    بنية winsize ws;

    ws.ws_کانو = کانن_ڳڻپ;
    ws.ws_صف = صفون_2;
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

    ts.ts_کانن_ڳڻپ = کانن_ڳڻپ;
    ts.ts_سٽون = صفون_2;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(عدم, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    إذا (tty_fd != fd)
	close(tty_fd);
    أرجع retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    فراغ
mch_سيٽ_shellsize(فراغ)
{
    إذا (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	رڪن_سيٽ_winsize((صحيح)صفون, (صحيح)Columns);
	ٻاهريون_flush();
	screen_شروعات();			// don't know where cursor is now
    }
}

#جيڪڏهن_پورو // VMS

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
wait4pid(pid_t ٻار, waitstatus *حالت)
{
    pid_t wait_pid = 0;
    ڊگهو delay_msec = 1;

    طالما (wait_pid != ٻار)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(ٻار, حالت, WNOHANG);
	إذا (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    إذا (++delay_msec > 10)
		delay_msec = 10;
	    استمر;
	}
	إذا (wait_pid <= 0
#جيڪڏهن_وصف_ٿيل ECHILD
		&& errno == ECHILD
#جيڪڏهن_پورو
	   )
	    اكسر;
    }
    أرجع wait_pid;
}

#إذا !defined(USE_SYSTEM) || defined(FEAT_ڪم_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ساكن فراغ
سيٽ_ٻار_environment(
	ڊگهو	صفون_2,
	ڊگهو	columns,
	حرف	*رڪن,
	صحيح	is_terminal اڻ_استعمال_ٿيل)
{
# ifdef HAVE_SETENV
    حرف	envbuf[50];
# وإلا
    ساكن حرف	envbuf_رڪن[30];
    ساكن حرف	envbuf_صفون[20];
    ساكن حرف	envbuf_سٽون[20];
    ساكن حرف	envbuf_Columns[20];
    ساكن حرف	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ساكن حرف	envbuf_نسخو[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ساكن حرف	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", رڪن, 1);
    sprintf((حرف *)envbuf, "%ld", صفون_2);
    setenv("ROWS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", صفون_2);
    setenv("LINES", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", columns);
    setenv("COLUMNS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%d", t_colors);
    setenv("COLORS", (حرف *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    إذا (is_terminal)
    {
	sprintf((حرف *)envbuf, "%ld",  (ڊگهو)get_vim_var_nr(VV_نسخو));
	setenv("VIM_TERMINAL", (حرف *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == عدم ? "" : (حرف *)serverName, 1);
#  endif
# وإلا
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_رڪن, حجم(envbuf_رڪن), "TERM=%s", رڪن);
    putenv(envbuf_رڪن);
    vim_snprintf(envbuf_صفون, حجم(envbuf_صفون), "ROWS=%ld", صفون_2);
    putenv(envbuf_صفون);
    vim_snprintf(envbuf_سٽون, حجم(envbuf_سٽون), "LINES=%ld", صفون_2);
    putenv(envbuf_سٽون);
    vim_snprintf(envbuf_Columns, حجم(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, حجم(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    إذا (is_terminal)
    {
	vim_snprintf(envbuf_نسخو, حجم(envbuf_نسخو),
			 "VIM_TERMINAL=%ld", (ڊگهو)get_vim_var_nr(VV_نسخو));
	putenv(envbuf_نسخو);
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
سيٽ_default_ٻار_environment(صحيح is_terminal)
{
    سيٽ_ٻار_environment(صفون, Columns, "dumb", is_terminal);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI) || defined(FEAT_ڪم_CHANNEL)
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

    إذا (name1 != عدم)
	*name1 = عدم;
    إذا (name2 != عدم)
	*name2 = عدم;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    إذا (*pty_master_fd < 0)
	أرجع;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# وإلا
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    إذا (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    وإلا
    {
	إذا (name1 != عدم)
	    *name1 = vim_strsave((char_u *)tty_name);
	إذا (name2 != عدم)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#جيڪڏهن_پورو

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ساكن فراغ
may_send_sigint(صحيح c اڻ_استعمال_ٿيل, pid_t pid اڻ_استعمال_ٿيل, pid_t wpid اڻ_استعمال_ٿيل)
{
#جيڪڏهن_وصف_ٿيل SIGINT
    إذا (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# وإلا
	kill(0, SIGINT);
# endif
	إذا (wpid > 0)
	    kill(wpid, SIGINT);
    }
#جيڪڏهن_پورو
}

#إذا !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

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
    إذا (*sh_tofree == عدم)		// out of memory
	أرجع FAIL;

    إذا (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	أرجع FAIL;
    *argvp = argv;

    إذا (cmd != عدم)
    {
	char_u	*s;
	char_u	*p;

	إذا (extra_shell_arg != عدم)
	    argv[argc++] = (حرف *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	إذا (*shcf_tofree == عدم)    // out of memory
	    أرجع FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	طالما (*p != NUL)
	{
	    argv[argc++] = (حرف *)s;
	    طالما (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (حرف *)cmd;
    }
    argv[argc] = عدم;
    أرجع OK;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساكن صحيح
mch_call_shell_terminal(
    char_u	*cmd,
    صحيح		options اڻ_استعمال_ٿيل)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    حرف	**argv = عدم;
    char_u	*tofree1 = عدم;
    char_u	*tofree2 = عدم;
    صحيح		retval = -1;
    buf_T	*buf;
    ڪم_T	*ڪم;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	اذهب theend;

    init_ڪم_options(&opt);
    ch_log(عدم, "starting terminal for system command '%s'", cmd);
    buf = رڪن_شروعات(عدم, argv, &opt, رڪن_شروعات_SYSTEM);
    إذا (buf == عدم)
	اذهب theend;

    ڪم = رڪن_getjob(buf->b_رڪن);
    ++ڪم->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    إذا (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	طالما (رڪن_use_loop())
	{
	    إذا (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_سرگرم)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		إذا (terminal_loop(TRUE) == OK)
		    عام_cmd(&oa, TRUE);
	    }
	    وإلا
		عام_cmd(&oa, TRUE);
	}
	retval = ڪم->jv_exitval;
	ch_log(عدم, "system command finished");

	ڪم_unref(ڪم);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    إذا (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_بفر(DOBUF_WIPE, DOBUF_پهريون, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    أرجع retval;
}
#جيڪڏهن_پورو

#جيڪڏهن_وصف_ٿيل USE_SYSTEM
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

    ٻاهريون_flush();

    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# إذا defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  إذا defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    إذا (cmd == عدم)
	x = system((حرف *)p_sh);
    وإلا
    {
# ifdef VMS
	إذا (ofn = strchr((حرف *)cmd, '>'))
	    *ofn++ = '\0';
	إذا (ifn = strchr((حرف *)cmd, '<'))
	{
	    حرف *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    إذا (p)
		*p = '\0';
	}
	إذا (ofn)
	    x = vms_sys((حرف *)cmd, ofn, ifn);
	وإلا
	    x = system((حرف *)cmd);
# وإلا
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == عدم ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	إذا (newcmd == عدم)
	    x = 0;
	وإلا
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
    x = vms_sys_حالت(x);
# endif
    إذا (emsg_silent)
	;
    وإلا إذا (x == 127)
	نياپو_puts(_("\nCannot execute shell sh\n"));
    وإلا إذا (x && !(options & SHELL_SILENT))
    {
	نياپو_puts(_("\nshell returned "));
	نياپو_outnum((ڊگهو)x);
	نياپو_putchar('\n');
    }

    إذا (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# إذا defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    أرجع x;
}

#وإلا // USE_SYSTEM

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
    صحيح		حالت = -1;
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

    ٻاهريون_flush();
    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    إذا (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    إذا (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	اذهب error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.اندريون_use && show_shell_mess)
# endif
		    )
    {
# ifdef FEAT_GUI
	/*
	 * Try to open a master pty.
	 * If this works, open the slave pty.
	 * If the slave can't be opened, close the master pty.
	 */
	إذا (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, عدم, عدم);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	إذا (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    إذا (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		إذا (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    إذا (pipe_error)
	    {
		نياپو_puts(_("\nCannot create pipes\n"));
		ٻاهريون_flush();
	    }
	}
    }

    إذا (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	بلاڪ_سگنل(&curset);
	pid = fork();	// maybe we should use vfork()
	إذا (pid == -1)
	{
	    UNBLOCK_سگنل(&curset);

	    نياپو_puts(_("\nCannot fork\n"));
	    إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.اندريون_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		إذا (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		وإلا				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	وإلا إذا (pid == 0)	// child
	{
	    reset_سگنل();		// handle signals normally
	    UNBLOCK_سگنل(&curset);

# ifdef FEAT_EVAL
	    إذا (ch_log_سرگرم())
	    {
		ch_log(عدم, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    إذا (!show_shell_mess || (options & SHELL_EXPAND))
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
		إذا (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    وإلا إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.اندريون_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		إذا (p_stmp)
		{
		    (فراغ)setsid();
#  إذا defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		إذا (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    إذا (options & SHELL_COOKED)
			تياري_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)عدم);
#  endif
		}
# endif
		سيٽ_default_ٻار_environment(FALSE);

		/*
		 * stderr is only redirected when using the GUI, so that a
		 * program like gpg can still access the terminal to get a
		 * passphrase using stderr.
		 */
# ifdef FEAT_GUI
		إذا (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    إذا (gui.اندريون_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		وإلا
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
		    إذا (gui.اندريون_use)
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
	وإلا			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_سگنل(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_سگنل(&curset);
# ifdef FEAT_ڪم_CHANNEL
	    ++dont_check_ڪم_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    إذا ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.اندريون_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    بفر[BUFLEN + 1];
		صحيح	    بفر_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		صحيح	    ta_len = 0;		// valid bytes in ta_buf[]
		صحيح	    len;
		صحيح	    p_وڌيڪ_save;
		صحيح	    old_State;
		صحيح	    c;
		صحيح	    toshell_fd;
		صحيح	    fromshell_fd;
		garray_T    ga;
		صحيح	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   شروعات_tv;
# endif

# ifdef FEAT_GUI
		إذا (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		وإلا
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
		p_وڌيڪ_save = p_وڌيڪ;
		p_وڌيڪ = FALSE;
		old_State = State;
		State = گهڻائي_EXTERNCMD;	// don't redraw at window resize

		إذا ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    إذا ((wpid = fork()) == -1)
		    {
			نياپو_puts(_("\nCannot fork\n"));
		    }
		    وإلا إذا (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_شروعات.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			لكل (;;)
			{
			    إذا (lplen == 0)
				len = 0;
			    وإلا إذا (lp[written] == NL)
				// NL -> NUL translation
				len = اكتب(toshell_fd, "", (size_t)1);
			    وإلا
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = اكتب(toshell_fd, (حرف *)lp + written,
					   s == عدم ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    إذا (len == (صحيح)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				إذا (lnum != curbuf->b_op_پڇاڙي.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_سٽ_ڳڻپ
						    || curbuf->b_p_eol)))
				    vim_ignored = اكتب(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				إذا (lnum > curbuf->b_op_پڇاڙي.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    اكسر;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    وإلا إذا (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    وإلا // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		إذا (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(شروعات_tv);
# endif
		لكل (;;)
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
		    إذا (!(options & SHELL_EXPAND)
			    && ((options &
					 (SHELL_READ|SHELL_WRITE|SHELL_COOKED))
				      != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
						    || gui.اندريون_use
# endif
						    )
			    && wait_pid == 0
			    && (ta_len > 0 || noread_cnt > 4))
		    {
		      إذا (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(شروعات_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      إذا (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			إذا (len == 1 && (pty_master_fd < 0 || cmd != عدم))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    إذا (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				close(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = رڪن_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			إذا (pty_master_fd < 0)
			{
			    لكل (i = ta_len; i < ta_len + len; ++i)
			    {
				إذا (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    نياپو_putchar(ta_buf[i]);
				وإلا إذا (has_mbyte)
				{
				    صحيح l = (*mb_ptr2len)(ta_buf + i);

				    نياپو_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				وإلا
				    نياپو_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
			    ٻاهريون_flush();
			}

			ta_len += len;

			/*
			 * Write the characters to the child, unless EOF has
			 * been typed for pipes.  Write one character at a
			 * time, to avoid losing too much typeahead.
			 * When writing buffer lines, drop the typed
			 * characters (only check for CTRL-C).
			 */
			إذا (options & SHELL_WRITE)
			    ta_len = 0;
			وإلا إذا (toshell_fd >= 0)
			{
			    len = اكتب(toshell_fd, (حرف *)ta_buf, (size_t)1);
			    إذا (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    إذا (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# وإلا
			kill(0, SIGINT);
# endif
			إذا (wpid > 0)
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
		    طالما (RealWaitForChar(fromshell_fd, 10L, عدم, عدم))
		    {
			len = read_eintr(fromshell_fd, بفر
				+ بفر_off, (size_t)(BUFLEN - بفر_off)
				);
			إذا (len <= 0)		    // end of file or error
			    اذهب ختم;

			noread_cnt = 0;
			إذا (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    لكل (i = 0; i < len; ++i)
			    {
				إذا (بفر[i] == NL)
				    append_ga_سٽ(&ga);
				وإلا إذا (بفر[i] == NUL)
				    ga_append(&ga, NL);
				وإلا
				    ga_append(&ga, بفر[i]);
			    }
			}
			وإلا إذا (has_mbyte)
			{
			    صحيح		l;
			    char_u	*p;

			    len += بفر_off;
			    بفر[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    لكل (p = بفر; p < بفر + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				إذا (l == 0)
				    l = 1;  // NUL byte?
				وإلا إذا (MB_BYTE2LEN(*p) != l)
				    اكسر;
			    }
			    إذا (p == بفر)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				إذا (len >= 12)
				    ++p;
				وإلا
				{
				    بفر_off = len;
				    استمر;
				}
			    }
			    c = *p;
			    *p = NUL;
			    نياپو_puts((حرف *)بفر);
			    إذا (p < بفر + len)
			    {
				*p = c;
				بفر_off = (بفر + len) - p;
				mch_memmove(بفر, p, بفر_off);
				استمر;
			    }
			    بفر_off = 0;
			}
			وإلا
			{
			    بفر[len] = NUL;
			    نياپو_puts((حرف *)بفر);
			}

			windgoto(نياپو_صف, cmdline_کانو_off + نياپو_کانو);
			cursor_on();
			ٻاهريون_flush();
			إذا (got_int)
			    اكسر;

# ifdef ELAPSED_FUNC
			إذا (wait_pid == 0)
			{
			    ڊگهو	msec = ELAPSED_FUNC(شروعات_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    إذا (msec > 2000)
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
		    إذا (wait_pid == pid)
		    {
			إذا (noread_cnt < 5)
			    استمر;
			اكسر;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &حالت, WNOHANG);
		    إذا ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(حالت)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    وإلا
			wait_pid = 0;

# إذا defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
# endif
# ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
# endif
		}
ختم:
		p_وڌيڪ = p_وڌيڪ_save;
		إذا (options & SHELL_READ)
		{
		    إذا (ga.ga_len > 0)
		    {
			append_ga_سٽ(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    وإلا
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		إذا (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		إذا (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# إذا (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    وإلا
	    {
		ڊگهو delay_msec = 1;

		إذا (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    ٻاهريون_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		لكل (;;)
		{
		    إذا (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  وإلا
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &حالت, WNOHANG);
		    إذا ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(حالت)))
		    {
			wait_pid = pid;
			اكسر;
		    }

#  إذا defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    إذا (++delay_msec > 10)
			delay_msec = 10;
		}

		إذا (tmode == TMODE_RAW)
		    // possibly enables modifyOtherKeys again
		    ٻاهريون_str_t_TI();
	    }
# endif

	    /*
	     * Wait until our child has exited.
	     * Ignore wait() returning pids of other children and returning
	     * because of some signal like SIGWINCH.
	     * Don't wait if wait_pid was already set above, indicating the
	     * child already exited.
	     */
	    إذا (wait_pid != pid)
		(فراغ)wait4pid(pid, &حالت);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    إذا (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    إذا (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, عدم);
	    }

# ifdef FEAT_ڪم_CHANNEL
	    --dont_check_ڪم_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    إذا (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    سيٽ_سگنل();

	    إذا (WIFEXITED(حالت))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(حالت);
		إذا (retval != 0 && !emsg_silent)
		{
		    إذا (retval == EXEC_FAILED)
		    {
			نياپو_puts(_("\nCannot execute shell "));
			نياپو_outtrans(p_sh);
			نياپو_putchar('\n');
		    }
		    وإلا إذا (!(options & SHELL_SILENT))
		    {
			نياپو_puts(_("\nshell returned "));
			نياپو_outnum((ڊگهو)retval);
			نياپو_putchar('\n');
		    }
		}
	    }
	    وإلا
		نياپو_puts(_("\nCommand terminated\n"));
	}
    }

error:
    إذا (!did_settmode)
	إذا (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    أرجع retval;
}
#جيڪڏهن_پورو // USE_SYSTEM

    صحيح
mch_call_shell(
    char_u	*cmd,
    صحيح		options)	// SHELL_*, see vim.h
{
#جيڪڏهن_وصف_ٿيل FEAT_EVAL
    ch_log(عدم, "executing shell command: %s", cmd);
#جيڪڏهن_پورو
#إذا defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    إذا (gui.اندريون_use && vim_strchr(p_go, GO_TERMINAL) != عدم
					      && (options & SHELL_SILENT) == 0)
	أرجع mch_call_shell_terminal(cmd, options);
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل USE_SYSTEM
    أرجع mch_call_shell_system(cmd, options);
#وإلا
    أرجع mch_call_shell_fork(cmd, options);
#جيڪڏهن_پورو
}

#إذا defined(FEAT_EVAL)
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
mch_get_cmd_output_سڌو(
    حرف	**argv,
    char_u	*infile,
    صحيح		flags اڻ_استعمال_ٿيل,
    صحيح		*ret_len)
{
    pid_t	pid;
    صحيح		fd_ٻاهريون[2] = {-1, -1};
    صحيح		حالت = -1;
    char_u	*بفر = عدم;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(عدم, "directly executing: %s", argv[0]);

    إذا (pipe(fd_ٻاهريون) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	أرجع عدم;
    }

    بلاڪ_سگنل(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# وإلا
    pid = fork();
# endif
    إذا (pid == -1)
    {
	UNBLOCK_سگنل(&curset);
	close(fd_ٻاهريون[0]);
	close(fd_ٻاهريون[1]);
	emsg(_("\nCannot fork\n"));
	أرجع عدم;
    }

    إذا (pid == 0)
    {
	// child process
	reset_سگنل();
	UNBLOCK_سگنل(&curset);

	إذا (ch_log_سرگرم())
	{
	    ch_log(عدم, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	إذا (infile != عدم)
	{
	    صحيح fd_اندريون = open((حرف *)infile, O_RDONLY);
	    إذا (fd_اندريون >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_اندريون);
		close(fd_اندريون);
	    }
	}
	وإلا
	{
	    صحيح nullfd = open("/dev/null", O_RDONLY);
	    إذا (nullfd >= 0)
	    {
		close(0);
		vim_ignored = dup(nullfd);
		close(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	close(fd_ٻاهريون[0]);
	close(1);
	vim_ignored = dup(fd_ٻاهريون[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_ٻاهريون[1]);
	close(fd_ٻاهريون[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_سگنل(&curset);
    close(fd_ٻاهريون[1]);

    // Read output from child.
    لكل (;;)
    {
	حرف	buf[4096];
	صحيح	n;

	n = (صحيح)اقرأ(fd_ٻاهريون[0], buf, حجم(buf));
	إذا (n <= 0)
	    اكسر;
	إذا (ga_grow(&ga, n) == FAIL)
	    اكسر;
	mch_memmove((حرف *)ga.ga_معلومات + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_ٻاهريون[0]);

    // Wait for child to finish.
    (فراغ)waitpid(pid, &حالت, 0);
    إذا (WIFEXITED(حالت))
	حالت = WEXITSTATUS(حالت);
    وإلا
	حالت = -1;
    سيٽ_vim_var_nr(VV_SHELL_ERROR, (ڊگهو)حالت);

    إذا (ga.ga_len > 0)
    {
	بفر = alloc(ga.ga_len + 1);
	إذا (بفر != عدم)
	{
	    mch_memmove(بفر, ga.ga_معلومات, ga.ga_len);
	    إذا (ret_len == عدم)
	    {
		صحيح	i;

		// Change NUL into SOH, otherwise the string is truncated.
		لكل (i = 0; i < ga.ga_len; ++i)
		    إذا (بفر[i] == NUL)
			بفر[i] = 1;
		بفر[ga.ga_len] = NUL;
	    }
	    وإلا
		*ret_len = ga.ga_len;
	}
    }
    وإلا إذا (ret_len != عدم)
	*ret_len = 0;

    ga_clear(&ga);
    أرجع بفر;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_ڪم_CHANNEL)
    فراغ
mch_ڪم_شروعات(حرف **argv, ڪم_T *ڪم, jobopt_T *options, صحيح is_terminal)
{
    pid_t	pid;
    صحيح		fd_اندريون[2] = {-1, -1};	// for stdin
    صحيح		fd_ٻاهريون[2] = {-1, -1};	// for stdout
    صحيح		fd_err[2] = {-1, -1};	// for stderr
    صحيح		pty_master_fd = -1;
    صحيح		pty_slave_fd = -1;
    channel_T	*channel = عدم;
    صحيح		use_null_for_اندريون = options->jo_io[PART_اندريون] == JIO_NULL;
    صحيح		use_null_for_ٻاهريون = options->jo_io[PART_ٻاهريون] == JIO_NULL;
    صحيح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحيح		use_file_for_اندريون = options->jo_io[PART_اندريون] == JIO_FILE;
    صحيح		use_file_for_ٻاهريون = options->jo_io[PART_ٻاهريون] == JIO_FILE;
    صحيح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحيح		use_بفر_for_اندريون = options->jo_io[PART_اندريون] == JIO_بفر;
    صحيح		use_ٻاهريون_for_err = options->jo_io[PART_ERR] == JIO_ٻاهريون;
    SIGSET_DECL(curset)

    إذا (use_ٻاهريون_for_err && use_null_for_ٻاهريون)
	use_null_for_err = TRUE;

    // default is to fail
    ڪم->jv_حالت = ڪم_FAILED;

    إذا (options->jo_pty
	    && (!(use_file_for_اندريون || use_null_for_اندريون)
		|| !(use_file_for_ٻاهريون || use_null_for_ٻاهريون)
		|| !(use_ٻاهريون_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &ڪم->jv_tty_ٻاهريون, &ڪم->jv_tty_اندريون);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    إذا (use_file_for_اندريون)
    {
	char_u *fname = options->jo_io_name[PART_اندريون];

	fd_اندريون[0] = mch_open((حرف *)fname, O_RDONLY, 0);
	إذا (fd_اندريون[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	إذا (!use_null_for_اندريون && (pty_master_fd < 0 || use_بفر_for_اندريون)
							    && pipe(fd_اندريون) < 0)
	    اذهب failed;

    إذا (use_file_for_ٻاهريون)
    {
	char_u *fname = options->jo_io_name[PART_ٻاهريون];

	fd_ٻاهريون[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	إذا (fd_ٻاهريون[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    وإلا إذا (!use_null_for_ٻاهريون && pty_master_fd < 0 && pipe(fd_ٻاهريون) < 0)
	اذهب failed;

    إذا (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	إذا (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    اذهب failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    وإلا إذا (!use_ٻاهريون_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_سيٽ & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	اذهب failed;

    إذا (!use_null_for_اندريون || !use_null_for_ٻاهريون || !use_null_for_err)
    {
	إذا (options->jo_سيٽ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    إذا (channel != عدم)
		++channel->ch_refcount;
	}
	وإلا
	    channel = جوڙيو_channel();
	إذا (channel == عدم)
	    اذهب failed;
	إذا (ڪم->jv_tty_ٻاهريون != عدم)
	    ch_log(channel, "using pty %s on fd %d",
					       ڪم->jv_tty_ٻاهريون, pty_master_fd);
    }

    بلاڪ_سگنل(&curset);
    pid = fork();	// maybe we should use vfork()
    إذا (pid == -1)
    {
	// failed to fork
	UNBLOCK_سگنل(&curset);
	اذهب failed;
    }
    إذا (pid == 0)
    {
	صحيح	null_fd = -1;
	صحيح	stderr_هلي = TRUE;

	// child
	reset_سگنل();		// handle signals normally
	UNBLOCK_سگنل(&curset);

# ifdef FEAT_EVAL
	إذا (ch_log_سرگرم())
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
	إذا (options->jo_رڪن_صفون > 0)
	{
	    حرف *رڪن = (حرف *)T_NAME;

#  ifdef FEAT_GUI
	    إذا (رڪن_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		رڪن = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    إذا (رڪن == عدم || *رڪن == NUL || STRNCMP(رڪن, "xterm", 5) != 0)
	    {
		إذا (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    رڪن = "xterm-256color";
		وإلا إذا (t_colors > 16)
		    رڪن = "xterm-color";
		وإلا
		    رڪن = "xterm";
	    }
	    سيٽ_ٻار_environment(
		    (ڊگهو)options->jo_رڪن_صفون,
		    (ڊگهو)options->jo_رڪن_کانن_ڳڻپ,
		    رڪن,
		    is_terminal);
	}
	وإلا
# endif
	    سيٽ_default_ٻار_environment(is_terminal);

	إذا (options->jo_env != عدم)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    صحيح		todo = (صحيح)dict->dv_hashtab.ht_استعمال_ٿيل;

	    FOR_ALL_HASHTAB_شيون(&dict->dv_hashtab, hi, todo)
		إذا (!HASHITEM_خالي_جايون(hi))
		{
		    typval_T *شي = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ڪنجي, tv_get_string(شي));
		    --todo;
		}
	}

	إذا (use_null_for_اندريون || use_null_for_ٻاهريون || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    إذا (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	إذا (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    تياري_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)عدم);
# endif
	}

	// set up stdin for the child
	close(0);
	إذا (use_null_for_اندريون && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	وإلا إذا (fd_اندريون[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وإلا
	    vim_ignored = dup(fd_اندريون[0]);

	// set up stderr for the child
	close(2);
	إذا (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_هلي = FALSE;
	}
	وإلا إذا (use_ٻاهريون_for_err)
	    vim_ignored = dup(fd_ٻاهريون[1]);
	وإلا إذا (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وإلا
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	إذا (use_null_for_ٻاهريون && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	وإلا إذا (fd_ٻاهريون[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	وإلا
	    vim_ignored = dup(fd_ٻاهريون[1]);

	إذا (fd_اندريون[0] >= 0)
	    close(fd_اندريون[0]);
	إذا (fd_اندريون[1] >= 0)
	    close(fd_اندريون[1]);
	إذا (fd_ٻاهريون[0] >= 0)
	    close(fd_ٻاهريون[0]);
	إذا (fd_ٻاهريون[1] >= 0)
	    close(fd_ٻاهريون[1]);
	إذا (fd_err[0] >= 0)
	    close(fd_err[0]);
	إذا (fd_err[1] >= 0)
	    close(fd_err[1]);
	إذا (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	إذا (null_fd >= 0)
	    close(null_fd);

	إذا (options->jo_cwd != عدم && mch_chdir((حرف *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	إذا (stderr_هلي)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_سگنل(&curset);

    ڪم->jv_pid = pid;
    ڪم->jv_حالت = ڪم_STARTED;
    ڪم->jv_channel = channel;  // ch_refcount was set above

    إذا (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    إذا (fd_اندريون[0] >= 0)
	close(fd_اندريون[0]);
    إذا (fd_ٻاهريون[1] >= 0)
	close(fd_ٻاهريون[1]);
    إذا (fd_err[1] >= 0)
	close(fd_err[1]);
    إذا (channel != عدم)
    {
	صحيح اندريون_fd = INVALID_FD;
	صحيح ٻاهريون_fd = INVALID_FD;
	صحيح err_fd = INVALID_FD;

	إذا (!(use_file_for_اندريون || use_null_for_اندريون))
	    اندريون_fd = fd_اندريون[1] >= 0 ? fd_اندريون[1] : pty_master_fd;

	إذا (!(use_file_for_ٻاهريون || use_null_for_ٻاهريون))
	    ٻاهريون_fd = fd_ٻاهريون[0] >= 0 ? fd_ٻاهريون[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	إذا (!(use_ٻاهريون_for_err || use_file_for_err || use_null_for_err))
	{
	    إذا (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    وإلا إذا (ٻاهريون_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_سيٽ_pipes(channel, اندريون_fd, ٻاهريون_fd, err_fd);
	channel_سيٽ_ڪم(channel, ڪم, options);
    }
    وإلا
    {
	إذا (fd_اندريون[1] >= 0)
	    close(fd_اندريون[1]);
	إذا (fd_ٻاهريون[0] >= 0)
	    close(fd_ٻاهريون[0]);
	إذا (fd_err[0] >= 0)
	    close(fd_err[0]);
	إذا (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    أرجع;

failed:
    channel_unref(channel);
    إذا (fd_اندريون[0] >= 0)
	close(fd_اندريون[0]);
    إذا (fd_اندريون[1] >= 0)
	close(fd_اندريون[1]);
    إذا (fd_ٻاهريون[0] >= 0)
	close(fd_ٻاهريون[0]);
    إذا (fd_ٻاهريون[1] >= 0)
	close(fd_ٻاهريون[1]);
    إذا (fd_err[0] >= 0)
	close(fd_err[0]);
    إذا (fd_err[1] >= 0)
	close(fd_err[1]);
    إذا (pty_master_fd >= 0)
	close(pty_master_fd);
    إذا (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ساكن char_u *
get_signal_name(صحيح sig)
{
    صحيح		i;
    char_u	numbuf[NUMBUFLEN];

    إذا (sig == SIGKILL)
	أرجع vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    لكل (i = 0; signal_info[i].sig != -1; i++)
	إذا (sig == signal_info[i].sig)
	    أرجع strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((حرف *)numbuf, NUMBUFLEN, "%d", sig);
    أرجع vim_strnsave(numbuf, i);
}

    حرف *
mch_ڪم_حالت(ڪم_T *ڪم)
{
    صحيح		حالت = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(ڪم->jv_pid, &حالت, WNOHANG);
    إذا (wait_pid == -1)
    {
	صحيح waitpid_errno = errno;
	إذا (waitpid_errno == ECHILD && mch_process_running(ڪم->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    أرجع "run";

	// process must have exited
	إذا (ڪم->jv_حالت < ڪم_ENDED)
	    ch_log(ڪم->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	اذهب return_dead;
    }
    إذا (wait_pid == 0)
	أرجع "run";
    إذا (WIFEXITED(حالت))
    {
	// LINTED avoid "bitwise operation on signed value"
	ڪم->jv_exitval = WEXITSTATUS(حالت);
	إذا (ڪم->jv_حالت < ڪم_ENDED)
	    ch_log(ڪم->jv_channel, "Job exited with %d", ڪم->jv_exitval);
	اذهب return_dead;
    }
    إذا (WIFSIGNALED(حالت))
    {
	ڪم->jv_exitval = -1;
	ڪم->jv_termsig = get_signal_name(WTERMSIG(حالت));
	إذا (ڪم->jv_حالت < ڪم_ENDED && ڪم->jv_termsig != عدم)
	    ch_log(ڪم->jv_channel, "Job terminated by signal \"%s\"",
							      ڪم->jv_termsig);
	اذهب return_dead;
    }
    أرجع "run";

return_dead:
    إذا (ڪم->jv_حالت < ڪم_ENDED)
	ڪم->jv_حالت = ڪم_ENDED;
    أرجع "dead";
}

    ڪم_T *
mch_detect_ended_ڪم(ڪم_T *ڪم_list)
{
    صحيح		حالت = -1;
    pid_t	wait_pid = 0;
    ڪم_T	*ڪم;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    إذا (dont_check_ڪم_ended > 0)
	أرجع عدم;
# endif

    wait_pid = waitpid(-1, &حالت, WNOHANG);
    إذا (wait_pid <= 0)
	// no process ended
	أرجع عدم;
    لكل (ڪم = ڪم_list; ڪم != عدم; ڪم = ڪم->jv_ايندڙ)
    {
	إذا (ڪم->jv_pid == wait_pid)
	{
	    إذا (WIFEXITED(حالت))
		// LINTED avoid "bitwise operation on signed value"
		ڪم->jv_exitval = WEXITSTATUS(حالت);
	    وإلا إذا (WIFSIGNALED(حالت))
	    {
		ڪم->jv_exitval = -1;
		ڪم->jv_termsig = get_signal_name(WTERMSIG(حالت));
	    }
	    إذا (ڪم->jv_حالت < ڪم_ENDED)
	    {
		ch_log(ڪم->jv_channel, "Job ended");
		ڪم->jv_حالت = ڪم_ENDED;
	    }
	    أرجع ڪم;
	}
    }
    أرجع عدم;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    صحيح
mch_signal_ڪم(ڪم_T *ڪم, char_u *how)
{
    صحيح	    sig = -1;

    إذا (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    وإلا إذا (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    وإلا إذا (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    وإلا إذا (STRCMP(how, "int") == 0)
	sig = SIGINT;
    وإلا إذا (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    وإلا إذا (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    وإلا إذا (محفوظ_isdigit(*how))
	sig = atoi((حرف *)how);
    وإلا
	أرجع FAIL;

    // Never kill ourselves!
    إذا (ڪم->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-ڪم->jv_pid, sig);
	kill(ڪم->jv_pid, sig);
    }

    أرجع OK;
}

/*
 * Clear the data related to "job".
 */
    فراغ
mch_clear_ڪم(ڪم_T *ڪم)
{
    // call waitpid because child process may become zombie
    (فراغ)waitpid(ڪم->jv_pid, عدم, WNOHANG);
}
#جيڪڏهن_پورو

#إذا defined(FEAT_TERMINAL)
    صحيح
mch_create_pty_channel(ڪم_T *ڪم, jobopt_T *options)
{
    صحيح		pty_master_fd = -1;
    صحيح		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &ڪم->jv_tty_ٻاهريون, &ڪم->jv_tty_اندريون);
    إذا (pty_master_fd < 0 || pty_slave_fd < 0)
	أرجع FAIL;
    close(pty_slave_fd);

    channel = جوڙيو_channel();
    إذا (channel == عدم)
    {
	close(pty_master_fd);
	أرجع FAIL;
    }
    إذا (ڪم->jv_tty_ٻاهريون != عدم)
	ch_log(channel, "using pty %s on fd %d",
					       ڪم->jv_tty_ٻاهريون, pty_master_fd);
    ڪم->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_سيٽ_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_سيٽ_ڪم(channel, ڪم, options);
    أرجع OK;
}
#جيڪڏهن_پورو

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    فراغ
mch_breakcheck(صحيح force)
{
    إذا ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, عدم, عدم))
	fill_داخلا_buf(FALSE);
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
WaitForChar(ڊگهو msec, صحيح *interrupted, صحيح ignore_داخلا)
{
#جيڪڏهن_وصف_ٿيل FEAT_TIMERS
    أرجع ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_داخلا) == OK;
#وإلا
    أرجع WaitForCharOrMouse(msec, interrupted, ignore_داخلا);
#جيڪڏهن_پورو
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
WaitForCharOrMouse(ڊگهو msec, صحيح *interrupted, صحيح ignore_داخلا)
{
#جيڪڏهن_وصف_ٿيل FEAT_MOUSE_GPM
    صحيح		gpm_process_wanted;
#جيڪڏهن_پورو
#جيڪڏهن_وصف_ٿيل FEAT_XCLIPBOARD
    صحيح		rest;
#جيڪڏهن_پورو
    صحيح		avail;

    إذا (!ignore_داخلا && داخلا_موجود())	    // something in inbuf[]
	أرجع 1;

#إذا defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    إذا (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	إذا (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#جيڪڏهن_پورو

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#إذا defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    إذا (do_xterm_trace())
	rest = msec;
# endif
    افعل
    {
# ifdef FEAT_XCLIPBOARD
	إذا (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    إذا (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    إذا (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	إذا (has_sound_callback_اندريون_قطار())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	إذا (!avail && !gpm_process_wanted)
# وإلا
	avail = RealWaitForChar(read_cmd_fd, msec, عدم, interrupted);
	إذا (!avail)
# endif
	{
	    إذا (!ignore_داخلا && داخلا_موجود())
		أرجع 1;
# ifdef FEAT_XCLIPBOARD
	    إذا (rest == 0 || !do_xterm_trace())
# endif
		اكسر;
	}
    }
    طالما (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#وإلا
    avail = RealWaitForChar(read_cmd_fd, msec, عدم, interrupted);
#جيڪڏهن_پورو
    أرجع avail;
}

#جيڪڏهن_وصف_نه_ٿيل VMS
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
RealWaitForChar(صحيح fd, ڊگهو msec, صحيح *check_for_gpm اڻ_استعمال_ٿيل, صحيح *interrupted)
{
    صحيح		ret;
    صحيح		نتيجو;
# إذا defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ساكن صحيح	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    ڊگهو	شروعات_msec = msec;
    elapsed_T	شروعات_tv;

    إذا (msec > 0)
	ELAPSED_INIT(شروعات_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    إذا (busy)
	أرجع 0;
# endif

# ifdef MAY_LOOP
    لكل (;;)
# endif
    {
# ifdef MAY_LOOP
	صحيح		ختم = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	صحيح		mzquantum_استعمال_ٿيل = FALSE;
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
	mzvim_check_ڌاڳا();
	إذا (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (صحيح)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_استعمال_ٿيل = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	إذا ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	إذا (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_عدد(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	إذا (check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	إذا (xsmp_icefd != -1)
	{
	    xsmp_idx = nfd;
	    fds[nfd].fd = xsmp_icefd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_ڪم_CHANNEL
	nfd = channel_poll_تياري(nfd, &fds, &towait);
#  endif
	إذا (interrupted != عدم)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	نتيجو = ret > 0 && (fds[0].revents & POLLIN);
	إذا (نتيجو == 0 && interrupted != عدم && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	إذا (ret == 0 && mzquantum_استعمال_ٿيل)
	    // MzThreads scheduling is required and timeout occurred
	    ختم = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	إذا (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	إذا (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    إذا (--ret == 0 && !داخلا_موجود())
		// Try again
		ختم = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	إذا (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	إذا (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    إذا (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_درخواستون();
		busy = FALSE;
	    }
	    وإلا إذا (fds[xsmp_idx].revents & POLLHUP)
	    {
		إذا (p_verbose > 0)
		    verb_نياپو(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    إذا (--ret == 0)
		ختم = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_ڪم_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	إذا (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# وإلا // HAVE_SELECT

	بنية timeval  tv;
	بنية timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ساكن fd_set	rfds, wfds, efds;
	صحيح		maxfd;
	ڊگهو		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_ڌاڳا();
	إذا (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_استعمال_ٿيل = TRUE;
	}
#  endif

	إذا (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	وإلا
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
	إذا ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    إذا (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	إذا (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_عدد(xterm_dpy), &rfds);
	    إذا (maxfd < Connection_عدد(xterm_dpy))
		maxfd = Connection_عدد(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	إذا (check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    إذا (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	إذا (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    إذا (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_ڪم_CHANNEL
	maxfd = channel_select_تياري(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	إذا (interrupted != عدم)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	نتيجو = ret > 0 && FD_ISSET(fd, &rfds);
	إذا (نتيجو)
	    --ret;
	وإلا إذا (interrupted != عدم && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	إذا (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    إذا (got_tstp && !اندريون_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    إذا (do_resize)
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
	إذا (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	إذا (ret == 0 && mzquantum_استعمال_ٿيل)
	    // loop if MzThreads must be scheduled and timeout occurred
	    ختم = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	إذا (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	إذا (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_عدد(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    إذا (--ret == 0 && !داخلا_موجود())
	    {
		// Try again
		ختم = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	إذا (ret > 0 && check_for_gpm != عدم && gpm_flag && gpm_fd >= 0)
	{
	    إذا (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    وإلا إذا (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	إذا (ret > 0 && xsmp_icefd != -1)
	{
	    إذا (FD_ISSET(xsmp_icefd, &efds))
	    {
		إذا (p_verbose > 0)
		    verb_نياپو(_("XSMP lost ICE connection"));
		xsmp_close();
		إذا (--ret == 0)
		    ختم = FALSE;   // keep going if event was only one
	    }
	    وإلا إذا (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_درخواستون();
		busy = FALSE;
		إذا (--ret == 0)
		    ختم = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_ڪم_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	إذا (ret >= 0)
	    (فراغ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	إذا (ختم || msec == 0)
	    اكسر;

#  إذا defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	إذا (clientserver_طريقو == CLIENTSERVER_طريقو_X11 && server_انتظار())
	    اكسر;
#  endif

	// We're going to loop around again, find out for how long
	إذا (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = شروعات_msec - ELAPSED_FUNC(شروعات_tv);
#  وإلا
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    إذا (msec <= 0)
		اكسر;	// waited long enough
	}
# endif
    }

    أرجع نتيجو;
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
    char_u	*رستو,
    صحيح		flags)		// EW_* flags
{
    أرجع unix_expandpath(gap, رستو, 0, flags, FALSE);
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
    صحيح		   انگ_pat,
    char_u	 **pat,
    صحيح		  *انگ_file,
    char_u	***file,
    صحيح		   flags)	// EW_* flags
{
    صحيح		i;
    size_t	len;
    ڊگهو	llen;
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
    char_u	*بفر;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    صحيح		shell_style = STYLE_ECHO;
    صحيح		check_spaces;
    ساكن صحيح	did_ڳوليو_nul = FALSE;
    صحيح		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ساكن string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ساكن string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *انگ_file = 0;	// default: no files found
    *file = عدم;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    إذا (!have_wildcard(انگ_pat, pat))
	أرجع save_patterns(انگ_pat, pat, انگ_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    إذا (sandbox != 0 && check_secure())
	أرجع FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    إذا (secure || restricted)
	لكل (i = 0; i < انگ_pat; ++i)
	    إذا (vim_strchr(pat[i], '`') != عدم
		    && (check_restricted() || check_secure()))
		أرجع FAIL;

    /*
     * get a name for the temp file
     */
    إذا ((tempname = vim_tempname('o', FALSE)) == عدم)
    {
	emsg(_(e_cant_get_عارضي_file_name));
	أرجع FAIL;
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
    إذا (انگ_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    وإلا إذا ((len = STRLEN(p_sh)) >= 3)
    {
	إذا (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	وإلا إذا (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    إذا (shell_style == STYLE_ECHO)
    {
	إذا (لڙي_ڳولا((حرف *)gettail(p_sh), "bash") != عدم)
	    shell_style = STYLE_GLOBSTAR;
	وإلا إذا (لڙي_ڳولا((حرف *)gettail(p_sh), "sh") != عدم)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    إذا (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.ڊيگهائي;
    وإلا إذا (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.ڊيگهائي + sh_globstar_opt.ڊيگهائي;

    لكل (i = 0; i < انگ_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# وإلا
	++len;				// add space
	لكل (j = 0; pat[i][j] != NUL; ++j)
	{
	    إذا (vim_strchr(SHELL_SPECIAL, pat[i][j]) != عدم)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    إذا (command == عدم)
    {
	// out of memory
	vim_free(tempname);
	أرجع FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    إذا (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((حرف *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)لڙي_ڳولا((حرف *)command, ")>");
	إذا (p == عدم)
	{
	    ;					    // can't happen ;-)
	}
	وإلا
	{
	    --p;
	    طالما (p > command && VIM_ISWHITE(*p))
		--p;
	    إذا (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    طالما (p > command && VIM_ISWHITE(*p))
		--p;
	    إذا (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    وإلا
    {
	إذا (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((حرف *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	وإلا إذا (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((حرف *)command, len, "print -N >%s", tempname);
	وإلا إذا (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	وإلا إذا (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	وإلا
	    commandlen = vim_snprintf((حرف *)command, len, "echo >%s", tempname);

	لكل (i = 0; i < انگ_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((حرف *)command + commandlen, len, " \"%s\"", pat[i]);
# وإلا
	    صحيح intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    لكل (j = 0; pat[i][j] != NUL; ++j)
	    {
		إذا (pat[i][j] == '`')
		    intick = !intick;
		وإلا إذا (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    إذا (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != عدم
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		وإلا إذا (!intick
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
    إذا (flags & EW_SILENT)
	show_shell_mess = FALSE;
    إذا (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    إذا (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    وإلا إذا (shell_style == STYLE_GLOB && !have_dollars(انگ_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    إذا (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = عدم;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    إذا (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	إذا (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    نياپو_putchar('\n');		// clear bottom line quickly
	    cmdline_صف = صفون - 1;	// continue on last line
# ifdef USE_SYSTEM
	    إذا (!(flags & EW_SILENT))
# endif
	    {
		نياپو(_(e_cannot_expand_wildcards));
		نياپو_شروعات();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	إذا (shell_style == STYLE_BT)
	    أرجع FAIL;
	اذهب notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((حرف *)tempname, READBIN);
    إذا (fd == عدم)
    {
	// Something went wrong, perhaps a file name with a special char.
	إذا (!(flags & EW_SILENT))
	{
	    نياپو(_(e_cannot_expand_wildcards));
	    نياپو_شروعات();		// don't overwrite this message
	}
	vim_free(tempname);
	اذهب notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    إذا (llen < 0)
	// just in case ftell() would fail
	بفر = عدم;
    وإلا
	بفر = alloc(llen + 1);
    إذا (بفر == عدم)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	أرجع FAIL;
    }
    len = llen;
    i = fread((حرف *)بفر, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    إذا (i != (صحيح)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(بفر);
	أرجع FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = بفر;
    لكل (i = 0; i < (صحيح)len; ++i)
	إذا (!(بفر[i] == CAR && بفر[i + 1] == NL))
	    *p++ = بفر[i];
    len = p - بفر;
# endif


    // file names are separated with Space
    إذا (shell_style == STYLE_ECHO)
    {
	بفر[len] = '\n';		// make sure the buffer ends in NL
	p = بفر;
	لكل (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    طالما (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    وإلا إذا (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	بفر[len] = NUL;		// make sure the buffer ends in NUL
	p = بفر;
	لكل (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    طالما (*p != '\n' && *p != NUL)
		++p;
	    إذا (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    وإلا
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
	إذا (shell_style == STYLE_PRINT && !did_ڳوليو_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    بفر[len] = NUL;
	    إذا (len && (صحيح)STRLEN(بفر) < (صحيح)len)
		did_ڳوليو_nul = TRUE;
	    وإلا
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	إذا (len && بفر[len - 1] == NUL)
	    --len;
	وإلا
	    بفر[len] = NUL;
	i = 0;
	لكل (p = بفر; p < بفر + len; ++p)
	    إذا (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	إذا (len)
	    ++i;			// count last entry
    }
    إذا (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(بفر);
	اذهب notfound;
    }
    *انگ_file = i;
    *file = ALLOC_MULT(char_u *, i);
    إذا (*file == عدم)
    {
	// out of memory
	vim_free(بفر);
	أرجع FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = بفر;
    لكل (i = 0; i < *انگ_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	إذا (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    طالما (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    إذا (p == بفر + len)		// last entry
		*p = NUL;
	    وإلا
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	وإلا		// NUL separates
	{
	    طالما (*p && p < بفر + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    لكل (j = 0, i = 0; i < *انگ_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	إذا (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    استمر;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	إذا ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    استمر;

	// Skip files that are not executable if we check for that.
	إذا (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], عدم, !(flags & EW_SHELLCMD)))
	    استمر;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	إذا (p)
	{
	    STRCPY(p, (*file)[i]);
	    إذا (dir)
		جوڙيو_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(بفر);
    *انگ_file = j;

    إذا (*انگ_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	اذهب notfound;
    }

    أرجع OK;

notfound:
    إذا (flags & EW_NOTFOUND)
	أرجع save_patterns(انگ_pat, pat, انگ_file, file);
    أرجع FAIL;
}

#جيڪڏهن_پورو // VMS

    ساكن صحيح
save_patterns(
    صحيح		انگ_pat,
    char_u	**pat,
    صحيح		*انگ_file,
    char_u	***file)
{
    صحيح		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, انگ_pat);
    إذا (*file == عدم)
	أرجع FAIL;
    لكل (i = 0; i < انگ_pat; i++)
    {
	s = vim_strsave(pat[i]);
	إذا (s != عدم)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *انگ_file = انگ_pat;
    أرجع OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    صحيح
mch_has_exp_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	    ++p;
	وإلا
	    إذا (vim_strchr((char_u *)
#جيڪڏهن_وصف_ٿيل VMS
				    "*?%"
#وإلا
				    "*?[{'"
#جيڪڏهن_پورو
						, *p) != عدم)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    صحيح
mch_has_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	    ++p;
	وإلا
	    إذا (vim_strchr((char_u *)
#جيڪڏهن_وصف_ٿيل VMS
				    "*?%$"
#وإلا
				    "*?[{`'$"
#جيڪڏهن_پورو
						, *p) != عدم
		|| (*p == '~' && p[1] != NUL))
	    أرجع TRUE;
    }
    أرجع FALSE;
}

    ساكن صحيح
have_wildcard(صحيح انگ, char_u **file)
{
    صحيح	    i;

    لكل (i = 0; i < انگ; i++)
	إذا (mch_has_wildcard(file[i]))
	    أرجع 1;
    أرجع 0;
}

    ساكن صحيح
have_dollars(صحيح انگ, char_u **file)
{
    صحيح	    i;

    لكل (i = 0; i < انگ; i++)
	إذا (vim_strchr(file[i], '$') != عدم)
	    أرجع TRUE;
    أرجع FALSE;
}

#إذا !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    صحيح
mch_rename(ثابت حرف *src, ثابت حرف *dest)
{
    بنية stat	    st;

    إذا (stat(dest, &st) >= 0)	    // fail if destination exists
	أرجع -1;
    إذا (link(src, dest) != 0)	    // link file to new name
	أرجع -1;
    إذا (mch_remove(src) == 0)	    // delete link to old name
	أرجع 0;
    أرجع -1;
}
#جيڪڏهن_پورو // !HAVE_RENAME

#إذا defined(FEAT_MOUSE_GPM)
# إذا defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ساكن صحيح
ڀريو_libgpm(فراغ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    إذا (libgpm_hinst == عدم)
    {
	إذا (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	أرجع FAIL;
    }

    إذا (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == عدم
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == عدم
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == عدم
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == عدم
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == عدم
      )
    {
	semsg(_(e_could_not_ڀريو_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = عدم;
	dll_gpm_flag = عدم;
	dll_gpm_fd   = عدم;
	أرجع FAIL;
    }
    أرجع OK;
}

    صحيح
gpm_موجود(فراغ)
{
    أرجع libgpm_hinst != عدم || ڀريو_libgpm() == OK;
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
    إذا (!gpm_موجود())
	أرجع 0;
# endif

    إذا (gpm_flag)
	أرجع 1; // already open

    gpm_connect.event_نقاب_2 = (GPM_مٿي | GPM_DRAG | GPM_هيٺ_ڏانهن);
    gpm_connect.default_نقاب = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    إذا (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	أرجع 1; // succeed
    }
    إذا (gpm_fd == -2)
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
    أرجع 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    صحيح
gpm_enabled(فراغ)
{
    أرجع gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ساكن فراغ
gpm_close(فراغ)
{
    إذا (gpm_enabled())
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
    صحيح			صف,کانو;
    بنانشان حرف	buttons_نقاب;
    بنانشان حرف	gpm_modifiers;
    ساكن بنانشان حرف old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع 0;
# endif

    صف = gpm_event.y - 1;
    کانو = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    بدل (GPM_BARE_EVENTS(gpm_event.type))
    {
	حالة GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    اكسر;
	حالة GPM_هيٺ_ڏانهن:
	    buttons_نقاب = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    بدل (buttons_نقاب)
	    {
		حالة GPM_B_کاٻو:
		    button = MOUSE_کاٻو;
		    اكسر;
		حالة GPM_B_وچ:
		    button = MOUSE_وچ;
		    اكسر;
		حالة GPM_B_ساڄو:
		    button = MOUSE_ساڄو;
		    اكسر;
		افتراضي:
		    أرجع 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    سيٽ_انگ_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    اكسر;
	حالة GPM_مٿي:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    اكسر;
	افتراضي:
	    أرجع 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    إذا (gpm_modifiers & ((1 << KG_سرڪايو) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_سرڪايو;

    إذا (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    إذا (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(کانو + ' ' + 1);
    string[5] = (char_u)(صف + ' ' + 1);
    جوڙيو_منزل_داخلا_buf(string, 6);
    أرجع 6;
}
#جيڪڏهن_پورو // FEAT_MOUSE_GPM

#جيڪڏهن_وصف_ٿيل FEAT_SYSMOUSE
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

    mouse.operation = MOUSE_گهڻائي;
    mouse.u.گهڻائي.گهڻائي = 0;
    mouse.u.گهڻائي.إشارة = SIGUSR2;
    إذا (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	أرجع FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    أرجع OK;
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
    mouse.operation = MOUSE_گهڻائي;
    mouse.u.گهڻائي.گهڻائي = 0;
    mouse.u.گهڻائي.إشارة = 0;
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
    صحيح			صف, کانو;
    صحيح			button;
    صحيح			buttons;
    ساكن صحيح		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    إذا (hold_gui_events)
	أرجع;
# endif

    mouse.operation = MOUSE_GETINFO;
    إذا (ioctl(1, FBIO_GETMODE, &video.vi_گهڻائي) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	صف = mouse.u.معلومات.y / video.vi_cheight;
	کانو = mouse.u.معلومات.x / video.vi_cwidth;
	buttons = mouse.u.معلومات.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	إذا (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	وإلا
	{
	    بدل (buttons)
	    {
		حالة 0:
		    button = MOUSE_RELEASE;
		    اكسر;
		حالة 1:
		    button = MOUSE_کاٻو;
		    اكسر;
		حالة 2:
		    button = MOUSE_وچ;
		    اكسر;
		حالة 4:
		    button = MOUSE_ساڄو;
		    اكسر;
		افتراضي:
		    أرجع;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(کانو + ' ' + 1);
	string[5] = (char_u)(صف + ' ' + 1);
	جوڙيو_منزل_داخلا_buf(string, 6);
    }
    أرجع;
}
#جيڪڏهن_پورو // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحيح
mch_get_random(char_u *buf, صحيح len)
{
    ساكن صحيح dev_urandom_state = NOTDONE;

    إذا (dev_urandom_state == FAIL)
	أرجع FAIL;

    صحيح fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    إذا (fd == -1)
	dev_urandom_state = FAIL;
    وإلا إذا (اقرأ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    وإلا
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    أرجع dev_urandom_state;
}

#إذا defined(FEAT_LIBCALL)
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
    char_u	**string_نتيجو, // NULL when using number_result
    صحيح		*عدد_نتيجو)
{
# إذا defined(USE_DLOPEN)
    فراغ	*hinstLib;
    حرف	*dlerr = عدم;
# وإلا
    shl_t	hinstLib;
# endif
    STRPROCSTR	Proc_جوڙيو;
    INTPROCSTR	Proc_جوڙيو_I;
    char_u	*retval_str = عدم;
    صحيح		retval_int = 0;
    صحيح		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# إذا defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (فراغ)dlerror();

    hinstLib = dlopen((حرف *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    إذا (hinstLib == عدم)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	إذا (dlerr != عدم)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# وإلا
    hinstLib = shl_ڀريو((ثابت حرف*)libname, BIND_فوري|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    إذا (hinstLib != عدم)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	إذا (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  إذا defined(USE_DLOPEN)
	    dlerr = عدم;
#  endif
	    mch_didjmp();
	}
	وإلا
# endif
	{
	    retval_str = عدم;
	    retval_int = 0;

	    إذا (argstring != عدم)
	    {
# إذا defined(USE_DLOPEN)
		*(فراغ **)(&Proc_جوڙيو) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# وإلا
		إذا (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
					TYPE_PROCEDURE, (فراغ *)&Proc_جوڙيو) < 0)
		    Proc_جوڙيو = عدم;
# endif
		إذا ((success = (Proc_جوڙيو != عدم
# إذا defined(USE_DLOPEN)
			    && dlerr == عدم
# endif
			    )))
		{
		    إذا (string_نتيجو == عدم)
			retval_int = ((STRPROCINT)(فراغ *)Proc_جوڙيو)(argstring);
		    وإلا
			retval_str = (Proc_جوڙيو)(argstring);
		}
	    }
	    وإلا
	    {
# إذا defined(USE_DLOPEN)
		*(فراغ **)(&Proc_جوڙيو_I) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# وإلا
		إذا (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
				       TYPE_PROCEDURE, (فراغ *)&Proc_جوڙيو_I) < 0)
		    Proc_جوڙيو_I = عدم;
# endif
		إذا ((success = (Proc_جوڙيو_I != عدم
# إذا defined(USE_DLOPEN)
			    && dlerr == عدم
# endif
			    )))
		{
		    إذا (string_نتيجو == عدم)
			retval_int = ((INTPROCINT)(فراغ *)Proc_جوڙيو_I)(argint);
		    وإلا
			retval_str = (Proc_جوڙيو_I)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    إذا (string_نتيجو == عدم)
		*عدد_نتيجو = retval_int;
	    وإلا إذا (retval_str != عدم
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_نتيجو = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	إذا (lc_signal != 0)
	{
	    صحيح i;

	    // try to find the name of this signal
	    لكل (i = 0; signal_info[i].sig != -1; i++)
		إذا (lc_signal == signal_info[i].sig)
		    اكسر;
	    semsg(_(e_got_sig_str_اندريون_libcall), signal_info[i].name);
	}
#  endif
# endif

# إذا defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	إذا (dlerr != عدم)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(فراغ)dlclose(hinstLib);
# وإلا
	(فراغ)shl_unload(hinstLib);
# endif
    }

    إذا (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	أرجع FAIL;
    }

    أرجع OK;
}
#جيڪڏهن_پورو

#إذا defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ساكن صحيح	xterm_trace = -1;	// default: disabled
ساكن صحيح	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    فراغ
تياري_رڪن_clip(فراغ)
{
    صحيح		z = 0;
    حرف	*strp = "";
    Widget	AppShell;

    إذا (!x_connect_منزل_server())
	أرجع;

    open_app_context();
    إذا (app_context != عدم && xterm_Shell == (Widget)0)
    {
	صحيح (*oldhandler)(Display*, XErrorEvent*);
# إذا defined(USING_SETJMP)
	صحيح (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T شروعات_tv;

	إذا (p_verbose > 0)
	    ELAPSED_INIT(شروعات_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# إذا defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	إذا (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = عدم;
	}
	وإلا
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", عدم, 0, &z, &strp);
	    إذا (xterm_dpy != عدم)
		xterm_dpy_retry_ڳڻپ = 0;
# إذا defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# إذا defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(فراغ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(فراغ)XSetErrorHandler(oldhandler);

	إذا (xterm_dpy == عدم)
	{
	    إذا (p_verbose > 0)
		verb_نياپو(_("Opening the X display failed"));
	    أرجع;
	}

	// Catch terminating error of the X server connection.
	(فراغ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	إذا (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_پيغام(ELAPSED_FUNC(شروعات_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		عدم);
	إذا (AppShell == (Widget)0)
	    أرجع;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		چوٽي_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		عدم);
	إذا (xterm_Shell == (Widget)0)
	    أرجع;

	x11_تياري_atoms(xterm_dpy);
	x11_تياري_selection(xterm_Shell);
	إذا (x11_display == عدم)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    إذا (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	إذا (x11_دري == 0 && (strp = getenv("WINDOWID")) != عدم)
	    x11_دري = (دري)atol(strp);
	// Check if $WINDOWID is valid.
	إذا (test_x11_دري(xterm_dpy) == FAIL)
	    x11_دري = 0;
	إذا (x11_دري != 0)
	    xterm_trace = 0;
    }
}

    فراغ
شروعات_xterm_trace(صحيح button)
{
    إذا (x11_دري == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	أرجع;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    فراغ
stop_xterm_trace(فراغ)
{
    إذا (xterm_trace < 0)
	أرجع;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ساكن صحيح
do_xterm_trace(فراغ)
{
    دري		پاڙ, ٻار;
    صحيح			پاڙ_x, پاڙ_y;
    صحيح			win_x, win_y;
    صحيح			صف, کانو;
    int_u		نقاب_return;
    char_u		buf[50];
    char_u		*strp;
    ڊگهو		got_hints;
    ساكن char_u	*mouse_ڪوڊ = عدم;
    ساكن size_t	mouse_codelen = 0;
    ساكن char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ساكن صحيح		prev_صف = 0, prev_کانو = 0;
    ساكن XSizeHints	xterm_hints;

    إذا (xterm_trace <= 0)
	أرجع FALSE;

    إذا (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	إذا (!XGetWMNormalHints(xterm_dpy, x11_دري, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.ويڪر_inc <= 1
		|| xterm_hints.اوچائي_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    أرجع FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_ڪوڊ = ڳوليو_termcode(mouse_name);
	إذا (mouse_ڪوڊ != عدم)
	    mouse_codelen = STRLEN(mouse_ڪوڊ);
	prev_صف = mouse_صف;
	prev_کانو = mouse_کانو;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQuery_اشارو(xterm_dpy, x11_دري, &پاڙ, &ٻار, &پاڙ_x, &پاڙ_y,
		      &win_x, &win_y, &نقاب_return);
	xterm_hints.y = win_y - (xterm_hints.اوچائي_inc * mouse_صف)
			      - (xterm_hints.اوچائي_inc / 2);
	إذا (xterm_hints.y <= xterm_hints.اوچائي_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.ويڪر_inc * mouse_کانو)
			      - (xterm_hints.ويڪر_inc / 2);
	إذا (xterm_hints.x <= xterm_hints.ويڪر_inc / 2)
	    xterm_hints.x = 2;
	أرجع TRUE;
    }

    إذا (mouse_ڪوڊ == عدم || mouse_codelen > 45)
    {
	xterm_trace = 0;
	أرجع FALSE;
    }

    XQuery_اشارو(xterm_dpy, x11_دري, &پاڙ, &ٻار, &پاڙ_x, &پاڙ_y,
		  &win_x, &win_y, &نقاب_return);

    صف = check_صف((win_y - xterm_hints.y) / xterm_hints.اوچائي_inc);
    کانو = check_کانو((win_x - xterm_hints.x) / xterm_hints.ويڪر_inc);
    إذا (صف == prev_صف && کانو == prev_کانو)
	أرجع TRUE;

    STRCPY(buf, mouse_ڪوڊ);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(کانو + ' ' + 1);
    *strp++ = (char_u)(صف + ' ' + 1);
    *strp = NUL;
    جوڙيو_منزل_داخلا_buf(buf, strp - buf);

    prev_صف = صف;
    prev_کانو = کانو;
    أرجع TRUE;
}

# إذا defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    فراغ
clear_xterm_clip(فراغ)
{
    إذا (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    إذا (xterm_dpy != عدم)
    {
#  إذا 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	إذا (x11_display == xterm_dpy)
	    x11_display = عدم;
	xterm_dpy = عدم;
    }
#  إذا 0
    إذا (app_context != (XtAppContext)عدم)
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
    إذا (gui.اندريون_use)
	gui_mch_update();
    وإلا
# endif
    إذا (xterm_Shell != (Widget)0)
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

    لكل (;;)
    {
	Xt_داخلا_نقاب نقاب = XtAppPending(app_context);

	إذا (نقاب == 0 || vim_is_داخلا_buf_ڀريل())
	    اكسر;

	إذا (نقاب & XtIMXEvent)
	{
	    // There is an event to process.
	    Xt_App_ايندڙ_Event(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		إذا (e->type == PropertyNotify && e->دري_2 == comm_دري
		   && e->atom == commProperty && e->state == Property_New_قدر)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	وإلا
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, نقاب);
	}
    }
}

    صحيح
clip_xterm_own_selection(Clipboard_T *cbd)
{
    إذا (xterm_Shell != (Widget)0)
	أرجع clip_x11_own_selection(xterm_Shell, cbd);
    أرجع FAIL;
}

    فراغ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    إذا (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    فراغ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    إذا (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    فراغ
clip_xterm_سيٽ_selection(Clipboard_T *cbd)
{
    clip_x11_سيٽ_selection(cbd);
}
#جيڪڏهن_پورو


#إذا defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# إذا defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ساكن فراغ
xsmp_handle_interaction(SmcConn smc_conn, Sm_اشارو client_معلومات_2 اڻ_استعمال_ٿيل)
{
    صحيح		save_cmod_flags;
    صحيح		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    إذا (check_بدليو_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    ٻاهريون_flush();

    // Done interaction
    Smc_Interact_پورو(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    إذا (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	Smc_Save_Yourself_پورو(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ساكن فراغ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    Sm_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل,
    صحيح		save_type اڻ_استعمال_ٿيل,
    Bool	shutdown,
    صحيح		interact_style اڻ_استعمال_ٿيل,
    Bool	fast اڻ_استعمال_ٿيل)
{
    // Handle already being in saveyourself
    إذا (xsmp.save_yourself)
	Smc_Save_Yourself_پورو(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    ٻاهريون_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    إذا (p_verbose > 0)
	verb_نياپو(_("XSMP handling save-yourself request"));

# إذا defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    إذا (shutdown && !fast && gui.اندريون_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_معلومات_2);
    وإلا
# endif
    {
	// Can stop the cycle here
	Smc_Save_Yourself_پورو(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    ساكن فراغ
xsmp_die(SmcConn smc_conn اڻ_استعمال_ٿيل, Sm_اشارو client_معلومات_2 اڻ_استعمال_ٿيل)
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
    SmcConn	smc_conn اڻ_استعمال_ٿيل,
    Sm_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل)
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
    Sm_اشارو	client_معلومات_2 اڻ_استعمال_ٿيل)
{
    إذا (xsmp.save_yourself)
	Smc_Save_Yourself_پورو(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ساكن فراغ
xsmp_ice_connection(
    IceConn	iceConn,
    Ice_اشارو	client_معلومات اڻ_استعمال_ٿيل,
    Bool	opening,
    Ice_اشارو	*watch_معلومات اڻ_استعمال_ٿيل)
{
    // Intercept creation of ICE connection fd
    إذا (opening)
    {
	xsmp_icefd = Ice_Connection_عدد(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, عدم);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    صحيح
xsmp_handle_درخواستون(فراغ)
{
    Bool rep;

    إذا (IceProcessMessages(xsmp.iceconn, عدم, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	إذا (p_verbose > 0)
	    verb_نياپو(_("XSMP lost ICE connection"));
	xsmp_close();
	أرجع FAIL;
    }
    وإلا
	أرجع OK;
}

ساكن صحيح dummy;

// Set up X Session Management Protocol
    فراغ
xsmp_init(فراغ)
{
    حرف		errorstring[80];
    SmcCallbacks	smcallbacks;
# إذا 0
    Sm_Prop_قدر		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    إذا (p_verbose > 0)
	verb_نياپو(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_معلومات_2 = عدم;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_معلومات_2 = عدم;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_معلومات_2 = عدم;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_معلومات_2 = عدم;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    إذا (Ice_جوڙيو_Connection_Watch(xsmp_ice_connection, &dummy) == 0)
    {
	إذا (p_verbose > 0)
	    verb_نياپو(_("XSMP ICE connection watch failed"));
	أرجع;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    عدم,
	    عدم,
	    SmProtoMajor,
	    SmProtoMinor,
	    Smc_Save_Yourself_Proc_نقاب | Smc_Die_Proc_نقاب
		     | Smc_Save_Complete_Proc_نقاب | Smc_Shutdown_Cancelled_Proc_نقاب,
	    &smcallbacks,
	    عدم,
	    &xsmp.clientid,
	    حجم(errorstring) - 1,
	    errorstring);
    إذا (xsmp.smcconn == عدم)
    {
	إذا (p_verbose > 0)
	{
	    حرف errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[حجم(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, حجم(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_نياپو(errorreport);
	}
	أرجع;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# إذا 0
    // ID ourselves
    smname.قدر = "vim";
    smname.ڊيگهائي = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.انگ_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    Smc_سيٽ_Properties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    فراغ
xsmp_close(فراغ)
{
    إذا (xsmp_icefd == -1)
	أرجع;

    SmcCloseConnection(xsmp.smcconn, 0, عدم);
    إذا (xsmp.clientid != عدم)
	حرر(xsmp.clientid);
    xsmp.clientid = عدم;
    xsmp_icefd = -1;
}
#جيڪڏهن_پورو // USE_XSMP

#إذا defined(FEAT_RELTIME)
# إذا defined(PROF_NSEC)
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
سيٽ_flag(اتحاد sigval اڻ_استعمال_ٿيل_3 اڻ_استعمال_ٿيل)
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

    إذا (timer_created)
    {
	صحيح ret = timer_settime(timer_id, 0, &disarm, عدم);

	إذا (ret < 0)
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
شروعات_timeout(ڊگهو msec)
{
    بنية itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    صحيح ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    إذا (!timer_created)
    {
	بنية sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = سيٽ_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	إذا (ret < 0)
	{
	    semsg(_(e_could_not_سيٽ_timeout_str), strerror(errno));
	    أرجع &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(عدم, "setting timeout timer to %d sec %ld nsec",
	       (صحيح)interval.it_value.tv_sec, (ڊگهو)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, عدم);
    إذا (ret < 0)
	semsg(_(e_could_not_سيٽ_timeout_str), strerror(errno));

    أرجع &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    فراغ
delete_timer(فراغ)
{
    إذا (!timer_created)
	أرجع;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# وإلا // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ساكن بنية sigaction		prev_sigaction;
ساكن متطاير sig_atomic_t	timeout_flag	     = FALSE;
ساكن صحيح			timer_سرگرم	     = FALSE;
ساكن صحيح			timer_handler_سرگرم = FALSE;
ساكن متطاير sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ساكن فراغ
سيٽ_flag SIGDEFARG(sigarg)
{
    إذا (alarm_pending)
	alarm_pending = FALSE;
    وإلا
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

    إذا (timer_سرگرم)
    {
	timer_سرگرم = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, عدم);
	إذا (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    إذا (timer_handler_سرگرم)
    {
	timer_handler_سرگرم = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, عدم);
	إذا (ret < 0)
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
شروعات_timeout(ڊگهو msec)
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
    إذا (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, عدم);
    }
    إذا (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = سيٽ_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    إذا (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_سيٽ_handler_for_timeout_str), strerror(errno));
	أرجع &timeout_flag;
    }
    timer_handler_سرگرم = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, عدم);
    إذا (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_سيٽ_timeout_str), strerror(errno));
	stop_timeout();
	أرجع &timeout_flag;
    }

    timer_سرگرم = TRUE;
    أرجع &timeout_flag;
}
# endif // PROF_NSEC
#جيڪڏهن_پورو  // FEAT_RELTIME
