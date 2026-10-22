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

#که_تعریف_شوی کړکۍ
# undef کړکۍ	// Amiga has its own Window definition
#د_که_پای

#تعریف_لغوه TRUE		// will be redefined by exec/types.h
#تعریف_لغوه FALSE

#که_نه_تعریف_شوی LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#د_که_پای

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
#که_تعریف_شوی __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#وإلا
# define	free_fib(x) vim_free(fib)
#د_که_پای

#إذا defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#د_که_پای

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#إذا defined(__amigaos4__)
ساكن ثابت حرف* __attribute__((کارول_شوی)) stackcookie = "$STACK: 1048576";
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
بینښه اوږد __stack = 1048576;
#وإلاإذا defined(__GNUC__) && defined(AMIGA)
بینښه اوږد __stack = 262144;
#د_که_پای

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#تعریف_لغوه	TRUE
#تعریف_کړه TRUE (1)
#تعریف_لغوه	FALSE
#تعریف_کړه FALSE (0)

#که_تعریف_شوی __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#وإلاإذا !defined(AZTEC_C) && !defined(__AROS__)
ساكن اوږد dos_packet(بنية پیغام_پورټ *, اوږد, اوږد);
#د_که_پای
ساكن صحيح lock2name(BPTR قلف, char_u *buf, اوږد   len);
ساكن فراغ بهر_عدد(اوږد n);
ساكن بنية File_Info_بلاک *get_fib(char_u *);
ساكن صحيح sortcmp(ثابت فراغ *a, ثابت فراغ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ساكن BPTR
خوندي_قلف(UBYTE *name, اوږد ډېرپېښ)
{
    بنية Process *me = (بنية Process *)پیدا_کړه_Task(عدم);
    APTR oldwin = me->pr_کړکۍ_Ptr;
    BPTR flock;

    me->pr_کړکۍ_Ptr = (APTR)-1L;
    flock = قلف_2(name, ډېرپېښ);
    me->pr_کړکۍ_Ptr = oldwin;
    أرجع flock;
}

ساكن BPTR		raw_دننه = (BPTR)عدم;
ساكن BPTR		raw_بهر = (BPTR)عدم;
ساكن صحيح		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
بنية Intuition_بنسټ	*Intuition_بنسټ = عدم;
#د_که_پای
#که_تعریف_شوی FEAT_ARP
بنية Arp_بنسټ		*Arp_بنسټ = عدم;
#د_که_پای

ساكن بنية کړکۍ	*wb_کړکۍ;
ساكن char_u		*oldwindowtitle = عدم;

#که_تعریف_شوی FEAT_ARP
صحيح			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#د_که_پای
صحيح			اندازه_سېټ = FALSE;   // set to TRUE if window size was set

#که_تعریف_شوی __GNUC__
ساكن حرف نسخه_2[] __attribute__((کارول_شوی)) =
    "\0$VER: Vim "
    VIM_نسخه_MAJOR_STR "."
    VIM_نسخه_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#د_که_پای

    فراغ
win_resize_on(فراغ)
{
    بهر_STR_NF("\033[12{");
}

    فراغ
win_resize_off(فراغ)
{
    بهر_STR_NF("\033[12}");
}

    فراغ
mch_write(char_u *p, صحيح len)
{
    Write(raw_بهر, (حرف *)p, (اوږد)len);
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
    اوږد    وخت,		// milliseconds
    صحيح	    tb_change_cnt ناکارول_شوی)
{
    صحيح	    len;
    اوږد    utime;

    إذا (وخت >= 0)
    {
	إذا (وخت == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	وإلا
	    utime = وخت * 1000L;   // convert from milli to micro secs
	إذا (WaitForChar(raw_دننه, utime) == 0)	// no character available
	    أرجع 0;
    }
    وإلا    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	إذا (WaitForChar(raw_دننه, p_ut * 1000L) == 0)
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
	len = Read(raw_دننه, (حرف *)buf, (اوږد)maxlen / ننوت_conv.vc_ضریب);
	إذا (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    إذا (ننوت_conv.vc_type != CONV_NONE)
		len = convert_ننوت(buf, len, maxlen);
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
    أرجع (WaitForChar(raw_دننه, 100L) != 0);
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
    أرجع (long_u)(AvailMem(special ? (اوږد)MEMF_CHIP : (اوږد)MEMF_ANY)) >> 10;
#د_که_پای
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    فراغ
mch_delay(اوږد msec, صحيح flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#که_تعریف_شوی LATTICE
    فراغ	    Delay(اوږد);
#د_که_پای

    إذا (msec <= 0)
	أرجع;

    إذا (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    وإلا
	WaitForChar(raw_دننه, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    suspend_shell();
}

#که_نه_تعریف_شوی DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#د_که_پای

    فراغ
mch_init(فراغ)
{
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساكن حرف	    intlibname[] = "intuition.library";
#د_که_پای

#که_تعریف_شوی AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#د_که_پای

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	بنية Process *me = (بنية Process *)پیدا_کړه_Task(عدم);
	me->pr_کړکۍ_Ptr = (APTR)-1L;
    }

    Columns = 80;
    لیکې = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    إذا (raw_دننه == (BPTR)عدم)
    {
	raw_دننه = ننوت_3();
	raw_بهر = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	إذا (!IsInteractive(raw_دننه))
	    raw_دننه = raw_بهر;
	وإلا إذا (!IsInteractive(raw_بهر))
	    raw_بهر = raw_دننه;
    }

    بهر_flush();

    wb_کړکۍ = عدم;
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    إذا ((Intuition_بنسټ = (بنية Intuition_بنسټ *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == عدم)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#د_که_پای
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
#تعریف_کړه BUF2_اندازه 320	    // length of buffer for argument with complete path

    صحيح
mch_check_win(صحيح argc, حرف **argv)
{
    صحيح		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_اندازه];
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
    بنية Library  *Dos_بنسټ;
#د_که_پای
    صحيح		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_بنسټ = OpenLibrary(DOS_LIBRARY, 37L);
    إذا (Dos_بنسټ != عدم)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_بنسټ);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    وإلا	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	وتل(3);
# وإلا
		    // need arp functions for dos 1.x
	إذا (!(Arp_بنسټ = (بنية Arp_بنسټ *) OpenLibrary((UBYTE *)ArpName, Arp_نسخه)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_نسخه);
	    وتل(3);
	}
# endif
    }
#د_که_پای	/* __amigaos4__ __AROS__ __MORPHOS__ */

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
#که_تعریف_شوی FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#د_که_پای
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
	    && (IsInteractive(ننوت_3()) || IsInteractive(Output())))
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
	    إذا (device != عدم && (raw_دننه = Open((UBYTE *)device,
					   (اوږد)ډېرپېښ_NEWFILE)) != (BPTR)عدم)
		اكسر;
	}
	إذا (raw_دننه == (BPTR)عدم)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    اذهب وتل;
	}
	raw_بهر = raw_دننه;
	close_win = TRUE;
	أرجع OK;
    }

    إذا ((nilfh = Open((UBYTE *)"NIL:", (اوږد)ډېرپېښ_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	اذهب وتل;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((حرف *)buf1, "t:nc%p", پیدا_کړه_Task(0));
#وإلا
    sprintf((حرف *)buf1, "t:nc%ld", (اوږد)buf1);
#د_که_پای
    إذا ((fh = Open((UBYTE *)buf1, (اوږد)ډېرپېښ_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((حرف *)buf1);
	mch_errmsg("\n");
	اذهب وتل;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    إذا (argc == 0)	// run from workbench
	ac = ((بنية WBStartup *)argv)->sm_عدد_دلیلونه;
    وإلا
	ac = argc;
    لكل (i = 0; i < ac; ++i)
    {
	إذا (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((بنية WBStartup *)argv)->sm_ArgList[i]);
	    إذا (argp->wa_قلف)
		(فراغ)lock2name(argp->wa_قلف, buf2, (اوږد)(BUF2_اندازه - 1));
#که_تعریف_شوی FEAT_ARP
	    إذا (dos2)	    // use 2.0 function
#د_که_پای
		جمع_کړه_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (اوږد)(BUF2_اندازه - 1));
#که_تعریف_شوی FEAT_ARP
	    وإلا	    // use arp function
		TackOn((حرف *)buf2, argp->wa_Name);
#د_که_پای
	    av = (حرف *)buf2;
	}
	وإلا
	    av = argv[i];

	// skip '-d' or "-dev" option
	إذا (av[0] == '-' && av[1] == 'd'
#که_تعریف_شوی FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#د_که_پای
		)
	{
	    ++i;
	    استمر;
	}
	إذا (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (اوږد)مزي_اوږدوالی(av));
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
#که_تعریف_شوی FEAT_ARP
	إذا (dos2)
	{
#د_که_پای
	    إذا (!System_نښانونه((UBYTE *)buf2, SYS_UserShell, TRUE, نښان_بشپړ))
		اكسر;
#که_تعریف_شوی FEAT_ARP
	}
	وإلا
	{
	    إذا (Execute((UBYTE *)buf2, nilfh, nilfh))
		اكسر;
	}
#د_که_پای
    }
    إذا (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	اذهب وتل;
    }
    exitval = 0;    // The Execute succeeded: exit this program

وتل:
    إذا (nilfh)
	Close(nilfh);
#که_تعریف_شوی FEAT_ARP
    إذا (Arp_بنسټ)
	CloseLibrary((بنية Library *) Arp_بنسټ);
#د_که_پای
    وتل(exitval);
    // NOTREACHED
    أرجع FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    صحيح
mch_ننوت_isatty(فراغ)
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
    صحيح		len ناکارول_شوی)		// buffer size, ignored here
{
    بنية File_Info_بلاک    *fib;
    size_t		    flen;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#که_تعریف_شوی __amigaos4__
    إذا (fib->fib_Dir_ثبت_Type == ST_ریښه)
	strcat(fib->fib_FileName, ":");
#د_که_پای
    إذا (flen == مزي_اوږدوالی(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ساكن بنية File_Info_بلاک *
get_fib(char_u *fname)
{
    BPTR		    flock;
    بنية File_Info_بلاک    *fib;

    إذا (fname == عدم)	    // safety check
	أرجع عدم;
#که_تعریف_شوی __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#وإلا
    fib = ALLOC_ONE(بنية File_Info_بلاک);
#د_که_پای
    إذا (fib == عدم)
	أرجع;

    flock = خوندي_قلف((UBYTE *)fname, (اوږد)ACCESS_READ);
    إذا (flock == (BPTR)عدم || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = عدم;
    }
    إذا (flock)
	Un_قلف(flock);
    أرجع fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    فراغ
mch_settitle(char_u *title, char_u *icon)
{
    إذا (wb_کړکۍ != عدم && title != عدم)
	سېټ_کړکۍ_Titles(wb_کړکۍ, (UBYTE *)title, (UBYTE *)-1L);
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
    أرجع (wb_کړکۍ != عدم);
}

    صحيح
mch_can_restore_icon(فراغ)
{
    أرجع FALSE;
}

    فراغ
mch_setmouse(صحيح on ناکارول_شوی)
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
#د_که_پای
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
#د_که_پای
}

/*
 * return process ID
 */
    اوږد
mch_get_pid(فراغ)
{
#إذا defined(__amigaos4__)
    أرجع (اوږد) getpid();
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    أرجع((اوږد)پیدا_کړه_Task(0));
#وإلا
    أرجع (اوږد)0;
#د_که_پای
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(char_u *buf, صحيح len)
{
    أرجع mch_ډک_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
mch_ډک_Name(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)
{
    BPTR	l;
    صحيح		retval = FAIL;
    صحيح		i;

    // Lock the file.  If it exists, we can get the exact name.
    إذا ((l = خوندي_قلف((UBYTE *)fname, (اوږد)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (اوږد)len - 1);
	Un_قلف(l);
    }
    وإلا إذا (force || !mch_is_ډک_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	إذا ((l = قلف_2((UBYTE *)"", (اوږد)ACCESS_READ)) != (BPTR)عدم)
	{
	    retval = lock2name(l, buf, (اوږد)len);
	    Un_قلف(l);
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
mch_is_ډک_Name(char_u *fname)
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
lock2name(BPTR قلف, char_u *buf, اوږد len)
{
#که_تعریف_شوی FEAT_ARP
    إذا (dos2)		    // use 2.0 function
#د_که_پای
	أرجع ((صحيح)Name_From_قلف(قلف, (UBYTE *)buf, len) ? OK : FAIL);
#که_تعریف_شوی FEAT_ARP
    وإلا		// use arp function
	أرجع ((صحيح)لار_Name(قلف, (حرف *)buf, (اوږد)(len/32)) ? OK : FAIL);
#د_که_پای
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    اوږد
mch_getperm(char_u *name)
{
    بنية File_Info_بلاک    *fib;
    اوږد		    retval = -1;

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
mch_setperm(char_u *name, اوږد perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    أرجع (سېټ_Protection((UBYTE *)name, (اوږد)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name ناکارول_شوی)
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
    بنية File_Info_بلاک    *fib;
    صحيح			    retval = FALSE;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع FALSE;

#که_تعریف_شوی __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#وإلا
    retval = ((fib->fib_Dir_ثبت_Type >= 0) ? TRUE : FALSE);
#د_که_پای
    free_fib(fib);
    أرجع retval;
}

/*
 * Create directory "name".
 */
    صحيح
mch_mkdir(char_u *name)
{
    BPTR	قلف;

    قلف = CreateDir(name);
    إذا (قلف == عدم)
	أرجع -1;

    Un_قلف(قلف);
    أرجع 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحيح
mch_can_exe(char_u *name, char_u **لار ناکارول_شوی, صحيح use_لار)
{
    صحيح exe = -1;
#که_تعریف_شوی __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = بارول_Seg(name);

    إذا (seg && Get_Seg_List_Info_نښانونه(seg, GSLI_Native, عدم, نښان_بشپړ) !=
	    Get_Seg_List_Info_نښانونه(seg, GSLI_68KHUNK, عدم, نښان_بشپړ))
    {
	// Test if file permissions allow execution.
	بنية Examine_معلومات *exd = Examine_Object_نښانونه(EX_String_Name_ننوت, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    وإلا
    {
	exe = 0;
    }

    Un_بارول_Seg(seg);

    // Search for executable in path if applicable.
    إذا (!exe && use_لار)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	بنية لار_غوټه *سر = Dup_Cmd_لار_List(عدم);

	// For each entry, recur to check for executable.
	لكل (بنية لار_غوټه *لکۍ = سر; !exe && لکۍ;
			       لکۍ = (بنية لار_غوټه *) BADDR(لکۍ->pn_بل))
	{
	    سېټ_Current_Dir(لکۍ->pn_قلف);
	    exe = mch_can_exe(name, لار, 0);
	}

	// Go back to where we were.
	Free_Cmd_لار_List(سر);
	سېټ_Current_Dir(cwd);
    }
#د_که_پای
    أرجع exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name ناکارول_شوی)
{
    // TODO
    أرجع غوټه_نورمال;
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

    إذا (raw_دننه)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    بهر_char('\n');
    إذا (raw_بهر)
    {
	إذا (غړی_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    إذا (اندازه_سېټ)
		بهر_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	بهر_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#که_تعریف_شوی FEAT_ARP
    إذا (Arp_بنسټ)
	CloseLibrary((بنية Library *) Arp_بنسټ);
#د_که_پای
    إذا (close_win)
	Close(raw_دننه);
    إذا (r)
	چاپ_کړه(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    وتل(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#تعریف_کړه MP(xx)	((بنية پیغام_پورټ *)((بنية FileHandle *) (BADDR(xx)))->fh_Type)

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
    إذا (!سېټ_ډېرپېښ_2(raw_دننه, tmode == TMODE_RAW ? 1 : 0))
#وإلا
    إذا (dos_packet(MP(raw_دننه), (اوږد)ACTION_SCREEN_ډېرپېښ,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#د_که_پای
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
    إذا (!غړی_console)
	أرجع FAIL;

    إذا (raw_دننه && raw_بهر)
    {
	// Save current console mode.
	صحيح old_tmode = cur_tmode;
	حرف ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	إذا (Write(raw_بهر, ctrl, حجم(ctrl)) == حجم(ctrl))
	{
	    حرف لټون[] = "\x9b""1;1;%d;%d r",
		 answ[حجم(لټون) + 8] = { '\0' };

	    // Read return sequence from input.
	    إذا (Read(raw_دننه, answ, حجم(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		إذا (له_مزي_ولوله(answ, لټون, &لیکې, &Columns) == 2)
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
    غړی_console = FALSE;
    Columns = 80;
    لیکې = 24;

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
    حرف	    id_a[حجم(بنية Info_معلومات) + 3];
# endif
    بنية Info_معلومات *id=0;

    إذا (!غړی_console)	// not an amiga window
	اذهب بهر;

    // insure longword alignment
# ifdef __amigaos4__
    إذا (!(id = AllocDosObject(DOS_INFODATA, 0)))
	اذهب بهر;
# وإلا
    id = (بنية Info_معلومات *)(((اوږد)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    إذا (اندازه_سېټ)
	بهر_STR("\233t\233u");	// CSI t CSI u
    بهر_flush();

    إذا (dos_packet(MP(raw_بهر), (اوږد)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_کړکۍ = (بنية کړکۍ *)id->id_Volume_غوټه) == عدم)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	غړی_console = FALSE;
	اذهب بهر;
    }
    إذا (oldwindowtitle == عدم)
	oldwindowtitle = (char_u *)wb_کړکۍ->Title;
    إذا (id->id_دننه_Use == (BPTR)عدم)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	أرجع FAIL;
    }
    conUnit = (بنية ConUnit *) ((بنية IOStdReq *) id->id_دننه_Use)->io_Unit;

    // get window size
    لیکې = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    إذا (لیکې < 0 || لیکې > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	لیکې = 24;
	غړی_console = FALSE;
	أرجع FAIL;
    }

    أرجع OK;
بهر:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    أرجع FAIL;
}
#د_که_پای

/*
 * Try to set the real window size to Rows and Columns.
 */
    فراغ
mch_سېټ_shellsize(فراغ)
{
    إذا (!غړی_console)
	أرجع;

    اندازه_سېټ = TRUE;
    بهر_char(CSI);
    بهر_عدد((اوږد)لیکې);
    بهر_char('t');
    بهر_char(CSI);
    بهر_عدد((اوږد)Columns);
    بهر_char('u');
    بهر_flush();
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
بهر_عدد(اوږد n)
{
    بهر_STR_NF(tltoa((بینښه اوږد)n));
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

    ساكن اوږد
dos_packet(
    بنية پیغام_پورټ *pid,    // process identifier ... (handlers message port)
    اوږد	    action, // packet type ... (what you want handler to do)
    اوږد	    arg)    // single argument
{
# ifdef FEAT_ARP
    بنية پیغام_پورټ	    *replyport;
    بنية StandardPacket   *packet;
    اوږد		    res1;

    إذا (dos2)
# endif
	أرجع DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (بنية پیغام_پورټ *) Create_پورټ(عدم, 0);	// use arp function
    إذا (!replyport)
	أرجع (0);

    // Allocate space for a packet, make it public and clear it
    packet = (بنية StandardPacket *)
	AllocMem((اوږد) حجم(بنية StandardPacket), MEMF_عامه | MEMF_CLEAR);
    إذا (!packet)
    {
	Delete_پورټ(replyport);
	أرجع (0);
    }
    packet->sp_پیغام.mn_غوټه.ln_Name = (حرف *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_پیغام);
    packet->sp_Pkt.dp_پورټ = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_پیغام(pid, (بنية پیغام_4 *)packet);	// send packet

    Wait_پورټ(replyport);
    Get_پیغام(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (اوږد) حجم(بنية StandardPacket));
    Delete_پورټ(replyport);

    أرجع (res1);
# endif
}
#د_که_پای // !defined(AZTEC_C) && !defined(__AROS__)

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
#که_تعریف_شوی AZTEC_C
    صحيح		use_execute;
    char_u	*shellcmd = عدم;
    char_u	*shellarg;
#د_که_پای
    صحيح		retval = 0;

    إذا (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	أرجع -1;
    }

    إذا (غړی_console)
	win_resize_off();	    // window resize events de-activated
    بهر_flush();

    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = قلف_2((UBYTE *)"", (اوږد)ACCESS_READ);   // remember current dir

#إذا !defined(AZTEC_C)		    // not tested very much
    إذا (cmd == عدم)
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = System_نښانونه(p_sh, SYS_UserShell, TRUE, نښان_بشپړ);
# ifdef FEAT_ARP
	وإلا
	    x = Execute(p_sh, raw_دننه, raw_بهر);
# endif
    }
    وإلا
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = System_نښانونه((حرف *)cmd, SYS_UserShell, TRUE, نښان_بشپړ);
# ifdef FEAT_ARP
	وإلا
	    x = Execute((حرف *)cmd, 0L, raw_بهر);
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
		پیغام_outnum((اوږد)x);
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
		x = System_نښانونه((UBYTE *)p_sh, SYS_UserShell, TRUE, نښان_بشپړ);
# ifdef FEAT_ARP
	    وإلا
		x = !Execute((UBYTE *)p_sh, raw_دننه, raw_بهر);
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
	    x = System_نښانونه((UBYTE *)cmd, SYS_UserShell, TRUE, نښان_بشپړ);
# ifdef FEAT_ARP
	وإلا
	    x = !Execute((UBYTE *)cmd, 0L, raw_بهر);
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
		پیغام_outnum((اوږد)x);
		پیغام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#د_که_پای	// AZTEC_C

    إذا ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_قلف(mydir);
    إذا (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    إذا (غړی_console)
	win_resize_on();		// window resize events activated
    أرجع retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    فراغ
mch_breakcheck(صحيح force ناکارول_شوی)
{
   إذا (سېټ_Signal(0L, (اوږد)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#که_تعریف_شوی _DCC
# define Chk_Abort chkabort
#د_که_پای

#که_تعریف_شوی LATTICE
فراغ __regargs __chkabort(فراغ);

فراغ __regargs __chkabort(فراغ)
{}

#وإلا
    اوږد
Chk_Abort(فراغ)
{
    أرجع(0L);
}
#د_که_پای

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

#که_تعریف_شوی __amigaos4__
# define	ANCHOR_BUF_اندازه	1024
#وإلا
# define ANCHOR_BUF_اندازه (512)
# define ANCHOR_اندازه (حجم(بنية Anchor_لار) + ANCHOR_BUF_اندازه)
#د_که_پای

    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    صحيح		flags)		// EW_* flags
{
    بنية Anchor_لار	*Anchor;
    LONG		پایله_2;
    char_u		*starbuf, *sp, *dp;
    صحيح			پیل_len;
    صحيح			matches;
#که_تعریف_شوی __amigaos4__
    بنية نښان_توکی	Anchor_نښانونه[] = {
	{ADO_Strlen, ANCHOR_BUF_اندازه},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{نښان_بشپړ, 0L}
    };
#د_که_پای

    پیل_len = gap->ga_len;

    // Get our AnchorBase
#که_تعریف_شوی __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_نښانونه);
#وإلا
    Anchor = alloc_clear(ANCHOR_اندازه);
#د_که_پای
    إذا (Anchor == عدم)
	أرجع 0;

#که_نه_تعریف_شوی __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_اندازه;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# وإلا
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#د_که_پای

#که_تعریف_شوی FEAT_ARP
    إذا (dos2)
    {
#د_که_پای
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
	پایله_2 = سمون_لومړی((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#که_تعریف_شوی FEAT_ARP
    }
    وإلا
	پایله_2 = پیدا_کړه_لومړی((حرف *)pat, Anchor);
#د_که_پای

    /*
     * Loop to get all matches.
     */
    طالما (پایله_2 == 0)
    {
#که_تعریف_شوی __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_بفر, flags);
#وإلا
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#د_که_پای
#که_تعریف_شوی FEAT_ARP
	إذا (dos2)
#د_که_پای
	    پایله_2 = سمون_بل_2(Anchor);
#که_تعریف_شوی FEAT_ARP
	وإلا
	    پایله_2 = پیدا_کړه_بل(Anchor);
#د_که_پای
    }
    matches = gap->ga_len - پیل_len;

    إذا (پایله_2 == ERROR_بفر_ډکېدنه)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    وإلا إذا (matches == 0 && پایله_2 != ERROR_OBJECT_NOT_وموندل
			  && پایله_2 != ERROR_DEVICE_NOT_MOUNTED
			  && پایله_2 != ERROR_NO_نور_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    إذا (matches)
	qsort((فراغ *)(((char_u **)gap->ga_معلومات) + پیل_len),
				  (size_t)matches, حجم(char_u *), sortcmp);

    // Free the wildcard stuff
#که_تعریف_شوی FEAT_ARP
    إذا (dos2)
#د_که_پای
	سمون_پای_ټکی_3(Anchor);
#که_تعریف_شوی FEAT_ARP
    وإلا
	FreeAnchorChain(Anchor);
#د_که_پای

Return:
#که_تعریف_شوی __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#وإلا
    vim_free(Anchor);
#د_که_پای

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

#که_تعریف_شوی FEAT_ARP
    إذا (!dos2)
	retval = (char_u *)getenv((حرف *)var);
    وإلا
#د_که_پای
    {
	VIM_CLEAR(alloced);
	retval = عدم;

	buf = alloc(IOSIZE);
	إذا (buf == عدم)
	    أرجع عدم;

	len = GetVar((UBYTE *)var, buf, (اوږد)(IOSIZE - 1), (اوږد)0);
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
mch_setenv(حرف *var, حرف *ارزښت, صحيح x ناکارول_شوی)
{
#که_تعریف_شوی FEAT_ARP
    إذا (!dos2)
	أرجع setenv(var, ارزښت);
#د_که_پای

    إذا (سېټ_Var((UBYTE *)var, (UBYTE *)ارزښت, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
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
    بنية Process *proc = (بنية Process *) پیدا_کړه_Task(0L);
    APTR win = proc->pr_کړکۍ_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_کړکۍ_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", ډېرپېښ_OLDFILE);

    proc->pr_کړکۍ_Ptr = win;

    صحيح حالت;

    إذا (!fh || Read(fh, buf, len) != len)
	حالت = FAIL;
    وإلا
	حالت = OK;

    Close(fh);
    أرجع حالت;
}
