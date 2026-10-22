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

#ಅನ್ತರ್ಭಾವ "vim.h"
#ಅನ್ತರ್ಭಾವ "version.h"

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ ಕಿಟಕಿ
# undef ಕಿಟಕಿ	// Amiga has its own Window definition
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು TRUE		// will be redefined by exec/types.h
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು FALSE

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#ಆದರೆ_ಅಂತ್ಯ

// XXX These are included from os_amiga.h
// #include <exec/types.h>
// #include <libraries/dos.h>
// #include <libraries/dosextens.h>
// #include <proto/exec.h>
// #include <proto/dos.h>
// #include <proto/intuition.h>

#ಅನ್ತರ್ಭಾವ <exec/memory.h>

#ಅನ್ತರ್ಭಾವ <dos/dostags.h>	    // for 2.0 functions
#ಅನ್ತರ್ಭಾವ <dos/dosasl.h>

// From version 4 of AmigaOS, several system structures must be allocated
// and freed using system functions. "struct AnchorPath" is one.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#ಇಲ್ಲದಿದ್ದರೆ
# define	free_fib(x) vim_free(fib)
#ಆದರೆ_ಅಂತ್ಯ

#ಆದರೆ defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#ಆದರೆ defined(__amigaos4__)
ಸ್ಥಿರ ಸ್ಥಿರಾಂಕ ಅಕ್ಷರ* __attribute__((ಬಳಸಿದ)) stackcookie = "$STACK: 1048576";
#ಅಥವಾಆದರೆ defined(__AROS__) || defined(__MORPHOS__)
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __stack = 1048576;
#ಅಥವಾಆದರೆ defined(__GNUC__) && defined(AMIGA)
ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ __stack = 262144;
#ಆದರೆ_ಅಂತ್ಯ

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	TRUE
#ವ್ಯಾಖ್ಯಾನಿಸು TRUE (1)
#ವ್ಯಾಖ್ಯಾನ_ರದ್ದು	FALSE
#ವ್ಯಾಖ್ಯಾನಿಸು FALSE (0)

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#ಅಥವಾಆದರೆ !defined(AZTEC_C) && !defined(__AROS__)
ಸ್ಥಿರ ದೀರ್ಘ dos_packet(ರಚನೆ ಸಂದೇಶ_ಪೋರ್ಟ್ *, ದೀರ್ಘ, ದೀರ್ಘ);
#ಆದರೆ_ಅಂತ್ಯ
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ lock2name(BPTR ಬೀಗ, char_u *buf, ದೀರ್ಘ   len);
ಸ್ಥಿರ ಶೂನ್ಯ ಹೊರಹೋಗು_ಅಂಕೆ(ದೀರ್ಘ n);
ಸ್ಥಿರ ರಚನೆ File_Info_ಖಂಡ *get_fib(char_u *);
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ sortcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ಸ್ಥಿರ BPTR
ಸುರಕ್ಷಿತ_ಬೀಗ(UBYTE *name, ದೀರ್ಘ ಬಹುಲಕ)
{
    ರಚನೆ Process *me = (ರಚನೆ Process *)ಕಂಡುಹಿಡಿ_Task(ಖಾಲಿ);
    APTR oldwin = me->pr_ಕಿಟಕಿ_Ptr;
    BPTR flock;

    me->pr_ಕಿಟಕಿ_Ptr = (APTR)-1L;
    flock = ಬೀಗ_2(name, ಬಹುಲಕ);
    me->pr_ಕಿಟಕಿ_Ptr = oldwin;
    ಹಿಂತಿರುಗಿ flock;
}

ಸ್ಥಿರ BPTR		raw_ಒಳ = (BPTR)ಖಾಲಿ;
ಸ್ಥಿರ BPTR		raw_ಹೊರಹೋಗು = (BPTR)ಖಾಲಿ;
ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ರಚನೆ Intuition_ತಳ	*Intuition_ತಳ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
ರಚನೆ Arp_ತಳ		*Arp_ತಳ = ಖಾಲಿ;
#ಆದರೆ_ಅಂತ್ಯ

ಸ್ಥಿರ ರಚನೆ ಕಿಟಕಿ	*wb_ಕಿಟಕಿ;
ಸ್ಥಿರ char_u		*oldwindowtitle = ಖಾಲಿ;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
ಪೂರ್ಣಾಂಕ			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#ಆದರೆ_ಅಂತ್ಯ
ಪೂರ್ಣಾಂಕ			ಗಾತ್ರ_ಗಣ = FALSE;   // set to TRUE if window size was set

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __GNUC__
ಸ್ಥಿರ ಅಕ್ಷರ ಆವೃತ್ತಿ_2[] __attribute__((ಬಳಸಿದ)) =
    "\0$VER: Vim "
    VIM_ಆವೃತ್ತಿ_MAJOR_STR "."
    VIM_ಆವೃತ್ತಿ_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
win_resize_on(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_STR_NF("\033[12{");
}

    ಶೂನ್ಯ
win_resize_off(ಶೂನ್ಯ)
{
    ಹೊರಹೋಗು_STR_NF("\033[12}");
}

    ಶೂನ್ಯ
