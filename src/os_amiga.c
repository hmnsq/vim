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

#அந்தர்பாவ "vim.h"
#அந்தர்பாவ "version.h"

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

#அந்தர்பாவ <exec/memory.h>

#அந்தர்பாவ <dos/dostags.h>	    // for 2.0 functions
#அந்தர்பாவ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#இல்லையெனில்
# define	free_fib(x) vim_free(fib)
#endif

#எனில் defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#எனில் defined(__amigaos4__)
நிலையான மாறிலி எழுத்து* __attribute__((used)) stackcookie = "$STACK: 1048576";
#அல்லதெனில் defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#அல்லதெனில் defined(__GNUC__) && defined(AMIGA)
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
#அல்லதெனில் !defined(AZTEC_C) && !defined(__AROS__)
நிலையான long dos_packet(கட்டமைப்பு செய்தி_Port *, long, long);
#endif
நிலையான முழு lock2name(BPTR lock, char_u *buf, long   len);
நிலையான வெறுமை out_இலக்கம்(long n);
நிலையான கட்டமைப்பு FileInfoBlock *get_fib(char_u *);
நிலையான முழு sortcmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    நிலையான BPTR
safe_Lock(UBYTE *name, long mode)
{
    கட்டமைப்பு Process *me = (கட்டமைப்பு Process *)FindTask(வெற்று);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    திரும்பு flock;
}

நிலையான BPTR		raw_in = (BPTR)வெற்று;
நிலையான BPTR		raw_out = (BPTR)வெற்று;
நிலையான முழு		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
கட்டமைப்பு IntuitionBase	*IntuitionBase = வெற்று;
#endif
#ifdef FEAT_ARP
கட்டமைப்பு ArpBase		*ArpBase = வெற்று;
#endif

நிலையான கட்டமைப்பு Window	*wb_window;
நிலையான char_u		*oldwindowtitle = வெற்று;

#ifdef FEAT_ARP
முழு			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
முழு			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
நிலையான எழுத்து version[] __attribute__((used)) =
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

    வெறுமை
win_resize_on(வெறுமை)
{
    OUT_STR_NF("\033[12{");
}

    வெறுமை
win_resize_off(வெறுமை)
{
    OUT_STR_NF("\033[12}");
}

    வெறுமை
