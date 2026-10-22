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

#અન્તર્ભાવ "vim.h"
#અન્તર્ભાવ "version.h"

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

#અન્તર્ભાવ <exec/memory.h>

#અન્તર્ભાવ <dos/dostags.h>	    // for 2.0 functions
#અન્તર્ભાવ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#નહીંતર
# define	free_fib(x) vim_free(fib)
#endif

#જો defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#જો defined(__amigaos4__)
સ્થિર અચલ અક્ષર* __attribute__((used)) stackcookie = "$STACK: 1048576";
#અથવાજો defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#અથવાજો defined(__GNUC__) && defined(AMIGA)
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
#અથવાજો !defined(AZTEC_C) && !defined(__AROS__)
સ્થિર long dos_packet(સંરચના સંદેશ_Port *, long, long);
#endif
સ્થિર પૂર્ણાંક lock2name(BPTR lock, char_u *buf, long   len);
સ્થિર શૂન્ય out_અંક(long n);
સ્થિર સંરચના FileInfoBlock *get_fib(char_u *);
સ્થિર પૂર્ણાંક sortcmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    સ્થિર BPTR
safe_Lock(UBYTE *name, long mode)
{
    સંરચના Process *me = (સંરચના Process *)FindTask(ખાલી);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    પાછા flock;
}

સ્થિર BPTR		raw_in = (BPTR)ખાલી;
સ્થિર BPTR		raw_out = (BPTR)ખાલી;
સ્થિર પૂર્ણાંક		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
સંરચના IntuitionBase	*IntuitionBase = ખાલી;
#endif
#ifdef FEAT_ARP
સંરચના ArpBase		*ArpBase = ખાલી;
#endif

સ્થિર સંરચના Window	*wb_window;
સ્થિર char_u		*oldwindowtitle = ખાલી;

#ifdef FEAT_ARP
પૂર્ણાંક			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
પૂર્ણાંક			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
સ્થિર અક્ષર version[] __attribute__((used)) =
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

    શૂન્ય
win_resize_on(શૂન્ય)
{
    OUT_STR_NF("\033[12{");
}

    શૂન્ય
win_resize_off(શૂન્ય)
{
    OUT_STR_NF("\033[12}");
}

    શૂન્ય
