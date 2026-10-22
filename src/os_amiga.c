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

#ଅନ୍ତର୍ଭାଵ "vim.h"
#ଅନ୍ତର୍ଭାଵ "version.h"

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

#ଅନ୍ତର୍ଭାଵ <exec/memory.h>

#ଅନ୍ତର୍ଭାଵ <dos/dostags.h>	    // for 2.0 functions
#ଅନ୍ତର୍ଭାଵ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#ଅନ୍ୟଥା
# define	free_fib(x) vim_free(fib)
#endif

#ଯଦି defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ଯଦି defined(__amigaos4__)
ସ୍ଥିର ଅଚଳ ଅକ୍ଷର* __attribute__((used)) stackcookie = "$STACK: 1048576";
#ନଚେତଯଦି defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#ନଚେତଯଦି defined(__GNUC__) && defined(AMIGA)
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
#ନଚେତଯଦି !defined(AZTEC_C) && !defined(__AROS__)
ସ୍ଥିର long dos_packet(ସଂରଚନା ସନ୍ଦେଶ_Port *, long, long);
#endif
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା lock2name(BPTR lock, char_u *buf, long   len);
ସ୍ଥିର ଶୂନ୍ୟ out_ଅଙ୍କ(long n);
ସ୍ଥିର ସଂରଚନା FileInfoBlock *get_fib(char_u *);
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା sortcmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ସ୍ଥିର BPTR
safe_Lock(UBYTE *name, long mode)
{
    ସଂରଚନା Process *me = (ସଂରଚନା Process *)FindTask(ଖାଲି);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    ଫେରନ୍ତୁ flock;
}

ସ୍ଥିର BPTR		raw_in = (BPTR)ଖାଲି;
ସ୍ଥିର BPTR		raw_out = (BPTR)ଖାଲି;
ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ସଂରଚନା IntuitionBase	*IntuitionBase = ଖାଲି;
#endif
#ifdef FEAT_ARP
ସଂରଚନା ArpBase		*ArpBase = ଖାଲି;
#endif

ସ୍ଥିର ସଂରଚନା Window	*wb_window;
ସ୍ଥିର char_u		*oldwindowtitle = ଖାଲି;

#ifdef FEAT_ARP
ପୂର୍ଣ୍ଣସଂଖ୍ୟା			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
ପୂର୍ଣ୍ଣସଂଖ୍ୟା			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
ସ୍ଥିର ଅକ୍ଷର version[] __attribute__((used)) =
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

    ଶୂନ୍ୟ
win_resize_on(ଶୂନ୍ୟ)
{
    OUT_STR_NF("\033[12{");
}

    ଶୂନ୍ୟ
win_resize_off(ଶୂନ୍ୟ)
{
    OUT_STR_NF("\033[12}");
}

    ଶୂନ୍ୟ