mch_write(char_u *p, ಪೂರ್ಣಾಂಕ len)
{
    Write(raw_ಹೊರಹೋಗು, (ಅಕ್ಷರ *)p, (ದೀರ್ಘ)len);
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
    ಪೂರ್ಣಾಂಕ
mch_inchar(
    char_u  *buf,
    ಪೂರ್ಣಾಂಕ	    maxlen,
    ದೀರ್ಘ    ಸಮಯ,		// milliseconds
    ಪೂರ್ಣಾಂಕ	    tb_change_cnt ಬಳಸದ)
{
    ಪೂರ್ಣಾಂಕ	    len;
    ದೀರ್ಘ    utime;

    ಆದರೆ (ಸಮಯ >= 0)
    {
	ಆದರೆ (ಸಮಯ == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	ಇಲ್ಲದಿದ್ದರೆ
	    utime = ಸಮಯ * 1000L;   // convert from milli to micro secs
	ಆದರೆ (WaitForChar(raw_ಒಳ, utime) == 0)	// no character available
	    ಹಿಂತಿರುಗಿ 0;
    }
    ಇಲ್ಲದಿದ್ದರೆ    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	ಆದರೆ (WaitForChar(raw_ಒಳ, p_ut * 1000L) == 0)
	{
	    ಆದರೆ (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (ಪೂರ್ಣಾಂಕ)KE_CURSORHOLD;
		ಹಿಂತಿರುಗಿ 3;
	    }
	    before_blocking();
	}
    }

    ಗಾಗಿ (;;)	    // repeat until we got a character
    {
	len = Read(raw_ಒಳ, (ಅಕ್ಷರ *)buf, (ದೀರ್ಘ)maxlen / ಒಳಹರಿವು_conv.vc_ಗುಣಕ);
	ಆದರೆ (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    ಆದರೆ (ಒಳಹರಿವು_conv.vc_type != CONV_NONE)
		len = convert_ಒಳಹರಿವು(buf, len, maxlen);
	    ಹಿಂತಿರುಗಿ len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    ಪೂರ್ಣಾಂಕ
mch_char_avail(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (WaitForChar(raw_ಒಳ, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(ಪೂರ್ಣಾಂಕ special)
{
#ಆದರೆ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ಹಿಂತಿರುಗಿ (long_u)AvailMem(MEMF_ANY) >> 10;
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (long_u)(AvailMem(special ? (ದೀರ್ಘ)MEMF_CHIP : (ದೀರ್ಘ)MEMF_ANY)) >> 10;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    ಶೂನ್ಯ
mch_delay(ದೀರ್ಘ msec, ಪೂರ್ಣಾಂಕ flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LATTICE
    ಶೂನ್ಯ	    Delay(ದೀರ್ಘ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (msec <= 0)
	ಹಿಂತಿರುಗಿ;

    ಆದರೆ (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    ಇಲ್ಲದಿದ್ದರೆ
	WaitForChar(raw_ಒಳ, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    ಶೂನ್ಯ
mch_suspend(ಶೂನ್ಯ)
{
    suspend_shell();
}

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#ಆದರೆ_ಅಂತ್ಯ

    ಶೂನ್ಯ
mch_init(ಶೂನ್ಯ)
{
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ಸ್ಥಿರ ಅಕ್ಷರ	    intlibname[] = "intuition.library";
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#ಆದರೆ_ಅಂತ್ಯ

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	ರಚನೆ Process *me = (ರಚನೆ Process *)ಕಂಡುಹಿಡಿ_Task(ಖಾಲಿ);
	me->pr_ಕಿಟಕಿ_Ptr = (APTR)-1L;
    }

    Columns = 80;
    ಅಡ್ಡಸಾಲುಗಳು = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    ಆದರೆ (raw_ಒಳ == (BPTR)ಖಾಲಿ)
    {
	raw_ಒಳ = ಒಳಹರಿವು_3();
	raw_ಹೊರಹೋಗು = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	ಆದರೆ (!IsInteractive(raw_ಒಳ))
	    raw_ಒಳ = raw_ಹೊರಹೋಗು;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!IsInteractive(raw_ಹೊರಹೋಗು))
	    raw_ಹೊರಹೋಗು = raw_ಒಳ;
    }

    ಹೊರಹೋಗು_flush();

    wb_ಕಿಟಕಿ = ಖಾಲಿ;
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ಆದರೆ ((Intuition_ತಳ = (ರಚನೆ Intuition_ತಳ *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == ಖಾಲಿ)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#ಆದರೆ_ಅಂತ್ಯ
}

#ಅನ್ತರ್ಭಾವ <workbench/startup.h>

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
#ವ್ಯಾಖ್ಯಾನಿಸು BUF2_ಗಾತ್ರ 320	    // length of buffer for argument with complete path

    ಪೂರ್ಣಾಂಕ
mch_check_win(ಪೂರ್ಣಾಂಕ argc, ಅಕ್ಷರ **argv)
{
    ಪೂರ್ಣಾಂಕ		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_ಗಾತ್ರ];
    ಸ್ಥಿರ char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ಸ್ಥಿರ char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ರಚನೆ WBArg    *argp;
    ಪೂರ್ಣಾಂಕ		    ac;
    ಅಕ್ಷರ	    *av;
    char_u	    *device = ಖಾಲಿ;
    ಪೂರ್ಣಾಂಕ		    exitval = 4;
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ರಚನೆ Library  *Dos_ತಳ;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_ತಳ = OpenLibrary(DOS_LIBRARY, 37L);
    ಆದರೆ (Dos_ತಳ != ಖಾಲಿ)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_ತಳ);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	ನಿರ್ಗಮಿಸು(3);
# ಇಲ್ಲದಿದ್ದರೆ
		    // need arp functions for dos 1.x
	ಆದರೆ (!(Arp_ತಳ = (ರಚನೆ Arp_ತಳ *) OpenLibrary((UBYTE *)ArpName, Arp_ಆವೃತ್ತಿ)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_ಆವೃತ್ತಿ);
	    ನಿರ್ಗಮಿಸು(3);
	}
# endif
    }
#ಆದರೆ_ಅಂತ್ಯ	/* __amigaos4__ __AROS__ __MORPHOS__ */

    /*
     * scan argv[] for the "-f" and "-d" arguments
     */
    ಗಾಗಿ (i = 1; i < argc; ++i)
	ಆದರೆ (argv[i][0] == '-')
	{
	    ಆಯ್ಕೆ (argv[i][1])
	    {
	    ಸ್ಥಿತಿ 'f':
		usewin = TRUE;
		ನಿಲ್ಲಿಸು;

	    ಸ್ಥಿತಿ 'd':
		ಆದರೆ (i < argc - 1
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#ಆದರೆ_ಅಂತ್ಯ
		   )
		    device = (char_u *)argv[i + 1];
		ನಿಲ್ಲಿಸು;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    ಆದರೆ (argc != 0
	    && device == ಖಾಲಿ
	    && (IsInteractive(ಒಳಹರಿವು_3()) || IsInteractive(Output())))
	ಹಿಂತಿರುಗಿ OK;

/*
 * When given the "-f" argument, we open our own window. We can't use the
 * newcli trick below, because the calling program (mail, rn, etc.) would not
 * know when we are finished.
 */
    ಆದರೆ (usewin)
    {
	/*
	 * Try to open a window. First try the specified device.
	 * Then try a 24 line 80 column window.
	 * If that fails, try two smaller ones.
	 */
	ಗಾಗಿ (i = -1; i < 3; ++i)
	{
	    ಆದರೆ (i >= 0)
		device = constrings[i];
	    ಆದರೆ (device != ಖಾಲಿ && (raw_ಒಳ = Open((UBYTE *)device,
					   (ದೀರ್ಘ)ಬಹುಲಕ_NEWFILE)) != (BPTR)ಖಾಲಿ)
		ನಿಲ್ಲಿಸು;
	}
	ಆದರೆ (raw_ಒಳ == (BPTR)ಖಾಲಿ)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    ಹೋಗು ನಿರ್ಗಮಿಸು;
	}
	raw_ಹೊರಹೋಗು = raw_ಒಳ;
	close_win = TRUE;
	ಹಿಂತಿರುಗಿ OK;
    }

    ಆದರೆ ((nilfh = Open((UBYTE *)"NIL:", (ದೀರ್ಘ)ಬಹುಲಕ_NEWFILE)) == (BPTR)ಖಾಲಿ)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#ಆದರೆ !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((ಅಕ್ಷರ *)buf1, "t:nc%p", ಕಂಡುಹಿಡಿ_Task(0));
#ಇಲ್ಲದಿದ್ದರೆ
    sprintf((ಅಕ್ಷರ *)buf1, "t:nc%ld", (ದೀರ್ಘ)buf1);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ ((fh = Open((UBYTE *)buf1, (ದೀರ್ಘ)ಬಹುಲಕ_NEWFILE)) == (BPTR)ಖಾಲಿ)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((ಅಕ್ಷರ *)buf1);
	mch_errmsg("\n");
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    ಆದರೆ (argc == 0)	// run from workbench
	ac = ((ರಚನೆ WBStartup *)argv)->sm_ಅಂಕೆ_ವಾದಗಳು;
    ಇಲ್ಲದಿದ್ದರೆ
	ac = argc;
    ಗಾಗಿ (i = 0; i < ac; ++i)
    {
	ಆದರೆ (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ರಚನೆ WBStartup *)argv)->sm_ArgList[i]);
	    ಆದರೆ (argp->wa_ಬೀಗ)
		(ಶೂನ್ಯ)lock2name(argp->wa_ಬೀಗ, buf2, (ದೀರ್ಘ)(BUF2_ಗಾತ್ರ - 1));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	    ಆದರೆ (dos2)	    // use 2.0 function
#ಆದರೆ_ಅಂತ್ಯ
		ಕೂಡಿಸು_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (ದೀರ್ಘ)(BUF2_ಗಾತ್ರ - 1));
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	    ಇಲ್ಲದಿದ್ದರೆ	    // use arp function
		TackOn((ಅಕ್ಷರ *)buf2, argp->wa_Name);
#ಆದರೆ_ಅಂತ್ಯ
	    av = (ಅಕ್ಷರ *)buf2;
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    av = argv[i];

	// skip '-d' or "-dev" option
	ಆದರೆ (av[0] == '-' && av[1] == 'd'
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#ಆದರೆ_ಅಂತ್ಯ
		)
	{
	    ++i;
	    ಮುಂದುವರಿ;
	}
	ಆದರೆ (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (ದೀರ್ಘ)ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(av));
	ಆದರೆ (vim_strchr((char_u *)av, ' '))
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
    ಗಾಗಿ (i = -1; i < 3; ++i)
    {
	ಆದರೆ (i >= 0)
	    device = constrings[i];
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (device == ಖಾಲಿ)
	    ಮುಂದುವರಿ;
	sprintf((ಅಕ್ಷರ *)buf2, "newcli <nil: >nil: %s from %s", (ಅಕ್ಷರ *)device, (ಅಕ್ಷರ *)buf1);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	ಆದರೆ (dos2)
	{
#ಆದರೆ_ಅಂತ್ಯ
	    ಆದರೆ (!System_ಟ್ಯಾಗುಗಳು((UBYTE *)buf2, SYS_UserShell, TRUE, ಟ್ಯಾಗ್_ಮುಗಿದ))
		ನಿಲ್ಲಿಸು;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಆದರೆ (Execute((UBYTE *)buf2, nilfh, nilfh))
		ನಿಲ್ಲಿಸು;
	}
#ಆದರೆ_ಅಂತ್ಯ
    }
    ಆದರೆ (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	ಹೋಗು ನಿರ್ಗಮಿಸು;
    }
    exitval = 0;    // The Execute succeeded: exit this program

ನಿರ್ಗಮಿಸು:
    ಆದರೆ (nilfh)
	Close(nilfh);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (Arp_ತಳ)
	CloseLibrary((ರಚನೆ Library *) Arp_ತಳ);
#ಆದರೆ_ಅಂತ್ಯ
    ನಿರ್ಗಮಿಸು(exitval);
    // NOTREACHED
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    ಪೂರ್ಣಾಂಕ
mch_ಒಳಹರಿವು_isatty(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    ಶೂನ್ಯ
fname_case(
    char_u	*name,
    ಪೂರ್ಣಾಂಕ		len ಬಳಸದ)		// buffer size, ignored here
{
    ರಚನೆ File_Info_ಖಂಡ    *fib;
    size_t		    flen;

    fib = get_fib(name);
    ಆದರೆ (fib == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    ಆದರೆ (fib->fib_Dir_ನಮೂದು_Type == ST_ಬೇರು)
	strcat(fib->fib_FileName, ":");
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (flen == ಅಕ್ಷರಸರಣಿ_ಉದ್ದ(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ಸ್ಥಿರ ರಚನೆ File_Info_ಖಂಡ *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ರಚನೆ File_Info_ಖಂಡ    *fib;

    ಆದರೆ (fname == ಖಾಲಿ)	    // safety check
	ಹಿಂತಿರುಗಿ ಖಾಲಿ;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#ಇಲ್ಲದಿದ್ದರೆ
    fib = ALLOC_ONE(ರಚನೆ File_Info_ಖಂಡ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (fib == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ;

    flock = ಸುರಕ್ಷಿತ_ಬೀಗ((UBYTE *)fname, (ದೀರ್ಘ)ACCESS_READ);
    ಆದರೆ (flock == (BPTR)ಖಾಲಿ || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = ಖಾಲಿ;
    }
    ಆದರೆ (flock)
	Un_ಬೀಗ(flock);
    ಹಿಂತಿರುಗಿ fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    ಶೂನ್ಯ
mch_settitle(char_u *title, char_u *icon)
{
    ಆದರೆ (wb_ಕಿಟಕಿ != ಖಾಲಿ && title != ಖಾಲಿ)
	ಗಣ_ಕಿಟಕಿ_Titles(wb_ಕಿಟಕಿ, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    ಶೂನ್ಯ
mch_restore_title(ಪೂರ್ಣಾಂಕ which)
{
    ಆದರೆ (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
mch_can_restore_title(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ (wb_ಕಿಟಕಿ != ಖಾಲಿ);
}

    ಪೂರ್ಣಾಂಕ
mch_can_restore_icon(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಶೂನ್ಯ
mch_setmouse(ಪೂರ್ಣಾಂಕ on ಬಳಸದ)
{
    // TODO: implement
}

/*
 * Insert user name in s[len].
 */
    ಪೂರ್ಣಾಂಕ
mch_get_user_name(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
#ಆದರೆ defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    ರಚನೆ passwd   *pwd = getpwuid(getuid());

    ಆದರೆ (pwd != ಖಾಲಿ && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	ಹಿಂತಿರುಗಿ OK;
    }
#ಆದರೆ_ಅಂತ್ಯ
    *s = NUL;
    ಹಿಂತಿರುಗಿ FAIL;
}

/*
 * Insert host name is s[len].
 */
    ಶೂನ್ಯ
mch_get_host_name(char_u *s, ಪೂರ್ಣಾಂಕ len)
{
#ಆದರೆ defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((ಅಕ್ಷರ *)s, len);
#ಇಲ್ಲದಿದ್ದರೆ
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * return process ID
 */
    ದೀರ್ಘ
mch_get_pid(ಶೂನ್ಯ)
{
#ಆದರೆ defined(__amigaos4__)
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ) getpid();
#ಅಥವಾಆದರೆ defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    ಹಿಂತಿರುಗಿ((ದೀರ್ಘ)ಕಂಡುಹಿಡಿ_Task(0));
#ಇಲ್ಲದಿದ್ದರೆ
    ಹಿಂತಿರುಗಿ (ದೀರ್ಘ)0;
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    ಪೂರ್ಣಾಂಕ
mch_dirname(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ಹಿಂತಿರುಗಿ mch_ತುಂಬಿದ_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
mch_ತುಂಬಿದ_Name(
    char_u	*fname,
    char_u	*buf,
    ಪೂರ್ಣಾಂಕ		len,
    ಪೂರ್ಣಾಂಕ		force)
{
    BPTR	l;
    ಪೂರ್ಣಾಂಕ		retval = FAIL;
    ಪೂರ್ಣಾಂಕ		i;

    // Lock the file.  If it exists, we can get the exact name.
    ಆದರೆ ((l = ಸುರಕ್ಷಿತ_ಬೀಗ((UBYTE *)fname, (ದೀರ್ಘ)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (ದೀರ್ಘ)len - 1);
	Un_ಬೀಗ(l);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (force || !mch_is_ತುಂಬಿದ_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	ಆದರೆ ((l = ಬೀಗ_2((UBYTE *)"", (ದೀರ್ಘ)ACCESS_READ)) != (BPTR)ಖಾಲಿ)
	{
	    retval = lock2name(l, buf, (ದೀರ್ಘ)len);
	    Un_ಬೀಗ(l);
	    ಆದರೆ (retval == OK)
	    {
		i = STRLEN(buf);
		// Concatenate the fname to the directory.  Don't add a slash
		// if fname is empty, but do change "" to "/".
		ಆದರೆ (i == 0 || *fname != NUL)
		{
		    ಆದರೆ (i < len - 1 && (i == 0 || buf[i - 1] != ':'))
			buf[i++] = '/';
		    vim_strncpy(buf + i, fname, len - i - 1);
		}
	    }
	}
    }
    ಆದರೆ (*buf == 0 || *buf == ':')
	retval = FAIL;	// something failed; use the file name
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    ಪೂರ್ಣಾಂಕ
mch_is_ತುಂಬಿದ_Name(char_u *fname)
{
    ಹಿಂತಿರುಗಿ (vim_strchr(fname, ':') != ಖಾಲಿ && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
lock2name(BPTR ಬೀಗ, char_u *buf, ದೀರ್ಘ len)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (dos2)		    // use 2.0 function
#ಆದರೆ_ಅಂತ್ಯ
	ಹಿಂತಿರುಗಿ ((ಪೂರ್ಣಾಂಕ)Name_From_ಬೀಗ(ಬೀಗ, (UBYTE *)buf, len) ? OK : FAIL);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಇಲ್ಲದಿದ್ದರೆ		// use arp function
	ಹಿಂತಿರುಗಿ ((ಪೂರ್ಣಾಂಕ)ಮಾರ್ಗ_Name(ಬೀಗ, (ಅಕ್ಷರ *)buf, (ದೀರ್ಘ)(len/32)) ? OK : FAIL);
#ಆದರೆ_ಅಂತ್ಯ
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    ದೀರ್ಘ
mch_getperm(char_u *name)
{
    ರಚನೆ File_Info_ಖಂಡ    *fib;
    ದೀರ್ಘ		    retval = -1;

    fib = get_fib(name);
    ಆದರೆ (fib == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
mch_setperm(char_u *name, ದೀರ್ಘ perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    ಹಿಂತಿರುಗಿ (ಗಣ_Protection((UBYTE *)name, (ದೀರ್ಘ)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    ಶೂನ್ಯ
mch_hide(char_u *name ಬಳಸದ)
{
    // can't hide a file
}

/*
 * return FALSE if "name" is not a directory
 * return TRUE if "name" is a directory.
 * return FALSE for error.
 */
    ಪೂರ್ಣಾಂಕ
mch_isdir(char_u *name)
{
    ರಚನೆ File_Info_ಖಂಡ    *fib;
    ಪೂರ್ಣಾಂಕ			    retval = FALSE;

    fib = get_fib(name);
    ಆದರೆ (fib == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ FALSE;

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#ಇಲ್ಲದಿದ್ದರೆ
    retval = ((fib->fib_Dir_ನಮೂದು_Type >= 0) ? TRUE : FALSE);
#ಆದರೆ_ಅಂತ್ಯ
    free_fib(fib);
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Create directory "name".
 */
    ಪೂರ್ಣಾಂಕ
mch_mkdir(char_u *name)
{
    BPTR	ಬೀಗ;

    ಬೀಗ = CreateDir(name);
    ಆದರೆ (ಬೀಗ == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ -1;

    Un_ಬೀಗ(ಬೀಗ);
    ಹಿಂತಿರುಗಿ 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    ಪೂರ್ಣಾಂಕ
mch_can_exe(char_u *name, char_u **ಮಾರ್ಗ ಬಳಸದ, ಪೂರ್ಣಾಂಕ use_ಮಾರ್ಗ)
{
    ಪೂರ್ಣಾಂಕ exe = -1;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = ತುಂಬು_Seg(name);

    ಆದರೆ (seg && Get_Seg_List_Info_ಟ್ಯಾಗುಗಳು(seg, GSLI_Native, ಖಾಲಿ, ಟ್ಯಾಗ್_ಮುಗಿದ) !=
	    Get_Seg_List_Info_ಟ್ಯಾಗುಗಳು(seg, GSLI_68KHUNK, ಖಾಲಿ, ಟ್ಯಾಗ್_ಮುಗಿದ))
    {
	// Test if file permissions allow execution.
	ರಚನೆ Examine_ದತ್ತಾಂಶ *exd = Examine_Object_ಟ್ಯಾಗುಗಳು(EX_String_Name_ಒಳಹರಿವು, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	exe = 0;
    }

    Un_ತುಂಬು_Seg(seg);

    // Search for executable in path if applicable.
    ಆದರೆ (!exe && use_ಮಾರ್ಗ)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ರಚನೆ ಮಾರ್ಗ_ಗಂಟು *ತಲೆ = Dup_Cmd_ಮಾರ್ಗ_List(ಖಾಲಿ);

	// For each entry, recur to check for executable.
	ಗಾಗಿ (ರಚನೆ ಮಾರ್ಗ_ಗಂಟು *ಬಾಲ = ತಲೆ; !exe && ಬಾಲ;
			       ಬಾಲ = (ರಚನೆ ಮಾರ್ಗ_ಗಂಟು *) BADDR(ಬಾಲ->pn_ಮುಂದಿನ))
	{
	    ಗಣ_Current_Dir(ಬಾಲ->pn_ಬೀಗ);
	    exe = mch_can_exe(name, ಮಾರ್ಗ, 0);
	}

	// Go back to where we were.
	Free_Cmd_ಮಾರ್ಗ_List(ತಲೆ);
	ಗಣ_Current_Dir(cwd);
    }
#ಆದರೆ_ಅಂತ್ಯ
    ಹಿಂತಿರುಗಿ exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    ಪೂರ್ಣಾಂಕ
mch_nodetype(char_u *name ಬಳಸದ)
{
    // TODO
    ಹಿಂತಿರುಗಿ ಗಂಟು_ಸಾಮಾನ್ಯ;
}

    ಶೂನ್ಯ
mch_early_init(ಶೂನ್ಯ)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    ಶೂನ್ಯ
mch_exit(ಪೂರ್ಣಾಂಕ r)
{
    exiting = TRUE;

    ಆದರೆ (raw_ಒಳ)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    ಹೊರಹೋಗು_char('\n');
    ಆದರೆ (raw_ಹೊರಹೋಗು)
    {
	ಆದರೆ (ಪದ_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    ಆದರೆ (ಗಾತ್ರ_ಗಣ)
		ಹೊರಹೋಗು_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	ಹೊರಹೋಗು_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (Arp_ತಳ)
	CloseLibrary((ರಚನೆ Library *) Arp_ತಳ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (close_win)
	Close(raw_ಒಳ);
    ಆದರೆ (r)
	ಬರೆ(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    ನಿರ್ಗಮಿಸು(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#ವ್ಯಾಖ್ಯಾನಿಸು MP(xx)	((ರಚನೆ ಸಂದೇಶ_ಪೋರ್ಟ್ *)((ರಚನೆ FileHandle *) (BADDR(xx)))->fh_Type)

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
    ಶೂನ್ಯ
mch_settmode(tmode_T tmode)
{
#ಆದರೆ defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    ಆದರೆ (!ಗಣ_ಬಹುಲಕ_2(raw_ಒಳ, tmode == TMODE_RAW ? 1 : 0))
#ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (dos_packet(MP(raw_ಒಳ), (ದೀರ್ಘ)ACTION_SCREEN_ಬಹುಲಕ,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#ಆದರೆ_ಅಂತ್ಯ
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

#ಅನ್ತರ್ಭಾವ <devices/conunit.h>

/*
 * Get console size in a system friendly way on AROS and MorphOS.
 * Return FAIL for failure, OK otherwise
 */
#ಆದರೆ defined(__AROS__) || defined(__MORPHOS__)
    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    ಆದರೆ (!ಪದ_console)
	ಹಿಂತಿರುಗಿ FAIL;

    ಆದರೆ (raw_ಒಳ && raw_ಹೊರಹೋಗು)
    {
	// Save current console mode.
	ಪೂರ್ಣಾಂಕ old_tmode = cur_tmode;
	ಅಕ್ಷರ ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	ಆದರೆ (Write(raw_ಹೊರಹೋಗು, ctrl, ಗಾತ್ರ(ctrl)) == ಗಾತ್ರ(ctrl))
	{
	    ಅಕ್ಷರ ಪರಿಶೀಲನೆ[] = "\x9b""1;1;%d;%d r",
		 answ[ಗಾತ್ರ(ಪರಿಶೀಲನೆ) + 8] = { '\0' };

	    // Read return sequence from input.
	    ಆದರೆ (Read(raw_ಒಳ, answ, ಗಾತ್ರ(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		ಆದರೆ (ಅಕ್ಷರಸರಣಿ_ಓದು(answ, ಪರಿಶೀಲನೆ, &ಅಡ್ಡಸಾಲುಗಳು, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    ಹಿಂತಿರುಗಿ OK;
		}
	    }
	}

	// Restore console mode.
	mch_settmode(old_tmode);
    }

    // I/O error. Default size fallback.
    ಪದ_console = FALSE;
    Columns = 80;
    ಅಡ್ಡಸಾಲುಗಳು = 24;

    ಹಿಂತಿರುಗಿ FAIL;
}
#ಇಲ್ಲದಿದ್ದರೆ
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    ಪೂರ್ಣಾಂಕ
mch_get_shellsize(ಶೂನ್ಯ)
{
    ರಚನೆ ConUnit  *conUnit;
# ifndef __amigaos4__
    ಅಕ್ಷರ	    id_a[ಗಾತ್ರ(ರಚನೆ Info_ದತ್ತಾಂಶ) + 3];
# endif
    ರಚನೆ Info_ದತ್ತಾಂಶ *id=0;

    ಆದರೆ (!ಪದ_console)	// not an amiga window
	ಹೋಗು ಹೊರಹೋಗು;

    // insure longword alignment
# ifdef __amigaos4__
    ಆದರೆ (!(id = AllocDosObject(DOS_INFODATA, 0)))
	ಹೋಗು ಹೊರಹೋಗು;
# ಇಲ್ಲದಿದ್ದರೆ
    id = (ರಚನೆ Info_ದತ್ತಾಂಶ *)(((ದೀರ್ಘ)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    ಆದರೆ (ಗಾತ್ರ_ಗಣ)
	ಹೊರಹೋಗು_STR("\233t\233u");	// CSI t CSI u
    ಹೊರಹೋಗು_flush();

    ಆದರೆ (dos_packet(MP(raw_ಹೊರಹೋಗು), (ದೀರ್ಘ)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_ಕಿಟಕಿ = (ರಚನೆ ಕಿಟಕಿ *)id->id_Volume_ಗಂಟು) == ಖಾಲಿ)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	ಪದ_console = FALSE;
	ಹೋಗು ಹೊರಹೋಗು;
    }
    ಆದರೆ (oldwindowtitle == ಖಾಲಿ)
	oldwindowtitle = (char_u *)wb_ಕಿಟಕಿ->Title;
    ಆದರೆ (id->id_ಒಳ_Use == (BPTR)ಖಾಲಿ)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	ಹಿಂತಿರುಗಿ FAIL;
    }
    conUnit = (ರಚನೆ ConUnit *) ((ರಚನೆ IOStdReq *) id->id_ಒಳ_Use)->io_Unit;

    // get window size
    ಅಡ್ಡಸಾಲುಗಳು = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    ಆದರೆ (ಅಡ್ಡಸಾಲುಗಳು < 0 || ಅಡ್ಡಸಾಲುಗಳು > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	ಅಡ್ಡಸಾಲುಗಳು = 24;
	ಪದ_console = FALSE;
	ಹಿಂತಿರುಗಿ FAIL;
    }

    ಹಿಂತಿರುಗಿ OK;
ಹೊರಹೋಗು:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    ಹಿಂತಿರುಗಿ FAIL;
}
#ಆದರೆ_ಅಂತ್ಯ

/*
 * Try to set the real window size to Rows and Columns.
 */
    ಶೂನ್ಯ
mch_ಗಣ_shellsize(ಶೂನ್ಯ)
{
    ಆದರೆ (!ಪದ_console)
	ಹಿಂತಿರುಗಿ;

    ಗಾತ್ರ_ಗಣ = TRUE;
    ಹೊರಹೋಗು_char(CSI);
    ಹೊರಹೋಗು_ಅಂಕೆ((ದೀರ್ಘ)ಅಡ್ಡಸಾಲುಗಳು);
    ಹೊರಹೋಗು_char('t');
    ಹೊರಹೋಗು_char(CSI);
    ಹೊರಹೋಗು_ಅಂಕೆ((ದೀರ್ಘ)Columns);
    ಹೊರಹೋಗು_char('u');
    ಹೊರಹೋಗು_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    ಶೂನ್ಯ
mch_new_shellsize(ಶೂನ್ಯ)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ಸ್ಥಿರ ಶೂನ್ಯ
ಹೊರಹೋಗು_ಅಂಕೆ(ದೀರ್ಘ n)
{
    ಹೊರಹೋಗು_STR_NF(tltoa((ಚಿಹ್ನೆರಹಿತ ದೀರ್ಘ)n));
}

#ಆದರೆ !defined(AZTEC_C) && !defined(__AROS__) && !defined(__amigaos4__)
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

    ಸ್ಥಿರ ದೀರ್ಘ
dos_packet(
    ರಚನೆ ಸಂದೇಶ_ಪೋರ್ಟ್ *pid,    // process identifier ... (handlers message port)
    ದೀರ್ಘ	    action, // packet type ... (what you want handler to do)
    ದೀರ್ಘ	    arg)    // single argument
{
# ifdef FEAT_ARP
    ರಚನೆ ಸಂದೇಶ_ಪೋರ್ಟ್	    *replyport;
    ರಚನೆ StandardPacket   *packet;
    ದೀರ್ಘ		    res1;

    ಆದರೆ (dos2)
# endif
	ಹಿಂತಿರುಗಿ DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ರಚನೆ ಸಂದೇಶ_ಪೋರ್ಟ್ *) Create_ಪೋರ್ಟ್(ಖಾಲಿ, 0);	// use arp function
    ಆದರೆ (!replyport)
	ಹಿಂತಿರುಗಿ (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ರಚನೆ StandardPacket *)
	AllocMem((ದೀರ್ಘ) ಗಾತ್ರ(ರಚನೆ StandardPacket), MEMF_ಬಹಿರಂಗ | MEMF_CLEAR);
    ಆದರೆ (!packet)
    {
	Delete_ಪೋರ್ಟ್(replyport);
	ಹಿಂತಿರುಗಿ (0);
    }
    packet->sp_ಸಂದೇಶ.mn_ಗಂಟು.ln_Name = (ಅಕ್ಷರ *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_ಸಂದೇಶ);
    packet->sp_Pkt.dp_ಪೋರ್ಟ್ = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_ಸಂದೇಶ(pid, (ರಚನೆ ಸಂದೇಶ_4 *)packet);	// send packet

    Wait_ಪೋರ್ಟ್(replyport);
    Get_ಸಂದೇಶ(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (ದೀರ್ಘ) ಗಾತ್ರ(ರಚನೆ StandardPacket));
    Delete_ಪೋರ್ಟ್(replyport);

    ಹಿಂತಿರುಗಿ (res1);
# endif
}
#ಆದರೆ_ಅಂತ್ಯ // !defined(AZTEC_C) && !defined(__AROS__)

/*
 * Call shell.
 * Return error number for failure, 0 otherwise
 */
    ಪೂರ್ಣಾಂಕ
mch_call_shell(
    char_u	*cmd,
    ಪೂರ್ಣಾಂಕ		options)	// SHELL_*, see vim.h
{
    BPTR	mydir;
    ಪೂರ್ಣಾಂಕ		x;
    ಪೂರ್ಣಾಂಕ		tmode = cur_tmode;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ AZTEC_C
    ಪೂರ್ಣಾಂಕ		use_execute;
    char_u	*shellcmd = ಖಾಲಿ;
    char_u	*shellarg;
#ಆದರೆ_ಅಂತ್ಯ
    ಪೂರ್ಣಾಂಕ		retval = 0;

    ಆದರೆ (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	ಹಿಂತಿರುಗಿ -1;
    }

    ಆದರೆ (ಪದ_console)
	win_resize_off();	    // window resize events de-activated
    ಹೊರಹೋಗು_flush();

    ಆದರೆ (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = ಬೀಗ_2((UBYTE *)"", (ದೀರ್ಘ)ACCESS_READ);   // remember current dir

#ಆದರೆ !defined(AZTEC_C)		    // not tested very much
    ಆದರೆ (cmd == ಖಾಲಿ)
    {
# ifdef FEAT_ARP
	ಆದರೆ (dos2)
# endif
	    x = System_ಟ್ಯಾಗುಗಳು(p_sh, SYS_UserShell, TRUE, ಟ್ಯಾಗ್_ಮುಗಿದ);
# ifdef FEAT_ARP
	ಇಲ್ಲದಿದ್ದರೆ
	    x = Execute(p_sh, raw_ಒಳ, raw_ಹೊರಹೋಗು);
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
# ifdef FEAT_ARP
	ಆದರೆ (dos2)
# endif
	    x = System_ಟ್ಯಾಗುಗಳು((ಅಕ್ಷರ *)cmd, SYS_UserShell, TRUE, ಟ್ಯಾಗ್_ಮುಗಿದ);
# ifdef FEAT_ARP
	ಇಲ್ಲದಿದ್ದರೆ
	    x = Execute((ಅಕ್ಷರ *)cmd, 0L, raw_ಹೊರಹೋಗು);
# endif
    }
# ifdef FEAT_ARP
    ಆದರೆ ((dos2 && x < 0) || (!dos2 && !x))
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (x < 0)
# endif
    {
	ಸಂದೇಶ_puts(_("Cannot execute "));
	ಆದರೆ (cmd == ಖಾಲಿ)
	{
	    ಸಂದೇಶ_puts(_("shell "));
	    ಸಂದೇಶ_outtrans(p_sh);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    ಸಂದೇಶ_outtrans(cmd);
	ಸಂದೇಶ_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (!dos2 || x)
# ಇಲ್ಲದಿದ್ದರೆ
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (x)
# endif
    {
	ಆದರೆ ((x = IoErr()) != 0)
	{
	    ಆದರೆ (!(options & SHELL_SILENT))
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_outnum((ದೀರ್ಘ)x);
		ಸಂದೇಶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#ಇಲ್ಲದಿದ್ದರೆ	// else part is for AZTEC_C
    ಆದರೆ (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    ಇಲ್ಲದಿದ್ದರೆ
	use_execute = 0;
    ಆದರೆ (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	ಆದರೆ (shellcmd == ಖಾಲಿ)	    // out of memory, use Execute
	    use_execute = 1;
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    ಆದರೆ (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    ಆದರೆ (cmd == ಖಾಲಿ)
    {
	ಆದರೆ (use_execute)
	{
# ifdef FEAT_ARP
	    ಆದರೆ (dos2)
# endif
		x = System_ಟ್ಯಾಗುಗಳು((UBYTE *)p_sh, SYS_UserShell, TRUE, ಟ್ಯಾಗ್_ಮುಗಿದ);
# ifdef FEAT_ARP
	    ಇಲ್ಲದಿದ್ದರೆ
		x = !Execute((UBYTE *)p_sh, raw_ಒಳ, raw_ಹೊರಹೋಗು);
# endif
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    x = fexecl((ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellarg, ಖಾಲಿ);
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (use_execute)
    {
# ifdef FEAT_ARP
	ಆದರೆ (dos2)
# endif
	    x = System_ಟ್ಯಾಗುಗಳು((UBYTE *)cmd, SYS_UserShell, TRUE, ಟ್ಯಾಗ್_ಮುಗಿದ);
# ifdef FEAT_ARP
	ಇಲ್ಲದಿದ್ದರೆ
	    x = !Execute((UBYTE *)cmd, 0L, raw_ಹೊರಹೋಗು);
# endif
    }
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (p_st & 1)
	x = fexecl((ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellarg,
							   (ಅಕ್ಷರ *)cmd, ಖಾಲಿ);
    ಇಲ್ಲದಿದ್ದರೆ
	x = fexecl((ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellcmd, (ಅಕ್ಷರ *)shellarg,
					   (ಅಕ್ಷರ *)p_shcf, (ಅಕ್ಷರ *)cmd, ಖಾಲಿ);
# ifdef FEAT_ARP
    ಆದರೆ ((dos2 && x < 0) || (!dos2 && x))
# ಇಲ್ಲದಿದ್ದರೆ
    ಆದರೆ (x < 0)
# endif
    {
	ಸಂದೇಶ_puts(_("Cannot execute "));
	ಆದರೆ (use_execute)
	{
	    ಆದರೆ (cmd == ಖಾಲಿ)
		ಸಂದೇಶ_outtrans(p_sh);
	    ಇಲ್ಲದಿದ್ದರೆ
		ಸಂದೇಶ_outtrans(cmd);
	}
	ಇಲ್ಲದಿದ್ದರೆ
	{
	    ಸಂದೇಶ_puts(_("shell "));
	    ಸಂದೇಶ_outtrans(shellcmd);
	}
	ಸಂದೇಶ_putchar('\n');
	retval = -1;
    }
    ಇಲ್ಲದಿದ್ದರೆ
    {
	ಆದರೆ (use_execute)
	{
# ifdef FEAT_ARP
	    ಆದರೆ (!dos2 || x)
# ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (x)
# endif
		x = IoErr();
	}
	ಇಲ್ಲದಿದ್ದರೆ
	    x = wait();
	ಆದರೆ (x)
	{
	    ಆದರೆ (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		ಸಂದೇಶ_putchar('\n');
		ಸಂದೇಶ_outnum((ದೀರ್ಘ)x);
		ಸಂದೇಶ_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#ಆದರೆ_ಅಂತ್ಯ	// AZTEC_C

    ಆದರೆ ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_ಬೀಗ(mydir);
    ಆದರೆ (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    ಆದರೆ (ಪದ_console)
	win_resize_on();		// window resize events activated
    ಹಿಂತಿರುಗಿ retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    ಶೂನ್ಯ
mch_breakcheck(ಪೂರ್ಣಾಂಕ force ಬಳಸದ)
{
   ಆದರೆ (ಗಣ_Signal(0L, (ದೀರ್ಘ)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ _DCC
# define Chk_Abort chkabort
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ LATTICE
ಶೂನ್ಯ __regargs __chkabort(ಶೂನ್ಯ);

ಶೂನ್ಯ __regargs __chkabort(ಶೂನ್ಯ)
{}

#ಇಲ್ಲದಿದ್ದರೆ
    ದೀರ್ಘ
Chk_Abort(ಶೂನ್ಯ)
{
    ಹಿಂತಿರುಗಿ(0L);
}
#ಆದರೆ_ಅಂತ್ಯ

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

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
# define	ANCHOR_BUF_ಗಾತ್ರ	1024
#ಇಲ್ಲದಿದ್ದರೆ
# define ANCHOR_BUF_ಗಾತ್ರ (512)
# define ANCHOR_ಗಾತ್ರ (ಗಾತ್ರ(ರಚನೆ Anchor_ಮಾರ್ಗ) + ANCHOR_BUF_ಗಾತ್ರ)
#ಆದರೆ_ಅಂತ್ಯ

    ಪೂರ್ಣಾಂಕ
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    ಪೂರ್ಣಾಂಕ		flags)		// EW_* flags
{
    ರಚನೆ Anchor_ಮಾರ್ಗ	*Anchor;
    LONG		ಫಲಿತಾಂಶ_2;
    char_u		*starbuf, *sp, *dp;
    ಪೂರ್ಣಾಂಕ			ಆರಂಭ_len;
    ಪೂರ್ಣಾಂಕ			matches;
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    ರಚನೆ ಟ್ಯಾಗ್_ಅಂಶ	Anchor_ಟ್ಯಾಗುಗಳು[] = {
	{ADO_Strlen, ANCHOR_BUF_ಗಾತ್ರ},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{ಟ್ಯಾಗ್_ಮುಗಿದ, 0L}
    };
#ಆದರೆ_ಅಂತ್ಯ

    ಆರಂಭ_len = gap->ga_len;

    // Get our AnchorBase
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_ಟ್ಯಾಗುಗಳು);
#ಇಲ್ಲದಿದ್ದರೆ
    Anchor = alloc_clear(ANCHOR_ಗಾತ್ರ);
#ಆದರೆ_ಅಂತ್ಯ
    ಆದರೆ (Anchor == ಖಾಲಿ)
	ಹಿಂತಿರುಗಿ 0;

#ವ್ಯಾಖ್ಯಾನಿತವಾಗದಿದ್ದರೆ __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_ಗಾತ್ರ;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# ಇಲ್ಲದಿದ್ದರೆ
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#ಆದರೆ_ಅಂತ್ಯ

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (dos2)
    {
#ಆದರೆ_ಅಂತ್ಯ
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	ಆದರೆ (starbuf == ಖಾಲಿ)
	    ಹೋಗು Return;
	ಗಾಗಿ (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    ಆದರೆ (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    ಇಲ್ಲದಿದ್ದರೆ
		*dp++ = *sp;
	}
	*dp = NUL;
	ಫಲಿತಾಂಶ_2 = ಹೊಂದಿಕೆ_ಮೊದಲ((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    }
    ಇಲ್ಲದಿದ್ದರೆ
	ಫಲಿತಾಂಶ_2 = ಕಂಡುಹಿಡಿ_ಮೊದಲ((ಅಕ್ಷರ *)pat, Anchor);
#ಆದರೆ_ಅಂತ್ಯ

    /*
     * Loop to get all matches.
     */
    ಆಗ (ಫಲಿತಾಂಶ_2 == 0)
    {
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_ಬಫರ್, flags);
#ಇಲ್ಲದಿದ್ದರೆ
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#ಆದರೆ_ಅಂತ್ಯ
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	ಆದರೆ (dos2)
#ಆದರೆ_ಅಂತ್ಯ
	    ಫಲಿತಾಂಶ_2 = ಹೊಂದಿಕೆ_ಮುಂದಿನ_2(Anchor);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
	ಇಲ್ಲದಿದ್ದರೆ
	    ಫಲಿತಾಂಶ_2 = ಕಂಡುಹಿಡಿ_ಮುಂದಿನ(Anchor);
#ಆದರೆ_ಅಂತ್ಯ
    }
    matches = gap->ga_len - ಆರಂಭ_len;

    ಆದರೆ (ಫಲಿತಾಂಶ_2 == ERROR_ಬಫರ್_ಅತಿಪ್ರವಾಹ)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (matches == 0 && ಫಲಿತಾಂಶ_2 != ERROR_OBJECT_NOT_ಸಿಕ್ಕಿತು
			  && ಫಲಿತಾಂಶ_2 != ERROR_DEVICE_NOT_MOUNTED
			  && ಫಲಿತಾಂಶ_2 != ERROR_NO_ಇನ್ನಷ್ಟು_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    ಆದರೆ (matches)
	qsort((ಶೂನ್ಯ *)(((char_u **)gap->ga_ದತ್ತಾಂಶ) + ಆರಂಭ_len),
				  (size_t)matches, ಗಾತ್ರ(char_u *), sortcmp);

    // Free the wildcard stuff
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (dos2)
#ಆದರೆ_ಅಂತ್ಯ
	ಹೊಂದಿಕೆ_ಕೊನೆ_3(Anchor);
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಇಲ್ಲದಿದ್ದರೆ
	FreeAnchorChain(Anchor);
#ಆದರೆ_ಅಂತ್ಯ

Return:
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#ಇಲ್ಲದಿದ್ದರೆ
    vim_free(Anchor);
#ಆದರೆ_ಅಂತ್ಯ

    ಹಿಂತಿರುಗಿ matches;
}

    ಸ್ಥಿರ ಪೂರ್ಣಾಂಕ
sortcmp(ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *a, ಸ್ಥಿರಾಂಕ ಶೂನ್ಯ *b)
{
    ಅಕ್ಷರ *s = *(ಅಕ್ಷರ **)a;
    ಅಕ್ಷರ *t = *(ಅಕ್ಷರ **)b;

    ಹಿಂತಿರುಗಿ pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    ಪೂರ್ಣಾಂಕ
mch_has_exp_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ ಆದರೆ (vim_strchr((char_u *)"*?[(#", *p) != ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
}

    ಪೂರ್ಣಾಂಕ
mch_has_wildcard(char_u *p)
{
    ಗಾಗಿ ( ; *p; MB_PTR_ADV(p))
    {
	ಆದರೆ (*p == '\\' && p[1] != NUL)
	    ++p;
	ಇಲ್ಲದಿದ್ದರೆ
	    ಆದರೆ (vim_strchr((char_u *)"*?[(#$`", *p) != ಖಾಲಿ
		    || (*p == '~' && p[1] != NUL))
		ಹಿಂತಿರುಗಿ TRUE;
    }
    ಹಿಂತಿರುಗಿ FALSE;
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
    ಪೂರ್ಣಾಂಕ		    len;
    UBYTE	    *buf;		// buffer to expand in
    char_u	    *retval;		// return value
    ಸ್ಥಿರ char_u   *alloced = ಖಾಲಿ;	// allocated memory

#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (!dos2)
	retval = (char_u *)getenv((ಅಕ್ಷರ *)var);
    ಇಲ್ಲದಿದ್ದರೆ
#ಆದರೆ_ಅಂತ್ಯ
    {
	VIM_CLEAR(alloced);
	retval = ಖಾಲಿ;

	buf = alloc(IOSIZE);
	ಆದರೆ (buf == ಖಾಲಿ)
	    ಹಿಂತಿರುಗಿ ಖಾಲಿ;

	len = GetVar((UBYTE *)var, buf, (ದೀರ್ಘ)(IOSIZE - 1), (ದೀರ್ಘ)0);
	ಆದರೆ (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    ಆದರೆ (retval == ಖಾಲಿ && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    ಹಿಂತಿರುಗಿ retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    ಪೂರ್ಣಾಂಕ
mch_setenv(ಅಕ್ಷರ *var, ಅಕ್ಷರ *ಮೌಲ್ಯ, ಪೂರ್ಣಾಂಕ x ಬಳಸದ)
{
#ವ್ಯಾಖ್ಯಾನಿತವಾದರೆ FEAT_ARP
    ಆದರೆ (!dos2)
	ಹಿಂತಿರುಗಿ setenv(var, ಮೌಲ್ಯ);
#ಆದರೆ_ಅಂತ್ಯ

    ಆದರೆ (ಗಣ_Var((UBYTE *)var, (UBYTE *)ಮೌಲ್ಯ, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	ಹಿಂತಿರುಗಿ 0;   // success
    ಹಿಂತಿರುಗಿ -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    ಪೂರ್ಣಾಂಕ
mch_get_random(char_u *buf, ಪೂರ್ಣಾಂಕ len)
{
    ರಚನೆ Process *proc = (ರಚನೆ Process *) ಕಂಡುಹಿಡಿ_Task(0L);
    APTR win = proc->pr_ಕಿಟಕಿ_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_ಕಿಟಕಿ_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", ಬಹುಲಕ_OLDFILE);

    proc->pr_ಕಿಟಕಿ_Ptr = win;

    ಪೂರ್ಣಾಂಕ ಸ್ಥಿತಿ_2;

    ಆದರೆ (!fh || Read(fh, buf, len) != len)
	ಸ್ಥಿತಿ_2 = FAIL;
    ಇಲ್ಲದಿದ್ದರೆ
	ಸ್ಥಿತಿ_2 = OK;

    Close(fh);
    ಹಿಂತಿರುಗಿ ಸ್ಥಿತಿ_2;
}
