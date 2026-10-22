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

#ਅਨ੍ਤਰ੍ਭਾਵ "vim.h"
#ਅਨ੍ਤਰ੍ਭਾਵ "version.h"

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ ਖਿੜਕੀ
# undef ਖਿੜਕੀ	// Amiga has its own Window definition
#ਜੇ_ਅੰਤ

#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ TRUE		// will be redefined by exec/types.h
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ FALSE

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#ਜੇ_ਅੰਤ

// XXX These are included from os_amiga.h
// #include <exec/types.h>
// #include <libraries/dos.h>
// #include <libraries/dosextens.h>
// #include <proto/exec.h>
// #include <proto/dos.h>
// #include <proto/intuition.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <exec/memory.h>

#ਅਨ੍ਤਰ੍ਭਾਵ <dos/dostags.h>	    // for 2.0 functions
#ਅਨ੍ਤਰ੍ਭਾਵ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#ਨਹੀਂਤਾਂ
# define	free_fib(x) vim_free(fib)
#ਜੇ_ਅੰਤ

#ਜੇ defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#ਜੇ_ਅੰਤ

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ਜੇ defined(__amigaos4__)
ਸਥਿਰ ਅਚਲ ਅੱਖਰ* __attribute__((ਵਰਤਿਆ)) stackcookie = "$STACK: 1048576";
#ਜਾਂਜੇ defined(__AROS__) || defined(__MORPHOS__)
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __stack = 1048576;
#ਜਾਂਜੇ defined(__GNUC__) && defined(AMIGA)
ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ __stack = 262144;
#ਜੇ_ਅੰਤ

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	TRUE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ TRUE (1)
#ਪਰਿਭਾਸ਼ਾ_ਰੱਦ	FALSE
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ FALSE (0)

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#ਜਾਂਜੇ !defined(AZTEC_C) && !defined(__AROS__)
ਸਥਿਰ ਲੰਮਾ dos_packet(ਬਣਤਰ ਸੁਨੇਹਾ_ਪੋਰਟ *, ਲੰਮਾ, ਲੰਮਾ);
#ਜੇ_ਅੰਤ
ਸਥਿਰ ਪੂਰਨ lock2name(BPTR ਤਾਲਾ, char_u *buf, ਲੰਮਾ   len);
ਸਥਿਰ ਖਾਲੀ ਬਾਹਰਲਾ_ਅੰਕ(ਲੰਮਾ n);
ਸਥਿਰ ਬਣਤਰ File_Info_ਖੰਡ *get_fib(char_u *);
ਸਥਿਰ ਪੂਰਨ sortcmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ਸਥਿਰ BPTR
ਸੁਰੱਖਿਅਤ_ਤਾਲਾ(UBYTE *name, ਲੰਮਾ ਬਹੁਲਕ)
{
    ਬਣਤਰ Process *me = (ਬਣਤਰ Process *)ਲੱਭੋ_Task(ਨਿਰਾ);
    APTR oldwin = me->pr_ਖਿੜਕੀ_Ptr;
    BPTR flock;

    me->pr_ਖਿੜਕੀ_Ptr = (APTR)-1L;
    flock = ਤਾਲਾ_2(name, ਬਹੁਲਕ);
    me->pr_ਖਿੜਕੀ_Ptr = oldwin;
    ਵਾਪਸ flock;
}

ਸਥਿਰ BPTR		raw_ਵਿੱਚ = (BPTR)ਨਿਰਾ;
ਸਥਿਰ BPTR		raw_ਬਾਹਰਲਾ = (BPTR)ਨਿਰਾ;
ਸਥਿਰ ਪੂਰਨ		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ਬਣਤਰ Intuition_ਅਧਾਰ	*Intuition_ਅਧਾਰ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
ਬਣਤਰ Arp_ਅਧਾਰ		*Arp_ਅਧਾਰ = ਨਿਰਾ;
#ਜੇ_ਅੰਤ

ਸਥਿਰ ਬਣਤਰ ਖਿੜਕੀ	*wb_ਖਿੜਕੀ;
ਸਥਿਰ char_u		*oldwindowtitle = ਨਿਰਾ;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
ਪੂਰਨ			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#ਜੇ_ਅੰਤ
ਪੂਰਨ			ਮਾਪ_ਸੈੱਟ = FALSE;   // set to TRUE if window size was set

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __GNUC__
ਸਥਿਰ ਅੱਖਰ ਸੰਸਕਰਣ_2[] __attribute__((ਵਰਤਿਆ)) =
    "\0$VER: Vim "
    VIM_ਸੰਸਕਰਣ_MAJOR_STR "."
    VIM_ਸੰਸਕਰਣ_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
win_resize_on(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_STR_NF("\033[12{");
}

    ਖਾਲੀ
win_resize_off(ਖਾਲੀ)
{
    ਬਾਹਰਲਾ_STR_NF("\033[12}");
}

    ਖਾਲੀ
