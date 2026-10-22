/* vi:set ts=8 sts=4 sw=4 noet:
 *
 * VIM - Vi IMproved	by Bram Moolenaar
 *
 * Do ":help uganda"  in Vim to read copying and usage conditions.
 * Do ":help credits" in Vim to see a list of people who contributed.
 * See README.txt for an overview of the Vim source code.
 */

/*
 * os_amiga.c
 *
 * Amiga system-dependent routines.
 */

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ifdef Window
# undef Window	// Amiga has its own Window definition
#endif

#undef TRUE		// will be redefined by exec/types.h
#undef FALSE

#ifndef LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#endif

// XXX These are included from os_amiga.h
// #include <exec/types.h>
// #include <libraries/dos.h>
// #include <libraries/dosextens.h>
// #include <proto/exec.h>
// #include <proto/dos.h>
// #include <proto/intuition.h>

#ಅನ್ತರ್ಭಾವ <exec/memory.h>

#ಅನ್ತರ್ಭಾವ <dos/dostags.h>	    // for 2.0 functions
#ಅನ್ತರ್ಭಾವ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#ಅನ್ಯಥಾ
# define	free_fib(x) vim_free(fib)
#endif

#ಯದಿ defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ಯದಿ defined(__amigaos4__)
ಸ್ಥಿರ ಧ್ರುವ ವರ್ಣ* __attribute__((used)) stackcookie = "$STACK: 1048576";
#ನೋಚೇದ್ಯದಿ defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#ನೋಚೇದ್ಯದಿ defined(__GNUC__) && defined(AMIGA)
unsigned long __stack = 262144;
#endif

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#undef	TRUE
#define TRUE (1)
#undef	FALSE
#define FALSE (0)

