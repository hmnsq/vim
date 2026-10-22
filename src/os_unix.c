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

#اگر_تعریف_شدہ FEAT_MZSCHEME
# include "if_mzsch.h"
#اگر_ختم

#شامل "os_unixx.h"	    // unix includes for os_unix.c only

#اگر_تعریف_شدہ USE_XSMP
# include <X11/SM/SMlib.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SELINUX
# include <selinux/selinux.h>
ساکن صحیح selinux_enabled = -1;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_XATTR
# include <sys/xattr.h>
#اگر_ختم

#اگر_تعریف_شدہ HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#اگر_ختم

#اگر_تعریف_شدہ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_اوسط
#  include <windows.h>
#  include "winclip.pro"
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_GPM

# include <gpm.h>

# ifdef DYNAMIC_GPM
#  define Gpm_Open     (*dll_Gpm_Open)
#  define Gpm_Close    (*dll_Gpm_Close)
#  define Gpm_GetEvent (*dll_Gpm_GetEvent)
#  define gpm_flag     (dll_gpm_flag != تہی ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != تہی ? *dll_gpm_fd   : -1)

ساکن صحیح (*dll_Gpm_Open)     (Gpm_Connect *, صحیح);
ساکن صحیح (*dll_Gpm_Close)    (خالی);
ساکن صحیح (*dll_Gpm_GetEvent) (Gpm_Event *);
ساکن صحیح *dll_gpm_flag;
ساکن صحیح *dll_gpm_fd;

ساکن خالی *libgpm_hinst;
# endif

// <linux/keyboard.h> contains defines conflicting with "keymap.h",
// I just copied relevant defines here. A cleaner solution would be to put gpm
// code into separate file and include there linux/keyboard.h
// #include <linux/keyboard.h>
# define KG_سرکاؤ	0
# define KG_CTRL	2
# define KG_ALT		3
# define KG_ALTGR	1
# define KG_SHIFTL	4
# define KG_SHIFTR	5
# define KG_CTRLL	6
# define KG_CTRLR	7
# define KG_CAPSSHIFT	8

ساکن خالی gpm_close(خالی);
ساکن صحیح gpm_open(خالی);
ساکن صحیح mch_gpm_process(خالی);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ساکن صحیح sysmouse_open(خالی);
ساکن خالی sysmouse_close(خالی);
ساکن خالی sig_sysmouse SIGPROTOARG;
#اگر_ختم

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
#اگر_ختم

#اگر defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#اگر_ختم

#اگر_تعریف_شدہ FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
#  include <X11/Shell.h>
#  include <X11/StringDefs.h>
ساکن Widget	xterm_Shell = (Widget)0;
ساکن خالی clip_update(خالی);
# endif

کھڑکی	    x11_کھڑکی = 0;
Display	    *x11_display = تہی;
#اگر_ختم

ساکن صحیح ignore_sigtstp = FALSE;

ساکن صحیح get_x11_title(صحیح);

ساکن char_u	*oldtitle = تہی;
ساکن متلون sig_atomic_t oldtitle_outdated = FALSE;
ساکن صحیح	unix_did_سیٹ_title = FALSE;
ساکن char_u	*oldicon = تہی;
ساکن صحیح	did_سیٹ_icon = FALSE;

ساکن خالی may_core_dump(خالی);

قسمتعریف صحیح waitstatus;
ساکن صحیح  WaitForChar(طویل msec, صحیح *interrupted, صحیح ignore_داخلہ);
ساکن صحیح  WaitForCharOrMouse(طویل msec, صحیح *interrupted, صحیح ignore_داخلہ);
#اگر_تعریف_شدہ VMS
صحیح  RealWaitForChar(صحیح, طویل, صحیح *, صحیح *interrupted);
#ورنہ
ساکن صحیح  RealWaitForChar(صحیح, طویل, صحیح *, صحیح *interrupted);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_XCLIPBOARD
ساکن صحیح do_xterm_trace(خالی);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#اگر_ختم

ساکن خالی handle_resize(خالی);

#اگر defined(SIGWINCH)
ساکن خالی sig_winch SIGPROTOARG;
#اگر_ختم
#اگر defined(SIGTSTP)
ساکن خالی sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ساکن متلون sig_atomic_t اندر_mch_suspend = FALSE;
#اگر_ختم
#اگر defined(SIGINT)
ساکن خالی catch_sigint SIGPROTOARG;
#اگر_ختم
#اگر defined(SIGUSR1)
ساکن خالی catch_sigusr1 SIGPROTOARG;
#اگر_ختم
#اگر defined(SIGPWR)
ساکن خالی catch_sigpwr SIGPROTOARG;
#اگر_ختم
#اگر defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define سیٹ_SIG_ALARM
ساکن خالی sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ساکن متلون sig_atomic_t sig_alarm_called;
#اگر_ختم
ساکن خالی deathtrap SIGPROTOARG;

ساکن خالی catch_int_signal(خالی);
ساکن خالی سیٹ_اشارات(خالی);
ساکن خالی catch_اشارات(خالی (*func_deadly)(صحیح), خالی (*func_other)(صحیح));
#اگر_تعریف_شدہ HAVE_SIGPROCMASK
# define SIGSET_DECL(سیٹ_2)	sigset_t سیٹ_2;
# define بلاک_اشارات(سیٹ_2)	بلاک_اشارات_2(سیٹ_2)
# define UNBLOCK_اشارات(سیٹ_2)	unblock_اشارات(سیٹ_2)
#ورنہ
# define SIGSET_DECL(سیٹ_2)
# define بلاک_اشارات(سیٹ_2)	کرو { /**/ } جب (0)
# define UNBLOCK_اشارات(سیٹ_2)	کرو { /**/ } جب (0)
#اگر_ختم
ساکن صحیح  have_wildcard(صحیح, char_u **);
ساکن صحیح  have_dollars(صحیح, char_u **);

ساکن صحیح save_patterns(صحیح عدد_pat, char_u **pat, صحیح *عدد_file, char_u ***file);

#اگر_غیر_تعریف_شدہ SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#اگر_ختم
#اگر_غیر_تعریف_شدہ SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#اگر_ختم

// volatile because it is used in signal handler sig_winch().
ساکن متلون sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ساکن متلون sig_atomic_t got_tstp = FALSE;
ساکن char_u	*extra_shell_arg = تہی;
ساکن صحیح	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ساکن متلون sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ساکن متلون sig_atomic_t اندر_mch_delay = FALSE; // sleeping in mch_delay()

#اگر defined(FEAT_کام_CHANNEL) && !defined(USE_SYSTEM)
ساکن صحیح dont_check_کام_ended = 0;
#اگر_ختم

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ساکن tmode_T mch_cur_tmode = TMODE_COOK;

#اگر_تعریف_شدہ USE_XSMP
قسمتعریف ساخت
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    حرف *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ساکن xsmp_config_T xsmp;
#اگر_ختم

#اگر_تعریف_شدہ SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#اگر_ختم