mch_write(char_u *p, ਪੂਰਨ len)
{
    Write(raw_ਬਾਹਰਲਾ, (ਅੱਖਰ *)p, (ਲੰਮਾ)len);
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
    ਪੂਰਨ
mch_inchar(
    char_u  *buf,
    ਪੂਰਨ	    maxlen,
    ਲੰਮਾ    ਸਮਾਂ,		// milliseconds
    ਪੂਰਨ	    tb_change_cnt ਅਣਵਰਤਿਆ)
{
    ਪੂਰਨ	    len;
    ਲੰਮਾ    utime;

    ਜੇ (ਸਮਾਂ >= 0)
    {
	ਜੇ (ਸਮਾਂ == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	ਨਹੀਂਤਾਂ
	    utime = ਸਮਾਂ * 1000L;   // convert from milli to micro secs
	ਜੇ (WaitForChar(raw_ਵਿੱਚ, utime) == 0)	// no character available
	    ਵਾਪਸ 0;
    }
    ਨਹੀਂਤਾਂ    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ਜੇ (WaitForChar(raw_ਵਿੱਚ, p_ut * 1000L) == 0)
	{
	    ਜੇ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ਪੂਰਨ)KE_CURSORHOLD;
		ਵਾਪਸ 3;
	    }
	    before_blocking();
	}
    }

    ਲਈ (;;)	    // repeat until we got a character
    {
	len = Read(raw_ਵਿੱਚ, (ਅੱਖਰ *)buf, (ਲੰਮਾ)maxlen / ਨਿਵੇਸ਼_conv.vc_ਗੁਣਕ);
	ਜੇ (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ਜੇ (ਨਿਵੇਸ਼_conv.vc_type != CONV_NONE)
		len = convert_ਨਿਵੇਸ਼(buf, len, maxlen);
	    ਵਾਪਸ len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    ਪੂਰਨ
mch_char_avail(ਖਾਲੀ)
{
    ਵਾਪਸ (WaitForChar(raw_ਵਿੱਚ, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(ਪੂਰਨ special)
{
#ਜੇ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ਵਾਪਸ (long_u)AvailMem(MEMF_ANY) >> 10;
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (long_u)(AvailMem(special ? (ਲੰਮਾ)MEMF_CHIP : (ਲੰਮਾ)MEMF_ANY)) >> 10;
#ਜੇ_ਅੰਤ
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    ਖਾਲੀ
mch_delay(ਲੰਮਾ msec, ਪੂਰਨ flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LATTICE
    ਖਾਲੀ	    Delay(ਲੰਮਾ);
#ਜੇ_ਅੰਤ

    ਜੇ (msec <= 0)
	ਵਾਪਸ;

    ਜੇ (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    ਨਹੀਂਤਾਂ
	WaitForChar(raw_ਵਿੱਚ, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    ਖਾਲੀ
mch_suspend(ਖਾਲੀ)
{
    suspend_shell();
}

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#ਜੇ_ਅੰਤ

    ਖਾਲੀ
mch_init(ਖਾਲੀ)
{
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ਸਥਿਰ ਅੱਖਰ	    intlibname[] = "intuition.library";
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#ਜੇ_ਅੰਤ

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	ਬਣਤਰ Process *me = (ਬਣਤਰ Process *)ਲੱਭੋ_Task(ਨਿਰਾ);
	me->pr_ਖਿੜਕੀ_Ptr = (APTR)-1L;
    }

    Columns = 80;
    ਪੰਕਤੀਆਂ = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ਜੇ (raw_ਵਿੱਚ == (BPTR)ਨਿਰਾ)
    {
	raw_ਵਿੱਚ = ਨਿਵੇਸ਼_3();
	raw_ਬਾਹਰਲਾ = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ਜੇ (!IsInteractive(raw_ਵਿੱਚ))
	    raw_ਵਿੱਚ = raw_ਬਾਹਰਲਾ;
	ਨਹੀਂਤਾਂ ਜੇ (!IsInteractive(raw_ਬਾਹਰਲਾ))
	    raw_ਬਾਹਰਲਾ = raw_ਵਿੱਚ;
    }

    ਬਾਹਰਲਾ_flush();

    wb_ਖਿੜਕੀ = ਨਿਰਾ;
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ਜੇ ((Intuition_ਅਧਾਰ = (ਬਣਤਰ Intuition_ਅਧਾਰ *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ਨਿਰਾ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#ਜੇ_ਅੰਤ
}

#ਅਨ੍ਤਰ੍ਭਾਵ <workbench/startup.h>

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
#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ BUF2_ਮਾਪ 320	    // length of buffer for argument with complete path

    ਪੂਰਨ
mch_check_win(ਪੂਰਨ argc, ਅੱਖਰ **argv)
{
    ਪੂਰਨ		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_ਮਾਪ];
    ਸਥਿਰ char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ਸਥਿਰ char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ਬਣਤਰ WBArg    *argp;
    ਪੂਰਨ		    ac;
    ਅੱਖਰ	    *av;
    char_u	    *device = ਨਿਰਾ;
    ਪੂਰਨ		    exitval = 4;
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ਬਣਤਰ Library  *Dos_ਅਧਾਰ;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_ਅਧਾਰ = OpenLibrary(DOS_LIBRARY, 37L);
    ਜੇ (Dos_ਅਧਾਰ != ਨਿਰਾ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_ਅਧਾਰ);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    ਨਹੀਂਤਾਂ	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	ਬਾਹਰ_ਨਿਕਲੋ(3);
# ਨਹੀਂਤਾਂ
		    // need arp functions for dos 1.x
	ਜੇ (!(Arp_ਅਧਾਰ = (ਬਣਤਰ Arp_ਅਧਾਰ *) OpenLibrary((UBYTE *)ArpName, Arp_ਸੰਸਕਰਣ)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_ਸੰਸਕਰਣ);
	    ਬਾਹਰ_ਨਿਕਲੋ(3);
	}
# endif
    }
#ਜੇ_ਅੰਤ	/* __amigaos4__ __AROS__ __MORPHOS__ */

    /*
     * scan argv[] for the "-f" and "-d" arguments
     */
    ਲਈ (i = 1; i < argc; ++i)
	ਜੇ (argv[i][0] == '-')
	{
	    ਚੋਣ (argv[i][1])
	    {
	    ਹਾਲਤ 'f':
		usewin = TRUE;
		ਰੋਕੋ;

	    ਹਾਲਤ 'd':
		ਜੇ (i < argc - 1
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#ਜੇ_ਅੰਤ
		   )
		    device = (char_u *)argv[i + 1];
		ਰੋਕੋ;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ਜੇ (argc != 0
	    && device == ਨਿਰਾ
	    && (IsInteractive(ਨਿਵੇਸ਼_3()) || IsInteractive(Output())))
	ਵਾਪਸ OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ਜੇ (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	ਲਈ (i = -1; i < 3; ++i)
	{
	    ਜੇ (i >= 0)
		device = constrings[i];
	    ਜੇ (device != ਨਿਰਾ && (raw_ਵਿੱਚ = Open((UBYTE *)device,
					   (ਲੰਮਾ)ਬਹੁਲਕ_NEWFILE)) != (BPTR)ਨਿਰਾ)
		ਰੋਕੋ;
	}
	ਜੇ (raw_ਵਿੱਚ == (BPTR)ਨਿਰਾ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
	}
	raw_ਬਾਹਰਲਾ = raw_ਵਿੱਚ;
	close_win = TRUE;
	ਵਾਪਸ OK;
    }

    ਜੇ ((nilfh = Open((UBYTE *)"NIL:", (ਲੰਮਾ)ਬਹੁਲਕ_NEWFILE)) == (BPTR)ਨਿਰਾ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ਜੇ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((ਅੱਖਰ *)buf1, "t:nc%p", ਲੱਭੋ_Task(0));
#ਨਹੀਂਤਾਂ
    sprintf((ਅੱਖਰ *)buf1, "t:nc%ld", (ਲੰਮਾ)buf1);
#ਜੇ_ਅੰਤ
    ਜੇ ((fh = Open((UBYTE *)buf1, (ਲੰਮਾ)ਬਹੁਲਕ_NEWFILE)) == (BPTR)ਨਿਰਾ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((ਅੱਖਰ *)buf1);
	mch_errmsg("\n");
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ਜੇ (argc == 0)	// run from workbench
	ac = ((ਬਣਤਰ WBStartup *)argv)->sm_ਅੰਕ_ਦਲੀਲਾਂ;
    ਨਹੀਂਤਾਂ
	ac = argc;
    ਲਈ (i = 0; i < ac; ++i)
    {
	ਜੇ (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ਬਣਤਰ WBStartup *)argv)->sm_ArgList[i]);
	    ਜੇ (argp->wa_ਤਾਲਾ)
		(ਖਾਲੀ)lock2name(argp->wa_ਤਾਲਾ, buf2, (ਲੰਮਾ)(BUF2_ਮਾਪ - 1));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	    ਜੇ (dos2)	    // use 2.0 function
#ਜੇ_ਅੰਤ
		ਜੋੜੋ_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (ਲੰਮਾ)(BUF2_ਮਾਪ - 1));
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	    ਨਹੀਂਤਾਂ	    // use arp function
		TackOn((ਅੱਖਰ *)buf2, argp->wa_Name);
#ਜੇ_ਅੰਤ
	    av = (ਅੱਖਰ *)buf2;
	}
	ਨਹੀਂਤਾਂ
	    av = argv[i];

	// skip '-d' or "-dev" option
	ਜੇ (av[0] == '-' && av[1] == 'd'
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#ਜੇ_ਅੰਤ
		)
	{
	    ++i;
	    ਜਾਰੀ;
	}
	ਜੇ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (ਲੰਮਾ)ਲੜੀ_ਲੰਬਾਈ(av));
	ਜੇ (vim_strchr((char_u *)av, ' '))
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
    ਲਈ (i = -1; i < 3; ++i)
    {
	ਜੇ (i >= 0)
	    device = constrings[i];
	ਨਹੀਂਤਾਂ ਜੇ (device == ਨਿਰਾ)
	    ਜਾਰੀ;
	sprintf((ਅੱਖਰ *)buf2, "newcli <nil: >nil: %s from %s", (ਅੱਖਰ *)device, (ਅੱਖਰ *)buf1);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	ਜੇ (dos2)
	{
#ਜੇ_ਅੰਤ
	    ਜੇ (!System_ਟੈਗ_ਸੂਚੀ((UBYTE *)buf2, SYS_UserShell, TRUE, ਟੈਗ_ਪੂਰਾ))
		ਰੋਕੋ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	}
	ਨਹੀਂਤਾਂ
	{
	    ਜੇ (Execute((UBYTE *)buf2, nilfh, nilfh))
		ਰੋਕੋ;
	}
#ਜੇ_ਅੰਤ
    }
    ਜੇ (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	ਜਾਓ ਬਾਹਰ_ਨਿਕਲੋ;
    }
    exitval = 0;    // The Execute succeeded: exit this program

ਬਾਹਰ_ਨਿਕਲੋ:
    ਜੇ (nilfh)
	Close(nilfh);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (Arp_ਅਧਾਰ)
	CloseLibrary((ਬਣਤਰ Library *) Arp_ਅਧਾਰ);
#ਜੇ_ਅੰਤ
    ਬਾਹਰ_ਨਿਕਲੋ(exitval);
    // NOTREACHED
    ਵਾਪਸ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    ਪੂਰਨ
mch_ਨਿਵੇਸ਼_isatty(ਖਾਲੀ)
{
    ਵਾਪਸ TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    ਖਾਲੀ
fname_case(
    char_u	*name,
    ਪੂਰਨ		len ਅਣਵਰਤਿਆ)		// buffer size, ignored here
{
    ਬਣਤਰ File_Info_ਖੰਡ    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ਜੇ (fib == ਨਿਰਾ)
	ਵਾਪਸ;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    ਜੇ (fib->fib_Dir_ਇੰਦਰਾਜ_Type == ST_ਜੜ੍ਹ)
	strcat(fib->fib_FileName, ":");
#ਜੇ_ਅੰਤ
    ਜੇ (flen == ਲੜੀ_ਲੰਬਾਈ(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ਸਥਿਰ ਬਣਤਰ File_Info_ਖੰਡ *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ਬਣਤਰ File_Info_ਖੰਡ    *fib;

    ਜੇ (fname == ਨਿਰਾ)	    // safety check
	ਵਾਪਸ ਨਿਰਾ;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#ਨਹੀਂਤਾਂ
    fib = ALLOC_ONE(ਬਣਤਰ File_Info_ਖੰਡ);
#ਜੇ_ਅੰਤ
    ਜੇ (fib == ਨਿਰਾ)
	ਵਾਪਸ;

    flock = ਸੁਰੱਖਿਅਤ_ਤਾਲਾ((UBYTE *)fname, (ਲੰਮਾ)ACCESS_READ);
    ਜੇ (flock == (BPTR)ਨਿਰਾ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ਨਿਰਾ;
    }
    ਜੇ (flock)
	Un_ਤਾਲਾ(flock);
    ਵਾਪਸ fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    ਖਾਲੀ
mch_settitle(char_u *title, char_u *icon)
{
    ਜੇ (wb_ਖਿੜਕੀ != ਨਿਰਾ && title != ਨਿਰਾ)
	ਸੈੱਟ_ਖਿੜਕੀ_Titles(wb_ਖਿੜਕੀ, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    ਖਾਲੀ
mch_restore_title(ਪੂਰਨ which)
{
    ਜੇ (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ਨਿਰਾ);
}

    ਪੂਰਨ
mch_can_restore_title(ਖਾਲੀ)
{
    ਵਾਪਸ (wb_ਖਿੜਕੀ != ਨਿਰਾ);
}

    ਪੂਰਨ
mch_can_restore_icon(ਖਾਲੀ)
{
    ਵਾਪਸ FALSE;
}

    ਖਾਲੀ
mch_setmouse(ਪੂਰਨ on ਅਣਵਰਤਿਆ)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    ਪੂਰਨ
mch_get_user_name(char_u *s, ਪੂਰਨ len)
{
#ਜੇ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ਬਣਤਰ passwd   *pwd = getpwuid(getuid());

    ਜੇ (pwd != ਨਿਰਾ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ਵਾਪਸ OK;
    }
#ਜੇ_ਅੰਤ
    *s = NUL;
    ਵਾਪਸ FAIL;
}

/*
 * Insert host name is s[len].
 */
    ਖਾਲੀ
mch_get_host_name(char_u *s, ਪੂਰਨ len)
{
#ਜੇ defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((ਅੱਖਰ *)s, len);
#ਨਹੀਂਤਾਂ
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#ਜੇ_ਅੰਤ
}

/*
 * return process ID
 */
    ਲੰਮਾ
mch_get_pid(ਖਾਲੀ)
{
#ਜੇ defined(__amigaos4__)
    ਵਾਪਸ (ਲੰਮਾ) getpid();
#ਜਾਂਜੇ defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ਵਾਪਸ((ਲੰਮਾ)ਲੱਭੋ_Task(0));
#ਨਹੀਂਤਾਂ
    ਵਾਪਸ (ਲੰਮਾ)0;
#ਜੇ_ਅੰਤ
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ਪੂਰਨ
mch_dirname(char_u *buf, ਪੂਰਨ len)
{
    ਵਾਪਸ mch_ਭਰਿਆ_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
mch_ਭਰਿਆ_Name(
    char_u	*fname,
    char_u	*buf,
    ਪੂਰਨ		len,
    ਪੂਰਨ		force)
{
    BPTR	l;
    ਪੂਰਨ		retval = FAIL;
    ਪੂਰਨ		i;

    // Lock the file.  If it exists, we can get the exact name.
    ਜੇ ((l = ਸੁਰੱਖਿਅਤ_ਤਾਲਾ((UBYTE *)fname, (ਲੰਮਾ)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (ਲੰਮਾ)len - 1);
	Un_ਤਾਲਾ(l);
    }
    ਨਹੀਂਤਾਂ ਜੇ (force || !mch_is_ਭਰਿਆ_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ਜੇ ((l = ਤਾਲਾ_2((UBYTE *)"", (ਲੰਮਾ)ACCESS_READ)) != (BPTR)ਨਿਰਾ)
	{
	    retval = lock2name(l, buf, (ਲੰਮਾ)len);
	    Un_ਤਾਲਾ(l);
	    ਜੇ (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ਜੇ (i == 0 || *fname != NUL)
		{
		    ਜੇ (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ਜੇ (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    ਵਾਪਸ retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ਪੂਰਨ
mch_is_ਭਰਿਆ_Name(char_u *fname)
{
    ਵਾਪਸ (vim_strchr(fname, ':') != ਨਿਰਾ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ਸਥਿਰ ਪੂਰਨ
lock2name(BPTR ਤਾਲਾ, char_u *buf, ਲੰਮਾ len)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (dos2)		    // use 2.0 function
#ਜੇ_ਅੰਤ
	ਵਾਪਸ ((ਪੂਰਨ)Name_From_ਤਾਲਾ(ਤਾਲਾ, (UBYTE *)buf, len) ? OK : FAIL);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਨਹੀਂਤਾਂ		// use arp function
	ਵਾਪਸ ((ਪੂਰਨ)ਰਸਤਾ_Name(ਤਾਲਾ, (ਅੱਖਰ *)buf, (ਲੰਮਾ)(len/32)) ? OK : FAIL);
#ਜੇ_ਅੰਤ
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    ਲੰਮਾ
mch_getperm(char_u *name)
{
    ਬਣਤਰ File_Info_ਖੰਡ    *fib;
    ਲੰਮਾ		    retval = -1;

    fib = get_fib(name);
    ਜੇ (fib == ਨਿਰਾ)
	ਵਾਪਸ -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ਵਾਪਸ retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
mch_setperm(char_u *name, ਲੰਮਾ perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ਵਾਪਸ (ਸੈੱਟ_Protection((UBYTE *)name, (ਲੰਮਾ)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    ਖਾਲੀ
mch_hide(char_u *name ਅਣਵਰਤਿਆ)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    ਪੂਰਨ
mch_isdir(char_u *name)
{
    ਬਣਤਰ File_Info_ਖੰਡ    *fib;
    ਪੂਰਨ			    retval = FALSE;

    fib = get_fib(name);
    ਜੇ (fib == ਨਿਰਾ)
	ਵਾਪਸ FALSE;

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#ਨਹੀਂਤਾਂ
    retval = ((fib->fib_Dir_ਇੰਦਰਾਜ_Type >= 0) ? TRUE : FALSE);
#ਜੇ_ਅੰਤ
    free_fib(fib);
    ਵਾਪਸ retval;
}

/*
 * Create directory "name".
 */
    ਪੂਰਨ
mch_mkdir(char_u *name)
{
    BPTR	ਤਾਲਾ;

    ਤਾਲਾ = CreateDir(name);
    ਜੇ (ਤਾਲਾ == ਨਿਰਾ)
	ਵਾਪਸ -1;

    Un_ਤਾਲਾ(ਤਾਲਾ);
    ਵਾਪਸ 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ਪੂਰਨ
mch_can_exe(char_u *name, char_u **ਰਸਤਾ ਅਣਵਰਤਿਆ, ਪੂਰਨ use_ਰਸਤਾ)
{
    ਪੂਰਨ exe = -1;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = ਲੱਦੋ_Seg(name);

    ਜੇ (seg && Get_Seg_List_Info_ਟੈਗ_ਸੂਚੀ(seg, GSLI_Native, ਨਿਰਾ, ਟੈਗ_ਪੂਰਾ) !=
	    Get_Seg_List_Info_ਟੈਗ_ਸੂਚੀ(seg, GSLI_68KHUNK, ਨਿਰਾ, ਟੈਗ_ਪੂਰਾ))
    {
	// Test if file permissions allow execution.
	ਬਣਤਰ Examine_ਅੰਕੜੇ *exd = Examine_Object_ਟੈਗ_ਸੂਚੀ(EX_String_Name_ਨਿਵੇਸ਼, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    ਨਹੀਂਤਾਂ
    {
	exe = 0;
    }

    Un_ਲੱਦੋ_Seg(seg);

    // Search for executable in path if applicable.
    ਜੇ (!exe && use_ਰਸਤਾ)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ਬਣਤਰ ਰਸਤਾ_ਗੰਢ *ਸਿਰ = Dup_Cmd_ਰਸਤਾ_List(ਨਿਰਾ);

	// For each entry, recur to check for executable.
	ਲਈ (ਬਣਤਰ ਰਸਤਾ_ਗੰਢ *ਪੂਛ = ਸਿਰ; !exe && ਪੂਛ;
			       ਪੂਛ = (ਬਣਤਰ ਰਸਤਾ_ਗੰਢ *) BADDR(ਪੂਛ->pn_ਅਗਲਾ))
	{
	    ਸੈੱਟ_Current_Dir(ਪੂਛ->pn_ਤਾਲਾ);
	    exe = mch_can_exe(name, ਰਸਤਾ, 0);
	}

	// Go back to where we were.
	Free_Cmd_ਰਸਤਾ_List(ਸਿਰ);
	ਸੈੱਟ_Current_Dir(cwd);
    }
#ਜੇ_ਅੰਤ
    ਵਾਪਸ exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ਪੂਰਨ
mch_nodetype(char_u *name ਅਣਵਰਤਿਆ)
{
    // TODO
    ਵਾਪਸ ਗੰਢ_ਸਾਧਾਰਨ;
}

    ਖਾਲੀ
mch_early_init(ਖਾਲੀ)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    ਖਾਲੀ
mch_exit(ਪੂਰਨ r)
{
    exiting = TRUE;

    ਜੇ (raw_ਵਿੱਚ)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    ਬਾਹਰਲਾ_char('\n');
    ਜੇ (raw_ਬਾਹਰਲਾ)
    {
	ਜੇ (ਪਦ_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ਜੇ (ਮਾਪ_ਸੈੱਟ)
		ਬਾਹਰਲਾ_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	ਬਾਹਰਲਾ_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (Arp_ਅਧਾਰ)
	CloseLibrary((ਬਣਤਰ Library *) Arp_ਅਧਾਰ);
#ਜੇ_ਅੰਤ
    ਜੇ (close_win)
	Close(raw_ਵਿੱਚ);
    ਜੇ (r)
	ਲਿਖੋ(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    ਬਾਹਰ_ਨਿਕਲੋ(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#ਪਰਿਭਾਸ਼ਿਤ_ਕਰੋ MP(xx)	((ਬਣਤਰ ਸੁਨੇਹਾ_ਪੋਰਟ *)((ਬਣਤਰ FileHandle *) (BADDR(xx)))->fh_Type)

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
    ਖਾਲੀ
mch_settmode(tmode_T tmode)
{
#ਜੇ defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ਜੇ (!ਸੈੱਟ_ਬਹੁਲਕ_2(raw_ਵਿੱਚ, tmode == TMODE_RAW ? 1 : 0))
#ਨਹੀਂਤਾਂ
    ਜੇ (dos_packet(MP(raw_ਵਿੱਚ), (ਲੰਮਾ)ACTION_SCREEN_ਬਹੁਲਕ,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#ਜੇ_ਅੰਤ
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

#ਅਨ੍ਤਰ੍ਭਾਵ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ਜੇ defined(__AROS__) || defined(__MORPHOS__)
    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    ਜੇ (!ਪਦ_console)
	ਵਾਪਸ FAIL;

    ਜੇ (raw_ਵਿੱਚ && raw_ਬਾਹਰਲਾ)
    {
	// Save current console mode.
	ਪੂਰਨ old_tmode = cur_tmode;
	ਅੱਖਰ ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ਜੇ (Write(raw_ਬਾਹਰਲਾ, ctrl, ਆਕਾਰ(ctrl)) == ਆਕਾਰ(ctrl))
	{
	    ਅੱਖਰ ਜਾਂਚ[] = "\x9b""1;1;%d;%d r",
		 answ[ਆਕਾਰ(ਜਾਂਚ) + 8] = { '\0' };

	    // Read return sequence from input.
	    ਜੇ (Read(raw_ਵਿੱਚ, answ, ਆਕਾਰ(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ਜੇ (ਲੜੀ_ਤੋਂ_ਪੜ੍ਹੋ(answ, ਜਾਂਚ, &ਪੰਕਤੀਆਂ, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ਵਾਪਸ OK;
		}
	    }
	}

	// Restore console mode.
	mch_settmode(old_tmode);
    }

    // I/O error. Default size fallback.
    ਪਦ_console = FALSE;
    Columns = 80;
    ਪੰਕਤੀਆਂ = 24;

    ਵਾਪਸ FAIL;
}
#ਨਹੀਂਤਾਂ
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    ਪੂਰਨ
mch_get_shellsize(ਖਾਲੀ)
{
    ਬਣਤਰ ConUnit  *conUnit;
# ifndef __amigaos4__
    ਅੱਖਰ	    id_a[ਆਕਾਰ(ਬਣਤਰ Info_ਅੰਕੜੇ) + 3];
# endif
    ਬਣਤਰ Info_ਅੰਕੜੇ *id=0;

    ਜੇ (!ਪਦ_console)	// not an amiga window
	ਜਾਓ ਬਾਹਰਲਾ;

    // insure longword alignment
# ifdef __amigaos4__
    ਜੇ (!(id = AllocDosObject(DOS_INFODATA, 0)))
	ਜਾਓ ਬਾਹਰਲਾ;
# ਨਹੀਂਤਾਂ
    id = (ਬਣਤਰ Info_ਅੰਕੜੇ *)(((ਲੰਮਾ)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ਜੇ (ਮਾਪ_ਸੈੱਟ)
	ਬਾਹਰਲਾ_STR("\233t\233u");	// CSI t CSI u
    ਬਾਹਰਲਾ_flush();

    ਜੇ (dos_packet(MP(raw_ਬਾਹਰਲਾ), (ਲੰਮਾ)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_ਖਿੜਕੀ = (ਬਣਤਰ ਖਿੜਕੀ *)id->id_Volume_ਗੰਢ) == ਨਿਰਾ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	ਪਦ_console = FALSE;
	ਜਾਓ ਬਾਹਰਲਾ;
    }
    ਜੇ (oldwindowtitle == ਨਿਰਾ)
	oldwindowtitle = (char_u *)wb_ਖਿੜਕੀ->Title;
    ਜੇ (id->id_ਵਿੱਚ_Use == (BPTR)ਨਿਰਾ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ਵਾਪਸ FAIL;
    }
    conUnit = (ਬਣਤਰ ConUnit *) ((ਬਣਤਰ IOStdReq *) id->id_ਵਿੱਚ_Use)->io_Unit;

    // get window size
    ਪੰਕਤੀਆਂ = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ਜੇ (ਪੰਕਤੀਆਂ < 0 || ਪੰਕਤੀਆਂ > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	ਪੰਕਤੀਆਂ = 24;
	ਪਦ_console = FALSE;
	ਵਾਪਸ FAIL;
    }

    ਵਾਪਸ OK;
ਬਾਹਰਲਾ:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ਵਾਪਸ FAIL;
}
#ਜੇ_ਅੰਤ

/*
 * Try to set the real window size to Rows and Columns.
 */
    ਖਾਲੀ
mch_ਸੈੱਟ_shellsize(ਖਾਲੀ)
{
    ਜੇ (!ਪਦ_console)
	ਵਾਪਸ;

    ਮਾਪ_ਸੈੱਟ = TRUE;
    ਬਾਹਰਲਾ_char(CSI);
    ਬਾਹਰਲਾ_ਅੰਕ((ਲੰਮਾ)ਪੰਕਤੀਆਂ);
    ਬਾਹਰਲਾ_char('t');
    ਬਾਹਰਲਾ_char(CSI);
    ਬਾਹਰਲਾ_ਅੰਕ((ਲੰਮਾ)Columns);
    ਬਾਹਰਲਾ_char('u');
    ਬਾਹਰਲਾ_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    ਖਾਲੀ
mch_new_shellsize(ਖਾਲੀ)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ਸਥਿਰ ਖਾਲੀ
ਬਾਹਰਲਾ_ਅੰਕ(ਲੰਮਾ n)
{
    ਬਾਹਰਲਾ_STR_NF(tltoa((ਬਿਨਾਨਿਸ਼ਾਨੀ ਲੰਮਾ)n));
}

#ਜੇ !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    ਸਥਿਰ ਲੰਮਾ
dos_packet(
    ਬਣਤਰ ਸੁਨੇਹਾ_ਪੋਰਟ *pid,    // process identifier ... (handlers message port)
    ਲੰਮਾ	    action, // packet type ... (what you want handler to do)
    ਲੰਮਾ	    arg)    // single argument
{
# ifdef FEAT_ARP
    ਬਣਤਰ ਸੁਨੇਹਾ_ਪੋਰਟ	    *replyport;
    ਬਣਤਰ StandardPacket   *packet;
    ਲੰਮਾ		    res1;

    ਜੇ (dos2)
# endif
	ਵਾਪਸ DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ਬਣਤਰ ਸੁਨੇਹਾ_ਪੋਰਟ *) Create_ਪੋਰਟ(ਨਿਰਾ, 0);	// use arp function
    ਜੇ (!replyport)
	ਵਾਪਸ (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ਬਣਤਰ StandardPacket *)
	AllocMem((ਲੰਮਾ) ਆਕਾਰ(ਬਣਤਰ StandardPacket), MEMF_ਜਨਤਕ | MEMF_CLEAR);
    ਜੇ (!packet)
    {
	Delete_ਪੋਰਟ(replyport);
	ਵਾਪਸ (0);
    }
    packet->sp_ਸੁਨੇਹਾ.mn_ਗੰਢ.ln_Name = (ਅੱਖਰ *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_ਸੁਨੇਹਾ);
    packet->sp_Pkt.dp_ਪੋਰਟ = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_ਸੁਨੇਹਾ(pid, (ਬਣਤਰ ਸੁਨੇਹਾ_4 *)packet);	// send packet

    Wait_ਪੋਰਟ(replyport);
    Get_ਸੁਨੇਹਾ(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (ਲੰਮਾ) ਆਕਾਰ(ਬਣਤਰ StandardPacket));
    Delete_ਪੋਰਟ(replyport);

    ਵਾਪਸ (res1);
# endif
}
#ਜੇ_ਅੰਤ // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    ਪੂਰਨ
mch_call_shell(
    char_u	*cmd,
    ਪੂਰਨ		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    ਪੂਰਨ		x;
    ਪੂਰਨ		tmode = cur_tmode;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ AZTEC_C
    ਪੂਰਨ		use_execute;
    char_u	*shellcmd = ਨਿਰਾ;
    char_u	*shellarg;
#ਜੇ_ਅੰਤ
    ਪੂਰਨ		retval = 0;

    ਜੇ (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ਵਾਪਸ -1;
    }

    ਜੇ (ਪਦ_console)
	win_resize_off();	    // window resize events de-activated
    ਬਾਹਰਲਾ_flush();

    ਜੇ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = ਤਾਲਾ_2((UBYTE *)"", (ਲੰਮਾ)ACCESS_READ);   // remember current dir

#ਜੇ !defined(AZTEC_C)		    // not tested very much
    ਜੇ (cmd == ਨਿਰਾ)
    {
# ifdef FEAT_ARP
	ਜੇ (dos2)
# endif
	    x = System_ਟੈਗ_ਸੂਚੀ(p_sh, SYS_UserShell, TRUE, ਟੈਗ_ਪੂਰਾ);
# ifdef FEAT_ARP
	ਨਹੀਂਤਾਂ
	    x = Execute(p_sh, raw_ਵਿੱਚ, raw_ਬਾਹਰਲਾ);
# endif
    }
    ਨਹੀਂਤਾਂ
    {
# ifdef FEAT_ARP
	ਜੇ (dos2)
# endif
	    x = System_ਟੈਗ_ਸੂਚੀ((ਅੱਖਰ *)cmd, SYS_UserShell, TRUE, ਟੈਗ_ਪੂਰਾ);
# ifdef FEAT_ARP
	ਨਹੀਂਤਾਂ
	    x = Execute((ਅੱਖਰ *)cmd, 0L, raw_ਬਾਹਰਲਾ);
# endif
    }
# ifdef FEAT_ARP
    ਜੇ ((dos2 && x < 0) || (!dos2 && !x))
# ਨਹੀਂਤਾਂ
    ਜੇ (x < 0)
# endif
    {
	ਸੁਨੇਹਾ_puts(_("Cannot execute "));
	ਜੇ (cmd == ਨਿਰਾ)
	{
	    ਸੁਨੇਹਾ_puts(_("shell "));
	    ਸੁਨੇਹਾ_outtrans(p_sh);
	}
	ਨਹੀਂਤਾਂ
	    ਸੁਨੇਹਾ_outtrans(cmd);
	ਸੁਨੇਹਾ_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    ਨਹੀਂਤਾਂ ਜੇ (!dos2 || x)
# ਨਹੀਂਤਾਂ
    ਨਹੀਂਤਾਂ ਜੇ (x)
# endif
    {
	ਜੇ ((x = IoErr()) != 0)
	{
	    ਜੇ (!(options & SHELL_SILENT))
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)x);
		ਸੁਨੇਹਾ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#ਨਹੀਂਤਾਂ	// else part is for AZTEC_C
    ਜੇ (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    ਨਹੀਂਤਾਂ
	use_execute = 0;
    ਜੇ (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ਜੇ (shellcmd == ਨਿਰਾ)	    // out of memory, use Execute
	    use_execute = 1;
	ਨਹੀਂਤਾਂ
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ਜੇ (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ਜੇ (cmd == ਨਿਰਾ)
    {
	ਜੇ (use_execute)
	{
# ifdef FEAT_ARP
	    ਜੇ (dos2)
# endif
		x = System_ਟੈਗ_ਸੂਚੀ((UBYTE *)p_sh, SYS_UserShell, TRUE, ਟੈਗ_ਪੂਰਾ);
# ifdef FEAT_ARP
	    ਨਹੀਂਤਾਂ
		x = !Execute((UBYTE *)p_sh, raw_ਵਿੱਚ, raw_ਬਾਹਰਲਾ);
# endif
	}
	ਨਹੀਂਤਾਂ
	    x = fexecl((ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellarg, ਨਿਰਾ);
    }
    ਨਹੀਂਤਾਂ ਜੇ (use_execute)
    {
# ifdef FEAT_ARP
	ਜੇ (dos2)
# endif
	    x = System_ਟੈਗ_ਸੂਚੀ((UBYTE *)cmd, SYS_UserShell, TRUE, ਟੈਗ_ਪੂਰਾ);
# ifdef FEAT_ARP
	ਨਹੀਂਤਾਂ
	    x = !Execute((UBYTE *)cmd, 0L, raw_ਬਾਹਰਲਾ);
# endif
    }
    ਨਹੀਂਤਾਂ ਜੇ (p_st & 1)
	x = fexecl((ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellarg,
							   (ਅੱਖਰ *)cmd, ਨਿਰਾ);
    ਨਹੀਂਤਾਂ
	x = fexecl((ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellcmd, (ਅੱਖਰ *)shellarg,
					   (ਅੱਖਰ *)p_shcf, (ਅੱਖਰ *)cmd, ਨਿਰਾ);
# ifdef FEAT_ARP
    ਜੇ ((dos2 && x < 0) || (!dos2 && x))
# ਨਹੀਂਤਾਂ
    ਜੇ (x < 0)
# endif
    {
	ਸੁਨੇਹਾ_puts(_("Cannot execute "));
	ਜੇ (use_execute)
	{
	    ਜੇ (cmd == ਨਿਰਾ)
		ਸੁਨੇਹਾ_outtrans(p_sh);
	    ਨਹੀਂਤਾਂ
		ਸੁਨੇਹਾ_outtrans(cmd);
	}
	ਨਹੀਂਤਾਂ
	{
	    ਸੁਨੇਹਾ_puts(_("shell "));
	    ਸੁਨੇਹਾ_outtrans(shellcmd);
	}
	ਸੁਨੇਹਾ_putchar('\n');
	retval = -1;
    }
    ਨਹੀਂਤਾਂ
    {
	ਜੇ (use_execute)
	{
# ifdef FEAT_ARP
	    ਜੇ (!dos2 || x)
# ਨਹੀਂਤਾਂ
	    ਜੇ (x)
# endif
		x = IoErr();
	}
	ਨਹੀਂਤਾਂ
	    x = wait();
	ਜੇ (x)
	{
	    ਜੇ (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		ਸੁਨੇਹਾ_putchar('\n');
		ਸੁਨੇਹਾ_outnum((ਲੰਮਾ)x);
		ਸੁਨੇਹਾ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#ਜੇ_ਅੰਤ	// AZTEC_C

    ਜੇ ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_ਤਾਲਾ(mydir);
    ਜੇ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ਜੇ (ਪਦ_console)
	win_resize_on();		// window resize events activated
    ਵਾਪਸ retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    ਖਾਲੀ
mch_breakcheck(ਪੂਰਨ force ਅਣਵਰਤਿਆ)
{
   ਜੇ (ਸੈੱਟ_Signal(0L, (ਲੰਮਾ)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ _DCC
# define Chk_Abort chkabort
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ LATTICE
ਖਾਲੀ __regargs __chkabort(ਖਾਲੀ);

ਖਾਲੀ __regargs __chkabort(ਖਾਲੀ)
{}

#ਨਹੀਂਤਾਂ
    ਲੰਮਾ
Chk_Abort(ਖਾਲੀ)
{
    ਵਾਪਸ(0L);
}
#ਜੇ_ਅੰਤ

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

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
# define	ANCHOR_BUF_ਮਾਪ	1024
#ਨਹੀਂਤਾਂ
# define ANCHOR_BUF_ਮਾਪ (512)
# define ANCHOR_ਮਾਪ (ਆਕਾਰ(ਬਣਤਰ Anchor_ਰਸਤਾ) + ANCHOR_BUF_ਮਾਪ)
#ਜੇ_ਅੰਤ

    ਪੂਰਨ
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    ਪੂਰਨ		flags)		// EW_* flags
{
    ਬਣਤਰ Anchor_ਰਸਤਾ	*Anchor;
    LONG		ਨਤੀਜਾ_2;
    char_u		*starbuf, *sp, *dp;
    ਪੂਰਨ			ਸ਼ੁਰੂ_len;
    ਪੂਰਨ			matches;
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    ਬਣਤਰ ਟੈਗ_ਵਸਤੂ	Anchor_ਟੈਗ_ਸੂਚੀ[] = {
	{ADO_Strlen, ANCHOR_BUF_ਮਾਪ},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{ਟੈਗ_ਪੂਰਾ, 0L}
    };
#ਜੇ_ਅੰਤ

    ਸ਼ੁਰੂ_len = gap->ga_len;

    // Get our AnchorBase
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_ਟੈਗ_ਸੂਚੀ);
#ਨਹੀਂਤਾਂ
    Anchor = alloc_clear(ANCHOR_ਮਾਪ);
#ਜੇ_ਅੰਤ
    ਜੇ (Anchor == ਨਿਰਾ)
	ਵਾਪਸ 0;

#ਜੇ_ਅਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_ਮਾਪ;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# ਨਹੀਂਤਾਂ
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#ਜੇ_ਅੰਤ

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (dos2)
    {
#ਜੇ_ਅੰਤ
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ਜੇ (starbuf == ਨਿਰਾ)
	    ਜਾਓ Return;
	ਲਈ (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ਜੇ (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    ਨਹੀਂਤਾਂ
		*dp++ = *sp;
	}
	*dp = NUL;
	ਨਤੀਜਾ_2 = ਮੇਲ_ਪਹਿਲਾ((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    }
    ਨਹੀਂਤਾਂ
	ਨਤੀਜਾ_2 = ਲੱਭੋ_ਪਹਿਲਾ((ਅੱਖਰ *)pat, Anchor);
#ਜੇ_ਅੰਤ

    /*
     * Loop to get all matches.
     */
    ਜਦੋਂ (ਨਤੀਜਾ_2 == 0)
    {
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_ਬਫਰ, flags);
#ਨਹੀਂਤਾਂ
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#ਜੇ_ਅੰਤ
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	ਜੇ (dos2)
#ਜੇ_ਅੰਤ
	    ਨਤੀਜਾ_2 = ਮੇਲ_ਅਗਲਾ_2(Anchor);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
	ਨਹੀਂਤਾਂ
	    ਨਤੀਜਾ_2 = ਲੱਭੋ_ਅਗਲਾ(Anchor);
#ਜੇ_ਅੰਤ
    }
    matches = gap->ga_len - ਸ਼ੁਰੂ_len;

    ਜੇ (ਨਤੀਜਾ_2 == ERROR_ਬਫਰ_ਉਛਾਲ)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    ਨਹੀਂਤਾਂ ਜੇ (matches == 0 && ਨਤੀਜਾ_2 != ERROR_OBJECT_NOT_ਮਿਲਿਆ
			  && ਨਤੀਜਾ_2 != ERROR_DEVICE_NOT_MOUNTED
			  && ਨਤੀਜਾ_2 != ERROR_NO_ਹੋਰ_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ਜੇ (matches)
	qsort((ਖਾਲੀ *)(((char_u **)gap->ga_ਅੰਕੜੇ) + ਸ਼ੁਰੂ_len),
				  (size_t)matches, ਆਕਾਰ(char_u *), sortcmp);

    // Free the wildcard stuff
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (dos2)
#ਜੇ_ਅੰਤ
	ਮੇਲ_ਸਿਰਾ_3(Anchor);
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਨਹੀਂਤਾਂ
	FreeAnchorChain(Anchor);
#ਜੇ_ਅੰਤ

Return:
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#ਨਹੀਂਤਾਂ
    vim_free(Anchor);
#ਜੇ_ਅੰਤ

    ਵਾਪਸ matches;
}

    ਸਥਿਰ ਪੂਰਨ
sortcmp(ਅਚਲ ਖਾਲੀ *a, ਅਚਲ ਖਾਲੀ *b)
{
    ਅੱਖਰ *s = *(ਅੱਖਰ **)a;
    ਅੱਖਰ *t = *(ਅੱਖਰ **)b;

    ਵਾਪਸ pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    ਪੂਰਨ
mch_has_exp_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ ਜੇ (vim_strchr((char_u *)"*?[(#", *p) != ਨਿਰਾ)
	    ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
}

    ਪੂਰਨ
mch_has_wildcard(char_u *p)
{
    ਲਈ ( ; *p; MB_PTR_ADV(p))
    {
	ਜੇ (*p == '\\' && p[1] != NUL)
	    ++p;
	ਨਹੀਂਤਾਂ
	    ਜੇ (vim_strchr((char_u *)"*?[(#$`", *p) != ਨਿਰਾ
		    || (*p == '~' && p[1] != NUL))
		ਵਾਪਸ TRUE;
    }
    ਵਾਪਸ FALSE;
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
    ਪੂਰਨ		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ਸਥਿਰ char_u   *alloced = ਨਿਰਾ;	// allocated memory

#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (!dos2)
	retval = (char_u *)getenv((ਅੱਖਰ *)var);
    ਨਹੀਂਤਾਂ
#ਜੇ_ਅੰਤ
    {
	VIM_CLEAR(alloced);
	retval = ਨਿਰਾ;

	buf = alloc(IOSIZE);
	ਜੇ (buf == ਨਿਰਾ)
	    ਵਾਪਸ ਨਿਰਾ;

	len = GetVar((UBYTE *)var, buf, (ਲੰਮਾ)(IOSIZE - 1), (ਲੰਮਾ)0);
	ਜੇ (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ਜੇ (retval == ਨਿਰਾ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ਵਾਪਸ retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    ਪੂਰਨ
mch_setenv(ਅੱਖਰ *var, ਅੱਖਰ *ਮੁੱਲ, ਪੂਰਨ x ਅਣਵਰਤਿਆ)
{
#ਜੇ_ਪਰਿਭਾਸ਼ਿਤ FEAT_ARP
    ਜੇ (!dos2)
	ਵਾਪਸ setenv(var, ਮੁੱਲ);
#ਜੇ_ਅੰਤ

    ਜੇ (ਸੈੱਟ_Var((UBYTE *)var, (UBYTE *)ਮੁੱਲ, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ਵਾਪਸ 0;   // success
    ਵਾਪਸ -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    ਪੂਰਨ
mch_get_random(char_u *buf, ਪੂਰਨ len)
{
    ਬਣਤਰ Process *proc = (ਬਣਤਰ Process *) ਲੱਭੋ_Task(0L);
    APTR win = proc->pr_ਖਿੜਕੀ_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_ਖਿੜਕੀ_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", ਬਹੁਲਕ_OLDFILE);

    proc->pr_ਖਿੜਕੀ_Ptr = win;

    ਪੂਰਨ ਸਥਿਤੀ;

    ਜੇ (!fh || Read(fh, buf, len) != len)
	ਸਥਿਤੀ = FAIL;
    ਨਹੀਂਤਾਂ
	ਸਥਿਤੀ = OK;

    Close(fh);
    ਵਾਪਸ ਸਥਿਤੀ;
}
