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

#অন্তর্ভাব "vim.h"
#অন্তর্ভাব "version.h"

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

#অন্তর্ভাব <exec/memory.h>

#অন্তর্ভাব <dos/dostags.h>	    // for 2.0 functions
#অন্তর্ভাব <dos/dosasl.h>

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
স্থির ধ্রুবক অক্ষর* __attribute__((used)) stackcookie = "$STACK: 1048576";
#নতুবা_যদি defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#নতুবা_যদি defined(__GNUC__) && defined(AMIGA)
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
#নতুবা_যদি !defined(AZTEC_C) && !defined(__AROS__)
স্থির long dos_packet(কাঠামো বার্তা_Port *, long, long);
#endif
স্থির পূর্ণসংখ্যা lock2name(BPTR lock, char_u *buf, long   len);
স্থির শূন্য out_অঙ্ক(long n);
স্থির কাঠামো FileInfoBlock *get_fib(char_u *);
স্থির পূর্ণসংখ্যা sortcmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b);

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
    কাঠামো Process *me = (কাঠামো Process *)FindTask(ফাঁকা);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    ফেরত flock;
}

স্থির BPTR		raw_in = (BPTR)ফাঁকা;
স্থির BPTR		raw_out = (BPTR)ফাঁকা;
স্থির পূর্ণসংখ্যা		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
কাঠামো IntuitionBase	*IntuitionBase = ফাঁকা;
#endif
#ifdef FEAT_ARP
কাঠামো ArpBase		*ArpBase = ফাঁকা;
#endif

স্থির কাঠামো Window	*wb_window;
স্থির char_u		*oldwindowtitle = ফাঁকা;