mch_write(char_u *p, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    Write(raw_out, (ଅକ୍ଷର *)p, (long)len);
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_inchar(
    char_u  *buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    maxlen,
    long    time,		// milliseconds
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    tb_change_cnt UNUSED)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା	    len;
    long    utime;

    ଯଦି (time >= 0)
    {
	ଯଦି (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	ଅନ୍ୟଥା
	    utime = time * 1000L;   // convert from milli to micro secs
	ଯଦି (WaitForChar(raw_in, utime) == 0)	// no character available
	    ଫେରନ୍ତୁ 0;
    }
    ଅନ୍ୟଥା    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ଯଦି (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    ଯଦି (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ପୂର୍ଣ୍ଣସଂଖ୍ୟା)KE_CURSORHOLD;
		ଫେରନ୍ତୁ 3;
	    }
	    before_blocking();
	}
    }

    ପାଇଁ (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (ଅକ୍ଷର *)buf, (long)maxlen / ନିବେଶ_conv.vc_factor);
	ଯଦି (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ଯଦି (ନିବେଶ_conv.vc_type != CONV_NONE)
		len = convert_ନିବେଶ(buf, len, maxlen);
	    ଫେରନ୍ତୁ len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_char_avail(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(ପୂର୍ଣ୍ଣସଂଖ୍ୟା special)
{
#ଯଦି defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ଫେରନ୍ତୁ (long_u)AvailMem(MEMF_ANY) >> 10;
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    ଶୂନ୍ୟ
mch_delay(long msec, ପୂର୍ଣ୍ଣସଂଖ୍ୟା flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    ଶୂନ୍ୟ	    Delay(long);
#endif

    ଯଦି (msec <= 0)
	ଫେରନ୍ତୁ;

    ଯଦି (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    ଅନ୍ୟଥା
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    ଶୂନ୍ୟ
mch_suspend(ଶୂନ୍ୟ)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    ଶୂନ୍ୟ
mch_init(ଶୂନ୍ୟ)
{
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ସ୍ଥିର ଅକ୍ଷର	    intlibname[] = "intuition.library";
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
	ସଂରଚନା Process *me = (ସଂରଚନା Process *)FindTask(ଖାଲି);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ଯଦି (raw_in == (BPTR)ଖାଲି)
    {
	raw_in = ନିବେଶ_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ଯଦି (!IsInteractive(raw_in))
	    raw_in = raw_out;
	ଅନ୍ୟଥା ଯଦି (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = ଖାଲି;
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ଯଦି ((IntuitionBase = (ସଂରଚନା IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ଖାଲି)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#ଅନ୍ତର୍ଭାଵ <workbench/startup.h>

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

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_check_win(ପୂର୍ଣ୍ଣସଂଖ୍ୟା argc, ଅକ୍ଷର **argv)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    ସ୍ଥିର char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ସ୍ଥିର char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ସଂରଚନା WBArg    *argp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    ac;
    ଅକ୍ଷର	    *av;
    char_u	    *device = ଖାଲି;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    exitval = 4;
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ସଂରଚନା Library  *DosBase;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    ଯଦି (DosBase != ଖାଲି)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    ଅନ୍ୟଥା	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# ଅନ୍ୟଥା
		    // need arp functions for dos 1.x
	ଯଦି (!(ArpBase = (ସଂରଚନା ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    ପାଇଁ (i = 1; i < argc; ++i)
	ଯଦି (argv[i][0] == '-')
	{
	    ବାଛ (argv[i][1])
	    {
	    ସ୍ଥିତି 'f':
		usewin = TRUE;
		ଭାଙ୍ଗ;

	    ସ୍ଥିତି 'd':
		ଯଦି (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		ଭାଙ୍ଗ;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ଯଦି (argc != 0
	    && device == ଖାଲି
	    && (IsInteractive(ନିବେଶ_3()) || IsInteractive(Output())))
	ଫେରନ୍ତୁ OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ଯଦି (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	ପାଇଁ (i = -1; i < 3; ++i)
	{
	    ଯଦି (i >= 0)
		device = constrings[i];
	    ଯଦି (device != ଖାଲି && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)ଖାଲି)
		ଭାଙ୍ଗ;
	}
	ଯଦି (raw_in == (BPTR)ଖାଲି)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    ଯାଅ exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	ଫେରନ୍ତୁ OK;
    }

    ଯଦି ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)ଖାଲି)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	ଯାଅ exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ଯଦି !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((ଅକ୍ଷର *)buf1, "t:nc%p", FindTask(0));
#ଅନ୍ୟଥା
    sprintf((ଅକ୍ଷର *)buf1, "t:nc%ld", (long)buf1);
#endif
    ଯଦି ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)ଖାଲି)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((ଅକ୍ଷର *)buf1);
	mch_errmsg("\n");
	ଯାଅ exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ଯଦି (argc == 0)	// run from workbench
	ac = ((ସଂରଚନା WBStartup *)argv)->sm_ଅଙ୍କ_Args;
    ଅନ୍ୟଥା
	ac = argc;
    ପାଇଁ (i = 0; i < ac; ++i)
    {
	ଯଦି (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ସଂରଚନା WBStartup *)argv)->sm_ArgList[i]);
	    ଯଦି (argp->wa_Lock)
		(ଶୂନ୍ୟ)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    ଯଦି (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    ଅନ୍ୟଥା	    // use arp function
		TackOn((ଅକ୍ଷର *)buf2, argp->wa_Name);
#endif
	    av = (ଅକ୍ଷର *)buf2;
	}
	ଅନ୍ୟଥା
	    av = argv[i];

	// skip '-d' or "-dev" option
	ଯଦି (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    ଜାରି;
	}
	ଯଦି (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	ଯଦି (vim_strchr((char_u *)av, ' '))
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
    ପାଇଁ (i = -1; i < 3; ++i)
    {
	ଯଦି (i >= 0)
	    device = constrings[i];
	ଅନ୍ୟଥା ଯଦି (device == ଖାଲି)
	    ଜାରି;
	sprintf((ଅକ୍ଷର *)buf2, "newcli <nil: >nil: %s from %s", (ଅକ୍ଷର *)device, (ଅକ୍ଷର *)buf1);
#ifdef FEAT_ARP
	ଯଦି (dos2)
	{
#endif
	    ଯଦି (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		ଭାଙ୍ଗ;
#ifdef FEAT_ARP
	}
	ଅନ୍ୟଥା
	{
	    ଯଦି (Execute((UBYTE *)buf2, nilfh, nilfh))
		ଭାଙ୍ଗ;
	}
#endif
    }
    ଯଦି (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	ଯାଅ exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    ଯଦି (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    ଯଦି (ArpBase)
	CloseLibrary((ସଂରଚନା Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_ନିବେଶ_isatty(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    ଶୂନ୍ୟ
fname_case(
    char_u	*name,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len UNUSED)		// buffer size, ignored here
{
    ସଂରଚନା FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ଯଦି (fib == ଖାଲି)
	ଫେରନ୍ତୁ;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    ଯଦି (fib->fib_DirEntryType == ST_ମୂଳ)
	strcat(fib->fib_FileName, ":");
#endif
    ଯଦି (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ସ୍ଥିର ସଂରଚନା FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ସଂରଚନା FileInfoBlock    *fib;

    ଯଦି (fname == ଖାଲି)	    // safety check
	ଫେରନ୍ତୁ ଖାଲି;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#ଅନ୍ୟଥା
    fib = ALLOC_ONE(ସଂରଚନା FileInfoBlock);
#endif
    ଯଦି (fib == ଖାଲି)
	ଫେରନ୍ତୁ;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    ଯଦି (flock == (BPTR)ଖାଲି || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ଖାଲି;
    }
    ଯଦି (flock)
	UnLock(flock);
    ଫେରନ୍ତୁ fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    ଶୂନ୍ୟ
mch_settitle(char_u *title, char_u *icon)
{
    ଯଦି (wb_window != ଖାଲି && title != ଖାଲି)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    ଶୂନ୍ୟ
mch_restore_title(ପୂର୍ଣ୍ଣସଂଖ୍ୟା which)
{
    ଯଦି (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_title(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ (wb_window != ଖାଲି);
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_restore_icon(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ FALSE;
}

    ଶୂନ୍ୟ
mch_setmouse(ପୂର୍ଣ୍ଣସଂଖ୍ୟା on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_user_name(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ଯଦି defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ସଂରଚନା passwd   *pwd = getpwuid(getuid());

    ଯଦି (pwd != ଖାଲି && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ଫେରନ୍ତୁ OK;
    }
#endif
    *s = NUL;
    ଫେରନ୍ତୁ FAIL;
}

/*
 * Insert host name is s[len].
 */
    ଶୂନ୍ୟ
mch_get_host_name(char_u *s, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
#ଯଦି defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((ଅକ୍ଷର *)s, len);
#ଅନ୍ୟଥା
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(ଶୂନ୍ୟ)
{
#ଯଦି defined(__amigaos4__)
    ଫେରନ୍ତୁ (long) getpid();
#ନଚେତଯଦି defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ଫେରନ୍ତୁ((long)FindTask(0));
#ଅନ୍ୟଥା
    ଫେରନ୍ତୁ (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_dirname(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ଫେରନ୍ତୁ mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		len,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		force)
{
    BPTR	l;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = FAIL;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		i;

    // Lock the file.  If it exists, we can get the exact name.
    ଯଦି ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    ଅନ୍ୟଥା ଯଦି (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ଯଦି ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)ଖାଲି)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    ଯଦି (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ଯଦି (i == 0 || *fname != NUL)
		{
		    ଯଦି (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ଯଦି (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    ଫେରନ୍ତୁ retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isFullName(char_u *fname)
{
    ଫେରନ୍ତୁ (vim_strchr(fname, ':') != ଖାଲି && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    ଯଦି (dos2)		    // use 2.0 function
#endif
	ଫେରନ୍ତୁ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    ଅନ୍ୟଥା		// use arp function
	ଫେରନ୍ତୁ ((ପୂର୍ଣ୍ଣସଂଖ୍ୟା)PathName(lock, (ଅକ୍ଷର *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    ସଂରଚନା FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    ଯଦି (fib == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ଫେରନ୍ତୁ retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ଫେରନ୍ତୁ (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    ଶୂନ୍ୟ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_isdir(char_u *name)
{
    ସଂରଚନା FileInfoBlock    *fib;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			    retval = FALSE;

    fib = get_fib(name);
    ଯଦି (fib == ଖାଲି)
	ଫେରନ୍ତୁ FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#ଅନ୍ୟଥା
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    ଫେରନ୍ତୁ retval;
}

/*
 * Create directory "name".
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    ଯଦି (lock == ଖାଲି)
	ଫେରନ୍ତୁ -1;

    UnLock(lock);
    ଫେରନ୍ତୁ 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_can_exe(char_u *name, char_u **path UNUSED, ପୂର୍ଣ୍ଣସଂଖ୍ୟା use_path)
{
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    ଯଦି (seg && GetSegListInfoTags(seg, GSLI_Native, ଖାଲି, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, ଖାଲି, TAG_DONE))
    {
	// Test if file permissions allow execution.
	ସଂରଚନା ExamineData *exd = ExamineObjectTags(EX_String_Name_ନିବେଶ, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    ଅନ୍ୟଥା
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    ଯଦି (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ସଂରଚନା Path_ଗଣ୍ଠି *head = DupCmdPathList(ଖାଲି);

	// For each entry, recur to check for executable.
	ପାଇଁ (ସଂରଚନା Path_ଗଣ୍ଠି *tail = head; !exe && tail;
			       tail = (ସଂରଚନା Path_ଗଣ୍ଠି *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    ଫେରନ୍ତୁ exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    ଫେରନ୍ତୁ ଗଣ୍ଠି_NORMAL;
}

    ଶୂନ୍ୟ
mch_early_init(ଶୂନ୍ୟ)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    ଶୂନ୍ୟ
mch_exit(ପୂର୍ଣ୍ଣସଂଖ୍ୟା r)
{
    exiting = TRUE;

    ଯଦି (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    ଯଦି (raw_out)
    {
	ଯଦି (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ଯଦି (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    ଯଦି (ArpBase)
	CloseLibrary((ସଂରଚନା Library *) ArpBase);
#endif
    ଯଦି (close_win)
	Close(raw_in);
    ଯଦି (r)
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

#define MP(xx)	((ସଂରଚନା ସନ୍ଦେଶ_Port *)((ସଂରଚନା FileHandle *) (BADDR(xx)))->fh_Type)

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
    ଶୂନ୍ୟ
mch_settmode(tmode_T tmode)
{
#ଯଦି defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ଯଦି (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#ଅନ୍ୟଥା
    ଯଦି (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#ଅନ୍ତର୍ଭାଵ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ଯଦି defined(__AROS__) || defined(__MORPHOS__)
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
{
    ଯଦି (!term_console)
	ଫେରନ୍ତୁ FAIL;

    ଯଦି (raw_in && raw_out)
    {
	// Save current console mode.
	ପୂର୍ଣ୍ଣସଂଖ୍ୟା old_tmode = cur_tmode;
	ଅକ୍ଷର ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ଯଦି (Write(raw_out, ctrl, ଆକାର(ctrl)) == ଆକାର(ctrl))
	{
	    ଅକ୍ଷର scan[] = "\x9b""1;1;%d;%d r",
		 answ[ଆକାର(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    ଯଦି (Read(raw_in, answ, ଆକାର(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ଯଦି (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ଫେରନ୍ତୁ OK;
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

    ଫେରନ୍ତୁ FAIL;
}
#ଅନ୍ୟଥା
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_shellsize(ଶୂନ୍ୟ)
{
    ସଂରଚନା ConUnit  *conUnit;
# ifndef __amigaos4__
    ଅକ୍ଷର	    id_a[ଆକାର(ସଂରଚନା InfoData) + 3];
# endif
    ସଂରଚନା InfoData *id=0;

    ଯଦି (!term_console)	// not an amiga window
	ଯାଅ out;

    // insure longword alignment
# ifdef __amigaos4__
    ଯଦି (!(id = AllocDosObject(DOS_INFODATA, 0)))
	ଯାଅ out;
# ଅନ୍ୟଥା
    id = (ସଂରଚନା InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ଯଦି (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    ଯଦି (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (ସଂରଚନା Window *)id->id_Volume_ଗଣ୍ଠି) == ଖାଲି)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	ଯାଅ out;
    }
    ଯଦି (oldwindowtitle == ଖାଲି)
	oldwindowtitle = (char_u *)wb_window->Title;
    ଯଦି (id->id_InUse == (BPTR)ଖାଲି)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ଫେରନ୍ତୁ FAIL;
    }
    conUnit = (ସଂରଚନା ConUnit *) ((ସଂରଚନା IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ଯଦି (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	ଫେରନ୍ତୁ FAIL;
    }

    ଫେରନ୍ତୁ OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ଫେରନ୍ତୁ FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    ଶୂନ୍ୟ
mch_set_shellsize(ଶୂନ୍ୟ)
{
    ଯଦି (!term_console)
	ଫେରନ୍ତୁ;

    size_set = TRUE;
    out_char(CSI);
    out_ଅଙ୍କ((long)Rows);
    out_char('t');
    out_char(CSI);
    out_ଅଙ୍କ((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    ଶୂନ୍ୟ
mch_new_shellsize(ଶୂନ୍ୟ)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ସ୍ଥିର ଶୂନ୍ୟ
out_ଅଙ୍କ(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#ଯଦି !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    ସ୍ଥିର long
dos_packet(
    ସଂରଚନା ସନ୍ଦେଶ_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    ସଂରଚନା ସନ୍ଦେଶ_Port	    *replyport;
    ସଂରଚନା StandardPacket   *packet;
    long		    res1;

    ଯଦି (dos2)
# endif
	ଫେରନ୍ତୁ DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ସଂରଚନା ସନ୍ଦେଶ_Port *) CreatePort(ଖାଲି, 0);	// use arp function
    ଯଦି (!replyport)
	ଫେରନ୍ତୁ (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ସଂରଚନା StandardPacket *)
	AllocMem((long) ଆକାର(ସଂରଚନା StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    ଯଦି (!packet)
    {
	DeletePort(replyport);
	ଫେରନ୍ତୁ (0);
    }
    packet->sp_ସନ୍ଦେଶ.mn_ଗଣ୍ଠି.ln_Name = (ଅକ୍ଷର *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_ସନ୍ଦେଶ);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_ସନ୍ଦେଶ(pid, (ସଂରଚନା Message *)packet);	// send packet

    WaitPort(replyport);
    Get_ସନ୍ଦେଶ(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) ଆକାର(ସଂରଚନା StandardPacket));
    DeletePort(replyport);

    ଫେରନ୍ତୁ (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_call_shell(
    char_u	*cmd,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		x;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		tmode = cur_tmode;
#ifdef AZTEC_C
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		use_execute;
    char_u	*shellcmd = ଖାଲି;
    char_u	*shellarg;
#endif
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		retval = 0;

    ଯଦି (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ଫେରନ୍ତୁ -1;
    }

    ଯଦି (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    ଯଦି (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#ଯଦି !defined(AZTEC_C)		    // not tested very much
    ଯଦି (cmd == ଖାଲି)
    {
# ifdef FEAT_ARP
	ଯଦି (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ଅନ୍ୟଥା
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    ଅନ୍ୟଥା
    {
# ifdef FEAT_ARP
	ଯଦି (dos2)
# endif
	    x = SystemTags((ଅକ୍ଷର *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ଅନ୍ୟଥା
	    x = Execute((ଅକ୍ଷର *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    ଯଦି ((dos2 && x < 0) || (!dos2 && !x))
# ଅନ୍ୟଥା
    ଯଦି (x < 0)
# endif
    {
	ସନ୍ଦେଶ_puts(_("Cannot execute "));
	ଯଦି (cmd == ଖାଲି)
	{
	    ସନ୍ଦେଶ_puts(_("shell "));
	    ସନ୍ଦେଶ_outtrans(p_sh);
	}
	ଅନ୍ୟଥା
	    ସନ୍ଦେଶ_outtrans(cmd);
	ସନ୍ଦେଶ_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    ଅନ୍ୟଥା ଯଦି (!dos2 || x)
# ଅନ୍ୟଥା
    ଅନ୍ୟଥା ଯଦି (x)
# endif
    {
	ଯଦି ((x = IoErr()) != 0)
	{
	    ଯଦି (!(options & SHELL_SILENT))
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ସନ୍ଦେଶ_outnum((long)x);
		ସନ୍ଦେଶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#ଅନ୍ୟଥା	// else part is for AZTEC_C
    ଯଦି (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    ଅନ୍ୟଥା
	use_execute = 0;
    ଯଦି (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ଯଦି (shellcmd == ଖାଲି)	    // out of memory, use Execute
	    use_execute = 1;
	ଅନ୍ୟଥା
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ଯଦି (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ଯଦି (cmd == ଖାଲି)
    {
	ଯଦି (use_execute)
	{
# ifdef FEAT_ARP
	    ଯଦି (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    ଅନ୍ୟଥା
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	ଅନ୍ୟଥା
	    x = fexecl((ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellarg, ଖାଲି);
    }
    ଅନ୍ୟଥା ଯଦି (use_execute)
    {
# ifdef FEAT_ARP
	ଯଦି (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	ଅନ୍ୟଥା
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    ଅନ୍ୟଥା ଯଦି (p_st & 1)
	x = fexecl((ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellarg,
							   (ଅକ୍ଷର *)cmd, ଖାଲି);
    ଅନ୍ୟଥା
	x = fexecl((ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellcmd, (ଅକ୍ଷର *)shellarg,
					   (ଅକ୍ଷର *)p_shcf, (ଅକ୍ଷର *)cmd, ଖାଲି);
# ifdef FEAT_ARP
    ଯଦି ((dos2 && x < 0) || (!dos2 && x))
# ଅନ୍ୟଥା
    ଯଦି (x < 0)
# endif
    {
	ସନ୍ଦେଶ_puts(_("Cannot execute "));
	ଯଦି (use_execute)
	{
	    ଯଦି (cmd == ଖାଲି)
		ସନ୍ଦେଶ_outtrans(p_sh);
	    ଅନ୍ୟଥା
		ସନ୍ଦେଶ_outtrans(cmd);
	}
	ଅନ୍ୟଥା
	{
	    ସନ୍ଦେଶ_puts(_("shell "));
	    ସନ୍ଦେଶ_outtrans(shellcmd);
	}
	ସନ୍ଦେଶ_putchar('\n');
	retval = -1;
    }
    ଅନ୍ୟଥା
    {
	ଯଦି (use_execute)
	{
# ifdef FEAT_ARP
	    ଯଦି (!dos2 || x)
# ଅନ୍ୟଥା
	    ଯଦି (x)
# endif
		x = IoErr();
	}
	ଅନ୍ୟଥା
	    x = wait();
	ଯଦି (x)
	{
	    ଯଦି (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		ସନ୍ଦେଶ_putchar('\n');
		ସନ୍ଦେଶ_outnum((long)x);
		ସନ୍ଦେଶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    ଯଦି ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    ଯଦି (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ଯଦି (term_console)
	win_resize_on();		// window resize events activated
    ଫେରନ୍ତୁ retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    ଶୂନ୍ୟ
mch_breakcheck(ପୂର୍ଣ୍ଣସଂଖ୍ୟା force UNUSED)
{
   ଯଦି (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
ଶୂନ୍ୟ __regargs __chkabort(ଶୂନ୍ୟ);

ଶୂନ୍ୟ __regargs __chkabort(ଶୂନ୍ୟ)
{}

#ଅନ୍ୟଥା
    long
Chk_Abort(ଶୂନ୍ୟ)
{
    ଫେରନ୍ତୁ(0L);
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
#ଅନ୍ୟଥା
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (ଆକାର(ସଂରଚନା AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		flags)		// EW_* flags
{
    ସଂରଚନା AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			start_len;
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା			matches;
#ifdef __amigaos4__
    ସଂରଚନା TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#ଅନ୍ୟଥା
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    ଯଦି (Anchor == ଖାଲି)
	ଫେରନ୍ତୁ 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# ଅନ୍ୟଥା
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    ଯଦି (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ଯଦି (starbuf == ଖାଲି)
	    ଯାଅ Return;
	ପାଇଁ (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ଯଦି (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    ଅନ୍ୟଥା
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    ଅନ୍ୟଥା
	Result = FindFirst((ଅକ୍ଷର *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    ଯେତେବେଳେ (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#ଅନ୍ୟଥା
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	ଯଦି (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	ଅନ୍ୟଥା
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    ଯଦି (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    ଅନ୍ୟଥା ଯଦି (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ଯଦି (matches)
	qsort((ଶୂନ୍ୟ *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, ଆକାର(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    ଯଦି (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    ଅନ୍ୟଥା
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#ଅନ୍ୟଥା
    vim_free(Anchor);
#endif

    ଫେରନ୍ତୁ matches;
}

    ସ୍ଥିର ପୂର୍ଣ୍ଣସଂଖ୍ୟା
sortcmp(ଅଚଳ ଶୂନ୍ୟ *a, ଅଚଳ ଶୂନ୍ୟ *b)
{
    ଅକ୍ଷର *s = *(ଅକ୍ଷର **)a;
    ଅକ୍ଷର *t = *(ଅକ୍ଷର **)b;

    ଫେରନ୍ତୁ pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_exp_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା ଯଦି (vim_strchr((char_u *)"*?[(#", *p) != ଖାଲି)
	    ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
}

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_has_wildcard(char_u *p)
{
    ପାଇଁ ( ; *p; MB_PTR_ADV(p))
    {
	ଯଦି (*p == '\\' && p[1] != NUL)
	    ++p;
	ଅନ୍ୟଥା
	    ଯଦି (vim_strchr((char_u *)"*?[(#$`", *p) != ଖାଲି
		    || (*p == '~' && p[1] != NUL))
		ଫେରନ୍ତୁ TRUE;
    }
    ଫେରନ୍ତୁ FALSE;
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
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ସ୍ଥିର char_u   *alloced = ଖାଲି;	// allocated memory

#ifdef FEAT_ARP
    ଯଦି (!dos2)
	retval = (char_u *)getenv((ଅକ୍ଷର *)var);
    ଅନ୍ୟଥା
#endif
    {
	VIM_CLEAR(alloced);
	retval = ଖାଲି;

	buf = alloc(IOSIZE);
	ଯଦି (buf == ଖାଲି)
	    ଫେରନ୍ତୁ ଖାଲି;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	ଯଦି (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ଯଦି (retval == ଖାଲି && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ଫେରନ୍ତୁ retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_setenv(ଅକ୍ଷର *var, ଅକ୍ଷର *ମୂଲ୍ୟ, ପୂର୍ଣ୍ଣସଂଖ୍ୟା x UNUSED)
{
#ifdef FEAT_ARP
    ଯଦି (!dos2)
	ଫେରନ୍ତୁ setenv(var, ମୂଲ୍ୟ);
#endif

    ଯଦି (SetVar((UBYTE *)var, (UBYTE *)ମୂଲ୍ୟ, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ଫେରନ୍ତୁ 0;   // success
    ଫେରନ୍ତୁ -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    ପୂର୍ଣ୍ଣସଂଖ୍ୟା
mch_get_random(char_u *buf, ପୂର୍ଣ୍ଣସଂଖ୍ୟା len)
{
    ସଂରଚନା Process *proc = (ସଂରଚନା Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    ପୂର୍ଣ୍ଣସଂଖ୍ୟା status;

    ଯଦି (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    ଅନ୍ୟଥା
	status = OK;

    Close(fh);
    ଫେରନ୍ତୁ status;
}
