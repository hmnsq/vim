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

#اگر_تعریف_شده پنجره
# undef پنجره	// Amiga has its own Window definition
#پایان_اگر

#لغو_تعریف TRUE		// will be redefined by exec/types.h
#لغو_تعریف FALSE

#اگر_تعریف_نشده LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#پایان_اگر

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
#اگر_تعریف_شده __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#وإلا
# define	free_fib(x) vim_free(fib)
#پایان_اگر

#إذا defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#پایان_اگر

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#إذا defined(__amigaos4__)
ساكن ثابت حرف* __attribute__((استفاده_شده)) stackcookie = "$STACK: 1048576";
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
بیعلامت دراز __stack = 1048576;
#وإلاإذا defined(__GNUC__) && defined(AMIGA)
بیعلامت دراز __stack = 262144;
#پایان_اگر

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#لغو_تعریف	TRUE
#تعریف_کن TRUE (1)
#لغو_تعریف	FALSE
#تعریف_کن FALSE (0)

#اگر_تعریف_شده __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#وإلاإذا !defined(AZTEC_C) && !defined(__AROS__)
ساكن دراز dos_packet(بنية پیام_پورت *, دراز, دراز);
#پایان_اگر
ساكن صحيح lock2name(BPTR قفل, char_u *buf, دراز   len);
ساكن فراغ بیرون_رقم(دراز n);
ساكن بنية File_Info_بلوک *get_fib(char_u *);
ساكن صحيح sortcmp(ثابت فراغ *a, ثابت فراغ *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ساكن BPTR
امن_قفل(UBYTE *name, دراز نما)
{
    بنية Process *me = (بنية Process *)پیدا_کن_Task(عدم);
    APTR oldwin = me->pr_پنجره_Ptr;
    BPTR flock;

    me->pr_پنجره_Ptr = (APTR)-1L;
    flock = قفل_2(name, نما);
    me->pr_پنجره_Ptr = oldwin;
    أرجع flock;
}

ساكن BPTR		raw_درون = (BPTR)عدم;
ساكن BPTR		raw_بیرون = (BPTR)عدم;
ساكن صحيح		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
بنية Intuition_پایه	*Intuition_پایه = عدم;
#پایان_اگر
#اگر_تعریف_شده FEAT_ARP
بنية Arp_پایه		*Arp_پایه = عدم;
#پایان_اگر

ساكن بنية پنجره	*wb_پنجره;
ساكن char_u		*oldwindowtitle = عدم;

#اگر_تعریف_شده FEAT_ARP
صحيح			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#پایان_اگر
صحيح			اندازه_مجموعه = FALSE;   // set to TRUE if window size was set

#اگر_تعریف_شده __GNUC__
ساكن حرف نسخه_2[] __attribute__((استفاده_شده)) =
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
#پایان_اگر

    فراغ
win_resize_on(فراغ)
{
    بیرون_STR_NF("\033[12{");
}

    فراغ
win_resize_off(فراغ)
{
    بیرون_STR_NF("\033[12}");
}

    فراغ
mch_write(char_u *p, صحيح len)
{
    Write(raw_بیرون, (حرف *)p, (دراز)len);
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
    دراز    زمان,		// milliseconds
    صحيح	    tb_change_cnt استفادهنشده)
{
    صحيح	    len;
    دراز    utime;

    إذا (زمان >= 0)
    {
	إذا (زمان == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	وإلا
	    utime = زمان * 1000L;   // convert from milli to micro secs
	إذا (WaitForChar(raw_درون, utime) == 0)	// no character available
	    أرجع 0;
    }
    وإلا    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	إذا (WaitForChar(raw_درون, p_ut * 1000L) == 0)
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
	len = Read(raw_درون, (حرف *)buf, (دراز)maxlen / درون_داد_conv.vc_ضریب);
	إذا (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    إذا (درون_داد_conv.vc_type != CONV_NONE)
		len = convert_درون_داد(buf, len, maxlen);
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
    أرجع (WaitForChar(raw_درون, 100L) != 0);
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
    أرجع (long_u)(AvailMem(special ? (دراز)MEMF_CHIP : (دراز)MEMF_ANY)) >> 10;
#پایان_اگر
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    فراغ
mch_delay(دراز msec, صحيح flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#اگر_تعریف_شده LATTICE
    فراغ	    Delay(دراز);
#پایان_اگر

    إذا (msec <= 0)
	أرجع;

    إذا (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    وإلا
	WaitForChar(raw_درون, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    فراغ
mch_suspend(فراغ)
{
    suspend_shell();
}

#اگر_تعریف_نشده DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#پایان_اگر

    فراغ
mch_init(فراغ)
{
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساكن حرف	    intlibname[] = "intuition.library";
#پایان_اگر

#اگر_تعریف_شده AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#پایان_اگر

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	بنية Process *me = (بنية Process *)پیدا_کن_Task(عدم);
	me->pr_پنجره_Ptr = (APTR)-1L;
    }

    Columns = 80;
    ردیفها = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    إذا (raw_درون == (BPTR)عدم)
    {
	raw_درون = درون_داد_3();
	raw_بیرون = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	إذا (!IsInteractive(raw_درون))
	    raw_درون = raw_بیرون;
	وإلا إذا (!IsInteractive(raw_بیرون))
	    raw_بیرون = raw_درون;
    }

    بیرون_flush();

    wb_پنجره = عدم;
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    إذا ((Intuition_پایه = (بنية Intuition_پایه *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == عدم)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#پایان_اگر
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
#تعریف_کن BUF2_اندازه 320	    // length of buffer for argument with complete path

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
    بنية Library  *Dos_پایه;
#پایان_اگر
    صحيح		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_پایه = OpenLibrary(DOS_LIBRARY, 37L);
    إذا (Dos_پایه != عدم)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_پایه);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    وإلا	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	خروج(3);
# وإلا
		    // need arp functions for dos 1.x
	إذا (!(Arp_پایه = (بنية Arp_پایه *) OpenLibrary((UBYTE *)ArpName, Arp_نسخه)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_نسخه);
	    خروج(3);
	}
# endif
    }
#پایان_اگر	/* __amigaos4__ __AROS__ __MORPHOS__ */

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
#اگر_تعریف_شده FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#پایان_اگر
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
	    && (IsInteractive(درون_داد_3()) || IsInteractive(Output())))
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
	    إذا (device != عدم && (raw_درون = Open((UBYTE *)device,
					   (دراز)نما_NEWFILE)) != (BPTR)عدم)
		اكسر;
	}
	إذا (raw_درون == (BPTR)عدم)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    اذهب خروج;
	}
	raw_بیرون = raw_درون;
	close_win = TRUE;
	أرجع OK;
    }

    إذا ((nilfh = Open((UBYTE *)"NIL:", (دراز)نما_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	اذهب خروج;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#إذا !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((حرف *)buf1, "t:nc%p", پیدا_کن_Task(0));
#وإلا
    sprintf((حرف *)buf1, "t:nc%ld", (دراز)buf1);
#پایان_اگر
    إذا ((fh = Open((UBYTE *)buf1, (دراز)نما_NEWFILE)) == (BPTR)عدم)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((حرف *)buf1);
	mch_errmsg("\n");
	اذهب خروج;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    إذا (argc == 0)	// run from workbench
	ac = ((بنية WBStartup *)argv)->sm_رقم_آرگومانها;
    وإلا
	ac = argc;
    لكل (i = 0; i < ac; ++i)
    {
	إذا (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((بنية WBStartup *)argv)->sm_ArgList[i]);
	    إذا (argp->wa_قفل)
		(فراغ)lock2name(argp->wa_قفل, buf2, (دراز)(BUF2_اندازه - 1));
#اگر_تعریف_شده FEAT_ARP
	    إذا (dos2)	    // use 2.0 function
#پایان_اگر
		جمع_کن_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (دراز)(BUF2_اندازه - 1));
#اگر_تعریف_شده FEAT_ARP
	    وإلا	    // use arp function
		TackOn((حرف *)buf2, argp->wa_Name);
#پایان_اگر
	    av = (حرف *)buf2;
	}
	وإلا
	    av = argv[i];

	// skip '-d' or "-dev" option
	إذا (av[0] == '-' && av[1] == 'd'
#اگر_تعریف_شده FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#پایان_اگر
		)
	{
	    ++i;
	    استمر;
	}
	إذا (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (دراز)طول_رشته(av));
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
#اگر_تعریف_شده FEAT_ARP
	إذا (dos2)
	{
#پایان_اگر
	    إذا (!System_برچسبها((UBYTE *)buf2, SYS_UserShell, TRUE, برچسب_انجامشده))
		اكسر;
#اگر_تعریف_شده FEAT_ARP
	}
	وإلا
	{
	    إذا (Execute((UBYTE *)buf2, nilfh, nilfh))
		اكسر;
	}
#پایان_اگر
    }
    إذا (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	اذهب خروج;
    }
    exitval = 0;    // The Execute succeeded: exit this program

خروج:
    إذا (nilfh)
	Close(nilfh);
#اگر_تعریف_شده FEAT_ARP
    إذا (Arp_پایه)
	CloseLibrary((بنية Library *) Arp_پایه);
#پایان_اگر
    خروج(exitval);
    // NOTREACHED
    أرجع FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    صحيح
mch_درون_داد_isatty(فراغ)
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
    صحيح		len استفادهنشده)		// buffer size, ignored here
{
    بنية File_Info_بلوک    *fib;
    size_t		    flen;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#اگر_تعریف_شده __amigaos4__
    إذا (fib->fib_Dir_مدخل_Type == ST_ریشه)
	strcat(fib->fib_FileName, ":");
#پایان_اگر
    إذا (flen == طول_رشته(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ساكن بنية File_Info_بلوک *
get_fib(char_u *fname)
{
    BPTR		    flock;
    بنية File_Info_بلوک    *fib;

    إذا (fname == عدم)	    // safety check
	أرجع عدم;
#اگر_تعریف_شده __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#وإلا
    fib = ALLOC_ONE(بنية File_Info_بلوک);
#پایان_اگر
    إذا (fib == عدم)
	أرجع;

    flock = امن_قفل((UBYTE *)fname, (دراز)ACCESS_READ);
    إذا (flock == (BPTR)عدم || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = عدم;
    }
    إذا (flock)
	Un_قفل(flock);
    أرجع fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    فراغ
mch_settitle(char_u *title, char_u *icon)
{
    إذا (wb_پنجره != عدم && title != عدم)
	مجموعه_پنجره_Titles(wb_پنجره, (UBYTE *)title, (UBYTE *)-1L);
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
    أرجع (wb_پنجره != عدم);
}

    صحيح
mch_can_restore_icon(فراغ)
{
    أرجع FALSE;
}

    فراغ
mch_setmouse(صحيح on استفادهنشده)
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
#پایان_اگر
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
#پایان_اگر
}

/*
 * return process ID
 */
    دراز
mch_get_pid(فراغ)
{
#إذا defined(__amigaos4__)
    أرجع (دراز) getpid();
#وإلاإذا defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    أرجع((دراز)پیدا_کن_Task(0));
#وإلا
    أرجع (دراز)0;
#پایان_اگر
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحيح
mch_dirname(char_u *buf, صحيح len)
{
    أرجع mch_پر_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    صحيح
mch_پر_Name(
    char_u	*fname,
    char_u	*buf,
    صحيح		len,
    صحيح		force)
{
    BPTR	l;
    صحيح		retval = FAIL;
    صحيح		i;

    // Lock the file.  If it exists, we can get the exact name.
    إذا ((l = امن_قفل((UBYTE *)fname, (دراز)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (دراز)len - 1);
	Un_قفل(l);
    }
    وإلا إذا (force || !mch_is_پر_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	إذا ((l = قفل_2((UBYTE *)"", (دراز)ACCESS_READ)) != (BPTR)عدم)
	{
	    retval = lock2name(l, buf, (دراز)len);
	    Un_قفل(l);
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
mch_is_پر_Name(char_u *fname)
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
lock2name(BPTR قفل, char_u *buf, دراز len)
{
#اگر_تعریف_شده FEAT_ARP
    إذا (dos2)		    // use 2.0 function
#پایان_اگر
	أرجع ((صحيح)Name_From_قفل(قفل, (UBYTE *)buf, len) ? OK : FAIL);
#اگر_تعریف_شده FEAT_ARP
    وإلا		// use arp function
	أرجع ((صحيح)مسیر_Name(قفل, (حرف *)buf, (دراز)(len/32)) ? OK : FAIL);
#پایان_اگر
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    دراز
mch_getperm(char_u *name)
{
    بنية File_Info_بلوک    *fib;
    دراز		    retval = -1;

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
mch_setperm(char_u *name, دراز perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    أرجع (مجموعه_Protection((UBYTE *)name, (دراز)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    فراغ
mch_hide(char_u *name استفادهنشده)
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
    بنية File_Info_بلوک    *fib;
    صحيح			    retval = FALSE;

    fib = get_fib(name);
    إذا (fib == عدم)
	أرجع FALSE;

#اگر_تعریف_شده __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#وإلا
    retval = ((fib->fib_Dir_مدخل_Type >= 0) ? TRUE : FALSE);
#پایان_اگر
    free_fib(fib);
    أرجع retval;
}

/*
 * Create directory "name".
 */
    صحيح
mch_mkdir(char_u *name)
{
    BPTR	قفل;

    قفل = CreateDir(name);
    إذا (قفل == عدم)
	أرجع -1;

    Un_قفل(قفل);
    أرجع 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحيح
mch_can_exe(char_u *name, char_u **مسیر استفادهنشده, صحيح use_مسیر)
{
    صحيح exe = -1;
#اگر_تعریف_شده __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = بارگیری_Seg(name);

    إذا (seg && Get_Seg_List_Info_برچسبها(seg, GSLI_Native, عدم, برچسب_انجامشده) !=
	    Get_Seg_List_Info_برچسبها(seg, GSLI_68KHUNK, عدم, برچسب_انجامشده))
    {
	// Test if file permissions allow execution.
	بنية Examine_داده *exd = Examine_Object_برچسبها(EX_String_Name_درون_داد, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    وإلا
    {
	exe = 0;
    }

    Un_بارگیری_Seg(seg);

    // Search for executable in path if applicable.
    إذا (!exe && use_مسیر)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	بنية مسیر_گره *سر = Dup_Cmd_مسیر_List(عدم);

	// For each entry, recur to check for executable.
	لكل (بنية مسیر_گره *دم = سر; !exe && دم;
			       دم = (بنية مسیر_گره *) BADDR(دم->pn_بعدی))
	{
	    مجموعه_Current_Dir(دم->pn_قفل);
	    exe = mch_can_exe(name, مسیر, 0);
	}

	// Go back to where we were.
	Free_Cmd_مسیر_List(سر);
	مجموعه_Current_Dir(cwd);
    }
#پایان_اگر
    أرجع exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحيح
mch_nodetype(char_u *name استفادهنشده)
{
    // TODO
    أرجع گره_نرمال;
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

    إذا (raw_درون)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    بیرون_char('\n');
    إذا (raw_بیرون)
    {
	إذا (جمله_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    إذا (اندازه_مجموعه)
		بیرون_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	بیرون_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#اگر_تعریف_شده FEAT_ARP
    إذا (Arp_پایه)
	CloseLibrary((بنية Library *) Arp_پایه);
#پایان_اگر
    إذا (close_win)
	Close(raw_درون);
    إذا (r)
	چاپ_کن(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    خروج(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#تعریف_کن MP(xx)	((بنية پیام_پورت *)((بنية FileHandle *) (BADDR(xx)))->fh_Type)

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
    إذا (!مجموعه_نما_2(raw_درون, tmode == TMODE_RAW ? 1 : 0))
#وإلا
    إذا (dos_packet(MP(raw_درون), (دراز)ACTION_SCREEN_نما,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#پایان_اگر
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
    إذا (!جمله_console)
	أرجع FAIL;

    إذا (raw_درون && raw_بیرون)
    {
	// Save current console mode.
	صحيح old_tmode = cur_tmode;
	حرف ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	إذا (Write(raw_بیرون, ctrl, حجم(ctrl)) == حجم(ctrl))
	{
	    حرف پیمایش[] = "\x9b""1;1;%d;%d r",
		 answ[حجم(پیمایش) + 8] = { '\0' };

	    // Read return sequence from input.
	    إذا (Read(raw_درون, answ, حجم(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		إذا (از_رشته_بخوان(answ, پیمایش, &ردیفها, &Columns) == 2)
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
    جمله_console = FALSE;
    Columns = 80;
    ردیفها = 24;

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
    حرف	    id_a[حجم(بنية Info_داده) + 3];
# endif
    بنية Info_داده *id=0;

    إذا (!جمله_console)	// not an amiga window
	اذهب بیرون;

    // insure longword alignment
# ifdef __amigaos4__
    إذا (!(id = AllocDosObject(DOS_INFODATA, 0)))
	اذهب بیرون;
# وإلا
    id = (بنية Info_داده *)(((دراز)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    إذا (اندازه_مجموعه)
	بیرون_STR("\233t\233u");	// CSI t CSI u
    بیرون_flush();

    إذا (dos_packet(MP(raw_بیرون), (دراز)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_پنجره = (بنية پنجره *)id->id_Volume_گره) == عدم)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	جمله_console = FALSE;
	اذهب بیرون;
    }
    إذا (oldwindowtitle == عدم)
	oldwindowtitle = (char_u *)wb_پنجره->Title;
    إذا (id->id_درون_Use == (BPTR)عدم)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	أرجع FAIL;
    }
    conUnit = (بنية ConUnit *) ((بنية IOStdReq *) id->id_درون_Use)->io_Unit;

    // get window size
    ردیفها = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    إذا (ردیفها < 0 || ردیفها > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	ردیفها = 24;
	جمله_console = FALSE;
	أرجع FAIL;
    }

    أرجع OK;
بیرون:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    أرجع FAIL;
}
#پایان_اگر

/*
 * Try to set the real window size to Rows and Columns.
 */
    فراغ
mch_مجموعه_shellsize(فراغ)
{
    إذا (!جمله_console)
	أرجع;

    اندازه_مجموعه = TRUE;
    بیرون_char(CSI);
    بیرون_رقم((دراز)ردیفها);
    بیرون_char('t');
    بیرون_char(CSI);
    بیرون_رقم((دراز)Columns);
    بیرون_char('u');
    بیرون_flush();
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
بیرون_رقم(دراز n)
{
    بیرون_STR_NF(tltoa((بیعلامت دراز)n));
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

    ساكن دراز
dos_packet(
    بنية پیام_پورت *pid,    // process identifier ... (handlers message port)
    دراز	    action, // packet type ... (what you want handler to do)
    دراز	    arg)    // single argument
{
# ifdef FEAT_ARP
    بنية پیام_پورت	    *replyport;
    بنية StandardPacket   *packet;
    دراز		    res1;

    إذا (dos2)
# endif
	أرجع DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (بنية پیام_پورت *) Create_پورت(عدم, 0);	// use arp function
    إذا (!replyport)
	أرجع (0);

    // Allocate space for a packet, make it public and clear it
    packet = (بنية StandardPacket *)
	AllocMem((دراز) حجم(بنية StandardPacket), MEMF_عمومی | MEMF_CLEAR);
    إذا (!packet)
    {
	Delete_پورت(replyport);
	أرجع (0);
    }
    packet->sp_پیام.mn_گره.ln_Name = (حرف *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_پیام);
    packet->sp_Pkt.dp_پورت = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_پیام(pid, (بنية پیام_4 *)packet);	// send packet

    Wait_پورت(replyport);
    Get_پیام(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (دراز) حجم(بنية StandardPacket));
    Delete_پورت(replyport);

    أرجع (res1);
# endif
}
#پایان_اگر // !defined(AZTEC_C) && !defined(__AROS__)

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
#اگر_تعریف_شده AZTEC_C
    صحيح		use_execute;
    char_u	*shellcmd = عدم;
    char_u	*shellarg;
#پایان_اگر
    صحيح		retval = 0;

    إذا (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	أرجع -1;
    }

    إذا (جمله_console)
	win_resize_off();	    // window resize events de-activated
    بیرون_flush();

    إذا (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = قفل_2((UBYTE *)"", (دراز)ACCESS_READ);   // remember current dir

#إذا !defined(AZTEC_C)		    // not tested very much
    إذا (cmd == عدم)
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = System_برچسبها(p_sh, SYS_UserShell, TRUE, برچسب_انجامشده);
# ifdef FEAT_ARP
	وإلا
	    x = Execute(p_sh, raw_درون, raw_بیرون);
# endif
    }
    وإلا
    {
# ifdef FEAT_ARP
	إذا (dos2)
# endif
	    x = System_برچسبها((حرف *)cmd, SYS_UserShell, TRUE, برچسب_انجامشده);
# ifdef FEAT_ARP
	وإلا
	    x = Execute((حرف *)cmd, 0L, raw_بیرون);
# endif
    }
# ifdef FEAT_ARP
    إذا ((dos2 && x < 0) || (!dos2 && !x))
# وإلا
    إذا (x < 0)
# endif
    {
	پیام_puts(_("Cannot execute "));
	إذا (cmd == عدم)
	{
	    پیام_puts(_("shell "));
	    پیام_outtrans(p_sh);
	}
	وإلا
	    پیام_outtrans(cmd);
	پیام_putchar('\n');
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
		پیام_putchar('\n');
		پیام_outnum((دراز)x);
		پیام_puts(_(" returned\n"));
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
		x = System_برچسبها((UBYTE *)p_sh, SYS_UserShell, TRUE, برچسب_انجامشده);
# ifdef FEAT_ARP
	    وإلا
		x = !Execute((UBYTE *)p_sh, raw_درون, raw_بیرون);
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
	    x = System_برچسبها((UBYTE *)cmd, SYS_UserShell, TRUE, برچسب_انجامشده);
# ifdef FEAT_ARP
	وإلا
	    x = !Execute((UBYTE *)cmd, 0L, raw_بیرون);
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
	پیام_puts(_("Cannot execute "));
	إذا (use_execute)
	{
	    إذا (cmd == عدم)
		پیام_outtrans(p_sh);
	    وإلا
		پیام_outtrans(cmd);
	}
	وإلا
	{
	    پیام_puts(_("shell "));
	    پیام_outtrans(shellcmd);
	}
	پیام_putchar('\n');
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
		پیام_putchar('\n');
		پیام_outnum((دراز)x);
		پیام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#پایان_اگر	// AZTEC_C

    إذا ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_قفل(mydir);
    إذا (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    إذا (جمله_console)
	win_resize_on();		// window resize events activated
    أرجع retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    فراغ
mch_breakcheck(صحيح force استفادهنشده)
{
   إذا (مجموعه_Signal(0L, (دراز)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#اگر_تعریف_شده _DCC
# define Chk_Abort chkabort
#پایان_اگر

#اگر_تعریف_شده LATTICE
فراغ __regargs __chkabort(فراغ);

فراغ __regargs __chkabort(فراغ)
{}

#وإلا
    دراز
Chk_Abort(فراغ)
{
    أرجع(0L);
}
#پایان_اگر

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

#اگر_تعریف_شده __amigaos4__
# define	ANCHOR_BUF_اندازه	1024
#وإلا
# define ANCHOR_BUF_اندازه (512)
# define ANCHOR_اندازه (حجم(بنية Anchor_مسیر) + ANCHOR_BUF_اندازه)
#پایان_اگر

    صحيح
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    صحيح		flags)		// EW_* flags
{
    بنية Anchor_مسیر	*Anchor;
    LONG		نتیجه_2;
    char_u		*starbuf, *sp, *dp;
    صحيح			شروع_len;
    صحيح			matches;
#اگر_تعریف_شده __amigaos4__
    بنية برچسب_قلم	Anchor_برچسبها[] = {
	{ADO_Strlen, ANCHOR_BUF_اندازه},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{برچسب_انجامشده, 0L}
    };
#پایان_اگر

    شروع_len = gap->ga_len;

    // Get our AnchorBase
#اگر_تعریف_شده __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_برچسبها);
#وإلا
    Anchor = alloc_clear(ANCHOR_اندازه);
#پایان_اگر
    إذا (Anchor == عدم)
	أرجع 0;

#اگر_تعریف_نشده __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_اندازه;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# وإلا
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#پایان_اگر

#اگر_تعریف_شده FEAT_ARP
    إذا (dos2)
    {
#پایان_اگر
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
	نتیجه_2 = تطابق_اولی((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#اگر_تعریف_شده FEAT_ARP
    }
    وإلا
	نتیجه_2 = پیدا_کن_اولی((حرف *)pat, Anchor);
#پایان_اگر

    /*
     * Loop to get all matches.
     */
    طالما (نتیجه_2 == 0)
    {
#اگر_تعریف_شده __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_میانگیر, flags);
#وإلا
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#پایان_اگر
#اگر_تعریف_شده FEAT_ARP
	إذا (dos2)
#پایان_اگر
	    نتیجه_2 = تطابق_بعدی_2(Anchor);
#اگر_تعریف_شده FEAT_ARP
	وإلا
	    نتیجه_2 = پیدا_کن_بعدی(Anchor);
#پایان_اگر
    }
    matches = gap->ga_len - شروع_len;

    إذا (نتیجه_2 == ERROR_میانگیر_سرریز)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    وإلا إذا (matches == 0 && نتیجه_2 != ERROR_OBJECT_NOT_یافت
			  && نتیجه_2 != ERROR_DEVICE_NOT_MOUNTED
			  && نتیجه_2 != ERROR_NO_بیشتر_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    إذا (matches)
	qsort((فراغ *)(((char_u **)gap->ga_داده) + شروع_len),
				  (size_t)matches, حجم(char_u *), sortcmp);

    // Free the wildcard stuff
#اگر_تعریف_شده FEAT_ARP
    إذا (dos2)
#پایان_اگر
	تطابق_انتها_3(Anchor);
#اگر_تعریف_شده FEAT_ARP
    وإلا
	FreeAnchorChain(Anchor);
#پایان_اگر

Return:
#اگر_تعریف_شده __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#وإلا
    vim_free(Anchor);
#پایان_اگر

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

#اگر_تعریف_شده FEAT_ARP
    إذا (!dos2)
	retval = (char_u *)getenv((حرف *)var);
    وإلا
#پایان_اگر
    {
	VIM_CLEAR(alloced);
	retval = عدم;

	buf = alloc(IOSIZE);
	إذا (buf == عدم)
	    أرجع عدم;

	len = GetVar((UBYTE *)var, buf, (دراز)(IOSIZE - 1), (دراز)0);
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
mch_setenv(حرف *var, حرف *مقدار, صحيح x استفادهنشده)
{
#اگر_تعریف_شده FEAT_ARP
    إذا (!dos2)
	أرجع setenv(var, مقدار);
#پایان_اگر

    إذا (مجموعه_Var((UBYTE *)var, (UBYTE *)مقدار, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
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
    بنية Process *proc = (بنية Process *) پیدا_کن_Task(0L);
    APTR win = proc->pr_پنجره_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_پنجره_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", نما_OLDFILE);

    proc->pr_پنجره_Ptr = win;

    صحيح وضعیت;

    إذا (!fh || Read(fh, buf, len) != len)
	وضعیت = FAIL;
    وإلا
	وضعیت = OK;

    Close(fh);
    أرجع وضعیت;
}
