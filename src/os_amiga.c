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

#অন্তর্ভাৱ "vim.h"
#অন্তর্ভাৱ "version.h"

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

#অন্তর্ভাৱ <exec/memory.h>

#অন্তর্ভাৱ <dos/dostags.h>	    // for 2.0 functions
#অন্তর্ভাৱ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#অন্যথা
# define	free_fib(x) vim_free(fib)
#endif

#যদি defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#যদি defined(__amigaos4__)
স্থির ধ্রুৱ ৱর্ণ* __attribute__((used)) stackcookie = "$STACK: 1048576";
#নোচেদ্যদি defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#নোচেদ্যদি defined(__GNUC__) && defined(AMIGA)
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
#নোচেদ্যদি !defined(AZTEC_C) && !defined(__AROS__)
স্থির long dos_packet(সংরচনা বাৰ্তা_Port *, long, long);
#endif
স্থির পূর্ণাঙ্ক lock2name(BPTR lock, char_u *buf, long   len);
স্থির শূন্য out_অঙ্ক(long n);
স্থির সংরচনা FileInfoBlock *get_fib(char_u *);
স্থির পূর্ণাঙ্ক sortcmp(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    স্থির BPTR
safe_Lock(UBYTE *name, long mode)
{
    সংরচনা Process *me = (সংরচনা Process *)FindTask(রিক্ত);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    ঘূৰাই flock;
}

স্থির BPTR		raw_in = (BPTR)রিক্ত;
স্থির BPTR		raw_out = (BPTR)রিক্ত;
স্থির পূর্ণাঙ্ক		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
সংরচনা IntuitionBase	*IntuitionBase = রিক্ত;
#endif
#ifdef FEAT_ARP
সংরচনা ArpBase		*ArpBase = রিক্ত;
#endif

স্থির সংরচনা Window	*wb_window;
স্থির char_u		*oldwindowtitle = রিক্ত;

#ifdef FEAT_ARP
পূর্ণাঙ্ক			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
পূর্ণাঙ্ক			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
স্থির ৱর্ণ version[] __attribute__((used)) =
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

    শূন্য
win_resize_on(শূন্য)
{
    OUT_STR_NF("\033[12{");
}

    শূন্য
win_resize_off(শূন্য)
{
    OUT_STR_NF("\033[12}");
}

    শূন্য
mch_write(char_u *p, পূর্ণাঙ্ক len)
{
    Write(raw_out, (ৱর্ণ *)p, (long)len);
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
    পূর্ণাঙ্ক
mch_inchar(
    char_u  *buf,
    পূর্ণাঙ্ক	    maxlen,
    long    time,		// milliseconds
    পূর্ণাঙ্ক	    tb_change_cnt UNUSED)
{
    পূর্ণাঙ্ক	    len;
    long    utime;

    যদি (time >= 0)
    {
	যদি (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	অন্যথা
	    utime = time * 1000L;   // convert from milli to micro secs
	যদি (WaitForChar(raw_in, utime) == 0)	// no character available
	    ঘূৰাই 0;
    }
    অন্যথা    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	যদি (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    যদি (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (পূর্ণাঙ্ক)KE_CURSORHOLD;
		ঘূৰাই 3;
	    }
	    before_blocking();
	}
    }

    বাবে (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (ৱর্ণ *)buf, (long)maxlen / প্ৰবেশ_conv.vc_factor);
	যদি (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    যদি (প্ৰবেশ_conv.vc_type != CONV_NONE)
		len = convert_প্ৰবেশ(buf, len, maxlen);
	    ঘূৰাই len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    পূর্ণাঙ্ক
mch_char_avail(শূন্য)
{
    ঘূৰাই (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(পূর্ণাঙ্ক special)
{
#যদি defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ঘূৰাই (long_u)AvailMem(MEMF_ANY) >> 10;
#অন্যথা
    ঘূৰাই (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    শূন্য
mch_delay(long msec, পূর্ণাঙ্ক flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    শূন্য	    Delay(long);
#endif

    যদি (msec <= 0)
	ঘূৰাই;

    যদি (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    অন্যথা
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    শূন্য
mch_suspend(শূন্য)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    শূন্য
mch_init(শূন্য)
{
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    স্থির ৱর্ণ	    intlibname[] = "intuition.library";
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
	সংরচনা Process *me = (সংরচনা Process *)FindTask(রিক্ত);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    যদি (raw_in == (BPTR)রিক্ত)
    {
	raw_in = প্ৰবেশ_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	যদি (!IsInteractive(raw_in))
	    raw_in = raw_out;
	অন্যথা যদি (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = রিক্ত;
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    যদি ((IntuitionBase = (সংরচনা IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == রিক্ত)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#অন্তর্ভাৱ <workbench/startup.h>

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

    পূর্ণাঙ্ক
mch_check_win(পূর্ণাঙ্ক argc, ৱর্ণ **argv)
{
    পূর্ণাঙ্ক		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    স্থির char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    স্থির char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    সংরচনা WBArg    *argp;
    পূর্ণাঙ্ক		    ac;
    ৱর্ণ	    *av;
    char_u	    *device = রিক্ত;
    পূর্ণাঙ্ক		    exitval = 4;
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    সংরচনা Library  *DosBase;
#endif
    পূর্ণাঙ্ক		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    যদি (DosBase != রিক্ত)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    অন্যথা	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# অন্যথা
		    // need arp functions for dos 1.x
	যদি (!(ArpBase = (সংরচনা ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    বাবে (i = 1; i < argc; ++i)
	যদি (argv[i][0] == '-')
	{
	    ৱিকল্প (argv[i][1])
	    {
	    পক্ষ 'f':
		usewin = TRUE;
		ভঙ্গ;

	    পক্ষ 'd':
		যদি (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		ভঙ্গ;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    যদি (argc != 0
	    && device == রিক্ত
	    && (IsInteractive(প্ৰবেশ_3()) || IsInteractive(Output())))
	ঘূৰাই OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    যদি (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	বাবে (i = -1; i < 3; ++i)
	{
	    যদি (i >= 0)
		device = constrings[i];
	    যদি (device != রিক্ত && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)রিক্ত)
		ভঙ্গ;
	}
	যদি (raw_in == (BPTR)রিক্ত)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    গচ্ছ exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	ঘূৰাই OK;
    }

    যদি ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)রিক্ত)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	গচ্ছ exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((ৱর্ণ *)buf1, "t:nc%p", FindTask(0));
#অন্যথা
    sprintf((ৱর্ণ *)buf1, "t:nc%ld", (long)buf1);
#endif
    যদি ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)রিক্ত)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((ৱর্ণ *)buf1);
	mch_errmsg("\n");
	গচ্ছ exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    যদি (argc == 0)	// run from workbench
	ac = ((সংরচনা WBStartup *)argv)->sm_অঙ্ক_Args;
    অন্যথা
	ac = argc;
    বাবে (i = 0; i < ac; ++i)
    {
	যদি (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((সংরচনা WBStartup *)argv)->sm_ArgList[i]);
	    যদি (argp->wa_Lock)
		(শূন্য)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    যদি (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    অন্যথা	    // use arp function
		TackOn((ৱর্ণ *)buf2, argp->wa_Name);
#endif
	    av = (ৱর্ণ *)buf2;
	}
	অন্যথা
	    av = argv[i];

	// skip '-d' or "-dev" option
	যদি (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    অনুৱর্ত;
	}
	যদি (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	যদি (vim_strchr((char_u *)av, ' '))
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
    বাবে (i = -1; i < 3; ++i)
    {
	যদি (i >= 0)
	    device = constrings[i];
	অন্যথা যদি (device == রিক্ত)
	    অনুৱর্ত;
	sprintf((ৱর্ণ *)buf2, "newcli <nil: >nil: %s from %s", (ৱর্ণ *)device, (ৱর্ণ *)buf1);
#ifdef FEAT_ARP
	যদি (dos2)
	{
#endif
	    যদি (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		ভঙ্গ;
#ifdef FEAT_ARP
	}
	অন্যথা
	{
	    যদি (Execute((UBYTE *)buf2, nilfh, nilfh))
		ভঙ্গ;
	}
#endif
    }
    যদি (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	গচ্ছ exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    যদি (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    যদি (ArpBase)
	CloseLibrary((সংরচনা Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    ঘূৰাই FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    পূর্ণাঙ্ক
mch_প্ৰবেশ_isatty(শূন্য)
{
    ঘূৰাই TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    শূন্য
fname_case(
    char_u	*name,
    পূর্ণাঙ্ক		len UNUSED)		// buffer size, ignored here
{
    সংরচনা FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    যদি (fib == রিক্ত)
	ঘূৰাই;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    যদি (fib->fib_DirEntryType == ST_মূল)
	strcat(fib->fib_FileName, ":");
#endif
    যদি (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    স্থির সংরচনা FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    সংরচনা FileInfoBlock    *fib;

    যদি (fname == রিক্ত)	    // safety check
	ঘূৰাই রিক্ত;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#অন্যথা
    fib = ALLOC_ONE(সংরচনা FileInfoBlock);
#endif
    যদি (fib == রিক্ত)
	ঘূৰাই;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    যদি (flock == (BPTR)রিক্ত || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = রিক্ত;
    }
    যদি (flock)
	UnLock(flock);
    ঘূৰাই fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    শূন্য
mch_settitle(char_u *title, char_u *icon)
{
    যদি (wb_window != রিক্ত && title != রিক্ত)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    শূন্য
mch_restore_title(পূর্ণাঙ্ক which)
{
    যদি (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, রিক্ত);
}

    পূর্ণাঙ্ক
mch_can_restore_title(শূন্য)
{
    ঘূৰাই (wb_window != রিক্ত);
}

    পূর্ণাঙ্ক
mch_can_restore_icon(শূন্য)
{
    ঘূৰাই FALSE;
}

    শূন্য
mch_setmouse(পূর্ণাঙ্ক on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    পূর্ণাঙ্ক
mch_get_user_name(char_u *s, পূর্ণাঙ্ক len)
{
#যদি defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    সংরচনা passwd   *pwd = getpwuid(getuid());

    যদি (pwd != রিক্ত && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ঘূৰাই OK;
    }
#endif
    *s = NUL;
    ঘূৰাই FAIL;
}

/*
 * Insert host name is s[len].
 */
    শূন্য
mch_get_host_name(char_u *s, পূর্ণাঙ্ক len)
{
#যদি defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((ৱর্ণ *)s, len);
#অন্যথা
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(শূন্য)
{
#যদি defined(__amigaos4__)
    ঘূৰাই (long) getpid();
#নোচেদ্যদি defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ঘূৰাই((long)FindTask(0));
#অন্যথা
    ঘূৰাই (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    পূর্ণাঙ্ক
mch_dirname(char_u *buf, পূর্ণাঙ্ক len)
{
    ঘূৰাই mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণাঙ্ক		len,
    পূর্ণাঙ্ক		force)
{
    BPTR	l;
    পূর্ণাঙ্ক		retval = FAIL;
    পূর্ণাঙ্ক		i;

    // Lock the file.  If it exists, we can get the exact name.
    যদি ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    অন্যথা যদি (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	যদি ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)রিক্ত)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    যদি (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		যদি (i == 0 || *fname != NUL)
		{
		    যদি (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    যদি (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    ঘূৰাই retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    পূর্ণাঙ্ক
mch_isFullName(char_u *fname)
{
    ঘূৰাই (vim_strchr(fname, ':') != রিক্ত && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    স্থির পূর্ণাঙ্ক
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    যদি (dos2)		    // use 2.0 function
#endif
	ঘূৰাই ((পূর্ণাঙ্ক)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    অন্যথা		// use arp function
	ঘূৰাই ((পূর্ণাঙ্ক)PathName(lock, (ৱর্ণ *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    সংরচনা FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    যদি (fib == রিক্ত)
	ঘূৰাই -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ঘূৰাই retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ঘূৰাই (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    শূন্য
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    পূর্ণাঙ্ক
mch_isdir(char_u *name)
{
    সংরচনা FileInfoBlock    *fib;
    পূর্ণাঙ্ক			    retval = FALSE;

    fib = get_fib(name);
    যদি (fib == রিক্ত)
	ঘূৰাই FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#অন্যথা
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    ঘূৰাই retval;
}

/*
 * Create directory "name".
 */
    পূর্ণাঙ্ক
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    যদি (lock == রিক্ত)
	ঘূৰাই -1;

    UnLock(lock);
    ঘূৰাই 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    পূর্ণাঙ্ক
mch_can_exe(char_u *name, char_u **path UNUSED, পূর্ণাঙ্ক use_path)
{
    পূর্ণাঙ্ক exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    যদি (seg && GetSegListInfoTags(seg, GSLI_Native, রিক্ত, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, রিক্ত, TAG_DONE))
    {
	// Test if file permissions allow execution.
	সংরচনা ExamineData *exd = ExamineObjectTags(EX_String_Name_প্ৰবেশ, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    অন্যথা
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    যদি (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	সংরচনা Path_গ্ৰন্থি *head = DupCmdPathList(রিক্ত);

	// For each entry, recur to check for executable.
	বাবে (সংরচনা Path_গ্ৰন্থি *tail = head; !exe && tail;
			       tail = (সংরচনা Path_গ্ৰন্থি *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    ঘূৰাই exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    পূর্ণাঙ্ক
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    ঘূৰাই গ্ৰন্থি_NORMAL;
}

    শূন্য
mch_early_init(শূন্য)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    শূন্য
mch_exit(পূর্ণাঙ্ক r)
{
    exiting = TRUE;

    যদি (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    যদি (raw_out)
    {
	যদি (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    যদি (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    যদি (ArpBase)
	CloseLibrary((সংরচনা Library *) ArpBase);
#endif
    যদি (close_win)
	Close(raw_in);
    যদি (r)
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

#define MP(xx)	((সংরচনা বাৰ্তা_Port *)((সংরচনা FileHandle *) (BADDR(xx)))->fh_Type)

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
    শূন্য
mch_settmode(tmode_T tmode)
{
#যদি defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    যদি (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#অন্যথা
    যদি (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#অন্তর্ভাৱ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#যদি defined(__AROS__) || defined(__MORPHOS__)
    পূর্ণাঙ্ক
mch_get_shellsize(শূন্য)
{
    যদি (!term_console)
	ঘূৰাই FAIL;

    যদি (raw_in && raw_out)
    {
	// Save current console mode.
	পূর্ণাঙ্ক old_tmode = cur_tmode;
	ৱর্ণ ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	যদি (Write(raw_out, ctrl, পরিমাণ(ctrl)) == পরিমাণ(ctrl))
	{
	    ৱর্ণ scan[] = "\x9b""1;1;%d;%d r",
		 answ[পরিমাণ(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    যদি (Read(raw_in, answ, পরিমাণ(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		যদি (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ঘূৰাই OK;
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

    ঘূৰাই FAIL;
}
#অন্যথা
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    পূর্ণাঙ্ক
mch_get_shellsize(শূন্য)
{
    সংরচনা ConUnit  *conUnit;
# ifndef __amigaos4__
    ৱর্ণ	    id_a[পরিমাণ(সংরচনা InfoData) + 3];
# endif
    সংরচনা InfoData *id=0;

    যদি (!term_console)	// not an amiga window
	গচ্ছ out;

    // insure longword alignment
# ifdef __amigaos4__
    যদি (!(id = AllocDosObject(DOS_INFODATA, 0)))
	গচ্ছ out;
# অন্যথা
    id = (সংরচনা InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    যদি (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    যদি (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (সংরচনা Window *)id->id_Volume_গ্ৰন্থি) == রিক্ত)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	গচ্ছ out;
    }
    যদি (oldwindowtitle == রিক্ত)
	oldwindowtitle = (char_u *)wb_window->Title;
    যদি (id->id_InUse == (BPTR)রিক্ত)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ঘূৰাই FAIL;
    }
    conUnit = (সংরচনা ConUnit *) ((সংরচনা IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    যদি (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	ঘূৰাই FAIL;
    }

    ঘূৰাই OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ঘূৰাই FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    শূন্য
mch_set_shellsize(শূন্য)
{
    যদি (!term_console)
	ঘূৰাই;

    size_set = TRUE;
    out_char(CSI);
    out_অঙ্ক((long)Rows);
    out_char('t');
    out_char(CSI);
    out_অঙ্ক((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    শূন্য
mch_new_shellsize(শূন্য)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    স্থির শূন্য
out_অঙ্ক(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#যদি !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    স্থির long
dos_packet(
    সংরচনা বাৰ্তা_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    সংরচনা বাৰ্তা_Port	    *replyport;
    সংরচনা StandardPacket   *packet;
    long		    res1;

    যদি (dos2)
# endif
	ঘূৰাই DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (সংরচনা বাৰ্তা_Port *) CreatePort(রিক্ত, 0);	// use arp function
    যদি (!replyport)
	ঘূৰাই (0);

    // Allocate space for a packet, make it public and clear it
    packet = (সংরচনা StandardPacket *)
	AllocMem((long) পরিমাণ(সংরচনা StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    যদি (!packet)
    {
	DeletePort(replyport);
	ঘূৰাই (0);
    }
    packet->sp_বাৰ্তা.mn_গ্ৰন্থি.ln_Name = (ৱর্ণ *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_বাৰ্তা);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_বাৰ্তা(pid, (সংরচনা Message *)packet);	// send packet

    WaitPort(replyport);
    Get_বাৰ্তা(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) পরিমাণ(সংরচনা StandardPacket));
    DeletePort(replyport);

    ঘূৰাই (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    পূর্ণাঙ্ক
mch_call_shell(
    char_u	*cmd,
    পূর্ণাঙ্ক		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    পূর্ণাঙ্ক		x;
    পূর্ণাঙ্ক		tmode = cur_tmode;
#ifdef AZTEC_C
    পূর্ণাঙ্ক		use_execute;
    char_u	*shellcmd = রিক্ত;
    char_u	*shellarg;
#endif
    পূর্ণাঙ্ক		retval = 0;

    যদি (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ঘূৰাই -1;
    }

    যদি (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    যদি (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#যদি !defined(AZTEC_C)		    // not tested very much
    যদি (cmd == রিক্ত)
    {
# ifdef FEAT_ARP
	যদি (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	অন্যথা
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    অন্যথা
    {
# ifdef FEAT_ARP
	যদি (dos2)
# endif
	    x = SystemTags((ৱর্ণ *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	অন্যথা
	    x = Execute((ৱর্ণ *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    যদি ((dos2 && x < 0) || (!dos2 && !x))
# অন্যথা
    যদি (x < 0)
# endif
    {
	বাৰ্তা_puts(_("Cannot execute "));
	যদি (cmd == রিক্ত)
	{
	    বাৰ্তা_puts(_("shell "));
	    বাৰ্তা_outtrans(p_sh);
	}
	অন্যথা
	    বাৰ্তা_outtrans(cmd);
	বাৰ্তা_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    অন্যথা যদি (!dos2 || x)
# অন্যথা
    অন্যথা যদি (x)
# endif
    {
	যদি ((x = IoErr()) != 0)
	{
	    যদি (!(options & SHELL_SILENT))
	    {
		বাৰ্তা_putchar('\n');
		বাৰ্তা_outnum((long)x);
		বাৰ্তা_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#অন্যথা	// else part is for AZTEC_C
    যদি (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    অন্যথা
	use_execute = 0;
    যদি (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	যদি (shellcmd == রিক্ত)	    // out of memory, use Execute
	    use_execute = 1;
	অন্যথা
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    যদি (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    যদি (cmd == রিক্ত)
    {
	যদি (use_execute)
	{
# ifdef FEAT_ARP
	    যদি (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    অন্যথা
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	অন্যথা
	    x = fexecl((ৱর্ণ *)shellcmd, (ৱর্ণ *)shellcmd, (ৱর্ণ *)shellarg, রিক্ত);
    }
    অন্যথা যদি (use_execute)
    {
# ifdef FEAT_ARP
	যদি (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	অন্যথা
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    অন্যথা যদি (p_st & 1)
	x = fexecl((ৱর্ণ *)shellcmd, (ৱর্ণ *)shellcmd, (ৱর্ণ *)shellarg,
							   (ৱর্ণ *)cmd, রিক্ত);
    অন্যথা
	x = fexecl((ৱর্ণ *)shellcmd, (ৱর্ণ *)shellcmd, (ৱর্ণ *)shellarg,
					   (ৱর্ণ *)p_shcf, (ৱর্ণ *)cmd, রিক্ত);
# ifdef FEAT_ARP
    যদি ((dos2 && x < 0) || (!dos2 && x))
# অন্যথা
    যদি (x < 0)
# endif
    {
	বাৰ্তা_puts(_("Cannot execute "));
	যদি (use_execute)
	{
	    যদি (cmd == রিক্ত)
		বাৰ্তা_outtrans(p_sh);
	    অন্যথা
		বাৰ্তা_outtrans(cmd);
	}
	অন্যথা
	{
	    বাৰ্তা_puts(_("shell "));
	    বাৰ্তা_outtrans(shellcmd);
	}
	বাৰ্তা_putchar('\n');
	retval = -1;
    }
    অন্যথা
    {
	যদি (use_execute)
	{
# ifdef FEAT_ARP
	    যদি (!dos2 || x)
# অন্যথা
	    যদি (x)
# endif
		x = IoErr();
	}
	অন্যথা
	    x = wait();
	যদি (x)
	{
	    যদি (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		বাৰ্তা_putchar('\n');
		বাৰ্তা_outnum((long)x);
		বাৰ্তা_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    যদি ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    যদি (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    যদি (term_console)
	win_resize_on();		// window resize events activated
    ঘূৰাই retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    শূন্য
mch_breakcheck(পূর্ণাঙ্ক force UNUSED)
{
   যদি (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
শূন্য __regargs __chkabort(শূন্য);

শূন্য __regargs __chkabort(শূন্য)
{}

#অন্যথা
    long
Chk_Abort(শূন্য)
{
    ঘূৰাই(0L);
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
#অন্যথা
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (পরিমাণ(সংরচনা AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    পূর্ণাঙ্ক
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    পূর্ণাঙ্ক		flags)		// EW_* flags
{
    সংরচনা AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    পূর্ণাঙ্ক			start_len;
    পূর্ণাঙ্ক			matches;
#ifdef __amigaos4__
    সংরচনা TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#অন্যথা
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    যদি (Anchor == রিক্ত)
	ঘূৰাই 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# অন্যথা
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    যদি (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	যদি (starbuf == রিক্ত)
	    গচ্ছ Return;
	বাবে (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    যদি (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    অন্যথা
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    অন্যথা
	Result = FindFirst((ৱর্ণ *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    যেতিয়ালৈ (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#অন্যথা
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	যদি (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	অন্যথা
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    যদি (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    অন্যথা যদি (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    যদি (matches)
	qsort((শূন্য *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, পরিমাণ(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    যদি (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    অন্যথা
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#অন্যথা
    vim_free(Anchor);
#endif

    ঘূৰাই matches;
}

    স্থির পূর্ণাঙ্ক
sortcmp(ধ্রুৱ শূন্য *a, ধ্রুৱ শূন্য *b)
{
    ৱর্ণ *s = *(ৱর্ণ **)a;
    ৱর্ণ *t = *(ৱর্ণ **)b;

    ঘূৰাই pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    পূর্ণাঙ্ক
mch_has_exp_wildcard(char_u *p)
{
    বাবে ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা যদি (vim_strchr((char_u *)"*?[(#", *p) != রিক্ত)
	    ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
}

    পূর্ণাঙ্ক
mch_has_wildcard(char_u *p)
{
    বাবে ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা
	    যদি (vim_strchr((char_u *)"*?[(#$`", *p) != রিক্ত
		    || (*p == '~' && p[1] != NUL))
		ঘূৰাই TRUE;
    }
    ঘূৰাই FALSE;
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
    পূর্ণাঙ্ক		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    স্থির char_u   *alloced = রিক্ত;	// allocated memory

#ifdef FEAT_ARP
    যদি (!dos2)
	retval = (char_u *)getenv((ৱর্ণ *)var);
    অন্যথা
#endif
    {
	VIM_CLEAR(alloced);
	retval = রিক্ত;

	buf = alloc(IOSIZE);
	যদি (buf == রিক্ত)
	    ঘূৰাই রিক্ত;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	যদি (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    যদি (retval == রিক্ত && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ঘূৰাই retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    পূর্ণাঙ্ক
mch_setenv(ৱর্ণ *var, ৱর্ণ *মান, পূর্ণাঙ্ক x UNUSED)
{
#ifdef FEAT_ARP
    যদি (!dos2)
	ঘূৰাই setenv(var, মান);
#endif

    যদি (SetVar((UBYTE *)var, (UBYTE *)মান, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ঘূৰাই 0;   // success
    ঘূৰাই -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    পূর্ণাঙ্ক
mch_get_random(char_u *buf, পূর্ণাঙ্ক len)
{
    সংরচনা Process *proc = (সংরচনা Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    পূর্ণাঙ্ক status;

    যদি (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    অন্যথা
	status = OK;

    Close(fh);
    ঘূৰাই status;
}
