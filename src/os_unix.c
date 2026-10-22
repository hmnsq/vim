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

#अन्तर्भाव "vim.h"

#ifdef FEAT_MZSCHEME
# include "if_mzsch.h"
#endif

#अन्तर्भाव "os_unixx.h"	    // unix includes for os_unix.c only

#ifdef USE_XSMP
# include <X11/SM/SMlib.h>
#endif

#ifdef HAVE_SELINUX
# include <selinux/selinux.h>
स्थिर पूर्णाङ्क selinux_enabled = -1;
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
#  define gpm_flag     (dll_gpm_flag != रिक्त ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != रिक्त ? *dll_gpm_fd   : -1)

स्थिर पूर्णाङ्क (*dll_Gpm_Open)     (Gpm_Connect *, पूर्णाङ्क);
स्थिर पूर्णाङ्क (*dll_Gpm_Close)    (शून्य);
स्थिर पूर्णाङ्क (*dll_Gpm_GetEvent) (Gpm_Event *);
स्थिर पूर्णाङ्क *dll_gpm_flag;
स्थिर पूर्णाङ्क *dll_gpm_fd;

स्थिर शून्य *libgpm_hinst;
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

स्थिर शून्य gpm_close(शून्य);
स्थिर पूर्णाङ्क gpm_open(शून्य);
स्थिर पूर्णाङ्क mch_gpm_process(शून्य);
#endif

#ifdef FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

स्थिर पूर्णाङ्क sysmouse_open(शून्य);
स्थिर शून्य sysmouse_close(शून्य);
स्थिर शून्य sig_sysmouse SIGPROTOARG;
#endif

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#यदि defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#endif

#यदि defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
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
स्थिर Widget	xterm_Shell = (Widget)0;
स्थिर शून्य clip_update(शून्य);
# endif

Window	    x11_window = 0;
Display	    *x11_display = रिक्त;
#endif

स्थिर पूर्णाङ्क ignore_sigtstp = FALSE;

स्थिर पूर्णाङ्क get_x11_title(पूर्णाङ्क);

स्थिर char_u	*oldtitle = रिक्त;
स्थिर चञ्चल संक_पूर्ण_प्रकार oldtitle_outdated = FALSE;
स्थिर पूर्णाङ्क	unix_did_set_title = FALSE;
स्थिर char_u	*oldicon = रिक्त;
स्थिर पूर्णाङ्क	did_set_icon = FALSE;

स्थिर शून्य may_core_dump(शून्य);

प्रकारनिर्देश पूर्णाङ्क waitstatus;
स्थिर पूर्णाङ्क  WaitForChar(दीर्घ msec, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेश);
स्थिर पूर्णाङ्क  WaitForCharOrMouse(दीर्घ msec, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेश);
#ifdef VMS
पूर्णाङ्क  RealWaitForChar(पूर्णाङ्क, दीर्घ, पूर्णाङ्क *, पूर्णाङ्क *interrupted);
#अन्यथा
स्थिर पूर्णाङ्क  RealWaitForChar(पूर्णाङ्क, दीर्घ, पूर्णाङ्क *, पूर्णाङ्क *interrupted);
#endif

#ifdef FEAT_XCLIPBOARD
स्थिर पूर्णाङ्क do_xterm_trace(शून्य);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#endif

स्थिर शून्य handle_resize(शून्य);

#यदि defined(SIGWINCH)
स्थिर शून्य sig_winch SIGPROTOARG;
#endif
#यदि defined(SIGTSTP)
स्थिर शून्य sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
स्थिर चञ्चल संक_पूर्ण_प्रकार in_mch_suspend = FALSE;
#endif
#यदि defined(संक_विघ्न)
स्थिर शून्य catch_sigint SIGPROTOARG;
#endif
#यदि defined(SIGUSR1)
स्थिर शून्य catch_sigusr1 SIGPROTOARG;
#endif
#यदि defined(SIGPWR)
स्थिर शून्य catch_sigpwr SIGPROTOARG;
#endif
#यदि defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define SET_SIG_ALARM
स्थिर शून्य sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
स्थिर चञ्चल संक_पूर्ण_प्रकार sig_alarm_called;
#endif
स्थिर शून्य deathtrap SIGPROTOARG;

स्थिर शून्य catch_int_signal(शून्य);
स्थिर शून्य set_signals(शून्य);
स्थिर शून्य catch_signals(शून्य (*func_deadly)(पूर्णाङ्क), शून्य (*func_other)(पूर्णाङ्क));
#ifdef HAVE_SIGPROCMASK
# define SIGSET_DECL(set)	sigset_t set;
# define BLOCK_SIGNALS(set)	block_signals(set)
# define UNBLOCK_SIGNALS(set)	unblock_signals(set)
#अन्यथा
# define SIGSET_DECL(set)
# define BLOCK_SIGNALS(set)	कुरु { /**/ } यावत् (0)
# define UNBLOCK_SIGNALS(set)	कुरु { /**/ } यावत् (0)
#endif
स्थिर पूर्णाङ्क  have_wildcard(पूर्णाङ्क, char_u **);
स्थिर पूर्णाङ्क  have_dollars(पूर्णाङ्क, char_u **);

स्थिर पूर्णाङ्क save_patterns(पूर्णाङ्क अंक_pat, char_u **pat, पूर्णाङ्क *अंक_file, char_u ***file);

#ifndef संक_त्रुटि
# define संक_त्रुटि	((sighandler_T)-1)
#endif
#ifndef SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#endif

// volatile because it is used in signal handler sig_winch().
स्थिर चञ्चल संक_पूर्ण_प्रकार do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
स्थिर चञ्चल संक_पूर्ण_प्रकार got_tstp = FALSE;
स्थिर char_u	*extra_shell_arg = रिक्त;
स्थिर पूर्णाङ्क	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
स्थिर चञ्चल संक_पूर्ण_प्रकार deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
स्थिर चञ्चल संक_पूर्ण_प्रकार in_mch_delay = FALSE; // sleeping in mch_delay()

#यदि defined(FEAT_JOB_CHANNEL) && !defined(USE_SYSTEM)
स्थिर पूर्णाङ्क dont_check_job_ended = 0;
#endif

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
स्थिर tmode_T mch_cur_tmode = TMODE_COOK;

#ifdef USE_XSMP
प्रकारनिर्देश संरचना
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    वर्ण *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

स्थिर xsmp_config_T xsmp;
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

