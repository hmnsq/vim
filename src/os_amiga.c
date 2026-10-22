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

#شامل "vim.h"
#شامل "version.h"

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

#شامل <exec/memory.h>

#شامل <dos/dostags.h>	    // for 2.0 functions
#شامل <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#وگرنه
# define	free_fib(x) vim_free(fib)
#endif

#اگر defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#اگر defined(__amigaos4__)
ایستا ثابت نویسه* __attribute__((used)) stackcookie = "$STACK: 1048576";
#وگرنهاگر defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#وگرنهاگر defined(__GNUC__) && defined(AMIGA)
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
#وگرنهاگر !defined(AZTEC_C) && !defined(__AROS__)
ایستا long dos_packet(ساختار پیام_Port *, long, long);
#endif
ایستا صحیح lock2name(BPTR lock, char_u *buf, long   len);
ایستا تهی out_رقم(long n);
ایستا ساختار FileInfoBlock *get_fib(char_u *);
ایستا صحیح sortcmp(ثابت تهی *a, ثابت تهی *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ایستا BPTR
safe_Lock(UBYTE *name, long mode)
{
    ساختار Process *me = (ساختار Process *)FindTask(پوچ);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    بازگشت flock;
}

ایستا BPTR		raw_in = (BPTR)پوچ;
ایستا BPTR		raw_out = (BPTR)پوچ;
ایستا صحیح		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ساختار IntuitionBase	*IntuitionBase = پوچ;
#endif
#ifdef FEAT_ARP
ساختار ArpBase		*ArpBase = پوچ;
#endif

ایستا ساختار Window	*wb_window;
ایستا char_u		*oldwindowtitle = پوچ;

#ifdef FEAT_ARP
صحیح			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
صحیح			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
ایستا نویسه version[] __attribute__((used)) =
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

    تهی
win_resize_on(تهی)
{
    OUT_STR_NF("\033[12{");
}

    تهی
win_resize_off(تهی)
{
    OUT_STR_NF("\033[12}");
}

    تهی
mch_write(char_u *p, صحیح len)
{
    Write(raw_out, (نویسه *)p, (long)len);
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
    صحیح
mch_inchar(
    char_u  *buf,
    صحیح	    maxlen,
    long    time,		// milliseconds
    صحیح	    tb_change_cnt UNUSED)
{
    صحیح	    len;
    long    utime;

    اگر (time >= 0)
    {
	اگر (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	وگرنه
	    utime = time * 1000L;   // convert from milli to micro secs
	اگر (WaitForChar(raw_in, utime) == 0)	// no character available
	    بازگشت 0;
    }
    وگرنه    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	اگر (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    اگر (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحیح)KE_CURSORHOLD;
		بازگشت 3;
	    }
	    before_blocking();
	}
    }

    برای (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (نویسه *)buf, (long)maxlen / درون_داد_conv.vc_factor);
	اگر (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    اگر (درون_داد_conv.vc_type != CONV_NONE)
		len = convert_درون_داد(buf, len, maxlen);
	    بازگشت len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    صحیح
mch_char_avail(تهی)
{
    بازگشت (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(صحیح special)
{
#اگر defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    بازگشت (long_u)AvailMem(MEMF_ANY) >> 10;
#وگرنه
    بازگشت (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    تهی
mch_delay(long msec, صحیح flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    تهی	    Delay(long);
#endif

    اگر (msec <= 0)
	بازگشت;

    اگر (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    وگرنه
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    تهی
mch_suspend(تهی)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    تهی
mch_init(تهی)
{
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ایستا نویسه	    intlibname[] = "intuition.library";
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
	ساختار Process *me = (ساختار Process *)FindTask(پوچ);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    اگر (raw_in == (BPTR)پوچ)
    {
	raw_in = درون_داد_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	اگر (!IsInteractive(raw_in))
	    raw_in = raw_out;
	وگرنه اگر (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = پوچ;
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    اگر ((IntuitionBase = (ساختار IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == پوچ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#شامل <workbench/startup.h>

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

    صحیح
mch_check_win(صحیح argc, نویسه **argv)
{
    صحیح		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    ایستا char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ایستا char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ساختار WBArg    *argp;
    صحیح		    ac;
    نویسه	    *av;
    char_u	    *device = پوچ;
    صحیح		    exitval = 4;
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساختار Library  *DosBase;
#endif
    صحیح		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    اگر (DosBase != پوچ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    وگرنه	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# وگرنه
		    // need arp functions for dos 1.x
	اگر (!(ArpBase = (ساختار ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    برای (i = 1; i < argc; ++i)
	اگر (argv[i][0] == '-')
	{
	    انتخاب (argv[i][1])
	    {
	    حالت 'f':
		usewin = TRUE;
		بشکن;

	    حالت 'd':
		اگر (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		بشکن;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    اگر (argc != 0
	    && device == پوچ
	    && (IsInteractive(درون_داد_3()) || IsInteractive(Output())))
	بازگشت OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    اگر (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	برای (i = -1; i < 3; ++i)
	{
	    اگر (i >= 0)
		device = constrings[i];
	    اگر (device != پوچ && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)پوچ)
		بشکن;
	}
	اگر (raw_in == (BPTR)پوچ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    برو exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	بازگشت OK;
    }

    اگر ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)پوچ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	برو exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((نویسه *)buf1, "t:nc%p", FindTask(0));
#وگرنه
    sprintf((نویسه *)buf1, "t:nc%ld", (long)buf1);
#endif
    اگر ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)پوچ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((نویسه *)buf1);
	mch_errmsg("\n");
	برو exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    اگر (argc == 0)	// run from workbench
	ac = ((ساختار WBStartup *)argv)->sm_رقم_Args;
    وگرنه
	ac = argc;
    برای (i = 0; i < ac; ++i)
    {
	اگر (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ساختار WBStartup *)argv)->sm_ArgList[i]);
	    اگر (argp->wa_Lock)
		(تهی)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    اگر (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    وگرنه	    // use arp function
		TackOn((نویسه *)buf2, argp->wa_Name);
#endif
	    av = (نویسه *)buf2;
	}
	وگرنه
	    av = argv[i];

	// skip '-d' or "-dev" option
	اگر (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    ادامه;
	}
	اگر (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	اگر (vim_strchr((char_u *)av, ' '))
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
    برای (i = -1; i < 3; ++i)
    {
	اگر (i >= 0)
	    device = constrings[i];
	وگرنه اگر (device == پوچ)
	    ادامه;
	sprintf((نویسه *)buf2, "newcli <nil: >nil: %s from %s", (نویسه *)device, (نویسه *)buf1);
#ifdef FEAT_ARP
	اگر (dos2)
	{
#endif
	    اگر (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		بشکن;
#ifdef FEAT_ARP
	}
	وگرنه
	{
	    اگر (Execute((UBYTE *)buf2, nilfh, nilfh))
		بشکن;
	}
#endif
    }
    اگر (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	برو exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    اگر (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    اگر (ArpBase)
	CloseLibrary((ساختار Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    بازگشت FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    صحیح
mch_درون_داد_isatty(تهی)
{
    بازگشت TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    تهی
fname_case(
    char_u	*name,
    صحیح		len UNUSED)		// buffer size, ignored here
{
    ساختار FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    اگر (fib == پوچ)
	بازگشت;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    اگر (fib->fib_DirEntryType == ST_ریشه)
	strcat(fib->fib_FileName, ":");
#endif
    اگر (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ایستا ساختار FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ساختار FileInfoBlock    *fib;

    اگر (fname == پوچ)	    // safety check
	بازگشت پوچ;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#وگرنه
    fib = ALLOC_ONE(ساختار FileInfoBlock);
#endif
    اگر (fib == پوچ)
	بازگشت;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    اگر (flock == (BPTR)پوچ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = پوچ;
    }
    اگر (flock)
	UnLock(flock);
    بازگشت fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    تهی
mch_settitle(char_u *title, char_u *icon)
{
    اگر (wb_window != پوچ && title != پوچ)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    تهی
mch_restore_title(صحیح which)
{
    اگر (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, پوچ);
}

    صحیح
mch_can_restore_title(تهی)
{
    بازگشت (wb_window != پوچ);
}

    صحیح
mch_can_restore_icon(تهی)
{
    بازگشت FALSE;
}

    تهی
mch_setmouse(صحیح on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    صحیح
mch_get_user_name(char_u *s, صحیح len)
{
#اگر defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ساختار passwd   *pwd = getpwuid(getuid());

    اگر (pwd != پوچ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	بازگشت OK;
    }
#endif
    *s = NUL;
    بازگشت FAIL;
}

/*
 * Insert host name is s[len].
 */
    تهی
mch_get_host_name(char_u *s, صحیح len)
{
#اگر defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((نویسه *)s, len);
#وگرنه
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(تهی)
{
#اگر defined(__amigaos4__)
    بازگشت (long) getpid();
#وگرنهاگر defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    بازگشت((long)FindTask(0));
#وگرنه
    بازگشت (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحیح
mch_dirname(char_u *buf, صحیح len)
{
    بازگشت mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)
{
    BPTR	l;
    صحیح		retval = FAIL;
    صحیح		i;

    // Lock the file.  If it exists, we can get the exact name.
    اگر ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    وگرنه اگر (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	اگر ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)پوچ)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    اگر (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		اگر (i == 0 || *fname != NUL)
		{
		    اگر (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    اگر (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    بازگشت retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_isFullName(char_u *fname)
{
    بازگشت (vim_strchr(fname, ':') != پوچ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ایستا صحیح
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    اگر (dos2)		    // use 2.0 function
#endif
	بازگشت ((صحیح)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    وگرنه		// use arp function
	بازگشت ((صحیح)PathName(lock, (نویسه *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    ساختار FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    اگر (fib == پوچ)
	بازگشت -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    بازگشت retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    بازگشت (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    تهی
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    صحیح
mch_isdir(char_u *name)
{
    ساختار FileInfoBlock    *fib;
    صحیح			    retval = FALSE;

    fib = get_fib(name);
    اگر (fib == پوچ)
	بازگشت FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#وگرنه
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    بازگشت retval;
}

/*
 * Create directory "name".
 */
    صحیح
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    اگر (lock == پوچ)
	بازگشت -1;

    UnLock(lock);
    بازگشت 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحیح
mch_can_exe(char_u *name, char_u **path UNUSED, صحیح use_path)
{
    صحیح exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    اگر (seg && GetSegListInfoTags(seg, GSLI_Native, پوچ, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, پوچ, TAG_DONE))
    {
	// Test if file permissions allow execution.
	ساختار ExamineData *exd = ExamineObjectTags(EX_String_Name_درون_داد, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    وگرنه
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    اگر (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ساختار Path_گره *head = DupCmdPathList(پوچ);

	// For each entry, recur to check for executable.
	برای (ساختار Path_گره *tail = head; !exe && tail;
			       tail = (ساختار Path_گره *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    بازگشت exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحیح
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    بازگشت گره_NORMAL;
}

    تهی
mch_early_init(تهی)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    تهی
mch_exit(صحیح r)
{
    exiting = TRUE;

    اگر (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    اگر (raw_out)
    {
	اگر (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    اگر (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    اگر (ArpBase)
	CloseLibrary((ساختار Library *) ArpBase);
#endif
    اگر (close_win)
	Close(raw_in);
    اگر (r)
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

#define MP(xx)	((ساختار پیام_Port *)((ساختار FileHandle *) (BADDR(xx)))->fh_Type)

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
    تهی
mch_settmode(tmode_T tmode)
{
#اگر defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    اگر (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#وگرنه
    اگر (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#شامل <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#اگر defined(__AROS__) || defined(__MORPHOS__)
    صحیح
mch_get_shellsize(تهی)
{
    اگر (!term_console)
	بازگشت FAIL;

    اگر (raw_in && raw_out)
    {
	// Save current console mode.
	صحیح old_tmode = cur_tmode;
	نویسه ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	اگر (Write(raw_out, ctrl, اندازه(ctrl)) == اندازه(ctrl))
	{
	    نویسه scan[] = "\x9b""1;1;%d;%d r",
		 answ[اندازه(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    اگر (Read(raw_in, answ, اندازه(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		اگر (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    بازگشت OK;
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

    بازگشت FAIL;
}
#وگرنه
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_get_shellsize(تهی)
{
    ساختار ConUnit  *conUnit;
# ifndef __amigaos4__
    نویسه	    id_a[اندازه(ساختار InfoData) + 3];
# endif
    ساختار InfoData *id=0;

    اگر (!term_console)	// not an amiga window
	برو out;

    // insure longword alignment
# ifdef __amigaos4__
    اگر (!(id = AllocDosObject(DOS_INFODATA, 0)))
	برو out;
# وگرنه
    id = (ساختار InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    اگر (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    اگر (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (ساختار Window *)id->id_Volume_گره) == پوچ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	برو out;
    }
    اگر (oldwindowtitle == پوچ)
	oldwindowtitle = (char_u *)wb_window->Title;
    اگر (id->id_InUse == (BPTR)پوچ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	بازگشت FAIL;
    }
    conUnit = (ساختار ConUnit *) ((ساختار IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    اگر (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	بازگشت FAIL;
    }

    بازگشت OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    بازگشت FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    تهی
mch_set_shellsize(تهی)
{
    اگر (!term_console)
	بازگشت;

    size_set = TRUE;
    out_char(CSI);
    out_رقم((long)Rows);
    out_char('t');
    out_char(CSI);
    out_رقم((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    تهی
mch_new_shellsize(تهی)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ایستا تهی
out_رقم(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#اگر !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    ایستا long
dos_packet(
    ساختار پیام_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    ساختار پیام_Port	    *replyport;
    ساختار StandardPacket   *packet;
    long		    res1;

    اگر (dos2)
# endif
	بازگشت DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ساختار پیام_Port *) CreatePort(پوچ, 0);	// use arp function
    اگر (!replyport)
	بازگشت (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ساختار StandardPacket *)
	AllocMem((long) اندازه(ساختار StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    اگر (!packet)
    {
	DeletePort(replyport);
	بازگشت (0);
    }
    packet->sp_پیام.mn_گره.ln_Name = (نویسه *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_پیام);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_پیام(pid, (ساختار Message *)packet);	// send packet

    WaitPort(replyport);
    Get_پیام(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) اندازه(ساختار StandardPacket));
    DeletePort(replyport);

    بازگشت (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    صحیح
mch_call_shell(
    char_u	*cmd,
    صحیح		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    صحیح		x;
    صحیح		tmode = cur_tmode;
#ifdef AZTEC_C
    صحیح		use_execute;
    char_u	*shellcmd = پوچ;
    char_u	*shellarg;
#endif
    صحیح		retval = 0;

    اگر (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	بازگشت -1;
    }

    اگر (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#اگر !defined(AZTEC_C)		    // not tested very much
    اگر (cmd == پوچ)
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وگرنه
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    وگرنه
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = SystemTags((نویسه *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وگرنه
	    x = Execute((نویسه *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    اگر ((dos2 && x < 0) || (!dos2 && !x))
# وگرنه
    اگر (x < 0)
# endif
    {
	پیام_puts(_("Cannot execute "));
	اگر (cmd == پوچ)
	{
	    پیام_puts(_("shell "));
	    پیام_outtrans(p_sh);
	}
	وگرنه
	    پیام_outtrans(cmd);
	پیام_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    وگرنه اگر (!dos2 || x)
# وگرنه
    وگرنه اگر (x)
# endif
    {
	اگر ((x = IoErr()) != 0)
	{
	    اگر (!(options & SHELL_SILENT))
	    {
		پیام_putchar('\n');
		پیام_outnum((long)x);
		پیام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#وگرنه	// else part is for AZTEC_C
    اگر (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    وگرنه
	use_execute = 0;
    اگر (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	اگر (shellcmd == پوچ)	    // out of memory, use Execute
	    use_execute = 1;
	وگرنه
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    اگر (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    اگر (cmd == پوچ)
    {
	اگر (use_execute)
	{
# ifdef FEAT_ARP
	    اگر (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    وگرنه
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	وگرنه
	    x = fexecl((نویسه *)shellcmd, (نویسه *)shellcmd, (نویسه *)shellarg, پوچ);
    }
    وگرنه اگر (use_execute)
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وگرنه
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    وگرنه اگر (p_st & 1)
	x = fexecl((نویسه *)shellcmd, (نویسه *)shellcmd, (نویسه *)shellarg,
							   (نویسه *)cmd, پوچ);
    وگرنه
	x = fexecl((نویسه *)shellcmd, (نویسه *)shellcmd, (نویسه *)shellarg,
					   (نویسه *)p_shcf, (نویسه *)cmd, پوچ);
# ifdef FEAT_ARP
    اگر ((dos2 && x < 0) || (!dos2 && x))
# وگرنه
    اگر (x < 0)
# endif
    {
	پیام_puts(_("Cannot execute "));
	اگر (use_execute)
	{
	    اگر (cmd == پوچ)
		پیام_outtrans(p_sh);
	    وگرنه
		پیام_outtrans(cmd);
	}
	وگرنه
	{
	    پیام_puts(_("shell "));
	    پیام_outtrans(shellcmd);
	}
	پیام_putchar('\n');
	retval = -1;
    }
    وگرنه
    {
	اگر (use_execute)
	{
# ifdef FEAT_ARP
	    اگر (!dos2 || x)
# وگرنه
	    اگر (x)
# endif
		x = IoErr();
	}
	وگرنه
	    x = wait();
	اگر (x)
	{
	    اگر (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		پیام_putchar('\n');
		پیام_outnum((long)x);
		پیام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    اگر ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    اگر (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    اگر (term_console)
	win_resize_on();		// window resize events activated
    بازگشت retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    تهی
mch_breakcheck(صحیح force UNUSED)
{
   اگر (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
تهی __regargs __chkabort(تهی);

تهی __regargs __chkabort(تهی)
{}

#وگرنه
    long
Chk_Abort(تهی)
{
    بازگشت(0L);
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
#وگرنه
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (اندازه(ساختار AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    صحیح
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    صحیح		flags)		// EW_* flags
{
    ساختار AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    صحیح			start_len;
    صحیح			matches;
#ifdef __amigaos4__
    ساختار TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#وگرنه
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    اگر (Anchor == پوچ)
	بازگشت 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# وگرنه
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    اگر (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	اگر (starbuf == پوچ)
	    برو Return;
	برای (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    اگر (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    وگرنه
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    وگرنه
	Result = FindFirst((نویسه *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    وقتی (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#وگرنه
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	اگر (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	وگرنه
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    اگر (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    وگرنه اگر (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    اگر (matches)
	qsort((تهی *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, اندازه(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    اگر (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    وگرنه
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#وگرنه
    vim_free(Anchor);
#endif

    بازگشت matches;
}

    ایستا صحیح
sortcmp(ثابت تهی *a, ثابت تهی *b)
{
    نویسه *s = *(نویسه **)a;
    نویسه *t = *(نویسه **)b;

    بازگشت pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	وگرنه اگر (vim_strchr((char_u *)"*?[(#", *p) != پوچ)
	    بازگشت TRUE;
    }
    بازگشت FALSE;
}

    صحیح
mch_has_wildcard(char_u *p)
{
    برای ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	وگرنه
	    اگر (vim_strchr((char_u *)"*?[(#$`", *p) != پوچ
		    || (*p == '~' && p[1] != NUL))
		بازگشت TRUE;
    }
    بازگشت FALSE;
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
    صحیح		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ایستا char_u   *alloced = پوچ;	// allocated memory

#ifdef FEAT_ARP
    اگر (!dos2)
	retval = (char_u *)getenv((نویسه *)var);
    وگرنه
#endif
    {
	VIM_CLEAR(alloced);
	retval = پوچ;

	buf = alloc(IOSIZE);
	اگر (buf == پوچ)
	    بازگشت پوچ;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	اگر (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    اگر (retval == پوچ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    بازگشت retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    صحیح
mch_setenv(نویسه *var, نویسه *مقدار, صحیح x UNUSED)
{
#ifdef FEAT_ARP
    اگر (!dos2)
	بازگشت setenv(var, مقدار);
#endif

    اگر (SetVar((UBYTE *)var, (UBYTE *)مقدار, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	بازگشت 0;   // success
    بازگشت -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ساختار Process *proc = (ساختار Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    صحیح status;

    اگر (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    وگرنه
	status = OK;

    Close(fh);
    بازگشت status;
}
