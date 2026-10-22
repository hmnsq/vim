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
#інакше
# define	free_fib(x) vim_free(fib)
#endif

#якщо defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#якщо defined(__amigaos4__)
static const char* __attribute__((used)) stackcookie = "$STACK: 1048576";
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
static long dos_packet(struct MsgPort *, long, long);
#endif
static ціле lock2name(BPTR lock, char_u *buf, long   len);
static void out_num(long n);
static struct FileInfoBlock *get_fib(char_u *);
static ціле sortcmp(const void *a, const void *b);

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
    struct Process *me = (struct Process *)FindTask(NULL);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    повернути flock;
}

static BPTR		raw_in = (BPTR)NULL;
static BPTR		raw_out = (BPTR)NULL;
static ціле		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
struct IntuitionBase	*IntuitionBase = NULL;
#endif
#ifdef FEAT_ARP
struct ArpBase		*ArpBase = NULL;
#endif

static struct Window	*wb_window;
static char_u		*oldwindowtitle = NULL;

#ifdef FEAT_ARP
ціле			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
ціле			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
static char version[] __attribute__((used)) =
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
mch_write(char_u *p, ціле len)
{
    Write(raw_out, (char *)p, (long)len);
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
    ціле
mch_inchar(
    char_u  *buf,
    ціле	    maxlen,
    long    time,		// milliseconds
    ціле	    tb_change_cnt UNUSED)
{
    ціле	    len;
    long    utime;

    якщо (time >= 0)
    {
	якщо (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	інакше
	    utime = time * 1000L;   // convert from milli to micro secs
	якщо (WaitForChar(raw_in, utime) == 0)	// no character available
	    повернути 0;
    }
    інакше    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	якщо (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    якщо (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ціле)KE_CURSORHOLD;
		повернути 3;
	    }
	    before_blocking();
	}
    }

    для (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (char *)buf, (long)maxlen / input_conv.vc_factor);
	якщо (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    якщо (input_conv.vc_type != CONV_NONE)
		len = convert_input(buf, len, maxlen);
	    повернути len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    ціле
mch_char_avail(void)
{
    повернути (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(ціле special)
{
#якщо defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    повернути (long_u)AvailMem(MEMF_ANY) >> 10;
#інакше
    повернути (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    void
mch_delay(long msec, ціле flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    void	    Delay(long);
#endif

    якщо (msec <= 0)
	повернути;

    якщо (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    інакше
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
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    static char	    intlibname[] = "intuition.library";
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
	struct Process *me = (struct Process *)FindTask(NULL);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    якщо (raw_in == (BPTR)NULL)
    {
	raw_in = Input();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	якщо (!IsInteractive(raw_in))
	    raw_in = raw_out;
	інакше якщо (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = NULL;
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    якщо ((IntuitionBase = (struct IntuitionBase *)
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

    ціле
mch_check_win(ціле argc, char **argv)
{
    ціле		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    static char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    static char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    struct WBArg    *argp;
    ціле		    ac;
    char	    *av;
    char_u	    *device = NULL;
    ціле		    exitval = 4;
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    struct Library  *DosBase;
#endif
    ціле		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    якщо (DosBase != NULL)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    інакше	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# інакше
		    // need arp functions for dos 1.x
	якщо (!(ArpBase = (struct ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    для (i = 1; i < argc; ++i)
	якщо (argv[i][0] == '-')
	{
	    switch (argv[i][1])
	    {
	    case 'f':
		usewin = TRUE;
		break;

	    case 'd':
		якщо (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		break;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    якщо (argc != 0
	    && device == NULL
	    && (IsInteractive(Input()) || IsInteractive(Output())))
	повернути OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    якщо (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	для (i = -1; i < 3; ++i)
	{
	    якщо (i >= 0)
		device = constrings[i];
	    якщо (device != NULL && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)NULL)
		break;
	}
	якщо (raw_in == (BPTR)NULL)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    goto exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	повернути OK;
    }

    якщо ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)NULL)
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
#якщо !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((char *)buf1, "t:nc%p", FindTask(0));
#інакше
    sprintf((char *)buf1, "t:nc%ld", (long)buf1);
#endif
    якщо ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)NULL)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((char *)buf1);
	mch_errmsg("\n");
	goto exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    якщо (argc == 0)	// run from workbench
	ac = ((struct WBStartup *)argv)->sm_NumArgs;
    інакше
	ac = argc;
    для (i = 0; i < ac; ++i)
    {
	якщо (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((struct WBStartup *)argv)->sm_ArgList[i]);
	    якщо (argp->wa_Lock)
		(void)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    якщо (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    інакше	    // use arp function
		TackOn((char *)buf2, argp->wa_Name);
#endif
	    av = (char *)buf2;
	}
	інакше
	    av = argv[i];

	// skip '-d' or "-dev" option
	якщо (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    continue;
	}
	якщо (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	якщо (vim_strchr((char_u *)av, ' '))
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
    для (i = -1; i < 3; ++i)
    {
	якщо (i >= 0)
	    device = constrings[i];
	інакше якщо (device == NULL)
	    continue;
	sprintf((char *)buf2, "newcli <nil: >nil: %s from %s", (char *)device, (char *)buf1);
#ifdef FEAT_ARP
	якщо (dos2)
	{
#endif
	    якщо (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		break;
#ifdef FEAT_ARP
	}
	інакше
	{
	    якщо (Execute((UBYTE *)buf2, nilfh, nilfh))
		break;
	}
#endif
    }
    якщо (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	goto exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    якщо (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    якщо (ArpBase)
	CloseLibrary((struct Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    повернути FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    ціле
mch_input_isatty(void)
{
    повернути TRUE;
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
    ціле		len UNUSED)		// buffer size, ignored here
{
    struct FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    якщо (fib == NULL)
	повернути;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    якщо (fib->fib_DirEntryType == ST_ROOT)
	strcat(fib->fib_FileName, ":");
#endif
    якщо (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    static struct FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    struct FileInfoBlock    *fib;

    якщо (fname == NULL)	    // safety check
	повернути NULL;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#інакше
    fib = ALLOC_ONE(struct FileInfoBlock);
#endif
    якщо (fib == NULL)
	повернути;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    якщо (flock == (BPTR)NULL || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = NULL;
    }
    якщо (flock)
	UnLock(flock);
    повернути fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    void
mch_settitle(char_u *title, char_u *icon)
{
    якщо (wb_window != NULL && title != NULL)
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
mch_restore_title(ціле which)
{
    якщо (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, NULL);
}

    ціле
mch_can_restore_title(void)
{
    повернути (wb_window != NULL);
}

    ціле
mch_can_restore_icon(void)
{
    повернути FALSE;
}

    void
mch_setmouse(ціле on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    ціле
mch_get_user_name(char_u *s, ціле len)
{
#якщо defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    struct passwd   *pwd = getpwuid(getuid());

    якщо (pwd != NULL && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	повернути OK;
    }
#endif
    *s = NUL;
    повернути FAIL;
}

/*
 * Insert host name is s[len].
 */
    void
mch_get_host_name(char_u *s, ціле len)
{
#якщо defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((char *)s, len);
#інакше
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
#якщо defined(__amigaos4__)
    повернути (long) getpid();
#elif defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    повернути((long)FindTask(0));
#інакше
    повернути (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ціле
mch_dirname(char_u *buf, ціле len)
{
    повернути mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    ціле
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    ціле		len,
    ціле		force)
{
    BPTR	l;
    ціле		retval = FAIL;
    ціле		i;

    // Lock the file.  If it exists, we can get the exact name.
    якщо ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    інакше якщо (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	якщо ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)NULL)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    якщо (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		якщо (i == 0 || *fname != NUL)
		{
		    якщо (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    якщо (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    повернути retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ціле
mch_isFullName(char_u *fname)
{
    повернути (vim_strchr(fname, ':') != NULL && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    static ціле
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    якщо (dos2)		    // use 2.0 function
#endif
	повернути ((ціле)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    інакше		// use arp function
	повернути ((ціле)PathName(lock, (char *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    struct FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    якщо (fib == NULL)
	повернути -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    повернути retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    ціле
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    повернути (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
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
    ціле
mch_isdir(char_u *name)
{
    struct FileInfoBlock    *fib;
    ціле			    retval = FALSE;

    fib = get_fib(name);
    якщо (fib == NULL)
	повернути FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#інакше
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    повернути retval;
}

/*
 * Create directory "name".
 */
    ціле
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    якщо (lock == NULL)
	повернути -1;

    UnLock(lock);
    повернути 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ціле
mch_can_exe(char_u *name, char_u **path UNUSED, ціле use_path)
{
    ціле exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    якщо (seg && GetSegListInfoTags(seg, GSLI_Native, NULL, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, NULL, TAG_DONE))
    {
	// Test if file permissions allow execution.
	struct ExamineData *exd = ExamineObjectTags(EX_StringNameInput, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    інакше
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    якщо (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	struct PathNode *head = DupCmdPathList(NULL);

	// For each entry, recur to check for executable.
	для (struct PathNode *tail = head; !exe && tail;
			       tail = (struct PathNode *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    повернути exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ціле
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    повернути NODE_NORMAL;
}

    void
mch_early_init(void)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    void
mch_exit(ціле r)
{
    exiting = TRUE;

    якщо (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    якщо (raw_out)
    {
	якщо (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    якщо (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    якщо (ArpBase)
	CloseLibrary((struct Library *) ArpBase);
#endif
    якщо (close_win)
	Close(raw_in);
    якщо (r)
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

#define MP(xx)	((struct MsgPort *)((struct FileHandle *) (BADDR(xx)))->fh_Type)

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
#якщо defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    якщо (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#інакше
    якщо (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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
#якщо defined(__AROS__) || defined(__MORPHOS__)
    ціле
mch_get_shellsize(void)
{
    якщо (!term_console)
	повернути FAIL;

    якщо (raw_in && raw_out)
    {
	// Save current console mode.
	ціле old_tmode = cur_tmode;
	char ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	якщо (Write(raw_out, ctrl, sizeof(ctrl)) == sizeof(ctrl))
	{
	    char scan[] = "\x9b""1;1;%d;%d r",
		 answ[sizeof(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    якщо (Read(raw_in, answ, sizeof(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		якщо (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    повернути OK;
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

    повернути FAIL;
}
#інакше
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    ціле
mch_get_shellsize(void)
{
    struct ConUnit  *conUnit;
# ifndef __amigaos4__
    char	    id_a[sizeof(struct InfoData) + 3];
# endif
    struct InfoData *id=0;

    якщо (!term_console)	// not an amiga window
	goto out;

    // insure longword alignment
# ifdef __amigaos4__
    якщо (!(id = AllocDosObject(DOS_INFODATA, 0)))
	goto out;
# інакше
    id = (struct InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    якщо (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    якщо (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (struct Window *)id->id_VolumeNode) == NULL)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	goto out;
    }
    якщо (oldwindowtitle == NULL)
	oldwindowtitle = (char_u *)wb_window->Title;
    якщо (id->id_InUse == (BPTR)NULL)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	повернути FAIL;
    }
    conUnit = (struct ConUnit *) ((struct IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    якщо (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	повернути FAIL;
    }

    повернути OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    повернути FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    void
mch_set_shellsize(void)
{
    якщо (!term_console)
	повернути;

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

#якщо !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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
    struct MsgPort *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    struct MsgPort	    *replyport;
    struct StandardPacket   *packet;
    long		    res1;

    якщо (dos2)
# endif
	повернути DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (struct MsgPort *) CreatePort(NULL, 0);	// use arp function
    якщо (!replyport)
	повернути (0);

    // Allocate space for a packet, make it public and clear it
    packet = (struct StandardPacket *)
	AllocMem((long) sizeof(struct StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    якщо (!packet)
    {
	DeletePort(replyport);
	повернути (0);
    }
    packet->sp_Msg.mn_Node.ln_Name = (char *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_Msg);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    PutMsg(pid, (struct Message *)packet);	// send packet

    WaitPort(replyport);
    GetMsg(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) sizeof(struct StandardPacket));
    DeletePort(replyport);

    повернути (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    ціле
mch_call_shell(
    char_u	*cmd,
    ціле		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    ціле		x;
    ціле		tmode = cur_tmode;
#ifdef AZTEC_C
    ціле		use_execute;
    char_u	*shellcmd = NULL;
    char_u	*shellarg;
#endif
    ціле		retval = 0;

    якщо (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	повернути -1;
    }

    якщо (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    якщо (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#якщо !defined(AZTEC_C)		    // not tested very much
    якщо (cmd == NULL)
    {
# ifdef FEAT_ARP
	якщо (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	інакше
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    інакше
    {
# ifdef FEAT_ARP
	якщо (dos2)
# endif
	    x = SystemTags((char *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	інакше
	    x = Execute((char *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    якщо ((dos2 && x < 0) || (!dos2 && !x))
# інакше
    якщо (x < 0)
# endif
    {
	msg_puts(_("Cannot execute "));
	якщо (cmd == NULL)
	{
	    msg_puts(_("shell "));
	    msg_outtrans(p_sh);
	}
	інакше
	    msg_outtrans(cmd);
	msg_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    інакше якщо (!dos2 || x)
# інакше
    інакше якщо (x)
# endif
    {
	якщо ((x = IoErr()) != 0)
	{
	    якщо (!(options & SHELL_SILENT))
	    {
		msg_putchar('\n');
		msg_outnum((long)x);
		msg_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#інакше	// else part is for AZTEC_C
    якщо (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    інакше
	use_execute = 0;
    якщо (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	якщо (shellcmd == NULL)	    // out of memory, use Execute
	    use_execute = 1;
	інакше
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    якщо (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    якщо (cmd == NULL)
    {
	якщо (use_execute)
	{
# ifdef FEAT_ARP
	    якщо (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    інакше
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	інакше
	    x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg, NULL);
    }
    інакше якщо (use_execute)
    {
# ifdef FEAT_ARP
	якщо (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	інакше
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    інакше якщо (p_st & 1)
	x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg,
							   (char *)cmd, NULL);
    інакше
	x = fexecl((char *)shellcmd, (char *)shellcmd, (char *)shellarg,
					   (char *)p_shcf, (char *)cmd, NULL);
# ifdef FEAT_ARP
    якщо ((dos2 && x < 0) || (!dos2 && x))
# інакше
    якщо (x < 0)
# endif
    {
	msg_puts(_("Cannot execute "));
	якщо (use_execute)
	{
	    якщо (cmd == NULL)
		msg_outtrans(p_sh);
	    інакше
		msg_outtrans(cmd);
	}
	інакше
	{
	    msg_puts(_("shell "));
	    msg_outtrans(shellcmd);
	}
	msg_putchar('\n');
	retval = -1;
    }
    інакше
    {
	якщо (use_execute)
	{
# ifdef FEAT_ARP
	    якщо (!dos2 || x)
# інакше
	    якщо (x)
# endif
		x = IoErr();
	}
	інакше
	    x = wait();
	якщо (x)
	{
	    якщо (!(options & SHELL_SILENT) && !emsg_silent)
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

    якщо ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    якщо (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    якщо (term_console)
	win_resize_on();		// window resize events activated
    повернути retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    void
mch_breakcheck(ціле force UNUSED)
{
   якщо (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
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

#інакше
    long
Chk_Abort(void)
{
    повернути(0L);
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
#інакше
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (sizeof(struct AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    ціле
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    ціле		flags)		// EW_* flags
{
    struct AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    ціле			start_len;
    ціле			matches;
#ifdef __amigaos4__
    struct TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#інакше
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    якщо (Anchor == NULL)
	повернути 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# інакше
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    якщо (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	якщо (starbuf == NULL)
	    goto Return;
	для (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    якщо (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    інакше
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    інакше
	Result = FindFirst((char *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    поки (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#інакше
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	якщо (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	інакше
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    якщо (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    інакше якщо (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    якщо (matches)
	qsort((void *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, sizeof(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    якщо (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    інакше
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#інакше
    vim_free(Anchor);
#endif

    повернути matches;
}

    static ціле
sortcmp(const void *a, const void *b)
{
    char *s = *(char **)a;
    char *t = *(char **)b;

    повернути pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    ціле
mch_has_exp_wildcard(char_u *p)
{
    для ( ; *p; MB_PTR_ADV(p))
    {
	якщо (*p == '\\' && p[1] != NUL)
	    ++p;
	інакше якщо (vim_strchr((char_u *)"*?[(#", *p) != NULL)
	    повернути TRUE;
    }
    повернути FALSE;
}

    ціле
mch_has_wildcard(char_u *p)
{
    для ( ; *p; MB_PTR_ADV(p))
    {
	якщо (*p == '\\' && p[1] != NUL)
	    ++p;
	інакше
	    якщо (vim_strchr((char_u *)"*?[(#$`", *p) != NULL
		    || (*p == '~' && p[1] != NUL))
		повернути TRUE;
    }
    повернути FALSE;
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
    ціле		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    static char_u   *alloced = NULL;	// allocated memory

#ifdef FEAT_ARP
    якщо (!dos2)
	retval = (char_u *)getenv((char *)var);
    інакше
#endif
    {
	VIM_CLEAR(alloced);
	retval = NULL;

	buf = alloc(IOSIZE);
	якщо (buf == NULL)
	    повернути NULL;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	якщо (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    якщо (retval == NULL && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    повернути retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    ціле
mch_setenv(char *var, char *value, ціле x UNUSED)
{
#ifdef FEAT_ARP
    якщо (!dos2)
	повернути setenv(var, value);
#endif

    якщо (SetVar((UBYTE *)var, (UBYTE *)value, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	повернути 0;   // success
    повернути -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    ціле
mch_get_random(char_u *buf, ціле len)
{
    struct Process *proc = (struct Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    ціле status;

    якщо (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    інакше
	status = OK;

    Close(fh);
    повернути status;
}