mch_write(char_u *p, முழு len)
{
    Write(raw_out, (எழுத்து *)p, (long)len);
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
    முழு
mch_inchar(
    char_u  *buf,
    முழு	    maxlen,
    long    time,		// milliseconds
    முழு	    tb_change_cnt UNUSED)
{
    முழு	    len;
    long    utime;

    எனில் (time >= 0)
    {
	எனில் (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	இல்லையெனில்
	    utime = time * 1000L;   // convert from milli to micro secs
	எனில் (WaitForChar(raw_in, utime) == 0)	// no character available
	    திரும்பு 0;
    }
    இல்லையெனில்    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	எனில் (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    எனில் (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (முழு)KE_CURSORHOLD;
		திரும்பு 3;
	    }
	    before_blocking();
	}
    }

    க்கு (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (எழுத்து *)buf, (long)maxlen / உள்ளீடு_conv.vc_factor);
	எனில் (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    எனில் (உள்ளீடு_conv.vc_type != CONV_NONE)
		len = convert_உள்ளீடு(buf, len, maxlen);
	    திரும்பு len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    முழு
mch_char_avail(வெறுமை)
{
    திரும்பு (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(முழு special)
{
#எனில் defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    திரும்பு (long_u)AvailMem(MEMF_ANY) >> 10;
#இல்லையெனில்
    திரும்பு (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    வெறுமை
mch_delay(long msec, முழு flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    வெறுமை	    Delay(long);
#endif

    எனில் (msec <= 0)
	திரும்பு;

    எனில் (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    இல்லையெனில்
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    வெறுமை
mch_suspend(வெறுமை)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    வெறுமை
mch_init(வெறுமை)
{
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    நிலையான எழுத்து	    intlibname[] = "intuition.library";
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
	கட்டமைப்பு Process *me = (கட்டமைப்பு Process *)FindTask(வெற்று);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    எனில் (raw_in == (BPTR)வெற்று)
    {
	raw_in = உள்ளீடு_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	எனில் (!IsInteractive(raw_in))
	    raw_in = raw_out;
	இல்லையெனில் எனில் (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = வெற்று;
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    எனில் ((IntuitionBase = (கட்டமைப்பு IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == வெற்று)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#அந்தர்பாவ <workbench/startup.h>

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

    முழு
mch_check_win(முழு argc, எழுத்து **argv)
{
    முழு		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    நிலையான char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    நிலையான char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    கட்டமைப்பு WBArg    *argp;
    முழு		    ac;
    எழுத்து	    *av;
    char_u	    *device = வெற்று;
    முழு		    exitval = 4;
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    கட்டமைப்பு Library  *DosBase;
#endif
    முழு		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    எனில் (DosBase != வெற்று)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    இல்லையெனில்	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# இல்லையெனில்
		    // need arp functions for dos 1.x
	எனில் (!(ArpBase = (கட்டமைப்பு ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    க்கு (i = 1; i < argc; ++i)
	எனில் (argv[i][0] == '-')
	{
	    தேர்வு (argv[i][1])
	    {
	    நிலை 'f':
		usewin = TRUE;
		நிறுத்து;

	    நிலை 'd':
		எனில் (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		நிறுத்து;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    எனில் (argc != 0
	    && device == வெற்று
	    && (IsInteractive(உள்ளீடு_3()) || IsInteractive(Output())))
	திரும்பு OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    எனில் (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	க்கு (i = -1; i < 3; ++i)
	{
	    எனில் (i >= 0)
		device = constrings[i];
	    எனில் (device != வெற்று && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)வெற்று)
		நிறுத்து;
	}
	எனில் (raw_in == (BPTR)வெற்று)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    செல் exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	திரும்பு OK;
    }

    எனில் ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)வெற்று)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	செல் exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#எனில் !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((எழுத்து *)buf1, "t:nc%p", FindTask(0));
#இல்லையெனில்
    sprintf((எழுத்து *)buf1, "t:nc%ld", (long)buf1);
#endif
    எனில் ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)வெற்று)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((எழுத்து *)buf1);
	mch_errmsg("\n");
	செல் exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    எனில் (argc == 0)	// run from workbench
	ac = ((கட்டமைப்பு WBStartup *)argv)->sm_இலக்கம்_Args;
    இல்லையெனில்
	ac = argc;
    க்கு (i = 0; i < ac; ++i)
    {
	எனில் (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((கட்டமைப்பு WBStartup *)argv)->sm_ArgList[i]);
	    எனில் (argp->wa_Lock)
		(வெறுமை)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    எனில் (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    இல்லையெனில்	    // use arp function
		TackOn((எழுத்து *)buf2, argp->wa_Name);
#endif
	    av = (எழுத்து *)buf2;
	}
	இல்லையெனில்
	    av = argv[i];

	// skip '-d' or "-dev" option
	எனில் (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    தொடர்;
	}
	எனில் (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	எனில் (vim_strchr((char_u *)av, ' '))
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
    க்கு (i = -1; i < 3; ++i)
    {
	எனில் (i >= 0)
	    device = constrings[i];
	இல்லையெனில் எனில் (device == வெற்று)
	    தொடர்;
	sprintf((எழுத்து *)buf2, "newcli <nil: >nil: %s from %s", (எழுத்து *)device, (எழுத்து *)buf1);
#ifdef FEAT_ARP
	எனில் (dos2)
	{
#endif
	    எனில் (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		நிறுத்து;
#ifdef FEAT_ARP
	}
	இல்லையெனில்
	{
	    எனில் (Execute((UBYTE *)buf2, nilfh, nilfh))
		நிறுத்து;
	}
#endif
    }
    எனில் (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	செல் exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    எனில் (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    எனில் (ArpBase)
	CloseLibrary((கட்டமைப்பு Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    திரும்பு FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    முழு
mch_உள்ளீடு_isatty(வெறுமை)
{
    திரும்பு TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    வெறுமை
fname_case(
    char_u	*name,
    முழு		len UNUSED)		// buffer size, ignored here
{
    கட்டமைப்பு FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    எனில் (fib == வெற்று)
	திரும்பு;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    எனில் (fib->fib_DirEntryType == ST_வேர்)
	strcat(fib->fib_FileName, ":");
#endif
    எனில் (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    நிலையான கட்டமைப்பு FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    கட்டமைப்பு FileInfoBlock    *fib;

    எனில் (fname == வெற்று)	    // safety check
	திரும்பு வெற்று;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#இல்லையெனில்
    fib = ALLOC_ONE(கட்டமைப்பு FileInfoBlock);
#endif
    எனில் (fib == வெற்று)
	திரும்பு;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    எனில் (flock == (BPTR)வெற்று || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = வெற்று;
    }
    எனில் (flock)
	UnLock(flock);
    திரும்பு fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    வெறுமை
mch_settitle(char_u *title, char_u *icon)
{
    எனில் (wb_window != வெற்று && title != வெற்று)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    வெறுமை
mch_restore_title(முழு which)
{
    எனில் (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, வெற்று);
}

    முழு
mch_can_restore_title(வெறுமை)
{
    திரும்பு (wb_window != வெற்று);
}

    முழு
mch_can_restore_icon(வெறுமை)
{
    திரும்பு FALSE;
}

    வெறுமை
mch_setmouse(முழு on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    முழு
mch_get_user_name(char_u *s, முழு len)
{
#எனில் defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    கட்டமைப்பு passwd   *pwd = getpwuid(getuid());

    எனில் (pwd != வெற்று && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	திரும்பு OK;
    }
#endif
    *s = NUL;
    திரும்பு FAIL;
}

/*
 * Insert host name is s[len].
 */
    வெறுமை
mch_get_host_name(char_u *s, முழு len)
{
#எனில் defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((எழுத்து *)s, len);
#இல்லையெனில்
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(வெறுமை)
{
#எனில் defined(__amigaos4__)
    திரும்பு (long) getpid();
#அல்லதெனில் defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    திரும்பு((long)FindTask(0));
#இல்லையெனில்
    திரும்பு (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    முழு
mch_dirname(char_u *buf, முழு len)
{
    திரும்பு mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    முழு		len,
    முழு		force)
{
    BPTR	l;
    முழு		retval = FAIL;
    முழு		i;

    // Lock the file.  If it exists, we can get the exact name.
    எனில் ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    இல்லையெனில் எனில் (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	எனில் ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)வெற்று)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    எனில் (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		எனில் (i == 0 || *fname != NUL)
		{
		    எனில் (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    எனில் (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    திரும்பு retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    முழு
mch_isFullName(char_u *fname)
{
    திரும்பு (vim_strchr(fname, ':') != வெற்று && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    நிலையான முழு
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    எனில் (dos2)		    // use 2.0 function
#endif
	திரும்பு ((முழு)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    இல்லையெனில்		// use arp function
	திரும்பு ((முழு)PathName(lock, (எழுத்து *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    கட்டமைப்பு FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    எனில் (fib == வெற்று)
	திரும்பு -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    திரும்பு retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    முழு
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    திரும்பு (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    வெறுமை
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    முழு
mch_isdir(char_u *name)
{
    கட்டமைப்பு FileInfoBlock    *fib;
    முழு			    retval = FALSE;

    fib = get_fib(name);
    எனில் (fib == வெற்று)
	திரும்பு FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#இல்லையெனில்
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    திரும்பு retval;
}

/*
 * Create directory "name".
 */
    முழு
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    எனில் (lock == வெற்று)
	திரும்பு -1;

    UnLock(lock);
    திரும்பு 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    முழு
mch_can_exe(char_u *name, char_u **path UNUSED, முழு use_path)
{
    முழு exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    எனில் (seg && GetSegListInfoTags(seg, GSLI_Native, வெற்று, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, வெற்று, TAG_DONE))
    {
	// Test if file permissions allow execution.
	கட்டமைப்பு ExamineData *exd = ExamineObjectTags(EX_String_Name_உள்ளீடு, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    இல்லையெனில்
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    எனில் (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	கட்டமைப்பு Path_முனை *head = DupCmdPathList(வெற்று);

	// For each entry, recur to check for executable.
	க்கு (கட்டமைப்பு Path_முனை *tail = head; !exe && tail;
			       tail = (கட்டமைப்பு Path_முனை *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    திரும்பு exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    முழு
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    திரும்பு முனை_NORMAL;
}

    வெறுமை
mch_early_init(வெறுமை)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    வெறுமை
mch_exit(முழு r)
{
    exiting = TRUE;

    எனில் (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    எனில் (raw_out)
    {
	எனில் (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    எனில் (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    எனில் (ArpBase)
	CloseLibrary((கட்டமைப்பு Library *) ArpBase);
#endif
    எனில் (close_win)
	Close(raw_in);
    எனில் (r)
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

#define MP(xx)	((கட்டமைப்பு செய்தி_Port *)((கட்டமைப்பு FileHandle *) (BADDR(xx)))->fh_Type)

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
    வெறுமை
mch_settmode(tmode_T tmode)
{
#எனில் defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    எனில் (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#இல்லையெனில்
    எனில் (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#அந்தர்பாவ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#எனில் defined(__AROS__) || defined(__MORPHOS__)
    முழு
mch_get_shellsize(வெறுமை)
{
    எனில் (!term_console)
	திரும்பு FAIL;

    எனில் (raw_in && raw_out)
    {
	// Save current console mode.
	முழு old_tmode = cur_tmode;
	எழுத்து ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	எனில் (Write(raw_out, ctrl, அளவு(ctrl)) == அளவு(ctrl))
	{
	    எழுத்து scan[] = "\x9b""1;1;%d;%d r",
		 answ[அளவு(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    எனில் (Read(raw_in, answ, அளவு(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		எனில் (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    திரும்பு OK;
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

    திரும்பு FAIL;
}
#இல்லையெனில்
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    முழு
mch_get_shellsize(வெறுமை)
{
    கட்டமைப்பு ConUnit  *conUnit;
# ifndef __amigaos4__
    எழுத்து	    id_a[அளவு(கட்டமைப்பு InfoData) + 3];
# endif
    கட்டமைப்பு InfoData *id=0;

    எனில் (!term_console)	// not an amiga window
	செல் out;

    // insure longword alignment
# ifdef __amigaos4__
    எனில் (!(id = AllocDosObject(DOS_INFODATA, 0)))
	செல் out;
# இல்லையெனில்
    id = (கட்டமைப்பு InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    எனில் (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    எனில் (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (கட்டமைப்பு Window *)id->id_Volume_முனை) == வெற்று)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	செல் out;
    }
    எனில் (oldwindowtitle == வெற்று)
	oldwindowtitle = (char_u *)wb_window->Title;
    எனில் (id->id_InUse == (BPTR)வெற்று)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	திரும்பு FAIL;
    }
    conUnit = (கட்டமைப்பு ConUnit *) ((கட்டமைப்பு IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    எனில் (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	திரும்பு FAIL;
    }

    திரும்பு OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    திரும்பு FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    வெறுமை
mch_set_shellsize(வெறுமை)
{
    எனில் (!term_console)
	திரும்பு;

    size_set = TRUE;
    out_char(CSI);
    out_இலக்கம்((long)Rows);
    out_char('t');
    out_char(CSI);
    out_இலக்கம்((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    வெறுமை
mch_new_shellsize(வெறுமை)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    நிலையான வெறுமை
out_இலக்கம்(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#எனில் !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    நிலையான long
dos_packet(
    கட்டமைப்பு செய்தி_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    கட்டமைப்பு செய்தி_Port	    *replyport;
    கட்டமைப்பு StandardPacket   *packet;
    long		    res1;

    எனில் (dos2)
# endif
	திரும்பு DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (கட்டமைப்பு செய்தி_Port *) CreatePort(வெற்று, 0);	// use arp function
    எனில் (!replyport)
	திரும்பு (0);

    // Allocate space for a packet, make it public and clear it
    packet = (கட்டமைப்பு StandardPacket *)
	AllocMem((long) அளவு(கட்டமைப்பு StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    எனில் (!packet)
    {
	DeletePort(replyport);
	திரும்பு (0);
    }
    packet->sp_செய்தி.mn_முனை.ln_Name = (எழுத்து *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_செய்தி);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_செய்தி(pid, (கட்டமைப்பு Message *)packet);	// send packet

    WaitPort(replyport);
    Get_செய்தி(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) அளவு(கட்டமைப்பு StandardPacket));
    DeletePort(replyport);

    திரும்பு (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    முழு
mch_call_shell(
    char_u	*cmd,
    முழு		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    முழு		x;
    முழு		tmode = cur_tmode;
#ifdef AZTEC_C
    முழு		use_execute;
    char_u	*shellcmd = வெற்று;
    char_u	*shellarg;
#endif
    முழு		retval = 0;

    எனில் (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	திரும்பு -1;
    }

    எனில் (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    எனில் (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#எனில் !defined(AZTEC_C)		    // not tested very much
    எனில் (cmd == வெற்று)
    {
# ifdef FEAT_ARP
	எனில் (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	இல்லையெனில்
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    இல்லையெனில்
    {
# ifdef FEAT_ARP
	எனில் (dos2)
# endif
	    x = SystemTags((எழுத்து *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	இல்லையெனில்
	    x = Execute((எழுத்து *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    எனில் ((dos2 && x < 0) || (!dos2 && !x))
# இல்லையெனில்
    எனில் (x < 0)
# endif
    {
	செய்தி_puts(_("Cannot execute "));
	எனில் (cmd == வெற்று)
	{
	    செய்தி_puts(_("shell "));
	    செய்தி_outtrans(p_sh);
	}
	இல்லையெனில்
	    செய்தி_outtrans(cmd);
	செய்தி_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    இல்லையெனில் எனில் (!dos2 || x)
# இல்லையெனில்
    இல்லையெனில் எனில் (x)
# endif
    {
	எனில் ((x = IoErr()) != 0)
	{
	    எனில் (!(options & SHELL_SILENT))
	    {
		செய்தி_putchar('\n');
		செய்தி_outnum((long)x);
		செய்தி_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#இல்லையெனில்	// else part is for AZTEC_C
    எனில் (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    இல்லையெனில்
	use_execute = 0;
    எனில் (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	எனில் (shellcmd == வெற்று)	    // out of memory, use Execute
	    use_execute = 1;
	இல்லையெனில்
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    எனில் (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    எனில் (cmd == வெற்று)
    {
	எனில் (use_execute)
	{
# ifdef FEAT_ARP
	    எனில் (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    இல்லையெனில்
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	இல்லையெனில்
	    x = fexecl((எழுத்து *)shellcmd, (எழுத்து *)shellcmd, (எழுத்து *)shellarg, வெற்று);
    }
    இல்லையெனில் எனில் (use_execute)
    {
# ifdef FEAT_ARP
	எனில் (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	இல்லையெனில்
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    இல்லையெனில் எனில் (p_st & 1)
	x = fexecl((எழுத்து *)shellcmd, (எழுத்து *)shellcmd, (எழுத்து *)shellarg,
							   (எழுத்து *)cmd, வெற்று);
    இல்லையெனில்
	x = fexecl((எழுத்து *)shellcmd, (எழுத்து *)shellcmd, (எழுத்து *)shellarg,
					   (எழுத்து *)p_shcf, (எழுத்து *)cmd, வெற்று);
# ifdef FEAT_ARP
    எனில் ((dos2 && x < 0) || (!dos2 && x))
# இல்லையெனில்
    எனில் (x < 0)
# endif
    {
	செய்தி_puts(_("Cannot execute "));
	எனில் (use_execute)
	{
	    எனில் (cmd == வெற்று)
		செய்தி_outtrans(p_sh);
	    இல்லையெனில்
		செய்தி_outtrans(cmd);
	}
	இல்லையெனில்
	{
	    செய்தி_puts(_("shell "));
	    செய்தி_outtrans(shellcmd);
	}
	செய்தி_putchar('\n');
	retval = -1;
    }
    இல்லையெனில்
    {
	எனில் (use_execute)
	{
# ifdef FEAT_ARP
	    எனில் (!dos2 || x)
# இல்லையெனில்
	    எனில் (x)
# endif
		x = IoErr();
	}
	இல்லையெனில்
	    x = wait();
	எனில் (x)
	{
	    எனில் (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		செய்தி_putchar('\n');
		செய்தி_outnum((long)x);
		செய்தி_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    எனில் ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    எனில் (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    எனில் (term_console)
	win_resize_on();		// window resize events activated
    திரும்பு retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    வெறுமை
mch_breakcheck(முழு force UNUSED)
{
   எனில் (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
வெறுமை __regargs __chkabort(வெறுமை);

வெறுமை __regargs __chkabort(வெறுமை)
{}

#இல்லையெனில்
    long
Chk_Abort(வெறுமை)
{
    திரும்பு(0L);
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
#இல்லையெனில்
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (அளவு(கட்டமைப்பு AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    முழு
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    முழு		flags)		// EW_* flags
{
    கட்டமைப்பு AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    முழு			start_len;
    முழு			matches;
#ifdef __amigaos4__
    கட்டமைப்பு TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#இல்லையெனில்
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    எனில் (Anchor == வெற்று)
	திரும்பு 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# இல்லையெனில்
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    எனில் (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	எனில் (starbuf == வெற்று)
	    செல் Return;
	க்கு (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    எனில் (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    இல்லையெனில்
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    இல்லையெனில்
	Result = FindFirst((எழுத்து *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    போது (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#இல்லையெனில்
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	எனில் (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	இல்லையெனில்
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    எனில் (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    இல்லையெனில் எனில் (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    எனில் (matches)
	qsort((வெறுமை *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, அளவு(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    எனில் (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    இல்லையெனில்
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#இல்லையெனில்
    vim_free(Anchor);
#endif

    திரும்பு matches;
}

    நிலையான முழு
sortcmp(மாறிலி வெறுமை *a, மாறிலி வெறுமை *b)
{
    எழுத்து *s = *(எழுத்து **)a;
    எழுத்து *t = *(எழுத்து **)b;

    திரும்பு pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    முழு
mch_has_exp_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில் எனில் (vim_strchr((char_u *)"*?[(#", *p) != வெற்று)
	    திரும்பு TRUE;
    }
    திரும்பு FALSE;
}

    முழு
mch_has_wildcard(char_u *p)
{
    க்கு ( ; *p; MB_PTR_ADV(p))
    {
	எனில் (*p == '\\' && p[1] != NUL)
	    ++p;
	இல்லையெனில்
	    எனில் (vim_strchr((char_u *)"*?[(#$`", *p) != வெற்று
		    || (*p == '~' && p[1] != NUL))
		திரும்பு TRUE;
    }
    திரும்பு FALSE;
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
    முழு		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    நிலையான char_u   *alloced = வெற்று;	// allocated memory

#ifdef FEAT_ARP
    எனில் (!dos2)
	retval = (char_u *)getenv((எழுத்து *)var);
    இல்லையெனில்
#endif
    {
	VIM_CLEAR(alloced);
	retval = வெற்று;

	buf = alloc(IOSIZE);
	எனில் (buf == வெற்று)
	    திரும்பு வெற்று;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	எனில் (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    எனில் (retval == வெற்று && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    திரும்பு retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    முழு
mch_setenv(எழுத்து *var, எழுத்து *மதிப்பு, முழு x UNUSED)
{
#ifdef FEAT_ARP
    எனில் (!dos2)
	திரும்பு setenv(var, மதிப்பு);
#endif

    எனில் (SetVar((UBYTE *)var, (UBYTE *)மதிப்பு, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	திரும்பு 0;   // success
    திரும்பு -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    முழு
mch_get_random(char_u *buf, முழு len)
{
    கட்டமைப்பு Process *proc = (கட்டமைப்பு Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    முழு status;

    எனில் (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    இல்லையெனில்
	status = OK;

    Close(fh);
    திரும்பு status;
}
