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

#అన్తర్భావ "vim.h"
#అన్తర్భావ "version.h"

#నిర్వచితమైతే కిటికీ
# undef కిటికీ	// Amiga has its own Window definition
#అయితే_ముగింపు

#నిర్వచనం_రద్దు TRUE		// will be redefined by exec/types.h
#నిర్వచనం_రద్దు FALSE

#నిర్వచితం_కాకపోతే LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#అయితే_ముగింపు

// XXX These are included from os_amiga.h
// #include <exec/types.h>
// #include <libraries/dos.h>
// #include <libraries/dosextens.h>
// #include <proto/exec.h>
// #include <proto/dos.h>
// #include <proto/intuition.h>

#అన్తర్భావ <exec/memory.h>

#అన్తర్భావ <dos/dostags.h>	    // for 2.0 functions
#అన్తర్భావ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#నిర్వచితమైతే __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#లేకపోతే
# define	free_fib(x) vim_free(fib)
#అయితే_ముగింపు

#ఒకవేళ defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#అయితే_ముగింపు

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ఒకవేళ defined(__amigaos4__)
స్థిర స్థిరరాశి అక్షరం* __attribute__((వాడిన)) stackcookie = "$STACK: 1048576";
#లేదావేళ defined(__AROS__) || defined(__MORPHOS__)
గుర్తులేని దీర్ఘ __stack = 1048576;
#లేదావేళ defined(__GNUC__) && defined(AMIGA)
గుర్తులేని దీర్ఘ __stack = 262144;
#అయితే_ముగింపు

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#నిర్వచనం_రద్దు	TRUE
#నిర్వచించు TRUE (1)
#నిర్వచనం_రద్దు	FALSE
#నిర్వచించు FALSE (0)

#నిర్వచితమైతే __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#లేదావేళ !defined(AZTEC_C) && !defined(__AROS__)
స్థిర దీర్ఘ dos_packet(నిర్మాణం సందేశం_పోర్టు *, దీర్ఘ, దీర్ఘ);
#అయితే_ముగింపు
స్థిర పూర్ణాంకం lock2name(BPTR తాళం, char_u *buf, దీర్ఘ   len);
స్థిర శూన్య బహిర్గతం_అంకె(దీర్ఘ n);
స్థిర నిర్మాణం File_Info_ఖండం *get_fib(char_u *);
స్థిర పూర్ణాంకం sortcmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    స్థిర BPTR
సురక్షిత_తాళం(UBYTE *name, దీర్ఘ బహుళకం)
{
    నిర్మాణం Process *me = (నిర్మాణం Process *)కనుగొను_Task(ఖాళీ);
    APTR oldwin = me->pr_కిటికీ_Ptr;
    BPTR flock;

    me->pr_కిటికీ_Ptr = (APTR)-1L;
    flock = తాళం_2(name, బహుళకం);
    me->pr_కిటికీ_Ptr = oldwin;
    తిరిగి flock;
}

స్థిర BPTR		raw_లోపల = (BPTR)ఖాళీ;
స్థిర BPTR		raw_బహిర్గతం = (BPTR)ఖాళీ;
స్థిర పూర్ణాంకం		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
నిర్మాణం Intuition_పునాది	*Intuition_పునాది = ఖాళీ;
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_ARP
నిర్మాణం Arp_పునాది		*Arp_పునాది = ఖాళీ;
#అయితే_ముగింపు

స్థిర నిర్మాణం కిటికీ	*wb_కిటికీ;
స్థిర char_u		*oldwindowtitle = ఖాళీ;

#నిర్వచితమైతే FEAT_ARP
పూర్ణాంకం			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#అయితే_ముగింపు
పూర్ణాంకం			కొలత_సమితి = FALSE;   // set to TRUE if window size was set

#నిర్వచితమైతే __GNUC__
స్థిర అక్షరం సంస్కరణ_2[] __attribute__((వాడిన)) =
    "\0$VER: Vim "
    VIM_సంస్కరణ_MAJOR_STR "."
    VIM_సంస్కరణ_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#అయితే_ముగింపు

    శూన్య
win_resize_on(శూన్య)
{
    బహిర్గతం_STR_NF("\033[12{");
}

    శూన్య
win_resize_off(శూన్య)
{
    బహిర్గతం_STR_NF("\033[12}");
}

    శూన్య
