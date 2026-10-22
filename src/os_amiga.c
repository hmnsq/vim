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

#അന്തര്ഭാവ "vim.h"
#അന്തര്ഭാവ "version.h"

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

#അന്തര്ഭാവ <exec/memory.h>

#അന്തര്ഭാവ <dos/dostags.h>	    // for 2.0 functions
#അന്തര്ഭാവ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#അല്ലെങ്കിൽ
# define	free_fib(x) vim_free(fib)
#endif

#എങ്കിൽ defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#എങ്കിൽ defined(__amigaos4__)
സ്ഥിതം സ്ഥിരം അക്ഷരം* __attribute__((used)) stackcookie = "$STACK: 1048576";
#അല്ലെങ്കിലെങ്കിൽ defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#അല്ലെങ്കിലെങ്കിൽ defined(__GNUC__) && defined(AMIGA)
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
#അല്ലെങ്കിലെങ്കിൽ !defined(AZTEC_C) && !defined(__AROS__)
സ്ഥിതം long dos_packet(ഘടന സന്ദേശം_Port *, long, long);
#endif
സ്ഥിതം പൂർണ്ണസംഖ്യ lock2name(BPTR lock, char_u *buf, long   len);
സ്ഥിതം ശൂന്യം out_അക്കം(long n);
സ്ഥിതം ഘടന FileInfoBlock *get_fib(char_u *);
സ്ഥിതം പൂർണ്ണസംഖ്യ sortcmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    സ്ഥിതം BPTR
safe_Lock(UBYTE *name, long mode)
{
    ഘടന Process *me = (ഘടന Process *)FindTask(ശൂന്യ);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    തിരികെ flock;
}

സ്ഥിതം BPTR		raw_in = (BPTR)ശൂന്യ;
സ്ഥിതം BPTR		raw_out = (BPTR)ശൂന്യ;
സ്ഥിതം പൂർണ്ണസംഖ്യ		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ഘടന IntuitionBase	*IntuitionBase = ശൂന്യ;
#endif
#ifdef FEAT_ARP
ഘടന ArpBase		*ArpBase = ശൂന്യ;
#endif

സ്ഥിതം ഘടന Window	*wb_window;
സ്ഥിതം char_u		*oldwindowtitle = ശൂന്യ;

#ifdef FEAT_ARP
പൂർണ്ണസംഖ്യ			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
പൂർണ്ണസംഖ്യ			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
സ്ഥിതം അക്ഷരം version[] __attribute__((used)) =
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

    ശൂന്യം
win_resize_on(ശൂന്യം)
{
    OUT_STR_NF("\033[12{");
}

    ശൂന്യം
win_resize_off(ശൂന്യം)
{
    OUT_STR_NF("\033[12}");
}

    ശൂന്യം