mch_write(char_u *p, પૂર્ણાંક len)
{
    Write(raw_out, (અક્ષર *)p, (long)len);
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
    પૂર્ણાંક
mch_inchar(
    char_u  *buf,
    પૂર્ણાંક	    maxlen,
    long    time,		// milliseconds
    પૂર્ણાંક	    tb_change_cnt UNUSED)
{
    પૂર્ણાંક	    len;
    long    utime;

    જો (time >= 0)
    {
	જો (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	નહીંતર
	    utime = time * 1000L;   // convert from milli to micro secs
	જો (WaitForChar(raw_in, utime) == 0)	// no character available
	    પાછા 0;
    }
    નહીંતર    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	જો (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    જો (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (પૂર્ણાંક)KE_CURSORHOLD;
		પાછા 3;
	    }
	    before_blocking();
	}
    }

    માટે (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (અક્ષર *)buf, (long)maxlen / આગત_conv.vc_factor);
	જો (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    જો (આગત_conv.vc_type != CONV_NONE)
		len = convert_આગત(buf, len, maxlen);
	    પાછા len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    પૂર્ણાંક
mch_char_avail(શૂન્ય)
{
    પાછા (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(પૂર્ણાંક special)
{
#જો defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    પાછા (long_u)AvailMem(MEMF_ANY) >> 10;
#નહીંતર
    પાછા (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    શૂન્ય
mch_delay(long msec, પૂર્ણાંક flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    શૂન્ય	    Delay(long);
#endif

    જો (msec <= 0)
	પાછા;

    જો (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    નહીંતર
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    શૂન્ય
mch_suspend(શૂન્ય)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    શૂન્ય
mch_init(શૂન્ય)
{
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    સ્થિર અક્ષર	    intlibname[] = "intuition.library";
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
	સંરચના Process *me = (સંરચના Process *)FindTask(ખાલી);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    જો (raw_in == (BPTR)ખાલી)
    {
	raw_in = આગત_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	જો (!IsInteractive(raw_in))
	    raw_in = raw_out;
	નહીંતર જો (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = ખાલી;
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    જો ((IntuitionBase = (સંરચના IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ખાલી)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#અન્તર્ભાવ <workbench/startup.h>

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

    પૂર્ણાંક
mch_check_win(પૂર્ણાંક argc, અક્ષર **argv)
{
    પૂર્ણાંક		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    સ્થિર char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    સ્થિર char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    સંરચના WBArg    *argp;
    પૂર્ણાંક		    ac;
    અક્ષર	    *av;
    char_u	    *device = ખાલી;
    પૂર્ણાંક		    exitval = 4;
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    સંરચના Library  *DosBase;
#endif
    પૂર્ણાંક		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    જો (DosBase != ખાલી)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    નહીંતર	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# નહીંતર
		    // need arp functions for dos 1.x
	જો (!(ArpBase = (સંરચના ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    માટે (i = 1; i < argc; ++i)
	જો (argv[i][0] == '-')
	{
	    પસંદગી (argv[i][1])
	    {
	    સ્થિતિ 'f':
		usewin = TRUE;
		તોડો;

	    સ્થિતિ 'd':
		જો (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		તોડો;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    જો (argc != 0
	    && device == ખાલી
	    && (IsInteractive(આગત_3()) || IsInteractive(Output())))
	પાછા OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    જો (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	માટે (i = -1; i < 3; ++i)
	{
	    જો (i >= 0)
		device = constrings[i];
	    જો (device != ખાલી && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)ખાલી)
		તોડો;
	}
	જો (raw_in == (BPTR)ખાલી)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    જાઓ exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	પાછા OK;
    }

    જો ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)ખાલી)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	જાઓ exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#જો !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((અક્ષર *)buf1, "t:nc%p", FindTask(0));
#નહીંતર
    sprintf((અક્ષર *)buf1, "t:nc%ld", (long)buf1);
#endif
    જો ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)ખાલી)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((અક્ષર *)buf1);
	mch_errmsg("\n");
	જાઓ exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    જો (argc == 0)	// run from workbench
	ac = ((સંરચના WBStartup *)argv)->sm_અંક_Args;
    નહીંતર
	ac = argc;
    માટે (i = 0; i < ac; ++i)
    {
	જો (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((સંરચના WBStartup *)argv)->sm_ArgList[i]);
	    જો (argp->wa_Lock)
		(શૂન્ય)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    જો (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    નહીંતર	    // use arp function
		TackOn((અક્ષર *)buf2, argp->wa_Name);
#endif
	    av = (અક્ષર *)buf2;
	}
	નહીંતર
	    av = argv[i];

	// skip '-d' or "-dev" option
	જો (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    ચાલુ;
	}
	જો (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	જો (vim_strchr((char_u *)av, ' '))
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
    માટે (i = -1; i < 3; ++i)
    {
	જો (i >= 0)
	    device = constrings[i];
	નહીંતર જો (device == ખાલી)
	    ચાલુ;
	sprintf((અક્ષર *)buf2, "newcli <nil: >nil: %s from %s", (અક્ષર *)device, (અક્ષર *)buf1);
#ifdef FEAT_ARP
	જો (dos2)
	{
#endif
	    જો (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		તોડો;
#ifdef FEAT_ARP
	}
	નહીંતર
	{
	    જો (Execute((UBYTE *)buf2, nilfh, nilfh))
		તોડો;
	}
#endif
    }
    જો (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	જાઓ exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    જો (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    જો (ArpBase)
	CloseLibrary((સંરચના Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    પાછા FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    પૂર્ણાંક
mch_આગત_isatty(શૂન્ય)
{
    પાછા TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    શૂન્ય
fname_case(
    char_u	*name,
    પૂર્ણાંક		len UNUSED)		// buffer size, ignored here
{
    સંરચના FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    જો (fib == ખાલી)
	પાછા;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    જો (fib->fib_DirEntryType == ST_મૂળ)
	strcat(fib->fib_FileName, ":");
#endif
    જો (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    સ્થિર સંરચના FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    સંરચના FileInfoBlock    *fib;

    જો (fname == ખાલી)	    // safety check
	પાછા ખાલી;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#નહીંતર
    fib = ALLOC_ONE(સંરચના FileInfoBlock);
#endif
    જો (fib == ખાલી)
	પાછા;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    જો (flock == (BPTR)ખાલી || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ખાલી;
    }
    જો (flock)
	UnLock(flock);
    પાછા fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    શૂન્ય
mch_settitle(char_u *title, char_u *icon)
{
    જો (wb_window != ખાલી && title != ખાલી)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    શૂન્ય
mch_restore_title(પૂર્ણાંક which)
{
    જો (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ખાલી);
}

    પૂર્ણાંક
mch_can_restore_title(શૂન્ય)
{
    પાછા (wb_window != ખાલી);
}

    પૂર્ણાંક
mch_can_restore_icon(શૂન્ય)
{
    પાછા FALSE;
}

    શૂન્ય
mch_setmouse(પૂર્ણાંક on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    પૂર્ણાંક
mch_get_user_name(char_u *s, પૂર્ણાંક len)
{
#જો defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    સંરચના passwd   *pwd = getpwuid(getuid());

    જો (pwd != ખાલી && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	પાછા OK;
    }
#endif
    *s = NUL;
    પાછા FAIL;
}

/*
 * Insert host name is s[len].
 */
    શૂન્ય
mch_get_host_name(char_u *s, પૂર્ણાંક len)
{
#જો defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((અક્ષર *)s, len);
#નહીંતર
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(શૂન્ય)
{
#જો defined(__amigaos4__)
    પાછા (long) getpid();
#અથવાજો defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    પાછા((long)FindTask(0));
#નહીંતર
    પાછા (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    પૂર્ણાંક
mch_dirname(char_u *buf, પૂર્ણાંક len)
{
    પાછા mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    પૂર્ણાંક		len,
    પૂર્ણાંક		force)
{
    BPTR	l;
    પૂર્ણાંક		retval = FAIL;
    પૂર્ણાંક		i;

    // Lock the file.  If it exists, we can get the exact name.
    જો ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    નહીંતર જો (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	જો ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)ખાલી)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    જો (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		જો (i == 0 || *fname != NUL)
		{
		    જો (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    જો (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    પાછા retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    પૂર્ણાંક
mch_isFullName(char_u *fname)
{
    પાછા (vim_strchr(fname, ':') != ખાલી && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    સ્થિર પૂર્ણાંક
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    જો (dos2)		    // use 2.0 function
#endif
	પાછા ((પૂર્ણાંક)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    નહીંતર		// use arp function
	પાછા ((પૂર્ણાંક)PathName(lock, (અક્ષર *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    સંરચના FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    જો (fib == ખાલી)
	પાછા -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    પાછા retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    પાછા (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    શૂન્ય
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    પૂર્ણાંક
mch_isdir(char_u *name)
{
    સંરચના FileInfoBlock    *fib;
    પૂર્ણાંક			    retval = FALSE;

    fib = get_fib(name);
    જો (fib == ખાલી)
	પાછા FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#નહીંતર
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    પાછા retval;
}

/*
 * Create directory "name".
 */
    પૂર્ણાંક
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    જો (lock == ખાલી)
	પાછા -1;

    UnLock(lock);
    પાછા 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    પૂર્ણાંક
mch_can_exe(char_u *name, char_u **path UNUSED, પૂર્ણાંક use_path)
{
    પૂર્ણાંક exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    જો (seg && GetSegListInfoTags(seg, GSLI_Native, ખાલી, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, ખાલી, TAG_DONE))
    {
	// Test if file permissions allow execution.
	સંરચના ExamineData *exd = ExamineObjectTags(EX_String_Name_આગત, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    નહીંતર
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    જો (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	સંરચના Path_ગાંઠ *head = DupCmdPathList(ખાલી);

	// For each entry, recur to check for executable.
	માટે (સંરચના Path_ગાંઠ *tail = head; !exe && tail;
			       tail = (સંરચના Path_ગાંઠ *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    પાછા exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    પૂર્ણાંક
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    પાછા ગાંઠ_NORMAL;
}

    શૂન્ય
mch_early_init(શૂન્ય)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    શૂન્ય
mch_exit(પૂર્ણાંક r)
{
    exiting = TRUE;

    જો (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    જો (raw_out)
    {
	જો (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    જો (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    જો (ArpBase)
	CloseLibrary((સંરચના Library *) ArpBase);
#endif
    જો (close_win)
	Close(raw_in);
    જો (r)
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

#define MP(xx)	((સંરચના સંદેશ_Port *)((સંરચના FileHandle *) (BADDR(xx)))->fh_Type)

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
    શૂન્ય
mch_settmode(tmode_T tmode)
{
#જો defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    જો (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#નહીંતર
    જો (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#અન્તર્ભાવ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#જો defined(__AROS__) || defined(__MORPHOS__)
    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
{
    જો (!term_console)
	પાછા FAIL;

    જો (raw_in && raw_out)
    {
	// Save current console mode.
	પૂર્ણાંક old_tmode = cur_tmode;
	અક્ષર ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	જો (Write(raw_out, ctrl, કદ(ctrl)) == કદ(ctrl))
	{
	    અક્ષર scan[] = "\x9b""1;1;%d;%d r",
		 answ[કદ(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    જો (Read(raw_in, answ, કદ(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		જો (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    પાછા OK;
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

    પાછા FAIL;
}
#નહીંતર
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    પૂર્ણાંક
mch_get_shellsize(શૂન્ય)
{
    સંરચના ConUnit  *conUnit;
# ifndef __amigaos4__
    અક્ષર	    id_a[કદ(સંરચના InfoData) + 3];
# endif
    સંરચના InfoData *id=0;

    જો (!term_console)	// not an amiga window
	જાઓ out;

    // insure longword alignment
# ifdef __amigaos4__
    જો (!(id = AllocDosObject(DOS_INFODATA, 0)))
	જાઓ out;
# નહીંતર
    id = (સંરચના InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    જો (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    જો (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (સંરચના Window *)id->id_Volume_ગાંઠ) == ખાલી)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	જાઓ out;
    }
    જો (oldwindowtitle == ખાલી)
	oldwindowtitle = (char_u *)wb_window->Title;
    જો (id->id_InUse == (BPTR)ખાલી)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	પાછા FAIL;
    }
    conUnit = (સંરચના ConUnit *) ((સંરચના IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    જો (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	પાછા FAIL;
    }

    પાછા OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    પાછા FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    શૂન્ય
mch_set_shellsize(શૂન્ય)
{
    જો (!term_console)
	પાછા;

    size_set = TRUE;
    out_char(CSI);
    out_અંક((long)Rows);
    out_char('t');
    out_char(CSI);
    out_અંક((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    શૂન્ય
mch_new_shellsize(શૂન્ય)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    સ્થિર શૂન્ય
out_અંક(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#જો !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    સ્થિર long
dos_packet(
    સંરચના સંદેશ_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    સંરચના સંદેશ_Port	    *replyport;
    સંરચના StandardPacket   *packet;
    long		    res1;

    જો (dos2)
# endif
	પાછા DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (સંરચના સંદેશ_Port *) CreatePort(ખાલી, 0);	// use arp function
    જો (!replyport)
	પાછા (0);

    // Allocate space for a packet, make it public and clear it
    packet = (સંરચના StandardPacket *)
	AllocMem((long) કદ(સંરચના StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    જો (!packet)
    {
	DeletePort(replyport);
	પાછા (0);
    }
    packet->sp_સંદેશ.mn_ગાંઠ.ln_Name = (અક્ષર *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_સંદેશ);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_સંદેશ(pid, (સંરચના Message *)packet);	// send packet

    WaitPort(replyport);
    Get_સંદેશ(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) કદ(સંરચના StandardPacket));
    DeletePort(replyport);

    પાછા (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    પૂર્ણાંક
mch_call_shell(
    char_u	*cmd,
    પૂર્ણાંક		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    પૂર્ણાંક		x;
    પૂર્ણાંક		tmode = cur_tmode;
#ifdef AZTEC_C
    પૂર્ણાંક		use_execute;
    char_u	*shellcmd = ખાલી;
    char_u	*shellarg;
#endif
    પૂર્ણાંક		retval = 0;

    જો (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	પાછા -1;
    }

    જો (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    જો (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#જો !defined(AZTEC_C)		    // not tested very much
    જો (cmd == ખાલી)
    {
# ifdef FEAT_ARP
	જો (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	નહીંતર
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    નહીંતર
    {
# ifdef FEAT_ARP
	જો (dos2)
# endif
	    x = SystemTags((અક્ષર *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	નહીંતર
	    x = Execute((અક્ષર *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    જો ((dos2 && x < 0) || (!dos2 && !x))
# નહીંતર
    જો (x < 0)
# endif
    {
	સંદેશ_puts(_("Cannot execute "));
	જો (cmd == ખાલી)
	{
	    સંદેશ_puts(_("shell "));
	    સંદેશ_outtrans(p_sh);
	}
	નહીંતર
	    સંદેશ_outtrans(cmd);
	સંદેશ_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    નહીંતર જો (!dos2 || x)
# નહીંતર
    નહીંતર જો (x)
# endif
    {
	જો ((x = IoErr()) != 0)
	{
	    જો (!(options & SHELL_SILENT))
	    {
		સંદેશ_putchar('\n');
		સંદેશ_outnum((long)x);
		સંદેશ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#નહીંતર	// else part is for AZTEC_C
    જો (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    નહીંતર
	use_execute = 0;
    જો (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	જો (shellcmd == ખાલી)	    // out of memory, use Execute
	    use_execute = 1;
	નહીંતર
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    જો (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    જો (cmd == ખાલી)
    {
	જો (use_execute)
	{
# ifdef FEAT_ARP
	    જો (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    નહીંતર
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	નહીંતર
	    x = fexecl((અક્ષર *)shellcmd, (અક્ષર *)shellcmd, (અક્ષર *)shellarg, ખાલી);
    }
    નહીંતર જો (use_execute)
    {
# ifdef FEAT_ARP
	જો (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	નહીંતર
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    નહીંતર જો (p_st & 1)
	x = fexecl((અક્ષર *)shellcmd, (અક્ષર *)shellcmd, (અક્ષર *)shellarg,
							   (અક્ષર *)cmd, ખાલી);
    નહીંતર
	x = fexecl((અક્ષર *)shellcmd, (અક્ષર *)shellcmd, (અક્ષર *)shellarg,
					   (અક્ષર *)p_shcf, (અક્ષર *)cmd, ખાલી);
# ifdef FEAT_ARP
    જો ((dos2 && x < 0) || (!dos2 && x))
# નહીંતર
    જો (x < 0)
# endif
    {
	સંદેશ_puts(_("Cannot execute "));
	જો (use_execute)
	{
	    જો (cmd == ખાલી)
		સંદેશ_outtrans(p_sh);
	    નહીંતર
		સંદેશ_outtrans(cmd);
	}
	નહીંતર
	{
	    સંદેશ_puts(_("shell "));
	    સંદેશ_outtrans(shellcmd);
	}
	સંદેશ_putchar('\n');
	retval = -1;
    }
    નહીંતર
    {
	જો (use_execute)
	{
# ifdef FEAT_ARP
	    જો (!dos2 || x)
# નહીંતર
	    જો (x)
# endif
		x = IoErr();
	}
	નહીંતર
	    x = wait();
	જો (x)
	{
	    જો (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		સંદેશ_putchar('\n');
		સંદેશ_outnum((long)x);
		સંદેશ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    જો ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    જો (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    જો (term_console)
	win_resize_on();		// window resize events activated
    પાછા retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    શૂન્ય
mch_breakcheck(પૂર્ણાંક force UNUSED)
{
   જો (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
શૂન્ય __regargs __chkabort(શૂન્ય);

શૂન્ય __regargs __chkabort(શૂન્ય)
{}

#નહીંતર
    long
Chk_Abort(શૂન્ય)
{
    પાછા(0L);
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
#નહીંતર
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (કદ(સંરચના AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    પૂર્ણાંક
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    પૂર્ણાંક		flags)		// EW_* flags
{
    સંરચના AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    પૂર્ણાંક			start_len;
    પૂર્ણાંક			matches;
#ifdef __amigaos4__
    સંરચના TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#નહીંતર
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    જો (Anchor == ખાલી)
	પાછા 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# નહીંતર
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    જો (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	જો (starbuf == ખાલી)
	    જાઓ Return;
	માટે (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    જો (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    નહીંતર
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    નહીંતર
	Result = FindFirst((અક્ષર *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    જ્યારે (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#નહીંતર
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	જો (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	નહીંતર
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    જો (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    નહીંતર જો (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    જો (matches)
	qsort((શૂન્ય *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, કદ(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    જો (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    નહીંતર
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#નહીંતર
    vim_free(Anchor);
#endif

    પાછા matches;
}

    સ્થિર પૂર્ણાંક
sortcmp(અચલ શૂન્ય *a, અચલ શૂન્ય *b)
{
    અક્ષર *s = *(અક્ષર **)a;
    અક્ષર *t = *(અક્ષર **)b;

    પાછા pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    પૂર્ણાંક
mch_has_exp_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર જો (vim_strchr((char_u *)"*?[(#", *p) != ખાલી)
	    પાછા TRUE;
    }
    પાછા FALSE;
}

    પૂર્ણાંક
mch_has_wildcard(char_u *p)
{
    માટે ( ; *p; MB_PTR_ADV(p))
    {
	જો (*p == '\\' && p[1] != NUL)
	    ++p;
	નહીંતર
	    જો (vim_strchr((char_u *)"*?[(#$`", *p) != ખાલી
		    || (*p == '~' && p[1] != NUL))
		પાછા TRUE;
    }
    પાછા FALSE;
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
    પૂર્ણાંક		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    સ્થિર char_u   *alloced = ખાલી;	// allocated memory

#ifdef FEAT_ARP
    જો (!dos2)
	retval = (char_u *)getenv((અક્ષર *)var);
    નહીંતર
#endif
    {
	VIM_CLEAR(alloced);
	retval = ખાલી;

	buf = alloc(IOSIZE);
	જો (buf == ખાલી)
	    પાછા ખાલી;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	જો (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    જો (retval == ખાલી && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    પાછા retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    પૂર્ણાંક
mch_setenv(અક્ષર *var, અક્ષર *કિંમત, પૂર્ણાંક x UNUSED)
{
#ifdef FEAT_ARP
    જો (!dos2)
	પાછા setenv(var, કિંમત);
#endif

    જો (SetVar((UBYTE *)var, (UBYTE *)કિંમત, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	પાછા 0;   // success
    પાછા -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    પૂર્ણાંક
mch_get_random(char_u *buf, પૂર્ણાંક len)
{
    સંરચના Process *proc = (સંરચના Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    પૂર્ણાંક status;

    જો (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    નહીંતર
	status = OK;

    Close(fh);
    પાછા status;
}
