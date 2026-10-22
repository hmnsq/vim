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

#اگر_تعریف_شدہ کھڑکی
# undef کھڑکی	// Amiga has its own Window definition
#اگر_ختم

#تعریف_منسوخ TRUE		// will be redefined by exec/types.h
#تعریف_منسوخ FALSE

#اگر_غیر_تعریف_شدہ LATTICE
# include <exec/exec.h>
# include <intuition/intuition.h>
#اگر_ختم

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
#اگر_تعریف_شدہ __amigaos4__
# include <dos/anchorpath.h>
# define	free_fib(x) FreeDosObject(DOS_FIB, x)
#ورنہ
# define	free_fib(x) vim_free(fib)
#اگر_ختم

#اگر defined(LATTICE) && !defined(SASC) && defined(FEAT_ARP)
# include <libraries/arp_pragmas.h>
#اگر_ختم

/*
 * Set stack size on startup.  1 MiB on NG systems (OS4, AROS, MorphOS)
 * which have plenty of RAM.  256 KiB on classic OS 3 -- enough for syntax
 * highlighting and Vim9 execution but conservative for systems with as
 * little as 2 MiB of Fast RAM.
 */
#اگر defined(__amigaos4__)
ساکن ثابت حرف* __attribute__((مستعمل)) stackcookie = "$STACK: 1048576";
#وگرنہ_اگر defined(__AROS__) || defined(__MORPHOS__)
غیرعلامتی طویل __stack = 1048576;
#وگرنہ_اگر defined(__GNUC__) && defined(AMIGA)
غیرعلامتی طویل __stack = 262144;
#اگر_ختم

/*
 * At this point TRUE and FALSE are defined as 1L and 0L, but we want 1 and 0.
 */
#تعریف_منسوخ	TRUE
#تعریف_کرو TRUE (1)
#تعریف_منسوخ	FALSE
#تعریف_کرو FALSE (0)

#اگر_تعریف_شدہ __amigaos4__
# define	dos_packet(a, b, c)   DoPkt(a, b, c, 0, 0, 0, 0)
#وگرنہ_اگر !defined(AZTEC_C) && !defined(__AROS__)
ساکن طویل dos_packet(ساخت پیغام_پورٹ *, طویل, طویل);
#اگر_ختم
ساکن صحیح lock2name(BPTR تالا, char_u *buf, طویل   len);
ساکن خالی برآمد_عدد(طویل n);
ساکن ساخت File_Info_بلاک *get_fib(char_u *);
ساکن صحیح sortcmp(ثابت خالی *a, ثابت خالی *b);

/*
 * Lock() wrapper that suppresses "Please insert volume" system requesters.
 * AmigaDOS pops a requester when Lock() is called with a name that matches a
 * non-mounted volume (e.g., a bare name like "vim" becomes "vim:" -> volume
 * request).  Setting pr_WindowPtr to -1 suppresses the requester and makes
 * Lock() return NULL immediately.  See dos.library/ErrorReport.
 */
    ساکن BPTR
محفوظ_تالا(UBYTE *name, طویل منوال)
{
    ساخت Process *me = (ساخت Process *)ڈھونڈو_Task(تہی);
    APTR oldwin = me->pr_کھڑکی_Ptr;
    BPTR flock;

    me->pr_کھڑکی_Ptr = (APTR)-1L;
    flock = تالا_2(name, منوال);
    me->pr_کھڑکی_Ptr = oldwin;
    واپس flock;
}

ساکن BPTR		raw_اندر = (BPTR)تہی;
ساکن BPTR		raw_برآمد = (BPTR)تہی;
ساکن صحیح		close_win = FALSE;  // set if Vim opened the window

/* Use autoopen for AmigaOS4, AROS and MorphOS */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
ساخت Intuition_بنیاد	*Intuition_بنیاد = تہی;
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ARP
ساخت Arp_بنیاد		*Arp_بنیاد = تہی;
#اگر_ختم

ساکن ساخت کھڑکی	*wb_کھڑکی;
ساکن char_u		*oldwindowtitle = تہی;

#اگر_تعریف_شدہ FEAT_ARP
صحیح			dos2 = FALSE;	    // Amiga DOS 2.0x or higher
#اگر_ختم
صحیح			سائز_سیٹ = FALSE;   // set to TRUE if window size was set

#اگر_تعریف_شدہ __GNUC__
ساکن حرف نسخہ_2[] __attribute__((مستعمل)) =
    "\0$VER: Vim "
    VIM_نسخہ_MAJOR_STR "."
    VIM_نسخہ_MINOR_STR
# ifdef PATCHLEVEL
    "." PATCHLEVEL
# endif
# ifdef BUILDDATE
    " (" BUILDDATE ")"
# endif
    ;
#اگر_ختم

    خالی
win_resize_on(خالی)
{
    برآمد_STR_NF("\033[12{");
}

    خالی
win_resize_off(خالی)
{
    برآمد_STR_NF("\033[12}");
}

    خالی
mch_write(char_u *p, صحیح len)
{
    Write(raw_برآمد, (حرف *)p, (طویل)len);
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
    طویل    وقت,		// milliseconds
    صحیح	    tb_change_cnt غیر_مستعمل)
{
    صحیح	    len;
    طویل    utime;

    اگر (وقت >= 0)
    {
	اگر (وقت == 0)
	    utime = 100L;	    // time = 0 causes problems in DOS 1.2
	ورنہ
	    utime = وقت * 1000L;   // convert from milli to micro secs
	اگر (WaitForChar(raw_اندر, utime) == 0)	// no character available
	    واپس 0;
    }
    ورنہ    // time == -1
    {
	/*
	 * If there is no character available within 2 seconds (default)
	 * write the autoscript file to disk.  Or cause the CursorHold event
	 * to be triggered.
	 */
	اگر (WaitForChar(raw_اندر, p_ut * 1000L) == 0)
	{
	    اگر (trigger_cursorhold() && maxlen >= 3)
	    {
		buf[0] = K_SPECIAL;
		buf[1] = KS_EXTRA;
		buf[2] = (صحیح)KE_CURSORHOLD;
		واپس 3;
	    }
	    before_blocking();
	}
    }

    برائے (;;)	    // repeat until we got a character
    {
	len = Read(raw_اندر, (حرف *)buf, (طویل)maxlen / داخلہ_conv.vc_ضربی_عدد);
	اگر (len > 0)
	{
	    // Convert from 'termencoding' to 'encoding'.
	    اگر (داخلہ_conv.vc_type != CONV_NONE)
		len = convert_داخلہ(buf, len, maxlen);
	    واپس len;
	}
    }
}

