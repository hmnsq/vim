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

#ضمن "vim.h"
#ضمن "version.h"

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

#ضمن <exec/memory.h>

#ضمن <dos/dostags.h>	    // for 2.0 functions
#ضمن <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#وإلا
# define	free_fib(x) vim_free(fib)
#endif

#إذا defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#إذا defined(__amigaos4__)
ساكن ثابت حرف* __attribute__((used)) stackcookie = "$STACK: 1048576";
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#وإلاإذا defined(__GNUC__) && defined(AMIGA)
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
#وإلاإذا !defined(AZTEC_C) && !defined(__AROS__)
ساكن long dos_packet(بنية پیغام_Port *, long, long);
#endif
ساكن صحيح lock2name(BPTR lock, char_u *buf, long   len);
ساكن فراغ out_عدد(long n);
ساكن بنية FileInfoBlock *get_fib(char_u *);
ساكن صحيح sortcmp(ثابت فراغ *a, ثابت فراغ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ساكن BPTR
safe_Lock(UBYTE *name, long mode)
{
    بنية Process *me = (بنية Process *)FindTask(عدم);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    أرجع flock;
}

ساكن BPTR		raw_in = (BPTR)عدم;
ساكن BPTR		raw_out = (BPTR)عدم;
ساكن صحيح		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
بنية IntuitionBase	*IntuitionBase = عدم;
#endif
#ifdef FEAT_ARP
بنية ArpBase		*ArpBase = عدم;
#endif

ساكن بنية Window	*wb_window;
ساكن char_u		*oldwindowtitle = عدم;

#ifdef FEAT_ARP
صحيح			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
صحيح			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
ساكن حرف version[] __attribute__((used)) =
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

    فراغ
win_resize_on(فراغ)
{
    OUT_STR_NF("\033[12{");
}

    فراغ
win_resize_off(فراغ)
{
    OUT_STR_NF("\033[12}");
}

    فراغ
mch_write(char_u *p, صحيح len)
{
    Write(raw_out, (حرف *)p, (long)len);
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
    صحيح
mch_inchar(
    char_u  *buf,
    صحيح	    maxlen,
    long    time,		// milliseconds
    صحيح	    tb_change_cnt UNUSED)
{
    صحيح	    len;
    long    utime;

    إذا (time >= 0)
    {
	إذا (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	وإلا
	    utime = time * 1000L;   // convert from milli to micro secs
	إذا (WaitForChar(raw_in, utime) == 0)	// no character available
	    أرجع 0;
    }
    وإلا    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	إذا (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    إذا (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحيح)KE_CURSORHOLD;
		أرجع 3;
	    }
	    before_blocking();
	}
    }

    لكل (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (حرف *)buf, (long)maxlen / داخلہ_conv.vc_factor);
	إذا (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    إذا (داخلہ_conv.vc_type != CONV_NONE)
		len = convert_داخلہ(buf, len, maxlen);
	    أرجع len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    صحيح
mch_char_avail(فراغ)
{
    أرجع (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(صحيح special)
{
#إذا defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    أرجع (long_u)AvailMem(MEMF_ANY) >> 10;
#وإلا
    أرجع (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    فراغ
mch_delay(long msec, صحيح flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    فراغ	    Delay(long);
#endif

    إذا (msec <= 0)
	أرجع;

    إذا (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    وإلا
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    فراغ
mch_init(فراغ)
{
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساكن حرف	    intlibname[] = "intuition.library";
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
	بنية Process *me = (بنية Process *)FindTask(عدم);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    إذا (raw_in == (BPTR)عدم)
    {
	raw_in = داخلہ_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	إذا (!IsInteractive(raw_in))
	    raw_in = raw_out;
	وإلا إذا (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = عدم;
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    إذا ((IntuitionBase = (بنية IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == عدم)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#ضمن <workbench/startup.h>

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

    صحيح
mch_check_win(صحيح argc, حرف **argv)
{
    صحيح		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    ساكن char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ساكن char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    بنية WBArg    *argp;
    صحيح		    ac;
    حرف	    *av;
    char_u	    *device = عدم;
    صحيح		    exitval = 4;
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    بنية Library  *DosBase;
#endif
    صحيح		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    إذا (DosBase != عدم)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    وإلا	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# وإلا
		    // need arp functions for dos 1.x
	إذا (!(ArpBase = (بنية ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    لكل (i = 1; i < argc; ++i)
	إذا (argv[i][0] == '-')
	{
	    بدل (argv[i][1])
	    {
	    حالة 'f':
		usewin = TRUE;
		اكسر;

	    حالة 'd':
		إذا (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		اكسر;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    إذا (argc != 0
	    && device == عدم
	    && (IsInteractive(داخلہ_3()) || IsInteractive(Output())))
	أرجع OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    إذا (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	لكل (i = -1; i < 3; ++i)
	{
	    إذا (i >= 0)
		device = constrings[i];
	    إذا (device != عدم && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)عدم)
		اكسر;
	}
	إذا (raw_in == (BPTR)عدم)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    اذهب exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	أرجع OK;
    }

    إذا ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	اذهب exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((حرف *)buf1, "t:nc%p", FindTask(0));
#وإلا
    sprintf((حرف *)buf1, "t:nc%ld", (long)buf1);
#endif
    إذا ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((حرف *)buf1);
	mch_errmsg("\n");
	اذهب exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    إذا (argc == 0)	// run from workbench
	ac = ((بنية WBStartup *)argv)->sm_عدد_Args;
    وإلا
	ac = argc;
    لكل (i = 0; i < ac; ++i)
    {
	إذا (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((بنية WBStartup *)argv)->sm_ArgList[i]);
	    إذا (argp->wa_Lock)
		(فراغ)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    إذا (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    وإلا	    // use arp function
		TackOn((حرف *)buf2, argp->wa_Name);
#endif
	    av = (حرف *)buf2;
	}
	وإلا
	    av = argv[i];

	// skip '-d' or "-dev" option
	إذا (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    استمر;
	}
	إذا (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	إذا (vim_strchr((char_u *)av, ' '))
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
    لكل (i = -1; i < 3; ++i)
    {
	إذا (i >= 0)
	    device = constrings[i];
	وإلا إذا (device == عدم)
	    استمر;
	sprintf((حرف *)buf2, "newcli <nil: >nil: %s from %s", (حرف *)device, (حرف *)buf1);
#ifdef FEAT_ARP
	إذا (dos2)
	{
#endif
	    إذا (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		اكسر;
#ifdef FEAT_ARP
	}
	وإلا
	{
	    إذا (Execute((UBYTE *)buf2, nilfh, nilfh))
		اكسر;
	}
#endif
    }
    إذا (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	اذهب exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    إذا (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    إذا (ArpBase)
	CloseLibrary((بنية Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    أرجع FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    صحيح
mch_داخلہ_isatty(فراغ)
{
    أرجع TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    فراغ
fname_case(
    char_u	*name,
    صحيح		len UNUSED)		// buffer size, ignored here
{
    بنية FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    إذا (fib->fib_DirEntryType == ST_جڑ)
	strcat(fib->fib_FileName, ":");
#endif
    إذا (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ساكن بنية FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    بنية FileInfoBlock    *fib;

    إذا (fname == عدم)	    // safety check
	أرجع عدم;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#وإلا
    fib = ALLOC_ONE(بنية FileInfoBlock);
#endif
    إذا (fib == عدم)
	أرجع;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    إذا (flock == (BPTR)عدم || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = عدم;
    }
    إذا (flock)
	UnLock(flock);
    أرجع fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    فراغ
mch_settitle(char_u *title, char_u *icon)
{
    إذا (wb_window != عدم && title != عدم)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    فراغ
mch_restore_title(صحيح which)
{
    إذا (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, عدم);
}

    صحيح
mch_can_restore_title(فراغ)
{
    أرجع (wb_window != عدم);
}

    صحيح
mch_can_restore_icon(فراغ)
{
    أرجع FALSE;
}

    فراغ
mch_setmouse(صحيح on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    صحيح
mch_get_user_name(char_u *s, صحيح len)
{
#إذا defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    بنية passwd   *pwd = getpwuid(getuid());

    إذا (pwd != عدم && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	أرجع OK;
    }
#endif
    *s = NUL;
    أرجع FAIL;
}

/*
 * Insert host name is s[len].
 */
    فراغ
mch_get_host_name(char_u *s, صحيح len)
{
#إذا defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((حرف *)s, len);
#وإلا
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(فراغ)
{
#إذا defined(__amigaos4__)
    أرجع (long) getpid();
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    أرجع((long)FindTask(0));
#وإلا
    أرجع (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(char_u *buf, صحيح len)
{
    أرجع mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)
{
    BPTR	l;
    صحيح		retval = FAIL;
    صحيح		i;

    // Lock the file.  If it exists, we can get the exact name.
    إذا ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    وإلا إذا (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	إذا ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)عدم)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    إذا (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		إذا (i == 0 || *fname != NUL)
		{
		    إذا (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    إذا (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    أرجع retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحيح
mch_isFullName(char_u *fname)
{
    أرجع (vim_strchr(fname, ':') != عدم && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ساكن صحيح
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    إذا (dos2)		    // use 2.0 function
#endif
	أرجع ((صحيح)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    وإلا		// use arp function
	أرجع ((صحيح)PathName(lock, (حرف *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    بنية FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    أرجع retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    أرجع (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    صحيح
mch_isdir(char_u *name)
{
    بنية FileInfoBlock    *fib;
    صحيح			    retval = FALSE;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#وإلا
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    أرجع retval;
}

/*
 * Create directory "name".
 */
    صحيح
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    إذا (lock == عدم)
	أرجع -1;

    UnLock(lock);
    أرجع 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحيح
mch_can_exe(char_u *name, char_u **path UNUSED, صحيح use_path)
{
    صحيح exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    إذا (seg && GetSegListInfoTags(seg, GSLI_Native, عدم, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, عدم, TAG_DONE))
    {
	// Test if file permissions allow execution.
	بنية ExamineData *exd = ExamineObjectTags(EX_String_Name_داخلہ, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    وإلا
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    إذا (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	بنية Path_گرہ *head = DupCmdPathList(عدم);

	// For each entry, recur to check for executable.
	لكل (بنية Path_گرہ *tail = head; !exe && tail;
			       tail = (بنية Path_گرہ *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    أرجع exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    أرجع گرہ_NORMAL;
}

    فراغ
mch_early_init(فراغ)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    فراغ
mch_exit(صحيح r)
{
    exiting = TRUE;

    إذا (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    إذا (raw_out)
    {
	إذا (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    إذا (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    إذا (ArpBase)
	CloseLibrary((بنية Library *) ArpBase);
#endif
    إذا (close_win)
	Close(raw_in);
    إذا (r)
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

#define MP(xx)	((بنية پیغام_Port *)((بنية FileHandle *) (BADDR(xx)))->fh_Type)

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
    فراغ
mch_settmode(tmode_T tmode)
{
#إذا defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    إذا (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#وإلا
    إذا (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#ضمن <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#إذا defined(__AROS__) || defined(__MORPHOS__)
    صحيح
mch_get_shellsize(فراغ)
{
    إذا (!term_console)
	أرجع FAIL;

    إذا (raw_in && raw_out)
    {
	// Save current console mode.
	صحيح old_tmode = cur_tmode;
	حرف ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	إذا (Write(raw_out, ctrl, حجم(ctrl)) == حجم(ctrl))
	{
	    حرف scan[] = "\x9b""1;1;%d;%d r",
		 answ[حجم(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    إذا (Read(raw_in, answ, حجم(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		إذا (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    أرجع OK;
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

    أرجع FAIL;
}
#وإلا
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    صحيح
mch_get_shellsize(فراغ)
{
    بنية ConUnit  *conUnit;
# ifndef __amigaos4__
    حرف	    id_a[حجم(بنية InfoData) + 3];
# endif
    بنية InfoData *id=0;

    إذا (!term_console)	// not an amiga window
	اذهب out;

    // insure longword alignment
# ifdef __amigaos4__
    إذا (!(id = AllocDosObject(DOS_INFODATA, 0)))
	اذهب out;
# وإلا
    id = (بنية InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    إذا (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    إذا (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (بنية Window *)id->id_Volume_گرہ) == عدم)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	اذهب out;
    }
    إذا (oldwindowtitle == عدم)
	oldwindowtitle = (char_u *)wb_window->Title;
    إذا (id->id_InUse == (BPTR)عدم)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	أرجع FAIL;
    }
    conUnit = (بنية ConUnit *) ((بنية IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    إذا (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	أرجع FAIL;
    }

    أرجع OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    أرجع FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    فراغ
mch_set_shellsize(فراغ)
{
    إذا (!term_console)
	أرجع;

    size_set = TRUE;
    out_char(CSI);
    out_عدد((long)Rows);
    out_char('t');
    out_char(CSI);
    out_عدد((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    فراغ
mch_new_shellsize(فراغ)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ساكن فراغ
out_عدد(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#إذا !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    ساكن long
dos_packet(
    بنية پیغام_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    بنية پیغام_Port	    *replyport;
    بنية StandardPacket   *packet;
    long		    res1;

    إذا (dos2)
# endif
	أرجع DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (بنية پیغام_Port *) CreatePort(عدم, 0);	// use arp function
    إذا (!replyport)
	أرجع (0);

    // Allocate space for a packet, make it public and clear it
    packet = (بنية StandardPacket *)
	AllocMem((long) حجم(بنية StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    إذا (!packet)
    {
	DeletePort(replyport);
	أرجع (0);
    }
    packet->sp_پیغام.mn_گرہ.ln_Name = (حرف *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_پیغام);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_پیغام(pid, (بنية Message *)packet);	// send packet

    WaitPort(replyport);
    Get_پیغام(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) حجم(بنية StandardPacket));
    DeletePort(replyport);

    أرجع (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    صحيح
mch_call_shell(
    char_u	*cmd,
    صحيح		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    صحيح		x;
    صحيح		tmode = cur_tmode;
#ifdef AZTEC_C
    صحيح		use_execute;
    char_u	*shellcmd = عدم;
    char_u	*shellarg;
#endif
    صحيح		retval = 0;

    إذا (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	أرجع -1;
    }

    إذا (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#إذا !defined(AZTEC_C)		    // not tested very much
    إذا (cmd == عدم)
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وإلا
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    وإلا
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = SystemTags((حرف *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وإلا
	    x = Execute((حرف *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    إذا ((dos2 && x < 0) || (!dos2 && !x))
# وإلا
    إذا (x < 0)
# endif
    {
	پیغام_puts(_("Cannot execute "));
	إذا (cmd == عدم)
	{
	    پیغام_puts(_("shell "));
	    پیغام_outtrans(p_sh);
	}
	وإلا
	    پیغام_outtrans(cmd);
	پیغام_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    وإلا إذا (!dos2 || x)
# وإلا
    وإلا إذا (x)
# endif
    {
	إذا ((x = IoErr()) != 0)
	{
	    إذا (!(options & SHELL_SILENT))
	    {
		پیغام_putchar('\n');
		پیغام_outnum((long)x);
		پیغام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#وإلا	// else part is for AZTEC_C
    إذا (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    وإلا
	use_execute = 0;
    إذا (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	إذا (shellcmd == عدم)	    // out of memory, use Execute
	    use_execute = 1;
	وإلا
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    إذا (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    إذا (cmd == عدم)
    {
	إذا (use_execute)
	{
# ifdef FEAT_ARP
	    إذا (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    وإلا
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	وإلا
	    x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg, عدم);
    }
    وإلا إذا (use_execute)
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	وإلا
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    وإلا إذا (p_st & 1)
	x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg,
							   (حرف *)cmd, عدم);
    وإلا
	x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg,
					   (حرف *)p_shcf, (حرف *)cmd, عدم);
# ifdef FEAT_ARP
    إذا ((dos2 && x < 0) || (!dos2 && x))
# وإلا
    إذا (x < 0)
# endif
    {
	پیغام_puts(_("Cannot execute "));
	إذا (use_execute)
	{
	    إذا (cmd == عدم)
		پیغام_outtrans(p_sh);
	    وإلا
		پیغام_outtrans(cmd);
	}
	وإلا
	{
	    پیغام_puts(_("shell "));
	    پیغام_outtrans(shellcmd);
	}
	پیغام_putchar('\n');
	retval = -1;
    }
    وإلا
    {
	إذا (use_execute)
	{
# ifdef FEAT_ARP
	    إذا (!dos2 || x)
# وإلا
	    إذا (x)
# endif
		x = IoErr();
	}
	وإلا
	    x = wait();
	إذا (x)
	{
	    إذا (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		پیغام_putchar('\n');
		پیغام_outnum((long)x);
		پیغام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    إذا ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    إذا (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    إذا (term_console)
	win_resize_on();		// window resize events activated
    أرجع retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    فراغ
mch_breakcheck(صحيح force UNUSED)
{
   إذا (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
فراغ __regargs __chkabort(فراغ);

فراغ __regargs __chkabort(فراغ)
{}

#وإلا
    long
Chk_Abort(فراغ)
{
    أرجع(0L);
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
#وإلا
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (حجم(بنية AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    صحيح		flags)		// EW_* flags
{
    بنية AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    صحيح			start_len;
    صحيح			matches;
#ifdef __amigaos4__
    بنية TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#وإلا
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    إذا (Anchor == عدم)
	أرجع 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# وإلا
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    إذا (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	إذا (starbuf == عدم)
	    اذهب Return;
	لكل (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    إذا (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    وإلا
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    وإلا
	Result = FindFirst((حرف *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    طالما (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#وإلا
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	إذا (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	وإلا
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    إذا (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    وإلا إذا (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    إذا (matches)
	qsort((فراغ *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, حجم(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    إذا (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    وإلا
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#وإلا
    vim_free(Anchor);
#endif

    أرجع matches;
}

    ساكن صحيح
sortcmp(ثابت فراغ *a, ثابت فراغ *b)
{
    حرف *s = *(حرف **)a;
    حرف *t = *(حرف **)b;

    أرجع pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    صحيح
mch_has_exp_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	    ++p;
	وإلا إذا (vim_strchr((char_u *)"*?[(#", *p) != عدم)
	    أرجع TRUE;
    }
    أرجع FALSE;
}

    صحيح
mch_has_wildcard(char_u *p)
{
    لكل ( ; *p; MB_PTR_ADV(p))
    {
	إذا (*p == '\\' && p[1] != NUL)
	    ++p;
	وإلا
	    إذا (vim_strchr((char_u *)"*?[(#$`", *p) != عدم
		    || (*p == '~' && p[1] != NUL))
		أرجع TRUE;
    }
    أرجع FALSE;
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
    صحيح		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ساكن char_u   *alloced = عدم;	// allocated memory

#ifdef FEAT_ARP
    إذا (!dos2)
	retval = (char_u *)getenv((حرف *)var);
    وإلا
#endif
    {
	VIM_CLEAR(alloced);
	retval = عدم;

	buf = alloc(IOSIZE);
	إذا (buf == عدم)
	    أرجع عدم;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	إذا (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    إذا (retval == عدم && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    أرجع retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    صحيح
mch_setenv(حرف *var, حرف *قدر, صحيح x UNUSED)
{
#ifdef FEAT_ARP
    إذا (!dos2)
	أرجع setenv(var, قدر);
#endif

    إذا (SetVar((UBYTE *)var, (UBYTE *)قدر, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	أرجع 0;   // success
    أرجع -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    صحيح
mch_get_random(char_u *buf, صحيح len)
{
    بنية Process *proc = (بنية Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    صحيح status;

    إذا (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    وإلا
	status = OK;

    Close(fh);
    أرجع status;
}