#ifdef FEAT_ARP
পূর্ণসংখ্যা			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
পূর্ণসংখ্যা			আকার_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
স্থির অক্ষর version[] __attribute__((used)) =
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
mch_write(char_u *p, পূর্ণসংখ্যা len)
{
    Write(raw_out, (অক্ষর *)p, (long)len);
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
    পূর্ণসংখ্যা
mch_inchar(
    char_u  *buf,
    পূর্ণসংখ্যা	    maxlen,
    long    time,		// milliseconds
    পূর্ণসংখ্যা	    tb_change_cnt UNUSED)
{
    পূর্ণসংখ্যা	    len;
    long    utime;

    যদি (time >= 0)
    {
	যদি (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	অন্যথা
	    utime = time * 1000L;   // convert from milli to micro secs
	যদি (WaitForChar(raw_in, utime) == 0)	// no character available
	    ফেরত 0;
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
		buf[2] = (পূর্ণসংখ্যা)KE_CURSORHOLD;
		ফেরত 3;
	    }
	    before_blocking();
	}
    }

    জন্য (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (অক্ষর *)buf, (long)maxlen / প্রবেশ_conv.vc_factor);
	যদি (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    যদি (প্রবেশ_conv.vc_type != CONV_NONE)
		len = convert_প্রবেশ(buf, len, maxlen);
	    ফেরত len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    পূর্ণসংখ্যা
mch_char_avail(শূন্য)
{
    ফেরত (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(পূর্ণসংখ্যা special)
{
#যদি defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ফেরত (long_u)AvailMem(MEMF_ANY) >> 10;
#অন্যথা
    ফেরত (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    শূন্য
mch_delay(long msec, পূর্ণসংখ্যা flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    শূন্য	    Delay(long);
#endif

    যদি (msec <= 0)
	ফেরত;

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
    স্থির অক্ষর	    intlibname[] = "intuition.library";
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
	কাঠামো Process *me = (কাঠামো Process *)FindTask(ফাঁকা);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    যদি (raw_in == (BPTR)ফাঁকা)
    {
	raw_in = প্রবেশ_3();
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

    wb_window = ফাঁকা;
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    যদি ((IntuitionBase = (কাঠামো IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ফাঁকা)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#অন্তর্ভাব <workbench/startup.h>

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
#define BUF2_আকার 320	    // length of buffer for argument with complete path

    পূর্ণসংখ্যা
mch_check_win(পূর্ণসংখ্যা argc, অক্ষর **argv)
{
    পূর্ণসংখ্যা		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_আকার];
    স্থির char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    স্থির char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    কাঠামো WBArg    *argp;
    পূর্ণসংখ্যা		    ac;
    অক্ষর	    *av;
    char_u	    *device = ফাঁকা;
    পূর্ণসংখ্যা		    exitval = 4;
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    কাঠামো Library  *DosBase;
#endif
    পূর্ণসংখ্যা		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    যদি (DosBase != ফাঁকা)
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
	যদি (!(ArpBase = (কাঠামো ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    জন্য (i = 1; i < argc; ++i)
	যদি (argv[i][0] == '-')
	{
	    নির্বাচন (argv[i][1])
	    {
	    ক্ষেত্র 'f':
		usewin = TRUE;
		বিরতি;

	    ক্ষেত্র 'd':
		যদি (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		বিরতি;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    যদি (argc != 0
	    && device == ফাঁকা
	    && (IsInteractive(প্রবেশ_3()) || IsInteractive(Output())))
	ফেরত OK;

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
	জন্য (i = -1; i < 3; ++i)
	{
	    যদি (i >= 0)
		device = constrings[i];
	    যদি (device != ফাঁকা && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)ফাঁকা)
		বিরতি;
	}
	যদি (raw_in == (BPTR)ফাঁকা)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    যাও exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	ফেরত OK;
    }

    যদি ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)ফাঁকা)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	যাও exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#যদি !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((অক্ষর *)buf1, "t:nc%p", FindTask(0));
#অন্যথা
    sprintf((অক্ষর *)buf1, "t:nc%ld", (long)buf1);
#endif
    যদি ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)ফাঁকা)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((অক্ষর *)buf1);
	mch_errmsg("\n");
	যাও exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    যদি (argc == 0)	// run from workbench
	ac = ((কাঠামো WBStartup *)argv)->sm_অঙ্ক_Args;
    অন্যথা
	ac = argc;
    জন্য (i = 0; i < ac; ++i)
    {
	যদি (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((কাঠামো WBStartup *)argv)->sm_ArgList[i]);
	    যদি (argp->wa_Lock)
		(শূন্য)lock2name(argp->wa_Lock, buf2, (long)(BUF2_আকার - 1));
#ifdef FEAT_ARP
	    যদি (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2_আকার - 1));
#ifdef FEAT_ARP
	    অন্যথা	    // use arp function
		TackOn((অক্ষর *)buf2, argp->wa_Name);
#endif
	    av = (অক্ষর *)buf2;
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
	    চালু;
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
    জন্য (i = -1; i < 3; ++i)
    {
	যদি (i >= 0)
	    device = constrings[i];
	অন্যথা যদি (device == ফাঁকা)
	    চালু;
	sprintf((অক্ষর *)buf2, "newcli <nil: >nil: %s from %s", (অক্ষর *)device, (অক্ষর *)buf1);
#ifdef FEAT_ARP
	যদি (dos2)
	{
#endif
	    যদি (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		বিরতি;
#ifdef FEAT_ARP
	}
	অন্যথা
	{
	    যদি (Execute((UBYTE *)buf2, nilfh, nilfh))
		বিরতি;
	}
#endif
    }
    যদি (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	যাও exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    যদি (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    যদি (ArpBase)
	CloseLibrary((কাঠামো Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    ফেরত FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    পূর্ণসংখ্যা
mch_প্রবেশ_isatty(শূন্য)
{
    ফেরত TRUE;
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
    পূর্ণসংখ্যা		len UNUSED)		// buffer size, ignored here
{
    কাঠামো FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    যদি (fib == ফাঁকা)
	ফেরত;

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
    স্থির কাঠামো FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    কাঠামো FileInfoBlock    *fib;

    যদি (fname == ফাঁকা)	    // safety check
	ফেরত ফাঁকা;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#অন্যথা
    fib = ALLOC_ONE(কাঠামো FileInfoBlock);
#endif
    যদি (fib == ফাঁকা)
	ফেরত;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    যদি (flock == (BPTR)ফাঁকা || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ফাঁকা;
    }
    যদি (flock)
	UnLock(flock);
    ফেরত fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    শূন্য
mch_settitle(char_u *title, char_u *icon)
{
    যদি (wb_window != ফাঁকা && title != ফাঁকা)
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
mch_restore_title(পূর্ণসংখ্যা which)
{
    যদি (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ফাঁকা);
}

    পূর্ণসংখ্যা
mch_can_restore_title(শূন্য)
{
    ফেরত (wb_window != ফাঁকা);
}

    পূর্ণসংখ্যা
mch_can_restore_icon(শূন্য)
{
    ফেরত FALSE;
}

    শূন্য
mch_setmouse(পূর্ণসংখ্যা on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    পূর্ণসংখ্যা
mch_get_user_name(char_u *s, পূর্ণসংখ্যা len)
{
#যদি defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    কাঠামো passwd   *pwd = getpwuid(getuid());

    যদি (pwd != ফাঁকা && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ফেরত OK;
    }
#endif
    *s = NUL;
    ফেরত FAIL;
}

/*
 * Insert host name is s[len].
 */
    শূন্য
mch_get_host_name(char_u *s, পূর্ণসংখ্যা len)
{
#যদি defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((অক্ষর *)s, len);
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
    ফেরত (long) getpid();
#নতুবা_যদি defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ফেরত((long)FindTask(0));
#অন্যথা
    ফেরত (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    পূর্ণসংখ্যা
mch_dirname(char_u *buf, পূর্ণসংখ্যা len)
{
    ফেরত mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    পূর্ণসংখ্যা		len,
    পূর্ণসংখ্যা		force)
{
    BPTR	l;
    পূর্ণসংখ্যা		retval = FAIL;
    পূর্ণসংখ্যা		i;

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
	যদি ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)ফাঁকা)
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
    ফেরত retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    পূর্ণসংখ্যা
mch_isFullName(char_u *fname)
{
    ফেরত (vim_strchr(fname, ':') != ফাঁকা && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    স্থির পূর্ণসংখ্যা
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    যদি (dos2)		    // use 2.0 function
#endif
	ফেরত ((পূর্ণসংখ্যা)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    অন্যথা		// use arp function
	ফেরত ((পূর্ণসংখ্যা)PathName(lock, (অক্ষর *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    কাঠামো FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    যদি (fib == ফাঁকা)
	ফেরত -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ফেরত retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ফেরত (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
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
    পূর্ণসংখ্যা
mch_isdir(char_u *name)
{
    কাঠামো FileInfoBlock    *fib;
    পূর্ণসংখ্যা			    retval = FALSE;

    fib = get_fib(name);
    যদি (fib == ফাঁকা)
	ফেরত FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#অন্যথা
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    ফেরত retval;
}

/*
 * Create directory "name".
 */
    পূর্ণসংখ্যা
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    যদি (lock == ফাঁকা)
	ফেরত -1;

    UnLock(lock);
    ফেরত 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    পূর্ণসংখ্যা
mch_can_exe(char_u *name, char_u **path UNUSED, পূর্ণসংখ্যা use_path)
{
    পূর্ণসংখ্যা exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    যদি (seg && GetSegListInfoTags(seg, GSLI_Native, ফাঁকা, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, ফাঁকা, TAG_DONE))
    {
	// Test if file permissions allow execution.
	কাঠামো Examine_তথ্য *exd = ExamineObjectTags(EX_String_Name_প্রবেশ, name);

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
	কাঠামো Path_গ্রন্থি *শীর্ষ = DupCmdPathList(ফাঁকা);

	// For each entry, recur to check for executable.
	জন্য (কাঠামো Path_গ্রন্থি *লেজ = শীর্ষ; !exe && লেজ;
			       লেজ = (কাঠামো Path_গ্রন্থি *) BADDR(লেজ->pn_পরবর্তী))
	{
	    SetCurrentDir(লেজ->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(শীর্ষ);
	SetCurrentDir(cwd);
    }
#endif
    ফেরত exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    পূর্ণসংখ্যা
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    ফেরত গ্রন্থি_NORMAL;
}

    শূন্য
mch_early_init(শূন্য)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    শূন্য
mch_exit(পূর্ণসংখ্যা r)
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
	    যদি (আকার_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    যদি (ArpBase)
	CloseLibrary((কাঠামো Library *) ArpBase);
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

#define MP(xx)	((কাঠামো বার্তা_Port *)((কাঠামো FileHandle *) (BADDR(xx)))->fh_Type)

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

#অন্তর্ভাব <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#যদি defined(__AROS__) || defined(__MORPHOS__)
    পূর্ণসংখ্যা
mch_get_shellsize(শূন্য)
{
    যদি (!term_console)
	ফেরত FAIL;

    যদি (raw_in && raw_out)
    {
	// Save current console mode.
	পূর্ণসংখ্যা old_tmode = cur_tmode;
	অক্ষর ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	যদি (Write(raw_out, ctrl, আকার(ctrl)) == আকার(ctrl))
	{
	    অক্ষর scan[] = "\x9b""1;1;%d;%d r",
		 answ[আকার(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    যদি (Read(raw_in, answ, আকার(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		যদি (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ফেরত OK;
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

    ফেরত FAIL;
}
#অন্যথা
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    পূর্ণসংখ্যা
mch_get_shellsize(শূন্য)
{
    কাঠামো ConUnit  *conUnit;
# ifndef __amigaos4__
    অক্ষর	    id_a[আকার(কাঠামো Info_তথ্য) + 3];
# endif
    কাঠামো Info_তথ্য *id=0;

    যদি (!term_console)	// not an amiga window
	যাও out;

    // insure longword alignment
# ifdef __amigaos4__
    যদি (!(id = AllocDosObject(DOS_INFODATA, 0)))
	যাও out;
# অন্যথা
    id = (কাঠামো Info_তথ্য *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    যদি (আকার_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    যদি (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (কাঠামো Window *)id->id_Volume_গ্রন্থি) == ফাঁকা)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	যাও out;
    }
    যদি (oldwindowtitle == ফাঁকা)
	oldwindowtitle = (char_u *)wb_window->Title;
    যদি (id->id_InUse == (BPTR)ফাঁকা)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ফেরত FAIL;
    }
    conUnit = (কাঠামো ConUnit *) ((কাঠামো IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    যদি (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	ফেরত FAIL;
    }

    ফেরত OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ফেরত FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    শূন্য
mch_set_shellsize(শূন্য)
{
    যদি (!term_console)
	ফেরত;

    আকার_set = TRUE;
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
    কাঠামো বার্তা_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    কাঠামো বার্তা_Port	    *replyport;
    কাঠামো StandardPacket   *packet;
    long		    res1;

    যদি (dos2)
# endif
	ফেরত DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (কাঠামো বার্তা_Port *) CreatePort(ফাঁকা, 0);	// use arp function
    যদি (!replyport)
	ফেরত (0);

    // Allocate space for a packet, make it public and clear it
    packet = (কাঠামো StandardPacket *)
	AllocMem((long) আকার(কাঠামো StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    যদি (!packet)
    {
	DeletePort(replyport);
	ফেরত (0);
    }
    packet->sp_বার্তা.mn_গ্রন্থি.ln_Name = (অক্ষর *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_বার্তা);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_বার্তা(pid, (কাঠামো Message *)packet);	// send packet

    WaitPort(replyport);
    Get_বার্তা(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) আকার(কাঠামো StandardPacket));
    DeletePort(replyport);

    ফেরত (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    পূর্ণসংখ্যা
mch_call_shell(
    char_u	*cmd,
    পূর্ণসংখ্যা		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    পূর্ণসংখ্যা		x;
    পূর্ণসংখ্যা		tmode = cur_tmode;
#ifdef AZTEC_C
    পূর্ণসংখ্যা		use_execute;
    char_u	*shellcmd = ফাঁকা;
    char_u	*shellarg;
#endif
    পূর্ণসংখ্যা		retval = 0;

    যদি (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ফেরত -1;
    }

    যদি (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    যদি (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#যদি !defined(AZTEC_C)		    // not tested very much
    যদি (cmd == ফাঁকা)
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
	    x = SystemTags((অক্ষর *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	অন্যথা
	    x = Execute((অক্ষর *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    যদি ((dos2 && x < 0) || (!dos2 && !x))
# অন্যথা
    যদি (x < 0)
# endif
    {
	বার্তা_puts(_("Cannot execute "));
	যদি (cmd == ফাঁকা)
	{
	    বার্তা_puts(_("shell "));
	    বার্তা_outtrans(p_sh);
	}
	অন্যথা
	    বার্তা_outtrans(cmd);
	বার্তা_putchar('\n');
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
		বার্তা_putchar('\n');
		বার্তা_outnum((long)x);
		বার্তা_puts(_(" returned\n"));
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
	যদি (shellcmd == ফাঁকা)	    // out of memory, use Execute
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
    যদি (cmd == ফাঁকা)
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
	    x = fexecl((অক্ষর *)shellcmd, (অক্ষর *)shellcmd, (অক্ষর *)shellarg, ফাঁকা);
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
	x = fexecl((অক্ষর *)shellcmd, (অক্ষর *)shellcmd, (অক্ষর *)shellarg,
							   (অক্ষর *)cmd, ফাঁকা);
    অন্যথা
	x = fexecl((অক্ষর *)shellcmd, (অক্ষর *)shellcmd, (অক্ষর *)shellarg,
					   (অক্ষর *)p_shcf, (অক্ষর *)cmd, ফাঁকা);
# ifdef FEAT_ARP
    যদি ((dos2 && x < 0) || (!dos2 && x))
# অন্যথা
    যদি (x < 0)
# endif
    {
	বার্তা_puts(_("Cannot execute "));
	যদি (use_execute)
	{
	    যদি (cmd == ফাঁকা)
		বার্তা_outtrans(p_sh);
	    অন্যথা
		বার্তা_outtrans(cmd);
	}
	অন্যথা
	{
	    বার্তা_puts(_("shell "));
	    বার্তা_outtrans(shellcmd);
	}
	বার্তা_putchar('\n');
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
		বার্তা_putchar('\n');
		বার্তা_outnum((long)x);
		বার্তা_puts(_(" returned\n"));
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
    ফেরত retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    শূন্য
mch_breakcheck(পূর্ণসংখ্যা force UNUSED)
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
    ফেরত(0L);
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
# define	ANCHOR_BUF_আকার	1024
#অন্যথা
# define ANCHOR_BUF_আকার (512)
# define ANCHOR_আকার (আকার(কাঠামো AnchorPath) + ANCHOR_BUF_আকার)
#endif

    পূর্ণসংখ্যা
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    পূর্ণসংখ্যা		flags)		// EW_* flags
{
    কাঠামো AnchorPath	*Anchor;
    LONG		ফলাফল_2;
    char_u		*starbuf, *sp, *dp;
    পূর্ণসংখ্যা			start_len;
    পূর্ণসংখ্যা			matches;
#ifdef __amigaos4__
    কাঠামো TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_আকার},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#অন্যথা
    Anchor = alloc_clear(ANCHOR_আকার);
#endif
    যদি (Anchor == ফাঁকা)
	ফেরত 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_আকার;  // ap_Length not supported anymore
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
	যদি (starbuf == ফাঁকা)
	    যাও Return;
	জন্য (sp = pat, dp = starbuf; *sp; ++sp)
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
	ফলাফল_2 = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    অন্যথা
	ফলাফল_2 = FindFirst((অক্ষর *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    যখন (ফলাফল_2 == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_বাফার, flags);
#অন্যথা
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	যদি (dos2)
#endif
	    ফলাফল_2 = Match_পরবর্তী(Anchor);
#ifdef FEAT_ARP
	অন্যথা
	    ফলাফল_2 = Find_পরবর্তী(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    যদি (ফলাফল_2 == ERROR_বাফার_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    অন্যথা যদি (matches == 0 && ফলাফল_2 != ERROR_OBJECT_NOT_FOUND
			  && ফলাফল_2 != ERROR_DEVICE_NOT_MOUNTED
			  && ফলাফল_2 != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    যদি (matches)
	qsort((শূন্য *)(((char_u **)gap->ga_তথ্য) + start_len),
				  (size_t)matches, আকার(char_u *), sortcmp);

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

    ফেরত matches;
}

    স্থির পূর্ণসংখ্যা
sortcmp(ধ্রুবক শূন্য *a, ধ্রুবক শূন্য *b)
{
    অক্ষর *s = *(অক্ষর **)a;
    অক্ষর *t = *(অক্ষর **)b;

    ফেরত pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    পূর্ণসংখ্যা
mch_has_exp_wildcard(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা যদি (vim_strchr((char_u *)"*?[(#", *p) != ফাঁকা)
	    ফেরত TRUE;
    }
    ফেরত FALSE;
}

    পূর্ণসংখ্যা
mch_has_wildcard(char_u *p)
{
    জন্য ( ; *p; MB_PTR_ADV(p))
    {
	যদি (*p == '\\' && p[1] != NUL)
	    ++p;
	অন্যথা
	    যদি (vim_strchr((char_u *)"*?[(#$`", *p) != ফাঁকা
		    || (*p == '~' && p[1] != NUL))
		ফেরত TRUE;
    }
    ফেরত FALSE;
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
    পূর্ণসংখ্যা		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    স্থির char_u   *alloced = ফাঁকা;	// allocated memory

#ifdef FEAT_ARP
    যদি (!dos2)
	retval = (char_u *)getenv((অক্ষর *)var);
    অন্যথা
#endif
    {
	VIM_CLEAR(alloced);
	retval = ফাঁকা;

	buf = alloc(IOSIZE);
	যদি (buf == ফাঁকা)
	    ফেরত ফাঁকা;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	যদি (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    যদি (retval == ফাঁকা && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ফেরত retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    পূর্ণসংখ্যা
mch_setenv(অক্ষর *var, অক্ষর *মান, পূর্ণসংখ্যা x UNUSED)
{
#ifdef FEAT_ARP
    যদি (!dos2)
	ফেরত setenv(var, মান);
#endif

    যদি (SetVar((UBYTE *)var, (UBYTE *)মান, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ফেরত 0;   // success
    ফেরত -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    পূর্ণসংখ্যা
mch_get_random(char_u *buf, পূর্ণসংখ্যা len)
{
    কাঠামো Process *proc = (কাঠামো Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    পূর্ণসংখ্যা status;

    যদি (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    অন্যথা
	status = OK;

    Close(fh);
    ফেরত status;
}