/*
 * return non-zero if a character is available
 */
    صحیح
mch_char_avail(خالی)
{
    واپس (WaitForChar(raw_اندر, 100L) != 0);
}

/*
 * Return amount of memory still available in Kbyte.
 */
    long_u
mch_avail_mem(صحیح special)
{
#اگر defined(__amigaos4__) || defined(__AROS__) || defined(__MORPHOS__)
    واپس (long_u)AvailMem(MEMF_ANY) >> 10;
#ورنہ
    واپس (long_u)(AvailMem(special ? (طویل)MEMF_CHIP : (طویل)MEMF_ANY)) >> 10;
#اگر_ختم
}

/*
 * Waits a specified amount of time, or until input arrives if
 * flags does not have MCH_DELAY_IGNOREINPUT.
 */
    خالی
mch_delay(طویل msec, صحیح flags)
{
    // Delay() is declared in <proto/dos.h> for GCC; the local prototype is
    // only needed for the LATTICE/SAS toolchains.
#اگر_تعریف_شدہ LATTICE
    خالی	    Delay(طویل);
#اگر_ختم

    اگر (msec <= 0)
	واپس;

    اگر (flags & MCH_DELAY_IGNOREINPUT)
	Delay(msec / 20L);	    // Delay works with 20 msec intervals
    ورنہ
	WaitForChar(raw_اندر, msec * 1000L);
}

/*
 * We have no job control, fake it by starting a new shell.
 */
    خالی
mch_suspend(خالی)
{
    suspend_shell();
}

#اگر_غیر_تعریف_شدہ DOS_LIBRARY
# define DOS_LIBRARY	((UBYTE *)"dos.library")
#اگر_ختم

    خالی