mch_write(char_u *p, പൂർണ്ണസംഖ്യ len)
{
    Write(raw_out, (അക്ഷരം *)p, (long)len);
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
    പൂർണ്ണസംഖ്യ
mch_inchar(
    char_u  *buf,
    പൂർണ്ണസംഖ്യ	    maxlen,
    long    time,		// milliseconds
    പൂർണ്ണസംഖ്യ	    tb_change_cnt UNUSED)
{
    പൂർണ്ണസംഖ്യ	    len;
    long    utime;

    എങ്കിൽ (time >= 0)
    {
	എങ്കിൽ (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	അല്ലെങ്കിൽ
	    utime = time * 1000L;   // convert from milli to micro secs
	എങ്കിൽ (WaitForChar(raw_in, utime) == 0)	// no character available
	    തിരികെ 0;
    }
    അല്ലെങ്കിൽ    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	എങ്കിൽ (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    എങ്കിൽ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (പൂർണ്ണസംഖ്യ)KE_CURSORHOLD;
		തിരികെ 3;
	    }
	    before_blocking();
	}
    }

    വേണ്ടി (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (അക്ഷരം *)buf, (long)maxlen / നിവേശം_conv.vc_factor);
	എങ്കിൽ (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    എങ്കിൽ (നിവേശം_conv.vc_type != CONV_NONE)
		len = convert_നിവേശം(buf, len, maxlen);
	    തിരികെ len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    പൂർണ്ണസംഖ്യ
mch_char_avail(ശൂന്യം)
{
    തിരികെ (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(പൂർണ്ണസംഖ്യ special)
{
#എങ്കിൽ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    തിരികെ (long_u)AvailMem(MEMF_ANY) >> 10;
#അല്ലെങ്കിൽ
    തിരികെ (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    ശൂന്യം
mch_delay(long msec, പൂർണ്ണസംഖ്യ flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    ശൂന്യം	    Delay(long);
#endif

    എങ്കിൽ (msec <= 0)
	തിരികെ;

    എങ്കിൽ (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    അല്ലെങ്കിൽ
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    ശൂന്യം
mch_suspend(ശൂന്യം)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    ശൂന്യം
mch_init(ശൂന്യം)
{
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    സ്ഥിതം അക്ഷരം	    intlibname[] = "intuition.library";
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
	ഘടന Process *me = (ഘടന Process *)FindTask(ശൂന്യ);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    എങ്കിൽ (raw_in == (BPTR)ശൂന്യ)
    {
	raw_in = നിവേശം_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	എങ്കിൽ (!IsInteractive(raw_in))
	    raw_in = raw_out;
	അല്ലെങ്കിൽ എങ്കിൽ (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = ശൂന്യ;
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    എങ്കിൽ ((IntuitionBase = (ഘടന IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ശൂന്യ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#അന്തര്ഭാവ <workbench/startup.h>

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

    പൂർണ്ണസംഖ്യ
mch_check_win(പൂർണ്ണസംഖ്യ argc, അക്ഷരം **argv)
{
    പൂർണ്ണസംഖ്യ		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    സ്ഥിതം char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    സ്ഥിതം char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ഘടന WBArg    *argp;
    പൂർണ്ണസംഖ്യ		    ac;
    അക്ഷരം	    *av;
    char_u	    *device = ശൂന്യ;
    പൂർണ്ണസംഖ്യ		    exitval = 4;
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ഘടന Library  *DosBase;
#endif
    പൂർണ്ണസംഖ്യ		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    എങ്കിൽ (DosBase != ശൂന്യ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    അല്ലെങ്കിൽ	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# അല്ലെങ്കിൽ
		    // need arp functions for dos 1.x
	എങ്കിൽ (!(ArpBase = (ഘടന ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    വേണ്ടി (i = 1; i < argc; ++i)
	എങ്കിൽ (argv[i][0] == '-')
	{
	    തിരഞ്ഞെടുപ്പ് (argv[i][1])
	    {
	    അവസ്ഥ 'f':
		usewin = TRUE;
		നിർത്തുക;

	    അവസ്ഥ 'd':
		എങ്കിൽ (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		നിർത്തുക;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    എങ്കിൽ (argc != 0
	    && device == ശൂന്യ
	    && (IsInteractive(നിവേശം_3()) || IsInteractive(Output())))
	തിരികെ OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    എങ്കിൽ (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	വേണ്ടി (i = -1; i < 3; ++i)
	{
	    എങ്കിൽ (i >= 0)
		device = constrings[i];
	    എങ്കിൽ (device != ശൂന്യ && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)ശൂന്യ)
		നിർത്തുക;
	}
	എങ്കിൽ (raw_in == (BPTR)ശൂന്യ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    പോകുക exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	തിരികെ OK;
    }

    എങ്കിൽ ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)ശൂന്യ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	പോകുക exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#എങ്കിൽ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((അക്ഷരം *)buf1, "t:nc%p", FindTask(0));
#അല്ലെങ്കിൽ
    sprintf((അക്ഷരം *)buf1, "t:nc%ld", (long)buf1);
#endif
    എങ്കിൽ ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)ശൂന്യ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((അക്ഷരം *)buf1);
	mch_errmsg("\n");
	പോകുക exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    എങ്കിൽ (argc == 0)	// run from workbench
	ac = ((ഘടന WBStartup *)argv)->sm_അക്കം_Args;
    അല്ലെങ്കിൽ
	ac = argc;
    വേണ്ടി (i = 0; i < ac; ++i)
    {
	എങ്കിൽ (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ഘടന WBStartup *)argv)->sm_ArgList[i]);
	    എങ്കിൽ (argp->wa_Lock)
		(ശൂന്യം)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    എങ്കിൽ (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    അല്ലെങ്കിൽ	    // use arp function
		TackOn((അക്ഷരം *)buf2, argp->wa_Name);
#endif
	    av = (അക്ഷരം *)buf2;
	}
	അല്ലെങ്കിൽ
	    av = argv[i];

	// skip '-d' or "-dev" option
	എങ്കിൽ (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    തുടരുക;
	}
	എങ്കിൽ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	എങ്കിൽ (vim_strchr((char_u *)av, ' '))
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
    വേണ്ടി (i = -1; i < 3; ++i)
    {
	എങ്കിൽ (i >= 0)
	    device = constrings[i];
	അല്ലെങ്കിൽ എങ്കിൽ (device == ശൂന്യ)
	    തുടരുക;
	sprintf((അക്ഷരം *)buf2, "newcli <nil: >nil: %s from %s", (അക്ഷരം *)device, (അക്ഷരം *)buf1);
#ifdef FEAT_ARP
	എങ്കിൽ (dos2)
	{
#endif
	    എങ്കിൽ (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		നിർത്തുക;
#ifdef FEAT_ARP
	}
	അല്ലെങ്കിൽ
	{
	    എങ്കിൽ (Execute((UBYTE *)buf2, nilfh, nilfh))
		നിർത്തുക;
	}
#endif
    }
    എങ്കിൽ (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	പോകുക exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    എങ്കിൽ (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    എങ്കിൽ (ArpBase)
	CloseLibrary((ഘടന Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    തിരികെ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    പൂർണ്ണസംഖ്യ
mch_നിവേശം_isatty(ശൂന്യം)
{
    തിരികെ TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    ശൂന്യം
fname_case(
    char_u	*name,
    പൂർണ്ണസംഖ്യ		len UNUSED)		// buffer size, ignored here
{
    ഘടന FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    എങ്കിൽ (fib == ശൂന്യ)
	തിരികെ;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    എങ്കിൽ (fib->fib_DirEntryType == ST_വേര്)
	strcat(fib->fib_FileName, ":");
#endif
    എങ്കിൽ (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    സ്ഥിതം ഘടന FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ഘടന FileInfoBlock    *fib;

    എങ്കിൽ (fname == ശൂന്യ)	    // safety check
	തിരികെ ശൂന്യ;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#അല്ലെങ്കിൽ
    fib = ALLOC_ONE(ഘടന FileInfoBlock);
#endif
    എങ്കിൽ (fib == ശൂന്യ)
	തിരികെ;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    എങ്കിൽ (flock == (BPTR)ശൂന്യ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ശൂന്യ;
    }
    എങ്കിൽ (flock)
	UnLock(flock);
    തിരികെ fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    ശൂന്യം
mch_settitle(char_u *title, char_u *icon)
{
    എങ്കിൽ (wb_window != ശൂന്യ && title != ശൂന്യ)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    ശൂന്യം
mch_restore_title(പൂർണ്ണസംഖ്യ which)
{
    എങ്കിൽ (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
mch_can_restore_title(ശൂന്യം)
{
    തിരികെ (wb_window != ശൂന്യ);
}

    പൂർണ്ണസംഖ്യ
mch_can_restore_icon(ശൂന്യം)
{
    തിരികെ FALSE;
}

    ശൂന്യം
mch_setmouse(പൂർണ്ണസംഖ്യ on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    പൂർണ്ണസംഖ്യ
mch_get_user_name(char_u *s, പൂർണ്ണസംഖ്യ len)
{
#എങ്കിൽ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ഘടന passwd   *pwd = getpwuid(getuid());

    എങ്കിൽ (pwd != ശൂന്യ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	തിരികെ OK;
    }
#endif
    *s = NUL;
    തിരികെ FAIL;
}

/*
 * Insert host name is s[len].
 */
    ശൂന്യം
mch_get_host_name(char_u *s, പൂർണ്ണസംഖ്യ len)
{
#എങ്കിൽ defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((അക്ഷരം *)s, len);
#അല്ലെങ്കിൽ
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(ശൂന്യം)
{
#എങ്കിൽ defined(__amigaos4__)
    തിരികെ (long) getpid();
#അല്ലെങ്കിലെങ്കിൽ defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    തിരികെ((long)FindTask(0));
#അല്ലെങ്കിൽ
    തിരികെ (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    പൂർണ്ണസംഖ്യ
mch_dirname(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    തിരികെ mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    പൂർണ്ണസംഖ്യ		len,
    പൂർണ്ണസംഖ്യ		force)
{
    BPTR	l;
    പൂർണ്ണസംഖ്യ		retval = FAIL;
    പൂർണ്ണസംഖ്യ		i;

    // Lock the file.  If it exists, we can get the exact name.
    എങ്കിൽ ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	എങ്കിൽ ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)ശൂന്യ)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    എങ്കിൽ (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		എങ്കിൽ (i == 0 || *fname != NUL)
		{
		    എങ്കിൽ (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    എങ്കിൽ (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    തിരികെ retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    പൂർണ്ണസംഖ്യ
mch_isFullName(char_u *fname)
{
    തിരികെ (vim_strchr(fname, ':') != ശൂന്യ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    സ്ഥിതം പൂർണ്ണസംഖ്യ
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    എങ്കിൽ (dos2)		    // use 2.0 function
#endif
	തിരികെ ((പൂർണ്ണസംഖ്യ)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    അല്ലെങ്കിൽ		// use arp function
	തിരികെ ((പൂർണ്ണസംഖ്യ)PathName(lock, (അക്ഷരം *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    ഘടന FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    എങ്കിൽ (fib == ശൂന്യ)
	തിരികെ -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    തിരികെ retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    തിരികെ (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    ശൂന്യം
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    പൂർണ്ണസംഖ്യ
mch_isdir(char_u *name)
{
    ഘടന FileInfoBlock    *fib;
    പൂർണ്ണസംഖ്യ			    retval = FALSE;

    fib = get_fib(name);
    എങ്കിൽ (fib == ശൂന്യ)
	തിരികെ FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#അല്ലെങ്കിൽ
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    തിരികെ retval;
}

/*
 * Create directory "name".
 */
    പൂർണ്ണസംഖ്യ
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    എങ്കിൽ (lock == ശൂന്യ)
	തിരികെ -1;

    UnLock(lock);
    തിരികെ 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    പൂർണ്ണസംഖ്യ
mch_can_exe(char_u *name, char_u **path UNUSED, പൂർണ്ണസംഖ്യ use_path)
{
    പൂർണ്ണസംഖ്യ exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    എങ്കിൽ (seg && GetSegListInfoTags(seg, GSLI_Native, ശൂന്യ, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, ശൂന്യ, TAG_DONE))
    {
	// Test if file permissions allow execution.
	ഘടന ExamineData *exd = ExamineObjectTags(EX_String_Name_നിവേശം, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    അല്ലെങ്കിൽ
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    എങ്കിൽ (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ഘടന Path_കണ്ണി *head = DupCmdPathList(ശൂന്യ);

	// For each entry, recur to check for executable.
	വേണ്ടി (ഘടന Path_കണ്ണി *tail = head; !exe && tail;
			       tail = (ഘടന Path_കണ്ണി *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    തിരികെ exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    പൂർണ്ണസംഖ്യ
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    തിരികെ കണ്ണി_NORMAL;
}

    ശൂന്യം
mch_early_init(ശൂന്യം)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    ശൂന്യം
mch_exit(പൂർണ്ണസംഖ്യ r)
{
    exiting = TRUE;

    എങ്കിൽ (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    എങ്കിൽ (raw_out)
    {
	എങ്കിൽ (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    എങ്കിൽ (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    എങ്കിൽ (ArpBase)
	CloseLibrary((ഘടന Library *) ArpBase);
#endif
    എങ്കിൽ (close_win)
	Close(raw_in);
    എങ്കിൽ (r)
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

#define MP(xx)	((ഘടന സന്ദേശം_Port *)((ഘടന FileHandle *) (BADDR(xx)))->fh_Type)

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
    ശൂന്യം
mch_settmode(tmode_T tmode)
{
#എങ്കിൽ defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    എങ്കിൽ (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#അല്ലെങ്കിൽ
    എങ്കിൽ (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#അന്തര്ഭാവ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#എങ്കിൽ defined(__AROS__) || defined(__MORPHOS__)
    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
{
    എങ്കിൽ (!term_console)
	തിരികെ FAIL;

    എങ്കിൽ (raw_in && raw_out)
    {
	// Save current console mode.
	പൂർണ്ണസംഖ്യ old_tmode = cur_tmode;
	അക്ഷരം ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	എങ്കിൽ (Write(raw_out, ctrl, വലുപ്പം(ctrl)) == വലുപ്പം(ctrl))
	{
	    അക്ഷരം scan[] = "\x9b""1;1;%d;%d r",
		 answ[വലുപ്പം(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    എങ്കിൽ (Read(raw_in, answ, വലുപ്പം(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		എങ്കിൽ (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    തിരികെ OK;
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

    തിരികെ FAIL;
}
#അല്ലെങ്കിൽ
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    പൂർണ്ണസംഖ്യ
mch_get_shellsize(ശൂന്യം)
{
    ഘടന ConUnit  *conUnit;
# ifndef __amigaos4__
    അക്ഷരം	    id_a[വലുപ്പം(ഘടന InfoData) + 3];
# endif
    ഘടന InfoData *id=0;

    എങ്കിൽ (!term_console)	// not an amiga window
	പോകുക out;

    // insure longword alignment
# ifdef __amigaos4__
    എങ്കിൽ (!(id = AllocDosObject(DOS_INFODATA, 0)))
	പോകുക out;
# അല്ലെങ്കിൽ
    id = (ഘടന InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    എങ്കിൽ (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    എങ്കിൽ (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (ഘടന Window *)id->id_Volume_കണ്ണി) == ശൂന്യ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	പോകുക out;
    }
    എങ്കിൽ (oldwindowtitle == ശൂന്യ)
	oldwindowtitle = (char_u *)wb_window->Title;
    എങ്കിൽ (id->id_InUse == (BPTR)ശൂന്യ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	തിരികെ FAIL;
    }
    conUnit = (ഘടന ConUnit *) ((ഘടന IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    എങ്കിൽ (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	തിരികെ FAIL;
    }

    തിരികെ OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    തിരികെ FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    ശൂന്യം
mch_set_shellsize(ശൂന്യം)
{
    എങ്കിൽ (!term_console)
	തിരികെ;

    size_set = TRUE;
    out_char(CSI);
    out_അക്കം((long)Rows);
    out_char('t');
    out_char(CSI);
    out_അക്കം((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    ശൂന്യം
mch_new_shellsize(ശൂന്യം)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    സ്ഥിതം ശൂന്യം
out_അക്കം(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#എങ്കിൽ !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    സ്ഥിതം long
dos_packet(
    ഘടന സന്ദേശം_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    ഘടന സന്ദേശം_Port	    *replyport;
    ഘടന StandardPacket   *packet;
    long		    res1;

    എങ്കിൽ (dos2)
# endif
	തിരികെ DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ഘടന സന്ദേശം_Port *) CreatePort(ശൂന്യ, 0);	// use arp function
    എങ്കിൽ (!replyport)
	തിരികെ (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ഘടന StandardPacket *)
	AllocMem((long) വലുപ്പം(ഘടന StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    എങ്കിൽ (!packet)
    {
	DeletePort(replyport);
	തിരികെ (0);
    }
    packet->sp_സന്ദേശം.mn_കണ്ണി.ln_Name = (അക്ഷരം *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_സന്ദേശം);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_സന്ദേശം(pid, (ഘടന Message *)packet);	// send packet

    WaitPort(replyport);
    Get_സന്ദേശം(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) വലുപ്പം(ഘടന StandardPacket));
    DeletePort(replyport);

    തിരികെ (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    പൂർണ്ണസംഖ്യ
mch_call_shell(
    char_u	*cmd,
    പൂർണ്ണസംഖ്യ		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    പൂർണ്ണസംഖ്യ		x;
    പൂർണ്ണസംഖ്യ		tmode = cur_tmode;
#ifdef AZTEC_C
    പൂർണ്ണസംഖ്യ		use_execute;
    char_u	*shellcmd = ശൂന്യ;
    char_u	*shellarg;
#endif
    പൂർണ്ണസംഖ്യ		retval = 0;

    എങ്കിൽ (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	തിരികെ -1;
    }

    എങ്കിൽ (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    എങ്കിൽ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#എങ്കിൽ !defined(AZTEC_C)		    // not tested very much
    എങ്കിൽ (cmd == ശൂന്യ)
    {
# ifdef FEAT_ARP
	എങ്കിൽ (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	അല്ലെങ്കിൽ
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    അല്ലെങ്കിൽ
    {
# ifdef FEAT_ARP
	എങ്കിൽ (dos2)
# endif
	    x = SystemTags((അക്ഷരം *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	അല്ലെങ്കിൽ
	    x = Execute((അക്ഷരം *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    എങ്കിൽ ((dos2 && x < 0) || (!dos2 && !x))
# അല്ലെങ്കിൽ
    എങ്കിൽ (x < 0)
# endif
    {
	സന്ദേശം_puts(_("Cannot execute "));
	എങ്കിൽ (cmd == ശൂന്യ)
	{
	    സന്ദേശം_puts(_("shell "));
	    സന്ദേശം_outtrans(p_sh);
	}
	അല്ലെങ്കിൽ
	    സന്ദേശം_outtrans(cmd);
	സന്ദേശം_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    അല്ലെങ്കിൽ എങ്കിൽ (!dos2 || x)
# അല്ലെങ്കിൽ
    അല്ലെങ്കിൽ എങ്കിൽ (x)
# endif
    {
	എങ്കിൽ ((x = IoErr()) != 0)
	{
	    എങ്കിൽ (!(options & SHELL_SILENT))
	    {
		സന്ദേശം_putchar('\n');
		സന്ദേശം_outnum((long)x);
		സന്ദേശം_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#അല്ലെങ്കിൽ	// else part is for AZTEC_C
    എങ്കിൽ (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    അല്ലെങ്കിൽ
	use_execute = 0;
    എങ്കിൽ (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	എങ്കിൽ (shellcmd == ശൂന്യ)	    // out of memory, use Execute
	    use_execute = 1;
	അല്ലെങ്കിൽ
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    എങ്കിൽ (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    എങ്കിൽ (cmd == ശൂന്യ)
    {
	എങ്കിൽ (use_execute)
	{
# ifdef FEAT_ARP
	    എങ്കിൽ (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    അല്ലെങ്കിൽ
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	അല്ലെങ്കിൽ
	    x = fexecl((അക്ഷരം *)shellcmd, (അക്ഷരം *)shellcmd, (അക്ഷരം *)shellarg, ശൂന്യ);
    }
    അല്ലെങ്കിൽ എങ്കിൽ (use_execute)
    {
# ifdef FEAT_ARP
	എങ്കിൽ (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	അല്ലെങ്കിൽ
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    അല്ലെങ്കിൽ എങ്കിൽ (p_st & 1)
	x = fexecl((അക്ഷരം *)shellcmd, (അക്ഷരം *)shellcmd, (അക്ഷരം *)shellarg,
							   (അക്ഷരം *)cmd, ശൂന്യ);
    അല്ലെങ്കിൽ
	x = fexecl((അക്ഷരം *)shellcmd, (അക്ഷരം *)shellcmd, (അക്ഷരം *)shellarg,
					   (അക്ഷരം *)p_shcf, (അക്ഷരം *)cmd, ശൂന്യ);
# ifdef FEAT_ARP
    എങ്കിൽ ((dos2 && x < 0) || (!dos2 && x))
# അല്ലെങ്കിൽ
    എങ്കിൽ (x < 0)
# endif
    {
	സന്ദേശം_puts(_("Cannot execute "));
	എങ്കിൽ (use_execute)
	{
	    എങ്കിൽ (cmd == ശൂന്യ)
		സന്ദേശം_outtrans(p_sh);
	    അല്ലെങ്കിൽ
		സന്ദേശം_outtrans(cmd);
	}
	അല്ലെങ്കിൽ
	{
	    സന്ദേശം_puts(_("shell "));
	    സന്ദേശം_outtrans(shellcmd);
	}
	സന്ദേശം_putchar('\n');
	retval = -1;
    }
    അല്ലെങ്കിൽ
    {
	എങ്കിൽ (use_execute)
	{
# ifdef FEAT_ARP
	    എങ്കിൽ (!dos2 || x)
# അല്ലെങ്കിൽ
	    എങ്കിൽ (x)
# endif
		x = IoErr();
	}
	അല്ലെങ്കിൽ
	    x = wait();
	എങ്കിൽ (x)
	{
	    എങ്കിൽ (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		സന്ദേശം_putchar('\n');
		സന്ദേശം_outnum((long)x);
		സന്ദേശം_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    എങ്കിൽ ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    എങ്കിൽ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    എങ്കിൽ (term_console)
	win_resize_on();		// window resize events activated
    തിരികെ retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    ശൂന്യം
mch_breakcheck(പൂർണ്ണസംഖ്യ force UNUSED)
{
   എങ്കിൽ (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
ശൂന്യം __regargs __chkabort(ശൂന്യം);

ശൂന്യം __regargs __chkabort(ശൂന്യം)
{}

#അല്ലെങ്കിൽ
    long
Chk_Abort(ശൂന്യം)
{
    തിരികെ(0L);
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
#അല്ലെങ്കിൽ
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (വലുപ്പം(ഘടന AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    പൂർണ്ണസംഖ്യ
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    പൂർണ്ണസംഖ്യ		flags)		// EW_* flags
{
    ഘടന AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    പൂർണ്ണസംഖ്യ			start_len;
    പൂർണ്ണസംഖ്യ			matches;
#ifdef __amigaos4__
    ഘടന TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#അല്ലെങ്കിൽ
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    എങ്കിൽ (Anchor == ശൂന്യ)
	തിരികെ 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# അല്ലെങ്കിൽ
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    എങ്കിൽ (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	എങ്കിൽ (starbuf == ശൂന്യ)
	    പോകുക Return;
	വേണ്ടി (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    എങ്കിൽ (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    അല്ലെങ്കിൽ
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    അല്ലെങ്കിൽ
	Result = FindFirst((അക്ഷരം *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    ആകുമ്പോൾ (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#അല്ലെങ്കിൽ
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	എങ്കിൽ (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	അല്ലെങ്കിൽ
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    എങ്കിൽ (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    അല്ലെങ്കിൽ എങ്കിൽ (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    എങ്കിൽ (matches)
	qsort((ശൂന്യം *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, വലുപ്പം(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    എങ്കിൽ (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    അല്ലെങ്കിൽ
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#അല്ലെങ്കിൽ
    vim_free(Anchor);
#endif

    തിരികെ matches;
}

    സ്ഥിതം പൂർണ്ണസംഖ്യ
sortcmp(സ്ഥിരം ശൂന്യം *a, സ്ഥിരം ശൂന്യം *b)
{
    അക്ഷരം *s = *(അക്ഷരം **)a;
    അക്ഷരം *t = *(അക്ഷരം **)b;

    തിരികെ pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    പൂർണ്ണസംഖ്യ
mch_has_exp_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ എങ്കിൽ (vim_strchr((char_u *)"*?[(#", *p) != ശൂന്യ)
	    തിരികെ TRUE;
    }
    തിരികെ FALSE;
}

    പൂർണ്ണസംഖ്യ
mch_has_wildcard(char_u *p)
{
    വേണ്ടി ( ; *p; MB_PTR_ADV(p))
    {
	എങ്കിൽ (*p == '\\' && p[1] != NUL)
	    ++p;
	അല്ലെങ്കിൽ
	    എങ്കിൽ (vim_strchr((char_u *)"*?[(#$`", *p) != ശൂന്യ
		    || (*p == '~' && p[1] != NUL))
		തിരികെ TRUE;
    }
    തിരികെ FALSE;
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
    പൂർണ്ണസംഖ്യ		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    സ്ഥിതം char_u   *alloced = ശൂന്യ;	// allocated memory

#ifdef FEAT_ARP
    എങ്കിൽ (!dos2)
	retval = (char_u *)getenv((അക്ഷരം *)var);
    അല്ലെങ്കിൽ
#endif
    {
	VIM_CLEAR(alloced);
	retval = ശൂന്യ;

	buf = alloc(IOSIZE);
	എങ്കിൽ (buf == ശൂന്യ)
	    തിരികെ ശൂന്യ;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	എങ്കിൽ (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    എങ്കിൽ (retval == ശൂന്യ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    തിരികെ retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    പൂർണ്ണസംഖ്യ
mch_setenv(അക്ഷരം *var, അക്ഷരം *മൂല്യം, പൂർണ്ണസംഖ്യ x UNUSED)
{
#ifdef FEAT_ARP
    എങ്കിൽ (!dos2)
	തിരികെ setenv(var, മൂല്യം);
#endif

    എങ്കിൽ (SetVar((UBYTE *)var, (UBYTE *)മൂല്യം, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	തിരികെ 0;   // success
    തിരികെ -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    പൂർണ്ണസംഖ്യ
mch_get_random(char_u *buf, പൂർണ്ണസംഖ്യ len)
{
    ഘടന Process *proc = (ഘടന Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    പൂർണ്ണസംഖ്യ status;

    എങ്കിൽ (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    അല്ലെങ്കിൽ
	status = OK;

    Close(fh);
    തിരികെ status;
}