ساکن ساخت signalinfo
{
    صحیح	    sig;	// Signal number, eg. SIGSEGV etc
    حرف    *name;	// Signal name (not char_u!).
    حرف    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#اگر_تعریف_شدہ SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGILL
    {SIGILL,	    "ILL",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#اگر_ختم
#اگر defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#اگر_ختم
#اگر_تعریف_شدہ SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#اگر_ختم
#اگر defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#اگر_ختم
#اگر defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#اگر_ختم
#اگر defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#اگر_ختم
#اگر_تعریف_شدہ SIGINT
    {SIGINT,	    "INT",	FALSE},
#اگر_ختم
#اگر_تعریف_شدہ SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#اگر_ختم
#اگر_تعریف_شدہ SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#اگر_ختم
#اگر_تعریف_شدہ SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#اگر_ختم
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(صحیح sig, sighandler_T func)
{
#اگر defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ساخت sigaction	sa, old;
    sigset_t		curset;
    صحیح			blocked;

    اگر (sigprocmask(SIG_BLOCK, تہی, &curset) == -1)
	واپس SIG_ERR;

    blocked = sigismember(&curset, sig);

    اگر (func == SIG_HOLD)
    {
	اگر (blocked)
	    واپس SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	اگر (sigaction(sig, تہی, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, تہی) == -1)
	    واپس SIG_ERR;
	واپس old.sa_handler;
    }

    اگر (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	اگر (sigprocmask(SIG_UNBLOCK, &curset, تہی) == -1)
	    واپس SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# ورنہ
    sa.sa_flags = 0;
# endif
    اگر (sigaction(sig, &sa, &old) == -1)
	واپس SIG_ERR;
    واپس blocked ? SIG_HOLD: old.sa_handler;
#وگرنہ_اگر defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    واپس sigset(sig, func);
#ورنہ
    // Oldest and most compatible solution.
    واپس اشارہ(sig, func);
#اگر_ختم
}

    صحیح
mch_chdir(حرف *راستہ)
{
    اگر (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", راستہ);
	verbose_leave();
    }
#اگر_تعریف_شدہ VMS
    واپس chdir(vms_fixfilename(راستہ));
#ورنہ
    واپس chdir(راستہ);
#اگر_ختم
}

#اگر defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#اگر_ختم

/*
 * Write s[len] to the screen (stdout).
 */
    خالی
mch_write(char_u *s, صحیح len)
{
    vim_ignored = (صحیح)اكتب(1, (حرف *)s, len);
    اگر (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, تہی, تہی);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ساکن صحیح
resize_func(صحیح check_only)
{
    اگر (check_only)
	واپس do_resize;
    جب (do_resize)
    {
#اگر_تعریف_شدہ FEAT_EVAL
	ch_log(تہی, "calling handle_resize() in resize_func()");
#اگر_ختم
	handle_resize();
    }
    واپس FALSE;
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
    طویل	wtime,	    // don't use "time", MIPS cannot handle it
    صحیح		tb_change_cnt)
{
    واپس inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ساکن خالی
handle_resize(خالی)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    صحیح
mch_char_avail(خالی)
{
    واپس WaitForChar(0L, تہی, FALSE);
}

#اگر defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    صحیح
mch_check_messages(خالی)
{
    واپس WaitForChar(0L, تہی, TRUE);
}
#اگر_ختم

#اگر defined(HAVE_کل_MEM)
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
mch_کل_mem(صحیح special غیر_مستعمل)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_پورٹ_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ساخت vm_statistics64	vm_stat;
	natural_t		گنتی = HOST_VM_INFO64_گنتی;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &گنتی);
#  ورنہ
	ساخت vm_statistics	vm_stat;
	natural_t		گنتی = HOST_VM_INFO_گنتی;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &گنتی);
#  endif
	اگر (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_گنتی + vm_stat.فعال_گنتی
					    + vm_stat.inactive_گنتی
#  ifdef MAC_OS_X_نسخہ_10_9
					    + vm_stat.compressor_صفحہ_گنتی
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_پورٹ_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    اگر (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	صحیح		mib[2];
	size_t		len = حجم(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  ورنہ
	// sysctl() may return 32 bit or 64 bit, accept both
	اتحاد {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  ورنہ
	mib[1] = HW_USERMEM;
#  endif
	اگر (sysctl(mib, 2, &physmem, &len, تہی, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  ورنہ
	    اگر (len == حجم(physmem.u64))
		mem = (long_u)physmem.u64;
	    ورنہ
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# اگر defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    اگر (mem == 0)
    {
	ساخت sysinfo sinfo;

	// Linux way of getting amount of RAM available
	اگر (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    جب (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  ورنہ
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    اگر (mem == 0)
    {
	طویل	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	اگر (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    جب (shiftright > 0 && (pagesize & 1) == 0)
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
	ساخت rlimit	rlp;

	اگر (getrlimit(RLIMIT_ڈیٹا, &rlp) == 0
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

    اگر (mem > 0)
	واپس mem >> shiftright;
    واپس (long_u)0x1fffff;
}
#اگر_ختم

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    خالی
mch_delay(طویل msec, صحیح flags)
{
    tmode_T	old_tmode;
    صحیح		call_settmode;
#اگر_تعریف_شدہ FEAT_MZSCHEME
    طویل	کل = msec; // remember original value
#اگر_ختم

    اگر (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	اندر_mch_delay = TRUE;
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
#اگر_تعریف_شدہ FEAT_MZSCHEME
	کرو
	{
	    // if total is large enough, wait by portions in p_mzq
	    اگر (کل > p_mzq)
		msec = p_mzq;
	    ورنہ
		msec = کل;
	    کل -= msec;
#اگر_ختم
#اگر_تعریف_شدہ HAVE_NANOSLEEP
	{
	    ساخت timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (خالی)nanosleep(&ts, تہی);
	}
#ورنہ
# ifdef HAVE_USLEEP
	جب (msec >= 1000)
	{
	    usleep((غیرعلامتی صحیح)(999 * 1000));
	    msec -= 999;
	}
	usleep((غیرعلامتی صحیح)(msec * 1000));
# ورنہ
#  ifndef HAVE_SELECT
	poll(تہی, 0, (صحیح)msec);
#  ورنہ
	{
	    ساخت timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, تہی, تہی, تہی, &tv);
	}
#  endif
# endif
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MZSCHEME
	}
	جب (کل > 0);
#اگر_ختم

	اگر (call_settmode)
	    settmode(old_tmode);
	اندر_mch_delay = FALSE;
    }
    ورنہ
	WaitForChar(msec, تہی, FALSE);
}

#اگر defined(HAVE_ڈھیر_حد_بندی) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ڈھیر_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ساکن صحیح ڈھیر_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ساکن خالی
check_ڈھیر_growth(حرف *p)
{
    صحیح		i;

    ڈھیر_grows_downwards = (p > (حرف *)&i);
}
#اگر_ختم

#اگر defined(HAVE_ڈھیر_حد_بندی)
ساکن حرف *ڈھیر_حد_بندی = تہی;

# اگر defined(THREAD_محفوظ) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ساکن خالی
get_ڈھیر_حد_بندی(خالی)
{
    ساخت rlimit	rlp;
    صحیح			i;
    طویل		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    اگر (getrlimit(RLIMIT_ڈھیر, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (حجم(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (طویل)rlp.rlim_cur;
# اگر defined(THREAD_محفوظ) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    سائز;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    اگر (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &سائز);
		اگر (lim > (طویل)سائز)
		    lim = (طویل)سائز;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	اگر (ڈھیر_grows_downwards)
	{
	    ڈھیر_حد_بندی = (حرف *)((طویل)&i - (lim / 16L * 15L));
	    اگر (ڈھیر_حد_بندی >= (حرف *)&i)
		// overflow, set to 1/16 of current stack position
		ڈھیر_حد_بندی = (حرف *)((طویل)&i / 16L);
	}
	ورنہ
	{
	    ڈھیر_حد_بندی = (حرف *)((طویل)&i + (lim / 16L * 15L));
	    اگر (ڈھیر_حد_بندی <= (حرف *)&i)
		ڈھیر_حد_بندی = تہی;	//حد_سے_تجاوز
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    صحیح
mch_stackcheck(حرف *p)
{
    اگر (ڈھیر_حد_بندی == تہی)
	واپس OK;

    اگر (ڈھیر_grows_downwards)
    {
	اگر (p < ڈھیر_حد_بندی)
	    واپس FAIL;
    }
    ورنہ اگر (p > ڈھیر_حد_بندی)
	واپس FAIL;
    واپس OK;
}
#اگر_ختم

#اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ساکن stack_t sigstk;			// for sigaltstack()
# ورنہ
ساکن ساخت sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ساکن طویل صحیح get_signal_ڈھیر_سائز(خالی)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    طویل صحیح سائز = -1;

    // return size only if sysconf doesn't return an error
    اگر ((سائز = sysconf(_SC_SIGSTKSZ)) > -1)
	واپس سائز;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    واپس SIGSTKSZ;
# endif

    // otherwise guess the size
    واپس 8000;
}

ساکن حرف *signal_ڈھیر;

    ساکن خالی
init_signal_ڈھیر(خالی)
{
    اگر (signal_ڈھیر == تہی)
	واپس;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_بنیاد
    sigstk.ss_بنیاد = signal_ڈھیر;
#  ورنہ
    sigstk.ss_sp = signal_ڈھیر;
#  endif
    sigstk.ss_size = get_signal_ڈھیر_سائز();
    sigstk.ss_flags = 0;
    (خالی)sigaltstack(&sigstk, تہی);
# ورنہ
    sigstk.ss_sp = signal_ڈھیر;
    اگر (ڈھیر_grows_downwards)
	sigstk.ss_sp += get_signal_ڈھیر_سائز() - 1;
    sigstk.ss_onstack = 0;
    (خالی)sigstack(&sigstk, تہی);
# endif
}
#اگر_ختم

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#اگر defined(SIGWINCH)
    ساکن خالی
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#اگر_ختم

#اگر defined(SIGTSTP)
    ساکن خالی
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    اگر (اندر_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	اٹھاؤ(sigarg);
    }
    ورنہ
	got_tstp = TRUE;

# اگر !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#اگر_ختم

#اگر defined(SIGINT)
    ساکن خالی
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#اگر_ختم

#اگر defined(SIGUSR1)
    ساکن خالی
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#اگر_ختم

#اگر defined(SIGPWR)
    ساکن خالی
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
#اگر_ختم

#اگر_تعریف_شدہ سیٹ_SIG_ALARM
/*
 * signal function for alarm().
 */
    ساکن خالی
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#اگر_ختم

#اگر defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ساکن JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ساکن متلون sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ساکن متلون sig_atomic_t lc_فعال = FALSE;

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
    ساکن خالی
mch_startjmp(خالی)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_فعال = TRUE;
}

    ساکن خالی
mch_endjmp(خالی)
{
    lc_فعال = FALSE;
}

    ساکن خالی
mch_didjmp(خالی)
{
# اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ڈھیر();
# endif
}
#اگر_ختم

/*
 * This function handles deadly signals.
 * It tries to preserve any swap files and exit properly.
 * (partly from Elvis).
 * NOTE: Avoid unsafe functions, such as allocating memory, they can result in
 * a deadlock.
 */
    ساکن خالی
deathtrap SIGDEFARG(sigarg)
{
    ساکن صحیح	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#اگر_تعریف_شدہ SIGHASARG
    صحیح		i;
#اگر_ختم

#اگر defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    اگر (lc_فعال)
    {
# اگر defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_فعال = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#اگر_ختم

#اگر_تعریف_شدہ SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    اگر (اندر_mch_delay && sigarg == SIGQUIT)
	واپس;
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
	واپس;
#اگر_ختم

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    بلاک_autocmds();

#اگر_تعریف_شدہ FEAT_EVAL
    // Set the v:dying variable.
    سیٹ_vim_var_nr(VV_DYING, (طویل)entered);
#اگر_ختم
    v_dying = entered;

#اگر_تعریف_شدہ HAVE_ڈھیر_حد_بندی
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ڈھیر_حد_بندی();
#اگر_ختم

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
#اگر_ختم

#اگر_تعریف_شدہ SIGHASARG
    // try to find the name of this signal
    برائے (i = 0; signal_info[i].sig != -1; i++)
	اگر (sigarg == signal_info[i].sig)
	    توڑو;
    deadly_signal = sigarg;
#اگر_ختم

    بھرا_screen = FALSE;	// don't write message to the GUI, it might be
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
	reset_اشارات();	// don't catch any signals anymore
	may_core_dump();
	اگر (entered >= 4)
	    _exit(8);
	باہر_نکلو(7);
    }
    اگر (entered == 2)
    {
	// No translation, it may call malloc().
	برآمد_STR("Vim: Double signal, exiting\n");
	برآمد_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#اگر_تعریف_شدہ SIGHASARG
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#ورنہ
    sprintf((حرف *)IObuff, "Vim: Caught deadly signal\r\n");
#اگر_ختم

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#اگر_تعریف_شدہ NBDEBUG
    reset_اشارات();
    may_core_dump();
    abort();
#اگر_ختم
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    ساکن خالی
after_sigcont(خالی)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#اگر defined(SIGCONT)
ساکن خالی sigcont_handler SIGPROTOARG;

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
ساکن متلون sig_atomic_t sigcont_received;
ساکن خالی sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ساکن خالی
sigcont_handler SIGDEFARG(sigarg)
{
    اگر (اندر_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    ورنہ
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	برآمد_flush();
    }
}
#اگر_ختم

#اگر defined(FEAT_CLIPBOARD)
# اگر defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ساکن خالی *clip_ستارہ_save = تہی;
ساکن خالی *clip_plus_save = تہی;
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
    ساکن خالی
loose_clipboard(خالی)
{
    اگر (clip_ستارہ.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	اگر (clip_ستارہ.owned)
	    clip_lose_selection(&clip_ستارہ);
	اگر (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	اگر (x11_display != تہی)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# اگر defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ساکن خالی
save_clipboard(خالی)
{
    اگر (clip_ستارہ.owned)
	clip_ستارہ_save = get_رجسٹر('*', TRUE);
    اگر (clip_plus.owned)
	clip_plus_save = get_رجسٹر('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ساکن خالی
restore_clipboard(خالی)
{
    اگر (clip_ستارہ_save != تہی)
    {
	اگر (!clip_gen_owner_exists(&clip_ستارہ))
	    put_رجسٹر('*', clip_ستارہ_save);
	ورنہ
	    free_رجسٹر(clip_ستارہ_save);
	clip_ستارہ_save = تہی;
    }
    اگر (clip_plus_save != تہی)
    {
	اگر (!clip_gen_owner_exists(&clip_plus))
	    put_رجسٹر('+', clip_plus_save);
	ورنہ
	    free_رجسٹر(clip_plus_save);
	clip_plus_save = تہی;
    }
}
# endif
#اگر_ختم

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    خالی
mch_suspend(خالی)
{
    اگر (ignore_sigtstp)
	واپس;

#اگر defined(SIGTSTP)
    اندر_mch_suspend = TRUE;

    برآمد_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    برآمد_flush();	    // needed to disable mouse on some systems

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
	طویل wait_وقت;

	برائے (wait_وقت = 0; !sigcont_received && wait_وقت <= 3L; wait_وقت++)
	    mch_delay(wait_وقت, 0);
    }
# endif
    اندر_mch_suspend = FALSE;

    after_sigcont();
#ورنہ
    suspend_shell();
#اگر_ختم
}

    خالی
mch_init(خالی)
{
    Columns = 80;
    صفیں = 24;

    برآمد_flush();

#اگر_تعریف_شدہ SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#اگر_ختم
    سیٹ_اشارات();

#اگر_تعریف_شدہ MACOS_CONVERT
    mac_conv_init();
#اگر_ختم
#اگر_تعریف_شدہ FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#اگر_ختم
}

    خالی
سیٹ_sigwinch_handler(خالی)
{
#اگر defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#اگر_ختم
}

    ساکن خالی
سیٹ_اشارات(خالی)
{
#اگر defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#اگر_ختم

#اگر_تعریف_شدہ SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.اندر_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#اگر_ختم
#اگر defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#اگر_ختم
#اگر_تعریف_شدہ SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#اگر_ختم

#اگر_تعریف_شدہ SIGINT
    catch_int_signal();
#اگر_ختم

#اگر_تعریف_شدہ SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#اگر_ختم

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#اگر_تعریف_شدہ SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#اگر_ختم

#اگر_تعریف_شدہ SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#اگر_ختم

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_اشارات(deathtrap, SIG_ERR);

#اگر defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    اگر (gui.اندر_use)
	mch_signal(SIGHUP, SIG_IGN);
#اگر_ختم
}

#اگر defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ساکن خالی
catch_int_signal(خالی)
{
    mch_signal(SIGINT, catch_sigint);
}
#اگر_ختم

    خالی
reset_اشارات(خالی)
{
    catch_اشارات(SIG_DFL, SIG_DFL);
#اگر defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#اگر_ختم
}

    ساکن خالی
catch_اشارات(
    خالی (*func_deadly)(صحیح),
    خالی (*func_other)(صحیح))
{
    صحیح	    i;

    برائے (i = 0; signal_info[i].sig != -1; i++)
    {
	اگر (signal_info[i].deadly)
	{
#اگر defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ساخت sigaction sa;

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
# ورنہ
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, تہی);
#ورنہ
# اگر defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ساخت sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_نقاب = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, تہی);
# ورنہ
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#اگر_ختم
	}
	ورنہ اگر (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#اگر_تعریف_شدہ SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#ورنہ
	    mch_signal(signal_info[i].sig, func_other);
#اگر_ختم
	}
    }
}

#اگر_تعریف_شدہ HAVE_SIGPROCMASK
    ساکن خالی
بلاک_اشارات_2(sigset_t *سیٹ_2)
{
    sigset_t	newset;
    صحیح		i;

    sigemptyset(&newset);

    برائے (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# اگر defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, سیٹ_2);
}

    ساکن خالی
unblock_اشارات(sigset_t *سیٹ_2)
{
    sigprocmask(SIG_SETMASK, سیٹ_2, تہی);
}
#اگر_ختم

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
    ساکن صحیح got_signal = 0;
    ساکن صحیح blocked = TRUE;

    انتخاب (sig)
    {
	حالت SIGNAL_بلاک:   blocked = TRUE;
			     توڑو;

	حالت SIGNAL_UNBLOCK: blocked = FALSE;
			     اگر (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     توڑو;

	طےشدہ:	     اگر (!blocked)
				 واپس TRUE;	// exit!
			     got_signal = sig;
#اگر_تعریف_شدہ SIGPWR
			     اگر (sig != SIGPWR)
#اگر_ختم
				 got_int = TRUE;    // break any loops
			     توڑو;
    }
    واپس FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    صحیح
mch_check_win(صحیح argc غیر_مستعمل, حرف **argv غیر_مستعمل)
{
    اگر (isatty(1))
	واپس OK;
    واپس FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    صحیح
mch_داخلہ_isatty(خالی)
{
    اگر (isatty(read_cmd_fd))
	واپس TRUE;
    واپس FALSE;
}

#اگر_تعریف_شدہ FEAT_X11

# اگر defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ساکن خالی
xopen_پیغام(طویل elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#اگر_ختم

#اگر defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ساکن صحیح	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ساکن صحیح
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGet_Error_متن(dpy, error_event->error_رمز, (حرف *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    اگر (!gui.اندر_use)
# endif
	پیغام((حرف *)IObuff);

    واپس 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ساکن صحیح
x_error_check(Display *dpy غیر_مستعمل, XErrorEvent *error_event غیر_مستعمل)
{
    got_x_error = TRUE;
    واپس 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ساکن صحیح
x_connect_مقصد_server(خالی)
{
    // No point in connecting if we are exiting or dying.
    اگر (exiting || v_dying)
	واپس FALSE;

# اگر defined(FEAT_CLIENTSERVER)
    اگر (x_force_connect)
	واپس TRUE;
# endif
    اگر (x_no_connect)
	واپس FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    اگر (clip_exclude_prog != تہی)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	اگر (regprog_اندر_use(clip_exclude_prog))
	    واپس FALSE;

	اگر (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    واپس FALSE;
    }
    واپس TRUE;
}

# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  اگر defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ساکن صحیح
x_IOerror_check(Display *dpy غیر_مستعمل)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   اگر defined(VMS) || defined(__CYGWIN__)
    واپس 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ساکن صحیح xterm_dpy_retry_گنتی = 0;

    ساکن صحیح
x_IOerror_handler(Display *dpy غیر_مستعمل)
{
    xterm_dpy = تہی;
    xterm_dpy_retry_گنتی = 5;  // Try reconnecting five times
    x11_کھڑکی = 0;
    x11_display = تہی;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  اگر defined(VMS) || defined(__CYGWIN__)
    واپس 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ساکن خالی
may_restore_x11_clipboard(خالی)
{
    // No point in restoring the connecting if we are exiting or dying.
    اگر (!exiting && !v_dying && xterm_dpy_retry_گنتی > 0)
    {
	--xterm_dpy_retry_گنتی;

#  ifndef LESSTIF_نسخہ
	// This has been reported to avoid Vim getting stuck.
	اگر (app_context != (XtAppContext)تہی)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)تہی;
	    x11_display = تہی; // freed by XtDestroyApplicationContext()
	}
#  endif

	تیاری_رکن_clip();
	get_x11_title(FALSE);
    }
}

    خالی
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    اگر (eap->arg != تہی && (arglen = STRLEN(eap->arg)) > 0)
    {
	اگر (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (حرف *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == تہی
		    ? (حرف *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_کھڑکی = 0;
    xterm_dpy_retry_گنتی = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    انتخاب_clipmethod();
}
# endif

/*
 * Test if "dpy" and x11_window are valid by getting the window title.
 * I don't actually want it yet, so there may be a simpler call to use, but
 * this will cause the error handler x_error_check() to be called if anything
 * is wrong, such as the window pointer being invalid (as can happen when the
 * user changes his DISPLAY, but not his WINDOWID) -- webb
 */
    ساکن صحیح
test_x11_کھڑکی(Display *dpy)
{
    صحیح			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	متن_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    اگر (XGetWMName(dpy, x11_کھڑکی, &متن_prop))
	XFree((خالی *)متن_prop.قدر);
    XSync(dpy, False);
    (خالی)XSetErrorHandler(old_handler);

    اگر (p_verbose > 0 && got_x_error)
	verb_پیغام(_("Testing the X display failed"));

    واپس (got_x_error ? FAIL : OK);
}
#اگر_ختم


#اگر_تعریف_شدہ FEAT_X11

ساکن صحیح get_x11_thing(صحیح get_title, صحیح test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ساکن صحیح
get_x11_windis(خالی)
{
    حرف	    *winid;
    ساکن صحیح	    نتیجہ = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_یہاں	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ساکن صحیح	    x11_display_from = XD_NONE;
    ساکن صحیح	    did_سیٹ_error_handler = FALSE;

    اگر (!did_سیٹ_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(خالی)XSetErrorHandler(x_error_handler);
	did_سیٹ_error_handler = TRUE;
    }

# اگر defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    اگر (gui.اندر_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	اگر (x11_display_from == XD_یہاں && x11_display != تہی)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	اگر (gui_get_x11_windis(&x11_کھڑکی, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    واپس OK;
	}
	x11_display = تہی;
	واپس FAIL;
    }
    ورنہ اگر (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_کھڑکی = 0;
	x11_display = تہی;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    اگر (!x_connect_مقصد_server())
	واپس FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    اگر (x11_کھڑکی == 0 && (winid = getenv("WINDOWID")) != تہی)
	x11_کھڑکی = (کھڑکی)atol(winid);

# ifdef FEAT_XCLIPBOARD
    اگر (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    اگر (xterm_dpy != تہی && x11_کھڑکی != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	اگر (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    اگر (x11_display_from == XD_یہاں && x11_display != تہی)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	اگر (test_x11_کھڑکی(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_کھڑکی = 0;
	    x11_display = تہی;
	    x11_display_from = XD_NONE;
	    واپس FAIL;
	}
	واپس OK;
    }
# endif

    اگر (x11_کھڑکی == 0 || x11_display == تہی)
	نتیجہ = -1;

    اگر (نتیجہ != -1)	    // Have already been here and set this
	واپس نتیجہ;	    // Don't do all these X calls again

    اگر (x11_کھڑکی != 0 && x11_display == تہی)
    {
# ifdef سیٹ_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T آغاز_tv;

	اگر (p_verbose > 0)
	    ELAPSED_INIT(آغاز_tv);
# endif

# ifdef سیٹ_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(تہی);

# ifdef سیٹ_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	اگر (p_verbose > 0 && sig_alarm_called)
	    verb_پیغام(_("Opening the X display timed out"));
# endif
	اگر (x11_display != تہی)
	{
# ifdef ELAPSED_FUNC
	    اگر (p_verbose > 0)
	    {
		verbose_enter();
		xopen_پیغام(ELAPSED_FUNC(آغاز_tv));
		verbose_leave();
	    }
# endif
	    اگر (test_x11_کھڑکی(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_کھڑکی = 0;
		XCloseDisplay(x11_display);
		x11_display = تہی;
	    }
	    ورنہ
		x11_display_from = XD_یہاں;
	}
    }
    اگر (x11_کھڑکی == 0 || x11_display == تہی)
	واپس (نتیجہ = FAIL);

# ifdef FEAT_EVAL
    سیٹ_vim_var_nr(VV_WINDOWID, (طویل)x11_کھڑکی);
# endif

    واپس (نتیجہ = OK);
}

/*
 * Determine original x11 Window Title
 */
    ساکن صحیح
get_x11_title(صحیح test_only)
{
    واپس get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ساکن صحیح
get_x11_icon(صحیح test_only)
{
    صحیح		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    اگر (oldicon == تہی && !test_only)
    {
	اگر (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ورنہ
	    oldicon = vim_strsave(T_NAME);
    }

    واپس retval;
}

    ساکن صحیح
get_x11_thing(
    صحیح		get_title,	// get title string
    صحیح		test_only)
{
    XTextProperty	متن_prop;
    صحیح			retval = FALSE;
    حالت_3		حالت_2;

    اگر (get_x11_windis() != OK)
	واپس FALSE;

    // Get window/icon name if any
    اگر (get_title)
	حالت_2 = XGetWMName(x11_display, x11_کھڑکی, &متن_prop);
    ورنہ
	حالت_2 = XGetWMIconName(x11_display, x11_کھڑکی, &متن_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    کھڑکی	    جڑ;
    کھڑکی	    والد;
    کھڑکی	    win = x11_کھڑکی;
    کھڑکی	   *children;
    غیرعلامتی صحیح    عدد_children;

    جب (!حالت_2 || متن_prop.قدر == تہی)
    {
	اگر (!XQuery_درخت(x11_display, win, &جڑ, &والد, &children,
		    &عدد_children))
	    توڑو;
	اگر (children)
	    XFree((خالی *)children);
	اگر (والد == جڑ || والد == 0)
	    توڑو;

	win = والد;
	اگر (get_title)
	    حالت_2 = XGetWMName(x11_display, win, &متن_prop);
	ورنہ
	    حالت_2 = XGetWMIconName(x11_display, win, &متن_prop);
    }

    اگر (حالت_2 && متن_prop.قدر != تہی)
    {
	retval = TRUE;
	اگر (!test_only)
	{
	    اگر (get_title)
		vim_free(oldtitle);
	    ورنہ
		vim_free(oldicon);
	    اگر (متن_prop.encoding == XA_STRING && !has_mbyte)
	    {
		اگر (get_title)
		    oldtitle = vim_strsave((char_u *)متن_prop.قدر);
		ورنہ
		    oldicon = vim_strsave((char_u *)متن_prop.قدر);
	    }
	    ورنہ
	    {
		حرف    **cl;
		حالت_3  transform_حالت;
		صحیح	    n = 0;

		transform_حالت = Xmb_متن_Property_مقصد_متن_List(x11_display,
			&متن_prop,
			&cl, &n);
		اگر (transform_حالت >= Success && n > 0 && cl[0])
		{
		    اگر (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    ورنہ
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		ورنہ
		{
		    اگر (get_title)
			oldtitle = vim_strsave((char_u *)متن_prop.قدر);
		    ورنہ
			oldicon = vim_strsave((char_u *)متن_prop.قدر);
		}
	    }
	}
	XFree((خالی *)متن_prop.قدر);
    }
    واپس retval;
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
    ساکن خالی
سیٹ_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    اگر (enc_utf8)
	Xutf8_سیٹ_WMProperties(x11_display, x11_کھڑکی, (ثابت حرف *)title,
					     تہی, تہی, 0, تہی, تہی, تہی);
    ورنہ
# endif
    {
# اگر XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_سیٹ_WMProperties(x11_display, x11_کھڑکی, (ثابت حرف *)title,
					     تہی, تہی, 0, تہی, تہی, تہی);
#  ورنہ
	XTextProperty	متن_prop;
	حرف		*c_title = (حرف *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(خالی)XString_List_مقصد_متن_Property(&c_title, 1, &متن_prop);
	XSetWMProperties(x11_display, x11_کھڑکی, &متن_prop,
					     تہی, تہی, 0, تہی, تہی, تہی);
#  endif
# ورنہ
	XStoreName(x11_display, x11_کھڑکی, (حرف *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ساکن خالی
سیٹ_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    اگر (enc_utf8)
	Xutf8_سیٹ_WMProperties(x11_display, x11_کھڑکی, تہی, (ثابت حرف *)icon,
						   تہی, 0, تہی, تہی, تہی);
    ورنہ
# endif
    {
# اگر XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_سیٹ_WMProperties(x11_display, x11_کھڑکی, تہی, (ثابت حرف *)icon,
						   تہی, 0, تہی, تہی, تہی);
#  ورنہ
	XTextProperty	متن_prop;
	حرف		*c_icon = (حرف *)icon;

	(خالی)XString_List_مقصد_متن_Property(&c_icon, 1, &متن_prop);
	XSetWMProperties(x11_display, x11_کھڑکی, تہی, &متن_prop,
						   تہی, 0, تہی, تہی, تہی);
#  endif
# ورنہ
	XSetIconName(x11_display, x11_کھڑکی, (حرف *)icon);
# endif
    }
    XFlush(x11_display);
}

#ورنہ  // FEAT_X11

    ساکن صحیح
get_x11_title(صحیح test_only غیر_مستعمل)
{
    واپس FALSE;
}

    ساکن صحیح
get_x11_icon(صحیح test_only)
{
    اگر (!test_only)
    {
	اگر (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ورنہ
	    oldicon = vim_strsave(T_NAME);
    }
    واپس FALSE;
}

#اگر_ختم // FEAT_X11

    صحیح
mch_can_restore_title(خالی)
{
    واپس get_x11_title(TRUE);
}

    صحیح
mch_can_restore_icon(خالی)
{
    واپس get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    خالی
mch_settitle(char_u *title, char_u *icon)
{
    صحیح		type = 0;
    ساکن صحیح	recursive = 0;

    اگر (T_NAME == تہی)	    // no terminal name (yet)
	واپس;
    اگر (title == تہی && icon == تہی)	    // nothing to do
	واپس;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    اگر (recursive)
	واپس;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#اگر_تعریف_شدہ FEAT_X11
    اگر (get_x11_windis() == OK)
	type = 1;
#اگر_ختم
#اگر defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    اگر (gui.اندر_use)
	type = 1;
#اگر_ختم

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    اگر ((type || *T_TS != NUL) && title != تہی)
    {
	اگر (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	اگر (oldtitle == تہی
#اگر_تعریف_شدہ FEAT_GUI
		&& !gui.اندر_use
#اگر_ختم
		)		// first call but not in GUI, save title
	    (خالی)get_x11_title(FALSE);

	اگر (*T_TS != NUL)		// it's OK if t_fs is empty
	    رکن_settitle(title);
#اگر_تعریف_شدہ FEAT_X11
	ورنہ
# ifdef FEAT_GUI_GTK
	اگر (!gui.اندر_use)		// don't do this if GTK+ is running
# endif
	    سیٹ_x11_title(title);		// x11
#اگر_ختم
#اگر defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	ورنہ
	    gui_mch_settitle(title, icon);
#اگر_ختم
	unix_did_سیٹ_title = TRUE;
    }

    اگر ((type || *T_CIS != NUL) && icon != تہی)
    {
	اگر (oldicon == تہی
#اگر_تعریف_شدہ FEAT_GUI
		&& !gui.اندر_use
#اگر_ختم
		)		// first call, save icon
	    get_x11_icon(FALSE);

	اگر (*T_CIS != NUL)
	{
	    برآمد_str(T_CIS);			// set icon start
	    برآمد_str_nf(icon);
	    برآمد_str(T_CIE);			// set icon end
	    برآمد_flush();
	}
#اگر_تعریف_شدہ FEAT_X11
	ورنہ
# ifdef FEAT_GUI_GTK
	اگر (!gui.اندر_use)		// don't do this if GTK+ is running
# endif
	    سیٹ_x11_icon(icon);			// x11
#اگر_ختم
	did_سیٹ_icon = TRUE;
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
    خالی
mch_restore_title(صحیح which)
{
    صحیح	do_ڈالو_نکالو = unix_did_سیٹ_title || did_سیٹ_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_سیٹ_title)
			? (oldtitle ? oldtitle : p_titleold) : تہی;
    char_u *tofree = تہی;
    اگر (title == oldtitle && oldtitle != تہی)
    {
	tofree = vim_strsave(title);
	اگر (tofree != تہی)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_سیٹ_icon) ? oldicon : تہی);
    vim_free(tofree);

    اگر (do_ڈالو_نکالو)
    {
	// pop and push from/to the stack
	رکن_نکالو_title(which);
	رکن_ڈالو_title(which);
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
    اگر (name == تہی)
	واپس FALSE;
    واپس ((STRNICMP(name, "xterm", 5) == 0
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
    واپس (name != تہی
	    && (رکن_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#اگر_ختم

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 * Return 3 for "urxvt".
 * Return 4 for "sgr".
 */
    صحیح
use_xterm_mouse(خالی)
{
    اگر (ttym_flags == TTYM_SGR)
	واپس 4;
    اگر (ttym_flags == TTYM_URXVT)
	واپس 3;
    اگر (ttym_flags == TTYM_XTERM2)
	واپس 2;
    اگر (ttym_flags == TTYM_XTERM)
	واپس 1;
    واپس 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    صحیح
vim_is_iris(char_u *name)
{
    اگر (name == تہی)
	واپس FALSE;
    واپس (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#اگر defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    صحیح
vim_is_vt300(char_u *name)
{
    اگر (name == تہی)
	واپس FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    واپس ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != تہی)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#اگر_ختم

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    صحیح
mch_get_user_name(char_u *s, صحیح len)
{
#اگر_تعریف_شدہ VMS
    vim_strncpy(s, (char_u *)cuserid(تہی), len - 1);
    واپس OK;
#ورنہ
    واپس mch_get_uname(getuid(), s, len);
#اگر_ختم
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    صحیح
mch_get_uname(uid_t uid, char_u *s, صحیح len)
{
#اگر defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ساخت passwd   *pw;

    اگر ((pw = getpwuid(uid)) != تہی
	    && pw->pw_name != تہی && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	واپس OK;
    }
#اگر_ختم
    sprintf((حرف *)s, "%d", (صحیح)uid);	    // assumes s is long enough
    واپس FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#اگر_تعریف_شدہ HAVE_SYS_UTSNAME_H
    خالی
mch_get_host_name(char_u *s, صحیح len)
{
    ساخت utsname vutsname;

    اگر (uname(&vutsname) < 0)
	*s = NUL;
    ورنہ
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#ورنہ // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    خالی
mch_get_host_name(char_u *s, صحیح len)
{
# ifdef VAXC
    vaxc$gethostname((حرف *)s, len);
# ورنہ
    gethostname((حرف *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#اگر_ختم // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    طویل
mch_get_pid(خالی)
{
    واپس (طویل)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    صحیح
mch_process_running(طویل pid)
{
    // If there is no error the process must be running.
    اگر (kill(pid, 0) == 0)
	واپس TRUE;
#اگر_تعریف_شدہ ESRCH
    // If the error is ESRCH then the process is not running.
    اگر (errno == ESRCH)
	واپس FALSE;
#اگر_ختم
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    واپس TRUE;
}

#اگر !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ساکن حرف *
strerror(صحیح err)
{
    خارجی صحیح	    sys_nerr;
    خارجی حرف	    *sys_errlist[];
    ساکن حرف	    er[20];

    اگر (err > 0 && err < sys_nerr)
	واپس (sys_errlist[err]);
    snprintf(er, حجم(er), "Error %d", err);
    واپس er;
}
#اگر_ختم

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    صحیح
mch_dirname(char_u *buf, صحیح len)
{
#اگر defined(USE_GETCWD)
    اگر (getcwd((حرف *)buf, len) == تہی)
    {
	STRCPY(buf, strerror(errno));
	واپس FAIL;
    }
    واپس OK;
#ورنہ
    واپس (getwd((حرف *)buf) != تہی ? OK : FAIL);
#اگر_ختم
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    صحیح
mch_بھرا_Name(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)		// also expand when already absolute path
{
    صحیح		buflen = 0;
#اگر_تعریف_شدہ HAVE_FCHDIR
    صحیح		fd = -1;
    ساکن صحیح	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#اگر_ختم
    char_u	olddir[MAXPATHL];
    char_u	*p;
    صحیح		retval = OK;
#اگر_تعریف_شدہ __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#اگر_ختم

#اگر_تعریف_شدہ VMS
    fname = vms_fixfilename(fname);
#اگر_ختم

#اگر_تعریف_شدہ __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# اگر CYGWIN_نسخہ_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_راستہ(CCP_WIN_A_مقصد_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# ورنہ
    cygwin_conv_مقصد_posix_راستہ(fname, posix_fname);
# endif
    fname = posix_fname;
#اگر_ختم

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    اگر ((force || !mch_is_بھرا_Name(fname))
	    && ((p = vim_strrchr(fname, '/')) == تہی || p != fname))
    {
	اگر (p == تہی && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	اگر (p != تہی)
	{
	    اگر (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#اگر_تعریف_شدہ HAVE_FCHDIR
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
#اگر_ختم

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    اگر (
#اگر_تعریف_شدہ HAVE_FCHDIR
		fd < 0 &&
#اگر_ختم
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((حرف *)olddir) != 0))
	    {
		p = تہی;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    ورنہ
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		اگر (p - fname >= len)
		    retval = FAIL;
		ورنہ
		{
		    vim_strncpy(buf, fname, p - fname);
		    اگر (mch_chdir((حرف *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			اگر (mch_is_بھرا_Name(fname))
			    retval = FAIL;
			ورنہ
			    p = تہی;
		    }
		    ورنہ اگر (*p == '/')
			fname = p + 1;
		    ورنہ
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
	اگر (p != تہی)
	{
	    صحیح	l;

#اگر_تعریف_شدہ HAVE_FCHDIR
	    اگر (fd >= 0)
	    {
		اگر (p_verbose >= 5)
		{
		    verbose_enter();
		    پیغام("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    ورنہ
#اگر_ختم
		l = mch_chdir((حرف *)olddir);
	    اگر (l != 0)
		emsg(_(e_cannot_go_عقب_مقصد_پچھلا_directory));
	}
#اگر_تعریف_شدہ HAVE_FCHDIR
	اگر (fd >= 0)
	    close(fd);
#اگر_ختم

	buflen = (صحیح)STRLEN(buf);
	اگر (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#اگر_غیر_تعریف_شدہ VMS
	ورنہ اگر (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += حجم(PATHSEP);
	}
#اگر_ختم
    }

    اگر (buflen == 0)
	buflen = (صحیح)STRLEN(buf);

    // Catch file names which are too long.
    اگر (retval == FAIL || (صحیح)(buflen + STRLEN(fname)) >= len)
	واپس FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    اگر (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    واپس OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_is_بھرا_Name(char_u *fname)
{
#اگر_تعریف_شدہ VMS
    واپس ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((حرف *)fname,':') || strchr((حرف *)fname,'"') ||
	    (strchr((حرف *)fname,'[') && strchr((حرف *)fname,']'))||
	    (strchr((حرف *)fname,'<') && strchr((حرف *)fname,'>'))   );
#ورنہ
    واپس (*fname == '/' || *fname == '~');
#اگر_ختم
}

#اگر defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    خالی
fname_case(
    char_u	*name,
    صحیح		len غیر_مستعمل)  // buffer size, only used when name gets longer
{
    ساخت stat st;
    char_u	*slash, *دم;
    size_t	taillen;
    DIR		*dirp;
    ساخت dirent *dp;

    اگر (mch_lstat((حرف *)name, &st) < 0)
	واپس;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    اگر (slash == تہی)
    {
	dirp = opendir(".");
	دم = name;
    }
    ورنہ
    {
	*slash = NUL;
	dirp = opendir((حرف *)name);
	*slash = '/';
	دم = slash + 1;
    }

    اگر (dirp == تہی)
	واپس;

    taillen = STRLEN(دم);
    جب ((dp = readdir(dirp)) != تہی)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	اگر (STRICMP(دم, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ساخت stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (دم - name), (char_u *)dp->d_name,
		    MAXPATHL - (دم - name));
	    اگر (mch_lstat((حرف *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(دم, dp->d_name);
		توڑو;
	    }
	}
    }

    closedir(dirp);
}
#اگر_ختم

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    طویل
mch_getperm(char_u *name)
{
    ساخت stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#اگر_تعریف_شدہ VMS
    اگر (stat((حرف *)vms_fixfilename(name), &statb))
#ورنہ
    اگر (stat((حرف *)name, &statb))
#اگر_ختم
	واپس -1;
#اگر_تعریف_شدہ __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    واپس statb.st_منوال & ~S_ADDACE;
#ورنہ
    واپس statb.st_منوال;
#اگر_ختم
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
mch_setperm(char_u *name, طویل perm)
{
    واپس (chmod((حرف *)
#اگر_تعریف_شدہ VMS
		    vms_fixfilename(name),
#ورنہ
		    name,
#اگر_ختم
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#اگر defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    صحیح
mch_fsetperm(صحیح fd, طویل perm)
{
    واپس (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#اگر_ختم

#اگر defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
قسمتعریف ساخت vim_acl_solaris_T {
    صحیح acl_cnt;
    aclent_t *acl_اندراج;
} vim_acl_solaris_T;
# endif

# اگر defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    خالی
mch_copy_sec(char_u *from_file, char_u *مقصد_file)
{
    اگر (from_file == تہی)
	واپس;

    اگر (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    اگر (selinux_enabled <= 0)
	واپس;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    حرف *from_context = تہی;
    حرف *مقصد_context = تہی;

    اگر (getfilecon((حرف *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	اگر (errno == EOPNOTSUPP)
	    واپس;

	پیغام_puts(_("\nCould not get security context for "));
	پیغام_outtrans(from_file);
	پیغام_putchar('\n');
	واپس;
    }
    اگر (getfilecon((حرف *)مقصد_file, &مقصد_context) < 0)
    {
	پیغام_puts(_("\nCould not get security context for "));
	پیغام_outtrans(مقصد_file);
	پیغام_putchar('\n');
	freecon (from_context);
	واپس ;
    }
    اگر (strcmp(from_context, مقصد_context) != 0)
    {
	اگر (setfilecon((حرف *)مقصد_file, from_context) < 0)
	{
	    پیغام_puts(_("\nCould not set security context for "));
	    پیغام_outtrans(مقصد_file);
	    پیغام_putchar('\n');
	}
    }
    freecon(مقصد_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# اگر defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    خالی
mch_copy_sec(char_u *from_file, char_u *مقصد_file)
{
    ساکن ثابت حرف * ثابت smack_copied_اوصاف[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    حرف	بفر[SMACK_LABEL_LEN];
    ثابت حرف	*name;
    صحیح		اشاریہ;
    صحیح		ret;
    ssize_t	سائز;

    اگر (from_file == تہی)
	واپس;

    سائز = listxattr((حرف *)from_file, تہی, 0);
    // not supported or no attributes to copy
    اگر (سائز <= 0)
	واپس;

    برائے (اشاریہ = 0 ; اشاریہ < (صحیح)ARRAY_لمبائی(smack_copied_اوصاف); اشاریہ++)
    {
	// get the name of the attribute to copy
	name = smack_copied_اوصاف[اشاریہ];

	// get the value of the attribute in buffer
	سائز = getxattr((حرف*)from_file, name, بفر, حجم(بفر));
	اگر (سائز >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((حرف*)مقصد_file, name, بفر, (size_t)سائز, 0);
	    اگر (ret < 0)
	    {
		vim_snprintf((حرف *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, مقصد_file);
		پیغام_outtrans(IObuff);
		پیغام_putchar('\n');
	    }
	}
	ورنہ
	{
	    // what reason of not having the attribute value?
	    انتخاب (errno)
	    {
		حالت ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    واپس; // leave because it isn't useful to continue

		حالت ERANGE:
		طےشدہ:
		    // no enough size OR unexpected error
		     vim_snprintf((حرف *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    پیغام_puts((حرف *)IObuff);
		    پیغام_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		حالت ENODATA:
		    // no attribute of this name
		    ret = removexattr((حرف*)مقصد_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    توڑو;
	    }
	}
    }
}
# endif // HAVE_SMACK

# اگر defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    خالی
mch_copy_xattr(char_u *from_file, char_u *مقصد_file)
{
    حرف	*xattr_buf;
    ssize_t	سائز;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    حرف	*کلید;
    حرف	*val = تہی;
    حرف	*errmsg = تہی;

    اگر (from_file == تہی)
	واپس;

    // get the length of the extended attributes
    سائز = listxattr((حرف *)from_file, تہی, 0);
    // not supported or no attributes to copy
    اگر (سائز <= 0)
	واپس;
    xattr_buf = (حرف*)alloc(سائز);
    اگر (xattr_buf == تہی)
	واپس;
    سائز = listxattr((حرف *)from_file, xattr_buf, سائز);
    tsize = سائز;

    errno = 0;

    برائے (صحیح باری = 0; باری < 2; باری++)
    {

	کلید = xattr_buf;
	اگر (باری == 1)
	    سائز = tsize;

	جب (سائز > 0)
	{
	    vallen = getxattr((حرف *)from_file, کلید,
		    val, باری ? max_vallen : 0);
	    // only set the attribute in the second round
	    اگر (vallen >= 0 && باری &&
		setxattr((حرف *)مقصد_file, کلید, val, vallen, 0) == 0)
		;
	    ورنہ اگر (errno)
	    {
		انتخاب (errno)
		{
		    حالت E2BIG:
			errmsg = e_xattr_e2big;
			جاؤ error_exit;
		    حالت ENOTSUP:
		    حالت EACCES:
		    حالت EPERM:
			توڑو;
		    حالت ERANGE:
			errmsg = e_xattr_erange;
			جاؤ error_exit;
		    طےشدہ:
			errmsg = e_xattr_other;
			جاؤ error_exit;
		}
	    }

	    اگر (باری == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(کلید) + 1;
	    سائز -= keylen;
	    کلید += keylen;
	}
	اگر (باری)
	    توڑو;

	val = (حرف*)alloc(max_vallen + 1);
	اگر (val == تہی)
	    جاؤ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    اگر (errmsg != تہی)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname غیر_مستعمل)
{
    vim_acl_T	ret = تہی;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((حرف *)fname, ACL_TYPE_ACCESS);
# ورنہ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    اگر (acl_get((حرف *)fname, 0, &aclent) < 0)
	واپس تہی;
    ret = (vim_acl_T)aclent;
#  ورنہ
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = حافظہ_دو(حجم(vim_acl_solaris_T));
    اگر (aclent == تہی)
	واپس تہی;
    اگر ((aclent->acl_cnt = acl((حرف *)fname, GETACLCNT, 0, تہی)) < 0)
    {
	آزاد(aclent);
	واپس تہی;
    }
    aclent->acl_اندراج = حافظہ_دو(aclent->acl_cnt * حجم(aclent_t));
    اگر (aclent->acl_اندراج == تہی)
    {
	آزاد(aclent);
	واپس تہی;
    }
    اگر (acl((حرف *)fname, GETACL, aclent->acl_cnt, aclent->acl_اندراج) < 0)
    {
	آزاد(aclent->acl_اندراج);
	آزاد(aclent);
	واپس تہی;
    }
    ret = (vim_acl_T)aclent;
#   ورنہ
#    اگر defined(HAVE_AIX_ACL)
    صحیح		aclsize;
    ساخت acl *aclent;

    aclsize = حجم(ساخت acl);
    aclent = حافظہ_دو(aclsize);
    اگر (aclent == تہی)
	واپس تہی;
    اگر (statacl((حرف *)fname, STX_معمول, aclent, aclsize) < 0)
    {
	اگر (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    اگر (aclent == تہی || statacl((حرف *)fname, STX_معمول, aclent, aclsize) < 0)
	    {
		آزاد(aclent);
		واپس تہی;
	    }
	}
	ورنہ
	{
	    آزاد(aclent);
	    واپس تہی;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    واپس ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    خالی
mch_سیٹ_acl(char_u *fname غیر_مستعمل, vim_acl_T aclent)
{
    اگر (aclent == تہی)
	واپس;
# ifdef HAVE_POSIX_ACL
    acl_سیٹ_file((حرف *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# ورنہ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_سیٹ((حرف *)fname, (acl_t *)aclent);
#  ورنہ
#   ifdef HAVE_SOLARIS_ACL
    acl((حرف *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_اندراج);
#   ورنہ
#    ifdef HAVE_AIX_ACL
    chacl((حرف *)fname, aclent, ((ساخت acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    خالی
mch_free_acl(vim_acl_T aclent)
{
    اگر (aclent == تہی)
	واپس;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# ورنہ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  ورنہ
#   ifdef HAVE_SOLARIS_ACL
    آزاد(((vim_acl_solaris_T *)aclent)->acl_اندراج);
    آزاد(aclent);
#   ورنہ
#    ifdef HAVE_AIX_ACL
    آزاد(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#اگر_ختم

/*
 * Set hidden flag for "name".
 */
    خالی
mch_hide(char_u *name غیر_مستعمل)
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
    ساخت stat statb;

    اگر (*name == NUL)	    // Some stat()s don't flag "" as an error.
	واپس FALSE;
    اگر (stat((حرف *)name, &statb))
	واپس FALSE;
    واپس (S_ISDIR(statb.st_منوال) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    صحیح
mch_isrealdir(char_u *name)
{
    ساخت stat statb;

    اگر (*name == NUL)	    // Some stat()s don't flag "" as an error.
	واپس FALSE;
    اگر (mch_lstat((حرف *)name, &statb))
	واپس FALSE;
    واپس (S_ISDIR(statb.st_منوال) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ساکن صحیح
executable_file(char_u *name)
{
    ساخت stat	st;

    اگر (stat((حرف *)name, &st))
	واپس 0;
#اگر_تعریف_شدہ VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    صحیح vms_executable = 0;
    اگر (S_ISREG(st.st_منوال) && mch_access((حرف *)name, X_OK) == 0)
    {
	اگر (لڑی_تلاش(vms_tolower((حرف*)name),".exe") != تہی
		|| لڑی_تلاش(vms_tolower((حرف*)name),".com")!= تہی)
	    vms_executable = 1;
    }
    واپس vms_executable;
#ورنہ
    واپس S_ISREG(st.st_منوال) && mch_access((حرف *)name, X_OK) == 0;
#اگر_ختم
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحیح
mch_can_exe(char_u *name, char_u **راستہ, صحیح use_راستہ)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_انتہا;
    size_t	elen;
    صحیح		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    اگر (!use_راستہ || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	اگر ((use_راستہ || gettail(name) != name) && executable_file(name))
	{
	    اگر (راستہ != تہی)
	    {
		اگر (name[0] != '/')
		    *راستہ = بھرا_Name_save(name, TRUE);
		ورنہ
		    *راستہ = vim_strsave(name);
	    }
	    واپس TRUE;
	}
	واپس FALSE;
    }

    p = (char_u *)getenv("PATH");
    اگر (p == تہی || *p == NUL)
	واپس -1;
    p_انتہا = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_انتہا - p) + 2;
    buf = alloc(bufsize);
    اگر (buf == تہی)
	واپس -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    برائے (;;)
    {
	e = (char_u *)strchr((حرف *)p, ':');
	اگر (e == تہی)
	    e = p_انتہا;
	elen = (size_t)(e - p);
	اگر (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((حرف *)buf, bufsize, "%.*s%s%s",
		(صحیح)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	اگر (retval == 1)
	{
	    اگر (راستہ != تہی)
	    {
		اگر (buf[0] != '/')
		    *راستہ = بھرا_Name_save(buf, TRUE);
		ورنہ
		    *راستہ = vim_strnsave(buf, buflen);
	    }
	    توڑو;
	}

	اگر (*e != ':')
	    توڑو;
	p = e + 1;
    }

    vim_free(buf);
    واپس retval;
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
    ساخت stat	st;

    اگر (stat((حرف *)name, &st))
	واپس گرہ_معمول;
    اگر (S_ISREG(st.st_منوال) || S_ISDIR(st.st_منوال))
	واپس گرہ_معمول;
    اگر (S_ISBLK(st.st_منوال))	// block device isn't writable
	واپس گرہ_OTHER;
    // Everything else is writable?
    واپس گرہ_WRITABLE;
}

    خالی
mch_early_init(خالی)
{
#اگر_تعریف_شدہ HAVE_CHECK_ڈھیر_GROWTH
    صحیح			i;

    check_ڈھیر_growth((حرف *)&i);

# ifdef HAVE_ڈھیر_حد_بندی
    get_ڈھیر_حد_بندی();
# endif

#اگر_ختم

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ڈھیر = alloc(get_signal_ڈھیر_سائز());
    init_signal_ڈھیر();
#اگر_ختم

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#اگر_تعریف_شدہ MACOS_X
# ifdef MAC_OS_X_نسخہ_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_سیٹ(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#اگر_ختم
}

#اگر defined(EXITFREE)
    خالی
mch_free_mem(خالی)
{
# اگر defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    اگر (clip_ستارہ.owned)
	clip_lose_selection(&clip_ستارہ);
    اگر (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    اگر (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_نسخہ
    // Lesstif crashes here, lose some memory
    اگر (xterm_dpy != تہی)
	XtCloseDisplay(xterm_dpy);
    اگر (app_context != (XtAppContext)تہی)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = تہی; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# اگر defined(FEAT_X11)
    اگر (x11_display != تہی
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# اگر defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ڈھیر);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#اگر_ختم

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ساکن خالی
exit_scroll(خالی)
{
    اگر (silent_منوال)
	واپس;
    اگر (newline_on_exit || پیغام_didout)
    {
	اگر (پیغام_use_printf())
	{
	    اگر (info_پیغام)
		mch_پیغام("\n");
	    ورنہ
		mch_errmsg("\r\n");
	}
	ورنہ
	    برآمد_char('\n');
    }
    ورنہ اگر (!is_not_a_رکن())
    {
	restore_cterm_colors();		// get original colors back
	پیغام_clr_eos_force();		// clear the rest of the display
	windgoto((صحیح)صفیں - 1, cmdline_ستون_off); // may have moved the cursor
    }
}

#اگر_تعریف_شدہ USE_GCOV_FLUSH
# اگر (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
خارجی خالی __gcov_dump(خالی);
خارجی خالی __gcov_reset(خالی);
#  define __gcov_flush() کرو { __gcov_dump(); __gcov_reset(); } جب (0)
# ورنہ
خارجی خالی __gcov_flush(خالی);
# endif
#اگر_ختم

    خالی
mch_exit(صحیح r)
{
    exiting = TRUE;

#اگر defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#اگر_ختم

#اگر_تعریف_شدہ FEAT_GUI
    اگر (!gui.اندر_use)
#اگر_ختم
    {
	settmode(TMODE_COOK);
	اگر (!is_not_a_رکن())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    رکن_نکالو_title(SAVE_RESTORE_BOTH);
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
	اگر (بھرا_screen)
	    cursor_on();
    }
    برآمد_flush();
    ml_close_all(TRUE);		// remove all memfiles

#اگر_تعریف_شدہ USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#اگر_ختم

    may_core_dump();
#اگر_تعریف_شدہ FEAT_GUI
    اگر (gui.اندر_use)
	gui_exit(r);
#اگر_ختم

#اگر_تعریف_شدہ MACOS_CONVERT
    mac_conv_cleanup();
#اگر_ختم

#اگر_تعریف_شدہ __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    اگر (deadly_signal != 0)
	واپس;
#اگر_ختم

#اگر_تعریف_شدہ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#اگر_ختم

#اگر_تعریف_شدہ EXITFREE
    free_all_mem();
#اگر_ختم

    باہر_نکلو(r);
}

    ساکن خالی
may_core_dump(خالی)
{
    اگر (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#اگر_غیر_تعریف_شدہ VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ساکن صحیح
get_tty_fd(صحیح fd)
{
    صحیح		tty_fd = fd;

# اگر defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    اگر (mch_isatty(fd) > 1)
    {
	حرف *name;

	name = ptsname(fd);
	اگر (name == تہی)
	    واپس -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	اگر (tty_fd < 0)
	    واپس -1;
    }
# endif
    واپس tty_fd;
}

    ساکن صحیح
mch_tcgetattr(صحیح fd, خالی *رکن)
{
    صحیح		tty_fd;
    صحیح		retval = -1;

    tty_fd = get_tty_fd(fd);
    اگر (tty_fd < 0)
	واپس -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ساخت termios *)رکن);
#  ورنہ
    retval = ioctl(tty_fd, TCGETA, (ساخت termio *)رکن);
#  endif
# ورنہ
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ساخت sgttyb *)رکن);
# endif
    اگر (tty_fd != fd)
	close(tty_fd);
    واپس retval;
}

    خالی
mch_settmode(tmode_T tmode)
{
    ساکن صحیح پہلا = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ساکن ساخت termios told;
	   ساخت termios tnew;
#  ورنہ
    ساکن ساخت termio told;
	   ساخت termio tnew;
#  endif

    اگر (پہلا)
    {
	پہلا = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    اگر (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == تہی || *T_XON == NUL ? IXON : 0));
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
#   ورنہ
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    ورنہ
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    ورنہ اگر (tmode == TMODE_SLEEP)
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
	جب (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  ورنہ
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# ورنہ
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ساکن ساخت sgttyb ttybold;
	   ساخت sgttyb ttybnew;

    اگر (پہلا)
    {
	پہلا = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    اگر (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    ورنہ اگر (tmode == TMODE_SLEEP)
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
    خالی
get_stty(خالی)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    اگر (get_tty_info(read_cmd_fd, &info) != OK)
	واپس;

    intr_char = info.مداخلت;
    buf[0] = info.backspace;
    buf[1] = NUL;
    جوڑو_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = ڈھونڈو_termcode((char_u *)"kD");
    اگر (p != تہی && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(تہی);
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
    ساخت termios کلیدیں;
#  ورنہ
    ساخت termio کلیدیں;
#  endif

    اگر (mch_tcgetattr(fd, &کلیدیں) != -1)
    {
	info->backspace = کلیدیں.c_cc[VERASE];
	info->مداخلت = کلیدیں.c_cc[VINTR];
	اگر (کلیدیں.c_iflag & ICRNL)
	    info->enter = NL;
	ورنہ
	    info->enter = CAR;
	اگر (کلیدیں.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	ورنہ
	    info->nl_does_cr = FALSE;
	واپس OK;
    }
# ورنہ
    // for "old" tty systems
    ساخت sgttyb کلیدیں;

    اگر (mch_tcgetattr(fd, &کلیدیں) != -1)
    {
	info->backspace = کلیدیں.sg_erase;
	info->مداخلت = کلیدیں.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	واپس OK;
    }
# endif
    واپس FAIL;
}

#اگر_ختم // VMS

ساکن صحیح	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    خالی
mch_setmouse(صحیح on)
{
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
    ساکن صحیح	bevalterm_ison = FALSE;
#اگر_ختم
    صحیح		xterm_mouse_vers;

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    اگر (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#اگر_ختم

    اگر (on == mouse_ison
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
	    && p_bevalterm == bevalterm_ison
#اگر_ختم
	    )
	// return quickly if nothing to do
	واپس;

    xterm_mouse_vers = use_xterm_mouse();

#اگر_تعریف_شدہ FEAT_MOUSE_URXVT
    اگر (ttym_flags == TTYM_URXVT)
    {
	برآمد_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#اگر_ختم

    اگر (T_CXM != تہی && *T_CXM != NUL)
    {
	رکن_enable_mouse(on);
    }
    ورنہ اگر (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	برآمد_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#اگر_تعریف_شدہ FEAT_BEVAL_رکن
    اگر (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	اگر (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    برآمد_str_nf((char_u *)("\033[?1003l"));
    }
#اگر_ختم

    اگر (xterm_mouse_vers > 0)
    {
	اگر (on)	// enable mouse events, use mouse tracking if available
	    برآمد_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#اگر_تعریف_شدہ FEAT_BEVAL_رکن
			    bevalterm_ison ? "\033[?1003h" :
#اگر_ختم
			      "\033[?1002h")
			: "\033[?1000h"));
	ورنہ	// disable mouse events, could probably always send the same
	    برآمد_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#اگر_تعریف_شدہ FEAT_MOUSE_DEC
    ورنہ اگر (ttym_flags == TTYM_DEC)
    {
	اگر (on)	// enable mouse events
	    برآمد_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	ورنہ	// disable mouse events
	    برآمد_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_GPM
    ورنہ
    {
	اگر (on)
	{
	    اگر (gpm_open())
		mouse_ison = TRUE;
	}
	ورنہ
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYSMOUSE
    ورنہ
    {
	اگر (on)
	{
	    اگر (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	ورنہ
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_JSB
    ورنہ
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
	    برآمد_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# ورنہ
	    برآمد_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	ورنہ
	{
	    برآمد_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
    ورنہ
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	اگر (on)
	    برآمد_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	ورنہ
	    برآمد_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#اگر_ختم
}

#اگر defined(FEAT_BEVAL_رکن)
/*
 * Called when 'balloonevalterm' changed.
 */
    خالی
mch_bevalterm_بدلا(خالی)
{
    mch_setmouse(mouse_ison);
}
#اگر_ختم

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    خالی
check_mouse_termcode(خالی)
{
#اگر_تعریف_شدہ FEAT_MOUSE_XTERM
    اگر (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
    {
	سیٹ_mouse_termcode(KS_MOUSE, (char_u *)(رکن_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	اگر (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ورنہ
	del_mouse_termcode(KS_MOUSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_GPM
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    ورنہ
	del_mouse_termcode(KS_GPM_MOUSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_SYSMOUSE
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    ورنہ
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    ورنہ
	del_mouse_termcode(KS_NETTERM_MOUSE);
#اگر_ختم

#اگر_تعریف_شدہ FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_DEC_MOUSE, (char_u *)(رکن_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    ورنہ
	del_mouse_termcode(KS_DEC_MOUSE);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    اگر (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
	سیٹ_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    ورنہ
	del_mouse_termcode(KS_PTERM_MOUSE);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_MOUSE_URXVT
    اگر (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.اندر_use
# endif
	    )
    {
	سیٹ_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(رکن_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	اگر (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ورنہ
	del_mouse_termcode(KS_URXVT_MOUSE);
#اگر_ختم
    اگر (use_xterm_mouse() == 4
#اگر_تعریف_شدہ FEAT_GUI
	    && !gui.اندر_use
#اگر_ختم
	    )
    {
	سیٹ_mouse_termcode(KS_SGR_MOUSE, (char_u *)(رکن_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	سیٹ_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(رکن_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	اگر (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ورنہ
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#اگر_غیر_تعریف_شدہ VMS

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 * Return OK when size could be determined, FAIL otherwise.
 */
    صحیح
mch_get_shellsize(خالی)
{
    طویل	صفیں_2 = 0;
    طویل	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	ساخت winsize	ws;
	صحیح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	اگر (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	اگر (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_ستون;
	    صفیں_2 = ws.ws_صف;
#  ifdef FEAT_EVAL
	    ch_log(تہی, "Got size with TIOCGWINSZ: %ld x %ld", columns, صفیں_2);
#  endif
	}
    }
# ورنہ // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ساخت ttysize	ts;
	صحیح fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	اگر (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	اگر (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_ستون_تعداد;
	    صفیں_2 = ts.ts_سطریں;
#   ifdef FEAT_EVAL
	    ch_log(تہی, "Got size with TIOCGSIZE: %ld x %ld", columns, صفیں_2);
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
    اگر (columns == 0 || صفیں_2 == 0 || vim_strchr(p_cpo, CPO_TSIZE) != تہی)
    {
	اگر ((p = (char_u *)getenv("LINES")))
	{
	    صفیں_2 = atoi((حرف *)p);
# ifdef FEAT_EVAL
	    ch_log(تہی, "Got 'lines' from $LINES: %ld", صفیں_2);
# endif
	}
	اگر ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((حرف *)p);
# ifdef FEAT_EVAL
	    ch_log(تہی, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    اگر (columns == 0 || صفیں_2 == 0)
    {
	getlinecol(&columns, &صفیں_2);
#  ifdef FEAT_EVAL
	ch_log(تہی, "Got size from termcap: %ld x %ld", columns, صفیں_2);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    اگر (columns <= 0 || صفیں_2 <= 0)
	واپس FAIL;

    صفیں = صفیں_2;
    Columns = columns;
    حد_بندی_screen_سائز();
    واپس OK;
}

# اگر defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    خالی
mch_calc_cell_سائز(ساخت cellsize *cs_برآمد)
{
   ساخت winsize   ws;
   صحیح		    retval;
   صحیح		    x_cell_سائز;
   صحیح		    y_cell_سائز;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(تہی, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   اگر (retval == -1 || ws.ws_ستون == 0 || ws.ws_صف == 0)
   {
	cs_برآمد->cs_xpixel = -1;
	cs_برآمد->cs_ypixel = -1;
	واپس;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_سائز = ws.ws_xpixel / ws.ws_ستون;
   y_cell_سائز = ws.ws_ypixel / ws.ws_صف;

   cs_برآمد->cs_xpixel = x_cell_سائز > 0 ? x_cell_سائز : -1;
   cs_برآمد->cs_ypixel = y_cell_سائز > 0 ? y_cell_سائز : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   اگر (x_cell_سائز <= 0 || y_cell_سائز <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       برآمد_STR("\033[14t");
       برآمد_STR("\033[16t");
       برآمد_flush();
   }
}
# endif

# اگر defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    صحیح
mch_report_winsize(صحیح fd, صحیح صفیں_2, صحیح ستون_تعداد)
{
    صحیح		tty_fd;
    صحیح		retval = -1;

    tty_fd = get_tty_fd(fd);
    اگر (tty_fd < 0)
	واپس FAIL;

#  اگر defined(TIOCSWINSZ)
    ساخت winsize ws;

    ws.ws_ستون = ستون_تعداد;
    ws.ws_صف = صفیں_2;
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
    ch_log(تہی, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ساخت ttysize ts;

    ts.ts_ستون_تعداد = ستون_تعداد;
    ts.ts_سطریں = صفیں_2;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(تہی, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    اگر (tty_fd != fd)
	close(tty_fd);
    واپس retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    خالی
mch_سیٹ_shellsize(خالی)
{
    اگر (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	رکن_سیٹ_winsize((صحیح)صفیں, (صحیح)Columns);
	برآمد_flush();
	screen_آغاز();			// don't know where cursor is now
    }
}

#اگر_ختم // VMS

/*
 * Rows and/or Columns has changed.
 */
    خالی
mch_new_shellsize(خالی)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ساکن pid_t
wait4pid(pid_t بچہ, waitstatus *حالت_2)
{
    pid_t wait_pid = 0;
    طویل delay_msec = 1;

    جب (wait_pid != بچہ)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(بچہ, حالت_2, WNOHANG);
	اگر (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    اگر (++delay_msec > 10)
		delay_msec = 10;
	    جاری;
	}
	اگر (wait_pid <= 0
#اگر_تعریف_شدہ ECHILD
		&& errno == ECHILD
#اگر_ختم
	   )
	    توڑو;
    }
    واپس wait_pid;
}

#اگر !defined(USE_SYSTEM) || defined(FEAT_کام_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ساکن خالی
سیٹ_بچہ_environment(
	طویل	صفیں_2,
	طویل	columns,
	حرف	*رکن,
	صحیح	is_terminal غیر_مستعمل)
{
# ifdef HAVE_SETENV
    حرف	envbuf[50];
# ورنہ
    ساکن حرف	envbuf_رکن[30];
    ساکن حرف	envbuf_صفیں[20];
    ساکن حرف	envbuf_سطریں[20];
    ساکن حرف	envbuf_Columns[20];
    ساکن حرف	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ساکن حرف	envbuf_نسخہ[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ساکن حرف	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", رکن, 1);
    sprintf((حرف *)envbuf, "%ld", صفیں_2);
    setenv("ROWS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", صفیں_2);
    setenv("LINES", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%ld", columns);
    setenv("COLUMNS", (حرف *)envbuf, 1);
    sprintf((حرف *)envbuf, "%d", t_colors);
    setenv("COLORS", (حرف *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    اگر (is_terminal)
    {
	sprintf((حرف *)envbuf, "%ld",  (طویل)get_vim_var_nr(VV_نسخہ));
	setenv("VIM_TERMINAL", (حرف *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == تہی ? "" : (حرف *)serverName, 1);
#  endif
# ورنہ
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_رکن, حجم(envbuf_رکن), "TERM=%s", رکن);
    putenv(envbuf_رکن);
    vim_snprintf(envbuf_صفیں, حجم(envbuf_صفیں), "ROWS=%ld", صفیں_2);
    putenv(envbuf_صفیں);
    vim_snprintf(envbuf_سطریں, حجم(envbuf_سطریں), "LINES=%ld", صفیں_2);
    putenv(envbuf_سطریں);
    vim_snprintf(envbuf_Columns, حجم(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, حجم(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    اگر (is_terminal)
    {
	vim_snprintf(envbuf_نسخہ, حجم(envbuf_نسخہ),
			 "VIM_TERMINAL=%ld", (طویل)get_vim_var_nr(VV_نسخہ));
	putenv(envbuf_نسخہ);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, حجم(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == تہی ? "" : (حرف *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ساکن خالی
سیٹ_default_بچہ_environment(صحیح is_terminal)
{
    سیٹ_بچہ_environment(صفیں, Columns, "dumb", is_terminal);
}
#اگر_ختم

#اگر defined(FEAT_GUI) || defined(FEAT_کام_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ساکن خالی
open_pty(صحیح *pty_master_fd, صحیح *pty_slave_fd, char_u **name1, char_u **name2)
{
    حرف	*tty_name;

    اگر (name1 != تہی)
	*name1 = تہی;
    اگر (name2 != تہی)
	*name2 = تہی;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    اگر (*pty_master_fd < 0)
	واپس;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# ورنہ
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    اگر (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    ورنہ
    {
	اگر (name1 != تہی)
	    *name1 = vim_strsave((char_u *)tty_name);
	اگر (name2 != تہی)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#اگر_ختم

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ساکن خالی
may_send_sigint(صحیح c غیر_مستعمل, pid_t pid غیر_مستعمل, pid_t wpid غیر_مستعمل)
{
#اگر_تعریف_شدہ SIGINT
    اگر (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# ورنہ
	kill(0, SIGINT);
# endif
	اگر (wpid > 0)
	    kill(wpid, SIGINT);
    }
#اگر_ختم
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
	حرف ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    حرف	**argv = تہی;
    صحیح		argc;

    *sh_tofree = vim_strsave(p_sh);
    اگر (*sh_tofree == تہی)		// out of memory
	واپس FAIL;

    اگر (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	واپس FAIL;
    *argvp = argv;

    اگر (cmd != تہی)
    {
	char_u	*s;
	char_u	*p;

	اگر (extra_shell_arg != تہی)
	    argv[argc++] = (حرف *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	اگر (*shcf_tofree == تہی)    // out of memory
	    واپس FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	جب (*p != NUL)
	{
	    argv[argc++] = (حرف *)s;
	    جب (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (حرف *)cmd;
    }
    argv[argc] = تہی;
    واپس OK;
}
#اگر_ختم

#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ساکن صحیح
mch_call_shell_terminal(
    char_u	*cmd,
    صحیح		options غیر_مستعمل)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    حرف	**argv = تہی;
    char_u	*tofree1 = تہی;
    char_u	*tofree2 = تہی;
    صحیح		retval = -1;
    buf_T	*buf;
    کام_T	*کام;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	جاؤ theend;

    init_کام_options(&opt);
    ch_log(تہی, "starting terminal for system command '%s'", cmd);
    buf = رکن_آغاز(تہی, argv, &opt, رکن_آغاز_SYSTEM);
    اگر (buf == تہی)
	جاؤ theend;

    کام = رکن_getjob(buf->b_رکن);
    ++کام->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    اگر (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	جب (رکن_use_loop())
	{
	    اگر (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_فعال)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		اگر (terminal_loop(TRUE) == OK)
		    معمول_cmd(&oa, TRUE);
	    }
	    ورنہ
		معمول_cmd(&oa, TRUE);
	}
	retval = کام->jv_exitval;
	ch_log(تہی, "system command finished");

	کام_unref(کام);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    اگر (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_بفر(DOBUF_WIPE, DOBUF_پہلا, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    واپس retval;
}
#اگر_ختم

#اگر_تعریف_شدہ USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ساکن صحیح
mch_call_shell_system(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    حرف	*ifn = تہی;
    حرف	*ofn = تہی;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    صحیح		x;

    برآمد_flush();

    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# اگر defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  اگر defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    اگر (cmd == تہی)
	x = system((حرف *)p_sh);
    ورنہ
    {
# ifdef VMS
	اگر (ofn = strchr((حرف *)cmd, '>'))
	    *ofn++ = '\0';
	اگر (ifn = strchr((حرف *)cmd, '<'))
	{
	    حرف *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    اگر (p)
		*p = '\0';
	}
	اگر (ofn)
	    x = vms_sys((حرف *)cmd, ofn, ifn);
	ورنہ
	    x = system((حرف *)cmd);
# ورنہ
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == تہی ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	اگر (newcmd == تہی)
	    x = 0;
	ورنہ
	{
	    sprintf((حرف *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == تہی ? "" : (حرف *)extra_shell_arg,
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
    اگر (emsg_silent)
	;
    ورنہ اگر (x == 127)
	پیغام_puts(_("\nCannot execute shell sh\n"));
    ورنہ اگر (x && !(options & SHELL_SILENT))
    {
	پیغام_puts(_("\nshell returned "));
	پیغام_outnum((طویل)x);
	پیغام_putchar('\n');
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
    واپس x;
}

#ورنہ // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ساکن صحیح
mch_call_shell_fork(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    صحیح		حالت_2 = -1;
    صحیح		retval = -1;
    حرف	**argv = تہی;
    char_u	*tofree1 = تہی;
    char_u	*tofree2 = تہی;
    صحیح		i;
    صحیح		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    صحیح		pty_slave_fd = -1;
# endif
    صحیح		fd_toshell[2];		// for pipes
    صحیح		fd_fromshell[2];
    صحیح		pipe_error = FALSE;
    صحیح		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    برآمد_flush();
    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    اگر (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    اگر (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	جاؤ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.اندر_use && show_shell_mess)
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
	    open_pty(&pty_master_fd, &pty_slave_fd, تہی, تہی);
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
		پیغام_puts(_("\nCannot create pipes\n"));
		برآمد_flush();
	    }
	}
    }

    اگر (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	بلاک_اشارات(&curset);
	pid = fork();	// maybe we should use vfork()
	اگر (pid == -1)
	{
	    UNBLOCK_اشارات(&curset);

	    پیغام_puts(_("\nCannot fork\n"));
	    اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.اندر_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		اگر (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		ورنہ				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	ورنہ اگر (pid == 0)	// child
	{
	    reset_اشارات();		// handle signals normally
	    UNBLOCK_اشارات(&curset);

# ifdef FEAT_EVAL
	    اگر (ch_log_فعال())
	    {
		ch_log(تہی, "closing channel log in the child process");
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
	    ورنہ اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.اندر_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		اگر (p_stmp)
		{
		    (خالی)setsid();
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
			تیاری_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)تہی);
#  endif
		}
# endif
		سیٹ_default_بچہ_environment(FALSE);

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
		    اگر (gui.اندر_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		ورنہ
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
		    اگر (gui.اندر_use)
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
	ورنہ			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_اشارات(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_اشارات(&curset);
# ifdef FEAT_کام_CHANNEL
	    ++dont_check_کام_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    اگر ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.اندر_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    بفر[BUFLEN + 1];
		صحیح	    بفر_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		صحیح	    ta_len = 0;		// valid bytes in ta_buf[]
		صحیح	    len;
		صحیح	    p_مزید_save;
		صحیح	    old_State;
		صحیح	    c;
		صحیح	    toshell_fd;
		صحیح	    fromshell_fd;
		garray_T    ga;
		صحیح	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   آغاز_tv;
# endif

# ifdef FEAT_GUI
		اگر (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		ورنہ
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
		p_مزید_save = p_مزید;
		p_مزید = FALSE;
		old_State = State;
		State = منوال_EXTERNCMD;	// don't redraw at window resize

		اگر ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    اگر ((wpid = fork()) == -1)
		    {
			پیغام_puts(_("\nCannot fork\n"));
		    }
		    ورنہ اگر (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_آغاز.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			برائے (;;)
			{
			    اگر (lplen == 0)
				len = 0;
			    ورنہ اگر (lp[written] == NL)
				// NL -> NUL translation
				len = اكتب(toshell_fd, "", (size_t)1);
			    ورنہ
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = اكتب(toshell_fd, (حرف *)lp + written,
					   s == تہی ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    اگر (len == (صحیح)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				اگر (lnum != curbuf->b_op_انتہا.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_سطر_گنتی
						    || curbuf->b_p_eol)))
				    vim_ignored = اكتب(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				اگر (lnum > curbuf->b_op_انتہا.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    توڑو;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    ورنہ اگر (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    ورنہ // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		اگر (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(آغاز_tv);
# endif
		برائے (;;)
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
						    || gui.اندر_use
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
			  ELAPSED_INIT(آغاز_tv);
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
			اگر (len == 1 && (pty_master_fd < 0 || cmd != تہی))
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
			len = رکن_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			اگر (pty_master_fd < 0)
			{
			    برائے (i = ta_len; i < ta_len + len; ++i)
			    {
				اگر (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    پیغام_putchar(ta_buf[i]);
				ورنہ اگر (has_mbyte)
				{
				    صحیح l = (*mb_ptr2len)(ta_buf + i);

				    پیغام_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				ورنہ
				    پیغام_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
			    برآمد_flush();
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
			ورنہ اگر (toshell_fd >= 0)
			{
			    len = اكتب(toshell_fd, (حرف *)ta_buf, (size_t)1);
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
# ورنہ
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
		    جب (RealWaitForChar(fromshell_fd, 10L, تہی, تہی))
		    {
			len = read_eintr(fromshell_fd, بفر
				+ بفر_off, (size_t)(BUFLEN - بفر_off)
				);
			اگر (len <= 0)		    // end of file or error
			    جاؤ ختم_شدہ;

			noread_cnt = 0;
			اگر (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    برائے (i = 0; i < len; ++i)
			    {
				اگر (بفر[i] == NL)
				    append_ga_سطر(&ga);
				ورنہ اگر (بفر[i] == NUL)
				    ga_append(&ga, NL);
				ورنہ
				    ga_append(&ga, بفر[i]);
			    }
			}
			ورنہ اگر (has_mbyte)
			{
			    صحیح		l;
			    char_u	*p;

			    len += بفر_off;
			    بفر[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    برائے (p = بفر; p < بفر + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				اگر (l == 0)
				    l = 1;  // NUL byte?
				ورنہ اگر (MB_BYTE2LEN(*p) != l)
				    توڑو;
			    }
			    اگر (p == بفر)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				اگر (len >= 12)
				    ++p;
				ورنہ
				{
				    بفر_off = len;
				    جاری;
				}
			    }
			    c = *p;
			    *p = NUL;
			    پیغام_puts((حرف *)بفر);
			    اگر (p < بفر + len)
			    {
				*p = c;
				بفر_off = (بفر + len) - p;
				mch_memmove(بفر, p, بفر_off);
				جاری;
			    }
			    بفر_off = 0;
			}
			ورنہ
			{
			    بفر[len] = NUL;
			    پیغام_puts((حرف *)بفر);
			}

			windgoto(پیغام_صف, cmdline_ستون_off + پیغام_ستون);
			cursor_on();
			برآمد_flush();
			اگر (got_int)
			    توڑو;

# ifdef ELAPSED_FUNC
			اگر (wait_pid == 0)
			{
			    طویل	msec = ELAPSED_FUNC(آغاز_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    اگر (msec > 2000)
			    {
				noread_cnt = 5;
				توڑو;
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
			    جاری;
			توڑو;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &حالت_2, WNOHANG);
		    اگر ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(حالت_2)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    ورنہ
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
ختم_شدہ:
		p_مزید = p_مزید_save;
		اگر (options & SHELL_READ)
		{
		    اگر (ga.ga_len > 0)
		    {
			append_ga_سطر(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    ورنہ
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
	    ورنہ
	    {
		طویل delay_msec = 1;

		اگر (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    برآمد_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		برائے (;;)
		{
		    اگر (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  ورنہ
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &حالت_2, WNOHANG);
		    اگر ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(حالت_2)))
		    {
			wait_pid = pid;
			توڑو;
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
		    برآمد_str_t_TI();
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
		(خالی)wait4pid(pid, &حالت_2);

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
		wait4pid(wpid, تہی);
	    }

# ifdef FEAT_کام_CHANNEL
	    --dont_check_کام_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    اگر (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    سیٹ_اشارات();

	    اگر (WIFEXITED(حالت_2))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(حالت_2);
		اگر (retval != 0 && !emsg_silent)
		{
		    اگر (retval == EXEC_FAILED)
		    {
			پیغام_puts(_("\nCannot execute shell "));
			پیغام_outtrans(p_sh);
			پیغام_putchar('\n');
		    }
		    ورنہ اگر (!(options & SHELL_SILENT))
		    {
			پیغام_puts(_("\nshell returned "));
			پیغام_outnum((طویل)retval);
			پیغام_putchar('\n');
		    }
		}
	    }
	    ورنہ
		پیغام_puts(_("\nCommand terminated\n"));
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

    واپس retval;
}
#اگر_ختم // USE_SYSTEM

    صحیح
mch_call_shell(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
#اگر_تعریف_شدہ FEAT_EVAL
    ch_log(تہی, "executing shell command: %s", cmd);
#اگر_ختم
#اگر defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    اگر (gui.اندر_use && vim_strchr(p_go, GO_TERMINAL) != تہی
					      && (options & SHELL_SILENT) == 0)
	واپس mch_call_shell_terminal(cmd, options);
#اگر_ختم
#اگر_تعریف_شدہ USE_SYSTEM
    واپس mch_call_shell_system(cmd, options);
#ورنہ
    واپس mch_call_shell_fork(cmd, options);
#اگر_ختم
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
mch_get_cmd_output_براہ_راست(
    حرف	**argv,
    char_u	*infile,
    صحیح		flags غیر_مستعمل,
    صحیح		*ret_len)
{
    pid_t	pid;
    صحیح		fd_برآمد[2] = {-1, -1};
    صحیح		حالت_2 = -1;
    char_u	*بفر = تہی;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(تہی, "directly executing: %s", argv[0]);

    اگر (pipe(fd_برآمد) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	واپس تہی;
    }

    بلاک_اشارات(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# ورنہ
    pid = fork();
# endif
    اگر (pid == -1)
    {
	UNBLOCK_اشارات(&curset);
	close(fd_برآمد[0]);
	close(fd_برآمد[1]);
	emsg(_("\nCannot fork\n"));
	واپس تہی;
    }

    اگر (pid == 0)
    {
	// child process
	reset_اشارات();
	UNBLOCK_اشارات(&curset);

	اگر (ch_log_فعال())
	{
	    ch_log(تہی, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	اگر (infile != تہی)
	{
	    صحیح fd_اندر = open((حرف *)infile, O_RDONLY);
	    اگر (fd_اندر >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_اندر);
		close(fd_اندر);
	    }
	}
	ورنہ
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
	close(fd_برآمد[0]);
	close(1);
	vim_ignored = dup(fd_برآمد[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_برآمد[1]);
	close(fd_برآمد[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_اشارات(&curset);
    close(fd_برآمد[1]);

    // Read output from child.
    برائے (;;)
    {
	حرف	buf[4096];
	صحیح	n;

	n = (صحیح)اقرأ(fd_برآمد[0], buf, حجم(buf));
	اگر (n <= 0)
	    توڑو;
	اگر (ga_grow(&ga, n) == FAIL)
	    توڑو;
	mch_memmove((حرف *)ga.ga_ڈیٹا + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_برآمد[0]);

    // Wait for child to finish.
    (خالی)waitpid(pid, &حالت_2, 0);
    اگر (WIFEXITED(حالت_2))
	حالت_2 = WEXITSTATUS(حالت_2);
    ورنہ
	حالت_2 = -1;
    سیٹ_vim_var_nr(VV_SHELL_ERROR, (طویل)حالت_2);

    اگر (ga.ga_len > 0)
    {
	بفر = alloc(ga.ga_len + 1);
	اگر (بفر != تہی)
	{
	    mch_memmove(بفر, ga.ga_ڈیٹا, ga.ga_len);
	    اگر (ret_len == تہی)
	    {
		صحیح	i;

		// Change NUL into SOH, otherwise the string is truncated.
		برائے (i = 0; i < ga.ga_len; ++i)
		    اگر (بفر[i] == NUL)
			بفر[i] = 1;
		بفر[ga.ga_len] = NUL;
	    }
	    ورنہ
		*ret_len = ga.ga_len;
	}
    }
    ورنہ اگر (ret_len != تہی)
	*ret_len = 0;

    ga_clear(&ga);
    واپس بفر;
}
#اگر_ختم

#اگر defined(FEAT_کام_CHANNEL)
    خالی
mch_کام_آغاز(حرف **argv, کام_T *کام, jobopt_T *options, صحیح is_terminal)
{
    pid_t	pid;
    صحیح		fd_اندر[2] = {-1, -1};	// for stdin
    صحیح		fd_برآمد[2] = {-1, -1};	// for stdout
    صحیح		fd_err[2] = {-1, -1};	// for stderr
    صحیح		pty_master_fd = -1;
    صحیح		pty_slave_fd = -1;
    channel_T	*channel = تہی;
    صحیح		use_null_for_اندر = options->jo_io[PART_اندر] == JIO_NULL;
    صحیح		use_null_for_برآمد = options->jo_io[PART_برآمد] == JIO_NULL;
    صحیح		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    صحیح		use_file_for_اندر = options->jo_io[PART_اندر] == JIO_FILE;
    صحیح		use_file_for_برآمد = options->jo_io[PART_برآمد] == JIO_FILE;
    صحیح		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    صحیح		use_بفر_for_اندر = options->jo_io[PART_اندر] == JIO_بفر;
    صحیح		use_برآمد_for_err = options->jo_io[PART_ERR] == JIO_برآمد;
    SIGSET_DECL(curset)

    اگر (use_برآمد_for_err && use_null_for_برآمد)
	use_null_for_err = TRUE;

    // default is to fail
    کام->jv_حالت = کام_FAILED;

    اگر (options->jo_pty
	    && (!(use_file_for_اندر || use_null_for_اندر)
		|| !(use_file_for_برآمد || use_null_for_برآمد)
		|| !(use_برآمد_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &کام->jv_tty_برآمد, &کام->jv_tty_اندر);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    اگر (use_file_for_اندر)
    {
	char_u *fname = options->jo_io_name[PART_اندر];

	fd_اندر[0] = mch_open((حرف *)fname, O_RDONLY, 0);
	اگر (fd_اندر[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    ورنہ
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	اگر (!use_null_for_اندر && (pty_master_fd < 0 || use_بفر_for_اندر)
							    && pipe(fd_اندر) < 0)
	    جاؤ failed;

    اگر (use_file_for_برآمد)
    {
	char_u *fname = options->jo_io_name[PART_برآمد];

	fd_برآمد[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	اگر (fd_برآمد[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    ورنہ اگر (!use_null_for_برآمد && pty_master_fd < 0 && pipe(fd_برآمد) < 0)
	جاؤ failed;

    اگر (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((حرف *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	اگر (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    جاؤ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    ورنہ اگر (!use_برآمد_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_سیٹ & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	جاؤ failed;

    اگر (!use_null_for_اندر || !use_null_for_برآمد || !use_null_for_err)
    {
	اگر (options->jo_سیٹ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    اگر (channel != تہی)
		++channel->ch_refcount;
	}
	ورنہ
	    channel = جوڑو_channel();
	اگر (channel == تہی)
	    جاؤ failed;
	اگر (کام->jv_tty_برآمد != تہی)
	    ch_log(channel, "using pty %s on fd %d",
					       کام->jv_tty_برآمد, pty_master_fd);
    }

    بلاک_اشارات(&curset);
    pid = fork();	// maybe we should use vfork()
    اگر (pid == -1)
    {
	// failed to fork
	UNBLOCK_اشارات(&curset);
	جاؤ failed;
    }
    اگر (pid == 0)
    {
	صحیح	null_fd = -1;
	صحیح	stderr_چلتا = TRUE;

	// child
	reset_اشارات();		// handle signals normally
	UNBLOCK_اشارات(&curset);

# ifdef FEAT_EVAL
	اگر (ch_log_فعال())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(خالی)setsid();
# endif

# ifdef FEAT_TERMINAL
	اگر (options->jo_رکن_صفیں > 0)
	{
	    حرف *رکن = (حرف *)T_NAME;

#  ifdef FEAT_GUI
	    اگر (رکن_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		رکن = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    اگر (رکن == تہی || *رکن == NUL || STRNCMP(رکن, "xterm", 5) != 0)
	    {
		اگر (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    رکن = "xterm-256color";
		ورنہ اگر (t_colors > 16)
		    رکن = "xterm-color";
		ورنہ
		    رکن = "xterm";
	    }
	    سیٹ_بچہ_environment(
		    (طویل)options->jo_رکن_صفیں,
		    (طویل)options->jo_رکن_ستون_تعداد,
		    رکن,
		    is_terminal);
	}
	ورنہ
# endif
	    سیٹ_default_بچہ_environment(is_terminal);

	اگر (options->jo_env != تہی)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    صحیح		todo = (صحیح)dict->dv_hashtab.ht_مستعمل;

	    FOR_ALL_HASHTAB_اشیاء(&dict->dv_hashtab, hi, todo)
		اگر (!HASHITEM_خالی_خانے(hi))
		{
		    typval_T *شے = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_کلید, tv_get_string(شے));
		    --todo;
		}
	}

	اگر (use_null_for_اندر || use_null_for_برآمد || use_null_for_err)
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
	    تیاری_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (حرف *)تہی);
# endif
	}

	// set up stdin for the child
	close(0);
	اگر (use_null_for_اندر && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ورنہ اگر (fd_اندر[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ورنہ
	    vim_ignored = dup(fd_اندر[0]);

	// set up stderr for the child
	close(2);
	اگر (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_چلتا = FALSE;
	}
	ورنہ اگر (use_برآمد_for_err)
	    vim_ignored = dup(fd_برآمد[1]);
	ورنہ اگر (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ورنہ
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	اگر (use_null_for_برآمد && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ورنہ اگر (fd_برآمد[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ورنہ
	    vim_ignored = dup(fd_برآمد[1]);

	اگر (fd_اندر[0] >= 0)
	    close(fd_اندر[0]);
	اگر (fd_اندر[1] >= 0)
	    close(fd_اندر[1]);
	اگر (fd_برآمد[0] >= 0)
	    close(fd_برآمد[0]);
	اگر (fd_برآمد[1] >= 0)
	    close(fd_برآمد[1]);
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

	اگر (options->jo_cwd != تہی && mch_chdir((حرف *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	اگر (stderr_چلتا)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_اشارات(&curset);

    کام->jv_pid = pid;
    کام->jv_حالت = کام_STARTED;
    کام->jv_channel = channel;  // ch_refcount was set above

    اگر (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    اگر (fd_اندر[0] >= 0)
	close(fd_اندر[0]);
    اگر (fd_برآمد[1] >= 0)
	close(fd_برآمد[1]);
    اگر (fd_err[1] >= 0)
	close(fd_err[1]);
    اگر (channel != تہی)
    {
	صحیح اندر_fd = INVALID_FD;
	صحیح برآمد_fd = INVALID_FD;
	صحیح err_fd = INVALID_FD;

	اگر (!(use_file_for_اندر || use_null_for_اندر))
	    اندر_fd = fd_اندر[1] >= 0 ? fd_اندر[1] : pty_master_fd;

	اگر (!(use_file_for_برآمد || use_null_for_برآمد))
	    برآمد_fd = fd_برآمد[0] >= 0 ? fd_برآمد[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	اگر (!(use_برآمد_for_err || use_file_for_err || use_null_for_err))
	{
	    اگر (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    ورنہ اگر (برآمد_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_سیٹ_pipes(channel, اندر_fd, برآمد_fd, err_fd);
	channel_سیٹ_کام(channel, کام, options);
    }
    ورنہ
    {
	اگر (fd_اندر[1] >= 0)
	    close(fd_اندر[1]);
	اگر (fd_برآمد[0] >= 0)
	    close(fd_برآمد[0]);
	اگر (fd_err[0] >= 0)
	    close(fd_err[0]);
	اگر (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    واپس;

failed:
    channel_unref(channel);
    اگر (fd_اندر[0] >= 0)
	close(fd_اندر[0]);
    اگر (fd_اندر[1] >= 0)
	close(fd_اندر[1]);
    اگر (fd_برآمد[0] >= 0)
	close(fd_برآمد[0]);
    اگر (fd_برآمد[1] >= 0)
	close(fd_برآمد[1]);
    اگر (fd_err[0] >= 0)
	close(fd_err[0]);
    اگر (fd_err[1] >= 0)
	close(fd_err[1]);
    اگر (pty_master_fd >= 0)
	close(pty_master_fd);
    اگر (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ساکن char_u *
get_signal_name(صحیح sig)
{
    صحیح		i;
    char_u	numbuf[NUMBUFLEN];

    اگر (sig == SIGKILL)
	واپس vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    برائے (i = 0; signal_info[i].sig != -1; i++)
	اگر (sig == signal_info[i].sig)
	    واپس strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((حرف *)numbuf, NUMBUFLEN, "%d", sig);
    واپس vim_strnsave(numbuf, i);
}

    حرف *
mch_کام_حالت(کام_T *کام)
{
    صحیح		حالت_2 = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(کام->jv_pid, &حالت_2, WNOHANG);
    اگر (wait_pid == -1)
    {
	صحیح waitpid_errno = errno;
	اگر (waitpid_errno == ECHILD && mch_process_running(کام->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    واپس "run";

	// process must have exited
	اگر (کام->jv_حالت < کام_ENDED)
	    ch_log(کام->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	جاؤ return_dead;
    }
    اگر (wait_pid == 0)
	واپس "run";
    اگر (WIFEXITED(حالت_2))
    {
	// LINTED avoid "bitwise operation on signed value"
	کام->jv_exitval = WEXITSTATUS(حالت_2);
	اگر (کام->jv_حالت < کام_ENDED)
	    ch_log(کام->jv_channel, "Job exited with %d", کام->jv_exitval);
	جاؤ return_dead;
    }
    اگر (WIFSIGNALED(حالت_2))
    {
	کام->jv_exitval = -1;
	کام->jv_termsig = get_signal_name(WTERMSIG(حالت_2));
	اگر (کام->jv_حالت < کام_ENDED && کام->jv_termsig != تہی)
	    ch_log(کام->jv_channel, "Job terminated by signal \"%s\"",
							      کام->jv_termsig);
	جاؤ return_dead;
    }
    واپس "run";

return_dead:
    اگر (کام->jv_حالت < کام_ENDED)
	کام->jv_حالت = کام_ENDED;
    واپس "dead";
}

    کام_T *
mch_detect_ended_کام(کام_T *کام_list)
{
    صحیح		حالت_2 = -1;
    pid_t	wait_pid = 0;
    کام_T	*کام;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    اگر (dont_check_کام_ended > 0)
	واپس تہی;
# endif

    wait_pid = waitpid(-1, &حالت_2, WNOHANG);
    اگر (wait_pid <= 0)
	// no process ended
	واپس تہی;
    برائے (کام = کام_list; کام != تہی; کام = کام->jv_اگلا)
    {
	اگر (کام->jv_pid == wait_pid)
	{
	    اگر (WIFEXITED(حالت_2))
		// LINTED avoid "bitwise operation on signed value"
		کام->jv_exitval = WEXITSTATUS(حالت_2);
	    ورنہ اگر (WIFSIGNALED(حالت_2))
	    {
		کام->jv_exitval = -1;
		کام->jv_termsig = get_signal_name(WTERMSIG(حالت_2));
	    }
	    اگر (کام->jv_حالت < کام_ENDED)
	    {
		ch_log(کام->jv_channel, "Job ended");
		کام->jv_حالت = کام_ENDED;
	    }
	    واپس کام;
	}
    }
    واپس تہی;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    صحیح
mch_signal_کام(کام_T *کام, char_u *how)
{
    صحیح	    sig = -1;

    اگر (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    ورنہ اگر (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    ورنہ اگر (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    ورنہ اگر (STRCMP(how, "int") == 0)
	sig = SIGINT;
    ورنہ اگر (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    ورنہ اگر (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    ورنہ اگر (محفوظ_isdigit(*how))
	sig = atoi((حرف *)how);
    ورنہ
	واپس FAIL;

    // Never kill ourselves!
    اگر (کام->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-کام->jv_pid, sig);
	kill(کام->jv_pid, sig);
    }

    واپس OK;
}

/*
 * Clear the data related to "job".
 */
    خالی
mch_clear_کام(کام_T *کام)
{
    // call waitpid because child process may become zombie
    (خالی)waitpid(کام->jv_pid, تہی, WNOHANG);
}
#اگر_ختم

#اگر defined(FEAT_TERMINAL)
    صحیح
mch_create_pty_channel(کام_T *کام, jobopt_T *options)
{
    صحیح		pty_master_fd = -1;
    صحیح		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &کام->jv_tty_برآمد, &کام->jv_tty_اندر);
    اگر (pty_master_fd < 0 || pty_slave_fd < 0)
	واپس FAIL;
    close(pty_slave_fd);

    channel = جوڑو_channel();
    اگر (channel == تہی)
    {
	close(pty_master_fd);
	واپس FAIL;
    }
    اگر (کام->jv_tty_برآمد != تہی)
	ch_log(channel, "using pty %s on fd %d",
					       کام->jv_tty_برآمد, pty_master_fd);
    کام->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_سیٹ_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_سیٹ_کام(channel, کام, options);
    واپس OK;
}
#اگر_ختم

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    خالی
mch_breakcheck(صحیح force)
{
    اگر ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, تہی, تہی))
	fill_داخلہ_buf(FALSE);
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
    ساکن صحیح
WaitForChar(طویل msec, صحیح *interrupted, صحیح ignore_داخلہ)
{
#اگر_تعریف_شدہ FEAT_TIMERS
    واپس ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_داخلہ) == OK;
#ورنہ
    واپس WaitForCharOrMouse(msec, interrupted, ignore_داخلہ);
#اگر_ختم
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
    ساکن صحیح
WaitForCharOrMouse(طویل msec, صحیح *interrupted, صحیح ignore_داخلہ)
{
#اگر_تعریف_شدہ FEAT_MOUSE_GPM
    صحیح		gpm_process_wanted;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_XCLIPBOARD
    صحیح		rest;
#اگر_ختم
    صحیح		avail;

    اگر (!ignore_داخلہ && داخلہ_دستیاب())	    // something in inbuf[]
	واپس 1;

#اگر defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    اگر (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	اگر (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#اگر_ختم

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
    کرو
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
	اگر (has_sound_callback_اندر_قطار())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	اگر (!avail && !gpm_process_wanted)
# ورنہ
	avail = RealWaitForChar(read_cmd_fd, msec, تہی, interrupted);
	اگر (!avail)
# endif
	{
	    اگر (!ignore_داخلہ && داخلہ_دستیاب())
		واپس 1;
# ifdef FEAT_XCLIPBOARD
	    اگر (rest == 0 || !do_xterm_trace())
# endif
		توڑو;
	}
    }
    جب (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#ورنہ
    avail = RealWaitForChar(read_cmd_fd, msec, تہی, interrupted);
#اگر_ختم
    واپس avail;
}

#اگر_غیر_تعریف_شدہ VMS
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
    ساکن صحیح
RealWaitForChar(صحیح fd, طویل msec, صحیح *check_for_gpm غیر_مستعمل, صحیح *interrupted)
{
    صحیح		ret;
    صحیح		نتیجہ;
# اگر defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ساکن صحیح	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    طویل	آغاز_msec = msec;
    elapsed_T	آغاز_tv;

    اگر (msec > 0)
	ELAPSED_INIT(آغاز_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    اگر (busy)
	واپس 0;
# endif

# ifdef MAY_LOOP
    برائے (;;)
# endif
    {
# ifdef MAY_LOOP
	صحیح		ختم_شدہ = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	صحیح		mzquantum_مستعمل = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	صحیح		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ساخت pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
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
	mzvim_check_دھاگے();
	اگر (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (صحیح)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_مستعمل = TRUE;
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
	    fds[nfd].fd = Connection_نمبر(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (check_for_gpm != تہی && gpm_flag && gpm_fd >= 0)
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
#  ifdef FEAT_کام_CHANNEL
	nfd = channel_poll_تیاری(nfd, &fds, &towait);
#  endif
	اگر (interrupted != تہی)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	نتیجہ = ret > 0 && (fds[0].revents & POLLIN);
	اگر (نتیجہ == 0 && interrupted != تہی && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	اگر (ret == 0 && mzquantum_مستعمل)
	    // MzThreads scheduling is required and timeout occurred
	    ختم_شدہ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	اگر (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	اگر (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    اگر (--ret == 0 && !داخلہ_دستیاب())
		// Try again
		ختم_شدہ = FALSE;
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
		xsmp_handle_درخواستیں();
		busy = FALSE;
	    }
	    ورنہ اگر (fds[xsmp_idx].revents & POLLHUP)
	    {
		اگر (p_verbose > 0)
		    verb_پیغام(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    اگر (--ret == 0)
		ختم_شدہ = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_کام_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	اگر (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# ورنہ // HAVE_SELECT

	ساخت timeval  tv;
	ساخت timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ساکن fd_set	rfds, wfds, efds;
	صحیح		maxfd;
	طویل		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_دھاگے();
	اگر (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_مستعمل = TRUE;
	}
#  endif

	اگر (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	ورنہ
	    tvp = تہی;

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
	    FD_SET(Connection_نمبر(xterm_dpy), &rfds);
	    اگر (maxfd < Connection_نمبر(xterm_dpy))
		maxfd = Connection_نمبر(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (check_for_gpm != تہی && gpm_flag && gpm_fd >= 0)
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
#  ifdef FEAT_کام_CHANNEL
	maxfd = channel_select_تیاری(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	اگر (interrupted != تہی)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	نتیجہ = ret > 0 && FD_ISSET(fd, &rfds);
	اگر (نتیجہ)
	    --ret;
	ورنہ اگر (interrupted != تہی && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	اگر (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    اگر (got_tstp && !اندر_mch_suspend)
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
		ch_log(تہی, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    جاؤ select_eintr;
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
	اگر (ret == 0 && mzquantum_مستعمل)
	    // loop if MzThreads must be scheduled and timeout occurred
	    ختم_شدہ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	اگر (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	اگر (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_نمبر(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    اگر (--ret == 0 && !داخلہ_دستیاب())
	    {
		// Try again
		ختم_شدہ = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	اگر (ret > 0 && check_for_gpm != تہی && gpm_flag && gpm_fd >= 0)
	{
	    اگر (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    ورنہ اگر (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	اگر (ret > 0 && xsmp_icefd != -1)
	{
	    اگر (FD_ISSET(xsmp_icefd, &efds))
	    {
		اگر (p_verbose > 0)
		    verb_پیغام(_("XSMP lost ICE connection"));
		xsmp_close();
		اگر (--ret == 0)
		    ختم_شدہ = FALSE;   // keep going if event was only one
	    }
	    ورنہ اگر (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_درخواستیں();
		busy = FALSE;
		اگر (--ret == 0)
		    ختم_شدہ = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_کام_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	اگر (ret >= 0)
	    (خالی)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	اگر (ختم_شدہ || msec == 0)
	    توڑو;

#  اگر defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	اگر (clientserver_طریقہ == CLIENTSERVER_طریقہ_X11 && server_انتظار())
	    توڑو;
#  endif

	// We're going to loop around again, find out for how long
	اگر (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = آغاز_msec - ELAPSED_FUNC(آغاز_tv);
#  ورنہ
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    اگر (msec <= 0)
		توڑو;	// waited long enough
	}
# endif
    }

    واپس نتیجہ;
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
    char_u	*راستہ,
    صحیح		flags)		// EW_* flags
{
    واپس unix_expandpath(gap, راستہ, 0, flags, FALSE);
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
    صحیح		   عدد_pat,
    char_u	 **pat,
    صحیح		  *عدد_file,
    char_u	***file,
    صحیح		   flags)	// EW_* flags
{
    صحیح		i;
    size_t	len;
    طویل	llen;
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
    char_u	*بفر;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    صحیح		shell_style = STYLE_ECHO;
    صحیح		check_spaces;
    ساکن صحیح	did_ڈھونڈو_nul = FALSE;
    صحیح		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ساکن string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ساکن string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *عدد_file = 0;	// default: no files found
    *file = تہی;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    اگر (!have_wildcard(عدد_pat, pat))
	واپس save_patterns(عدد_pat, pat, عدد_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    اگر (sandbox != 0 && check_secure())
	واپس FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    اگر (secure || restricted)
	برائے (i = 0; i < عدد_pat; ++i)
	    اگر (vim_strchr(pat[i], '`') != تہی
		    && (check_restricted() || check_secure()))
		واپس FAIL;

    /*
     * get a name for the temp file
     */
    اگر ((tempname = vim_tempname('o', FALSE)) == تہی)
    {
	emsg(_(e_cant_get_عارضی_file_name));
	واپس FAIL;
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
    اگر (عدد_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    ورنہ اگر ((len = STRLEN(p_sh)) >= 3)
    {
	اگر (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	ورنہ اگر (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    اگر (shell_style == STYLE_ECHO)
    {
	اگر (لڑی_تلاش((حرف *)gettail(p_sh), "bash") != تہی)
	    shell_style = STYLE_GLOBSTAR;
	ورنہ اگر (لڑی_تلاش((حرف *)gettail(p_sh), "sh") != تہی)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    اگر (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.لمبائی;
    ورنہ اگر (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.لمبائی + sh_globstar_opt.لمبائی;

    برائے (i = 0; i < عدد_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# ورنہ
	++len;				// add space
	برائے (j = 0; pat[i][j] != NUL; ++j)
	{
	    اگر (vim_strchr(SHELL_SPECIAL, pat[i][j]) != تہی)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    اگر (command == تہی)
    {
	// out of memory
	vim_free(tempname);
	واپس FAIL;
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
	commandlen = vim_snprintf((حرف *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)لڑی_تلاش((حرف *)command, ")>");
	اگر (p == تہی)
	{
	    ;					    // can't happen ;-)
	}
	ورنہ
	{
	    --p;
	    جب (p > command && VIM_ISWHITE(*p))
		--p;
	    اگر (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    جب (p > command && VIM_ISWHITE(*p))
		--p;
	    اگر (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    ورنہ
    {
	اگر (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((حرف *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	ورنہ اگر (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((حرف *)command, len, "print -N >%s", tempname);
	ورنہ اگر (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	ورنہ اگر (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((حرف *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	ورنہ
	    commandlen = vim_snprintf((حرف *)command, len, "echo >%s", tempname);

	برائے (i = 0; i < عدد_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((حرف *)command + commandlen, len, " \"%s\"", pat[i]);
# ورنہ
	    صحیح intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    برائے (j = 0; pat[i][j] != NUL; ++j)
	    {
		اگر (pat[i][j] == '`')
		    intick = !intick;
		ورنہ اگر (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    اگر (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != تہی
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		ورنہ اگر (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != تہی)
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
    ورنہ اگر (shell_style == STYLE_GLOB && !have_dollars(عدد_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    اگر (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = تہی;		// cleanup
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
	    پیغام_putchar('\n');		// clear bottom line quickly
	    cmdline_صف = صفیں - 1;	// continue on last line
# ifdef USE_SYSTEM
	    اگر (!(flags & EW_SILENT))
# endif
	    {
		پیغام(_(e_cannot_expand_wildcards));
		پیغام_آغاز();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	اگر (shell_style == STYLE_BT)
	    واپس FAIL;
	جاؤ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((حرف *)tempname, READBIN);
    اگر (fd == تہی)
    {
	// Something went wrong, perhaps a file name with a special char.
	اگر (!(flags & EW_SILENT))
	{
	    پیغام(_(e_cannot_expand_wildcards));
	    پیغام_آغاز();		// don't overwrite this message
	}
	vim_free(tempname);
	جاؤ notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    اگر (llen < 0)
	// just in case ftell() would fail
	بفر = تہی;
    ورنہ
	بفر = alloc(llen + 1);
    اگر (بفر == تہی)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	واپس FAIL;
    }
    len = llen;
    i = fread((حرف *)بفر, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    اگر (i != (صحیح)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(بفر);
	واپس FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = بفر;
    برائے (i = 0; i < (صحیح)len; ++i)
	اگر (!(بفر[i] == CAR && بفر[i + 1] == NL))
	    *p++ = بفر[i];
    len = p - بفر;
# endif


    // file names are separated with Space
    اگر (shell_style == STYLE_ECHO)
    {
	بفر[len] = '\n';		// make sure the buffer ends in NL
	p = بفر;
	برائے (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    جب (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    ورنہ اگر (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	بفر[len] = NUL;		// make sure the buffer ends in NUL
	p = بفر;
	برائے (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    جب (*p != '\n' && *p != NUL)
		++p;
	    اگر (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    ورنہ
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
	اگر (shell_style == STYLE_PRINT && !did_ڈھونڈو_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    بفر[len] = NUL;
	    اگر (len && (صحیح)STRLEN(بفر) < (صحیح)len)
		did_ڈھونڈو_nul = TRUE;
	    ورنہ
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	اگر (len && بفر[len - 1] == NUL)
	    --len;
	ورنہ
	    بفر[len] = NUL;
	i = 0;
	برائے (p = بفر; p < بفر + len; ++p)
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
	vim_free(بفر);
	جاؤ notfound;
    }
    *عدد_file = i;
    *file = ALLOC_MULT(char_u *, i);
    اگر (*file == تہی)
    {
	// out of memory
	vim_free(بفر);
	واپس FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = بفر;
    برائے (i = 0; i < *عدد_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	اگر (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    جب (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    اگر (p == بفر + len)		// last entry
		*p = NUL;
	    ورنہ
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	ورنہ		// NUL separates
	{
	    جب (*p && p < بفر + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    برائے (j = 0, i = 0; i < *عدد_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	اگر (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    جاری;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	اگر ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    جاری;

	// Skip files that are not executable if we check for that.
	اگر (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], تہی, !(flags & EW_SHELLCMD)))
	    جاری;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	اگر (p)
	{
	    STRCPY(p, (*file)[i]);
	    اگر (dir)
		جوڑو_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(بفر);
    *عدد_file = j;

    اگر (*عدد_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	جاؤ notfound;
    }

    واپس OK;

notfound:
    اگر (flags & EW_NOTFOUND)
	واپس save_patterns(عدد_pat, pat, عدد_file, file);
    واپس FAIL;
}

#اگر_ختم // VMS

    ساکن صحیح
save_patterns(
    صحیح		عدد_pat,
    char_u	**pat,
    صحیح		*عدد_file,
    char_u	***file)
{
    صحیح		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, عدد_pat);
    اگر (*file == تہی)
	واپس FAIL;
    برائے (i = 0; i < عدد_pat; i++)
    {
	s = vim_strsave(pat[i]);
	اگر (s != تہی)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *عدد_file = عدد_pat;
    واپس OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	ورنہ
	    اگر (vim_strchr((char_u *)
#اگر_تعریف_شدہ VMS
				    "*?%"
#ورنہ
				    "*?[{'"
#اگر_ختم
						, *p) != تہی)
	    واپس TRUE;
    }
    واپس FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    صحیح
mch_has_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	ورنہ
	    اگر (vim_strchr((char_u *)
#اگر_تعریف_شدہ VMS
				    "*?%$"
#ورنہ
				    "*?[{`'$"
#اگر_ختم
						, *p) != تہی
		|| (*p == '~' && p[1] != NUL))
	    واپس TRUE;
    }
    واپس FALSE;
}

    ساکن صحیح
have_wildcard(صحیح عدد, char_u **file)
{
    صحیح	    i;

    برائے (i = 0; i < عدد; i++)
	اگر (mch_has_wildcard(file[i]))
	    واپس 1;
    واپس 0;
}

    ساکن صحیح
have_dollars(صحیح عدد, char_u **file)
{
    صحیح	    i;

    برائے (i = 0; i < عدد; i++)
	اگر (vim_strchr(file[i], '$') != تہی)
	    واپس TRUE;
    واپس FALSE;
}

#اگر !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    صحیح
mch_rename(ثابت حرف *src, ثابت حرف *dest)
{
    ساخت stat	    st;

    اگر (stat(dest, &st) >= 0)	    // fail if destination exists
	واپس -1;
    اگر (link(src, dest) != 0)	    // link file to new name
	واپس -1;
    اگر (mch_remove(src) == 0)	    // delete link to old name
	واپس 0;
    واپس -1;
}
#اگر_ختم // !HAVE_RENAME

#اگر defined(FEAT_MOUSE_GPM)
# اگر defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ساکن صحیح
بھرو_libgpm(خالی)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    اگر (libgpm_hinst == تہی)
    {
	اگر (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	واپس FAIL;
    }

    اگر (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == تہی
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == تہی
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == تہی
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == تہی
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == تہی
      )
    {
	semsg(_(e_could_not_بھرو_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = تہی;
	dll_gpm_flag = تہی;
	dll_gpm_fd   = تہی;
	واپس FAIL;
    }
    واپس OK;
}

    صحیح
gpm_دستیاب(خالی)
{
    واپس libgpm_hinst != تہی || بھرو_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ساکن صحیح
gpm_open(خالی)
{
    ساکن Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    اگر (!gpm_دستیاب())
	واپس 0;
# endif

    اگر (gpm_flag)
	واپس 1; // already open

    gpm_connect.event_نقاب_2 = (GPM_اوپر | GPM_DRAG | GPM_نیچے_کو);
    gpm_connect.default_نقاب = ~GPM_HARD;
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
	واپس 1; // succeed
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
    واپس 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    صحیح
gpm_enabled(خالی)
{
    واپس gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ساکن خالی
gpm_close(خالی)
{
    اگر (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ساکن صحیح
mch_gpm_process(خالی)
{
    صحیح			button;
    ساکن Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    صحیح			صف,ستون;
    غیرعلامتی حرف	buttons_نقاب;
    غیرعلامتی حرف	gpm_modifiers;
    ساکن غیرعلامتی حرف old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس 0;
# endif

    صف = gpm_event.y - 1;
    ستون = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    انتخاب (GPM_BARE_EVENTS(gpm_event.type))
    {
	حالت GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    توڑو;
	حالت GPM_نیچے_کو:
	    buttons_نقاب = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    انتخاب (buttons_نقاب)
	    {
		حالت GPM_B_بایاں:
		    button = MOUSE_بایاں;
		    توڑو;
		حالت GPM_B_درمیان:
		    button = MOUSE_درمیان;
		    توڑو;
		حالت GPM_B_دایاں:
		    button = MOUSE_دایاں;
		    توڑو;
		طےشدہ:
		    واپس 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    سیٹ_عدد_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    توڑو;
	حالت GPM_اوپر:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    توڑو;
	طےشدہ:
	    واپس 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    اگر (gpm_modifiers & ((1 << KG_سرکاؤ) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_سرکاؤ;

    اگر (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    اگر (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(ستون + ' ' + 1);
    string[5] = (char_u)(صف + ' ' + 1);
    جوڑو_مقصد_داخلہ_buf(string, 6);
    واپس 6;
}
#اگر_ختم // FEAT_MOUSE_GPM

#اگر_تعریف_شدہ FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ساکن صحیح
sysmouse_open(خالی)
{
    ساخت mouse_info   mouse;

    mouse.operation = MOUSE_منوال;
    mouse.u.منوال.منوال = 0;
    mouse.u.منوال.اشارہ = SIGUSR2;
    اگر (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	واپس FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    واپس OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ساکن خالی
sysmouse_close(خالی)
{
    ساخت mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_منوال;
    mouse.u.منوال.منوال = 0;
    mouse.u.منوال.اشارہ = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ساکن خالی
sig_sysmouse SIGDEFARG(sigarg)
{
    ساخت mouse_info	mouse;
    ساخت video_info	video;
    char_u		string[6];
    صحیح			صف, ستون;
    صحیح			button;
    صحیح			buttons;
    ساکن صحیح		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    اگر (hold_gui_events)
	واپس;
# endif

    mouse.operation = MOUSE_GETINFO;
    اگر (ioctl(1, FBIO_GETMODE, &video.vi_منوال) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	صف = mouse.u.ڈیٹا.y / video.vi_cheight;
	ستون = mouse.u.ڈیٹا.x / video.vi_cwidth;
	buttons = mouse.u.ڈیٹا.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	اگر (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	ورنہ
	{
	    انتخاب (buttons)
	    {
		حالت 0:
		    button = MOUSE_RELEASE;
		    توڑو;
		حالت 1:
		    button = MOUSE_بایاں;
		    توڑو;
		حالت 2:
		    button = MOUSE_درمیان;
		    توڑو;
		حالت 4:
		    button = MOUSE_دایاں;
		    توڑو;
		طےشدہ:
		    واپس;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(ستون + ' ' + 1);
	string[5] = (char_u)(صف + ' ' + 1);
	جوڑو_مقصد_داخلہ_buf(string, 6);
    }
    واپس;
}
#اگر_ختم // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ساکن صحیح dev_urandom_state = NOTDONE;

    اگر (dev_urandom_state == FAIL)
	واپس FAIL;

    صحیح fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    اگر (fd == -1)
	dev_urandom_state = FAIL;
    ورنہ اگر (اقرأ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    ورنہ
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    واپس dev_urandom_state;
}

#اگر defined(FEAT_LIBCALL)
قسمتعریف char_u * (*STRPROCSTR)(char_u *);
قسمتعریف char_u * (*INTPROCSTR)(صحیح);
قسمتعریف صحیح (*STRPROCINT)(char_u *);
قسمتعریف صحیح (*INTPROCINT)(صحیح);

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
    char_u	**string_نتیجہ, // NULL when using number_result
    صحیح		*نمبر_نتیجہ)
{
# اگر defined(USE_DLOPEN)
    خالی	*hinstLib;
    حرف	*dlerr = تہی;
# ورنہ
    shl_t	hinstLib;
# endif
    STRPROCSTR	Proc_جوڑو;
    INTPROCSTR	Proc_جوڑو_I;
    char_u	*retval_str = تہی;
    صحیح		retval_int = 0;
    صحیح		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# اگر defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (خالی)dlerror();

    hinstLib = dlopen((حرف *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    اگر (hinstLib == تہی)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	اگر (dlerr != تہی)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# ورنہ
    hinstLib = shl_بھرو((ثابت حرف*)libname, BIND_فوری|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    اگر (hinstLib != تہی)
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
	    dlerr = تہی;
#  endif
	    mch_didjmp();
	}
	ورنہ
# endif
	{
	    retval_str = تہی;
	    retval_int = 0;

	    اگر (argstring != تہی)
	    {
# اگر defined(USE_DLOPEN)
		*(خالی **)(&Proc_جوڑو) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# ورنہ
		اگر (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
					TYPE_PROCEDURE, (خالی *)&Proc_جوڑو) < 0)
		    Proc_جوڑو = تہی;
# endif
		اگر ((success = (Proc_جوڑو != تہی
# اگر defined(USE_DLOPEN)
			    && dlerr == تہی
# endif
			    )))
		{
		    اگر (string_نتیجہ == تہی)
			retval_int = ((STRPROCINT)(خالی *)Proc_جوڑو)(argstring);
		    ورنہ
			retval_str = (Proc_جوڑو)(argstring);
		}
	    }
	    ورنہ
	    {
# اگر defined(USE_DLOPEN)
		*(خالی **)(&Proc_جوڑو_I) = dlsym(hinstLib, (ثابت حرف *)funcname);
		dlerr = dlerror();
# ورنہ
		اگر (shl_findsym(&hinstLib, (ثابت حرف *)funcname,
				       TYPE_PROCEDURE, (خالی *)&Proc_جوڑو_I) < 0)
		    Proc_جوڑو_I = تہی;
# endif
		اگر ((success = (Proc_جوڑو_I != تہی
# اگر defined(USE_DLOPEN)
			    && dlerr == تہی
# endif
			    )))
		{
		    اگر (string_نتیجہ == تہی)
			retval_int = ((INTPROCINT)(خالی *)Proc_جوڑو_I)(argint);
		    ورنہ
			retval_str = (Proc_جوڑو_I)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    اگر (string_نتیجہ == تہی)
		*نمبر_نتیجہ = retval_int;
	    ورنہ اگر (retval_str != تہی
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_نتیجہ = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	اگر (lc_signal != 0)
	{
	    صحیح i;

	    // try to find the name of this signal
	    برائے (i = 0; signal_info[i].sig != -1; i++)
		اگر (lc_signal == signal_info[i].sig)
		    توڑو;
	    semsg(_(e_got_sig_str_اندر_libcall), signal_info[i].name);
	}
#  endif
# endif

# اگر defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	اگر (dlerr != تہی)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(خالی)dlclose(hinstLib);
# ورنہ
	(خالی)shl_unload(hinstLib);
# endif
    }

    اگر (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	واپس FAIL;
    }

    واپس OK;
}
#اگر_ختم

#اگر defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ساکن صحیح	xterm_trace = -1;	// default: disabled
ساکن صحیح	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    خالی
تیاری_رکن_clip(خالی)
{
    صحیح		z = 0;
    حرف	*strp = "";
    Widget	AppShell;

    اگر (!x_connect_مقصد_server())
	واپس;

    open_app_context();
    اگر (app_context != تہی && xterm_Shell == (Widget)0)
    {
	صحیح (*oldhandler)(Display*, XErrorEvent*);
# اگر defined(USING_SETJMP)
	صحیح (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T آغاز_tv;

	اگر (p_verbose > 0)
	    ELAPSED_INIT(آغاز_tv);
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
	    xterm_dpy = تہی;
	}
	ورنہ
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", تہی, 0, &z, &strp);
	    اگر (xterm_dpy != تہی)
		xterm_dpy_retry_گنتی = 0;
# اگر defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# اگر defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(خالی)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(خالی)XSetErrorHandler(oldhandler);

	اگر (xterm_dpy == تہی)
	{
	    اگر (p_verbose > 0)
		verb_پیغام(_("Opening the X display failed"));
	    واپس;
	}

	// Catch terminating error of the X server connection.
	(خالی)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	اگر (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_پیغام(ELAPSED_FUNC(آغاز_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		تہی);
	اگر (AppShell == (Widget)0)
	    واپس;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		چوٹی_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		تہی);
	اگر (xterm_Shell == (Widget)0)
	    واپس;

	x11_تیاری_atoms(xterm_dpy);
	x11_تیاری_selection(xterm_Shell);
	اگر (x11_display == تہی)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    اگر (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	اگر (x11_کھڑکی == 0 && (strp = getenv("WINDOWID")) != تہی)
	    x11_کھڑکی = (کھڑکی)atol(strp);
	// Check if $WINDOWID is valid.
	اگر (test_x11_کھڑکی(xterm_dpy) == FAIL)
	    x11_کھڑکی = 0;
	اگر (x11_کھڑکی != 0)
	    xterm_trace = 0;
    }
}

    خالی
آغاز_xterm_trace(صحیح button)
{
    اگر (x11_کھڑکی == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	واپس;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    خالی
stop_xterm_trace(خالی)
{
    اگر (xterm_trace < 0)
	واپس;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ساکن صحیح
do_xterm_trace(خالی)
{
    کھڑکی		جڑ, بچہ;
    صحیح			جڑ_x, جڑ_y;
    صحیح			win_x, win_y;
    صحیح			صف, ستون;
    int_u		نقاب_return;
    char_u		buf[50];
    char_u		*strp;
    طویل		got_hints;
    ساکن char_u	*mouse_رمز = تہی;
    ساکن size_t	mouse_codelen = 0;
    ساکن char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ساکن صحیح		پچھلا_صف = 0, پچھلا_ستون = 0;
    ساکن XSizeHints	xterm_hints;

    اگر (xterm_trace <= 0)
	واپس FALSE;

    اگر (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	اگر (!XGetWMNormalHints(xterm_dpy, x11_کھڑکی, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.چوڑائی_inc <= 1
		|| xterm_hints.اونچائی_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    واپس FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_رمز = ڈھونڈو_termcode(mouse_name);
	اگر (mouse_رمز != تہی)
	    mouse_codelen = STRLEN(mouse_رمز);
	پچھلا_صف = mouse_صف;
	پچھلا_ستون = mouse_ستون;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQuery_اشارہ_گر(xterm_dpy, x11_کھڑکی, &جڑ, &بچہ, &جڑ_x, &جڑ_y,
		      &win_x, &win_y, &نقاب_return);
	xterm_hints.y = win_y - (xterm_hints.اونچائی_inc * mouse_صف)
			      - (xterm_hints.اونچائی_inc / 2);
	اگر (xterm_hints.y <= xterm_hints.اونچائی_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.چوڑائی_inc * mouse_ستون)
			      - (xterm_hints.چوڑائی_inc / 2);
	اگر (xterm_hints.x <= xterm_hints.چوڑائی_inc / 2)
	    xterm_hints.x = 2;
	واپس TRUE;
    }

    اگر (mouse_رمز == تہی || mouse_codelen > 45)
    {
	xterm_trace = 0;
	واپس FALSE;
    }

    XQuery_اشارہ_گر(xterm_dpy, x11_کھڑکی, &جڑ, &بچہ, &جڑ_x, &جڑ_y,
		  &win_x, &win_y, &نقاب_return);

    صف = check_صف((win_y - xterm_hints.y) / xterm_hints.اونچائی_inc);
    ستون = check_ستون((win_x - xterm_hints.x) / xterm_hints.چوڑائی_inc);
    اگر (صف == پچھلا_صف && ستون == پچھلا_ستون)
	واپس TRUE;

    STRCPY(buf, mouse_رمز);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(ستون + ' ' + 1);
    *strp++ = (char_u)(صف + ' ' + 1);
    *strp = NUL;
    جوڑو_مقصد_داخلہ_buf(buf, strp - buf);

    پچھلا_صف = صف;
    پچھلا_ستون = ستون;
    واپس TRUE;
}

# اگر defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    خالی
clear_xterm_clip(خالی)
{
    اگر (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    اگر (xterm_dpy != تہی)
    {
#  اگر 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	اگر (x11_display == xterm_dpy)
	    x11_display = تہی;
	xterm_dpy = تہی;
    }
#  اگر 0
    اگر (app_context != (XtAppContext)تہی)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)تہی;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ساکن خالی
clip_update(خالی)
{
# ifdef FEAT_GUI
    اگر (gui.اندر_use)
	gui_mch_update();
    ورنہ
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
    خالی
xterm_update(خالی)
{
    XEvent event;

    برائے (;;)
    {
	Xt_داخلہ_نقاب نقاب = XtAppPending(app_context);

	اگر (نقاب == 0 || vim_is_داخلہ_buf_بھرا())
	    توڑو;

	اگر (نقاب & XtIMXEvent)
	{
	    // There is an event to process.
	    Xt_App_اگلا_Event(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		اگر (e->type == PropertyNotify && e->کھڑکی_2 == comm_کھڑکی
		   && e->atom == commProperty && e->state == Property_New_قدر)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	ورنہ
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, نقاب);
	}
    }
}

    صحیح
clip_xterm_own_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	واپس clip_x11_own_selection(xterm_Shell, cbd);
    واپس FAIL;
}

    خالی
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    خالی
clip_xterm_request_selection(Clipboard_T *cbd)
{
    اگر (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    خالی
clip_xterm_سیٹ_selection(Clipboard_T *cbd)
{
    clip_x11_سیٹ_selection(cbd);
}
#اگر_ختم


#اگر defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# اگر defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ساکن خالی
xsmp_handle_interaction(SmcConn smc_conn, Sm_اشارہ_گر client_ڈیٹا_2 غیر_مستعمل)
{
    صحیح		save_cmod_flags;
    صحیح		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    اگر (check_بدلا_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    برآمد_flush();

    // Done interaction
    Smc_Interact_مکمل(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    اگر (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	Smc_Save_Yourself_مکمل(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ساکن خالی
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    Sm_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل,
    صحیح		save_type غیر_مستعمل,
    Bool	shutdown,
    صحیح		interact_style غیر_مستعمل,
    Bool	fast غیر_مستعمل)
{
    // Handle already being in saveyourself
    اگر (xsmp.save_yourself)
	Smc_Save_Yourself_مکمل(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    برآمد_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    اگر (p_verbose > 0)
	verb_پیغام(_("XSMP handling save-yourself request"));

# اگر defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    اگر (shutdown && !fast && gui.اندر_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_ڈیٹا_2);
    ورنہ
# endif
    {
	// Can stop the cycle here
	Smc_Save_Yourself_مکمل(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    ساکن خالی
xsmp_die(SmcConn smc_conn غیر_مستعمل, Sm_اشارہ_گر client_ڈیٹا_2 غیر_مستعمل)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ساکن خالی
xsmp_save_complete(
    SmcConn	smc_conn غیر_مستعمل,
    Sm_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل)
{
    xsmp.save_yourself = False;
}


/*
 * Callback to tell us that an instigated shutdown was cancelled
 * (maybe even by us)
 */
    ساکن خالی
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    Sm_اشارہ_گر	client_ڈیٹا_2 غیر_مستعمل)
{
    اگر (xsmp.save_yourself)
	Smc_Save_Yourself_مکمل(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ساکن خالی
xsmp_ice_connection(
    IceConn	iceConn,
    Ice_اشارہ_گر	client_ڈیٹا غیر_مستعمل,
    Bool	opening,
    Ice_اشارہ_گر	*watch_ڈیٹا غیر_مستعمل)
{
    // Intercept creation of ICE connection fd
    اگر (opening)
    {
	xsmp_icefd = Ice_Connection_نمبر(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, تہی);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    صحیح
xsmp_handle_درخواستیں(خالی)
{
    Bool rep;

    اگر (IceProcessMessages(xsmp.iceconn, تہی, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	اگر (p_verbose > 0)
	    verb_پیغام(_("XSMP lost ICE connection"));
	xsmp_close();
	واپس FAIL;
    }
    ورنہ
	واپس OK;
}

ساکن صحیح dummy;

// Set up X Session Management Protocol
    خالی
xsmp_init(خالی)
{
    حرف		errorstring[80];
    SmcCallbacks	smcallbacks;
# اگر 0
    Sm_Prop_قدر		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    اگر (p_verbose > 0)
	verb_پیغام(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_ڈیٹا_2 = تہی;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_ڈیٹا_2 = تہی;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_ڈیٹا_2 = تہی;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_ڈیٹا_2 = تہی;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    اگر (Ice_جوڑو_Connection_Watch(xsmp_ice_connection, &dummy) == 0)
    {
	اگر (p_verbose > 0)
	    verb_پیغام(_("XSMP ICE connection watch failed"));
	واپس;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    تہی,
	    تہی,
	    SmProtoMajor,
	    SmProtoMinor,
	    Smc_Save_Yourself_Proc_نقاب | Smc_Die_Proc_نقاب
		     | Smc_Save_Complete_Proc_نقاب | Smc_Shutdown_Cancelled_Proc_نقاب,
	    &smcallbacks,
	    تہی,
	    &xsmp.clientid,
	    حجم(errorstring) - 1,
	    errorstring);
    اگر (xsmp.smcconn == تہی)
    {
	اگر (p_verbose > 0)
	{
	    حرف errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[حجم(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, حجم(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_پیغام(errorreport);
	}
	واپس;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# اگر 0
    // ID ourselves
    smname.قدر = "vim";
    smname.لمبائی = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.عدد_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    Smc_سیٹ_Properties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    خالی
xsmp_close(خالی)
{
    اگر (xsmp_icefd == -1)
	واپس;

    SmcCloseConnection(xsmp.smcconn, 0, تہی);
    اگر (xsmp.clientid != تہی)
	آزاد(xsmp.clientid);
    xsmp.clientid = تہی;
    xsmp_icefd = -1;
}
#اگر_ختم // USE_XSMP

#اگر defined(FEAT_RELTIME)
# اگر defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ساکن متلون sig_atomic_t timeout_flag = FALSE;
ساکن timer_t		     timer_id;
ساکن صحیح		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ساکن خالی
سیٹ_flag(اتحاد sigval غیر_مستعمل_3 غیر_مستعمل)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    خالی
stop_timeout(خالی)
{
    ساکن ساخت itimerspec disarm = {{0, 0}, {0, 0}};

    اگر (timer_created)
    {
	صحیح ret = timer_settime(timer_id, 0, &disarm, تہی);

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
    متلون sig_atomic_t *
آغاز_timeout(طویل msec)
{
    ساخت itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    صحیح ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    اگر (!timer_created)
    {
	ساخت sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = سیٹ_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	اگر (ret < 0)
	{
	    semsg(_(e_could_not_سیٹ_timeout_str), strerror(errno));
	    واپس &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(تہی, "setting timeout timer to %d sec %ld nsec",
	       (صحیح)interval.it_value.tv_sec, (طویل)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, تہی);
    اگر (ret < 0)
	semsg(_(e_could_not_سیٹ_timeout_str), strerror(errno));

    واپس &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    خالی
delete_timer(خالی)
{
    اگر (!timer_created)
	واپس;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# ورنہ // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ساکن ساخت sigaction		پچھلا_sigaction;
ساکن متلون sig_atomic_t	timeout_flag	     = FALSE;
ساکن صحیح			timer_فعال	     = FALSE;
ساکن صحیح			timer_handler_فعال = FALSE;
ساکن متلون sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ساکن خالی
سیٹ_flag SIGDEFARG(sigarg)
{
    اگر (alarm_pending)
	alarm_pending = FALSE;
    ورنہ
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    خالی
stop_timeout(خالی)
{
    ساکن ساخت itimerval disarm = {{0, 0}, {0, 0}};
    صحیح			    ret;

    اگر (timer_فعال)
    {
	timer_فعال = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, تہی);
	اگر (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    اگر (timer_handler_فعال)
    {
	timer_handler_فعال = FALSE;
	ret = sigaction(SIGALRM, &پچھلا_sigaction, تہی);
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
    متلون sig_atomic_t *
آغاز_timeout(طویل msec)
{
    ساخت itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ساخت sigaction	handle_alarm;
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
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, تہی);
    }
    اگر (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = سیٹ_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &پچھلا_sigaction) : ret;
    اگر (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_سیٹ_handler_for_timeout_str), strerror(errno));
	واپس &timeout_flag;
    }
    timer_handler_فعال = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, تہی);
    اگر (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_سیٹ_timeout_str), strerror(errno));
	stop_timeout();
	واپس &timeout_flag;
    }

    timer_فعال = TRUE;
    واپس &timeout_flag;
}
# endif // PROF_NSEC
#اگر_ختم  // FEAT_RELTIME