mch_init(خالی)
{
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساکن حرف	    intlibname[] = "intuition.library";
#اگر_ختم

#اگر_تعریف_شدہ AZTEC_C
    Enable_Abort = 0;		// disallow vim to be aborted
#اگر_ختم

    // Suppress "Please insert volume" system requesters.  Vim probes many
    // paths at startup ($VIM, $VIMRUNTIME, defaults.vim, vimrc, etc.) and
    // Lock()/Open() calls on non-existent paths can trigger requesters from
    // AmigaDOS (and from the C runtime library which calls Lock() internally).
    // A CLI editor should handle missing files gracefully, not pop up dialogs.
    {
	ساخت Process *me = (ساخت Process *)ڈھونڈو_Task(تہی);
	me->pr_کھڑکی_Ptr = (APTR)-1L;
    }

    Columns = 80;
    صفیں = 24;

    /*
     * Set input and output channels, unless we have opened our own window
     */
    اگر (raw_اندر == (BPTR)تہی)
    {
	raw_اندر = داخلہ_3();
	raw_برآمد = Output();
	/*
	 * If Input() is not interactive, then Output() will be (because of
	 * check in mch_check_win()).  Used for "Vim -".
	 * Also check the other way around, for "Vim -h | more".
	 */
	اگر (!IsInteractive(raw_اندر))
	    raw_اندر = raw_برآمد;
	ورنہ اگر (!IsInteractive(raw_برآمد))
	    raw_برآمد = raw_اندر;
    }

    برآمد_flush();

    wb_کھڑکی = تہی;
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    اگر ((Intuition_بنیاد = (ساخت Intuition_بنیاد *)
				OpenLibrary((UBYTE *)intlibname, 0L)) == تہی)
    {
	mch_errmsg(_("cannot open "));
	mch_errmsg(intlibname);
	mch_errmsg("!?\n");
	mch_exit(3);
    }
#اگر_ختم
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
#تعریف_کرو BUF2_سائز 320	    // length of buffer for argument with complete path

    صحیح
mch_check_win(صحیح argc, حرف **argv)
{
    صحیح		    i;
    BPTR	    nilfh, fh;
    char_u	    buf1[24];
    char_u	    buf2[BUF2_سائز];
    ساکن char_u   *(constrings[3]) = {(char_u *)"con:0/0/662/210/",
					(char_u *)"con:0/0/640/200/",
					(char_u *)"con:0/0/320/200/"};
    ساکن char_u   *winerr = (char_u *)N_("VIM: Can't open window!\n");
    ساخت WBArg    *argp;
    صحیح		    ac;
    حرف	    *av;
    char_u	    *device = تہی;
    صحیح		    exitval = 4;
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    ساخت Library  *Dos_بنیاد;
#اگر_ختم
    صحیح		    usewin = FALSE;

/*
 * check if we are running under DOS 2.0x or higher
 */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    Dos_بنیاد = OpenLibrary(DOS_LIBRARY, 37L);
    اگر (Dos_بنیاد != تہی)
    // if (((struct Library *)DOSBase)->lib_Version >= 37)
    {
	CloseLibrary(Dos_بنیاد);
# ifdef FEAT_ARP
	dos2 = TRUE;
# endif
    }
    ورنہ	    // without arp functions we NEED 2.0
    {
# ifndef FEAT_ARP
	mch_errmsg(_("Need Amigados version 2.04 or later\n"));
	باہر_نکلو(3);
# ورنہ
		    // need arp functions for dos 1.x
	اگر (!(Arp_بنیاد = (ساخت Arp_بنیاد *) OpenLibrary((UBYTE *)ArpName, Arp_نسخہ)))
	{
	    fprintf(stderr, _("Need %s version %ld\n"), ArpName, Arp_نسخہ);
	    باہر_نکلو(3);
	}
# endif
    }
#اگر_ختم	/* __amigaos4__ __AROS__ __MORPHOS__ */

    /*
     * scan argv[] for the "-f" and "-d" arguments
     */
    برائے (i = 1; i < argc; ++i)
	اگر (argv[i][0] == '-')
	{
	    انتخاب (argv[i][1])
	    {
	    حالت 'f':
		usewin = TRUE;
		توڑو;

	    حالت 'd':
		اگر (i < argc - 1
#اگر_تعریف_شدہ FEAT_DIFF
			// require using "-dev", "-d" means diff mode
			&& argv[i][2] == 'e' && argv[i][3] == 'v'
#اگر_ختم
		   )
		    device = (char_u *)argv[i + 1];
		توڑو;
	    }
	}

/*
 * If we were not started from workbench, do not have a "-d" or "-dev"
 * argument and we have been started with an interactive window, use that
 * window.
 */
    اگر (argc != 0
	    && device == تہی
	    && (IsInteractive(داخلہ_3()) || IsInteractive(Output())))
	واپس OK;

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
	برائے (i = -1; i < 3; ++i)
	{
	    اگر (i >= 0)
		device = constrings[i];
	    اگر (device != تہی && (raw_اندر = Open((UBYTE *)device,
					   (طویل)منوال_NEWFILE)) != (BPTR)تہی)
		توڑو;
	}
	اگر (raw_اندر == (BPTR)تہی)	// all three failed
	{
	    mch_errmsg(_(winerr));
	    جاؤ باہر_نکلو;
	}
	raw_برآمد = raw_اندر;
	close_win = TRUE;
	واپس OK;
    }

    اگر ((nilfh = Open((UBYTE *)"NIL:", (طویل)منوال_NEWFILE)) == (BPTR)تہی)
    {
	mch_errmsg(_("Cannot open NIL:\n"));
	جاؤ باہر_نکلو;
    }

    /*
     * Make a unique name for the temp file (which we will not delete!).
     * Use a pointer on the stack (nobody else will be using it).
     * Under AmigaOS4, this assumption might change in the future, so
     * we use a pointer to the current task instead. This should be a
     * shared structure and thus globally unique.
     */
#اگر !defined(__amigaos4__) && !defined(__AROS__) && !defined(__MORPHOS__)
    sprintf((حرف *)buf1, "t:nc%p", ڈھونڈو_Task(0));
#ورنہ
    sprintf((حرف *)buf1, "t:nc%ld", (طویل)buf1);
#اگر_ختم
    اگر ((fh = Open((UBYTE *)buf1, (طویل)منوال_NEWFILE)) == (BPTR)تہی)
    {
	mch_errmsg(_("Cannot create "));
	mch_errmsg((حرف *)buf1);
	mch_errmsg("\n");
	جاؤ باہر_نکلو;
    }
    /*
     * Write the command into the file, put quotes around the arguments that
     * have a space in them.
     */
    اگر (argc == 0)	// run from workbench
	ac = ((ساخت WBStartup *)argv)->sm_عدد_دلائل;
    ورنہ
	ac = argc;
    برائے (i = 0; i < ac; ++i)
    {
	اگر (argc == 0)
	{
	    *buf2 = NUL;
	    argp = &(((ساخت WBStartup *)argv)->sm_ArgList[i]);
	    اگر (argp->wa_تالا)
		(خالی)lock2name(argp->wa_تالا, buf2, (طویل)(BUF2_سائز - 1));
#اگر_تعریف_شدہ FEAT_ARP
	    اگر (dos2)	    // use 2.0 function
#اگر_ختم
		جوڑو_Part((UBYTE *)buf2, (UBYTE *)argp->wa_Name, (طویل)(BUF2_سائز - 1));
#اگر_تعریف_شدہ FEAT_ARP
	    ورنہ	    // use arp function
		TackOn((حرف *)buf2, argp->wa_Name);
#اگر_ختم
	    av = (حرف *)buf2;
	}
	ورنہ
	    av = argv[i];

	// skip '-d' or "-dev" option
	اگر (av[0] == '-' && av[1] == 'd'
#اگر_تعریف_شدہ FEAT_DIFF
		&& av[2] == 'e' && av[3] == 'v'
#اگر_ختم
		)
	{
	    ++i;
	    جاری;
	}
	اگر (vim_strchr((char_u *)av, ' '))
	    Write(fh, "\"", 1L);
	Write(fh, av, (طویل)لڑی_لمبائی(av));
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
    برائے (i = -1; i < 3; ++i)
    {
	اگر (i >= 0)
	    device = constrings[i];
	ورنہ اگر (device == تہی)
	    جاری;
	sprintf((حرف *)buf2, "newcli <nil: >nil: %s from %s", (حرف *)device, (حرف *)buf1);
#اگر_تعریف_شدہ FEAT_ARP
	اگر (dos2)
	{
#اگر_ختم
	    اگر (!System_لیبل_فہرست((UBYTE *)buf2, SYS_UserShell, TRUE, لیبل_مکمل))
		توڑو;
#اگر_تعریف_شدہ FEAT_ARP
	}
	ورنہ
	{
	    اگر (Execute((UBYTE *)buf2, nilfh, nilfh))
		توڑو;
	}
#اگر_ختم
    }
    اگر (i == 3)	    // all three failed
    {
	DeleteFile((UBYTE *)buf1);
	mch_errmsg(_(winerr));
	جاؤ باہر_نکلو;
    }
    exitval = 0;    // The Execute succeeded: exit this program

باہر_نکلو:
    اگر (nilfh)
	Close(nilfh);
#اگر_تعریف_شدہ FEAT_ARP
    اگر (Arp_بنیاد)
	CloseLibrary((ساخت Library *) Arp_بنیاد);
#اگر_ختم
    باہر_نکلو(exitval);
    // NOTREACHED
    واپس FAIL;
}

/*
 * Return TRUE if the input comes from a terminal, FALSE otherwise.
 * We fake there is a window, because we can always open one!
 */
    صحیح
mch_داخلہ_isatty(خالی)
{
    واپس TRUE;
}

/*
 * fname_case(): Set the case of the file name, if it already exists.
 *		 This will cause the file name to remain exactly the same
 *		 if the file system ignores, but preserves case.
 */
//ARGSUSED
    خالی
fname_case(
    char_u	*name,
    صحیح		len غیر_مستعمل)		// buffer size, ignored here
{
    ساخت File_Info_بلاک    *fib;
    size_t		    flen;

    fib = get_fib(name);
    اگر (fib == تہی)
	واپس;

    flen = STRLEN(name);
    // TODO: Check if this fix applies to AmigaOS < 4 too.
#اگر_تعریف_شدہ __amigaos4__
    اگر (fib->fib_Dir_اندراج_Type == ST_جڑ)
	strcat(fib->fib_FileName, ":");
#اگر_ختم
    اگر (flen == لڑی_لمبائی(fib->fib_FileName))	// safety check
	mch_memmove(name, fib->fib_FileName, flen);
    free_fib(fib);
}

/*
 * Get the FileInfoBlock for file "fname"
 * The returned structure has to be free()d.
 * Returns NULL on error.
 */
    ساکن ساخت File_Info_بلاک *
get_fib(char_u *fname)
{
    BPTR		    flock;
    ساخت File_Info_بلاک    *fib;

    اگر (fname == تہی)	    // safety check
	واپس تہی;
#اگر_تعریف_شدہ __amigaos4__
    fib = AllocDosObject(DOS_FIB,0);
#ورنہ
    fib = ALLOC_ONE(ساخت File_Info_بلاک);
#اگر_ختم
    اگر (fib == تہی)
	واپس;

    flock = محفوظ_تالا((UBYTE *)fname, (طویل)ACCESS_READ);
    اگر (flock == (BPTR)تہی || !Examine(flock, fib))
    {
	free_fib(fib);  // in case of an error the memory is freed here
	fib = تہی;
    }
    اگر (flock)
	Un_تالا(flock);
    واپس fib;
}

/*
 * set the title of our window
 * icon name is not set
 */
    خالی
mch_settitle(char_u *title, char_u *icon)
{
    اگر (wb_کھڑکی != تہی && title != تہی)
	سیٹ_کھڑکی_Titles(wb_کھڑکی, (UBYTE *)title, (UBYTE *)-1L);
}

/*
 * Restore the window/icon title.
 * which is one of:
 *  SAVE_RESTORE_TITLE  Just restore title
 *  SAVE_RESTORE_ICON   Just restore icon (which we don't have)
 *  SAVE_RESTORE_BOTH   Restore title and icon (which we don't have)
 */
    خالی
mch_restore_title(صحیح which)
{
    اگر (which & SAVE_RESTORE_TITLE)
	mch_settitle(oldwindowtitle, تہی);
}

    صحیح
mch_can_restore_title(خالی)
{
    واپس (wb_کھڑکی != تہی);
}

    صحیح
mch_can_restore_icon(خالی)
{
    واپس FALSE;
}

    خالی
mch_setmouse(صحیح on غیر_مستعمل)
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
    ساخت passwd   *pwd = getpwuid(getuid());

    اگر (pwd != تہی && pwd->pw_name && len > 0)
    {
	vim_strncpy(s, (char_u *)pwd->pw_name, len - 1);
	واپس OK;
    }
#اگر_ختم
    *s = NUL;
    واپس FAIL;
}