स्थिर संरचना signalinfo
{
    पूर्णाङ्क	    sig;	// Signal number, eg. SIGSEGV etc
    वर्ण    *name;	// Signal name (not char_u!).
    वर्ण    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#ifdef SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#endif
#ifdef SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#endif
#ifdef संक_अवैध
    {संक_अवैध,	    "ILL",	TRUE},
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
#ifdef संक_भ_त्रुटि
    {संक_भ_त्रुटि,	    "FPE",	TRUE},
#endif
#ifdef SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#endif
#यदि defined(संक_अंश) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {संक_अंश,	    "SEGV",	TRUE},
#endif
#ifdef SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#endif
#ifdef SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#endif
#ifdef संक_इति
    {संक_इति,	    "TERM",	TRUE},
#endif
#यदि defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#endif
#यदि defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
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
#यदि defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#endif
#ifdef संक_विघ्न
    {संक_विघ्न,	    "INT",	FALSE},
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
mch_signal(पूर्णाङ्क sig, sighandler_T func)
{
#यदि defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    संरचना sigaction	sa, old;
    sigset_t		curset;
    पूर्णाङ्क			blocked;

    यदि (sigprocmask(SIG_BLOCK, रिक्त, &curset) == -1)
	प्रत्यागम संक_त्रुटि;

    blocked = sigismember(&curset, sig);

    यदि (func == SIG_HOLD)
    {
	यदि (blocked)
	    प्रत्यागम SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	यदि (sigaction(sig, रिक्त, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, रिक्त) == -1)
	    प्रत्यागम संक_त्रुटि;
	प्रत्यागम old.sa_handler;
    }

    यदि (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	यदि (sigprocmask(SIG_UNBLOCK, &curset, रिक्त) == -1)
	    प्रत्यागम संक_त्रुटि;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# अन्यथा
    sa.sa_flags = 0;
# endif
    यदि (sigaction(sig, &sa, &old) == -1)
	प्रत्यागम संक_त्रुटि;
    प्रत्यागम blocked ? SIG_HOLD: old.sa_handler;
#नोचेद्यदि defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    प्रत्यागम sigset(sig, func);
#अन्यथा
    // Oldest and most compatible solution.
    प्रत्यागम सङ्केत(sig, func);
#endif
}

    पूर्णाङ्क
mch_chdir(वर्ण *path)
{
    यदि (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", path);
	verbose_leave();
    }
#ifdef VMS
    प्रत्यागम स_बदलो(vms_fixfilename(path));
#अन्यथा
    प्रत्यागम स_बदलो(path);
#endif
}

#यदि defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#endif

/*
 * Write s[len] to the screen (stdout).
 */
    शून्य
mch_write(char_u *s, पूर्णाङ्क len)
{
    vim_ignored = (पूर्णाङ्क)लिख(1, (वर्ण *)s, len);
    यदि (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, रिक्त, रिक्त);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    स्थिर पूर्णाङ्क
resize_func(पूर्णाङ्क check_only)
{
    यदि (check_only)
	प्रत्यागम do_resize;
    यावत् (do_resize)
    {
#ifdef FEAT_EVAL
	ch_log(रिक्त, "calling handle_resize() in resize_func()");
#endif
	handle_resize();
    }
    प्रत्यागम FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    पूर्णाङ्क
mch_inchar(
    char_u	*buf,
    पूर्णाङ्क		maxlen,
    दीर्घ	wtime,	    // don't use "time", MIPS cannot handle it
    पूर्णाङ्क		tb_change_cnt)
{
    प्रत्यागम inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    स्थिर शून्य
handle_resize(शून्य)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    पूर्णाङ्क
mch_char_avail(शून्य)
{
    प्रत्यागम WaitForChar(0L, रिक्त, FALSE);
}

#यदि defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    पूर्णाङ्क
mch_check_messages(शून्य)
{
    प्रत्यागम WaitForChar(0L, रिक्त, TRUE);
}
#endif

#यदि defined(HAVE_TOTAL_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# यदि defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# यदि defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_total_mem(पूर्णाङ्क special UNUSED)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_port_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	संरचना vm_statistics64	vm_stat;
	natural_t		गिनती = HOST_VM_INFO64_गिनती;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &गिनती);
#  अन्यथा
	संरचना vm_statistics	vm_stat;
	natural_t		गिनती = HOST_VM_INFO_गिनती;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &गिनती);
#  endif
	यदि (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_गिनती + vm_stat.active_गिनती
					    + vm_stat.inactive_गिनती
#  ifdef MAC_OS_X_VERSION_10_9
					    + vm_stat.compressor_page_गिनती
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_port_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    यदि (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	पूर्णाङ्क		mib[2];
	माप_प्रकार		len = परिमाण(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  अन्यथा
	// sysctl() may return 32 bit or 64 bit, accept both
	सङ्घ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  अन्यथा
	mib[1] = HW_USERMEM;
#  endif
	यदि (sysctl(mib, 2, &physmem, &len, रिक्त, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  अन्यथा
	    यदि (len == परिमाण(physmem.u64))
		mem = (long_u)physmem.u64;
	    अन्यथा
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# यदि defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    यदि (mem == 0)
    {
	संरचना sysinfo sinfo;

	// Linux way of getting amount of RAM available
	यदि (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    यावत् (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  अन्यथा
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    यदि (mem == 0)
    {
	दीर्घ	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	यदि (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    यावत् (shiftright > 0 && (pagesize & 1) == 0)
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
# यदि defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	संरचना rlimit	rlp;

	यदि (getrlimit(RLIMIT_DATA, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (परिमाण(long_u) * 8 - 1))
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

    यदि (mem > 0)
	प्रत्यागम mem >> shiftright;
    प्रत्यागम (long_u)0x1fffff;
}
#endif

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    शून्य
mch_delay(दीर्घ msec, पूर्णाङ्क flags)
{
    tmode_T	old_tmode;
    पूर्णाङ्क		call_settmode;
#ifdef FEAT_MZSCHEME
    दीर्घ	total = msec; // remember original value
#endif

    यदि (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	in_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	यदि (call_settmode)
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
	कुरु
	{
	    // if total is large enough, wait by portions in p_mzq
	    यदि (total > p_mzq)
		msec = p_mzq;
	    अन्यथा
		msec = total;
	    total -= msec;
#endif
#ifdef HAVE_NANOSLEEP
	{
	    संरचना timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (शून्य)nanosleep(&ts, रिक्त);
	}
#अन्यथा
# ifdef HAVE_USLEEP
	यावत् (msec >= 1000)
	{
	    usleep((अचिन्हित पूर्णाङ्क)(999 * 1000));
	    msec -= 999;
	}
	usleep((अचिन्हित पूर्णाङ्क)(msec * 1000));
# अन्यथा
#  ifndef HAVE_SELECT
	poll(रिक्त, 0, (पूर्णाङ्क)msec);
#  अन्यथा
	{
	    संरचना timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, रिक्त, रिक्त, रिक्त, &tv);
	}
#  endif
# endif
#endif
#ifdef FEAT_MZSCHEME
	}
	यावत् (total > 0);
#endif

	यदि (call_settmode)
	    settmode(old_tmode);
	in_mch_delay = FALSE;
    }
    अन्यथा
	WaitForChar(msec, रिक्त, FALSE);
}

#यदि defined(HAVE_ढेर_LIMIT) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ढेर_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
स्थिर पूर्णाङ्क ढेर_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    स्थिर शून्य
check_ढेर_growth(वर्ण *p)
{
    पूर्णाङ्क		i;

    ढेर_grows_downwards = (p > (वर्ण *)&i);
}
#endif

#यदि defined(HAVE_ढेर_LIMIT)
स्थिर वर्ण *ढेर_limit = रिक्त;

# यदि defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    स्थिर शून्य
get_ढेर_limit(शून्य)
{
    संरचना rlimit	rlp;
    पूर्णाङ्क			i;
    दीर्घ		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    यदि (getrlimit(RLIMIT_ढेर, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (परिमाण(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (दीर्घ)rlp.rlim_cur;
# यदि defined(_THREAD_SAFE) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    माप_प्रकार	    size;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    यदि (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &size);
		यदि (lim > (दीर्घ)size)
		    lim = (दीर्घ)size;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	यदि (ढेर_grows_downwards)
	{
	    ढेर_limit = (वर्ण *)((दीर्घ)&i - (lim / 16L * 15L));
	    यदि (ढेर_limit >= (वर्ण *)&i)
		// overflow, set to 1/16 of current stack position
		ढेर_limit = (वर्ण *)((दीर्घ)&i / 16L);
	}
	अन्यथा
	{
	    ढेर_limit = (वर्ण *)((दीर्घ)&i + (lim / 16L * 15L));
	    यदि (ढेर_limit <= (वर्ण *)&i)
		ढेर_limit = रिक्त;	// overflow
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    पूर्णाङ्क
mch_stackcheck(वर्ण *p)
{
    यदि (ढेर_limit == रिक्त)
	प्रत्यागम OK;

    यदि (ढेर_grows_downwards)
    {
	यदि (p < ढेर_limit)
	    प्रत्यागम FAIL;
    }
    अन्यथा यदि (p > ढेर_limit)
	प्रत्यागम FAIL;
    प्रत्यागम OK;
}
#endif

#यदि defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
स्थिर stack_t sigstk;			// for sigaltstack()
# अन्यथा
स्थिर संरचना sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
स्थिर दीर्घ पूर्णाङ्क get_signal_ढेर_size(शून्य)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    दीर्घ पूर्णाङ्क size = -1;

    // return size only if sysconf doesn't return an error
    यदि ((size = sysconf(_SC_SIGSTKSZ)) > -1)
	प्रत्यागम size;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    प्रत्यागम SIGSTKSZ;
# endif

    // otherwise guess the size
    प्रत्यागम 8000;
}

स्थिर वर्ण *signal_ढेर;

    स्थिर शून्य
init_signal_ढेर(शून्य)
{
    यदि (signal_ढेर == रिक्त)
	प्रत्यागम;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_BASE
    sigstk.ss_base = signal_ढेर;
#  अन्यथा
    sigstk.ss_sp = signal_ढेर;
#  endif
    sigstk.ss_size = get_signal_ढेर_size();
    sigstk.ss_flags = 0;
    (शून्य)sigaltstack(&sigstk, रिक्त);
# अन्यथा
    sigstk.ss_sp = signal_ढेर;
    यदि (ढेर_grows_downwards)
	sigstk.ss_sp += get_signal_ढेर_size() - 1;
    sigstk.ss_onstack = 0;
    (शून्य)sigstack(&sigstk, रिक्त);
# endif
}
#endif

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#यदि defined(SIGWINCH)
    स्थिर शून्य
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#endif

#यदि defined(SIGTSTP)
    स्थिर शून्य
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    यदि (in_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? संक_छोड़ो : संक_पूर्व);
	उत्थापय(sigarg);
    }
    अन्यथा
	got_tstp = TRUE;

# यदि !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#endif

#यदि defined(संक_विघ्न)
    स्थिर शून्य
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(संक_विघ्न, catch_sigint);
    got_int = TRUE;
}
#endif

#यदि defined(SIGUSR1)
    स्थिर शून्य
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#endif

#यदि defined(SIGPWR)
    स्थिर शून्य
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
    स्थिर शून्य
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#endif

#यदि defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
स्थिर JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
स्थिर चञ्चल संक_पूर्ण_प्रकार lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
स्थिर चञ्चल संक_पूर्ण_प्रकार lc_active = FALSE;

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
    स्थिर शून्य
mch_startjmp(शून्य)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_active = TRUE;
}

    स्थिर शून्य
mch_endjmp(शून्य)
{
    lc_active = FALSE;
}

    स्थिर शून्य
mch_didjmp(शून्य)
{
# यदि defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ढेर();
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
    स्थिर शून्य
deathtrap SIGDEFARG(sigarg)
{
    स्थिर पूर्णाङ्क	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ifdef SIGHASARG
    पूर्णाङ्क		i;
#endif

#यदि defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    यदि (lc_active)
    {
# यदि defined(SIGHASARG)
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
    यदि (in_mch_delay && sigarg == SIGQUIT)
	प्रत्यागम;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    यदि (entered == 0
	    && (0
# ifdef SIGHUP
		|| sigarg == SIGHUP
# endif
# ifdef SIGQUIT
		|| sigarg == SIGQUIT
# endif
# ifdef संक_इति
		|| sigarg == संक_इति
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
	प्रत्यागम;
#endif

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    block_autocmds();

#ifdef FEAT_EVAL
    // Set the v:dying variable.
    set_vim_var_nr(VV_DYING, (दीर्घ)entered);
#endif
    v_dying = entered;

#ifdef HAVE_ढेर_LIMIT
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ढेर_limit();
#endif

#यदि 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	खाता *fp = ख_खोलो(VI_GDB_FILE, "w");
	यदि (fp)
	{
	    ख_लिखो(fp,
		    "file %s\n"
		    "attach %d\n"
		    "set height 1000\n"
		    "bt full\n"
		    , VIM_NAME, getpid());
	    ख_बंद(fp);
	    प्रणाली("xterm -e gdb -x "VI_GDB_FILE);
	    unlink(VI_GDB_FILE);
	}
    }
#endif

#ifdef SIGHASARG
    // try to find the name of this signal
    प्रति (i = 0; signal_info[i].sig != -1; i++)
	यदि (sigarg == signal_info[i].sig)
	    भङ्ग;
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
    यदि (entered >= 3)
    {
	reset_signals();	// don't catch any signals anymore
	may_core_dump();
	यदि (entered >= 4)
	    _exit(8);
	निकास(7);
    }
    यदि (entered == 2)
    {
	// No translation, it may call malloc().
	OUT_STR("Vim: Double signal, exiting\n");
	out_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ifdef SIGHASARG
    प्र_लिखो((वर्ण *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#अन्यथा
    प्र_लिखो((वर्ण *)IObuff, "Vim: Caught deadly signal\r\n");
#endif

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#ifdef NBDEBUG
    reset_signals();
    may_core_dump();
    पात();
#endif
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    स्थिर शून्य
after_sigcont(शून्य)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#यदि defined(SIGCONT)
स्थिर शून्य sigcont_handler SIGPROTOARG;

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
स्थिर चञ्चल संक_पूर्ण_प्रकार sigcont_received;
स्थिर शून्य sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    स्थिर शून्य
sigcont_handler SIGDEFARG(sigarg)
{
    यदि (in_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    अन्यथा
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

#यदि defined(FEAT_CLIPBOARD)
# यदि defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
स्थिर शून्य *clip_star_save = रिक्त;
स्थिर शून्य *clip_plus_save = रिक्त;
# endif

# यदि defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    स्थिर शून्य
loose_clipboard(शून्य)
{
    यदि (clip_star.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	यदि (clip_star.owned)
	    clip_lose_selection(&clip_star);
	यदि (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	यदि (x11_display != रिक्त)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# यदि defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    स्थिर शून्य
save_clipboard(शून्य)
{
    यदि (clip_star.owned)
	clip_star_save = get_register('*', TRUE);
    यदि (clip_plus.owned)
	clip_plus_save = get_register('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    स्थिर शून्य
restore_clipboard(शून्य)
{
    यदि (clip_star_save != रिक्त)
    {
	यदि (!clip_gen_owner_exists(&clip_star))
	    put_register('*', clip_star_save);
	अन्यथा
	    free_register(clip_star_save);
	clip_star_save = रिक्त;
    }
    यदि (clip_plus_save != रिक्त)
    {
	यदि (!clip_gen_owner_exists(&clip_plus))
	    put_register('+', clip_plus_save);
	अन्यथा
	    free_register(clip_plus_save);
	clip_plus_save = रिक्त;
    }
}
# endif
#endif

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    शून्य
mch_suspend(शून्य)
{
    यदि (ignore_sigtstp)
	प्रत्यागम;

#यदि defined(SIGTSTP)
    in_mch_suspend = TRUE;

    out_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    out_flush();	    // needed to disable mouse on some systems

# यदि defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# यदि defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    समाप्त(0, SIGTSTP);	    // send ourselves a STOP signal

# यदि defined(SIGCONT)
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
	दीर्घ wait_time;

	प्रति (wait_time = 0; !sigcont_received && wait_time <= 3L; wait_time++)
	    mch_delay(wait_time, 0);
    }
# endif
    in_mch_suspend = FALSE;

    after_sigcont();
#अन्यथा
    suspend_shell();
#endif
}

    शून्य
mch_init(शून्य)
{
    Columns = 80;
    Rows = 24;

    out_flush();

#ifdef SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || संक_छोड़ो == mch_signal(SIGTSTP, संक_त्रुटि);
#endif
    set_signals();

#ifdef MACOS_CONVERT
    mac_conv_init();
#endif
#ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#endif
}

    शून्य
set_sigwinch_handler(शून्य)
{
#यदि defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#endif
}

    स्थिर शून्य
set_signals(शून्य)
{
#यदि defined(SIGWINCH)
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
    mch_signal(SIGTSTP, ignore_sigtstp ? संक_छोड़ो
# ifdef FEAT_GUI
				: gui.in_use || gui.starting ? संक_पूर्व
# endif
				    : sig_tstp);
#endif
#यदि defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#endif
#ifdef SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, संक_छोड़ो);
#endif

#ifdef संक_विघ्न
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
    mch_signal(SIGALRM, संक_छोड़ो);
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
    catch_signals(deathtrap, संक_त्रुटि);

#यदि defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    यदि (gui.in_use)
	mch_signal(SIGHUP, संक_छोड़ो);
#endif
}

#यदि defined(संक_विघ्न)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    स्थिर शून्य
catch_int_signal(शून्य)
{
    mch_signal(संक_विघ्न, catch_sigint);
}
#endif

    शून्य
reset_signals(शून्य)
{
    catch_signals(संक_पूर्व, संक_पूर्व);
#यदि defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, संक_पूर्व);
#endif
}

    स्थिर शून्य
catch_signals(
    शून्य (*func_deadly)(पूर्णाङ्क),
    शून्य (*func_other)(पूर्णाङ्क))
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; signal_info[i].sig != -1; i++)
    {
	यदि (signal_info[i].deadly)
	{
#यदि defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    संरचना sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# यदि defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# अन्यथा
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, रिक्त);
#अन्यथा
# यदि defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    संरचना sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_mask = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, रिक्त);
# अन्यथा
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#endif
	}
	अन्यथा यदि (func_other != संक_त्रुटि)
	{
	    // Deal with non-deadly signals.
#ifdef SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? संक_छोड़ो : func_other);
#अन्यथा
	    mch_signal(signal_info[i].sig, func_other);
#endif
	}
    }
}

#ifdef HAVE_SIGPROCMASK
    स्थिर शून्य
block_signals(sigset_t *set)
{
    sigset_t	newset;
    पूर्णाङ्क		i;

    sigemptyset(&newset);

    प्रति (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# यदि defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, set);
}

    स्थिर शून्य
unblock_signals(sigset_t *set)
{
    sigprocmask(SIG_SETMASK, set, रिक्त);
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
    पूर्णाङ्क
vim_handle_signal(पूर्णाङ्क sig)
{
    स्थिर पूर्णाङ्क got_signal = 0;
    स्थिर पूर्णाङ्क blocked = TRUE;

    विकल्प (sig)
    {
	पक्ष SIGNAL_BLOCK:   blocked = TRUE;
			     भङ्ग;

	पक्ष SIGNAL_UNBLOCK: blocked = FALSE;
			     यदि (got_signal != 0)
			     {
				 समाप्त(getpid(), got_signal);
				 got_signal = 0;
			     }
			     भङ्ग;

	स्वभाव:	     यदि (!blocked)
				 प्रत्यागम TRUE;	// exit!
			     got_signal = sig;
#ifdef SIGPWR
			     यदि (sig != SIGPWR)
#endif
				 got_int = TRUE;    // break any loops
			     भङ्ग;
    }
    प्रत्यागम FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    पूर्णाङ्क
mch_check_win(पूर्णाङ्क argc UNUSED, वर्ण **argv UNUSED)
{
    यदि (isatty(1))
	प्रत्यागम OK;
    प्रत्यागम FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    पूर्णाङ्क
mch_निवेश_isatty(शून्य)
{
    यदि (isatty(read_cmd_fd))
	प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#ifdef FEAT_X11

# यदि defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    स्थिर शून्य
xopen_message(दीर्घ elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#endif

#यदि defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

स्थिर पूर्णाङ्क	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    स्थिर पूर्णाङ्क
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGetErrorText(dpy, error_event->error_code, (वर्ण *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    यदि (!gui.in_use)
# endif
	संदेश((वर्ण *)IObuff);

    प्रत्यागम 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    स्थिर पूर्णाङ्क
x_error_check(Display *dpy UNUSED, XErrorEvent *error_event UNUSED)
{
    got_x_error = TRUE;
    प्रत्यागम 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    स्थिर पूर्णाङ्क
x_connect_to_server(शून्य)
{
    // No point in connecting if we are exiting or dying.
    यदि (exiting || v_dying)
	प्रत्यागम FALSE;

# यदि defined(FEAT_CLIENTSERVER)
    यदि (x_force_connect)
	प्रत्यागम TRUE;
# endif
    यदि (x_no_connect)
	प्रत्यागम FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    यदि (clip_exclude_prog != रिक्त)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	यदि (regprog_in_use(clip_exclude_prog))
	    प्रत्यागम FALSE;

	यदि (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    प्रत्यागम FALSE;
    }
    प्रत्यागम TRUE;
}

# यदि defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  यदि defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    स्थिर पूर्णाङ्क
x_IOerror_check(Display *dpy UNUSED)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   यदि defined(VMS) || defined(__CYGWIN__)
    प्रत्यागम 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
स्थिर पूर्णाङ्क xterm_dpy_retry_गिनती = 0;

    स्थिर पूर्णाङ्क
x_IOerror_handler(Display *dpy UNUSED)
{
    xterm_dpy = रिक्त;
    xterm_dpy_retry_गिनती = 5;  // Try reconnecting five times
    x11_window = 0;
    x11_display = रिक्त;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  यदि defined(VMS) || defined(__CYGWIN__)
    प्रत्यागम 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    स्थिर शून्य
may_restore_x11_clipboard(शून्य)
{
    // No point in restoring the connecting if we are exiting or dying.
    यदि (!exiting && !v_dying && xterm_dpy_retry_गिनती > 0)
    {
	--xterm_dpy_retry_गिनती;

#  ifndef LESSTIF_VERSION
	// This has been reported to avoid Vim getting stuck.
	यदि (app_context != (XtAppContext)रिक्त)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)रिक्त;
	    x11_display = रिक्त; // freed by XtDestroyApplicationContext()
	}
#  endif

	setup_term_clip();
	get_x11_title(FALSE);
    }
}

    शून्य
ex_xrestore(exarg_T *eap)
{
    माप_प्रकार  arglen;

    यदि (eap->arg != रिक्त && (arglen = STRLEN(eap->arg)) > 0)
    {
	यदि (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (वर्ण *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == रिक्त
		    ? (वर्ण *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_window = 0;
    xterm_dpy_retry_गिनती = 5;  // Try reconnecting five times
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
    स्थिर पूर्णाङ्क
test_x11_window(Display *dpy)
{
    पूर्णाङ्क			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	text_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    यदि (XGetWMName(dpy, x11_window, &text_prop))
	XFree((शून्य *)text_prop.मूल्य);
    XSync(dpy, False);
    (शून्य)XSetErrorHandler(old_handler);

    यदि (p_verbose > 0 && got_x_error)
	verb_संदेश(_("Testing the X display failed"));

    प्रत्यागम (got_x_error ? FAIL : OK);
}
#endif


#ifdef FEAT_X11

स्थिर पूर्णाङ्क get_x11_thing(पूर्णाङ्क get_title, पूर्णाङ्क test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    स्थिर पूर्णाङ्क
get_x11_windis(शून्य)
{
    वर्ण	    *winid;
    स्थिर पूर्णाङ्क	    result = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_HERE	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    स्थिर पूर्णाङ्क	    x11_display_from = XD_NONE;
    स्थिर पूर्णाङ्क	    did_set_error_handler = FALSE;

    यदि (!did_set_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(शून्य)XSetErrorHandler(x_error_handler);
	did_set_error_handler = TRUE;
    }

# यदि defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    यदि (gui.in_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	यदि (x11_display_from == XD_HERE && x11_display != रिक्त)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	यदि (gui_get_x11_windis(&x11_window, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    प्रत्यागम OK;
	}
	x11_display = रिक्त;
	प्रत्यागम FAIL;
    }
    अन्यथा यदि (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_window = 0;
	x11_display = रिक्त;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    यदि (!x_connect_to_server())
	प्रत्यागम FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    यदि (x11_window == 0 && (winid = दो_पर्या("WINDOWID")) != रिक्त)
	x11_window = (Window)म_से_द(winid);

# ifdef FEAT_XCLIPBOARD
    यदि (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    यदि (xterm_dpy != रिक्त && x11_window != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	यदि (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    यदि (x11_display_from == XD_HERE && x11_display != रिक्त)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	यदि (test_x11_window(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_window = 0;
	    x11_display = रिक्त;
	    x11_display_from = XD_NONE;
	    प्रत्यागम FAIL;
	}
	प्रत्यागम OK;
    }
# endif

    यदि (x11_window == 0 || x11_display == रिक्त)
	result = -1;

    यदि (result != -1)	    // Have already been here and set this
	प्रत्यागम result;	    // Don't do all these X calls again

    यदि (x11_window != 0 && x11_display == रिक्त)
    {
# ifdef SET_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	यदि (p_verbose > 0)
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
	x11_display = XOpenDisplay(रिक्त);

# ifdef SET_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	यदि (p_verbose > 0 && sig_alarm_called)
	    verb_संदेश(_("Opening the X display timed out"));
# endif
	यदि (x11_display != रिक्त)
	{
# ifdef ELAPSED_FUNC
	    यदि (p_verbose > 0)
	    {
		verbose_enter();
		xopen_message(ELAPSED_FUNC(start_tv));
		verbose_leave();
	    }
# endif
	    यदि (test_x11_window(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_window = 0;
		XCloseDisplay(x11_display);
		x11_display = रिक्त;
	    }
	    अन्यथा
		x11_display_from = XD_HERE;
	}
    }
    यदि (x11_window == 0 || x11_display == रिक्त)
	प्रत्यागम (result = FAIL);

# ifdef FEAT_EVAL
    set_vim_var_nr(VV_WINDOWID, (दीर्घ)x11_window);
# endif

    प्रत्यागम (result = OK);
}

/*
 * Determine original x11 Window Title
 */
    स्थिर पूर्णाङ्क
get_x11_title(पूर्णाङ्क test_only)
{
    प्रत्यागम get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    स्थिर पूर्णाङ्क
get_x11_icon(पूर्णाङ्क test_only)
{
    पूर्णाङ्क		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    यदि (oldicon == रिक्त && !test_only)
    {
	यदि (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	अन्यथा
	    oldicon = vim_strsave(T_NAME);
    }

    प्रत्यागम retval;
}

    स्थिर पूर्णाङ्क
get_x11_thing(
    पूर्णाङ्क		get_title,	// get title string
    पूर्णाङ्क		test_only)
{
    XTextProperty	text_prop;
    पूर्णाङ्क			retval = FALSE;
    Status		status;

    यदि (get_x11_windis() != OK)
	प्रत्यागम FALSE;

    // Get window/icon name if any
    यदि (get_title)
	status = XGetWMName(x11_display, x11_window, &text_prop);
    अन्यथा
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
    Window	    मूल;
    Window	    parent;
    Window	    win = x11_window;
    Window	   *children;
    अचिन्हित पूर्णाङ्क    अंक_children;

    यावत् (!status || text_prop.मूल्य == रिक्त)
    {
	यदि (!XQuery_वृक्ष(x11_display, win, &मूल, &parent, &children,
		    &अंक_children))
	    भङ्ग;
	यदि (children)
	    XFree((शून्य *)children);
	यदि (parent == मूल || parent == 0)
	    भङ्ग;

	win = parent;
	यदि (get_title)
	    status = XGetWMName(x11_display, win, &text_prop);
	अन्यथा
	    status = XGetWMIconName(x11_display, win, &text_prop);
    }

    यदि (status && text_prop.मूल्य != रिक्त)
    {
	retval = TRUE;
	यदि (!test_only)
	{
	    यदि (get_title)
		vim_free(oldtitle);
	    अन्यथा
		vim_free(oldicon);
	    यदि (text_prop.encoding == XA_STRING && !has_mbyte)
	    {
		यदि (get_title)
		    oldtitle = vim_strsave((char_u *)text_prop.मूल्य);
		अन्यथा
		    oldicon = vim_strsave((char_u *)text_prop.मूल्य);
	    }
	    अन्यथा
	    {
		वर्ण    **cl;
		Status  transform_status;
		पूर्णाङ्क	    n = 0;

		transform_status = XmbTextPropertyToTextList(x11_display,
			&text_prop,
			&cl, &n);
		यदि (transform_status >= Success && n > 0 && cl[0])
		{
		    यदि (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    अन्यथा
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		अन्यथा
		{
		    यदि (get_title)
			oldtitle = vim_strsave((char_u *)text_prop.मूल्य);
		    अन्यथा
			oldicon = vim_strsave((char_u *)text_prop.मूल्य);
		}
	    }
	}
	XFree((शून्य *)text_prop.मूल्य);
    }
    प्रत्यागम retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# यदि defined(X_HAVE_UTF8_STRING)
#  यदि X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    स्थिर शून्य
set_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    यदि (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, (ध्रुव वर्ण *)title,
					     रिक्त, रिक्त, 0, रिक्त, रिक्त, रिक्त);
    अन्यथा
# endif
    {
# यदि XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, (ध्रुव वर्ण *)title,
					     रिक्त, रिक्त, 0, रिक्त, रिक्त, रिक्त);
#  अन्यथा
	XTextProperty	text_prop;
	वर्ण		*c_title = (वर्ण *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(शून्य)XStringListToTextProperty(&c_title, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, &text_prop,
					     रिक्त, रिक्त, 0, रिक्त, रिक्त, रिक्त);
#  endif
# अन्यथा
	XStoreName(x11_display, x11_window, (वर्ण *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    स्थिर शून्य
set_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    यदि (enc_utf8)
	Xutf8SetWMProperties(x11_display, x11_window, रिक्त, (ध्रुव वर्ण *)icon,
						   रिक्त, 0, रिक्त, रिक्त, रिक्त);
    अन्यथा
# endif
    {
# यदि XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	XmbSetWMProperties(x11_display, x11_window, रिक्त, (ध्रुव वर्ण *)icon,
						   रिक्त, 0, रिक्त, रिक्त, रिक्त);
#  अन्यथा
	XTextProperty	text_prop;
	वर्ण		*c_icon = (वर्ण *)icon;

	(शून्य)XStringListToTextProperty(&c_icon, 1, &text_prop);
	XSetWMProperties(x11_display, x11_window, रिक्त, &text_prop,
						   रिक्त, 0, रिक्त, रिक्त, रिक्त);
#  endif
# अन्यथा
	XSetIconName(x11_display, x11_window, (वर्ण *)icon);
# endif
    }
    XFlush(x11_display);
}

#अन्यथा  // FEAT_X11

    स्थिर पूर्णाङ्क
get_x11_title(पूर्णाङ्क test_only UNUSED)
{
    प्रत्यागम FALSE;
}

    स्थिर पूर्णाङ्क
get_x11_icon(पूर्णाङ्क test_only)
{
    यदि (!test_only)
    {
	यदि (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	अन्यथा
	    oldicon = vim_strsave(T_NAME);
    }
    प्रत्यागम FALSE;
}

#endif // FEAT_X11

    पूर्णाङ्क
mch_can_restore_title(शून्य)
{
    प्रत्यागम get_x11_title(TRUE);
}

    पूर्णाङ्क
mch_can_restore_icon(शून्य)
{
    प्रत्यागम get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    शून्य
mch_settitle(char_u *title, char_u *icon)
{
    पूर्णाङ्क		type = 0;
    स्थिर पूर्णाङ्क	recursive = 0;

    यदि (T_NAME == रिक्त)	    // no terminal name (yet)
	प्रत्यागम;
    यदि (title == रिक्त && icon == रिक्त)	    // nothing to do
	प्रत्यागम;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    यदि (recursive)
	प्रत्यागम;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ifdef FEAT_X11
    यदि (get_x11_windis() == OK)
	type = 1;
#endif
#यदि defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    यदि (gui.in_use)
	type = 1;
#endif

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    यदि ((type || *T_TS != NUL) && title != रिक्त)
    {
	यदि (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	यदि (oldtitle == रिक्त
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call but not in GUI, save title
	    (शून्य)get_x11_title(FALSE);

	यदि (*T_TS != NUL)		// it's OK if t_fs is empty
	    term_settitle(title);
#ifdef FEAT_X11
	अन्यथा
# ifdef FEAT_GUI_GTK
	यदि (!gui.in_use)		// don't do this if GTK+ is running
# endif
	    set_x11_title(title);		// x11
#endif
#यदि defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	अन्यथा
	    gui_mch_settitle(title, icon);
#endif
	unix_did_set_title = TRUE;
    }

    यदि ((type || *T_CIS != NUL) && icon != रिक्त)
    {
	यदि (oldicon == रिक्त
#ifdef FEAT_GUI
		&& !gui.in_use
#endif
		)		// first call, save icon
	    get_x11_icon(FALSE);

	यदि (*T_CIS != NUL)
	{
	    out_str(T_CIS);			// set icon start
	    out_str_nf(icon);
	    out_str(T_CIE);			// set icon end
	    out_flush();
	}
#ifdef FEAT_X11
	अन्यथा
# ifdef FEAT_GUI_GTK
	यदि (!gui.in_use)		// don't do this if GTK+ is running
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
    शून्य
mch_restore_title(पूर्णाङ्क which)
{
    पूर्णाङ्क	do_धकेलो_निकालो = unix_did_set_title || did_set_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_set_title)
			? (oldtitle ? oldtitle : p_titleold) : रिक्त;
    char_u *tofree = रिक्त;
    यदि (title == oldtitle && oldtitle != रिक्त)
    {
	tofree = vim_strsave(title);
	यदि (tofree != रिक्त)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_set_icon) ? oldicon : रिक्त);
    vim_free(tofree);

    यदि (do_धकेलो_निकालो)
    {
	// pop and push from/to the stack
	term_निकालो_title(which);
	term_धकेलो_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    पूर्णाङ्क
vim_is_xterm(char_u *name)
{
    यदि (name == रिक्त)
	प्रत्यागम FALSE;
    प्रत्यागम ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#यदि defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    पूर्णाङ्क
use_xterm_like_mouse(char_u *name)
{
    प्रत्यागम (name != रिक्त
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
    पूर्णाङ्क
use_xterm_mouse(शून्य)
{
    यदि (ttym_flags == TTYM_SGR)
	प्रत्यागम 4;
    यदि (ttym_flags == TTYM_URXVT)
	प्रत्यागम 3;
    यदि (ttym_flags == TTYM_XTERM2)
	प्रत्यागम 2;
    यदि (ttym_flags == TTYM_XTERM)
	प्रत्यागम 1;
    प्रत्यागम 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    पूर्णाङ्क
vim_is_iris(char_u *name)
{
    यदि (name == रिक्त)
	प्रत्यागम FALSE;
    प्रत्यागम (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#यदि defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    पूर्णाङ्क
vim_is_vt300(char_u *name)
{
    यदि (name == रिक्त)
	प्रत्यागम FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    प्रत्यागम ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != रिक्त)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#endif

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    पूर्णाङ्क
mch_get_user_name(char_u *s, पूर्णाङ्क len)
{
#ifdef VMS
    vim_strncpy(s, (char_u *)cuserid(रिक्त), len - 1);
    प्रत्यागम OK;
#अन्यथा
    प्रत्यागम mch_get_uname(getuid(), s, len);
#endif
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    पूर्णाङ्क
mch_get_uname(uid_t uid, char_u *s, पूर्णाङ्क len)
{
#यदि defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    संरचना passwd   *pw;

    यदि ((pw = getpwuid(uid)) != रिक्त
	    && pw->pw_name != रिक्त && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	प्रत्यागम OK;
    }
#endif
    प्र_लिखो((वर्ण *)s, "%d", (पूर्णाङ्क)uid);	    // assumes s is long enough
    प्रत्यागम FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ifdef HAVE_SYS_UTSNAME_H
    शून्य
mch_get_host_name(char_u *s, पूर्णाङ्क len)
{
    संरचना utsname vutsname;

    यदि (uname(&vutsname) < 0)
	*s = NUL;
    अन्यथा
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#अन्यथा // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    शून्य
mch_get_host_name(char_u *s, पूर्णाङ्क len)
{
# ifdef VAXC
    vaxc$gethostname((वर्ण *)s, len);
# अन्यथा
    gethostname((वर्ण *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#endif // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    दीर्घ
mch_get_pid(शून्य)
{
    प्रत्यागम (दीर्घ)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    पूर्णाङ्क
mch_process_running(दीर्घ pid)
{
    // If there is no error the process must be running.
    यदि (समाप्त(pid, 0) == 0)
	प्रत्यागम TRUE;
#ifdef ESRCH
    // If the error is ESRCH then the process is not running.
    यदि (त्रुटि_सं == ESRCH)
	प्रत्यागम FALSE;
#endif
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    प्रत्यागम TRUE;
}

#यदि !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    स्थिर वर्ण *
म_त्रुटि(पूर्णाङ्क err)
{
    बाह्य पूर्णाङ्क	    sys_nerr;
    बाह्य वर्ण	    *sys_errlist[];
    स्थिर वर्ण	    er[20];

    यदि (err > 0 && err < sys_nerr)
	प्रत्यागम (sys_errlist[err]);
    snprintf(er, परिमाण(er), "Error %d", err);
    प्रत्यागम er;
}
#endif

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    पूर्णाङ्क
mch_dirname(char_u *buf, पूर्णाङ्क len)
{
#यदि defined(USE_GETCWD)
    यदि (getcwd((वर्ण *)buf, len) == रिक्त)
    {
	STRCPY(buf, म_त्रुटि(त्रुटि_सं));
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
#अन्यथा
    प्रत्यागम (getwd((वर्ण *)buf) != रिक्त ? OK : FAIL);
#endif
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    पूर्णाङ्क
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    पूर्णाङ्क		len,
    पूर्णाङ्क		force)		// also expand when already absolute path
{
    पूर्णाङ्क		buflen = 0;
#ifdef HAVE_FCHDIR
    पूर्णाङ्क		fd = -1;
    स्थिर पूर्णाङ्क	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#endif
    char_u	olddir[MAXPATHL];
    char_u	*p;
    पूर्णाङ्क		retval = OK;
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
# यदि CYGWIN_VERSION_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_path(CCP_WIN_A_TO_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# अन्यथा
    cygwin_conv_to_posix_path(fname, posix_fname);
# endif
    fname = posix_fname;
#endif

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    यदि ((force || !mch_isFullName(fname))
	    && ((p = vim_strrchr(fname, '/')) == रिक्त || p != fname))
    {
	यदि (p == रिक्त && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	यदि (p != रिक्त)
	{
	    यदि (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ifdef HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    यदि (!dont_fchdir)
	    {
		fd = खोलो(".", O_RDONLY | O_EXTRA, 0);
		यदि (fd >= 0 && fchdir(fd) < 0)
		{
		    बंद(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#endif

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    यदि (
#ifdef HAVE_FCHDIR
		fd < 0 &&
#endif
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((वर्ण *)olddir) != 0))
	    {
		p = रिक्त;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    अन्यथा
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		यदि (p - fname >= len)
		    retval = FAIL;
		अन्यथा
		{
		    vim_strncpy(buf, fname, p - fname);
		    यदि (mch_chdir((वर्ण *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			यदि (mch_isFullName(fname))
			    retval = FAIL;
			अन्यथा
			    p = रिक्त;
		    }
		    अन्यथा यदि (*p == '/')
			fname = p + 1;
		    अन्यथा
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	यदि (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	यदि (p != रिक्त)
	{
	    पूर्णाङ्क	l;

#ifdef HAVE_FCHDIR
	    यदि (fd >= 0)
	    {
		यदि (p_verbose >= 5)
		{
		    verbose_enter();
		    संदेश("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    अन्यथा
#endif
		l = mch_chdir((वर्ण *)olddir);
	    यदि (l != 0)
		emsg(_(e_cannot_go_back_to_previous_directory));
	}
#ifdef HAVE_FCHDIR
	यदि (fd >= 0)
	    बंद(fd);
#endif

	buflen = (पूर्णाङ्क)STRLEN(buf);
	यदि (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ifndef VMS
	अन्यथा यदि (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += परिमाण(PATHSEP);
	}
#endif
    }

    यदि (buflen == 0)
	buflen = (पूर्णाङ्क)STRLEN(buf);

    // Catch file names which are too long.
    यदि (retval == FAIL || (पूर्णाङ्क)(buflen + STRLEN(fname)) >= len)
	प्रत्यागम FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    यदि (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    प्रत्यागम OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    पूर्णाङ्क
mch_isFullName(char_u *fname)
{
#ifdef VMS
    प्रत्यागम ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     म_अक्षर((वर्ण *)fname,':') || म_अक्षर((वर्ण *)fname,'"') ||
	    (म_अक्षर((वर्ण *)fname,'[') && म_अक्षर((वर्ण *)fname,']'))||
	    (म_अक्षर((वर्ण *)fname,'<') && म_अक्षर((वर्ण *)fname,'>'))   );
#अन्यथा
    प्रत्यागम (*fname == '/' || *fname == '~');
#endif
}

#यदि defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    शून्य
fname_case(
    char_u	*name,
    पूर्णाङ्क		len UNUSED)  // buffer size, only used when name gets longer
{
    संरचना stat st;
    char_u	*slash, *tail;
    माप_प्रकार	taillen;
    सूची		*dirp;
    संरचना dirent *dp;

    यदि (mch_lstat((वर्ण *)name, &st) < 0)
	प्रत्यागम;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    यदि (slash == रिक्त)
    {
	dirp = सूची_खोलो(".");
	tail = name;
    }
    अन्यथा
    {
	*slash = NUL;
	dirp = सूची_खोलो((वर्ण *)name);
	*slash = '/';
	tail = slash + 1;
    }

    यदि (dirp == रिक्त)
	प्रत्यागम;

    taillen = STRLEN(tail);
    यावत् ((dp = सूची_पढ़ो(dirp)) != रिक्त)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	यदि (STRICMP(tail, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    संरचना stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (tail - name), (char_u *)dp->d_name,
		    MAXPATHL - (tail - name));
	    यदि (mch_lstat((वर्ण *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(tail, dp->d_name);
		भङ्ग;
	    }
	}
    }

    बंद_सूची(dirp);
}
#endif

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    दीर्घ
mch_getperm(char_u *name)
{
    संरचना stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ifdef VMS
    यदि (stat((वर्ण *)vms_fixfilename(name), &statb))
#अन्यथा
    यदि (stat((वर्ण *)name, &statb))
#endif
	प्रत्यागम -1;
#ifdef __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    प्रत्यागम statb.st_mode & ~S_ADDACE;
#अन्यथा
    प्रत्यागम statb.st_mode;
#endif
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
mch_setperm(char_u *name, दीर्घ perm)
{
    प्रत्यागम (chmod((वर्ण *)
#ifdef VMS
		    vms_fixfilename(name),
#अन्यथा
		    name,
#endif
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#यदि defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    पूर्णाङ्क
mch_fsetperm(पूर्णाङ्क fd, दीर्घ perm)
{
    प्रत्यागम (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#endif

#यदि defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
प्रकारनिर्देश संरचना vim_acl_solaris_T {
    पूर्णाङ्क acl_cnt;
    aclent_t *acl_entry;
} vim_acl_solaris_T;
# endif

# यदि defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    शून्य
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    यदि (from_file == रिक्त)
	प्रत्यागम;

    यदि (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    यदि (selinux_enabled <= 0)
	प्रत्यागम;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    वर्ण *from_context = रिक्त;
    वर्ण *to_context = रिक्त;

    यदि (getfilecon((वर्ण *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	यदि (त्रुटि_सं == EOPNOTSUPP)
	    प्रत्यागम;

	संदेश_puts(_("\nCould not get security context for "));
	संदेश_outtrans(from_file);
	संदेश_putchar('\n');
	प्रत्यागम;
    }
    यदि (getfilecon((वर्ण *)to_file, &to_context) < 0)
    {
	संदेश_puts(_("\nCould not get security context for "));
	संदेश_outtrans(to_file);
	संदेश_putchar('\n');
	freecon (from_context);
	प्रत्यागम ;
    }
    यदि (म_भेद(from_context, to_context) != 0)
    {
	यदि (setfilecon((वर्ण *)to_file, from_context) < 0)
	{
	    संदेश_puts(_("\nCould not set security context for "));
	    संदेश_outtrans(to_file);
	    संदेश_putchar('\n');
	}
    }
    freecon(to_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# यदि defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    शून्य
mch_copy_sec(char_u *from_file, char_u *to_file)
{
    स्थिर ध्रुव वर्ण * ध्रुव smack_copied_attributes[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    वर्ण	buffer[SMACK_LABEL_LEN];
    ध्रुव वर्ण	*name;
    पूर्णाङ्क		index;
    पूर्णाङ्क		ret;
    ssize_t	size;

    यदि (from_file == रिक्त)
	प्रत्यागम;

    size = listxattr((वर्ण *)from_file, रिक्त, 0);
    // not supported or no attributes to copy
    यदि (size <= 0)
	प्रत्यागम;

    प्रति (index = 0 ; index < (पूर्णाङ्क)ARRAY_LENGTH(smack_copied_attributes); index++)
    {
	// get the name of the attribute to copy
	name = smack_copied_attributes[index];

	// get the value of the attribute in buffer
	size = getxattr((वर्ण*)from_file, name, buffer, परिमाण(buffer));
	यदि (size >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((वर्ण*)to_file, name, buffer, (माप_प्रकार)size, 0);
	    यदि (ret < 0)
	    {
		vim_snprintf((वर्ण *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, to_file);
		संदेश_outtrans(IObuff);
		संदेश_putchar('\n');
	    }
	}
	अन्यथा
	{
	    // what reason of not having the attribute value?
	    विकल्प (त्रुटि_सं)
	    {
		पक्ष ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    प्रत्यागम; // leave because it isn't useful to continue

		पक्ष दुस्फल:
		स्वभाव:
		    // no enough size OR unexpected error
		     vim_snprintf((वर्ण *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    संदेश_puts((वर्ण *)IObuff);
		    संदेश_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		पक्ष ENODATA:
		    // no attribute of this name
		    ret = removexattr((वर्ण*)to_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    भङ्ग;
	    }
	}
    }
}
# endif // HAVE_SMACK

# यदि defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    शून्य
mch_copy_xattr(char_u *from_file, char_u *to_file)
{
    वर्ण	*xattr_buf;
    ssize_t	size;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    वर्ण	*कुंजी;
    वर्ण	*val = रिक्त;
    वर्ण	*errmsg = रिक्त;

    यदि (from_file == रिक्त)
	प्रत्यागम;

    // get the length of the extended attributes
    size = listxattr((वर्ण *)from_file, रिक्त, 0);
    // not supported or no attributes to copy
    यदि (size <= 0)
	प्रत्यागम;
    xattr_buf = (वर्ण*)alloc(size);
    यदि (xattr_buf == रिक्त)
	प्रत्यागम;
    size = listxattr((वर्ण *)from_file, xattr_buf, size);
    tsize = size;

    त्रुटि_सं = 0;

    प्रति (पूर्णाङ्क round = 0; round < 2; round++)
    {

	कुंजी = xattr_buf;
	यदि (round == 1)
	    size = tsize;

	यावत् (size > 0)
	{
	    vallen = getxattr((वर्ण *)from_file, कुंजी,
		    val, round ? max_vallen : 0);
	    // only set the attribute in the second round
	    यदि (vallen >= 0 && round &&
		setxattr((वर्ण *)to_file, कुंजी, val, vallen, 0) == 0)
		;
	    अन्यथा यदि (त्रुटि_सं)
	    {
		विकल्प (त्रुटि_सं)
		{
		    पक्ष E2BIG:
			errmsg = e_xattr_e2big;
			गच्छ error_exit;
		    पक्ष ENOTSUP:
		    पक्ष EACCES:
		    पक्ष EPERM:
			भङ्ग;
		    पक्ष दुस्फल:
			errmsg = e_xattr_erange;
			गच्छ error_exit;
		    स्वभाव:
			errmsg = e_xattr_other;
			गच्छ error_exit;
		}
	    }

	    यदि (round == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(कुंजी) + 1;
	    size -= keylen;
	    कुंजी += keylen;
	}
	यदि (round)
	    भङ्ग;

	val = (वर्ण*)alloc(max_vallen + 1);
	यदि (val == रिक्त)
	    गच्छ error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    यदि (errmsg != रिक्त)
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
    vim_acl_T	ret = रिक्त;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((वर्ण *)fname, ACL_TYPE_ACCESS);
# अन्यथा
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    यदि (acl_get((वर्ण *)fname, 0, &aclent) < 0)
	प्रत्यागम रिक्त;
    ret = (vim_acl_T)aclent;
#  अन्यथा
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = दो_स्मृति(परिमाण(vim_acl_solaris_T));
    यदि (aclent == रिक्त)
	प्रत्यागम रिक्त;
    यदि ((aclent->acl_cnt = acl((वर्ण *)fname, GETACLCNT, 0, रिक्त)) < 0)
    {
	मुक्त(aclent);
	प्रत्यागम रिक्त;
    }
    aclent->acl_entry = दो_स्मृति(aclent->acl_cnt * परिमाण(aclent_t));
    यदि (aclent->acl_entry == रिक्त)
    {
	मुक्त(aclent);
	प्रत्यागम रिक्त;
    }
    यदि (acl((वर्ण *)fname, GETACL, aclent->acl_cnt, aclent->acl_entry) < 0)
    {
	मुक्त(aclent->acl_entry);
	मुक्त(aclent);
	प्रत्यागम रिक्त;
    }
    ret = (vim_acl_T)aclent;
#   अन्यथा
#    यदि defined(HAVE_AIX_ACL)
    पूर्णाङ्क		aclsize;
    संरचना acl *aclent;

    aclsize = परिमाण(संरचना acl);
    aclent = दो_स्मृति(aclsize);
    यदि (aclent == रिक्त)
	प्रत्यागम रिक्त;
    यदि (statacl((वर्ण *)fname, STX_NORMAL, aclent, aclsize) < 0)
    {
	यदि (त्रुटि_सं == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = पुनः_स्मृति(aclent, aclsize);
	    यदि (aclent == रिक्त || statacl((वर्ण *)fname, STX_NORMAL, aclent, aclsize) < 0)
	    {
		मुक्त(aclent);
		प्रत्यागम रिक्त;
	    }
	}
	अन्यथा
	{
	    मुक्त(aclent);
	    प्रत्यागम रिक्त;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    प्रत्यागम ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    शून्य
mch_set_acl(char_u *fname UNUSED, vim_acl_T aclent)
{
    यदि (aclent == रिक्त)
	प्रत्यागम;
# ifdef HAVE_POSIX_ACL
    acl_set_file((वर्ण *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# अन्यथा
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_set((वर्ण *)fname, (acl_t *)aclent);
#  अन्यथा
#   ifdef HAVE_SOLARIS_ACL
    acl((वर्ण *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_entry);
#   अन्यथा
#    ifdef HAVE_AIX_ACL
    chacl((वर्ण *)fname, aclent, ((संरचना acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    शून्य
mch_free_acl(vim_acl_T aclent)
{
    यदि (aclent == रिक्त)
	प्रत्यागम;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# अन्यथा
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  अन्यथा
#   ifdef HAVE_SOLARIS_ACL
    मुक्त(((vim_acl_solaris_T *)aclent)->acl_entry);
    मुक्त(aclent);
#   अन्यथा
#    ifdef HAVE_AIX_ACL
    मुक्त(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#endif

/*
 * Set hidden flag for "name".
 */
    शून्य
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    पूर्णाङ्क
mch_isdir(char_u *name)
{
    संरचना stat statb;

    यदि (*name == NUL)	    // Some stat()s don't flag "" as an error.
	प्रत्यागम FALSE;
    यदि (stat((वर्ण *)name, &statb))
	प्रत्यागम FALSE;
    प्रत्यागम (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    पूर्णाङ्क
mch_isrealdir(char_u *name)
{
    संरचना stat statb;

    यदि (*name == NUL)	    // Some stat()s don't flag "" as an error.
	प्रत्यागम FALSE;
    यदि (mch_lstat((वर्ण *)name, &statb))
	प्रत्यागम FALSE;
    प्रत्यागम (S_ISDIR(statb.st_mode) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    स्थिर पूर्णाङ्क
executable_file(char_u *name)
{
    संरचना stat	st;

    यदि (stat((वर्ण *)name, &st))
	प्रत्यागम 0;
#ifdef VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    पूर्णाङ्क vms_executable = 0;
    यदि (S_ISREG(st.st_mode) && mch_access((वर्ण *)name, X_OK) == 0)
    {
	यदि (म_माला(vms_tolower((वर्ण*)name),".exe") != रिक्त
		|| म_माला(vms_tolower((वर्ण*)name),".com")!= रिक्त)
	    vms_executable = 1;
    }
    प्रत्यागम vms_executable;
#अन्यथा
    प्रत्यागम S_ISREG(st.st_mode) && mch_access((वर्ण *)name, X_OK) == 0;
#endif
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    पूर्णाङ्क
mch_can_exe(char_u *name, char_u **path, पूर्णाङ्क use_path)
{
    char_u	*buf;
    माप_प्रकार	bufsize;
    माप_प्रकार	buflen;
    char_u	*p, *e;
    char_u	*p_end;
    माप_प्रकार	elen;
    पूर्णाङ्क		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    यदि (!use_path || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	यदि ((use_path || gettail(name) != name) && executable_file(name))
	{
	    यदि (path != रिक्त)
	    {
		यदि (name[0] != '/')
		    *path = FullName_save(name, TRUE);
		अन्यथा
		    *path = vim_strsave(name);
	    }
	    प्रत्यागम TRUE;
	}
	प्रत्यागम FALSE;
    }

    p = (char_u *)दो_पर्या("PATH");
    यदि (p == रिक्त || *p == NUL)
	प्रत्यागम -1;
    p_end = p + STRLEN(p);
    bufsize = STRLEN(name) + (माप_प्रकार)(p_end - p) + 2;
    buf = alloc(bufsize);
    यदि (buf == रिक्त)
	प्रत्यागम -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    प्रति (;;)
    {
	e = (char_u *)म_अक्षर((वर्ण *)p, ':');
	यदि (e == रिक्त)
	    e = p_end;
	elen = (माप_प्रकार)(e - p);
	यदि (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((वर्ण *)buf, bufsize, "%.*s%s%s",
		(पूर्णाङ्क)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	यदि (retval == 1)
	{
	    यदि (path != रिक्त)
	    {
		यदि (buf[0] != '/')
		    *path = FullName_save(buf, TRUE);
		अन्यथा
		    *path = vim_strnsave(buf, buflen);
	    }
	    भङ्ग;
	}

	यदि (*e != ':')
	    भङ्ग;
	p = e + 1;
    }

    vim_free(buf);
    प्रत्यागम retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    पूर्णाङ्क
mch_nodetype(char_u *name)
{
    संरचना stat	st;

    यदि (stat((वर्ण *)name, &st))
	प्रत्यागम गाँठ_NORMAL;
    यदि (S_ISREG(st.st_mode) || S_ISDIR(st.st_mode))
	प्रत्यागम गाँठ_NORMAL;
    यदि (S_ISBLK(st.st_mode))	// block device isn't writable
	प्रत्यागम गाँठ_OTHER;
    // Everything else is writable?
    प्रत्यागम गाँठ_WRITABLE;
}

    शून्य
mch_early_init(शून्य)
{
#ifdef HAVE_CHECK_ढेर_GROWTH
    पूर्णाङ्क			i;

    check_ढेर_growth((वर्ण *)&i);

# ifdef HAVE_ढेर_LIMIT
    get_ढेर_limit();
# endif

#endif

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#यदि defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ढेर = alloc(get_signal_ढेर_size());
    init_signal_ढेर();
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

#यदि defined(EXITFREE)
    शून्य
mch_free_mem(शून्य)
{
# यदि defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    यदि (clip_star.owned)
	clip_lose_selection(&clip_star);
    यदि (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# यदि defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    यदि (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_VERSION
    // Lesstif crashes here, lose some memory
    यदि (xterm_dpy != रिक्त)
	XtCloseDisplay(xterm_dpy);
    यदि (app_context != (XtAppContext)रिक्त)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = रिक्त; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# यदि defined(FEAT_X11)
    यदि (x11_display != रिक्त
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# यदि defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ढेर);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#endif

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    स्थिर शून्य
exit_scroll(शून्य)
{
    यदि (silent_mode)
	प्रत्यागम;
    यदि (newline_on_exit || संदेश_didout)
    {
	यदि (संदेश_use_printf())
	{
	    यदि (info_message)
		mch_संदेश("\n");
	    अन्यथा
		mch_errmsg("\r\n");
	}
	अन्यथा
	    out_char('\n');
    }
    अन्यथा यदि (!is_not_a_term())
    {
	restore_cterm_colors();		// get original colors back
	संदेश_clr_eos_force();		// clear the rest of the display
	windgoto((पूर्णाङ्क)Rows - 1, cmdline_col_off); // may have moved the cursor
    }
}

#ifdef USE_GCOV_FLUSH
# यदि (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
बाह्य शून्य __gcov_dump(शून्य);
बाह्य शून्य __gcov_reset(शून्य);
#  define __gcov_flush() कुरु { __gcov_dump(); __gcov_reset(); } यावत् (0)
# अन्यथा
बाह्य शून्य __gcov_flush(शून्य);
# endif
#endif

    शून्य
mch_exit(पूर्णाङ्क r)
{
    exiting = TRUE;

#यदि defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#endif

#ifdef FEAT_GUI
    यदि (!gui.in_use)
#endif
    {
	settmode(TMODE_COOK);
	यदि (!is_not_a_term())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    term_निकालो_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	यदि (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	यदि (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	यदि (full_screen)
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
    यदि (gui.in_use)
	gui_exit(r);
#endif

#ifdef MACOS_CONVERT
    mac_conv_cleanup();
#endif

#ifdef __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    यदि (deadly_signal != 0)
	प्रत्यागम;
#endif

#ifdef FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#endif

#ifdef EXITFREE
    free_all_mem();
#endif

    निकास(r);
}

    स्थिर शून्य
may_core_dump(शून्य)
{
    यदि (deadly_signal != 0)
    {
	mch_signal(deadly_signal, संक_पूर्व);
	समाप्त(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ifndef VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    स्थिर पूर्णाङ्क
get_tty_fd(पूर्णाङ्क fd)
{
    पूर्णाङ्क		tty_fd = fd;

# यदि defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    यदि (mch_isatty(fd) > 1)
    {
	वर्ण *name;

	name = ptsname(fd);
	यदि (name == रिक्त)
	    प्रत्यागम -1;

	tty_fd = खोलो(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	यदि (tty_fd < 0)
	    प्रत्यागम -1;
    }
# endif
    प्रत्यागम tty_fd;
}

    स्थिर पूर्णाङ्क
mch_tcgetattr(पूर्णाङ्क fd, शून्य *term)
{
    पूर्णाङ्क		tty_fd;
    पूर्णाङ्क		retval = -1;

    tty_fd = get_tty_fd(fd);
    यदि (tty_fd < 0)
	प्रत्यागम -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (संरचना termios *)term);
#  अन्यथा
    retval = ioctl(tty_fd, TCGETA, (संरचना termio *)term);
#  endif
# अन्यथा
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (संरचना sgttyb *)term);
# endif
    यदि (tty_fd != fd)
	बंद(tty_fd);
    प्रत्यागम retval;
}

    शून्य
mch_settmode(tmode_T tmode)
{
    स्थिर पूर्णाङ्क first = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    स्थिर संरचना termios told;
	   संरचना termios tnew;
#  अन्यथा
    स्थिर संरचना termio told;
	   संरचना termio tnew;
#  endif

    यदि (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    यदि (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == रिक्त || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  यदि defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   अन्यथा
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    अन्यथा
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    अन्यथा यदि (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  यदि defined(HAVE_TERMIOS_H)
    {
	पूर्णाङ्क	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	यावत् (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && त्रुटि_सं == EINTR && n > 0)
	    --n;
    }
#  अन्यथा
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# अन्यथा
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    स्थिर संरचना sgttyb ttybold;
	   संरचना sgttyb ttybnew;

    यदि (first)
    {
	first = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    यदि (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    अन्यथा यदि (tmode == TMODE_SLEEP)
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
    शून्य
get_stty(शून्य)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    यदि (get_tty_info(read_cmd_fd, &info) != OK)
	प्रत्यागम;

    intr_char = info.interrupt;
    buf[0] = info.backspace;
    buf[1] = NUL;
    add_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = find_termcode((char_u *)"kD");
    यदि (p != रिक्त && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(रिक्त);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    पूर्णाङ्क
get_tty_info(पूर्णाङ्क fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    संरचना termios कुंजियाँ;
#  अन्यथा
    संरचना termio कुंजियाँ;
#  endif

    यदि (mch_tcgetattr(fd, &कुंजियाँ) != -1)
    {
	info->backspace = कुंजियाँ.c_cc[VERASE];
	info->interrupt = कुंजियाँ.c_cc[VINTR];
	यदि (कुंजियाँ.c_iflag & ICRNL)
	    info->enter = NL;
	अन्यथा
	    info->enter = CAR;
	यदि (कुंजियाँ.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	अन्यथा
	    info->nl_does_cr = FALSE;
	प्रत्यागम OK;
    }
# अन्यथा
    // for "old" tty systems
    संरचना sgttyb कुंजियाँ;

    यदि (mch_tcgetattr(fd, &कुंजियाँ) != -1)
    {
	info->backspace = कुंजियाँ.sg_erase;
	info->interrupt = कुंजियाँ.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	प्रत्यागम OK;
    }
# endif
    प्रत्यागम FAIL;
}

#endif // VMS

स्थिर पूर्णाङ्क	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    शून्य
mch_setmouse(पूर्णाङ्क on)
{
#ifdef FEAT_BEVAL_TERM
    स्थिर पूर्णाङ्क	bevalterm_ison = FALSE;
#endif
    पूर्णाङ्क		xterm_mouse_vers;

#यदि defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    यदि (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#endif

    यदि (on == mouse_ison
#ifdef FEAT_BEVAL_TERM
	    && p_bevalterm == bevalterm_ison
#endif
	    )
	// return quickly if nothing to do
	प्रत्यागम;

    xterm_mouse_vers = use_xterm_mouse();

#ifdef FEAT_MOUSE_URXVT
    यदि (ttym_flags == TTYM_URXVT)
    {
	out_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#endif

    यदि (T_CXM != रिक्त && *T_CXM != NUL)
    {
	term_enable_mouse(on);
    }
    अन्यथा यदि (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	out_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ifdef FEAT_BEVAL_TERM
    यदि (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	यदि (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    out_str_nf((char_u *)("\033[?1003l"));
    }
#endif

    यदि (xterm_mouse_vers > 0)
    {
	यदि (on)	// enable mouse events, use mouse tracking if available
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ifdef FEAT_BEVAL_TERM
			    bevalterm_ison ? "\033[?1003h" :
#endif
			      "\033[?1002h")
			: "\033[?1000h"));
	अन्यथा	// disable mouse events, could probably always send the same
	    out_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ifdef FEAT_MOUSE_DEC
    अन्यथा यदि (ttym_flags == TTYM_DEC)
    {
	यदि (on)	// enable mouse events
	    out_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	अन्यथा	// disable mouse events
	    out_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#endif

#ifdef FEAT_MOUSE_GPM
    अन्यथा
    {
	यदि (on)
	{
	    यदि (gpm_open())
		mouse_ison = TRUE;
	}
	अन्यथा
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_SYSMOUSE
    अन्यथा
    {
	यदि (on)
	{
	    यदि (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	अन्यथा
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#endif

#ifdef FEAT_MOUSE_JSB
    अन्यथा
    {
	यदि (on)
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
# अन्यथा
	    out_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	अन्यथा
	{
	    out_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#endif
#ifdef FEAT_MOUSE_PTERM
    अन्यथा
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	यदि (on)
	    out_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	अन्यथा
	    out_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#endif
}

#यदि defined(FEAT_BEVAL_TERM)
/*
 * Called when 'balloonevalterm' changed.
 */
    शून्य
mch_bevalterm_changed(शून्य)
{
    mch_setmouse(mouse_ison);
}
#endif

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    शून्य
check_mouse_termcode(शून्य)
{
#ifdef FEAT_MOUSE_XTERM
    यदि (use_xterm_mouse()
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
	यदि (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    अन्यथा
	del_mouse_termcode(KS_MOUSE);
#endif

#ifdef FEAT_MOUSE_GPM
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    अन्यथा
	del_mouse_termcode(KS_GPM_MOUSE);
#endif

#ifdef FEAT_SYSMOUSE
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#endif

#ifdef FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    अन्यथा
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    अन्यथा
	del_mouse_termcode(KS_NETTERM_MOUSE);
#endif

#ifdef FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_DEC_MOUSE, (char_u *)(term_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    अन्यथा
	del_mouse_termcode(KS_DEC_MOUSE);
#endif
#ifdef FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    यदि (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
	set_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    अन्यथा
	del_mouse_termcode(KS_PTERM_MOUSE);
#endif
#ifdef FEAT_MOUSE_URXVT
    यदि (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.in_use
# endif
	    )
    {
	set_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	यदि (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    अन्यथा
	del_mouse_termcode(KS_URXVT_MOUSE);
#endif
    यदि (use_xterm_mouse() == 4
#ifdef FEAT_GUI
	    && !gui.in_use
#endif
	    )
    {
	set_mouse_termcode(KS_SGR_MOUSE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	set_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(term_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	यदि (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    अन्यथा
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
    पूर्णाङ्क
mch_get_shellsize(शून्य)
{
    दीर्घ	rows = 0;
    दीर्घ	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	संरचना winsize	ws;
	पूर्णाङ्क fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	यदि (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	यदि (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_col;
	    rows = ws.ws_row;
#  ifdef FEAT_EVAL
	    ch_log(रिक्त, "Got size with TIOCGWINSZ: %ld x %ld", columns, rows);
#  endif
	}
    }
# अन्यथा // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	संरचना ttysize	ts;
	पूर्णाङ्क fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	यदि (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	यदि (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_cols;
	    rows = ts.ts_पंक्तियाँ;
#   ifdef FEAT_EVAL
	    ch_log(रिक्त, "Got size with TIOCGSIZE: %ld x %ld", columns, rows);
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
    यदि (columns == 0 || rows == 0 || vim_strchr(p_cpo, CPO_TSIZE) != रिक्त)
    {
	यदि ((p = (char_u *)दो_पर्या("LINES")))
	{
	    rows = म_से_प((वर्ण *)p);
# ifdef FEAT_EVAL
	    ch_log(रिक्त, "Got 'lines' from $LINES: %ld", rows);
# endif
	}
	यदि ((p = (char_u *)दो_पर्या("COLUMNS")))
	{
	    columns = म_से_प((वर्ण *)p);
# ifdef FEAT_EVAL
	    ch_log(रिक्त, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    यदि (columns == 0 || rows == 0)
    {
	getlinecol(&columns, &rows);
#  ifdef FEAT_EVAL
	ch_log(रिक्त, "Got size from termcap: %ld x %ld", columns, rows);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    यदि (columns <= 0 || rows <= 0)
	प्रत्यागम FAIL;

    Rows = rows;
    Columns = columns;
    limit_screen_size();
    प्रत्यागम OK;
}

# यदि defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    शून्य
mch_calc_cell_size(संरचना cellsize *cs_out)
{
   संरचना winsize   ws;
   पूर्णाङ्क		    retval;
   पूर्णाङ्क		    x_cell_size;
   पूर्णाङ्क		    y_cell_size;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(रिक्त, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   यदि (retval == -1 || ws.ws_col == 0 || ws.ws_row == 0)
   {
	cs_out->cs_xpixel = -1;
	cs_out->cs_ypixel = -1;
	प्रत्यागम;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_size = ws.ws_xpixel / ws.ws_col;
   y_cell_size = ws.ws_ypixel / ws.ws_row;

   cs_out->cs_xpixel = x_cell_size > 0 ? x_cell_size : -1;
   cs_out->cs_ypixel = y_cell_size > 0 ? y_cell_size : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   यदि (x_cell_size <= 0 || y_cell_size <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       OUT_STR("\033[14t");
       OUT_STR("\033[16t");
       out_flush();
   }
}
# endif

# यदि defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    पूर्णाङ्क
mch_report_winsize(पूर्णाङ्क fd, पूर्णाङ्क rows, पूर्णाङ्क cols)
{
    पूर्णाङ्क		tty_fd;
    पूर्णाङ्क		retval = -1;

    tty_fd = get_tty_fd(fd);
    यदि (tty_fd < 0)
	प्रत्यागम FAIL;

#  यदि defined(TIOCSWINSZ)
    संरचना winsize ws;

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
    ch_log(रिक्त, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    संरचना ttysize ts;

    ts.ts_cols = cols;
    ts.ts_पंक्तियाँ = rows;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(रिक्त, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    यदि (tty_fd != fd)
	बंद(tty_fd);
    प्रत्यागम retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    शून्य
mch_set_shellsize(शून्य)
{
    यदि (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	term_set_winsize((पूर्णाङ्क)Rows, (पूर्णाङ्क)Columns);
	out_flush();
	screen_start();			// don't know where cursor is now
    }
}

#endif // VMS

/*
 * Rows and/or Columns has changed.
 */
    शून्य
mch_new_shellsize(शून्य)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    स्थिर pid_t
wait4pid(pid_t child, waitstatus *status)
{
    pid_t wait_pid = 0;
    दीर्घ delay_msec = 1;

    यावत् (wait_pid != child)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(child, status, WNOHANG);
	यदि (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    यदि (++delay_msec > 10)
		delay_msec = 10;
	    अनुवर्त;
	}
	यदि (wait_pid <= 0
#ifdef ECHILD
		&& त्रुटि_सं == ECHILD
#endif
	   )
	    भङ्ग;
    }
    प्रत्यागम wait_pid;
}

#यदि !defined(USE_SYSTEM) || defined(FEAT_JOB_CHANNEL)
/*
 * Set the environment for a child process.
 */
    स्थिर शून्य
set_child_environment(
	दीर्घ	rows,
	दीर्घ	columns,
	वर्ण	*term,
	पूर्णाङ्क	is_terminal UNUSED)
{
# ifdef HAVE_SETENV
    वर्ण	envbuf[50];
# अन्यथा
    स्थिर वर्ण	envbuf_Term[30];
    स्थिर वर्ण	envbuf_Rows[20];
    स्थिर वर्ण	envbuf_पंक्तियाँ[20];
    स्थिर वर्ण	envbuf_Columns[20];
    स्थिर वर्ण	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    स्थिर वर्ण	envbuf_Version[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    स्थिर वर्ण	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", term, 1);
    प्र_लिखो((वर्ण *)envbuf, "%ld", rows);
    setenv("ROWS", (वर्ण *)envbuf, 1);
    प्र_लिखो((वर्ण *)envbuf, "%ld", rows);
    setenv("LINES", (वर्ण *)envbuf, 1);
    प्र_लिखो((वर्ण *)envbuf, "%ld", columns);
    setenv("COLUMNS", (वर्ण *)envbuf, 1);
    प्र_लिखो((वर्ण *)envbuf, "%d", t_colors);
    setenv("COLORS", (वर्ण *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    यदि (is_terminal)
    {
	प्र_लिखो((वर्ण *)envbuf, "%ld",  (दीर्घ)get_vim_var_nr(VV_VERSION));
	setenv("VIM_TERMINAL", (वर्ण *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == रिक्त ? "" : (वर्ण *)serverName, 1);
#  endif
# अन्यथा
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_Term, परिमाण(envbuf_Term), "TERM=%s", term);
    putenv(envbuf_Term);
    vim_snprintf(envbuf_Rows, परिमाण(envbuf_Rows), "ROWS=%ld", rows);
    putenv(envbuf_Rows);
    vim_snprintf(envbuf_पंक्तियाँ, परिमाण(envbuf_पंक्तियाँ), "LINES=%ld", rows);
    putenv(envbuf_पंक्तियाँ);
    vim_snprintf(envbuf_Columns, परिमाण(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, परिमाण(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    यदि (is_terminal)
    {
	vim_snprintf(envbuf_Version, परिमाण(envbuf_Version),
			 "VIM_TERMINAL=%ld", (दीर्घ)get_vim_var_nr(VV_VERSION));
	putenv(envbuf_Version);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, परिमाण(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == रिक्त ? "" : (वर्ण *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    स्थिर शून्य
set_default_child_environment(पूर्णाङ्क is_terminal)
{
    set_child_environment(Rows, Columns, "dumb", is_terminal);
}
#endif

#यदि defined(FEAT_GUI) || defined(FEAT_JOB_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    स्थिर शून्य
open_pty(पूर्णाङ्क *pty_master_fd, पूर्णाङ्क *pty_slave_fd, char_u **name1, char_u **name2)
{
    वर्ण	*tty_name;

    यदि (name1 != रिक्त)
	*name1 = रिक्त;
    यदि (name2 != रिक्त)
	*name2 = रिक्त;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    यदि (*pty_master_fd < 0)
	प्रत्यागम;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = खोलो(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# अन्यथा
    *pty_slave_fd = खोलो(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    यदि (*pty_slave_fd < 0)
    {
	बंद(*pty_master_fd);
	*pty_master_fd = -1;
    }
    अन्यथा
    {
	यदि (name1 != रिक्त)
	    *name1 = vim_strsave((char_u *)tty_name);
	यदि (name2 != रिक्त)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#endif

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    स्थिर शून्य
may_send_sigint(पूर्णाङ्क c UNUSED, pid_t pid UNUSED, pid_t wpid UNUSED)
{
#ifdef संक_विघ्न
    यदि (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	समाप्त(-pid, संक_विघ्न);
# अन्यथा
	समाप्त(0, संक_विघ्न);
# endif
	यदि (wpid > 0)
	    समाप्त(wpid, संक_विघ्न);
    }
#endif
}

#यदि !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    पूर्णाङ्क
unix_build_argv(
	char_u *cmd,
	वर्ण ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    वर्ण	**argv = रिक्त;
    पूर्णाङ्क		argc;

    *sh_tofree = vim_strsave(p_sh);
    यदि (*sh_tofree == रिक्त)		// out of memory
	प्रत्यागम FAIL;

    यदि (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	प्रत्यागम FAIL;
    *argvp = argv;

    यदि (cmd != रिक्त)
    {
	char_u	*s;
	char_u	*p;

	यदि (extra_shell_arg != रिक्त)
	    argv[argc++] = (वर्ण *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	यदि (*shcf_tofree == रिक्त)    // out of memory
	    प्रत्यागम FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	यावत् (*p != NUL)
	{
	    argv[argc++] = (वर्ण *)s;
	    यावत् (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (वर्ण *)cmd;
    }
    argv[argc] = रिक्त;
    प्रत्यागम OK;
}
#endif

#यदि defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    स्थिर पूर्णाङ्क
mch_call_shell_terminal(
    char_u	*cmd,
    पूर्णाङ्क		options UNUSED)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    वर्ण	**argv = रिक्त;
    char_u	*tofree1 = रिक्त;
    char_u	*tofree2 = रिक्त;
    पूर्णाङ्क		retval = -1;
    buf_T	*buf;
    job_T	*job;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    यदि (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	गच्छ theend;

    init_job_options(&opt);
    ch_log(रिक्त, "starting terminal for system command '%s'", cmd);
    buf = term_start(रिक्त, argv, &opt, TERM_START_SYSTEM);
    यदि (buf == रिक्त)
	गच्छ theend;

    job = term_getjob(buf->b_term);
    ++job->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    यदि (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	यावत् (term_use_loop())
	{
	    यदि (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_active)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		यदि (terminal_loop(TRUE) == OK)
		    normal_cmd(&oa, TRUE);
	    }
	    अन्यथा
		normal_cmd(&oa, TRUE);
	}
	retval = job->jv_exitval;
	ch_log(रिक्त, "system command finished");

	job_unref(job);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    यदि (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_buffer(DOBUF_WIPE, DOBUF_FIRST, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    प्रत्यागम retval;
}
#endif

#ifdef USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    स्थिर पूर्णाङ्क
mch_call_shell_system(
    char_u	*cmd,
    पूर्णाङ्क		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    वर्ण	*ifn = रिक्त;
    वर्ण	*ofn = रिक्त;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    पूर्णाङ्क		x;

    out_flush();

    यदि (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# यदि defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  यदि defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    यदि (cmd == रिक्त)
	x = प्रणाली((वर्ण *)p_sh);
    अन्यथा
    {
# ifdef VMS
	यदि (ofn = म_अक्षर((वर्ण *)cmd, '>'))
	    *ofn++ = '\0';
	यदि (ifn = म_अक्षर((वर्ण *)cmd, '<'))
	{
	    वर्ण *p;

	    *ifn++ = '\0';
	    p = म_अक्षर(ifn,' '); // chop off any trailing spaces
	    यदि (p)
		*p = '\0';
	}
	यदि (ofn)
	    x = vms_sys((वर्ण *)cmd, ofn, ifn);
	अन्यथा
	    x = प्रणाली((वर्ण *)cmd);
# अन्यथा
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == रिक्त ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	यदि (newcmd == रिक्त)
	    x = 0;
	अन्यथा
	{
	    प्र_लिखो((वर्ण *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == रिक्त ? "" : (वर्ण *)extra_shell_arg,
		    (वर्ण *)p_shcf,
		    (वर्ण *)cmd);
	    x = प्रणाली((वर्ण *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_status(x);
# endif
    यदि (emsg_silent)
	;
    अन्यथा यदि (x == 127)
	संदेश_puts(_("\nCannot execute shell sh\n"));
    अन्यथा यदि (x && !(options & SHELL_SILENT))
    {
	संदेश_puts(_("\nshell returned "));
	संदेश_outnum((दीर्घ)x);
	संदेश_putchar('\n');
    }

    यदि (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# यदि defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    प्रत्यागम x;
}

#अन्यथा // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    स्थिर पूर्णाङ्क
mch_call_shell_fork(
    char_u	*cmd,
    पूर्णाङ्क		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    पूर्णाङ्क		status = -1;
    पूर्णाङ्क		retval = -1;
    वर्ण	**argv = रिक्त;
    char_u	*tofree1 = रिक्त;
    char_u	*tofree2 = रिक्त;
    पूर्णाङ्क		i;
    पूर्णाङ्क		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    पूर्णाङ्क		pty_slave_fd = -1;
# endif
    पूर्णाङ्क		fd_toshell[2];		// for pipes
    पूर्णाङ्क		fd_fromshell[2];
    पूर्णाङ्क		pipe_error = FALSE;
    पूर्णाङ्क		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    out_flush();
    यदि (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    यदि (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    यदि (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	गच्छ error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    यदि ((options & (SHELL_READ|SHELL_WRITE))
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
	यदि (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, रिक्त, रिक्त);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	यदि (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    यदि (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		यदि (pipe_error)			    // pipe create failed
		{
		    बंद(fd_toshell[0]);
		    बंद(fd_toshell[1]);
		}
	    }
	    यदि (pipe_error)
	    {
		संदेश_puts(_("\nCannot create pipes\n"));
		out_flush();
	    }
	}
    }

    यदि (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	BLOCK_SIGNALS(&curset);
	pid = विभाजन();	// maybe we should use vfork()
	यदि (pid == -1)
	{
	    UNBLOCK_SIGNALS(&curset);

	    संदेश_puts(_("\nCannot fork\n"));
	    यदि ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.in_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		यदि (pty_master_fd >= 0)		// close the pseudo tty
		{
		    बंद(pty_master_fd);
		    बंद(pty_slave_fd);
		}
		अन्यथा				// close the pipes
# endif
		{
		    बंद(fd_toshell[0]);
		    बंद(fd_toshell[1]);
		    बंद(fd_fromshell[0]);
		    बंद(fd_fromshell[1]);
		}
	    }
	}
	अन्यथा यदि (pid == 0)	// child
	{
	    reset_signals();		// handle signals normally
	    UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	    यदि (ch_log_active())
	    {
		ch_log(रिक्त, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    यदि (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		पूर्णाङ्क fd;

		/*
		 * Don't want to show any message from the shell.  Can't just
		 * close stdout and stderr though, because some systems will
		 * break if you try to write to them after that, so we must
		 * use dup() to replace them with something else -- webb
		 * Connect stdin to /dev/null too, so ":n `cat`" doesn't hang,
		 * waiting for input.
		 */
		fd = खोलो("/dev/null", O_RDWR | O_EXTRA, 0);
		ख_बंद(मानक_निवेश);
		ख_बंद(मानक_निकास);
		ख_बंद(मानक_त्रुटि);

		/*
		 * If any of these open()'s and dup()'s fail, we just continue
		 * anyway.  It's not fatal, and on most systems it will make
		 * no difference at all.  On a few it will cause the execvp()
		 * to exit with a non-zero status even when the completion
		 * could be done, which is nothing too serious.  If the open()
		 * or dup() failed we'd just do the same thing ourselves
		 * anyway -- webb
		 */
		यदि (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    बंद(fd);
		}
	    }
	    अन्यथा यदि ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.in_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		यदि (p_stmp)
		{
		    (शून्य)setsid();
#  यदि defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, संक_छोड़ो);
#  endif
		}
# endif
# ifdef FEAT_GUI
		यदि (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    यदि (options & SHELL_COOKED)
			setup_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (वर्ण *)रिक्त);
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
		यदि (pty_master_fd >= 0)
		{
		    बंद(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    बंद(0);
		    vim_ignored = dup(pty_slave_fd);
		    बंद(1);
		    vim_ignored = dup(pty_slave_fd);
		    यदि (gui.in_use)
		    {
			बंद(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    बंद(pty_slave_fd);    // has been dupped, close it now
		}
		अन्यथा
# endif
		{
		    // set up stdin for the child
		    बंद(fd_toshell[1]);
		    बंद(0);
		    vim_ignored = dup(fd_toshell[0]);
		    बंद(fd_toshell[0]);

		    // set up stdout for the child
		    बंद(fd_fromshell[0]);
		    बंद(1);
		    vim_ignored = dup(fd_fromshell[1]);
		    बंद(fd_fromshell[1]);

# ifdef FEAT_GUI
		    यदि (gui.in_use)
		    {
			// set up stderr for the child
			बंद(2);
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
	अन्यथा			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_signals(संक_छोड़ो, संक_त्रुटि);
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
	    यदि ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.in_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    buffer[BUFLEN + 1];
		पूर्णाङ्क	    buffer_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		पूर्णाङ्क	    ta_len = 0;		// valid bytes in ta_buf[]
		पूर्णाङ्क	    len;
		पूर्णाङ्क	    p_more_save;
		पूर्णाङ्क	    old_State;
		पूर्णाङ्क	    c;
		पूर्णाङ्क	    toshell_fd;
		पूर्णाङ्क	    fromshell_fd;
		garray_T    ga;
		पूर्णाङ्क	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   start_tv;
# endif

# ifdef FEAT_GUI
		यदि (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		अन्यथा
# endif
		{
		    बंद(fd_toshell[0]);
		    बंद(fd_fromshell[1]);
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

		यदि ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    यदि ((wpid = विभाजन()) == -1)
		    {
			संदेश_puts(_("\nCannot fork\n"));
		    }
		    अन्यथा यदि (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_start.lnum;
			माप_प्रकार	    written = 0;
			char_u	    *lp = ml_get(lnum);
			माप_प्रकार	    lplen = (माप_प्रकार)ml_get_len(lnum);

			बंद(fromshell_fd);
			प्रति (;;)
			{
			    यदि (lplen == 0)
				len = 0;
			    अन्यथा यदि (lp[written] == NL)
				// NL -> NUL translation
				len = लिख(toshell_fd, "", (माप_प्रकार)1);
			    अन्यथा
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = लिख(toshell_fd, (वर्ण *)lp + written,
					   s == रिक्त ? lplen - written
					      : (माप_प्रकार)(s - (lp + written)));
			    }
			    यदि (len == (पूर्णाङ्क)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				यदि (lnum != curbuf->b_op_end.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_पंक्ति_गिनती
						    || curbuf->b_p_eol)))
				    vim_ignored = लिख(toshell_fd, "\n",
								   (माप_प्रकार)1);
				++lnum;
				यदि (lnum > curbuf->b_op_end.lnum)
				{
				    // finished all the lines, close pipe
				    बंद(toshell_fd);
				    भङ्ग;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    अन्यथा यदि (len > 0)
				written += (माप_प्रकार)len;
			}
			_exit(0);
		    }
		    अन्यथा // parent
		    {
			बंद(toshell_fd);
			toshell_fd = -1;
		    }
		}

		यदि (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(start_tv);
# endif
		प्रति (;;)
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
		    यदि (!(options & SHELL_EXPAND)
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
		      यदि (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(start_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      यदि (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			यदि (len == 1 && (pty_master_fd < 0 || cmd != रिक्त))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    यदि (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				बंद(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = term_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			यदि (pty_master_fd < 0)
			{
			    प्रति (i = ta_len; i < ta_len + len; ++i)
			    {
				यदि (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    संदेश_putchar(ta_buf[i]);
				अन्यथा यदि (has_mbyte)
				{
				    पूर्णाङ्क l = (*mb_ptr2len)(ta_buf + i);

				    संदेश_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				अन्यथा
				    संदेश_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(संदेश_row, cmdline_col_off + संदेश_col);
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
			यदि (options & SHELL_WRITE)
			    ta_len = 0;
			अन्यथा यदि (toshell_fd >= 0)
			{
			    len = लिख(toshell_fd, (वर्ण *)ta_buf, (माप_प्रकार)1);
			    यदि (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    यदि (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			समाप्त(-pid, संक_विघ्न);
# अन्यथा
			समाप्त(0, संक_विघ्न);
# endif
			यदि (wpid > 0)
			    समाप्त(wpid, संक_विघ्न);
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
		    यावत् (RealWaitForChar(fromshell_fd, 10L, रिक्त, रिक्त))
		    {
			len = read_eintr(fromshell_fd, buffer
				+ buffer_off, (माप_प्रकार)(BUFLEN - buffer_off)
				);
			यदि (len <= 0)		    // end of file or error
			    गच्छ finished;

			noread_cnt = 0;
			यदि (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    प्रति (i = 0; i < len; ++i)
			    {
				यदि (buffer[i] == NL)
				    append_ga_पंक्ति(&ga);
				अन्यथा यदि (buffer[i] == NUL)
				    ga_append(&ga, NL);
				अन्यथा
				    ga_append(&ga, buffer[i]);
			    }
			}
			अन्यथा यदि (has_mbyte)
			{
			    पूर्णाङ्क		l;
			    char_u	*p;

			    len += buffer_off;
			    buffer[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    प्रति (p = buffer; p < buffer + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				यदि (l == 0)
				    l = 1;  // NUL byte?
				अन्यथा यदि (MB_BYTE2LEN(*p) != l)
				    भङ्ग;
			    }
			    यदि (p == buffer)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				यदि (len >= 12)
				    ++p;
				अन्यथा
				{
				    buffer_off = len;
				    अनुवर्त;
				}
			    }
			    c = *p;
			    *p = NUL;
			    संदेश_puts((वर्ण *)buffer);
			    यदि (p < buffer + len)
			    {
				*p = c;
				buffer_off = (buffer + len) - p;
				mch_memmove(buffer, p, buffer_off);
				अनुवर्त;
			    }
			    buffer_off = 0;
			}
			अन्यथा
			{
			    buffer[len] = NUL;
			    संदेश_puts((वर्ण *)buffer);
			}

			windgoto(संदेश_row, cmdline_col_off + संदेश_col);
			cursor_on();
			out_flush();
			यदि (got_int)
			    भङ्ग;

# ifdef ELAPSED_FUNC
			यदि (wait_pid == 0)
			{
			    दीर्घ	msec = ELAPSED_FUNC(start_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    यदि (msec > 2000)
			    {
				noread_cnt = 5;
				भङ्ग;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    यदि (wait_pid == pid)
		    {
			यदि (noread_cnt < 5)
			    अनुवर्त;
			भङ्ग;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    यदि ((wait_pid == (pid_t)-1 && त्रुटि_सं == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    अन्यथा
			wait_pid = 0;

# यदि defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		यदि (options & SHELL_READ)
		{
		    यदि (ga.ga_len > 0)
		    {
			append_ga_पंक्ति(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    अन्यथा
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		यदि (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		यदि (toshell_fd >= 0)
		    बंद(toshell_fd);
		बंद(fromshell_fd);
	    }
# यदि (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    अन्यथा
	    {
		दीर्घ delay_msec = 1;

		यदि (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    out_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		प्रति (;;)
		{
		    यदि (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			समाप्त(-pid, संक_विघ्न);
#  अन्यथा
			समाप्त(0, संक_विघ्न);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &status, WNOHANG);
		    यदि ((wait_pid == (pid_t)-1 && त्रुटि_सं == ECHILD)
			    || (wait_pid == pid && WIFEXITED(status)))
		    {
			wait_pid = pid;
			भङ्ग;
		    }

#  यदि defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    यदि (++delay_msec > 10)
			delay_msec = 10;
		}

		यदि (tmode == TMODE_RAW)
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
	    यदि (wait_pid != pid)
		(शून्य)wait4pid(pid, &status);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    यदि (pty_master_fd >= 0)
		बंद(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    यदि (wpid > 0)
	    {
		समाप्त(wpid, SIGKILL);
		wait4pid(wpid, रिक्त);
	    }

# ifdef FEAT_JOB_CHANNEL
	    --dont_check_job_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    यदि (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    set_signals();

	    यदि (WIFEXITED(status))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(status);
		यदि (retval != 0 && !emsg_silent)
		{
		    यदि (retval == EXEC_FAILED)
		    {
			संदेश_puts(_("\nCannot execute shell "));
			संदेश_outtrans(p_sh);
			संदेश_putchar('\n');
		    }
		    अन्यथा यदि (!(options & SHELL_SILENT))
		    {
			संदेश_puts(_("\nshell returned "));
			संदेश_outnum((दीर्घ)retval);
			संदेश_putchar('\n');
		    }
		}
	    }
	    अन्यथा
		संदेश_puts(_("\nCommand terminated\n"));
	}
    }

error:
    यदि (!did_settmode)
	यदि (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    प्रत्यागम retval;
}
#endif // USE_SYSTEM

    पूर्णाङ्क
mch_call_shell(
    char_u	*cmd,
    पूर्णाङ्क		options)	// SHELL_*, see vim.h
{
#ifdef FEAT_EVAL
    ch_log(रिक्त, "executing shell command: %s", cmd);
#endif
#यदि defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    यदि (gui.in_use && vim_strchr(p_go, GO_TERMINAL) != रिक्त
					      && (options & SHELL_SILENT) == 0)
	प्रत्यागम mch_call_shell_terminal(cmd, options);
#endif
#ifdef USE_SYSTEM
    प्रत्यागम mch_call_shell_system(cmd, options);
#अन्यथा
    प्रत्यागम mch_call_shell_fork(cmd, options);
#endif
}

#यदि defined(FEAT_EVAL)
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
    वर्ण	**argv,
    char_u	*infile,
    पूर्णाङ्क		flags UNUSED,
    पूर्णाङ्क		*ret_len)
{
    pid_t	pid;
    पूर्णाङ्क		fd_out[2] = {-1, -1};
    पूर्णाङ्क		status = -1;
    char_u	*buffer = रिक्त;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(रिक्त, "directly executing: %s", argv[0]);

    यदि (pipe(fd_out) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	प्रत्यागम रिक्त;
    }

    BLOCK_SIGNALS(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# अन्यथा
    pid = विभाजन();
# endif
    यदि (pid == -1)
    {
	UNBLOCK_SIGNALS(&curset);
	बंद(fd_out[0]);
	बंद(fd_out[1]);
	emsg(_("\nCannot fork\n"));
	प्रत्यागम रिक्त;
    }

    यदि (pid == 0)
    {
	// child process
	reset_signals();
	UNBLOCK_SIGNALS(&curset);

	यदि (ch_log_active())
	{
	    ch_log(रिक्त, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	यदि (infile != रिक्त)
	{
	    पूर्णाङ्क fd_in = खोलो((वर्ण *)infile, O_RDONLY);
	    यदि (fd_in >= 0)
	    {
		बंद(0);
		vim_ignored = dup(fd_in);
		बंद(fd_in);
	    }
	}
	अन्यथा
	{
	    पूर्णाङ्क nullfd = खोलो("/dev/null", O_RDONLY);
	    यदि (nullfd >= 0)
	    {
		बंद(0);
		vim_ignored = dup(nullfd);
		बंद(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	बंद(fd_out[0]);
	बंद(1);
	vim_ignored = dup(fd_out[1]);
	// Also redirect stderr to the pipe.
	बंद(2);
	vim_ignored = dup(fd_out[1]);
	बंद(fd_out[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_SIGNALS(&curset);
    बंद(fd_out[1]);

    // Read output from child.
    प्रति (;;)
    {
	वर्ण	buf[4096];
	पूर्णाङ्क	n;

	n = (पूर्णाङ्क)पठ(fd_out[0], buf, परिमाण(buf));
	यदि (n <= 0)
	    भङ्ग;
	यदि (ga_grow(&ga, n) == FAIL)
	    भङ्ग;
	mch_memmove((वर्ण *)ga.ga_data + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    बंद(fd_out[0]);

    // Wait for child to finish.
    (शून्य)waitpid(pid, &status, 0);
    यदि (WIFEXITED(status))
	status = WEXITSTATUS(status);
    अन्यथा
	status = -1;
    set_vim_var_nr(VV_SHELL_ERROR, (दीर्घ)status);

    यदि (ga.ga_len > 0)
    {
	buffer = alloc(ga.ga_len + 1);
	यदि (buffer != रिक्त)
	{
	    mch_memmove(buffer, ga.ga_data, ga.ga_len);
	    यदि (ret_len == रिक्त)
	    {
		पूर्णाङ्क	i;

		// Change NUL into SOH, otherwise the string is truncated.
		प्रति (i = 0; i < ga.ga_len; ++i)
		    यदि (buffer[i] == NUL)
			buffer[i] = 1;
		buffer[ga.ga_len] = NUL;
	    }
	    अन्यथा
		*ret_len = ga.ga_len;
	}
    }
    अन्यथा यदि (ret_len != रिक्त)
	*ret_len = 0;

    ga_clear(&ga);
    प्रत्यागम buffer;
}
#endif

#यदि defined(FEAT_JOB_CHANNEL)
    शून्य
mch_job_start(वर्ण **argv, job_T *job, jobopt_T *options, पूर्णाङ्क is_terminal)
{
    pid_t	pid;
    पूर्णाङ्क		fd_in[2] = {-1, -1};	// for stdin
    पूर्णाङ्क		fd_out[2] = {-1, -1};	// for stdout
    पूर्णाङ्क		fd_err[2] = {-1, -1};	// for stderr
    पूर्णाङ्क		pty_master_fd = -1;
    पूर्णाङ्क		pty_slave_fd = -1;
    channel_T	*channel = रिक्त;
    पूर्णाङ्क		use_null_for_in = options->jo_io[PART_IN] == JIO_NULL;
    पूर्णाङ्क		use_null_for_out = options->jo_io[PART_OUT] == JIO_NULL;
    पूर्णाङ्क		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    पूर्णाङ्क		use_file_for_in = options->jo_io[PART_IN] == JIO_FILE;
    पूर्णाङ्क		use_file_for_out = options->jo_io[PART_OUT] == JIO_FILE;
    पूर्णाङ्क		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    पूर्णाङ्क		use_buffer_for_in = options->jo_io[PART_IN] == JIO_BUFFER;
    पूर्णाङ्क		use_out_for_err = options->jo_io[PART_ERR] == JIO_OUT;
    SIGSET_DECL(curset)

    यदि (use_out_for_err && use_null_for_out)
	use_null_for_err = TRUE;

    // default is to fail
    job->jv_status = JOB_FAILED;

    यदि (options->jo_pty
	    && (!(use_file_for_in || use_null_for_in)
		|| !(use_file_for_out || use_null_for_out)
		|| !(use_out_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &job->jv_tty_out, &job->jv_tty_in);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    यदि (use_file_for_in)
    {
	char_u *fname = options->jo_io_name[PART_IN];

	fd_in[0] = mch_open((वर्ण *)fname, O_RDONLY, 0);
	यदि (fd_in[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    गच्छ failed;
	}
    }
    अन्यथा
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	यदि (!use_null_for_in && (pty_master_fd < 0 || use_buffer_for_in)
							    && pipe(fd_in) < 0)
	    गच्छ failed;

    यदि (use_file_for_out)
    {
	char_u *fname = options->jo_io_name[PART_OUT];

	fd_out[1] = mch_open((वर्ण *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	यदि (fd_out[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    गच्छ failed;
	}
    }
    अन्यथा यदि (!use_null_for_out && pty_master_fd < 0 && pipe(fd_out) < 0)
	गच्छ failed;

    यदि (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((वर्ण *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	यदि (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    गच्छ failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    अन्यथा यदि (!use_out_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_set & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	गच्छ failed;

    यदि (!use_null_for_in || !use_null_for_out || !use_null_for_err)
    {
	यदि (options->jo_set & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    यदि (channel != रिक्त)
		++channel->ch_refcount;
	}
	अन्यथा
	    channel = add_channel();
	यदि (channel == रिक्त)
	    गच्छ failed;
	यदि (job->jv_tty_out != रिक्त)
	    ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    }

    BLOCK_SIGNALS(&curset);
    pid = विभाजन();	// maybe we should use vfork()
    यदि (pid == -1)
    {
	// failed to fork
	UNBLOCK_SIGNALS(&curset);
	गच्छ failed;
    }
    यदि (pid == 0)
    {
	पूर्णाङ्क	null_fd = -1;
	पूर्णाङ्क	stderr_works = TRUE;

	// child
	reset_signals();		// handle signals normally
	UNBLOCK_SIGNALS(&curset);

# ifdef FEAT_EVAL
	यदि (ch_log_active())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(शून्य)setsid();
# endif

# ifdef FEAT_TERMINAL
	यदि (options->jo_term_rows > 0)
	{
	    वर्ण *term = (वर्ण *)T_NAME;

#  ifdef FEAT_GUI
	    यदि (term_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		term = दो_पर्या("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    यदि (term == रिक्त || *term == NUL || STRNCMP(term, "xterm", 5) != 0)
	    {
		यदि (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    term = "xterm-256color";
		अन्यथा यदि (t_colors > 16)
		    term = "xterm-color";
		अन्यथा
		    term = "xterm";
	    }
	    set_child_environment(
		    (दीर्घ)options->jo_term_rows,
		    (दीर्घ)options->jo_term_cols,
		    term,
		    is_terminal);
	}
	अन्यथा
# endif
	    set_default_child_environment(is_terminal);

	यदि (options->jo_env != रिक्त)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    पूर्णाङ्क		todo = (पूर्णाङ्क)dict->dv_hashtab.ht_used;

	    FOR_ALL_HASHTAB_वस्तुएँ(&dict->dv_hashtab, hi, todo)
		यदि (!HASHITEM_EMPTY(hi))
		{
		    typval_T *item = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_कुंजी, tv_get_string(item));
		    --todo;
		}
	}

	यदि (use_null_for_in || use_null_for_out || use_null_for_err)
	{
	    null_fd = खोलो("/dev/null", O_RDWR | O_EXTRA, 0);
	    यदि (null_fd < 0)
	    {
		लिखो_त्रुटि("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	यदि (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    setup_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (वर्ण *)रिक्त);
# endif
	}

	// set up stdin for the child
	बंद(0);
	यदि (use_null_for_in && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	अन्यथा यदि (fd_in[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	अन्यथा
	    vim_ignored = dup(fd_in[0]);

	// set up stderr for the child
	बंद(2);
	यदि (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_works = FALSE;
	}
	अन्यथा यदि (use_out_for_err)
	    vim_ignored = dup(fd_out[1]);
	अन्यथा यदि (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	अन्यथा
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	बंद(1);
	यदि (use_null_for_out && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	अन्यथा यदि (fd_out[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	अन्यथा
	    vim_ignored = dup(fd_out[1]);

	यदि (fd_in[0] >= 0)
	    बंद(fd_in[0]);
	यदि (fd_in[1] >= 0)
	    बंद(fd_in[1]);
	यदि (fd_out[0] >= 0)
	    बंद(fd_out[0]);
	यदि (fd_out[1] >= 0)
	    बंद(fd_out[1]);
	यदि (fd_err[0] >= 0)
	    बंद(fd_err[0]);
	यदि (fd_err[1] >= 0)
	    बंद(fd_err[1]);
	यदि (pty_master_fd >= 0)
	{
	    बंद(pty_master_fd); // not used in the child
	    बंद(pty_slave_fd);  // was duped above
	}

	यदि (null_fd >= 0)
	    बंद(null_fd);

	यदि (options->jo_cwd != रिक्त && mch_chdir((वर्ण *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	यदि (stderr_works)
	    लिखो_त्रुटि("executing job failed");
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

    यदि (pty_master_fd >= 0)
	बंद(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    यदि (fd_in[0] >= 0)
	बंद(fd_in[0]);
    यदि (fd_out[1] >= 0)
	बंद(fd_out[1]);
    यदि (fd_err[1] >= 0)
	बंद(fd_err[1]);
    यदि (channel != रिक्त)
    {
	पूर्णाङ्क in_fd = INVALID_FD;
	पूर्णाङ्क out_fd = INVALID_FD;
	पूर्णाङ्क err_fd = INVALID_FD;

	यदि (!(use_file_for_in || use_null_for_in))
	    in_fd = fd_in[1] >= 0 ? fd_in[1] : pty_master_fd;

	यदि (!(use_file_for_out || use_null_for_out))
	    out_fd = fd_out[0] >= 0 ? fd_out[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	यदि (!(use_out_for_err || use_file_for_err || use_null_for_err))
	{
	    यदि (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    अन्यथा यदि (out_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_set_pipes(channel, in_fd, out_fd, err_fd);
	channel_set_job(channel, job, options);
    }
    अन्यथा
    {
	यदि (fd_in[1] >= 0)
	    बंद(fd_in[1]);
	यदि (fd_out[0] >= 0)
	    बंद(fd_out[0]);
	यदि (fd_err[0] >= 0)
	    बंद(fd_err[0]);
	यदि (pty_master_fd >= 0)
	    बंद(pty_master_fd);
    }

    // success!
    प्रत्यागम;

failed:
    channel_unref(channel);
    यदि (fd_in[0] >= 0)
	बंद(fd_in[0]);
    यदि (fd_in[1] >= 0)
	बंद(fd_in[1]);
    यदि (fd_out[0] >= 0)
	बंद(fd_out[0]);
    यदि (fd_out[1] >= 0)
	बंद(fd_out[1]);
    यदि (fd_err[0] >= 0)
	बंद(fd_err[0]);
    यदि (fd_err[1] >= 0)
	बंद(fd_err[1]);
    यदि (pty_master_fd >= 0)
	बंद(pty_master_fd);
    यदि (pty_slave_fd >= 0)
	बंद(pty_slave_fd);
}

    स्थिर char_u *
get_signal_name(पूर्णाङ्क sig)
{
    पूर्णाङ्क		i;
    char_u	numbuf[NUMBUFLEN];

    यदि (sig == SIGKILL)
	प्रत्यागम vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    प्रति (i = 0; signal_info[i].sig != -1; i++)
	यदि (sig == signal_info[i].sig)
	    प्रत्यागम strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((वर्ण *)numbuf, NUMBUFLEN, "%d", sig);
    प्रत्यागम vim_strnsave(numbuf, i);
}

    वर्ण *
mch_job_status(job_T *job)
{
    पूर्णाङ्क		status = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(job->jv_pid, &status, WNOHANG);
    यदि (wait_pid == -1)
    {
	पूर्णाङ्क waitpid_errno = त्रुटि_सं;
	यदि (waitpid_errno == ECHILD && mch_process_running(job->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    प्रत्यागम "run";

	// process must have exited
	यदि (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job no longer exists: %s",
						      म_त्रुटि(waitpid_errno));
	गच्छ return_dead;
    }
    यदि (wait_pid == 0)
	प्रत्यागम "run";
    यदि (WIFEXITED(status))
    {
	// LINTED avoid "bitwise operation on signed value"
	job->jv_exitval = WEXITSTATUS(status);
	यदि (job->jv_status < JOB_ENDED)
	    ch_log(job->jv_channel, "Job exited with %d", job->jv_exitval);
	गच्छ return_dead;
    }
    यदि (WIFSIGNALED(status))
    {
	job->jv_exitval = -1;
	job->jv_termsig = get_signal_name(WTERMSIG(status));
	यदि (job->jv_status < JOB_ENDED && job->jv_termsig != रिक्त)
	    ch_log(job->jv_channel, "Job terminated by signal \"%s\"",
							      job->jv_termsig);
	गच्छ return_dead;
    }
    प्रत्यागम "run";

return_dead:
    यदि (job->jv_status < JOB_ENDED)
	job->jv_status = JOB_ENDED;
    प्रत्यागम "dead";
}

    job_T *
mch_detect_ended_job(job_T *job_list)
{
    पूर्णाङ्क		status = -1;
    pid_t	wait_pid = 0;
    job_T	*job;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    यदि (dont_check_job_ended > 0)
	प्रत्यागम रिक्त;
# endif

    wait_pid = waitpid(-1, &status, WNOHANG);
    यदि (wait_pid <= 0)
	// no process ended
	प्रत्यागम रिक्त;
    प्रति (job = job_list; job != रिक्त; job = job->jv_next)
    {
	यदि (job->jv_pid == wait_pid)
	{
	    यदि (WIFEXITED(status))
		// LINTED avoid "bitwise operation on signed value"
		job->jv_exitval = WEXITSTATUS(status);
	    अन्यथा यदि (WIFSIGNALED(status))
	    {
		job->jv_exitval = -1;
		job->jv_termsig = get_signal_name(WTERMSIG(status));
	    }
	    यदि (job->jv_status < JOB_ENDED)
	    {
		ch_log(job->jv_channel, "Job ended");
		job->jv_status = JOB_ENDED;
	    }
	    प्रत्यागम job;
	}
    }
    प्रत्यागम रिक्त;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    पूर्णाङ्क
mch_signal_job(job_T *job, char_u *how)
{
    पूर्णाङ्क	    sig = -1;

    यदि (*how == NUL || STRCMP(how, "term") == 0)
	sig = संक_इति;
    अन्यथा यदि (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    अन्यथा यदि (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    अन्यथा यदि (STRCMP(how, "int") == 0)
	sig = संक_विघ्न;
    अन्यथा यदि (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    अन्यथा यदि (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    अन्यथा यदि (SAFE_isdigit(*how))
	sig = म_से_प((वर्ण *)how);
    अन्यथा
	प्रत्यागम FAIL;

    // Never kill ourselves!
    यदि (job->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	समाप्त(-job->jv_pid, sig);
	समाप्त(job->jv_pid, sig);
    }

    प्रत्यागम OK;
}

/*
 * Clear the data related to "job".
 */
    शून्य
mch_clear_job(job_T *job)
{
    // call waitpid because child process may become zombie
    (शून्य)waitpid(job->jv_pid, रिक्त, WNOHANG);
}
#endif

#यदि defined(FEAT_TERMINAL)
    पूर्णाङ्क
mch_create_pty_channel(job_T *job, jobopt_T *options)
{
    पूर्णाङ्क		pty_master_fd = -1;
    पूर्णाङ्क		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &job->jv_tty_out, &job->jv_tty_in);
    यदि (pty_master_fd < 0 || pty_slave_fd < 0)
	प्रत्यागम FAIL;
    बंद(pty_slave_fd);

    channel = add_channel();
    यदि (channel == रिक्त)
    {
	बंद(pty_master_fd);
	प्रत्यागम FAIL;
    }
    यदि (job->jv_tty_out != रिक्त)
	ch_log(channel, "using pty %s on fd %d",
					       job->jv_tty_out, pty_master_fd);
    job->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_set_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_set_job(channel, job, options);
    प्रत्यागम OK;
}
#endif

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    शून्य
mch_breakcheck(पूर्णाङ्क force)
{
    यदि ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, रिक्त, रिक्त))
	fill_निवेश_buf(FALSE);
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
    स्थिर पूर्णाङ्क
WaitForChar(दीर्घ msec, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेश)
{
#ifdef FEAT_TIMERS
    प्रत्यागम ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_निवेश) == OK;
#अन्यथा
    प्रत्यागम WaitForCharOrMouse(msec, interrupted, ignore_निवेश);
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
    स्थिर पूर्णाङ्क
WaitForCharOrMouse(दीर्घ msec, पूर्णाङ्क *interrupted, पूर्णाङ्क ignore_निवेश)
{
#ifdef FEAT_MOUSE_GPM
    पूर्णाङ्क		gpm_process_wanted;
#endif
#ifdef FEAT_XCLIPBOARD
    पूर्णाङ्क		rest;
#endif
    पूर्णाङ्क		avail;

    यदि (!ignore_निवेश && निवेश_available())	    // something in inbuf[]
	प्रत्यागम 1;

#यदि defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    यदि (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	यदि (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#endif

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#यदि defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    यदि (do_xterm_trace())
	rest = msec;
# endif
    कुरु
    {
# ifdef FEAT_XCLIPBOARD
	यदि (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    यदि (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    यदि (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	यदि (has_sound_callback_in_queue())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	यदि (!avail && !gpm_process_wanted)
# अन्यथा
	avail = RealWaitForChar(read_cmd_fd, msec, रिक्त, interrupted);
	यदि (!avail)
# endif
	{
	    यदि (!ignore_निवेश && निवेश_available())
		प्रत्यागम 1;
# ifdef FEAT_XCLIPBOARD
	    यदि (rest == 0 || !do_xterm_trace())
# endif
		भङ्ग;
	}
    }
    यावत् (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#अन्यथा
    avail = RealWaitForChar(read_cmd_fd, msec, रिक्त, interrupted);
#endif
    प्रत्यागम avail;
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
    स्थिर पूर्णाङ्क
RealWaitForChar(पूर्णाङ्क fd, दीर्घ msec, पूर्णाङ्क *check_for_gpm UNUSED, पूर्णाङ्क *interrupted)
{
    पूर्णाङ्क		ret;
    पूर्णाङ्क		result;
# यदि defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    स्थिर पूर्णाङ्क	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    दीर्घ	start_msec = msec;
    elapsed_T	start_tv;

    यदि (msec > 0)
	ELAPSED_INIT(start_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    यदि (busy)
	प्रत्यागम 0;
# endif

# ifdef MAY_LOOP
    प्रति (;;)
# endif
    {
# ifdef MAY_LOOP
	पूर्णाङ्क		finished = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	पूर्णाङ्क		mzquantum_used = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	पूर्णाङ्क		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	संरचना pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	पूर्णाङ्क		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	पूर्णाङ्क             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	पूर्णाङ्क		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	पूर्णाङ्क		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	पूर्णाङ्क		xsmp_idx = -1;
#  endif
	पूर्णाङ्क		towait = (पूर्णाङ्क)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	यदि (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (पूर्णाङ्क)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	यदि ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	यदि (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_संख्या(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	यदि (check_for_gpm != रिक्त && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	यदि (xsmp_icefd != -1)
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
	यदि (interrupted != रिक्त)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	result = ret > 0 && (fds[0].revents & POLLIN);
	यदि (result == 0 && interrupted != रिक्त && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	यदि (ret == 0 && mzquantum_used)
	    // MzThreads scheduling is required and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	यदि (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	यदि (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    यदि (--ret == 0 && !निवेश_available())
		// Try again
		finished = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	यदि (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	यदि (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    यदि (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
	    }
	    अन्यथा यदि (fds[xsmp_idx].revents & POLLHUP)
	    {
		यदि (p_verbose > 0)
		    verb_संदेश(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    यदि (--ret == 0)
		finished = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	यदि (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# अन्यथा // HAVE_SELECT

	संरचना timeval  tv;
	संरचना timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	स्थिर fd_set	rfds, wfds, efds;
	पूर्णाङ्क		maxfd;
	दीर्घ		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_threads();
	यदि (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_used = TRUE;
	}
#  endif

	यदि (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	अन्यथा
	    tvp = रिक्त;

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
	यदि ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    यदि (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	यदि (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_संख्या(xterm_dpy), &rfds);
	    यदि (maxfd < Connection_संख्या(xterm_dpy))
		maxfd = Connection_संख्या(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	यदि (check_for_gpm != रिक्त && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    यदि (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	यदि (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    यदि (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	maxfd = channel_select_setup(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	यदि (interrupted != रिक्त)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	result = ret > 0 && FD_ISSET(fd, &rfds);
	यदि (result)
	    --ret;
	अन्यथा यदि (interrupted != रिक्त && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	यदि (ret == -1 && त्रुटि_सं == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    यदि (got_tstp && !in_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    यदि (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(रिक्त, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    गच्छ select_eintr;
	}
#  endif
#  ifdef __TANDEM
	यदि (ret == -1 && त्रुटि_सं == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	यदि (ret == 0 && mzquantum_used)
	    // loop if MzThreads must be scheduled and timeout occurred
	    finished = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	यदि (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	यदि (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_संख्या(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    यदि (--ret == 0 && !निवेश_available())
	    {
		// Try again
		finished = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	यदि (ret > 0 && check_for_gpm != रिक्त && gpm_flag && gpm_fd >= 0)
	{
	    यदि (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    अन्यथा यदि (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	यदि (ret > 0 && xsmp_icefd != -1)
	{
	    यदि (FD_ISSET(xsmp_icefd, &efds))
	    {
		यदि (p_verbose > 0)
		    verb_संदेश(_("XSMP lost ICE connection"));
		xsmp_close();
		यदि (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	    अन्यथा यदि (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_requests();
		busy = FALSE;
		यदि (--ret == 0)
		    finished = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_JOB_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	यदि (ret >= 0)
	    (शून्य)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	यदि (finished || msec == 0)
	    भङ्ग;

#  यदि defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	यदि (clientserver_method == CLIENTSERVER_METHOD_X11 && server_waiting())
	    भङ्ग;
#  endif

	// We're going to loop around again, find out for how long
	यदि (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = start_msec - ELAPSED_FUNC(start_tv);
#  अन्यथा
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    यदि (msec <= 0)
		भङ्ग;	// waited long enough
	}
# endif
    }

    प्रत्यागम result;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    पूर्णाङ्क
mch_expandpath(
    garray_T	*gap,
    char_u	*path,
    पूर्णाङ्क		flags)		// EW_* flags
{
    प्रत्यागम unix_expandpath(gap, path, 0, flags, FALSE);
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

# ifndef शुरू_से
#  define शुरू_से 0
# endif
# ifndef अंत_से
#  define अंत_से 2
# endif

# define SHELL_SPECIAL (char_u *)"\t \"&'$;<>()\\|\n"

    पूर्णाङ्क
mch_expand_wildcards(
    पूर्णाङ्क		   अंक_pat,
    char_u	 **pat,
    पूर्णाङ्क		  *अंक_file,
    char_u	***file,
    पूर्णाङ्क		   flags)	// EW_* flags
{
    पूर्णाङ्क		i;
    माप_प्रकार	len;
    दीर्घ	llen;
    char_u	*p;
    पूर्णाङ्क		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    पूर्णाङ्क		j;
    char_u	*tempname;
    माप_प्रकार	tempnamelen;
    char_u	*command;
    माप_प्रकार	commandlen;
    खाता	*fd;
    char_u	*buffer;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    पूर्णाङ्क		shell_style = STYLE_ECHO;
    पूर्णाङ्क		check_spaces;
    स्थिर पूर्णाङ्क	did_find_nul = FALSE;
    पूर्णाङ्क		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    स्थिर string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    स्थिर string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *अंक_file = 0;	// default: no files found
    *file = रिक्त;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    यदि (!have_wildcard(अंक_pat, pat))
	प्रत्यागम save_patterns(अंक_pat, pat, अंक_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    यदि (sandbox != 0 && check_secure())
	प्रत्यागम FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    यदि (secure || restricted)
	प्रति (i = 0; i < अंक_pat; ++i)
	    यदि (vim_strchr(pat[i], '`') != रिक्त
		    && (check_restricted() || check_secure()))
		प्रत्यागम FAIL;

    /*
     * get a name for the temp file
     */
    यदि ((tempname = vim_tempname('o', FALSE)) == रिक्त)
    {
	emsg(_(e_cant_get_temp_file_name));
	प्रत्यागम FAIL;
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
    यदि (अंक_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    अन्यथा यदि ((len = STRLEN(p_sh)) >= 3)
    {
	यदि (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	अन्यथा यदि (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    यदि (shell_style == STYLE_ECHO)
    {
	यदि (म_माला((वर्ण *)gettail(p_sh), "bash") != रिक्त)
	    shell_style = STYLE_GLOBSTAR;
	अन्यथा यदि (म_माला((वर्ण *)gettail(p_sh), "sh") != रिक्त)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    यदि (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.length;
    अन्यथा यदि (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.length + sh_globstar_opt.length;

    प्रति (i = 0; i < अंक_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# अन्यथा
	++len;				// add space
	प्रति (j = 0; pat[i][j] != NUL; ++j)
	{
	    यदि (vim_strchr(SHELL_SPECIAL, pat[i][j]) != रिक्त)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    यदि (command == रिक्त)
    {
	// out of memory
	vim_free(tempname);
	प्रत्यागम FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    यदि (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((वर्ण *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)म_माला((वर्ण *)command, ")>");
	यदि (p == रिक्त)
	{
	    ;					    // can't happen ;-)
	}
	अन्यथा
	{
	    --p;
	    यावत् (p > command && VIM_ISWHITE(*p))
		--p;
	    यदि (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    यावत् (p > command && VIM_ISWHITE(*p))
		--p;
	    यदि (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    अन्यथा
    {
	यदि (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((वर्ण *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	अन्यथा यदि (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((वर्ण *)command, len, "print -N >%s", tempname);
	अन्यथा यदि (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((वर्ण *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	अन्यथा यदि (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((वर्ण *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	अन्यथा
	    commandlen = vim_snprintf((वर्ण *)command, len, "echo >%s", tempname);

	प्रति (i = 0; i < अंक_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((वर्ण *)command + commandlen, len, " \"%s\"", pat[i]);
# अन्यथा
	    पूर्णाङ्क intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    प्रति (j = 0; pat[i][j] != NUL; ++j)
	    {
		यदि (pat[i][j] == '`')
		    intick = !intick;
		अन्यथा यदि (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    यदि (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != रिक्त
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		अन्यथा यदि (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != रिक्त)
		    // Put a backslash before a special character, but not
		    // when inside ``. And not for $var when EW_KEEPDOLLAR is
		    // set.
		    *p++ = '\\';

		// Copy one character.
		*p++ = pat[i][j];
	    }
	    *p = NUL;
	    commandlen = (माप_प्रकार)(p - command);
# endif
	}
    }
    यदि (flags & EW_SILENT)
	show_shell_mess = FALSE;
    यदि (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    यदि (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    अन्यथा यदि (shell_style == STYLE_GLOB && !have_dollars(अंक_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    यदि (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = रिक्त;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    यदि (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	यदि (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    संदेश_putchar('\n');		// clear bottom line quickly
	    cmdline_row = Rows - 1;	// continue on last line
# ifdef USE_SYSTEM
	    यदि (!(flags & EW_SILENT))
# endif
	    {
		संदेश(_(e_cannot_expand_wildcards));
		संदेश_start();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	यदि (shell_style == STYLE_BT)
	    प्रत्यागम FAIL;
	गच्छ notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = ख_खोलो((वर्ण *)tempname, READBIN);
    यदि (fd == रिक्त)
    {
	// Something went wrong, perhaps a file name with a special char.
	यदि (!(flags & EW_SILENT))
	{
	    संदेश(_(e_cannot_expand_wildcards));
	    संदेश_start();		// don't overwrite this message
	}
	vim_free(tempname);
	गच्छ notfound;
    }
    ख_जाओ(fd, 0L, अंत_से);
    llen = ख_बताओ(fd);			// get size of temp file
    ख_जाओ(fd, 0L, शुरू_से);
    यदि (llen < 0)
	// just in case ftell() would fail
	buffer = रिक्त;
    अन्यथा
	buffer = alloc(llen + 1);
    यदि (buffer == रिक्त)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	ख_बंद(fd);
	प्रत्यागम FAIL;
    }
    len = llen;
    i = ख_पढ़ो((वर्ण *)buffer, 1, len, fd);
    ख_बंद(fd);
    mch_remove(tempname);
    यदि (i != (पूर्णाङ्क)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(buffer);
	प्रत्यागम FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = buffer;
    प्रति (i = 0; i < (पूर्णाङ्क)len; ++i)
	यदि (!(buffer[i] == CAR && buffer[i + 1] == NL))
	    *p++ = buffer[i];
    len = p - buffer;
# endif


    // file names are separated with Space
    यदि (shell_style == STYLE_ECHO)
    {
	buffer[len] = '\n';		// make sure the buffer ends in NL
	p = buffer;
	प्रति (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    यावत् (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    अन्यथा यदि (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	buffer[len] = NUL;		// make sure the buffer ends in NUL
	p = buffer;
	प्रति (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    यावत् (*p != '\n' && *p != NUL)
		++p;
	    यदि (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    अन्यथा
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
	यदि (shell_style == STYLE_PRINT && !did_find_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    buffer[len] = NUL;
	    यदि (len && (पूर्णाङ्क)STRLEN(buffer) < (पूर्णाङ्क)len)
		did_find_nul = TRUE;
	    अन्यथा
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	यदि (len && buffer[len - 1] == NUL)
	    --len;
	अन्यथा
	    buffer[len] = NUL;
	i = 0;
	प्रति (p = buffer; p < buffer + len; ++p)
	    यदि (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	यदि (len)
	    ++i;			// count last entry
    }
    यदि (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(buffer);
	गच्छ notfound;
    }
    *अंक_file = i;
    *file = ALLOC_MULT(char_u *, i);
    यदि (*file == रिक्त)
    {
	// out of memory
	vim_free(buffer);
	प्रत्यागम FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = buffer;
    प्रति (i = 0; i < *अंक_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	यदि (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    यावत् (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    यदि (p == buffer + len)		// last entry
		*p = NUL;
	    अन्यथा
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	अन्यथा		// NUL separates
	{
	    यावत् (*p && p < buffer + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    प्रति (j = 0, i = 0; i < *अंक_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	यदि (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    अनुवर्त;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	यदि ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    अनुवर्त;

	// Skip files that are not executable if we check for that.
	यदि (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], रिक्त, !(flags & EW_SHELLCMD)))
	    अनुवर्त;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	यदि (p)
	{
	    STRCPY(p, (*file)[i]);
	    यदि (dir)
		add_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(buffer);
    *अंक_file = j;

    यदि (*अंक_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	गच्छ notfound;
    }

    प्रत्यागम OK;

notfound:
    यदि (flags & EW_NOTFOUND)
	प्रत्यागम save_patterns(अंक_pat, pat, अंक_file, file);
    प्रत्यागम FAIL;
}

#endif // VMS

    स्थिर पूर्णाङ्क
save_patterns(
    पूर्णाङ्क		अंक_pat,
    char_u	**pat,
    पूर्णाङ्क		*अंक_file,
    char_u	***file)
{
    पूर्णाङ्क		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, अंक_pat);
    यदि (*file == रिक्त)
	प्रत्यागम FAIL;
    प्रति (i = 0; i < अंक_pat; i++)
    {
	s = vim_strsave(pat[i]);
	यदि (s != रिक्त)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *अंक_file = अंक_pat;
    प्रत्यागम OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    पूर्णाङ्क
mch_has_exp_wildcard(char_u *p)
{
    प्रति ( ; *p; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा
	    यदि (vim_strchr((char_u *)
#ifdef VMS
				    "*?%"
#अन्यथा
				    "*?[{'"
#endif
						, *p) != रिक्त)
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    पूर्णाङ्क
mch_has_wildcard(char_u *p)
{
    प्रति ( ; *p; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा
	    यदि (vim_strchr((char_u *)
#ifdef VMS
				    "*?%$"
#अन्यथा
				    "*?[{`'$"
#endif
						, *p) != रिक्त
		|| (*p == '~' && p[1] != NUL))
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

    स्थिर पूर्णाङ्क
have_wildcard(पूर्णाङ्क अंक, char_u **file)
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < अंक; i++)
	यदि (mch_has_wildcard(file[i]))
	    प्रत्यागम 1;
    प्रत्यागम 0;
}

    स्थिर पूर्णाङ्क
have_dollars(पूर्णाङ्क अंक, char_u **file)
{
    पूर्णाङ्क	    i;

    प्रति (i = 0; i < अंक; i++)
	यदि (vim_strchr(file[i], '$') != रिक्त)
	    प्रत्यागम TRUE;
    प्रत्यागम FALSE;
}

#यदि !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    पूर्णाङ्क
mch_rename(ध्रुव वर्ण *src, ध्रुव वर्ण *dest)
{
    संरचना stat	    st;

    यदि (stat(dest, &st) >= 0)	    // fail if destination exists
	प्रत्यागम -1;
    यदि (link(src, dest) != 0)	    // link file to new name
	प्रत्यागम -1;
    यदि (mch_remove(src) == 0)	    // delete link to old name
	प्रत्यागम 0;
    प्रत्यागम -1;
}
#endif // !HAVE_RENAME

#यदि defined(FEAT_MOUSE_GPM)
# यदि defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    स्थिर पूर्णाङ्क
load_libgpm(शून्य)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    यदि (libgpm_hinst == रिक्त)
    {
	यदि (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	प्रत्यागम FAIL;
    }

    यदि (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == रिक्त
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == रिक्त
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == रिक्त
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == रिक्त
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == रिक्त
      )
    {
	semsg(_(e_could_not_load_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = रिक्त;
	dll_gpm_flag = रिक्त;
	dll_gpm_fd   = रिक्त;
	प्रत्यागम FAIL;
    }
    प्रत्यागम OK;
}

    पूर्णाङ्क
gpm_available(शून्य)
{
    प्रत्यागम libgpm_hinst != रिक्त || load_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    स्थिर पूर्णाङ्क
gpm_open(शून्य)
{
    स्थिर Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    यदि (!gpm_available())
	प्रत्यागम 0;
# endif

    यदि (gpm_flag)
	प्रत्यागम 1; // already open

    gpm_connect.eventMask = (GPM_UP | GPM_DRAG | GPM_DOWN);
    gpm_connect.defaultMask = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    यदि (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? संक_छोड़ो : sig_tstp);
# endif
	प्रत्यागम 1; // succeed
    }
    यदि (gpm_fd == -2)
    {
	Gpm_Close(); // We don't want to talk to xterm via gpm

	// Gpm_Close fails to properly restore the WINCH and TSTP handlers,
	// leading to Vim ignoring resize signals. We have to re-initialize
	// these handlers again here.
# ifdef SIGWINCH
	mch_signal(SIGWINCH, sig_winch);
# endif
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? संक_छोड़ो : sig_tstp);
# endif
    }
    प्रत्यागम 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    पूर्णाङ्क
gpm_enabled(शून्य)
{
    प्रत्यागम gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    स्थिर शून्य
gpm_close(शून्य)
{
    यदि (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    स्थिर पूर्णाङ्क
mch_gpm_process(शून्य)
{
    पूर्णाङ्क			button;
    स्थिर Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    पूर्णाङ्क			row,col;
    अचिन्हित वर्ण	buttons_mask;
    अचिन्हित वर्ण	gpm_modifiers;
    स्थिर अचिन्हित वर्ण old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    यदि (hold_gui_events)
	प्रत्यागम 0;
# endif

    row = gpm_event.y - 1;
    col = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    विकल्प (GPM_BARE_EVENTS(gpm_event.type))
    {
	पक्ष GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    भङ्ग;
	पक्ष GPM_DOWN:
	    buttons_mask = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    विकल्प (buttons_mask)
	    {
		पक्ष GPM_B_बायाँ:
		    button = MOUSE_बायाँ;
		    भङ्ग;
		पक्ष GPM_B_मध्य:
		    button = MOUSE_मध्य;
		    भङ्ग;
		पक्ष GPM_B_दायाँ:
		    button = MOUSE_दायाँ;
		    भङ्ग;
		स्वभाव:
		    प्रत्यागम 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    SET_अंक_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    भङ्ग;
	पक्ष GPM_UP:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    भङ्ग;
	स्वभाव:
	    प्रत्यागम 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    यदि (gpm_modifiers & ((1 << KG_SHIFT) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_SHIFT;

    यदि (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    यदि (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(col + ' ' + 1);
    string[5] = (char_u)(row + ' ' + 1);
    add_to_निवेश_buf(string, 6);
    प्रत्यागम 6;
}
#endif // FEAT_MOUSE_GPM

#ifdef FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    स्थिर पूर्णाङ्क
sysmouse_open(शून्य)
{
    संरचना mouse_info   mouse;

    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.सङ्केत = SIGUSR2;
    यदि (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	प्रत्यागम FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    प्रत्यागम OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    स्थिर शून्य
sysmouse_close(शून्य)
{
    संरचना mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? संक_छोड़ो : संक_पूर्व);
    mouse.operation = MOUSE_MODE;
    mouse.u.mode.mode = 0;
    mouse.u.mode.सङ्केत = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    स्थिर शून्य
sig_sysmouse SIGDEFARG(sigarg)
{
    संरचना mouse_info	mouse;
    संरचना video_info	video;
    char_u		string[6];
    पूर्णाङ्क			row, col;
    पूर्णाङ्क			button;
    पूर्णाङ्क			buttons;
    स्थिर पूर्णाङ्क		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    यदि (hold_gui_events)
	प्रत्यागम;
# endif

    mouse.operation = MOUSE_GETINFO;
    यदि (ioctl(1, FBIO_GETMODE, &video.vi_mode) != -1
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
	यदि (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	अन्यथा
	{
	    विकल्प (buttons)
	    {
		पक्ष 0:
		    button = MOUSE_RELEASE;
		    भङ्ग;
		पक्ष 1:
		    button = MOUSE_बायाँ;
		    भङ्ग;
		पक्ष 2:
		    button = MOUSE_मध्य;
		    भङ्ग;
		पक्ष 4:
		    button = MOUSE_दायाँ;
		    भङ्ग;
		स्वभाव:
		    प्रत्यागम;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(col + ' ' + 1);
	string[5] = (char_u)(row + ' ' + 1);
	add_to_निवेश_buf(string, 6);
    }
    प्रत्यागम;
}
#endif // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    पूर्णाङ्क
mch_get_random(char_u *buf, पूर्णाङ्क len)
{
    स्थिर पूर्णाङ्क dev_urandom_state = NOTDONE;

    यदि (dev_urandom_state == FAIL)
	प्रत्यागम FAIL;

    पूर्णाङ्क fd = खोलो("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    यदि (fd == -1)
	dev_urandom_state = FAIL;
    अन्यथा यदि (पठ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	बंद(fd);
    }
    अन्यथा
    {
	dev_urandom_state = FAIL;
	बंद(fd);
    }

    प्रत्यागम dev_urandom_state;
}

#यदि defined(FEAT_LIBCALL)
प्रकारनिर्देश char_u * (*STRPROCSTR)(char_u *);
प्रकारनिर्देश char_u * (*INTPROCSTR)(पूर्णाङ्क);
प्रकारनिर्देश पूर्णाङ्क (*STRPROCINT)(char_u *);
प्रकारनिर्देश पूर्णाङ्क (*INTPROCINT)(पूर्णाङ्क);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    पूर्णाङ्क
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    पूर्णाङ्क		argint,
    char_u	**string_result, // NULL when using number_result
    पूर्णाङ्क		*संख्या_result)
{
# यदि defined(USE_DLOPEN)
    शून्य	*hinstLib;
    वर्ण	*dlerr = रिक्त;
# अन्यथा
    shl_t	hinstLib;
# endif
    STRPROCSTR	ProcAdd;
    INTPROCSTR	ProcAddI;
    char_u	*retval_str = रिक्त;
    पूर्णाङ्क		retval_int = 0;
    पूर्णाङ्क		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# यदि defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (शून्य)dlerror();

    hinstLib = dlopen((वर्ण *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    यदि (hinstLib == रिक्त)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	यदि (dlerr != रिक्त)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# अन्यथा
    hinstLib = shl_load((ध्रुव वर्ण*)libname, BIND_IMMEDIATE|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    यदि (hinstLib != रिक्त)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	यदि (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  यदि defined(USE_DLOPEN)
	    dlerr = रिक्त;
#  endif
	    mch_didjmp();
	}
	अन्यथा
# endif
	{
	    retval_str = रिक्त;
	    retval_int = 0;

	    यदि (argstring != रिक्त)
	    {
# यदि defined(USE_DLOPEN)
		*(शून्य **)(&ProcAdd) = dlsym(hinstLib, (ध्रुव वर्ण *)funcname);
		dlerr = dlerror();
# अन्यथा
		यदि (shl_findsym(&hinstLib, (ध्रुव वर्ण *)funcname,
					TYPE_PROCEDURE, (शून्य *)&ProcAdd) < 0)
		    ProcAdd = रिक्त;
# endif
		यदि ((success = (ProcAdd != रिक्त
# यदि defined(USE_DLOPEN)
			    && dlerr == रिक्त
# endif
			    )))
		{
		    यदि (string_result == रिक्त)
			retval_int = ((STRPROCINT)(शून्य *)ProcAdd)(argstring);
		    अन्यथा
			retval_str = (ProcAdd)(argstring);
		}
	    }
	    अन्यथा
	    {
# यदि defined(USE_DLOPEN)
		*(शून्य **)(&ProcAddI) = dlsym(hinstLib, (ध्रुव वर्ण *)funcname);
		dlerr = dlerror();
# अन्यथा
		यदि (shl_findsym(&hinstLib, (ध्रुव वर्ण *)funcname,
				       TYPE_PROCEDURE, (शून्य *)&ProcAddI) < 0)
		    ProcAddI = रिक्त;
# endif
		यदि ((success = (ProcAddI != रिक्त
# यदि defined(USE_DLOPEN)
			    && dlerr == रिक्त
# endif
			    )))
		{
		    यदि (string_result == रिक्त)
			retval_int = ((INTPROCINT)(शून्य *)ProcAddI)(argint);
		    अन्यथा
			retval_str = (ProcAddI)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    यदि (string_result == रिक्त)
		*संख्या_result = retval_int;
	    अन्यथा यदि (retval_str != रिक्त
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_result = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	यदि (lc_signal != 0)
	{
	    पूर्णाङ्क i;

	    // try to find the name of this signal
	    प्रति (i = 0; signal_info[i].sig != -1; i++)
		यदि (lc_signal == signal_info[i].sig)
		    भङ्ग;
	    semsg(_(e_got_sig_str_in_libcall), signal_info[i].name);
	}
#  endif
# endif

# यदि defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	यदि (dlerr != रिक्त)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(शून्य)dlclose(hinstLib);
# अन्यथा
	(शून्य)shl_unload(hinstLib);
# endif
    }

    यदि (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
}
#endif

#यदि defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
स्थिर पूर्णाङ्क	xterm_trace = -1;	// default: disabled
स्थिर पूर्णाङ्क	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    शून्य
setup_term_clip(शून्य)
{
    पूर्णाङ्क		z = 0;
    वर्ण	*strp = "";
    Widget	AppShell;

    यदि (!x_connect_to_server())
	प्रत्यागम;

    open_app_context();
    यदि (app_context != रिक्त && xterm_Shell == (Widget)0)
    {
	पूर्णाङ्क (*oldhandler)(Display*, XErrorEvent*);
# यदि defined(USING_SETJMP)
	पूर्णाङ्क (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T start_tv;

	यदि (p_verbose > 0)
	    ELAPSED_INIT(start_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# यदि defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	यदि (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = रिक्त;
	}
	अन्यथा
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", रिक्त, 0, &z, &strp);
	    यदि (xterm_dpy != रिक्त)
		xterm_dpy_retry_गिनती = 0;
# यदि defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# यदि defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(शून्य)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(शून्य)XSetErrorHandler(oldhandler);

	यदि (xterm_dpy == रिक्त)
	{
	    यदि (p_verbose > 0)
		verb_संदेश(_("Opening the X display failed"));
	    प्रत्यागम;
	}

	// Catch terminating error of the X server connection.
	(शून्य)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	यदि (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_message(ELAPSED_FUNC(start_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		रिक्त);
	यदि (AppShell == (Widget)0)
	    प्रत्यागम;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		शीर्ष_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		रिक्त);
	यदि (xterm_Shell == (Widget)0)
	    प्रत्यागम;

	x11_setup_atoms(xterm_dpy);
	x11_setup_selection(xterm_Shell);
	यदि (x11_display == रिक्त)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    यदि (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	यदि (x11_window == 0 && (strp = दो_पर्या("WINDOWID")) != रिक्त)
	    x11_window = (Window)म_से_द(strp);
	// Check if $WINDOWID is valid.
	यदि (test_x11_window(xterm_dpy) == FAIL)
	    x11_window = 0;
	यदि (x11_window != 0)
	    xterm_trace = 0;
    }
}

    शून्य
start_xterm_trace(पूर्णाङ्क button)
{
    यदि (x11_window == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	प्रत्यागम;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    शून्य
stop_xterm_trace(शून्य)
{
    यदि (xterm_trace < 0)
	प्रत्यागम;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    स्थिर पूर्णाङ्क
do_xterm_trace(शून्य)
{
    Window		मूल, child;
    पूर्णाङ्क			मूल_x, मूल_y;
    पूर्णाङ्क			win_x, win_y;
    पूर्णाङ्क			row, col;
    int_u		mask_return;
    char_u		buf[50];
    char_u		*strp;
    दीर्घ		got_hints;
    स्थिर char_u	*mouse_code = रिक्त;
    स्थिर माप_प्रकार	mouse_codelen = 0;
    स्थिर char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    स्थिर पूर्णाङ्क		prev_row = 0, prev_col = 0;
    स्थिर XSizeHints	xterm_hints;

    यदि (xterm_trace <= 0)
	प्रत्यागम FALSE;

    यदि (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	यदि (!XGetWMNormalHints(xterm_dpy, x11_window, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.width_inc <= 1
		|| xterm_hints.height_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    प्रत्यागम FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_code = find_termcode(mouse_name);
	यदि (mouse_code != रिक्त)
	    mouse_codelen = STRLEN(mouse_code);
	prev_row = mouse_row;
	prev_col = mouse_col;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQueryPointer(xterm_dpy, x11_window, &मूल, &child, &मूल_x, &मूल_y,
		      &win_x, &win_y, &mask_return);
	xterm_hints.y = win_y - (xterm_hints.height_inc * mouse_row)
			      - (xterm_hints.height_inc / 2);
	यदि (xterm_hints.y <= xterm_hints.height_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.width_inc * mouse_col)
			      - (xterm_hints.width_inc / 2);
	यदि (xterm_hints.x <= xterm_hints.width_inc / 2)
	    xterm_hints.x = 2;
	प्रत्यागम TRUE;
    }

    यदि (mouse_code == रिक्त || mouse_codelen > 45)
    {
	xterm_trace = 0;
	प्रत्यागम FALSE;
    }

    XQueryPointer(xterm_dpy, x11_window, &मूल, &child, &मूल_x, &मूल_y,
		  &win_x, &win_y, &mask_return);

    row = check_row((win_y - xterm_hints.y) / xterm_hints.height_inc);
    col = check_col((win_x - xterm_hints.x) / xterm_hints.width_inc);
    यदि (row == prev_row && col == prev_col)
	प्रत्यागम TRUE;

    STRCPY(buf, mouse_code);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(col + ' ' + 1);
    *strp++ = (char_u)(row + ' ' + 1);
    *strp = NUL;
    add_to_निवेश_buf(buf, strp - buf);

    prev_row = row;
    prev_col = col;
    प्रत्यागम TRUE;
}

# यदि defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    शून्य
clear_xterm_clip(शून्य)
{
    यदि (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    यदि (xterm_dpy != रिक्त)
    {
#  यदि 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	यदि (x11_display == xterm_dpy)
	    x11_display = रिक्त;
	xterm_dpy = रिक्त;
    }
#  यदि 0
    यदि (app_context != (XtAppContext)रिक्त)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)रिक्त;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    स्थिर शून्य
clip_update(शून्य)
{
# ifdef FEAT_GUI
    यदि (gui.in_use)
	gui_mch_update();
    अन्यथा
# endif
    यदि (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    शून्य
xterm_update(शून्य)
{
    XEvent event;

    प्रति (;;)
    {
	Xt_निवेश_Mask mask = XtAppPending(app_context);

	यदि (mask == 0 || vim_is_निवेश_buf_full())
	    भङ्ग;

	यदि (mask & XtIMXEvent)
	{
	    // There is an event to process.
	    XtAppNextEvent(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		यदि (e->type == PropertyNotify && e->window == commWindow
		   && e->atom == commProperty && e->state == Property_New_मूल्य)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	अन्यथा
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, mask);
	}
    }
}

    पूर्णाङ्क
clip_xterm_own_selection(Clipboard_T *cbd)
{
    यदि (xterm_Shell != (Widget)0)
	प्रत्यागम clip_x11_own_selection(xterm_Shell, cbd);
    प्रत्यागम FAIL;
}

    शून्य
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    यदि (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    शून्य
clip_xterm_request_selection(Clipboard_T *cbd)
{
    यदि (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    शून्य
clip_xterm_set_selection(Clipboard_T *cbd)
{
    clip_x11_set_selection(cbd);
}
#endif


#यदि defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# यदि defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    स्थिर शून्य
xsmp_handle_interaction(SmcConn smc_conn, SmPointer client_data UNUSED)
{
    पूर्णाङ्क		save_cmod_flags;
    पूर्णाङ्क		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    यदि (check_changed_any(FALSE, FALSE))
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
    यदि (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	SmcSaveYourselfDone(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    स्थिर शून्य
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED,
    पूर्णाङ्क		save_type UNUSED,
    Bool	shutdown,
    पूर्णाङ्क		interact_style UNUSED,
    Bool	fast UNUSED)
{
    // Handle already being in saveyourself
    यदि (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    out_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    यदि (p_verbose > 0)
	verb_संदेश(_("XSMP handling save-yourself request"));

# यदि defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    यदि (shutdown && !fast && gui.in_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_data);
    अन्यथा
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
    स्थिर शून्य
xsmp_die(SmcConn smc_conn UNUSED, SmPointer client_data UNUSED)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    स्थिर शून्य
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
    स्थिर शून्य
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    SmPointer	client_data UNUSED)
{
    यदि (xsmp.save_yourself)
	SmcSaveYourselfDone(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    स्थिर शून्य
xsmp_ice_connection(
    IceConn	iceConn,
    IcePointer	clientData UNUSED,
    Bool	opening,
    IcePointer	*watchData UNUSED)
{
    // Intercept creation of ICE connection fd
    यदि (opening)
    {
	xsmp_icefd = Ice_Connection_संख्या(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, रिक्त);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    पूर्णाङ्क
xsmp_handle_requests(शून्य)
{
    Bool rep;

    यदि (IceProcessMessages(xsmp.iceconn, रिक्त, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	यदि (p_verbose > 0)
	    verb_संदेश(_("XSMP lost ICE connection"));
	xsmp_close();
	प्रत्यागम FAIL;
    }
    अन्यथा
	प्रत्यागम OK;
}

स्थिर पूर्णाङ्क dummy;

// Set up X Session Management Protocol
    शून्य
xsmp_init(शून्य)
{
    वर्ण		errorstring[80];
    SmcCallbacks	smcallbacks;
# यदि 0
    Sm_Prop_मूल्य		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    यदि (p_verbose > 0)
	verb_संदेश(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_data = रिक्त;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_data = रिक्त;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_data = रिक्त;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_data = रिक्त;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    यदि (IceAddConnectionWatch(xsmp_ice_connection, &dummy) == 0)
    {
	यदि (p_verbose > 0)
	    verb_संदेश(_("XSMP ICE connection watch failed"));
	प्रत्यागम;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    रिक्त,
	    रिक्त,
	    SmProtoMajor,
	    SmProtoMinor,
	    SmcSaveYourselfProcMask | SmcDieProcMask
		     | SmcSaveCompleteProcMask | SmcShutdownCancelledProcMask,
	    &smcallbacks,
	    रिक्त,
	    &xsmp.clientid,
	    परिमाण(errorstring) - 1,
	    errorstring);
    यदि (xsmp.smcconn == रिक्त)
    {
	यदि (p_verbose > 0)
	{
	    वर्ण errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[परिमाण(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, परिमाण(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_संदेश(errorreport);
	}
	प्रत्यागम;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# यदि 0
    // ID ourselves
    smname.मूल्य = "vim";
    smname.length = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.अंक_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    SmcSetProperties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    शून्य
xsmp_close(शून्य)
{
    यदि (xsmp_icefd == -1)
	प्रत्यागम;

    SmcCloseConnection(xsmp.smcconn, 0, रिक्त);
    यदि (xsmp.clientid != रिक्त)
	मुक्त(xsmp.clientid);
    xsmp.clientid = रिक्त;
    xsmp_icefd = -1;
}
#endif // USE_XSMP

#यदि defined(FEAT_RELTIME)
# यदि defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
स्थिर चञ्चल संक_पूर्ण_प्रकार timeout_flag = FALSE;
स्थिर timer_t		     timer_id;
स्थिर पूर्णाङ्क		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    स्थिर शून्य
set_flag(सङ्घ sigval _unused UNUSED)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    शून्य
stop_timeout(शून्य)
{
    स्थिर संरचना itimerspec disarm = {{0, 0}, {0, 0}};

    यदि (timer_created)
    {
	पूर्णाङ्क ret = timer_settime(timer_id, 0, &disarm, रिक्त);

	यदि (ret < 0)
	    semsg(_(e_could_not_clear_timeout_str), म_त्रुटि(त्रुटि_सं));
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
    चञ्चल संक_पूर्ण_प्रकार *
start_timeout(दीर्घ msec)
{
    संरचना itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    पूर्णाङ्क ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    यदि (!timer_created)
    {
	संरचना sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = set_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	यदि (ret < 0)
	{
	    semsg(_(e_could_not_set_timeout_str), म_त्रुटि(त्रुटि_सं));
	    प्रत्यागम &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(रिक्त, "setting timeout timer to %d sec %ld nsec",
	       (पूर्णाङ्क)interval.it_value.tv_sec, (दीर्घ)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, रिक्त);
    यदि (ret < 0)
	semsg(_(e_could_not_set_timeout_str), म_त्रुटि(त्रुटि_सं));

    प्रत्यागम &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    शून्य
delete_timer(शून्य)
{
    यदि (!timer_created)
	प्रत्यागम;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# अन्यथा // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
स्थिर संरचना sigaction		prev_sigaction;
स्थिर चञ्चल संक_पूर्ण_प्रकार	timeout_flag	     = FALSE;
स्थिर पूर्णाङ्क			timer_active	     = FALSE;
स्थिर पूर्णाङ्क			timer_handler_active = FALSE;
स्थिर चञ्चल संक_पूर्ण_प्रकार	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    स्थिर शून्य
set_flag SIGDEFARG(sigarg)
{
    यदि (alarm_pending)
	alarm_pending = FALSE;
    अन्यथा
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    शून्य
stop_timeout(शून्य)
{
    स्थिर संरचना itimerval disarm = {{0, 0}, {0, 0}};
    पूर्णाङ्क			    ret;

    यदि (timer_active)
    {
	timer_active = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, रिक्त);
	यदि (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), म_त्रुटि(त्रुटि_सं));
    }

    यदि (timer_handler_active)
    {
	timer_handler_active = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, रिक्त);
	यदि (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_reset_handler_for_timeout_str),
							      म_त्रुटि(त्रुटि_सं));
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
    चञ्चल संक_पूर्ण_प्रकार *
start_timeout(दीर्घ msec)
{
    संरचना itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    संरचना sigaction	handle_alarm;
    पूर्णाङ्क			ret;
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
    ret = ret == 0 ? संक_बाकी(&sigs) : ret;
    यदि (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, रिक्त);
    }
    यदि (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), म_त्रुटि(त्रुटि_सं));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = set_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    यदि (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_handler_for_timeout_str), म_त्रुटि(त्रुटि_सं));
	प्रत्यागम &timeout_flag;
    }
    timer_handler_active = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, रिक्त);
    यदि (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_set_timeout_str), म_त्रुटि(त्रुटि_सं));
	stop_timeout();
	प्रत्यागम &timeout_flag;
    }

    timer_active = TRUE;
    प्रत्यागम &timeout_flag;
}
# endif // PROF_NSEC
#endif  // FEAT_RELTIME