mch_write(char_u *p, పూర్ణాంకం len)
{
    Write(raw_బహిర్గతం, (అక్షరం *)p, (దీర్ఘ)len);
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
    పూర్ణాంకం
mch_inchar(
    char_u  *buf,
    పూర్ణాంకం	    maxlen,
    దీర్ఘ    సమయం,		// milliseconds
    పూర్ణాంకం	    tb_change_cnt వాడని)
{
    పూర్ణాంకం	    len;
    దీర్ఘ    utime;

    ఒకవేళ (సమయం >= 0)
    {
	ఒకవేళ (సమయం == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	లేకపోతే
	    utime = సమయం * 1000L;   // convert from milli to micro secs
	ఒకవేళ (WaitForChar(raw_లోపల, utime) == 0)	// no character available
	    తిరిగి 0;
    }
    లేకపోతే    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ఒకవేళ (WaitForChar(raw_లోపల, p_ut * 1000L) == 0)
	{
	    ఒకవేళ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (పూర్ణాంకం)KE_CURSORHOLD;
		తిరిగి 3;
	    }
	    before_blocking();
	}
    }

    కొరకు (;;)	    // repeat until we got a character
    {
	len = Read(raw_లోపల, (అక్షరం *)buf, (దీర్ఘ)maxlen / ఆదానం_conv.vc_గుణకం);
	ఒకవేళ (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ఒకవేళ (ఆదానం_conv.vc_type != CONV_NONE)
		len = convert_ఆదానం(buf, len, maxlen);
	    తిరిగి len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    పూర్ణాంకం
mch_char_avail(శూన్య)
{
    తిరిగి (WaitForChar(raw_లోపల, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(పూర్ణాంకం special)
{
#ఒకవేళ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    తిరిగి (long_u)AvailMem(MEMF_ANY) >> 10;
#లేకపోతే
    తిరిగి (long_u)(AvailMem(special ? (దీర్ఘ)MEMF_CHIP : (దీర్ఘ)MEMF_ANY)) >> 10;
#అయితే_ముగింపు
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    శూన్య
mch_delay(దీర్ఘ msec, పూర్ణాంకం flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#నిర్వచితమైతే LATTICE
    శూన్య	    Delay(దీర్ఘ);
#అయితే_ముగింపు

    ఒకవేళ (msec <= 0)
	తిరిగి;

    ఒకవేళ (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    లేకపోతే
	WaitForChar(raw_లోపల, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    శూన్య
mch_suspend(శూన్య)
{
    suspend_shell();
}

#నిర్వచితం_కాకపోతే DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#అయితే_ముగింపు

    శూన్య
mch_init(శూన్య)
{
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    స్థిర అక్షరం	    intlibname[] = "intuition.library";
#అయితే_ముగింపు

#నిర్వచితమైతే AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#అయితే_ముగింపు

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	నిర్మాణం Process *me = (నిర్మాణం Process *)కనుగొను_Task(ఖాళీ);
	me->pr_కిటికీ_Ptr = (APTR)-1L;
    }

    Columns = 80;
    అడ్డువరుసలు = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ఒకవేళ (raw_లోపల == (BPTR)ఖాళీ)
    {
	raw_లోపల = ఆదానం_3();
	raw_బహిర్గతం = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ఒకవేళ (!IsInteractive(raw_లోపల))
	    raw_లోపల = raw_బహిర్గతం;
	లేకపోతే ఒకవేళ (!IsInteractive(raw_బహిర్గతం))
	    raw_బహిర్గతం = raw_లోపల;
    }

    బహిర్గతం_flush();

    wb_కిటికీ = ఖాళీ;
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ఒకవేళ ((Intuition_పునాది = (నిర్మాణం Intuition_పునాది *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ఖాళీ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#అయితే_ముగింపు
}

#అన్తర్భావ <workbench/startup.h>

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
#నిర్వచించు BUF2_కొలత 320	    // length of buffer for argument with complete path

    పూర్ణాంకం
mch_check_win(పూర్ణాంకం argc, అక్షరం **argv)
{
    పూర్ణాంకం		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_కొలత];
    స్థిర char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    స్థిర char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    నిర్మాణం WBArg    *argp;
    పూర్ణాంకం		    ac;
    అక్షరం	    *av;
    char_u	    *device = ఖాళీ;
    పూర్ణాంకం		    exitval = 4;
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    నిర్మాణం Library  *Dos_పునాది;
#అయితే_ముగింపు
    పూర్ణాంకం		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_పునాది = OpenLibrary(DOS_LIBRARY, 37L);
    ఒకవేళ (Dos_పునాది != ఖాళీ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_పునాది);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    లేకపోతే	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	నిష్క్రమించు(3);
# లేకపోతే
		    // need arp functions for dos 1.x
	ఒకవేళ (!(Arp_పునాది = (నిర్మాణం Arp_పునాది *) OpenLibrary((UBYTE *)ArpName, Arp_సంస్కరణ)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_సంస్కరణ);
	    నిష్క్రమించు(3);
	}
# endif
    }
#అయితే_ముగింపు	/* __amigaos4__ __AROS__ __MORPHOS__ */

    /*
     * scan argv[] for the "-f" and "-d" arguments
     */
    కొరకు (i = 1; i < argc; ++i)
	ఒకవేళ (argv[i][0] == '-')
	{
	    ఎంపిక (argv[i][1])
	    {
	    స్థితి 'f':
		usewin = TRUE;
		ఆపు;

	    స్థితి 'd':
		ఒకవేళ (i < argc - 1
#నిర్వచితమైతే FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#అయితే_ముగింపు
		   )
		    device = (char_u *)argv[i + 1];
		ఆపు;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ఒకవేళ (argc != 0
	    && device == ఖాళీ
	    && (IsInteractive(ఆదానం_3()) || IsInteractive(Output())))
	తిరిగి OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ఒకవేళ (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	కొరకు (i = -1; i < 3; ++i)
	{
	    ఒకవేళ (i >= 0)
		device = constrings[i];
	    ఒకవేళ (device != ఖాళీ && (raw_లోపల = Open((UBYTE *)device,
					   (దీర్ఘ)బహుళకం_NEWFILE)) != (BPTR)ఖాళీ)
		ఆపు;
	}
	ఒకవేళ (raw_లోపల == (BPTR)ఖాళీ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    వెళ్ళు నిష్క్రమించు;
	}
	raw_బహిర్గతం = raw_లోపల;
	close_win = TRUE;
	తిరిగి OK;
    }

    ఒకవేళ ((nilfh = Open((UBYTE *)"NIL:", (దీర్ఘ)బహుళకం_NEWFILE)) == (BPTR)ఖాళీ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	వెళ్ళు నిష్క్రమించు;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ఒకవేళ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((అక్షరం *)buf1, "t:nc%p", కనుగొను_Task(0));
#లేకపోతే
    sprintf((అక్షరం *)buf1, "t:nc%ld", (దీర్ఘ)buf1);
#అయితే_ముగింపు
    ఒకవేళ ((fh = Open((UBYTE *)buf1, (దీర్ఘ)బహుళకం_NEWFILE)) == (BPTR)ఖాళీ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((అక్షరం *)buf1);
	mch_errmsg("\n");
	వెళ్ళు నిష్క్రమించు;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ఒకవేళ (argc == 0)	// run from workbench
	ac = ((నిర్మాణం WBStartup *)argv)->sm_అంకె_వాదనలు;
    లేకపోతే
	ac = argc;
    కొరకు (i = 0; i < ac; ++i)
    {
	ఒకవేళ (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((నిర్మాణం WBStartup *)argv)->sm_ArgList[i]);
	    ఒకవేళ (argp->wa_తాళం)
		(శూన్య)lock2name(argp->wa_తాళం, buf2, (దీర్ఘ)(BUF2_కొలత - 1));
#నిర్వచితమైతే FEAT_ARP
	    ఒకవేళ (dos2)	    // use 2.0 function
#అయితే_ముగింపు
		కూడు_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (దీర్ఘ)(BUF2_కొలత - 1));
#నిర్వచితమైతే FEAT_ARP
	    లేకపోతే	    // use arp function
		TackOn((అక్షరం *)buf2, argp->wa_Name);
#అయితే_ముగింపు
	    av = (అక్షరం *)buf2;
	}
	లేకపోతే
	    av = argv[i];

	// skip '-d' or "-dev" option
	ఒకవేళ (av[0] == '-' && av[1] == 'd'
#నిర్వచితమైతే FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#అయితే_ముగింపు
		)
	{
	    ++i;
	    కొనసాగు;
	}
	ఒకవేళ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (దీర్ఘ)అక్షరశ్రేణి_పొడవు(av));
	ఒకవేళ (vim_strchr((char_u *)av, ' '))
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
    కొరకు (i = -1; i < 3; ++i)
    {
	ఒకవేళ (i >= 0)
	    device = constrings[i];
	లేకపోతే ఒకవేళ (device == ఖాళీ)
	    కొనసాగు;
	sprintf((అక్షరం *)buf2, "newcli <nil: >nil: %s from %s", (అక్షరం *)device, (అక్షరం *)buf1);
#నిర్వచితమైతే FEAT_ARP
	ఒకవేళ (dos2)
	{
#అయితే_ముగింపు
	    ఒకవేళ (!System_ట్యాగులు((UBYTE *)buf2, SYS_UserShell, TRUE, ట్యాగ్_పూర్తి))
		ఆపు;
#నిర్వచితమైతే FEAT_ARP
	}
	లేకపోతే
	{
	    ఒకవేళ (Execute((UBYTE *)buf2, nilfh, nilfh))
		ఆపు;
	}
#అయితే_ముగింపు
    }
    ఒకవేళ (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	వెళ్ళు నిష్క్రమించు;
    }
    exitval = 0;    // The Execute succeeded: exit this program

నిష్క్రమించు:
    ఒకవేళ (nilfh)
	Close(nilfh);
#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (Arp_పునాది)
	CloseLibrary((నిర్మాణం Library *) Arp_పునాది);
#అయితే_ముగింపు
    నిష్క్రమించు(exitval);
    // NOTREACHED
    తిరిగి FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    పూర్ణాంకం
mch_ఆదానం_isatty(శూన్య)
{
    తిరిగి TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    శూన్య
fname_case(
    char_u	*name,
    పూర్ణాంకం		len వాడని)		// buffer size, ignored here
{
    నిర్మాణం File_Info_ఖండం    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ఒకవేళ (fib == ఖాళీ)
	తిరిగి;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#నిర్వచితమైతే __amigaos4__
    ఒకవేళ (fib->fib_Dir_నమోదు_Type == ST_మూలం)
	strcat(fib->fib_FileName, ":");
#అయితే_ముగింపు
    ఒకవేళ (flen == అక్షరశ్రేణి_పొడవు(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    స్థిర నిర్మాణం File_Info_ఖండం *
get_fib(char_u *fname)
{
    BPTR		    flock;
    నిర్మాణం File_Info_ఖండం    *fib;

    ఒకవేళ (fname == ఖాళీ)	    // safety check
	తిరిగి ఖాళీ;
#నిర్వచితమైతే __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#లేకపోతే
    fib = ALLOC_ONE(నిర్మాణం File_Info_ఖండం);
#అయితే_ముగింపు
    ఒకవేళ (fib == ఖాళీ)
	తిరిగి;

    flock = సురక్షిత_తాళం((UBYTE *)fname, (దీర్ఘ)ACCESS_READ);
    ఒకవేళ (flock == (BPTR)ఖాళీ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ఖాళీ;
    }
    ఒకవేళ (flock)
	Un_తాళం(flock);
    తిరిగి fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    శూన్య
mch_settitle(char_u *title, char_u *icon)
{
    ఒకవేళ (wb_కిటికీ != ఖాళీ && title != ఖాళీ)
	సమితి_కిటికీ_Titles(wb_కిటికీ, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    శూన్య
mch_restore_title(పూర్ణాంకం which)
{
    ఒకవేళ (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ఖాళీ);
}

    పూర్ణాంకం
mch_can_restore_title(శూన్య)
{
    తిరిగి (wb_కిటికీ != ఖాళీ);
}

    పూర్ణాంకం
mch_can_restore_icon(శూన్య)
{
    తిరిగి FALSE;
}

    శూన్య
mch_setmouse(పూర్ణాంకం on వాడని)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    పూర్ణాంకం
mch_get_user_name(char_u *s, పూర్ణాంకం len)
{
#ఒకవేళ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    నిర్మాణం passwd   *pwd = getpwuid(getuid());

    ఒకవేళ (pwd != ఖాళీ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	తిరిగి OK;
    }
#అయితే_ముగింపు
    *s = NUL;
    తిరిగి FAIL;
}

/*
 * Insert host name is s[len].
 */
    శూన్య
mch_get_host_name(char_u *s, పూర్ణాంకం len)
{
#ఒకవేళ defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((అక్షరం *)s, len);
#లేకపోతే
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#అయితే_ముగింపు
}

/*
 * return process ID
 */
    దీర్ఘ
mch_get_pid(శూన్య)
{
#ఒకవేళ defined(__amigaos4__)
    తిరిగి (దీర్ఘ) getpid();
#లేదావేళ defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    తిరిగి((దీర్ఘ)కనుగొను_Task(0));
#లేకపోతే
    తిరిగి (దీర్ఘ)0;
#అయితే_ముగింపు
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    పూర్ణాంకం
mch_dirname(char_u *buf, పూర్ణాంకం len)
{
    తిరిగి mch_నిండిన_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
mch_నిండిన_Name(
    char_u	*fname,
    char_u	*buf,
    పూర్ణాంకం		len,
    పూర్ణాంకం		force)
{
    BPTR	l;
    పూర్ణాంకం		retval = FAIL;
    పూర్ణాంకం		i;

    // Lock the file.  If it exists, we can get the exact name.
    ఒకవేళ ((l = సురక్షిత_తాళం((UBYTE *)fname, (దీర్ఘ)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (దీర్ఘ)len - 1);
	Un_తాళం(l);
    }
    లేకపోతే ఒకవేళ (force || !mch_is_నిండిన_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ఒకవేళ ((l = తాళం_2((UBYTE *)"", (దీర్ఘ)ACCESS_READ)) != (BPTR)ఖాళీ)
	{
	    retval = lock2name(l, buf, (దీర్ఘ)len);
	    Un_తాళం(l);
	    ఒకవేళ (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ఒకవేళ (i == 0 || *fname != NUL)
		{
		    ఒకవేళ (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ఒకవేళ (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    తిరిగి retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    పూర్ణాంకం
mch_is_నిండిన_Name(char_u *fname)
{
    తిరిగి (vim_strchr(fname, ':') != ఖాళీ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    స్థిర పూర్ణాంకం
lock2name(BPTR తాళం, char_u *buf, దీర్ఘ len)
{
#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (dos2)		    // use 2.0 function
#అయితే_ముగింపు
	తిరిగి ((పూర్ణాంకం)Name_From_తాళం(తాళం, (UBYTE *)buf, len) ? OK : FAIL);
#నిర్వచితమైతే FEAT_ARP
    లేకపోతే		// use arp function
	తిరిగి ((పూర్ణాంకం)మార్గం_Name(తాళం, (అక్షరం *)buf, (దీర్ఘ)(len/32)) ? OK : FAIL);
#అయితే_ముగింపు
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    దీర్ఘ
mch_getperm(char_u *name)
{
    నిర్మాణం File_Info_ఖండం    *fib;
    దీర్ఘ		    retval = -1;

    fib = get_fib(name);
    ఒకవేళ (fib == ఖాళీ)
	తిరిగి -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    తిరిగి retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
mch_setperm(char_u *name, దీర్ఘ perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    తిరిగి (సమితి_Protection((UBYTE *)name, (దీర్ఘ)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    శూన్య
mch_hide(char_u *name వాడని)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    పూర్ణాంకం
mch_isdir(char_u *name)
{
    నిర్మాణం File_Info_ఖండం    *fib;
    పూర్ణాంకం			    retval = FALSE;

    fib = get_fib(name);
    ఒకవేళ (fib == ఖాళీ)
	తిరిగి FALSE;

#నిర్వచితమైతే __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#లేకపోతే
    retval = ((fib->fib_Dir_నమోదు_Type >= 0) ? TRUE : FALSE);
#అయితే_ముగింపు
    free_fib(fib);
    తిరిగి retval;
}

/*
 * Create directory "name".
 */
    పూర్ణాంకం
mch_mkdir(char_u *name)
{
    BPTR	తాళం;

    తాళం = CreateDir(name);
    ఒకవేళ (తాళం == ఖాళీ)
	తిరిగి -1;

    Un_తాళం(తాళం);
    తిరిగి 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    పూర్ణాంకం
mch_can_exe(char_u *name, char_u **మార్గం వాడని, పూర్ణాంకం use_మార్గం)
{
    పూర్ణాంకం exe = -1;
#నిర్వచితమైతే __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = ఎక్కించు_Seg(name);

    ఒకవేళ (seg && Get_Seg_List_Info_ట్యాగులు(seg, GSLI_Native, ఖాళీ, ట్యాగ్_పూర్తి) !=
	    Get_Seg_List_Info_ట్యాగులు(seg, GSLI_68KHUNK, ఖాళీ, ట్యాగ్_పూర్తి))
    {
	// Test if file permissions allow execution.
	నిర్మాణం Examine_దత్తాంశం *exd = Examine_Object_ట్యాగులు(EX_String_Name_ఆదానం, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    లేకపోతే
    {
	exe = 0;
    }

    Un_ఎక్కించు_Seg(seg);

    // Search for executable in path if applicable.
    ఒకవేళ (!exe && use_మార్గం)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	నిర్మాణం మార్గం_కణుపు *తల = Dup_Cmd_మార్గం_List(ఖాళీ);

	// For each entry, recur to check for executable.
	కొరకు (నిర్మాణం మార్గం_కణుపు *తోక = తల; !exe && తోక;
			       తోక = (నిర్మాణం మార్గం_కణుపు *) BADDR(తోక->pn_తదుపరి))
	{
	    సమితి_Current_Dir(తోక->pn_తాళం);
	    exe = mch_can_exe(name, మార్గం, 0);
	}

	// Go back to where we were.
	Free_Cmd_మార్గం_List(తల);
	సమితి_Current_Dir(cwd);
    }
#అయితే_ముగింపు
    తిరిగి exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    పూర్ణాంకం
mch_nodetype(char_u *name వాడని)
{
    // TODO
    తిరిగి కణుపు_సాధారణ;
}

    శూన్య
mch_early_init(శూన్య)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    శూన్య
mch_exit(పూర్ణాంకం r)
{
    exiting = TRUE;

    ఒకవేళ (raw_లోపల)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    బహిర్గతం_char('\n');
    ఒకవేళ (raw_బహిర్గతం)
    {
	ఒకవేళ (పదం_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ఒకవేళ (కొలత_సమితి)
		బహిర్గతం_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	బహిర్గతం_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (Arp_పునాది)
	CloseLibrary((నిర్మాణం Library *) Arp_పునాది);
#అయితే_ముగింపు
    ఒకవేళ (close_win)
	Close(raw_లోపల);
    ఒకవేళ (r)
	రాయి(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    నిష్క్రమించు(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#నిర్వచించు MP(xx)	((నిర్మాణం సందేశం_పోర్టు *)((నిర్మాణం FileHandle *) (BADDR(xx)))->fh_Type)

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
    శూన్య
mch_settmode(tmode_T tmode)
{
#ఒకవేళ defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ఒకవేళ (!సమితి_బహుళకం_2(raw_లోపల, tmode == TMODE_RAW ? 1 : 0))
#లేకపోతే
    ఒకవేళ (dos_packet(MP(raw_లోపల), (దీర్ఘ)ACTION_SCREEN_బహుళకం,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#అయితే_ముగింపు
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

#అన్తర్భావ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ఒకవేళ defined(__AROS__) || defined(__MORPHOS__)
    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    ఒకవేళ (!పదం_console)
	తిరిగి FAIL;

    ఒకవేళ (raw_లోపల && raw_బహిర్గతం)
    {
	// Save current console mode.
	పూర్ణాంకం old_tmode = cur_tmode;
	అక్షరం ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ఒకవేళ (Write(raw_బహిర్గతం, ctrl, పరిమాణం(ctrl)) == పరిమాణం(ctrl))
	{
	    అక్షరం పరిశీలన[] = "\x9b""1;1;%d;%d r",
		 answ[పరిమాణం(పరిశీలన) + 8] = { '\0' };

	    // Read return sequence from input.
	    ఒకవేళ (Read(raw_లోపల, answ, పరిమాణం(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ఒకవేళ (అక్షరశ్రేణి_చదువు(answ, పరిశీలన, &అడ్డువరుసలు, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    తిరిగి OK;
		}
	    }
	}

	// Restore console mode.
	mch_settmode(old_tmode);
    }

    // I/O error. Default size fallback.
    పదం_console = FALSE;
    Columns = 80;
    అడ్డువరుసలు = 24;

    తిరిగి FAIL;
}
#లేకపోతే
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    పూర్ణాంకం
mch_get_shellsize(శూన్య)
{
    నిర్మాణం ConUnit  *conUnit;
# ifndef __amigaos4__
    అక్షరం	    id_a[పరిమాణం(నిర్మాణం Info_దత్తాంశం) + 3];
# endif
    నిర్మాణం Info_దత్తాంశం *id=0;

    ఒకవేళ (!పదం_console)	// not an amiga window
	వెళ్ళు బహిర్గతం;

    // insure longword alignment
# ifdef __amigaos4__
    ఒకవేళ (!(id = AllocDosObject(DOS_INFODATA, 0)))
	వెళ్ళు బహిర్గతం;
# లేకపోతే
    id = (నిర్మాణం Info_దత్తాంశం *)(((దీర్ఘ)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ఒకవేళ (కొలత_సమితి)
	బహిర్గతం_STR("\233t\233u");	// CSI t CSI u
    బహిర్గతం_flush();

    ఒకవేళ (dos_packet(MP(raw_బహిర్గతం), (దీర్ఘ)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_కిటికీ = (నిర్మాణం కిటికీ *)id->id_Volume_కణుపు) == ఖాళీ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	పదం_console = FALSE;
	వెళ్ళు బహిర్గతం;
    }
    ఒకవేళ (oldwindowtitle == ఖాళీ)
	oldwindowtitle = (char_u *)wb_కిటికీ->Title;
    ఒకవేళ (id->id_లోపల_Use == (BPTR)ఖాళీ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	తిరిగి FAIL;
    }
    conUnit = (నిర్మాణం ConUnit *) ((నిర్మాణం IOStdReq *) id->id_లోపల_Use)->io_Unit;

    // get window size
    అడ్డువరుసలు = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ఒకవేళ (అడ్డువరుసలు < 0 || అడ్డువరుసలు > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	అడ్డువరుసలు = 24;
	పదం_console = FALSE;
	తిరిగి FAIL;
    }

    తిరిగి OK;
బహిర్గతం:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    తిరిగి FAIL;
}
#అయితే_ముగింపు

/*
 * Try to set the real window size to Rows and Columns.
 */
    శూన్య
mch_సమితి_shellsize(శూన్య)
{
    ఒకవేళ (!పదం_console)
	తిరిగి;

    కొలత_సమితి = TRUE;
    బహిర్గతం_char(CSI);
    బహిర్గతం_అంకె((దీర్ఘ)అడ్డువరుసలు);
    బహిర్గతం_char('t');
    బహిర్గతం_char(CSI);
    బహిర్గతం_అంకె((దీర్ఘ)Columns);
    బహిర్గతం_char('u');
    బహిర్గతం_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    శూన్య
mch_new_shellsize(శూన్య)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    స్థిర శూన్య
బహిర్గతం_అంకె(దీర్ఘ n)
{
    బహిర్గతం_STR_NF(tltoa((గుర్తులేని దీర్ఘ)n));
}

#ఒకవేళ !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    స్థిర దీర్ఘ
dos_packet(
    నిర్మాణం సందేశం_పోర్టు *pid,    // process identifier ... (handlers message port)
    దీర్ఘ	    action, // packet type ... (what you want handler to do)
    దీర్ఘ	    arg)    // single argument
{
# ifdef FEAT_ARP
    నిర్మాణం సందేశం_పోర్టు	    *replyport;
    నిర్మాణం StandardPacket   *packet;
    దీర్ఘ		    res1;

    ఒకవేళ (dos2)
# endif
	తిరిగి DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (నిర్మాణం సందేశం_పోర్టు *) Create_పోర్టు(ఖాళీ, 0);	// use arp function
    ఒకవేళ (!replyport)
	తిరిగి (0);

    // Allocate space for a packet, make it public and clear it
    packet = (నిర్మాణం StandardPacket *)
	AllocMem((దీర్ఘ) పరిమాణం(నిర్మాణం StandardPacket), MEMF_ప్రజా | MEMF_CLEAR);
    ఒకవేళ (!packet)
    {
	Delete_పోర్టు(replyport);
	తిరిగి (0);
    }
    packet->sp_సందేశం.mn_కణుపు.ln_Name = (అక్షరం *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_సందేశం);
    packet->sp_Pkt.dp_పోర్టు = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_సందేశం(pid, (నిర్మాణం సందేశం_4 *)packet);	// send packet

    Wait_పోర్టు(replyport);
    Get_సందేశం(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (దీర్ఘ) పరిమాణం(నిర్మాణం StandardPacket));
    Delete_పోర్టు(replyport);

    తిరిగి (res1);
# endif
}
#అయితే_ముగింపు // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    పూర్ణాంకం
mch_call_shell(
    char_u	*cmd,
    పూర్ణాంకం		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    పూర్ణాంకం		x;
    పూర్ణాంకం		tmode = cur_tmode;
#నిర్వచితమైతే AZTEC_C
    పూర్ణాంకం		use_execute;
    char_u	*shellcmd = ఖాళీ;
    char_u	*shellarg;
#అయితే_ముగింపు
    పూర్ణాంకం		retval = 0;

    ఒకవేళ (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	తిరిగి -1;
    }

    ఒకవేళ (పదం_console)
	win_resize_off();	    // window resize events de-activated
    బహిర్గతం_flush();

    ఒకవేళ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = తాళం_2((UBYTE *)"", (దీర్ఘ)ACCESS_READ);   // remember current dir

#ఒకవేళ !defined(AZTEC_C)		    // not tested very much
    ఒకవేళ (cmd == ఖాళీ)
    {
# ifdef FEAT_ARP
	ఒకవేళ (dos2)
# endif
	    x = System_ట్యాగులు(p_sh, SYS_UserShell, TRUE, ట్యాగ్_పూర్తి);
# ifdef FEAT_ARP
	లేకపోతే
	    x = Execute(p_sh, raw_లోపల, raw_బహిర్గతం);
# endif
    }
    లేకపోతే
    {
# ifdef FEAT_ARP
	ఒకవేళ (dos2)
# endif
	    x = System_ట్యాగులు((అక్షరం *)cmd, SYS_UserShell, TRUE, ట్యాగ్_పూర్తి);
# ifdef FEAT_ARP
	లేకపోతే
	    x = Execute((అక్షరం *)cmd, 0L, raw_బహిర్గతం);
# endif
    }
# ifdef FEAT_ARP
    ఒకవేళ ((dos2 && x < 0) || (!dos2 && !x))
# లేకపోతే
    ఒకవేళ (x < 0)
# endif
    {
	సందేశం_puts(_("Cannot execute "));
	ఒకవేళ (cmd == ఖాళీ)
	{
	    సందేశం_puts(_("shell "));
	    సందేశం_outtrans(p_sh);
	}
	లేకపోతే
	    సందేశం_outtrans(cmd);
	సందేశం_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    లేకపోతే ఒకవేళ (!dos2 || x)
# లేకపోతే
    లేకపోతే ఒకవేళ (x)
# endif
    {
	ఒకవేళ ((x = IoErr()) != 0)
	{
	    ఒకవేళ (!(options & SHELL_SILENT))
	    {
		సందేశం_putchar('\n');
		సందేశం_outnum((దీర్ఘ)x);
		సందేశం_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#లేకపోతే	// else part is for AZTEC_C
    ఒకవేళ (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    లేకపోతే
	use_execute = 0;
    ఒకవేళ (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ఒకవేళ (shellcmd == ఖాళీ)	    // out of memory, use Execute
	    use_execute = 1;
	లేకపోతే
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ఒకవేళ (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ఒకవేళ (cmd == ఖాళీ)
    {
	ఒకవేళ (use_execute)
	{
# ifdef FEAT_ARP
	    ఒకవేళ (dos2)
# endif
		x = System_ట్యాగులు((UBYTE *)p_sh, SYS_UserShell, TRUE, ట్యాగ్_పూర్తి);
# ifdef FEAT_ARP
	    లేకపోతే
		x = !Execute((UBYTE *)p_sh, raw_లోపల, raw_బహిర్గతం);
# endif
	}
	లేకపోతే
	    x = fexecl((అక్షరం *)shellcmd, (అక్షరం *)shellcmd, (అక్షరం *)shellarg, ఖాళీ);
    }
    లేకపోతే ఒకవేళ (use_execute)
    {
# ifdef FEAT_ARP
	ఒకవేళ (dos2)
# endif
	    x = System_ట్యాగులు((UBYTE *)cmd, SYS_UserShell, TRUE, ట్యాగ్_పూర్తి);
# ifdef FEAT_ARP
	లేకపోతే
	    x = !Execute((UBYTE *)cmd, 0L, raw_బహిర్గతం);
# endif
    }
    లేకపోతే ఒకవేళ (p_st & 1)
	x = fexecl((అక్షరం *)shellcmd, (అక్షరం *)shellcmd, (అక్షరం *)shellarg,
							   (అక్షరం *)cmd, ఖాళీ);
    లేకపోతే
	x = fexecl((అక్షరం *)shellcmd, (అక్షరం *)shellcmd, (అక్షరం *)shellarg,
					   (అక్షరం *)p_shcf, (అక్షరం *)cmd, ఖాళీ);
# ifdef FEAT_ARP
    ఒకవేళ ((dos2 && x < 0) || (!dos2 && x))
# లేకపోతే
    ఒకవేళ (x < 0)
# endif
    {
	సందేశం_puts(_("Cannot execute "));
	ఒకవేళ (use_execute)
	{
	    ఒకవేళ (cmd == ఖాళీ)
		సందేశం_outtrans(p_sh);
	    లేకపోతే
		సందేశం_outtrans(cmd);
	}
	లేకపోతే
	{
	    సందేశం_puts(_("shell "));
	    సందేశం_outtrans(shellcmd);
	}
	సందేశం_putchar('\n');
	retval = -1;
    }
    లేకపోతే
    {
	ఒకవేళ (use_execute)
	{
# ifdef FEAT_ARP
	    ఒకవేళ (!dos2 || x)
# లేకపోతే
	    ఒకవేళ (x)
# endif
		x = IoErr();
	}
	లేకపోతే
	    x = wait();
	ఒకవేళ (x)
	{
	    ఒకవేళ (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		సందేశం_putchar('\n');
		సందేశం_outnum((దీర్ఘ)x);
		సందేశం_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#అయితే_ముగింపు	// AZTEC_C

    ఒకవేళ ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_తాళం(mydir);
    ఒకవేళ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ఒకవేళ (పదం_console)
	win_resize_on();		// window resize events activated
    తిరిగి retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    శూన్య
mch_breakcheck(పూర్ణాంకం force వాడని)
{
   ఒకవేళ (సమితి_Signal(0L, (దీర్ఘ)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#నిర్వచితమైతే _DCC
# define Chk_Abort chkabort
#అయితే_ముగింపు

#నిర్వచితమైతే LATTICE
శూన్య __regargs __chkabort(శూన్య);

శూన్య __regargs __chkabort(శూన్య)
{}

#లేకపోతే
    దీర్ఘ
Chk_Abort(శూన్య)
{
    తిరిగి(0L);
}
#అయితే_ముగింపు

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

#నిర్వచితమైతే __amigaos4__
# define	ANCHOR_BUF_కొలత	1024
#లేకపోతే
# define ANCHOR_BUF_కొలత (512)
# define ANCHOR_కొలత (పరిమాణం(నిర్మాణం Anchor_మార్గం) + ANCHOR_BUF_కొలత)
#అయితే_ముగింపు

    పూర్ణాంకం
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    పూర్ణాంకం		flags)		// EW_* flags
{
    నిర్మాణం Anchor_మార్గం	*Anchor;
    LONG		ఫలితం_2;
    char_u		*starbuf, *sp, *dp;
    పూర్ణాంకం			ప్రారంభం_len;
    పూర్ణాంకం			matches;
#నిర్వచితమైతే __amigaos4__
    నిర్మాణం ట్యాగ్_అంశం	Anchor_ట్యాగులు[] = {
	{ADO_Strlen, ANCHOR_BUF_కొలత},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{ట్యాగ్_పూర్తి, 0L}
    };
#అయితే_ముగింపు

    ప్రారంభం_len = gap->ga_len;

    // Get our AnchorBase
#నిర్వచితమైతే __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_ట్యాగులు);
#లేకపోతే
    Anchor = alloc_clear(ANCHOR_కొలత);
#అయితే_ముగింపు
    ఒకవేళ (Anchor == ఖాళీ)
	తిరిగి 0;

#నిర్వచితం_కాకపోతే __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_కొలత;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# లేకపోతే
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#అయితే_ముగింపు

#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (dos2)
    {
#అయితే_ముగింపు
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ఒకవేళ (starbuf == ఖాళీ)
	    వెళ్ళు Return;
	కొరకు (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ఒకవేళ (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    లేకపోతే
		*dp++ = *sp;
	}
	*dp = NUL;
	ఫలితం_2 = సరిపోలిక_మొదటి((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#నిర్వచితమైతే FEAT_ARP
    }
    లేకపోతే
	ఫలితం_2 = కనుగొను_మొదటి((అక్షరం *)pat, Anchor);
#అయితే_ముగింపు

    /*
     * Loop to get all matches.
     */
    అయితే (ఫలితం_2 == 0)
    {
#నిర్వచితమైతే __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_బఫరు, flags);
#లేకపోతే
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#అయితే_ముగింపు
#నిర్వచితమైతే FEAT_ARP
	ఒకవేళ (dos2)
#అయితే_ముగింపు
	    ఫలితం_2 = సరిపోలిక_తదుపరి_2(Anchor);
#నిర్వచితమైతే FEAT_ARP
	లేకపోతే
	    ఫలితం_2 = కనుగొను_తదుపరి(Anchor);
#అయితే_ముగింపు
    }
    matches = gap->ga_len - ప్రారంభం_len;

    ఒకవేళ (ఫలితం_2 == ERROR_బఫరు_అధికప్రవాహం)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    లేకపోతే ఒకవేళ (matches == 0 && ఫలితం_2 != ERROR_OBJECT_NOT_దొరికింది
			  && ఫలితం_2 != ERROR_DEVICE_NOT_MOUNTED
			  && ఫలితం_2 != ERROR_NO_మరిన్ని_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ఒకవేళ (matches)
	qsort((శూన్య *)(((char_u **)gap->ga_దత్తాంశం) + ప్రారంభం_len),
				  (size_t)matches, పరిమాణం(char_u *), sortcmp);

    // Free the wildcard stuff
#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (dos2)
#అయితే_ముగింపు
	సరిపోలిక_చివర_3(Anchor);
#నిర్వచితమైతే FEAT_ARP
    లేకపోతే
	FreeAnchorChain(Anchor);
#అయితే_ముగింపు

Return:
#నిర్వచితమైతే __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#లేకపోతే
    vim_free(Anchor);
#అయితే_ముగింపు

    తిరిగి matches;
}

    స్థిర పూర్ణాంకం
sortcmp(స్థిరరాశి శూన్య *a, స్థిరరాశి శూన్య *b)
{
    అక్షరం *s = *(అక్షరం **)a;
    అక్షరం *t = *(అక్షరం **)b;

    తిరిగి pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    పూర్ణాంకం
mch_has_exp_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే ఒకవేళ (vim_strchr((char_u *)"*?[(#", *p) != ఖాళీ)
	    తిరిగి TRUE;
    }
    తిరిగి FALSE;
}

    పూర్ణాంకం
mch_has_wildcard(char_u *p)
{
    కొరకు ( ; *p; MB_PTR_ADV(p))
    {
	ఒకవేళ (*p == '\\' && p[1] != NUL)
	    ++p;
	లేకపోతే
	    ఒకవేళ (vim_strchr((char_u *)"*?[(#$`", *p) != ఖాళీ
		    || (*p == '~' && p[1] != NUL))
		తిరిగి TRUE;
    }
    తిరిగి FALSE;
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
    పూర్ణాంకం		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    స్థిర char_u   *alloced = ఖాళీ;	// allocated memory

#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (!dos2)
	retval = (char_u *)getenv((అక్షరం *)var);
    లేకపోతే
#అయితే_ముగింపు
    {
	VIM_CLEAR(alloced);
	retval = ఖాళీ;

	buf = alloc(IOSIZE);
	ఒకవేళ (buf == ఖాళీ)
	    తిరిగి ఖాళీ;

	len = GetVar((UBYTE *)var, buf, (దీర్ఘ)(IOSIZE - 1), (దీర్ఘ)0);
	ఒకవేళ (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ఒకవేళ (retval == ఖాళీ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    తిరిగి retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    పూర్ణాంకం
mch_setenv(అక్షరం *var, అక్షరం *విలువ, పూర్ణాంకం x వాడని)
{
#నిర్వచితమైతే FEAT_ARP
    ఒకవేళ (!dos2)
	తిరిగి setenv(var, విలువ);
#అయితే_ముగింపు

    ఒకవేళ (సమితి_Var((UBYTE *)var, (UBYTE *)విలువ, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	తిరిగి 0;   // success
    తిరిగి -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    పూర్ణాంకం
mch_get_random(char_u *buf, పూర్ణాంకం len)
{
    నిర్మాణం Process *proc = (నిర్మాణం Process *) కనుగొను_Task(0L);
    APTR win = proc->pr_కిటికీ_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_కిటికీ_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", బహుళకం_OLDFILE);

    proc->pr_కిటికీ_Ptr = win;

    పూర్ణాంకం స్థితి_2;

    ఒకవేళ (!fh || Read(fh, buf, len) != len)
	స్థితి_2 = FAIL;
    లేకపోతే
	స్థితి_2 = OK;

    Close(fh);
    తిరిగి స్థితి_2;
}