/*
 * Insert host name is s[len].
 */
    خالی
mch_get_host_name(char_u *s, صحیح len)
{
#اگر defined(__amigaos4__) || defined(__MORPHOS__)
    gethostname((حرف *)s, len);
#ورنہ
    // AROS and classic OS 3 (libnix) do not have gethostname().
    vim_strncpy(s, (char_u *)"amiga", len - 1);
#اگر_ختم
}

/*
 * return process ID
 */
    طویل
mch_get_pid(خالی)
{
#اگر defined(__amigaos4__)
    واپس (طویل) getpid();
#وگرنہ_اگر defined(__AROS__) || defined(__MORPHOS__)
    // This is as close to a pid as we can come. We could use CLI numbers also,
    // but then we would have two different types of process identifiers.
    واپس((طویل)ڈھونڈو_Task(0));
#ورنہ
    واپس (طویل)0;
#اگر_ختم
}

/*
 * Get name of current directory into buffer 'buf' of length 'len' bytes.
 * Return OK for success, FAIL for failure.
 */
    صحیح
mch_dirname(char_u *buf, صحیح len)
{
    واپس mch_بھرا_Name((char_u *)"", buf, len, FALSE);
}

/*
 * get absolute file name into buffer 'buf' of length 'len' bytes
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_بھرا_Name(
    char_u	*fname,
    char_u	*buf,
    صحیح		len,
    صحیح		force)
{
    BPTR	l;
    صحیح		retval = FAIL;
    صحیح		i;

    // Lock the file.  If it exists, we can get the exact name.
    اگر ((l = محفوظ_تالا((UBYTE *)fname, (طویل)ACCESS_READ)) != (BPTR)0)
    {
	retval = lock2name(l, buf, (طویل)len - 1);
	Un_تالا(l);
    }
    ورنہ اگر (force || !mch_is_بھرا_Name(fname))	    // not a full path yet
    {
	/*
	 * If the file cannot be locked (doesn't exist), try to lock the
	 * current directory and concatenate the file name.
	 */
	اگر ((l = تالا_2((UBYTE *)"", (طویل)ACCESS_READ)) != (BPTR)تہی)
	{
	    retval = lock2name(l, buf, (طویل)len);
	    Un_تالا(l);
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
    واپس retval;
}

/*
 * Return TRUE if "fname" does not depend on the current directory.
 */
    صحیح
mch_is_بھرا_Name(char_u *fname)
{
    واپس (vim_strchr(fname, ':') != تہی && *fname != ':');
}

/*
 * Get the full file name from a lock. Use 2.0 function if possible, because
 * the arp function has more restrictions on the path length.
 *
 * return FAIL for failure, OK otherwise
 */
    ساکن صحیح
lock2name(BPTR تالا, char_u *buf, طویل len)
{
#اگر_تعریف_شدہ FEAT_ARP
    اگر (dos2)		    // use 2.0 function
#اگر_ختم
	واپس ((صحیح)Name_From_تالا(تالا, (UBYTE *)buf, len) ? OK : FAIL);
#اگر_تعریف_شدہ FEAT_ARP
    ورنہ		// use arp function
	واپس ((صحیح)راستہ_Name(تالا, (حرف *)buf, (طویل)(len/32)) ? OK : FAIL);
#اگر_ختم
}

/*
 * get file permissions for 'name'
 * Returns -1 when it doesn't exist.
 */
    طویل
mch_getperm(char_u *name)
{
    ساخت File_Info_بلاک    *fib;
    طویل		    retval = -1;

    fib = get_fib(name);
    اگر (fib == تہی)
	واپس -1;

    retval = fib->fib_Protection;
    free_fib(fib);
    واپس retval;
}

/*
 * set file permission for 'name' to 'perm'
 *
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_setperm(char_u *name, طویل perm)
{
    perm &= ~FIBF_ARCHIVE;		// reset archived bit
    واپس (سیٹ_Protection((UBYTE *)name, (طویل)perm) ? OK : FAIL);
}

/*
 * Set hidden flag for "name".
 */
    خالی
mch_hide(char_u *name غیر_مستعمل)
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
    ساخت File_Info_بلاک    *fib;
    صحیح			    retval = FALSE;

    fib = get_fib(name);
    اگر (fib == تہی)
	واپس FALSE;

#اگر_تعریف_شدہ __amigaos4__
    retval = (FIB_IS_DRAWER(fib)) ? TRUE : FALSE;
#ورنہ
    retval = ((fib->fib_Dir_اندراج_Type >= 0) ? TRUE : FALSE);
#اگر_ختم
    free_fib(fib);
    واپس retval;
}

/*
 * Create directory "name".
 */
    صحیح
