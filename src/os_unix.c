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

#ಅನ್ತರ್ಭಾವ "vim.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
# include "if_mzsch.h"
#ಆದರೆ_ಅಂತ್ಯ

#ಅನ್ತರ್ಭಾವ "os_unixx.h"	    // unix includes for os_unix.c only

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
# include <X11/SM/SMlib.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SELINUX
# include <selinux/selinux.h>
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ selinux_enabled = -1;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XATTR
# include <sys/xattr.h>
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SMACK
# include <sys/xattr.h>
# include <linux/xattr.h>
# ifndef SMACK_LABEL_LEN
#  define SMACK_LABEL_LEN 1024
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
# include <cygwin/version.h>
# include <sys/cygwin.h>	// for cygwin_conv_to_posix_path() and/or
				// for cygwin_conv_path()
# ifdef FEAT_CYGWIN_WIN32_CLIPBOARD
#  define WIN32_LEAN_AND_ಸರಾಸರಿ
#  include <windows.h>
#  include "winclip.pro"
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM

# include <gpm.h>

# ifdef DYNAMIC_GPM
#  define Gpm_Open     (*dll_Gpm_Open)
#  define Gpm_Close    (*dll_Gpm_Close)
#  define Gpm_GetEvent (*dll_Gpm_GetEvent)
#  define gpm_flag     (dll_gpm_flag != ಖಾಲಿ ? *dll_gpm_flag :  0)
#  define gpm_fd       (dll_gpm_fd   != ಖಾಲಿ ? *dll_gpm_fd   : -1)

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Gpm_Open)     (Gpm_Connect *, ಪೂರ್ಣಾಂಕ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Gpm_Close)    (ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ (*dll_Gpm_GetEvent) (Gpm_Event *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *dll_gpm_flag;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ *dll_gpm_fd;

ಸ್ಥಿರ ಶೂನ್ಯ *libgpm_hinst;
# endif

// <linux/keyboard.h> contains defines conflicting with "keymap.h",
// I just copied relevant defines here. A cleaner solution would be to put gpm
// code into separate file and include there linux/keyboard.h
// #include <linux/keyboard.h>
# define KG_ಸರಿಸು	0
# define KG_CTRL	2
# define KG_ALT		3
# define KG_ALTGR	1
# define KG_SHIFTL	4
# define KG_SHIFTR	5
# define KG_CTRLL	6
# define KG_CTRLR	7
# define KG_CAPSSHIFT	8

ಸ್ಥಿರ ಶೂನ್ಯ gpm_close(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ gpm_open(ಶೂನ್ಯ);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ mch_gpm_process(ಶೂನ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYSMOUSE
# include <sys/consio.h>
# include <sys/fbio.h>

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sysmouse_open(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ sysmouse_close(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ sig_sysmouse SIGPROTOARG;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * end of autoconf section. To be extended...
 */

// Are the following #ifdefs still required? And why? Is that for X11?

#ಆದರೆ defined(ESIX) || defined(M_UNIX) && !defined(SCO)
# ifdef SIGWINCH
#  undef SIGWINCH
# endif
# ifdef TIOCGWINSZ
#  undef TIOCGWINSZ
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SIGWINDOW) && !defined(SIGWINCH)	// hpux 9.01 has it
# define SIGWINCH SIGWINDOW
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11
# include <X11/Xlib.h>
# include <X11/Xutil.h>
# include <X11/Xatom.h>
# ifdef FEAT_XCLIPBOARD
#  include <X11/Intrinsic.h>
#  include <X11/Shell.h>
#  include <X11/StringDefs.h>
ಸ್ಥಿರ Widget	xterm_Shell = (Widget)0;
ಸ್ಥಿರ ಶೂನ್ಯ clip_update(ಶೂನ್ಯ);
# endif

ಕಿಟಕಿ	    x11_ಕಿಟಕಿ = 0;
Display	    *x11_display = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ignore_sigtstp = FALSE;

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_x11_title(ಪೂರ್ಣಾಂಕ);

ಸ್ಥಿರ char_u	*oldtitle = ಖಾಲಿ;
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t oldtitle_outdated = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	unix_did_ಗಣ_title = FALSE;
ಸ್ಥಿರ char_u	*oldicon = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_ಗಣ_icon = FALSE;

ಸ್ಥಿರ ಶೂನ್ಯ may_core_dump(ಶೂನ್ಯ);

ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ waitstatus;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  WaitForChar(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  WaitForCharOrMouse(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
ಪೂರ್ಣಾಂಕ  RealWaitForChar(ಪೂರ್ಣಾಂಕ, ದೀರ್ಘ, ಪೂರ್ಣಾಂಕ *, ಪೂರ್ಣಾಂಕ *interrupted);
#ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  RealWaitForChar(ಪೂರ್ಣಾಂಕ, ದೀರ್ಘ, ಪೂರ್ಣಾಂಕ *, ಪೂರ್ಣಾಂಕ *interrupted);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XCLIPBOARD
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ do_xterm_trace(ಶೂನ್ಯ);
# define XT_TRACE_DELAY	50	// delay for xterm tracing
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ಶೂನ್ಯ handle_resize(ಶೂನ್ಯ);

#ಆದರೆ defined(SIGWINCH)
ಸ್ಥಿರ ಶೂನ್ಯ sig_winch SIGPROTOARG;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGTSTP)
ಸ್ಥಿರ ಶೂನ್ಯ sig_tstp SIGPROTOARG;
// volatile because it is used in signal handler sig_tstp() and
// sigcont_handler().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t ಒಳ_mch_suspend = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGINT)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigint SIGPROTOARG;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGUSR1)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigusr1 SIGPROTOARG;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGPWR)
ಸ್ಥಿರ ಶೂನ್ಯ catch_sigpwr SIGPROTOARG;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGALRM) && defined(FEAT_X11) && !defined(FEAT_GUI_GTK)
# define ಗಣ_SIG_ALARM
ಸ್ಥಿರ ಶೂನ್ಯ sig_alarm SIGPROTOARG;
// volatile because it is used in signal handler sig_alarm().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t sig_alarm_called;
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಶೂನ್ಯ deathtrap SIGPROTOARG;

ಸ್ಥಿರ ಶೂನ್ಯ catch_int_signal(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ ಗಣ_ಸಂಕೇತಗಳು(ಶೂನ್ಯ);
ಸ್ಥಿರ ಶೂನ್ಯ catch_ಸಂಕೇತಗಳು(ಶೂನ್ಯ (*func_deadly)(ಪೂರ್ಣಾಂಕ), ಶೂನ್ಯ (*func_other)(ಪೂರ್ಣಾಂಕ));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGPROCMASK
# define SIGSET_DECL(ಗಣ_2)	sigset_t ಗಣ_2;
# define ಖಂಡ_ಸಂಕೇತಗಳು(ಗಣ_2)	ಖಂಡ_ಸಂಕೇತಗಳು_2(ಗಣ_2)
# define UNBLOCK_ಸಂಕೇತಗಳು(ಗಣ_2)	unblock_ಸಂಕೇತಗಳು(ಗಣ_2)
#ಇಲ್ಲದಿದ್ದರೆ
# define SIGSET_DECL(ಗಣ_2)
# define ಖಂಡ_ಸಂಕೇತಗಳು(ಗಣ_2)	ಮಾಡು { /**/ } ಆಗ (0)
# define UNBLOCK_ಸಂಕೇತಗಳು(ಗಣ_2)	ಮಾಡು { /**/ } ಆಗ (0)
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  have_wildcard(ಪೂರ್ಣಾಂಕ, char_u **);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ  have_dollars(ಪೂರ್ಣಾಂಕ, char_u **);

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ save_patterns(ಪೂರ್ಣಾಂಕ ಅಂಕೆ_pat, char_u **pat, ಪೂರ್ಣಾಂಕ *ಅಂಕೆ_file, char_u ***file);

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIG_ERR
# define SIG_ERR	((sighandler_T)-1)
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ SIG_HOLD
# define SIG_HOLD	((sighandler_T)-2)
#ಆದರೆ_ಅಂತ್ಯ

// volatile because it is used in signal handler sig_winch().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t do_resize = FALSE;
// volatile because it is used in signal handler sig_tstp().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t got_tstp = FALSE;
ಸ್ಥಿರ char_u	*extra_shell_arg = ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	show_shell_mess = TRUE;
// volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t deadly_signal = 0;	   // The signal we caught
// volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t ಒಳ_mch_delay = FALSE; // sleeping in mch_delay()

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL) && !defined(USE_SYSTEM)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dont_check_ಕೆಲಸ_ended = 0;
#ಆದರೆ_ಅಂತ್ಯ

// Current terminal mode from mch_settmode().  Can differ from cur_tmode.
ಸ್ಥಿರ tmode_T mch_cur_tmode = TMODE_COOK;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_XSMP
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ
{
    SmcConn smcconn;	    // The SM connection ID
    IceConn iceconn;	    // The ICE connection ID
    ಅಕ್ಷರ *clientid;	    // The client ID for the current smc session
    Bool save_yourself;     // If we're in the middle of a save_yourself
    Bool shutdown;	    // If we're in shutdown mode
} xsmp_config_T;

ಸ್ಥಿರ xsmp_config_T xsmp;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SYS_SIGLIST_DECLARED
/*
 * I have seen
 *  extern char *_sys_siglist[NSIG];
 * on Linux, NetBSD and Solaris. It contains a nice list of strings
 * that describe the signals. That is nearly what we want here.  But
 * autoconf does only check for sys_siglist (without the underscore), I
 * do not want to change everything today.... jw.
 * This is why AC_DECL_SYS_SIGLIST is commented out in configure.ac.
 */
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ರಚನೆ signalinfo
{
    ಪೂರ್ಣಾಂಕ	    sig;	// Signal number, eg. SIGSEGV etc
    ಅಕ್ಷರ    *name;	// Signal name (not char_u!).
    ಅಕ್ಷರ    deadly;	// Catch as a deadly signal?
} signal_info[] =
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHUP
    {SIGHUP,	    "HUP",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGQUIT
    {SIGQUIT,	    "QUIT",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGILL
    {SIGILL,	    "ILL",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTRAP
    {SIGTRAP,	    "TRAP",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGABRT
    {SIGABRT,	    "ABRT",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGEMT
    {SIGEMT,	    "EMT",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGFPE
    {SIGFPE,	    "FPE",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGBUS
    {SIGBUS,	    "BUS",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGSEGV) && !defined(FEAT_MZSCHEME)
    // MzScheme uses SEGV in its garbage collector
    {SIGSEGV,	    "SEGV",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGSYS
    {SIGSYS,	    "SYS",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGALRM
    {SIGALRM,	    "ALRM",	FALSE},	// Perl's alarm() can trigger it
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTERM
    {SIGTERM,	    "TERM",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGVTALRM) && !defined(FEAT_RUBY)
    {SIGVTALRM,	    "VTALRM",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGPROF) && !defined(FEAT_MZSCHEME) && !defined(WE_ARE_PROFILING)
    // MzScheme uses SIGPROF for its own needs; On Linux with profiling
    // this makes Vim exit.  WE_ARE_PROFILING is defined in Makefile.
    {SIGPROF,	    "PROF",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGXCPU
    {SIGXCPU,	    "XCPU",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGXFSZ
    {SIGXFSZ,	    "XFSZ",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGUSR1
    {SIGUSR1,	    "USR1",	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGUSR2) && !defined(FEAT_SYSMOUSE)
    // Used for sysmouse handling
    {SIGUSR2,	    "USR2",	TRUE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGINT
    {SIGINT,	    "INT",	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGWINCH
    {SIGWINCH,	    "WINCH",	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTSTP
    {SIGTSTP,	    "TSTP",	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGPIPE
    {SIGPIPE,	    "PIPE",	FALSE},
#ಆದರೆ_ಅಂತ್ಯ
    {-1,	    "Unknown!", FALSE}
};

    sighandler_T
mch_signal(ಪೂರ್ಣಾಂಕ sig, sighandler_T func)
{
#ಆದರೆ defined(HAVE_SIGACTION) && defined(HAVE_SIGPROCMASK)
    // Modern implementation: use sigaction().
    ರಚನೆ sigaction	sa, old;
    sigset_t		curset;
    ಪೂರ್ಣಾಂಕ			blocked;

    ಆದರೆ (sigprocmask(SIG_BLOCK, ಖಾಲಿ, &curset) == -1)
	ಹಿಂತಿರುಗಿ SIG_ERR;

    blocked = sigismember(&curset, sig);

    ಆದರೆ (func == SIG_HOLD)
    {
	ಆದರೆ (blocked)
	    ಹಿಂತಿರುಗಿ SIG_HOLD;

	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ಆದರೆ (sigaction(sig, ಖಾಲಿ, &old) == -1
				|| sigprocmask(SIG_BLOCK, &curset, ಖಾಲಿ) == -1)
	    ಹಿಂತಿರುಗಿ SIG_ERR;
	ಹಿಂತಿರುಗಿ old.sa_handler;
    }

    ಆದರೆ (blocked)
    {
	sigemptyset(&curset);
	sigaddset(&curset, sig);

	ಆದರೆ (sigprocmask(SIG_UNBLOCK, &curset, ಖಾಲಿ) == -1)
	    ಹಿಂತಿರುಗಿ SIG_ERR;
    }

    sa.sa_handler = func;
    sigemptyset(&sa.sa_mask);
# ifdef SA_RESTART
    sa.sa_flags = SA_RESTART;
# ಇಲ್ಲದಿದ್ದರೆ
    sa.sa_flags = 0;
# endif
    ಆದರೆ (sigaction(sig, &sa, &old) == -1)
	ಹಿಂತಿರುಗಿ SIG_ERR;
    ಹಿಂತಿರುಗಿ blocked ? SIG_HOLD: old.sa_handler;
#ಅಥವಾಆದರೆ defined(HAVE_SIGSET)
    // Using sigset() is preferred above signal().
    ಹಿಂತಿರುಗಿ sigset(sig, func);
#ಇಲ್ಲದಿದ್ದರೆ
    // Oldest and most compatible solution.
    ಹಿಂತಿರುಗಿ ಸಂಕೇತ(sig, func);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಪೂರ್ಣಾಂಕ
mch_chdir(ಅಕ್ಷರ *ಮಾರ್ಗ)
{
    ಆದರೆ (p_verbose >= 5)
    {
	verbose_enter();
	smsg("chdir(%s)", ಮಾರ್ಗ);
	verbose_leave();
    }
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಹಿಂತಿರುಗಿ chdir(vms_fixfilename(ಮಾರ್ಗ));
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ chdir(ಮಾರ್ಗ);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(ECHOE) && defined(ICANON) \
    && (defined(HAVE_TERMIO_H) || defined(HAVE_TERMIOS_H))
# define NEW_TTY_SYSTEM
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Write s[len] to the screen (stdout).
 */
    ಶೂನ್ಯ
mch_write(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    vim_ignored = (ಪೂರ್ಣಾಂಕ)ಲಿಖ(1, (ಅಕ್ಷರ *)s, len);
    ಆದರೆ (p_wd)		// Unix is too fast, slow down a bit more
	RealWaitForChar(read_cmd_fd, p_wd, ಖಾಲಿ, ಖಾಲಿ);
}

/*
 * Function passed to inchar_loop() to handle window resizing.
 * If "check_only" is TRUE: Return whether there was a resize.
 * If "check_only" is FALSE: Deal with the window resized.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
resize_func(ಪೂರ್ಣಾಂಕ check_only)
{
    ಆದರೆ (check_only)
	ಹಿಂತಿರುಗಿ do_resize;
    ಆಗ (do_resize)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
	ch_log(ಖಾಲಿ, "calling handle_resize() in resize_func()");
#ಆದರೆ_ಅಂತ್ಯ
	handle_resize();
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * Return the number of characters that are available.
 * If wtime == 0 do not wait for characters.
 * If wtime == n wait a short time for characters.
 * If wtime == -1 wait forever for characters.
 */
    ಪೂರ್ಣಾಂಕ
mch_inchar(
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		maxlen,
    ದೀರ್ಘ	wtime,	    // don't use "time", MIPS cannot handle it
    ಪೂರ್ಣಾಂಕ		tb_change_cnt)
{
    ಹಿಂತಿರುಗಿ inchar_loop(buf, maxlen, wtime, tb_change_cnt,
		       WaitForChar, resize_func);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
handle_resize(ಶೂನ್ಯ)
{
    do_resize = FALSE;
    shell_resized();
}

/*
 * Return non-zero if a character is available.
 */
    ಪೂರ್ಣಾಂಕ
mch_char_avail(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ WaitForChar(0L, ಖಾಲಿ, FALSE);
}

#ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Check for any pending input or messages.
 */
    ಪೂರ್ಣಾಂಕ
mch_check_messages(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ WaitForChar(0L, ಖಾಲಿ, TRUE);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_ಒಟ್ಟು_MEM)
# ifdef HAVE_SYS_RESOURCE_H
#  include <sys/resource.h>
# endif
# ಆದರೆ defined(HAVE_SYS_SYSCTL_H) && defined(HAVE_SYSCTL)
#  include <sys/sysctl.h>
# endif
# ಆದರೆ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
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
mch_ಒಟ್ಟು_mem(ಪೂರ್ಣಾಂಕ special ಬಳಸದ)
{
    long_u	mem = 0;
    long_u	shiftright = 10;  // how much to shift "mem" right for Kbyte

# ifdef MACOS_X
    {
	// Mac (Darwin) way of getting the amount of RAM available
	mach_ಪೋರ್ಟ್_t		host = mach_host_self();
	kern_return_t		kret;
#  ifdef HOST_VM_INFO64
	ರಚನೆ vm_statistics64	vm_stat;
	natural_t		ಎಣಿಕೆ_2 = HOST_VM_INFO64_ಎಣಿಕೆ;

	kret = host_statistics64(host, HOST_VM_INFO64,
					     (host_info64_t)&vm_stat, &ಎಣಿಕೆ_2);
#  ಇಲ್ಲದಿದ್ದರೆ
	ರಚನೆ vm_statistics	vm_stat;
	natural_t		ಎಣಿಕೆ_2 = HOST_VM_INFO_ಎಣಿಕೆ;

	kret = host_statistics(host, HOST_VM_INFO,
					       (host_info_t)&vm_stat, &ಎಣಿಕೆ_2);
#  endif
	ಆದರೆ (kret == KERN_SUCCESS)
	    // get the amount of user memory by summing each usage
	    mem = (long_u)(vm_stat.free_ಎಣಿಕೆ + vm_stat.ಸಕ್ರಿಯ_ಎಣಿಕೆ
					    + vm_stat.inactive_ಎಣಿಕೆ
#  ifdef MAC_OS_X_ಆವೃತ್ತಿ_10_9
					    + vm_stat.compressor_ಪುಟ_ಎಣಿಕೆ
#  endif
					    ) * sysconf(_SC_PAGESIZE);
	mach_ಪೋರ್ಟ್_deallocate(mach_task_self(), host);
    }
# endif

# ifdef HAVE_SYSCTL
    ಆದರೆ (mem == 0)
    {
	// BSD way of getting the amount of RAM available.
	ಪೂರ್ಣಾಂಕ		mib[2];
	size_t		len = ಗಾತ್ರ(long_u);
#  ifdef HW_USERMEM64
	long_u		physmem;
#  ಇಲ್ಲದಿದ್ದರೆ
	// sysctl() may return 32 bit or 64 bit, accept both
	ಒಕ್ಕೂಟ {
	    int_u	u32;
	    long_u	u64;
	} physmem;
#  endif

	mib[0] = CTL_HW;
#  ifdef HW_USERMEM64
	mib[1] = HW_USERMEM64;
#  ಇಲ್ಲದಿದ್ದರೆ
	mib[1] = HW_USERMEM;
#  endif
	ಆದರೆ (sysctl(mib, 2, &physmem, &len, ಖಾಲಿ, 0) == 0)
	{
#  ifdef HW_USERMEM64
	    mem = (long_u)physmem;
#  ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (len == ಗಾತ್ರ(physmem.u64))
		mem = (long_u)physmem.u64;
	    ಇಲ್ಲದಿದ್ದರೆ
		mem = (long_u)physmem.u32;
#  endif
	}
    }
# endif

# ಆದರೆ defined(HAVE_SYS_SYSINFO_H) && defined(HAVE_SYSINFO)
    ಆದರೆ (mem == 0)
    {
	ರಚನೆ sysinfo sinfo;

	// Linux way of getting amount of RAM available
	ಆದರೆ (sysinfo(&sinfo) == 0)
	{
#  ifdef HAVE_SYSINFO_MEM_UNIT
	    // avoid overflow as much as possible
	    ಆಗ (shiftright > 0 && (sinfo.mem_unit & 1) == 0)
	    {
		sinfo.mem_unit = sinfo.mem_unit >> 1;
		--shiftright;
	    }
	    mem = sinfo.totalram * sinfo.mem_unit;
#  ಇಲ್ಲದಿದ್ದರೆ
	    mem = sinfo.totalram;
#  endif
	}
    }
# endif

# ifdef HAVE_SYSCONF
    ಆದರೆ (mem == 0)
    {
	ದೀರ್ಘ	    pagesize, pagecount;

	// Solaris way of getting amount of RAM available
	pagesize = sysconf(_SC_PAGESIZE);
	pagecount = sysconf(_SC_PHYS_PAGES);
	ಆದರೆ (pagesize > 0 && pagecount > 0)
	{
	    // avoid overflow as much as possible
	    ಆಗ (shiftright > 0 && (pagesize & 1) == 0)
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
# ಆದರೆ defined(HAVE_SYS_RESOURCE_H) && defined(HAVE_GETRLIMIT)
    {
	ರಚನೆ rlimit	rlp;

	ಆದರೆ (getrlimit(RLIMIT_ದತ್ತಾಂಶ, &rlp) == 0
		&& rlp.rlim_cur < ((rlim_t)1 << (ಗಾತ್ರ(long_u) * 8 - 1))
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

    ಆದರೆ (mem > 0)
	ಹಿಂತಿರುಗಿ mem >> shiftright;
    ಹಿಂತಿರುಗಿ (long_u)0x1fffff;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * "flags": MCH_DELAY_IGNOREINPUT - don't read input
 *	    MCH_DELAY_SETTMODE - use settmode() even for short delays
 */
    ಶೂನ್ಯ
mch_delay(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ flags)
{
    tmode_T	old_tmode;
    ಪೂರ್ಣಾಂಕ		call_settmode;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
    ದೀರ್ಘ	ಒಟ್ಟು = msec; // remember original value
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (flags & MCH_DELAY_IGNOREINPUT)
    {
	// Go to cooked mode without echo, to allow SIGINT interrupting us
	// here.  But we don't want QUIT to kill us (CTRL-\ used in a
	// shell may produce SIGQUIT).
	// Only do this if sleeping for more than half a second.
	ಒಳ_mch_delay = TRUE;
	call_settmode = mch_cur_tmode == TMODE_RAW
			       && (msec > 500 || (flags & MCH_DELAY_SETTMODE));
	ಆದರೆ (call_settmode)
	{
	    old_tmode = mch_cur_tmode;
	    settmode(TMODE_SLEEP);
	}

	/*
	 * Everybody sleeps in a different way...
	 * Prefer nanosleep(), some versions of usleep() can only sleep up to
	 * one second.
	 */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
	ಮಾಡು
	{
	    // if total is large enough, wait by portions in p_mzq
	    ಆದರೆ (ಒಟ್ಟು > p_mzq)
		msec = p_mzq;
	    ಇಲ್ಲದಿದ್ದರೆ
		msec = ಒಟ್ಟು;
	    ಒಟ್ಟು -= msec;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_NANOSLEEP
	{
	    ರಚನೆ timespec ts;

	    ts.tv_sec = msec / 1000;
	    ts.tv_nsec = (msec % 1000) * 1000000;
	    (ಶೂನ್ಯ)nanosleep(&ts, ಖಾಲಿ);
	}
#ಇಲ್ಲದಿದ್ದರೆ
# ifdef HAVE_USLEEP
	ಆಗ (msec >= 1000)
	{
	    usleep((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(999 * 1000));
	    msec -= 999;
	}
	usleep((ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ)(msec * 1000));
# ಇಲ್ಲದಿದ್ದರೆ
#  ifndef HAVE_SELECT
	poll(ಖಾಲಿ, 0, (ಪೂರ್ಣಾಂಕ)msec);
#  ಇಲ್ಲದಿದ್ದರೆ
	{
	    ರಚನೆ timeval tv;

	    tv.tv_sec = msec / 1000;
	    tv.tv_usec = (msec % 1000) * 1000;
	    // NOTE: Solaris 2.6 has a bug that makes select() hang here.  Get
	    // a patch from Sun to fix this.  Reported by Gunnar Pedersen.
	    select(0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ, &tv);
	}
#  endif
# endif
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MZSCHEME
	}
	ಆಗ (ಒಟ್ಟು > 0);
#ಆದರೆ_ಅಂತ್ಯ

	ಆದರೆ (call_settmode)
	    settmode(old_tmode);
	ಒಳ_mch_delay = FALSE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	WaitForChar(msec, ಖಾಲಿ, FALSE);
}

#ಆದರೆ defined(HAVE_ರಾಶಿ_ಎಲ್ಲೆ) \
	|| (!defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGSTACK))
# define HAVE_CHECK_ರಾಶಿ_GROWTH
/*
 * Support for checking for an almost-out-of-stack-space situation.
 */

/*
 * Return a pointer to an item on the stack.  Used to find out if the stack
 * grows up or down.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ರಾಶಿ_grows_downwards;

/*
 * Find out if the stack grows upwards or downwards.
 * "p" points to a variable on the stack of the caller.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
check_ರಾಶಿ_growth(ಅಕ್ಷರ *p)
{
    ಪೂರ್ಣಾಂಕ		i;

    ರಾಶಿ_grows_downwards = (p > (ಅಕ್ಷರ *)&i);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_ರಾಶಿ_ಎಲ್ಲೆ)
ಸ್ಥಿರ ಅಕ್ಷರ *ರಾಶಿ_ಎಲ್ಲೆ = ಖಾಲಿ;

# ಆದರೆ defined(THREAD_ಸುರಕ್ಷಿತ) && defined(HAVE_PTHREAD_NP_H)
#  include <pthread.h>
#  include <pthread_np.h>
# endif

/*
 * Find out until how var the stack can grow without getting into trouble.
 * Called when starting up and when switching to the signal stack in
 * deathtrap().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
get_ರಾಶಿ_ಎಲ್ಲೆ(ಶೂನ್ಯ)
{
    ರಚನೆ rlimit	rlp;
    ಪೂರ್ಣಾಂಕ			i;
    ದೀರ್ಘ		lim;

    // Set the stack limit to 15/16 of the allowable size.  Skip this when the
    // limit doesn't fit in a long (rlim_cur might be "long long").
    ಆದರೆ (getrlimit(RLIMIT_ರಾಶಿ, &rlp) == 0
	    && rlp.rlim_cur < ((rlim_t)1 << (ಗಾತ್ರ(long_u) * 8 - 1))
# ifdef RLIM_INFINITY
	    && rlp.rlim_cur != RLIM_INFINITY
# endif
       )
    {
	lim = (ದೀರ್ಘ)rlp.rlim_cur;
# ಆದರೆ defined(THREAD_ಸುರಕ್ಷಿತ) && defined(HAVE_PTHREAD_NP_H)
	{
	    pthread_attr_t  attr;
	    size_t	    ಗಾತ್ರ_2;

	    // On FreeBSD the initial thread always has a fixed stack size, no
	    // matter what the limits are set to.  Normally it's 1 Mbyte.
	    pthread_attr_init(&attr);
	    ಆದರೆ (pthread_attr_get_np(pthread_self(), &attr) == 0)
	    {
		pthread_attr_getstacksize(&attr, &ಗಾತ್ರ_2);
		ಆದರೆ (lim > (ದೀರ್ಘ)ಗಾತ್ರ_2)
		    lim = (ದೀರ್ಘ)ಗಾತ್ರ_2;
	    }
	    pthread_attr_destroy(&attr);
	}
# endif
	ಆದರೆ (ರಾಶಿ_grows_downwards)
	{
	    ರಾಶಿ_ಎಲ್ಲೆ = (ಅಕ್ಷರ *)((ದೀರ್ಘ)&i - (lim / 16L * 15L));
	    ಆದರೆ (ರಾಶಿ_ಎಲ್ಲೆ >= (ಅಕ್ಷರ *)&i)
		// overflow, set to 1/16 of current stack position
		ರಾಶಿ_ಎಲ್ಲೆ = (ಅಕ್ಷರ *)((ದೀರ್ಘ)&i / 16L);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ರಾಶಿ_ಎಲ್ಲೆ = (ಅಕ್ಷರ *)((ದೀರ್ಘ)&i + (lim / 16L * 15L));
	    ಆದರೆ (ರಾಶಿ_ಎಲ್ಲೆ <= (ಅಕ್ಷರ *)&i)
		ರಾಶಿ_ಎಲ್ಲೆ = ಖಾಲಿ;	//ಅತಿಪ್ರವಾಹ
	}
    }
}

/*
 * Return FAIL when running out of stack space.
 * "p" must point to any variable local to the caller that's on the stack.
 */
    ಪೂರ್ಣಾಂಕ
mch_stackcheck(ಅಕ್ಷರ *p)
{
    ಆದರೆ (ರಾಶಿ_ಎಲ್ಲೆ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ OK;

    ಆದರೆ (ರಾಶಿ_grows_downwards)
    {
	ಆದರೆ (p < ರಾಶಿ_ಎಲ್ಲೆ)
	    ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p > ರಾಶಿ_ಎಲ್ಲೆ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
/*
 * Support for using the signal stack.
 * This helps when we run out of stack space, which causes a SIGSEGV.  The
 * signal handler then must run on another stack, since the normal stack is
 * completely full.
 */

# ifdef HAVE_SIGALTSTACK
ಸ್ಥಿರ stack_t sigstk;			// for sigaltstack()
# ಇಲ್ಲದಿದ್ದರೆ
ಸ್ಥಿರ ರಚನೆ sigstack sigstk;		// for sigstack()
# endif

/*
 * Get a size of signal stack.
 * Preference (if available): sysconf > SIGSTKSZ > guessed size
 */
ಸ್ಥಿರ ದೀರ್ಘ ಪೂರ್ಣಾಂಕ get_signal_ರಾಶಿ_ಗಾತ್ರ(ಶೂನ್ಯ)
{
# ifdef HAVE_SYSCONF_SIGSTKSZ
    ದೀರ್ಘ ಪೂರ್ಣಾಂಕ ಗಾತ್ರ_2 = -1;

    // return size only if sysconf doesn't return an error
    ಆದರೆ ((ಗಾತ್ರ_2 = sysconf(_SC_SIGSTKSZ)) > -1)
	ಹಿಂತಿರುಗಿ ಗಾತ್ರ_2;
# endif

# ifdef SIGSTKSZ
    // if sysconf() isn't available or gives error, return SIGSTKSZ
    // if defined
    ಹಿಂತಿರುಗಿ SIGSTKSZ;
# endif

    // otherwise guess the size
    ಹಿಂತಿರುಗಿ 8000;
}

ಸ್ಥಿರ ಅಕ್ಷರ *signal_ರಾಶಿ;

    ಸ್ಥಿರ ಶೂನ್ಯ
init_signal_ರಾಶಿ(ಶೂನ್ಯ)
{
    ಆದರೆ (signal_ರಾಶಿ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

# ifdef HAVE_SIGALTSTACK
#  ifdef HAVE_SS_ತಳ
    sigstk.ss_ತಳ = signal_ರಾಶಿ;
#  ಇಲ್ಲದಿದ್ದರೆ
    sigstk.ss_sp = signal_ರಾಶಿ;
#  endif
    sigstk.ss_size = get_signal_ರಾಶಿ_ಗಾತ್ರ();
    sigstk.ss_flags = 0;
    (ಶೂನ್ಯ)sigaltstack(&sigstk, ಖಾಲಿ);
# ಇಲ್ಲದಿದ್ದರೆ
    sigstk.ss_sp = signal_ರಾಶಿ;
    ಆದರೆ (ರಾಶಿ_grows_downwards)
	sigstk.ss_sp += get_signal_ರಾಶಿ_ಗಾತ್ರ() - 1;
    sigstk.ss_onstack = 0;
    (ಶೂನ್ಯ)sigstack(&sigstk, ಖಾಲಿ);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * We need correct prototypes for a signal function, otherwise mean compilers
 * will barf when the second argument to signal() is ``wrong''.
 * Let me try it with a few tricky defines from my own osdef.h	(jw).
 */
#ಆದರೆ defined(SIGWINCH)
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_winch SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGWINCH, sig_winch);
    do_resize = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SIGTSTP)
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_tstp SIGDEFARG(sigarg)
{
    // Second time we get called we actually need to suspend
    ಆದರೆ (ಒಳ_mch_suspend)
    {
	mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN : SIG_DFL);
	ಎತ್ತು(sigarg);
    }
    ಇಲ್ಲದಿದ್ದರೆ
	got_tstp = TRUE;

# ಆದರೆ !defined(__ANDROID__) && !defined(__OpenBSD__) && !defined(__DragonFly__)
    // This is not required on all systems.  On some systems (at least Android,
    // OpenBSD, and DragonFlyBSD) this breaks suspending with CTRL-Z.
    mch_signal(SIGTSTP, sig_tstp);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SIGINT)
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_sigint SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGINT, catch_sigint);
    got_int = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SIGUSR1)
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_sigusr1 SIGDEFARG(sigarg)
{
    // this is not required on all systems, but it doesn't hurt anybody
    mch_signal(SIGUSR1, catch_sigusr1);
    got_sigusr1 = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(SIGPWR)
    ಸ್ಥಿರ ಶೂನ್ಯ
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
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಗಣ_SIG_ALARM
/*
 * signal function for alarm().
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_alarm SIGDEFARG(sigarg)
{
    // doesn't do anything, just to break a system call
    sig_alarm_called = TRUE;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_SETJMP_H) \
	&& ((defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)) \
	    || defined(FEAT_LIBCALL))
# define USING_SETJMP 1

// argument to SETJMP()
ಸ್ಥಿರ JMP_BUF lc_jump_env;

# ifdef SIGHASARG
// Caught signal number, 0 when no signal was caught; used for mch_libcall().
// Volatile because it is used in signal handlers.
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t lc_signal;
# endif

// TRUE when lc_jump_env is valid.
// Volatile because it is used in signal handler deathtrap().
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t lc_ಸಕ್ರಿಯ = FALSE;

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
    ಸ್ಥಿರ ಶೂನ್ಯ
mch_startjmp(ಶೂನ್ಯ)
{
# ifdef SIGHASARG
    lc_signal = 0;
# endif
    lc_ಸಕ್ರಿಯ = TRUE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mch_endjmp(ಶೂನ್ಯ)
{
    lc_ಸಕ್ರಿಯ = FALSE;
}

    ಸ್ಥಿರ ಶೂನ್ಯ
mch_didjmp(ಶೂನ್ಯ)
{
# ಆದರೆ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    // On FreeBSD the signal stack has to be reset after using siglongjmp(),
    // otherwise catching the signal only works once.
    init_signal_ರಾಶಿ();
# endif
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * This function handles deadly signals.
 * It tries to preserve any swap files and exit properly.
 * (partly from Elvis).
 * NOTE: Avoid unsafe functions, such as allocating memory, they can result in
 * a deadlock.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
deathtrap SIGDEFARG(sigarg)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	entered = 0;	    // count the number of times we got here.
				    // Note: when memory has been corrupted
				    // this may get an arbitrary value!
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHASARG
    ಪೂರ್ಣಾಂಕ		i;
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(USING_SETJMP)
    /*
     * Catch a crash in protected code.
     * Restores the environment saved in lc_jump_env, which looks like
     * SETJMP() returns 1.
     */
    ಆದರೆ (lc_ಸಕ್ರಿಯ)
    {
# ಆದರೆ defined(SIGHASARG)
	lc_signal = sigarg;
# endif
	lc_ಸಕ್ರಿಯ = FALSE;	// don't jump again
	LONGJMP(lc_jump_env, 1);
	// NOTREACHED
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHASARG
# ifdef SIGQUIT
    // While in mch_delay() we go to cooked mode to allow a CTRL-C to
    // interrupt us.  But in cooked mode we may also get SIGQUIT, e.g., when
    // pressing CTRL-\, but we don't want Vim to exit then.
    ಆದರೆ (ಒಳ_mch_delay && sigarg == SIGQUIT)
	ಹಿಂತಿರುಗಿ;
# endif

    // When SIGHUP, SIGQUIT, etc. are blocked: postpone the effect and return
    // here.  This avoids that a non-reentrant function is interrupted, e.g.,
    // free().  Calling free() again may then cause a crash.
    ಆದರೆ (entered == 0
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
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

    // Remember how often we have been called.
    ++entered;

    // Executing autocommands is likely to use more stack space than we have
    // available in the signal stack.
    ಖಂಡ_autocmds();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    // Set the v:dying variable.
    ಗಣ_vim_var_nr(VV_DYING, (ದೀರ್ಘ)entered);
#ಆದರೆ_ಅಂತ್ಯ
    v_dying = entered;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_ರಾಶಿ_ಎಲ್ಲೆ
    // Since we are now using the signal stack, need to reset the stack
    // limit.  Otherwise using a regexp will fail.
    get_ರಾಶಿ_ಎಲ್ಲೆ();
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ 0
    // This is for opening gdb the moment Vim crashes.
    // You need to manually adjust the file name and Vim executable name.
    // Suggested by SungHyun Nam.
    {
# define VI_GDB_FILE "/tmp/vimgdb"
# define VIM_NAME "/usr/bin/vim"
	FILE *fp = fopen(VI_GDB_FILE, "w");
	ಆದರೆ (fp)
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
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHASARG
    // try to find the name of this signal
    ಗಾಗಿ (i = 0; signal_info[i].sig != -1; i++)
	ಆದರೆ (sigarg == signal_info[i].sig)
	    ನಿಲ್ಲಿಸು;
    deadly_signal = sigarg;
#ಆದರೆ_ಅಂತ್ಯ

    ತುಂಬಿದ_screen = FALSE;	// don't write message to the GUI, it might be
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
    ಆದರೆ (entered >= 3)
    {
	reset_ಸಂಕೇತಗಳು();	// don't catch any signals anymore
	may_core_dump();
	ಆದರೆ (entered >= 4)
	    _exit(8);
	ನಿರ್ಗಮಿಸು(7);
    }
    ಆದರೆ (entered == 2)
    {
	// No translation, it may call malloc().
	ಹೊರಹೋಗು_STR("Vim: Double signal, exiting\n");
	ಹೊರಹೋಗು_flush();
	getout(1);
    }

    // No translation, it may call malloc().
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGHASARG
    sprintf((ಅಕ್ಷರ *)IObuff, "Vim: Caught deadly signal %s\r\n",
							 signal_info[i].name);
#ಇಲ್ಲದಿದ್ದರೆ
    sprintf((ಅಕ್ಷರ *)IObuff, "Vim: Caught deadly signal\r\n");
#ಆದರೆ_ಅಂತ್ಯ

    // Preserve files and exit.  This sets the really_exiting flag to prevent
    // calling free().
    preserve_exit();

    // NOTREACHED

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ NBDEBUG
    reset_ಸಂಕೇತಗಳು();
    may_core_dump();
    abort();
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Invoked after receiving SIGCONT.  We don't know what happened while
 * sleeping, deal with part of that.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
after_sigcont(ಶೂನ್ಯ)
{
    // Don't change "oldtitle" in a signal handler, set a flag to obtain it
    // again later.
    oldtitle_outdated = TRUE;

    settmode(TMODE_RAW);
    need_check_timestamps = TRUE;
    did_check_timestamps = FALSE;
}

#ಆದರೆ defined(SIGCONT)
ಸ್ಥಿರ ಶೂನ್ಯ sigcont_handler SIGPROTOARG;

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
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t sigcont_received;
ಸ್ಥಿರ ಶೂನ್ಯ sigcont_handler SIGPROTOARG;

/*
 * signal handler for SIGCONT
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sigcont_handler SIGDEFARG(sigarg)
{
    ಆದರೆ (ಒಳ_mch_suspend)
    {
	sigcont_received = TRUE;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// We didn't suspend ourselves, assume we were stopped by a SIGSTOP
	// signal (which can't be intercepted) and get a SIGCONT.  Need to get
	// back to a sane mode. We should redraw, but we can't really do that
	// in a signal handler, do a redraw later.
	after_sigcont();
	redraw_later(UPD_CLEAR);
	cursor_on_force();
	ಹೊರಹೋಗು_flush();
    }
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_CLIPBOARD)
# ಆದರೆ defined(USE_SYSTEM) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
ಸ್ಥಿರ ಶೂನ್ಯ *clip_ನಕ್ಷತ್ರ_save = ಖಾಲಿ;
ಸ್ಥಿರ ಶೂನ್ಯ *clip_plus_save = ಖಾಲಿ;
# endif

# ಆದರೆ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Called when Vim is going to sleep or execute a shell command.
 * We can't respond to requests for the X or Wayland selections.
 * Lose them, otherwise other applications will hang.  But first
 * copy the text to cut buffer 0 (for X11). Wayland users must have
 * a clipboard manager to replicate such behaviour.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
loose_clipboard(ಶೂನ್ಯ)
{
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned || clip_plus.owned)
    {
#  ifdef FEAT_X11
	x11_export_final_selection();
#  endif
	ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	    clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
	ಆದರೆ (clip_plus.owned)
	    clip_lose_selection(&clip_plus);
#  ifdef FEAT_X11
	ಆದರೆ (x11_display != ಖಾಲಿ)
	    XFlush(x11_display);
#  endif
    }
}
# endif

# ಆದರೆ defined(USE_SYSTEM) && (defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD))
/*
 * Save clipboard text to restore later.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
save_clipboard(ಶೂನ್ಯ)
{
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	clip_ನಕ್ಷತ್ರ_save = get_ರಿಜಿಸ್ಟರ್('*', TRUE);
    ಆದರೆ (clip_plus.owned)
	clip_plus_save = get_ರಿಜಿಸ್ಟರ್('+', TRUE);
}

/*
 * Restore clipboard text if no one own the X/Wayland selection.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
restore_clipboard(ಶೂನ್ಯ)
{
    ಆದರೆ (clip_ನಕ್ಷತ್ರ_save != ಖಾಲಿ)
    {
	ಆದರೆ (!clip_gen_owner_exists(&clip_ನಕ್ಷತ್ರ))
	    put_ರಿಜಿಸ್ಟರ್('*', clip_ನಕ್ಷತ್ರ_save);
	ಇಲ್ಲದಿದ್ದರೆ
	    free_ರಿಜಿಸ್ಟರ್(clip_ನಕ್ಷತ್ರ_save);
	clip_ನಕ್ಷತ್ರ_save = ಖಾಲಿ;
    }
    ಆದರೆ (clip_plus_save != ಖಾಲಿ)
    {
	ಆದರೆ (!clip_gen_owner_exists(&clip_plus))
	    put_ರಿಜಿಸ್ಟರ್('+', clip_plus_save);
	ಇಲ್ಲದಿದ್ದರೆ
	    free_ರಿಜಿಸ್ಟರ್(clip_plus_save);
	clip_plus_save = ಖಾಲಿ;
    }
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

/*
 * If the machine has job control, use it to suspend the program,
 * otherwise fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    ಆದರೆ (ignore_sigtstp)
	ಹಿಂತಿರುಗಿ;

#ಆದರೆ defined(SIGTSTP)
    ಒಳ_mch_suspend = TRUE;

    ಹೊರಹೋಗು_flush();	    // needed to make cursor visible on some systems
    settmode(TMODE_COOK);
    ಹೊರಹೋಗು_flush();	    // needed to disable mouse on some systems

# ಆದರೆ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    loose_clipboard();
# endif
# ಆದರೆ defined(SIGCONT)
    sigcont_received = FALSE;
# endif

    kill(0, SIGTSTP);	    // send ourselves a STOP signal

# ಆದರೆ defined(SIGCONT)
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
	ದೀರ್ಘ wait_ಸಮಯ;

	ಗಾಗಿ (wait_ಸಮಯ = 0; !sigcont_received && wait_ಸಮಯ <= 3L; wait_ಸಮಯ++)
	    mch_delay(wait_ಸಮಯ, 0);
    }
# endif
    ಒಳ_mch_suspend = FALSE;

    after_sigcont();
#ಇಲ್ಲದಿದ್ದರೆ
    suspend_shell();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
    Columns = 80;
    ಅಡ್ಡಸಾಲುಗಳು = 24;

    ಹೊರಹೋಗು_flush();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTSTP
    // Check whether we were invoked with SIGTSTP set to be ignored. If it is
    // that indicates the shell (or program) that launched us does not support
    // tty job control and thus we should ignore that signal. If invoked as a
    // restricted editor (e.g., as "rvim") SIGTSTP is always ignored.
    ignore_sigtstp = restricted || SIG_IGN == mch_signal(SIGTSTP, SIG_ERR);
#ಆದರೆ_ಅಂತ್ಯ
    ಗಣ_ಸಂಕೇತಗಳು();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
    mac_conv_init();
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_CYGWIN_WIN32_CLIPBOARD
    win_clip_init();
#ಆದರೆ_ಅಂತ್ಯ
}

    ಶೂನ್ಯ
ಗಣ_sigwinch_handler(ಶೂನ್ಯ)
{
#ಆದರೆ defined(SIGWINCH)
    mch_signal(SIGWINCH, sig_winch);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಸಂಕೇತಗಳು(ಶೂನ್ಯ)
{
#ಆದರೆ defined(SIGWINCH)
    /*
     * WINDOW CHANGE signal is handled with sig_winch().
     */
    mch_signal(SIGWINCH, sig_winch);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTSTP
    // See mch_init() for the conditions under which we ignore SIGTSTP.
    // In the GUI default TSTP processing is OK.
    // Checking both gui.in_use and gui.starting because gui.in_use is not set
    // at this point (set after menus are displayed), but gui.starting is set.
    mch_signal(SIGTSTP, ignore_sigtstp ? SIG_IGN
# ifdef FEAT_GUI
				: gui.ಒಳ_use || gui.starting ? SIG_DFL
# endif
				    : sig_tstp);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(SIGCONT)
    mch_signal(SIGCONT, sigcont_handler);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGPIPE
    /*
     * We want to ignore breaking of PIPEs.
     */
    mch_signal(SIGPIPE, SIG_IGN);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGINT
    catch_int_signal();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGUSR1
    /*
     * Call user's handler on SIGUSR1
     */
    mch_signal(SIGUSR1, catch_sigusr1);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Ignore alarm signals (Perl's alarm() generates it).
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGALRM
    mch_signal(SIGALRM, SIG_IGN);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGPWR
    /*
     * Catch SIGPWR (power failure?) to preserve the swap files, so that no
     * work will be lost.
     */
    mch_signal(SIGPWR, catch_sigpwr);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Arrange for other signals to gracefully shutdown Vim.
     */
    catch_ಸಂಕೇತಗಳು(deathtrap, SIG_ERR);

#ಆದರೆ defined(FEAT_GUI) && defined(SIGHUP)
    /*
     * When the GUI is running, ignore the hangup signal.
     */
    ಆದರೆ (gui.ಒಳ_use)
	mch_signal(SIGHUP, SIG_IGN);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(SIGINT)
/*
 * Catch CTRL-C (only works while in Cooked mode).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
catch_int_signal(ಶೂನ್ಯ)
{
    mch_signal(SIGINT, catch_sigint);
}
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
reset_ಸಂಕೇತಗಳು(ಶೂನ್ಯ)
{
    catch_ಸಂಕೇತಗಳು(SIG_DFL, SIG_DFL);
#ಆದರೆ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    mch_signal(SIGCONT, SIG_DFL);
#ಆದರೆ_ಅಂತ್ಯ
}

    ಸ್ಥಿರ ಶೂನ್ಯ
catch_ಸಂಕೇತಗಳು(
    ಶೂನ್ಯ (*func_deadly)(ಪೂರ್ಣಾಂಕ),
    ಶೂನ್ಯ (*func_other)(ಪೂರ್ಣಾಂಕ))
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; signal_info[i].sig != -1; i++)
    {
	ಆದರೆ (signal_info[i].deadly)
	{
#ಆದರೆ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGACTION)
	    ರಚನೆ sigaction sa;

	    // Setup to use the alternate stack for the signal function.
	    sa.sa_handler = func_deadly;
	    sigemptyset(&sa.sa_mask);
# ಆದರೆ defined(__linux__) && defined(_REENTRANT)
	    // On Linux, with glibc compiled for kernel 2.2, there is a bug in
	    // thread handling in combination with using the alternate stack:
	    // pthread library functions try to use the stack pointer to
	    // identify the current thread, causing a SEGV signal, which
	    // recursively calls deathtrap() and hangs.
	    sa.sa_flags = 0;
# ಇಲ್ಲದಿದ್ದರೆ
	    sa.sa_flags = SA_ONSTACK;
# endif
	    sigaction(signal_info[i].sig, &sa, ಖಾಲಿ);
#ಇಲ್ಲದಿದ್ದರೆ
# ಆದರೆ defined(HAVE_SIGALTSTACK) && defined(HAVE_SIGVEC)
	    ರಚನೆ sigvec sv;

	    // Setup to use the alternate stack for the signal function.
	    sv.sv_handler = func_deadly;
	    sv.sv_ಮುಖವಾಡ = 0;
	    sv.sv_flags = SV_ONSTACK;
	    sigvec(signal_info[i].sig, &sv, ಖಾಲಿ);
# ಇಲ್ಲದಿದ್ದರೆ
	    mch_signal(signal_info[i].sig, func_deadly);
# endif
#ಆದರೆ_ಅಂತ್ಯ
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (func_other != SIG_ERR)
	{
	    // Deal with non-deadly signals.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGTSTP
	    mch_signal(signal_info[i].sig,
		    signal_info[i].sig == SIGTSTP && ignore_sigtstp
						       ? SIG_IGN : func_other);
#ಇಲ್ಲದಿದ್ದರೆ
	    mch_signal(signal_info[i].sig, func_other);
#ಆದರೆ_ಅಂತ್ಯ
	}
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SIGPROCMASK
    ಸ್ಥಿರ ಶೂನ್ಯ
ಖಂಡ_ಸಂಕೇತಗಳು_2(sigset_t *ಗಣ_2)
{
    sigset_t	newset;
    ಪೂರ್ಣಾಂಕ		i;

    sigemptyset(&newset);

    ಗಾಗಿ (i = 0; signal_info[i].sig != -1; i++)
	sigaddset(&newset, signal_info[i].sig);

# ಆದರೆ defined(SIGCONT)
    // SIGCONT isn't in the list, because its default action is ignore
    sigaddset(&newset, SIGCONT);
# endif

    sigprocmask(SIG_BLOCK, &newset, ಗಣ_2);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
unblock_ಸಂಕೇತಗಳು(sigset_t *ಗಣ_2)
{
    sigprocmask(SIG_SETMASK, ಗಣ_2, ಖಾಲಿ);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Handling of SIGHUP, SIGQUIT and SIGTERM:
 * "when" == a signal:       when busy, postpone and return FALSE, otherwise
 *			     return TRUE
 * "when" == SIGNAL_BLOCK:   Going to be busy, block signals
 * "when" == SIGNAL_UNBLOCK: Going to wait, unblock signals, use postponed
 *			     signal
 * Returns TRUE when Vim should exit.
 */
    ಪೂರ್ಣಾಂಕ
vim_handle_signal(ಪೂರ್ಣಾಂಕ sig)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ got_signal = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ blocked = TRUE;

    ಆಯ್ಕೆ (sig)
    {
	ಸ್ಥಿತಿ SIGNAL_ಖಂಡ:   blocked = TRUE;
			     ನಿಲ್ಲಿಸು;

	ಸ್ಥಿತಿ SIGNAL_UNBLOCK: blocked = FALSE;
			     ಆದರೆ (got_signal != 0)
			     {
				 kill(getpid(), got_signal);
				 got_signal = 0;
			     }
			     ನಿಲ್ಲಿಸು;

	ಪೂರ್ವನಿಯೋಜಿತ:	     ಆದರೆ (!blocked)
				 ಹಿಂತಿರುಗಿ TRUE;	// exit!
			     got_signal = sig;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGPWR
			     ಆದರೆ (sig != SIGPWR)
#ಆದರೆ_ಅಂತ್ಯ
				 got_int = TRUE;    // break any loops
			     ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Check_win checks whether we have an interactive stdout.
 */
    ಪೂರ್ಣಾಂಕ
mch_check_win(ಪೂರ್ಣಾಂಕ argc ಬಳಸದ, ಅಕ್ಷರ **argv ಬಳಸದ)
{
    ಆದರೆ (isatty(1))
	ಹಿಂತಿರುಗಿ OK;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
    ಆದರೆ (isatty(read_cmd_fd))
	ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11

# ಆದರೆ defined(ELAPSED_TIMEVAL)

/*
 * Give a message about the elapsed time for opening the X window.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xopen_ಸಂದೇಶ(ದೀರ್ಘ elapsed_msec)
{
    smsg(_("Opening the X display took %ld msec"), elapsed_msec);
}
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_X11)
/*
 * A few functions shared by X11 title and clipboard code.
 */

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	got_x_error = FALSE;

/*
 * X Error handler, otherwise X just exits!  (very rude) -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_error_handler(Display *dpy, XErrorEvent *error_event)
{
    XGet_Error_ಪಠ್ಯ(dpy, error_event->error_ಸಂಕೇತ, (ಅಕ್ಷರ *)IObuff, IOSIZE);
    STRCAT(IObuff, _("\nVim: Got X error\n"));

    // In the GUI we cannot print a message and continue, because no X calls
    // are allowed here (causes my system to hang).  Silently continuing seems
    // like the best alternative.  Do preserve files, in case we crash.
    ml_sync_all(FALSE, FALSE);

# ifdef FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
# endif
	ಸಂದೇಶ((ಅಕ್ಷರ *)IObuff);

    ಹಿಂತಿರುಗಿ 0;		// NOTREACHED
}

/*
 * Another X Error handler, just used to check for errors.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_error_check(Display *dpy ಬಳಸದ, XErrorEvent *error_event ಬಳಸದ)
{
    got_x_error = TRUE;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return TRUE when connection to the X server is desired.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_connect_ಗಮ್ಯ_server(ಶೂನ್ಯ)
{
    // No point in connecting if we are exiting or dying.
    ಆದರೆ (exiting || v_dying)
	ಹಿಂತಿರುಗಿ FALSE;

# ಆದರೆ defined(FEAT_CLIENTSERVER)
    ಆದರೆ (x_force_connect)
	ಹಿಂತಿರುಗಿ TRUE;
# endif
    ಆದರೆ (x_no_connect)
	ಹಿಂತಿರುಗಿ FALSE;

    // Check for a match with "exclude:" from 'clipboard'.
    ಆದರೆ (clip_exclude_prog != ಖಾಲಿ)
    {
	// Just in case we get called recursively, return FALSE.  This could
	// happen if vpeekc() is used while executing the prog and it causes a
	// related callback to be invoked.
	ಆದರೆ (regprog_ಒಳ_use(clip_exclude_prog))
	    ಹಿಂತಿರುಗಿ FALSE;

	ಆದರೆ (vim_regexec_prog(&clip_exclude_prog, FALSE, T_NAME, (colnr_T)0))
	    ಹಿಂತಿರುಗಿ FALSE;
    }
    ಹಿಂತಿರುಗಿ TRUE;
}

# ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
#  ಆದರೆ defined(USING_SETJMP)
/*
 * An X IO Error handler, used to catch error while opening the display.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_IOerror_check(Display *dpy ಬಳಸದ)
{
    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(lc_jump_env, 1);
#   ಆದರೆ defined(VMS) || defined(__CYGWIN__)
    ಹಿಂತಿರುಗಿ 0;  // avoid the compiler complains about missing return value
#   endif
}
#  endif

/*
 * An X IO Error handler, used to catch terminal errors.
 */
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ xterm_dpy_retry_ಎಣಿಕೆ = 0;

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
x_IOerror_handler(Display *dpy ಬಳಸದ)
{
    xterm_dpy = ಖಾಲಿ;
    xterm_dpy_retry_ಎಣಿಕೆ = 5;  // Try reconnecting five times
    x11_ಕಿಟಕಿ = 0;
    x11_display = ಖಾಲಿ;
    xterm_Shell = (Widget)0;

    // This function should not return, it causes exit().  Longjump instead.
    LONGJMP(x_jump_env, 1);
#  ಆದರೆ defined(VMS) || defined(__CYGWIN__)
    ಹಿಂತಿರುಗಿ 0;  // avoid the compiler complains about missing return value
#  endif
}

/*
 * If the X11 connection was lost try to restore it.
 * Helps when the X11 server was stopped and restarted while Vim was inactive
 * (e.g. through tmux).
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_restore_x11_clipboard(ಶೂನ್ಯ)
{
    // No point in restoring the connecting if we are exiting or dying.
    ಆದರೆ (!exiting && !v_dying && xterm_dpy_retry_ಎಣಿಕೆ > 0)
    {
	--xterm_dpy_retry_ಎಣಿಕೆ;

#  ifndef LESSTIF_ಆವೃತ್ತಿ
	// This has been reported to avoid Vim getting stuck.
	ಆದರೆ (app_context != (XtAppContext)ಖಾಲಿ)
	{
	    XtDestroyApplicationContext(app_context);
	    app_context = (XtAppContext)ಖಾಲಿ;
	    x11_display = ಖಾಲಿ; // freed by XtDestroyApplicationContext()
	}
#  endif

	ಸಿದ್ಧತೆ_ಪದ_clip();
	get_x11_title(FALSE);
    }
}

    ಶೂನ್ಯ
ex_xrestore(exarg_T *eap)
{
    size_t  arglen;

    ಆದರೆ (eap->arg != ಖಾಲಿ && (arglen = STRLEN(eap->arg)) > 0)
    {
	ಆದರೆ (xterm_display_allocated)
	    vim_free(xterm_display);
	xterm_display = (ಅಕ್ಷರ *)vim_strnsave(eap->arg, arglen);
	xterm_display_allocated = TRUE;
    }
    smsg(_("restoring X11 display %s"), xterm_display == ಖಾಲಿ
		    ? (ಅಕ್ಷರ *)mch_getenv((char_u *)"DISPLAY") : xterm_display);

    clear_xterm_clip();
    x11_ಕಿಟಕಿ = 0;
    xterm_dpy_retry_ಎಣಿಕೆ = 5;  // Try reconnecting five times
    may_restore_x11_clipboard();
    ಆರಿಸು_clipmethod();
}
# endif

/*
 * Test if "dpy" and x11_window are valid by getting the window title.
 * I don't actually want it yet, so there may be a simpler call to use, but
 * this will cause the error handler x_error_check() to be called if anything
 * is wrong, such as the window pointer being invalid (as can happen when the
 * user changes his DISPLAY, but not his WINDOWID) -- webb
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
test_x11_ಕಿಟಕಿ(Display *dpy)
{
    ಪೂರ್ಣಾಂಕ			(*old_handler)(Display*, XErrorEvent*);
    XTextProperty	ಪಠ್ಯ_prop;

    old_handler = XSetErrorHandler(x_error_check);
    got_x_error = FALSE;
    ಆದರೆ (XGetWMName(dpy, x11_ಕಿಟಕಿ, &ಪಠ್ಯ_prop))
	XFree((ಶೂನ್ಯ *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
    XSync(dpy, False);
    (ಶೂನ್ಯ)XSetErrorHandler(old_handler);

    ಆದರೆ (p_verbose > 0 && got_x_error)
	verb_ಸಂದೇಶ(_("Testing the X display failed"));

    ಹಿಂತಿರುಗಿ (got_x_error ? FAIL : OK);
}
#ಆದರೆ_ಅಂತ್ಯ


#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ get_x11_thing(ಪೂರ್ಣಾಂಕ get_title, ಪೂರ್ಣಾಂಕ test_only);

/*
 * try to get x11 window and display
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_windis(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ	    *winid;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    ಫಲಿತಾಂಶ = -1;
# define XD_NONE	 0	// x11_display not set here
# define XD_ಇಲ್ಲಿ	 1	// x11_display opened here
# define XD_GUI	 2	// x11_display used from gui.dpy
# define XD_XTERM 3	// x11_display used from xterm_dpy
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    x11_display_from = XD_NONE;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	    did_ಗಣ_error_handler = FALSE;

    ಆದರೆ (!did_ಗಣ_error_handler)
    {
	// X just exits if it finds an error otherwise!
	(ಶೂನ್ಯ)XSetErrorHandler(x_error_handler);
	did_ಗಣ_error_handler = TRUE;
    }

# ಆದರೆ defined(FEAT_GUI_X11) || defined(FEAT_GUI_GTK)
    ಆದರೆ (gui.ಒಳ_use)
    {
	/*
	 * If the X11 display was opened here before, for the window where Vim
	 * was started, close that one now to avoid a memory leak.
	 */
	ಆದರೆ (x11_display_from == XD_ಇಲ್ಲಿ && x11_display != ಖಾಲಿ)
	{
	    XCloseDisplay(x11_display);
	    x11_display_from = XD_NONE;
	}
	ಆದರೆ (gui_get_x11_windis(&x11_ಕಿಟಕಿ, &x11_display) == OK)
	{
	    x11_display_from = XD_GUI;
	    ಹಿಂತಿರುಗಿ OK;
	}
	x11_display = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x11_display_from == XD_GUI)
    {
	// GUI must have stopped somehow, clear x11_display
	x11_ಕಿಟಕಿ = 0;
	x11_display = ಖಾಲಿ;
	x11_display_from = XD_NONE;
    }
# endif

    // When started with the "-X" argument, don't try connecting.
    ಆದರೆ (!x_connect_ಗಮ್ಯ_server())
	ಹಿಂತಿರುಗಿ FAIL;

    /*
     * If WINDOWID not set, should try another method to find out
     * what the current window number is. The only code I know for
     * this is very complicated.
     * We assume that zero is invalid for WINDOWID.
     */
    ಆದರೆ (x11_ಕಿಟಕಿ == 0 && (winid = getenv("WINDOWID")) != ಖಾಲಿ)
	x11_ಕಿಟಕಿ = (ಕಿಟಕಿ)atol(winid);

# ifdef FEAT_XCLIPBOARD
    ಆದರೆ (xterm_dpy == x11_display)
	// x11_display may have been set to xterm_dpy elsewhere
	x11_display_from = XD_XTERM;

    ಆದರೆ (xterm_dpy != ಖಾಲಿ && x11_ಕಿಟಕಿ != 0)
    {
	// We may have checked it already, but Gnome terminal can move us to
	// another window, so we need to check every time.
	ಆದರೆ (x11_display_from != XD_XTERM)
	{
	    /*
	     * If the X11 display was opened here before, for the window where
	     * Vim was started, close that one now to avoid a memory leak.
	     */
	    ಆದರೆ (x11_display_from == XD_ಇಲ್ಲಿ && x11_display != ಖಾಲಿ)
		XCloseDisplay(x11_display);
	    x11_display = xterm_dpy;
	    x11_display_from = XD_XTERM;
	}
	ಆದರೆ (test_x11_ಕಿಟಕಿ(x11_display) == FAIL)
	{
	    // probably bad $WINDOWID
	    x11_ಕಿಟಕಿ = 0;
	    x11_display = ಖಾಲಿ;
	    x11_display_from = XD_NONE;
	    ಹಿಂತಿರುಗಿ FAIL;
	}
	ಹಿಂತಿರುಗಿ OK;
    }
# endif

    ಆದರೆ (x11_ಕಿಟಕಿ == 0 || x11_display == ಖಾಲಿ)
	ಫಲಿತಾಂಶ = -1;

    ಆದರೆ (ಫಲಿತಾಂಶ != -1)	    // Have already been here and set this
	ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;	    // Don't do all these X calls again

    ಆದರೆ (x11_ಕಿಟಕಿ != 0 && x11_display == ಖಾಲಿ)
    {
# ifdef ಗಣ_SIG_ALARM
	sighandler_T sig_save;
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ಆರಂಭ_tv;

	ಆದರೆ (p_verbose > 0)
	    ELAPSED_INIT(ಆರಂಭ_tv);
# endif

# ifdef ಗಣ_SIG_ALARM
	/*
	 * Opening the Display may hang if the DISPLAY setting is wrong, or
	 * the network connection is bad.  Set an alarm timer to get out.
	 */
	sig_alarm_called = FALSE;
	sig_save = mch_signal(SIGALRM, sig_alarm);
	alarm(2);
# endif
	x11_display = XOpenDisplay(ಖಾಲಿ);

# ifdef ಗಣ_SIG_ALARM
	alarm(0);
	mch_signal(SIGALRM, sig_save);
	ಆದರೆ (p_verbose > 0 && sig_alarm_called)
	    verb_ಸಂದೇಶ(_("Opening the X display timed out"));
# endif
	ಆದರೆ (x11_display != ಖಾಲಿ)
	{
# ifdef ELAPSED_FUNC
	    ಆದರೆ (p_verbose > 0)
	    {
		verbose_enter();
		xopen_ಸಂದೇಶ(ELAPSED_FUNC(ಆರಂಭ_tv));
		verbose_leave();
	    }
# endif
	    ಆದರೆ (test_x11_ಕಿಟಕಿ(x11_display) == FAIL)
	    {
		// Maybe window id is bad
		x11_ಕಿಟಕಿ = 0;
		XCloseDisplay(x11_display);
		x11_display = ಖಾಲಿ;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		x11_display_from = XD_ಇಲ್ಲಿ;
	}
    }
    ಆದರೆ (x11_ಕಿಟಕಿ == 0 || x11_display == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ = FAIL);

# ifdef FEAT_EVAL
    ಗಣ_vim_var_nr(VV_WINDOWID, (ದೀರ್ಘ)x11_ಕಿಟಕಿ);
# endif

    ಹಿಂತಿರುಗಿ (ಫಲಿತಾಂಶ = OK);
}

/*
 * Determine original x11 Window Title
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_title(ಪೂರ್ಣಾಂಕ test_only)
{
    ಹಿಂತಿರುಗಿ get_x11_thing(TRUE, test_only);
}

/*
 * Determine original x11 Window icon
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_icon(ಪೂರ್ಣಾಂಕ test_only)
{
    ಪೂರ್ಣಾಂಕ		retval = FALSE;

    retval = get_x11_thing(FALSE, test_only);

    // could not get old icon, use terminal name
    ಆದರೆ (oldicon == ಖಾಲಿ && !test_only)
    {
	ಆದರೆ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ಇಲ್ಲದಿದ್ದರೆ
	    oldicon = vim_strsave(T_NAME);
    }

    ಹಿಂತಿರುಗಿ retval;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_thing(
    ಪೂರ್ಣಾಂಕ		get_title,	// get title string
    ಪೂರ್ಣಾಂಕ		test_only)
{
    XTextProperty	ಪಠ್ಯ_prop;
    ಪೂರ್ಣಾಂಕ			retval = FALSE;
    ಸ್ಥಿತಿ_3		ಸ್ಥಿತಿ_2;

    ಆದರೆ (get_x11_windis() != OK)
	ಹಿಂತಿರುಗಿ FALSE;

    // Get window/icon name if any
    ಆದರೆ (get_title)
	ಸ್ಥಿತಿ_2 = XGetWMName(x11_display, x11_ಕಿಟಕಿ, &ಪಠ್ಯ_prop);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = XGetWMIconName(x11_display, x11_ಕಿಟಕಿ, &ಪಠ್ಯ_prop);

    /*
     * If terminal is xterm, then x11_window may be a child window of the
     * outer xterm window that actually contains the window/icon name, so
     * keep traversing up the tree until a window with a title/icon is
     * found.
     */
    // Previously this was only done for xterm and alike.  I don't see a
    // reason why it would fail for other terminal emulators.
    // if (term_is_xterm)
    ಕಿಟಕಿ	    ಬೇರು;
    ಕಿಟಕಿ	    ಪೋಷಕ;
    ಕಿಟಕಿ	    win = x11_ಕಿಟಕಿ;
    ಕಿಟಕಿ	   *children;
    ಚಿಹ್ನೆರಹಿತ ಪೂರ್ಣಾಂಕ    ಅಂಕೆ_children;

    ಆಗ (!ಸ್ಥಿತಿ_2 || ಪಠ್ಯ_prop.ಮೌಲ್ಯ == ಖಾಲಿ)
    {
	ಆದರೆ (!XQuery_ವೃಕ್ಷ(x11_display, win, &ಬೇರು, &ಪೋಷಕ, &children,
		    &ಅಂಕೆ_children))
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (children)
	    XFree((ಶೂನ್ಯ *)children);
	ಆದರೆ (ಪೋಷಕ == ಬೇರು || ಪೋಷಕ == 0)
	    ನಿಲ್ಲಿಸು;

	win = ಪೋಷಕ;
	ಆದರೆ (get_title)
	    ಸ್ಥಿತಿ_2 = XGetWMName(x11_display, win, &ಪಠ್ಯ_prop);
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸ್ಥಿತಿ_2 = XGetWMIconName(x11_display, win, &ಪಠ್ಯ_prop);
    }

    ಆದರೆ (ಸ್ಥಿತಿ_2 && ಪಠ್ಯ_prop.ಮೌಲ್ಯ != ಖಾಲಿ)
    {
	retval = TRUE;
	ಆದರೆ (!test_only)
	{
	    ಆದರೆ (get_title)
		vim_free(oldtitle);
	    ಇಲ್ಲದಿದ್ದರೆ
		vim_free(oldicon);
	    ಆದರೆ (ಪಠ್ಯ_prop.encoding == XA_STRING && !has_mbyte)
	    {
		ಆದರೆ (get_title)
		    oldtitle = vim_strsave((char_u *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
		ಇಲ್ಲದಿದ್ದರೆ
		    oldicon = vim_strsave((char_u *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ಅಕ್ಷರ    **cl;
		ಸ್ಥಿತಿ_3  transform_ಸ್ಥಿತಿ;
		ಪೂರ್ಣಾಂಕ	    n = 0;

		transform_ಸ್ಥಿತಿ = Xmb_ಪಠ್ಯ_Property_ಗಮ್ಯ_ಪಠ್ಯ_List(x11_display,
			&ಪಠ್ಯ_prop,
			&cl, &n);
		ಆದರೆ (transform_ಸ್ಥಿತಿ >= Success && n > 0 && cl[0])
		{
		    ಆದರೆ (get_title)
			oldtitle = vim_strsave((char_u *) cl[0]);
		    ಇಲ್ಲದಿದ್ದರೆ
			oldicon = vim_strsave((char_u *) cl[0]);
		    XFreeStringList(cl);
		}
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    ಆದರೆ (get_title)
			oldtitle = vim_strsave((char_u *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
		    ಇಲ್ಲದಿದ್ದರೆ
			oldicon = vim_strsave((char_u *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
		}
	    }
	}
	XFree((ಶೂನ್ಯ *)ಪಠ್ಯ_prop.ಮೌಲ್ಯ);
    }
    ಹಿಂತಿರುಗಿ retval;
}

// Xutf8 functions are not available on older systems. Note that on some
// systems X_HAVE_UTF8_STRING may be defined in a header file but
// Xutf8SetWMProperties() is not in the X11 library.  Configure checks for
// that and defines HAVE_XUTF8SETWMPROPERTIES.
# ಆದರೆ defined(X_HAVE_UTF8_STRING)
#  ಆದರೆ X_HAVE_UTF8_STRING && HAVE_XUTF8SETWMPROPERTIES
#   define USE_UTF8_STRING
#  endif
# endif

/*
 * Set x11 Window Title
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_x11_title(char_u *title)
{
	// XmbSetWMProperties() and Xutf8SetWMProperties() should use a STRING
	// when possible, COMPOUND_TEXT otherwise.  COMPOUND_TEXT isn't
	// supported everywhere and STRING doesn't work for multi-byte titles.
# ifdef USE_UTF8_STRING
    ಆದರೆ (enc_utf8)
	Xutf8_ಗಣ_WMProperties(x11_display, x11_ಕಿಟಕಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title,
					     ಖಾಲಿ, ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
# ಆದರೆ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_ಗಣ_WMProperties(x11_display, x11_ಕಿಟಕಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)title,
					     ಖಾಲಿ, ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#  ಇಲ್ಲದಿದ್ದರೆ
	XTextProperty	ಪಠ್ಯ_prop;
	ಅಕ್ಷರ		*c_title = (ಅಕ್ಷರ *)title;

	// directly from example 3-18 "basicwin" of Xlib Programming Manual
	(ಶೂನ್ಯ)XString_List_ಗಮ್ಯ_ಪಠ್ಯ_Property(&c_title, 1, &ಪಠ್ಯ_prop);
	XSetWMProperties(x11_display, x11_ಕಿಟಕಿ, &ಪಠ್ಯ_prop,
					     ಖಾಲಿ, ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
	XStoreName(x11_display, x11_ಕಿಟಕಿ, (ಅಕ್ಷರ *)title);
# endif
    }
    XFlush(x11_display);
}

/*
 * Set x11 Window icon
 *
 * get_x11_windis() must be called before this and have returned OK
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_x11_icon(char_u *icon)
{
    // See above for comments about using X*SetWMProperties().
# ifdef USE_UTF8_STRING
    ಆದರೆ (enc_utf8)
	Xutf8_ಗಣ_WMProperties(x11_display, x11_ಕಿಟಕಿ, ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)icon,
						   ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
# ಆದರೆ XtSpecificationRelease >= 4
#  ifdef FEAT_XFONTSET
	Xmb_ಗಣ_WMProperties(x11_display, x11_ಕಿಟಕಿ, ಖಾಲಿ, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)icon,
						   ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#  ಇಲ್ಲದಿದ್ದರೆ
	XTextProperty	ಪಠ್ಯ_prop;
	ಅಕ್ಷರ		*c_icon = (ಅಕ್ಷರ *)icon;

	(ಶೂನ್ಯ)XString_List_ಗಮ್ಯ_ಪಠ್ಯ_Property(&c_icon, 1, &ಪಠ್ಯ_prop);
	XSetWMProperties(x11_display, x11_ಕಿಟಕಿ, ಖಾಲಿ, &ಪಠ್ಯ_prop,
						   ಖಾಲಿ, 0, ಖಾಲಿ, ಖಾಲಿ, ಖಾಲಿ);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
	XSetIconName(x11_display, x11_ಕಿಟಕಿ, (ಅಕ್ಷರ *)icon);
# endif
    }
    XFlush(x11_display);
}

#ಇಲ್ಲದಿದ್ದರೆ  // FEAT_X11

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_title(ಪೂರ್ಣಾಂಕ test_only ಬಳಸದ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_x11_icon(ಪೂರ್ಣಾಂಕ test_only)
{
    ಆದರೆ (!test_only)
    {
	ಆದರೆ (STRNCMP(T_NAME, "builtin_", 8) == 0)
	    oldicon = vim_strsave(T_NAME + 8);
	ಇಲ್ಲದಿದ್ದರೆ
	    oldicon = vim_strsave(T_NAME);
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ_ಅಂತ್ಯ // FEAT_X11

    ಪೂರ್ಣಾಂಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_x11_title(TRUE);
}

    ಪೂರ್ಣಾಂಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ get_x11_icon(TRUE);
}

/*
 * Set the window title and icon.
 */
    ಶೂನ್ಯ
mch_settitle(char_u *title, char_u *icon)
{
    ಪೂರ್ಣಾಂಕ		type = 0;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	recursive = 0;

    ಆದರೆ (T_NAME == ಖಾಲಿ)	    // no terminal name (yet)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (title == ಖಾಲಿ && icon == ಖಾಲಿ)	    // nothing to do
	ಹಿಂತಿರುಗಿ;

    // When one of the X11 functions causes a deadly signal, we get here again
    // recursively.  Avoid hanging then (something is probably locked).
    ಆದರೆ (recursive)
	ಹಿಂತಿರುಗಿ;
    ++recursive;

    /*
     * if the window ID and the display is known, we may use X11 calls
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11
    ಆದರೆ (get_x11_windis() == OK)
	type = 1;
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_PHOTON) \
    || defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU)
    ಆದರೆ (gui.ಒಳ_use)
	type = 1;
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Note: if "t_ts" is set, title is set with escape sequence rather
     *	     than x11 calls, because the x11 calls don't always work
     */
    ಆದರೆ ((type || *T_TS != NUL) && title != ಖಾಲಿ)
    {
	ಆದರೆ (oldtitle_outdated)
	{
	    oldtitle_outdated = FALSE;
	    VIM_CLEAR(oldtitle);
	}
	ಆದರೆ (oldtitle == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		)		// first call but not in GUI, save title
	    (ಶೂನ್ಯ)get_x11_title(FALSE);

	ಆದರೆ (*T_TS != NUL)		// it's OK if t_fs is empty
	    ಪದ_settitle(title);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11
	ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_GUI_GTK
	ಆದರೆ (!gui.ಒಳ_use)		// don't do this if GTK+ is running
# endif
	    ಗಣ_x11_title(title);		// x11
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI_GTK) || defined(FEAT_GUI_HAIKU) \
	|| defined(FEAT_GUI_PHOTON)
	ಇಲ್ಲದಿದ್ದರೆ
	    gui_mch_settitle(title, icon);
#ಆದರೆ_ಅಂತ್ಯ
	unix_did_ಗಣ_title = TRUE;
    }

    ಆದರೆ ((type || *T_CIS != NUL) && icon != ಖಾಲಿ)
    {
	ಆದರೆ (oldicon == ಖಾಲಿ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
		&& !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
		)		// first call, save icon
	    get_x11_icon(FALSE);

	ಆದರೆ (*T_CIS != NUL)
	{
	    ಹೊರಹೋಗು_str(T_CIS);			// set icon start
	    ಹೊರಹೋಗು_str_nf(icon);
	    ಹೊರಹೋಗು_str(T_CIE);			// set icon end
	    ಹೊರಹೋಗು_flush();
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_X11
	ಇಲ್ಲದಿದ್ದರೆ
# ifdef FEAT_GUI_GTK
	ಆದರೆ (!gui.ಒಳ_use)		// don't do this if GTK+ is running
# endif
	    ಗಣ_x11_icon(icon);			// x11
#ಆದರೆ_ಅಂತ್ಯ
	did_ಗಣ_icon = TRUE;
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
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಂಕ which)
{
    ಪೂರ್ಣಾಂಕ	do_ತಳ್ಳು_ತೆಗೆ = unix_did_ಗಣ_title || did_ಗಣ_icon;

    // Only restore the title or icon when it has been set.
    // When using "oldtitle" make a copy, it might be freed halfway.
    char_u *title = ((which & SAVE_RESTORE_TITLE) && unix_did_ಗಣ_title)
			? (oldtitle ? oldtitle : p_titleold) : ಖಾಲಿ;
    char_u *tofree = ಖಾಲಿ;
    ಆದರೆ (title == oldtitle && oldtitle != ಖಾಲಿ)
    {
	tofree = vim_strsave(title);
	ಆದರೆ (tofree != ಖಾಲಿ)
	    title = tofree;
    }
    mch_settitle(title,
	       ((which & SAVE_RESTORE_ICON) && did_ಗಣ_icon) ? oldicon : ಖಾಲಿ);
    vim_free(tofree);

    ಆದರೆ (do_ತಳ್ಳು_ತೆಗೆ)
    {
	// pop and push from/to the stack
	ಪದ_ತೆಗೆ_title(which);
	ಪದ_ತಳ್ಳು_title(which);
    }
}


/*
 * Return TRUE if "name" looks like some xterm name.
 * This matches "xterm.*", thus "xterm-256color", "xterm-kitty", etc.
 * Do not consider "xterm-kitty" an xterm, it is not fully xterm compatible,
 * using the "xterm-kitty" terminfo entry should work better.
 * Seiichi Sato mentioned that "mlterm" works like xterm.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_xterm(char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ ((STRNICMP(name, "xterm", 5) == 0
				     && STRNICMP(name, "xterm-kitty", 11) != 0)
		|| STRNICMP(name, "nxterm", 6) == 0
		|| STRNICMP(name, "kterm", 5) == 0
		|| STRNICMP(name, "mlterm", 6) == 0
		|| STRNICMP(name, "rxvt", 4) == 0
		|| STRNICMP(name, "screen.xterm", 12) == 0
		|| STRCMP(name, "builtin_xterm") == 0);
}

#ಆದರೆ defined(FEAT_MOUSE_XTERM)
/*
 * Return TRUE if "name" appears to be that of a terminal
 * known to support the xterm-style mouse protocol.
 * Relies on term_is_xterm having been set to its correct value.
 */
    ಪೂರ್ಣಾಂಕ
use_xterm_like_mouse(char_u *name)
{
    ಹಿಂತಿರುಗಿ (name != ಖಾಲಿ
	    && (ಪದ_is_xterm
		|| STRNICMP(name, "screen", 6) == 0
		|| STRNICMP(name, "tmux", 4) == 0
		|| STRNICMP(name, "gnome", 5) == 0
		|| STRICMP(name, "st") == 0
		|| STRNICMP(name, "st-", 3) == 0
		|| STRNICMP(name, "stterm", 6) == 0));
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Return non-zero when using an xterm mouse, according to 'ttymouse'.
 * Return 1 for "xterm".
 * Return 2 for "xterm2".
 * Return 3 for "urxvt".
 * Return 4 for "sgr".
 */
    ಪೂರ್ಣಾಂಕ
use_xterm_mouse(ಶೂನ್ಯ)
{
    ಆದರೆ (ttym_flags == TTYM_SGR)
	ಹಿಂತಿರುಗಿ 4;
    ಆದರೆ (ttym_flags == TTYM_URXVT)
	ಹಿಂತಿರುಗಿ 3;
    ಆದರೆ (ttym_flags == TTYM_XTERM2)
	ಹಿಂತಿರುಗಿ 2;
    ಆದರೆ (ttym_flags == TTYM_XTERM)
	ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return TRUE if "name" is an iris-ansi terminal name.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_iris(char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ (STRNICMP(name, "iris-ansi", 9) == 0
	    || STRCMP(name, "builtin_iris-ansi") == 0);
}

#ಆದರೆ defined(VMS)
/*
 * Return TRUE if "name" is a vt300-like terminal name.
 */
    ಪೂರ್ಣಾಂಕ
vim_is_vt300(char_u *name)
{
    ಆದರೆ (name == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;
    // Actually all ANSI compatible terminals should be here.
    // Catch at least VT1xx - VT5xx
    ಹಿಂತಿರುಗಿ ((STRNICMP(name, "vt", 2) == 0
			     && vim_strchr((char_u *)"12345", name[2]) != ಖಾಲಿ)
	    || STRCMP(name, "builtin_vt320") == 0);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Insert user name in s[len].
 * Return OK if a name found.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_user_name(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    vim_strncpy(s, (char_u *)cuserid(ಖಾಲಿ), len - 1);
    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ mch_get_uname(getuid(), s, len);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Insert user name for "uid" in s[len].
 * Return OK if a name found.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_uname(uid_t uid, char_u *s, ಪೂರ್ಣಾಂಕ len)
{
#ಆದರೆ defined(HAVE_PWD_H) && defined(HAVE_GETPWUID)
    ರಚನೆ passwd   *pw;

    ಆದರೆ ((pw = getpwuid(uid)) != ಖಾಲಿ
	    && pw->pw_name != ಖಾಲಿ && *(pw->pw_name) != NUL)
    {
	vim_strncpy(s, (char_u *)pw->pw_name, len - 1);
	ಹಿಂತಿರುಗಿ OK;
    }
#ಆದರೆ_ಅಂತ್ಯ
    sprintf((ಅಕ್ಷರ *)s, "%d", (ಪೂರ್ಣಾಂಕ)uid);	    // assumes s is long enough
    ಹಿಂತಿರುಗಿ FAIL;			    // a number is not a name
}

/*
 * Insert host name is s[len].
 */

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_SYS_UTSNAME_H
    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
    ರಚನೆ utsname vutsname;

    ಆದರೆ (uname(&vutsname) < 0)
	*s = NUL;
    ಇಲ್ಲದಿದ್ದರೆ
	vim_strncpy(s, (char_u *)vutsname.nodename, len - 1);
}
#ಇಲ್ಲದಿದ್ದರೆ // HAVE_SYS_UTSNAME_H

# ifdef HAVE_SYS_SYSTEMINFO_H
#  define gethostname(nam, len) sysinfo(SI_HOSTNAME, nam, len)
# endif

    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
# ifdef VAXC
    vaxc$gethostname((ಅಕ್ಷರ *)s, len);
# ಇಲ್ಲದಿದ್ದರೆ
    gethostname((ಅಕ್ಷರ *)s, len);
# endif
    s[len - 1] = NUL;	// make sure it's terminated
}
#ಆದರೆ_ಅಂತ್ಯ // HAVE_SYS_UTSNAME_H

/*
 * return process ID
 */
    ದೀರ್ಘ
mch_get_pid(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)getpid();
}

/*
 * return TRUE if process "pid" is still running
 */
    ಪೂರ್ಣಾಂಕ
mch_process_running(ದೀರ್ಘ pid)
{
    // If there is no error the process must be running.
    ಆದರೆ (kill(pid, 0) == 0)
	ಹಿಂತಿರುಗಿ TRUE;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ESRCH
    // If the error is ESRCH then the process is not running.
    ಆದರೆ (errno == ESRCH)
	ಹಿಂತಿರುಗಿ FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    // If the process is running and owned by another user we get EPERM.  With
    // other errors the process might be running, assuming it is then.
    ಹಿಂತಿರುಗಿ TRUE;
}

#ಆದರೆ !defined(HAVE_STRERROR) && defined(USE_GETCWD)
    ಸ್ಥಿರ ಅಕ್ಷರ *
strerror(ಪೂರ್ಣಾಂಕ err)
{
    ಬಾಹ್ಯ ಪೂರ್ಣಾಂಕ	    sys_nerr;
    ಬಾಹ್ಯ ಅಕ್ಷರ	    *sys_errlist[];
    ಸ್ಥಿರ ಅಕ್ಷರ	    er[20];

    ಆದರೆ (err > 0 && err < sys_nerr)
	ಹಿಂತಿರುಗಿ (sys_errlist[err]);
    snprintf(er, ಗಾತ್ರ(er), "Error %d", err);
    ಹಿಂತಿರುಗಿ er;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get name of current directory into buffer "buf" of length "len" bytes.
 * "len" must be at least PATH_MAX.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_dirname(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
#ಆದರೆ defined(USE_GETCWD)
    ಆದರೆ (getcwd((ಅಕ್ಷರ *)buf, len) == ಖಾಲಿ)
    {
	STRCPY(buf, strerror(errno));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (getwd((ಅಕ್ಷರ *)buf) != ಖಾಲಿ ? OK : FAIL);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Get absolute file name into "buf[len]".
 *
 * return FAIL for failure, OK for success
 */
    ಪೂರ್ಣಾಂಕ
mch_ತುಂಬಿದ_Name(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		force)		// also expand when already absolute path
{
    ಪೂರ್ಣಾಂಕ		buflen = 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHDIR
    ಪೂರ್ಣಾಂಕ		fd = -1;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	dont_fchdir = FALSE;	// TRUE when fchdir() doesn't work
#ಆದರೆ_ಅಂತ್ಯ
    char_u	olddir[MAXPATHL];
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		retval = OK;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
    char_u	posix_fname[MAXPATHL];	// Cygwin docs mention MAX_PATH, but
					// it's not always defined
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    fname = vms_fixfilename(fname);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __CYGWIN__
    /*
     * This helps for when "/etc/hosts" is a symlink to "c:/something/hosts".
     */
# ಆದರೆ CYGWIN_ಆವೃತ್ತಿ_DLL_MAJOR >= 1007
    // Use CCP_RELATIVE to avoid that it sometimes returns a path that ends in
    // a forward slash.
    cygwin_conv_ಮಾರ್ಗ(CCP_WIN_A_ಗಮ್ಯ_POSIX | CCP_RELATIVE,
		     fname, posix_fname, MAXPATHL);
# ಇಲ್ಲದಿದ್ದರೆ
    cygwin_conv_ಗಮ್ಯ_posix_ಮಾರ್ಗ(fname, posix_fname);
# endif
    fname = posix_fname;
#ಆದರೆ_ಅಂತ್ಯ

    // Expand it if forced or not an absolute path.
    // Do not do it for "/file", the result is always "/".
    ಆದರೆ ((force || !mch_is_ತುಂಬಿದ_Name(fname))
	    && ((p = vim_strrchr(fname, '/')) == ಖಾಲಿ || p != fname))
    {
	ಆದರೆ (p == ಖಾಲಿ && STRCMP(fname, "..") == 0)
	    // Handle ".." without path separators.
	    p = fname + 2;
	/*
	 * If the file name has a path, change to that directory for a moment,
	 * and then get the directory (and get back to where we were).
	 * This will get the correct path name with "../" things.
	 */
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಆದರೆ (STRCMP(p, "/..") == 0)
		// For "/path/dir/.." include the "/..".
		p += 3;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHDIR
	    /*
	     * Use fchdir() if possible, it's said to be faster and more
	     * reliable.  But on SunOS 4 it might not work.  Check this by
	     * doing a fchdir() right now.
	     */
	    ಆದರೆ (!dont_fchdir)
	    {
		fd = open(".", O_RDONLY | O_EXTRA, 0);
		ಆದರೆ (fd >= 0 && fchdir(fd) < 0)
		{
		    close(fd);
		    fd = -1;
		    dont_fchdir = TRUE;	    // don't try again
		}
	    }
#ಆದರೆ_ಅಂತ್ಯ

	    // Only change directory when we are sure we can return to where
	    // we are now.  After doing "su" chdir(".") might not work.
	    ಆದರೆ (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHDIR
		fd < 0 &&
#ಆದರೆ_ಅಂತ್ಯ
			(mch_dirname(olddir, MAXPATHL) == FAIL
					   || mch_chdir((ಅಕ್ಷರ *)olddir) != 0))
	    {
		p = ಖಾಲಿ;	// can't get current dir: don't chdir
		retval = FAIL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		// The directory is copied into buf[], to be able to remove
		// the file name without changing it (could be a string in
		// read-only memory)
		ಆದರೆ (p - fname >= len)
		    retval = FAIL;
		ಇಲ್ಲದಿದ್ದರೆ
		{
		    vim_strncpy(buf, fname, p - fname);
		    ಆದರೆ (mch_chdir((ಅಕ್ಷರ *)buf))
		    {
			// Path does not exist (yet).  For a full path fail,
			// will use the path as-is.  For a relative path use
			// the current directory and append the file name.
			ಆದರೆ (mch_is_ತುಂಬಿದ_Name(fname))
			    retval = FAIL;
			ಇಲ್ಲದಿದ್ದರೆ
			    p = ಖಾಲಿ;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (*p == '/')
			fname = p + 1;
		    ಇಲ್ಲದಿದ್ದರೆ
			fname = p;
		    *buf = NUL;
		}
	    }
	}
	ಆದರೆ (mch_dirname(buf, len) == FAIL)
	{
	    retval = FAIL;
	    *buf = NUL;
	}
	ಆದರೆ (p != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ	l;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHDIR
	    ಆದರೆ (fd >= 0)
	    {
		ಆದರೆ (p_verbose >= 5)
		{
		    verbose_enter();
		    ಸಂದೇಶ("fchdir() to previous dir");
		    verbose_leave();
		}
		l = fchdir(fd);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
		l = mch_chdir((ಅಕ್ಷರ *)olddir);
	    ಆದರೆ (l != 0)
		emsg(_(e_cannot_go_ಹಿಂಭಾಗ_ಗಮ್ಯ_ಹಿಂದಿನ_directory));
	}
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_FCHDIR
	ಆದರೆ (fd >= 0)
	    close(fd);
#ಆದರೆ_ಅಂತ್ಯ

	buflen = (ಪೂರ್ಣಾಂಕ)STRLEN(buf);
	ಆದರೆ (buflen >= len - 1)
	    retval = FAIL; // no space for trailing "/"
#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (buflen > 0 && buf[buflen - 1] != PATHSEP && *fname != NUL
						   && STRCMP(fname, ".") != 0)
	{
	    STRCPY(buf + buflen, PATHSEPSTR);
	    buflen += ಗಾತ್ರ(PATHSEP);
	}
#ಆದರೆ_ಅಂತ್ಯ
    }

    ಆದರೆ (buflen == 0)
	buflen = (ಪೂರ್ಣಾಂಕ)STRLEN(buf);

    // Catch file names which are too long.
    ಆದರೆ (retval == FAIL || (ಪೂರ್ಣಾಂಕ)(buflen + STRLEN(fname)) >= len)
	ಹಿಂತಿರುಗಿ FAIL;

    // Do not append ".", "/dir/." is equal to "/dir".
    ಆದರೆ (STRCMP(fname, ".") != 0)
	STRCPY(buf + buflen, fname);

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_ತುಂಬಿದ_Name(char_u *fname)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಹಿಂತಿರುಗಿ ( fname[0] == '/'	       || fname[0] == '.'	    ||
	     strchr((ಅಕ್ಷರ *)fname,':') || strchr((ಅಕ್ಷರ *)fname,'"') ||
	    (strchr((ಅಕ್ಷರ *)fname,'[') && strchr((ಅಕ್ಷರ *)fname,']'))||
	    (strchr((ಅಕ್ಷರ *)fname,'<') && strchr((ಅಕ್ಷರ *)fname,'>'))   );
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (*fname == '/' || *fname == '~');
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(USE_FNAME_CASE)
/*
 * Set the case of the file name, if it already exists.  This will cause the
 * file name to remain exactly the same.
 * Only required for file systems where case is ignored and preserved.
 */
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		len ಬಳಸದ)  // buffer size, only used when name gets longer
{
    ರಚನೆ stat st;
    char_u	*slash, *ಬಾಲ;
    size_t	taillen;
    DIR		*dirp;
    ರಚನೆ dirent *dp;

    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)name, &st) < 0)
	ಹಿಂತಿರುಗಿ;

    // Open the directory where the file is located.
    slash = vim_strrchr(name, '/');
    ಆದರೆ (slash == ಖಾಲಿ)
    {
	dirp = opendir(".");
	ಬಾಲ = name;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	*slash = NUL;
	dirp = opendir((ಅಕ್ಷರ *)name);
	*slash = '/';
	ಬಾಲ = slash + 1;
    }

    ಆದರೆ (dirp == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    taillen = STRLEN(ಬಾಲ);
    ಆಗ ((dp = readdir(dirp)) != ಖಾಲಿ)
    {
	// Only accept names that differ in case and are the same byte
	// length. TODO: accept different length name.
	ಆದರೆ (STRICMP(ಬಾಲ, dp->d_name) == 0
		&& taillen == STRLEN(dp->d_name))
	{
	    char_u	newname[MAXPATHL + 1];
	    ರಚನೆ stat st2;

	    // Verify the inode is equal.
	    vim_strncpy(newname, name, MAXPATHL);
	    vim_strncpy(newname + (ಬಾಲ - name), (char_u *)dp->d_name,
		    MAXPATHL - (ಬಾಲ - name));
	    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)newname, &st2) >= 0
		    && st.st_ino == st2.st_ino
		    && st.st_dev == st2.st_dev)
	    {
		STRCPY(ಬಾಲ, dp->d_name);
		ನಿಲ್ಲಿಸು;
	    }
	}
    }

    closedir(dirp);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Get file permissions for 'name'.
 * Returns -1 when it doesn't exist.
 */
    ದೀರ್ಘ
mch_getperm(char_u *name)
{
    ರಚನೆ stat statb;

    // Keep the #ifdef outside of stat(), it may be a macro.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    ಆದರೆ (stat((ಅಕ್ಷರ *)vms_fixfilename(name), &statb))
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (stat((ಅಕ್ಷರ *)name, &statb))
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __INTERIX
    // The top bit makes the value negative, which means the file doesn't
    // exist.  Remove the bit, we don't use it.
    ಹಿಂತಿರುಗಿ statb.st_ಬಹುಲಕ & ~S_ADDACE;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ statb.st_ಬಹುಲಕ;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Set file permission for "name" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_setperm(char_u *name, ದೀರ್ಘ perm)
{
    ಹಿಂತಿರುಗಿ (chmod((ಅಕ್ಷರ *)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
		    vms_fixfilename(name),
#ಇಲ್ಲದಿದ್ದರೆ
		    name,
#ಆದರೆ_ಅಂತ್ಯ
		    (mode_t)perm) == 0 ? OK : FAIL);
}

#ಆದರೆ defined(HAVE_FCHMOD)
/*
 * Set file permission for open file "fd" to "perm".
 * Return FAIL for failure, OK otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_fsetperm(ಪೂರ್ಣಾಂಕ fd, ದೀರ್ಘ perm)
{
    ಹಿಂತಿರುಗಿ (fchmod(fd, (mode_t)perm) == 0 ? OK : FAIL);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(HAVE_ACL)
# ifdef HAVE_SYS_ACL_H
#  include <sys/acl.h>
# endif
# ifdef HAVE_SYS_ACCESS_H
#  include <sys/access.h>
# endif

# ifdef HAVE_SOLARIS_ACL
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ರಚನೆ vim_acl_solaris_T {
    ಪೂರ್ಣಾಂಕ acl_cnt;
    aclent_t *acl_ನಮೂದು;
} vim_acl_solaris_T;
# endif

# ಆದರೆ defined(HAVE_SELINUX)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ಶೂನ್ಯ
mch_copy_sec(char_u *from_file, char_u *ಗಮ್ಯ_file)
{
    ಆದರೆ (from_file == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (selinux_enabled == -1)
	selinux_enabled = is_selinux_enabled();

    ಆದರೆ (selinux_enabled <= 0)
	ಹಿಂತಿರುಗಿ;

    // Use "char *" instead of "security_context_t" to avoid a deprecation
    // warning.
    ಅಕ್ಷರ *from_context = ಖಾಲಿ;
    ಅಕ್ಷರ *ಗಮ್ಯ_context = ಖಾಲಿ;

    ಆದರೆ (getfilecon((ಅಕ್ಷರ *)from_file, &from_context) < 0)
    {
	// If the filesystem doesn't support extended attributes,
	// the original had no special security context and the
	// target cannot have one either.
	ಆದರೆ (errno == EOPNOTSUPP)
	    ಹಿಂತಿರುಗಿ;

	ಸಂದೇಶ_puts(_("\nCould not get security context for "));
	ಸಂದೇಶ_outtrans(from_file);
	ಸಂದೇಶ_putchar('\n');
	ಹಿಂತಿರುಗಿ;
    }
    ಆದರೆ (getfilecon((ಅಕ್ಷರ *)ಗಮ್ಯ_file, &ಗಮ್ಯ_context) < 0)
    {
	ಸಂದೇಶ_puts(_("\nCould not get security context for "));
	ಸಂದೇಶ_outtrans(ಗಮ್ಯ_file);
	ಸಂದೇಶ_putchar('\n');
	freecon (from_context);
	ಹಿಂತಿರುಗಿ ;
    }
    ಆದರೆ (strcmp(from_context, ಗಮ್ಯ_context) != 0)
    {
	ಆದರೆ (setfilecon((ಅಕ್ಷರ *)ಗಮ್ಯ_file, from_context) < 0)
	{
	    ಸಂದೇಶ_puts(_("\nCould not set security context for "));
	    ಸಂದೇಶ_outtrans(ಗಮ್ಯ_file);
	    ಸಂದೇಶ_putchar('\n');
	}
    }
    freecon(ಗಮ್ಯ_context);
    freecon(from_context);
}
# endif // HAVE_SELINUX

# ಆದರೆ defined(HAVE_SMACK) && !defined(PROTO)
/*
 * Copy security info from "from_file" to "to_file".
 */
    ಶೂನ್ಯ
mch_copy_sec(char_u *from_file, char_u *ಗಮ್ಯ_file)
{
    ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ * ಸ್ಥಿರಾಂಕ smack_copied_ಗುಣಲಕ್ಷಣಗಳು[] =
	{
	    XATTR_NAME_SMACK,
	    XATTR_NAME_SMACKEXEC,
	    XATTR_NAME_SMACKMMAP
	};

    ಅಕ್ಷರ	ಬಫರ್[SMACK_LABEL_LEN];
    ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ	*name;
    ಪೂರ್ಣಾಂಕ		ಸೂಚ್ಯಂಕ;
    ಪೂರ್ಣಾಂಕ		ret;
    ssize_t	ಗಾತ್ರ_2;

    ಆದರೆ (from_file == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    ಗಾತ್ರ_2 = listxattr((ಅಕ್ಷರ *)from_file, ಖಾಲಿ, 0);
    // not supported or no attributes to copy
    ಆದರೆ (ಗಾತ್ರ_2 <= 0)
	ಹಿಂತಿರುಗಿ;

    ಗಾಗಿ (ಸೂಚ್ಯಂಕ = 0 ; ಸೂಚ್ಯಂಕ < (ಪೂರ್ಣಾಂಕ)ARRAY_ಉದ್ದಳತೆ(smack_copied_ಗುಣಲಕ್ಷಣಗಳು); ಸೂಚ್ಯಂಕ++)
    {
	// get the name of the attribute to copy
	name = smack_copied_ಗುಣಲಕ್ಷಣಗಳು[ಸೂಚ್ಯಂಕ];

	// get the value of the attribute in buffer
	ಗಾತ್ರ_2 = getxattr((ಅಕ್ಷರ*)from_file, name, ಬಫರ್, ಗಾತ್ರ(ಬಫರ್));
	ಆದರೆ (ಗಾತ್ರ_2 >= 0)
	{
	    // copy the attribute value of buffer
	    ret = setxattr((ಅಕ್ಷರ*)ಗಮ್ಯ_file, name, ಬಫರ್, (size_t)ಗಾತ್ರ_2, 0);
	    ಆದರೆ (ret < 0)
	    {
		vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			_("Could not set security context %s for %s"),
			name, ಗಮ್ಯ_file);
		ಸಂದೇಶ_outtrans(IObuff);
		ಸಂದೇಶ_putchar('\n');
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // what reason of not having the attribute value?
	    ಆಯ್ಕೆ (errno)
	    {
		ಸ್ಥಿತಿ ENOTSUP:
		    // extended attributes aren't supported or enabled
		    // should a message be echoed? not sure...
		    ಹಿಂತಿರುಗಿ; // leave because it isn't useful to continue

		ಸ್ಥಿತಿ ERANGE:
		ಪೂರ್ವನಿಯೋಜಿತ:
		    // no enough size OR unexpected error
		     vim_snprintf((ಅಕ್ಷರ *)IObuff, IOSIZE,
			    _("Could not get security context %s for %s. Removing it!"),
			    name, from_file);
		    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)IObuff);
		    ಸಂದೇಶ_putchar('\n');
		    // FALLTHROUGH to remove the attribute

		ಸ್ಥಿತಿ ENODATA:
		    // no attribute of this name
		    ret = removexattr((ಅಕ್ಷರ*)ಗಮ್ಯ_file, name);
		    // Silently ignore errors, apparently this happens when
		    // smack is not actually being used.
		    ನಿಲ್ಲಿಸು;
	    }
	}
    }
}
# endif // HAVE_SMACK

# ಆದರೆ defined(FEAT_XATTR)
/*
 * Copy extended attributes from_file to to_file
 */
    ಶೂನ್ಯ
mch_copy_xattr(char_u *from_file, char_u *ಗಮ್ಯ_file)
{
    ಅಕ್ಷರ	*xattr_buf;
    ssize_t	ಗಾತ್ರ_2;
    ssize_t	tsize;
    ssize_t	keylen, vallen, max_vallen = 0;
    ಅಕ್ಷರ	*ಕೀಲಿ;
    ಅಕ್ಷರ	*val = ಖಾಲಿ;
    ಅಕ್ಷರ	*errmsg = ಖಾಲಿ;

    ಆದರೆ (from_file == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    // get the length of the extended attributes
    ಗಾತ್ರ_2 = listxattr((ಅಕ್ಷರ *)from_file, ಖಾಲಿ, 0);
    // not supported or no attributes to copy
    ಆದರೆ (ಗಾತ್ರ_2 <= 0)
	ಹಿಂತಿರುಗಿ;
    xattr_buf = (ಅಕ್ಷರ*)alloc(ಗಾತ್ರ_2);
    ಆದರೆ (xattr_buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
    ಗಾತ್ರ_2 = listxattr((ಅಕ್ಷರ *)from_file, xattr_buf, ಗಾತ್ರ_2);
    tsize = ಗಾತ್ರ_2;

    errno = 0;

    ಗಾಗಿ (ಪೂರ್ಣಾಂಕ ಸುತ್ತು = 0; ಸುತ್ತು < 2; ಸುತ್ತು++)
    {

	ಕೀಲಿ = xattr_buf;
	ಆದರೆ (ಸುತ್ತು == 1)
	    ಗಾತ್ರ_2 = tsize;

	ಆಗ (ಗಾತ್ರ_2 > 0)
	{
	    vallen = getxattr((ಅಕ್ಷರ *)from_file, ಕೀಲಿ,
		    val, ಸುತ್ತು ? max_vallen : 0);
	    // only set the attribute in the second round
	    ಆದರೆ (vallen >= 0 && ಸುತ್ತು &&
		setxattr((ಅಕ್ಷರ *)ಗಮ್ಯ_file, ಕೀಲಿ, val, vallen, 0) == 0)
		;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (errno)
	    {
		ಆಯ್ಕೆ (errno)
		{
		    ಸ್ಥಿತಿ E2BIG:
			errmsg = e_xattr_e2big;
			ಹೋಗು error_exit;
		    ಸ್ಥಿತಿ ENOTSUP:
		    ಸ್ಥಿತಿ EACCES:
		    ಸ್ಥಿತಿ EPERM:
			ನಿಲ್ಲಿಸು;
		    ಸ್ಥಿತಿ ERANGE:
			errmsg = e_xattr_erange;
			ಹೋಗು error_exit;
		    ಪೂರ್ವನಿಯೋಜಿತ:
			errmsg = e_xattr_other;
			ಹೋಗು error_exit;
		}
	    }

	    ಆದರೆ (ಸುತ್ತು == 0 && vallen > max_vallen)
		max_vallen = vallen;

	    // add one for terminating null
	    keylen = STRLEN(ಕೀಲಿ) + 1;
	    ಗಾತ್ರ_2 -= keylen;
	    ಕೀಲಿ += keylen;
	}
	ಆದರೆ (ಸುತ್ತು)
	    ನಿಲ್ಲಿಸು;

	val = (ಅಕ್ಷರ*)alloc(max_vallen + 1);
	ಆದರೆ (val == ಖಾಲಿ)
	    ಹೋಗು error_exit;

    }
error_exit:
    vim_free(xattr_buf);
    vim_free(val);

    ಆದರೆ (errmsg != ಖಾಲಿ)
	emsg(_(errmsg));
}
# endif

/*
 * Return a pointer to the ACL of file "fname" in allocated memory.
 * Return NULL if the ACL is not available for whatever reason.
 */
    vim_acl_T
mch_get_acl(char_u *fname ಬಳಸದ)
{
    vim_acl_T	ret = ಖಾಲಿ;
# ifdef HAVE_POSIX_ACL
    ret = (vim_acl_T)acl_get_file((ಅಕ್ಷರ *)fname, ACL_TYPE_ACCESS);
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_t *aclent;

    ಆದರೆ (acl_get((ಅಕ್ಷರ *)fname, 0, &aclent) < 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ret = (vim_acl_T)aclent;
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef HAVE_SOLARIS_ACL
    vim_acl_solaris_T   *aclent;

    aclent = ಸ್ಮರಣೆ_ಕೊಡು(ಗಾತ್ರ(vim_acl_solaris_T));
    ಆದರೆ (aclent == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ ((aclent->acl_cnt = acl((ಅಕ್ಷರ *)fname, GETACLCNT, 0, ಖಾಲಿ)) < 0)
    {
	ಬಿಡುಗಡೆ(aclent);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    aclent->acl_ನಮೂದು = ಸ್ಮರಣೆ_ಕೊಡು(aclent->acl_cnt * ಗಾತ್ರ(aclent_t));
    ಆದರೆ (aclent->acl_ನಮೂದು == ಖಾಲಿ)
    {
	ಬಿಡುಗಡೆ(aclent);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ಆದರೆ (acl((ಅಕ್ಷರ *)fname, GETACL, aclent->acl_cnt, aclent->acl_ನಮೂದು) < 0)
    {
	ಬಿಡುಗಡೆ(aclent->acl_ನಮೂದು);
	ಬಿಡುಗಡೆ(aclent);
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }
    ret = (vim_acl_T)aclent;
#   ಇಲ್ಲದಿದ್ದರೆ
#    ಆದರೆ defined(HAVE_AIX_ACL)
    ಪೂರ್ಣಾಂಕ		aclsize;
    ರಚನೆ acl *aclent;

    aclsize = ಗಾತ್ರ(ರಚನೆ acl);
    aclent = ಸ್ಮರಣೆ_ಕೊಡು(aclsize);
    ಆದರೆ (aclent == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಆದರೆ (statacl((ಅಕ್ಷರ *)fname, STX_ಸಾಮಾನ್ಯ, aclent, aclsize) < 0)
    {
	ಆದರೆ (errno == ENOSPC)
	{
	    aclsize = aclent->acl_len;
	    aclent = realloc(aclent, aclsize);
	    ಆದರೆ (aclent == ಖಾಲಿ || statacl((ಅಕ್ಷರ *)fname, STX_ಸಾಮಾನ್ಯ, aclent, aclsize) < 0)
	    {
		ಬಿಡುಗಡೆ(aclent);
		ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಬಿಡುಗಡೆ(aclent);
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
	}
    }
    ret = (vim_acl_T)aclent;
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
    ಹಿಂತಿರುಗಿ ret;
}

/*
 * Set the ACL of file "fname" to "acl" (unless it's NULL).
 */
    ಶೂನ್ಯ
mch_ಗಣ_acl(char_u *fname ಬಳಸದ, vim_acl_T aclent)
{
    ಆದರೆ (aclent == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
# ifdef HAVE_POSIX_ACL
    acl_ಗಣ_file((ಅಕ್ಷರ *)fname, ACL_TYPE_ACCESS, (acl_t)aclent);
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_ಗಣ((ಅಕ್ಷರ *)fname, (acl_t *)aclent);
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef HAVE_SOLARIS_ACL
    acl((ಅಕ್ಷರ *)fname, SETACL, ((vim_acl_solaris_T *)aclent)->acl_cnt,
	    ((vim_acl_solaris_T *)aclent)->acl_ನಮೂದು);
#   ಇಲ್ಲದಿದ್ದರೆ
#    ifdef HAVE_AIX_ACL
    chacl((ಅಕ್ಷರ *)fname, aclent, ((ರಚನೆ acl *)aclent)->acl_len);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}

    ಶೂನ್ಯ
mch_free_acl(vim_acl_T aclent)
{
    ಆದರೆ (aclent == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;
# ifdef HAVE_POSIX_ACL
    acl_free((acl_t)aclent);
# ಇಲ್ಲದಿದ್ದರೆ
#  ifdef HAVE_SOLARIS_ZFS_ACL
    acl_free((acl_t *)aclent);
#  ಇಲ್ಲದಿದ್ದರೆ
#   ifdef HAVE_SOLARIS_ACL
    ಬಿಡುಗಡೆ(((vim_acl_solaris_T *)aclent)->acl_ನಮೂದು);
    ಬಿಡುಗಡೆ(aclent);
#   ಇಲ್ಲದಿದ್ದರೆ
#    ifdef HAVE_AIX_ACL
    ಬಿಡುಗಡೆ(aclent);
#    endif // HAVE_AIX_ACL
#   endif // HAVE_SOLARIS_ACL
#  endif // HAVE_SOLARIS_ZFS_ACL
# endif // HAVE_POSIX_ACL
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name ಬಳಸದ)
{
    // can't hide a file
}

/*
 * return TRUE if "name" is a directory or a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಂಕ
mch_isdir(char_u *name)
{
    ರಚನೆ stat statb;

    ಆದರೆ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (stat((ಅಕ್ಷರ *)name, &statb))
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ (S_ISDIR(statb.st_ಬಹುಲಕ) ? TRUE : FALSE);
}

/*
 * return TRUE if "name" is a directory, NOT a symlink to a directory
 * return FALSE if "name" is not a directory
 * return FALSE for error
 */
    ಪೂರ್ಣಾಂಕ
mch_isrealdir(char_u *name)
{
    ರಚನೆ stat statb;

    ಆದರೆ (*name == NUL)	    // Some stat()s don't flag "" as an error.
	ಹಿಂತಿರುಗಿ FALSE;
    ಆದರೆ (mch_lstat((ಅಕ್ಷರ *)name, &statb))
	ಹಿಂತಿರುಗಿ FALSE;
    ಹಿಂತಿರುಗಿ (S_ISDIR(statb.st_ಬಹುಲಕ) ? TRUE : FALSE);
}

/*
 * Return 1 if "name" is an executable file, 0 if not or it doesn't exist.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
executable_file(char_u *name)
{
    ರಚನೆ stat	st;

    ಆದರೆ (stat((ಅಕ್ಷರ *)name, &st))
	ಹಿಂತಿರುಗಿ 0;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
    // Like on Unix system file can have executable rights but not necessarily
    // be an executable, but on Unix is not a default for an ordinary file to
    // have an executable flag - on VMS it is in most cases.
    // Therefore, this check does not have any sense - let keep us to the
    // conventions instead:
    // *.COM and *.EXE files are the executables - the rest are not. This is
    // not ideal but better than it was.
    ಪೂರ್ಣಾಂಕ vms_executable = 0;
    ಆದರೆ (S_ISREG(st.st_ಬಹುಲಕ) && mch_access((ಅಕ್ಷರ *)name, X_OK) == 0)
    {
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(vms_tolower((ಅಕ್ಷರ*)name),".exe") != ಖಾಲಿ
		|| ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು(vms_tolower((ಅಕ್ಷರ*)name),".com")!= ಖಾಲಿ)
	    vms_executable = 1;
    }
    ಹಿಂತಿರುಗಿ vms_executable;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ S_ISREG(st.st_ಬಹುಲಕ) && mch_access((ಅಕ್ಷರ *)name, X_OK) == 0;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Return TRUE if "name" can be found in $PATH and executed, FALSE if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ಪೂರ್ಣಾಂಕ
mch_can_exe(char_u *name, char_u **ಮಾರ್ಗ, ಪೂರ್ಣಾಂಕ use_ಮಾರ್ಗ)
{
    char_u	*buf;
    size_t	bufsize;
    size_t	buflen;
    char_u	*p, *e;
    char_u	*p_ಕೊನೆ;
    size_t	elen;
    ಪೂರ್ಣಾಂಕ		retval;

    // When "use_path" is false and if it's an absolute or relative path don't
    // need to use $PATH.
    ಆದರೆ (!use_ಮಾರ್ಗ || gettail(name) != name)
    {
	// There must be a path separator, files in the current directory
	// can't be executed.
	ಆದರೆ ((use_ಮಾರ್ಗ || gettail(name) != name) && executable_file(name))
	{
	    ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	    {
		ಆದರೆ (name[0] != '/')
		    *ಮಾರ್ಗ = ತುಂಬಿದ_Name_save(name, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		    *ಮಾರ್ಗ = vim_strsave(name);
	    }
	    ಹಿಂತಿರುಗಿ TRUE;
	}
	ಹಿಂತಿರುಗಿ FALSE;
    }

    p = (char_u *)getenv("PATH");
    ಆದರೆ (p == ಖಾಲಿ || *p == NUL)
	ಹಿಂತಿರುಗಿ -1;
    p_ಕೊನೆ = p + STRLEN(p);
    bufsize = STRLEN(name) + (size_t)(p_ಕೊನೆ - p) + 2;
    buf = alloc(bufsize);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    /*
     * Walk through all entries in $PATH to check if "name" exists there and
     * is an executable file.
     */
    ಗಾಗಿ (;;)
    {
	e = (char_u *)strchr((ಅಕ್ಷರ *)p, ':');
	ಆದರೆ (e == ಖಾಲಿ)
	    e = p_ಕೊನೆ;
	elen = (size_t)(e - p);
	ಆದರೆ (elen <= 1)		// empty entry means current dir
	{
	    p = (char_u *)"./";
	    elen = STRLEN_LITERAL("./");
	}
	buflen = vim_snprintf((ಅಕ್ಷರ *)buf, bufsize, "%.*s%s%s",
		(ಪೂರ್ಣಾಂಕ)elen,
		p,
		(after_pathsep(p, p + elen)) ? "" : PATHSEPSTR,
		name);
	retval = executable_file(buf);
	ಆದರೆ (retval == 1)
	{
	    ಆದರೆ (ಮಾರ್ಗ != ಖಾಲಿ)
	    {
		ಆದರೆ (buf[0] != '/')
		    *ಮಾರ್ಗ = ತುಂಬಿದ_Name_save(buf, TRUE);
		ಇಲ್ಲದಿದ್ದರೆ
		    *ಮಾರ್ಗ = vim_strnsave(buf, buflen);
	    }
	    ನಿಲ್ಲಿಸು;
	}

	ಆದರೆ (*e != ':')
	    ನಿಲ್ಲಿಸು;
	p = e + 1;
    }

    vim_free(buf);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಂಕ
mch_nodetype(char_u *name)
{
    ರಚನೆ stat	st;

    ಆದರೆ (stat((ಅಕ್ಷರ *)name, &st))
	ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;
    ಆದರೆ (S_ISREG(st.st_ಬಹುಲಕ) || S_ISDIR(st.st_ಬಹುಲಕ))
	ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;
    ಆದರೆ (S_ISBLK(st.st_ಬಹುಲಕ))	// block device isn't writable
	ಹಿಂತಿರುಗಿ ಗಂಟು_OTHER;
    // Everything else is writable?
    ಹಿಂತಿರುಗಿ ಗಂಟು_WRITABLE;
}

    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ HAVE_CHECK_ರಾಶಿ_GROWTH
    ಪೂರ್ಣಾಂಕ			i;

    check_ರಾಶಿ_growth((ಅಕ್ಷರ *)&i);

# ifdef HAVE_ರಾಶಿ_ಎಲ್ಲೆ
    get_ರಾಶಿ_ಎಲ್ಲೆ();
# endif

#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Setup an alternative stack for signals.  Helps to catch signals when
     * running out of stack space.
     * Use of sigaltstack() is preferred, it's more portable.
     * Ignore any errors.
     */
#ಆದರೆ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    signal_ರಾಶಿ = alloc(get_signal_ರಾಶಿ_ಗಾತ್ರ());
    init_signal_ರಾಶಿ();
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Inform the macOS scheduler that Vim renders UI, and so shouldn’t have its
     * threads’ quality of service classes clamped.
     */
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_X
# ifdef MAC_OS_X_ಆವೃತ್ತಿ_10_7
    integer_t policy = TASK_DEFAULT_APPLICATION;
    task_policy_ಗಣ(mach_task_self(), TASK_CATEGORY_POLICY, &policy, 1);
# endif
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(EXITFREE)
    ಶೂನ್ಯ
mch_free_mem(ಶೂನ್ಯ)
{
# ಆದರೆ defined(FEAT_CLIPBOARD) && defined(FEAT_X11)
    ಆದರೆ (clip_ನಕ್ಷತ್ರ.owned)
	clip_lose_selection(&clip_ನಕ್ಷತ್ರ);
    ಆದರೆ (clip_plus.owned)
	clip_lose_selection(&clip_plus);
# endif
# ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಆದರೆ (xterm_Shell != (Widget)0)
	XtDestroyWidget(xterm_Shell);
#  ifndef LESSTIF_ಆವೃತ್ತಿ
    // Lesstif crashes here, lose some memory
    ಆದರೆ (xterm_dpy != ಖಾಲಿ)
	XtCloseDisplay(xterm_dpy);
    ಆದರೆ (app_context != (XtAppContext)ಖಾಲಿ)
    {
	XtDestroyApplicationContext(app_context);
#   ifdef FEAT_X11
	x11_display = ಖಾಲಿ; // freed by XtDestroyApplicationContext()
#   endif
    }
#  endif
# endif
# ಆದರೆ defined(FEAT_X11)
    ಆದರೆ (x11_display != ಖಾಲಿ
#  ifdef FEAT_XCLIPBOARD
	    && x11_display != xterm_dpy
#  endif
	    )
	XCloseDisplay(x11_display);
# endif
# ಆದರೆ defined(HAVE_SIGALTSTACK) || defined(HAVE_SIGSTACK)
    VIM_CLEAR(signal_ರಾಶಿ);
# endif
    vim_free(oldtitle);
    vim_free(oldicon);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Output a newline when exiting.
 * Make sure the newline goes to the same stream as the text.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
exit_scroll(ಶೂನ್ಯ)
{
    ಆದರೆ (silent_ಬಹುಲಕ)
	ಹಿಂತಿರುಗಿ;
    ಆದರೆ (newline_on_exit || ಸಂದೇಶ_didout)
    {
	ಆದರೆ (ಸಂದೇಶ_use_printf())
	{
	    ಆದರೆ (info_ಸಂದೇಶ)
		mch_ಸಂದೇಶ("\n");
	    ಇಲ್ಲದಿದ್ದರೆ
		mch_errmsg("\r\n");
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊರಹೋಗು_char('\n');
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!is_not_a_ಪದ())
    {
	restore_cterm_colors();		// get original colors back
	ಸಂದೇಶ_clr_eos_force();		// clear the rest of the display
	windgoto((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು - 1, cmdline_ಲಂಬಸಾಲು_off); // may have moved the cursor
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GCOV_FLUSH
# ಆದರೆ (defined(__GNUC__) \
	    && ((__GNUC__ == 11 && __GNUC_MINOR__ >= 1) || (__GNUC__ >= 12))) \
	|| (defined(__clang__) && (__clang_major__ >= 12))
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_dump(ಶೂನ್ಯ);
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_reset(ಶೂನ್ಯ);
#  define __gcov_flush() ಮಾಡು { __gcov_dump(); __gcov_reset(); } ಆಗ (0)
# ಇಲ್ಲದಿದ್ದರೆ
ಬಾಹ್ಯ ಶೂನ್ಯ __gcov_flush(ಶೂನ್ಯ);
# endif
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಂಕ r)
{
    exiting = TRUE;

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_CLIPBOARD)
    x11_export_final_selection();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (!gui.ಒಳ_use)
#ಆದರೆ_ಅಂತ್ಯ
    {
	settmode(TMODE_COOK);
	ಆದರೆ (!is_not_a_ಪದ())
	{
	    // restore xterm title and icon name
	    mch_restore_title(SAVE_RESTORE_BOTH);
	    ಪದ_ತೆಗೆ_title(SAVE_RESTORE_BOTH);
	}

	/*
	 * When t_ti is not empty but it doesn't cause swapping terminal
	 * pages, need to output a newline when msg_didout is set.  But when
	 * t_ti does swap pages it should not go to the shell page.  Do this
	 * before stoptermcap().
	 */
	ಆದರೆ (swapping_screen() && !newline_on_exit)
	    exit_scroll();

	// Stop termcap: May need to check for T_CRV response, which
	// requires RAW mode.
	stoptermcap();

	/*
	 * A newline is only required after a message in the alternate screen.
	 * This is set to TRUE by wait_return().
	 */
	ಆದರೆ (!swapping_screen() || newline_on_exit)
	    exit_scroll();

	// Cursor may have been switched off without calling starttermcap()
	// when doing "vim -u vimrc" and vimrc contains ":q".
	ಆದರೆ (ತುಂಬಿದ_screen)
	    cursor_on();
    }
    ಹೊರಹೋಗು_flush();
    ml_close_all(TRUE);		// remove all memfiles

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_GCOV_FLUSH
    // Flush coverage info before possibly being killed by a deadly signal.
    __gcov_flush();
#ಆದರೆ_ಅಂತ್ಯ

    may_core_dump();
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_exit(r);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ MACOS_CONVERT
    mac_conv_cleanup();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __QNX__
    // A core dump won't be created if the signal handler
    // doesn't return, so we can't call exit()
    ಆದರೆ (deadly_signal != 0)
	ಹಿಂತಿರುಗಿ;
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_NETBEANS_INTG
    netbeans_send_disconnect();
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ EXITFREE
    free_all_mem();
#ಆದರೆ_ಅಂತ್ಯ

    ನಿರ್ಗಮಿಸು(r);
}

    ಸ್ಥಿರ ಶೂನ್ಯ
may_core_dump(ಶೂನ್ಯ)
{
    ಆದರೆ (deadly_signal != 0)
    {
	mch_signal(deadly_signal, SIG_DFL);
	kill(getpid(), deadly_signal);	// Die using the signal we caught
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS

/*
 * Get the file descriptor to use for tty operations.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
get_tty_fd(ಪೂರ್ಣಾಂಕ fd)
{
    ಪೂರ್ಣಾಂಕ		tty_fd = fd;

# ಆದರೆ defined(HAVE_SVR4_PTYS) && defined(SUN_SYSTEM)
    // On SunOS: Get the terminal parameters from "fd", or the slave device of
    // "fd" when it is a master device.
    ಆದರೆ (mch_isatty(fd) > 1)
    {
	ಅಕ್ಷರ *name;

	name = ptsname(fd);
	ಆದರೆ (name == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ -1;

	tty_fd = open(name, O_RDONLY | O_NOCTTY | O_EXTRA, 0);
	ಆದರೆ (tty_fd < 0)
	    ಹಿಂತಿರುಗಿ -1;
    }
# endif
    ಹಿಂತಿರುಗಿ tty_fd;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_tcgetattr(ಪೂರ್ಣಾಂಕ fd, ಶೂನ್ಯ *ಪದ)
{
    ಪೂರ್ಣಾಂಕ		tty_fd;
    ಪೂರ್ಣಾಂಕ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ಆದರೆ (tty_fd < 0)
	ಹಿಂತಿರುಗಿ -1;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    retval = tcgetattr(tty_fd, (ರಚನೆ termios *)ಪದ);
#  ಇಲ್ಲದಿದ್ದರೆ
    retval = ioctl(tty_fd, TCGETA, (ರಚನೆ termio *)ಪದ);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    // for "old" tty systems
    retval = ioctl(tty_fd, TIOCGETP, (ರಚನೆ sgttyb *)ಪದ);
# endif
    ಆದರೆ (tty_fd != fd)
	close(tty_fd);
    ಹಿಂತಿರುಗಿ retval;
}

    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ ಮೊದಲ = TRUE;

# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ಸ್ಥಿರ ರಚನೆ termios told;
	   ರಚನೆ termios tnew;
#  ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ರಚನೆ termio told;
	   ರಚನೆ termio tnew;
#  endif

    ಆದರೆ (ಮೊದಲ)
    {
	ಮೊದಲ = FALSE;
	mch_tcgetattr(read_cmd_fd, &told);
    }

    tnew = told;
    ಆದರೆ (tmode == TMODE_RAW)
    {
	// ~ICRNL enables typing ^V^M
	// ~IXON disables CTRL-S stopping output, so that it can be mapped.
	tnew.c_iflag &= ~(ICRNL |
		(T_XON == ಖಾಲಿ || *T_XON == NUL ? IXON : 0));
	tnew.c_lflag &= ~(ICANON | ECHO | ISIG | ECHOE
#  ಆದರೆ defined(IEXTEN)
		    | IEXTEN	    // IEXTEN enables typing ^V on SOLARIS
#  endif
				);
#  ifdef ONLCR
	// Don't map NL -> CR NL, we do it ourselves.
	// Also disable expanding tabs if possible.
#   ifdef XTABS
	tnew.c_oflag &= ~(ONLCR | XTABS);
#   ಇಲ್ಲದಿದ್ದರೆ
#    ifdef TAB3
	tnew.c_oflag &= ~(ONLCR | TAB3);
#    ಇಲ್ಲದಿದ್ದರೆ
	tnew.c_oflag &= ~ONLCR;
#    endif
#   endif
#  endif
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tmode == TMODE_SLEEP)
    {
	// Also reset ICANON here, otherwise on Solaris select() won't see
	// typeahead characters.
	tnew.c_lflag &= ~(ICANON | ECHO);
	tnew.c_cc[VMIN] = 1;		// return after 1 char
	tnew.c_cc[VTIME] = 0;		// don't wait
    }

#  ಆದರೆ defined(HAVE_TERMIOS_H)
    {
	ಪೂರ್ಣಾಂಕ	n = 10;

	// A signal may cause tcsetattr() to fail (e.g., SIGCONT).  Retry a
	// few times.
	ಆಗ (tcsetattr(read_cmd_fd, TCSANOW, &tnew) == -1
						   && errno == EINTR && n > 0)
	    --n;
    }
#  ಇಲ್ಲದಿದ್ದರೆ
    ioctl(read_cmd_fd, TCSETA, &tnew);
#  endif

# ಇಲ್ಲದಿದ್ದರೆ
    /*
     * for "old" tty systems
     */
#  ifndef TIOCSETN
#   define TIOCSETN TIOCSETP	// for hpux 9.0
#  endif
    ಸ್ಥಿರ ರಚನೆ sgttyb ttybold;
	   ರಚನೆ sgttyb ttybnew;

    ಆದರೆ (ಮೊದಲ)
    {
	ಮೊದಲ = FALSE;
	mch_tcgetattr(read_cmd_fd, &ttybold);
    }

    ttybnew = ttybold;
    ಆದರೆ (tmode == TMODE_RAW)
    {
	ttybnew.sg_flags &= ~(CRMOD | ECHO);
	ttybnew.sg_flags |= RAW;
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (tmode == TMODE_SLEEP)
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
    ಶೂನ್ಯ
get_stty(ಶೂನ್ಯ)
{
    ttyinfo_T	info;
    char_u	buf[2];
    char_u	*p;

    ಆದರೆ (get_tty_info(read_cmd_fd, &info) != OK)
	ಹಿಂತಿರುಗಿ;

    intr_char = info.ಅಡಚಣೆ;
    buf[0] = info.backspace;
    buf[1] = NUL;
    ಕೂಡಿಸು_termcode((char_u *)"kb", buf, FALSE);

    // If <BS> and <DEL> are now the same, redefine <DEL>.
    p = ಕಂಡುಹಿಡಿ_termcode((char_u *)"kD");
    ಆದರೆ (p != ಖಾಲಿ && p[0] == buf[0] && p[1] == buf[1])
	do_fixdel(ಖಾಲಿ);
}

/*
 * Obtain the characters that Backspace and Enter produce on "fd".
 * Returns OK or FAIL.
 */
    ಪೂರ್ಣಾಂಕ
get_tty_info(ಪೂರ್ಣಾಂಕ fd, ttyinfo_T *info)
{
# ifdef NEW_TTY_SYSTEM
#  ifdef HAVE_TERMIOS_H
    ರಚನೆ termios ಕೀಲಿಗಳು;
#  ಇಲ್ಲದಿದ್ದರೆ
    ರಚನೆ termio ಕೀಲಿಗಳು;
#  endif

    ಆದರೆ (mch_tcgetattr(fd, &ಕೀಲಿಗಳು) != -1)
    {
	info->backspace = ಕೀಲಿಗಳು.c_cc[VERASE];
	info->ಅಡಚಣೆ = ಕೀಲಿಗಳು.c_cc[VINTR];
	ಆದರೆ (ಕೀಲಿಗಳು.c_iflag & ICRNL)
	    info->enter = NL;
	ಇಲ್ಲದಿದ್ದರೆ
	    info->enter = CAR;
	ಆದರೆ (ಕೀಲಿಗಳು.c_oflag & ONLCR)
	    info->nl_does_cr = TRUE;
	ಇಲ್ಲದಿದ್ದರೆ
	    info->nl_does_cr = FALSE;
	ಹಿಂತಿರುಗಿ OK;
    }
# ಇಲ್ಲದಿದ್ದರೆ
    // for "old" tty systems
    ರಚನೆ sgttyb ಕೀಲಿಗಳು;

    ಆದರೆ (mch_tcgetattr(fd, &ಕೀಲಿಗಳು) != -1)
    {
	info->backspace = ಕೀಲಿಗಳು.sg_erase;
	info->ಅಡಚಣೆ = ಕೀಲಿಗಳು.sg_kill;
	info->enter = CAR;
	info->nl_does_cr = TRUE;
	ಹಿಂತಿರುಗಿ OK;
    }
# endif
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ_ಅಂತ್ಯ // VMS

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	mouse_ison = FALSE;

/*
 * Set mouse clicks on or off and possible enable mouse movement events.
 */
    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಂಕ on)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	bevalterm_ison = FALSE;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		xterm_mouse_vers;

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
    ಆದರೆ (!on)
	// Make sure not tracing mouse movements.  Important when a button-down
	// was received but no release yet.
	stop_xterm_trace();
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (on == mouse_ison
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
	    && p_bevalterm == bevalterm_ison
#ಆದರೆ_ಅಂತ್ಯ
	    )
	// return quickly if nothing to do
	ಹಿಂತಿರುಗಿ;

    xterm_mouse_vers = use_xterm_mouse();

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_URXVT
    ಆದರೆ (ttym_flags == TTYM_URXVT)
    {
	ಹೊರಹೋಗು_str_nf((char_u *)(on ? "\033[?1015h" : "\033[?1015l"));
	mouse_ison = on;
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (T_CXM != ಖಾಲಿ && *T_CXM != NUL)
    {
	ಪದ_enable_mouse(on);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ttym_flags == TTYM_SGR)
    {
	// SGR mode supports columns above 223
	ಹೊರಹೋಗು_str_nf((char_u *)(on ? "\033[?1006h" : "\033[?1006l"));
	mouse_ison = on;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
    ಆದರೆ (bevalterm_ison != (p_bevalterm && on))
    {
	bevalterm_ison = (p_bevalterm && on);
	ಆದರೆ (xterm_mouse_vers > 1 && !bevalterm_ison)
	    // disable mouse movement events, enabling is below
	    ಹೊರಹೋಗು_str_nf((char_u *)("\033[?1003l"));
    }
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (xterm_mouse_vers > 0)
    {
	ಆದರೆ (on)	// enable mouse events, use mouse tracking if available
	    ಹೊರಹೋಗು_str_nf((char_u *)
		       (xterm_mouse_vers > 1
			? (
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_BEVAL_ಪದ
			    bevalterm_ison ? "\033[?1003h" :
#ಆದರೆ_ಅಂತ್ಯ
			      "\033[?1002h")
			: "\033[?1000h"));
	ಇಲ್ಲದಿದ್ದರೆ	// disable mouse events, could probably always send the same
	    ಹೊರಹೋಗು_str_nf((char_u *)
		       (xterm_mouse_vers > 1 ? "\033[?1002l" : "\033[?1000l"));
	mouse_ison = on;
    }

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ttym_flags == TTYM_DEC)
    {
	ಆದರೆ (on)	// enable mouse events
	    ಹೊರಹೋಗು_str_nf((char_u *)"\033[1;2'z\033[1;3'{");
	ಇಲ್ಲದಿದ್ದರೆ	// disable mouse events
	    ಹೊರಹೋಗು_str_nf((char_u *)"\033['z");
	mouse_ison = on;
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (on)
	{
	    ಆದರೆ (gpm_open())
		mouse_ison = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    gpm_close();
	    mouse_ison = FALSE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYSMOUSE
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (on)
	{
	    ಆದರೆ (sysmouse_open() == OK)
		mouse_ison = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    sysmouse_close();
	    mouse_ison = FALSE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (on)
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
	    ಹೊರಹೋಗು_str_nf((char_u *)"\033[0~ZwLMRK1Q\033\\");
# ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊರಹೋಗು_str_nf((char_u *)"\033[0~ZwLMRK+1Q\033\\");
# endif
	    mouse_ison = TRUE;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಹೊರಹೋಗು_str_nf((char_u *)"\033[0~ZwQ\033\\");
	    mouse_ison = FALSE;
	}
    }
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
    ಇಲ್ಲದಿದ್ದರೆ
    {
	// 1 = button press, 6 = release, 7 = drag, 1h...9l = right button
	ಆದರೆ (on)
	    ಹೊರಹೋಗು_str_nf("\033[>1h\033[>6h\033[>7h\033[>1h\033[>9l");
	ಇಲ್ಲದಿದ್ದರೆ
	    ಹೊರಹೋಗು_str_nf("\033[>1l\033[>6l\033[>7l\033[>1l\033[>9h");
	mouse_ison = on;
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_BEVAL_ಪದ)
/*
 * Called when 'balloonevalterm' changed.
 */
    ಶೂನ್ಯ
mch_bevalterm_ಬದಲಾಯಿತು(ಶೂನ್ಯ)
{
    mch_setmouse(mouse_ison);
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set the mouse termcode, depending on the 'term' and 'ttymouse' options.
 */
    ಶೂನ್ಯ
check_mouse_termcode(ಶೂನ್ಯ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_XTERM
    ಆದರೆ (use_xterm_mouse()
# ifdef FEAT_MOUSE_URXVT
	    && use_xterm_mouse() != 3
# endif
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
    {
	ಗಣ_mouse_termcode(KS_MOUSE, (char_u *)(ಪದ_is_8bit(T_NAME)
							? "\233M" : "\033[M"));
	ಆದರೆ (*p_mouse != NUL)
	{
	    // force mouse off and maybe on to send possibly new mouse
	    // activation sequence to the xterm, with(out) drag tracing.
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_GPM_MOUSE, (char_u *)"\033MG");
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_GPM_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYSMOUSE
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_MOUSE, (char_u *)"\033MS");
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_JSB
    // Conflicts with xterm mouse: "\033[" and "\033[M" ???
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_JSBTERM_MOUSE, (char_u *)"\033[0~zw");
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_JSBTERM_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_NET
    // There is no conflict, but one may type "ESC }" from Insert mode.  Don't
    // define it in the GUI or when using an xterm.
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_NETTERM_MOUSE, (char_u *)"\033}");
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_NETTERM_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_DEC
    // Conflicts with xterm mouse: "\033[" and "\033[M"
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_DEC_MOUSE, (char_u *)(ಪದ_is_8bit(T_NAME)
							  ? "\233" : "\033["));
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_DEC_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_PTERM
    // same conflict as the dec mouse
    ಆದರೆ (!use_xterm_mouse()
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
	ಗಣ_mouse_termcode(KS_PTERM_MOUSE, (char_u *)"\033[");
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_PTERM_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_URXVT
    ಆದರೆ (use_xterm_mouse() == 3
# ifdef FEAT_GUI
	    && !gui.ಒಳ_use
# endif
	    )
    {
	ಗಣ_mouse_termcode(KS_URXVT_MOUSE, (char_u *)(ಪದ_is_8bit(T_NAME)
						      ? "\233*M" : "\033[*M"));

	ಆದರೆ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	del_mouse_termcode(KS_URXVT_MOUSE);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (use_xterm_mouse() == 4
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_GUI
	    && !gui.ಒಳ_use
#ಆದರೆ_ಅಂತ್ಯ
	    )
    {
	ಗಣ_mouse_termcode(KS_SGR_MOUSE, (char_u *)(ಪದ_is_8bit(T_NAME)
						    ? "\233<*M" : "\033[<*M"));

	ಗಣ_mouse_termcode(KS_SGR_MOUSE_RELEASE, (char_u *)(ಪದ_is_8bit(T_NAME)
						    ? "\233<*m" : "\033[<*m"));

	ಆದರೆ (*p_mouse != NUL)
	{
	    mch_setmouse(FALSE);
	    setmouse();
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	del_mouse_termcode(KS_SGR_MOUSE);
	del_mouse_termcode(KS_SGR_MOUSE_RELEASE);
    }
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS

/*
 * Try to get the current window size:
 * 1. with an ioctl(), most accurate method
 * 2. from the environment variables LINES and COLUMNS
 * 3. from the termcap
 * 4. keep using the old values
 * Return OK when size could be determined, FAIL otherwise.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    ದೀರ್ಘ	ಅಡ್ಡಸಾಲುಗಳು_2 = 0;
    ದೀರ್ಘ	columns = 0;
    char_u	*p;

    /*
     * 1. try using an ioctl. It is the most accurate method.
     *
     * Try using TIOCGWINSZ first, some systems that have it also define
     * TIOCGSIZE but don't have a struct ttysize.
     */
# ifdef TIOCGWINSZ
    {
	ರಚನೆ winsize	ws;
	ಪೂರ್ಣಾಂಕ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ಆದರೆ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ಆದರೆ (ioctl(fd, TIOCGWINSZ, &ws) == 0)
	{
	    columns = ws.ws_ಲಂಬಸಾಲು;
	    ಅಡ್ಡಸಾಲುಗಳು_2 = ws.ws_ಅಡ್ಡಸಾಲು;
#  ifdef FEAT_EVAL
	    ch_log(ಖಾಲಿ, "Got size with TIOCGWINSZ: %ld x %ld", columns, ಅಡ್ಡಸಾಲುಗಳು_2);
#  endif
	}
    }
# ಇಲ್ಲದಿದ್ದರೆ // TIOCGWINSZ
#  ifdef TIOCGSIZE
    {
	ರಚನೆ ttysize	ts;
	ಪೂರ್ಣಾಂಕ fd = 1;

	// When stdout is not a tty, use stdin for the ioctl().
	ಆದರೆ (!isatty(fd) && isatty(read_cmd_fd))
	    fd = read_cmd_fd;
	ಆದರೆ (ioctl(fd, TIOCGSIZE, &ts) == 0)
	{
	    columns = ts.ts_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
	    ಅಡ್ಡಸಾಲುಗಳು_2 = ts.ts_ಸಾಲುಗಳು;
#   ifdef FEAT_EVAL
	    ch_log(ಖಾಲಿ, "Got size with TIOCGSIZE: %ld x %ld", columns, ಅಡ್ಡಸಾಲುಗಳು_2);
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
    ಆದರೆ (columns == 0 || ಅಡ್ಡಸಾಲುಗಳು_2 == 0 || vim_strchr(p_cpo, CPO_TSIZE) != ಖಾಲಿ)
    {
	ಆದರೆ ((p = (char_u *)getenv("LINES")))
	{
	    ಅಡ್ಡಸಾಲುಗಳು_2 = atoi((ಅಕ್ಷರ *)p);
# ifdef FEAT_EVAL
	    ch_log(ಖಾಲಿ, "Got 'lines' from $LINES: %ld", ಅಡ್ಡಸಾಲುಗಳು_2);
# endif
	}
	ಆದರೆ ((p = (char_u *)getenv("COLUMNS")))
	{
	    columns = atoi((ಅಕ್ಷರ *)p);
# ifdef FEAT_EVAL
	    ch_log(ಖಾಲಿ, "Got 'columns' from $COLUMNS: %ld", columns);
# endif
	}
    }

# ifdef HAVE_TGETENT
    /*
     * 3. try reading "co" and "li" entries from termcap
     */
    ಆದರೆ (columns == 0 || ಅಡ್ಡಸಾಲುಗಳು_2 == 0)
    {
	getlinecol(&columns, &ಅಡ್ಡಸಾಲುಗಳು_2);
#  ifdef FEAT_EVAL
	ch_log(ಖಾಲಿ, "Got size from termcap: %ld x %ld", columns, ಅಡ್ಡಸಾಲುಗಳು_2);
#  endif
    }
# endif

    /*
     * 4. If everything fails, use the old values
     */
    ಆದರೆ (columns <= 0 || ಅಡ್ಡಸಾಲುಗಳು_2 <= 0)
	ಹಿಂತಿರುಗಿ FAIL;

    ಅಡ್ಡಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    Columns = columns;
    ಎಲ್ಲೆ_screen_ಗಾತ್ರ();
    ಹಿಂತಿರುಗಿ OK;
}

# ಆದರೆ defined(FEAT_EVAL) || defined(FEAT_IMAGE)
/*
 * Try to get the current terminal cell size. May return -1 x -1.
 */
    ಶೂನ್ಯ
mch_calc_cell_ಗಾತ್ರ(ರಚನೆ cellsize *cs_ಹೊರಹೋಗು)
{
   ರಚನೆ winsize   ws;
   ಪೂರ್ಣಾಂಕ		    retval;
   ಪೂರ್ಣಾಂಕ		    x_cell_ಗಾತ್ರ;
   ಪೂರ್ಣಾಂಕ		    y_cell_ಗಾತ್ರ;

   // If fd is not a terminal, then -1 is returned
   retval = ioctl(read_cmd_fd, TIOCGWINSZ, &ws);

#  ifdef FEAT_EVAL
   ch_log(ಖಾಲಿ, "ioctl(TIOCGWINSZ) %s", retval == 0 ? "success" : "failed");
#  endif

   ಆದರೆ (retval == -1 || ws.ws_ಲಂಬಸಾಲು == 0 || ws.ws_ಅಡ್ಡಸಾಲು == 0)
   {
	cs_ಹೊರಹೋಗು->cs_xpixel = -1;
	cs_ಹೊರಹೋಗು->cs_ypixel = -1;
	ಹಿಂತಿರುಗಿ;
   }

   // Calculate parent tty's pixel per cell.
   x_cell_ಗಾತ್ರ = ws.ws_xpixel / ws.ws_ಲಂಬಸಾಲು;
   y_cell_ಗಾತ್ರ = ws.ws_ypixel / ws.ws_ಅಡ್ಡಸಾಲು;

   cs_ಹೊರಹೋಗು->cs_xpixel = x_cell_ಗಾತ್ರ > 0 ? x_cell_ಗಾತ್ರ : -1;
   cs_ಹೊರಹೋಗು->cs_ypixel = y_cell_ಗಾತ್ರ > 0 ? y_cell_ಗಾತ್ರ : -1;

   // Some terminals do not handle TIOCGWINSZ, send CSI 14 t (query terminal
   // size in pixels), and CSI 16 t (less common, gets cell size in pixels
   // directly).
   ಆದರೆ (x_cell_ಗಾತ್ರ <= 0 || y_cell_ಗಾತ್ರ <= 0)
   {
       // Default to 8x16 pixels for now until we receive a response.
       ಹೊರಹೋಗು_STR("\033[14t");
       ಹೊರಹೋಗು_STR("\033[16t");
       ಹೊರಹೋಗು_flush();
   }
}
# endif

# ಆದರೆ defined(FEAT_TERMINAL)
/*
 * Report the windows size "rows" and "cols" to tty "fd".
 */
    ಪೂರ್ಣಾಂಕ
mch_report_winsize(ಪೂರ್ಣಾಂಕ fd, ಪೂರ್ಣಾಂಕ ಅಡ್ಡಸಾಲುಗಳು_2, ಪೂರ್ಣಾಂಕ ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ)
{
    ಪೂರ್ಣಾಂಕ		tty_fd;
    ಪೂರ್ಣಾಂಕ		retval = -1;

    tty_fd = get_tty_fd(fd);
    ಆದರೆ (tty_fd < 0)
	ಹಿಂತಿರುಗಿ FAIL;

#  ಆದರೆ defined(TIOCSWINSZ)
    ರಚನೆ winsize ws;

    ws.ws_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ws.ws_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು_2;
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
    ch_log(ಖಾಲಿ, "ioctl(TIOCSWINSZ) %s", retval == 0 ? "success" : "failed");
#  elif defined(TIOCSSIZE)
    ರಚನೆ ttysize ts;

    ts.ts_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ = ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ;
    ts.ts_ಸಾಲುಗಳು = ಅಡ್ಡಸಾಲುಗಳು_2;
    retval = ioctl(tty_fd, TIOCSSIZE, &ts);
    ch_log(ಖಾಲಿ, "ioctl(TIOCSSIZE) %s", retval == 0 ? "success" : "failed");
#  endif
    ಆದರೆ (tty_fd != fd)
	close(tty_fd);
    ಹಿಂತಿರುಗಿ retval == 0 ? OK : FAIL;
}
# endif

/*
 * Try to set the window size to Rows and Columns.
 */
    ಶೂನ್ಯ
mch_ಗಣ_shellsize(ಶೂನ್ಯ)
{
    ಆದರೆ (*T_CWS)
    {
	/*
	 * NOTE: if you get an error here that term_set_winsize() is
	 * undefined, check the output of configure.  It could probably not
	 * find a ncurses, termcap or termlib library.
	 */
	ಪದ_ಗಣ_winsize((ಪೂರ್ಣಾಂಕ)ಅಡ್ಡಸಾಲುಗಳು, (ಪೂರ್ಣಾಂಕ)Columns);
	ಹೊರಹೋಗು_flush();
	screen_ಆರಂಭ();			// don't know where cursor is now
    }
}

#ಆದರೆ_ಅಂತ್ಯ // VMS

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // Nothing to do.
}

/*
 * Wait for process "child" to end.
 * Return "child" if it exited properly, <= 0 on error.
 */
    ಸ್ಥಿರ pid_t
wait4pid(pid_t ಮಗು, waitstatus *ಸ್ಥಿತಿ_2)
{
    pid_t wait_pid = 0;
    ದೀರ್ಘ delay_msec = 1;

    ಆಗ (wait_pid != ಮಗು)
    {
	// When compiled with Python threads are probably used, in which case
	// wait() sometimes hangs for no obvious reason.  Use waitpid()
	// instead and loop (like the GUI). Also needed for other interfaces,
	// they might call system().
	wait_pid = waitpid(ಮಗು, ಸ್ಥಿತಿ_2, WNOHANG);
	ಆದರೆ (wait_pid == 0)
	{
	    // Wait for 1 to 10 msec before trying again.
	    mch_delay(delay_msec, MCH_DELAY_IGNOREINPUT | MCH_DELAY_SETTMODE);
	    ಆದರೆ (++delay_msec > 10)
		delay_msec = 10;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (wait_pid <= 0
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ECHILD
		&& errno == ECHILD
#ಆದರೆ_ಅಂತ್ಯ
	   )
	    ನಿಲ್ಲಿಸು;
    }
    ಹಿಂತಿರುಗಿ wait_pid;
}

#ಆದರೆ !defined(USE_SYSTEM) || defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Set the environment for a child process.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_ಮಗು_environment(
	ದೀರ್ಘ	ಅಡ್ಡಸಾಲುಗಳು_2,
	ದೀರ್ಘ	columns,
	ಅಕ್ಷರ	*ಪದ,
	ಪೂರ್ಣಾಂಕ	is_terminal ಬಳಸದ)
{
# ifdef HAVE_SETENV
    ಅಕ್ಷರ	envbuf[50];
# ಇಲ್ಲದಿದ್ದರೆ
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_ಪದ[30];
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_ಅಡ್ಡಸಾಲುಗಳು[20];
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_ಸಾಲುಗಳು[20];
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_Columns[20];
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_Colors[20];
#  ifdef FEAT_TERMINAL
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_ಆವೃತ್ತಿ[20];
#  endif
#  ifdef FEAT_CLIENTSERVER
    ಸ್ಥಿರ ಅಕ್ಷರ	envbuf_Servername[60];
#  endif
# endif

# ifdef HAVE_SETENV
    setenv("TERM", ಪದ, 1);
    sprintf((ಅಕ್ಷರ *)envbuf, "%ld", ಅಡ್ಡಸಾಲುಗಳು_2);
    setenv("ROWS", (ಅಕ್ಷರ *)envbuf, 1);
    sprintf((ಅಕ್ಷರ *)envbuf, "%ld", ಅಡ್ಡಸಾಲುಗಳು_2);
    setenv("LINES", (ಅಕ್ಷರ *)envbuf, 1);
    sprintf((ಅಕ್ಷರ *)envbuf, "%ld", columns);
    setenv("COLUMNS", (ಅಕ್ಷರ *)envbuf, 1);
    sprintf((ಅಕ್ಷರ *)envbuf, "%d", t_colors);
    setenv("COLORS", (ಅಕ್ಷರ *)envbuf, 1);
#  ifdef FEAT_TERMINAL
    ಆದರೆ (is_terminal)
    {
	sprintf((ಅಕ್ಷರ *)envbuf, "%ld",  (ದೀರ್ಘ)get_vim_var_nr(VV_ಆವೃತ್ತಿ));
	setenv("VIM_TERMINAL", (ಅಕ್ಷರ *)envbuf, 1);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    setenv("VIM_SERVERNAME", serverName == ಖಾಲಿ ? "" : (ಅಕ್ಷರ *)serverName, 1);
#  endif
# ಇಲ್ಲದಿದ್ದರೆ
    /*
     * Putenv does not copy the string, it has to remain valid.
     * Use a static array to avoid losing allocated memory.
     * This won't work well when running multiple children...
     */
    vim_snprintf(envbuf_ಪದ, ಗಾತ್ರ(envbuf_ಪದ), "TERM=%s", ಪದ);
    putenv(envbuf_ಪದ);
    vim_snprintf(envbuf_ಅಡ್ಡಸಾಲುಗಳು, ಗಾತ್ರ(envbuf_ಅಡ್ಡಸಾಲುಗಳು), "ROWS=%ld", ಅಡ್ಡಸಾಲುಗಳು_2);
    putenv(envbuf_ಅಡ್ಡಸಾಲುಗಳು);
    vim_snprintf(envbuf_ಸಾಲುಗಳು, ಗಾತ್ರ(envbuf_ಸಾಲುಗಳು), "LINES=%ld", ಅಡ್ಡಸಾಲುಗಳು_2);
    putenv(envbuf_ಸಾಲುಗಳು);
    vim_snprintf(envbuf_Columns, ಗಾತ್ರ(envbuf_Columns),
						       "COLUMNS=%ld", columns);
    putenv(envbuf_Columns);
    vim_snprintf(envbuf_Colors, ಗಾತ್ರ(envbuf_Colors), "COLORS=%ld", t_colors);
    putenv(envbuf_Colors);
#  ifdef FEAT_TERMINAL
    ಆದರೆ (is_terminal)
    {
	vim_snprintf(envbuf_ಆವೃತ್ತಿ, ಗಾತ್ರ(envbuf_ಆವೃತ್ತಿ),
			 "VIM_TERMINAL=%ld", (ದೀರ್ಘ)get_vim_var_nr(VV_ಆವೃತ್ತಿ));
	putenv(envbuf_ಆವೃತ್ತಿ);
    }
#  endif
#  ifdef FEAT_CLIENTSERVER
    vim_snprintf(envbuf_Servername, ಗಾತ್ರ(envbuf_Servername),
	    "VIM_SERVERNAME=%s", serverName == ಖಾಲಿ ? "" : (ಅಕ್ಷರ *)serverName);
    putenv(envbuf_Servername);
#  endif
# endif
}

    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_default_ಮಗು_environment(ಪೂರ್ಣಾಂಕ is_terminal)
{
    ಗಣ_ಮಗು_environment(ಅಡ್ಡಸಾಲುಗಳು, Columns, "dumb", is_terminal);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI) || defined(FEAT_ಕೆಲಸ_CHANNEL)
/*
 * Open a PTY, with FD for the master and slave side.
 * When failing "pty_master_fd" and "pty_slave_fd" are -1.
 * When successful both file descriptors are stored and the allocated pty name
 * is stored in both "*name1" and "*name2".
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
open_pty(ಪೂರ್ಣಾಂಕ *pty_master_fd, ಪೂರ್ಣಾಂಕ *pty_slave_fd, char_u **name1, char_u **name2)
{
    ಅಕ್ಷರ	*tty_name;

    ಆದರೆ (name1 != ಖಾಲಿ)
	*name1 = ಖಾಲಿ;
    ಆದರೆ (name2 != ಖಾಲಿ)
	*name2 = ಖಾಲಿ;

    *pty_master_fd = mch_openpty(&tty_name);	    // open pty
    ಆದರೆ (*pty_master_fd < 0)
	ಹಿಂತಿರುಗಿ;

    // Leaving out O_NOCTTY may lead to waitpid() always returning
    // 0 on Mac OS X 10.7 thereby causing freezes. Let's assume
    // adding O_NOCTTY always works when defined.
# ifdef O_NOCTTY
    *pty_slave_fd = open(tty_name, O_RDWR | O_NOCTTY | O_EXTRA, 0);
# ಇಲ್ಲದಿದ್ದರೆ
    *pty_slave_fd = open(tty_name, O_RDWR | O_EXTRA, 0);
# endif
    ಆದರೆ (*pty_slave_fd < 0)
    {
	close(*pty_master_fd);
	*pty_master_fd = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (name1 != ಖಾಲಿ)
	    *name1 = vim_strsave((char_u *)tty_name);
	ಆದರೆ (name2 != ಖಾಲಿ)
	    *name2 = vim_strsave((char_u *)tty_name);
    }
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Send SIGINT to a child process if "c" is an interrupt character.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
may_send_sigint(ಪೂರ್ಣಾಂಕ c ಬಳಸದ, pid_t pid ಬಳಸದ, pid_t wpid ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ SIGINT
    ಆದರೆ (c == Ctrl_C || c == intr_char)
    {
# ifdef HAVE_SETSID
	kill(-pid, SIGINT);
# ಇಲ್ಲದಿದ್ದರೆ
	kill(0, SIGINT);
# endif
	ಆದರೆ (wpid > 0)
	    kill(wpid, SIGINT);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ !defined(USE_SYSTEM) || defined(FEAT_TERMINAL)

/*
 * Parse "cmd" and return the result in "argvp" which is an allocated array of
 * pointers, the last one is NULL.
 * The "sh_tofree" and "shcf_tofree" must be later freed by the caller.
 */
    ಪೂರ್ಣಾಂಕ
unix_build_argv(
	char_u *cmd,
	ಅಕ್ಷರ ***argvp,
	char_u **sh_tofree,
	char_u **shcf_tofree)
{
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		argc;

    *sh_tofree = vim_strsave(p_sh);
    ಆದರೆ (*sh_tofree == ಖಾಲಿ)		// out of memory
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (mch_parse_cmd(*sh_tofree, TRUE, &argv, &argc) == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;
    *argvp = argv;

    ಆದರೆ (cmd != ಖಾಲಿ)
    {
	char_u	*s;
	char_u	*p;

	ಆದರೆ (extra_shell_arg != ಖಾಲಿ)
	    argv[argc++] = (ಅಕ್ಷರ *)extra_shell_arg;

	// Break 'shellcmdflag' into white separated parts.  This doesn't
	// handle quoted strings, they are very unlikely to appear.
	*shcf_tofree = alloc(STRLEN(p_shcf) + 1);
	ಆದರೆ (*shcf_tofree == ಖಾಲಿ)    // out of memory
	    ಹಿಂತಿರುಗಿ FAIL;
	s = *shcf_tofree;
	p = p_shcf;
	ಆಗ (*p != NUL)
	{
	    argv[argc++] = (ಅಕ್ಷರ *)s;
	    ಆಗ (*p && *p != ' ' && *p != TAB)
		*s++ = *p++;
	    *s++ = NUL;
	    p = skipwhite(p);
	}

	argv[argc++] = (ಅಕ್ಷರ *)cmd;
    }
    argv[argc] = ಖಾಲಿ;
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
/*
 * Use a terminal window to run a shell command in.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_call_shell_terminal(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options ಬಳಸದ)	// SHELL_*, see vim.h
{
    jobopt_T	opt;
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    char_u	*tofree1 = ಖಾಲಿ;
    char_u	*tofree2 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval = -1;
    buf_T	*buf;
    ಕೆಲಸ_T	*ಕೆಲಸ;
    aco_save_T	aco;
    oparg_T	oa;		// operator arguments

    ಆದರೆ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ಹೋಗು theend;

    init_ಕೆಲಸ_options(&opt);
    ch_log(ಖಾಲಿ, "starting terminal for system command '%s'", cmd);
    buf = ಪದ_ಆರಂಭ(ಖಾಲಿ, argv, &opt, ಪದ_ಆರಂಭ_SYSTEM);
    ಆದರೆ (buf == ಖಾಲಿ)
	ಹೋಗು theend;

    ಕೆಲಸ = ಪದ_getjob(buf->b_ಪದ);
    ++ಕೆಲಸ->jv_refcount;

    // Find a window to make "buf" curbuf.
    aucmd_prepbuf(&aco, buf);
    ಆದರೆ (curbuf == buf)
    {
	// Only when managed to find a window for "buf",
	clear_oparg(&oa);
	ಆಗ (ಪದ_use_loop())
	{
	    ಆದರೆ (oa.op_type == OP_NOP && oa.regname == NUL && !VIsual_ಸಕ್ರಿಯ)
	    {
		// If terminal_loop() returns OK we got a key that is handled
		// in Normal model. We don't do redrawing anyway.
		ಆದರೆ (terminal_loop(TRUE) == OK)
		    ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಾಮಾನ್ಯ_cmd(&oa, TRUE);
	}
	retval = ಕೆಲಸ->jv_exitval;
	ch_log(ಖಾಲಿ, "system command finished");

	ಕೆಲಸ_unref(ಕೆಲಸ);

	// restore curwin/curbuf and a few other things
	aucmd_restbuf(&aco);
    }

    // Only require pressing Enter when redrawing, to avoid that system() gets
    // the hit-enter prompt even though it didn't output anything.
    ಆದರೆ (RedrawingDisabled == 0)
	wait_return(TRUE);
    do_ಬಫರ್(DOBUF_WIPE, DOBUF_ಮೊದಲ, FORWARD, buf->b_fnum, TRUE);

theend:
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);
    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SYSTEM
/*
 * Use system() to start the shell: simple but slow.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_call_shell_system(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options)	// SHELL_*, see vim.h
{
# ifdef VMS
    ಅಕ್ಷರ	*ifn = ಖಾಲಿ;
    ಅಕ್ಷರ	*ofn = ಖಾಲಿ;
# endif
    tmode_T	tmode = cur_tmode;
    char_u	*newcmd;	// only needed for unix
    ಪೂರ್ಣಾಂಕ		x;

    ಹೊರಹೋಗು_flush();

    ಆದರೆ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode

# ಆದರೆ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
#  ಆದರೆ defined(FEAT_X11) || defined(FEAT_WAYLAND_CLIPBOARD)
    save_clipboard();
#  endif
    loose_clipboard();
# endif

    ಆದರೆ (cmd == ಖಾಲಿ)
	x = system((ಅಕ್ಷರ *)p_sh);
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ifdef VMS
	ಆದರೆ (ofn = strchr((ಅಕ್ಷರ *)cmd, '>'))
	    *ofn++ = '\0';
	ಆದರೆ (ifn = strchr((ಅಕ್ಷರ *)cmd, '<'))
	{
	    ಅಕ್ಷರ *p;

	    *ifn++ = '\0';
	    p = strchr(ifn,' '); // chop off any trailing spaces
	    ಆದರೆ (p)
		*p = '\0';
	}
	ಆದರೆ (ofn)
	    x = vms_sys((ಅಕ್ಷರ *)cmd, ofn, ifn);
	ಇಲ್ಲದಿದ್ದರೆ
	    x = system((ಅಕ್ಷರ *)cmd);
# ಇಲ್ಲದಿದ್ದರೆ
	newcmd = alloc(STRLEN(p_sh)
		+ (extra_shell_arg == ಖಾಲಿ ? 0 : STRLEN(extra_shell_arg))
		+ STRLEN(p_shcf) + STRLEN(cmd) + 4);
	ಆದರೆ (newcmd == ಖಾಲಿ)
	    x = 0;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    sprintf((ಅಕ್ಷರ *)newcmd, "%s %s %s %s", p_sh,
		    extra_shell_arg == ಖಾಲಿ ? "" : (ಅಕ್ಷರ *)extra_shell_arg,
		    (ಅಕ್ಷರ *)p_shcf,
		    (ಅಕ್ಷರ *)cmd);
	    x = system((ಅಕ್ಷರ *)newcmd);
	    vim_free(newcmd);
	}
# endif
    }
# ifdef VMS
    x = vms_sys_ಸ್ಥಿತಿ(x);
# endif
    ಆದರೆ (emsg_silent)
	;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x == 127)
	ಸಂದೇಶ_puts(_("\nCannot execute shell sh\n"));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x && !(options & SHELL_SILENT))
    {
	ಸಂದೇಶ_puts(_("\nshell returned "));
	ಸಂದೇಶ_outnum((ದೀರ್ಘ)x);
	ಸಂದೇಶ_putchar('\n');
    }

    ಆದರೆ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);	// set to raw mode
    }
    resettitle();
# ಆದರೆ defined(FEAT_CLIPBOARD) && (defined(FEAT_X11) \
	|| defined(FEAT_WAYLAND_CLIPBOARD))
    restore_clipboard();
# endif
    ಹಿಂತಿರುಗಿ x;
}

#ಇಲ್ಲದಿದ್ದರೆ // USE_SYSTEM

# define EXEC_FAILED 122    // Exit code when shell didn't execute.  Don't use
			    // 127, some shells use that already
# define OPEN_NULL_FAILED 123 // Exit code if /dev/null can't be opened

/*
 * Don't use system(), use fork()/exec().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_call_shell_fork(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options)	// SHELL_*, see vim.h
{
    tmode_T	tmode = cur_tmode;
    pid_t	pid;
    pid_t	wpid = 0;
    pid_t	wait_pid = 0;
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = -1;
    ಪೂರ್ಣಾಂಕ		retval = -1;
    ಅಕ್ಷರ	**argv = ಖಾಲಿ;
    char_u	*tofree1 = ಖಾಲಿ;
    char_u	*tofree2 = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		i;
    ಪೂರ್ಣಾಂಕ		pty_master_fd = -1;	    // for pty's
# ifdef FEAT_GUI
    ಪೂರ್ಣಾಂಕ		pty_slave_fd = -1;
# endif
    ಪೂರ್ಣಾಂಕ		fd_toshell[2];		// for pipes
    ಪೂರ್ಣಾಂಕ		fd_fromshell[2];
    ಪೂರ್ಣಾಂಕ		pipe_error = FALSE;
    ಪೂರ್ಣಾಂಕ		did_settmode = FALSE;	// settmode(TMODE_RAW) called

    ಹೊರಹೋಗು_flush();
    ಆದರೆ (options & SHELL_COOKED)
	settmode(TMODE_COOK);		// set to normal mode
    ಆದರೆ (tmode == TMODE_RAW)
	// The shell may have messed with the mode, always set it later.
	cur_tmode = TMODE_UNKNOWN;

    ಆದರೆ (unix_build_argv(cmd, &argv, &tofree1, &tofree2) == FAIL)
	ಹೋಗು error;

    /*
     * For the GUI, when writing the output into the buffer and when reading
     * input from the buffer: Try using a pseudo-tty to get the stdin/stdout
     * of the executed command into the Vim window.  Or use a pipe.
     */
    ಆದರೆ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
	    || (gui.ಒಳ_use && show_shell_mess)
# endif
		    )
    {
# ifdef FEAT_GUI
	/*
	 * Try to open a master pty.
	 * If this works, open the slave pty.
	 * If the slave can't be opened, close the master pty.
	 */
	ಆದರೆ (p_guipty && !(options & (SHELL_READ|SHELL_WRITE)))
	    open_pty(&pty_master_fd, &pty_slave_fd, ಖಾಲಿ, ಖಾಲಿ);
	/*
	 * If not opening a pty or it didn't work, try using pipes.
	 */
	ಆದರೆ (pty_master_fd < 0)
# endif
	{
	    pipe_error = (pipe(fd_toshell) < 0);
	    ಆದರೆ (!pipe_error)			    // pipe create OK
	    {
		pipe_error = (pipe(fd_fromshell) < 0);
		ಆದರೆ (pipe_error)			    // pipe create failed
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		}
	    }
	    ಆದರೆ (pipe_error)
	    {
		ಸಂದೇಶ_puts(_("\nCannot create pipes\n"));
		ಹೊರಹೋಗು_flush();
	    }
	}
    }

    ಆದರೆ (!pipe_error)			// pty or pipe opened or not used
    {
	SIGSET_DECL(curset)
	ಖಂಡ_ಸಂಕೇತಗಳು(&curset);
	pid = fork();	// maybe we should use vfork()
	ಆದರೆ (pid == -1)
	{
	    UNBLOCK_ಸಂಕೇತಗಳು(&curset);

	    ಸಂದೇಶ_puts(_("\nCannot fork\n"));
	    ಆದರೆ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		|| (gui.ಒಳ_use && show_shell_mess)
# endif
		    )
	    {
# ifdef FEAT_GUI
		ಆದರೆ (pty_master_fd >= 0)		// close the pseudo tty
		{
		    close(pty_master_fd);
		    close(pty_slave_fd);
		}
		ಇಲ್ಲದಿದ್ದರೆ				// close the pipes
# endif
		{
		    close(fd_toshell[0]);
		    close(fd_toshell[1]);
		    close(fd_fromshell[0]);
		    close(fd_fromshell[1]);
		}
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pid == 0)	// child
	{
	    reset_ಸಂಕೇತಗಳು();		// handle signals normally
	    UNBLOCK_ಸಂಕೇತಗಳು(&curset);

# ifdef FEAT_EVAL
	    ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
	    {
		ch_log(ಖಾಲಿ, "closing channel log in the child process");
		ch_logfile((char_u *)"", (char_u *)"");
	    }
# endif

	    ಆದರೆ (!show_shell_mess || (options & SHELL_EXPAND))
	    {
		ಪೂರ್ಣಾಂಕ fd;

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
		ಆದರೆ (fd >= 0)
		{
		    vim_ignored = dup(fd); // To replace stdin  (fd 0)
		    vim_ignored = dup(fd); // To replace stdout (fd 1)
		    vim_ignored = dup(fd); // To replace stderr (fd 2)

		    // Don't need this now that we've duplicated it
		    close(fd);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || gui.ಒಳ_use
# endif
		    )
	    {

# ifdef HAVE_SETSID
		// Create our own process group, so that the child and all its
		// children can be kill()ed.  Don't do this when using pipes,
		// because stdin is not a tty, we would lose /dev/tty.
		ಆದರೆ (p_stmp)
		{
		    (ಶೂನ್ಯ)setsid();
#  ಆದರೆ defined(SIGHUP)
		    // When doing "!xterm&" and 'shell' is bash: the shell
		    // will exit and send SIGHUP to all processes in its
		    // group, killing the just started process.  Ignore SIGHUP
		    // to avoid that. (suggested by Simon Schubert)
		    mch_signal(SIGHUP, SIG_IGN);
#  endif
		}
# endif
# ifdef FEAT_GUI
		ಆದರೆ (pty_slave_fd >= 0)
		{
		    // push stream discipline modules
		    ಆದರೆ (options & SHELL_COOKED)
			ಸಿದ್ಧತೆ_slavepty(pty_slave_fd);
#  ifdef TIOCSCTTY
		    // Try to become controlling tty (probably doesn't work,
		    // unless run by root)
		    ioctl(pty_slave_fd, TIOCSCTTY, (ಅಕ್ಷರ *)ಖಾಲಿ);
#  endif
		}
# endif
		ಗಣ_default_ಮಗು_environment(FALSE);

		/*
		 * stderr is only redirected when using the GUI, so that a
		 * program like gpg can still access the terminal to get a
		 * passphrase using stderr.
		 */
# ifdef FEAT_GUI
		ಆದರೆ (pty_master_fd >= 0)
		{
		    close(pty_master_fd);   // close master side of pty

		    // set up stdin/stdout/stderr for the child
		    close(0);
		    vim_ignored = dup(pty_slave_fd);
		    close(1);
		    vim_ignored = dup(pty_slave_fd);
		    ಆದರೆ (gui.ಒಳ_use)
		    {
			close(2);
			vim_ignored = dup(pty_slave_fd);
		    }

		    close(pty_slave_fd);    // has been dupped, close it now
		}
		ಇಲ್ಲದಿದ್ದರೆ
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
		    ಆದರೆ (gui.ಒಳ_use)
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
	ಇಲ್ಲದಿದ್ದರೆ			// parent
	{
	    /*
	     * While child is running, ignore terminating signals.
	     * Do catch CTRL-C, so that "got_int" is set.
	     */
	    catch_ಸಂಕೇತಗಳು(SIG_IGN, SIG_ERR);
	    catch_int_signal();
	    UNBLOCK_ಸಂಕೇತಗಳು(&curset);
# ifdef FEAT_ಕೆಲಸ_CHANNEL
	    ++dont_check_ಕೆಲಸ_ended;
# endif
	    /*
	     * For the GUI we redirect stdin, stdout and stderr to our window.
	     * This is also used to pipe stdin/stdout to/from the external
	     * command.
	     */
	    ಆದರೆ ((options & (SHELL_READ|SHELL_WRITE))
# ifdef FEAT_GUI
		    || (gui.ಒಳ_use && show_shell_mess)
# endif
	       )
	    {
# define BUFLEN 100		// length for buffer, pseudo tty limit is 128
		char_u	    ಬಫರ್[BUFLEN + 1];
		ಪೂರ್ಣಾಂಕ	    ಬಫರ್_off = 0;	// valid bytes in buffer[]
		char_u	    ta_buf[BUFLEN + 1];	// TypeAHead
		ಪೂರ್ಣಾಂಕ	    ta_len = 0;		// valid bytes in ta_buf[]
		ಪೂರ್ಣಾಂಕ	    len;
		ಪೂರ್ಣಾಂಕ	    p_ಇನ್ನಷ್ಟು_save;
		ಪೂರ್ಣಾಂಕ	    old_State;
		ಪೂರ್ಣಾಂಕ	    c;
		ಪೂರ್ಣಾಂಕ	    toshell_fd;
		ಪೂರ್ಣಾಂಕ	    fromshell_fd;
		garray_T    ga;
		ಪೂರ್ಣಾಂಕ	    noread_cnt;
# ifdef ELAPSED_FUNC
		elapsed_T   ಆರಂಭ_tv;
# endif

# ifdef FEAT_GUI
		ಆದರೆ (pty_master_fd >= 0)
		{
		    fromshell_fd = pty_master_fd;
		    toshell_fd = dup(pty_master_fd);
		}
		ಇಲ್ಲದಿದ್ದರೆ
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
		p_ಇನ್ನಷ್ಟು_save = p_ಇನ್ನಷ್ಟು;
		p_ಇನ್ನಷ್ಟು = FALSE;
		old_State = State;
		State = ಬಹುಲಕ_EXTERNCMD;	// don't redraw at window resize

		ಆದರೆ ((options & SHELL_WRITE) && toshell_fd >= 0)
		{
		    // Fork a process that will write the lines to the
		    // external program.
		    ಆದರೆ ((wpid = fork()) == -1)
		    {
			ಸಂದೇಶ_puts(_("\nCannot fork\n"));
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (wpid == 0) // child
		    {
			linenr_T    lnum = curbuf->b_op_ಆರಂಭ.lnum;
			size_t	    written = 0;
			char_u	    *lp = ml_get(lnum);
			size_t	    lplen = (size_t)ml_get_len(lnum);

			close(fromshell_fd);
			ಗಾಗಿ (;;)
			{
			    ಆದರೆ (lplen == 0)
				len = 0;
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (lp[written] == NL)
				// NL -> NUL translation
				len = ಲಿಖ(toshell_fd, "", (size_t)1);
			    ಇಲ್ಲದಿದ್ದರೆ
			    {
				char_u	*s = vim_strchr(lp + written, NL);

				len = ಲಿಖ(toshell_fd, (ಅಕ್ಷರ *)lp + written,
					   s == ಖಾಲಿ ? lplen - written
					      : (size_t)(s - (lp + written)));
			    }
			    ಆದರೆ (len == (ಪೂರ್ಣಾಂಕ)(lplen - written))
			    {
				// Finished a line, add a NL, unless this line
				// should not have one.
				ಆದರೆ (lnum != curbuf->b_op_ಕೊನೆ.lnum
					|| (!curbuf->b_p_bin
					    && curbuf->b_p_fixeol)
					|| (lnum != curbuf->b_no_eol_lnum
					    && (lnum != curbuf->b_ml.ml_ಸಾಲು_ಎಣಿಕೆ
						    || curbuf->b_p_eol)))
				    vim_ignored = ಲಿಖ(toshell_fd, "\n",
								   (size_t)1);
				++lnum;
				ಆದರೆ (lnum > curbuf->b_op_ಕೊನೆ.lnum)
				{
				    // finished all the lines, close pipe
				    close(toshell_fd);
				    ನಿಲ್ಲಿಸು;
				}
				lp = ml_get(lnum);
				lplen = ml_get_len(lnum);
				written = 0;
			    }
			    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (len > 0)
				written += (size_t)len;
			}
			_exit(0);
		    }
		    ಇಲ್ಲದಿದ್ದರೆ // parent
		    {
			close(toshell_fd);
			toshell_fd = -1;
		    }
		}

		ಆದರೆ (options & SHELL_READ)
		    ga_init2(&ga, 1, BUFLEN);

		noread_cnt = 0;
# ifdef ELAPSED_FUNC
		ELAPSED_INIT(ಆರಂಭ_tv);
# endif
		ಗಾಗಿ (;;)
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
		    ಆದರೆ (!(options & SHELL_EXPAND)
			    && ((options &
					 (SHELL_READ|SHELL_WRITE|SHELL_COOKED))
				      != (SHELL_READ|SHELL_WRITE|SHELL_COOKED)
# ifdef FEAT_GUI
						    || gui.ಒಳ_use
# endif
						    )
			    && wait_pid == 0
			    && (ta_len > 0 || noread_cnt > 4))
		    {
		      ಆದರೆ (ta_len == 0)
		      {
			  // Get extra characters when we don't have any.
			  // Reset the counter and timer.
			  noread_cnt = 0;
# ifdef ELAPSED_FUNC
			  ELAPSED_INIT(ಆರಂಭ_tv);
# endif
			  len = ui_inchar(ta_buf, BUFLEN, 10L, 0);
		      }
		      ಆದರೆ (ta_len > 0 || len > 0)
		      {
			/*
			 * For pipes:
			 * Check for CTRL-C: send interrupt signal to child.
			 * Check for CTRL-D: EOF, close pipe to child.
			 */
			ಆದರೆ (len == 1 && (pty_master_fd < 0 || cmd != ಖಾಲಿ))
			{
			    /*
			     * Send SIGINT to the child's group or all
			     * processes in our group.
			     */
			    may_send_sigint(ta_buf[ta_len], pid, wpid);

			    ಆದರೆ (pty_master_fd < 0 && toshell_fd >= 0
					       && ta_buf[ta_len] == Ctrl_D)
			    {
				close(toshell_fd);
				toshell_fd = -1;
			    }
			}

			// Remove Vim-specific codes from the input.
			len = ಪದ_replace_keycodes(ta_buf, ta_len, len);

			/*
			 * For pipes: echo the typed characters.
			 * For a pty this does not seem to work.
			 */
			ಆದರೆ (pty_master_fd < 0)
			{
			    ಗಾಗಿ (i = ta_len; i < ta_len + len; ++i)
			    {
				ಆದರೆ (ta_buf[i] == '\n' || ta_buf[i] == '\b')
				    ಸಂದೇಶ_putchar(ta_buf[i]);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
				{
				    ಪೂರ್ಣಾಂಕ l = (*mb_ptr2len)(ta_buf + i);

				    ಸಂದೇಶ_outtrans_len(ta_buf + i, l);
				    i += l - 1;
				}
				ಇಲ್ಲದಿದ್ದರೆ
				    ಸಂದೇಶ_outtrans_len(ta_buf + i, 1);
			    }
			    windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
			    ಹೊರಹೋಗು_flush();
			}

			ta_len += len;

			/*
			 * Write the characters to the child, unless EOF has
			 * been typed for pipes.  Write one character at a
			 * time, to avoid losing too much typeahead.
			 * When writing buffer lines, drop the typed
			 * characters (only check for CTRL-C).
			 */
			ಆದರೆ (options & SHELL_WRITE)
			    ta_len = 0;
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (toshell_fd >= 0)
			{
			    len = ಲಿಖ(toshell_fd, (ಅಕ್ಷರ *)ta_buf, (size_t)1);
			    ಆದರೆ (len > 0)
			    {
				ta_len -= len;
				mch_memmove(ta_buf, ta_buf + len, ta_len);
			    }
			}
		      }
		    }

		    ಆದರೆ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
# ifdef HAVE_SETSID
			kill(-pid, SIGINT);
# ಇಲ್ಲದಿದ್ದರೆ
			kill(0, SIGINT);
# endif
			ಆದರೆ (wpid > 0)
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
		    ಆಗ (RealWaitForChar(fromshell_fd, 10L, ಖಾಲಿ, ಖಾಲಿ))
		    {
			len = read_eintr(fromshell_fd, ಬಫರ್
				+ ಬಫರ್_off, (size_t)(BUFLEN - ಬಫರ್_off)
				);
			ಆದರೆ (len <= 0)		    // end of file or error
			    ಹೋಗು ಪೂರ್ಣಗೊಂಡ;

			noread_cnt = 0;
			ಆದರೆ (options & SHELL_READ)
			{
			    // Do NUL -> NL translation, append NL separated
			    // lines to the current buffer.
			    ಗಾಗಿ (i = 0; i < len; ++i)
			    {
				ಆದರೆ (ಬಫರ್[i] == NL)
				    append_ga_ಸಾಲು(&ga);
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಬಫರ್[i] == NUL)
				    ga_append(&ga, NL);
				ಇಲ್ಲದಿದ್ದರೆ
				    ga_append(&ga, ಬಫರ್[i]);
			    }
			}
			ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (has_mbyte)
			{
			    ಪೂರ್ಣಾಂಕ		l;
			    char_u	*p;

			    len += ಬಫರ್_off;
			    ಬಫರ್[len] = NUL;

			    // Check if the last character in buffer[] is
			    // incomplete, keep these bytes for the next
			    // round.
			    ಗಾಗಿ (p = ಬಫರ್; p < ಬಫರ್ + len; p += l)
			    {
				l = MB_CPTR2LEN(p);
				ಆದರೆ (l == 0)
				    l = 1;  // NUL byte?
				ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (MB_BYTE2LEN(*p) != l)
				    ನಿಲ್ಲಿಸು;
			    }
			    ಆದರೆ (p == ಬಫರ್)	// no complete character
			    {
				// avoid getting stuck at an illegal byte
				ಆದರೆ (len >= 12)
				    ++p;
				ಇಲ್ಲದಿದ್ದರೆ
				{
				    ಬಫರ್_off = len;
				    ಮುಂದುವರಿ;
				}
			    }
			    c = *p;
			    *p = NUL;
			    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)ಬಫರ್);
			    ಆದರೆ (p < ಬಫರ್ + len)
			    {
				*p = c;
				ಬಫರ್_off = (ಬಫರ್ + len) - p;
				mch_memmove(ಬಫರ್, p, ಬಫರ್_off);
				ಮುಂದುವರಿ;
			    }
			    ಬಫರ್_off = 0;
			}
			ಇಲ್ಲದಿದ್ದರೆ
			{
			    ಬಫರ್[len] = NUL;
			    ಸಂದೇಶ_puts((ಅಕ್ಷರ *)ಬಫರ್);
			}

			windgoto(ಸಂದೇಶ_ಅಡ್ಡಸಾಲು, cmdline_ಲಂಬಸಾಲು_off + ಸಂದೇಶ_ಲಂಬಸಾಲು);
			cursor_on();
			ಹೊರಹೋಗು_flush();
			ಆದರೆ (got_int)
			    ನಿಲ್ಲಿಸು;

# ifdef ELAPSED_FUNC
			ಆದರೆ (wait_pid == 0)
			{
			    ದೀರ್ಘ	msec = ELAPSED_FUNC(ಆರಂಭ_tv);

			    // Avoid that we keep looping here without
			    // checking for a CTRL-C for a long time.  Don't
			    // break out too often to avoid losing typeahead.
			    ಆದರೆ (msec > 2000)
			    {
				noread_cnt = 5;
				ನಿಲ್ಲಿಸು;
			    }
			}
# endif
		    }

		    // If we already detected the child has finished, continue
		    // reading output for a short while.  Some text may be
		    // buffered.
		    ಆದರೆ (wait_pid == pid)
		    {
			ಆದರೆ (noread_cnt < 5)
			    ಮುಂದುವರಿ;
			ನಿಲ್ಲಿಸು;
		    }

		    /*
		     * Check if the child still exists, before checking for
		     * typed characters (otherwise we would lose typeahead).
		     */
		    wait_pid = waitpid(pid, &ಸ್ಥಿತಿ_2, WNOHANG);
		    ಆದರೆ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(ಸ್ಥಿತಿ_2)))
		    {
			// Don't break the loop yet, try reading more
			// characters from "fromshell_fd" first.  When using
			// pipes there might still be something to read and
			// then we'll break the loop at the "break" above.
			wait_pid = pid;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			wait_pid = 0;

# ಆದರೆ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
		    // Handle any X events, e.g. serving the clipboard.
		    clip_update();
# endif
# ifdef FEAT_WAYLAND
		    // Handle Wayland events such as sending data as the source
		    // client.
		    wayland_update();
# endif
		}
ಪೂರ್ಣಗೊಂಡ:
		p_ಇನ್ನಷ್ಟು = p_ಇನ್ನಷ್ಟು_save;
		ಆದರೆ (options & SHELL_READ)
		{
		    ಆದರೆ (ga.ga_len > 0)
		    {
			append_ga_ಸಾಲು(&ga);
			// remember that the NL was missing
			curbuf->b_no_eol_lnum = curwin->w_cursor.lnum;
		    }
		    ಇಲ್ಲದಿದ್ದರೆ
			curbuf->b_no_eol_lnum = 0;
		    ga_clear(&ga);
		}

		/*
		 * Give all typeahead that wasn't used back to ui_inchar().
		 */
		ಆದರೆ (ta_len)
		    ui_inchar_undo(ta_buf, ta_len);
		State = old_State;
		ಆದರೆ (toshell_fd >= 0)
		    close(toshell_fd);
		close(fromshell_fd);
	    }
# ಆದರೆ (defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)) || defined(FEAT_WAYLAND)
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		ದೀರ್ಘ delay_msec = 1;

		ಆದರೆ (tmode == TMODE_RAW)
		    // Possibly disables modifyOtherKeys, so that the system
		    // can recognize CTRL-C.
		    ಹೊರಹೋಗು_str_t_TE();

		/*
		 * Similar to the loop above, but only handle X and Wayland
		 * events, no I/O.
		 */
		ಗಾಗಿ (;;)
		{
		    ಆದರೆ (got_int)
		    {
			// CTRL-C sends a signal to the child, we ignore it
			// ourselves
#  ifdef HAVE_SETSID
			kill(-pid, SIGINT);
#  ಇಲ್ಲದಿದ್ದರೆ
			kill(0, SIGINT);
#  endif
			got_int = FALSE;
		    }
		    wait_pid = waitpid(pid, &ಸ್ಥಿತಿ_2, WNOHANG);
		    ಆದರೆ ((wait_pid == (pid_t)-1 && errno == ECHILD)
			    || (wait_pid == pid && WIFEXITED(ಸ್ಥಿತಿ_2)))
		    {
			wait_pid = pid;
			ನಿಲ್ಲಿಸು;
		    }

#  ಆದರೆ defined(FEAT_XCLIPBOARD) && defined(FEAT_X11)
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
		    ಆದರೆ (++delay_msec > 10)
			delay_msec = 10;
		}

		ಆದರೆ (tmode == TMODE_RAW)
		    // possibly enables modifyOtherKeys again
		    ಹೊರಹೋಗು_str_t_TI();
	    }
# endif

	    /*
	     * Wait until our child has exited.
	     * Ignore wait() returning pids of other children and returning
	     * because of some signal like SIGWINCH.
	     * Don't wait if wait_pid was already set above, indicating the
	     * child already exited.
	     */
	    ಆದರೆ (wait_pid != pid)
		(ಶೂನ್ಯ)wait4pid(pid, &ಸ್ಥಿತಿ_2);

# ifdef FEAT_GUI
	    // Close slave side of pty.  Only do this after the child has
	    // exited, otherwise the child may hang when it tries to write on
	    // the pty.
	    ಆದರೆ (pty_master_fd >= 0)
		close(pty_slave_fd);
# endif

	    // Make sure the child that writes to the external program is
	    // dead.
	    ಆದರೆ (wpid > 0)
	    {
		kill(wpid, SIGKILL);
		wait4pid(wpid, ಖಾಲಿ);
	    }

# ifdef FEAT_ಕೆಲಸ_CHANNEL
	    --dont_check_ಕೆಲಸ_ended;
# endif

	    /*
	     * Set to raw mode right now, otherwise a CTRL-C after
	     * catch_signals() will kill Vim.
	     */
	    ಆದರೆ (tmode == TMODE_RAW)
		settmode(TMODE_RAW);
	    did_settmode = TRUE;
	    ಗಣ_ಸಂಕೇತಗಳು();

	    ಆದರೆ (WIFEXITED(ಸ್ಥಿತಿ_2))
	    {
		// LINTED avoid "bitwise operation on signed value"
		retval = WEXITSTATUS(ಸ್ಥಿತಿ_2);
		ಆದರೆ (retval != 0 && !emsg_silent)
		{
		    ಆದರೆ (retval == EXEC_FAILED)
		    {
			ಸಂದೇಶ_puts(_("\nCannot execute shell "));
			ಸಂದೇಶ_outtrans(p_sh);
			ಸಂದೇಶ_putchar('\n');
		    }
		    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!(options & SHELL_SILENT))
		    {
			ಸಂದೇಶ_puts(_("\nshell returned "));
			ಸಂದೇಶ_outnum((ದೀರ್ಘ)retval);
			ಸಂದೇಶ_putchar('\n');
		    }
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_puts(_("\nCommand terminated\n"));
	}
    }

error:
    ಆದರೆ (!did_settmode)
	ಆದರೆ (tmode == TMODE_RAW)
	    settmode(TMODE_RAW);	// set to raw mode
    resettitle();
    vim_free(argv);
    vim_free(tofree1);
    vim_free(tofree2);

    ಹಿಂತಿರುಗಿ retval;
}
#ಆದರೆ_ಅಂತ್ಯ // USE_SYSTEM

    ಪೂರ್ಣಾಂಕ
mch_call_shell(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options)	// SHELL_*, see vim.h
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_EVAL
    ch_log(ಖಾಲಿ, "executing shell command: %s", cmd);
#ಆದರೆ_ಅಂತ್ಯ
#ಆದರೆ defined(FEAT_GUI) && defined(FEAT_TERMINAL)
    ಆದರೆ (gui.ಒಳ_use && vim_strchr(p_go, GO_TERMINAL) != ಖಾಲಿ
					      && (options & SHELL_SILENT) == 0)
	ಹಿಂತಿರುಗಿ mch_call_shell_terminal(cmd, options);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ USE_SYSTEM
    ಹಿಂತಿರುಗಿ mch_call_shell_system(cmd, options);
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ mch_call_shell_fork(cmd, options);
#ಆದರೆ_ಅಂತ್ಯ
}

#ಆದರೆ defined(FEAT_EVAL)
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
mch_get_cmd_output_ನೇರ(
    ಅಕ್ಷರ	**argv,
    char_u	*infile,
    ಪೂರ್ಣಾಂಕ		flags ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		*ret_len)
{
    pid_t	pid;
    ಪೂರ್ಣಾಂಕ		fd_ಹೊರಹೋಗು[2] = {-1, -1};
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = -1;
    char_u	*ಬಫರ್ = ಖಾಲಿ;
    garray_T	ga;
    SIGSET_DECL(curset)

    ga_init2(&ga, 1, 4096);

    ch_log(ಖಾಲಿ, "directly executing: %s", argv[0]);

    ಆದರೆ (pipe(fd_ಹೊರಹೋಗು) < 0)
    {
	emsg(_(e_cannot_create_pipes));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಖಂಡ_ಸಂಕೇತಗಳು(&curset);
# ifdef VMS
    pid = 0; // VMS does not have fork
# ಇಲ್ಲದಿದ್ದರೆ
    pid = fork();
# endif
    ಆದರೆ (pid == -1)
    {
	UNBLOCK_ಸಂಕೇತಗಳು(&curset);
	close(fd_ಹೊರಹೋಗು[0]);
	close(fd_ಹೊರಹೋಗು[1]);
	emsg(_("\nCannot fork\n"));
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    }

    ಆದರೆ (pid == 0)
    {
	// child process
	reset_ಸಂಕೇತಗಳು();
	UNBLOCK_ಸಂಕೇತಗಳು(&curset);

	ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
	{
	    ch_log(ಖಾಲಿ, "closing channel log in the child process");
	    ch_logfile((char_u *)"", (char_u *)"");
	}

	// Set up stdin.
	ಆದರೆ (infile != ಖಾಲಿ)
	{
	    ಪೂರ್ಣಾಂಕ fd_ಒಳ = open((ಅಕ್ಷರ *)infile, O_RDONLY);
	    ಆದರೆ (fd_ಒಳ >= 0)
	    {
		close(0);
		vim_ignored = dup(fd_ಒಳ);
		close(fd_ಒಳ);
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಪೂರ್ಣಾಂಕ nullfd = open("/dev/null", O_RDONLY);
	    ಆದರೆ (nullfd >= 0)
	    {
		close(0);
		vim_ignored = dup(nullfd);
		close(nullfd);
	    }
	}

	// Set up stdout: write end of pipe.
	close(fd_ಹೊರಹೋಗು[0]);
	close(1);
	vim_ignored = dup(fd_ಹೊರಹೋಗು[1]);
	// Also redirect stderr to the pipe.
	close(2);
	vim_ignored = dup(fd_ಹೊರಹೋಗು[1]);
	close(fd_ಹೊರಹೋಗು[1]);

	execvp(argv[0], argv);
	_exit(127);
	// NOTREACHED
    }

    // parent process
    UNBLOCK_ಸಂಕೇತಗಳು(&curset);
    close(fd_ಹೊರಹೋಗು[1]);

    // Read output from child.
    ಗಾಗಿ (;;)
    {
	ಅಕ್ಷರ	buf[4096];
	ಪೂರ್ಣಾಂಕ	n;

	n = (ಪೂರ್ಣಾಂಕ)ಪಠ(fd_ಹೊರಹೋಗು[0], buf, ಗಾತ್ರ(buf));
	ಆದರೆ (n <= 0)
	    ನಿಲ್ಲಿಸು;
	ಆದರೆ (ga_grow(&ga, n) == FAIL)
	    ನಿಲ್ಲಿಸು;
	mch_memmove((ಅಕ್ಷರ *)ga.ga_ದತ್ತಾಂಶ + ga.ga_len, buf, n);
	ga.ga_len += n;
    }
    close(fd_ಹೊರಹೋಗು[0]);

    // Wait for child to finish.
    (ಶೂನ್ಯ)waitpid(pid, &ಸ್ಥಿತಿ_2, 0);
    ಆದರೆ (WIFEXITED(ಸ್ಥಿತಿ_2))
	ಸ್ಥಿತಿ_2 = WEXITSTATUS(ಸ್ಥಿತಿ_2);
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = -1;
    ಗಣ_vim_var_nr(VV_SHELL_ERROR, (ದೀರ್ಘ)ಸ್ಥಿತಿ_2);

    ಆದರೆ (ga.ga_len > 0)
    {
	ಬಫರ್ = alloc(ga.ga_len + 1);
	ಆದರೆ (ಬಫರ್ != ಖಾಲಿ)
	{
	    mch_memmove(ಬಫರ್, ga.ga_ದತ್ತಾಂಶ, ga.ga_len);
	    ಆದರೆ (ret_len == ಖಾಲಿ)
	    {
		ಪೂರ್ಣಾಂಕ	i;

		// Change NUL into SOH, otherwise the string is truncated.
		ಗಾಗಿ (i = 0; i < ga.ga_len; ++i)
		    ಆದರೆ (ಬಫರ್[i] == NUL)
			ಬಫರ್[i] = 1;
		ಬಫರ್[ga.ga_len] = NUL;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*ret_len = ga.ga_len;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ret_len != ಖಾಲಿ)
	*ret_len = 0;

    ga_clear(&ga);
    ಹಿಂತಿರುಗಿ ಬಫರ್;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_ಕೆಲಸ_CHANNEL)
    ಶೂನ್ಯ
mch_ಕೆಲಸ_ಆರಂಭ(ಅಕ್ಷರ **argv, ಕೆಲಸ_T *ಕೆಲಸ, jobopt_T *options, ಪೂರ್ಣಾಂಕ is_terminal)
{
    pid_t	pid;
    ಪೂರ್ಣಾಂಕ		fd_ಒಳ[2] = {-1, -1};	// for stdin
    ಪೂರ್ಣಾಂಕ		fd_ಹೊರಹೋಗು[2] = {-1, -1};	// for stdout
    ಪೂರ್ಣಾಂಕ		fd_err[2] = {-1, -1};	// for stderr
    ಪೂರ್ಣಾಂಕ		pty_master_fd = -1;
    ಪೂರ್ಣಾಂಕ		pty_slave_fd = -1;
    channel_T	*channel = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		use_null_for_ಒಳ = options->jo_io[PART_ಒಳ] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_null_for_ಹೊರಹೋಗು = options->jo_io[PART_ಹೊರಹೋಗು] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_null_for_err = options->jo_io[PART_ERR] == JIO_NULL;
    ಪೂರ್ಣಾಂಕ		use_file_for_ಒಳ = options->jo_io[PART_ಒಳ] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_file_for_ಹೊರಹೋಗು = options->jo_io[PART_ಹೊರಹೋಗು] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_file_for_err = options->jo_io[PART_ERR] == JIO_FILE;
    ಪೂರ್ಣಾಂಕ		use_ಬಫರ್_for_ಒಳ = options->jo_io[PART_ಒಳ] == JIO_ಬಫರ್;
    ಪೂರ್ಣಾಂಕ		use_ಹೊರಹೋಗು_for_err = options->jo_io[PART_ERR] == JIO_ಹೊರಹೋಗು;
    SIGSET_DECL(curset)

    ಆದರೆ (use_ಹೊರಹೋಗು_for_err && use_null_for_ಹೊರಹೋಗು)
	use_null_for_err = TRUE;

    // default is to fail
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_FAILED;

    ಆದರೆ (options->jo_pty
	    && (!(use_file_for_ಒಳ || use_null_for_ಒಳ)
		|| !(use_file_for_ಹೊರಹೋಗು || use_null_for_ಹೊರಹೋಗು)
		|| !(use_ಹೊರಹೋಗು_for_err || use_file_for_err || use_null_for_err)))
	open_pty(&pty_master_fd, &pty_slave_fd,
					    &ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು, &ಕೆಲಸ->jv_tty_ಒಳ);

    // TODO: without the channel feature connect the child to /dev/null?
    // Open pipes for stdin, stdout, stderr.
    ಆದರೆ (use_file_for_ಒಳ)
    {
	char_u *fname = options->jo_io_name[PART_ಒಳ];

	fd_ಒಳ[0] = mch_open((ಅಕ್ಷರ *)fname, O_RDONLY, 0);
	ಆದರೆ (fd_ಒಳ[0] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
	// When writing buffer lines to the input don't use the pty, so that
	// the pipe can be closed when all lines were written.
	ಆದರೆ (!use_null_for_ಒಳ && (pty_master_fd < 0 || use_ಬಫರ್_for_ಒಳ)
							    && pipe(fd_ಒಳ) < 0)
	    ಹೋಗು failed;

    ಆದರೆ (use_file_for_ಹೊರಹೋಗು)
    {
	char_u *fname = options->jo_io_name[PART_ಹೊರಹೋಗು];

	fd_ಹೊರಹೋಗು[1] = mch_open((ಅಕ್ಷರ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0644);
	ಆದರೆ (fd_ಹೊರಹೋಗು[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!use_null_for_ಹೊರಹೋಗು && pty_master_fd < 0 && pipe(fd_ಹೊರಹೋಗು) < 0)
	ಹೋಗು failed;

    ಆದರೆ (use_file_for_err)
    {
	char_u *fname = options->jo_io_name[PART_ERR];

	fd_err[1] = mch_open((ಅಕ್ಷರ *)fname, O_WRONLY | O_CREAT | O_TRUNC, 0600);
	ಆದರೆ (fd_err[1] < 0)
	{
	    semsg(_(e_cant_open_file_str), fname);
	    ಹೋಗು failed;
	}
    }
    // only create a pipe for the error fd, when either a callback has been setup
    // or pty is not used (e.g. terminal uses pty by default)
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!use_ಹೊರಹೋಗು_for_err && !use_null_for_err
		&& (pty_master_fd < 0 || (options->jo_ಗಣ & JO_ERR_CALLBACK))
			    && pipe(fd_err) < 0)
	ಹೋಗು failed;

    ಆದರೆ (!use_null_for_ಒಳ || !use_null_for_ಹೊರಹೋಗು || !use_null_for_err)
    {
	ಆದರೆ (options->jo_ಗಣ & JO_CHANNEL)
	{
	    channel = options->jo_channel;
	    ಆದರೆ (channel != ಖಾಲಿ)
		++channel->ch_refcount;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    channel = ಕೂಡಿಸು_channel();
	ಆದರೆ (channel == ಖಾಲಿ)
	    ಹೋಗು failed;
	ಆದರೆ (ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು != ಖಾಲಿ)
	    ch_log(channel, "using pty %s on fd %d",
					       ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು, pty_master_fd);
    }

    ಖಂಡ_ಸಂಕೇತಗಳು(&curset);
    pid = fork();	// maybe we should use vfork()
    ಆದರೆ (pid == -1)
    {
	// failed to fork
	UNBLOCK_ಸಂಕೇತಗಳು(&curset);
	ಹೋಗು failed;
    }
    ಆದರೆ (pid == 0)
    {
	ಪೂರ್ಣಾಂಕ	null_fd = -1;
	ಪೂರ್ಣಾಂಕ	stderr_ಕೆಲಸಮಾಡುತ್ತದೆ = TRUE;

	// child
	reset_ಸಂಕೇತಗಳು();		// handle signals normally
	UNBLOCK_ಸಂಕೇತಗಳು(&curset);

# ifdef FEAT_EVAL
	ಆದರೆ (ch_log_ಸಕ್ರಿಯ())
	    // close the log file in the child
	    ch_logfile((char_u *)"", (char_u *)"");
# endif

# ifdef HAVE_SETSID
	// Create our own process group, so that the child and all its
	// children can be kill()ed.  Don't do this when using pipes,
	// because stdin is not a tty, we would lose /dev/tty.
	(ಶೂನ್ಯ)setsid();
# endif

# ifdef FEAT_TERMINAL
	ಆದರೆ (options->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು > 0)
	{
	    ಅಕ್ಷರ *ಪದ = (ಅಕ್ಷರ *)T_NAME;

#  ifdef FEAT_GUI
	    ಆದರೆ (ಪದ_is_gui(T_NAME))
		// In the GUI 'term' is not what we want, use $TERM.
		ಪದ = getenv("TERM");
#  endif
	    // Use 'term' or $TERM if it starts with "xterm", otherwise fall
	    // back to "xterm" or "xterm-color".
	    ಆದರೆ (ಪದ == ಖಾಲಿ || *ಪದ == NUL || STRNCMP(ಪದ, "xterm", 5) != 0)
	    {
		ಆದರೆ (t_colors >= 256)
		    // TODO: should we check this name is supported?
		    ಪದ = "xterm-256color";
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (t_colors > 16)
		    ಪದ = "xterm-color";
		ಇಲ್ಲದಿದ್ದರೆ
		    ಪದ = "xterm";
	    }
	    ಗಣ_ಮಗು_environment(
		    (ದೀರ್ಘ)options->jo_ಪದ_ಅಡ್ಡಸಾಲುಗಳು,
		    (ದೀರ್ಘ)options->jo_ಪದ_ಲಂಬಸಾಲು_ಸಂಖ್ಯೆ,
		    ಪದ,
		    is_terminal);
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	    ಗಣ_default_ಮಗು_environment(is_terminal);

	ಆದರೆ (options->jo_env != ಖಾಲಿ)
	{
	    dict_T	*dict = options->jo_env;
	    hashitem_T	*hi;
	    ಪೂರ್ಣಾಂಕ		todo = (ಪೂರ್ಣಾಂಕ)dict->dv_hashtab.ht_ಬಳಸಿದ;

	    FOR_ALL_HASHTAB_ವಸ್ತುಗಳು(&dict->dv_hashtab, hi, todo)
		ಆದರೆ (!HASHITEM_ಖಾಲಿಗಳು(hi))
		{
		    typval_T *ಅಂಶ = &dict_lookup(hi)->di_tv;

		    vim_setenv(hi->hi_ಕೀಲಿ, tv_get_string(ಅಂಶ));
		    --todo;
		}
	}

	ಆದರೆ (use_null_for_ಒಳ || use_null_for_ಹೊರಹೋಗು || use_null_for_err)
	{
	    null_fd = open("/dev/null", O_RDWR | O_EXTRA, 0);
	    ಆದರೆ (null_fd < 0)
	    {
		perror("opening /dev/null failed");
		_exit(OPEN_NULL_FAILED);
	    }
	}

	ಆದರೆ (pty_slave_fd >= 0)
	{
	    // push stream discipline modules
	    ಸಿದ್ಧತೆ_slavepty(pty_slave_fd);
# ifdef TIOCSCTTY
	    // Try to become controlling tty (probably doesn't work,
	    // unless run by root)
	    ioctl(pty_slave_fd, TIOCSCTTY, (ಅಕ್ಷರ *)ಖಾಲಿ);
# endif
	}

	// set up stdin for the child
	close(0);
	ಆದರೆ (use_null_for_ಒಳ && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd_ಒಳ[0] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_ignored = dup(fd_ಒಳ[0]);

	// set up stderr for the child
	close(2);
	ಆದರೆ (use_null_for_err && null_fd >= 0)
	{
	    vim_ignored = dup(null_fd);
	    stderr_ಕೆಲಸಮಾಡುತ್ತದೆ = FALSE;
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_ಹೊರಹೋಗು_for_err)
	    vim_ignored = dup(fd_ಹೊರಹೋಗು[1]);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd_err[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_ignored = dup(fd_err[1]);

	// set up stdout for the child
	close(1);
	ಆದರೆ (use_null_for_ಹೊರಹೋಗು && null_fd >= 0)
	    vim_ignored = dup(null_fd);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fd_ಹೊರಹೋಗು[1] < 0)
	    vim_ignored = dup(pty_slave_fd);
	ಇಲ್ಲದಿದ್ದರೆ
	    vim_ignored = dup(fd_ಹೊರಹೋಗು[1]);

	ಆದರೆ (fd_ಒಳ[0] >= 0)
	    close(fd_ಒಳ[0]);
	ಆದರೆ (fd_ಒಳ[1] >= 0)
	    close(fd_ಒಳ[1]);
	ಆದರೆ (fd_ಹೊರಹೋಗು[0] >= 0)
	    close(fd_ಹೊರಹೋಗು[0]);
	ಆದರೆ (fd_ಹೊರಹೋಗು[1] >= 0)
	    close(fd_ಹೊರಹೋಗು[1]);
	ಆದರೆ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ಆದರೆ (fd_err[1] >= 0)
	    close(fd_err[1]);
	ಆದರೆ (pty_master_fd >= 0)
	{
	    close(pty_master_fd); // not used in the child
	    close(pty_slave_fd);  // was duped above
	}

	ಆದರೆ (null_fd >= 0)
	    close(null_fd);

	ಆದರೆ (options->jo_cwd != ಖಾಲಿ && mch_chdir((ಅಕ್ಷರ *)options->jo_cwd) != 0)
	    _exit(EXEC_FAILED);

	// See above for type of argv.
	execvp(argv[0], argv);

	ಆದರೆ (stderr_ಕೆಲಸಮಾಡುತ್ತದೆ)
	    perror("executing job failed");
# ifdef EXITFREE
	// calling free_all_mem() here causes problems. Ignore valgrind
	// reporting possibly leaked memory.
# endif
	_exit(EXEC_FAILED);	    // exec failed, return failure code
    }

    // parent
    UNBLOCK_ಸಂಕೇತಗಳು(&curset);

    ಕೆಲಸ->jv_pid = pid;
    ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_STARTED;
    ಕೆಲಸ->jv_channel = channel;  // ch_refcount was set above

    ಆದರೆ (pty_master_fd >= 0)
	close(pty_slave_fd); // not used in the parent
    // close child stdin, stdout and stderr
    ಆದರೆ (fd_ಒಳ[0] >= 0)
	close(fd_ಒಳ[0]);
    ಆದರೆ (fd_ಹೊರಹೋಗು[1] >= 0)
	close(fd_ಹೊರಹೋಗು[1]);
    ಆದರೆ (fd_err[1] >= 0)
	close(fd_err[1]);
    ಆದರೆ (channel != ಖಾಲಿ)
    {
	ಪೂರ್ಣಾಂಕ ಒಳ_fd = INVALID_FD;
	ಪೂರ್ಣಾಂಕ ಹೊರಹೋಗು_fd = INVALID_FD;
	ಪೂರ್ಣಾಂಕ err_fd = INVALID_FD;

	ಆದರೆ (!(use_file_for_ಒಳ || use_null_for_ಒಳ))
	    ಒಳ_fd = fd_ಒಳ[1] >= 0 ? fd_ಒಳ[1] : pty_master_fd;

	ಆದರೆ (!(use_file_for_ಹೊರಹೋಗು || use_null_for_ಹೊರಹೋಗು))
	    ಹೊರಹೋಗು_fd = fd_ಹೊರಹೋಗು[0] >= 0 ? fd_ಹೊರಹೋಗು[0] : pty_master_fd;

	// When using pty_master_fd only set it for stdout, do not duplicate
	// it for stderr, it only needs to be read once.
	ಆದರೆ (!(use_ಹೊರಹೋಗು_for_err || use_file_for_err || use_null_for_err))
	{
	    ಆದರೆ (fd_err[0] >= 0)
		err_fd = fd_err[0];
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಹೊರಹೋಗು_fd != pty_master_fd)
		err_fd = pty_master_fd;
	}

	channel_ಗಣ_pipes(channel, ಒಳ_fd, ಹೊರಹೋಗು_fd, err_fd);
	channel_ಗಣ_ಕೆಲಸ(channel, ಕೆಲಸ, options);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (fd_ಒಳ[1] >= 0)
	    close(fd_ಒಳ[1]);
	ಆದರೆ (fd_ಹೊರಹೋಗು[0] >= 0)
	    close(fd_ಹೊರಹೋಗು[0]);
	ಆದರೆ (fd_err[0] >= 0)
	    close(fd_err[0]);
	ಆದರೆ (pty_master_fd >= 0)
	    close(pty_master_fd);
    }

    // success!
    ಹಿಂತಿರುಗಿ;

failed:
    channel_unref(channel);
    ಆದರೆ (fd_ಒಳ[0] >= 0)
	close(fd_ಒಳ[0]);
    ಆದರೆ (fd_ಒಳ[1] >= 0)
	close(fd_ಒಳ[1]);
    ಆದರೆ (fd_ಹೊರಹೋಗು[0] >= 0)
	close(fd_ಹೊರಹೋಗು[0]);
    ಆದರೆ (fd_ಹೊರಹೋಗು[1] >= 0)
	close(fd_ಹೊರಹೋಗು[1]);
    ಆದರೆ (fd_err[0] >= 0)
	close(fd_err[0]);
    ಆದರೆ (fd_err[1] >= 0)
	close(fd_err[1]);
    ಆದರೆ (pty_master_fd >= 0)
	close(pty_master_fd);
    ಆದರೆ (pty_slave_fd >= 0)
	close(pty_slave_fd);
}

    ಸ್ಥಿರ char_u *
get_signal_name(ಪೂರ್ಣಾಂಕ sig)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	numbuf[NUMBUFLEN];

    ಆದರೆ (sig == SIGKILL)
	ಹಿಂತಿರುಗಿ vim_strnsave((char_u *)"kill", STRLEN_LITERAL("kill"));

    ಗಾಗಿ (i = 0; signal_info[i].sig != -1; i++)
	ಆದರೆ (sig == signal_info[i].sig)
	    ಹಿಂತಿರುಗಿ strlow_save((char_u *)signal_info[i].name);

    i = vim_snprintf((ಅಕ್ಷರ *)numbuf, NUMBUFLEN, "%d", sig);
    ಹಿಂತಿರುಗಿ vim_strnsave(numbuf, i);
}

    ಅಕ್ಷರ *
mch_ಕೆಲಸ_ಸ್ಥಿತಿ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = -1;
    pid_t	wait_pid = 0;

    wait_pid = waitpid(ಕೆಲಸ->jv_pid, &ಸ್ಥಿತಿ_2, WNOHANG);
    ಆದರೆ (wait_pid == -1)
    {
	ಪೂರ್ಣಾಂಕ waitpid_errno = errno;
	ಆದರೆ (waitpid_errno == ECHILD && mch_process_running(ಕೆಲಸ->jv_pid))
	    // The process is alive, but it was probably reparented (for
	    // example by ptrace called by a debugger like lldb or gdb).
	    // Note: This assumes that process IDs are not reused.
	    ಹಿಂತಿರುಗಿ "run";

	// process must have exited
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED)
	    ch_log(ಕೆಲಸ->jv_channel, "Job no longer exists: %s",
						      strerror(waitpid_errno));
	ಹೋಗು return_dead;
    }
    ಆದರೆ (wait_pid == 0)
	ಹಿಂತಿರುಗಿ "run";
    ಆದರೆ (WIFEXITED(ಸ್ಥಿತಿ_2))
    {
	// LINTED avoid "bitwise operation on signed value"
	ಕೆಲಸ->jv_exitval = WEXITSTATUS(ಸ್ಥಿತಿ_2);
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED)
	    ch_log(ಕೆಲಸ->jv_channel, "Job exited with %d", ಕೆಲಸ->jv_exitval);
	ಹೋಗು return_dead;
    }
    ಆದರೆ (WIFSIGNALED(ಸ್ಥಿತಿ_2))
    {
	ಕೆಲಸ->jv_exitval = -1;
	ಕೆಲಸ->jv_termsig = get_signal_name(WTERMSIG(ಸ್ಥಿತಿ_2));
	ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED && ಕೆಲಸ->jv_termsig != ಖಾಲಿ)
	    ch_log(ಕೆಲಸ->jv_channel, "Job terminated by signal \"%s\"",
							      ಕೆಲಸ->jv_termsig);
	ಹೋಗು return_dead;
    }
    ಹಿಂತಿರುಗಿ "run";

return_dead:
    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED)
	ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ENDED;
    ಹಿಂತಿರುಗಿ "dead";
}

    ಕೆಲಸ_T *
mch_detect_ended_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ_list)
{
    ಪೂರ್ಣಾಂಕ		ಸ್ಥಿತಿ_2 = -1;
    pid_t	wait_pid = 0;
    ಕೆಲಸ_T	*ಕೆಲಸ;

# ifndef USE_SYSTEM
    // Do not do this when waiting for a shell command to finish, we would get
    // the exit value here (and discard it), the exit value obtained there
    // would then be wrong.
    ಆದರೆ (dont_check_ಕೆಲಸ_ended > 0)
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
# endif

    wait_pid = waitpid(-1, &ಸ್ಥಿತಿ_2, WNOHANG);
    ಆದರೆ (wait_pid <= 0)
	// no process ended
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
    ಗಾಗಿ (ಕೆಲಸ = ಕೆಲಸ_list; ಕೆಲಸ != ಖಾಲಿ; ಕೆಲಸ = ಕೆಲಸ->jv_ಮುಂದಿನ)
    {
	ಆದರೆ (ಕೆಲಸ->jv_pid == wait_pid)
	{
	    ಆದರೆ (WIFEXITED(ಸ್ಥಿತಿ_2))
		// LINTED avoid "bitwise operation on signed value"
		ಕೆಲಸ->jv_exitval = WEXITSTATUS(ಸ್ಥಿತಿ_2);
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (WIFSIGNALED(ಸ್ಥಿತಿ_2))
	    {
		ಕೆಲಸ->jv_exitval = -1;
		ಕೆಲಸ->jv_termsig = get_signal_name(WTERMSIG(ಸ್ಥಿತಿ_2));
	    }
	    ಆದರೆ (ಕೆಲಸ->jv_ಸ್ಥಿತಿ < ಕೆಲಸ_ENDED)
	    {
		ch_log(ಕೆಲಸ->jv_channel, "Job ended");
		ಕೆಲಸ->jv_ಸ್ಥಿತಿ = ಕೆಲಸ_ENDED;
	    }
	    ಹಿಂತಿರುಗಿ ಕೆಲಸ;
	}
    }
    ಹಿಂತಿರುಗಿ ಖಾಲಿ;
}

/*
 * Send a (deadly) signal to "job".
 * Return FAIL if "how" is not a valid name.
 */
    ಪೂರ್ಣಾಂಕ
mch_signal_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ, char_u *how)
{
    ಪೂರ್ಣಾಂಕ	    sig = -1;

    ಆದರೆ (*how == NUL || STRCMP(how, "term") == 0)
	sig = SIGTERM;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(how, "hup") == 0)
	sig = SIGHUP;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(how, "quit") == 0)
	sig = SIGQUIT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(how, "int") == 0)
	sig = SIGINT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(how, "kill") == 0)
	sig = SIGKILL;
# ifdef SIGWINCH
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(how, "winch") == 0)
	sig = SIGWINCH;
# endif
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಸುರಕ್ಷಿತ_isdigit(*how))
	sig = atoi((ಅಕ್ಷರ *)how);
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ FAIL;

    // Never kill ourselves!
    ಆದರೆ (ಕೆಲಸ->jv_pid != 0)
    {
	// TODO: have an option to only kill the process, not the group?
	kill(-ಕೆಲಸ->jv_pid, sig);
	kill(ಕೆಲಸ->jv_pid, sig);
    }

    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Clear the data related to "job".
 */
    ಶೂನ್ಯ
mch_clear_ಕೆಲಸ(ಕೆಲಸ_T *ಕೆಲಸ)
{
    // call waitpid because child process may become zombie
    (ಶೂನ್ಯ)waitpid(ಕೆಲಸ->jv_pid, ಖಾಲಿ, WNOHANG);
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_TERMINAL)
    ಪೂರ್ಣಾಂಕ
mch_create_pty_channel(ಕೆಲಸ_T *ಕೆಲಸ, jobopt_T *options)
{
    ಪೂರ್ಣಾಂಕ		pty_master_fd = -1;
    ಪೂರ್ಣಾಂಕ		pty_slave_fd = -1;
    channel_T	*channel;

    open_pty(&pty_master_fd, &pty_slave_fd, &ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು, &ಕೆಲಸ->jv_tty_ಒಳ);
    ಆದರೆ (pty_master_fd < 0 || pty_slave_fd < 0)
	ಹಿಂತಿರುಗಿ FAIL;
    close(pty_slave_fd);

    channel = ಕೂಡಿಸು_channel();
    ಆದರೆ (channel == ಖಾಲಿ)
    {
	close(pty_master_fd);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಆದರೆ (ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು != ಖಾಲಿ)
	ch_log(channel, "using pty %s on fd %d",
					       ಕೆಲಸ->jv_tty_ಹೊರಹೋಗು, pty_master_fd);
    ಕೆಲಸ->jv_channel = channel;  // ch_refcount was set by add_channel()
    channel->ch_keep_open = TRUE;

    // Only set the pty_master_fd for stdout, do not duplicate it for stderr,
    // it only needs to be read once.
    channel_ಗಣ_pipes(channel, pty_master_fd, pty_master_fd, INVALID_FD);
    channel_ಗಣ_ಕೆಲಸ(channel, ಕೆಲಸ, options);
    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Check for CTRL-C typed by reading all available characters.
 * In cooked mode we should get SIGINT, no need to check.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಂಕ force)
{
    ಆದರೆ ((mch_cur_tmode == TMODE_RAW || force)
			       && RealWaitForChar(read_cmd_fd, 0L, ಖಾಲಿ, ಖಾಲಿ))
	fill_ಒಳಹರಿವು_buf(FALSE);
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
WaitForChar(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_TIMERS
    ಹಿಂತಿರುಗಿ ui_wait_for_chars_or_timer(
		    msec, WaitForCharOrMouse, interrupted, ignore_ಒಳಹರಿವು) == OK;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ WaitForCharOrMouse(msec, interrupted, ignore_ಒಳಹರಿವು);
#ಆದರೆ_ಅಂತ್ಯ
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
WaitForCharOrMouse(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ *interrupted, ಪೂರ್ಣಾಂಕ ignore_ಒಳಹರಿವು)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_MOUSE_GPM
    ಪೂರ್ಣಾಂಕ		gpm_process_wanted;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_XCLIPBOARD
    ಪೂರ್ಣಾಂಕ		rest;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		avail;

    ಆದರೆ (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_ಲಭ್ಯ())	    // something in inbuf[]
	ಹಿಂತಿರುಗಿ 1;

#ಆದರೆ defined(FEAT_MOUSE_DEC)
    // May need to query the mouse position.
    ಆದರೆ (WantQueryMouse)
    {
	WantQueryMouse = FALSE;
	ಆದರೆ (!no_query_mouse_for_testing)
	    mch_write((char_u *)"\033[1'|", 5);
    }
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * For FEAT_MOUSE_GPM and FEAT_XCLIPBOARD we loop here to process mouse
     * events.  This is a bit complicated, because they might both be defined.
     */
#ಆದರೆ defined(FEAT_MOUSE_GPM) || defined(FEAT_XCLIPBOARD)
# ifdef FEAT_XCLIPBOARD
    rest = 0;
    ಆದರೆ (do_xterm_trace())
	rest = msec;
# endif
    ಮಾಡು
    {
# ifdef FEAT_XCLIPBOARD
	ಆದರೆ (rest != 0)
	{
	    msec = XT_TRACE_DELAY;
	    ಆದರೆ (rest >= 0 && rest < XT_TRACE_DELAY)
		msec = rest;
	    ಆದರೆ (rest >= 0)
		rest -= msec;
	}
# endif
# ifdef FEAT_SOUND_MACOSX
	// Invoke any pending sound callbacks.
	process_cfrunloop();
# endif
# ifdef FEAT_SOUND_CANBERRA
	// Invoke any pending sound callbacks.
	ಆದರೆ (has_sound_callback_ಒಳ_ಸರತಿ())
	    invoke_sound_callback();
# endif
# ifdef FEAT_MOUSE_GPM
	gpm_process_wanted = 0;
	avail = RealWaitForChar(read_cmd_fd, msec,
					     &gpm_process_wanted, interrupted);
	ಆದರೆ (!avail && !gpm_process_wanted)
# ಇಲ್ಲದಿದ್ದರೆ
	avail = RealWaitForChar(read_cmd_fd, msec, ಖಾಲಿ, interrupted);
	ಆದರೆ (!avail)
# endif
	{
	    ಆದರೆ (!ignore_ಒಳಹರಿವು && ಒಳಹರಿವು_ಲಭ್ಯ())
		ಹಿಂತಿರುಗಿ 1;
# ifdef FEAT_XCLIPBOARD
	    ಆದರೆ (rest == 0 || !do_xterm_trace())
# endif
		ನಿಲ್ಲಿಸು;
	}
    }
    ಆಗ (FALSE
# ifdef FEAT_MOUSE_GPM
	   || (gpm_process_wanted && mch_gpm_process() == 0)
# endif
# ifdef FEAT_XCLIPBOARD
	   || (!avail && rest != 0)
# endif
	  )
	;

#ಇಲ್ಲದಿದ್ದರೆ
    avail = RealWaitForChar(read_cmd_fd, msec, ಖಾಲಿ, interrupted);
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ avail;
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ VMS
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
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
RealWaitForChar(ಪೂರ್ಣಾಂಕ fd, ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ *check_for_gpm ಬಳಸದ, ಪೂರ್ಣಾಂಕ *interrupted)
{
    ಪೂರ್ಣಾಂಕ		ret;
    ಪೂರ್ಣಾಂಕ		ಫಲಿತಾಂಶ;
# ಆದರೆ defined(FEAT_XCLIPBOARD) || defined(USE_XSMP) || defined(FEAT_MZSCHEME)
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	busy = FALSE;

    // May retry getting characters after an event was handled.
#  define MAY_LOOP

#  ifdef ELAPSED_FUNC
    // Remember at what time we started, so that we know how much longer we
    // should wait after being interrupted.
    ದೀರ್ಘ	ಆರಂಭ_msec = msec;
    elapsed_T	ಆರಂಭ_tv;

    ಆದರೆ (msec > 0)
	ELAPSED_INIT(ಆರಂಭ_tv);
#  endif

    // Handle being called recursively.  This may happen for the session
    // manager stuff, it may save the file, which does a breakcheck.
    ಆದರೆ (busy)
	ಹಿಂತಿರುಗಿ 0;
# endif

# ifdef MAY_LOOP
    ಗಾಗಿ (;;)
# endif
    {
# ifdef MAY_LOOP
	ಪೂರ್ಣಾಂಕ		ಪೂರ್ಣಗೊಂಡ = TRUE; // default is to 'loop' just once
#  ifdef FEAT_MZSCHEME
	ಪೂರ್ಣಾಂಕ		mzquantum_ಬಳಸಿದ = FALSE;
#  endif
# endif
# ifdef FEAT_WAYLAND
	ಪೂರ್ಣಾಂಕ		wayland_fd = -1;
# endif
# ifndef HAVE_SELECT
			// each channel may use in, out and err
	ರಚನೆ pollfd   fds[7 + 3 * MAX_OPEN_CHANNELS + 2 * MAX_CLIENT_CHANNELS];
	ಪೂರ್ಣಾಂಕ		nfd;
#  ifdef FEAT_WAYLAND_CLIPBOARD
	ಪೂರ್ಣಾಂಕ             wayland_idx = -1;
#  endif
#  ifdef FEAT_XCLIPBOARD
	ಪೂರ್ಣಾಂಕ		xterm_idx = -1;
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಪೂರ್ಣಾಂಕ		gpm_idx = -1;
#  endif
#  ifdef USE_XSMP
	ಪೂರ್ಣಾಂಕ		xsmp_idx = -1;
#  endif
	ಪೂರ್ಣಾಂಕ		towait = (ಪೂರ್ಣಾಂಕ)msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_ಎಳೆಗಳು();
	ಆದರೆ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = (ಪೂರ್ಣಾಂಕ)p_mzq;    // don't wait longer than 'mzquantum'
	    mzquantum_ಬಳಸಿದ = TRUE;
	}
#  endif
	fds[0].fd = fd;
	fds[0].events = POLLIN;
	nfd = 1;

#  ifdef FEAT_WAYLAND
	ಆದರೆ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    wayland_idx = nfd;
	    fds[nfd].fd = wayland_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_XCLIPBOARD
	may_restore_clipboard();
	ಆದರೆ (xterm_Shell != (Widget)0)
	{
	    xterm_idx = nfd;
	    fds[nfd].fd = Connection_ಸಂಖ್ಯೆ(xterm_dpy);
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಆದರೆ (check_for_gpm != ಖಾಲಿ && gpm_flag && gpm_fd >= 0)
	{
	    gpm_idx = nfd;
	    fds[nfd].fd = gpm_fd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef USE_XSMP
	ಆದರೆ (xsmp_icefd != -1)
	{
	    xsmp_idx = nfd;
	    fds[nfd].fd = xsmp_icefd;
	    fds[nfd].events = POLLIN;
	    nfd++;
	}
#  endif
#  ifdef FEAT_ಕೆಲಸ_CHANNEL
	nfd = channel_poll_ಸಿದ್ಧತೆ(nfd, &fds, &towait);
#  endif
	ಆದರೆ (interrupted != ಖಾಲಿ)
	    *interrupted = FALSE;

	ret = poll(fds, nfd, towait);

	ಫಲಿತಾಂಶ = ret > 0 && (fds[0].revents & POLLIN);
	ಆದರೆ (ಫಲಿತಾಂಶ == 0 && interrupted != ಖಾಲಿ && ret > 0)
	    *interrupted = TRUE;

#  ifdef FEAT_MZSCHEME
	ಆದರೆ (ret == 0 && mzquantum_ಬಳಸಿದ)
	    // MzThreads scheduling is required and timeout occurred
	    ಪೂರ್ಣಗೊಂಡ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ಆದರೆ (wayland_idx >= 0)
	    wayland_poll_check(fds[wayland_idx].revents);
#  endif

#  ifdef FEAT_XCLIPBOARD
	ಆದರೆ (xterm_Shell != (Widget)0 && (fds[xterm_idx].revents & POLLIN))
	{
	    xterm_update();      // Maybe we should hand out clipboard
	    ಆದರೆ (--ret == 0 && !ಒಳಹರಿವು_ಲಭ್ಯ())
		// Try again
		ಪೂರ್ಣಗೊಂಡ = FALSE;
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಆದರೆ (gpm_idx >= 0 && (fds[gpm_idx].revents & POLLIN))
	    *check_for_gpm = 1;
#  endif
#  ifdef USE_XSMP
	ಆದರೆ (xsmp_idx >= 0 && (fds[xsmp_idx].revents & (POLLIN | POLLHUP)))
	{
	    ಆದರೆ (fds[xsmp_idx].revents & POLLIN)
	    {
		busy = TRUE;
		xsmp_handle_ವಿನಂತಿಗಳು();
		busy = FALSE;
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (fds[xsmp_idx].revents & POLLHUP)
	    {
		ಆದರೆ (p_verbose > 0)
		    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
		xsmp_close();
	    }
	    ಆದರೆ (--ret == 0)
		ಪೂರ್ಣಗೊಂಡ = FALSE;	// Try again
	}
#  endif
#  ifdef FEAT_ಕೆಲಸ_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ಆದರೆ (ret >= 0)
	    channel_poll_check(ret, &fds);
#  endif

# ಇಲ್ಲದಿದ್ದರೆ // HAVE_SELECT

	ರಚನೆ timeval  tv;
	ರಚನೆ timeval	*tvp;
	// These are static because they can take 8 Kbyte each and cause the
	// signal stack to run out with -O3.
	ಸ್ಥಿರ fd_set	rfds, wfds, efds;
	ಪೂರ್ಣಾಂಕ		maxfd;
	ದೀರ್ಘ		towait = msec;

#  ifdef FEAT_MZSCHEME
	mzvim_check_ಎಳೆಗಳು();
	ಆದರೆ (mzthreads_allowed() && p_mzq > 0 && (msec < 0 || msec > p_mzq))
	{
	    towait = p_mzq;	// don't wait longer than 'mzquantum'
	    mzquantum_ಬಳಸಿದ = TRUE;
	}
#  endif

	ಆದರೆ (towait >= 0)
	{
	    tv.tv_sec = towait / 1000;
	    tv.tv_usec = (towait % 1000) * (1000000/1000);
	    tvp = &tv;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    tvp = ಖಾಲಿ;

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
	ಆದರೆ ((wayland_fd = wayland_prepare_read()) >= 0)
	{
	    FD_SET(wayland_fd, &rfds);

	    ಆದರೆ (maxfd < wayland_fd)
		maxfd = wayland_fd;
	}
#  endif

#  ifdef FEAT_XCLIPBOARD
	may_restore_x11_clipboard();
	ಆದರೆ (xterm_Shell != (Widget)0)
	{
	    FD_SET(Connection_ಸಂಖ್ಯೆ(xterm_dpy), &rfds);
	    ಆದರೆ (maxfd < Connection_ಸಂಖ್ಯೆ(xterm_dpy))
		maxfd = Connection_ಸಂಖ್ಯೆ(xterm_dpy);

	    // An event may have already been read but not handled.  In
	    // particularly, XFlush may cause this.
	    xterm_update();
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಆದರೆ (check_for_gpm != ಖಾಲಿ && gpm_flag && gpm_fd >= 0)
	{
	    FD_SET(gpm_fd, &rfds);
	    FD_SET(gpm_fd, &efds);
	    ಆದರೆ (maxfd < gpm_fd)
		maxfd = gpm_fd;
	}
#  endif
#  ifdef USE_XSMP
	ಆದರೆ (xsmp_icefd != -1)
	{
	    FD_SET(xsmp_icefd, &rfds);
	    FD_SET(xsmp_icefd, &efds);
	    ಆದರೆ (maxfd < xsmp_icefd)
		maxfd = xsmp_icefd;
	}
#  endif
#  ifdef FEAT_ಕೆಲಸ_CHANNEL
	maxfd = channel_select_ಸಿದ್ಧತೆ(maxfd, &rfds, &wfds, &tv, &tvp);
#  endif
	ಆದರೆ (interrupted != ಖಾಲಿ)
	    *interrupted = FALSE;

	ret = select(maxfd + 1, SELECT_TYPE_ARG234 &rfds,
		      SELECT_TYPE_ARG234 &wfds, SELECT_TYPE_ARG234 &efds, tvp);
	ಫಲಿತಾಂಶ = ret > 0 && FD_ISSET(fd, &rfds);
	ಆದರೆ (ಫಲಿತಾಂಶ)
	    --ret;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (interrupted != ಖಾಲಿ && ret > 0)
	    *interrupted = TRUE;

#  ifdef EINTR
	ಆದರೆ (ret == -1 && errno == EINTR)
	{
	    // Check whether the EINTR is caused by SIGTSTP
	    ಆದರೆ (got_tstp && !ಒಳ_mch_suspend)
	    {
		exarg_T ea;

		ea.forceit = TRUE;
		ex_stop(&ea);
		got_tstp = FALSE;
	    }

	    // Check whether window has been resized, EINTR may be caused by
	    // SIGWINCH.
	    ಆದರೆ (do_resize)
	    {
#   ifdef FEAT_EVAL
		ch_log(ಖಾಲಿ, "calling handle_resize() in RealWaitForChar()");
#   endif
		handle_resize();
	    }

	    // Interrupted by a signal, need to try again.  We ignore msec
	    // here, because we do want to check even after a timeout if
	    // characters are available.  Needed for reading output of an
	    // external command after the process has finished.
	    ಹೋಗು select_eintr;
	}
#  endif
#  ifdef __TANDEM
	ಆದರೆ (ret == -1 && errno == ENOTSUP)
	{
	    FD_ZERO(&rfds);
	    FD_ZERO(&efds);
	    ret = 0;
	}
#  endif
#  ifdef FEAT_MZSCHEME
	ಆದರೆ (ret == 0 && mzquantum_ಬಳಸಿದ)
	    // loop if MzThreads must be scheduled and timeout occurred
	    ಪೂರ್ಣಗೊಂಡ = FALSE;
#  endif

#  ifdef FEAT_WAYLAND
	ಆದರೆ (wayland_fd != -1)
	    wayland_select_check(ret > 0 && FD_ISSET(wayland_fd, &rfds));
#  endif

#  ifdef FEAT_XCLIPBOARD
	ಆದರೆ (ret > 0 && xterm_Shell != (Widget)0
		&& FD_ISSET(Connection_ಸಂಖ್ಯೆ(xterm_dpy), &rfds))
	{
	    xterm_update();	      // Maybe we should hand out clipboard
	    // continue looping when we only got the X event and the input
	    // buffer is empty
	    ಆದರೆ (--ret == 0 && !ಒಳಹರಿವು_ಲಭ್ಯ())
	    {
		// Try again
		ಪೂರ್ಣಗೊಂಡ = FALSE;
	    }
	}
#  endif
#  ifdef FEAT_MOUSE_GPM
	ಆದರೆ (ret > 0 && check_for_gpm != ಖಾಲಿ && gpm_flag && gpm_fd >= 0)
	{
	    ಆದರೆ (FD_ISSET(gpm_fd, &efds))
		gpm_close();
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FD_ISSET(gpm_fd, &rfds))
		*check_for_gpm = 1;
	}
#  endif
#  ifdef USE_XSMP
	ಆದರೆ (ret > 0 && xsmp_icefd != -1)
	{
	    ಆದರೆ (FD_ISSET(xsmp_icefd, &efds))
	    {
		ಆದರೆ (p_verbose > 0)
		    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
		xsmp_close();
		ಆದರೆ (--ret == 0)
		    ಪೂರ್ಣಗೊಂಡ = FALSE;   // keep going if event was only one
	    }
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (FD_ISSET(xsmp_icefd, &rfds))
	    {
		busy = TRUE;
		xsmp_handle_ವಿನಂತಿಗಳು();
		busy = FALSE;
		ಆದರೆ (--ret == 0)
		    ಪೂರ್ಣಗೊಂಡ = FALSE;   // keep going if event was only one
	    }
	}
#  endif
#  ifdef FEAT_ಕೆಲಸ_CHANNEL
	// also call when ret == 0, we may be polling a keep-open channel
	ಆದರೆ (ret >= 0)
	    (ಶೂನ್ಯ)channel_select_check(ret, &rfds, &wfds);
#  endif

# endif // HAVE_SELECT

# ifdef MAY_LOOP
	ಆದರೆ (ಪೂರ್ಣಗೊಂಡ || msec == 0)
	    ನಿಲ್ಲಿಸು;

#  ಆದರೆ defined(FEAT_CLIENTSERVER) && defined(FEAT_X11)
	ಆದರೆ (clientserver_ವಿಧಾನ == CLIENTSERVER_ವಿಧಾನ_X11 && server_ಕಾಯುವಿಕೆ())
	    ನಿಲ್ಲಿಸು;
#  endif

	// We're going to loop around again, find out for how long
	ಆದರೆ (msec > 0)
	{
#  ifdef ELAPSED_FUNC
	    // Compute remaining wait time.
	    msec = ಆರಂಭ_msec - ELAPSED_FUNC(ಆರಂಭ_tv);
#  ಇಲ್ಲದಿದ್ದರೆ
	    // Guess we got interrupted halfway.
	    msec = msec / 2;
#  endif
	    ಆದರೆ (msec <= 0)
		ನಿಲ್ಲಿಸು;	// waited long enough
	}
# endif
    }

    ಹಿಂತಿರುಗಿ ಫಲಿತಾಂಶ;
}

/*
 * Expand a path into all matching files and/or directories.  Handles "*",
 * "?", "[a-z]", "**", etc.
 * "path" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 */
    ಪೂರ್ಣಾಂಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*ಮಾರ್ಗ,
    ಪೂರ್ಣಾಂಕ		flags)		// EW_* flags
{
    ಹಿಂತಿರುಗಿ unix_expandpath(gap, ಮಾರ್ಗ, 0, flags, FALSE);
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

    ಪೂರ್ಣಾಂಕ
mch_expand_wildcards(
    ಪೂರ್ಣಾಂಕ		   ಅಂಕೆ_pat,
    char_u	 **pat,
    ಪೂರ್ಣಾಂಕ		  *ಅಂಕೆ_file,
    char_u	***file,
    ಪೂರ್ಣಾಂಕ		   flags)	// EW_* flags
{
    ಪೂರ್ಣಾಂಕ		i;
    size_t	len;
    ದೀರ್ಘ	llen;
    char_u	*p;
    ಪೂರ್ಣಾಂಕ		dir;

    /*
     * This is the non-OS/2 implementation (really Unix).
     */
    ಪೂರ್ಣಾಂಕ		j;
    char_u	*tempname;
    size_t	tempnamelen;
    char_u	*command;
    size_t	commandlen;
    FILE	*fd;
    char_u	*ಬಫರ್;
# define STYLE_ECHO	0	// use "echo", the default
# define STYLE_GLOB	1	// use "glob", for csh
# define STYLE_VIMGLOB	2	// use "vimglob", for Posix sh
# define STYLE_PRINT	3	// use "print -N", for zsh
# define STYLE_BT	4	// `cmd` expansion, execute the pattern directly
# define STYLE_GLOBSTAR	5	// use extended shell glob for bash (this uses extended
				// globbing functionality using globstar, needs bash > 4)
    ಪೂರ್ಣಾಂಕ		shell_style = STYLE_ECHO;
    ಪೂರ್ಣಾಂಕ		check_spaces;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	did_ಕಂಡುಹಿಡಿ_nul = FALSE;
    ಪೂರ್ಣಾಂಕ		ampersand = FALSE;
				// vimglob() function to define for Posix shell
    ಸ್ಥಿರ string_T sh_vimglob_func = STR_LITERAL_INIT("vimglob() { while [ $# -ge 1 ]; do echo \"$1\"; shift; done }; vimglob >");
				// vimglob() function with globstar setting enabled, only for bash >= 4.X
    ಸ್ಥಿರ string_T sh_globstar_opt = STR_LITERAL_INIT("[[ ${BASH_VERSINFO[0]} -ge 4 ]] && shopt -s globstar; ");


    *ಅಂಕೆ_file = 0;	// default: no files found
    *file = ಖಾಲಿ;

    /*
     * If there are no wildcards, just copy the names to allocated memory.
     * Saves a lot of time, because we don't have to start a new shell.
     */
    ಆದರೆ (!have_wildcard(ಅಂಕೆ_pat, pat))
	ಹಿಂತಿರುಗಿ save_patterns(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file);

# ifdef HAVE_SANDBOX
    // Don't allow any shell command in the sandbox.
    ಆದರೆ (sandbox != 0 && check_secure())
	ಹಿಂತಿರುಗಿ FAIL;
# endif

    /*
     * Don't allow the use of backticks in secure and restricted mode.
     */
    ಆದರೆ (secure || restricted)
	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
	    ಆದರೆ (vim_strchr(pat[i], '`') != ಖಾಲಿ
		    && (check_restricted() || check_secure()))
		ಹಿಂತಿರುಗಿ FAIL;

    /*
     * get a name for the temp file
     */
    ಆದರೆ ((tempname = vim_tempname('o', FALSE)) == ಖಾಲಿ)
    {
	emsg(_(e_cant_get_ತಾತ್ಕಾಲಿಕ_file_name));
	ಹಿಂತಿರುಗಿ FAIL;
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
    ಆದರೆ (ಅಂಕೆ_pat == 1 && *pat[0] == '`'
	    && (len = STRLEN(pat[0])) > 2
	    && *(pat[0] + len - 1) == '`')
	shell_style = STYLE_BT;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ ((len = STRLEN(p_sh)) >= 3)
    {
	ಆದರೆ (STRCMP(p_sh + len - 3, "csh") == 0)
	    shell_style = STYLE_GLOB;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (STRCMP(p_sh + len - 3, "zsh") == 0)
	    shell_style = STYLE_PRINT;
    }
    ಆದರೆ (shell_style == STYLE_ECHO)
    {
	ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "bash") != ಖಾಲಿ)
	    shell_style = STYLE_GLOBSTAR;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)gettail(p_sh), "sh") != ಖಾಲಿ)
	    shell_style = STYLE_VIMGLOB;
    }

    // Compute the length of the command.  We need 2 extra bytes: for the
    // optional '&' and for the NUL.
    // Worst case: "unset nonomatch; print -N >" plus two is 29
    tempnamelen = STRLEN(tempname);
    len = tempnamelen + 29;
    ಆದರೆ (shell_style == STYLE_VIMGLOB)
	len += sh_vimglob_func.ಉದ್ದಳತೆ;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_GLOBSTAR)
	len += sh_vimglob_func.ಉದ್ದಳತೆ + sh_globstar_opt.ಉದ್ದಳತೆ;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
    {
	// Count the length of the patterns in the same way as they are put in
	// "command" below.
# ifdef USE_SYSTEM
	len += STRLEN(pat[i]) + 3;	// add space and two quotes
# ಇಲ್ಲದಿದ್ದರೆ
	++len;				// add space
	ಗಾಗಿ (j = 0; pat[i][j] != NUL; ++j)
	{
	    ಆದರೆ (vim_strchr(SHELL_SPECIAL, pat[i][j]) != ಖಾಲಿ)
		++len;		// may add a backslash
	    ++len;
	}
# endif
    }
    command = alloc(len);
    ಆದರೆ (command == ಖಾಲಿ)
    {
	// out of memory
	vim_free(tempname);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    /*
     * Build the shell command:
     * - Set $nonomatch depending on EW_NOTFOUND (hopefully the shell
     *	 recognizes this).
     * - Add the shell command to print the expanded names.
     * - Add the temp file name.
     * - Add the file name patterns.
     */
    ಆದರೆ (shell_style == STYLE_BT)
    {
	// change `command; command& ` to (command; command )
	commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "(%s)>%s", pat[0] + 1, tempname);   // +1 to exclude first backtick

	p = (char_u *)ಅಕ್ಷರಸರಣಿ_ಹುಡುಕು((ಅಕ್ಷರ *)command, ")>");
	ಆದರೆ (p == ಖಾಲಿ)
	{
	    ;					    // can't happen ;-)
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    --p;
	    ಆಗ (p > command && VIM_ISWHITE(*p))
		--p;
	    ಆದರೆ (*p == '`')			    // remove backtick
		*p = ' ';

	    --p;
	    ಆಗ (p > command && VIM_ISWHITE(*p))
		--p;
	    ಆದರೆ (*p == '&')			    // remove ampersand
	    {
		ampersand = TRUE;
		*p = ' ';
	    }
	}
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (shell_style == STYLE_GLOB)
	{
	    // Assume the nonomatch option is valid only for csh like shells,
	    // otherwise, this may set the positional parameters for the shell,
	    // e.g. "$*".
	    commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "%sset nonomatch; glob >%s",
				(flags & EW_NOTFOUND) ? "" : "un", tempname);
	}
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_PRINT)
	    commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "print -N >%s", tempname);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_VIMGLOB)
	    commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "%s%s", sh_vimglob_func.string, tempname);
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_GLOBSTAR)
	    commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "%s%s%s", sh_globstar_opt.string,
				sh_vimglob_func.string, tempname);
	ಇಲ್ಲದಿದ್ದರೆ
	    commandlen = vim_snprintf((ಅಕ್ಷರ *)command, len, "echo >%s", tempname);

	ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; ++i)
	{
	    // When using system() always add extra quotes, because the shell
	    // is started twice.  Otherwise put a backslash before special
	    // characters, except inside ``.
# ifdef USE_SYSTEM
	    commandlen += vim_snprintf((ಅಕ್ಷರ *)command + commandlen, len, " \"%s\"", pat[i]);
# ಇಲ್ಲದಿದ್ದರೆ
	    ಪೂರ್ಣಾಂಕ intick = FALSE;

	    p = command + commandlen;
	    *p++ = ' ';
	    ಗಾಗಿ (j = 0; pat[i][j] != NUL; ++j)
	    {
		ಆದರೆ (pat[i][j] == '`')
		    intick = !intick;
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (pat[i][j] == '\\' && pat[i][j + 1] != NUL)
		{
		    // Remove a backslash, take char literally.  But keep
		    // backslash inside backticks, before a special character
		    // and before a backtick.
		    ಆದರೆ (intick
			  || vim_strchr(SHELL_SPECIAL, pat[i][j + 1]) != ಖಾಲಿ
			  || pat[i][j + 1] == '`')
			*p++ = '\\';
		    ++j;
		}
		ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!intick
			 && ((flags & EW_KEEPDOLLAR) == 0 || pat[i][j] != '$')
			      && vim_strchr(SHELL_SPECIAL, pat[i][j]) != ಖಾಲಿ)
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
    ಆದರೆ (flags & EW_SILENT)
	show_shell_mess = FALSE;
    ಆದರೆ (ampersand)
	STRCPY(command + commandlen, "&");		// put the '&' after the redirection

    /*
     * Using zsh -G: If a pattern has no matches, it is just deleted from
     * the argument list, otherwise zsh gives an error message and doesn't
     * expand any other pattern.
     */
    ಆದರೆ (shell_style == STYLE_PRINT)
	extra_shell_arg = (char_u *)"-G";   // Use zsh NULL_GLOB option

    /*
     * If we use -f then shell variables set in .cshrc won't get expanded.
     * vi can do it, so we will too, but it is only necessary if there is a "$"
     * in one of the patterns, otherwise we can still use the fast option.
     */
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_GLOB && !have_dollars(ಅಂಕೆ_pat, pat))
	extra_shell_arg = (char_u *)"-f";	// Use csh fast option

    /*
     * execute the shell command
     */
    i = call_shell(command, SHELL_EXPAND | SHELL_SILENT);

    // When running in the background, give it some time to create the temp
    // file, but don't wait for it to finish.
    ಆದರೆ (ampersand)
	mch_delay(10L, MCH_DELAY_IGNOREINPUT);

    extra_shell_arg = ಖಾಲಿ;		// cleanup
    show_shell_mess = TRUE;
    vim_free(command);

    ಆದರೆ (i != 0)				// mch_call_shell() failed
    {
	mch_remove(tempname);
	vim_free(tempname);
	/*
	 * With interactive completion, the error message is not printed.
	 * However with USE_SYSTEM, I don't know how to turn off error messages
	 * from the shell, so screen may still get messed up -- webb.
	 */
# ifndef USE_SYSTEM
	ಆದರೆ (!(flags & EW_SILENT))
# endif
	{
	    redraw_later_clear();	// probably messed up screen
	    ಸಂದೇಶ_putchar('\n');		// clear bottom line quickly
	    cmdline_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲುಗಳು - 1;	// continue on last line
# ifdef USE_SYSTEM
	    ಆದರೆ (!(flags & EW_SILENT))
# endif
	    {
		ಸಂದೇಶ(_(e_cannot_expand_wildcards));
		ಸಂದೇಶ_ಆರಂಭ();		// don't overwrite this message
	    }
	}
	// If a `cmd` expansion failed, don't list `cmd` as a match, even when
	// EW_NOTFOUND is given
	ಆದರೆ (shell_style == STYLE_BT)
	    ಹಿಂತಿರುಗಿ FAIL;
	ಹೋಗು notfound;
    }

    /*
     * read the names from the file into memory
     */
    fd = fopen((ಅಕ್ಷರ *)tempname, READBIN);
    ಆದರೆ (fd == ಖಾಲಿ)
    {
	// Something went wrong, perhaps a file name with a special char.
	ಆದರೆ (!(flags & EW_SILENT))
	{
	    ಸಂದೇಶ(_(e_cannot_expand_wildcards));
	    ಸಂದೇಶ_ಆರಂಭ();		// don't overwrite this message
	}
	vim_free(tempname);
	ಹೋಗು notfound;
    }
    fseek(fd, 0L, SEEK_END);
    llen = ftell(fd);			// get size of temp file
    fseek(fd, 0L, SEEK_SET);
    ಆದರೆ (llen < 0)
	// just in case ftell() would fail
	ಬಫರ್ = ಖಾಲಿ;
    ಇಲ್ಲದಿದ್ದರೆ
	ಬಫರ್ = alloc(llen + 1);
    ಆದರೆ (ಬಫರ್ == ಖಾಲಿ)
    {
	// out of memory
	mch_remove(tempname);
	vim_free(tempname);
	fclose(fd);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    len = llen;
    i = fread((ಅಕ್ಷರ *)ಬಫರ್, 1, len, fd);
    fclose(fd);
    mch_remove(tempname);
    ಆದರೆ (i != (ಪೂರ್ಣಾಂಕ)len)
    {
	// unexpected read error
	semsg(_(e_cant_read_file_str), tempname);
	vim_free(tempname);
	vim_free(ಬಫರ್);
	ಹಿಂತಿರುಗಿ FAIL;
    }
    vim_free(tempname);

# ifdef __CYGWIN__
    // Translate <CR><NL> into <NL>.  Caution, buffer may contain NUL.
    p = ಬಫರ್;
    ಗಾಗಿ (i = 0; i < (ಪೂರ್ಣಾಂಕ)len; ++i)
	ಆದರೆ (!(ಬಫರ್[i] == CAR && ಬಫರ್[i + 1] == NL))
	    *p++ = ಬಫರ್[i];
    len = p - ಬಫರ್;
# endif


    // file names are separated with Space
    ಆದರೆ (shell_style == STYLE_ECHO)
    {
	ಬಫರ್[len] = '\n';		// make sure the buffer ends in NL
	p = ಬಫರ್;
	ಗಾಗಿ (i = 0; *p != '\n'; ++i)	// count number of entries
	{
	    ಆಗ (*p != ' ' && *p != '\n')
		++p;
	    p = skipwhite(p);		// skip to next entry
	}
    }
    // file names are separated with NL
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (shell_style == STYLE_BT ||
	    shell_style == STYLE_VIMGLOB ||
	    shell_style == STYLE_GLOBSTAR)
    {
	ಬಫರ್[len] = NUL;		// make sure the buffer ends in NUL
	p = ಬಫರ್;
	ಗಾಗಿ (i = 0; *p != NUL; ++i)	// count number of entries
	{
	    ಆಗ (*p != '\n' && *p != NUL)
		++p;
	    ಆದರೆ (*p != NUL)
		++p;
	    p = skipwhite(p);		// skip leading white space
	}
    }
    // file names are separated with NUL
    ಇಲ್ಲದಿದ್ದರೆ
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
	ಆದರೆ (shell_style == STYLE_PRINT && !did_ಕಂಡುಹಿಡಿ_nul)
	{
	    // If there is a NUL, set did_find_nul, else set check_spaces
	    ಬಫರ್[len] = NUL;
	    ಆದರೆ (len && (ಪೂರ್ಣಾಂಕ)STRLEN(ಬಫರ್) < (ಪೂರ್ಣಾಂಕ)len)
		did_ಕಂಡುಹಿಡಿ_nul = TRUE;
	    ಇಲ್ಲದಿದ್ದರೆ
		check_spaces = TRUE;
	}

	/*
	 * Make sure the buffer ends with a NUL.  For STYLE_PRINT there
	 * already is one, for STYLE_GLOB it needs to be added.
	 */
	ಆದರೆ (len && ಬಫರ್[len - 1] == NUL)
	    --len;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಬಫರ್[len] = NUL;
	i = 0;
	ಗಾಗಿ (p = ಬಫರ್; p < ಬಫರ್ + len; ++p)
	    ಆದರೆ (*p == NUL || (*p == ' ' && check_spaces))   // count entry
	    {
		++i;
		*p = NUL;
	    }
	ಆದರೆ (len)
	    ++i;			// count last entry
    }
    ಆದರೆ (i == 0)
    {
	/*
	 * Can happen when using /bin/sh and typing ":e $NO_SUCH_VAR^I".
	 * /bin/sh will happily expand it to nothing rather than returning an
	 * error; and hey, it's good to check anyway -- webb.
	 */
	vim_free(ಬಫರ್);
	ಹೋಗು notfound;
    }
    *ಅಂಕೆ_file = i;
    *file = ALLOC_MULT(char_u *, i);
    ಆದರೆ (*file == ಖಾಲಿ)
    {
	// out of memory
	vim_free(ಬಫರ್);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    /*
     * Isolate the individual file names.
     */
    p = ಬಫರ್;
    ಗಾಗಿ (i = 0; i < *ಅಂಕೆ_file; ++i)
    {
	(*file)[i] = p;
	// Space or NL separates
	ಆದರೆ (shell_style == STYLE_ECHO || shell_style == STYLE_BT
		|| shell_style == STYLE_VIMGLOB || shell_style == STYLE_GLOBSTAR)
	{
	    ಆಗ (!(shell_style == STYLE_ECHO && *p == ' ')
						   && *p != '\n' && *p != NUL)
		++p;
	    ಆದರೆ (p == ಬಫರ್ + len)		// last entry
		*p = NUL;
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
		*p++ = NUL;
		p = skipwhite(p);		// skip to next entry
	    }
	}
	ಇಲ್ಲದಿದ್ದರೆ		// NUL separates
	{
	    ಆಗ (*p && p < ಬಫರ್ + len)	// skip entry
		++p;
	    ++p;				// skip NUL
	}
    }

    /*
     * Move the file names to allocated memory.
     */
    ಗಾಗಿ (j = 0, i = 0; i < *ಅಂಕೆ_file; ++i)
    {
	// Require the files to exist.	Helps when using /bin/sh
	ಆದರೆ (!(flags & EW_NOTFOUND) && mch_getperm((*file)[i]) < 0)
	    ಮುಂದುವರಿ;

	// check if this entry should be included
	dir = (mch_isdir((*file)[i]));
	ಆದರೆ ((dir && !(flags & EW_DIR)) || (!dir && !(flags & EW_FILE)))
	    ಮುಂದುವರಿ;

	// Skip files that are not executable if we check for that.
	ಆದರೆ (!dir && (flags & EW_EXEC)
		    && !mch_can_exe((*file)[i], ಖಾಲಿ, !(flags & EW_SHELLCMD)))
	    ಮುಂದುವರಿ;

	p = alloc(STRLEN((*file)[i]) + 1 + dir);
	ಆದರೆ (p)
	{
	    STRCPY(p, (*file)[i]);
	    ಆದರೆ (dir)
		ಕೂಡಿಸು_pathsep(p);	    // add '/' to a directory name
	    (*file)[j++] = p;
	}
    }
    vim_free(ಬಫರ್);
    *ಅಂಕೆ_file = j;

    ಆದರೆ (*ಅಂಕೆ_file == 0)	    // rejected all entries
    {
	VIM_CLEAR(*file);
	ಹೋಗು notfound;
    }

    ಹಿಂತಿರುಗಿ OK;

notfound:
    ಆದರೆ (flags & EW_NOTFOUND)
	ಹಿಂತಿರುಗಿ save_patterns(ಅಂಕೆ_pat, pat, ಅಂಕೆ_file, file);
    ಹಿಂತಿರುಗಿ FAIL;
}

#ಆದರೆ_ಅಂತ್ಯ // VMS

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
save_patterns(
    ಪೂರ್ಣಾಂಕ		ಅಂಕೆ_pat,
    char_u	**pat,
    ಪೂರ್ಣಾಂಕ		*ಅಂಕೆ_file,
    char_u	***file)
{
    ಪೂರ್ಣಾಂಕ		i;
    char_u	*s;

    *file = ALLOC_MULT(char_u *, ಅಂಕೆ_pat);
    ಆದರೆ (*file == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FAIL;
    ಗಾಗಿ (i = 0; i < ಅಂಕೆ_pat; i++)
    {
	s = vim_strsave(pat[i]);
	ಆದರೆ (s != ಖಾಲಿ)
	    // Be compatible with expand_filename(): halve the number of
	    // backslashes.
	    backslash_halve(s);
	(*file)[i] = s;
    }
    *ಅಂಕೆ_file = ಅಂಕೆ_pat;
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Return TRUE if the string "p" contains a wildcard that mch_expandpath() can
 * expand.
 */
    ಪೂರ್ಣಾಂಕ
mch_has_exp_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (vim_strchr((char_u *)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
				    "*?%"
#ಇಲ್ಲದಿದ್ದರೆ
				    "*?[{'"
#ಆದರೆ_ಅಂತ್ಯ
						, *p) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

/*
 * Return TRUE if the string "p" contains a wildcard.
 * Don't recognize '~' at the end as a wildcard.
 */
    ಪೂರ್ಣಾಂಕ
mch_has_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (vim_strchr((char_u *)
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ VMS
				    "*?%$"
#ಇಲ್ಲದಿದ್ದರೆ
				    "*?[{`'$"
#ಆದರೆ_ಅಂತ್ಯ
						, *p) != ಖಾಲಿ
		|| (*p == '~' && p[1] != NUL))
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
have_wildcard(ಪೂರ್ಣಾಂಕ ಅಂಕೆ, char_u **file)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ; i++)
	ಆದರೆ (mch_has_wildcard(file[i]))
	    ಹಿಂತಿರುಗಿ 1;
    ಹಿಂತಿರುಗಿ 0;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
have_dollars(ಪೂರ್ಣಾಂಕ ಅಂಕೆ, char_u **file)
{
    ಪೂರ್ಣಾಂಕ	    i;

    ಗಾಗಿ (i = 0; i < ಅಂಕೆ; i++)
	ಆದರೆ (vim_strchr(file[i], '$') != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    ಹಿಂತಿರುಗಿ FALSE;
}

#ಆದರೆ !defined(HAVE_RENAME)
/*
 * Scaled-down version of rename(), which is missing in Xenix.
 * This version can only move regular files and will fail if the
 * destination exists.
 */
    ಪೂರ್ಣಾಂಕ
mch_rename(ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *src, ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *dest)
{
    ರಚನೆ stat	    st;

    ಆದರೆ (stat(dest, &st) >= 0)	    // fail if destination exists
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (link(src, dest) != 0)	    // link file to new name
	ಹಿಂತಿರುಗಿ -1;
    ಆದರೆ (mch_remove(src) == 0)	    // delete link to old name
	ಹಿಂತಿರುಗಿ 0;
    ಹಿಂತಿರುಗಿ -1;
}
#ಆದರೆ_ಅಂತ್ಯ // !HAVE_RENAME

#ಆದರೆ defined(FEAT_MOUSE_GPM)
# ಆದರೆ defined(DYNAMIC_GPM)
/*
 * Initialize Gpm's symbols for dynamic linking.
 * Must be called only if libgpm_hinst is NULL.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
ತುಂಬು_libgpm(ಶೂನ್ಯ)
{
    libgpm_hinst = dlopen("libgpm.so", RTLD_LAZY|RTLD_GLOBAL);

    ಆದರೆ (libgpm_hinst == ಖಾಲಿ)
    {
	ಆದರೆ (p_verbose > 0)
	    smsg_attr(HL_ATTR(HLF_W),
			       _("Could not load gpm library: %s"), dlerror());
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಆದರೆ (
	    (dll_Gpm_Open     = dlsym(libgpm_hinst, "Gpm_Open"))     == ಖಾಲಿ
	||  (dll_Gpm_Close    = dlsym(libgpm_hinst, "Gpm_Close"))    == ಖಾಲಿ
	||  (dll_Gpm_GetEvent = dlsym(libgpm_hinst, "Gpm_GetEvent")) == ಖಾಲಿ
	||  (dll_gpm_flag     = dlsym(libgpm_hinst, "gpm_flag"))     == ಖಾಲಿ
	||  (dll_gpm_fd       = dlsym(libgpm_hinst, "gpm_fd"))       == ಖಾಲಿ
      )
    {
	semsg(_(e_could_not_ತುಂಬು_library_str_str), "gpm", dlerror());
	dlclose(libgpm_hinst);
	libgpm_hinst = ಖಾಲಿ;
	dll_gpm_flag = ಖಾಲಿ;
	dll_gpm_fd   = ಖಾಲಿ;
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಹಿಂತಿರುಗಿ OK;
}

    ಪೂರ್ಣಾಂಕ
gpm_ಲಭ್ಯ(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ libgpm_hinst != ಖಾಲಿ || ತುಂಬು_libgpm() == OK;
}
# endif // DYNAMIC_GPM

/*
 * Initializes connection with gpm (if it isn't already opened)
 * Return 1 if succeeded (or connection already opened), 0 if failed
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
gpm_open(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ Gpm_Connect gpm_connect; // Must it be kept till closing ?

# ifdef DYNAMIC_GPM
    ಆದರೆ (!gpm_ಲಭ್ಯ())
	ಹಿಂತಿರುಗಿ 0;
# endif

    ಆದರೆ (gpm_flag)
	ಹಿಂತಿರುಗಿ 1; // already open

    gpm_connect.event_ಮುಖವಾಡ_2 = (GPM_ಮೇಲೆ | GPM_DRAG | GPM_ಕೆಳಕ್ಕೆ);
    gpm_connect.default_ಮುಖವಾಡ = ~GPM_HARD;
    // Default handling for mouse move
    gpm_connect.minMod = 0; // Handle any modifier keys
    gpm_connect.maxMod = 0xffff;
    ಆದರೆ (Gpm_Open(&gpm_connect, 0) > 0)
    {
	// gpm library tries to handling TSTP causes
	// problems. Anyways, we close connection to Gpm whenever
	// we are going to suspend or starting an external process
	// so we shouldn't  have problem with this
# ifdef SIGTSTP
	mch_signal(SIGTSTP, restricted ? SIG_IGN : sig_tstp);
# endif
	ಹಿಂತಿರುಗಿ 1; // succeed
    }
    ಆದರೆ (gpm_fd == -2)
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
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Returns TRUE if the GPM mouse is enabled.
 */
    ಪೂರ್ಣಾಂಕ
gpm_enabled(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ gpm_flag && gpm_fd >= 0;
}

/*
 * Closes connection to gpm
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
gpm_close(ಶೂನ್ಯ)
{
    ಆದರೆ (gpm_enabled())
	Gpm_Close();
}

/*
 * Reads gpm event and adds special keys to input buf. Returns length of
 * generated key sequence.
 * This function is styled after gui_send_mouse_event().
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
mch_gpm_process(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ			button;
    ಸ್ಥಿರ Gpm_Event	gpm_event;
    char_u		string[6];
    int_u		vim_modifiers;
    ಪೂರ್ಣಾಂಕ			ಅಡ್ಡಸಾಲು,ಲಂಬಸಾಲು;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ	buttons_ಮುಖವಾಡ;
    ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ	gpm_modifiers;
    ಸ್ಥಿರ ಚಿಹ್ನೆರಹಿತ ಅಕ್ಷರ old_buttons = 0;

    Gpm_GetEvent(&gpm_event);

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ 0;
# endif

    ಅಡ್ಡಸಾಲು = gpm_event.y - 1;
    ಲಂಬಸಾಲು = gpm_event.x - 1;

    string[0] = ESC; // Our termcode
    string[1] = 'M';
    string[2] = 'G';
    ಆಯ್ಕೆ (GPM_BARE_EVENTS(gpm_event.type))
    {
	ಸ್ಥಿತಿ GPM_DRAG:
	    string[3] = MOUSE_DRAG;
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GPM_ಕೆಳಕ್ಕೆ:
	    buttons_ಮುಖವಾಡ = gpm_event.buttons & ~old_buttons;
	    old_buttons = gpm_event.buttons;
	    ಆಯ್ಕೆ (buttons_ಮುಖವಾಡ)
	    {
		ಸ್ಥಿತಿ GPM_B_ಎಡ:
		    button = MOUSE_ಎಡ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ GPM_B_ಮಧ್ಯ:
		    button = MOUSE_ಮಧ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ GPM_B_ಬಲ:
		    button = MOUSE_ಬಲ;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಹಿಂತಿರುಗಿ 0;
		    // Don't know what to do. Can more than one button be
		    // reported in one event?
	    }
	    string[3] = (char_u)(button | 0x20);
	    ಗಣ_ಅಂಕೆ_MOUSE_CLICKS(string[3], gpm_event.clicks + 1);
	    ನಿಲ್ಲಿಸು;
	ಸ್ಥಿತಿ GPM_ಮೇಲೆ:
	    string[3] = MOUSE_RELEASE;
	    old_buttons &= ~gpm_event.buttons;
	    ನಿಲ್ಲಿಸು;
	ಪೂರ್ವನಿಯೋಜಿತ:
	    ಹಿಂತಿರುಗಿ 0;
    }
    // This code is based on gui_x11_mouse_cb in gui_x11.c
    gpm_modifiers = gpm_event.modifiers;
    vim_modifiers = 0x0;
    // I ignore capslock stats. Aren't we all just hate capslock mixing with
    // Vim commands ? Besides, gpm_event.modifiers is unsigned char, and
    // K_CAPSSHIFT is defined 8, so it probably isn't even reported
    ಆದರೆ (gpm_modifiers & ((1 << KG_ಸರಿಸು) | (1 << KG_SHIFTR) | (1 << KG_SHIFTL)))
	vim_modifiers |= MOUSE_ಸರಿಸು;

    ಆದರೆ (gpm_modifiers & ((1 << KG_CTRL) | (1 << KG_CTRLR) | (1 << KG_CTRLL)))
	vim_modifiers |= MOUSE_CTRL;
    ಆದರೆ (gpm_modifiers & ((1 << KG_ALT) | (1 << KG_ALTGR)))
	vim_modifiers |= MOUSE_ALT;
    string[3] |= vim_modifiers;
    string[4] = (char_u)(ಲಂಬಸಾಲು + ' ' + 1);
    string[5] = (char_u)(ಅಡ್ಡಸಾಲು + ' ' + 1);
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 6);
    ಹಿಂತಿರುಗಿ 6;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_MOUSE_GPM

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_SYSMOUSE
/*
 * Initialize connection with sysmouse.
 * Let virtual console inform us with SIGUSR2 for pending sysmouse
 * output, any sysmouse output than will be processed via sig_sysmouse().
 * Return OK if succeeded, FAIL if failed.
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sysmouse_open(ಶೂನ್ಯ)
{
    ರಚನೆ mouse_info   mouse;

    mouse.operation = MOUSE_ಬಹುಲಕ;
    mouse.u.ಬಹುಲಕ.ಬಹುಲಕ = 0;
    mouse.u.ಬಹುಲಕ.ಸಂಕೇತ = SIGUSR2;
    ಆದರೆ (ioctl(1, CONS_MOUSECTL, &mouse) == -1)
	ಹಿಂತಿರುಗಿ FAIL;

    mch_signal(SIGUSR2, sig_sysmouse);
    mouse.operation = MOUSE_SHOW;
    ioctl(1, CONS_MOUSECTL, &mouse);
    ಹಿಂತಿರುಗಿ OK;
}

/*
 * Stop processing SIGUSR2 signals, and also make sure that
 * virtual console do not send us any sysmouse related signal.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sysmouse_close(ಶೂನ್ಯ)
{
    ರಚನೆ mouse_info	mouse;

    mch_signal(SIGUSR2, restricted ? SIG_IGN : SIG_DFL);
    mouse.operation = MOUSE_ಬಹುಲಕ;
    mouse.u.ಬಹುಲಕ.ಬಹುಲಕ = 0;
    mouse.u.ಬಹುಲಕ.ಸಂಕೇತ = 0;
    ioctl(1, CONS_MOUSECTL, &mouse);
}

/*
 * Gets info from sysmouse and adds special keys to input buf.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
sig_sysmouse SIGDEFARG(sigarg)
{
    ರಚನೆ mouse_info	mouse;
    ರಚನೆ video_info	video;
    char_u		string[6];
    ಪೂರ್ಣಾಂಕ			ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
    ಪೂರ್ಣಾಂಕ			button;
    ಪೂರ್ಣಾಂಕ			buttons;
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		oldbuttons = 0;

# ifdef FEAT_GUI
    // Don't put events in the input queue now.
    ಆದರೆ (hold_gui_events)
	ಹಿಂತಿರುಗಿ;
# endif

    mouse.operation = MOUSE_GETINFO;
    ಆದರೆ (ioctl(1, FBIO_GETMODE, &video.vi_ಬಹುಲಕ) != -1
	    && ioctl(1, FBIO_MODEINFO, &video) != -1
	    && ioctl(1, CONS_MOUSECTL, &mouse) != -1
	    && video.vi_cheight > 0 && video.vi_cwidth > 0)
    {
	ಅಡ್ಡಸಾಲು = mouse.u.ದತ್ತಾಂಶ.y / video.vi_cheight;
	ಲಂಬಸಾಲು = mouse.u.ದತ್ತಾಂಶ.x / video.vi_cwidth;
	buttons = mouse.u.ದತ್ತಾಂಶ.buttons;
	string[0] = ESC; // Our termcode
	string[1] = 'M';
	string[2] = 'S';
	ಆದರೆ (oldbuttons == buttons && buttons != 0)
	{
	    button = MOUSE_DRAG;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆಯ್ಕೆ (buttons)
	    {
		ಸ್ಥಿತಿ 0:
		    button = MOUSE_RELEASE;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 1:
		    button = MOUSE_ಎಡ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 2:
		    button = MOUSE_ಮಧ್ಯ;
		    ನಿಲ್ಲಿಸು;
		ಸ್ಥಿತಿ 4:
		    button = MOUSE_ಬಲ;
		    ನಿಲ್ಲಿಸು;
		ಪೂರ್ವನಿಯೋಜಿತ:
		    ಹಿಂತಿರುಗಿ;
	    }
	    oldbuttons = buttons;
	}
	string[3] = (char_u)(button);
	string[4] = (char_u)(ಲಂಬಸಾಲು + ' ' + 1);
	string[5] = (char_u)(ಅಡ್ಡಸಾಲು + ' ' + 1);
	ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(string, 6);
    }
    ಹಿಂತಿರುಗಿ;
}
#ಆದರೆ_ಅಂತ್ಯ // FEAT_SYSMOUSE

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if the OS PRNG is not available or something went wrong.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dev_urandom_state = NOTDONE;

    ಆದರೆ (dev_urandom_state == FAIL)
	ಹಿಂತಿರುಗಿ FAIL;

    ಪೂರ್ಣಾಂಕ fd = open("/dev/urandom", O_RDONLY);

    // Attempt reading /dev/urandom.
    ಆದರೆ (fd == -1)
	dev_urandom_state = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (ಪಠ(fd, buf, len) == len)
    {
	dev_urandom_state = OK;
	close(fd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	dev_urandom_state = FAIL;
	close(fd);
    }

    ಹಿಂತಿರುಗಿ dev_urandom_state;
}

#ಆದರೆ defined(FEAT_LIBCALL)
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ char_u * (*STRPROCSTR)(char_u *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ char_u * (*INTPROCSTR)(ಪೂರ್ಣಾಂಕ);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*STRPROCINT)(char_u *);
ಪ್ರಕಾರವ್ಯಾಖ್ಯೆ ಪೂರ್ಣಾಂಕ (*INTPROCINT)(ಪೂರ್ಣಾಂಕ);

/*
 * Call a DLL routine which takes either a string or int param
 * and returns an allocated string.
 */
    ಪೂರ್ಣಾಂಕ
mch_libcall(
    char_u	*libname,
    char_u	*funcname,
    char_u	*argstring,	// NULL when using a argint
    ಪೂರ್ಣಾಂಕ		argint,
    char_u	**string_ಫಲಿತಾಂಶ, // NULL when using number_result
    ಪೂರ್ಣಾಂಕ		*ಸಂಖ್ಯೆ_ಫಲಿತಾಂಶ)
{
# ಆದರೆ defined(USE_DLOPEN)
    ಶೂನ್ಯ	*hinstLib;
    ಅಕ್ಷರ	*dlerr = ಖಾಲಿ;
# ಇಲ್ಲದಿದ್ದರೆ
    shl_t	hinstLib;
# endif
    STRPROCSTR	Proc_ಕೂಡಿಸು;
    INTPROCSTR	Proc_ಕೂಡಿಸು_I;
    char_u	*retval_str = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		retval_int = 0;
    ಪೂರ್ಣಾಂಕ		success = FALSE;

    /*
     * Get a handle to the DLL module.
     */
# ಆದರೆ defined(USE_DLOPEN)
    // First clear any error, it's not cleared by the dlopen() call.
    (ಶೂನ್ಯ)dlerror();

    hinstLib = dlopen((ಅಕ್ಷರ *)libname, RTLD_LAZY
#  ifdef RTLD_LOCAL
	    | RTLD_LOCAL
#  endif
	    );
    ಆದರೆ (hinstLib == ಖಾಲಿ)
    {
	// "dlerr" must be used before dlclose()
	dlerr = dlerror();
	ಆದರೆ (dlerr != ಖಾಲಿ)
	    semsg(_("dlerror = \"%s\""), dlerr);
    }
# ಇಲ್ಲದಿದ್ದರೆ
    hinstLib = shl_ತುಂಬು((ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ*)libname, BIND_ತಕ್ಷಣ|BIND_VERBOSE, 0L);
# endif

    // If the handle is valid, try to get the function address.
    ಆದರೆ (hinstLib != ಖಾಲಿ)
    {
# ifdef USING_SETJMP
	/*
	 * Catch a crash when calling the library function.  For example when
	 * using a number where a string pointer is expected.
	 */
	mch_startjmp();
	ಆದರೆ (SETJMP(lc_jump_env) != 0)
	{
	    success = FALSE;
#  ಆದರೆ defined(USE_DLOPEN)
	    dlerr = ಖಾಲಿ;
#  endif
	    mch_didjmp();
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
	    retval_str = ಖಾಲಿ;
	    retval_int = 0;

	    ಆದರೆ (argstring != ಖಾಲಿ)
	    {
# ಆದರೆ defined(USE_DLOPEN)
		*(ಶೂನ್ಯ **)(&Proc_ಕೂಡಿಸು) = dlsym(hinstLib, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)funcname);
		dlerr = dlerror();
# ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (shl_findsym(&hinstLib, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)funcname,
					TYPE_PROCEDURE, (ಶೂನ್ಯ *)&Proc_ಕೂಡಿಸು) < 0)
		    Proc_ಕೂಡಿಸು = ಖಾಲಿ;
# endif
		ಆದರೆ ((success = (Proc_ಕೂಡಿಸು != ಖಾಲಿ
# ಆದರೆ defined(USE_DLOPEN)
			    && dlerr == ಖಾಲಿ
# endif
			    )))
		{
		    ಆದರೆ (string_ಫಲಿತಾಂಶ == ಖಾಲಿ)
			retval_int = ((STRPROCINT)(ಶೂನ್ಯ *)Proc_ಕೂಡಿಸು)(argstring);
		    ಇಲ್ಲದಿದ್ದರೆ
			retval_str = (Proc_ಕೂಡಿಸು)(argstring);
		}
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
	    {
# ಆದರೆ defined(USE_DLOPEN)
		*(ಶೂನ್ಯ **)(&Proc_ಕೂಡಿಸು_I) = dlsym(hinstLib, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)funcname);
		dlerr = dlerror();
# ಇಲ್ಲದಿದ್ದರೆ
		ಆದರೆ (shl_findsym(&hinstLib, (ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ *)funcname,
				       TYPE_PROCEDURE, (ಶೂನ್ಯ *)&Proc_ಕೂಡಿಸು_I) < 0)
		    Proc_ಕೂಡಿಸು_I = ಖಾಲಿ;
# endif
		ಆದರೆ ((success = (Proc_ಕೂಡಿಸು_I != ಖಾಲಿ
# ಆದರೆ defined(USE_DLOPEN)
			    && dlerr == ಖಾಲಿ
# endif
			    )))
		{
		    ಆದರೆ (string_ಫಲಿತಾಂಶ == ಖಾಲಿ)
			retval_int = ((INTPROCINT)(ಶೂನ್ಯ *)Proc_ಕೂಡಿಸು_I)(argint);
		    ಇಲ್ಲದಿದ್ದರೆ
			retval_str = (Proc_ಕೂಡಿಸು_I)(argint);
		}
	    }

	    // Save the string before we free the library.
	    // Assume that a "1" or "-1" result is an illegal pointer.
	    ಆದರೆ (string_ಫಲಿತಾಂಶ == ಖಾಲಿ)
		*ಸಂಖ್ಯೆ_ಫಲಿತಾಂಶ = retval_int;
	    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (retval_str != ಖಾಲಿ
		    && retval_str != (char_u *)1
		    && retval_str != (char_u *)-1)
		*string_ಫಲಿತಾಂಶ = vim_strsave(retval_str);
	}

# ifdef USING_SETJMP
	mch_endjmp();
#  ifdef SIGHASARG
	ಆದರೆ (lc_signal != 0)
	{
	    ಪೂರ್ಣಾಂಕ i;

	    // try to find the name of this signal
	    ಗಾಗಿ (i = 0; signal_info[i].sig != -1; i++)
		ಆದರೆ (lc_signal == signal_info[i].sig)
		    ನಿಲ್ಲಿಸು;
	    semsg(_(e_got_sig_str_ಒಳ_libcall), signal_info[i].name);
	}
#  endif
# endif

# ಆದರೆ defined(USE_DLOPEN)
	// "dlerr" must be used before dlclose()
	ಆದರೆ (dlerr != ಖಾಲಿ)
	    semsg(_("dlerror = \"%s\""), dlerr);

	// Free the DLL module.
	(ಶೂನ್ಯ)dlclose(hinstLib);
# ಇಲ್ಲದಿದ್ದರೆ
	(ಶೂನ್ಯ)shl_unload(hinstLib);
# endif
    }

    ಆದರೆ (!success)
    {
	semsg(_(e_library_call_failed_for_str), funcname);
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
}
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(FEAT_X11) && defined(FEAT_XCLIPBOARD)
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	xterm_trace = -1;	// default: disabled
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ	xterm_button;

/*
 * Setup a dummy window for X selections in a terminal.
 */
    ಶೂನ್ಯ
ಸಿದ್ಧತೆ_ಪದ_clip(ಶೂನ್ಯ)
{
    ಪೂರ್ಣಾಂಕ		z = 0;
    ಅಕ್ಷರ	*strp = "";
    Widget	AppShell;

    ಆದರೆ (!x_connect_ಗಮ್ಯ_server())
	ಹಿಂತಿರುಗಿ;

    open_app_context();
    ಆದರೆ (app_context != ಖಾಲಿ && xterm_Shell == (Widget)0)
    {
	ಪೂರ್ಣಾಂಕ (*oldhandler)(Display*, XErrorEvent*);
# ಆದರೆ defined(USING_SETJMP)
	ಪೂರ್ಣಾಂಕ (*oldIOhandler)(Display*);
# endif
# ifdef ELAPSED_FUNC
	elapsed_T ಆರಂಭ_tv;

	ಆದರೆ (p_verbose > 0)
	    ELAPSED_INIT(ಆರಂಭ_tv);
# endif

	// Ignore X errors while opening the display
	oldhandler = XSetErrorHandler(x_error_check);

# ಆದರೆ defined(USING_SETJMP)
	// Ignore X IO errors while opening the display
	oldIOhandler = XSetIOErrorHandler(x_IOerror_check);
	mch_startjmp();
	ಆದರೆ (SETJMP(lc_jump_env) != 0)
	{
	    mch_didjmp();
	    xterm_dpy = ಖಾಲಿ;
	}
	ಇಲ್ಲದಿದ್ದರೆ
# endif
	{
	    xterm_dpy = XtOpenDisplay(app_context, xterm_display,
		    "vim_xterm", "Vim_xterm", ಖಾಲಿ, 0, &z, &strp);
	    ಆದರೆ (xterm_dpy != ಖಾಲಿ)
		xterm_dpy_retry_ಎಣಿಕೆ = 0;
# ಆದರೆ defined(USING_SETJMP)
	    mch_endjmp();
# endif
	}

# ಆದರೆ defined(USING_SETJMP)
	// Now handle X IO errors normally.
	(ಶೂನ್ಯ)XSetIOErrorHandler(oldIOhandler);
# endif
	// Now handle X errors normally.
	(ಶೂನ್ಯ)XSetErrorHandler(oldhandler);

	ಆದರೆ (xterm_dpy == ಖಾಲಿ)
	{
	    ಆದರೆ (p_verbose > 0)
		verb_ಸಂದೇಶ(_("Opening the X display failed"));
	    ಹಿಂತಿರುಗಿ;
	}

	// Catch terminating error of the X server connection.
	(ಶೂನ್ಯ)XSetIOErrorHandler(x_IOerror_handler);

# ifdef ELAPSED_FUNC
	ಆದರೆ (p_verbose > 0)
	{
	    verbose_enter();
	    xopen_ಸಂದೇಶ(ELAPSED_FUNC(ಆರಂಭ_tv));
	    verbose_leave();
	}
# endif

	// Create a Shell to make converters work.
	AppShell = XtVaAppCreateShell("vim_xterm", "Vim_xterm",
		applicationShellWidgetClass, xterm_dpy,
		ಖಾಲಿ);
	ಆದರೆ (AppShell == (Widget)0)
	    ಹಿಂತಿರುಗಿ;
	xterm_Shell = XtVaCreatePopupShell("VIM",
		ತುದಿ_Level_Shell_Widget_Class, AppShell,
		XtNmappedWhenManaged, 0,
		XtNwidth, 1,
		XtNheight, 1,
		ಖಾಲಿ);
	ಆದರೆ (xterm_Shell == (Widget)0)
	    ಹಿಂತಿರುಗಿ;

	x11_ಸಿದ್ಧತೆ_atoms(xterm_dpy);
	x11_ಸಿದ್ಧತೆ_selection(xterm_Shell);
	ಆದರೆ (x11_display == ಖಾಲಿ)
	    x11_display = xterm_dpy;

	XtRealizeWidget(xterm_Shell);
	XSync(xterm_dpy, False);
	xterm_update();
    }
    ಆದರೆ (xterm_Shell != (Widget)0)
    {
	clip_init(TRUE);
	ಆದರೆ (x11_ಕಿಟಕಿ == 0 && (strp = getenv("WINDOWID")) != ಖಾಲಿ)
	    x11_ಕಿಟಕಿ = (ಕಿಟಕಿ)atol(strp);
	// Check if $WINDOWID is valid.
	ಆದರೆ (test_x11_ಕಿಟಕಿ(xterm_dpy) == FAIL)
	    x11_ಕಿಟಕಿ = 0;
	ಆದರೆ (x11_ಕಿಟಕಿ != 0)
	    xterm_trace = 0;
    }
}

    ಶೂನ್ಯ
ಆರಂಭ_xterm_trace(ಪೂರ್ಣಾಂಕ button)
{
    ಆದರೆ (x11_ಕಿಟಕಿ == 0 || xterm_trace < 0 || xterm_Shell == (Widget)0)
	ಹಿಂತಿರುಗಿ;
    xterm_trace = 1;
    xterm_button = button;
    do_xterm_trace();
}


    ಶೂನ್ಯ
stop_xterm_trace(ಶೂನ್ಯ)
{
    ಆದರೆ (xterm_trace < 0)
	ಹಿಂತಿರುಗಿ;
    xterm_trace = 0;
}

/*
 * Query the xterm pointer and generate mouse termcodes if necessary
 * return TRUE if dragging is active, else FALSE
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
do_xterm_trace(ಶೂನ್ಯ)
{
    ಕಿಟಕಿ		ಬೇರು, ಮಗು;
    ಪೂರ್ಣಾಂಕ			ಬೇರು_x, ಬೇರು_y;
    ಪೂರ್ಣಾಂಕ			win_x, win_y;
    ಪೂರ್ಣಾಂಕ			ಅಡ್ಡಸಾಲು, ಲಂಬಸಾಲು;
    int_u		ಮುಖವಾಡ_return;
    char_u		buf[50];
    char_u		*strp;
    ದೀರ್ಘ		got_hints;
    ಸ್ಥಿರ char_u	*mouse_ಸಂಕೇತ = ಖಾಲಿ;
    ಸ್ಥಿರ size_t	mouse_codelen = 0;
    ಸ್ಥಿರ char_u	mouse_name[2] = {KS_MOUSE, KE_FILLER};
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		prev_ಅಡ್ಡಸಾಲು = 0, prev_ಲಂಬಸಾಲು = 0;
    ಸ್ಥಿರ XSizeHints	xterm_hints;

    ಆದರೆ (xterm_trace <= 0)
	ಹಿಂತಿರುಗಿ FALSE;

    ಆದರೆ (xterm_trace == 1)
    {
	// Get the hints just before tracking starts.  The font size might
	// have changed recently.
	ಆದರೆ (!XGetWMNormalHints(xterm_dpy, x11_ಕಿಟಕಿ, &xterm_hints, &got_hints)
		|| !(got_hints & PResizeInc)
		|| xterm_hints.ಅಗಲ_inc <= 1
		|| xterm_hints.ಎತ್ತರ_inc <= 1)
	{
	    xterm_trace = -1;  // Not enough data -- disable tracing
	    ಹಿಂತಿರುಗಿ FALSE;
	}

	// Rely on the same mouse code for the duration of this
	mouse_ಸಂಕೇತ = ಕಂಡುಹಿಡಿ_termcode(mouse_name);
	ಆದರೆ (mouse_ಸಂಕೇತ != ಖಾಲಿ)
	    mouse_codelen = STRLEN(mouse_ಸಂಕೇತ);
	prev_ಅಡ್ಡಸಾಲು = mouse_ಅಡ್ಡಸಾಲು;
	prev_ಲಂಬಸಾಲು = mouse_ಲಂಬಸಾಲು;
	xterm_trace = 2;

	// Find the offset of the chars, there might be a scrollbar on the
	// left of the window and/or a menu on the top (eterm etc.)
	XQuery_ಸೂಚಕ(xterm_dpy, x11_ಕಿಟಕಿ, &ಬೇರು, &ಮಗು, &ಬೇರು_x, &ಬೇರು_y,
		      &win_x, &win_y, &ಮುಖವಾಡ_return);
	xterm_hints.y = win_y - (xterm_hints.ಎತ್ತರ_inc * mouse_ಅಡ್ಡಸಾಲು)
			      - (xterm_hints.ಎತ್ತರ_inc / 2);
	ಆದರೆ (xterm_hints.y <= xterm_hints.ಎತ್ತರ_inc / 2)
	    xterm_hints.y = 2;
	xterm_hints.x = win_x - (xterm_hints.ಅಗಲ_inc * mouse_ಲಂಬಸಾಲು)
			      - (xterm_hints.ಅಗಲ_inc / 2);
	ಆದರೆ (xterm_hints.x <= xterm_hints.ಅಗಲ_inc / 2)
	    xterm_hints.x = 2;
	ಹಿಂತಿರುಗಿ TRUE;
    }

    ಆದರೆ (mouse_ಸಂಕೇತ == ಖಾಲಿ || mouse_codelen > 45)
    {
	xterm_trace = 0;
	ಹಿಂತಿರುಗಿ FALSE;
    }

    XQuery_ಸೂಚಕ(xterm_dpy, x11_ಕಿಟಕಿ, &ಬೇರು, &ಮಗು, &ಬೇರು_x, &ಬೇರು_y,
		  &win_x, &win_y, &ಮುಖವಾಡ_return);

    ಅಡ್ಡಸಾಲು = check_ಅಡ್ಡಸಾಲು((win_y - xterm_hints.y) / xterm_hints.ಎತ್ತರ_inc);
    ಲಂಬಸಾಲು = check_ಲಂಬಸಾಲು((win_x - xterm_hints.x) / xterm_hints.ಅಗಲ_inc);
    ಆದರೆ (ಅಡ್ಡಸಾಲು == prev_ಅಡ್ಡಸಾಲು && ಲಂಬಸಾಲು == prev_ಲಂಬಸಾಲು)
	ಹಿಂತಿರುಗಿ TRUE;

    STRCPY(buf, mouse_ಸಂಕೇತ);
    strp = buf + mouse_codelen;
    *strp++ = (xterm_button | MOUSE_DRAG) & ~0x20;
    *strp++ = (char_u)(ಲಂಬಸಾಲು + ' ' + 1);
    *strp++ = (char_u)(ಅಡ್ಡಸಾಲು + ' ' + 1);
    *strp = NUL;
    ಕೂಡಿಸು_ಗಮ್ಯ_ಒಳಹರಿವು_buf(buf, strp - buf);

    prev_ಅಡ್ಡಸಾಲು = ಅಡ್ಡಸಾಲು;
    prev_ಲಂಬಸಾಲು = ಲಂಬಸಾಲು;
    ಹಿಂತಿರುಗಿ TRUE;
}

# ಆದರೆ defined(FEAT_GUI) || defined(FEAT_XCLIPBOARD)
/*
 * Destroy the display, window and app_context.  Required for GTK.
 */
    ಶೂನ್ಯ
clear_xterm_clip(ಶೂನ್ಯ)
{
    ಆದರೆ (xterm_Shell != (Widget)0)
    {
	XtDestroyWidget(xterm_Shell);
	xterm_Shell = (Widget)0;
    }
    ಆದರೆ (xterm_dpy != ಖಾಲಿ)
    {
#  ಆದರೆ 0
	// Lesstif and Solaris crash here, lose some memory
	XtCloseDisplay(xterm_dpy);
#  endif
	ಆದರೆ (x11_display == xterm_dpy)
	    x11_display = ಖಾಲಿ;
	xterm_dpy = ಖಾಲಿ;
    }
#  ಆದರೆ 0
    ಆದರೆ (app_context != (XtAppContext)ಖಾಲಿ)
    {
	// Lesstif and Solaris crash here, lose some memory
	XtDestroyApplicationContext(app_context);
	app_context = (XtAppContext)ಖಾಲಿ;
    }
#  endif
}
# endif

/*
 * Catch up with GUI or X events.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
clip_update(ಶೂನ್ಯ)
{
# ifdef FEAT_GUI
    ಆದರೆ (gui.ಒಳ_use)
	gui_mch_update();
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    ಆದರೆ (xterm_Shell != (Widget)0)
	xterm_update();
}

/*
 * Catch up with any queued X events.  This may put keyboard input into the
 * input buffer, call resize call-backs, trigger timers etc.  If there is
 * nothing in the X event queue (& no timers pending), then we return
 * immediately.
 */
    ಶೂನ್ಯ
xterm_update(ಶೂನ್ಯ)
{
    XEvent event;

    ಗಾಗಿ (;;)
    {
	Xt_ಒಳಹರಿವು_ಮುಖವಾಡ ಮುಖವಾಡ = XtAppPending(app_context);

	ಆದರೆ (ಮುಖವಾಡ == 0 || vim_is_ಒಳಹರಿವು_buf_ತುಂಬಿದ())
	    ನಿಲ್ಲಿಸು;

	ಆದರೆ (ಮುಖವಾಡ & XtIMXEvent)
	{
	    // There is an event to process.
	    Xt_App_ಮುಂದಿನ_Event(app_context, &event);
# ifdef FEAT_CLIENTSERVER
	    {
		XPropertyEvent *e = (XPropertyEvent *)&event;

		ಆದರೆ (e->type == PropertyNotify && e->ಕಿಟಕಿ_2 == comm_ಕಿಟಕಿ
		   && e->atom == commProperty && e->state == Property_New_ಮೌಲ್ಯ)
		    serverEventProc(xterm_dpy, &event, 0);
	    }
# endif
	    XtDispatchEvent(&event);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    // There is something else than an event to process.
	    XtAppProcessEvent(app_context, ಮುಖವಾಡ);
	}
    }
}

    ಪೂರ್ಣಾಂಕ
clip_xterm_own_selection(Clipboard_T *cbd)
{
    ಆದರೆ (xterm_Shell != (Widget)0)
	ಹಿಂತಿರುಗಿ clip_x11_own_selection(xterm_Shell, cbd);
    ಹಿಂತಿರುಗಿ FAIL;
}

    ಶೂನ್ಯ
clip_xterm_lose_selection(Clipboard_T *cbd)
{
    ಆದರೆ (xterm_Shell != (Widget)0)
	clip_x11_lose_selection(xterm_Shell, cbd);
}

    ಶೂನ್ಯ
clip_xterm_request_selection(Clipboard_T *cbd)
{
    ಆದರೆ (xterm_Shell != (Widget)0)
	clip_x11_request_selection(xterm_Shell, xterm_dpy, cbd);
}

    ಶೂನ್ಯ
clip_xterm_ಗಣ_selection(Clipboard_T *cbd)
{
    clip_x11_ಗಣ_selection(cbd);
}
#ಆದರೆ_ಅಂತ್ಯ


#ಆದರೆ defined(USE_XSMP)
/*
 * Code for X Session Management Protocol.
 */

# ಆದರೆ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
/*
 * This is our chance to ask the user if they want to save,
 * or abort the logout
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_handle_interaction(SmcConn smc_conn, Sm_ಸೂಚಕ client_ದತ್ತಾಂಶ_2 ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ		save_cmod_flags;
    ಪೂರ್ಣಾಂಕ		cancel_shutdown = False;

    save_cmod_flags = cmdmod.cmod_flags;
    cmdmod.cmod_flags |= CMOD_CONFIRM;
    ಆದರೆ (check_ಬದಲಾಯಿತು_any(FALSE, FALSE))
	// Mustn't logout
	cancel_shutdown = True;
    cmdmod.cmod_flags = save_cmod_flags;
    setcursor();		// position cursor
    ಹೊರಹೋಗು_flush();

    // Done interaction
    Smc_Interact_ಮುಗಿದ(smc_conn, cancel_shutdown);

    // Finish off
    // Only end save-yourself here if we're not cancelling shutdown;
    // we'll get a cancelled callback later in which we'll end it.
    // Hopefully get around glitchy SMs (like GNOME-1)
    ಆದರೆ (!cancel_shutdown)
    {
	xsmp.save_yourself = False;
	Smc_Save_Yourself_ಮುಗಿದ(smc_conn, True);
    }
}
# endif

/*
 * Callback that starts save-yourself.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_handle_save_yourself(
    SmcConn	smc_conn,
    Sm_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ,
    ಪೂರ್ಣಾಂಕ		save_type ಬಳಸದ,
    Bool	shutdown,
    ಪೂರ್ಣಾಂಕ		interact_style ಬಳಸದ,
    Bool	fast ಬಳಸದ)
{
    // Handle already being in saveyourself
    ಆದರೆ (xsmp.save_yourself)
	Smc_Save_Yourself_ಮುಗಿದ(smc_conn, True);
    xsmp.save_yourself = True;
    xsmp.shutdown = shutdown;

    // First up, preserve all files
    ಹೊರಹೋಗು_flush();
    ml_sync_all(FALSE, FALSE);	// preserve all swap files

    ಆದರೆ (p_verbose > 0)
	verb_ಸಂದೇಶ(_("XSMP handling save-yourself request"));

# ಆದರೆ defined(FEAT_GUI) && defined(USE_XSMP_INTERACT)
    // Now see if we can ask about unsaved files
    ಆದರೆ (shutdown && !fast && gui.ಒಳ_use)
	// Need to interact with user, but need SM's permission
	SmcInteractRequest(smc_conn, SmDialogError,
					xsmp_handle_interaction, client_ದತ್ತಾಂಶ_2);
    ಇಲ್ಲದಿದ್ದರೆ
# endif
    {
	// Can stop the cycle here
	Smc_Save_Yourself_ಮುಗಿದ(smc_conn, True);
	xsmp.save_yourself = False;
    }
}


/*
 * Callback to warn us of imminent death.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_die(SmcConn smc_conn ಬಳಸದ, Sm_ಸೂಚಕ client_ದತ್ತಾಂಶ_2 ಬಳಸದ)
{
    xsmp_close();

    // quit quickly leaving swapfiles for modified buffers behind
    getout_preserve_modified(0);
}


/*
 * Callback to tell us that save-yourself has completed.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_save_complete(
    SmcConn	smc_conn ಬಳಸದ,
    Sm_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ)
{
    xsmp.save_yourself = False;
}


/*
 * Callback to tell us that an instigated shutdown was cancelled
 * (maybe even by us)
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_shutdown_cancelled(
    SmcConn	smc_conn,
    Sm_ಸೂಚಕ	client_ದತ್ತಾಂಶ_2 ಬಳಸದ)
{
    ಆದರೆ (xsmp.save_yourself)
	Smc_Save_Yourself_ಮುಗಿದ(smc_conn, True);
    xsmp.save_yourself = False;
    xsmp.shutdown = False;
}


/*
 * Callback to tell us that a new ICE connection has been established.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
xsmp_ice_connection(
    IceConn	iceConn,
    Ice_ಸೂಚಕ	client_ದತ್ತಾಂಶ ಬಳಸದ,
    Bool	opening,
    Ice_ಸೂಚಕ	*watch_ದತ್ತಾಂಶ ಬಳಸದ)
{
    // Intercept creation of ICE connection fd
    ಆದರೆ (opening)
    {
	xsmp_icefd = Ice_Connection_ಸಂಖ್ಯೆ(iceConn);
	IceRemoveConnectionWatch(xsmp_ice_connection, ಖಾಲಿ);
    }
}


// Handle any ICE processing that's required; return FAIL if SM lost
    ಪೂರ್ಣಾಂಕ
xsmp_handle_ವಿನಂತಿಗಳು(ಶೂನ್ಯ)
{
    Bool rep;

    ಆದರೆ (IceProcessMessages(xsmp.iceconn, ಖಾಲಿ, &rep)
						 == IceProcessMessagesIOError)
    {
	// Lost ICE
	ಆದರೆ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("XSMP lost ICE connection"));
	xsmp_close();
	ಹಿಂತಿರುಗಿ FAIL;
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಹಿಂತಿರುಗಿ OK;
}

ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ dummy;

// Set up X Session Management Protocol
    ಶೂನ್ಯ
xsmp_init(ಶೂನ್ಯ)
{
    ಅಕ್ಷರ		errorstring[80];
    SmcCallbacks	smcallbacks;
# ಆದರೆ 0
    Sm_Prop_ಮೌಲ್ಯ		smname;
    SmProp		smnameprop;
    SmProp		*smprops[1];
# endif

    ಆದರೆ (p_verbose > 0)
	verb_ಸಂದೇಶ(_("XSMP opening connection"));

    xsmp.save_yourself = xsmp.shutdown = False;

    // Set up SM callbacks - must have all, even if they're not used
    smcallbacks.save_yourself.callback = xsmp_handle_save_yourself;
    smcallbacks.save_yourself.client_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;
    smcallbacks.die.callback = xsmp_die;
    smcallbacks.die.client_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;
    smcallbacks.save_complete.callback = xsmp_save_complete;
    smcallbacks.save_complete.client_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;
    smcallbacks.shutdown_cancelled.callback = xsmp_shutdown_cancelled;
    smcallbacks.shutdown_cancelled.client_ದತ್ತಾಂಶ_2 = ಖಾಲಿ;

    // Set up a watch on ICE connection creations.  The "dummy" argument is
    // apparently required for FreeBSD (we get a BUS error when using NULL).
    ಆದರೆ (Ice_ಕೂಡಿಸು_Connection_Watch(xsmp_ice_connection, &dummy) == 0)
    {
	ಆದರೆ (p_verbose > 0)
	    verb_ಸಂದೇಶ(_("XSMP ICE connection watch failed"));
	ಹಿಂತಿರುಗಿ;
    }

    // Create an SM connection
    xsmp.smcconn = SmcOpenConnection(
	    ಖಾಲಿ,
	    ಖಾಲಿ,
	    SmProtoMajor,
	    SmProtoMinor,
	    Smc_Save_Yourself_Proc_ಮುಖವಾಡ | Smc_Die_Proc_ಮುಖವಾಡ
		     | Smc_Save_Complete_Proc_ಮುಖವಾಡ | Smc_Shutdown_Cancelled_Proc_ಮುಖವಾಡ,
	    &smcallbacks,
	    ಖಾಲಿ,
	    &xsmp.clientid,
	    ಗಾತ್ರ(errorstring) - 1,
	    errorstring);
    ಆದರೆ (xsmp.smcconn == ಖಾಲಿ)
    {
	ಆದರೆ (p_verbose > 0)
	{
	    ಅಕ್ಷರ errorreport[132];

	    // If the message is too long it might not be NUL terminated.  Add
	    // a NUL at the end to make sure we don't go over the end.
	    errorstring[ಗಾತ್ರ(errorstring) - 1] = NUL;
	    vim_snprintf(errorreport, ಗಾತ್ರ(errorreport),
			 _("XSMP SmcOpenConnection failed: %s"), errorstring);
	    verb_ಸಂದೇಶ(errorreport);
	}
	ಹಿಂತಿರುಗಿ;
    }
    xsmp.iceconn = SmcGetIceConnection(xsmp.smcconn);

# ಆದರೆ 0
    // ID ourselves
    smname.ಮೌಲ್ಯ = "vim";
    smname.ಉದ್ದಳತೆ = 3;
    smnameprop.name = "SmProgram";
    smnameprop.type = "SmARRAY8";
    smnameprop.ಅಂಕೆ_vals = 1;
    smnameprop.vals = &smname;

    smprops[0] = &smnameprop;
    Smc_ಗಣ_Properties(xsmp.smcconn, 1, smprops);
# endif
}


// Shut down XSMP comms.
    ಶೂನ್ಯ
xsmp_close(ಶೂನ್ಯ)
{
    ಆದರೆ (xsmp_icefd == -1)
	ಹಿಂತಿರುಗಿ;

    SmcCloseConnection(xsmp.smcconn, 0, ಖಾಲಿ);
    ಆದರೆ (xsmp.clientid != ಖಾಲಿ)
	ಬಿಡುಗಡೆ(xsmp.clientid);
    xsmp.clientid = ಖಾಲಿ;
    xsmp_icefd = -1;
}
#ಆದರೆ_ಅಂತ್ಯ // USE_XSMP

#ಆದರೆ defined(FEAT_RELTIME)
# ಆದರೆ defined(PROF_NSEC)
/*
 * Implement timeout with timer_create() and timer_settime().
 */
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t timeout_flag = FALSE;
ಸ್ಥಿರ timer_t		     timer_id;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		     timer_created = FALSE;

/*
 * Callback for when the timer expires.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_flag(ಒಕ್ಕೂಟ sigval ಬಳಸದ_3 ಬಳಸದ)
{
    timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ರಚನೆ itimerspec disarm = {{0, 0}, {0, 0}};

    ಆದರೆ (timer_created)
    {
	ಪೂರ್ಣಾಂಕ ret = timer_settime(timer_id, 0, &disarm, ಖಾಲಿ);

	ಆದರೆ (ret < 0)
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
    ಅಸ್ಥಿರ sig_atomic_t *
ಆರಂಭ_timeout(ದೀರ್ಘ msec)
{
    ರಚನೆ itimerspec interval = {
	    {0, 0},					// Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000000}};	// Timeout interval
    ಪೂರ್ಣಾಂಕ ret;

    // This is really the caller's responsibility, but let's make sure the
    // previous timer has been stopped.
    stop_timeout();

    ಆದರೆ (!timer_created)
    {
	ರಚನೆ sigevent action = {0};

	action.sigev_notify = SIGEV_THREAD;
	action.sigev_notify_function = ಗಣ_flag;
	ret = timer_create(CLOCK_MONOTONIC, &action, &timer_id);
	ಆದರೆ (ret < 0)
	{
	    semsg(_(e_could_not_ಗಣ_timeout_str), strerror(errno));
	    ಹಿಂತಿರುಗಿ &timeout_flag;
	}
	timer_created = TRUE;
    }

#  ifdef FEAT_EVAL
    ch_log(ಖಾಲಿ, "setting timeout timer to %d sec %ld nsec",
	       (ಪೂರ್ಣಾಂಕ)interval.it_value.tv_sec, (ದೀರ್ಘ)interval.it_value.tv_nsec);
#  endif
    ret = timer_settime(timer_id, 0, &interval, ಖಾಲಿ);
    ಆದರೆ (ret < 0)
	semsg(_(e_could_not_ಗಣ_timeout_str), strerror(errno));

    ಹಿಂತಿರುಗಿ &timeout_flag;
}

/*
 * To be used before fork/exec: delete any created timer.
 */
    ಶೂನ್ಯ
delete_timer(ಶೂನ್ಯ)
{
    ಆದರೆ (!timer_created)
	ಹಿಂತಿರುಗಿ;

    timer_delete(timer_id);
    timer_created = FALSE;
}

# ಇಲ್ಲದಿದ್ದರೆ // PROF_NSEC

/*
 * Implement timeout with setitimer()
 */
ಸ್ಥಿರ ರಚನೆ sigaction		prev_sigaction;
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t	timeout_flag	     = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ			timer_ಸಕ್ರಿಯ	     = FALSE;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ			timer_handler_ಸಕ್ರಿಯ = FALSE;
ಸ್ಥಿರ ಅಸ್ಥಿರ sig_atomic_t	alarm_pending	     = FALSE;

/*
 * Handle SIGALRM for a timeout.
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಗಣ_flag SIGDEFARG(sigarg)
{
    ಆದರೆ (alarm_pending)
	alarm_pending = FALSE;
    ಇಲ್ಲದಿದ್ದರೆ
	timeout_flag = TRUE;
}

/*
 * Stop any active timeout.
 */
    ಶೂನ್ಯ
stop_timeout(ಶೂನ್ಯ)
{
    ಸ್ಥಿರ ರಚನೆ itimerval disarm = {{0, 0}, {0, 0}};
    ಪೂರ್ಣಾಂಕ			    ret;

    ಆದರೆ (timer_ಸಕ್ರಿಯ)
    {
	timer_ಸಕ್ರಿಯ = FALSE;
	ret = setitimer(ITIMER_REAL, &disarm, ಖಾಲಿ);
	ಆದರೆ (ret < 0)
	    // Should only get here as a result of coding errors.
	    semsg(_(e_could_not_clear_timeout_str), strerror(errno));
    }

    ಆದರೆ (timer_handler_ಸಕ್ರಿಯ)
    {
	timer_handler_ಸಕ್ರಿಯ = FALSE;
	ret = sigaction(SIGALRM, &prev_sigaction, ಖಾಲಿ);
	ಆದರೆ (ret < 0)
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
    ಅಸ್ಥಿರ sig_atomic_t *
ಆರಂಭ_timeout(ದೀರ್ಘ msec)
{
    ರಚನೆ itimerval	interval = {
	    {0, 0},				    // Do not repeat.
	    {msec / 1000, (msec % 1000) * 1000}};   // Timeout interval
    ರಚನೆ sigaction	handle_alarm;
    ಪೂರ್ಣಾಂಕ			ret;
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
    ಆದರೆ (ret == 0)
    {
	alarm_pending = sigismember(&sigs, SIGALRM);
	ret = sigprocmask(SIG_SETMASK, &saved_sigs, ಖಾಲಿ);
    }
    ಆದರೆ (unlikely(ret != 0 || alarm_pending < 0))
    {
	// Just catching coding errors. Write an error message, but carry on.
	semsg(_(e_could_not_check_for_pending_sigalrm_str), strerror(errno));
	alarm_pending = FALSE;
    }

    // Set up the alarm handler first.
    ret = sigemptyset(&handle_alarm.sa_mask);
    handle_alarm.sa_handler = ಗಣ_flag;
    handle_alarm.sa_flags = 0;
    ret = ret == 0 ?  sigaction(SIGALRM, &handle_alarm, &prev_sigaction) : ret;
    ಆದರೆ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_ಗಣ_handler_for_timeout_str), strerror(errno));
	ಹಿಂತಿರುಗಿ &timeout_flag;
    }
    timer_handler_ಸಕ್ರಿಯ = TRUE;

    // Set up the interval timer once the alarm handler is in place.
    ret = setitimer(ITIMER_REAL, &interval, ಖಾಲಿ);
    ಆದರೆ (ret < 0)
    {
	// Should only get here as a result of coding errors.
	semsg(_(e_could_not_ಗಣ_timeout_str), strerror(errno));
	stop_timeout();
	ಹಿಂತಿರುಗಿ &timeout_flag;
    }

    timer_ಸಕ್ರಿಯ = TRUE;
    ಹಿಂತಿರುಗಿ &timeout_flag;
}
# endif // PROF_NSEC
#ಆದರೆ_ಅಂತ್ಯ  // FEAT_RELTIME
