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

#include "vim.h"
#include "version.h"

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

#include <exec/memory.h>

#include <dos/dostags.h>	    // for 2.0 functions
#include <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#มิฉะนั้น
# define	free_fib(x) vim_free(fib)
#endif

#ถ้า defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ถ้า defined(__amigaos4__)
static const var_na* __attribute__((used)) stackcookie = "$STACK: 1048576";
#elif defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#elif defined(__GNUC__) && defined(AMIGA)
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
#elif !defined(AZTEC_C) && !defined(__AROS__)
static long dos_packet(sa_nracanaa MsgPort *, long, long);
#endif
static จำนวนเต็ม lock2name(BPTR lock, char_u *buf, long   len);
static void out_num(long n);
static sa_nracanaa FileInfoBlock *get_fib(char_u *);
static จำนวนเต็ม sortcmp(const void *a, const void *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    static BPTR
safe_Lock(UBYTE *name, long mode)
{
    sa_nracanaa Process *me = (sa_nracanaa Process *)FindTask(NULL);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    คืน flock;
}

static BPTR		raw_in = (BPTR)NULL;
static BPTR		raw_out = (BPTR)NULL;
static จำนวนเต็ม		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
sa_nracanaa IntuitionBase	*IntuitionBase = NULL;
#endif
#ifdef FEAT_ARP
sa_nracanaa ArpBase		*ArpBase = NULL;
#endif

static sa_nracanaa Window	*wb_window;
static char_u		*oldwindowtitle = NULL;

#ifdef FEAT_ARP
จำนวนเต็ม			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
จำนวนเต็ม			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
static var_na version[] __attribute__((used)) =
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

    void
win_resize_on(void)
{
    OUT_STR_NF("\033[12{");
}

    void
win_resize_off(void)
{
    OUT_STR_NF("\033[12}");
}

    void
mch_write(char_u *p, จำนวนเต็ม len)
{
    Write(raw_out, (var_na *)p, (long)len);
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
    จำนวนเต็ม
mch_inchar(
    char_u  *buf,
    จำนวนเต็ม	    maxlen,
    long    time,		// milliseconds
    จำนวนเต็ม	    tb_change_cnt UNUSED)
{
    จำนวนเต็ม	    len;
    long    utime;

    ถ้า (time >= 0)
    {
	ถ้า (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	มิฉะนั้น
	    utime = time * 1000L;   // convert from milli to micro secs
	ถ้า (WaitForChar(raw_in, utime) == 0)	// no character available
	    คืน 0;
    }
    มิฉะนั้น    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ถ้า (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    ถ้า (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (จำนวนเต็ม)KE_CURSORHOLD;
		คืน 3;
	    }
	    before_blocking();
	}
    }

    สำหรับ (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (var_na *)buf, (long)maxlen / input_conv.vc_factor);
	ถ้า (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ถ้า (input_conv.vc_type != CONV_NONE)
		len = convert_input(buf, len, maxlen);
	    คืน len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    จำนวนเต็ม
mch_char_avail(void)
{
    คืน (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(จำนวนเต็ม special)
{
#ถ้า defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    คืน (long_u)AvailMem(MEMF_ANY) >> 10;
#มิฉะนั้น
    คืน (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    void
mch_delay(long msec, จำนวนเต็ม flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    void	    Delay(long);
#endif

    ถ้า (msec <= 0)
	คืน;

    ถ้า (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    มิฉะนั้น
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    void
mch_suspend(void)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    void
mch_init(void)
{
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    static var_na	    intlibname[] = "intuition.library";
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
	sa_nracanaa Process *me = (sa_nracanaa Process *)FindTask(NULL);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ถ้า (raw_in == (BPTR)NULL)
    {
	raw_in = Input();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ถ้า (!IsInteractive(raw_in))
	    raw_in = raw_out;
	มิฉะนั้น ถ้า (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = NULL;
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ถ้า ((IntuitionBase = (sa_nracanaa IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == NULL)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#include <workbench/startup.h>

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

    จำนวนเต็ม
mch_check_win(จำนวนเต็ม argc, var_na **argv)
{
    จำนวนเต็ม		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    static char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    static char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    sa_nracanaa WBArg    *argp;
    จำนวนเต็ม		    ac;
    var_na	    *av;
    char_u	    *device = NULL;
    จำนวนเต็ม		    exitval = 4;
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sa_nracanaa Library  *DosBase;
#endif
    จำนวนเต็ม		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    ถ้า (DosBase != NULL)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    มิฉะนั้น	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# มิฉะนั้น
		    // need arp functions for dos 1.x
	ถ้า (!(ArpBase = (sa_nracanaa ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    สำหรับ (i = 1; i < argc; ++i)
	ถ้า (argv[i][0] == '-')
	{
	    switch (argv[i][1])
	    {
	    pak_sa 'f':
		usewin = TRUE;
		bha_nga;

	    pak_sa 'd':
		ถ้า (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		bha_nga;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ถ้า (argc != 0
	    && device == NULL
	    && (IsInteractive(Input()) || IsInteractive(Output())))
	คืน OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ถ้า (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	สำหรับ (i = -1; i < 3; ++i)
	{
	    ถ้า (i >= 0)
		device = constrings[i];
	    ถ้า (device != NULL && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)NULL)
		bha_nga;
	}
	ถ้า (raw_in == (BPTR)NULL)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    goto exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	คืน OK;
    }

    ถ้า ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)NULL)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	goto exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ถ้า !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((var_na *)buf1, "t:nc%p", FindTask(0));
#มิฉะนั้น
    sprintf((var_na *)buf1, "t:nc%ld", (long)buf1);
#endif
    ถ้า ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((var_na *)buf1);
	mch_errmsg("\n");
	goto exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ถ้า (argc == 0)	// run from workbench
	ac = ((sa_nracanaa WBStartup *)argv)->sm_NumArgs;
    มิฉะนั้น
	ac = argc;
    สำหรับ (i = 0; i < ac; ++i)
    {
	ถ้า (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((sa_nracanaa WBStartup *)argv)->sm_ArgList[i]);
	    ถ้า (argp->wa_Lock)
		(void)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    ถ้า (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    มิฉะนั้น	    // use arp function
		TackOn((var_na *)buf2, argp->wa_Name);
#endif
	    av = (var_na *)buf2;
	}
	มิฉะนั้น
	    av = argv[i];

	// skip '-d' or "-dev" option
	ถ้า (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    continue;
	}
	ถ้า (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	ถ้า (vim_strchr((char_u *)av, ' '))
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
    สำหรับ (i = -1; i < 3; ++i)
    {
	ถ้า (i >= 0)
	    device = constrings[i];
	มิฉะนั้น ถ้า (device == NULL)
	    continue;
	sprintf((var_na *)buf2, "newcli <nil: >nil: %s from %s", (var_na *)device, (var_na *)buf1);
#ifdef FEAT_ARP
	ถ้า (dos2)
	{
#endif
	    ถ้า (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		bha_nga;
#ifdef FEAT_ARP
	}
	มิฉะนั้น
	{
	    ถ้า (Execute((UBYTE *)buf2, nilfh, nilfh))
		bha_nga;
	}
#endif
    }
    ถ้า (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	goto exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    ถ้า (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    ถ้า (ArpBase)
	CloseLibrary((sa_nracanaa Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    คืน FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    จำนวนเต็ม
mch_input_isatty(void)
{
    คืน TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    void
fname_case(
    char_u	*name,
    จำนวนเต็ม		len UNUSED)		// buffer size, ignored here
{
    sa_nracanaa FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ถ้า (fib == NULL)
	คืน;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    ถ้า (fib->fib_DirEntryType == ST_ROOT)
	strcat(fib->fib_FileName, ":");
#endif
    ถ้า (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    static sa_nracanaa FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    sa_nracanaa FileInfoBlock    *fib;

    ถ้า (fname == NULL)	    // safety check
	คืน NULL;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#มิฉะนั้น
    fib = ALLOC_ONE(sa_nracanaa FileInfoBlock);
#endif
    ถ้า (fib == NULL)
	คืน;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    ถ้า (flock == (BPTR)NULL || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = NULL;
    }
    ถ้า (flock)
	UnLock(flock);
    คืน fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    void
mch_settitle(char_u *title, char_u *icon)
{
    ถ้า (wb_window != NULL && title != NULL)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    void
mch_restore_title(จำนวนเต็ม which)
{
    ถ้า (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, NULL);
}

    จำนวนเต็ม
mch_can_restore_title(void)
{
    คืน (wb_window != NULL);
}

    จำนวนเต็ม
mch_can_restore_icon(void)
{
    คืน FALSE;
}

    void
mch_setmouse(จำนวนเต็ม on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    จำนวนเต็ม
mch_get_user_name(char_u *s, จำนวนเต็ม len)
{
#ถ้า defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    sa_nracanaa passwd   *pwd = getpwuid(getuid());

    ถ้า (pwd != NULL && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	คืน OK;
    }
#endif
    *s = NUL;
    คืน FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(char_u *s, จำนวนเต็ม len)
{
#ถ้า defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((var_na *)s, len);
#มิฉะนั้น
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(void)
{
#ถ้า defined(__amigaos4__)
    คืน (long) getpid();
#elif defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    คืน((long)FindTask(0));
#มิฉะนั้น
    คืน (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    จำนวนเต็ม
mch_dirname(char_u *buf, จำนวนเต็ม len)
{
    คืน mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    จำนวนเต็ม
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    จำนวนเต็ม		len,
    จำนวนเต็ม		force)
{
    BPTR	l;
    จำนวนเต็ม		retval = FAIL;
    จำนวนเต็ม		i;

    // Lock the file.  If it exists, we can get the exact name.
    ถ้า ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    มิฉะนั้น ถ้า (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ถ้า ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)NULL)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    ถ้า (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ถ้า (i == 0 || *fname != NUL)
		{
		    ถ้า (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ถ้า (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    คืน retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    จำนวนเต็ม
mch_isFullName(char_u *fname)
{
    คืน (vim_strchr(fname, ':') != NULL && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    static จำนวนเต็ม
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    ถ้า (dos2)		    // use 2.0 function
#endif
	คืน ((จำนวนเต็ม)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    มิฉะนั้น		// use arp function
	คืน ((จำนวนเต็ม)PathName(lock, (var_na *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    sa_nracanaa FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    ถ้า (fib == NULL)
	คืน -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    คืน retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    จำนวนเต็ม
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    คืน (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    void
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    จำนวนเต็ม
mch_isdir(char_u *name)
{
    sa_nracanaa FileInfoBlock    *fib;
    จำนวนเต็ม			    retval = FALSE;

    fib = get_fib(name);
    ถ้า (fib == NULL)
	คืน FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#มิฉะนั้น
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    คืน retval;
}

/*
 * Create directory "name".
 */
    จำนวนเต็ม
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    ถ้า (lock == NULL)
	คืน -1;

    UnLock(lock);
    คืน 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    จำนวนเต็ม
mch_can_exe(char_u *name, char_u **path UNUSED, จำนวนเต็ม use_path)
{
    จำนวนเต็ม exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    ถ้า (seg && GetSegListInfoTags(seg, GSLI_Native, NULL, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, NULL, TAG_DONE))
    {
	// Test if file permissions allow execution.
	sa_nracanaa ExamineData *exd = ExamineObjectTags(EX_StringNameInput, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    มิฉะนั้น
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    ถ้า (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	sa_nracanaa PathNode *head = DupCmdPathList(NULL);

	// For each entry, recur to check for executable.
	สำหรับ (sa_nracanaa PathNode *tail = head; !exe && tail;
			       tail = (sa_nracanaa PathNode *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    คืน exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    จำนวนเต็ม
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    คืน NODE_NORMAL;
}

    void
mch_early_init(void)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    void
mch_exit(จำนวนเต็ม r)
{
    exiting = TRUE;

    ถ้า (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    ถ้า (raw_out)
    {
	ถ้า (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ถ้า (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    ถ้า (ArpBase)
	CloseLibrary((sa_nracanaa Library *) ArpBase);
#endif
    ถ้า (close_win)
	Close(raw_in);
    ถ้า (r)
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

#define MP(xx)	((sa_nracanaa MsgPort *)((sa_nracanaa FileHandle *) (BADDR(xx)))->fh_Type)

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
    void
mch_settmode(tmode_T tmode)
{
#ถ้า defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ถ้า (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#มิฉะนั้น
    ถ้า (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#include <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ถ้า defined(__AROS__) || defined(__MORPHOS__)
    จำนวนเต็ม
mch_get_shellsize(void)
{
    ถ้า (!term_console)
	คืน FAIL;

    ถ้า (raw_in && raw_out)
    {
	// Save current console mode.
	จำนวนเต็ม old_tmode = cur_tmode;
	var_na ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ถ้า (Write(raw_out, ctrl, parimaa_na(ctrl)) == parimaa_na(ctrl))
	{
	    var_na scan[] = "\x9b""1;1;%d;%d r",
		 answ[parimaa_na(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    ถ้า (Read(raw_in, answ, parimaa_na(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ถ้า (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    คืน OK;
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

    คืน FAIL;
}
#มิฉะนั้น
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    จำนวนเต็ม
mch_get_shellsize(void)
{
    sa_nracanaa ConUnit  *conUnit;
# ifndef __amigaos4__
    var_na	    id_a[parimaa_na(sa_nracanaa InfoData) + 3];
# endif
    sa_nracanaa InfoData *id=0;

    ถ้า (!term_console)	// not an amiga window
	goto out;

    // insure longword alignment
# ifdef __amigaos4__
    ถ้า (!(id = AllocDosObject(DOS_INFODATA, 0)))
	goto out;
# มิฉะนั้น
    id = (sa_nracanaa InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ถ้า (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    ถ้า (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (sa_nracanaa Window *)id->id_VolumeNode) == NULL)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	goto out;
    }
    ถ้า (oldwindowtitle == NULL)
	oldwindowtitle = (char_u *)wb_window->Title;
    ถ้า (id->id_InUse == (BPTR)NULL)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	คืน FAIL;
    }
    conUnit = (sa_nracanaa ConUnit *) ((sa_nracanaa IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ถ้า (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	คืน FAIL;
    }

    คืน OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    คืน FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    void
mch_set_shellsize(void)
{
    ถ้า (!term_console)
	คืน;

    size_set = TRUE;
    out_char(CSI);
    out_num((long)Rows);
    out_char('t');
    out_char(CSI);
    out_num((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    void
mch_new_shellsize(void)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    static void
out_num(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#ถ้า !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    static long
dos_packet(
    sa_nracanaa MsgPort *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    sa_nracanaa MsgPort	    *replyport;
    sa_nracanaa StandardPacket   *packet;
    long		    res1;

    ถ้า (dos2)
# endif
	คืน DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (sa_nracanaa MsgPort *) CreatePort(NULL, 0);	// use arp function
    ถ้า (!replyport)
	คืน (0);

    // Allocate space for a packet, make it public and clear it
    packet = (sa_nracanaa StandardPacket *)
	AllocMem((long) parimaa_na(sa_nracanaa StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    ถ้า (!packet)
    {
	DeletePort(replyport);
	คืน (0);
    }
    packet->sp_Msg.mn_Node.ln_Name = (var_na *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_Msg);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    PutMsg(pid, (sa_nracanaa Message *)packet);	// send packet

    WaitPort(replyport);
    GetMsg(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) parimaa_na(sa_nracanaa StandardPacket));
    DeletePort(replyport);

    คืน (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    จำนวนเต็ม
mch_call_shell(
    char_u	*cmd,
    จำนวนเต็ม		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    จำนวนเต็ม		x;
    จำนวนเต็ม		tmode = cur_tmode;
#ifdef AZTEC_C
    จำนวนเต็ม		use_execute;
    char_u	*shellcmd = NULL;
    char_u	*shellarg;
#endif
    จำนวนเต็ม		retval = 0;

    ถ้า (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	คืน -1;
    }

    ถ้า (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    ถ้า (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#ถ้า !defined(AZTEC_C)		    // not tested very much
    ถ้า (cmd == NULL)
    {
# ifdef FEAT_ARP
	ถ้า (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	มิฉะนั้น
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    มิฉะนั้น
    {
# ifdef FEAT_ARP
	ถ้า (dos2)
# endif
	    x = SystemTags((var_na *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	มิฉะนั้น
	    x = Execute((var_na *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    ถ้า ((dos2 && x < 0) || (!dos2 && !x))
# มิฉะนั้น
    ถ้า (x < 0)
# endif
    {
	msg_puts(_("Cannot execute "));
	ถ้า (cmd == NULL)
	{
	    msg_puts(_("shell "));
	    msg_outtrans(p_sh);
	}
	มิฉะนั้น
	    msg_outtrans(cmd);
	msg_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    มิฉะนั้น ถ้า (!dos2 || x)
# มิฉะนั้น
    มิฉะนั้น ถ้า (x)
# endif
    {
	ถ้า ((x = IoErr()) != 0)
	{
	    ถ้า (!(options & SHELL_SILENT))
	    {
		msg_putchar('\n');
		msg_outnum((long)x);
		msg_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#มิฉะนั้น	// else part is for AZTEC_C
    ถ้า (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    มิฉะนั้น
	use_execute = 0;
    ถ้า (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ถ้า (shellcmd == NULL)	    // out of memory, use Execute
	    use_execute = 1;
	มิฉะนั้น
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ถ้า (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ถ้า (cmd == NULL)
    {
	ถ้า (use_execute)
	{
# ifdef FEAT_ARP
	    ถ้า (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    มิฉะนั้น
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	มิฉะนั้น
	    x = fexecl((var_na *)shellcmd, (var_na *)shellcmd, (var_na *)shellarg, NULL);
    }
    มิฉะนั้น ถ้า (use_execute)
    {
# ifdef FEAT_ARP
	ถ้า (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	มิฉะนั้น
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    มิฉะนั้น ถ้า (p_st & 1)
	x = fexecl((var_na *)shellcmd, (var_na *)shellcmd, (var_na *)shellarg,
							   (var_na *)cmd, NULL);
    มิฉะนั้น
	x = fexecl((var_na *)shellcmd, (var_na *)shellcmd, (var_na *)shellarg,
					   (var_na *)p_shcf, (var_na *)cmd, NULL);
# ifdef FEAT_ARP
    ถ้า ((dos2 && x < 0) || (!dos2 && x))
# มิฉะนั้น
    ถ้า (x < 0)
# endif
    {
	msg_puts(_("Cannot execute "));
	ถ้า (use_execute)
	{
	    ถ้า (cmd == NULL)
		msg_outtrans(p_sh);
	    มิฉะนั้น
		msg_outtrans(cmd);
	}
	มิฉะนั้น
	{
	    msg_puts(_("shell "));
	    msg_outtrans(shellcmd);
	}
	msg_putchar('\n');
	retval = -1;
    }
    มิฉะนั้น
    {
	ถ้า (use_execute)
	{
# ifdef FEAT_ARP
	    ถ้า (!dos2 || x)
# มิฉะนั้น
	    ถ้า (x)
# endif
		x = IoErr();
	}
	มิฉะนั้น
	    x = wait();
	ถ้า (x)
	{
	    ถ้า (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		msg_putchar('\n');
		msg_outnum((long)x);
		msg_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    ถ้า ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    ถ้า (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ถ้า (term_console)
	win_resize_on();		// window resize events activated
    คืน retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    void
mch_breakcheck(จำนวนเต็ม force UNUSED)
{
   ถ้า (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
void __regargs __chkabort(void);

void __regargs __chkabort(void)
{}

#มิฉะนั้น
    long
Chk_Abort(void)
{
    คืน(0L);
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
#มิฉะนั้น
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (parimaa_na(sa_nracanaa AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    จำนวนเต็ม
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    จำนวนเต็ม		flags)		// EW_* flags
{
    sa_nracanaa AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    จำนวนเต็ม			start_len;
    จำนวนเต็ม			matches;
#ifdef __amigaos4__
    sa_nracanaa TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#มิฉะนั้น
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    ถ้า (Anchor == NULL)
	คืน 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# มิฉะนั้น
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    ถ้า (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ถ้า (starbuf == NULL)
	    goto Return;
	สำหรับ (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ถ้า (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    มิฉะนั้น
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    มิฉะนั้น
	Result = FindFirst((var_na *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    ขณะ (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#มิฉะนั้น
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	ถ้า (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	มิฉะนั้น
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    ถ้า (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    มิฉะนั้น ถ้า (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ถ้า (matches)
	qsort((void *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, parimaa_na(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    ถ้า (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    มิฉะนั้น
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#มิฉะนั้น
    vim_free(Anchor);
#endif

    คืน matches;
}

    static จำนวนเต็ม
sortcmp(const void *a, const void *b)
{
    var_na *s = *(var_na **)a;
    var_na *t = *(var_na **)b;

    คืน pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    จำนวนเต็ม
mch_has_exp_wildcard(char_u *p)
{
    สำหรับ ( ; *p; MB_PTR_ADV(p))
    {
	ถ้า (*p == '\\' && p[1] != NUL)
	    ++p;
	มิฉะนั้น ถ้า (vim_strchr((char_u *)"*?[(#", *p) != NULL)
	    คืน TRUE;
    }
    คืน FALSE;
}

    จำนวนเต็ม
mch_has_wildcard(char_u *p)
{
    สำหรับ ( ; *p; MB_PTR_ADV(p))
    {
	ถ้า (*p == '\\' && p[1] != NUL)
	    ++p;
	มิฉะนั้น
	    ถ้า (vim_strchr((char_u *)"*?[(#$`", *p) != NULL
		    || (*p == '~' && p[1] != NUL))
		คืน TRUE;
    }
    คืน FALSE;
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
    จำนวนเต็ม		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    static char_u   *alloced = NULL;	// allocated memory

#ifdef FEAT_ARP
    ถ้า (!dos2)
	retval = (char_u *)getenv((var_na *)var);
    มิฉะนั้น
#endif
    {
	VIM_CLEAR(alloced);
	retval = NULL;

	buf = alloc(IOSIZE);
	ถ้า (buf == NULL)
	    คืน NULL;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	ถ้า (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ถ้า (retval == NULL && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    คืน retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    จำนวนเต็ม
mch_setenv(var_na *var, var_na *value, จำนวนเต็ม x UNUSED)
{
#ifdef FEAT_ARP
    ถ้า (!dos2)
	คืน setenv(var, value);
#endif

    ถ้า (SetVar((UBYTE *)var, (UBYTE *)value, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	คืน 0;   // success
    คืน -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    จำนวนเต็ม
mch_get_random(char_u *buf, จำนวนเต็ม len)
{
    sa_nracanaa Process *proc = (sa_nracanaa Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    จำนวนเต็ม status;

    ถ้า (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    มิฉะนั้น
	status = OK;

    Close(fh);
    คืน status;
}