mch_mkdir(char_u *name)
{
    BPTR	تالا;

    تالا = CreateDir(name);
    اگر (تالا == تہی)
	واپس -1;

    Un_تالا(تالا);
    واپس 0;
}

/*
 * Return 1 if "name" can be executed, 0 if not.
 * If "use_path" is FALSE only check if "name" is executable.
 * Return -1 if unknown.
 */
    صحیح
mch_can_exe(char_u *name, char_u **راستہ غیر_مستعمل, صحیح use_راستہ)
{
    صحیح exe = -1;
#اگر_تعریف_شدہ __amigaos4__
    // Load file sections using elf.library or hunk.library.
    BPTR seg = بھرو_Seg(name);

    اگر (seg && Get_Seg_List_Info_لیبل_فہرست(seg, GSLI_Native, تہی, لیبل_مکمل) !=
	    Get_Seg_List_Info_لیبل_فہرست(seg, GSLI_68KHUNK, تہی, لیبل_مکمل))
    {
	// Test if file permissions allow execution.
	ساخت Examine_ڈیٹا *exd = Examine_Object_لیبل_فہرست(EX_String_Name_داخلہ, name);

	exe = (exd && !(exd->Protection & EXDF_NO_EXECUTE)) ? 1 : 0;
	FreeDosObject(DOS_EXAMINEDATA, exd);
    }
    ورنہ
    {
	exe = 0;
    }

    Un_بھرو_Seg(seg);

    // Search for executable in path if applicable.
    اگر (!exe && use_راستہ)
    {
	// Save current working dir.
	BPTR cwd = GetCurrentDir();
	ساخت راستہ_گرہ *سر = Dup_Cmd_راستہ_List(تہی);

	// For each entry, recur to check for executable.
	برائے (ساخت راستہ_گرہ *دم = سر; !exe && دم;
			       دم = (ساخت راستہ_گرہ *) BADDR(دم->pn_اگلا))
	{
	    سیٹ_Current_Dir(دم->pn_تالا);
	    exe = mch_can_exe(name, راستہ, 0);
	}

	// Go back to where we were.
	Free_Cmd_راستہ_List(سر);
	سیٹ_Current_Dir(cwd);
    }
#اگر_ختم
    واپس exe;
}

/*
 * Check what "name" is:
 * NODE_NORMAL: file or directory (or doesn't exist)
 * NODE_WRITABLE: writable device, socket, fifo, etc.
 * NODE_OTHER: non-writable things
 */
    صحیح
mch_nodetype(char_u *name غیر_مستعمل)
{
    // TODO
    واپس گرہ_معمول;
}

    خالی
mch_early_init(خالی)
{
}

/*
 * Careful: mch_exit() may be called before mch_init()!
 */
    خالی
mch_exit(صحیح r)
{
    exiting = TRUE;

    اگر (raw_اندر)			    // put terminal in 'normal' mode
    {
	settmode(TMODE_COOK);
	stoptermcap();
    }
    برآمد_char('\n');
    اگر (raw_برآمد)
    {
	اگر (رکن_console)
	{
	    win_resize_off();	    // window resize events de-activated
	    اگر (سائز_سیٹ)
		برآمد_STR("\233t\233u");	// reset window size (CSI t CSI u)
	}
	برآمد_flush();
    }

    mch_restore_title(SAVE_RESTORE_BOTH);    // restore window title

    ml_close_all(TRUE);		    // remove all memfiles

#اگر_تعریف_شدہ FEAT_ARP
    اگر (Arp_بنیاد)
	CloseLibrary((ساخت Library *) Arp_بنیاد);
#اگر_ختم
    اگر (close_win)
	Close(raw_اندر);
    اگر (r)
	لکھو(_("Vim exiting with %d\n"), r); // somehow this makes :cq work!?
    باہر_نکلو(r);
}

/*
 * This is a routine for setting a given stream to raw or cooked mode on the
 * Amiga . This is useful when you are using Lattice C to produce programs
 * that want to read single characters with the "getch()" or "fgetc" call.
 *
 * Written : 18-Jun-87 By Chuck McManis.
 */

#تعریف_کرو MP(xx)	((ساخت پیغام_پورٹ *)((ساخت FileHandle *) (BADDR(xx)))->fh_Type)

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
    خالی
