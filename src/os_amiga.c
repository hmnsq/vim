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

#अन्तर्भाव "vim.h"
#अन्तर्भाव "version.h"

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

#अन्तर्भाव <exec/memory.h>

#अन्तर्भाव <dos/dostags.h>	    // for 2.0 functions
#अन्तर्भाव <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ifdef __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#अन्यथा
# define	free_fib(x) vim_free(fib)
#endif

#यदि defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#endif

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#यदि defined(__amigaos4__)
स्थिर ध्रुव वर्ण* __attribute__((used)) stackcookie = "$STACK: 1048576";
#नोचेद्यदि defined(__AROS__) || defined(__MORPHOS__)
unsigned long __stack = 1048576;
#नोचेद्यदि defined(__GNUC__) && defined(AMIGA)
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
#नोचेद्यदि !defined(AZTEC_C) && !defined(__AROS__)
स्थिर long dos_packet(संरचना संदेश_Port *, long, long);
#endif
स्थिर पूर्णाङ्क lock2name(BPTR lock, char_u *buf, long   len);
स्थिर शून्य out_अंक(long n);
स्थिर संरचना FileInfoBlock *get_fib(char_u *);
स्थिर पूर्णाङ्क sortcmp(ध्रुव शून्य *a, ध्रुव शून्य *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    स्थिर BPTR
safe_Lock(UBYTE *name, long mode)
{
    संरचना Process *me = (संरचना Process *)FindTask(रिक्त);
    APTR oldwin = me->pr_WindowPtr;
    BPTR flock;

    me->pr_WindowPtr = (APTR)-1L;
    flock = Lock(name, mode);
    me->pr_WindowPtr = oldwin;
    प्रत्यागम flock;
}

स्थिर BPTR		raw_in = (BPTR)रिक्त;
स्थिर BPTR		raw_out = (BPTR)रिक्त;
स्थिर पूर्णाङ्क		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
संरचना IntuitionBase	*IntuitionBase = रिक्त;
#endif
#ifdef FEAT_ARP
संरचना ArpBase		*ArpBase = रिक्त;
#endif

स्थिर संरचना Window	*wb_window;
स्थिर char_u		*oldwindowtitle = रिक्त;

#ifdef FEAT_ARP
पूर्णाङ्क			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#endif
पूर्णाङ्क			size_set = FALSE;   // set to TRUE if window size was set

#ifdef __GNUC__
स्थिर वर्ण version[] __attribute__((used)) =
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

    शून्य
win_resize_on(शून्य)
{
    OUT_STR_NF("\033[12{");
}

    शून्य
win_resize_off(शून्य)
{
    OUT_STR_NF("\033[12}");
}

    शून्य
mch_write(char_u *p, पूर्णाङ्क len)
{
    Write(raw_out, (वर्ण *)p, (long)len);
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
    पूर्णाङ्क
mch_inchar(
    char_u  *buf,
    पूर्णाङ्क	    maxlen,
    long    time,		// milliseconds
    पूर्णाङ्क	    tb_change_cnt UNUSED)
{
    पूर्णाङ्क	    len;
    long    utime;

    यदि (time >= 0)
    {
	यदि (time == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	अन्यथा
	    utime = time * 1000L;   // convert from milli to micro secs
	यदि (WaitForChar(raw_in, utime) == 0)	// no character available
	    प्रत्यागम 0;
    }
    अन्यथा    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	यदि (WaitForChar(raw_in, p_ut * 1000L) == 0)
	{
	    यदि (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (पूर्णाङ्क)KE_CURSORHOLD;
		प्रत्यागम 3;
	    }
	    before_blocking();
	}
    }

    प्रति (;;)	    // repeat until we got a character
    {
	len = Read(raw_in, (वर्ण *)buf, (long)maxlen / निवेश_conv.vc_factor);
	यदि (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    यदि (निवेश_conv.vc_type != CONV_NONE)
		len = convert_निवेश(buf, len, maxlen);
	    प्रत्यागम len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    पूर्णाङ्क
mch_char_avail(शून्य)
{
    प्रत्यागम (WaitForChar(raw_in, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(पूर्णाङ्क special)
{
#यदि defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    प्रत्यागम (long_u)AvailMem(MEMF_ANY) >> 10;
#अन्यथा
    प्रत्यागम (long_u)(AvailMem(special ? (long)MEMF_CHIP : (long)MEMF_ANY)) >> 10;
#endif
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    शून्य
mch_delay(long msec, पूर्णाङ्क flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ifdef LATTICE
    शून्य	    Delay(long);
#endif

    यदि (msec <= 0)
	प्रत्यागम;

    यदि (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    अन्यथा
	WaitForChar(raw_in, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    शून्य
mch_suspend(शून्य)
{
    suspend_shell();
}

#ifndef DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#endif

    शून्य
mch_init(शून्य)
{
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    स्थिर वर्ण	    intlibname[] = "intuition.library";
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
	संरचना Process *me = (संरचना Process *)FindTask(रिक्त);
	me->pr_WindowPtr = (APTR)-1L;
    }

    Columns = 80;
    Rows = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    यदि (raw_in == (BPTR)रिक्त)
    {
	raw_in = निवेश_3();
	raw_out = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	यदि (!IsInteractive(raw_in))
	    raw_in = raw_out;
	अन्यथा यदि (!IsInteractive(raw_out))
	    raw_out = raw_in;
    }

    out_flush();

    wb_window = रिक्त;
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    यदि ((IntuitionBase = (संरचना IntuitionBase *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == रिक्त)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#endif
}

#अन्तर्भाव <workbench/startup.h>

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

    पूर्णाङ्क
mch_check_win(पूर्णाङ्क argc, वर्ण **argv)
{
    पूर्णाङ्क		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2SIZE];
    स्थिर char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    स्थिर char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    संरचना WBArg    *argp;
    पूर्णाङ्क		    ac;
    वर्ण	    *av;
    char_u	    *device = रिक्त;
    पूर्णाङ्क		    exitval = 4;
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    संरचना Library  *DosBase;
#endif
    पूर्णाङ्क		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    DosBase = OpenLibrary(DOS_LIBRARY, 37L);
    यदि (DosBase != रिक्त)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(DosBase);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    अन्यथा	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	exit(3);
# अन्यथा
		    // need arp functions for dos 1.x
	यदि (!(ArpBase = (संरचना ArpBase *) OpenLibrary((UBYTE *)ArpName, ArpVersion)))
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
    प्रति (i = 1; i < argc; ++i)
	यदि (argv[i][0] == '-')
	{
	    विकल्प (argv[i][1])
	    {
	    पक्ष 'f':
		usewin = TRUE;
		भङ्ग;

	    पक्ष 'd':
		यदि (i < argc - 1
#ifdef FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#endif
		   )
		    device = (char_u *)argv[i + 1];
		भङ्ग;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    यदि (argc != 0
	    && device == रिक्त
	    && (IsInteractive(निवेश_3()) || IsInteractive(Output())))
	प्रत्यागम OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    यदि (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	प्रति (i = -1; i < 3; ++i)
	{
	    यदि (i >= 0)
		device = constrings[i];
	    यदि (device != रिक्त && (raw_in = Open((UBYTE *)device,
					   (long)MODE_NEWFILE)) != (BPTR)रिक्त)
		भङ्ग;
	}
	यदि (raw_in == (BPTR)रिक्त)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    गच्छ exit;
	}
	raw_out = raw_in;
	close_win = TRUE;
	प्रत्यागम OK;
    }

    यदि ((nilfh = Open((UBYTE *)"NIL:", (long)MODE_NEWFILE)) == (BPTR)रिक्त)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	गच्छ exit;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#यदि !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((वर्ण *)buf1, "t:nc%p", FindTask(0));
#अन्यथा
    sprintf((वर्ण *)buf1, "t:nc%ld", (long)buf1);
#endif
    यदि ((fh = Open((UBYTE *)buf1, (long)MODE_NEWFILE)) == (BPTR)रिक्त)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((वर्ण *)buf1);
	mch_errmsg("\n");
	गच्छ exit;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    यदि (argc == 0)	// run from workbench
	ac = ((संरचना WBStartup *)argv)->sm_अंक_Args;
    अन्यथा
	ac = argc;
    प्रति (i = 0; i < ac; ++i)
    {
	यदि (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((संरचना WBStartup *)argv)->sm_ArgList[i]);
	    यदि (argp->wa_Lock)
		(शून्य)lock2name(argp->wa_Lock, buf2, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    यदि (dos2)	    // use 2.0 function
#endif
		AddPart((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (long)(BUF2SIZE - 1));
#ifdef FEAT_ARP
	    अन्यथा	    // use arp function
		TackOn((वर्ण *)buf2, argp->wa_Name);
#endif
	    av = (वर्ण *)buf2;
	}
	अन्यथा
	    av = argv[i];

	// skip '-d' or "-dev" option
	यदि (av[0] == '-' && av[1] == 'd'
#ifdef FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#endif
		)
	{
	    ++i;
	    अनुवर्त;
	}
	यदि (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (long)strlen(av));
	यदि (vim_strchr((char_u *)av, ' '))
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
    प्रति (i = -1; i < 3; ++i)
    {
	यदि (i >= 0)
	    device = constrings[i];
	अन्यथा यदि (device == रिक्त)
	    अनुवर्त;
	sprintf((वर्ण *)buf2, "newcli <nil: >nil: %s from %s", (वर्ण *)device, (वर्ण *)buf1);
#ifdef FEAT_ARP
	यदि (dos2)
	{
#endif
	    यदि (!SystemTags((UBYTE *)buf2, SYS_UserShell, TRUE, TAG_DONE))
		भङ्ग;
#ifdef FEAT_ARP
	}
	अन्यथा
	{
	    यदि (Execute((UBYTE *)buf2, nilfh, nilfh))
		भङ्ग;
	}
#endif
    }
    यदि (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	गच्छ exit;
    }
    exitval = 0;    // The Execute succeeded: exit this program

exit:
    यदि (nilfh)
	Close(nilfh);
#ifdef FEAT_ARP
    यदि (ArpBase)
	CloseLibrary((संरचना Library *) ArpBase);
#endif
    exit(exitval);
    // NOTREACHED
    प्रत्यागम FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    पूर्णाङ्क
mch_निवेश_isatty(शून्य)
{
    प्रत्यागम TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    शून्य
fname_case(
    char_u	*name,
    पूर्णाङ्क		len UNUSED)		// buffer size, ignored here
{
    संरचना FileInfoBlock    *fib;
    size_t		    flen;

    fib = get_fib(name);
    यदि (fib == रिक्त)
	प्रत्यागम;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ifdef __amigaos4__
    यदि (fib->fib_DirEntryType == ST_मूल)
	strcat(fib->fib_FileName, ":");
#endif
    यदि (flen == strlen(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    स्थिर संरचना FileInfoBlock *
get_fib(char_u *fname)
{
    BPTR		    flock;
    संरचना FileInfoBlock    *fib;

    यदि (fname == रिक्त)	    // safety check
	प्रत्यागम रिक्त;
#ifdef __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#अन्यथा
    fib = ALLOC_ONE(संरचना FileInfoBlock);
#endif
    यदि (fib == रिक्त)
	प्रत्यागम;

    flock = safe_Lock((UBYTE *)fname, (long)ACCESS_READ);
    यदि (flock == (BPTR)रिक्त || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = रिक्त;
    }
    यदि (flock)
	UnLock(flock);
    प्रत्यागम fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    शून्य
mch_settitle(char_u *title, char_u *icon)
{
    यदि (wb_window != रिक्त && title != रिक्त)
	SetWindowTitles(wb_window, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    शून्य
mch_restore_title(पूर्णाङ्क which)
{
    यदि (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, रिक्त);
}

    पूर्णाङ्क
mch_can_restore_title(शून्य)
{
    प्रत्यागम (wb_window != रिक्त);
}

    पूर्णाङ्क
mch_can_restore_icon(शून्य)
{
    प्रत्यागम FALSE;
}

    शून्य
mch_setmouse(पूर्णाङ्क on UNUSED)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    पूर्णाङ्क
mch_get_user_name(char_u *s, पूर्णाङ्क len)
{
#यदि defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    संरचना passwd   *pwd = getpwuid(getuid());

    यदि (pwd != रिक्त && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	प्रत्यागम OK;
    }
#endif
    *s = NUL;
    प्रत्यागम FAIL;
}

/*
 * Insert host name is s[len].
 */
    शून्य
mch_get_host_name(char_u *s, पूर्णाङ्क len)
{
#यदि defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((वर्ण *)s, len);
#अन्यथा
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#endif
}

/*
 * return process ID
 */
    long
mch_get_pid(शून्य)
{
#यदि defined(__amigaos4__)
    प्रत्यागम (long) getpid();
#नोचेद्यदि defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    प्रत्यागम((long)FindTask(0));
#अन्यथा
    प्रत्यागम (long)0;
#endif
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    पूर्णाङ्क
mch_dirname(char_u *buf, पूर्णाङ्क len)
{
    प्रत्यागम mch_FullName((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
mch_FullName(
    char_u	*fname,
    char_u	*buf,
    पूर्णाङ्क		len,
    पूर्णाङ्क		force)
{
    BPTR	l;
    पूर्णाङ्क		retval = FAIL;
    पूर्णाङ्क		i;

    // Lock the file.  If it exists, we can get the exact name.
    यदि ((l = safe_Lock((UBYTE *)fname, (long)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (long)len - 1);
	UnLock(l);
    }
    अन्यथा यदि (force || !mch_isFullName(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	यदि ((l = Lock((UBYTE *)"", (long)ACCESS_READ)) != (BPTR)रिक्त)
	{
	    retval = lock2name(l, buf, (long)len);
	    UnLock(l);
	    यदि (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		यदि (i == 0 || *fname != NUL)
		{
		    यदि (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    यदि (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    प्रत्यागम retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    पूर्णाङ्क
mch_isFullName(char_u *fname)
{
    प्रत्यागम (vim_strchr(fname, ':') != रिक्त && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    स्थिर पूर्णाङ्क
lock2name(BPTR lock, char_u *buf, long len)
{
#ifdef FEAT_ARP
    यदि (dos2)		    // use 2.0 function
#endif
	प्रत्यागम ((पूर्णाङ्क)NameFromLock(lock, (UBYTE *)buf, len) ? OK : FAIL);
#ifdef FEAT_ARP
    अन्यथा		// use arp function
	प्रत्यागम ((पूर्णाङ्क)PathName(lock, (वर्ण *)buf, (long)(len/32)) ? OK : FAIL);
#endif
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    long
mch_getperm(char_u *name)
{
    संरचना FileInfoBlock    *fib;
    long		    retval = -1;

    fib = get_fib(name);
    यदि (fib == रिक्त)
	प्रत्यागम -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    प्रत्यागम retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
mch_setperm(char_u *name, long perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    प्रत्यागम (SetProtection((UBYTE *)name, (long)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    शून्य
mch_hide(char_u *name UNUSED)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    पूर्णाङ्क
mch_isdir(char_u *name)
{
    संरचना FileInfoBlock    *fib;
    पूर्णाङ्क			    retval = FALSE;

    fib = get_fib(name);
    यदि (fib == रिक्त)
	प्रत्यागम FALSE;

#ifdef __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#अन्यथा
    retval = ((fib->fib_DirEntryType >= 0) ? TRUE : FALSE);
#endif
    free_fib(fib);
    प्रत्यागम retval;
}

/*
 * Create directory "name".
 */
    पूर्णाङ्क
mch_mkdir(char_u *name)
{
    BPTR	lock;

    lock = CreateDir(name);
    यदि (lock == रिक्त)
	प्रत्यागम -1;

    UnLock(lock);
    प्रत्यागम 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    पूर्णाङ्क
mch_can_exe(char_u *name, char_u **path UNUSED, पूर्णाङ्क use_path)
{
    पूर्णाङ्क exe = -1;
#ifdef __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = LoadSeg(name);

    यदि (seg && GetSegListInfoTags(seg, GSLI_Native, रिक्त, TAG_DONE) !=
	    GetSegListInfoTags(seg, GSLI_68KHUNK, रिक्त, TAG_DONE))
    {
	// Test if file permissions allow execution.
	संरचना ExamineData *exd = ExamineObjectTags(EX_String_Name_निवेश, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    अन्यथा
    {
	exe = 0;
    }

    UnLoadSeg(seg);

    // Search for executable in path if applicable.
    यदि (!exe && use_path)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	संरचना Path_गाँठ *head = DupCmdPathList(रिक्त);

	// For each entry, recur to check for executable.
	प्रति (संरचना Path_गाँठ *tail = head; !exe && tail;
			       tail = (संरचना Path_गाँठ *) BADDR(tail->pn_Next))
	{
	    SetCurrentDir(tail->pn_Lock);
	    exe = mch_can_exe(name, path, 0);
	}

	// Go back to where we were.
	FreeCmdPathList(head);
	SetCurrentDir(cwd);
    }
#endif
    प्रत्यागम exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    पूर्णाङ्क
mch_nodetype(char_u *name UNUSED)
{
    // TODO
    प्रत्यागम गाँठ_NORMAL;
}

    शून्य
mch_early_init(शून्य)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    शून्य
mch_exit(पूर्णाङ्क r)
{
    exiting = TRUE;

    यदि (raw_in)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    out_char('\n');
    यदि (raw_out)
    {
	यदि (term_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    यदि (size_set)
		OUT_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	out_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ifdef FEAT_ARP
    यदि (ArpBase)
	CloseLibrary((संरचना Library *) ArpBase);
#endif
    यदि (close_win)
	Close(raw_in);
    यदि (r)
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

#define MP(xx)	((संरचना संदेश_Port *)((संरचना FileHandle *) (BADDR(xx)))->fh_Type)

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
    शून्य
mch_settmode(tmode_T tmode)
{
#यदि defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    यदि (!SetMode(raw_in, tmode == TMODE_RAW ? 1 : 0))
#अन्यथा
    यदि (dos_packet(MP(raw_in), (long)ACTION_SCREEN_MODE,
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

#अन्तर्भाव <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#यदि defined(__AROS__) || defined(__MORPHOS__)
    पूर्णाङ्क
mch_get_shellsize(शून्य)
{
    यदि (!term_console)
	प्रत्यागम FAIL;

    यदि (raw_in && raw_out)
    {
	// Save current console mode.
	पूर्णाङ्क old_tmode = cur_tmode;
	वर्ण ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	यदि (Write(raw_out, ctrl, परिमाण(ctrl)) == परिमाण(ctrl))
	{
	    वर्ण scan[] = "\x9b""1;1;%d;%d r",
		 answ[परिमाण(scan) + 8] = { '\0' };

	    // Read return sequence from input.
	    यदि (Read(raw_in, answ, परिमाण(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		यदि (sscanf(answ, scan, &Rows, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    प्रत्यागम OK;
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

    प्रत्यागम FAIL;
}
#अन्यथा
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    पूर्णाङ्क
mch_get_shellsize(शून्य)
{
    संरचना ConUnit  *conUnit;
# ifndef __amigaos4__
    वर्ण	    id_a[परिमाण(संरचना InfoData) + 3];
# endif
    संरचना InfoData *id=0;

    यदि (!term_console)	// not an amiga window
	गच्छ out;

    // insure longword alignment
# ifdef __amigaos4__
    यदि (!(id = AllocDosObject(DOS_INFODATA, 0)))
	गच्छ out;
# अन्यथा
    id = (संरचना InfoData *)(((long)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    यदि (size_set)
	OUT_STR("\233t\233u");	// CSI t CSI u
    out_flush();

    यदि (dos_packet(MP(raw_out), (long)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_window = (संरचना Window *)id->id_Volume_गाँठ) == रिक्त)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	term_console = FALSE;
	गच्छ out;
    }
    यदि (oldwindowtitle == रिक्त)
	oldwindowtitle = (char_u *)wb_window->Title;
    यदि (id->id_InUse == (BPTR)रिक्त)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	प्रत्यागम FAIL;
    }
    conUnit = (संरचना ConUnit *) ((संरचना IOStdReq *) id->id_InUse)->io_Unit;

    // get window size
    Rows = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    यदि (Rows < 0 || Rows > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	Rows = 24;
	term_console = FALSE;
	प्रत्यागम FAIL;
    }

    प्रत्यागम OK;
out:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    प्रत्यागम FAIL;
}
#endif

/*
 * Try to set the real window size to Rows and Columns.
 */
    शून्य
mch_set_shellsize(शून्य)
{
    यदि (!term_console)
	प्रत्यागम;

    size_set = TRUE;
    out_char(CSI);
    out_अंक((long)Rows);
    out_char('t');
    out_char(CSI);
    out_अंक((long)Columns);
    out_char('u');
    out_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    शून्य
mch_new_shellsize(शून्य)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    स्थिर शून्य
out_अंक(long n)
{
    OUT_STR_NF(tltoa((unsigned long)n));
}

#यदि !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    स्थिर long
dos_packet(
    संरचना संदेश_Port *pid,    // process identifier ... (handlers message port)
    long	    action, // packet type ... (what you want handler to do)
    long	    arg)    // single argument
{
# ifdef FEAT_ARP
    संरचना संदेश_Port	    *replyport;
    संरचना StandardPacket   *packet;
    long		    res1;

    यदि (dos2)
# endif
	प्रत्यागम DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (संरचना संदेश_Port *) CreatePort(रिक्त, 0);	// use arp function
    यदि (!replyport)
	प्रत्यागम (0);

    // Allocate space for a packet, make it public and clear it
    packet = (संरचना StandardPacket *)
	AllocMem((long) परिमाण(संरचना StandardPacket), MEMF_PUBLIC | MEMF_CLEAR);
    यदि (!packet)
    {
	DeletePort(replyport);
	प्रत्यागम (0);
    }
    packet->sp_संदेश.mn_गाँठ.ln_Name = (वर्ण *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_संदेश);
    packet->sp_Pkt.dp_Port = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_संदेश(pid, (संरचना Message *)packet);	// send packet

    WaitPort(replyport);
    Get_संदेश(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (long) परिमाण(संरचना StandardPacket));
    DeletePort(replyport);

    प्रत्यागम (res1);
# endif
}
#endif // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    पूर्णाङ्क
mch_call_shell(
    char_u	*cmd,
    पूर्णाङ्क		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    पूर्णाङ्क		x;
    पूर्णाङ्क		tmode = cur_tmode;
#ifdef AZTEC_C
    पूर्णाङ्क		use_execute;
    char_u	*shellcmd = रिक्त;
    char_u	*shellarg;
#endif
    पूर्णाङ्क		retval = 0;

    यदि (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	प्रत्यागम -1;
    }

    यदि (term_console)
	win_resize_off();	    // window resize events de-activated
    out_flush();

    यदि (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = Lock((UBYTE *)"", (long)ACCESS_READ);   // remember current dir

#यदि !defined(AZTEC_C)		    // not tested very much
    यदि (cmd == रिक्त)
    {
# ifdef FEAT_ARP
	यदि (dos2)
# endif
	    x = SystemTags(p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	अन्यथा
	    x = Execute(p_sh, raw_in, raw_out);
# endif
    }
    अन्यथा
    {
# ifdef FEAT_ARP
	यदि (dos2)
# endif
	    x = SystemTags((वर्ण *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	अन्यथा
	    x = Execute((वर्ण *)cmd, 0L, raw_out);
# endif
    }
# ifdef FEAT_ARP
    यदि ((dos2 && x < 0) || (!dos2 && !x))
# अन्यथा
    यदि (x < 0)
# endif
    {
	संदेश_puts(_("Cannot execute "));
	यदि (cmd == रिक्त)
	{
	    संदेश_puts(_("shell "));
	    संदेश_outtrans(p_sh);
	}
	अन्यथा
	    संदेश_outtrans(cmd);
	संदेश_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    अन्यथा यदि (!dos2 || x)
# अन्यथा
    अन्यथा यदि (x)
# endif
    {
	यदि ((x = IoErr()) != 0)
	{
	    यदि (!(options & SHELL_SILENT))
	    {
		संदेश_putchar('\n');
		संदेश_outnum((long)x);
		संदेश_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#अन्यथा	// else part is for AZTEC_C
    यदि (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    अन्यथा
	use_execute = 0;
    यदि (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	यदि (shellcmd == रिक्त)	    // out of memory, use Execute
	    use_execute = 1;
	अन्यथा
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    यदि (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    यदि (cmd == रिक्त)
    {
	यदि (use_execute)
	{
# ifdef FEAT_ARP
	    यदि (dos2)
# endif
		x = SystemTags((UBYTE *)p_sh, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	    अन्यथा
		x = !Execute((UBYTE *)p_sh, raw_in, raw_out);
# endif
	}
	अन्यथा
	    x = fexecl((वर्ण *)shellcmd, (वर्ण *)shellcmd, (वर्ण *)shellarg, रिक्त);
    }
    अन्यथा यदि (use_execute)
    {
# ifdef FEAT_ARP
	यदि (dos2)
# endif
	    x = SystemTags((UBYTE *)cmd, SYS_UserShell, TRUE, TAG_DONE);
# ifdef FEAT_ARP
	अन्यथा
	    x = !Execute((UBYTE *)cmd, 0L, raw_out);
# endif
    }
    अन्यथा यदि (p_st & 1)
	x = fexecl((वर्ण *)shellcmd, (वर्ण *)shellcmd, (वर्ण *)shellarg,
							   (वर्ण *)cmd, रिक्त);
    अन्यथा
	x = fexecl((वर्ण *)shellcmd, (वर्ण *)shellcmd, (वर्ण *)shellarg,
					   (वर्ण *)p_shcf, (वर्ण *)cmd, रिक्त);
# ifdef FEAT_ARP
    यदि ((dos2 && x < 0) || (!dos2 && x))
# अन्यथा
    यदि (x < 0)
# endif
    {
	संदेश_puts(_("Cannot execute "));
	यदि (use_execute)
	{
	    यदि (cmd == रिक्त)
		संदेश_outtrans(p_sh);
	    अन्यथा
		संदेश_outtrans(cmd);
	}
	अन्यथा
	{
	    संदेश_puts(_("shell "));
	    संदेश_outtrans(shellcmd);
	}
	संदेश_putchar('\n');
	retval = -1;
    }
    अन्यथा
    {
	यदि (use_execute)
	{
# ifdef FEAT_ARP
	    यदि (!dos2 || x)
# अन्यथा
	    यदि (x)
# endif
		x = IoErr();
	}
	अन्यथा
	    x = wait();
	यदि (x)
	{
	    यदि (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		संदेश_putchar('\n');
		संदेश_outnum((long)x);
		संदेश_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#endif	// AZTEC_C

    यदि ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	UnLock(mydir);
    यदि (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    यदि (term_console)
	win_resize_on();		// window resize events activated
    प्रत्यागम retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    शून्य
mch_breakcheck(पूर्णाङ्क force UNUSED)
{
   यदि (SetSignal(0L, (long)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ifdef _DCC
# define Chk_Abort chkabort
#endif

#ifdef LATTICE
शून्य __regargs __chkabort(शून्य);

शून्य __regargs __chkabort(शून्य)
{}

#अन्यथा
    long
Chk_Abort(शून्य)
{
    प्रत्यागम(0L);
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
#अन्यथा
# define ANCHOR_BUF_SIZE (512)
# define ANCHOR_SIZE (परिमाण(संरचना AnchorPath) + ANCHOR_BUF_SIZE)
#endif

    पूर्णाङ्क
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    पूर्णाङ्क		flags)		// EW_* flags
{
    संरचना AnchorPath	*Anchor;
    LONG		Result;
    char_u		*starbuf, *sp, *dp;
    पूर्णाङ्क			start_len;
    पूर्णाङ्क			matches;
#ifdef __amigaos4__
    संरचना TagItem	AnchorTags[] = {
	{ADO_Strlen, ANCHOR_BUF_SIZE},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{TAG_DONE, 0L}
    };
#endif

    start_len = gap->ga_len;

    // Get our AnchorBase
#ifdef __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, AnchorTags);
#अन्यथा
    Anchor = alloc_clear(ANCHOR_SIZE);
#endif
    यदि (Anchor == रिक्त)
	प्रत्यागम 0;

#ifndef __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_SIZE;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# अन्यथा
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#endif

#ifdef FEAT_ARP
    यदि (dos2)
    {
#endif
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	यदि (starbuf == रिक्त)
	    गच्छ Return;
	प्रति (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    यदि (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    अन्यथा
		*dp++ = *sp;
	}
	*dp = NUL;
	Result = MatchFirst((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ifdef FEAT_ARP
    }
    अन्यथा
	Result = FindFirst((वर्ण *)pat, Anchor);
#endif

    /*
     * Loop to get all matches.
     */
    यावत् (Result == 0)
    {
#ifdef __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_Buffer, flags);
#अन्यथा
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#endif
#ifdef FEAT_ARP
	यदि (dos2)
#endif
	    Result = MatchNext(Anchor);
#ifdef FEAT_ARP
	अन्यथा
	    Result = FindNext(Anchor);
#endif
    }
    matches = gap->ga_len - start_len;

    यदि (Result == ERROR_BUFFER_OVERFLOW)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    अन्यथा यदि (matches == 0 && Result != ERROR_OBJECT_NOT_FOUND
			  && Result != ERROR_DEVICE_NOT_MOUNTED
			  && Result != ERROR_NO_MORE_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    यदि (matches)
	qsort((शून्य *)(((char_u **)gap->ga_data) + start_len),
				  (size_t)matches, परिमाण(char_u *), sortcmp);

    // Free the wildcard stuff
#ifdef FEAT_ARP
    यदि (dos2)
#endif
	MatchEnd(Anchor);
#ifdef FEAT_ARP
    अन्यथा
	FreeAnchorChain(Anchor);
#endif

Return:
#ifdef __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#अन्यथा
    vim_free(Anchor);
#endif

    प्रत्यागम matches;
}

    स्थिर पूर्णाङ्क
sortcmp(ध्रुव शून्य *a, ध्रुव शून्य *b)
{
    वर्ण *s = *(वर्ण **)a;
    वर्ण *t = *(वर्ण **)b;

    प्रत्यागम pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    पूर्णाङ्क
mch_has_exp_wildcard(char_u *p)
{
    प्रति ( ; *p; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा यदि (vim_strchr((char_u *)"*?[(#", *p) != रिक्त)
	    प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
}

    पूर्णाङ्क
mch_has_wildcard(char_u *p)
{
    प्रति ( ; *p; MB_PTR_ADV(p))
    {
	यदि (*p == '\\' && p[1] != NUL)
	    ++p;
	अन्यथा
	    यदि (vim_strchr((char_u *)"*?[(#$`", *p) != रिक्त
		    || (*p == '~' && p[1] != NUL))
		प्रत्यागम TRUE;
    }
    प्रत्यागम FALSE;
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
    पूर्णाङ्क		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    स्थिर char_u   *alloced = रिक्त;	// allocated memory

#ifdef FEAT_ARP
    यदि (!dos2)
	retval = (char_u *)getenv((वर्ण *)var);
    अन्यथा
#endif
    {
	VIM_CLEAR(alloced);
	retval = रिक्त;

	buf = alloc(IOSIZE);
	यदि (buf == रिक्त)
	    प्रत्यागम रिक्त;

	len = GetVar((UBYTE *)var, buf, (long)(IOSIZE - 1), (long)0);
	यदि (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    यदि (retval == रिक्त && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    प्रत्यागम retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    पूर्णाङ्क
mch_setenv(वर्ण *var, वर्ण *मूल्य, पूर्णाङ्क x UNUSED)
{
#ifdef FEAT_ARP
    यदि (!dos2)
	प्रत्यागम setenv(var, मूल्य);
#endif

    यदि (SetVar((UBYTE *)var, (UBYTE *)मूल्य, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	प्रत्यागम 0;   // success
    प्रत्यागम -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    पूर्णाङ्क
mch_get_random(char_u *buf, पूर्णाङ्क len)
{
    संरचना Process *proc = (संरचना Process *) FindTask(0L);
    APTR win = proc->pr_WindowPtr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_WindowPtr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", MODE_OLDFILE);

    proc->pr_WindowPtr = win;

    पूर्णाङ्क status;

    यदि (!fh || Read(fh, buf, len) != len)
	status = FAIL;
    अन्यथा
	status = OK;

    Close(fh);
    प्रत्यागम status;
}