#ifdef __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#ನೋಚೇದ್ಯದಿ !defined(AZTEC_C) && !defined(__AROS__)
ಸ್ಥಿರ long dos_packet(ಸಂರಚನಾ ಸಂದೇಶ_Port *, long, long);
#endif
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ lock2name(BPTR lock, char_u *buf, long   len);
ಸ್ಥಿರ ಶೂನ್ಯ out_ಅಂಕೆ(long n);
ಸ್ಥಿರ ಸಂರಚನಾ FileInfoBlock *get_fib(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ sortcmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ಸ್ಥಿರ BPTR
safe_Lock(UBYTE *name, long mode)
{
    ಸಂರಚನಾ Process *me = (ಸಂರಚನಾ Process *)FindTask(ರಿಕ್ತ);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    ಪ್ರತ್ಯಾಗಮ flock;
}

ಸ್ಥಿರ BPTR		raw_in = (BPTR)ರಿಕ್ತ;
ಸ್ಥಿರ BPTR		raw_out = (BPTR)ರಿಕ್ತ;
ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ಸಂರಚನಾ IntuitionBase	*IntuitionBase = ರಿಕ್ತ;
#endif
#ifdef FEAT_ARP
ಸಂರಚನಾ ArpBase		*ArpBase = ರಿಕ್ತ;
#endif

ಸ್ಥಿರ ಸಂರಚನಾ Window	*wb_window;
ಸ್ಥಿರ char_u		*oldwindowtitle = ರಿಕ್ತ;

#ifdef FEAT_ARP
ಪೂರ್ಣಾಙ್ಕ			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
ಪೂರ್ಣಾಙ್ಕ			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
ಸ್ಥಿರ ವರ್ಣ version[] __attribute__((used)) =
    "\0$VER: Vim "
    VIM_VERSION_MAJOR_STR "."
    VIM_VERSION_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#endif

    ಶೂನ್ಯ
win_resize_on(ಶೂನ್ಯ)
{
    OUT_STR_NF("\033[12{");
}

    ಶೂನ್ಯ
win_resize_off(ಶೂನ್ಯ)
{
    OUT_STR_NF("\033[12}");
}

    ಶೂನ್ಯ
mch_write(char_u *p, ಪೂರ್ಣಾಙ್ಕ len)
{
    Write(raw_out, (ವರ್ಣ *)p, (long)len);
}

/*
 * mch_inchar(): low level input function.
 * Get a characters from the keyboard.
 * If time == 0 do not wait for characters.
 * If time == n wait a short time for characters.
 * If time == -1 wait forever for characters.
 *
 * Return number of characters read.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_inchar(
    char_u  *buf,
    ಪೂರ್ಣಾಙ್ಕ	    maxlen,
    long    time,		// milliseconds
    ಪೂರ್ಣಾಙ್ಕ	    tb_change_cnt UNUSED)
{
    ಪೂರ್ಣಾಙ್ಕ	    len;
    long    utime;

    ಯದಿ (time >= 0)
    {
	ಯದಿ (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	ಅನ್ಯಥಾ
	    utime = time * 1000L;   // convert from milli to micro secs
	ಯದಿ (WaitForChar(raw_in, utime) == 0)	// no character available
	    ಪ್ರತ್ಯಾಗಮ 0;
    }
    ಅನ್ಯಥಾ    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ಯದಿ (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    ಯದಿ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ಪೂರ್ಣಾಙ್ಕ)KE_CURSORHOLD;
		ಪ್ರತ್ಯಾಗಮ 3;
	    }
	    before_blocking();
	}
    }

    ಪ್ರತಿ (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (ವರ್ಣ *)buf, (long)maxlen / ಒಳಹರಿವು_conv.vc_factor);
	ಯದಿ (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ಯದಿ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
		len = convert_ಒಳಹರಿವು(buf, len, maxlen);
	    ಪ್ರತ್ಯಾಗಮ len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_char_avail(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(ಪೂರ್ಣಾಙ್ಕ special)
{
#ಯದಿ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ಪ್ರತ್ಯಾಗಮ (long_u)AvailMem(MEMF_ANY) >> 10;
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    ಶೂನ್ಯ
mch_delay(long msec, ಪೂರ್ಣಾಙ್ಕ flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    ಶೂನ್ಯ	    Delay(long);
#endif

    ಯದಿ (msec <= 0)
	ಪ್ರತ್ಯಾಗಮ;

    ಯದಿ (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    ಅನ್ಯಥಾ
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ಸ್ಥಿರ ವರ್ಣ	    intlibname[] = "intuition.library";
#endif

#ifdef AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#endif

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	ಸಂರಚನಾ Process *me = (ಸಂರಚನಾ Process *)FindTask(ರಿಕ್ತ);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ಯದಿ (raw_in == (BPTR)ರಿಕ್ತ)
    {
	raw_in = ಒಳಹರಿವು_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ಯದಿ (!IsInteractive(raw_in))
	    raw_in = raw_out;
	ಅನ್ಯಥಾ ಯದಿ (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = ರಿಕ್ತ;
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ಯದಿ ((IntuitionBase = (ಸಂರಚನಾ IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ರಿಕ್ತ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#ಅನ್ತರ್ಭಾವ <workbench/startup.h>

/*
 * Check_win checks whether we have an interactive window.
 * If not, a new window is opened with the newcli command.
 * If we would open a window ourselves, the :sh and :! commands would not
 * work properly (Why? probably because we are then running in a background
 * CLI). This also is the best way to assure proper working in a next
 * Workbench release.
 *
 * For the -f option (foreground mode) we open our own window and disable :sh.
 * Otherwise the calling program would never know when editing is finished.
 */
#define BUF2SIZE 320	    // length of buffer for argument with complete path

    ಪೂರ್ಣಾಙ್ಕ
mch_check_win(ಪೂರ್ಣಾಙ್ಕ argc, ವರ್ಣ **argv)
{
    ಪೂರ್ಣಾಙ್ಕ		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    ಸ್ಥಿರ char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ಸ್ಥಿರ char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ಸಂರಚನಾ WBArg    *argp;
    ಪೂರ್ಣಾಙ್ಕ		    ac;
    ವರ್ಣ	    *av;
    char_u	    *device = ರಿಕ್ತ;
    ಪೂರ್ಣಾಙ್ಕ		    exitval = 4;
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ಸಂರಚನಾ Library  *DosBase;
#endif
    ಪೂರ್ಣಾಙ್ಕ		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    ಯದಿ (DosBase != ರಿಕ್ತ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    ಅನ್ಯಥಾ	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# ಅನ್ಯಥಾ
		    // need arp functions for dos 1.x
	ಯದಿ (!(ArpBase = (ಸಂರಚನಾ ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, ArpVersion);
	    exit(3);
	}
# endif
    }
#endif	/* __amigaos4__ __AROS__ __MORPHOS__ */

    /*
     * scan argv[] for the "-f" and "-d" arguments
     */
    ಪ್ರತಿ (i = 1; i < argc; ++i)
	ಯದಿ (argv[i][0] == '-')
	{
	    ವಿಕಲ್ಪ (argv[i][1])
	    {
	    ಪಕ್ಷ 'f':
		usewin = TRUE;
		ಭಙ್ಗ;

	    ಪಕ್ಷ 'd':
		ಯದಿ (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		ಭಙ್ಗ;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ಯದಿ (argc != 0
	    && device == ರಿಕ್ತ
	    && (IsInteractive(ಒಳಹರಿವು_3()) || IsInteractive(Output())))
	ಪ್ರತ್ಯಾಗಮ OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ಯದಿ (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	ಪ್ರತಿ (i = -1; i < 3; ++i)
	{
	    ಯದಿ (i >= 0)
		device = constrings[i];
	    ಯದಿ (device != ರಿಕ್ತ && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)ರಿಕ್ತ)
		ಭಙ್ಗ;
	}
	ಯದಿ (raw_in == (BPTR)ರಿಕ್ತ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    ಗಚ್ಛ exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	ಪ್ರತ್ಯಾಗಮ OK;
    }

    ಯದಿ ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)ರಿಕ್ತ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	ಗಚ್ಛ exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ಯದಿ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((ವರ್ಣ *)buf1, "t:nc%p", FindTask(0));
#ಅನ್ಯಥಾ
    sprintf((ವರ್ಣ *)buf1, "t:nc%ld", (long)buf1);
#endif
    ಯದಿ ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)ರಿಕ್ತ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((ವರ್ಣ *)buf1);
	mch_errmsg("\n");
	ಗಚ್ಛ exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ಯದಿ (argc == 0)	// run from workbench
	ac = ((ಸಂರಚನಾ WBStartup *)argv)->sm_ಅಂಕೆ_Args;
    ಅನ್ಯಥಾ
	ac = argc;
    ಪ್ರತಿ (i = 0; i < ac; ++i)
    {
	ಯದಿ (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ಸಂರಚನಾ WBStartup *)argv)->sm_ArgList[i]);
	    ಯದಿ (argp->wa_Lock)
		(ಶೂನ್ಯ)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    ಯದಿ (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    ಅನ್ಯಥಾ	    // use arp function
		TackOn((ವರ್ಣ *)buf2, argp->wa_Name);
#endif
	    av = (ವರ್ಣ *)buf2;
	}
	ಅನ್ಯಥಾ
	    av = argv[i];

	// skip '-d' or "-dev" option
	ಯದಿ (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    ಅನುವರ್ತ;
	}
	ಯದಿ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	ಯದಿ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, " ", 1L);
    }
    Write(fh, "\nendcli\n", 8L);
    Close(fh);

/*
 * Try to open a new cli in a window. If "-d" or "-dev" argument was given try
 * to open the specified device. Then try a 24 line 80 column window.  If that
 * fails, try two smaller ones.
 */
    ಪ್ರತಿ (i = -1; i < 3; ++i)
    {
	ಯದಿ (i >= 0)
	    device = constrings[i];
	ಅನ್ಯಥಾ ಯದಿ (device == ರಿಕ್ತ)
	    ಅನುವರ್ತ;
	sprintf((ವರ್ಣ *)buf2, "newcli <nil: >nil: %s from %s", (ವರ್ಣ *)device, (ವರ್ಣ *)buf1);
#ifdef FEAT_ARP
	ಯದಿ (dos2)
	{
#endif
	    ಯದಿ (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		ಭಙ್ಗ;
#ifdef FEAT_ARP
	}
	ಅನ್ಯಥಾ
	{
	    ಯದಿ (Execute((UBYTE *)buf2, nilfh, nilfh))
		ಭಙ್ಗ;
	}
#endif
    }
    ಯದಿ (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	ಗಚ್ಛ exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    ಯದಿ (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    ಯದಿ (ArpBase)
	CloseLibrary((ಸಂರಚನಾ Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಙ್ಕ		len UNUSED)		// buffer size, ignored here
{
    ಸಂರಚನಾ FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ಯದಿ (fib == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    ಯದಿ (fib->fib_DirEntryType == ST_ಬೇರು)
	strcat(fib->fib_FileName, ":");
#endif
    ಯದಿ (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ಸ್ಥಿರ ಸಂರಚನಾ FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ಸಂರಚನಾ FileInfoBlock    *fib;

    ಯದಿ (fname == ರಿಕ್ತ)	    // safety check
	ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#ಅನ್ಯಥಾ
    fib = ALLOC_ONE(ಸಂರಚನಾ FileInfoBlock);
#endif
    ಯದಿ (fib == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    ಯದಿ (flock == (BPTR)ರಿಕ್ತ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ರಿಕ್ತ;
    }
    ಯದಿ (flock)
	UnLock(flock);
    ಪ್ರತ್ಯಾಗಮ fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    ಶೂನ್ಯ
mch_settitle(char_u *title, char_u *icon)
{
    ಯದಿ (wb_window != ರಿಕ್ತ && title != ರಿಕ್ತ)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಙ್ಕ which)
{
    ಯದಿ (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ (wb_window != ರಿಕ್ತ);
}

    ಪೂರ್ಣಾಙ್ಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಙ್ಕ on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_user_name(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
#ಯದಿ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ಸಂರಚನಾ passwd   *pwd = getpwuid(getuid());

    ಯದಿ (pwd != ರಿಕ್ತ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ಪ್ರತ್ಯಾಗಮ OK;
    }
#endif
    *s = NUL;
    ಪ್ರತ್ಯಾಗಮ FAIL;
}

/*
 * Insert host name is s[len].
 */
    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಙ್ಕ len)
{
#ಯದಿ defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((ವರ್ಣ *)s, len);
#ಅನ್ಯಥಾ
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(ಶೂನ್ಯ)
{
#ಯದಿ defined(__amigaos4__)
    ಪ್ರತ್ಯಾಗಮ (long) getpid();
#ನೋಚೇದ್ಯದಿ defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ಪ್ರತ್ಯಾಗಮ((long)FindTask(0));
#ಅನ್ಯಥಾ
    ಪ್ರತ್ಯಾಗಮ (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_dirname(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಪ್ರತ್ಯಾಗಮ mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಙ್ಕ		len,
    ಪೂರ್ಣಾಙ್ಕ		force)
{
    BPTR	l;
    ಪೂರ್ಣಾಙ್ಕ		retval = FAIL;
    ಪೂರ್ಣಾಙ್ಕ		i;

    // Lock the file.  If it exists, we can get the exact name.
    ಯದಿ ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    ಅನ್ಯಥಾ ಯದಿ (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ಯದಿ ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)ರಿಕ್ತ)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    ಯದಿ (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ಯದಿ (i == 0 || *fname != NUL)
		{
		    ಯದಿ (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ಯದಿ (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isFullName(char_u *fname)
{
    ಪ್ರತ್ಯಾಗಮ (vim_strchr(fname, ':') != ರಿಕ್ತ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    ಯದಿ (dos2)		    // use 2.0 function
#endif
	ಪ್ರತ್ಯಾಗಮ ((ಪೂರ್ಣಾಙ್ಕ)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    ಅನ್ಯಥಾ		// use arp function
	ಪ್ರತ್ಯಾಗಮ ((ಪೂರ್ಣಾಙ್ಕ)PathName(lock, (ವರ್ಣ *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    ಸಂರಚನಾ FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    ಯದಿ (fib == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ಪ್ರತ್ಯಾಗಮ (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_isdir(char_u *name)
{
    ಸಂರಚನಾ FileInfoBlock    *fib;
    ಪೂರ್ಣಾಙ್ಕ			    retval = FALSE;

    fib = get_fib(name);
    ಯದಿ (fib == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#ಅನ್ಯಥಾ
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Create directory "name".
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    ಯದಿ (lock == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ -1;

    UnLock(lock);
    ಪ್ರತ್ಯಾಗಮ 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_can_exe(char_u *name, char_u **path UNUSED, ಪೂರ್ಣಾಙ್ಕ use_path)
{
    ಪೂರ್ಣಾಙ್ಕ exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    ಯದಿ (seg && GetSegListInfoTags(seg, GSLI_Native, ರಿಕ್ತ, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, ರಿಕ್ತ, TAG_DONE))
    {
	// Test if file permissions allow execution.
	ಸಂರಚನಾ ExamineData *exd = ExamineObjectTags(EX_String_Name_ಒಳಹರಿವು, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    ಅನ್ಯಥಾ
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    ಯದಿ (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ಸಂರಚನಾ Path_ಗಂಟು *head = DupCmdPathList(ರಿಕ್ತ);

	// For each entry, recur to check for executable.
	ಪ್ರತಿ (ಸಂರಚನಾ Path_ಗಂಟು *tail = head; !exe && tail;
			       tail = (ಸಂರಚನಾ Path_ಗಂಟು *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    ಪ್ರತ್ಯಾಗಮ exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    ಪ್ರತ್ಯಾಗಮ ಗಂಟು_NORMAL;
}

    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಙ್ಕ r)
{
    exiting = TRUE;

    ಯದಿ (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    ಯದಿ (raw_out)
    {
	ಯದಿ (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ಯದಿ (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    ಯದಿ (ArpBase)
	CloseLibrary((ಸಂರಚನಾ Library *) ArpBase);
#endif
    ಯದಿ (close_win)
	Close(raw_in);
    ಯದಿ (r)
	printf(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    exit(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#define MP(xx)	((ಸಂರಚನಾ ಸಂದೇಶ_Port *)((ಸಂರಚನಾ FileHandle *) (BADDR(xx)))->fh_Type)

/*
 * Function mch_settmode() - Convert the specified file pointer to 'raw' or
 * 'cooked' mode. This only works on TTY's.
 *
 * Raw: keeps DOS from translating keys for you, also (BIG WIN) it means
 *	getch() will return immediately rather than wait for a return. You
 *	lose editing features though.
 *
 * Cooked: This function returns the designate file pointer to its normal,
 *	wait for a <CR> mode. This is exactly like raw() except that
 *	it sends a 0 to the console to make it back into a CON: from a RAW:
 */
    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
#ಯದಿ defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ಯದಿ (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#ಅನ್ಯಥಾ
    ಯದಿ (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#endif
	mch_errmsg(_("cannot change console mode ?!\n"));
}

/*
 * Code for this routine came from the following :
 *
 * ConPackets.c -  C. Scheppner, A. Finkel, P. Lindsay	CBM
 *   DOS packet example
 *   Requires 1.2
 *
 * Found on Fish Disk 56.
 *
 * Heavely modified by mool.
 */

#ಅನ್ತರ್ಭಾವ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ಯದಿ defined(__AROS__) || defined(__MORPHOS__)
    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    ಯದಿ (!term_console)
	ಪ್ರತ್ಯಾಗಮ FAIL;

    ಯದಿ (raw_in && raw_out)
    {
	// Save current console mode.
	ಪೂರ್ಣಾಙ್ಕ old_tmode = cur_tmode;
	ವರ್ಣ ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ಯದಿ (Write(raw_out, ctrl, ಪರಿಮಾಣ(ctrl)) == ಪರಿಮಾಣ(ctrl))
	{
	    ವರ್ಣ scan[] = "\x9b""1;1;%d;%d r",
		 answ[ಪರಿಮಾಣ(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    ಯದಿ (Read(raw_in, answ, ಪರಿಮಾಣ(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ಯದಿ (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ಪ್ರತ್ಯಾಗಮ OK;
		}
	    }
	}

	// Restore console mode.
	mch_settmode(old_tmode);
    }

    // I/O error. Default size fallback.
    term_console = FALSE;
    Columns = 80;
    Rows = 24;

    ಪ್ರತ್ಯಾಗಮ FAIL;
}
#ಅನ್ಯಥಾ
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    ಸಂರಚನಾ ConUnit  *conUnit;
# ifndef __amigaos4__
    ವರ್ಣ	    id_a[ಪರಿಮಾಣ(ಸಂರಚನಾ InfoData) + 3];
# endif
    ಸಂರಚನಾ InfoData *id=0;

    ಯದಿ (!term_console)	// not an amiga window
	ಗಚ್ಛ out;

    // insure longword alignment
# ifdef __amigaos4__
    ಯದಿ (!(id = AllocDosObject(DOS_INFODATA, 0)))
	ಗಚ್ಛ out;
# ಅನ್ಯಥಾ
    id = (ಸಂರಚನಾ InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ಯದಿ (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    ಯದಿ (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (ಸಂರಚನಾ Window *)id->id_Volume_ಗಂಟು) == ರಿಕ್ತ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	ಗಚ್ಛ out;
    }
    ಯದಿ (oldwindowtitle == ರಿಕ್ತ)
	oldwindowtitle = (char_u *)wb_window->Title;
    ಯದಿ (id->id_InUse == (BPTR)ರಿಕ್ತ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }
    conUnit = (ಸಂರಚನಾ ConUnit *) ((ಸಂರಚನಾ IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ಯದಿ (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	ಪ್ರತ್ಯಾಗಮ FAIL;
    }

    ಪ್ರತ್ಯಾಗಮ OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ಪ್ರತ್ಯಾಗಮ FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    ಶೂನ್ಯ
mch_set_shellsize(ಶೂನ್ಯ)
{
    ಯದಿ (!term_console)
	ಪ್ರತ್ಯಾಗಮ;

    size_set = TRUE;
    out_char(CSI);
    out_ಅಂಕೆ((long)Rows);
    out_char('t');
    out_char(CSI);
    out_ಅಂಕೆ((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
out_ಅಂಕೆ(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#ಯದಿ !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
/*
 * Sendpacket.c
 *
 * An invaluable addition to your Amiga.lib file. This code sends a packet to
 * the given message port. This makes working around DOS lots easier.
 *
 * Note, I didn't write this, those wonderful folks at CBM did. I do suggest
 * however that you may wish to add it to Amiga.Lib, to do so, compile it and
 * say 'oml lib:amiga.lib -r sendpacket.o'
 */

//#include <proto/exec.h>
//#include <proto/dos.h>
# include <exec/memory.h>

/*
 * Function - dos_packet written by Phil Lindsay, Carolyn Scheppner, and Andy
 * Finkel. This function will send a packet of the given type to the Message
 * Port supplied.
 */

    ಸ್ಥಿರ long
dos_packet(
    ಸಂರಚನಾ ಸಂದೇಶ_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    ಸಂರಚನಾ ಸಂದೇಶ_Port	    *replyport;
    ಸಂರಚನಾ StandardPacket   *packet;
    long		    res1;

    ಯದಿ (dos2)
# endif
	ಪ್ರತ್ಯಾಗಮ DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ಸಂರಚನಾ ಸಂದೇಶ_Port *) CreatePort(ರಿಕ್ತ, 0);	// use arp function
    ಯದಿ (!replyport)
	ಪ್ರತ್ಯಾಗಮ (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ಸಂರಚನಾ StandardPacket *)
	AllocMem((long) ಪರಿಮಾಣ(ಸಂರಚನಾ StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    ಯದಿ (!packet)
    {
	DeletePort(replyport);
	ಪ್ರತ್ಯಾಗಮ (0);
    }
    packet->sp_ಸಂದೇಶ.mn_ಗಂಟು.ln_Name = (ವರ್ಣ *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_ಸಂದೇಶ);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_ಸಂದೇಶ(pid, (ಸಂರಚನಾ Message *)packet);	// send packet

    WaitPort(replyport);
    Get_ಸಂದೇಶ(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) ಪರಿಮಾಣ(ಸಂರಚನಾ StandardPacket));
    DeletePort(replyport);

    ಪ್ರತ್ಯಾಗಮ (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_call_shell(
    char_u	*cmd,
    ಪೂರ್ಣಾಙ್ಕ		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    ಪೂರ್ಣಾಙ್ಕ		x;
    ಪೂರ್ಣಾಙ್ಕ		tmode = cur_tmode;
#ifdef AZTEC_C
    ಪೂರ್ಣಾಙ್ಕ		use_execute;
    char_u	*shellcmd = ರಿಕ್ತ;
    char_u	*shellarg;
#endif
    ಪೂರ್ಣಾಙ್ಕ		retval = 0;

    ಯದಿ (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ಪ್ರತ್ಯಾಗಮ -1;
    }

    ಯದಿ (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    ಯದಿ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#ಯದಿ !defined(AZTEC_C)		    // not tested very much
    ಯದಿ (cmd == ರಿಕ್ತ)
    {
# ifdef FEAT_ARP
	ಯದಿ (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ಅನ್ಯಥಾ
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    ಅನ್ಯಥಾ
    {
# ifdef FEAT_ARP
	ಯದಿ (dos2)
# endif
	    x = SystemTags((ವರ್ಣ *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ಅನ್ಯಥಾ
	    x = Execute((ವರ್ಣ *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    ಯದಿ ((dos2 && x < 0) || (!dos2 && !x))
# ಅನ್ಯಥಾ
    ಯದಿ (x < 0)
# endif
    {
	ಸಂದೇಶ_puts(_("Cannot execute "));
	ಯದಿ (cmd == ರಿಕ್ತ)
	{
	    ಸಂದೇಶ_puts(_("shell "));
	    ಸಂದೇಶ_outtrans(p_sh);
	}
	ಅನ್ಯಥಾ
	    ಸಂದೇಶ_outtrans(cmd);
	ಸಂದೇಶ_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    ಅನ್ಯಥಾ ಯದಿ (!dos2 || x)
# ಅನ್ಯಥಾ
    ಅನ್ಯಥಾ ಯದಿ (x)
# endif
    {
	ಯದಿ ((x = IoErr()) != 0)
	{
	    ಯದಿ (!(options & SHELL_SILENT))
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_outnum((long)x);
		ಸಂದೇಶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#ಅನ್ಯಥಾ	// else part is for AZTEC_C
    ಯದಿ (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    ಅನ್ಯಥಾ
	use_execute = 0;
    ಯದಿ (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ಯದಿ (shellcmd == ರಿಕ್ತ)	    // out of memory, use Execute
	    use_execute = 1;
	ಅನ್ಯಥಾ
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ಯದಿ (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ಯದಿ (cmd == ರಿಕ್ತ)
    {
	ಯದಿ (use_execute)
	{
# ifdef FEAT_ARP
	    ಯದಿ (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    ಅನ್ಯಥಾ
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	ಅನ್ಯಥಾ
	    x = fexecl((ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellarg, ರಿಕ್ತ);
    }
    ಅನ್ಯಥಾ ಯದಿ (use_execute)
    {
# ifdef FEAT_ARP
	ಯದಿ (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ಅನ್ಯಥಾ
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    ಅನ್ಯಥಾ ಯದಿ (p_st & 1)
	x = fexecl((ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellarg,
							   (ವರ್ಣ *)cmd, ರಿಕ್ತ);
    ಅನ್ಯಥಾ
	x = fexecl((ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellcmd, (ವರ್ಣ *)shellarg,
					   (ವರ್ಣ *)p_shcf, (ವರ್ಣ *)cmd, ರಿಕ್ತ);
# ifdef FEAT_ARP
    ಯದಿ ((dos2 && x < 0) || (!dos2 && x))
# ಅನ್ಯಥಾ
    ಯದಿ (x < 0)
# endif
    {
	ಸಂದೇಶ_puts(_("Cannot execute "));
	ಯದಿ (use_execute)
	{
	    ಯದಿ (cmd == ರಿಕ್ತ)
		ಸಂದೇಶ_outtrans(p_sh);
	    ಅನ್ಯಥಾ
		ಸಂದೇಶ_outtrans(cmd);
	}
	ಅನ್ಯಥಾ
	{
	    ಸಂದೇಶ_puts(_("shell "));
	    ಸಂದೇಶ_outtrans(shellcmd);
	}
	ಸಂದೇಶ_putchar('\n');
	retval = -1;
    }
    ಅನ್ಯಥಾ
    {
	ಯದಿ (use_execute)
	{
# ifdef FEAT_ARP
	    ಯದಿ (!dos2 || x)
# ಅನ್ಯಥಾ
	    ಯದಿ (x)
# endif
		x = IoErr();
	}
	ಅನ್ಯಥಾ
	    x = wait();
	ಯದಿ (x)
	{
	    ಯದಿ (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_outnum((long)x);
		ಸಂದೇಶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    ಯದಿ ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    ಯದಿ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ಯದಿ (term_console)
	win_resize_on();		// window resize events activated
    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಙ್ಕ force UNUSED)
{
   ಯದಿ (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
ಶೂನ್ಯ __regargs __chkabort(ಶೂನ್ಯ);

ಶೂನ್ಯ __regargs __chkabort(ಶೂನ್ಯ)
{}

#ಅನ್ಯಥಾ
    long
Chk_Abort(ಶೂನ್ಯ)
{
    ಪ್ರತ್ಯಾಗಮ(0L);
}
#endif

/*
 * mch_expandpath() - this code does wild-card pattern matching using the arp
 *		      routines.
 *
 * "pat" has backslashes before chars that are not to be expanded.
 * Returns the number of matches found.
 *
 * This is based on WildDemo2.c (found in arp1.1 distribution).
 * That code's copyright follows:
 *	Copyright (c) 1987, Scott Ballantyne
 *	Use and abuse as you please.
 */

#ifdef __amigaos4__
# define	ANCHOR_BUF_SIZE	1024
#ಅನ್ಯಥಾ
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (ಪರಿಮಾಣ(ಸಂರಚನಾ AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    ಪೂರ್ಣಾಙ್ಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    ಪೂರ್ಣಾಙ್ಕ		flags)		// EW_* flags
{
    ಸಂರಚನಾ AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    ಪೂರ್ಣಾಙ್ಕ			start_len;
    ಪೂರ್ಣಾಙ್ಕ			matches;
#ifdef __amigaos4__
    ಸಂರಚನಾ TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#ಅನ್ಯಥಾ
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    ಯದಿ (Anchor == ರಿಕ್ತ)
	ಪ್ರತ್ಯಾಗಮ 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# ಅನ್ಯಥಾ
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    ಯದಿ (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ಯದಿ (starbuf == ರಿಕ್ತ)
	    ಗಚ್ಛ Return;
	ಪ್ರತಿ (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ಯದಿ (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    ಅನ್ಯಥಾ
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    ಅನ್ಯಥಾ
	Result = FindFirst((ವರ್ಣ *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    ಯಾವತ್ (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#ಅನ್ಯಥಾ
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	ಯದಿ (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	ಅನ್ಯಥಾ
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    ಯದಿ (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    ಅನ್ಯಥಾ ಯದಿ (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ಯದಿ (matches)
	qsort((ಶೂನ್ಯ *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, ಪರಿಮಾಣ(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    ಯದಿ (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    ಅನ್ಯಥಾ
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#ಅನ್ಯಥಾ
    vim_free(Anchor);
#endif

    ಪ್ರತ್ಯಾಗಮ matches;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಙ್ಕ
sortcmp(ಧ್ರುವ ಶೂನ್ಯ *a, ಧ್ರುವ ಶೂನ್ಯ *b)
{
    ವರ್ಣ *s = *(ವರ್ಣ **)a;
    ವರ್ಣ *t = *(ವರ್ಣ **)b;

    ಪ್ರತ್ಯಾಗಮ pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_has_exp_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ ಯದಿ (vim_strchr((char_u *)"*?[(#", *p) != ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

    ಪೂರ್ಣಾಙ್ಕ
mch_has_wildcard(char_u *p)
{
    ಪ್ರತಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಯದಿ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಅನ್ಯಥಾ
	    ಯದಿ (vim_strchr((char_u *)"*?[(#$`", *p) != ರಿಕ್ತ
		    || (*p == '~' && p[1] != NUL))
		ಪ್ರತ್ಯಾಗಮ TRUE;
    }
    ಪ್ರತ್ಯಾಗಮ FALSE;
}

/*
 * With AmigaDOS 2.0 support for reading local environment variables
 *
 * Two buffers are allocated:
 * - A big one to do the expansion into.  It is freed before returning.
 * - A small one to hold the return value.  It is kept until the next call.
 */
    char_u *
mch_getenv(char_u *var)
{
    ಪೂರ್ಣಾಙ್ಕ		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ಸ್ಥಿರ char_u   *alloced = ರಿಕ್ತ;	// allocated memory

#ifdef FEAT_ARP
    ಯದಿ (!dos2)
	retval = (char_u *)getenv((ವರ್ಣ *)var);
    ಅನ್ಯಥಾ
#endif
    {
	VIM_CLEAR(alloced);
	retval = ರಿಕ್ತ;

	buf = alloc(IOSIZE);
	ಯದಿ (buf == ರಿಕ್ತ)
	    ಪ್ರತ್ಯಾಗಮ ರಿಕ್ತ;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	ಯದಿ (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ಯದಿ (retval == ರಿಕ್ತ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ಪ್ರತ್ಯಾಗಮ retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    ಪೂರ್ಣಾಙ್ಕ
mch_setenv(ವರ್ಣ *var, ವರ್ಣ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಙ್ಕ x UNUSED)
{
#ifdef FEAT_ARP
    ಯದಿ (!dos2)
	ಪ್ರತ್ಯಾಗಮ setenv(var, ಮೌಲ್ಯ);
#endif

    ಯದಿ (SetVar((UBYTE *)var, (UBYTE *)ಮೌಲ್ಯ, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ಪ್ರತ್ಯಾಗಮ 0;   // success
    ಪ್ರತ್ಯಾಗಮ -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    ಪೂರ್ಣಾಙ್ಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಙ್ಕ len)
{
    ಸಂರಚನಾ Process *proc = (ಸಂರಚನಾ Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    ಪೂರ್ಣಾಙ್ಕ status;

    ಯದಿ (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    ಅನ್ಯಥಾ
	status = OK;

    Close(fh);
    ಪ್ರತ್ಯಾಗಮ status;
}