mch_settmode(tmode_T tmode)
{
#اگر defined(__AROS__) || defined(__amigaos4__) || defined(__MORPHOS__)
    اگر (!سیٹ_منوال_2(raw_اندر, tmode == TMODE_RAW ? 1 : 0))
#ورنہ
    اگر (dos_packet(MP(raw_اندر), (طویل)ACTION_SCREEN_منوال,
					  tmode == TMODE_RAW ? -1L : 0L) == 0)
#اگر_ختم
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
mch_get_shellsize(خالی)
{
    اگر (!رکن_console)
	واپس FAIL;

    اگر (raw_اندر && raw_برآمد)
    {
	// Save current console mode.
	صحیح old_tmode = cur_tmode;
	حرف ctrl[] = "\x9b""0 q";

	// Set RAW mode.
	mch_settmode(TMODE_RAW);

	// Write control sequence to console.
	اگر (Write(raw_برآمد, ctrl, حجم(ctrl)) == حجم(ctrl))
	{
	    حرف جائزہ[] = "\x9b""1;1;%d;%d r",
		 answ[حجم(جائزہ) + 8] = { '\0' };

	    // Read return sequence from input.
	    اگر (Read(raw_اندر, answ, حجم(answ) - 1) > 0)
	    {
		// Parse result and set Vim globals.
		اگر (لڑی_سے_پڑھو(answ, جائزہ, &صفیں, &Columns) == 2)
		{
		    // Restore console mode.
		    mch_settmode(old_tmode);
		    واپس OK;
		}
	    }
	}

	// Restore console mode.
	mch_settmode(old_tmode);
    }

    // I/O error. Default size fallback.
    رکن_console = FALSE;
    Columns = 80;
    صفیں = 24;

    واپس FAIL;
}
#ورنہ
/*
 * Try to get the real window size,
 * return FAIL for failure, OK otherwise
 */
    صحیح
mch_get_shellsize(خالی)
{
    ساخت ConUnit  *conUnit;
# ifndef __amigaos4__
    حرف	    id_a[حجم(ساخت Info_ڈیٹا) + 3];
# endif
    ساخت Info_ڈیٹا *id=0;

    اگر (!رکن_console)	// not an amiga window
	جاؤ برآمد;

    // insure longword alignment
# ifdef __amigaos4__
    اگر (!(id = AllocDosObject(DOS_INFODATA, 0)))
	جاؤ برآمد;
# ورنہ
    id = (ساخت Info_ڈیٹا *)(((طویل)id_a + 3L) & ~3L);
# endif

    /*
     * Should make console aware of real window size, not the one we set.
     * Unfortunately, under DOS 2.0x this redraws the window and it
     * is rarely needed, so we skip it now, unless we changed the size.
     */
    اگر (سائز_سیٹ)
	برآمد_STR("\233t\233u");	// CSI t CSI u
    برآمد_flush();

    اگر (dos_packet(MP(raw_برآمد), (طویل)ACTION_DISK_INFO, ((ULONG) id) >> 2) == 0
	    || (wb_کھڑکی = (ساخت کھڑکی *)id->id_Volume_گرہ) == تہی)
    {
	// it's not an amiga window, maybe aux device
	// terminal type should be set
	رکن_console = FALSE;
	جاؤ برآمد;
    }
    اگر (oldwindowtitle == تہی)
	oldwindowtitle = (char_u *)wb_کھڑکی->Title;
    اگر (id->id_اندر_Use == (BPTR)تہی)
    {
	mch_errmsg(_("mch_get_shellsize: not a console??\n"));
	واپس FAIL;
    }
    conUnit = (ساخت ConUnit *) ((ساخت IOStdReq *) id->id_اندر_Use)->io_Unit;

    // get window size
    صفیں = conUnit->cu_YMax + 1;
    Columns = conUnit->cu_XMax + 1;
    اگر (صفیں < 0 || صفیں > 200)	    // cannot be an amiga window
    {
	Columns = 80;
	صفیں = 24;
	رکن_console = FALSE;
	واپس FAIL;
    }

    واپس OK;
برآمد:
# ifdef __amigaos4__
    FreeDosObject(DOS_INFODATA, id); // Safe to pass NULL
# endif

    واپس FAIL;
}
#اگر_ختم

/*
 * Try to set the real window size to Rows and Columns.
 */
    خالی
mch_سیٹ_shellsize(خالی)
{
    اگر (!رکن_console)
	واپس;

    سائز_سیٹ = TRUE;
    برآمد_char(CSI);
    برآمد_عدد((طویل)صفیں);
    برآمد_char('t');
    برآمد_char(CSI);
    برآمد_عدد((طویل)Columns);
    برآمد_char('u');
    برآمد_flush();
}

/*
 * Rows and/or Columns has changed.
 */
    خالی
mch_new_shellsize(خالی)
{
    // Nothing to do.
}

/*
 * out_num - output a (big) number fast
 */
    ساکن خالی
برآمد_عدد(طویل n)
{
    برآمد_STR_NF(tltoa((غیرعلامتی طویل)n));
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

    ساکن طویل
dos_packet(
    ساخت پیغام_پورٹ *pid,    // process identifier ... (handlers message port)
    طویل	    action, // packet type ... (what you want handler to do)
    طویل	    arg)    // single argument
{
# ifdef FEAT_ARP
    ساخت پیغام_پورٹ	    *replyport;
    ساخت StandardPacket   *packet;
    طویل		    res1;

    اگر (dos2)
# endif
	واپس DoPkt(pid, action, arg, 0L, 0L, 0L, 0L);	// use 2.0 function
# ifdef FEAT_ARP

    replyport = (ساخت پیغام_پورٹ *) Create_پورٹ(تہی, 0);	// use arp function
    اگر (!replyport)
	واپس (0);

    // Allocate space for a packet, make it public and clear it
    packet = (ساخت StandardPacket *)
	AllocMem((طویل) حجم(ساخت StandardPacket), MEMF_عوامی | MEMF_CLEAR);
    اگر (!packet)
    {
	Delete_پورٹ(replyport);
	واپس (0);
    }
    packet->sp_پیغام.mn_گرہ.ln_Name = (حرف *) &(packet->sp_Pkt);
    packet->sp_Pkt.dp_Link = &(packet->sp_پیغام);
    packet->sp_Pkt.dp_پورٹ = replyport;
    packet->sp_Pkt.dp_Type = action;
    packet->sp_Pkt.dp_Arg1 = arg;

    Put_پیغام(pid, (ساخت پیغام_4 *)packet);	// send packet

    Wait_پورٹ(replyport);
    Get_پیغام(replyport);

    res1 = packet->sp_Pkt.dp_Res1;

    FreeMem(packet, (طویل) حجم(ساخت StandardPacket));
    Delete_پورٹ(replyport);

    واپس (res1);
# endif
}
#اگر_ختم // !defined(AZTEC_C) && !defined(__AROS__)

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
#اگر_تعریف_شدہ AZTEC_C
    صحیح		use_execute;
    char_u	*shellcmd = تہی;
    char_u	*shellarg;
#اگر_ختم
    صحیح		retval = 0;

    اگر (close_win)
    {
	// if Vim opened a window: Executing a shell may cause crashes
	emsg(_(e_cannot_execute_shell_with_f_option));
	واپس -1;
    }

    اگر (رکن_console)
	win_resize_off();	    // window resize events de-activated
    برآمد_flush();

    اگر (options & SHELL_COOKED)
	settmode(TMODE_COOK);	    // set to normal mode
    mydir = تالا_2((UBYTE *)"", (طویل)ACCESS_READ);   // remember current dir

#اگر !defined(AZTEC_C)		    // not tested very much
    اگر (cmd == تہی)
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = System_لیبل_فہرست(p_sh, SYS_UserShell, TRUE, لیبل_مکمل);
# ifdef FEAT_ARP
	ورنہ
	    x = Execute(p_sh, raw_اندر, raw_برآمد);
# endif
    }
    ورنہ
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = System_لیبل_فہرست((حرف *)cmd, SYS_UserShell, TRUE, لیبل_مکمل);
# ifdef FEAT_ARP
	ورنہ
	    x = Execute((حرف *)cmd, 0L, raw_برآمد);
# endif
    }
# ifdef FEAT_ARP
    اگر ((dos2 && x < 0) || (!dos2 && !x))
# ورنہ
    اگر (x < 0)
# endif
    {
	پیغام_puts(_("Cannot execute "));
	اگر (cmd == تہی)
	{
	    پیغام_puts(_("shell "));
	    پیغام_outtrans(p_sh);
	}
	ورنہ
	    پیغام_outtrans(cmd);
	پیغام_putchar('\n');
	retval = -1;
    }
# ifdef FEAT_ARP
    ورنہ اگر (!dos2 || x)
# ورنہ
    ورنہ اگر (x)
# endif
    {
	اگر ((x = IoErr()) != 0)
	{
	    اگر (!(options & SHELL_SILENT))
	    {
		پیغام_putchar('\n');
		پیغام_outnum((طویل)x);
		پیغام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
#ورنہ	// else part is for AZTEC_C
    اگر (p_st >= 4 || (p_st >= 2 && !(options & SHELL_FILTER)))
	use_execute = 1;
    ورنہ
	use_execute = 0;
    اگر (!use_execute)
    {
	/*
	 * separate shell name from argument
	 */
	shellcmd = vim_strsave(p_sh);
	اگر (shellcmd == تہی)	    // out of memory, use Execute
	    use_execute = 1;
	ورنہ
	{
	    shellarg = skiptowhite(shellcmd);	// find start of arguments
	    اگر (*shellarg != NUL)
	    {
		*shellarg++ = NUL;
		shellarg = skipwhite(shellarg);
	    }
	}
    }
    اگر (cmd == تہی)
    {
	اگر (use_execute)
	{
# ifdef FEAT_ARP
	    اگر (dos2)
# endif
		x = System_لیبل_فہرست((UBYTE *)p_sh, SYS_UserShell, TRUE, لیبل_مکمل);
# ifdef FEAT_ARP
	    ورنہ
		x = !Execute((UBYTE *)p_sh, raw_اندر, raw_برآمد);
# endif
	}
	ورنہ
	    x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg, تہی);
    }
    ورنہ اگر (use_execute)
    {
# ifdef FEAT_ARP
	اگر (dos2)
# endif
	    x = System_لیبل_فہرست((UBYTE *)cmd, SYS_UserShell, TRUE, لیبل_مکمل);
# ifdef FEAT_ARP
	ورنہ
	    x = !Execute((UBYTE *)cmd, 0L, raw_برآمد);
# endif
    }
    ورنہ اگر (p_st & 1)
	x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg,
							   (حرف *)cmd, تہی);
    ورنہ
	x = fexecl((حرف *)shellcmd, (حرف *)shellcmd, (حرف *)shellarg,
					   (حرف *)p_shcf, (حرف *)cmd, تہی);
# ifdef FEAT_ARP
    اگر ((dos2 && x < 0) || (!dos2 && x))
# ورنہ
    اگر (x < 0)
# endif
    {
	پیغام_puts(_("Cannot execute "));
	اگر (use_execute)
	{
	    اگر (cmd == تہی)
		پیغام_outtrans(p_sh);
	    ورنہ
		پیغام_outtrans(cmd);
	}
	ورنہ
	{
	    پیغام_puts(_("shell "));
	    پیغام_outtrans(shellcmd);
	}
	پیغام_putchar('\n');
	retval = -1;
    }
    ورنہ
    {
	اگر (use_execute)
	{
# ifdef FEAT_ARP
	    اگر (!dos2 || x)
# ورنہ
	    اگر (x)
# endif
		x = IoErr();
	}
	ورنہ
	    x = wait();
	اگر (x)
	{
	    اگر (!(options & SHELL_SILENT) && !emsg_silent)
	    {
		پیغام_putchar('\n');
		پیغام_outnum((طویل)x);
		پیغام_puts(_(" returned\n"));
	    }
	    retval = x;
	}
    }
    vim_free(shellcmd);
#اگر_ختم	// AZTEC_C

    اگر ((mydir = CurrentDir(mydir)) != 0) // make sure we stay in the same directory
	Un_تالا(mydir);
    اگر (tmode == TMODE_RAW)
    {
	// The shell may have messed with the mode, always set it.
	cur_tmode = TMODE_UNKNOWN;
	settmode(TMODE_RAW);		// set to raw mode
    }
    resettitle();
    اگر (رکن_console)
	win_resize_on();		// window resize events activated
    واپس retval;
}

/*
 * check for an "interrupt signal"
 * We only react to a CTRL-C, but also clear the other break signals to avoid
 * trouble with lattice-c programs.
 */
    خالی
mch_breakcheck(صحیح force غیر_مستعمل)
{
   اگر (سیٹ_Signal(0L, (طویل)(SIGBREAKF_CTRL_C|SIGBREAKF_CTRL_D|SIGBREAKF_CTRL_E|SIGBREAKF_CTRL_F)) & SIGBREAKF_CTRL_C)
	got_int = TRUE;
}

// this routine causes manx to use this Chk_Abort() rather than its own
// otherwise it resets our ^C when doing any I/O (even when Enable_Abort
// is zero).  Since we want to check for our own ^C's

#اگر_تعریف_شدہ _DCC
# define Chk_Abort chkabort
#اگر_ختم

#اگر_تعریف_شدہ LATTICE
خالی __regargs __chkabort(خالی);

خالی __regargs __chkabort(خالی)
{}

#ورنہ
    طویل
Chk_Abort(خالی)
{
    واپس(0L);
}
#اگر_ختم

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

#اگر_تعریف_شدہ __amigaos4__
# define	ANCHOR_BUF_سائز	1024
#ورنہ
# define ANCHOR_BUF_سائز (512)
# define ANCHOR_سائز (حجم(ساخت Anchor_راستہ) + ANCHOR_BUF_سائز)
#اگر_ختم

    صحیح
mch_expandpath(
    garray_T	*gap,
    char_u	*pat,
    صحیح		flags)		// EW_* flags
{
    ساخت Anchor_راستہ	*Anchor;
    LONG		نتیجہ_2;
    char_u		*starbuf, *sp, *dp;
    صحیح			آغاز_len;
    صحیح			matches;
#اگر_تعریف_شدہ __amigaos4__
    ساخت لیبل_شے	Anchor_لیبل_فہرست[] = {
	{ADO_Strlen, ANCHOR_BUF_سائز},
	{ADO_Flags, APF_DODOT|APF_DOWILD|APF_MultiAssigns},
	{لیبل_مکمل, 0L}
    };
#اگر_ختم

    آغاز_len = gap->ga_len;

    // Get our AnchorBase
#اگر_تعریف_شدہ __amigaos4__
    Anchor = AllocDosObject(DOS_ANCHORPATH, Anchor_لیبل_فہرست);
#ورنہ
    Anchor = alloc_clear(ANCHOR_سائز);
#اگر_ختم
    اگر (Anchor == تہی)
	واپس 0;

#اگر_غیر_تعریف_شدہ __amigaos4__
    Anchor->ap_Strlen = ANCHOR_BUF_سائز;  // ap_Length not supported anymore
# ifdef APF_DODOT
    Anchor->ap_Flags = APF_DODOT | APF_DOWILD;	// allow '.' for current dir
# ورنہ
    Anchor->ap_Flags = APF_DoDot | APF_DoWild;	// allow '.' for current dir
# endif
#اگر_ختم

#اگر_تعریف_شدہ FEAT_ARP
    اگر (dos2)
    {
#اگر_ختم
	// hack to replace '*' by '#?'
	starbuf = alloc(2 * STRLEN(pat) + 1);
	اگر (starbuf == تہی)
	    جاؤ Return;
	برائے (sp = pat, dp = starbuf; *sp; ++sp)
	{
	    اگر (*sp == '*')
	    {
		*dp++ = '#';
		*dp++ = '?';
	    }
	    ورنہ
		*dp++ = *sp;
	}
	*dp = NUL;
	نتیجہ_2 = مطابقت_پہلا((UBYTE *)starbuf, Anchor);
	vim_free(starbuf);
#اگر_تعریف_شدہ FEAT_ARP
    }
    ورنہ
	نتیجہ_2 = ڈھونڈو_پہلا((حرف *)pat, Anchor);
#اگر_ختم

    /*
     * Loop to get all matches.
     */
    جب (نتیجہ_2 == 0)
    {
#اگر_تعریف_شدہ __amigaos4__
	addfile(gap, (char_u *)Anchor->ap_بفر, flags);
#ورنہ
	addfile(gap, (char_u *)Anchor->ap_Buf, flags);
#اگر_ختم
#اگر_تعریف_شدہ FEAT_ARP
	اگر (dos2)
#اگر_ختم
	    نتیجہ_2 = مطابقت_اگلا_2(Anchor);
#اگر_تعریف_شدہ FEAT_ARP
	ورنہ
	    نتیجہ_2 = ڈھونڈو_اگلا(Anchor);
#اگر_ختم
    }
    matches = gap->ga_len - آغاز_len;

    اگر (نتیجہ_2 == ERROR_بفر_حد_سے_تجاوز)
	emsg(_("ANCHOR_BUF_SIZE too small."));
    ورنہ اگر (matches == 0 && نتیجہ_2 != ERROR_OBJECT_NOT_ملا
			  && نتیجہ_2 != ERROR_DEVICE_NOT_MOUNTED
			  && نتیجہ_2 != ERROR_NO_مزید_ENTRIES)
	emsg(_("I/O ERROR"));

    /*
     * Sort the files for this pattern.
     */
    اگر (matches)
	qsort((خالی *)(((char_u **)gap->ga_ڈیٹا) + آغاز_len),
				  (size_t)matches, حجم(char_u *), sortcmp);

    // Free the wildcard stuff
#اگر_تعریف_شدہ FEAT_ARP
    اگر (dos2)
#اگر_ختم
	مطابقت_انتہا_3(Anchor);
#اگر_تعریف_شدہ FEAT_ARP
    ورنہ
	FreeAnchorChain(Anchor);
#اگر_ختم

Return:
#اگر_تعریف_شدہ __amigaos4__
    FreeDosObject(DOS_ANCHORPATH, Anchor);
#ورنہ
    vim_free(Anchor);
#اگر_ختم

    واپس matches;
}

    ساکن صحیح
sortcmp(ثابت خالی *a, ثابت خالی *b)
{
    حرف *s = *(حرف **)a;
    حرف *t = *(حرف **)b;

    واپس pathcmp(s, t, -1);
}

/*
 * Return TRUE if "p" has wildcards that can be expanded by mch_expandpath().
 */
    صحیح
mch_has_exp_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	ورنہ اگر (vim_strchr((char_u *)"*?[(#", *p) != تہی)
	    واپس TRUE;
    }
    واپس FALSE;
}

    صحیح
mch_has_wildcard(char_u *p)
{
    برائے ( ; *p; MB_PTR_ADV(p))
    {
	اگر (*p == '\\' && p[1] != NUL)
	    ++p;
	ورنہ
	    اگر (vim_strchr((char_u *)"*?[(#$`", *p) != تہی
		    || (*p == '~' && p[1] != NUL))
		واپس TRUE;
    }
    واپس FALSE;
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
    ساکن char_u   *alloced = تہی;	// allocated memory

#اگر_تعریف_شدہ FEAT_ARP
    اگر (!dos2)
	retval = (char_u *)getenv((حرف *)var);
    ورنہ
#اگر_ختم
    {
	VIM_CLEAR(alloced);
	retval = تہی;

	buf = alloc(IOSIZE);
	اگر (buf == تہی)
	    واپس تہی;

	len = GetVar((UBYTE *)var, buf, (طویل)(IOSIZE - 1), (طویل)0);
	اگر (len >= 0)
	{
	    retval = vim_strsave((char_u *)buf);
	    alloced = retval;
	}

	vim_free(buf);
    }

    // if $VIM is not defined, use "vim:" instead
    اگر (retval == تہی && STRCMP(var, "VIM") == 0)
	retval = (char_u *)"vim:";

    واپس retval;
}

/*
 * Amiga version of setenv() with AmigaDOS 2.0 support.
 */
// ARGSUSED
    صحیح
mch_setenv(حرف *var, حرف *قدر, صحیح x غیر_مستعمل)
{
#اگر_تعریف_شدہ FEAT_ARP
    اگر (!dos2)
	واپس setenv(var, قدر);
#اگر_ختم

    اگر (سیٹ_Var((UBYTE *)var, (UBYTE *)قدر, (LONG)-1, (ULONG)GVF_LOCAL_ONLY))
	واپس 0;   // success
    واپس -1;	    // failure
}

/*
 * Fill the buffer 'buf' with 'len' random bytes.
 * Returns FAIL if RANDOM: is not available or something went wrong.
 */
    صحیح
mch_get_random(char_u *buf, صحیح len)
{
    ساخت Process *proc = (ساخت Process *) ڈھونڈو_Task(0L);
    APTR win = proc->pr_کھڑکی_Ptr;

    // Don't show requester if RANDOM: doesn't exist
    proc->pr_کھڑکی_Ptr = (APTR) -1L;

    BPTR fh = Open("RANDOM:", منوال_OLDFILE);

    proc->pr_کھڑکی_Ptr = win;

    صحیح حالت_2;

    اگر (!fh || Read(fh, buf, len) != len)
	حالت_2 = FAIL;
    ورنہ
	حالت_2 = OK;

    Close(fh);
    واپس حالت_2;
}
